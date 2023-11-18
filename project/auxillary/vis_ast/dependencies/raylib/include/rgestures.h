/**********************************************************************************************
*
*   rgestures - Gestures system, gestures processing based on input events (touch/mouse)
*
*   CONFIGURATION:
*       #define RGESTURES_IMPLEMENTATION
*           Generates the implementation of the library into the included file.
*           If not defined, the library is in header only mode and can be included in other headers
*           or source files without problems. But only ONE file should hold the implementation.
*
*       #define RGESTURES_STANDALONE
*           If defined, the library can be used as standalone to process gesture events with
*           no external dependencies.
*
*   CONTRIBUTORS:
*       Marc Palau:         Initial implementation (2014)
*       Albert Martos:      Complete redesign and testing (2015)
*       Ian Eito:           Complete redesign and testing (2015)
*       Ramon Santamaria:   Supervision, review, update and maintenance
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2023 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#ifndef RGESTURES_H
#define RGESTURES_H

#ifndef RL_PI
    #define RL_PI 3.14159265358979323846
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#ifndef RL_MAX_TOUCH_POINTS
    #define RL_MAX_TOUCH_POINTS        8        // Maximum number of touch points supported
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
// NOTE: Below types are required for standalone usage
//----------------------------------------------------------------------------------
// Boolean type
#if (defined(__STDC__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
    #include <stdbool.h>
#elif !defined(__cplusplus) && !defined(bool) && !defined(RL_BOOL_TYPE)
    typedef enum bool { false = 0, true = !false } bool;
#endif

#if !defined(RL_VECTOR2_TYPE)
// RL_Vector2 type
typedef struct RL_Vector2 {
    float x;
    float y;
} RL_Vector2;
#endif

#if defined(RGESTURES_STANDALONE)
// Gestures type
// NOTE: It could be used as flags to enable only some gestures
typedef enum {
    RL_GESTURE_NONE        = 0,
    RL_GESTURE_TAP         = 1,
    RL_GESTURE_DOUBLETAP   = 2,
    RL_GESTURE_HOLD        = 4,
    RL_GESTURE_DRAG        = 8,
    RL_GESTURE_SWIPE_RIGHT = 16,
    RL_GESTURE_SWIPE_LEFT  = 32,
    RL_GESTURE_SWIPE_UP    = 64,
    RL_GESTURE_SWIPE_DOWN  = 128,
    RL_GESTURE_PINCH_IN    = 256,
    RL_GESTURE_PINCH_OUT   = 512
} RL_Gesture;
#endif

typedef enum {
    TOUCH_ACTION_UP = 0,
    TOUCH_ACTION_DOWN,
    TOUCH_ACTION_MOVE,
    TOUCH_ACTION_CANCEL
} RL_TouchAction;

// RL_Gesture event
typedef struct {
    int touchAction;
    int pointCount;
    int pointId[RL_MAX_TOUCH_POINTS];
    RL_Vector2 position[RL_MAX_TOUCH_POINTS];
} RL_GestureEvent;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

void RL_ProcessGestureEvent(RL_GestureEvent event);           // Process gesture event and translate it into gestures
void RL_UpdateGestures(void);                              // Update gestures detected (must be called every frame)

#if defined(RGESTURES_STANDALONE)
void RL_SetGesturesEnabled(unsigned int flags);            // Enable a set of gestures using flags
bool RL_IsGestureDetected(int gesture);                    // Check if a gesture have been detected
int RL_GetGestureDetected(void);                           // Get latest detected gesture

float RL_GetGestureHoldDuration(void);                     // Get gesture hold time in seconds
RL_Vector2 RL_GetGestureDragVector(void);                     // Get gesture drag vector
float RL_GetGestureDragAngle(void);                        // Get gesture drag angle
RL_Vector2 RL_GetGesturePinchVector(void);                    // Get gesture pinch delta
float RL_GetGesturePinchAngle(void);                       // Get gesture pinch angle
#endif

#if defined(__cplusplus)
}
#endif

#endif // RGESTURES_H

/***********************************************************************************
*
*   RGESTURES IMPLEMENTATION
*
************************************************************************************/

#if defined(RGESTURES_IMPLEMENTATION)

#if defined(RGESTURES_STANDALONE)
#if defined(_WIN32)
    #if defined(__cplusplus)
    extern "C" {        // Prevents name mangling of functions
    #endif
    // Functions required to query time on Windows
    int __stdcall RL_QueryPerformanceCounter(unsigned long long int *lpPerformanceCount);
    int __stdcall RL_QueryPerformanceFrequency(unsigned long long int *lpFrequency);
    #if defined(__cplusplus)
    }
    #endif
