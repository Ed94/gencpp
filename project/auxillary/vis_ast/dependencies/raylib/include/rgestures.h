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

#include "config.h"

RL_NS_BEGIN

#if !defined(RL_VECTOR2_TYPE)
// Vector2 type
typedef struct Vector2 {
    float x;
    float y;
} Vector2;
#endif

#if defined(RGESTURES_STANDALONE)
// Gestures type
// NOTE: It could be used as flags to enable only some gestures
typedef enum {
    GESTURE_NONE        = 0,
    GESTURE_TAP         = 1,
    GESTURE_DOUBLETAP   = 2,
    GESTURE_HOLD        = 4,
    GESTURE_DRAG        = 8,
    GESTURE_SWIPE_RIGHT = 16,
    GESTURE_SWIPE_LEFT  = 32,
    GESTURE_SWIPE_UP    = 64,
    GESTURE_SWIPE_DOWN  = 128,
    GESTURE_PINCH_IN    = 256,
    GESTURE_PINCH_OUT   = 512
} Gesture;
#endif

typedef enum {
    TOUCH_ACTION_UP = 0,
    TOUCH_ACTION_DOWN,
    TOUCH_ACTION_MOVE,
    TOUCH_ACTION_CANCEL
} TouchAction;

// Gesture event
typedef struct {
    int touchAction;
    int pointCount;
    int pointId[RL_MAX_TOUCH_POINTS];
    Vector2 position[RL_MAX_TOUCH_POINTS];
} GestureEvent;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

RL_EXTERN_C_BEGIN

void process_gesture_event(GestureEvent event);           // Process gesture event and translate it into gestures
void update_gestures(void);                              // Update gestures detected (must be called every frame)

#if defined(RGESTURES_STANDALONE)
void set_gestures_enabled(unsigned int flags);            // Enable a set of gestures using flags
bool is_gesture_detected(int gesture);                    // Check if a gesture have been detected
int get_gesture_detected(void);                           // Get latest detected gesture

float get_gesture_hold_duration(void);                     // Get gesture hold time in seconds
Vector2 get_gesture_drag_vector(void);                     // Get gesture drag vector
float get_gesture_drag_angle(void);                        // Get gesture drag angle
Vector2 get_gesture_pinch_vector(void);                    // Get gesture pinch delta
float get_gesture_pinch_angle(void);                       // Get gesture pinch angle
#endif

RL_EXTERN_C_END

RL_NS_END

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
    int __stdcall query_performance_counter(unsigned long long int *lpPerformanceCount);
    int __stdcall query_performance_frequency(unsigned long long int *lpFrequency);
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

RL_NS_BEGIN

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
        Vector2 upPosition;             // Touch up position
        Vector2 downPositionA;          // First touch down position
        Vector2 downPositionB;          // Second touch down position
        Vector2 downDragPosition;       // Touch drag position
        Vector2 moveDownPositionA;      // First touch down position on move
        Vector2 moveDownPositionB;      // Second touch down position on move
        Vector2 previousPositionA;      // Previous position A to compare for pinch gestures
        Vector2 previousPositionB;      // Previous position B to compare for pinch gestures
        int tapCounter;                 // TAP counter (one tap implies TOUCH_ACTION_DOWN and TOUCH_ACTION_UP actions)
    } Touch;
    struct {
        bool resetRequired;             // HOLD reset to get first touch point again
        double timeDuration;            // HOLD duration in seconds
    } Hold;
    struct {
        Vector2 vector;                 // DRAG vector (between initial and current position)
        float angle;                    // DRAG angle (relative to x-axis)
        float distance;                 // DRAG distance (from initial touch point to final) (normalized [0..1])
        float intensity;                // DRAG intensity, how far why did the DRAG (pixels per frame)
    } Drag;
    struct {
        double startTime;               // SWIPE start time to calculate drag intensity
    } Swipe;
    struct {
        Vector2 vector;                 // PINCH vector (between first and second touch points)
        float angle;                    // PINCH angle (relative to x-axis)
        float distance;                 // PINCH displacement distance (normalized [0..1])
    } Pinch;
} GesturesData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static GesturesData GESTURES = {
#ifdef __cplusplus
    (unsigned int)-1,
    GESTURE_NONE,
    0b0000001111111111
#else
    .Touch.firstId = -1,
    .current = GESTURE_NONE,        // No current gesture detected
    .enabledFlags = 0b0000001111111111  // All gestures supported by default
#endif
};

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static float rg_vector2_angle(Vector2 initialPosition, Vector2 finalPosition);
static float rg_vector2_distance(Vector2 v1, Vector2 v2);
static double rg_get_current_time(void);

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Enable only desired gestures to be detected
void set_gestures_enabled(unsigned int flags)
{
    GESTURES.enabledFlags = flags;
}

