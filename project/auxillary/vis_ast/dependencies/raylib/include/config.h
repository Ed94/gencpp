/**********************************************************************************************
 *
 *   raylib configuration flags
 *
 *   This file defines all the configuration flags for the different raylib modules
 *
 *   LICENSE: zlib/libpng
 *
 *   Copyright (c) 2018-2023 Ahmad Fatoum & Ramon Santamaria (@raysan5)
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

#ifndef CONFIG_H
#define RL_CONFIG_H

//------------------------------------------------------------------------------------
// Module selection - Some modules could be avoided
// Mandatory modules: rcore, rlgl, utils
//------------------------------------------------------------------------------------
#define RL_SUPPORT_MODULE_RSHAPES   1
#define RL_SUPPORT_MODULE_RTEXTURES 1
#define RL_SUPPORT_MODULE_RTEXT     1    // WARNING: It requires SUPPORT_MODULE_RTEXTURES to load sprite font textures
#define RL_SUPPORT_MODULE_RMODELS   1
#define RL_SUPPORT_MODULE_RAUDIO    1

//------------------------------------------------------------------------------------
// Module: rcore - Configuration Flags
//------------------------------------------------------------------------------------
// Camera module is included (rcamera.h) and multiple predefined cameras are available: free, 1st/3rd person, orbital
#define RL_SUPPORT_CAMERA_SYSTEM 1
// Gestures module is included (rgestures.h) to support gestures detection: tap, hold, swipe, drag
#define RL_SUPPORT_GESTURES_SYSTEM 1
// Mouse gestures are directly mapped like touches and processed by gestures system
#define RL_SUPPORT_MOUSE_GESTURES 1
// Reconfigure standard input to receive key inputs, works with SSH connection.
#define RL_SUPPORT_SSH_KEYBOARD_RPI 1
// Setting a higher resolution can improve the accuracy of time-out intervals in wait functions.
// However, it can also reduce overall system performance, because the thread scheduler switches tasks more often.
#define RL_SUPPORT_WINMM_HIGHRES_TIMER 1
// Use busy wait loop for timing sync, if not defined, a high-resolution timer is set up and used
// #define SUPPORT_BUSY_WAIT_LOOP          1
// Use a partial-busy wait loop, in this case frame sleeps for most of the time, but then runs a busy loop at the end for accuracy
#define RL_SUPPORT_PARTIALBUSY_WAIT_LOOP 1
// Allow automatic screen capture of current screen pressing F12, defined in KeyCallback()
#define RL_SUPPORT_SCREEN_CAPTURE 1
// Allow automatic gif recording of current screen pressing CTRL+F12, defined in KeyCallback()
#define RL_SUPPORT_GIF_RECORDING 1
// Support CompressData() and DecompressData() functions
#define RL_SUPPORT_COMPRESSION_API 1
// Support automatic generated events, loading and recording of those events when required
// #define SUPPORT_EVENTS_AUTOMATION       1
// Support custom frame control, only for advance users
// By default EndDrawing() does this job: draws everything + SwapScreenBuffer() + manage frame timing + PollInputEvents()
// Enabling this flag allows manual control of the frame processes, use at your own risk
// #define SUPPORT_CUSTOM_FRAME_CONTROL    1

// rcore: Configuration values
//------------------------------------------------------------------------------------
#define RL_MAX_FILEPATH_CAPACITY  8192    // Maximum file paths capacity
#define RL_MAX_FILEPATH_LENGTH    4096    // Maximum length for filepaths (Linux PATH_MAX default value)

#define RL_MAX_KEYBOARD_KEYS      512    // Maximum number of keyboard keys supported
#define RL_MAX_MOUSE_BUTTONS      8      // Maximum number of mouse buttons supported
#define RL_MAX_GAMEPADS           4      // Maximum number of gamepads supported
#define RL_MAX_GAMEPAD_AXIS       8      // Maximum number of axis supported (per gamepad)
#define RL_MAX_GAMEPAD_BUTTONS    32     // Maximum number of buttons supported (per gamepad)
#define RL_MAX_TOUCH_POINTS       8      // Maximum number of touch points supported
#define RL_MAX_KEY_PRESSED_QUEUE  16     // Maximum number of keys in the key input queue
#define RL_MAX_CHAR_PRESSED_QUEUE 16     // Maximum number of characters in the char input queue

#define RL_MAX_DECOMPRESSION_SIZE 64    // Max size allocated for decompression in MB


//------------------------------------------------------------------------------------
// Module: rlgl - Configuration values
//------------------------------------------------------------------------------------

// Enable OpenGL Debug Context (only available on OpenGL 4.3)
// #define RLGL_ENABLE_OPENGL_DEBUG_CONTEXT       1

// Show OpenGL extensions and capabilities detailed logs on init
// #define RLGL_SHOW_GL_DETAILS_INFO              1

// #define RL_DEFAULT_BATCH_BUFFER_ELEMENTS    4096    // Default internal render batch elements limits




// Default shader vertex attribute names to set location points
// NOTE: When a new shader is loaded, the following locations are tried to be set for convenience



//------------------------------------------------------------------------------------
// Module: rshapes - Configuration Flags
//------------------------------------------------------------------------------------
// Use QUADS instead of TRIANGLES for drawing when possible
// Some lines-based shapes could still use lines
#define RL_SUPPORT_QUADS_DRAW_MODE 1


//------------------------------------------------------------------------------------
// Module: rtextures - Configuration Flags
//------------------------------------------------------------------------------------
// Selecte desired fileformats to be supported for image data loading
#define RL_SUPPORT_FILEFORMAT_PNG 1
// #define SUPPORT_FILEFORMAT_BMP      1
// #define SUPPORT_FILEFORMAT_TGA      1
// #define SUPPORT_FILEFORMAT_JPG      1
#define RL_SUPPORT_FILEFORMAT_GIF 1
#define RL_SUPPORT_FILEFORMAT_QOI 1
// #define SUPPORT_FILEFORMAT_PSD      1
#define RL_SUPPORT_FILEFORMAT_DDS 1
// #define SUPPORT_FILEFORMAT_HDR      1
// #define SUPPORT_FILEFORMAT_PIC          1
// #define SUPPORT_FILEFORMAT_KTX      1
// #define SUPPORT_FILEFORMAT_ASTC     1
// #define SUPPORT_FILEFORMAT_PKM      1
// #define SUPPORT_FILEFORMAT_PVR      1
// #define SUPPORT_FILEFORMAT_SVG      1

// Support image export functionality (.png, .bmp, .tga, .jpg, .qoi)
#define RL_SUPPORT_IMAGE_EXPORT 1
// Support procedural image generation functionality (gradient, spot, perlin-noise, cellular)
#define RL_SUPPORT_IMAGE_GENERATION 1
// Support multiple image editing functions to scale, adjust colors, flip, draw on images, crop...
// If not defined, still some functions are supported: ImageFormat(), ImageCrop(), ImageToPOT()
#define RL_SUPPORT_IMAGE_MANIPULATION 1


//------------------------------------------------------------------------------------
// Module: rtext - Configuration Flags
//------------------------------------------------------------------------------------
// Default font is loaded on window initialization to be available for the user to render simple text
// NOTE: If enabled, uses external module functions to load default raylib font
#define RL_SUPPORT_DEFAULT_FONT 1
// Selected desired font fileformats to be supported for loading
#define RL_SUPPORT_FILEFORMAT_FNT 1
#define RL_SUPPORT_FILEFORMAT_TTF 1

// Support text management functions
// If not defined, still some functions are supported: TextLength(), TextFormat()
#define RL_SUPPORT_TEXT_MANIPULATION 1

// On font atlas image generation [GenImageFontAtlas()], add a 3x3 pixels white rectangle
// at the bottom-right corner of the atlas. It can be useful to for shapes drawing, to allow
// drawing text and shapes with a single draw call [SetShapesTexture()].
#define RL_SUPPORT_FONT_ATLAS_WHITE_REC 1

// rtext: Configuration values
//------------------------------------------------------------------------------------
#define RL_MAX_TEXT_BUFFER_LENGTH 1024    // Size of internal static buffers used on some functions:
// TextFormat(), TextSubtext(), TextToUpper(), TextToLower(), TextToPascal(), TextSplit()
#define RL_MAX_TEXTSPLIT_COUNT 128    // Maximum number of substrings to split: TextSplit()


//------------------------------------------------------------------------------------
// Module: rmodels - Configuration Flags
//------------------------------------------------------------------------------------
// Selected desired model fileformats to be supported for loading
#define RL_SUPPORT_FILEFORMAT_OBJ  1
#define RL_SUPPORT_FILEFORMAT_MTL  1
#define RL_SUPPORT_FILEFORMAT_IQM  1
#define RL_SUPPORT_FILEFORMAT_GLTF 1
#define RL_SUPPORT_FILEFORMAT_VOX  1
#define RL_SUPPORT_FILEFORMAT_M3D  1
// Support procedural mesh generation functions, uses external par_shapes.h library
// NOTE: Some generated meshes DO NOT include generated texture coordinates
#define RL_SUPPORT_MESH_GENERATION 1

// rmodels: Configuration values
//------------------------------------------------------------------------------------
#define RL_MAX_MATERIAL_MAPS       12    // Maximum number of shader maps supported
#define RL_MAX_MESH_VERTEX_BUFFERS 7     // Maximum vertex buffers (VBO) per mesh

//------------------------------------------------------------------------------------
// Module: raudio - Configuration Flags
//------------------------------------------------------------------------------------
// Desired audio fileformats to be supported for loading
#define RL_SUPPORT_FILEFORMAT_WAV 1
#define RL_SUPPORT_FILEFORMAT_OGG 1
#define RL_SUPPORT_FILEFORMAT_MP3 1
#define RL_SUPPORT_FILEFORMAT_QOA 1
// #define SUPPORT_FILEFORMAT_FLAC         1
#define RL_SUPPORT_FILEFORMAT_XM  1
#define RL_SUPPORT_FILEFORMAT_MOD 1

// raudio: Configuration values
//------------------------------------------------------------------------------------
#define RL_AUDIO_DEVICE_FORMAT            ma_format_f32    // Device output format (miniaudio: float-32bit)
#define RL_AUDIO_DEVICE_CHANNELS          2                // Device output channels: stereo
#define RL_AUDIO_DEVICE_SAMPLE_RATE       0                // Device sample rate (device default)

#define RL_MAX_AUDIO_BUFFER_POOL_CHANNELS 16    // Maximum number of audio pool channels

//------------------------------------------------------------------------------------
// Module: utils - Configuration Flags
//------------------------------------------------------------------------------------
// Standard file io library (stdio.h) included
#define RL_SUPPORT_STANDARD_FILEIO 1
// Show TRACELOG() output messages
// NOTE: By default LOG_DEBUG traces not shown
#define RL_SUPPORT_TRACELOG 1
// #define SUPPORT_TRACELOG_DEBUG          1

// utils: Configuration values
//------------------------------------------------------------------------------------
#define RL_MAX_TRACELOG_MSG_LENGTH 256    // Max length of one trace-log message

#endif
// CONFIG_H
