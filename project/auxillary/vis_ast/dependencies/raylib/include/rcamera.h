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

RL_NS_BEGIN

//----------------------------------------------------------------------------------
// Types and Structures Definition
// NOTE: Below types are required for standalone usage
//----------------------------------------------------------------------------------
#if defined(RCAMERA_STANDALONE)
    // Vector2, 2 components
    typedef struct Vector2 {
        float x;                // Vector x component
        float y;                // Vector y component
    } Vector2;

    // Vector3, 3 components
    typedef struct Vector3 {
        float x;                // Vector x component
        float y;                // Vector y component
        float z;                // Vector z component
    } Vector3;

    // Matrix, 4x4 components, column major, OpenGL style, right-handed
    typedef struct Matrix {
        float m0, m4, m8, m12;  // Matrix first row (4 components)
        float m1, m5, m9, m13;  // Matrix second row (4 components)
        float m2, m6, m10, m14; // Matrix third row (4 components)
        float m3, m7, m11, m15; // Matrix fourth row (4 components)
    } Matrix;

    // Camera type, defines a camera position/orientation in 3d space
    typedef struct Camera3D {
        Vector3 position;       // Camera position
        Vector3 target;         // Camera target it looks-at
        Vector3 up;             // Camera up vector (rotation over its axis)
        float fovy;             // Camera field-of-view apperture in Y (degrees) in perspective, used as near plane width in orthographic
        int projection;         // Camera projection type: CAMERA_PERSPECTIVE or CAMERA_ORTHOGRAPHIC
    } Camera3D;

    typedef Camera3D Camera;    // Camera type fallback, defaults to Camera3D

    // Camera projection
    typedef enum {
        CAMERA_PERSPECTIVE = 0, // Perspective projection
        CAMERA_ORTHOGRAPHIC     // Orthographic projection
    } CameraProjection;

    // Camera system modes
    typedef enum {
        CAMERA_CUSTOM = 0,      // Camera custom, controlled by user (update_camera() does nothing)
        CAMERA_FREE,            // Camera free mode
        CAMERA_ORBITAL,         // Camera orbital, around target, zoom supported
        CAMERA_FIRST_PERSON,    // Camera first person
        CAMERA_THIRD_PERSON     // Camera third person
    } CameraMode;
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

RL_EXTERN_C_BEGIN

RLAPI Vector3 get_camera_forward(Camera *camera);
RLAPI Vector3 get_camera_up(Camera *camera);
RLAPI Vector3 get_camera_right(Camera *camera);

// Camera movement
RLAPI void camera_move_forward(Camera *camera, float distance, bool moveInWorldPlane);
RLAPI void camera_move_up(Camera *camera, float distance);
RLAPI void camera_move_right(Camera *camera, float distance, bool moveInWorldPlane);
RLAPI void camera_move_to_target(Camera *camera, float delta);

// Camera rotation
RLAPI void camera_yaw(Camera *camera, float angle, bool rotateAroundTarget);
RLAPI void camera_pitch(Camera *camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp);
RLAPI void camera_roll(Camera *camera, float angle);

RLAPI Matrix get_camera_view_matrix(Camera *camera);
RLAPI Matrix get_camera_projection_matrix(Camera* camera, float aspect);

RL_EXTERN_C_END

RL_NS_END

#endif // RCAMERA_H


/***********************************************************************************
*
*   CAMERA IMPLEMENTATION
*
************************************************************************************/

#if defined(RCAMERA_IMPLEMENTATION)

#include "raymath.h"        // Required for vector maths:
                            // vector3_add()
                            // vector3_subtract()
                            // vector3_scale()
                            // vector3_normalize()
                            // vector3_distance()
                            // vector3_cross_product()
                            // vector3_rotate_by_axis_angle()
                            // vector3_angle()
                            // vector3_negate()
                            // matrix_look_at()
                            // matrix_perspective()
                            // matrix_ortho()
                            // matrix_identity()

// raylib required functionality:
                            // get_mouse_delta()
                            // get_mouse_wheel_move()
                            // is_key_down()
                            // is_key_pressed()
                            // get_frame_time()

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define CAMERA_MOVE_SPEED                               0.09f
#define CAMERA_ROTATION_SPEED                           0.03f
#define CAMERA_PAN_SPEED                                0.2f

// Camera mouse movement sensitivity
#define CAMERA_MOUSE_MOVE_SENSITIVITY                   0.003f     // TODO: it should be independant of framerate
#define CAMERA_MOUSE_SCROLL_SENSITIVITY                 1.5f

