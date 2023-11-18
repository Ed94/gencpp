/*******************************************************************************************
*
*   rcamera - Basic camera system with support for multiple camera modes
*
*   CONFIGURATION:
*       #define RCAMERA_IMPLEMENTATION
*           Generates the implementation of the library into the included file.
*           If not defined, the library is in header only mode and can be included in other headers
*           or source files without problems. But only ONE file should hold the implementation.
*
*       #define RCAMERA_STANDALONE
*           If defined, the library can be used as standalone as a camera system but some
*           functions must be redefined to manage inputs accordingly.
*
*   CONTRIBUTORS:
*       Ramon Santamaria:   Supervision, review, update and maintenance
*       Christoph Wagner:   Complete redesign, using raymath (2022)
*       Marc Palau:         Initial implementation (2014)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2022-2023 Christoph Wagner (@Crydsch) & Ramon Santamaria (@raysan5)
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

#ifndef RCAMERA_H
#define RCAMERA_H

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Function specifiers definition

// Function specifiers in case library is build/used as a shared library (Windows)
// NOTE: Microsoft specifiers to tell compiler that symbols are imported/exported from a .dll
#if defined(_WIN32)
#if defined(RL_BUILD_LIBTYPE_SHARED)
#if defined(__TINYC__)
#define __declspec(x) __attribute__((x))
#endif
#define RLAPI __declspec(dllexport)     // We are building the library as a Win32 shared library (.dll)
#elif defined(RL_USE_LIBTYPE_SHARED)
#define RLAPI __declspec(dllimport)     // We are using the library as a Win32 shared library (.dll)
#endif
#endif

#ifndef RLAPI
    #define RLAPI       // Functions defined as 'extern' by default (implicit specifiers)
#endif

#if defined(RCAMERA_STANDALONE)
    #define RL_CAMERA_CULL_DISTANCE_NEAR      0.01
    #define RL_CAMERA_CULL_DISTANCE_FAR    1000.0
#else
    #define RL_CAMERA_CULL_DISTANCE_NEAR   RL_CULL_DISTANCE_NEAR
    #define RL_CAMERA_CULL_DISTANCE_FAR    RL_CULL_DISTANCE_FAR
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
// NOTE: Below types are required for standalone usage
//----------------------------------------------------------------------------------
#if defined(RCAMERA_STANDALONE)
    // RL_Vector2, 2 components
    typedef struct RL_Vector2 {
        float x;                // Vector x component
        float y;                // Vector y component
    } RL_Vector2;

    // RL_Vector3, 3 components
    typedef struct RL_Vector3 {
        float x;                // Vector x component
        float y;                // Vector y component
        float z;                // Vector z component
    } RL_Vector3;

    // RL_Matrix, 4x4 components, column major, OpenGL style, right-handed
    typedef struct RL_Matrix {
        float m0, m4, m8, m12;  // RL_Matrix first row (4 components)
        float m1, m5, m9, m13;  // RL_Matrix second row (4 components)
        float m2, m6, m10, m14; // RL_Matrix third row (4 components)
        float m3, m7, m11, m15; // RL_Matrix fourth row (4 components)
    } RL_Matrix;

    // RL_Camera type, defines a camera position/orientation in 3d space
    typedef struct RL_Camera3D {
        RL_Vector3 position;       // RL_Camera position
        RL_Vector3 target;         // RL_Camera target it looks-at
        RL_Vector3 up;             // RL_Camera up vector (rotation over its axis)
        float fovy;             // RL_Camera field-of-view apperture in Y (degrees) in perspective, used as near plane width in orthographic
        int projection;         // RL_Camera projection type: RL_CAMERA_PERSPECTIVE or RL_CAMERA_ORTHOGRAPHIC
    } RL_Camera3D;

    typedef RL_Camera3D RL_Camera;    // RL_Camera type fallback, defaults to RL_Camera3D

    // RL_Camera projection
    typedef enum {
        RL_CAMERA_PERSPECTIVE = 0, // Perspective projection
        RL_CAMERA_ORTHOGRAPHIC     // Orthographic projection
    } RL_CameraProjection;

    // RL_Camera system modes
    typedef enum {
        RL_CAMERA_CUSTOM = 0,      // RL_Camera custom, controlled by user (RL_UpdateCamera() does nothing)
        RL_CAMERA_FREE,            // RL_Camera free mode
        RL_CAMERA_ORBITAL,         // RL_Camera orbital, around target, zoom supported
        RL_CAMERA_FIRST_PERSON,    // RL_Camera first person
        RL_CAMERA_THIRD_PERSON     // RL_Camera third person
    } RL_CameraMode;
#endif

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

RLAPI RL_Vector3 GetCameraForward(RL_Camera *camera);
RLAPI RL_Vector3 GetCameraUp(RL_Camera *camera);
RLAPI RL_Vector3 GetCameraRight(RL_Camera *camera);

// RL_Camera movement
RLAPI void CameraMoveForward(RL_Camera *camera, float distance, bool moveInWorldPlane);
RLAPI void CameraMoveUp(RL_Camera *camera, float distance);
RLAPI void CameraMoveRight(RL_Camera *camera, float distance, bool moveInWorldPlane);
RLAPI void CameraMoveToTarget(RL_Camera *camera, float delta);

// RL_Camera rotation
RLAPI void CameraYaw(RL_Camera *camera, float angle, bool rotateAroundTarget);
RLAPI void CameraPitch(RL_Camera *camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp);
RLAPI void CameraRoll(RL_Camera *camera, float angle);

RLAPI RL_Matrix GetCameraViewMatrix(RL_Camera *camera);
RLAPI RL_Matrix GetCameraProjectionMatrix(RL_Camera* camera, float aspect);

#if defined(__cplusplus)
}
#endif

#endif // RCAMERA_H


/***********************************************************************************
*
*   CAMERA IMPLEMENTATION
*
************************************************************************************/