#elif defined(__linux__)
    #if _POSIX_C_SOURCE < 199309L
        #undef _POSIX_C_SOURCE
        #define _POSIX_C_SOURCE 199309L // Required for CLOCK_MONOTONIC if compiled with c99 without gnu ext.
    #endif
    #include <sys/time.h>               // Required for: timespec
    #include <time.h>                   // Required for: clock_gettime()

    #include <math.h>                   // Required for: sqrtf(), atan2f()
#endif
#if defined(__APPLE__)                  // macOS also defines __MACH__
    #include <mach/clock.h>             // Required for: clock_get_time()
    #include <mach/mach.h>              // Required for: mach_timespec_t
#endif
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define RL_FORCE_TO_SWIPE      0.2f        // Swipe force, measured in normalized screen units/time
#define RL_MINIMUM_DRAG        0.015f      // Drag minimum force, measured in normalized screen units (0.0f to 1.0f)
#define RL_DRAG_TIMEOUT        0.3f        // Drag minimum time for web, measured in seconds
#define RL_MINIMUM_PINCH       0.005f      // Pinch minimum force, measured in normalized screen units (0.0f to 1.0f)
#define RL_TAP_TIMEOUT         0.3f        // Tap minimum time, measured in seconds
#define RL_PINCH_TIMEOUT       0.3f        // Pinch minimum time, measured in seconds
#define RL_DOUBLETAP_RANGE     0.03f       // DoubleTap range, measured in normalized screen units (0.0f to 1.0f)

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Gestures module state context [136 bytes]
typedef struct {
    unsigned int current;               // Current detected gesture
    unsigned int enabledFlags;          // Enabled gestures flags
    struct {
        int firstId;                    // Touch id for first touch point
        int pointCount;                 // Touch points counter
        double eventTime;               // Time stamp when an event happened
        RL_Vector2 upPosition;             // Touch up position
        RL_Vector2 downPositionA;          // First touch down position
        RL_Vector2 downPositionB;          // Second touch down position
        RL_Vector2 downDragPosition;       // Touch drag position
        RL_Vector2 moveDownPositionA;      // First touch down position on move
        RL_Vector2 moveDownPositionB;      // Second touch down position on move
        RL_Vector2 previousPositionA;      // Previous position A to compare for pinch gestures
        RL_Vector2 previousPositionB;      // Previous position B to compare for pinch gestures
        int tapCounter;                 // TAP counter (one tap implies TOUCH_ACTION_DOWN and TOUCH_ACTION_UP actions)
    } Touch;
    struct {
        bool resetRequired;             // HOLD reset to get first touch point again
        double timeDuration;            // HOLD duration in seconds
    } Hold;
    struct {
        RL_Vector2 vector;                 // DRAG vector (between initial and current position)
        float angle;                    // DRAG angle (relative to x-axis)
        float distance;                 // DRAG distance (from initial touch point to final) (normalized [0..1])
        float intensity;                // DRAG intensity, how far why did the DRAG (pixels per frame)
    } Drag;
    struct {
        double startTime;               // SWIPE start time to calculate drag intensity
    } Swipe;
    struct {
        RL_Vector2 vector;                 // PINCH vector (between first and second touch points)
        float angle;                    // PINCH angle (relative to x-axis)
        float distance;                 // PINCH displacement distance (normalized [0..1])
    } Pinch;
} RL_GesturesData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static RL_GesturesData RL_GESTURES = {
    .Touch.firstId = -1,
    .current = RL_GESTURE_NONE,        // No current gesture detected
    .enabledFlags = 0b0000001111111111  // All gestures supported by default
};

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static float rgVector2Angle(RL_Vector2 initialPosition, RL_Vector2 finalPosition);
static float rgVector2Distance(RL_Vector2 v1, RL_Vector2 v2);
static double rgGetCurrentTime(void);

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Enable only desired gestures to be detected
void RL_SetGesturesEnabled(unsigned int flags)
{
    RL_GESTURES.enabledFlags = flags;
}

// Check if a gesture have been detected
bool RL_IsGestureDetected(unsigned int gesture)
{
    if ((RL_GESTURES.enabledFlags & RL_GESTURES.current) == gesture) return true;
    else return false;
}