// Check if a gesture have been detected
bool is_gesture_detected(unsigned int gesture)
{
    if ((GESTURES.enabledFlags & GESTURES.current) == gesture) return true;
    else return false;
}

// Process gesture event and translate it into gestures
void process_gesture_event(GestureEvent event)
{
    // Reset required variables
    GESTURES.Touch.pointCount = event.pointCount;      // Required on update_gestures()

    if (GESTURES.Touch.pointCount == 1)     // One touch point
    {
        if (event.touchAction == TOUCH_ACTION_DOWN)
        {
            GESTURES.Touch.tapCounter++;    // Tap counter

            // Detect GESTURE_DOUBLE_TAP
            if ((GESTURES.current == GESTURE_NONE) && (GESTURES.Touch.tapCounter >= 2) && ((rg_get_current_time() - GESTURES.Touch.eventTime) < RL_TAP_TIMEOUT) && (rg_vector2_distance(GESTURES.Touch.downPositionA, event.position[0]) < RL_DOUBLETAP_RANGE))
            {
                GESTURES.current = GESTURE_DOUBLETAP;
                GESTURES.Touch.tapCounter = 0;
            }
            else    // Detect GESTURE_TAP
            {
                GESTURES.Touch.tapCounter = 1;
                GESTURES.current = GESTURE_TAP;
            }

            GESTURES.Touch.downPositionA = event.position[0];
            GESTURES.Touch.downDragPosition = event.position[0];

            GESTURES.Touch.upPosition = GESTURES.Touch.downPositionA;
            GESTURES.Touch.eventTime = rg_get_current_time();

            GESTURES.Swipe.startTime = rg_get_current_time();

        #ifdef __cplusplus
            GESTURES.Drag.vector = Vector2{ 0.0f, 0.0f };
        #else
            GESTURES.Drag.vector = (Vector2){ 0.0f, 0.0f };
        #endif
        }
        else if (event.touchAction == TOUCH_ACTION_UP)
        {
            // A swipe can happen while the current gesture is drag, but (specially for web) also hold, so set upPosition for both cases
            if (GESTURES.current == GESTURE_DRAG || GESTURES.current == GESTURE_HOLD) GESTURES.Touch.upPosition = event.position[0];

            // NOTE: GESTURES.Drag.intensity dependent on the resolution of the screen
            GESTURES.Drag.distance = rg_vector2_distance(GESTURES.Touch.downPositionA, GESTURES.Touch.upPosition);
            GESTURES.Drag.intensity = GESTURES.Drag.distance/(float)((rg_get_current_time() - GESTURES.Swipe.startTime));

            // Detect GESTURE_SWIPE
            if ((GESTURES.Drag.intensity > RL_FORCE_TO_SWIPE) && (GESTURES.current != GESTURE_DRAG))
            {
                // NOTE: Angle should be inverted in Y
                GESTURES.Drag.angle = 360.0f - rg_vector2_angle(GESTURES.Touch.downPositionA, GESTURES.Touch.upPosition);

                if ((GESTURES.Drag.angle < 30) || (GESTURES.Drag.angle > 330)) GESTURES.current = GESTURE_SWIPE_RIGHT;          // Right
                else if ((GESTURES.Drag.angle >= 30) && (GESTURES.Drag.angle <= 150)) GESTURES.current = GESTURE_SWIPE_UP;      // Up
                else if ((GESTURES.Drag.angle > 150) && (GESTURES.Drag.angle < 210)) GESTURES.current = GESTURE_SWIPE_LEFT;     // Left
                else if ((GESTURES.Drag.angle >= 210) && (GESTURES.Drag.angle <= 330)) GESTURES.current = GESTURE_SWIPE_DOWN;   // Down
                else GESTURES.current = GESTURE_NONE;
            }
            else
            {
                GESTURES.Drag.distance = 0.0f;
                GESTURES.Drag.intensity = 0.0f;
                GESTURES.Drag.angle = 0.0f;

                GESTURES.current = GESTURE_NONE;
            }

        #if __cplusplus
            GESTURES.Touch.downDragPosition = Vector2{ 0.0f, 0.0f };
        #else
            GESTURES.Touch.downDragPosition = (Vector2){ 0.0f, 0.0f };
        #endif
            GESTURES.Touch.pointCount = 0;
        }
        else if (event.touchAction == TOUCH_ACTION_MOVE)
        {
            GESTURES.Touch.moveDownPositionA = event.position[0];

            if (GESTURES.current == GESTURE_HOLD)
            {
                if (GESTURES.Hold.resetRequired) GESTURES.Touch.downPositionA = event.position[0];

                GESTURES.Hold.resetRequired = false;

                // Detect GESTURE_DRAG
                if ((rg_get_current_time() - GESTURES.Touch.eventTime) > RL_DRAG_TIMEOUT)
                {
                    GESTURES.Touch.eventTime = rg_get_current_time();
                    GESTURES.current = GESTURE_DRAG;
                }
            }

            GESTURES.Drag.vector.x = GESTURES.Touch.moveDownPositionA.x - GESTURES.Touch.downDragPosition.x;
            GESTURES.Drag.vector.y = GESTURES.Touch.moveDownPositionA.y - GESTURES.Touch.downDragPosition.y;
        }
    }
    else if (GESTURES.Touch.pointCount == 2)    // Two touch points
    {
        if (event.touchAction == TOUCH_ACTION_DOWN)
        {
            GESTURES.Touch.downPositionA = event.position[0];
            GESTURES.Touch.downPositionB = event.position[1];

            GESTURES.Touch.previousPositionA = GESTURES.Touch.downPositionA;
            GESTURES.Touch.previousPositionB = GESTURES.Touch.downPositionB;

            //GESTURES.Pinch.distance = rg_vector2_distance(GESTURES.Touch.downPositionA, GESTURES.Touch.downPositionB);

            GESTURES.Pinch.vector.x = GESTURES.Touch.downPositionB.x - GESTURES.Touch.downPositionA.x;
            GESTURES.Pinch.vector.y = GESTURES.Touch.downPositionB.y - GESTURES.Touch.downPositionA.y;

            GESTURES.current = GESTURE_HOLD;
            GESTURES.Hold.timeDuration = rg_get_current_time();
        }
        else if (event.touchAction == TOUCH_ACTION_MOVE)
        {
            GESTURES.Pinch.distance = rg_vector2_distance(GESTURES.Touch.moveDownPositionA, GESTURES.Touch.moveDownPositionB);

            GESTURES.Touch.moveDownPositionA = event.position[0];
            GESTURES.Touch.moveDownPositionB = event.position[1];

            GESTURES.Pinch.vector.x = GESTURES.Touch.moveDownPositionB.x - GESTURES.Touch.moveDownPositionA.x;
            GESTURES.Pinch.vector.y = GESTURES.Touch.moveDownPositionB.y - GESTURES.Touch.moveDownPositionA.y;

            if ((rg_vector2_distance(GESTURES.Touch.previousPositionA, GESTURES.Touch.moveDownPositionA) >= RL_MINIMUM_PINCH) || (rg_vector2_distance(GESTURES.Touch.previousPositionB, GESTURES.Touch.moveDownPositionB) >= RL_MINIMUM_PINCH))
            {
                if ( rg_vector2_distance(GESTURES.Touch.previousPositionA, GESTURES.Touch.previousPositionB) > rg_vector2_distance(GESTURES.Touch.moveDownPositionA, GESTURES.Touch.moveDownPositionB) ) GESTURES.current = GESTURE_PINCH_IN;
                else GESTURES.current = GESTURE_PINCH_OUT;
            }
            else
            {
                GESTURES.current = GESTURE_HOLD;
                GESTURES.Hold.timeDuration = rg_get_current_time();
            }

            // NOTE: Angle should be inverted in Y
            GESTURES.Pinch.angle = 360.0f - rg_vector2_angle(GESTURES.Touch.moveDownPositionA, GESTURES.Touch.moveDownPositionB);
        }
        else if (event.touchAction == TOUCH_ACTION_UP)
        {
            GESTURES.Pinch.distance = 0.0f;
            GESTURES.Pinch.angle = 0.0f;
        #if __cplusplus
            GESTURES.Pinch.vector = Vector2{ 0.0f, 0.0f };
        #else
            GESTURES.Pinch.vector = (Vector2){ 0.0f, 0.0f };
        #endif
            GESTURES.Touch.pointCount = 0;

            GESTURES.current = GESTURE_NONE;
        }
    }
    else if (GESTURES.Touch.pointCount > 2)     // More than two touch points
    {
        // TODO: Process gesture events for more than two points
    }
}