#if defined(RCAMERA_IMPLEMENTATION)

#include "raymath.h"        // Required for vector maths:
                            // RL_Vector3Add()
                            // RL_Vector3Subtract()
                            // RL_Vector3Scale()
                            // RL_Vector3Normalize()
                            // RL_Vector3Distance()
                            // RL_Vector3CrossProduct()
                            // RL_Vector3RotateByAxisAngle()
                            // RL_Vector3Angle()
                            // RL_Vector3Negate()
                            // RL_MatrixLookAt()
                            // RL_MatrixPerspective()
                            // RL_MatrixOrtho()
                            // RL_MatrixIdentity()

// raylib required functionality:
                            // RL_GetMouseDelta()
                            // RL_GetMouseWheelMove()
                            // RL_IsKeyDown()
                            // RL_IsKeyPressed()
                            // RL_GetFrameTime()

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define RL_CAMERA_MOVE_SPEED                               0.09f
#define RL_CAMERA_ROTATION_SPEED                           0.03f
#define RL_CAMERA_PAN_SPEED                                0.2f

// RL_Camera mouse movement sensitivity
#define RL_CAMERA_MOUSE_MOVE_SENSITIVITY                   0.003f     // TODO: it should be independant of framerate
#define RL_CAMERA_MOUSE_SCROLL_SENSITIVITY                 1.5f

#define RL_CAMERA_ORBITAL_SPEED                            0.5f       // Radians per second


#define RL_CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER  8.0f
#define RL_CAMERA_FIRST_PERSON_STEP_DIVIDER                30.0f
#define RL_CAMERA_FIRST_PERSON_WAVING_DIVIDER              200.0f

// PLAYER (used by camera)
#define RL_PLAYER_MOVEMENT_SENSITIVITY                     20.0f

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
// Returns the cameras forward vector (normalized)
RL_Vector3 GetCameraForward(RL_Camera *camera)
{
    return RL_Vector3Normalize(RL_Vector3Subtract(camera->target, camera->position));
}

// Returns the cameras up vector (normalized)
// Note: The up vector might not be perpendicular to the forward vector
RL_Vector3 GetCameraUp(RL_Camera *camera)
{
    return RL_Vector3Normalize(camera->up);
}

// Returns the cameras right vector (normalized)
RL_Vector3 GetCameraRight(RL_Camera *camera)
{
    RL_Vector3 forward = GetCameraForward(camera);
    RL_Vector3 up = GetCameraUp(camera);

    return RL_Vector3CrossProduct(forward, up);
}

// Moves the camera in its forward direction
void CameraMoveForward(RL_Camera *camera, float distance, bool moveInWorldPlane)
{
    RL_Vector3 forward = GetCameraForward(camera);

    if (moveInWorldPlane)
    {
        // Project vector onto world plane
        forward.y = 0;
        forward = RL_Vector3Normalize(forward);
    }

    // Scale by distance
    forward = RL_Vector3Scale(forward, distance);

    // Move position and target
    camera->position = RL_Vector3Add(camera->position, forward);
    camera->target = RL_Vector3Add(camera->target, forward);
}

// Moves the camera in its up direction
void CameraMoveUp(RL_Camera *camera, float distance)
{
    RL_Vector3 up = GetCameraUp(camera);

    // Scale by distance
    up = RL_Vector3Scale(up, distance);

    // Move position and target
    camera->position = RL_Vector3Add(camera->position, up);
    camera->target = RL_Vector3Add(camera->target, up);
}