// Process gesture event and translate it into gestures
void RL_ProcessGestureEvent(RL_GestureEvent event)
{
    // Reset required variables
    RL_GESTURES.Touch.pointCount = event.pointCount;      // Required on RL_UpdateGestures()

    if (RL_GESTURES.Touch.pointCount == 1)     // One touch point
    {
        if (event.touchAction == TOUCH_ACTION_DOWN)
        {
            RL_GESTURES.Touch.tapCounter++;    // Tap counter

            // Detect GESTURE_DOUBLE_TAP
            if ((RL_GESTURES.current == RL_GESTURE_NONE) && (RL_GESTURES.Touch.tapCounter >= 2) && ((rgGetCurrentTime() - RL_GESTURES.Touch.eventTime) < RL_TAP_TIMEOUT) && (rgVector2Distance(RL_GESTURES.Touch.downPositionA, event.position[0]) < RL_DOUBLETAP_RANGE))
            {
                RL_GESTURES.current = RL_GESTURE_DOUBLETAP;
                RL_GESTURES.Touch.tapCounter = 0;
            }
            else    // Detect RL_GESTURE_TAP
            {
                RL_GESTURES.Touch.tapCounter = 1;
                RL_GESTURES.current = RL_GESTURE_TAP;
            }

            RL_GESTURES.Touch.downPositionA = event.position[0];
            RL_GESTURES.Touch.downDragPosition = event.position[0];

            RL_GESTURES.Touch.upPosition = RL_GESTURES.Touch.downPositionA;
            RL_GESTURES.Touch.eventTime = rgGetCurrentTime();

            RL_GESTURES.Swipe.startTime = rgGetCurrentTime();

            RL_GESTURES.Drag.vector = (RL_Vector2){ 0.0f, 0.0f };
        }
        else if (event.touchAction == TOUCH_ACTION_UP)
        {
            // A swipe can happen while the current gesture is drag, but (specially for web) also hold, so set upPosition for both cases
            if (RL_GESTURES.current == RL_GESTURE_DRAG || RL_GESTURES.current == RL_GESTURE_HOLD) RL_GESTURES.Touch.upPosition = event.position[0];

            // NOTE: RL_GESTURES.Drag.intensity dependent on the resolution of the screen
            RL_GESTURES.Drag.distance = rgVector2Distance(RL_GESTURES.Touch.downPositionA, RL_GESTURES.Touch.upPosition);
            RL_GESTURES.Drag.intensity = RL_GESTURES.Drag.distance/(float)((rgGetCurrentTime() - RL_GESTURES.Swipe.startTime));

            // Detect GESTURE_SWIPE
            if ((RL_GESTURES.Drag.intensity > RL_FORCE_TO_SWIPE) && (RL_GESTURES.current != RL_GESTURE_DRAG))
            {
                // NOTE: Angle should be inverted in Y
                RL_GESTURES.Drag.angle = 360.0f - rgVector2Angle(RL_GESTURES.Touch.downPositionA, RL_GESTURES.Touch.upPosition);

                if ((RL_GESTURES.Drag.angle < 30) || (RL_GESTURES.Drag.angle > 330)) RL_GESTURES.current = RL_GESTURE_SWIPE_RIGHT;          // Right
                else if ((RL_GESTURES.Drag.angle >= 30) && (RL_GESTURES.Drag.angle <= 150)) RL_GESTURES.current = RL_GESTURE_SWIPE_UP;      // Up
                else if ((RL_GESTURES.Drag.angle > 150) && (RL_GESTURES.Drag.angle < 210)) RL_GESTURES.current = RL_GESTURE_SWIPE_LEFT;     // Left
                else if ((RL_GESTURES.Drag.angle >= 210) && (RL_GESTURES.Drag.angle <= 330)) RL_GESTURES.current = RL_GESTURE_SWIPE_DOWN;   // Down
                else RL_GESTURES.current = RL_GESTURE_NONE;
            }
            else
            {
                RL_GESTURES.Drag.distance = 0.0f;
                RL_GESTURES.Drag.intensity = 0.0f;
                RL_GESTURES.Drag.angle = 0.0f;

                RL_GESTURES.current = RL_GESTURE_NONE;
            }

            RL_GESTURES.Touch.downDragPosition = (RL_Vector2){ 0.0f, 0.0f };
            RL_GESTURES.Touch.pointCount = 0;
        }
        else if (event.touchAction == TOUCH_ACTION_MOVE)
        {
            RL_GESTURES.Touch.moveDownPositionA = event.position[0];

            if (RL_GESTURES.current == RL_GESTURE_HOLD)
            {
                if (RL_GESTURES.Hold.resetRequired) RL_GESTURES.Touch.downPositionA = event.position[0];

                RL_GESTURES.Hold.resetRequired = false;

                // Detect RL_GESTURE_DRAG
                if ((rgGetCurrentTime() - RL_GESTURES.Touch.eventTime) > RL_DRAG_TIMEOUT)
                {
                    RL_GESTURES.Touch.eventTime = rgGetCurrentTime();
                    RL_GESTURES.current = RL_GESTURE_DRAG;
                }
            }

            RL_GESTURES.Drag.vector.x = RL_GESTURES.Touch.moveDownPositionA.x - RL_GESTURES.Touch.downDragPosition.x;
            RL_GESTURES.Drag.vector.y = RL_GESTURES.Touch.moveDownPositionA.y - RL_GESTURES.Touch.downDragPosition.y;
        }
    }
    else if (RL_GESTURES.Touch.pointCount == 2)    // Two touch points
    {
        if (event.touchAction == TOUCH_ACTION_DOWN)
        {
            RL_GESTURES.Touch.downPositionA = event.position[0];
            RL_GESTURES.Touch.downPositionB = event.position[1];

            RL_GESTURES.Touch.previousPositionA = RL_GESTURES.Touch.downPositionA;
            RL_GESTURES.Touch.previousPositionB = RL_GESTURES.Touch.downPositionB;

            //RL_GESTURES.Pinch.distance = rgVector2Distance(RL_GESTURES.Touch.downPositionA, RL_GESTURES.Touch.downPositionB);

            RL_GESTURES.Pinch.vector.x = RL_GESTURES.Touch.downPositionB.x - RL_GESTURES.Touch.downPositionA.x;
            RL_GESTURES.Pinch.vector.y = RL_GESTURES.Touch.downPositionB.y - RL_GESTURES.Touch.downPositionA.y;

            RL_GESTURES.current = RL_GESTURE_HOLD;
            RL_GESTURES.Hold.timeDuration = rgGetCurrentTime();
        }
        else if (event.touchAction == TOUCH_ACTION_MOVE)
        {
            RL_GESTURES.Pinch.distance = rgVector2Distance(RL_GESTURES.Touch.moveDownPositionA, RL_GESTURES.Touch.moveDownPositionB);

            RL_GESTURES.Touch.moveDownPositionA = event.position[0];
            RL_GESTURES.Touch.moveDownPositionB = event.position[1];

            RL_GESTURES.Pinch.vector.x = RL_GESTURES.Touch.moveDownPositionB.x - RL_GESTURES.Touch.moveDownPositionA.x;
            RL_GESTURES.Pinch.vector.y = RL_GESTURES.Touch.moveDownPositionB.y - RL_GESTURES.Touch.moveDownPositionA.y;

            if ((rgVector2Distance(RL_GESTURES.Touch.previousPositionA, RL_GESTURES.Touch.moveDownPositionA) >= RL_MINIMUM_PINCH) || (rgVector2Distance(RL_GESTURES.Touch.previousPositionB, RL_GESTURES.Touch.moveDownPositionB) >= RL_MINIMUM_PINCH))
            {
                if ( rgVector2Distance(RL_GESTURES.Touch.previousPositionA, RL_GESTURES.Touch.previousPositionB) > rgVector2Distance(RL_GESTURES.Touch.moveDownPositionA, RL_GESTURES.Touch.moveDownPositionB) ) RL_GESTURES.current = RL_GESTURE_PINCH_IN;
                else RL_GESTURES.current = RL_GESTURE_PINCH_OUT;
            }
            else
            {
                RL_GESTURES.current = RL_GESTURE_HOLD;
                RL_GESTURES.Hold.timeDuration = rgGetCurrentTime();
            }

            // NOTE: Angle should be inverted in Y
            RL_GESTURES.Pinch.angle = 360.0f - rgVector2Angle(RL_GESTURES.Touch.moveDownPositionA, RL_GESTURES.Touch.moveDownPositionB);
        }
        else if (event.touchAction == TOUCH_ACTION_UP)
        {
            RL_GESTURES.Pinch.distance = 0.0f;
            RL_GESTURES.Pinch.angle = 0.0f;
            RL_GESTURES.Pinch.vector = (RL_Vector2){ 0.0f, 0.0f };
            RL_GESTURES.Touch.pointCount = 0;

            RL_GESTURES.current = RL_GESTURE_NONE;
        }
    }
    else if (RL_GESTURES.Touch.pointCount > 2)     // More than two touch points
    {
        // TODO: Process gesture events for more than two points
    }
}

