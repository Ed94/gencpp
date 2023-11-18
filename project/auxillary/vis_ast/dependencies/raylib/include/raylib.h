/**********************************************************************************************
*
*   raylib v5.0 - A simple and easy-to-use library to enjoy videogames programming (www.raylib.com)
*
*   FEATURES:
*       - NO external dependencies, all required libraries included with raylib
*       - Multiplatform: Windows, Linux, FreeBSD, OpenBSD, NetBSD, DragonFly,
*                        MacOS, Haiku, Android, Raspberry Pi, DRM native, HTML5.
*       - Written in plain C code (C99) in PascalCase/camelCase notation
*       - Hardware accelerated with OpenGL (1.1, 2.1, 3.3, 4.3 or ES2 - choose at compile)
*       - Unique OpenGL abstraction layer (usable as standalone module): [RLGL_]
*       - Multiple Fonts formats supported (TTF, XNA fonts, AngelCode fonts)
*       - Outstanding texture formats support, including compressed formats (DXT, ETC, ASTC)
*       - Full 3d support for 3d Shapes, Models, Billboards, Heightmaps and more!
*       - Flexible Materials system, supporting classic maps and PBR maps
*       - Animated 3D models supported (skeletal bones animation) (IQM)
*       - Shaders support, including RL_Model shaders and Postprocessing shaders
*       - Powerful math module for Vector, RL_Matrix and RL_Quaternion operations: [raymath]
*       - Audio loading and playing with streaming support (WAV, OGG, MP3, FLAC, XM, MOD)
*       - VR stereo rendering with configurable HMD device parameters
*       - Bindings to multiple programming languages available!
*
*   NOTES:
*       - One default RL_Font is loaded on RL_InitWindow()->LoadFontDefault() [core, text]
*       - One default RL_Texture2D is loaded on RLGL_Init(), 1x1 white pixel R8G8B8A8 [RLGL_] (OpenGL 3.3 or ES2)
*       - One default RL_Shader is loaded on RLGL_Init()->RLGL_LoadShaderDefault() [RLGL_] (OpenGL 3.3 or ES2)
*       - One default RenderBatch is loaded on RLGL_Init()->RLGL_LoadRenderBatch() [RLGL_] (OpenGL 3.3 or ES2)
*
*   DEPENDENCIES (included):
*       [rcore] rglfw (Camilla Löwy - github.com/glfw/glfw) for window/context management and input (PLATFORM_DESKTOP)
*       [RLGL_] glad (David Herberth - github.com/Dav1dde/glad) for OpenGL 3.3 extensions loading (PLATFORM_DESKTOP)
*       [raudio] miniaudio (David Reid - github.com/mackron/miniaudio) for audio device/context management
*
*   OPTIONAL DEPENDENCIES (included):
*       [rcore] msf_gif (Miles Fogle) for GIF recording
*       [rcore] sinfl (Micha Mettke) for DEFLATE decompression algorithm
*       [rcore] sdefl (Micha Mettke) for DEFLATE compression algorithm
*       [rtextures] stb_image (Sean Barret) for images loading (BMP, TGA, PNG, JPEG, HDR...)
*       [rtextures] stb_image_write (Sean Barret) for image writing (BMP, TGA, PNG, JPG)
*       [rtextures] stb_image_resize (Sean Barret) for image resizing algorithms
*       [rtext] stb_truetype (Sean Barret) for ttf fonts loading
*       [rtext] stb_rect_pack (Sean Barret) for rectangles packing
*       [rmodels] par_shapes (Philip Rideout) for parametric 3d shapes generation
*       [rmodels] tinyobj_loader_c (Syoyo Fujita) for models loading (OBJ, MTL)
*       [rmodels] cgltf (Johannes Kuhlmann) for models loading (glTF)
*       [rmodels] Model3D (bzt) for models loading (M3D, https://bztsrc.gitlab.io/model3d)
*       [raudio] dr_wav (David Reid) for WAV audio file loading
*       [raudio] dr_flac (David Reid) for FLAC audio file loading
*       [raudio] dr_mp3 (David Reid) for MP3 audio file loading
*       [raudio] stb_vorbis (Sean Barret) for OGG audio loading
*       [raudio] jar_xm (Joshua Reisenauer) for XM audio module loading
*       [raudio] jar_mod (Joshua Reisenauer) for MOD audio module loading
*
*
*   LICENSE: zlib/libpng
*
*   raylib is licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software:
*
*   Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)
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

#ifndef RAYLIB_H
#define RAYLIB_H

#include <stdarg.h>     // Required for: va_list - Only used by RL_TraceLogCallback

#define RAYLIB_VERSION_MAJOR 5
#define RAYLIB_VERSION_MINOR 0
#define RAYLIB_VERSION_PATCH 0
#define RAYLIB_VERSION  "5.0"

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

//----------------------------------------------------------------------------------
// Some basic Defines
//----------------------------------------------------------------------------------
#ifndef RL_PI
    #define RL_PI 3.14159265358979323846f
#endif
#ifndef RL_DEG2RAD
    #define RL_DEG2RAD (RL_PI/180.0f)
#endif
#ifndef RL_RAD2DEG
    #define RL_RAD2DEG (180.0f/RL_PI)
#endif

// Allow custom memory allocators
// NOTE: Require recompiling raylib sources
#ifndef RL_MALLOC
    #define RL_MALLOC(sz)       malloc(sz)
#endif
#ifndef RL_CALLOC
    #define RL_CALLOC(n,sz)     calloc(n,sz)
#endif
#ifndef RL_REALLOC
    #define RL_REALLOC(ptr,sz)  realloc(ptr,sz)
#endif
#ifndef RL_FREE
    #define RL_FREE(ptr)        free(ptr)
#endif

// NOTE: MSVC C++ compiler does not support compound literals (C99 feature)
// Plain structures in C++ (without constructors) can be initialized with { }
// This is called aggregate initialization (C++11 feature)
#if defined(__cplusplus)
    #define RL_CLITERAL(type)      type
#else
    #define RL_CLITERAL(type)      (type)
#endif

// Some compilers (mostly macos clang) default to C++98,
// where aggregate initialization can't be used
// So, give a more clear error stating how to fix this
#if !defined(_MSC_VER) && (defined(__cplusplus) && __cplusplus < 201103L)
    #error "C++11 or later is required. Add -std=c++11"
#endif

// NOTE: We set some defines with some data types declared by raylib
// Other modules (raymath, RLGL_) also require some of those types, so,
// to be able to use those other modules as standalone (not depending on raylib)
// this defines are very useful for internal check and avoid type (re)definitions
#define RL_COLOR_TYPE
#define RL_RECTANGLE_TYPE
#define RL_VECTOR2_TYPE
#define RL_VECTOR3_TYPE
#define RL_VECTOR4_TYPE
#define RL_QUATERNION_TYPE
#define RL_MATRIX_TYPE

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on RL_WHITE background
#define RL_LIGHTGRAY  RL_CLITERAL(RL_Color){ 200, 200, 200, 255 }   // Light Gray
#define RL_GRAY       RL_CLITERAL(RL_Color){ 130, 130, 130, 255 }   // Gray
#define RL_DARKGRAY   RL_CLITERAL(RL_Color){ 80, 80, 80, 255 }      // Dark Gray
#define RL_YELLOW     RL_CLITERAL(RL_Color){ 253, 249, 0, 255 }     // Yellow
#define RL_GOLD       RL_CLITERAL(RL_Color){ 255, 203, 0, 255 }     // Gold
#define RL_ORANGE     RL_CLITERAL(RL_Color){ 255, 161, 0, 255 }     // Orange
#define RL_PINK       RL_CLITERAL(RL_Color){ 255, 109, 194, 255 }   // Pink
#define RL_RED        RL_CLITERAL(RL_Color){ 230, 41, 55, 255 }     // Red
#define RL_MAROON     RL_CLITERAL(RL_Color){ 190, 33, 55, 255 }     // Maroon
#define RL_GREEN      RL_CLITERAL(RL_Color){ 0, 228, 48, 255 }      // Green
#define RL_LIME       RL_CLITERAL(RL_Color){ 0, 158, 47, 255 }      // Lime
#define RL_DARKGREEN  RL_CLITERAL(RL_Color){ 0, 117, 44, 255 }      // Dark Green
#define RL_SKYBLUE    RL_CLITERAL(RL_Color){ 102, 191, 255, 255 }   // Sky Blue
#define RL_BLUE       RL_CLITERAL(RL_Color){ 0, 121, 241, 255 }     // Blue
#define RL_DARKBLUE   RL_CLITERAL(RL_Color){ 0, 82, 172, 255 }      // Dark Blue
#define RL_PURPLE     RL_CLITERAL(RL_Color){ 200, 122, 255, 255 }   // Purple
#define RL_VIOLET     RL_CLITERAL(RL_Color){ 135, 60, 190, 255 }    // Violet
#define RL_DARKPURPLE RL_CLITERAL(RL_Color){ 112, 31, 126, 255 }    // Dark Purple
#define RL_BEIGE      RL_CLITERAL(RL_Color){ 211, 176, 131, 255 }   // Beige
#define RL_BROWN      RL_CLITERAL(RL_Color){ 127, 106, 79, 255 }    // Brown
#define RL_DARKBROWN  RL_CLITERAL(RL_Color){ 76, 63, 47, 255 }      // Dark Brown

#define RL_WHITE      RL_CLITERAL(RL_Color){ 255, 255, 255, 255 }   // White
#define RL_BLACK      RL_CLITERAL(RL_Color){ 0, 0, 0, 255 }         // Black
#define RL_BLANK      RL_CLITERAL(RL_Color){ 0, 0, 0, 0 }           // Blank (Transparent)
#define RL_MAGENTA    RL_CLITERAL(RL_Color){ 255, 0, 255, 255 }     // Magenta
#define RL_RAYWHITE   RL_CLITERAL(RL_Color){ 245, 245, 245, 255 }   // My own White (raylib logo)

//----------------------------------------------------------------------------------
// Structures Definition
//----------------------------------------------------------------------------------
// Boolean type
#if (defined(__STDC__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
    #include <stdbool.h>
#elif !defined(__cplusplus) && !defined(bool)
    typedef enum bool { false = 0, true = !false } bool;
    #define RL_BOOL_TYPE
#endif

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

// RL_Vector4, 4 components
typedef struct RL_Vector4 {
    float x;                // Vector x component
    float y;                // Vector y component
    float z;                // Vector z component
    float w;                // Vector w component
} RL_Vector4;

// RL_Quaternion, 4 components (RL_Vector4 alias)
typedef RL_Vector4 RL_Quaternion;

// RL_Matrix, 4x4 components, column major, OpenGL style, right-handed
typedef struct RL_Matrix {
    float m0, m4, m8, m12;  // RL_Matrix first row (4 components)
    float m1, m5, m9, m13;  // RL_Matrix second row (4 components)
    float m2, m6, m10, m14; // RL_Matrix third row (4 components)
    float m3, m7, m11, m15; // RL_Matrix fourth row (4 components)
} RL_Matrix;

// RL_Color, 4 components, R8G8B8A8 (32bit)
typedef struct RL_Color {
    unsigned char r;        // RL_Color red value
    unsigned char g;        // RL_Color green value
    unsigned char b;        // RL_Color blue value
    unsigned char a;        // RL_Color alpha value
} RL_Color;

// RL_Rectangle, 4 components
typedef struct RL_Rectangle {
    float x;                // RL_Rectangle top-left corner position x
    float y;                // RL_Rectangle top-left corner position y
    float width;            // RL_Rectangle width
    float height;           // RL_Rectangle height
} RL_Rectangle;

// RL_Image, pixel data stored in CPU memory (RAM)
typedef struct RL_Image {
    void *data;             // RL_Image raw data
    int width;              // RL_Image base width
    int height;             // RL_Image base height
    int mipmaps;            // Mipmap levels, 1 by default
    int format;             // Data format (RL_PixelFormat type)
} RL_Image;

// RL_Texture, tex data stored in GPU memory (VRAM)
typedef struct RL_Texture {
    unsigned int id;        // OpenGL texture id
    int width;              // RL_Texture base width
    int height;             // RL_Texture base height
    int mipmaps;            // Mipmap levels, 1 by default
    int format;             // Data format (RL_PixelFormat type)
} RL_Texture;

// RL_Texture2D, same as RL_Texture
typedef RL_Texture RL_Texture2D;

// RL_TextureCubemap, same as RL_Texture
typedef RL_Texture RL_TextureCubemap;

// RL_RenderTexture, fbo for texture rendering
typedef struct RL_RenderTexture {
    unsigned int id;        // OpenGL framebuffer object id
    RL_Texture texture;        // RL_Color buffer attachment texture
    RL_Texture depth;          // Depth buffer attachment texture
} RL_RenderTexture;

// RL_RenderTexture2D, same as RL_RenderTexture
typedef RL_RenderTexture RL_RenderTexture2D;

// RL_NPatchInfo, n-patch layout info
typedef struct RL_NPatchInfo {
    RL_Rectangle source;       // RL_Texture source rectangle
    int left;               // Left border offset
    int top;                // Top border offset
    int right;              // Right border offset
    int bottom;             // Bottom border offset
    int layout;             // Layout of the n-patch: 3x3, 1x3 or 3x1
} RL_NPatchInfo;

// RL_GlyphInfo, font characters glyphs info
typedef struct RL_GlyphInfo {
    int value;              // Character value (Unicode)
    int offsetX;            // Character offset X when drawing
    int offsetY;            // Character offset Y when drawing
    int advanceX;           // Character advance position X
    RL_Image image;            // Character image data
} RL_GlyphInfo;

// RL_Font, font texture and RL_GlyphInfo array data
typedef struct RL_Font {
    int baseSize;           // Base size (default chars height)
    int glyphCount;         // Number of glyph characters
    int glyphPadding;       // Padding around the glyph characters
    RL_Texture2D texture;      // RL_Texture atlas containing the glyphs
    RL_Rectangle *recs;        // Rectangles in texture for the glyphs
    RL_GlyphInfo *glyphs;      // Glyphs info data
} RL_Font;

// RL_Camera, defines position/orientation in 3d space
typedef struct RL_Camera3D {
    RL_Vector3 position;       // RL_Camera position
    RL_Vector3 target;         // RL_Camera target it looks-at
    RL_Vector3 up;             // RL_Camera up vector (rotation over its axis)
    float fovy;             // RL_Camera field-of-view aperture in Y (degrees) in perspective, used as near plane width in orthographic
    int projection;         // RL_Camera projection: RL_CAMERA_PERSPECTIVE or RL_CAMERA_ORTHOGRAPHIC
} RL_Camera3D;

typedef RL_Camera3D RL_Camera;    // RL_Camera type fallback, defaults to RL_Camera3D

// RL_Camera2D, defines position/orientation in 2d space
typedef struct RL_Camera2D {
    RL_Vector2 offset;         // RL_Camera offset (displacement from target)
    RL_Vector2 target;         // RL_Camera target (rotation and zoom origin)
    float rotation;         // RL_Camera rotation in degrees
    float zoom;             // RL_Camera zoom (scaling), should be 1.0f by default
} RL_Camera2D;

// RL_Mesh, vertex data and vao/vbo
typedef struct RL_Mesh {
    int vertexCount;        // Number of vertices stored in arrays
    int triangleCount;      // Number of triangles stored (indexed or not)

    // Vertex attributes data
    float *vertices;        // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float *texcoords;       // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    float *texcoords2;      // Vertex texture second coordinates (UV - 2 components per vertex) (shader-location = 5)
    float *normals;         // Vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
    float *tangents;        // Vertex tangents (XYZW - 4 components per vertex) (shader-location = 4)
    unsigned char *colors;      // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
    unsigned short *indices;    // Vertex indices (in case vertex data comes indexed)

    // Animation vertex data
    float *animVertices;    // Animated vertex positions (after bones transformations)
    float *animNormals;     // Animated normals (after bones transformations)
    unsigned char *boneIds; // Vertex bone ids, max 255 bone ids, up to 4 bones influence by vertex (skinning)
    float *boneWeights;     // Vertex bone weight, up to 4 bones influence by vertex (skinning)

    // OpenGL identifiers
    unsigned int vaoId;     // OpenGL Vertex Array Object id
    unsigned int *vboId;    // OpenGL Vertex Buffer Objects id (default vertex data)
} RL_Mesh;

// RL_Shader
typedef struct RL_Shader {
    unsigned int id;        // RL_Shader program id
    int *locs;              // RL_Shader locations array (RL_MAX_SHADER_LOCATIONS)
} RL_Shader;

// RL_MaterialMap
typedef struct RL_MaterialMap {
    RL_Texture2D texture;      // RL_Material map texture
    RL_Color color;            // RL_Material map color
    float value;            // RL_Material map value
} RL_MaterialMap;

// RL_Material, includes shader and maps
typedef struct RL_Material {
    RL_Shader shader;          // RL_Material shader
    RL_MaterialMap *maps;      // RL_Material maps array (RL_MAX_MATERIAL_MAPS)
    float params[4];        // RL_Material generic parameters (if required)
} RL_Material;

// RL_Transform, vertex transformation data
typedef struct RL_Transform {
    RL_Vector3 translation;    // Translation
    RL_Quaternion rotation;    // Rotation
    RL_Vector3 scale;          // Scale
} RL_Transform;

// Bone, skeletal animation bone
typedef struct RL_BoneInfo {
    char name[32];          // Bone name
    int parent;             // Bone parent
} RL_BoneInfo;

// RL_Model, meshes, materials and animation data
typedef struct RL_Model {
    RL_Matrix transform;       // Local transform matrix

    int meshCount;          // Number of meshes
    int materialCount;      // Number of materials
    RL_Mesh *meshes;           // Meshes array
    RL_Material *materials;    // Materials array
    int *meshMaterial;      // RL_Mesh material number

    // Animation data
    int boneCount;          // Number of bones
    RL_BoneInfo *bones;        // Bones information (skeleton)
    RL_Transform *bindPose;    // Bones base transformation (pose)
} RL_Model;

// RL_ModelAnimation
typedef struct RL_ModelAnimation {
    int boneCount;          // Number of bones
    int frameCount;         // Number of animation frames
    RL_BoneInfo *bones;        // Bones information (skeleton)
    RL_Transform **framePoses; // Poses array by frame
    char name[32];          // Animation name
} RL_ModelAnimation;

// RL_Ray, ray for raycasting
typedef struct RL_Ray {
    RL_Vector3 position;       // RL_Ray position (origin)
    RL_Vector3 direction;      // RL_Ray direction
} RL_Ray;

// RL_RayCollision, ray hit information
typedef struct RL_RayCollision {
    bool hit;               // Did the ray hit something?
    float distance;         // Distance to the nearest hit
    RL_Vector3 point;          // RL_Point of the nearest hit
    RL_Vector3 normal;         // Surface normal of hit
} RL_RayCollision;

// RL_BoundingBox
typedef struct RL_BoundingBox {
    RL_Vector3 min;            // Minimum vertex box-corner
    RL_Vector3 max;            // Maximum vertex box-corner
} RL_BoundingBox;

// RL_Wave, audio wave data
typedef struct RL_Wave {
    unsigned int frameCount;    // Total number of frames (considering channels)
    unsigned int sampleRate;    // Frequency (samples per second)
    unsigned int sampleSize;    // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    unsigned int channels;      // Number of channels (1-mono, 2-stereo, ...)
    void *data;                 // Buffer data pointer
} RL_Wave;

// Opaque structs declaration
// NOTE: Actual structs are defined internally in raudio module
typedef struct RL_AudioBuffer RL_AudioBuffer;
typedef struct RL_AudioProcessor RL_AudioProcessor;

// RL_AudioStream, custom audio stream
typedef struct RL_AudioStream {
    RL_AudioBuffer *buffer;       // Pointer to internal data used by the audio system
    RL_AudioProcessor *processor; // Pointer to internal data processor, useful for audio effects

    unsigned int sampleRate;    // Frequency (samples per second)
    unsigned int sampleSize;    // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    unsigned int channels;      // Number of channels (1-mono, 2-stereo, ...)
} RL_AudioStream;

// RL_Sound
typedef struct RL_Sound {
    RL_AudioStream stream;         // Audio stream
    unsigned int frameCount;    // Total number of frames (considering channels)
} RL_Sound;

// RL_Music, audio stream, anything longer than ~10 seconds should be streamed
typedef struct RL_Music {
    RL_AudioStream stream;         // Audio stream
    unsigned int frameCount;    // Total number of frames (considering channels)
    bool looping;               // RL_Music looping enable

    int ctxType;                // Type of music context (audio filetype)
    void *ctxData;              // Audio context data, depends on type
} RL_Music;

// RL_VrDeviceInfo, Head-Mounted-Display device parameters
typedef struct RL_VrDeviceInfo {
    int hResolution;                // Horizontal resolution in pixels
    int vResolution;                // Vertical resolution in pixels
    float hScreenSize;              // Horizontal size in meters
    float vScreenSize;              // Vertical size in meters
    float vScreenCenter;            // Screen center in meters
    float eyeToScreenDistance;      // Distance between eye and display in meters
    float lensSeparationDistance;   // Lens separation distance in meters
    float interpupillaryDistance;   // IPD (distance between pupils) in meters
    float lensDistortionValues[4];  // Lens distortion constant parameters
    float chromaAbCorrection[4];    // Chromatic aberration correction parameters
} RL_VrDeviceInfo;

// RL_VrStereoConfig, VR stereo rendering configuration for simulator
typedef struct RL_VrStereoConfig {
    RL_Matrix projection[2];           // VR projection matrices (per eye)
    RL_Matrix viewOffset[2];           // VR view offset matrices (per eye)
    float leftLensCenter[2];        // VR left lens center
    float rightLensCenter[2];       // VR right lens center
    float leftScreenCenter[2];      // VR left screen center
    float rightScreenCenter[2];     // VR right screen center
    float scale[2];                 // VR distortion scale
    float scaleIn[2];               // VR distortion scale in
} RL_VrStereoConfig;

// File path list
typedef struct RL_FilePathList {
    unsigned int capacity;          // Filepaths max entries
    unsigned int count;             // Filepaths entries count
    char **paths;                   // Filepaths entries
} RL_FilePathList;

// Automation event
typedef struct RL_AutomationEvent {
    unsigned int frame;             // Event frame
    unsigned int type;              // Event type (AutomationEventType)
    int params[4];                  // Event parameters (if required)
} RL_AutomationEvent;

// Automation event list
typedef struct RL_AutomationEventList {
    unsigned int capacity;          // Events max entries (RL_MAX_AUTOMATION_EVENTS)
    unsigned int count;             // Events entries count
    RL_AutomationEvent *events;        // Events entries
} RL_AutomationEventList;

//----------------------------------------------------------------------------------
// Enumerators Definition
//----------------------------------------------------------------------------------
// System/Window config flags
// NOTE: Every bit registers one state (use it with bit masks)
// By default all flags are set to 0
typedef enum {
    RL_FLAG_VSYNC_HINT         = 0x00000040,   // Set to try enabling V-Sync on GPU
    RL_FLAG_FULLSCREEN_MODE    = 0x00000002,   // Set to run program in fullscreen
    RL_FLAG_WINDOW_RESIZABLE   = 0x00000004,   // Set to allow resizable window
    RL_FLAG_WINDOW_UNDECORATED = 0x00000008,   // Set to disable window decoration (frame and buttons)
    RL_FLAG_WINDOW_HIDDEN      = 0x00000080,   // Set to hide window
    RL_FLAG_WINDOW_MINIMIZED   = 0x00000200,   // Set to minimize window (iconify)
    RL_FLAG_WINDOW_MAXIMIZED   = 0x00000400,   // Set to maximize window (expanded to monitor)
    RL_FLAG_WINDOW_UNFOCUSED   = 0x00000800,   // Set to window non focused
    RL_FLAG_WINDOW_TOPMOST     = 0x00001000,   // Set to window always on top
    RL_FLAG_WINDOW_ALWAYS_RUN  = 0x00000100,   // Set to allow windows running while minimized
    RL_FLAG_WINDOW_TRANSPARENT = 0x00000010,   // Set to allow transparent framebuffer
    RL_FLAG_WINDOW_HIGHDPI     = 0x00002000,   // Set to support HighDPI
    RL_FLAG_WINDOW_MOUSE_PASSTHROUGH = 0x00004000, // Set to support mouse passthrough, only supported when RL_FLAG_WINDOW_UNDECORATED
    RL_FLAG_BORDERLESS_WINDOWED_MODE = 0x00008000, // Set to run program in borderless windowed mode
    RL_FLAG_MSAA_4X_HINT       = 0x00000020,   // Set to try enabling MSAA 4X
    RL_FLAG_INTERLACED_HINT    = 0x00010000    // Set to try enabling interlaced video format (for V3D)
} RL_ConfigFlags;

// Trace log level
// NOTE: Organized by priority level
typedef enum {
    RL_LOG_ALL = 0,        // Display all logs
    RL_LOG_TRACE,          // Trace logging, intended for internal use only
    RL_LOG_DEBUG,          // Debug logging, used for internal debugging, it should be disabled on release builds
    RL_LOG_INFO,           // Info logging, used for program execution info
    RL_LOG_WARNING,        // Warning logging, used on recoverable failures
    RL_LOG_ERROR,          // Error logging, used on unrecoverable failures
    RL_LOG_FATAL,          // Fatal logging, used to abort program: exit(EXIT_FAILURE)
    RL_LOG_NONE            // Disable logging
} RL_TraceLogLevel;

// Keyboard keys (US keyboard layout)
// NOTE: Use RL_GetKeyPressed() to allow redefining
// required keys for alternative layouts
typedef enum {
    RL_KEY_NULL            = 0,        // Key: NULL, used for no key pressed
    // Alphanumeric keys
    RL_KEY_APOSTROPHE      = 39,       // Key: '
    RL_KEY_COMMA           = 44,       // Key: ,
    RL_KEY_MINUS           = 45,       // Key: -
    RL_KEY_PERIOD          = 46,       // Key: .
    RL_KEY_SLASH           = 47,       // Key: /
    RL_KEY_ZERO            = 48,       // Key: 0
    RL_KEY_ONE             = 49,       // Key: 1
    RL_KEY_TWO             = 50,       // Key: 2
    RL_KEY_THREE           = 51,       // Key: 3
    RL_KEY_FOUR            = 52,       // Key: 4
    RL_KEY_FIVE            = 53,       // Key: 5
    RL_KEY_SIX             = 54,       // Key: 6
    RL_KEY_SEVEN           = 55,       // Key: 7
    RL_KEY_EIGHT           = 56,       // Key: 8
    RL_KEY_NINE            = 57,       // Key: 9
    RL_KEY_SEMICOLON       = 59,       // Key: ;
    RL_KEY_EQUAL           = 61,       // Key: =
    RL_KEY_A               = 65,       // Key: A | a
    RL_KEY_B               = 66,       // Key: B | b
    RL_KEY_C               = 67,       // Key: C | c
    RL_KEY_D               = 68,       // Key: D | d
    RL_KEY_E               = 69,       // Key: E | e
    RL_KEY_F               = 70,       // Key: F | f
    RL_KEY_G               = 71,       // Key: G | g
    RL_KEY_H               = 72,       // Key: H | h
    RL_KEY_I               = 73,       // Key: I | i
    RL_KEY_J               = 74,       // Key: J | j
    RL_KEY_K               = 75,       // Key: K | k
    RL_KEY_L               = 76,       // Key: L | l
    RL_KEY_M               = 77,       // Key: M | m
    RL_KEY_N               = 78,       // Key: N | n
    RL_KEY_O               = 79,       // Key: O | o
    RL_KEY_P               = 80,       // Key: P | p
    RL_KEY_Q               = 81,       // Key: Q | q
    RL_KEY_R               = 82,       // Key: R | r
    RL_KEY_S               = 83,       // Key: S | s
    RL_KEY_T               = 84,       // Key: T | t
    RL_KEY_U               = 85,       // Key: U | u
    RL_KEY_V               = 86,       // Key: V | v
    RL_KEY_W               = 87,       // Key: W | w
    RL_KEY_X               = 88,       // Key: X | x
    RL_KEY_Y               = 89,       // Key: Y | y
    RL_KEY_Z               = 90,       // Key: Z | z
    RL_KEY_LEFT_BRACKET    = 91,       // Key: [
    KEY_BACKSLASH       = 92,       // Key: '\'
    KEY_RIGHT_BRACKET   = 93,       // Key: ]
    RL_KEY_GRAVE           = 96,       // Key: `
    // Function keys
    RL_KEY_SPACE           = 32,       // Key: Space
    RL_KEY_ESCAPE          = 256,      // Key: Esc
    RL_KEY_ENTER           = 257,      // Key: Enter
    RL_KEY_TAB             = 258,      // Key: Tab
    RL_KEY_BACKSPACE       = 259,      // Key: Backspace
    RL_KEY_INSERT          = 260,      // Key: Ins
    RL_KEY_DELETE          = 261,      // Key: Del
    RL_KEY_RIGHT           = 262,      // Key: Cursor right
    RL_KEY_LEFT            = 263,      // Key: Cursor left
    RL_KEY_DOWN            = 264,      // Key: Cursor down
    RL_KEY_UP              = 265,      // Key: Cursor up
    RL_KEY_PAGE_UP         = 266,      // Key: Page up
    RL_KEY_PAGE_DOWN       = 267,      // Key: Page down
    RL_KEY_HOME            = 268,      // Key: Home
    RL_KEY_END             = 269,      // Key: End
    RL_KEY_CAPS_LOCK       = 280,      // Key: Caps lock
    RL_KEY_SCROLL_LOCK     = 281,      // Key: Scroll down
    RL_KEY_NUM_LOCK        = 282,      // Key: Num lock
    RL_KEY_PRINT_SCREEN    = 283,      // Key: Print screen
    RL_KEY_PAUSE           = 284,      // Key: Pause
    RL_KEY_F1              = 290,      // Key: F1
    RL_KEY_F2              = 291,      // Key: F2
    RL_KEY_F3              = 292,      // Key: F3
    RL_KEY_F4              = 293,      // Key: F4
    RL_KEY_F5              = 294,      // Key: F5
    RL_KEY_F6              = 295,      // Key: F6
    RL_KEY_F7              = 296,      // Key: F7
    RL_KEY_F8              = 297,      // Key: F8
    RL_KEY_F9              = 298,      // Key: F9
    RL_KEY_F10             = 299,      // Key: F10
    RL_KEY_F11             = 300,      // Key: F11
    RL_KEY_F12             = 301,      // Key: F12
    RL_KEY_LEFT_SHIFT      = 340,      // Key: Shift left
    RL_KEY_LEFT_CONTROL    = 341,      // Key: Control left
    RL_KEY_LEFT_ALT        = 342,      // Key: Alt left
    RL_KEY_LEFT_SUPER      = 343,      // Key: Super left
    RL_KEY_RIGHT_SHIFT     = 344,      // Key: Shift right
    RL_KEY_RIGHT_CONTROL   = 345,      // Key: Control right
    RL_KEY_RIGHT_ALT       = 346,      // Key: Alt right
    RL_KEY_RIGHT_SUPER     = 347,      // Key: Super right
    RL_KEY_KB_MENU         = 348,      // Key: KB menu
    // Keypad keys
    RL_KEY_KP_0            = 320,      // Key: Keypad 0
    RL_KEY_KP_1            = 321,      // Key: Keypad 1
    RL_KEY_KP_2            = 322,      // Key: Keypad 2
    RL_KEY_KP_3            = 323,      // Key: Keypad 3
    RL_KEY_KP_4            = 324,      // Key: Keypad 4
    RL_KEY_KP_5            = 325,      // Key: Keypad 5
    RL_KEY_KP_6            = 326,      // Key: Keypad 6
    RL_KEY_KP_7            = 327,      // Key: Keypad 7
    RL_KEY_KP_8            = 328,      // Key: Keypad 8
    RL_KEY_KP_9            = 329,      // Key: Keypad 9
    RL_KEY_KP_DECIMAL      = 330,      // Key: Keypad .
    RL_KEY_KP_DIVIDE       = 331,      // Key: Keypad /
    RL_KEY_KP_MULTIPLY     = 332,      // Key: Keypad *
    RL_KEY_KP_SUBTRACT     = 333,      // Key: Keypad -
    RL_KEY_KP_ADD          = 334,      // Key: Keypad +
    RL_KEY_KP_ENTER        = 335,      // Key: Keypad Enter
    RL_KEY_KP_EQUAL        = 336,      // Key: Keypad =
    // Android key buttons
    RL_KEY_BACK            = 4,        // Key: Android back button
    RL_KEY_MENU            = 82,       // Key: Android menu button
    RL_KEY_VOLUME_UP       = 24,       // Key: Android volume up button
    RL_KEY_VOLUME_DOWN     = 25        // Key: Android volume down button
} RL_KeyboardKey;

// Add backwards compatibility support for deprecated names
#define RL_MOUSE_LEFT_BUTTON   RL_MOUSE_BUTTON_LEFT
#define RL_MOUSE_RIGHT_BUTTON  RL_MOUSE_BUTTON_RIGHT
#define RL_MOUSE_MIDDLE_BUTTON RL_MOUSE_BUTTON_MIDDLE

// Mouse buttons
typedef enum {
    RL_MOUSE_BUTTON_LEFT    = 0,       // Mouse button left
    RL_MOUSE_BUTTON_RIGHT   = 1,       // Mouse button right
    RL_MOUSE_BUTTON_MIDDLE  = 2,       // Mouse button middle (pressed wheel)
    RL_MOUSE_BUTTON_SIDE    = 3,       // Mouse button side (advanced mouse device)
    RL_MOUSE_BUTTON_EXTRA   = 4,       // Mouse button extra (advanced mouse device)
    RL_MOUSE_BUTTON_FORWARD = 5,       // Mouse button forward (advanced mouse device)
    RL_MOUSE_BUTTON_BACK    = 6,       // Mouse button back (advanced mouse device)
} RL_MouseButton;

// Mouse cursor
typedef enum {
    RL_MOUSE_CURSOR_DEFAULT       = 0,     // Default pointer shape
    RL_MOUSE_CURSOR_ARROW         = 1,     // Arrow shape
    RL_MOUSE_CURSOR_IBEAM         = 2,     // Text writing cursor shape
    RL_MOUSE_CURSOR_CROSSHAIR     = 3,     // Cross shape
    RL_MOUSE_CURSOR_POINTING_HAND = 4,     // Pointing hand cursor
    RL_MOUSE_CURSOR_RESIZE_EW     = 5,     // Horizontal resize/move arrow shape
    RL_MOUSE_CURSOR_RESIZE_NS     = 6,     // Vertical resize/move arrow shape
    RL_MOUSE_CURSOR_RESIZE_NWSE   = 7,     // Top-left to bottom-right diagonal resize/move arrow shape
    RL_MOUSE_CURSOR_RESIZE_NESW   = 8,     // The top-right to bottom-left diagonal resize/move arrow shape
    RL_MOUSE_CURSOR_RESIZE_ALL    = 9,     // The omnidirectional resize/move cursor shape
    RL_MOUSE_CURSOR_NOT_ALLOWED   = 10     // The operation-not-allowed shape
} RL_MouseCursor;

// Gamepad buttons
typedef enum {
    RL_GAMEPAD_BUTTON_UNKNOWN = 0,         // Unknown button, just for error checking
    RL_GAMEPAD_BUTTON_LEFT_FACE_UP,        // Gamepad left DPAD up button
    RL_GAMEPAD_BUTTON_LEFT_FACE_RIGHT,     // Gamepad left DPAD right button
    RL_GAMEPAD_BUTTON_LEFT_FACE_DOWN,      // Gamepad left DPAD down button
    RL_GAMEPAD_BUTTON_LEFT_FACE_LEFT,      // Gamepad left DPAD left button
    RL_GAMEPAD_BUTTON_RIGHT_FACE_UP,       // Gamepad right button up (i.e. PS3: Triangle, Xbox: Y)
    RL_GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,    // Gamepad right button right (i.e. PS3: Square, Xbox: X)
    RL_GAMEPAD_BUTTON_RIGHT_FACE_DOWN,     // Gamepad right button down (i.e. PS3: Cross, Xbox: A)
    RL_GAMEPAD_BUTTON_RIGHT_FACE_LEFT,     // Gamepad right button left (i.e. PS3: Circle, Xbox: B)
    RL_GAMEPAD_BUTTON_LEFT_TRIGGER_1,      // Gamepad top/back trigger left (first), it could be a trailing button
    RL_GAMEPAD_BUTTON_LEFT_TRIGGER_2,      // Gamepad top/back trigger left (second), it could be a trailing button
    RL_GAMEPAD_BUTTON_RIGHT_TRIGGER_1,     // Gamepad top/back trigger right (one), it could be a trailing button
    RL_GAMEPAD_BUTTON_RIGHT_TRIGGER_2,     // Gamepad top/back trigger right (second), it could be a trailing button
    RL_GAMEPAD_BUTTON_MIDDLE_LEFT,         // Gamepad center buttons, left one (i.e. PS3: Select)
    RL_GAMEPAD_BUTTON_MIDDLE,              // Gamepad center buttons, middle one (i.e. PS3: PS, Xbox: XBOX)
    RL_GAMEPAD_BUTTON_MIDDLE_RIGHT,        // Gamepad center buttons, right one (i.e. PS3: Start)
    RL_GAMEPAD_BUTTON_LEFT_THUMB,          // Gamepad joystick pressed button left
    RL_GAMEPAD_BUTTON_RIGHT_THUMB          // Gamepad joystick pressed button right
} RL_GamepadButton;

// Gamepad axis
typedef enum {
    RL_GAMEPAD_AXIS_LEFT_X        = 0,     // Gamepad left stick X axis
    RL_GAMEPAD_AXIS_LEFT_Y        = 1,     // Gamepad left stick Y axis
    RL_GAMEPAD_AXIS_RIGHT_X       = 2,     // Gamepad right stick X axis
    RL_GAMEPAD_AXIS_RIGHT_Y       = 3,     // Gamepad right stick Y axis
    RL_GAMEPAD_AXIS_LEFT_TRIGGER  = 4,     // Gamepad back trigger left, pressure level: [1..-1]
    RL_GAMEPAD_AXIS_RIGHT_TRIGGER = 5      // Gamepad back trigger right, pressure level: [1..-1]
} RL_GamepadAxis;

// RL_Material map index
typedef enum {
    RL_MATERIAL_MAP_ALBEDO = 0,        // Albedo material (same as: RL_MATERIAL_MAP_DIFFUSE)
    RL_MATERIAL_MAP_METALNESS,         // Metalness material (same as: RL_MATERIAL_MAP_SPECULAR)
    RL_MATERIAL_MAP_NORMAL,            // Normal material
    RL_MATERIAL_MAP_ROUGHNESS,         // Roughness material
    RL_MATERIAL_MAP_OCCLUSION,         // Ambient occlusion material
    RL_MATERIAL_MAP_EMISSION,          // Emission material
    RL_MATERIAL_MAP_HEIGHT,            // Heightmap material
    RL_MATERIAL_MAP_CUBEMAP,           // Cubemap material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
    RL_MATERIAL_MAP_IRRADIANCE,        // Irradiance material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
    RL_MATERIAL_MAP_PREFILTER,         // Prefilter material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
    RL_MATERIAL_MAP_BRDF               // Brdf material
} RL_MaterialMapIndex;

#define RL_MATERIAL_MAP_DIFFUSE      RL_MATERIAL_MAP_ALBEDO
#define RL_MATERIAL_MAP_SPECULAR     RL_MATERIAL_MAP_METALNESS

// RL_Shader location index
typedef enum {
    RL_SHADER_LOC_VERTEX_POSITION = 0, // RL_Shader location: vertex attribute: position
    RL_SHADER_LOC_VERTEX_TEXCOORD01,   // RL_Shader location: vertex attribute: texcoord01
    RL_SHADER_LOC_VERTEX_TEXCOORD02,   // RL_Shader location: vertex attribute: texcoord02
    RL_SHADER_LOC_VERTEX_NORMAL,       // RL_Shader location: vertex attribute: normal
    RL_SHADER_LOC_VERTEX_TANGENT,      // RL_Shader location: vertex attribute: tangent
    RL_SHADER_LOC_VERTEX_COLOR,        // RL_Shader location: vertex attribute: color
    RL_SHADER_LOC_MATRIX_MVP,          // RL_Shader location: matrix uniform: model-view-projection
    RL_SHADER_LOC_MATRIX_VIEW,         // RL_Shader location: matrix uniform: view (camera transform)
    RL_SHADER_LOC_MATRIX_PROJECTION,   // RL_Shader location: matrix uniform: projection
    RL_SHADER_LOC_MATRIX_MODEL,        // RL_Shader location: matrix uniform: model (transform)
    RL_SHADER_LOC_MATRIX_NORMAL,       // RL_Shader location: matrix uniform: normal
    RL_SHADER_LOC_VECTOR_VIEW,         // RL_Shader location: vector uniform: view
    RL_SHADER_LOC_COLOR_DIFFUSE,       // RL_Shader location: vector uniform: diffuse color
    RL_SHADER_LOC_COLOR_SPECULAR,      // RL_Shader location: vector uniform: specular color
    RL_SHADER_LOC_COLOR_AMBIENT,       // RL_Shader location: vector uniform: ambient color
    RL_SHADER_LOC_MAP_ALBEDO,          // RL_Shader location: sampler2d texture: albedo (same as: RL_SHADER_LOC_MAP_DIFFUSE)
    RL_SHADER_LOC_MAP_METALNESS,       // RL_Shader location: sampler2d texture: metalness (same as: RL_SHADER_LOC_MAP_SPECULAR)
    RL_SHADER_LOC_MAP_NORMAL,          // RL_Shader location: sampler2d texture: normal
    RL_SHADER_LOC_MAP_ROUGHNESS,       // RL_Shader location: sampler2d texture: roughness
    RL_SHADER_LOC_MAP_OCCLUSION,       // RL_Shader location: sampler2d texture: occlusion
    RL_SHADER_LOC_MAP_EMISSION,        // RL_Shader location: sampler2d texture: emission
    RL_SHADER_LOC_MAP_HEIGHT,          // RL_Shader location: sampler2d texture: height
    RL_SHADER_LOC_MAP_CUBEMAP,         // RL_Shader location: samplerCube texture: cubemap
    RL_SHADER_LOC_MAP_IRRADIANCE,      // RL_Shader location: samplerCube texture: irradiance
    RL_SHADER_LOC_MAP_PREFILTER,       // RL_Shader location: samplerCube texture: prefilter
    RL_SHADER_LOC_MAP_BRDF             // RL_Shader location: sampler2d texture: brdf
} RL_ShaderLocationIndex;

#define RL_SHADER_LOC_MAP_DIFFUSE      RL_SHADER_LOC_MAP_ALBEDO
#define RL_SHADER_LOC_MAP_SPECULAR     RL_SHADER_LOC_MAP_METALNESS

// RL_Shader uniform data type
typedef enum {
    RL_SHADER_UNIFORM_FLOAT = 0,       // RL_Shader uniform type: float
    RL_SHADER_UNIFORM_VEC2,            // RL_Shader uniform type: vec2 (2 float)
    RL_SHADER_UNIFORM_VEC3,            // RL_Shader uniform type: vec3 (3 float)
    RL_SHADER_UNIFORM_VEC4,            // RL_Shader uniform type: vec4 (4 float)
    RL_SHADER_UNIFORM_INT,             // RL_Shader uniform type: int
    RL_SHADER_UNIFORM_IVEC2,           // RL_Shader uniform type: ivec2 (2 int)
    RL_SHADER_UNIFORM_IVEC3,           // RL_Shader uniform type: ivec3 (3 int)
    RL_SHADER_UNIFORM_IVEC4,           // RL_Shader uniform type: ivec4 (4 int)
    RL_SHADER_UNIFORM_SAMPLER2D        // RL_Shader uniform type: sampler2d
} RL_ShaderUniformDataType;

// RL_Shader attribute data types
typedef enum {
    RL_SHADER_ATTRIB_FLOAT = 0,        // RL_Shader attribute type: float
    RL_SHADER_ATTRIB_VEC2,             // RL_Shader attribute type: vec2 (2 float)
    RL_SHADER_ATTRIB_VEC3,             // RL_Shader attribute type: vec3 (3 float)
    RL_SHADER_ATTRIB_VEC4              // RL_Shader attribute type: vec4 (4 float)
} RL_ShaderAttributeDataType;

// Pixel formats
// NOTE: Support depends on OpenGL version and platform
typedef enum {
    RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1, // 8 bit per pixel (no alpha)
    RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,    // 8*2 bpp (2 channels)
    RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5,        // 16 bpp
    RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8,        // 24 bpp
    RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1,      // 16 bpp (1 bit alpha)
    RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,      // 16 bpp (4 bit alpha)
    RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,      // 32 bpp
    RL_PIXELFORMAT_UNCOMPRESSED_R32,           // 32 bpp (1 channel - float)
    RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32,     // 32*3 bpp (3 channels - float)
    RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,  // 32*4 bpp (4 channels - float)
    RL_PIXELFORMAT_UNCOMPRESSED_R16,           // 16 bpp (1 channel - half float)
    RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16,     // 16*3 bpp (3 channels - half float)
    RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16,  // 16*4 bpp (4 channels - half float)
    RL_PIXELFORMAT_COMPRESSED_DXT1_RGB,        // 4 bpp (no alpha)
    RL_PIXELFORMAT_COMPRESSED_DXT1_RGBA,       // 4 bpp (1 bit alpha)
    RL_PIXELFORMAT_COMPRESSED_DXT3_RGBA,       // 8 bpp
    RL_PIXELFORMAT_COMPRESSED_DXT5_RGBA,       // 8 bpp
    RL_PIXELFORMAT_COMPRESSED_ETC1_RGB,        // 4 bpp
    RL_PIXELFORMAT_COMPRESSED_ETC2_RGB,        // 4 bpp
    RL_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA,   // 8 bpp
    RL_PIXELFORMAT_COMPRESSED_PVRT_RGB,        // 4 bpp
    RL_PIXELFORMAT_COMPRESSED_PVRT_RGBA,       // 4 bpp
    RL_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,   // 8 bpp
    RL_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA    // 2 bpp
} RL_PixelFormat;

// RL_Texture parameters: filter mode
// NOTE 1: Filtering considers mipmaps if available in the texture
// NOTE 2: Filter is accordingly set for minification and magnification
typedef enum {
    RL_TEXTURE_FILTER_POINT = 0,               // No filter, just pixel approximation
    RL_TEXTURE_FILTER_BILINEAR,                // Linear filtering
    RL_TEXTURE_FILTER_TRILINEAR,               // Trilinear filtering (linear with mipmaps)
    RL_TEXTURE_FILTER_ANISOTROPIC_4X,          // Anisotropic filtering 4x
    RL_TEXTURE_FILTER_ANISOTROPIC_8X,          // Anisotropic filtering 8x
    RL_TEXTURE_FILTER_ANISOTROPIC_16X,         // Anisotropic filtering 16x
} RL_TextureFilter;

// RL_Texture parameters: wrap mode
typedef enum {
    RL_TEXTURE_WRAP_REPEAT = 0,                // Repeats texture in tiled mode
    RL_TEXTURE_WRAP_CLAMP,                     // Clamps texture to edge pixel in tiled mode
    RL_TEXTURE_WRAP_MIRROR_REPEAT,             // Mirrors and repeats the texture in tiled mode
    RL_TEXTURE_WRAP_MIRROR_CLAMP               // Mirrors and clamps to border the texture in tiled mode
} RL_TextureWrap;

// Cubemap layouts
typedef enum {
    RL_CUBEMAP_LAYOUT_AUTO_DETECT = 0,         // Automatically detect layout type
    RL_CUBEMAP_LAYOUT_LINE_VERTICAL,           // Layout is defined by a vertical line with faces
    RL_CUBEMAP_LAYOUT_LINE_HORIZONTAL,         // Layout is defined by a horizontal line with faces
    RL_CUBEMAP_LAYOUT_CROSS_THREE_BY_FOUR,     // Layout is defined by a 3x4 cross with cubemap faces
    RL_CUBEMAP_LAYOUT_CROSS_FOUR_BY_THREE,     // Layout is defined by a 4x3 cross with cubemap faces
    RL_CUBEMAP_LAYOUT_PANORAMA                 // Layout is defined by a panorama image (equirrectangular map)
} RL_CubemapLayout;

// RL_Font type, defines generation method
typedef enum {
    RL_FONT_DEFAULT = 0,               // Default font generation, anti-aliased
    RL_FONT_BITMAP,                    // Bitmap font generation, no anti-aliasing
    RL_FONT_SDF                        // SDF font generation, requires external shader
} RL_FontType;

// RL_Color blending modes (pre-defined)
typedef enum {
    RL_BLEND_ALPHA = 0,                // Blend textures considering alpha (default)
    RL_BLEND_ADDITIVE,                 // Blend textures adding colors
    RL_BLEND_MULTIPLIED,               // Blend textures multiplying colors
    RL_BLEND_ADD_COLORS,               // Blend textures adding colors (alternative)
    RL_BLEND_SUBTRACT_COLORS,          // Blend textures subtracting colors (alternative)
    RL_BLEND_ALPHA_PREMULTIPLY,        // Blend premultiplied textures considering alpha
    RL_BLEND_CUSTOM,                   // Blend textures using custom src/dst factors (use RLGL_SetBlendFactors())
    RL_BLEND_CUSTOM_SEPARATE           // Blend textures using custom rgb/alpha separate src/dst factors (use RLGL_SetBlendFactorsSeparate())
} RL_BlendMode;

// RL_Gesture
// NOTE: Provided as bit-wise flags to enable only desired gestures
typedef enum {
    RL_GESTURE_NONE        = 0,        // No gesture
    RL_GESTURE_TAP         = 1,        // Tap gesture
    RL_GESTURE_DOUBLETAP   = 2,        // Double tap gesture
    RL_GESTURE_HOLD        = 4,        // Hold gesture
    RL_GESTURE_DRAG        = 8,        // Drag gesture
    RL_GESTURE_SWIPE_RIGHT = 16,       // Swipe right gesture
    RL_GESTURE_SWIPE_LEFT  = 32,       // Swipe left gesture
    RL_GESTURE_SWIPE_UP    = 64,       // Swipe up gesture
    RL_GESTURE_SWIPE_DOWN  = 128,      // Swipe down gesture
    RL_GESTURE_PINCH_IN    = 256,      // Pinch in gesture
    RL_GESTURE_PINCH_OUT   = 512       // Pinch out gesture
} RL_Gesture;

// RL_Camera system modes
typedef enum {
    RL_CAMERA_CUSTOM = 0,              // Custom camera
    RL_CAMERA_FREE,                    // Free camera
    RL_CAMERA_ORBITAL,                 // Orbital camera
    RL_CAMERA_FIRST_PERSON,            // First person camera
    RL_CAMERA_THIRD_PERSON             // Third person camera
} RL_CameraMode;

// RL_Camera projection
typedef enum {
    RL_CAMERA_PERSPECTIVE = 0,         // Perspective projection
    RL_CAMERA_ORTHOGRAPHIC             // Orthographic projection
} RL_CameraProjection;

// N-patch layout
typedef enum {
    RL_NPATCH_NINE_PATCH = 0,          // Npatch layout: 3x3 tiles
    RL_NPATCH_THREE_PATCH_VERTICAL,    // Npatch layout: 1x3 tiles
    RL_NPATCH_THREE_PATCH_HORIZONTAL   // Npatch layout: 3x1 tiles
} RL_NPatchLayout;

// Callbacks to hook some internal functions
// WARNING: These callbacks are intended for advance users
typedef void (*RL_TraceLogCallback)(int logLevel, const char *text, va_list args);  // Logging: Redirect trace log messages
typedef unsigned char *(*RL_LoadFileDataCallback)(const char *fileName, int *dataSize);    // FileIO: Load binary data
typedef bool (*RL_SaveFileDataCallback)(const char *fileName, void *data, int dataSize);   // FileIO: Save binary data
typedef char *(*RL_LoadFileTextCallback)(const char *fileName);            // FileIO: Load text data
typedef bool (*RL_SaveFileTextCallback)(const char *fileName, char *text); // FileIO: Save text data

//------------------------------------------------------------------------------------
// Global Variables Definition
//------------------------------------------------------------------------------------
// It's lonely here...

//------------------------------------------------------------------------------------
// Window and Graphics Device Functions (Module: core)
//------------------------------------------------------------------------------------

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

// Window-related functions
RLAPI void RL_InitWindow(int width, int height, const char *title);  // Initialize window and OpenGL context
RLAPI void RL_CloseWindow(void);                                     // Close window and unload OpenGL context
RLAPI bool RL_WindowShouldClose(void);                               // Check if application should close (RL_KEY_ESCAPE pressed or windows close icon clicked)
RLAPI bool RL_IsWindowReady(void);                                   // Check if window has been initialized successfully
RLAPI bool RL_IsWindowFullscreen(void);                              // Check if window is currently fullscreen
RLAPI bool RL_IsWindowHidden(void);                                  // Check if window is currently hidden (only PLATFORM_DESKTOP)
RLAPI bool RL_IsWindowMinimized(void);                               // Check if window is currently minimized (only PLATFORM_DESKTOP)
RLAPI bool RL_IsWindowMaximized(void);                               // Check if window is currently maximized (only PLATFORM_DESKTOP)
RLAPI bool RL_IsWindowFocused(void);                                 // Check if window is currently focused (only PLATFORM_DESKTOP)
RLAPI bool RL_IsWindowResized(void);                                 // Check if window has been resized last frame
RLAPI bool RL_IsWindowState(unsigned int flag);                      // Check if one specific window flag is enabled
RLAPI void RL_SetWindowState(unsigned int flags);                    // Set window configuration state using flags (only PLATFORM_DESKTOP)
RLAPI void RL_ClearWindowState(unsigned int flags);                  // Clear window configuration state flags
RLAPI void RL_ToggleFullscreen(void);                                // Toggle window state: fullscreen/windowed (only PLATFORM_DESKTOP)
RLAPI void RL_ToggleBorderlessWindowed(void);                        // Toggle window state: borderless windowed (only PLATFORM_DESKTOP)
RLAPI void RL_MaximizeWindow(void);                                  // Set window state: maximized, if resizable (only PLATFORM_DESKTOP)
RLAPI void RL_MinimizeWindow(void);                                  // Set window state: minimized, if resizable (only PLATFORM_DESKTOP)
RLAPI void RL_RestoreWindow(void);                                   // Set window state: not minimized/maximized (only PLATFORM_DESKTOP)
RLAPI void RL_SetWindowIcon(RL_Image image);                            // Set icon for window (single image, RGBA 32bit, only PLATFORM_DESKTOP)
RLAPI void RL_SetWindowIcons(RL_Image *images, int count);              // Set icon for window (multiple images, RGBA 32bit, only PLATFORM_DESKTOP)
RLAPI void RL_SetWindowTitle(const char *title);                     // Set title for window (only PLATFORM_DESKTOP and PLATFORM_WEB)
RLAPI void RL_SetWindowPosition(int x, int y);                       // Set window position on screen (only PLATFORM_DESKTOP)
RLAPI void RL_SetWindowMonitor(int monitor);                         // Set monitor for the current window
RLAPI void RL_SetWindowMinSize(int width, int height);               // Set window minimum dimensions (for RL_FLAG_WINDOW_RESIZABLE)
RLAPI void RL_SetWindowMaxSize(int width, int height);               // Set window maximum dimensions (for RL_FLAG_WINDOW_RESIZABLE)
RLAPI void RL_SetWindowSize(int width, int height);                  // Set window dimensions
RLAPI void RL_SetWindowOpacity(float opacity);                       // Set window opacity [0.0f..1.0f] (only PLATFORM_DESKTOP)
RLAPI void RL_SetWindowFocused(void);                                // Set window focused (only PLATFORM_DESKTOP)
RLAPI void *RL_GetWindowHandle(void);                                // Get native window handle
RLAPI int RL_GetScreenWidth(void);                                   // Get current screen width
RLAPI int RL_GetScreenHeight(void);                                  // Get current screen height
RLAPI int RL_GetRenderWidth(void);                                   // Get current render width (it considers HiDPI)
RLAPI int RL_GetRenderHeight(void);                                  // Get current render height (it considers HiDPI)
RLAPI int RL_GetMonitorCount(void);                                  // Get number of connected monitors
RLAPI int RL_GetCurrentMonitor(void);                                // Get current connected monitor
RLAPI RL_Vector2 RL_GetMonitorPosition(int monitor);                    // Get specified monitor position
RLAPI int RL_GetMonitorWidth(int monitor);                           // Get specified monitor width (current video mode used by monitor)
RLAPI int RL_GetMonitorHeight(int monitor);                          // Get specified monitor height (current video mode used by monitor)
RLAPI int RL_GetMonitorPhysicalWidth(int monitor);                   // Get specified monitor physical width in millimetres
RLAPI int RL_GetMonitorPhysicalHeight(int monitor);                  // Get specified monitor physical height in millimetres
RLAPI int RL_GetMonitorRefreshRate(int monitor);                     // Get specified monitor refresh rate
RLAPI RL_Vector2 RL_GetWindowPosition(void);                            // Get window position XY on monitor
RLAPI RL_Vector2 RL_GetWindowScaleDPI(void);                            // Get window scale DPI factor
RLAPI const char *RL_GetMonitorName(int monitor);                    // Get the human-readable, UTF-8 encoded name of the specified monitor
RLAPI void RL_SetClipboardText(const char *text);                    // Set clipboard text content
RLAPI const char *RL_GetClipboardText(void);                         // Get clipboard text content
RLAPI void RL_EnableEventWaiting(void);                              // Enable waiting for events on RL_EndDrawing(), no automatic event polling
RLAPI void RL_DisableEventWaiting(void);                             // Disable waiting for events on RL_EndDrawing(), automatic events polling

// Cursor-related functions
RLAPI void RL_ShowCursor(void);                                      // Shows cursor
RLAPI void RL_HideCursor(void);                                      // Hides cursor
RLAPI bool RL_IsCursorHidden(void);                                  // Check if cursor is not visible
RLAPI void RL_EnableCursor(void);                                    // Enables cursor (unlock cursor)
RLAPI void RL_DisableCursor(void);                                   // Disables cursor (lock cursor)
RLAPI bool RL_IsCursorOnScreen(void);                                // Check if cursor is on the screen

// Drawing-related functions
RLAPI void RL_ClearBackground(RL_Color color);                          // Set background color (framebuffer clear color)
RLAPI void RL_BeginDrawing(void);                                    // Setup canvas (framebuffer) to start drawing
RLAPI void RL_EndDrawing(void);                                      // End canvas drawing and swap buffers (double buffering)
RLAPI void RL_BeginMode2D(RL_Camera2D camera);                          // Begin 2D mode with custom camera (2D)
RLAPI void RL_EndMode2D(void);                                       // Ends 2D mode with custom camera
RLAPI void RL_BeginMode3D(RL_Camera3D camera);                          // Begin 3D mode with custom camera (3D)
RLAPI void RL_EndMode3D(void);                                       // Ends 3D mode and returns to default 2D orthographic mode
RLAPI void RL_BeginTextureMode(RL_RenderTexture2D target);              // Begin drawing to render texture
RLAPI void RL_EndTextureMode(void);                                  // Ends drawing to render texture
RLAPI void RL_BeginShaderMode(RL_Shader shader);                        // Begin custom shader drawing
RLAPI void RL_EndShaderMode(void);                                   // End custom shader drawing (use default shader)
RLAPI void RL_BeginBlendMode(int mode);                              // Begin blending mode (alpha, additive, multiplied, subtract, custom)
RLAPI void RL_EndBlendMode(void);                                    // End blending mode (reset to default: alpha blending)
RLAPI void RL_BeginScissorMode(int x, int y, int width, int height); // Begin scissor mode (define screen area for following drawing)
RLAPI void RL_EndScissorMode(void);                                  // End scissor mode
RLAPI void RL_BeginVrStereoMode(RL_VrStereoConfig config);              // Begin stereo rendering (requires VR simulator)
RLAPI void RL_EndVrStereoMode(void);                                 // End stereo rendering (requires VR simulator)

// VR stereo config functions for VR simulator
RLAPI RL_VrStereoConfig RL_LoadVrStereoConfig(RL_VrDeviceInfo device);     // Load VR stereo config for VR simulator device parameters
RLAPI void RL_UnloadVrStereoConfig(RL_VrStereoConfig config);           // Unload VR stereo config

// RL_Shader management functions
// NOTE: RL_Shader functionality is not available on OpenGL 1.1
RLAPI RL_Shader RL_LoadShader(const char *vsFileName, const char *fsFileName);   // Load shader from files and bind default locations
RLAPI RL_Shader RL_LoadShaderFromMemory(const char *vsCode, const char *fsCode); // Load shader from code strings and bind default locations
RLAPI bool RL_IsShaderReady(RL_Shader shader);                                   // Check if a shader is ready
RLAPI int RL_GetShaderLocation(RL_Shader shader, const char *uniformName);       // Get shader uniform location
RLAPI int RL_GetShaderLocationAttrib(RL_Shader shader, const char *attribName);  // Get shader attribute location
RLAPI void RL_SetShaderValue(RL_Shader shader, int locIndex, const void *value, int uniformType);               // Set shader uniform value
RLAPI void RL_SetShaderValueV(RL_Shader shader, int locIndex, const void *value, int uniformType, int count);   // Set shader uniform value vector
RLAPI void RL_SetShaderValueMatrix(RL_Shader shader, int locIndex, RL_Matrix mat);         // Set shader uniform value (matrix 4x4)
RLAPI void RL_SetShaderValueTexture(RL_Shader shader, int locIndex, RL_Texture2D texture); // Set shader uniform value for texture (sampler2d)
RLAPI void RL_UnloadShader(RL_Shader shader);                                    // Unload shader from GPU memory (VRAM)

// Screen-space-related functions
RLAPI RL_Ray RL_GetMouseRay(RL_Vector2 mousePosition, RL_Camera camera);      // Get a ray trace from mouse position
RLAPI RL_Matrix RL_GetCameraMatrix(RL_Camera camera);                      // Get camera transform matrix (view matrix)
RLAPI RL_Matrix RL_GetCameraMatrix2D(RL_Camera2D camera);                  // Get camera 2d transform matrix
RLAPI RL_Vector2 RL_GetWorldToScreen(RL_Vector3 position, RL_Camera camera);  // Get the screen space position for a 3d world space position
RLAPI RL_Vector2 RL_GetScreenToWorld2D(RL_Vector2 position, RL_Camera2D camera); // Get the world space position for a 2d camera screen space position
RLAPI RL_Vector2 RL_GetWorldToScreenEx(RL_Vector3 position, RL_Camera camera, int width, int height); // Get size position for a 3d world space position
RLAPI RL_Vector2 RL_GetWorldToScreen2D(RL_Vector2 position, RL_Camera2D camera); // Get the screen space position for a 2d camera world space position

// Timing-related functions
RLAPI void RL_SetTargetFPS(int fps);                                 // Set target FPS (maximum)
RLAPI float RL_GetFrameTime(void);                                   // Get time in seconds for last frame drawn (delta time)
RLAPI double RL_GetTime(void);                                       // Get elapsed time in seconds since RL_InitWindow()
RLAPI int RL_GetFPS(void);                                           // Get current FPS

// Custom frame control functions
// NOTE: Those functions are intended for advance users that want full control over the frame processing
// By default RL_EndDrawing() does this job: draws everything + RL_SwapScreenBuffer() + manage frame timing + RL_PollInputEvents()
// To avoid that behaviour and control frame processes manually, enable in config.h: SUPPORT_CUSTOM_FRAME_CONTROL
RLAPI void RL_SwapScreenBuffer(void);                                // Swap back buffer with front buffer (screen drawing)
RLAPI void RL_PollInputEvents(void);                                 // Register all input events
RLAPI void RL_WaitTime(double seconds);                              // Wait for some time (halt program execution)

// Random values generation functions
RLAPI void RL_SetRandomSeed(unsigned int seed);                      // Set the seed for the random number generator
RLAPI int RL_GetRandomValue(int min, int max);                       // Get a random value between min and max (both included)
RLAPI int *LoadRandomSequence(unsigned int count, int min, int max); // Load random values sequence, no values repeated
RLAPI void UnloadRandomSequence(int *sequence);                   // Unload random values sequence

// Misc. functions
RLAPI void RL_TakeScreenshot(const char *fileName);                  // Takes a screenshot of current screen (filename extension defines format)
RLAPI void RL_SetConfigFlags(unsigned int flags);                    // Setup init configuration flags (view FLAGS)
RLAPI void RL_OpenURL(const char *url);                              // Open URL with default system browser (if available)

// NOTE: Following functions implemented in module [utils]
//------------------------------------------------------------------
RLAPI void RL_TraceLog(int logLevel, const char *text, ...);         // Show trace log messages (RL_LOG_DEBUG, RL_LOG_INFO, RL_LOG_WARNING, RL_LOG_ERROR...)
RLAPI void RL_SetTraceLogLevel(int logLevel);                        // Set the current threshold (minimum) log level
RLAPI void *RL_MemAlloc(unsigned int size);                          // Internal memory allocator
RLAPI void *RL_MemRealloc(void *ptr, unsigned int size);             // Internal memory reallocator
RLAPI void RL_MemFree(void *ptr);                                    // Internal memory free

// Set custom callbacks
// WARNING: Callbacks setup is intended for advance users
RLAPI void RL_SetTraceLogCallback(RL_TraceLogCallback callback);         // Set custom trace log
RLAPI void RL_SetLoadFileDataCallback(RL_LoadFileDataCallback callback); // Set custom file binary data loader
RLAPI void RL_SetSaveFileDataCallback(RL_SaveFileDataCallback callback); // Set custom file binary data saver
RLAPI void RL_SetLoadFileTextCallback(RL_LoadFileTextCallback callback); // Set custom file text data loader
RLAPI void RL_SetSaveFileTextCallback(RL_SaveFileTextCallback callback); // Set custom file text data saver

// Files management functions
RLAPI unsigned char *RL_LoadFileData(const char *fileName, int *dataSize); // Load file data as byte array (read)
RLAPI void RL_UnloadFileData(unsigned char *data);                   // Unload file data allocated by RL_LoadFileData()
RLAPI bool RL_SaveFileData(const char *fileName, void *data, int dataSize); // Save data to file from byte array (write), returns true on success
RLAPI bool RL_ExportDataAsCode(const unsigned char *data, int dataSize, const char *fileName); // Export data to code (.h), returns true on success
RLAPI char *RL_LoadFileText(const char *fileName);                   // Load text data from file (read), returns a '\0' terminated string
RLAPI void RL_UnloadFileText(char *text);                            // Unload file text data allocated by RL_LoadFileText()
RLAPI bool RL_SaveFileText(const char *fileName, char *text);        // Save text data to file (write), string must be '\0' terminated, returns true on success
//------------------------------------------------------------------

// File system functions
RLAPI bool RL_FileExists(const char *fileName);                      // Check if file exists
RLAPI bool RL_DirectoryExists(const char *dirPath);                  // Check if a directory path exists
RLAPI bool RL_IsFileExtension(const char *fileName, const char *ext); // Check file extension (including point: .png, .wav)
RLAPI int RL_GetFileLength(const char *fileName);                    // Get file length in bytes (NOTE: GetFileSize() conflicts with windows.h)
RLAPI const char *RL_GetFileExtension(const char *fileName);         // Get pointer to extension for a filename string (includes dot: '.png')
RLAPI const char *RL_GetFileName(const char *filePath);              // Get pointer to filename for a path string
RLAPI const char *RL_GetFileNameWithoutExt(const char *filePath);    // Get filename string without extension (uses static string)
RLAPI const char *RL_GetDirectoryPath(const char *filePath);         // Get full path for a given fileName with path (uses static string)
RLAPI const char *RL_GetPrevDirectoryPath(const char *dirPath);      // Get previous directory path for a given path (uses static string)
RLAPI const char *RL_GetWorkingDirectory(void);                      // Get current working directory (uses static string)
RLAPI const char *RL_GetApplicationDirectory(void);                  // Get the directory of the running application (uses static string)
RLAPI bool RL_ChangeDirectory(const char *dir);                      // Change working directory, return true on success
RLAPI bool RL_IsPathFile(const char *path);                          // Check if a given path is a file or a directory
RLAPI RL_FilePathList RL_LoadDirectoryFiles(const char *dirPath);       // Load directory filepaths
RLAPI RL_FilePathList RL_LoadDirectoryFilesEx(const char *basePath, const char *filter, bool scanSubdirs); // Load directory filepaths with extension filtering and recursive directory scan
RLAPI void RL_UnloadDirectoryFiles(RL_FilePathList files);              // Unload filepaths
RLAPI bool RL_IsFileDropped(void);                                   // Check if a file has been dropped into window
RLAPI RL_FilePathList RL_LoadDroppedFiles(void);                        // Load dropped filepaths
RLAPI void RL_UnloadDroppedFiles(RL_FilePathList files);                // Unload dropped filepaths
RLAPI long RL_GetFileModTime(const char *fileName);                  // Get file modification time (last write time)

// Compression/Encoding functionality
RLAPI unsigned char *RL_CompressData(const unsigned char *data, int dataSize, int *compDataSize);        // Compress data (DEFLATE algorithm), memory must be RL_MemFree()
RLAPI unsigned char *RL_DecompressData(const unsigned char *compData, int compDataSize, int *dataSize);  // Decompress data (DEFLATE algorithm), memory must be RL_MemFree()
RLAPI char *RL_EncodeDataBase64(const unsigned char *data, int dataSize, int *outputSize);               // Encode data to Base64 string, memory must be RL_MemFree()
RLAPI unsigned char *RL_DecodeDataBase64(const unsigned char *data, int *outputSize);                    // Decode Base64 string data, memory must be RL_MemFree()

// Automation events functionality
RLAPI RL_AutomationEventList RL_LoadAutomationEventList(const char *fileName);                // Load automation events list from file, NULL for empty list, capacity = RL_MAX_AUTOMATION_EVENTS
RLAPI void RL_UnloadAutomationEventList(RL_AutomationEventList *list);                        // Unload automation events list from file
RLAPI bool RL_ExportAutomationEventList(RL_AutomationEventList list, const char *fileName);   // Export automation events list as text file
RLAPI void RL_SetAutomationEventList(RL_AutomationEventList *list);                           // Set automation event list to record to
RLAPI void RL_SetAutomationEventBaseFrame(int frame);                                      // Set automation event internal base frame to start recording
RLAPI void RL_StartAutomationEventRecording(void);                                         // Start recording automation events (RL_AutomationEventList must be set)
RLAPI void RL_StopAutomationEventRecording(void);                                          // Stop recording automation events
RLAPI void RL_PlayAutomationEvent(RL_AutomationEvent event);                                  // Play a recorded automation event

//------------------------------------------------------------------------------------
// Input Handling Functions (Module: core)
//------------------------------------------------------------------------------------

// Input-related functions: keyboard
RLAPI bool RL_IsKeyPressed(int key);                             // Check if a key has been pressed once
RLAPI bool RL_IsKeyPressedRepeat(int key);                       // Check if a key has been pressed again (Only PLATFORM_DESKTOP)
RLAPI bool RL_IsKeyDown(int key);                                // Check if a key is being pressed
RLAPI bool RL_IsKeyReleased(int key);                            // Check if a key has been released once
RLAPI bool RL_IsKeyUp(int key);                                  // Check if a key is NOT being pressed
RLAPI int RL_GetKeyPressed(void);                                // Get key pressed (keycode), call it multiple times for keys queued, returns 0 when the queue is empty
RLAPI int RL_GetCharPressed(void);                               // Get char pressed (unicode), call it multiple times for chars queued, returns 0 when the queue is empty
RLAPI void RL_SetExitKey(int key);                               // Set a custom key to exit program (default is ESC)

// Input-related functions: gamepads
RLAPI bool RL_IsGamepadAvailable(int gamepad);                   // Check if a gamepad is available
RLAPI const char *RL_GetGamepadName(int gamepad);                // Get gamepad internal name id
RLAPI bool RL_IsGamepadButtonPressed(int gamepad, int button);   // Check if a gamepad button has been pressed once
RLAPI bool RL_IsGamepadButtonDown(int gamepad, int button);      // Check if a gamepad button is being pressed
RLAPI bool RL_IsGamepadButtonReleased(int gamepad, int button);  // Check if a gamepad button has been released once
RLAPI bool RL_IsGamepadButtonUp(int gamepad, int button);        // Check if a gamepad button is NOT being pressed
RLAPI int RL_GetGamepadButtonPressed(void);                      // Get the last gamepad button pressed
RLAPI int RL_GetGamepadAxisCount(int gamepad);                   // Get gamepad axis count for a gamepad
RLAPI float RL_GetGamepadAxisMovement(int gamepad, int axis);    // Get axis movement value for a gamepad axis
RLAPI int RL_SetGamepadMappings(const char *mappings);           // Set internal gamepad mappings (SDL_GameControllerDB)

// Input-related functions: mouse
RLAPI bool RL_IsMouseButtonPressed(int button);                  // Check if a mouse button has been pressed once
RLAPI bool RL_IsMouseButtonDown(int button);                     // Check if a mouse button is being pressed
RLAPI bool RL_IsMouseButtonReleased(int button);                 // Check if a mouse button has been released once
RLAPI bool RL_IsMouseButtonUp(int button);                       // Check if a mouse button is NOT being pressed
RLAPI int RL_GetMouseX(void);                                    // Get mouse position X
RLAPI int RL_GetMouseY(void);                                    // Get mouse position Y
RLAPI RL_Vector2 RL_GetMousePosition(void);                         // Get mouse position XY
RLAPI RL_Vector2 RL_GetMouseDelta(void);                            // Get mouse delta between frames
RLAPI void RL_SetMousePosition(int x, int y);                    // Set mouse position XY
RLAPI void RL_SetMouseOffset(int offsetX, int offsetY);          // Set mouse offset
RLAPI void RL_SetMouseScale(float scaleX, float scaleY);         // Set mouse scaling
RLAPI float RL_GetMouseWheelMove(void);                          // Get mouse wheel movement for X or Y, whichever is larger
RLAPI RL_Vector2 RL_GetMouseWheelMoveV(void);                       // Get mouse wheel movement for both X and Y
RLAPI void RL_SetMouseCursor(int cursor);                        // Set mouse cursor

// Input-related functions: touch
RLAPI int RL_GetTouchX(void);                                    // Get touch position X for touch point 0 (relative to screen size)
RLAPI int RL_GetTouchY(void);                                    // Get touch position Y for touch point 0 (relative to screen size)
RLAPI RL_Vector2 RL_GetTouchPosition(int index);                    // Get touch position XY for a touch point index (relative to screen size)
RLAPI int RL_GetTouchPointId(int index);                         // Get touch point identifier for given index
RLAPI int RL_GetTouchPointCount(void);                           // Get number of touch points

//------------------------------------------------------------------------------------
// Gestures and Touch Handling Functions (Module: rgestures)
//------------------------------------------------------------------------------------
RLAPI void RL_SetGesturesEnabled(unsigned int flags);      // Enable a set of gestures using flags
RLAPI bool RL_IsGestureDetected(unsigned int gesture);     // Check if a gesture have been detected
RLAPI int RL_GetGestureDetected(void);                     // Get latest detected gesture
RLAPI float RL_GetGestureHoldDuration(void);               // Get gesture hold time in milliseconds
RLAPI RL_Vector2 RL_GetGestureDragVector(void);               // Get gesture drag vector
RLAPI float RL_GetGestureDragAngle(void);                  // Get gesture drag angle
RLAPI RL_Vector2 RL_GetGesturePinchVector(void);              // Get gesture pinch delta
RLAPI float RL_GetGesturePinchAngle(void);                 // Get gesture pinch angle

//------------------------------------------------------------------------------------
// RL_Camera System Functions (Module: rcamera)
//------------------------------------------------------------------------------------
RLAPI void RL_UpdateCamera(RL_Camera *camera, int mode);      // Update camera position for selected mode
RLAPI void RL_UpdateCameraPro(RL_Camera *camera, RL_Vector3 movement, RL_Vector3 rotation, float zoom); // Update camera movement/rotation

//------------------------------------------------------------------------------------
// Basic Shapes Drawing Functions (Module: shapes)
//------------------------------------------------------------------------------------
// Set texture and rectangle to be used on shapes drawing
// NOTE: It can be useful when using basic shapes and one single font,
// defining a font char white rectangle would allow drawing everything in a single draw call
RLAPI void RL_SetShapesTexture(RL_Texture2D texture, RL_Rectangle source);       // Set texture and rectangle to be used on shapes drawing

// Basic shapes drawing functions
RLAPI void RL_DrawPixel(int posX, int posY, RL_Color color);                                                   // Draw a pixel
RLAPI void RL_DrawPixelV(RL_Vector2 position, RL_Color color);                                                    // Draw a pixel (Vector version)
RLAPI void RL_DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, RL_Color color);                // Draw a line
RLAPI void RL_DrawLineV(RL_Vector2 startPos, RL_Vector2 endPos, RL_Color color);                                     // Draw a line (using gl lines)
RLAPI void RL_DrawLineEx(RL_Vector2 startPos, RL_Vector2 endPos, float thick, RL_Color color);                       // Draw a line (using triangles/quads)
RLAPI void RL_DrawLineStrip(RL_Vector2 *points, int pointCount, RL_Color color);                                  // Draw lines sequence (using gl lines)
RLAPI void RL_DrawLineBezier(RL_Vector2 startPos, RL_Vector2 endPos, float thick, RL_Color color);                   // Draw line segment cubic-bezier in-out interpolation
RLAPI void RL_DrawCircle(int centerX, int centerY, float radius, RL_Color color);                              // Draw a color-filled circle
RLAPI void RL_DrawCircleSector(RL_Vector2 center, float radius, float startAngle, float endAngle, int segments, RL_Color color);      // Draw a piece of a circle
RLAPI void RL_DrawCircleSectorLines(RL_Vector2 center, float radius, float startAngle, float endAngle, int segments, RL_Color color); // Draw circle sector outline
RLAPI void RL_DrawCircleGradient(int centerX, int centerY, float radius, RL_Color color1, RL_Color color2);       // Draw a gradient-filled circle
RLAPI void RL_DrawCircleV(RL_Vector2 center, float radius, RL_Color color);                                       // Draw a color-filled circle (Vector version)
RLAPI void RL_DrawCircleLines(int centerX, int centerY, float radius, RL_Color color);                         // Draw circle outline
RLAPI void RL_DrawCircleLinesV(RL_Vector2 center, float radius, RL_Color color);                                  // Draw circle outline (Vector version)
RLAPI void RL_DrawEllipse(int centerX, int centerY, float radiusH, float radiusV, RL_Color color);             // Draw ellipse
RLAPI void RL_DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, RL_Color color);        // Draw ellipse outline
RLAPI void RL_DrawRing(RL_Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, RL_Color color); // Draw ring
RLAPI void RL_DrawRingLines(RL_Vector2 center, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, RL_Color color);    // Draw ring outline
RLAPI void RL_DrawRectangle(int posX, int posY, int width, int height, RL_Color color);                        // Draw a color-filled rectangle
RLAPI void RL_DrawRectangleV(RL_Vector2 position, RL_Vector2 size, RL_Color color);                                  // Draw a color-filled rectangle (Vector version)
RLAPI void RL_DrawRectangleRec(RL_Rectangle rec, RL_Color color);                                                 // Draw a color-filled rectangle
RLAPI void RL_DrawRectanglePro(RL_Rectangle rec, RL_Vector2 origin, float rotation, RL_Color color);                 // Draw a color-filled rectangle with pro parameters
RLAPI void RL_DrawRectangleGradientV(int posX, int posY, int width, int height, RL_Color color1, RL_Color color2);// Draw a vertical-gradient-filled rectangle
RLAPI void RL_DrawRectangleGradientH(int posX, int posY, int width, int height, RL_Color color1, RL_Color color2);// Draw a horizontal-gradient-filled rectangle
RLAPI void RL_DrawRectangleGradientEx(RL_Rectangle rec, RL_Color col1, RL_Color col2, RL_Color col3, RL_Color col4);       // Draw a gradient-filled rectangle with custom vertex colors
RLAPI void RL_DrawRectangleLines(int posX, int posY, int width, int height, RL_Color color);                   // Draw rectangle outline
RLAPI void RL_DrawRectangleLinesEx(RL_Rectangle rec, float lineThick, RL_Color color);                            // Draw rectangle outline with extended parameters
RLAPI void RL_DrawRectangleRounded(RL_Rectangle rec, float roundness, int segments, RL_Color color);              // Draw rectangle with rounded edges
RLAPI void RL_DrawRectangleRoundedLines(RL_Rectangle rec, float roundness, int segments, float lineThick, RL_Color color); // Draw rectangle with rounded edges outline
RLAPI void RL_DrawTriangle(RL_Vector2 v1, RL_Vector2 v2, RL_Vector2 v3, RL_Color color);                                // Draw a color-filled triangle (vertex in counter-clockwise order!)
RLAPI void RL_DrawTriangleLines(RL_Vector2 v1, RL_Vector2 v2, RL_Vector2 v3, RL_Color color);                           // Draw triangle outline (vertex in counter-clockwise order!)
RLAPI void RL_DrawTriangleFan(RL_Vector2 *points, int pointCount, RL_Color color);                                // Draw a triangle fan defined by points (first vertex is the center)
RLAPI void RL_DrawTriangleStrip(RL_Vector2 *points, int pointCount, RL_Color color);                              // Draw a triangle strip defined by points
RLAPI void RL_DrawPoly(RL_Vector2 center, int sides, float radius, float rotation, RL_Color color);               // Draw a regular polygon (Vector version)
RLAPI void RL_DrawPolyLines(RL_Vector2 center, int sides, float radius, float rotation, RL_Color color);          // Draw a polygon outline of n sides
RLAPI void RL_DrawPolyLinesEx(RL_Vector2 center, int sides, float radius, float rotation, float lineThick, RL_Color color); // Draw a polygon outline of n sides with extended parameters

// Splines drawing functions
RLAPI void DrawSplineLinear(RL_Vector2 *points, int pointCount, float thick, RL_Color color);                  // Draw spline: Linear, minimum 2 points
RLAPI void DrawSplineBasis(RL_Vector2 *points, int pointCount, float thick, RL_Color color);                   // Draw spline: B-Spline, minimum 4 points
RLAPI void DrawSplineCatmullRom(RL_Vector2 *points, int pointCount, float thick, RL_Color color);              // Draw spline: Catmull-Rom, minimum 4 points
RLAPI void DrawSplineBezierQuadratic(RL_Vector2 *points, int pointCount, float thick, RL_Color color);         // Draw spline: Quadratic Bezier, minimum 3 points (1 control point): [p1, c2, p3, c4...]
RLAPI void DrawSplineBezierCubic(RL_Vector2 *points, int pointCount, float thick, RL_Color color);             // Draw spline: Cubic Bezier, minimum 4 points (2 control points): [p1, c2, c3, p4, c5, c6...]
RLAPI void DrawSplineSegmentLinear(RL_Vector2 p1, RL_Vector2 p2, float thick, RL_Color color);                    // Draw spline segment: Linear, 2 points
RLAPI void DrawSplineSegmentBasis(RL_Vector2 p1, RL_Vector2 p2, RL_Vector2 p3, RL_Vector2 p4, float thick, RL_Color color); // Draw spline segment: B-Spline, 4 points
RLAPI void DrawSplineSegmentCatmullRom(RL_Vector2 p1, RL_Vector2 p2, RL_Vector2 p3, RL_Vector2 p4, float thick, RL_Color color); // Draw spline segment: Catmull-Rom, 4 points
RLAPI void DrawSplineSegmentBezierQuadratic(RL_Vector2 p1, RL_Vector2 c2, RL_Vector2 p3, float thick, RL_Color color); // Draw spline segment: Quadratic Bezier, 2 points, 1 control point
RLAPI void DrawSplineSegmentBezierCubic(RL_Vector2 p1, RL_Vector2 c2, RL_Vector2 c3, RL_Vector2 p4, float thick, RL_Color color); // Draw spline segment: Cubic Bezier, 2 points, 2 control points

// Spline segment point evaluation functions, for a given t [0.0f .. 1.0f]
RLAPI RL_Vector2 GetSplinePointLinear(RL_Vector2 startPos, RL_Vector2 endPos, float t);                           // Get (evaluate) spline point: Linear
RLAPI RL_Vector2 GetSplinePointBasis(RL_Vector2 p1, RL_Vector2 p2, RL_Vector2 p3, RL_Vector2 p4, float t);              // Get (evaluate) spline point: B-Spline
RLAPI RL_Vector2 GetSplinePointCatmullRom(RL_Vector2 p1, RL_Vector2 p2, RL_Vector2 p3, RL_Vector2 p4, float t);         // Get (evaluate) spline point: Catmull-Rom
RLAPI RL_Vector2 GetSplinePointBezierQuad(RL_Vector2 p1, RL_Vector2 c2, RL_Vector2 p3, float t);                     // Get (evaluate) spline point: Quadratic Bezier
RLAPI RL_Vector2 GetSplinePointBezierCubic(RL_Vector2 p1, RL_Vector2 c2, RL_Vector2 c3, RL_Vector2 p4, float t);        // Get (evaluate) spline point: Cubic Bezier

// Basic shapes collision detection functions
RLAPI bool RL_CheckCollisionRecs(RL_Rectangle rec1, RL_Rectangle rec2);                                           // Check collision between two rectangles
RLAPI bool RL_CheckCollisionCircles(RL_Vector2 center1, float radius1, RL_Vector2 center2, float radius2);        // Check collision between two circles
RLAPI bool RL_CheckCollisionCircleRec(RL_Vector2 center, float radius, RL_Rectangle rec);                         // Check collision between circle and rectangle
RLAPI bool RL_CheckCollisionPointRec(RL_Vector2 point, RL_Rectangle rec);                                         // Check if point is inside rectangle
RLAPI bool RL_CheckCollisionPointCircle(RL_Vector2 point, RL_Vector2 center, float radius);                       // Check if point is inside circle
RLAPI bool RL_CheckCollisionPointTriangle(RL_Vector2 point, RL_Vector2 p1, RL_Vector2 p2, RL_Vector2 p3);               // Check if point is inside a triangle
RLAPI bool RL_CheckCollisionPointPoly(RL_Vector2 point, RL_Vector2 *points, int pointCount);                      // Check if point is within a polygon described by array of vertices
RLAPI bool RL_CheckCollisionLines(RL_Vector2 startPos1, RL_Vector2 endPos1, RL_Vector2 startPos2, RL_Vector2 endPos2, RL_Vector2 *collisionPoint); // Check the collision between two lines defined by two points each, returns collision point by reference
RLAPI bool RL_CheckCollisionPointLine(RL_Vector2 point, RL_Vector2 p1, RL_Vector2 p2, int threshold);                // Check if point belongs to line created between two points [p1] and [p2] with defined margin in pixels [threshold]
RLAPI RL_Rectangle RL_GetCollisionRec(RL_Rectangle rec1, RL_Rectangle rec2);                                         // Get collision rectangle for two rectangles collision

//------------------------------------------------------------------------------------
// RL_Texture Loading and Drawing Functions (Module: textures)
//------------------------------------------------------------------------------------

// RL_Image loading functions
// NOTE: These functions do not require GPU access
RLAPI RL_Image RL_LoadImage(const char *fileName);                                                             // Load image from file into CPU memory (RAM)
RLAPI RL_Image RL_LoadImageRaw(const char *fileName, int width, int height, int format, int headerSize);       // Load image from RAW file data
RLAPI RL_Image RL_LoadImageSvg(const char *fileNameOrString, int width, int height);                           // Load image from SVG file data or string with specified size
RLAPI RL_Image RL_LoadImageAnim(const char *fileName, int *frames);                                            // Load image sequence from file (frames appended to image.data)
RLAPI RL_Image RL_LoadImageFromMemory(const char *fileType, const unsigned char *fileData, int dataSize);      // Load image from memory buffer, fileType refers to extension: i.e. '.png'
RLAPI RL_Image RL_LoadImageFromTexture(RL_Texture2D texture);                                                     // Load image from GPU texture data
RLAPI RL_Image RL_LoadImageFromScreen(void);                                                                   // Load image from screen buffer and (screenshot)
RLAPI bool RL_IsImageReady(RL_Image image);                                                                    // Check if an image is ready
RLAPI void RL_UnloadImage(RL_Image image);                                                                     // Unload image from CPU memory (RAM)
RLAPI bool RL_ExportImage(RL_Image image, const char *fileName);                                               // Export image data to file, returns true on success
RLAPI unsigned char *RL_ExportImageToMemory(RL_Image image, const char *fileType, int *fileSize);              // Export image to memory buffer
RLAPI bool RL_ExportImageAsCode(RL_Image image, const char *fileName);                                         // Export image as code file defining an array of bytes, returns true on success

// RL_Image generation functions
RLAPI RL_Image RL_GenImageColor(int width, int height, RL_Color color);                                           // Generate image: plain color
RLAPI RL_Image RL_GenImageGradientLinear(int width, int height, int direction, RL_Color start, RL_Color end);        // Generate image: linear gradient, direction in degrees [0..360], 0=Vertical gradient
RLAPI RL_Image RL_GenImageGradientRadial(int width, int height, float density, RL_Color inner, RL_Color outer);      // Generate image: radial gradient
RLAPI RL_Image RL_GenImageGradientSquare(int width, int height, float density, RL_Color inner, RL_Color outer);      // Generate image: square gradient
RLAPI RL_Image RL_GenImageChecked(int width, int height, int checksX, int checksY, RL_Color col1, RL_Color col2);    // Generate image: checked
RLAPI RL_Image RL_GenImageWhiteNoise(int width, int height, float factor);                                     // Generate image: white noise
RLAPI RL_Image RL_GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale);           // Generate image: perlin noise
RLAPI RL_Image RL_GenImageCellular(int width, int height, int tileSize);                                       // Generate image: cellular algorithm, bigger tileSize means bigger cells
RLAPI RL_Image RL_GenImageText(int width, int height, const char *text);                                       // Generate image: grayscale image from text data

// RL_Image manipulation functions
RLAPI RL_Image RL_ImageCopy(RL_Image image);                                                                      // Create an image duplicate (useful for transformations)
RLAPI RL_Image RL_ImageFromImage(RL_Image image, RL_Rectangle rec);                                                  // Create an image from another image piece
RLAPI RL_Image RL_ImageText(const char *text, int fontSize, RL_Color color);                                      // Create an image from text (default font)
RLAPI RL_Image RL_ImageTextEx(RL_Font font, const char *text, float fontSize, float spacing, RL_Color tint);         // Create an image from text (custom sprite font)
RLAPI void RL_ImageFormat(RL_Image *image, int newFormat);                                                     // Convert image data to desired format
RLAPI void RL_ImageToPOT(RL_Image *image, RL_Color fill);                                                         // Convert image to POT (power-of-two)
RLAPI void RL_ImageCrop(RL_Image *image, RL_Rectangle crop);                                                      // Crop an image to a defined rectangle
RLAPI void RL_ImageAlphaCrop(RL_Image *image, float threshold);                                                // Crop image depending on alpha value
RLAPI void RL_ImageAlphaClear(RL_Image *image, RL_Color color, float threshold);                                  // Clear alpha channel to desired color
RLAPI void RL_ImageAlphaMask(RL_Image *image, RL_Image alphaMask);                                                // Apply alpha mask to image
RLAPI void RL_ImageAlphaPremultiply(RL_Image *image);                                                          // Premultiply alpha channel
RLAPI void RL_ImageBlurGaussian(RL_Image *image, int blurSize);                                                // Apply Gaussian blur using a box blur approximation
RLAPI void RL_ImageResize(RL_Image *image, int newWidth, int newHeight);                                       // Resize image (Bicubic scaling algorithm)
RLAPI void RL_ImageResizeNN(RL_Image *image, int newWidth,int newHeight);                                      // Resize image (Nearest-Neighbor scaling algorithm)
RLAPI void RL_ImageResizeCanvas(RL_Image *image, int newWidth, int newHeight, int offsetX, int offsetY, RL_Color fill);  // Resize canvas and fill with color
RLAPI void RL_ImageMipmaps(RL_Image *image);                                                                   // Compute all mipmap levels for a provided image
RLAPI void RL_ImageDither(RL_Image *image, int rBpp, int gBpp, int bBpp, int aBpp);                            // Dither image data to 16bpp or lower (Floyd-Steinberg dithering)
RLAPI void RL_ImageFlipVertical(RL_Image *image);                                                              // Flip image vertically
RLAPI void RL_ImageFlipHorizontal(RL_Image *image);                                                            // Flip image horizontally
RLAPI void RL_ImageRotate(RL_Image *image, int degrees);                                                       // Rotate image by input angle in degrees (-359 to 359)
RLAPI void RL_ImageRotateCW(RL_Image *image);                                                                  // Rotate image clockwise 90deg
RLAPI void RL_ImageRotateCCW(RL_Image *image);                                                                 // Rotate image counter-clockwise 90deg
RLAPI void RL_ImageColorTint(RL_Image *image, RL_Color color);                                                    // Modify image color: tint
RLAPI void RL_ImageColorInvert(RL_Image *image);                                                               // Modify image color: invert
RLAPI void RL_ImageColorGrayscale(RL_Image *image);                                                            // Modify image color: grayscale
RLAPI void RL_ImageColorContrast(RL_Image *image, float contrast);                                             // Modify image color: contrast (-100 to 100)
RLAPI void RL_ImageColorBrightness(RL_Image *image, int brightness);                                           // Modify image color: brightness (-255 to 255)
RLAPI void RL_ImageColorReplace(RL_Image *image, RL_Color color, RL_Color replace);                                  // Modify image color: replace color
RLAPI RL_Color *RL_LoadImageColors(RL_Image image);                                                               // Load color data from image as a RL_Color array (RGBA - 32bit)
RLAPI RL_Color *RL_LoadImagePalette(RL_Image image, int maxPaletteSize, int *colorCount);                         // Load colors palette from image as a RL_Color array (RGBA - 32bit)
RLAPI void RL_UnloadImageColors(RL_Color *colors);                                                             // Unload color data loaded with RL_LoadImageColors()
RLAPI void RL_UnloadImagePalette(RL_Color *colors);                                                            // Unload colors palette loaded with RL_LoadImagePalette()
RLAPI RL_Rectangle RL_GetImageAlphaBorder(RL_Image image, float threshold);                                       // Get image alpha border rectangle
RLAPI RL_Color RL_GetImageColor(RL_Image image, int x, int y);                                                    // Get image pixel color at (x, y) position

// RL_Image drawing functions
// NOTE: RL_Image software-rendering functions (CPU)
RLAPI void RL_ImageClearBackground(RL_Image *dst, RL_Color color);                                                // Clear image background with given color
RLAPI void RL_ImageDrawPixel(RL_Image *dst, int posX, int posY, RL_Color color);                                  // Draw pixel within an image
RLAPI void RL_ImageDrawPixelV(RL_Image *dst, RL_Vector2 position, RL_Color color);                                   // Draw pixel within an image (Vector version)
RLAPI void RL_ImageDrawLine(RL_Image *dst, int startPosX, int startPosY, int endPosX, int endPosY, RL_Color color); // Draw line within an image
RLAPI void RL_ImageDrawLineV(RL_Image *dst, RL_Vector2 start, RL_Vector2 end, RL_Color color);                          // Draw line within an image (Vector version)
RLAPI void RL_ImageDrawCircle(RL_Image *dst, int centerX, int centerY, int radius, RL_Color color);               // Draw a filled circle within an image
RLAPI void RL_ImageDrawCircleV(RL_Image *dst, RL_Vector2 center, int radius, RL_Color color);                        // Draw a filled circle within an image (Vector version)
RLAPI void RL_ImageDrawCircleLines(RL_Image *dst, int centerX, int centerY, int radius, RL_Color color);          // Draw circle outline within an image
RLAPI void RL_ImageDrawCircleLinesV(RL_Image *dst, RL_Vector2 center, int radius, RL_Color color);                   // Draw circle outline within an image (Vector version)
RLAPI void RL_ImageDrawRectangle(RL_Image *dst, int posX, int posY, int width, int height, RL_Color color);       // Draw rectangle within an image
RLAPI void RL_ImageDrawRectangleV(RL_Image *dst, RL_Vector2 position, RL_Vector2 size, RL_Color color);                 // Draw rectangle within an image (Vector version)
RLAPI void RL_ImageDrawRectangleRec(RL_Image *dst, RL_Rectangle rec, RL_Color color);                                // Draw rectangle within an image
RLAPI void RL_ImageDrawRectangleLines(RL_Image *dst, RL_Rectangle rec, int thick, RL_Color color);                   // Draw rectangle lines within an image
RLAPI void RL_ImageDraw(RL_Image *dst, RL_Image src, RL_Rectangle srcRec, RL_Rectangle dstRec, RL_Color tint);             // Draw a source image within a destination image (tint applied to source)
RLAPI void RL_ImageDrawText(RL_Image *dst, const char *text, int posX, int posY, int fontSize, RL_Color color);   // Draw text (using default font) within an image (destination)
RLAPI void RL_ImageDrawTextEx(RL_Image *dst, RL_Font font, const char *text, RL_Vector2 position, float fontSize, float spacing, RL_Color tint); // Draw text (custom sprite font) within an image (destination)

// RL_Texture loading functions
// NOTE: These functions require GPU access
RLAPI RL_Texture2D RL_LoadTexture(const char *fileName);                                                       // Load texture from file into GPU memory (VRAM)
RLAPI RL_Texture2D RL_LoadTextureFromImage(RL_Image image);                                                       // Load texture from image data
RLAPI RL_TextureCubemap RL_LoadTextureCubemap(RL_Image image, int layout);                                        // Load cubemap from image, multiple image cubemap layouts supported
RLAPI RL_RenderTexture2D RL_LoadRenderTexture(int width, int height);                                          // Load texture for rendering (framebuffer)
RLAPI bool RL_IsTextureReady(RL_Texture2D texture);                                                            // Check if a texture is ready
RLAPI void RL_UnloadTexture(RL_Texture2D texture);                                                             // Unload texture from GPU memory (VRAM)
RLAPI bool RL_IsRenderTextureReady(RL_RenderTexture2D target);                                                 // Check if a render texture is ready
RLAPI void RL_UnloadRenderTexture(RL_RenderTexture2D target);                                                  // Unload render texture from GPU memory (VRAM)
RLAPI void RL_UpdateTexture(RL_Texture2D texture, const void *pixels);                                         // Update GPU texture with new data
RLAPI void RL_UpdateTextureRec(RL_Texture2D texture, RL_Rectangle rec, const void *pixels);                       // Update GPU texture rectangle with new data

// RL_Texture configuration functions
RLAPI void RL_GenTextureMipmaps(RL_Texture2D *texture);                                                        // Generate GPU mipmaps for a texture
RLAPI void RL_SetTextureFilter(RL_Texture2D texture, int filter);                                              // Set texture scaling filter mode
RLAPI void RL_SetTextureWrap(RL_Texture2D texture, int wrap);                                                  // Set texture wrapping mode

// RL_Texture drawing functions
RLAPI void RL_DrawTexture(RL_Texture2D texture, int posX, int posY, RL_Color tint);                               // Draw a RL_Texture2D
RLAPI void RL_DrawTextureV(RL_Texture2D texture, RL_Vector2 position, RL_Color tint);                                // Draw a RL_Texture2D with position defined as RL_Vector2
RLAPI void RL_DrawTextureEx(RL_Texture2D texture, RL_Vector2 position, float rotation, float scale, RL_Color tint);  // Draw a RL_Texture2D with extended parameters
RLAPI void RL_DrawTextureRec(RL_Texture2D texture, RL_Rectangle source, RL_Vector2 position, RL_Color tint);            // Draw a part of a texture defined by a rectangle
RLAPI void RL_DrawTexturePro(RL_Texture2D texture, RL_Rectangle source, RL_Rectangle dest, RL_Vector2 origin, float rotation, RL_Color tint); // Draw a part of a texture defined by a rectangle with 'pro' parameters
RLAPI void RL_DrawTextureNPatch(RL_Texture2D texture, RL_NPatchInfo nPatchInfo, RL_Rectangle dest, RL_Vector2 origin, float rotation, RL_Color tint); // Draws a texture (or part of it) that stretches or shrinks nicely

// RL_Color/pixel related functions
RLAPI RL_Color RL_Fade(RL_Color color, float alpha);                                 // Get color with alpha applied, alpha goes from 0.0f to 1.0f
RLAPI int RL_ColorToInt(RL_Color color);                                          // Get hexadecimal value for a RL_Color
RLAPI RL_Vector4 RL_ColorNormalize(RL_Color color);                                  // Get RL_Color normalized as float [0..1]
RLAPI RL_Color RL_ColorFromNormalized(RL_Vector4 normalized);                        // Get RL_Color from normalized values [0..1]
RLAPI RL_Vector3 RL_ColorToHSV(RL_Color color);                                      // Get HSV values for a RL_Color, hue [0..360], saturation/value [0..1]
RLAPI RL_Color RL_ColorFromHSV(float hue, float saturation, float value);         // Get a RL_Color from HSV values, hue [0..360], saturation/value [0..1]
RLAPI RL_Color RL_ColorTint(RL_Color color, RL_Color tint);                             // Get color multiplied with another color
RLAPI RL_Color RL_ColorBrightness(RL_Color color, float factor);                     // Get color with brightness correction, brightness factor goes from -1.0f to 1.0f
RLAPI RL_Color RL_ColorContrast(RL_Color color, float contrast);                     // Get color with contrast correction, contrast values between -1.0f and 1.0f
RLAPI RL_Color RL_ColorAlpha(RL_Color color, float alpha);                           // Get color with alpha applied, alpha goes from 0.0f to 1.0f
RLAPI RL_Color RL_ColorAlphaBlend(RL_Color dst, RL_Color src, RL_Color tint);              // Get src alpha-blended into dst color with tint
RLAPI RL_Color RL_GetColor(unsigned int hexValue);                                // Get RL_Color structure from hexadecimal value
RLAPI RL_Color RL_GetPixelColor(void *srcPtr, int format);                        // Get RL_Color from a source pixel pointer of certain format
RLAPI void RL_SetPixelColor(void *dstPtr, RL_Color color, int format);            // Set color formatted into destination pixel pointer
RLAPI int RL_GetPixelDataSize(int width, int height, int format);              // Get pixel data size in bytes for certain format

//------------------------------------------------------------------------------------
// RL_Font Loading and Text Drawing Functions (Module: text)
//------------------------------------------------------------------------------------

// RL_Font loading/unloading functions
RLAPI RL_Font RL_GetFontDefault(void);                                                            // Get the default RL_Font
RLAPI RL_Font RL_LoadFont(const char *fileName);                                                  // Load font from file into GPU memory (VRAM)
RLAPI RL_Font RL_LoadFontEx(const char *fileName, int fontSize, int *codepoints, int codepointCount);  // Load font from file with extended parameters, use NULL for codepoints and 0 for codepointCount to load the default character set
RLAPI RL_Font RL_LoadFontFromImage(RL_Image image, RL_Color key, int firstChar);                        // Load font from RL_Image (XNA style)
RLAPI RL_Font RL_LoadFontFromMemory(const char *fileType, const unsigned char *fileData, int dataSize, int fontSize, int *codepoints, int codepointCount); // Load font from memory buffer, fileType refers to extension: i.e. '.ttf'
RLAPI bool RL_IsFontReady(RL_Font font);                                                          // Check if a font is ready
RLAPI RL_GlyphInfo *RL_LoadFontData(const unsigned char *fileData, int dataSize, int fontSize, int *codepoints, int codepointCount, int type); // Load font data for further use
RLAPI RL_Image RL_GenImageFontAtlas(const RL_GlyphInfo *glyphs, RL_Rectangle **glyphRecs, int glyphCount, int fontSize, int padding, int packMethod); // Generate image font atlas using chars info
RLAPI void RL_UnloadFontData(RL_GlyphInfo *glyphs, int glyphCount);                               // Unload font chars info data (RAM)
RLAPI void RL_UnloadFont(RL_Font font);                                                           // Unload font from GPU memory (VRAM)
RLAPI bool RL_ExportFontAsCode(RL_Font font, const char *fileName);                               // Export font as code file, returns true on success

// Text drawing functions
RLAPI void RL_DrawFPS(int posX, int posY);                                                     // Draw current FPS
RLAPI void RL_DrawText(const char *text, int posX, int posY, int fontSize, RL_Color color);       // Draw text (using default font)
RLAPI void RL_DrawTextEx(RL_Font font, const char *text, RL_Vector2 position, float fontSize, float spacing, RL_Color tint); // Draw text using font and additional parameters
RLAPI void RL_DrawTextPro(RL_Font font, const char *text, RL_Vector2 position, RL_Vector2 origin, float rotation, float fontSize, float spacing, RL_Color tint); // Draw text using RL_Font and pro parameters (rotation)
RLAPI void RL_DrawTextCodepoint(RL_Font font, int codepoint, RL_Vector2 position, float fontSize, RL_Color tint); // Draw one character (codepoint)
RLAPI void RL_DrawTextCodepoints(RL_Font font, const int *codepoints, int codepointCount, RL_Vector2 position, float fontSize, float spacing, RL_Color tint); // Draw multiple character (codepoint)

// Text font info functions
RLAPI void RL_SetTextLineSpacing(int spacing);                                                 // Set vertical line spacing when drawing with line-breaks
RLAPI int RL_MeasureText(const char *text, int fontSize);                                      // Measure string width for default font
RLAPI RL_Vector2 RL_MeasureTextEx(RL_Font font, const char *text, float fontSize, float spacing);    // Measure string size for RL_Font
RLAPI int RL_GetGlyphIndex(RL_Font font, int codepoint);                                          // Get glyph index position in font for a codepoint (unicode character), fallback to '?' if not found
RLAPI RL_GlyphInfo RL_GetGlyphInfo(RL_Font font, int codepoint);                                     // Get glyph font info data for a codepoint (unicode character), fallback to '?' if not found
RLAPI RL_Rectangle RL_GetGlyphAtlasRec(RL_Font font, int codepoint);                                 // Get glyph rectangle in font atlas for a codepoint (unicode character), fallback to '?' if not found

// Text codepoints management functions (unicode characters)
RLAPI char *RL_LoadUTF8(const int *codepoints, int length);                // Load UTF-8 text encoded from codepoints array
RLAPI void RL_UnloadUTF8(char *text);                                      // Unload UTF-8 text encoded from codepoints array
RLAPI int *RL_LoadCodepoints(const char *text, int *count);                // Load all codepoints from a UTF-8 text string, codepoints count returned by parameter
RLAPI void RL_UnloadCodepoints(int *codepoints);                           // Unload codepoints data from memory
RLAPI int RL_GetCodepointCount(const char *text);                          // Get total number of codepoints in a UTF-8 encoded string
RLAPI int RL_GetCodepoint(const char *text, int *codepointSize);           // Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
RLAPI int RL_GetCodepointNext(const char *text, int *codepointSize);       // Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
RLAPI int RL_GetCodepointPrevious(const char *text, int *codepointSize);   // Get previous codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
RLAPI const char *RL_CodepointToUTF8(int codepoint, int *utf8Size);        // Encode one codepoint into UTF-8 byte array (array length returned as parameter)

// Text strings management functions (no UTF-8 strings, only byte chars)
// NOTE: Some strings allocate memory internally for returned strings, just be careful!
RLAPI int RL_TextCopy(char *dst, const char *src);                                             // Copy one string to another, returns bytes copied
RLAPI bool RL_TextIsEqual(const char *text1, const char *text2);                               // Check if two text string are equal
RLAPI unsigned int RL_TextLength(const char *text);                                            // Get text length, checks for '\0' ending
RLAPI const char *RL_TextFormat(const char *text, ...);                                        // Text formatting with variables (sprintf() style)
RLAPI const char *RL_TextSubtext(const char *text, int position, int length);                  // Get a piece of a text string
RLAPI char *RL_TextReplace(char *text, const char *replace, const char *by);                   // Replace text string (WARNING: memory must be freed!)
RLAPI char *RL_TextInsert(const char *text, const char *insert, int position);                 // Insert text in a position (WARNING: memory must be freed!)
RLAPI const char *RL_TextJoin(const char **textList, int count, const char *delimiter);        // Join text strings with delimiter
RLAPI const char **TextSplit(const char *text, char delimiter, int *count);                 // Split text into multiple strings
RLAPI void RL_TextAppend(char *text, const char *append, int *position);                       // Append text at specific position and move cursor!
RLAPI int RL_TextFindIndex(const char *text, const char *find);                                // Find first text occurrence within a string
RLAPI const char *RL_TextToUpper(const char *text);                      // Get upper case version of provided string
RLAPI const char *RL_TextToLower(const char *text);                      // Get lower case version of provided string
RLAPI const char *RL_TextToPascal(const char *text);                     // Get Pascal case notation version of provided string
RLAPI int RL_TextToInteger(const char *text);                            // Get integer value from text (negative values not supported)

//------------------------------------------------------------------------------------
// Basic 3d Shapes Drawing Functions (Module: models)
//------------------------------------------------------------------------------------

// Basic geometric 3D shapes drawing functions
RLAPI void RL_DrawLine3D(RL_Vector3 startPos, RL_Vector3 endPos, RL_Color color);                                    // Draw a line in 3D world space
RLAPI void RL_DrawPoint3D(RL_Vector3 position, RL_Color color);                                                   // Draw a point in 3D space, actually a small line
RLAPI void RL_DrawCircle3D(RL_Vector3 center, float radius, RL_Vector3 rotationAxis, float rotationAngle, RL_Color color); // Draw a circle in 3D world space
RLAPI void RL_DrawTriangle3D(RL_Vector3 v1, RL_Vector3 v2, RL_Vector3 v3, RL_Color color);                              // Draw a color-filled triangle (vertex in counter-clockwise order!)
RLAPI void RL_DrawTriangleStrip3D(RL_Vector3 *points, int pointCount, RL_Color color);                            // Draw a triangle strip defined by points
RLAPI void RL_DrawCube(RL_Vector3 position, float width, float height, float length, RL_Color color);             // Draw cube
RLAPI void RL_DrawCubeV(RL_Vector3 position, RL_Vector3 size, RL_Color color);                                       // Draw cube (Vector version)
RLAPI void RL_DrawCubeWires(RL_Vector3 position, float width, float height, float length, RL_Color color);        // Draw cube wires
RLAPI void RL_DrawCubeWiresV(RL_Vector3 position, RL_Vector3 size, RL_Color color);                                  // Draw cube wires (Vector version)
RLAPI void RL_DrawSphere(RL_Vector3 centerPos, float radius, RL_Color color);                                     // Draw sphere
RLAPI void RL_DrawSphereEx(RL_Vector3 centerPos, float radius, int rings, int slices, RL_Color color);            // Draw sphere with extended parameters
RLAPI void RL_DrawSphereWires(RL_Vector3 centerPos, float radius, int rings, int slices, RL_Color color);         // Draw sphere wires
RLAPI void RL_DrawCylinder(RL_Vector3 position, float radiusTop, float radiusBottom, float height, int slices, RL_Color color); // Draw a cylinder/cone
RLAPI void RL_DrawCylinderEx(RL_Vector3 startPos, RL_Vector3 endPos, float startRadius, float endRadius, int sides, RL_Color color); // Draw a cylinder with base at startPos and top at endPos
RLAPI void RL_DrawCylinderWires(RL_Vector3 position, float radiusTop, float radiusBottom, float height, int slices, RL_Color color); // Draw a cylinder/cone wires
RLAPI void RL_DrawCylinderWiresEx(RL_Vector3 startPos, RL_Vector3 endPos, float startRadius, float endRadius, int sides, RL_Color color); // Draw a cylinder wires with base at startPos and top at endPos
RLAPI void RL_DrawCapsule(RL_Vector3 startPos, RL_Vector3 endPos, float radius, int slices, int rings, RL_Color color); // Draw a capsule with the center of its sphere caps at startPos and endPos
RLAPI void RL_DrawCapsuleWires(RL_Vector3 startPos, RL_Vector3 endPos, float radius, int slices, int rings, RL_Color color); // Draw capsule wireframe with the center of its sphere caps at startPos and endPos
RLAPI void RL_DrawPlane(RL_Vector3 centerPos, RL_Vector2 size, RL_Color color);                                      // Draw a plane XZ
RLAPI void RL_DrawRay(RL_Ray ray, RL_Color color);                                                                // Draw a ray line
RLAPI void RL_DrawGrid(int slices, float spacing);                                                          // Draw a grid (centered at (0, 0, 0))

//------------------------------------------------------------------------------------
// RL_Model 3d Loading and Drawing Functions (Module: models)
//------------------------------------------------------------------------------------

// RL_Model management functions
RLAPI RL_Model RL_LoadModel(const char *fileName);                                                // Load model from files (meshes and materials)
RLAPI RL_Model RL_LoadModelFromMesh(RL_Mesh mesh);                                                   // Load model from generated mesh (default material)
RLAPI bool RL_IsModelReady(RL_Model model);                                                       // Check if a model is ready
RLAPI void RL_UnloadModel(RL_Model model);                                                        // Unload model (including meshes) from memory (RAM and/or VRAM)
RLAPI RL_BoundingBox RL_GetModelBoundingBox(RL_Model model);                                         // Compute model bounding box limits (considers all meshes)

// RL_Model drawing functions
RLAPI void RL_DrawModel(RL_Model model, RL_Vector3 position, float scale, RL_Color tint);               // Draw a model (with texture if set)
RLAPI void RL_DrawModelEx(RL_Model model, RL_Vector3 position, RL_Vector3 rotationAxis, float rotationAngle, RL_Vector3 scale, RL_Color tint); // Draw a model with extended parameters
RLAPI void RL_DrawModelWires(RL_Model model, RL_Vector3 position, float scale, RL_Color tint);          // Draw a model wires (with texture if set)
RLAPI void RL_DrawModelWiresEx(RL_Model model, RL_Vector3 position, RL_Vector3 rotationAxis, float rotationAngle, RL_Vector3 scale, RL_Color tint); // Draw a model wires (with texture if set) with extended parameters
RLAPI void RL_DrawBoundingBox(RL_BoundingBox box, RL_Color color);                                   // Draw bounding box (wires)
RLAPI void RL_DrawBillboard(RL_Camera camera, RL_Texture2D texture, RL_Vector3 position, float size, RL_Color tint);   // Draw a billboard texture
RLAPI void RL_DrawBillboardRec(RL_Camera camera, RL_Texture2D texture, RL_Rectangle source, RL_Vector3 position, RL_Vector2 size, RL_Color tint); // Draw a billboard texture defined by source
RLAPI void RL_DrawBillboardPro(RL_Camera camera, RL_Texture2D texture, RL_Rectangle source, RL_Vector3 position, RL_Vector3 up, RL_Vector2 size, RL_Vector2 origin, float rotation, RL_Color tint); // Draw a billboard texture defined by source and rotation

// RL_Mesh management functions
RLAPI void RL_UploadMesh(RL_Mesh *mesh, bool dynamic);                                            // Upload mesh vertex data in GPU and provide VAO/VBO ids
RLAPI void RL_UpdateMeshBuffer(RL_Mesh mesh, int index, const void *data, int dataSize, int offset); // Update mesh vertex data in GPU for a specific buffer index
RLAPI void RL_UnloadMesh(RL_Mesh mesh);                                                           // Unload mesh data from CPU and GPU
RLAPI void RL_DrawMesh(RL_Mesh mesh, RL_Material material, RL_Matrix transform);                        // Draw a 3d mesh with material and transform
RLAPI void RL_DrawMeshInstanced(RL_Mesh mesh, RL_Material material, const RL_Matrix *transforms, int instances); // Draw multiple mesh instances with material and different transforms
RLAPI bool RL_ExportMesh(RL_Mesh mesh, const char *fileName);                                     // Export mesh data to file, returns true on success
RLAPI RL_BoundingBox RL_GetMeshBoundingBox(RL_Mesh mesh);                                            // Compute mesh bounding box limits
RLAPI void RL_GenMeshTangents(RL_Mesh *mesh);                                                     // Compute mesh tangents

// RL_Mesh generation functions
RLAPI RL_Mesh RL_GenMeshPoly(int sides, float radius);                                            // Generate polygonal mesh
RLAPI RL_Mesh RL_GenMeshPlane(float width, float length, int resX, int resZ);                     // Generate plane mesh (with subdivisions)
RLAPI RL_Mesh RL_GenMeshCube(float width, float height, float length);                            // Generate cuboid mesh
RLAPI RL_Mesh RL_GenMeshSphere(float radius, int rings, int slices);                              // Generate sphere mesh (standard sphere)
RLAPI RL_Mesh RL_GenMeshHemiSphere(float radius, int rings, int slices);                          // Generate half-sphere mesh (no bottom cap)
RLAPI RL_Mesh RL_GenMeshCylinder(float radius, float height, int slices);                         // Generate cylinder mesh
RLAPI RL_Mesh RL_GenMeshCone(float radius, float height, int slices);                             // Generate cone/pyramid mesh
RLAPI RL_Mesh RL_GenMeshTorus(float radius, float size, int radSeg, int sides);                   // Generate torus mesh
RLAPI RL_Mesh RL_GenMeshKnot(float radius, float size, int radSeg, int sides);                    // Generate trefoil knot mesh
RLAPI RL_Mesh RL_GenMeshHeightmap(RL_Image heightmap, RL_Vector3 size);                                 // Generate heightmap mesh from image data
RLAPI RL_Mesh RL_GenMeshCubicmap(RL_Image cubicmap, RL_Vector3 cubeSize);                               // Generate cubes-based map mesh from image data

// RL_Material loading/unloading functions
RLAPI RL_Material *RL_LoadMaterials(const char *fileName, int *materialCount);                    // Load materials from model file
RLAPI RL_Material RL_LoadMaterialDefault(void);                                                   // Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
RLAPI bool RL_IsMaterialReady(RL_Material material);                                              // Check if a material is ready
RLAPI void RL_UnloadMaterial(RL_Material material);                                               // Unload material from GPU memory (VRAM)
RLAPI void RL_SetMaterialTexture(RL_Material *material, int mapType, RL_Texture2D texture);          // Set texture for a material map type (RL_MATERIAL_MAP_DIFFUSE, RL_MATERIAL_MAP_SPECULAR...)
RLAPI void RL_SetModelMeshMaterial(RL_Model *model, int meshId, int materialId);                  // Set material for a mesh

// RL_Model animations loading/unloading functions
RLAPI RL_ModelAnimation *RL_LoadModelAnimations(const char *fileName, int *animCount);            // Load model animations from file
RLAPI void RL_UpdateModelAnimation(RL_Model model, RL_ModelAnimation anim, int frame);               // Update model animation pose
RLAPI void RL_UnloadModelAnimation(RL_ModelAnimation anim);                                       // Unload animation data
RLAPI void RL_UnloadModelAnimations(RL_ModelAnimation *animations, int animCount);                // Unload animation array data
RLAPI bool RL_IsModelAnimationValid(RL_Model model, RL_ModelAnimation anim);                         // Check model animation skeleton match

// Collision detection functions
RLAPI bool RL_CheckCollisionSpheres(RL_Vector3 center1, float radius1, RL_Vector3 center2, float radius2);   // Check collision between two spheres
RLAPI bool RL_CheckCollisionBoxes(RL_BoundingBox box1, RL_BoundingBox box2);                                 // Check collision between two bounding boxes
RLAPI bool RL_CheckCollisionBoxSphere(RL_BoundingBox box, RL_Vector3 center, float radius);                  // Check collision between box and sphere
RLAPI RL_RayCollision RL_GetRayCollisionSphere(RL_Ray ray, RL_Vector3 center, float radius);                    // Get collision info between ray and sphere
RLAPI RL_RayCollision RL_GetRayCollisionBox(RL_Ray ray, RL_BoundingBox box);                                    // Get collision info between ray and box
RLAPI RL_RayCollision RL_GetRayCollisionMesh(RL_Ray ray, RL_Mesh mesh, RL_Matrix transform);                       // Get collision info between ray and mesh
RLAPI RL_RayCollision RL_GetRayCollisionTriangle(RL_Ray ray, RL_Vector3 p1, RL_Vector3 p2, RL_Vector3 p3);            // Get collision info between ray and triangle
RLAPI RL_RayCollision RL_GetRayCollisionQuad(RL_Ray ray, RL_Vector3 p1, RL_Vector3 p2, RL_Vector3 p3, RL_Vector3 p4);    // Get collision info between ray and quad

//------------------------------------------------------------------------------------
// Audio Loading and Playing Functions (Module: audio)
//------------------------------------------------------------------------------------
typedef void (*AudioCallback)(void *bufferData, unsigned int frames);

// Audio device management functions
RLAPI void RL_InitAudioDevice(void);                                     // Initialize audio device and context
RLAPI void RL_CloseAudioDevice(void);                                    // Close the audio device and context
RLAPI bool RL_IsAudioDeviceReady(void);                                  // Check if audio device has been initialized successfully
RLAPI void RL_SetMasterVolume(float volume);                             // Set master volume (listener)
RLAPI float RL_GetMasterVolume(void);                                    // Get master volume (listener)

// RL_Wave/RL_Sound loading/unloading functions
RLAPI RL_Wave RL_LoadWave(const char *fileName);                            // Load wave data from file
RLAPI RL_Wave RL_LoadWaveFromMemory(const char *fileType, const unsigned char *fileData, int dataSize); // Load wave from memory buffer, fileType refers to extension: i.e. '.wav'
RLAPI bool RL_IsWaveReady(RL_Wave wave);                                    // Checks if wave data is ready
RLAPI RL_Sound RL_LoadSound(const char *fileName);                          // Load sound from file
RLAPI RL_Sound RL_LoadSoundFromWave(RL_Wave wave);                             // Load sound from wave data
RLAPI RL_Sound RL_LoadSoundAlias(RL_Sound source);                             // Create a new sound that shares the same sample data as the source sound, does not own the sound data
RLAPI bool RL_IsSoundReady(RL_Sound sound);                                 // Checks if a sound is ready
RLAPI void RL_UpdateSound(RL_Sound sound, const void *data, int sampleCount); // Update sound buffer with new data
RLAPI void RL_UnloadWave(RL_Wave wave);                                     // Unload wave data
RLAPI void RL_UnloadSound(RL_Sound sound);                                  // Unload sound
RLAPI void RL_UnloadSoundAlias(RL_Sound alias);                             // Unload a sound alias (does not deallocate sample data)
RLAPI bool RL_ExportWave(RL_Wave wave, const char *fileName);               // Export wave data to file, returns true on success
RLAPI bool RL_ExportWaveAsCode(RL_Wave wave, const char *fileName);         // Export wave sample data to code (.h), returns true on success

// RL_Wave/RL_Sound management functions
RLAPI void RL_PlaySound(RL_Sound sound);                                    // Play a sound
RLAPI void RL_StopSound(RL_Sound sound);                                    // Stop playing a sound
RLAPI void RL_PauseSound(RL_Sound sound);                                   // Pause a sound
RLAPI void RL_ResumeSound(RL_Sound sound);                                  // Resume a paused sound
RLAPI bool RL_IsSoundPlaying(RL_Sound sound);                               // Check if a sound is currently playing
RLAPI void RL_SetSoundVolume(RL_Sound sound, float volume);                 // Set volume for a sound (1.0 is max level)
RLAPI void RL_SetSoundPitch(RL_Sound sound, float pitch);                   // Set pitch for a sound (1.0 is base level)
RLAPI void RL_SetSoundPan(RL_Sound sound, float pan);                       // Set pan for a sound (0.5 is center)
RLAPI RL_Wave RL_WaveCopy(RL_Wave wave);                                       // Copy a wave to a new wave
RLAPI void RL_WaveCrop(RL_Wave *wave, int initSample, int finalSample);     // Crop a wave to defined samples range
RLAPI void RL_WaveFormat(RL_Wave *wave, int sampleRate, int sampleSize, int channels); // Convert wave data to desired format
RLAPI float *RL_LoadWaveSamples(RL_Wave wave);                              // Load samples data from wave as a 32bit float data array
RLAPI void RL_UnloadWaveSamples(float *samples);                         // Unload samples data loaded with RL_LoadWaveSamples()

// RL_Music management functions
RLAPI RL_Music RL_LoadMusicStream(const char *fileName);                    // Load music stream from file
RLAPI RL_Music RL_LoadMusicStreamFromMemory(const char *fileType, const unsigned char *data, int dataSize); // Load music stream from data
RLAPI bool RL_IsMusicReady(RL_Music music);                                 // Checks if a music stream is ready
RLAPI void RL_UnloadMusicStream(RL_Music music);                            // Unload music stream
RLAPI void RL_PlayMusicStream(RL_Music music);                              // Start music playing
RLAPI bool RL_IsMusicStreamPlaying(RL_Music music);                         // Check if music is playing
RLAPI void RL_UpdateMusicStream(RL_Music music);                            // Updates buffers for music streaming
RLAPI void RL_StopMusicStream(RL_Music music);                              // Stop music playing
RLAPI void RL_PauseMusicStream(RL_Music music);                             // Pause music playing
RLAPI void RL_ResumeMusicStream(RL_Music music);                            // Resume playing paused music
RLAPI void RL_SeekMusicStream(RL_Music music, float position);              // Seek music to a position (in seconds)
RLAPI void RL_SetMusicVolume(RL_Music music, float volume);                 // Set volume for music (1.0 is max level)
RLAPI void RL_SetMusicPitch(RL_Music music, float pitch);                   // Set pitch for a music (1.0 is base level)
RLAPI void RL_SetMusicPan(RL_Music music, float pan);                       // Set pan for a music (0.5 is center)
RLAPI float RL_GetMusicTimeLength(RL_Music music);                          // Get music time length (in seconds)
RLAPI float RL_GetMusicTimePlayed(RL_Music music);                          // Get current music time played (in seconds)

// RL_AudioStream management functions
RLAPI RL_AudioStream RL_LoadAudioStream(unsigned int sampleRate, unsigned int sampleSize, unsigned int channels); // Load audio stream (to stream raw audio pcm data)
RLAPI bool RL_IsAudioStreamReady(RL_AudioStream stream);                    // Checks if an audio stream is ready
RLAPI void RL_UnloadAudioStream(RL_AudioStream stream);                     // Unload audio stream and free memory
RLAPI void RL_UpdateAudioStream(RL_AudioStream stream, const void *data, int frameCount); // Update audio stream buffers with data
RLAPI bool RL_IsAudioStreamProcessed(RL_AudioStream stream);                // Check if any audio stream buffers requires refill
RLAPI void RL_PlayAudioStream(RL_AudioStream stream);                       // Play audio stream
RLAPI void RL_PauseAudioStream(RL_AudioStream stream);                      // Pause audio stream
RLAPI void RL_ResumeAudioStream(RL_AudioStream stream);                     // Resume audio stream
RLAPI bool RL_IsAudioStreamPlaying(RL_AudioStream stream);                  // Check if audio stream is playing
RLAPI void RL_StopAudioStream(RL_AudioStream stream);                       // Stop audio stream
RLAPI void RL_SetAudioStreamVolume(RL_AudioStream stream, float volume);    // Set volume for audio stream (1.0 is max level)
RLAPI void RL_SetAudioStreamPitch(RL_AudioStream stream, float pitch);      // Set pitch for audio stream (1.0 is base level)
RLAPI void RL_SetAudioStreamPan(RL_AudioStream stream, float pan);          // Set pan for audio stream (0.5 is centered)
RLAPI void RL_SetAudioStreamBufferSizeDefault(int size);                 // Default size for new audio streams
RLAPI void RL_SetAudioStreamCallback(RL_AudioStream stream, AudioCallback callback); // Audio thread callback to request new data

RLAPI void RL_AttachAudioStreamProcessor(RL_AudioStream stream, AudioCallback processor); // Attach audio stream processor to stream, receives the samples as <float>s
RLAPI void RL_DetachAudioStreamProcessor(RL_AudioStream stream, AudioCallback processor); // Detach audio stream processor from stream

RLAPI void RL_AttachAudioMixedProcessor(AudioCallback processor); // Attach audio stream processor to the entire audio pipeline, receives the samples as <float>s
RLAPI void RL_DetachAudioMixedProcessor(AudioCallback processor); // Detach audio stream processor from the entire audio pipeline

#if defined(__cplusplus)
}
#endif

#endif // RAYLIB_H