// Moves the camera target in its current right direction
void CameraMoveRight(RL_Camera *camera, float distance, bool moveInWorldPlane)
{
    RL_Vector3 right = GetCameraRight(camera);

    if (moveInWorldPlane)
    {
        // Project vector onto world plane
        right.y = 0;
        right = RL_Vector3Normalize(right);
    }

    // Scale by distance
    right = RL_Vector3Scale(right, distance);

    // Move position and target
    camera->position = RL_Vector3Add(camera->position, right);
    camera->target = RL_Vector3Add(camera->target, right);
}

// Moves the camera position closer/farther to/from the camera target
void CameraMoveToTarget(RL_Camera *camera, float delta)
{
    float distance = RL_Vector3Distance(camera->position, camera->target);

    // Apply delta
    distance += delta;

    // Distance must be greater than 0
    if (distance <= 0) distance = 0.001f;

    // Set new distance by moving the position along the forward vector
    RL_Vector3 forward = GetCameraForward(camera);
    camera->position = RL_Vector3Add(camera->target, RL_Vector3Scale(forward, -distance));
}

// Rotates the camera around its up vector
// Yaw is "looking left and right"
// If rotateAroundTarget is false, the camera rotates around its position
// Note: angle must be provided in radians
void CameraYaw(RL_Camera *camera, float angle, bool rotateAroundTarget)
{
    // Rotation axis
    RL_Vector3 up = GetCameraUp(camera);

    // View vector
    RL_Vector3 targetPosition = RL_Vector3Subtract(camera->target, camera->position);

    // Rotate view vector around up axis
    targetPosition = RL_Vector3RotateByAxisAngle(targetPosition, up, angle);

    if (rotateAroundTarget)
    {
        // Move position relative to target
        camera->position = RL_Vector3Subtract(camera->target, targetPosition);
    }
    else // rotate around camera.position
    {
        // Move target relative to position
        camera->target = RL_Vector3Add(camera->position, targetPosition);
    }
}