// Update gestures detected (must be called every frame)
void RL_UpdateGestures(void)
{
    // NOTE: Gestures are processed through system callbacks on touch events

    // Detect RL_GESTURE_HOLD
    if (((RL_GESTURES.current == RL_GESTURE_TAP) || (RL_GESTURES.current == RL_GESTURE_DOUBLETAP)) && (RL_GESTURES.Touch.pointCount < 2))
    {
        RL_GESTURES.current = RL_GESTURE_HOLD;
        RL_GESTURES.Hold.timeDuration = rgGetCurrentTime();
    }

    // Detect RL_GESTURE_NONE
    if ((RL_GESTURES.current == RL_GESTURE_SWIPE_RIGHT) || (RL_GESTURES.current == RL_GESTURE_SWIPE_UP) || (RL_GESTURES.current == RL_GESTURE_SWIPE_LEFT) || (RL_GESTURES.current == RL_GESTURE_SWIPE_DOWN))
    {
        RL_GESTURES.current = RL_GESTURE_NONE;
    }
}

// Get latest detected gesture
int RL_GetGestureDetected(void)
{
    // Get current gesture only if enabled
    return (RL_GESTURES.enabledFlags & RL_GESTURES.current);
}

// Hold time measured in ms
float RL_GetGestureHoldDuration(void)
{
    // NOTE: time is calculated on current gesture HOLD

    double time = 0.0;

    if (RL_GESTURES.current == RL_GESTURE_HOLD) time = rgGetCurrentTime() - RL_GESTURES.Hold.timeDuration;

    return (float)time;
}