#define CAMERA_ORBITAL_SPEED                            0.5f       // Radians per second


#define CAMERA_FIRST_PERSON_STEP_TRIGONOMETRIC_DIVIDER  8.0f
#define CAMERA_FIRST_PERSON_STEP_DIVIDER                30.0f
#define CAMERA_FIRST_PERSON_WAVING_DIVIDER              200.0f

// PLAYER (used by camera)
#define PLAYER_MOVEMENT_SENSITIVITY                     20.0f

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

RL_NS_BEGIN

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
// Returns the cameras forward vector (normalized)
Vector3 get_camera_forward(Camera *camera)
{
    return vector3_normalize(vector3_subtract(camera->target, camera->position));
}

// Returns the cameras up vector (normalized)
// Note: The up vector might not be perpendicular to the forward vector
Vector3 get_camera_up(Camera *camera)
{
    return vector3_normalize(camera->up);
}

// Returns the cameras right vector (normalized)
Vector3 get_camera_right(Camera *camera)
{
    Vector3 forward = get_camera_forward(camera);
    Vector3 up = get_camera_up(camera);

    return vector3_cross_product(forward, up);
}

// Moves the camera in its forward direction
void camera_move_forward(Camera *camera, float distance, bool moveInWorldPlane)
{
    Vector3 forward = get_camera_forward(camera);

    if (moveInWorldPlane)
    {
        // Project vector onto world plane
        forward.y = 0;
        forward = vector3_normalize(forward);
    }

    // Scale by distance
    forward = vector3_scale(forward, distance);

    // Move position and target
    camera->position = vector3_add(camera->position, forward);
    camera->target = vector3_add(camera->target, forward);
}

// Moves the camera in its up direction
void camera_move_up(Camera *camera, float distance)
{
    Vector3 up = get_camera_up(camera);

    // Scale by distance
    up = vector3_scale(up, distance);

    // Move position and target
    camera->position = vector3_add(camera->position, up);
    camera->target = vector3_add(camera->target, up);
}

// Moves the camera target in its current right direction
void camera_move_right(Camera *camera, float distance, bool moveInWorldPlane)
{
    Vector3 right = get_camera_right(camera);

    if (moveInWorldPlane)
    {
        // Project vector onto world plane
        right.y = 0;
        right = vector3_normalize(right);
    }

    // Scale by distance
    right = vector3_scale(right, distance);

    // Move position and target
    camera->position = vector3_add(camera->position, right);
    camera->target = vector3_add(camera->target, right);
}

// Moves the camera position closer/farther to/from the camera target
void camera_move_to_target(Camera *camera, float delta)
{
    float distance = vector3_distance(camera->position, camera->target);

    // Apply delta
    distance += delta;

    // Distance must be greater than 0
    if (distance <= 0) distance = 0.001f;

    // Set new distance by moving the position along the forward vector
    Vector3 forward = get_camera_forward(camera);
    camera->position = vector3_add(camera->target, vector3_scale(forward, -distance));
}

// Rotates the camera around its up vector
// Yaw is "looking left and right"
// If rotateAroundTarget is false, the camera rotates around its position
// Note: angle must be provided in radians
void camera_yaw(Camera *camera, float angle, bool rotateAroundTarget)
{
    // Rotation axis
    Vector3 up = get_camera_up(camera);

    // View vector
    Vector3 targetPosition = vector3_subtract(camera->target, camera->position);

    // Rotate view vector around up axis
    targetPosition = vector3_rotate_by_axis_angle(targetPosition, up, angle);

    if (rotateAroundTarget)
    {
        // Move position relative to target
        camera->position = vector3_subtract(camera->target, targetPosition);
    }
    else // rotate around camera.position
    {
        // Move target relative to position
        camera->target = vector3_add(camera->position, targetPosition);
    }
}