// Update gestures detected (must be called every frame)
void update_gestures(void)
{
    // NOTE: Gestures are processed through system callbacks on touch events

    // Detect GESTURE_HOLD
    if (((GESTURES.current == GESTURE_TAP) || (GESTURES.current == GESTURE_DOUBLETAP)) && (GESTURES.Touch.pointCount < 2))
    {
        GESTURES.current = GESTURE_HOLD;
        GESTURES.Hold.timeDuration = rg_get_current_time();
    }

    // Detect GESTURE_NONE
    if ((GESTURES.current == GESTURE_SWIPE_RIGHT) || (GESTURES.current == GESTURE_SWIPE_UP) || (GESTURES.current == GESTURE_SWIPE_LEFT) || (GESTURES.current == GESTURE_SWIPE_DOWN))
    {
        GESTURES.current = GESTURE_NONE;
    }
}

// Get latest detected gesture
int get_gesture_detected(void)
{
    // Get current gesture only if enabled
    return (GESTURES.enabledFlags & GESTURES.current);
}

// Hold time measured in ms
float get_gesture_hold_duration(void)
{
    // NOTE: time is calculated on current gesture HOLD

    double time = 0.0;

    if (GESTURES.current == GESTURE_HOLD) time = rg_get_current_time() - GESTURES.Hold.timeDuration;

    return (float)time;
}