// Rotates the camera around its right vector, pitch is "looking up and down"
//  - lockView prevents camera overrotation (aka "somersaults")
//  - rotateAroundTarget defines if rotation is around target or around its position
//  - rotateUp rotates the up direction as well (typically only usefull in RL_CAMERA_FREE)
// NOTE: angle must be provided in radians
void CameraPitch(RL_Camera *camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
{
    // Up direction
    RL_Vector3 up = GetCameraUp(camera);

    // View vector
    RL_Vector3 targetPosition = RL_Vector3Subtract(camera->target, camera->position);

    if (lockView)
    {
        // In these camera modes we clamp the Pitch angle
        // to allow only viewing straight up or down.

        // RL_Clamp view up
        float maxAngleUp = RL_Vector3Angle(up, targetPosition);
        maxAngleUp -= 0.001f; // avoid numerical errors
        if (angle > maxAngleUp) angle = maxAngleUp;

        // RL_Clamp view down
        float maxAngleDown = RL_Vector3Angle(RL_Vector3Negate(up), targetPosition);
        maxAngleDown *= -1.0f; // downwards angle is negative
        maxAngleDown += 0.001f; // avoid numerical errors
        if (angle < maxAngleDown) angle = maxAngleDown;
    }

    // Rotation axis
    RL_Vector3 right = GetCameraRight(camera);

    // Rotate view vector around right axis
    targetPosition = RL_Vector3RotateByAxisAngle(targetPosition, right, angle);

    if (rotateAroundTarget)
    {
        // Move position relative to target
        camera->position = RL_Vector3Subtract(camera->target, targetPosition);
    }
    else // rotate around camera.position
    {
        // Move target relative to position
        camera->target = RL_Vector3Add(camera->position, targetPosition);
    }

    if (rotateUp)
    {
        // Rotate up direction around right axis
        camera->up = RL_Vector3RotateByAxisAngle(camera->up, right, angle);
    }
}

// Rotates the camera around its forward vector
// Roll is "turning your head sideways to the left or right"
// Note: angle must be provided in radians
void CameraRoll(RL_Camera *camera, float angle)
{
    // Rotation axis
    RL_Vector3 forward = GetCameraForward(camera);

    // Rotate up direction around forward axis
    camera->up = RL_Vector3RotateByAxisAngle(camera->up, forward, angle);
}

// Returns the camera view matrix
RL_Matrix GetCameraViewMatrix(RL_Camera *camera)
{
    return RL_MatrixLookAt(camera->position, camera->target, camera->up);
}

// Returns the camera projection matrix
RL_Matrix GetCameraProjectionMatrix(RL_Camera *camera, float aspect)
{
    if (camera->projection == RL_CAMERA_PERSPECTIVE)
    {
        return RL_MatrixPerspective(camera->fovy*RL_DEG2RAD, aspect, RL_CAMERA_CULL_DISTANCE_NEAR, RL_CAMERA_CULL_DISTANCE_FAR);
    }
    else if (camera->projection == RL_CAMERA_ORTHOGRAPHIC)
    {
        double top = camera->fovy/2.0;
        double right = top*aspect;

        return RL_MatrixOrtho(-right, right, -top, top, RL_CAMERA_CULL_DISTANCE_NEAR, RL_CAMERA_CULL_DISTANCE_FAR);
    }

    return RL_MatrixIdentity();
}

#if !defined(RCAMERA_STANDALONE)
// Update camera position for selected mode
// RL_Camera mode: RL_CAMERA_FREE, RL_CAMERA_FIRST_PERSON, RL_CAMERA_THIRD_PERSON, RL_CAMERA_ORBITAL or CUSTOM
void RL_UpdateCamera(RL_Camera *camera, int mode)
{
    RL_Vector2 mousePositionDelta = RL_GetMouseDelta();

    bool moveInWorldPlane = ((mode == RL_CAMERA_FIRST_PERSON) || (mode == RL_CAMERA_THIRD_PERSON));
    bool rotateAroundTarget = ((mode == RL_CAMERA_THIRD_PERSON) || (mode == RL_CAMERA_ORBITAL));
    bool lockView = ((mode == RL_CAMERA_FIRST_PERSON) || (mode == RL_CAMERA_THIRD_PERSON) || (mode == RL_CAMERA_ORBITAL));
    bool rotateUp = false;

    if (mode == RL_CAMERA_ORBITAL)
    {
        // Orbital can just orbit
        RL_Matrix rotation = RL_MatrixRotate(GetCameraUp(camera), RL_CAMERA_ORBITAL_SPEED*RL_GetFrameTime());
        RL_Vector3 view = RL_Vector3Subtract(camera->position, camera->target);
        view = RL_Vector3Transform(view, rotation);
        camera->position = RL_Vector3Add(camera->target, view);
    }
    else
    {
        // RL_Camera rotation
        if (RL_IsKeyDown(RL_KEY_DOWN)) CameraPitch(camera, -RL_CAMERA_ROTATION_SPEED, lockView, rotateAroundTarget, rotateUp);
        if (RL_IsKeyDown(RL_KEY_UP)) CameraPitch(camera, RL_CAMERA_ROTATION_SPEED, lockView, rotateAroundTarget, rotateUp);
        if (RL_IsKeyDown(RL_KEY_RIGHT)) CameraYaw(camera, -RL_CAMERA_ROTATION_SPEED, rotateAroundTarget);
        if (RL_IsKeyDown(RL_KEY_LEFT)) CameraYaw(camera, RL_CAMERA_ROTATION_SPEED, rotateAroundTarget);
        if (RL_IsKeyDown(RL_KEY_Q)) CameraRoll(camera, -RL_CAMERA_ROTATION_SPEED);
        if (RL_IsKeyDown(RL_KEY_E)) CameraRoll(camera, RL_CAMERA_ROTATION_SPEED);

        // RL_Camera movement
        if (!RL_IsGamepadAvailable(0))
        {
            // RL_Camera pan (for RL_CAMERA_FREE)
            if ((mode == RL_CAMERA_FREE) && (RL_IsMouseButtonDown(RL_MOUSE_BUTTON_MIDDLE)))
            {
                const RL_Vector2 mouseDelta = RL_GetMouseDelta();
                if (mouseDelta.x > 0.0f) CameraMoveRight(camera, RL_CAMERA_PAN_SPEED, moveInWorldPlane);
                if (mouseDelta.x < 0.0f) CameraMoveRight(camera, -RL_CAMERA_PAN_SPEED, moveInWorldPlane);
                if (mouseDelta.y > 0.0f) CameraMoveUp(camera, -RL_CAMERA_PAN_SPEED);
                if (mouseDelta.y < 0.0f) CameraMoveUp(camera, RL_CAMERA_PAN_SPEED);
            }
            else
            {
                // Mouse support
                CameraYaw(camera, -mousePositionDelta.x*RL_CAMERA_MOUSE_MOVE_SENSITIVITY, rotateAroundTarget);
                CameraPitch(camera, -mousePositionDelta.y*RL_CAMERA_MOUSE_MOVE_SENSITIVITY, lockView, rotateAroundTarget, rotateUp);
            }

            // Keyboard support
            if (RL_IsKeyDown(RL_KEY_W)) CameraMoveForward(camera, RL_CAMERA_MOVE_SPEED, moveInWorldPlane);
            if (RL_IsKeyDown(RL_KEY_A)) CameraMoveRight(camera, -RL_CAMERA_MOVE_SPEED, moveInWorldPlane);
            if (RL_IsKeyDown(RL_KEY_S)) CameraMoveForward(camera, -RL_CAMERA_MOVE_SPEED, moveInWorldPlane);
            if (RL_IsKeyDown(RL_KEY_D)) CameraMoveRight(camera, RL_CAMERA_MOVE_SPEED, moveInWorldPlane);
        }
        else
        {
            // Gamepad controller support
            CameraYaw(camera, -(RL_GetGamepadAxisMovement(0, RL_GAMEPAD_AXIS_RIGHT_X) * 2)*RL_CAMERA_MOUSE_MOVE_SENSITIVITY, rotateAroundTarget);
            CameraPitch(camera, -(RL_GetGamepadAxisMovement(0, RL_GAMEPAD_AXIS_RIGHT_Y) * 2)*RL_CAMERA_MOUSE_MOVE_SENSITIVITY, lockView, rotateAroundTarget, rotateUp);

            if (RL_GetGamepadAxisMovement(0, RL_GAMEPAD_AXIS_LEFT_Y) <= -0.25f) CameraMoveForward(camera, RL_CAMERA_MOVE_SPEED, moveInWorldPlane);
            if (RL_GetGamepadAxisMovement(0, RL_GAMEPAD_AXIS_LEFT_X) <= -0.25f) CameraMoveRight(camera, -RL_CAMERA_MOVE_SPEED, moveInWorldPlane);
            if (RL_GetGamepadAxisMovement(0, RL_GAMEPAD_AXIS_LEFT_Y) >= 0.25f) CameraMoveForward(camera, -RL_CAMERA_MOVE_SPEED, moveInWorldPlane);
            if (RL_GetGamepadAxisMovement(0, RL_GAMEPAD_AXIS_LEFT_X) >= 0.25f) CameraMoveRight(camera, RL_CAMERA_MOVE_SPEED, moveInWorldPlane);
        }

        if (mode == RL_CAMERA_FREE)
        {
            if (RL_IsKeyDown(RL_KEY_SPACE)) CameraMoveUp(camera, RL_CAMERA_MOVE_SPEED);
            if (RL_IsKeyDown(RL_KEY_LEFT_CONTROL)) CameraMoveUp(camera, -RL_CAMERA_MOVE_SPEED);
        }
    }

    if ((mode == RL_CAMERA_THIRD_PERSON) || (mode == RL_CAMERA_ORBITAL) || (mode == RL_CAMERA_FREE))
    {
        // Zoom target distance
        CameraMoveToTarget(camera, -RL_GetMouseWheelMove());
        if (RL_IsKeyPressed(RL_KEY_KP_SUBTRACT)) CameraMoveToTarget(camera, 2.0f);
        if (RL_IsKeyPressed(RL_KEY_KP_ADD)) CameraMoveToTarget(camera, -2.0f);
    }
}
#endif // !RCAMERA_STANDALONE

// Update camera movement, movement/rotation values should be provided by user
void RL_UpdateCameraPro(RL_Camera *camera, RL_Vector3 movement, RL_Vector3 rotation, float zoom)
{
    // Required values
    // movement.x - Move forward/backward
    // movement.y - Move right/left
    // movement.z - Move up/down
    // rotation.x - yaw
    // rotation.y - pitch
    // rotation.z - roll
    // zoom - Move towards target

    bool lockView = true;
    bool rotateAroundTarget = false;
    bool rotateUp = false;
    bool moveInWorldPlane = true;

    // RL_Camera rotation
    CameraPitch(camera, -rotation.y*RL_DEG2RAD, lockView, rotateAroundTarget, rotateUp);
    CameraYaw(camera, -rotation.x*RL_DEG2RAD, rotateAroundTarget);
    CameraRoll(camera, rotation.z*RL_DEG2RAD);

    // RL_Camera movement
    CameraMoveForward(camera, movement.x, moveInWorldPlane);
    CameraMoveRight(camera, movement.y, moveInWorldPlane);
    CameraMoveUp(camera, movement.z);

    // Zoom target distance
    CameraMoveToTarget(camera, zoom);
}

#endif // RCAMERA_IMPLEMENTATION