// Rotates the camera around its right vector, pitch is "looking up and down"
//  - lockView prevents camera overrotation (aka "somersaults")
//  - rotateAroundTarget defines if rotation is around target or around its position
//  - rotateUp rotates the up direction as well (typically only usefull in CAMERA_FREE)
// NOTE: angle must be provided in radians
void camera_pitch(Camera *camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
{
    // Up direction
    Vector3 up = get_camera_up(camera);

    // View vector
    Vector3 targetPosition = vector3_subtract(camera->target, camera->position);

    if (lockView)
    {
        // In these camera modes we clamp the Pitch angle
        // to allow only viewing straight up or down.

        // clamp view up
        float maxAngleUp = vector3_angle(up, targetPosition);
        maxAngleUp -= 0.001f; // avoid numerical errors
        if (angle > maxAngleUp) angle = maxAngleUp;

        // clamp view down
        float maxAngleDown = vector3_angle(vector3_negate(up), targetPosition);
        maxAngleDown *= -1.0f; // downwards angle is negative
        maxAngleDown += 0.001f; // avoid numerical errors
        if (angle < maxAngleDown) angle = maxAngleDown;
    }

    // Rotation axis
    Vector3 right = get_camera_right(camera);

    // Rotate view vector around right axis
    targetPosition = vector3_rotate_by_axis_angle(targetPosition, right, angle);

    if (rotateAroundTarget)
    {
        // Move position relative to target
        camera->position = vector3_subtract(camera->target, targetPosition);
    }
    else // rotate around camera.position
    {
        // Move target relative to position
        camera->target = vector3_add(camera->position, targetPosition);
    }

    if (rotateUp)
    {
        // Rotate up direction around right axis
        camera->up = vector3_rotate_by_axis_angle(camera->up, right, angle);
    }
}

// Rotates the camera around its forward vector
// Roll is "turning your head sideways to the left or right"
// Note: angle must be provided in radians
void camera_roll(Camera *camera, float angle)
{
    // Rotation axis
    Vector3 forward = get_camera_forward(camera);

    // Rotate up direction around forward axis
    camera->up = vector3_rotate_by_axis_angle(camera->up, forward, angle);
}

// Returns the camera view matrix
Matrix get_camera_view_matrix(Camera *camera)
{
    return matrix_look_at(camera->position, camera->target, camera->up);
}

// Returns the camera projection matrix
Matrix get_camera_projection_matrix(Camera *camera, float aspect)
{
    if (camera->projection == CAMERA_PERSPECTIVE)
    {
        return matrix_perspective(camera->fovy*RL_DEG2RAD, aspect, RL_CAMERA_CULL_DISTANCE_NEAR, RL_CAMERA_CULL_DISTANCE_FAR);
    }
    else if (camera->projection == CAMERA_ORTHOGRAPHIC)
    {
        double top = camera->fovy/2.0;
        double right = top*aspect;

        return matrix_ortho(-right, right, -top, top, RL_CAMERA_CULL_DISTANCE_NEAR, RL_CAMERA_CULL_DISTANCE_FAR);
    }

    return matrix_identity();
}

#if !defined(RCAMERA_STANDALONE)
// Update camera position for selected mode
// Camera mode: CAMERA_FREE, CAMERA_FIRST_PERSON, CAMERA_THIRD_PERSON, CAMERA_ORBITAL or CUSTOM
void update_camera(Camera *camera, int mode)
{
    Vector2 mousePositionDelta = get_mouse_delta();

    bool moveInWorldPlane = ((mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON));
    bool rotateAroundTarget = ((mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL));
    bool lockView = ((mode == CAMERA_FIRST_PERSON) || (mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL));
    bool rotateUp = false;

    if (mode == CAMERA_ORBITAL)
    {
        // Orbital can just orbit
        Matrix rotation = matrix_rotate(get_camera_up(camera), CAMERA_ORBITAL_SPEED*get_frame_time());
        Vector3 view = vector3_subtract(camera->position, camera->target);
        view = vector3_transform(view, rotation);
        camera->position = vector3_add(camera->target, view);
    }
    else
    {
        // Camera rotation
        if (is_key_down(KEY_DOWN)) camera_pitch(camera, -CAMERA_ROTATION_SPEED, lockView, rotateAroundTarget, rotateUp);
        if (is_key_down(KEY_UP)) camera_pitch(camera, CAMERA_ROTATION_SPEED, lockView, rotateAroundTarget, rotateUp);
        if (is_key_down(KEY_RIGHT)) camera_yaw(camera, -CAMERA_ROTATION_SPEED, rotateAroundTarget);
        if (is_key_down(KEY_LEFT)) camera_yaw(camera, CAMERA_ROTATION_SPEED, rotateAroundTarget);
        if (is_key_down(KEY_Q)) camera_roll(camera, -CAMERA_ROTATION_SPEED);
        if (is_key_down(KEY_E)) camera_roll(camera, CAMERA_ROTATION_SPEED);

        // Camera movement
        if (!is_gamepad_available(0))
        {
            // Camera pan (for CAMERA_FREE)
            if ((mode == CAMERA_FREE) && (is_mouse_button_down(MOUSE_BUTTON_MIDDLE)))
            {
                const Vector2 mouseDelta = get_mouse_delta();
                if (mouseDelta.x > 0.0f) camera_move_right(camera, CAMERA_PAN_SPEED, moveInWorldPlane);
                if (mouseDelta.x < 0.0f) camera_move_right(camera, -CAMERA_PAN_SPEED, moveInWorldPlane);
                if (mouseDelta.y > 0.0f) camera_move_up(camera, -CAMERA_PAN_SPEED);
                if (mouseDelta.y < 0.0f) camera_move_up(camera, CAMERA_PAN_SPEED);
            }
            else
            {
                // Mouse support
                camera_yaw(camera, -mousePositionDelta.x*CAMERA_MOUSE_MOVE_SENSITIVITY, rotateAroundTarget);
                camera_pitch(camera, -mousePositionDelta.y*CAMERA_MOUSE_MOVE_SENSITIVITY, lockView, rotateAroundTarget, rotateUp);
            }

            // Keyboard support
            if (is_key_down(KEY_W)) camera_move_forward(camera, CAMERA_MOVE_SPEED, moveInWorldPlane);
            if (is_key_down(KEY_A)) camera_move_right(camera, -CAMERA_MOVE_SPEED, moveInWorldPlane);
            if (is_key_down(KEY_S)) camera_move_forward(camera, -CAMERA_MOVE_SPEED, moveInWorldPlane);
            if (is_key_down(KEY_D)) camera_move_right(camera, CAMERA_MOVE_SPEED, moveInWorldPlane);
        }
        else
        {
            // Gamepad controller support
            camera_yaw(camera, -(get_gamepad_axis_movement(0, GAMEPAD_AXIS_RIGHT_X) * 2)*CAMERA_MOUSE_MOVE_SENSITIVITY, rotateAroundTarget);
            camera_pitch(camera, -(get_gamepad_axis_movement(0, GAMEPAD_AXIS_RIGHT_Y) * 2)*CAMERA_MOUSE_MOVE_SENSITIVITY, lockView, rotateAroundTarget, rotateUp);

            if (get_gamepad_axis_movement(0, GAMEPAD_AXIS_LEFT_Y) <= -0.25f) camera_move_forward(camera, CAMERA_MOVE_SPEED, moveInWorldPlane);
            if (get_gamepad_axis_movement(0, GAMEPAD_AXIS_LEFT_X) <= -0.25f) camera_move_right(camera, -CAMERA_MOVE_SPEED, moveInWorldPlane);
            if (get_gamepad_axis_movement(0, GAMEPAD_AXIS_LEFT_Y) >= 0.25f) camera_move_forward(camera, -CAMERA_MOVE_SPEED, moveInWorldPlane);
            if (get_gamepad_axis_movement(0, GAMEPAD_AXIS_LEFT_X) >= 0.25f) camera_move_right(camera, CAMERA_MOVE_SPEED, moveInWorldPlane);
        }

        if (mode == CAMERA_FREE)
        {
            if (is_key_down(KEY_SPACE)) camera_move_up(camera, CAMERA_MOVE_SPEED);
            if (is_key_down(KEY_LEFT_CONTROL)) camera_move_up(camera, -CAMERA_MOVE_SPEED);
        }
    }

    if ((mode == CAMERA_THIRD_PERSON) || (mode == CAMERA_ORBITAL) || (mode == CAMERA_FREE))
    {
        // Zoom target distance
        camera_move_to_target(camera, -get_mouse_wheel_move());
        if (is_key_pressed(KEY_KP_SUBTRACT)) camera_move_to_target(camera, 2.0f);
        if (is_key_pressed(KEY_KP_ADD)) camera_move_to_target(camera, -2.0f);
    }
}
#endif // !RCAMERA_STANDALONE

// Update camera movement, movement/rotation values should be provided by user
void update_camera_pro(Camera *camera, Vector3 movement, Vector3 rotation, float zoom)
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

    // Camera rotation
    camera_pitch(camera, -rotation.y*RL_DEG2RAD, lockView, rotateAroundTarget, rotateUp);
    camera_yaw(camera, -rotation.x*RL_DEG2RAD, rotateAroundTarget);
    camera_roll(camera, rotation.z*RL_DEG2RAD);

    // Camera movement
    camera_move_forward(camera, movement.x, moveInWorldPlane);
    camera_move_right(camera, movement.y, moveInWorldPlane);
    camera_move_up(camera, movement.z);

    // Zoom target distance
    camera_move_to_target(camera, zoom);
}

RL_NS_END

#endif // RCAMERA_IMPLEMENTATION