// Get drag vector (between initial touch point to current)
Vector2 get_gesture_drag_vector(void)
{
    // NOTE: drag vector is calculated on one touch points TOUCH_ACTION_MOVE

    return GESTURES.Drag.vector;
}

// Get drag angle
// NOTE: Angle in degrees, horizontal-right is 0, counterclockwise
float get_gesture_drag_angle(void)
{
    // NOTE: drag angle is calculated on one touch points TOUCH_ACTION_UP

    return GESTURES.Drag.angle;
}

// Get distance between two pinch points
Vector2 get_gesture_pinch_vector(void)
{
    // NOTE: Pinch distance is calculated on two touch points TOUCH_ACTION_MOVE

    return GESTURES.Pinch.vector;
}

// Get angle between two pinch points
// NOTE: Angle in degrees, horizontal-right is 0, counterclockwise
float get_gesture_pinch_angle(void)
{
    // NOTE: pinch angle is calculated on two touch points TOUCH_ACTION_MOVE

    return GESTURES.Pinch.angle;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
// Get angle from two-points vector with X-axis
static float rg_vector2_angle(Vector2 v1, Vector2 v2)
{
    float angle = atan2f(v2.y - v1.y, v2.x - v1.x)*(180.0f/RL_PI);

    if (angle < 0) angle += 360.0f;

    return angle;
}

// Calculate distance between two Vector2
static float rg_vector2_distance(Vector2 v1, Vector2 v2)
{
    float result;

    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;

    result = (float)sqrt(dx*dx + dy*dy);

    return result;
}

// Time measure returned are seconds
static double rg_get_current_time(void)
{
    double time = 0;

#if !defined(RGESTURES_STANDALONE)
    time = get_time();
#else
#if defined(_WIN32)
    unsigned long long int clockFrequency, currentTime;

    query_performance_frequency(&clockFrequency);     // BE CAREFUL: Costly operation!
    query_performance_counter(&currentTime);

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

RL_NS_END

#endif // RGESTURES_IMPLEMENTATION