// Get drag vector (between initial touch point to current)
RL_Vector2 RL_GetGestureDragVector(void)
{
    // NOTE: drag vector is calculated on one touch points TOUCH_ACTION_MOVE

    return RL_GESTURES.Drag.vector;
}

// Get drag angle
// NOTE: Angle in degrees, horizontal-right is 0, counterclockwise
float RL_GetGestureDragAngle(void)
{
    // NOTE: drag angle is calculated on one touch points TOUCH_ACTION_UP

    return RL_GESTURES.Drag.angle;
}

// Get distance between two pinch points
RL_Vector2 RL_GetGesturePinchVector(void)
{
    // NOTE: Pinch distance is calculated on two touch points TOUCH_ACTION_MOVE

    return RL_GESTURES.Pinch.vector;
}

// Get angle between two pinch points
// NOTE: Angle in degrees, horizontal-right is 0, counterclockwise
float RL_GetGesturePinchAngle(void)
{
    // NOTE: pinch angle is calculated on two touch points TOUCH_ACTION_MOVE

    return RL_GESTURES.Pinch.angle;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
// Get angle from two-points vector with X-axis
static float rgVector2Angle(RL_Vector2 v1, RL_Vector2 v2)
{
    float angle = atan2f(v2.y - v1.y, v2.x - v1.x)*(180.0f/RL_PI);

    if (angle < 0) angle += 360.0f;

    return angle;
}

// Calculate distance between two RL_Vector2
static float rgVector2Distance(RL_Vector2 v1, RL_Vector2 v2)
{
    float result;

    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;

    result = (float)sqrt(dx*dx + dy*dy);

    return result;
}

// Time measure returned are seconds
static double rgGetCurrentTime(void)
{
    double time = 0;

#if !defined(RGESTURES_STANDALONE)
    time = RL_GetTime();
#else
#if defined(_WIN32)
    unsigned long long int clockFrequency, currentTime;

    RL_QueryPerformanceFrequency(&clockFrequency);     // BE CAREFUL: Costly operation!
    RL_QueryPerformanceCounter(&currentTime);

    time = (double)currentTime/clockFrequency;  // Time in seconds
#endif

#if defined(__linux__)
    // NOTE: Only for Linux-based systems
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    unsigned long long int nowTime = (unsigned long long int)now.tv_sec*1000000000LLU + (unsigned long long int)now.tv_nsec;     // Time in nanoseconds

    time = ((double)nowTime*1e-9);     // Time in seconds
#endif

#if defined(__APPLE__)
    //#define CLOCK_REALTIME  CALENDAR_CLOCK    // returns UTC time since 1970-01-01
    //#define CLOCK_MONOTONIC SYSTEM_CLOCK      // returns the time since boot time

    clock_serv_t cclock;
    mach_timespec_t now;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);

    // NOTE: OS X does not have clock_gettime(), using clock_get_time()
    clock_get_time(cclock, &now);
    mach_port_deallocate(mach_task_self(), cclock);
    unsigned long long int nowTime = (unsigned long long int)now.tv_sec*1000000000LLU + (unsigned long long int)now.tv_nsec;     // Time in nanoseconds

    time = ((double)nowTime*1e-9);     // Time in seconds
#endif
#endif

    return time;
}

#endif // RGESTURES_IMPLEMENTATION
