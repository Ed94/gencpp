/**********************************************************************************************
 *
 *   raylib v4.6-dev - A simple and easy-to-use library to enjoy videogames programming (www.raylib.com)
 *
 *   FEATURES:
 *       - NO external dependencies, all required libraries included with raylib
 *       - Multiplatform: Windows, Linux, FreeBSD, OpenBSD, NetBSD, DragonFly,
 *                        MacOS, Haiku, Android, Raspberry Pi, DRM native, HTML5.
 *       - Written in plain C code (C99) in PascalCase/camelCase notation
 *       - Hardware accelerated with OpenGL (1.1, 2.1, 3.3, 4.3 or ES2 - choose at compile)
 *       - Unique OpenGL abstraction layer (usable as standalone module): [rlgl]
 *       - Multiple Fonts formats supported (TTF, XNA fonts, AngelCode fonts)
 *       - Outstanding texture formats support, including compressed formats (DXT, ETC, ASTC)
 *       - Full 3d support for 3d Shapes, Models, Billboards, Heightmaps and more!
 *       - Flexible Materials system, supporting classic maps and PBR maps
 *       - Animated 3D models supported (skeletal bones animation) (IQM)
 *       - Shaders support, including Model shaders and Postprocessing shaders
 *       - Powerful math module for Vector, Matrix and Quaternion operations: [raymath]
 *       - Audio loading and playing with streaming support (WAV, OGG, MP3, FLAC, XM, MOD)
 *       - VR stereo rendering with configurable HMD device parameters
 *       - Bindings to multiple programming languages available!
 *
 *   NOTES:
 *       - One default Font is loaded on InitWindow()->LoadFontDefault() [core, text]
 *       - One default Texture2D is loaded on rlglInit(), 1x1 white pixel R8G8B8A8 [rlgl] (OpenGL 3.3 or ES2)
 *       - One default Shader is loaded on rlglInit()->rlLoadShaderDefault() [rlgl] (OpenGL 3.3 or ES2)
 *       - One default RenderBatch is loaded on rlglInit()->rlLoadRenderBatch() [rlgl] (OpenGL 3.3 or ES2)
 *
 *   DEPENDENCIES (included):
 *       [rcore] rglfw (Camilla Löwy - github.com/glfw/glfw) for window/context management and input (PLATFORM_DESKTOP)
 *       [rlgl] glad (David Herberth - github.com/Dav1dde/glad) for OpenGL 3.3 extensions loading (PLATFORM_DESKTOP)
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

#include <stdarg.h>
// Required for: va_list - Only used by TraceLogCallback


// Function specifiers in case library is build/used as a shared library (Windows)
// NOTE: Microsoft specifiers to tell compiler that symbols are imported/exported from a .dll
#if defined( _WIN32 )
#if defined( BUILD_LIBTYPE_SHARED )
#if defined( __TINYC__ )
#define RL___declspec( x ) __attribute__( ( x ) )
#endif
#elif defined( USE_LIBTYPE_SHARED )
#endif
#endif

#ifndef RLAPI
#endif

//----------------------------------------------------------------------------------
// Some basic Defines
//----------------------------------------------------------------------------------
#ifndef PI
#define RL_PI 3.14159265358979323846f
#endif
#ifndef DEG2RAD
#define RL_DEG2RAD ( PI / 180.0f )
#endif
#ifndef RAD2DEG
#define RL_RAD2DEG ( 180.0f / PI )
#endif

// Allow custom memory allocators
// NOTE: Require recompiling raylib sources
#ifndef RL_MALLOC
#endif
#ifndef RL_CALLOC
#endif
#ifndef RL_REALLOC
#endif
#ifndef RL_FREE
#endif

// NOTE: MSVC C++ compiler does not support compound literals (C99 feature)
// Plain structures in C++ (without constructors) can be initialized with { }
// This is called aggregate initialization (C++11 feature)
#if defined( __cplusplus )
#define RL_CLITERAL( type ) type
#else
#define RL_CLITERAL( type ) ( type )
#endif

// Some compilers (mostly macos clang) default to C++98,
// where aggregate initialization can't be used
// So, give a more clear error stating how to fix this
#if ! defined( _MSC_VER ) && ( defined( __cplusplus ) && __cplusplus < 201103L )
#error "C++11 or later is required. Add -std=c++11"
#endif

// NOTE: We set some defines with some data types declared by raylib
// Other modules (raymath, rlgl) also require some of those types, so,
// to be able to use those other modules as standalone (not depending on raylib)
// this defines are very useful for internal check and avoid type (re)definitions

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define RL_LIGHTGRAY       \
	CLITERAL( Color )      \
	{                      \
		200, 200, 200, 255 \
	}    // Light Gray
#define RL_GRAY            \
	CLITERAL( Color )      \
	{                      \
		130, 130, 130, 255 \
	}    // Gray
#define RL_DARKGRAY     \
	CLITERAL( Color )   \
	{                   \
		80, 80, 80, 255 \
	}    // Dark Gray
#define RL_YELLOW        \
	CLITERAL( Color )    \
	{                    \
		253, 249, 0, 255 \
	}    // Yellow
#define RL_GOLD          \
	CLITERAL( Color )    \
	{                    \
		255, 203, 0, 255 \
	}    // Gold
#define RL_ORANGE        \
	CLITERAL( Color )    \
	{                    \
		255, 161, 0, 255 \
	}    // Orange
#define RL_PINK            \
	CLITERAL( Color )      \
	{                      \
		255, 109, 194, 255 \
	}    // Pink
#define RL_RED           \
	CLITERAL( Color )    \
	{                    \
		230, 41, 55, 255 \
	}    // Red
#define RL_MAROON        \
	CLITERAL( Color )    \
	{                    \
		190, 33, 55, 255 \
	}    // Maroon
#define RL_GREEN        \
	CLITERAL( Color )   \
	{                   \
		0, 228, 48, 255 \
	}    // Green
#define RL_LIME         \
	CLITERAL( Color )   \
	{                   \
		0, 158, 47, 255 \
	}    // Lime
#define RL_DARKGREEN    \
	CLITERAL( Color )   \
	{                   \
		0, 117, 44, 255 \
	}    // Dark Green
#define RL_SKYBLUE         \
	CLITERAL( Color )      \
	{                      \
		102, 191, 255, 255 \
	}    // Sky Blue
#define RL_BLUE          \
	CLITERAL( Color )    \
	{                    \
		0, 121, 241, 255 \
	}    // Blue
#define RL_DARKBLUE     \
	CLITERAL( Color )   \
	{                   \
		0, 82, 172, 255 \
	}    // Dark Blue
#define RL_PURPLE          \
	CLITERAL( Color )      \
	{                      \
		200, 122, 255, 255 \
	}    // Purple
#define RL_VIOLET         \
	CLITERAL( Color )     \
	{                     \
		135, 60, 190, 255 \
	}    // Violet
#define RL_DARKPURPLE     \
	CLITERAL( Color )     \
	{                     \
		112, 31, 126, 255 \
	}    // Dark Purple
#define RL_BEIGE           \
	CLITERAL( Color )      \
	{                      \
		211, 176, 131, 255 \
	}    // Beige
#define RL_BROWN          \
	CLITERAL( Color )     \
	{                     \
		127, 106, 79, 255 \
	}    // Brown
#define RL_DARKBROWN    \
	CLITERAL( Color )   \
	{                   \
		76, 63, 47, 255 \
	}    // Dark Brown

#define RL_WHITE           \
	CLITERAL( Color )      \
	{                      \
		255, 255, 255, 255 \
	}    // White
#define RL_BLACK      \
	CLITERAL( Color ) \
	{                 \
		0, 0, 0, 255  \
	}    // Black
#define RL_BLANK      \
	CLITERAL( Color ) \
	{                 \
		0, 0, 0, 0    \
	}    // Blank (Transparent)
#define RL_MAGENTA       \
	CLITERAL( Color )    \
	{                    \
		255, 0, 255, 255 \
	}    // Magenta
#define RL_RAYWHITE        \
	CLITERAL( Color )      \
	{                      \
		245, 245, 245, 255 \
	}    // My own White (raylib logo)

//----------------------------------------------------------------------------------
// Structures Definition
//----------------------------------------------------------------------------------
// Boolean type
#if ( defined( __STDC__ ) && __STDC_VERSION__ >= 199901L ) || ( defined( _MSC_VER ) && _MSC_VER >= 1800 )
#include <stdbool.h>
#elif ! defined( __cplusplus ) && ! defined( bool )
typedef enum bool
{
	False = 0,
	True  = ! false
} bool;
#endif

// Vector2, 2 components
typedef struct Vector2
{
	f32 x;
	f32 y;

} Vector2;

// Vector3, 3 components
typedef struct Vector3
{
	f32 x;
	f32 y;
	f32 z;

} Vector3;

// Vector4, 4 components
typedef struct Vector4
{
	f32 x;
	f32 y;
	f32 z;
	f32 w;

} Vector4;

// Quaternion, 4 components (Vector4 alias)
typedef Vector4 Quaternion;

// Matrix, 4x4 components, column major, OpenGL style, right-handed
typedef struct Matrix
{
	f32 m0, m4, m8, m12;
	f32 m1, m5, m9, m13;
	f32 m2, m6, m10, m14;
	f32 m3, m7, m11, m15;

} Matrix;

// Color, 4 components, R8G8B8A8 (32bit)
typedef struct Color
{
	u8 r;
	u8 g;
	u8 b;
	u8 a;

} Color;

// Rectangle, 4 components
typedef struct Rectangle
{
	f32 x;
	f32 y;
	f32 width;
	f32 height;

} Rectangle;

// Image, pixel data stored in CPU memory (RAM)
typedef struct Image
{
	void* data;
	s32   width;
	s32   height;
	s32   mipmaps;
	s32   format;

} Image;

// Texture, tex data stored in GPU memory (VRAM)
typedef struct Texture
{
	u32 id;
	s32 width;
	s32 height;
	s32 mipmaps;
	s32 format;

} Texture;

// Texture2D, same as Texture
typedef Texture Texture2D;

// TextureCubemap, same as Texture
typedef Texture TextureCubemap;

// RenderTexture, fbo for texture rendering
typedef struct RenderTexture
{
	u32     id;
	Texture texture;
	Texture depth;

} RenderTexture;

// RenderTexture2D, same as RenderTexture
typedef RenderTexture RenderTexture2D;

// NPatchInfo, n-patch layout info
typedef struct NPatchInfo
{
	Rectangle source;
	s32       left;
	s32       top;
	s32       right;
	s32       bottom;
	s32       layout;

} NPatchInfo;

// GlyphInfo, font characters glyphs info
typedef struct GlyphInfo
{
	s32   value;
	s32   offsetX;
	s32   offsetY;
	s32   advanceX;
	Image image;

} GlyphInfo;

// Font, font texture and GlyphInfo array data
typedef struct Font
{
	s32        baseSize;
	s32        glyphCount;
	s32        glyphPadding;
	Texture2D  texture;
	Rectangle* recs;
	GlyphInfo* glyphs;

} Font;

// Camera, defines position/orientation in 3d space
typedef struct Camera3D
{
	Vector3 position;
	Vector3 target;
	Vector3 up;
	f32     fovy;
	s32     projection;

} Camera3D;

typedef Camera3D Camera;    // Camera type fallback, defaults to Camera3D

// Camera2D, defines position/orientation in 2d space
typedef struct Camera2D
{
	Vector2 offset;
	Vector2 target;
	f32     rotation;
	f32     zoom;

} Camera2D;

// Mesh, vertex data and vao/vbo
typedef struct Mesh
{
	s32 vertexCount;
	s32 triangleCount;

	// Vertex attributes data
	f32* vertices;
	f32* texcoords;
	f32* texcoords2;
	f32* normals;
	f32* tangents;
	u8*  colors;
	u8*  indices;

	// Animation vertex data
	f32* animVertices;
	f32* animNormals;
	u8*  boneIds;
	f32* boneWeights;

	// OpenGL identifiers
	u32           vaoId;
	unsigned int* vboId;

} Mesh;

// Shader
typedef struct Shader
{
	u32  id;
	s32* locs;

} Shader;

// MaterialMap
typedef struct MaterialMap
{
	Texture2D texture;
	Color     color;
	f32       value;

} MaterialMap;

// Material, includes shader and maps
typedef struct Material
{
	Shader       shader;
	MaterialMap* maps;
	f32          params;

} Material;

// Transform, vertex transformation data
typedef struct Transform
{
	Vector3    translation;
	Quaternion rotation;
	Vector3    scale;

} Transform;

// Bone, skeletal animation bone
typedef struct BoneInfo
{
	char name[ 32 ];
	s32  parent;

} BoneInfo;

// Model, meshes, materials and animation data
typedef struct Model
{
	Matrix transform;

	s32       meshCount;
	s32       materialCount;
	Mesh*     meshes;
	Material* materials;
	s32*      meshMaterial;

	// Animation data
	s32        boneCount;
	BoneInfo*  bones;
	Transform* bindPose;

} Model;

// ModelAnimation
typedef struct ModelAnimation
{
	s32         boneCount;
	s32         frameCount;
	BoneInfo*   bones;
	Transform** framePoses;
	char        name[ 32 ];

} ModelAnimation;

// Ray, ray for raycasting
typedef struct Ray
{
	Vector3 position;
	Vector3 direction;

} Ray;

// RayCollision, ray hit information
typedef struct RayCollision
{
	bool    hit;
	f32     distance;
	Vector3 point;
	Vector3 normal;

} RayCollision;

// BoundingBox
typedef struct BoundingBox
{
	Vector3 min;
	Vector3 max;

} BoundingBox;

// Wave, audio wave data
typedef struct Wave
{
	u32   frameCount;
	u32   sampleRate;
	u32   sampleSize;
	u32   channels;
	void* data;

} Wave;

// Opaque structs declaration
// NOTE: Actual structs are defined internally in raudio module
typedef struct rAudioBuffer    rAudioBuffer;
typedef struct rAudioProcessor rAudioProcessor;

// AudioStream, custom audio stream
typedef struct AudioStream
{
	rAudioBuffer*    buffer;
	rAudioProcessor* processor;

	u32 sampleRate;
	u32 sampleSize;
	u32 channels;

} AudioStream;

// Sound
typedef struct Sound
{
	AudioStream stream;
	u32         frameCount;

} Sound;

// Music, audio stream, anything longer than ~10 seconds should be streamed
typedef struct Music
{
	AudioStream stream;
	u32         frameCount;
	bool        looping;

	s32   ctxType;
	void* ctxData;

} Music;

// VrDeviceInfo, Head-Mounted-Display device parameters
typedef struct VrDeviceInfo
{
	s32 hResolution;
	s32 vResolution;
	f32 hScreenSize;
	f32 vScreenSize;
	f32 vScreenCenter;
	f32 eyeToScreenDistance;
	f32 lensSeparationDistance;
	f32 interpupillaryDistance;
	f32 lensDistortionValues;
	f32 chromaAbCorrection;

} VrDeviceInfo;

// VrStereoConfig, VR stereo rendering configuration for simulator
typedef struct VrStereoConfig
{
	Matrix projection[ 2 ];
	Matrix viewOffset[ 2 ];
	f32    leftLensCenter;
	f32    rightLensCenter;
	f32    leftScreenCenter;
	f32    rightScreenCenter;
	f32    scale;
	f32    scaleIn;

} VrStereoConfig;

// File path list
typedef struct FilePathList
{
	u32    capacity;
	u32    count;
	char** paths;

} FilePathList;

//----------------------------------------------------------------------------------
// Enumerators Definition
//----------------------------------------------------------------------------------
// System/Window config flags
// NOTE: Every bit registers one state (use it with bit masks)
// By default all flags are set to 0
typedef enum
{
	Flag_Vsync_Hint               = 0x00000040,    // Set to try enabling V-Sync on GPU
	Flag_Fullscreen_Mode          = 0x00000002,    // Set to run program in fullscreen
	Flag_Window_Resizable         = 0x00000004,    // Set to allow resizable window
	Flag_Window_Undecorated       = 0x00000008,    // Set to disable window decoration (frame and buttons)
	Flag_Window_Hidden            = 0x00000080,    // Set to hide window
	Flag_Window_Minimized         = 0x00000200,    // Set to minimize window (iconify)
	Flag_Window_Maximized         = 0x00000400,    // Set to maximize window (expanded to monitor)
	Flag_Window_Unfocused         = 0x00000800,    // Set to window non focused
	Flag_Window_Topmost           = 0x00001000,    // Set to window always on top
	Flag_Window_Always_Run        = 0x00000100,    // Set to allow windows running while minimized
	Flag_Window_Transparent       = 0x00000010,    // Set to allow transparent framebuffer
	Flag_Window_Highdpi           = 0x00002000,    // Set to support HighDPI
	Flag_Window_Mouse_Passthrough = 0x00004000,    // Set to support mouse passthrough, only supported when FLAG_WINDOW_UNDECORATED
	Flag_Borderless_Windowed_Mode = 0x00008000,    // Set to run program in borderless windowed mode
	Flag_Msaa_4x_Hint             = 0x00000020,    // Set to try enabling MSAA 4X
	Flag_Interlaced_Hint          = 0x00010000     // set to try enabling interlaced video format (for v3d)

} ConfigFlags;

// Trace log level
// NOTE: Organized by priority level
typedef enum
{
	Log_All = 0,    // Display all logs
	Log_Trace,      // Trace logging, intended for internal use only
	Log_Debug,      // Debug logging, used for internal debugging, it should be disabled on release builds
	Log_Info,       // Info logging, used for program execution info
	Log_Warning,    // Warning logging, used on recoverable failures
	Log_Error,      // Error logging, used on unrecoverable failures
	Log_Fatal,      // Fatal logging, used to abort program: exit(EXIT_FAILURE)
	Log_None        // disable logging

} TraceLogLevel;

// Keyboard keys (US keyboard layout)
// NOTE: Use GetKeyPressed() to allow redefining
// required keys for alternative layouts
typedef enum
{
	Key_Null = 0,    // Key: NULL, used for no key pressed
	// Alphanumeric keys
	Key_Apostrophe    = 39,    // Key: '
	Key_Comma         = 44,    // Key: ,
	Key_Minus         = 45,    // Key: -
	Key_Period        = 46,    // Key: .
	Key_Slash         = 47,    // Key: /
	Key_Zero          = 48,    // Key: 0
	Key_One           = 49,    // Key: 1
	Key_Two           = 50,    // Key: 2
	Key_Three         = 51,    // Key: 3
	Key_Four          = 52,    // Key: 4
	Key_Five          = 53,    // Key: 5
	Key_Six           = 54,    // Key: 6
	Key_Seven         = 55,    // Key: 7
	Key_Eight         = 56,    // Key: 8
	Key_Nine          = 57,    // Key: 9
	Key_Semicolon     = 59,    // Key: ;
	Key_Equal         = 61,    // Key: =
	Key_A             = 65,    // Key: A | a
	Key_B             = 66,    // Key: B | b
	Key_C             = 67,    // Key: C | c
	Key_D             = 68,    // Key: D | d
	Key_E             = 69,    // Key: E | e
	Key_F             = 70,    // Key: F | f
	Key_G             = 71,    // Key: G | g
	Key_H             = 72,    // Key: H | h
	Key_I             = 73,    // Key: I | i
	Key_J             = 74,    // Key: J | j
	Key_K             = 75,    // Key: K | k
	Key_L             = 76,    // Key: L | l
	Key_M             = 77,    // Key: M | m
	Key_N             = 78,    // Key: N | n
	Key_O             = 79,    // Key: O | o
	Key_P             = 80,    // Key: P | p
	Key_Q             = 81,    // Key: Q | q
	Key_R             = 82,    // Key: R | r
	Key_S             = 83,    // Key: S | s
	Key_T             = 84,    // Key: T | t
	Key_U             = 85,    // Key: U | u
	Key_V             = 86,    // Key: V | v
	Key_W             = 87,    // Key: W | w
	Key_X             = 88,    // Key: X | x
	Key_Y             = 89,    // Key: Y | y
	Key_Z             = 90,    // Key: Z | z
	Key_Left_Bracket  = 91,    // Key: [
	Key_Backslash     = 92,    // Key: '\'
	Key_Right_Bracket = 93,    // Key: ]
	Key_Grave         = 96,    // Key: `
	// Function keys
	Key_Space         = 32,     // Key: Space
	Key_Escape        = 256,    // Key: Esc
	Key_Enter         = 257,    // Key: Enter
	Key_Tab           = 258,    // Key: Tab
	Key_Backspace     = 259,    // Key: Backspace
	Key_Insert        = 260,    // Key: Ins
	Key_Delete        = 261,    // Key: Del
	Key_Right         = 262,    // Key: Cursor right
	Key_Left          = 263,    // Key: Cursor left
	Key_Down          = 264,    // Key: Cursor down
	Key_Up            = 265,    // Key: Cursor up
	Key_Page_Up       = 266,    // Key: Page up
	Key_Page_Down     = 267,    // Key: Page down
	Key_Home          = 268,    // Key: Home
	Key_End           = 269,    // Key: End
	Key_Caps_Lock     = 280,    // Key: Caps lock
	Key_Scroll_Lock   = 281,    // Key: Scroll down
	Key_Num_Lock      = 282,    // Key: Num lock
	Key_Print_Screen  = 283,    // Key: Print screen
	Key_Pause         = 284,    // Key: Pause
	Key_F1            = 290,    // Key: F1
	Key_F2            = 291,    // Key: F2
	Key_F3            = 292,    // Key: F3
	Key_F4            = 293,    // Key: F4
	Key_F5            = 294,    // Key: F5
	Key_F6            = 295,    // Key: F6
	Key_F7            = 296,    // Key: F7
	Key_F8            = 297,    // Key: F8
	Key_F9            = 298,    // Key: F9
	Key_F10           = 299,    // Key: F10
	Key_F11           = 300,    // Key: F11
	Key_F12           = 301,    // Key: F12
	Key_Left_Shift    = 340,    // Key: Shift left
	Key_Left_Control  = 341,    // Key: Control left
	Key_Left_Alt      = 342,    // Key: Alt left
	Key_Left_Super    = 343,    // Key: Super left
	Key_Right_Shift   = 344,    // Key: Shift right
	Key_Right_Control = 345,    // Key: Control right
	Key_Right_Alt     = 346,    // Key: Alt right
	Key_Right_Super   = 347,    // Key: Super right
	Key_Kb_Menu       = 348,    // Key: KB menu
	// Keypad keys
	Key_Kp_0        = 320,    // Key: Keypad 0
	Key_Kp_1        = 321,    // Key: Keypad 1
	Key_Kp_2        = 322,    // Key: Keypad 2
	Key_Kp_3        = 323,    // Key: Keypad 3
	Key_Kp_4        = 324,    // Key: Keypad 4
	Key_Kp_5        = 325,    // Key: Keypad 5
	Key_Kp_6        = 326,    // Key: Keypad 6
	Key_Kp_7        = 327,    // Key: Keypad 7
	Key_Kp_8        = 328,    // Key: Keypad 8
	Key_Kp_9        = 329,    // Key: Keypad 9
	Key_Kp_Decimal  = 330,    // Key: Keypad .
	Key_Kp_Divide   = 331,    // Key: Keypad /
	Key_Kp_Multiply = 332,    // Key: Keypad *
	Key_Kp_Subtract = 333,    // Key: Keypad -
	Key_Kp_Add      = 334,    // Key: Keypad +
	Key_Kp_Enter    = 335,    // Key: Keypad Enter
	Key_Kp_Equal    = 336,    // Key: Keypad =
	// Android key buttons
	Key_Back        = 4,     // Key: Android back button
	Key_Menu        = 82,    // Key: Android menu button
	Key_Volume_Up   = 24,    // Key: Android volume up button
	Key_Volume_Down = 25     // key: android volume down button

} KeyboardKey;

// Add backwards compatibility support for deprecated names
#define RL_MOUSE_LEFT_BUTTON   MOUSE_BUTTON_LEFT
#define RL_MOUSE_RIGHT_BUTTON  MOUSE_BUTTON_RIGHT
#define RL_MOUSE_MIDDLE_BUTTON MOUSE_BUTTON_MIDDLE

// Mouse buttons
typedef enum
{
	Mouse_Button_Left    = 0,    // Mouse button left
	Mouse_Button_Right   = 1,    // Mouse button right
	Mouse_Button_Middle  = 2,    // Mouse button middle (pressed wheel)
	Mouse_Button_Side    = 3,    // Mouse button side (advanced mouse device)
	Mouse_Button_Extra   = 4,    // Mouse button extra (advanced mouse device)
	Mouse_Button_Forward = 5,    // Mouse button forward (advanced mouse device)
	Mouse_Button_Back    = 6,    // Mouse button back (advanced mouse device)

} MouseButton;

// Mouse cursor
typedef enum
{
	Mouse_Cursor_Default       = 0,    // Default pointer shape
	Mouse_Cursor_Arrow         = 1,    // Arrow shape
	Mouse_Cursor_Ibeam         = 2,    // Text writing cursor shape
	Mouse_Cursor_Crosshair     = 3,    // Cross shape
	Mouse_Cursor_Pointing_Hand = 4,    // Pointing hand cursor
	Mouse_Cursor_Resize_Ew     = 5,    // Horizontal resize/move arrow shape
	Mouse_Cursor_Resize_Ns     = 6,    // Vertical resize/move arrow shape
	Mouse_Cursor_Resize_Nwse   = 7,    // Top-left to bottom-right diagonal resize/move arrow shape
	Mouse_Cursor_Resize_Nesw   = 8,    // The top-right to bottom-left diagonal resize/move arrow shape
	Mouse_Cursor_Resize_All    = 9,    // The omnidirectional resize/move cursor shape
	Mouse_Cursor_Not_Allowed   = 10    // the operation-not-allowed shape

} MouseCursor;

// Gamepad buttons
typedef enum
{
	Gamepad_Button_Unknown = 0,         // Unknown button, just for error checking
	Gamepad_Button_Left_Face_Up,        // Gamepad left DPAD up button
	Gamepad_Button_Left_Face_Right,     // Gamepad left DPAD right button
	Gamepad_Button_Left_Face_Down,      // Gamepad left DPAD down button
	Gamepad_Button_Left_Face_Left,      // Gamepad left DPAD left button
	Gamepad_Button_Right_Face_Up,       // Gamepad right button up (i.e. PS3: Triangle, Xbox: Y)
	Gamepad_Button_Right_Face_Right,    // Gamepad right button right (i.e. PS3: Square, Xbox: X)
	Gamepad_Button_Right_Face_Down,     // Gamepad right button down (i.e. PS3: Cross, Xbox: A)
	Gamepad_Button_Right_Face_Left,     // Gamepad right button left (i.e. PS3: Circle, Xbox: B)
	Gamepad_Button_Left_Trigger_1,      // Gamepad top/back trigger left (first), it could be a trailing button
	Gamepad_Button_Left_Trigger_2,      // Gamepad top/back trigger left (second), it could be a trailing button
	Gamepad_Button_Right_Trigger_1,     // Gamepad top/back trigger right (one), it could be a trailing button
	Gamepad_Button_Right_Trigger_2,     // Gamepad top/back trigger right (second), it could be a trailing button
	Gamepad_Button_Middle_Left,         // Gamepad center buttons, left one (i.e. PS3: Select)
	Gamepad_Button_Middle,              // Gamepad center buttons, middle one (i.e. PS3: PS, Xbox: XBOX)
	Gamepad_Button_Middle_Right,        // Gamepad center buttons, right one (i.e. PS3: Start)
	Gamepad_Button_Left_Thumb,          // Gamepad joystick pressed button left
	Gamepad_Button_Right_Thumb          // gamepad joystick pressed button right

} GamepadButton;

// Gamepad axis
typedef enum
{
	Gamepad_Axis_Left_X        = 0,    // Gamepad left stick X axis
	Gamepad_Axis_Left_Y        = 1,    // Gamepad left stick Y axis
	Gamepad_Axis_Right_X       = 2,    // Gamepad right stick X axis
	Gamepad_Axis_Right_Y       = 3,    // Gamepad right stick Y axis
	Gamepad_Axis_Left_Trigger  = 4,    // Gamepad back trigger left, pressure level: [1..-1]
	Gamepad_Axis_Right_Trigger = 5     // gamepad back trigger right, pressure level: [1..-1]

} GamepadAxis;

// Material map index
typedef enum
{
	Material_Map_Albedo = 0,    // Albedo material (same as: MATERIAL_MAP_DIFFUSE)
	Material_Map_Metalness,     // Metalness material (same as: MATERIAL_MAP_SPECULAR)
	Material_Map_Normal,        // Normal material
	Material_Map_Roughness,     // Roughness material
	Material_Map_Occlusion,     // Ambient occlusion material
	Material_Map_Emission,      // Emission material
	Material_Map_Height,        // Heightmap material
	Material_Map_Cubemap,       // Cubemap material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
	Material_Map_Irradiance,    // Irradiance material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
	Material_Map_Prefilter,     // Prefilter material (NOTE: Uses GL_TEXTURE_CUBE_MAP)
	Material_Map_Brdf           // brdf material

} MaterialMapIndex;

#define RL_MATERIAL_MAP_DIFFUSE  MATERIAL_MAP_ALBEDO
#define RL_MATERIAL_MAP_SPECULAR MATERIAL_MAP_METALNESS

// Shader location index
typedef enum
{
	Shader_Loc_Vertex_Position = 0,    // Shader location: vertex attribute: position
	Shader_Loc_Vertex_Texcoord01,      // Shader location: vertex attribute: texcoord01
	Shader_Loc_Vertex_Texcoord02,      // Shader location: vertex attribute: texcoord02
	Shader_Loc_Vertex_Normal,          // Shader location: vertex attribute: normal
	Shader_Loc_Vertex_Tangent,         // Shader location: vertex attribute: tangent
	Shader_Loc_Vertex_Color,           // Shader location: vertex attribute: color
	Shader_Loc_Matrix_Mvp,             // Shader location: matrix uniform: model-view-projection
	Shader_Loc_Matrix_View,            // Shader location: matrix uniform: view (camera transform)
	Shader_Loc_Matrix_Projection,      // Shader location: matrix uniform: projection
	Shader_Loc_Matrix_Model,           // Shader location: matrix uniform: model (transform)
	Shader_Loc_Matrix_Normal,          // Shader location: matrix uniform: normal
	Shader_Loc_Vector_View,            // Shader location: vector uniform: view
	Shader_Loc_Color_Diffuse,          // Shader location: vector uniform: diffuse color
	Shader_Loc_Color_Specular,         // Shader location: vector uniform: specular color
	Shader_Loc_Color_Ambient,          // Shader location: vector uniform: ambient color
	Shader_Loc_Map_Albedo,             // Shader location: sampler2d texture: albedo (same as: SHADER_LOC_MAP_DIFFUSE)
	Shader_Loc_Map_Metalness,          // Shader location: sampler2d texture: metalness (same as: SHADER_LOC_MAP_SPECULAR)
	Shader_Loc_Map_Normal,             // Shader location: sampler2d texture: normal
	Shader_Loc_Map_Roughness,          // Shader location: sampler2d texture: roughness
	Shader_Loc_Map_Occlusion,          // Shader location: sampler2d texture: occlusion
	Shader_Loc_Map_Emission,           // Shader location: sampler2d texture: emission
	Shader_Loc_Map_Height,             // Shader location: sampler2d texture: height
	Shader_Loc_Map_Cubemap,            // Shader location: samplerCube texture: cubemap
	Shader_Loc_Map_Irradiance,         // Shader location: samplerCube texture: irradiance
	Shader_Loc_Map_Prefilter,          // Shader location: samplerCube texture: prefilter
	Shader_Loc_Map_Brdf                // shader location: sampler2d texture: brdf

} ShaderLocationIndex;

#define RL_SHADER_LOC_MAP_DIFFUSE  SHADER_LOC_MAP_ALBEDO
#define RL_SHADER_LOC_MAP_SPECULAR SHADER_LOC_MAP_METALNESS

// Shader uniform data type
typedef enum
{
	Shader_Uniform_Float = 0,    // Shader uniform type: float
	Shader_Uniform_Vec2,         // Shader uniform type: vec2 (2 float)
	Shader_Uniform_Vec3,         // Shader uniform type: vec3 (3 float)
	Shader_Uniform_Vec4,         // Shader uniform type: vec4 (4 float)
	Shader_Uniform_Int,          // Shader uniform type: int
	Shader_Uniform_Ivec2,        // Shader uniform type: ivec2 (2 int)
	Shader_Uniform_Ivec3,        // Shader uniform type: ivec3 (3 int)
	Shader_Uniform_Ivec4,        // Shader uniform type: ivec4 (4 int)
	Shader_Uniform_Sampler2d     // shader uniform type: sampler2d

} ShaderUniformDataType;

// Shader attribute data types
typedef enum
{
	Shader_Attrib_Float = 0,    // Shader attribute type: float
	Shader_Attrib_Vec2,         // Shader attribute type: vec2 (2 float)
	Shader_Attrib_Vec3,         // Shader attribute type: vec3 (3 float)
	Shader_Attrib_Vec4          // shader attribute type: vec4 (4 float)

} ShaderAttributeDataType;

// Pixel formats
// NOTE: Support depends on OpenGL version and platform
typedef enum
{
	Pixelformat_Uncompressed_Grayscale = 1,    // 8 bit per pixel (no alpha)
	Pixelformat_Uncompressed_Gray_Alpha,       // 8*2 bpp (2 channels)
	Pixelformat_Uncompressed_R5g6b5,           // 16 bpp
	Pixelformat_Uncompressed_R8g8b8,           // 24 bpp
	Pixelformat_Uncompressed_R5g5b5a1,         // 16 bpp (1 bit alpha)
	Pixelformat_Uncompressed_R4g4b4a4,         // 16 bpp (4 bit alpha)
	Pixelformat_Uncompressed_R8g8b8a8,         // 32 bpp
	Pixelformat_Uncompressed_R32,              // 32 bpp (1 channel - float)
	Pixelformat_Uncompressed_R32g32b32,        // 32*3 bpp (3 channels - float)
	Pixelformat_Uncompressed_R32g32b32a32,     // 32*4 bpp (4 channels - float)
	Pixelformat_Uncompressed_R16,              // 16 bpp (1 channel - half float)
	Pixelformat_Uncompressed_R16g16b16,        // 16*3 bpp (3 channels - half float)
	Pixelformat_Uncompressed_R16g16b16a16,     // 16*4 bpp (4 channels - half float)
	Pixelformat_Compressed_Dxt1_Rgb,           // 4 bpp (no alpha)
	Pixelformat_Compressed_Dxt1_Rgba,          // 4 bpp (1 bit alpha)
	Pixelformat_Compressed_Dxt3_Rgba,          // 8 bpp
	Pixelformat_Compressed_Dxt5_Rgba,          // 8 bpp
	Pixelformat_Compressed_Etc1_Rgb,           // 4 bpp
	Pixelformat_Compressed_Etc2_Rgb,           // 4 bpp
	Pixelformat_Compressed_Etc2_Eac_Rgba,      // 8 bpp
	Pixelformat_Compressed_Pvrt_Rgb,           // 4 bpp
	Pixelformat_Compressed_Pvrt_Rgba,          // 4 bpp
	Pixelformat_Compressed_Astc_4x4_Rgba,      // 8 bpp
	Pixelformat_Compressed_Astc_8x8_Rgba       // 2 bpp

} PixelFormat;

// Texture parameters: filter mode
// NOTE 1: Filtering considers mipmaps if available in the texture
// NOTE 2: Filter is accordingly set for minification and magnification
typedef enum
{
	Texture_Filter_Point = 0,          // No filter, just pixel approximation
	Texture_Filter_Bilinear,           // Linear filtering
	Texture_Filter_Trilinear,          // Trilinear filtering (linear with mipmaps)
	Texture_Filter_Anisotropic_4x,     // Anisotropic filtering 4x
	Texture_Filter_Anisotropic_8x,     // Anisotropic filtering 8x
	Texture_Filter_Anisotropic_16x,    // Anisotropic filtering 16x

} TextureFilter;

// Texture parameters: wrap mode
typedef enum
{
	Texture_Wrap_Repeat = 0,       // Repeats texture in tiled mode
	Texture_Wrap_Clamp,            // Clamps texture to edge pixel in tiled mode
	Texture_Wrap_Mirror_Repeat,    // Mirrors and repeats the texture in tiled mode
	Texture_Wrap_Mirror_Clamp      // mirrors and clamps to border the texture in tiled mode

} TextureWrap;

// Cubemap layouts
typedef enum
{
	Cubemap_Layout_Auto_Detect = 0,        // Automatically detect layout type
	Cubemap_Layout_Line_Vertical,          // Layout is defined by a vertical line with faces
	Cubemap_Layout_Line_Horizontal,        // Layout is defined by a horizontal line with faces
	Cubemap_Layout_Cross_Three_By_Four,    // Layout is defined by a 3x4 cross with cubemap faces
	Cubemap_Layout_Cross_Four_By_Three,    // Layout is defined by a 4x3 cross with cubemap faces
	Cubemap_Layout_Panorama                // layout is defined by a panorama image (equirrectangular map)

} CubemapLayout;

// Font type, defines generation method
typedef enum
{
	Font_Default = 0,    // Default font generation, anti-aliased
	Font_Bitmap,         // Bitmap font generation, no anti-aliasing
	Font_Sdf             // sdf font generation, requires external shader

} FontType;

// Color blending modes (pre-defined)
typedef enum
{
	Blend_Alpha = 0,            // Blend textures considering alpha (default)
	Blend_Additive,             // Blend textures adding colors
	Blend_Multiplied,           // Blend textures multiplying colors
	Blend_Add_Colors,           // Blend textures adding colors (alternative)
	Blend_Subtract_Colors,      // Blend textures subtracting colors (alternative)
	Blend_Alpha_Premultiply,    // Blend premultiplied textures considering alpha
	Blend_Custom,               // Blend textures using custom src/dst factors (use rlSetBlendFactors())
	Blend_Custom_Separate       // blend textures using custom rgb/alpha separate src/dst factors (use rlsetblendfactorsseparate())

} BlendMode;

// Gesture
// NOTE: Provided as bit-wise flags to enable only desired gestures
typedef enum
{
	Gesture_None        = 0,      // No gesture
	Gesture_Tap         = 1,      // Tap gesture
	Gesture_Doubletap   = 2,      // Double tap gesture
	Gesture_Hold        = 4,      // Hold gesture
	Gesture_Drag        = 8,      // Drag gesture
	Gesture_Swipe_Right = 16,     // Swipe right gesture
	Gesture_Swipe_Left  = 32,     // Swipe left gesture
	Gesture_Swipe_Up    = 64,     // Swipe up gesture
	Gesture_Swipe_Down  = 128,    // Swipe down gesture
	Gesture_Pinch_In    = 256,    // Pinch in gesture
	Gesture_Pinch_Out   = 512     // pinch out gesture

} Gesture;

// Camera system modes
typedef enum
{
	Camera_Custom = 0,      // Custom camera
	Camera_Free,            // Free camera
	Camera_Orbital,         // Orbital camera
	Camera_First_Person,    // First person camera
	Camera_Third_Person     // third person camera

} CameraMode;

// Camera projection
typedef enum
{
	Camera_Perspective = 0,    // Perspective projection
	Camera_Orthographic        // orthographic projection

} CameraProjection;

// N-patch layout
typedef enum
{
	Npatch_Nine_Patch = 0,           // Npatch layout: 3x3 tiles
	Npatch_Three_Patch_Vertical,     // Npatch layout: 1x3 tiles
	Npatch_Three_Patch_Horizontal    // npatch layout: 3x1 tiles

} NPatchLayout;

// Callbacks to hook some internal functions
// WARNING: These callbacks are intended for advance users
typedef void           ( *TraceLogCallback )( int logLevel, char const* text, va_list args );          // Logging: Redirect trace log messages
typedef unsigned char* ( *LoadFileDataCallback )( char const* fileName, int* dataSize );               // FileIO: Load binary data
typedef bool           ( *SaveFileDataCallback )( char const* fileName, void* data, int dataSize );    // FileIO: Save binary data
typedef char*          ( *LoadFileTextCallback )( char const* fileName );                              // FileIO: Load text data
typedef bool           ( *SaveFileTextCallback )( char const* fileName, char* text );                  // FileIO: Save text data

//------------------------------------------------------------------------------------
// Global Variables Definition
//------------------------------------------------------------------------------------
// It's lonely here...

//------------------------------------------------------------------------------------
// Window and Graphics Device Functions (Module: core)
//------------------------------------------------------------------------------------

#if defined( __cplusplus )
namespace raylib
{
	extern "C"
	{
// Prevents name mangling of functions
#endif

		// Window-related functions
		RLAPI void        init_window( s32 width, s32 height, char const* title );    // Initialize window and OpenGL context
		RLAPI void        close_window( void );                                       // Close window and unload OpenGL context
		RLAPI bool        window_should_close( void );           // Check if application should close (KEY_ESCAPE pressed or windows close icon clicked)
		RLAPI bool        is_window_ready( void );               // Check if window has been initialized successfully
		RLAPI bool        is_window_fullscreen( void );          // Check if window is currently fullscreen
		RLAPI bool        is_window_hidden( void );              // Check if window is currently hidden (only PLATFORM_DESKTOP)
		RLAPI bool        is_window_minimized( void );           // Check if window is currently minimized (only PLATFORM_DESKTOP)
		RLAPI bool        is_window_maximized( void );           // Check if window is currently maximized (only PLATFORM_DESKTOP)
		RLAPI bool        is_window_focused( void );             // Check if window is currently focused (only PLATFORM_DESKTOP)
		RLAPI bool        is_window_resized( void );             // Check if window has been resized last frame
		RLAPI bool        is_window_state( u32 flag );           // Check if one specific window flag is enabled
		RLAPI void        set_window_state( u32 flags );         // Set window configuration state using flags (only PLATFORM_DESKTOP)
		RLAPI void        clear_window_state( u32 flags );       // Clear window configuration state flags
		RLAPI void        toggle_fullscreen( void );             // Toggle window state: fullscreen/windowed (only PLATFORM_DESKTOP)
		RLAPI void        toggle_borderless_windowed( void );    // Toggle window state: borderless windowed (only PLATFORM_DESKTOP)
		RLAPI void        maximize_window( void );               // Set window state: maximized, if resizable (only PLATFORM_DESKTOP)
		RLAPI void        minimize_window( void );               // Set window state: minimized, if resizable (only PLATFORM_DESKTOP)
		RLAPI void        restore_window( void );                // Set window state: not minimized/maximized (only PLATFORM_DESKTOP)
		RLAPI void        set_window_icon( Image image );        // Set icon for window (single image, RGBA 32bit, only PLATFORM_DESKTOP)
		RLAPI void        set_window_icons( Image* images, s32 count );    // Set icon for window (multiple images, RGBA 32bit, only PLATFORM_DESKTOP)
		RLAPI void        set_window_title( char const* title );           // Set title for window (only PLATFORM_DESKTOP and PLATFORM_WEB)
		RLAPI void        set_window_position( s32 x, s32 y );             // Set window position on screen (only PLATFORM_DESKTOP)
		RLAPI void        set_window_monitor( s32 monitor );               // Set monitor for the current window
		RLAPI void        set_window_min_size( s32 width, s32 height );    // Set window minimum dimensions (for FLAG_WINDOW_RESIZABLE)
		RLAPI void        set_window_max_size( s32 width, s32 height );    // Set window maximum dimensions (for FLAG_WINDOW_RESIZABLE)
		RLAPI void        set_window_size( s32 width, s32 height );        // Set window dimensions
		RLAPI void        set_window_opacity( f32 opacity );               // Set window opacity [0.0f..1.0f] (only PLATFORM_DESKTOP)
		RLAPI void        set_window_focused( void );                      // Set window focused (only PLATFORM_DESKTOP)
		RLAPI void*       get_window_handle( void );                       // Get native window handle
		RLAPI int         get_screen_width( void );                        // Get current screen width
		RLAPI int         get_screen_height( void );                       // Get current screen height
		RLAPI int         get_render_width( void );                        // Get current render width (it considers HiDPI)
		RLAPI int         get_render_height( void );                       // Get current render height (it considers HiDPI)
		RLAPI int         get_monitor_count( void );                       // Get number of connected monitors
		RLAPI int         get_current_monitor( void );                     // Get current connected monitor
		RLAPI Vector2     get_monitor_position( s32 monitor );             // Get specified monitor position
		RLAPI int         get_monitor_width( s32 monitor );                // Get specified monitor width (current video mode used by monitor)
		RLAPI int         get_monitor_height( s32 monitor );               // Get specified monitor height (current video mode used by monitor)
		RLAPI int         get_monitor_physical_width( s32 monitor );       // Get specified monitor physical width in millimetres
		RLAPI int         get_monitor_physical_height( s32 monitor );      // Get specified monitor physical height in millimetres
		RLAPI int         get_monitor_refresh_rate( s32 monitor );         // Get specified monitor refresh rate
		RLAPI Vector2     get_window_position( void );                     // Get window position XY on monitor
		RLAPI Vector2     get_window_scale_dpi( void );                    // Get window scale DPI factor
		RLAPI char const* get_monitor_name( s32 monitor );                 // Get the human-readable, UTF-8 encoded name of the specified monitor
		RLAPI void        set_clipboard_text( char const* text );          // Set clipboard text content
		RLAPI char const* get_clipboard_text( void );                      // Get clipboard text content
		RLAPI void        enable_event_waiting( void );                    // Enable waiting for events on EndDrawing(), no automatic event polling
		RLAPI void        disable_event_waiting( void );                   // Disable waiting for events on EndDrawing(), automatic events polling

		// Cursor-related functions
		RLAPI void show_cursor( void );            // Shows cursor
		RLAPI void hide_cursor( void );            // Hides cursor
		RLAPI bool is_cursor_hidden( void );       // Check if cursor is not visible
		RLAPI void enable_cursor( void );          // Enables cursor (unlock cursor)
		RLAPI void disable_cursor( void );         // Disables cursor (lock cursor)
		RLAPI bool is_cursor_on_screen( void );    // Check if cursor is on the screen

		// Drawing-related functions
		RLAPI void clear_background( Color color );                              // Set background color (framebuffer clear color)
		RLAPI void begin_drawing( void );                                        // Setup canvas (framebuffer) to start drawing
		RLAPI void end_drawing( void );                                          // End canvas drawing and swap buffers (double buffering)
		RLAPI void begin_mode_2d( Camera2D camera );                             // Begin 2D mode with custom camera (2D)
		RLAPI void end_mode_2d( void );                                          // Ends 2D mode with custom camera
		RLAPI void begin_mode_3d( Camera3D camera );                             // Begin 3D mode with custom camera (3D)
		RLAPI void end_mode_3d( void );                                          // Ends 3D mode and returns to default 2D orthographic mode
		RLAPI void begin_texture_mode( RenderTexture2D target );                 // Begin drawing to render texture
		RLAPI void end_texture_mode( void );                                     // Ends drawing to render texture
		RLAPI void begin_shader_mode( Shader shader );                           // Begin custom shader drawing
		RLAPI void end_shader_mode( void );                                      // End custom shader drawing (use default shader)
		RLAPI void begin_blend_mode( s32 mode );                                 // Begin blending mode (alpha, additive, multiplied, subtract, custom)
		RLAPI void end_blend_mode( void );                                       // End blending mode (reset to default: alpha blending)
		RLAPI void begin_scissor_mode( s32 x, s32 y, s32 width, s32 height );    // Begin scissor mode (define screen area for following drawing)
		RLAPI void end_scissor_mode( void );                                     // End scissor mode
		RLAPI void begin_vr_stereo_mode( VrStereoConfig config );                // Begin stereo rendering (requires VR simulator)
		RLAPI void end_vr_stereo_mode( void );                                   // End stereo rendering (requires VR simulator)

		// VR stereo config functions for VR simulator
		RLAPI VrStereoConfig load_vr_stereo_config( VrDeviceInfo device );        // Load VR stereo config for VR simulator device parameters
		RLAPI void           unload_vr_stereo_config( VrStereoConfig config );    // Unload VR stereo config

		// Shader management functions
		// NOTE: Shader functionality is not available on OpenGL 1.1
		RLAPI Shader load_shader( char const* vsFileName, char const* fsFileName );          // Load shader from files and bind default locations
		RLAPI Shader load_shader_from_memory( char const* vsCode, char const* fsCode );      // Load shader from code strings and bind default locations
		RLAPI bool   is_shader_ready( Shader shader );                                       // Check if a shader is ready
		RLAPI int    get_shader_location( Shader shader, char const* uniformName );          // Get shader uniform location
		RLAPI int    get_shader_location_attrib( Shader shader, char const* attribName );    // Get shader attribute location
		RLAPI void   set_shader_value( Shader shader, s32 locIndex, void const* value, s32 uniformType );                 // Set shader uniform value
		RLAPI void   set_shader_value_v( Shader shader, s32 locIndex, void const* value, s32 uniformType, s32 count );    // Set shader uniform value vector
		RLAPI void   set_shader_value_matrix( Shader shader, s32 locIndex, Matrix mat );            // Set shader uniform value (matrix 4x4)
		RLAPI void   set_shader_value_texture( Shader shader, s32 locIndex, Texture2D texture );    // Set shader uniform value for texture (sampler2d)
		RLAPI void   unload_shader( Shader shader );                                                // Unload shader from GPU memory (VRAM)

		// Screen-space-related functions
		RLAPI Ray     get_mouse_ray( Vector2 mousePosition, Camera camera );          // Get a ray trace from mouse position
		RLAPI Matrix  get_camera_matrix( Camera camera );                             // Get camera transform matrix (view matrix)
		RLAPI Matrix  get_camera_matrix_2d( Camera2D camera );                        // Get camera 2d transform matrix
		RLAPI Vector2 get_world_to_screen( Vector3 position, Camera camera );         // Get the screen space position for a 3d world space position
		RLAPI Vector2 get_screen_to_world_2d( Vector2 position, Camera2D camera );    // Get the world space position for a 2d camera screen space position
		RLAPI Vector2 get_world_to_screen_ex( Vector3 position, Camera camera, s32 width, s32 height );    // Get size position for a 3d world space position
		RLAPI Vector2 get_world_to_screen_2d( Vector2 position, Camera2D camera );    // Get the screen space position for a 2d camera world space position

		// Timing-related functions
		RLAPI void   set_target_fps( s32 fps );    // Set target FPS (maximum)
		RLAPI float  get_frame_time( void );       // Get time in seconds for last frame drawn (delta time)
		RLAPI double get_time( void );             // Get elapsed time in seconds since InitWindow()
		RLAPI int    get_fps( void );              // Get current FPS

		// Custom frame control functions
		// NOTE: Those functions are intended for advance users that want full control over the frame processing
		// By default EndDrawing() does this job: draws everything + SwapScreenBuffer() + manage frame timing + PollInputEvents()
		// To avoid that behaviour and control frame processes manually, enable in config.h: SUPPORT_CUSTOM_FRAME_CONTROL
		RLAPI void swap_screen_buffer( void );     // Swap back buffer with front buffer (screen drawing)
		RLAPI void poll_input_events( void );      // Register all input events
		RLAPI void wait_time( double seconds );    // Wait for some time (halt program execution)

		// Misc. functions
		RLAPI int  get_random_value( s32 min, s32 max );       // Get a random value between min and max (both included)
		RLAPI void set_random_seed( u32 seed );                // Set the seed for the random number generator
		RLAPI void take_screenshot( char const* fileName );    // Takes a screenshot of current screen (filename extension defines format)
		RLAPI void set_config_flags( u32 flags );              // Setup init configuration flags (view FLAGS)
		RLAPI void open_url( char const* url );                // Open URL with default system browser (if available)

		// NOTE: Following functions implemented in module [utils]
		//------------------------------------------------------------------
		RLAPI void  trace_log( s32 logLevel, char const* text, ... );    // Show trace log messages (LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR...)
		RLAPI void  set_trace_log_level( s32 logLevel );                 // Set the current threshold (minimum) log level
		RLAPI void* mem_alloc( u32 size );                               // Internal memory allocator
		RLAPI void* mem_realloc( void* ptr, u32 size );                  // Internal memory reallocator
		RLAPI void  mem_free( void* ptr );                               // Internal memory free

		// Set custom callbacks
		// WARNING: Callbacks setup is intended for advance users
		RLAPI void set_trace_log_callback( TraceLogCallback callback );             // Set custom trace log
		RLAPI void set_load_file_data_callback( LoadFileDataCallback callback );    // Set custom file binary data loader
		RLAPI void set_save_file_data_callback( SaveFileDataCallback callback );    // Set custom file binary data saver
		RLAPI void set_load_file_text_callback( LoadFileTextCallback callback );    // Set custom file text data loader
		RLAPI void set_save_file_text_callback( SaveFileTextCallback callback );    // Set custom file text data saver

		// Files management functions
		RLAPI unsigned char* load_file_data( char const* fileName, s32* dataSize );     // Load file data as byte array (read)
		RLAPI void           unload_file_data( u8* data );                              // Unload file data allocated by LoadFileData()
		RLAPI bool save_file_data( char const* fileName, void* data, s32 dataSize );    // Save data to file from byte array (write), returns true on success
		RLAPI bool export_data_as_code( unsigned char const* data, s32 dataSize, char const* fileName );    // Export data to code (.h), returns true on success
		RLAPI char* load_file_text( char const* fileName );    // Load text data from file (read), returns a '\0' terminated string
		RLAPI void  unload_file_text( char* text );            // Unload file text data allocated by LoadFileText()
		RLAPI bool
		    save_file_text( char const* fileName, char* text );    // Save text data to file (write), string must be '\0' terminated, returns true on success
		//------------------------------------------------------------------

		// File system functions
		RLAPI bool         file_exists( char const* fileName );                           // Check if file exists
		RLAPI bool         directory_exists( char const* dirPath );                       // Check if a directory path exists
		RLAPI bool         is_file_extension( char const* fileName, char const* ext );    // Check file extension (including point: .png, .wav)
		RLAPI int          get_file_length( char const* fileName );              // Get file length in bytes (NOTE: GetFileSize() conflicts with windows.h)
		RLAPI char const*  get_file_extension( char const* fileName );           // Get pointer to extension for a filename string (includes dot: '.png')
		RLAPI char const*  get_file_name( char const* filePath );                // Get pointer to filename for a path string
		RLAPI char const*  get_file_name_without_ext( char const* filePath );    // Get filename string without extension (uses static string)
		RLAPI char const*  get_directory_path( char const* filePath );           // Get full path for a given fileName with path (uses static string)
		RLAPI char const*  get_prev_directory_path( char const* dirPath );       // Get previous directory path for a given path (uses static string)
		RLAPI char const*  get_working_directory( void );                        // Get current working directory (uses static string)
		RLAPI char const*  get_application_directory( void );                    // Get the directory of the running application (uses static string)
		RLAPI bool         change_directory( char const* dir );                  // Change working directory, return true on success
		RLAPI bool         is_path_file( char const* path );                     // Check if a given path is a file or a directory
		RLAPI FilePathList load_directory_files( char const* dirPath );          // Load directory filepaths
		RLAPI FilePathList load_directory_files_ex(
		    char const* basePath,
		    char const* filter,
		    bool        scanSubdirs
		);                                                                  // Load directory filepaths with extension filtering and recursive directory scan
		RLAPI void         unload_directory_files( FilePathList files );    // Unload filepaths
		RLAPI bool         is_file_dropped( void );                         // Check if a file has been dropped into window
		RLAPI FilePathList load_dropped_files( void );                      // Load dropped filepaths
		RLAPI void         unload_dropped_files( FilePathList files );      // Unload dropped filepaths
		RLAPI long         get_file_mod_time( char const* fileName );       // Get file modification time (last write time)

		// Compression/Encoding functionality
		RLAPI unsigned char*
		    compress_data( unsigned char const* data, s32 dataSize, s32* compDataSize );    // Compress data (DEFLATE algorithm), memory must be MemFree()
		RLAPI unsigned char* decompress_data(
		    unsigned char const* compData,
		    s32                  compDataSize,
		    s32*                 dataSize
		);    // Decompress data (DEFLATE algorithm), memory must be MemFree()
		RLAPI char* encode_data_base64( unsigned char const* data, s32 dataSize, s32* outputSize );    // Encode data to Base64 string, memory must be MemFree()
		RLAPI unsigned char* decode_data_base64( unsigned char const* data, s32* outputSize );         // Decode Base64 string data, memory must be MemFree()

		//------------------------------------------------------------------------------------
		// Input Handling Functions (Module: core)
		//------------------------------------------------------------------------------------

		// Input-related functions: keyboard
		RLAPI bool is_key_pressed( s32 key );           // Check if a key has been pressed once
		RLAPI bool is_key_pressed_repeat( s32 key );    // Check if a key has been pressed again (Only PLATFORM_DESKTOP)
		RLAPI bool is_key_down( s32 key );              // Check if a key is being pressed
		RLAPI bool is_key_released( s32 key );          // Check if a key has been released once
		RLAPI bool is_key_up( s32 key );                // Check if a key is NOT being pressed
		RLAPI int  get_key_pressed( void );             // Get key pressed (keycode), call it multiple times for keys queued, returns 0 when the queue is empty
		RLAPI int  get_char_pressed( void );    // Get char pressed (unicode), call it multiple times for chars queued, returns 0 when the queue is empty
		RLAPI void set_exit_key( s32 key );     // Set a custom key to exit program (default is ESC)

		// Input-related functions: gamepads
		RLAPI bool        is_gamepad_available( s32 gamepad );                      // Check if a gamepad is available
		RLAPI char const* get_gamepad_name( s32 gamepad );                          // Get gamepad internal name id
		RLAPI bool        is_gamepad_button_pressed( s32 gamepad, s32 button );     // Check if a gamepad button has been pressed once
		RLAPI bool        is_gamepad_button_down( s32 gamepad, s32 button );        // Check if a gamepad button is being pressed
		RLAPI bool        is_gamepad_button_released( s32 gamepad, s32 button );    // Check if a gamepad button has been released once
		RLAPI bool        is_gamepad_button_up( s32 gamepad, s32 button );          // Check if a gamepad button is NOT being pressed
		RLAPI int         get_gamepad_button_pressed( void );                       // Get the last gamepad button pressed
		RLAPI int         get_gamepad_axis_count( s32 gamepad );                    // Get gamepad axis count for a gamepad
		RLAPI float       get_gamepad_axis_movement( s32 gamepad, s32 axis );       // Get axis movement value for a gamepad axis
		RLAPI int         set_gamepad_mappings( char const* mappings );             // Set internal gamepad mappings (SDL_GameControllerDB)

		// Input-related functions: mouse
		RLAPI bool    is_mouse_button_pressed( s32 button );           // Check if a mouse button has been pressed once
		RLAPI bool    is_mouse_button_down( s32 button );              // Check if a mouse button is being pressed
		RLAPI bool    is_mouse_button_released( s32 button );          // Check if a mouse button has been released once
		RLAPI bool    is_mouse_button_up( s32 button );                // Check if a mouse button is NOT being pressed
		RLAPI int     get_mouse_x( void );                             // Get mouse position X
		RLAPI int     get_mouse_y( void );                             // Get mouse position Y
		RLAPI Vector2 get_mouse_position( void );                      // Get mouse position XY
		RLAPI Vector2 get_mouse_delta( void );                         // Get mouse delta between frames
		RLAPI void    set_mouse_position( s32 x, s32 y );              // Set mouse position XY
		RLAPI void    set_mouse_offset( s32 offsetX, s32 offsetY );    // Set mouse offset
		RLAPI void    set_mouse_scale( f32 scaleX, f32 scaleY );       // Set mouse scaling
		RLAPI float   get_mouse_wheel_move( void );                    // Get mouse wheel movement for X or Y, whichever is larger
		RLAPI Vector2 get_mouse_wheel_move_v( void );                  // Get mouse wheel movement for both X and Y
		RLAPI void    set_mouse_cursor( s32 cursor );                  // Set mouse cursor

		// Input-related functions: touch
		RLAPI int     get_touch_x( void );                // Get touch position X for touch point 0 (relative to screen size)
		RLAPI int     get_touch_y( void );                // Get touch position Y for touch point 0 (relative to screen size)
		RLAPI Vector2 get_touch_position( s32 index );    // Get touch position XY for a touch point index (relative to screen size)
		RLAPI int     get_touch_point_id( s32 index );    // Get touch point identifier for given index
		RLAPI int     get_touch_point_count( void );      // Get number of touch points

		//------------------------------------------------------------------------------------
		// Gestures and Touch Handling Functions (Module: rgestures)
		//------------------------------------------------------------------------------------
		RLAPI void    set_gestures_enabled( u32 flags );     // Enable a set of gestures using flags
		RLAPI bool    is_gesture_detected( u32 gesture );    // Check if a gesture have been detected
		RLAPI int     get_gesture_detected( void );          // Get latest detected gesture
		RLAPI float   get_gesture_hold_duration( void );     // Get gesture hold time in milliseconds
		RLAPI Vector2 get_gesture_drag_vector( void );       // Get gesture drag vector
		RLAPI float   get_gesture_drag_angle( void );        // Get gesture drag angle
		RLAPI Vector2 get_gesture_pinch_vector( void );      // Get gesture pinch delta
		RLAPI float   get_gesture_pinch_angle( void );       // Get gesture pinch angle

		//------------------------------------------------------------------------------------
		// Camera System Functions (Module: rcamera)
		//------------------------------------------------------------------------------------
		RLAPI void update_camera( Camera* camera, s32 mode );                                            // Update camera position for selected mode
		RLAPI void update_camera_pro( Camera* camera, Vector3 movement, Vector3 rotation, f32 zoom );    // Update camera movement/rotation

		//------------------------------------------------------------------------------------
		// Basic Shapes Drawing Functions (Module: shapes)
		//------------------------------------------------------------------------------------
		// Set texture and rectangle to be used on shapes drawing
		// NOTE: It can be useful when using basic shapes and one single font,
		// defining a font char white rectangle would allow drawing everything in a single draw call
		RLAPI void set_shapes_texture( Texture2D texture, Rectangle source );    // Set texture and rectangle to be used on shapes drawing

		// Basic shapes drawing functions
		RLAPI void draw_pixel( s32 posX, s32 posY, Color color );                                       // Draw a pixel
		RLAPI void draw_pixel_v( Vector2 position, Color color );                                       // Draw a pixel (Vector version)
		RLAPI void draw_line( s32 startPosX, s32 startPosY, s32 endPosX, s32 endPosY, Color color );    // Draw a line
		RLAPI void draw_line_v( Vector2 startPos, Vector2 endPos, Color color );                        // Draw a line (Vector version)
		RLAPI void draw_line_ex( Vector2 startPos, Vector2 endPos, f32 thick, Color color );            // Draw a line defining thickness
		RLAPI void draw_line_bezier( Vector2 startPos, Vector2 endPos, f32 thick, Color color );        // Draw a line using cubic-bezier curves in-out
		RLAPI void draw_line_bezier_quad( Vector2 startPos, Vector2 endPos, Vector2 controlPos, f32 thick, Color color );    // Draw line using quadratic bezier
		                                                                                                                     // curves with a control point
		RLAPI void draw_line_bezier_cubic(
		    Vector2 startPos,
		    Vector2 endPos,
		    Vector2 startControlPos,
		    Vector2 endControlPos,
		    f32     thick,
		    Color   color
		);                                                                                          // Draw line using cubic bezier curves with 2 control points
		RLAPI void draw_line_bspline( Vector2* points, s32 pointCount, f32 thick, Color color );    // Draw a B-Spline line, minimum 4 points
		RLAPI void draw_line_catmull_rom( Vector2* points, s32 pointCount, f32 thick, Color color );    // Draw a Catmull Rom spline line, minimum 4 points
		RLAPI void draw_line_strip( Vector2* points, s32 pointCount, Color color );                     // Draw lines sequence
		RLAPI void draw_circle( s32 centerX, s32 centerY, f32 radius, Color color );                    // Draw a color-filled circle
		RLAPI void draw_circle_sector( Vector2 center, f32 radius, f32 startAngle, f32 endAngle, s32 segments, Color color );    // Draw a piece of a circle
		RLAPI void
		    draw_circle_sector_lines( Vector2 center, f32 radius, f32 startAngle, f32 endAngle, s32 segments, Color color );    // Draw circle sector outline
		RLAPI void draw_circle_gradient( s32 centerX, s32 centerY, f32 radius, Color color1, Color color2 );                    // Draw a gradient-filled circle
		RLAPI void draw_circle_v( Vector2 center, f32 radius, Color color );                                 // Draw a color-filled circle (Vector version)
		RLAPI void draw_circle_lines( s32 centerX, s32 centerY, f32 radius, Color color );                   // Draw circle outline
		RLAPI void draw_circle_lines_v( Vector2 center, f32 radius, Color color );                           // Draw circle outline (Vector version)
		RLAPI void draw_ellipse( s32 centerX, s32 centerY, f32 radiusH, f32 radiusV, Color color );          // Draw ellipse
		RLAPI void draw_ellipse_lines( s32 centerX, s32 centerY, f32 radiusH, f32 radiusV, Color color );    // Draw ellipse outline
		RLAPI void draw_ring( Vector2 center, f32 innerRadius, f32 outerRadius, f32 startAngle, f32 endAngle, s32 segments, Color color );          // Draw ring
		RLAPI void draw_ring_lines( Vector2 center, f32 innerRadius, f32 outerRadius, f32 startAngle, f32 endAngle, s32 segments, Color color );    // Draw ring
		                                                                                                                                            // outline
		RLAPI void draw_rectangle( s32 posX, s32 posY, s32 width, s32 height, Color color );          // Draw a color-filled rectangle
		RLAPI void draw_rectangle_v( Vector2 position, Vector2 size, Color color );                   // Draw a color-filled rectangle (Vector version)
		RLAPI void draw_rectangle_rec( Rectangle rec, Color color );                                  // Draw a color-filled rectangle
		RLAPI void draw_rectangle_pro( Rectangle rec, Vector2 origin, f32 rotation, Color color );    // Draw a color-filled rectangle with pro parameters
		RLAPI void
		    draw_rectangle_gradient_v( s32 posX, s32 posY, s32 width, s32 height, Color color1, Color color2 );    // Draw a vertical-gradient-filled rectangle
		RLAPI void draw_rectangle_gradient_h( s32 posX, s32 posY, s32 width, s32 height, Color color1, Color color2 );    // Draw a horizontal-gradient-filled
		                                                                                                                  // rectangle
		RLAPI void draw_rectangle_gradient_ex( Rectangle rec, Color col1, Color col2, Color col3, Color col4 );    // Draw a gradient-filled rectangle with
		                                                                                                           // custom vertex colors
		RLAPI void draw_rectangle_lines( s32 posX, s32 posY, s32 width, s32 height, Color color );                 // Draw rectangle outline
		RLAPI void draw_rectangle_lines_ex( Rectangle rec, f32 lineThick, Color color );                 // Draw rectangle outline with extended parameters
		RLAPI void draw_rectangle_rounded( Rectangle rec, f32 roundness, s32 segments, Color color );    // Draw rectangle with rounded edges
		RLAPI void draw_rectangle_rounded_lines( Rectangle rec, f32 roundness, s32 segments, f32 lineThick, Color color );    // Draw rectangle with rounded
		                                                                                                                      // edges outline
		RLAPI void draw_triangle( Vector2 v1, Vector2 v2, Vector2 v3, Color color );    // Draw a color-filled triangle (vertex in counter-clockwise order!)
		RLAPI void draw_triangle_lines( Vector2 v1, Vector2 v2, Vector2 v3, Color color );    // Draw triangle outline (vertex in counter-clockwise order!)
		RLAPI void draw_triangle_fan( Vector2* points, s32 pointCount, Color color );      // Draw a triangle fan defined by points (first vertex is the center)
		RLAPI void draw_triangle_strip( Vector2* points, s32 pointCount, Color color );    // Draw a triangle strip defined by points
		RLAPI void draw_poly( Vector2 center, s32 sides, f32 radius, f32 rotation, Color color );          // Draw a regular polygon (Vector version)
		RLAPI void draw_poly_lines( Vector2 center, s32 sides, f32 radius, f32 rotation, Color color );    // Draw a polygon outline of n sides
		RLAPI void draw_poly_lines_ex( Vector2 center, s32 sides, f32 radius, f32 rotation, f32 lineThick, Color color );    // Draw a polygon outline of n
		                                                                                                                     // sides with extended parameters

		// Basic shapes collision detection functions
		RLAPI bool check_collision_recs( Rectangle rec1, Rectangle rec2 );                                   // Check collision between two rectangles
		RLAPI bool check_collision_circles( Vector2 center1, f32 radius1, Vector2 center2, f32 radius2 );    // Check collision between two circles
		RLAPI bool check_collision_circle_rec( Vector2 center, f32 radius, Rectangle rec );                  // Check collision between circle and rectangle
		RLAPI bool check_collision_point_rec( Vector2 point, Rectangle rec );                                // Check if point is inside rectangle
		RLAPI bool check_collision_point_circle( Vector2 point, Vector2 center, f32 radius );                // Check if point is inside circle
		RLAPI bool check_collision_point_triangle( Vector2 point, Vector2 p1, Vector2 p2, Vector2 p3 );      // Check if point is inside a triangle
		RLAPI bool check_collision_point_poly(
		    Vector2  point,
		    Vector2* points,
		    s32      pointCount
		);    // Check if point is within a polygon described by array of vertices
		RLAPI bool check_collision_lines(
		    Vector2  startPos1,
		    Vector2  endPos1,
		    Vector2  startPos2,
		    Vector2  endPos2,
		    Vector2* collisionPoint
		);    // Check the collision between two lines defined by two points each, returns collision point by reference
		RLAPI bool check_collision_point_line(
		    Vector2 point,
		    Vector2 p1,
		    Vector2 p2,
		    s32     threshold
		);    // Check if point belongs to line created between two points [p1] and [p2] with defined margin in pixels [threshold]
		RLAPI Rectangle get_collision_rec( Rectangle rec1, Rectangle rec2 );    // Get collision rectangle for two rectangles collision

		//------------------------------------------------------------------------------------
		// Texture Loading and Drawing Functions (Module: textures)
		//------------------------------------------------------------------------------------

		// Image loading functions
		// NOTE: These functions do not require GPU access
		RLAPI Image load_image( char const* fileName );                                                           // Load image from file into CPU memory (RAM)
		RLAPI Image load_image_raw( char const* fileName, s32 width, s32 height, s32 format, s32 headerSize );    // Load image from RAW file data
		RLAPI Image load_image_svg( char const* fileNameOrString, s32 width, s32 height );    // Load image from SVG file data or string with specified size
		RLAPI Image load_image_anim( char const* fileName, s32* frames );                     // Load image sequence from file (frames appended to image.data)
		RLAPI Image load_image_from_memory(
		    char const*          fileType,
		    unsigned char const* fileData,
		    s32                  dataSize
		);                                                                         // Load image from memory buffer, fileType refers to extension: i.e. '.png'
		RLAPI Image          load_image_from_texture( Texture2D texture );         // Load image from GPU texture data
		RLAPI Image          load_image_from_screen( void );                       // Load image from screen buffer and (screenshot)
		RLAPI bool           is_image_ready( Image image );                        // Check if an image is ready
		RLAPI void           unload_image( Image image );                          // Unload image from CPU memory (RAM)
		RLAPI bool           export_image( Image image, char const* fileName );    // Export image data to file, returns true on success
		RLAPI unsigned char* export_image_to_memory( Image image, char const* fileType, s32* fileSize );    // Export image to memory buffer
		RLAPI bool
		    export_image_as_code( Image image, char const* fileName );    // Export image as code file defining an array of bytes, returns true on success

		// Image generation functions
		RLAPI Image gen_image_color( s32 width, s32 height, Color color );                                        // Generate image: plain color
		RLAPI Image gen_image_gradient_linear( s32 width, s32 height, s32 direction, Color start, Color end );    // Generate image: linear gradient, direction
		                                                                                                          // in degrees [0..360], 0=Vertical gradient
		RLAPI Image gen_image_gradient_radial( s32 width, s32 height, f32 density, Color inner, Color outer );    // Generate image: radial gradient
		RLAPI Image gen_image_gradient_square( s32 width, s32 height, f32 density, Color inner, Color outer );    // Generate image: square gradient
		RLAPI Image gen_image_checked( s32 width, s32 height, s32 checksX, s32 checksY, Color col1, Color col2 );    // Generate image: checked
		RLAPI Image gen_image_white_noise( s32 width, s32 height, f32 factor );                                      // Generate image: white noise
		RLAPI Image gen_image_perlin_noise( s32 width, s32 height, s32 offsetX, s32 offsetY, f32 scale );            // Generate image: perlin noise
		RLAPI Image gen_image_cellular( s32 width, s32 height, s32 tileSize );    // Generate image: cellular algorithm, bigger tileSize means bigger cells
		RLAPI Image gen_image_text( s32 width, s32 height, char const* text );    // Generate image: grayscale image from text data

		// Image manipulation functions
		RLAPI Image image_copy( Image image );                                    // Create an image duplicate (useful for transformations)
		RLAPI Image image_from_image( Image image, Rectangle rec );               // Create an image from another image piece
		RLAPI Image image_text( char const* text, s32 fontSize, Color color );    // Create an image from text (default font)
		RLAPI Image image_text_ex( Font font, char const* text, f32 fontSize, f32 spacing, Color tint );    // Create an image from text (custom sprite font)
		RLAPI void  image_format( Image* image, s32 newFormat );                                            // Convert image data to desired format
		RLAPI void  image_to_pot( Image* image, Color fill );                                               // Convert image to POT (power-of-two)
		RLAPI void  image_crop( Image* image, Rectangle crop );                                             // Crop an image to a defined rectangle
		RLAPI void  image_alpha_crop( Image* image, f32 threshold );                                        // Crop image depending on alpha value
		RLAPI void  image_alpha_clear( Image* image, Color color, f32 threshold );                          // Clear alpha channel to desired color
		RLAPI void  image_alpha_mask( Image* image, Image alphaMask );                                      // Apply alpha mask to image
		RLAPI void  image_alpha_premultiply( Image* image );                                                // Premultiply alpha channel
		RLAPI void  image_blur_gaussian( Image* image, s32 blurSize );               // Apply Gaussian blur using a box blur approximation
		RLAPI void  image_resize( Image* image, s32 newWidth, s32 newHeight );       // Resize image (Bicubic scaling algorithm)
		RLAPI void  image_resize_nn( Image* image, s32 newWidth, s32 newHeight );    // Resize image (Nearest-Neighbor scaling algorithm)
		RLAPI void
		    image_resize_canvas( Image* image, s32 newWidth, s32 newHeight, s32 offsetX, s32 offsetY, Color fill );    // Resize canvas and fill with color
		RLAPI void   image_mipmaps( Image* image );                                           // Compute all mipmap levels for a provided image
		RLAPI void   image_dither( Image* image, s32 rBpp, s32 gBpp, s32 bBpp, s32 aBpp );    // Dither image data to 16bpp or lower (Floyd-Steinberg dithering)
		RLAPI void   image_flip_vertical( Image* image );                                     // Flip image vertically
		RLAPI void   image_flip_horizontal( Image* image );                                   // Flip image horizontally
		RLAPI void   image_rotate( Image* image, s32 degrees );                               // Rotate image by input angle in degrees (-359 to 359)
		RLAPI void   image_rotate_cw( Image* image );                                         // Rotate image clockwise 90deg
		RLAPI void   image_rotate_ccw( Image* image );                                        // Rotate image counter-clockwise 90deg
		RLAPI void   image_color_tint( Image* image, Color color );                           // Modify image color: tint
		RLAPI void   image_color_invert( Image* image );                                      // Modify image color: invert
		RLAPI void   image_color_grayscale( Image* image );                                   // Modify image color: grayscale
		RLAPI void   image_color_contrast( Image* image, f32 contrast );                      // Modify image color: contrast (-100 to 100)
		RLAPI void   image_color_brightness( Image* image, s32 brightness );                  // Modify image color: brightness (-255 to 255)
		RLAPI void   image_color_replace( Image* image, Color color, Color replace );         // Modify image color: replace color
		RLAPI Color* load_image_colors( Image image );                                        // Load color data from image as a Color array (RGBA - 32bit)
		RLAPI Color*
		           load_image_palette( Image image, s32 maxPaletteSize, s32* colorCount );    // Load colors palette from image as a Color array (RGBA - 32bit)
		RLAPI void unload_image_colors( Color* colors );                                      // Unload color data loaded with LoadImageColors()
		RLAPI void unload_image_palette( Color* colors );                                     // Unload colors palette loaded with LoadImagePalette()
		RLAPI Rectangle get_image_alpha_border( Image image, f32 threshold );                 // Get image alpha border rectangle
		RLAPI Color     get_image_color( Image image, s32 x, s32 y );                         // Get image pixel color at (x, y) position

		// Image drawing functions
		// NOTE: Image software-rendering functions (CPU)
		RLAPI void image_clear_background( Image* dst, Color color );                  // Clear image background with given color
		RLAPI void image_draw_pixel( Image* dst, s32 posX, s32 posY, Color color );    // Draw pixel within an image
		RLAPI void image_draw_pixel_v( Image* dst, Vector2 position, Color color );    // Draw pixel within an image (Vector version)
		RLAPI void image_draw_line( Image* dst, s32 startPosX, s32 startPosY, s32 endPosX, s32 endPosY, Color color );    // Draw line within an image
		RLAPI void image_draw_line_v( Image* dst, Vector2 start, Vector2 end, Color color );              // Draw line within an image (Vector version)
		RLAPI void image_draw_circle( Image* dst, s32 centerX, s32 centerY, s32 radius, Color color );    // Draw a filled circle within an image
		RLAPI void image_draw_circle_v( Image* dst, Vector2 center, s32 radius, Color color );    // Draw a filled circle within an image (Vector version)
		RLAPI void image_draw_circle_lines( Image* dst, s32 centerX, s32 centerY, s32 radius, Color color );    // Draw circle outline within an image
		RLAPI void image_draw_circle_lines_v( Image* dst, Vector2 center, s32 radius, Color color );    // Draw circle outline within an image (Vector version)
		RLAPI void image_draw_rectangle( Image* dst, s32 posX, s32 posY, s32 width, s32 height, Color color );    // Draw rectangle within an image
		RLAPI void image_draw_rectangle_v( Image* dst, Vector2 position, Vector2 size, Color color );      // Draw rectangle within an image (Vector version)
		RLAPI void image_draw_rectangle_rec( Image* dst, Rectangle rec, Color color );                     // Draw rectangle within an image
		RLAPI void image_draw_rectangle_lines( Image* dst, Rectangle rec, s32 thick, Color color );        // Draw rectangle lines within an image
		RLAPI void image_draw( Image* dst, Image src, Rectangle srcRec, Rectangle dstRec, Color tint );    // Draw a source image within a destination image
		                                                                                                   // (tint applied to source)
		RLAPI void image_draw_text( Image* dst, char const* text, s32 posX, s32 posY, s32 fontSize, Color color );    // Draw text (using default font) within
		                                                                                                              // an image (destination)
		RLAPI void image_draw_text_ex(
		    Image*      dst,
		    Font        font,
		    char const* text,
		    Vector2     position,
		    f32         fontSize,
		    f32         spacing,
		    Color       tint
		);    // Draw text (custom sprite font) within an image (destination)

		// Texture loading functions
		// NOTE: These functions require GPU access
		RLAPI Texture2D       load_texture( char const* fileName );                       // Load texture from file into GPU memory (VRAM)
		RLAPI Texture2D       load_texture_from_image( Image image );                     // Load texture from image data
		RLAPI TextureCubemap  load_texture_cubemap( Image image, s32 layout );            // Load cubemap from image, multiple image cubemap layouts supported
		RLAPI RenderTexture2D load_render_texture( s32 width, s32 height );               // Load texture for rendering (framebuffer)
		RLAPI bool            is_texture_ready( Texture2D texture );                      // Check if a texture is ready
		RLAPI void            unload_texture( Texture2D texture );                        // Unload texture from GPU memory (VRAM)
		RLAPI bool            is_render_texture_ready( RenderTexture2D target );          // Check if a render texture is ready
		RLAPI void            unload_render_texture( RenderTexture2D target );            // Unload render texture from GPU memory (VRAM)
		RLAPI void            update_texture( Texture2D texture, void const* pixels );    // Update GPU texture with new data
		RLAPI void            update_texture_rec( Texture2D texture, Rectangle rec, void const* pixels );    // Update GPU texture rectangle with new data

		// Texture configuration functions
		RLAPI void gen_texture_mipmaps( Texture2D* texture );              // Generate GPU mipmaps for a texture
		RLAPI void set_texture_filter( Texture2D texture, s32 filter );    // Set texture scaling filter mode
		RLAPI void set_texture_wrap( Texture2D texture, s32 wrap );        // Set texture wrapping mode

		// Texture drawing functions
		RLAPI void draw_texture( Texture2D texture, s32 posX, s32 posY, Color tint );    // Draw a Texture2D
		RLAPI void draw_texture_v( Texture2D texture, Vector2 position, Color tint );    // Draw a Texture2D with position defined as Vector2
		RLAPI void draw_texture_ex( Texture2D texture, Vector2 position, f32 rotation, f32 scale, Color tint );    // Draw a Texture2D with extended parameters
		RLAPI void draw_texture_rec( Texture2D texture, Rectangle source, Vector2 position, Color tint );    // Draw a part of a texture defined by a rectangle
		RLAPI void draw_texture_pro(
		    Texture2D texture,
		    Rectangle source,
		    Rectangle dest,
		    Vector2   origin,
		    f32       rotation,
		    Color     tint
		);    // Draw a part of a texture defined by a rectangle with 'pro' parameters
		RLAPI void draw_texture_npatch(
		    Texture2D  texture,
		    NPatchInfo nPatchInfo,
		    Rectangle  dest,
		    Vector2    origin,
		    f32        rotation,
		    Color      tint
		);    // Draws a texture (or part of it) that stretches or shrinks nicely

		// Color/pixel related functions
		RLAPI Color   fade( Color color, f32 alpha );                          // Get color with alpha applied, alpha goes from 0.0f to 1.0f
		RLAPI int     color_to_int( Color color );                             // Get hexadecimal value for a Color
		RLAPI Vector4 color_normalize( Color color );                          // Get Color normalized as float [0..1]
		RLAPI Color   color_from_normalized( Vector4 normalized );             // Get Color from normalized values [0..1]
		RLAPI Vector3 color_to_hsv( Color color );                             // Get HSV values for a Color, hue [0..360], saturation/value [0..1]
		RLAPI Color   color_from_hsv( f32 hue, f32 saturation, f32 value );    // Get a Color from HSV values, hue [0..360], saturation/value [0..1]
		RLAPI Color   color_tint( Color color, Color tint );                   // Get color multiplied with another color
		RLAPI Color   color_brightness( Color color, f32 factor );    // Get color with brightness correction, brightness factor goes from -1.0f to 1.0f
		RLAPI Color   color_contrast( Color color, f32 contrast );    // Get color with contrast correction, contrast values between -1.0f and 1.0f
		RLAPI Color   color_alpha( Color color, f32 alpha );          // Get color with alpha applied, alpha goes from 0.0f to 1.0f
		RLAPI Color   color_alpha_blend( Color dst, Color src, Color tint );       // Get src alpha-blended into dst color with tint
		RLAPI Color   get_color( u32 hexValue );                                   // Get Color structure from hexadecimal value
		RLAPI Color   get_pixel_color( void* srcPtr, s32 format );                 // Get Color from a source pixel pointer of certain format
		RLAPI void    set_pixel_color( void* dstPtr, Color color, s32 format );    // Set color formatted into destination pixel pointer
		RLAPI int     get_pixel_data_size( s32 width, s32 height, s32 format );    // Get pixel data size in bytes for certain format

		//------------------------------------------------------------------------------------
		// Font Loading and Text Drawing Functions (Module: text)
		//------------------------------------------------------------------------------------

		// Font loading/unloading functions
		RLAPI Font get_font_default( void );             // Get the default Font
		RLAPI Font load_font( char const* fileName );    // Load font from file into GPU memory (VRAM)
		RLAPI Font load_font_ex(
		    char const* fileName,
		    s32         fontSize,
		    s32*        codepoints,
		    s32         codepointCount
		);    // Load font from file with extended parameters, use NULL for codepoints and 0 for codepointCount to load the default character set
		RLAPI Font load_font_from_image( Image image, Color key, s32 firstChar );    // Load font from Image (XNA style)
		RLAPI Font load_font_from_memory(
		    char const*          fileType,
		    unsigned char const* fileData,
		    s32                  dataSize,
		    s32                  fontSize,
		    s32*                 codepoints,
		    s32                  codepointCount
		);                                        // Load font from memory buffer, fileType refers to extension: i.e. '.ttf'
		RLAPI bool is_font_ready( Font font );    // Check if a font is ready
		RLAPI GlyphInfo*
		    load_font_data( unsigned char const* fileData, s32 dataSize, s32 fontSize, s32* codepoints, s32 codepointCount, s32 type );    // Load font data for
		                                                                                                                                   // further use
		RLAPI Image gen_image_font_atlas(
		    GlyphInfo const* glyphs,
		    Rectangle**      glyphRecs,
		    s32              glyphCount,
		    s32              fontSize,
		    s32              padding,
		    s32              packMethod
		);                                                                    // Generate image font atlas using chars info
		RLAPI void unload_font_data( GlyphInfo* glyphs, s32 glyphCount );     // Unload font chars info data (RAM)
		RLAPI void unload_font( Font font );                                  // Unload font from GPU memory (VRAM)
		RLAPI bool export_font_as_code( Font font, char const* fileName );    // Export font as code file, returns true on success

		// Text drawing functions
		RLAPI void draw_fps( s32 posX, s32 posY );                                                                          // Draw current FPS
		RLAPI void draw_text( char const* text, s32 posX, s32 posY, s32 fontSize, Color color );                            // Draw text (using default font)
		RLAPI void draw_text_ex( Font font, char const* text, Vector2 position, f32 fontSize, f32 spacing, Color tint );    // Draw text using font and
		                                                                                                                    // additional parameters
		RLAPI void draw_text_pro(
		    Font        font,
		    char const* text,
		    Vector2     position,
		    Vector2     origin,
		    f32         rotation,
		    f32         fontSize,
		    f32         spacing,
		    Color       tint
		);    // Draw text using Font and pro parameters (rotation)
		RLAPI void draw_text_codepoint( Font font, s32 codepoint, Vector2 position, f32 fontSize, Color tint );    // Draw one character (codepoint)
		RLAPI void draw_text_codepoints(
		    Font       font,
		    int const* codepoints,
		    s32        codepointCount,
		    Vector2    position,
		    f32        fontSize,
		    f32        spacing,
		    Color      tint
		);    // Draw multiple character (codepoint)

		// Text font info functions
		RLAPI void    set_text_line_spacing( s32 spacing );                                         // Set vertical line spacing when drawing with line-breaks
		RLAPI int     measure_text( char const* text, s32 fontSize );                               // Measure string width for default font
		RLAPI Vector2 measure_text_ex( Font font, char const* text, f32 fontSize, f32 spacing );    // Measure string size for Font
		RLAPI int     get_glyph_index(
		        Font font,
		        s32  codepoint
		    );    // Get glyph index position in font for a codepoint (unicode character), fallback to '?' if not found
		RLAPI GlyphInfo
		    get_glyph_info( Font font, s32 codepoint );    // Get glyph font info data for a codepoint (unicode character), fallback to '?' if not found
		RLAPI Rectangle get_glyph_atlas_rec(
		    Font font,
		    s32  codepoint
		);    // Get glyph rectangle in font atlas for a codepoint (unicode character), fallback to '?' if not found

		// Text codepoints management functions (unicode characters)
		RLAPI char* load_utf8( int const* codepoints, s32 length );     // Load UTF-8 text encoded from codepoints array
		RLAPI void  unload_utf8( char* text );                          // Unload UTF-8 text encoded from codepoints array
		RLAPI int*  load_codepoints( char const* text, s32* count );    // Load all codepoints from a UTF-8 text string, codepoints count returned by parameter
		RLAPI void  unload_codepoints( s32* codepoints );               // Unload codepoints data from memory
		RLAPI int   get_codepoint_count( char const* text );            // Get total number of codepoints in a UTF-8 encoded string
		RLAPI int   get_codepoint( char const* text, s32* codepointSize );    // Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
		RLAPI int
		    get_codepoint_next( char const* text, s32* codepointSize );    // Get next codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
		RLAPI int get_codepoint_previous(
		    char const* text,
		    s32*        codepointSize
		);    // Get previous codepoint in a UTF-8 encoded string, 0x3f('?') is returned on failure
		RLAPI char const*
		    codepoint_to_utf8( s32 codepoint, s32* utf8Size );    // Encode one codepoint into UTF-8 byte array (array length returned as parameter)

		// Text strings management functions (no UTF-8 strings, only byte chars)
		// NOTE: Some strings allocate memory internally for returned strings, just be careful!
		RLAPI int          text_copy( char* dst, char const* src );                              // Copy one string to another, returns bytes copied
		RLAPI bool         text_is_equal( char const* text1, char const* text2 );                // Check if two text string are equal
		RLAPI unsigned int text_length( char const* text );                                      // Get text length, checks for '\0' ending
		RLAPI char const*  text_format( char const* text, ... );                                 // Text formatting with variables (sprintf() style)
		RLAPI char const*  text_subtext( char const* text, s32 position, s32 length );           // Get a piece of a text string
		RLAPI char*        text_replace( char* text, char const* replace, char const* by );      // Replace text string (WARNING: memory must be freed!)
		RLAPI char*        text_insert( char const* text, char const* insert, s32 position );    // Insert text in a position (WARNING: memory must be freed!)
		RLAPI char const*  text_join( char const** textList, s32 count, char const* delimiter );    // Join text strings with delimiter
		RLAPI char const** text_split( char const* text, char delimiter, s32* count );              // Split text into multiple strings
		RLAPI void         text_append( char* text, char const* append, s32* position );            // Append text at specific position and move cursor!
		RLAPI int          text_find_index( char const* text, char const* find );                   // Find first text occurrence within a string
		RLAPI char const*  text_to_upper( char const* text );                                       // Get upper case version of provided string
		RLAPI char const*  text_to_lower( char const* text );                                       // Get lower case version of provided string
		RLAPI char const*  text_to_pascal( char const* text );                                      // Get Pascal case notation version of provided string
		RLAPI int          text_to_integer( char const* text );    // Get integer value from text (negative values not supported)

		//------------------------------------------------------------------------------------
		// Basic 3d Shapes Drawing Functions (Module: models)
		//------------------------------------------------------------------------------------

		// Basic geometric 3D shapes drawing functions
		RLAPI void draw_line_3d( Vector3 startPos, Vector3 endPos, Color color );    // Draw a line in 3D world space
		RLAPI void draw_point_3d( Vector3 position, Color color );                   // Draw a point in 3D space, actually a small line
		RLAPI void draw_circle_3d( Vector3 center, f32 radius, Vector3 rotationAxis, f32 rotationAngle, Color color );    // Draw a circle in 3D world space
		RLAPI void draw_triangle_3d( Vector3 v1, Vector3 v2, Vector3 v3, Color color );    // Draw a color-filled triangle (vertex in counter-clockwise order!)
		RLAPI void draw_triangle_strip_3d( Vector3* points, s32 pointCount, Color color );                    // Draw a triangle strip defined by points
		RLAPI void draw_cube( Vector3 position, f32 width, f32 height, f32 length, Color color );             // Draw cube
		RLAPI void draw_cube_v( Vector3 position, Vector3 size, Color color );                                // Draw cube (Vector version)
		RLAPI void draw_cube_wires( Vector3 position, f32 width, f32 height, f32 length, Color color );       // Draw cube wires
		RLAPI void draw_cube_wires_v( Vector3 position, Vector3 size, Color color );                          // Draw cube wires (Vector version)
		RLAPI void draw_sphere( Vector3 centerPos, f32 radius, Color color );                                 // Draw sphere
		RLAPI void draw_sphere_ex( Vector3 centerPos, f32 radius, s32 rings, s32 slices, Color color );       // Draw sphere with extended parameters
		RLAPI void draw_sphere_wires( Vector3 centerPos, f32 radius, s32 rings, s32 slices, Color color );    // Draw sphere wires
		RLAPI void draw_cylinder( Vector3 position, f32 radiusTop, f32 radiusBottom, f32 height, s32 slices, Color color );    // Draw a cylinder/cone
		RLAPI void
		    draw_cylinder_ex( Vector3 startPos, Vector3 endPos, f32 startRadius, f32 endRadius, s32 sides, Color color );    // Draw a cylinder with base at
		                                                                                                                     // startPos and top at endPos
		RLAPI void
		    draw_cylinder_wires( Vector3 position, f32 radiusTop, f32 radiusBottom, f32 height, s32 slices, Color color );    // Draw a cylinder/cone wires
		RLAPI void draw_cylinder_wires_ex(
		    Vector3 startPos,
		    Vector3 endPos,
		    f32     startRadius,
		    f32     endRadius,
		    s32     sides,
		    Color   color
		);    // Draw a cylinder wires with base at startPos and top at endPos
		RLAPI void draw_capsule( Vector3 startPos, Vector3 endPos, f32 radius, s32 slices, s32 rings, Color color );    // Draw a capsule with the center of its
		                                                                                                                // sphere caps at startPos and endPos
		RLAPI void draw_capsule_wires(
		    Vector3 startPos,
		    Vector3 endPos,
		    f32     radius,
		    s32     slices,
		    s32     rings,
		    Color   color
		);    // Draw capsule wireframe with the center of its sphere caps at startPos and endPos
		RLAPI void draw_plane( Vector3 centerPos, Vector2 size, Color color );    // Draw a plane XZ
		RLAPI void draw_ray( Ray ray, Color color );                              // Draw a ray line
		RLAPI void draw_grid( s32 slices, f32 spacing );                          // Draw a grid (centered at (0, 0, 0))

		//------------------------------------------------------------------------------------
		// Model 3d Loading and Drawing Functions (Module: models)
		//------------------------------------------------------------------------------------

		// Model management functions
		RLAPI Model       load_model( char const* fileName );       // Load model from files (meshes and materials)
		RLAPI Model       load_model_from_mesh( Mesh mesh );        // Load model from generated mesh (default material)
		RLAPI bool        is_model_ready( Model model );            // Check if a model is ready
		RLAPI void        unload_model( Model model );              // Unload model (including meshes) from memory (RAM and/or VRAM)
		RLAPI BoundingBox get_model_bounding_box( Model model );    // Compute model bounding box limits (considers all meshes)

		// Model drawing functions
		RLAPI void draw_model( Model model, Vector3 position, f32 scale, Color tint );    // Draw a model (with texture if set)
		RLAPI void draw_model_ex( Model model, Vector3 position, Vector3 rotationAxis, f32 rotationAngle, Vector3 scale, Color tint );    // Draw a model with
		                                                                                                                                  // extended parameters
		RLAPI void draw_model_wires( Model model, Vector3 position, f32 scale, Color tint );    // Draw a model wires (with texture if set)
		RLAPI void draw_model_wires_ex(
		    Model   model,
		    Vector3 position,
		    Vector3 rotationAxis,
		    f32     rotationAngle,
		    Vector3 scale,
		    Color   tint
		);                                                               // Draw a model wires (with texture if set) with extended parameters
		RLAPI void draw_bounding_box( BoundingBox box, Color color );    // Draw bounding box (wires)
		RLAPI void draw_billboard( Camera camera, Texture2D texture, Vector3 position, f32 size, Color tint );    // Draw a billboard texture
		RLAPI void
		    draw_billboard_rec( Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector2 size, Color tint );    // Draw a billboard texture
		                                                                                                                             // defined by source
		RLAPI void draw_billboard_pro(
		    Camera    camera,
		    Texture2D texture,
		    Rectangle source,
		    Vector3   position,
		    Vector3   up,
		    Vector2   size,
		    Vector2   origin,
		    f32       rotation,
		    Color     tint
		);    // Draw a billboard texture defined by source and rotation

		// Mesh management functions
		RLAPI void upload_mesh( Mesh* mesh, bool dynamic );    // Upload mesh vertex data in GPU and provide VAO/VBO ids
		RLAPI void update_mesh_buffer( Mesh mesh, s32 index, void const* data, s32 dataSize, s32 offset );    // Update mesh vertex data in GPU for a specific
		                                                                                                      // buffer index
		RLAPI void unload_mesh( Mesh mesh );                                                                  // Unload mesh data from CPU and GPU
		RLAPI void draw_mesh( Mesh mesh, Material material, Matrix transform );                               // Draw a 3d mesh with material and transform
		RLAPI void draw_mesh_instanced( Mesh mesh, Material material, Matrix const* transforms, s32 instances );    // Draw multiple mesh instances with
		                                                                                                            // material and different transforms
		RLAPI bool        export_mesh( Mesh mesh, char const* fileName );    // Export mesh data to file, returns true on success
		RLAPI BoundingBox get_mesh_bounding_box( Mesh mesh );                // Compute mesh bounding box limits
		RLAPI void        gen_mesh_tangents( Mesh* mesh );                   // Compute mesh tangents

		// Mesh generation functions
		RLAPI Mesh gen_mesh_poly( s32 sides, f32 radius );                           // Generate polygonal mesh
		RLAPI Mesh gen_mesh_plane( f32 width, f32 length, s32 resX, s32 resZ );      // Generate plane mesh (with subdivisions)
		RLAPI Mesh gen_mesh_cube( f32 width, f32 height, f32 length );               // Generate cuboid mesh
		RLAPI Mesh gen_mesh_sphere( f32 radius, s32 rings, s32 slices );             // Generate sphere mesh (standard sphere)
		RLAPI Mesh gen_mesh_hemi_sphere( f32 radius, s32 rings, s32 slices );        // Generate half-sphere mesh (no bottom cap)
		RLAPI Mesh gen_mesh_cylinder( f32 radius, f32 height, s32 slices );          // Generate cylinder mesh
		RLAPI Mesh gen_mesh_cone( f32 radius, f32 height, s32 slices );              // Generate cone/pyramid mesh
		RLAPI Mesh gen_mesh_torus( f32 radius, f32 size, s32 radSeg, s32 sides );    // Generate torus mesh
		RLAPI Mesh gen_mesh_knot( f32 radius, f32 size, s32 radSeg, s32 sides );     // Generate trefoil knot mesh
		RLAPI Mesh gen_mesh_heightmap( Image heightmap, Vector3 size );              // Generate heightmap mesh from image data
		RLAPI Mesh gen_mesh_cubicmap( Image cubicmap, Vector3 cubeSize );            // Generate cubes-based map mesh from image data

		// Material loading/unloading functions
		RLAPI Material* load_materials( char const* fileName, s32* materialCount );    // Load materials from model file
		RLAPI Material  load_material_default( void );                                 // Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
		RLAPI bool      is_material_ready( Material material );                        // Check if a material is ready
		RLAPI void      unload_material( Material material );                          // Unload material from GPU memory (VRAM)
		RLAPI void      set_material_texture(
		         Material* material,
		         s32       mapType,
		         Texture2D texture
		     );    // Set texture for a material map type (MATERIAL_MAP_DIFFUSE, MATERIAL_MAP_SPECULAR...)
		RLAPI void set_model_mesh_material( Model* model, s32 meshId, s32 materialId );    // Set material for a mesh

		// Model animations loading/unloading functions
		RLAPI ModelAnimation* load_model_animations( char const* fileName, s32* animCount );            // Load model animations from file
		RLAPI void            update_model_animation( Model model, ModelAnimation anim, s32 frame );    // Update model animation pose
		RLAPI void            unload_model_animation( ModelAnimation anim );                            // Unload animation data
		RLAPI void            unload_model_animations( ModelAnimation* animations, s32 animCount );     // Unload animation array data
		RLAPI bool            is_model_animation_valid( Model model, ModelAnimation anim );             // Check model animation skeleton match

		// Collision detection functions
		RLAPI bool         check_collision_spheres( Vector3 center1, f32 radius1, Vector3 center2, f32 radius2 );    // Check collision between two spheres
		RLAPI bool         check_collision_boxes( BoundingBox box1, BoundingBox box2 );                          // Check collision between two bounding boxes
		RLAPI bool         check_collision_box_sphere( BoundingBox box, Vector3 center, f32 radius );            // Check collision between box and sphere
		RLAPI RayCollision get_ray_collision_sphere( Ray ray, Vector3 center, f32 radius );                      // Get collision info between ray and sphere
		RLAPI RayCollision get_ray_collision_box( Ray ray, BoundingBox box );                                    // Get collision info between ray and box
		RLAPI RayCollision get_ray_collision_mesh( Ray ray, Mesh mesh, Matrix transform );                       // Get collision info between ray and mesh
		RLAPI RayCollision get_ray_collision_triangle( Ray ray, Vector3 p1, Vector3 p2, Vector3 p3 );            // Get collision info between ray and triangle
		RLAPI RayCollision get_ray_collision_quad( Ray ray, Vector3 p1, Vector3 p2, Vector3 p3, Vector3 p4 );    // Get collision info between ray and quad

		//------------------------------------------------------------------------------------
		// Audio Loading and Playing Functions (Module: audio)
		//------------------------------------------------------------------------------------
		typedef void ( *AudioCallback )( void* bufferData, unsigned int frames );

		// Audio device management functions
		RLAPI void  init_audio_device( void );          // Initialize audio device and context
		RLAPI void  close_audio_device( void );         // Close the audio device and context
		RLAPI bool  is_audio_device_ready( void );      // Check if audio device has been initialized successfully
		RLAPI void  set_master_volume( f32 volume );    // Set master volume (listener)
		RLAPI float get_master_volume( void );          // Get master volume (listener)

		// Wave/Sound loading/unloading functions
		RLAPI Wave load_wave( char const* fileName );    // Load wave data from file
		RLAPI Wave load_wave_from_memory(
		    char const*          fileType,
		    unsigned char const* fileData,
		    s32                  dataSize
		);                                                 // Load wave from memory buffer, fileType refers to extension: i.e. '.wav'
		RLAPI bool  is_wave_ready( Wave wave );            // Checks if wave data is ready
		RLAPI Sound load_sound( char const* fileName );    // Load sound from file
		RLAPI Sound load_sound_from_wave( Wave wave );     // Load sound from wave data
		RLAPI Sound load_sound_alias( Sound source );    // Create a new sound that shares the same sample data as the source sound, does not own the sound data
		RLAPI bool  is_sound_ready( Sound sound );       // Checks if a sound is ready
		RLAPI void  update_sound( Sound sound, void const* data, s32 sampleCount );    // Update sound buffer with new data
		RLAPI void  unload_wave( Wave wave );                                          // Unload wave data
		RLAPI void  unload_sound( Sound sound );                                       // Unload sound
		RLAPI void  unload_sound_alias( Sound alias );                                 // Unload a sound alias (does not deallocate sample data)
		RLAPI bool  export_wave( Wave wave, char const* fileName );                    // Export wave data to file, returns true on success
		RLAPI bool  export_wave_as_code( Wave wave, char const* fileName );            // Export wave sample data to code (.h), returns true on success

		// Wave/Sound management functions
		RLAPI void   play_sound( Sound sound );                                                  // Play a sound
		RLAPI void   stop_sound( Sound sound );                                                  // Stop playing a sound
		RLAPI void   pause_sound( Sound sound );                                                 // Pause a sound
		RLAPI void   resume_sound( Sound sound );                                                // Resume a paused sound
		RLAPI bool   is_sound_playing( Sound sound );                                            // Check if a sound is currently playing
		RLAPI void   set_sound_volume( Sound sound, f32 volume );                                // Set volume for a sound (1.0 is max level)
		RLAPI void   set_sound_pitch( Sound sound, f32 pitch );                                  // Set pitch for a sound (1.0 is base level)
		RLAPI void   set_sound_pan( Sound sound, f32 pan );                                      // Set pan for a sound (0.5 is center)
		RLAPI Wave   wave_copy( Wave wave );                                                     // Copy a wave to a new wave
		RLAPI void   wave_crop( Wave* wave, s32 initSample, s32 finalSample );                   // Crop a wave to defined samples range
		RLAPI void   wave_format( Wave* wave, s32 sampleRate, s32 sampleSize, s32 channels );    // Convert wave data to desired format
		RLAPI float* load_wave_samples( Wave wave );                                             // Load samples data from wave as a 32bit float data array
		RLAPI void   unload_wave_samples( f32* samples );                                        // Unload samples data loaded with LoadWaveSamples()

		// Music management functions
		RLAPI Music load_music_stream( char const* fileName );                                                         // Load music stream from file
		RLAPI Music load_music_stream_from_memory( char const* fileType, unsigned char const* data, s32 dataSize );    // Load music stream from data
		RLAPI bool  is_music_ready( Music music );                                                                     // Checks if a music stream is ready
		RLAPI void  unload_music_stream( Music music );                                                                // Unload music stream
		RLAPI void  play_music_stream( Music music );                                                                  // Start music playing
		RLAPI bool  is_music_stream_playing( Music music );                                                            // Check if music is playing
		RLAPI void  update_music_stream( Music music );                                                                // Updates buffers for music streaming
		RLAPI void  stop_music_stream( Music music );                                                                  // Stop music playing
		RLAPI void  pause_music_stream( Music music );                                                                 // Pause music playing
		RLAPI void  resume_music_stream( Music music );                                                                // Resume playing paused music
		RLAPI void  seek_music_stream( Music music, f32 position );                                                    // Seek music to a position (in seconds)
		RLAPI void  set_music_volume( Music music, f32 volume );    // Set volume for music (1.0 is max level)
		RLAPI void  set_music_pitch( Music music, f32 pitch );      // Set pitch for a music (1.0 is base level)
		RLAPI void  set_music_pan( Music music, f32 pan );          // Set pan for a music (0.5 is center)
		RLAPI float get_music_time_length( Music music );           // Get music time length (in seconds)
		RLAPI float get_music_time_played( Music music );           // Get current music time played (in seconds)

		// AudioStream management functions
		RLAPI AudioStream load_audio_stream( u32 sampleRate, u32 sampleSize, u32 channels );              // Load audio stream (to stream raw audio pcm data)
		RLAPI bool        is_audio_stream_ready( AudioStream stream );                                    // Checks if an audio stream is ready
		RLAPI void        unload_audio_stream( AudioStream stream );                                      // Unload audio stream and free memory
		RLAPI void        update_audio_stream( AudioStream stream, void const* data, s32 frameCount );    // Update audio stream buffers with data
		RLAPI bool        is_audio_stream_processed( AudioStream stream );                                // Check if any audio stream buffers requires refill
		RLAPI void        play_audio_stream( AudioStream stream );                                        // Play audio stream
		RLAPI void        pause_audio_stream( AudioStream stream );                                       // Pause audio stream
		RLAPI void        resume_audio_stream( AudioStream stream );                                      // Resume audio stream
		RLAPI bool        is_audio_stream_playing( AudioStream stream );                                  // Check if audio stream is playing
		RLAPI void        stop_audio_stream( AudioStream stream );                                        // Stop audio stream
		RLAPI void        set_audio_stream_volume( AudioStream stream, f32 volume );                      // Set volume for audio stream (1.0 is max level)
		RLAPI void        set_audio_stream_pitch( AudioStream stream, f32 pitch );                        // Set pitch for audio stream (1.0 is base level)
		RLAPI void        set_audio_stream_pan( AudioStream stream, f32 pan );                            // Set pan for audio stream (0.5 is centered)
		RLAPI void        set_audio_stream_buffer_size_default( s32 size );                               // Default size for new audio streams
		RLAPI void        set_audio_stream_callback( AudioStream stream, AudioCallback callback );        // Audio thread callback to request new data

		RLAPI void attach_audio_stream_processor(
		    AudioStream   stream,
		    AudioCallback processor
		);    // Attach audio stream processor to stream, receives the samples as <float>s
		RLAPI void detach_audio_stream_processor( AudioStream stream, AudioCallback processor );    // Detach audio stream processor from stream

		RLAPI void attach_audio_mixed_processor( AudioCallback processor
		);    // Attach audio stream processor to the entire audio pipeline, receives the samples as <float>s
		RLAPI void detach_audio_mixed_processor( AudioCallback processor );    // Detach audio stream processor from the entire audio pipeline

#if defined( __cplusplus )
	}
}
#endif

#endif
// RAYLIB_H
