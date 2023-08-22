#pragma once
#include "macros.hpp"

#pragma region Basic Types

#define GEN_U8_MIN 0u
#define GEN_U8_MAX 0xffu
#define GEN_I8_MIN ( -0x7f - 1 )
#define GEN_I8_MAX 0x7f

#define GEN_U16_MIN 0u
#define GEN_U16_MAX 0xffffu
#define GEN_I16_MIN ( -0x7fff - 1 )
#define GEN_I16_MAX 0x7fff

#define GEN_U32_MIN 0u
#define GEN_U32_MAX 0xffffffffu
#define GEN_I32_MIN ( -0x7fffffff - 1 )
#define GEN_I32_MAX 0x7fffffff

#define GEN_U64_MIN 0ull
#define GEN_U64_MAX 0xffffffffffffffffull
#define GEN_I64_MIN ( -0x7fffffffffffffffll - 1 )
#define GEN_I64_MAX 0x7fffffffffffffffll

#if defined( GEN_ARCH_32_BIT )
#	define GEN_USIZE_MIN GEN_U32_MIN
#	define GEN_USIZE_MAX GEN_U32_MAX
#	define GEN_ISIZE_MIN GEN_S32_MIN
#	define GEN_ISIZE_MAX GEN_S32_MAX
#elif defined( GEN_ARCH_64_BIT )
#	define GEN_USIZE_MIN GEN_U64_MIN
#	define GEN_USIZE_MAX GEN_U64_MAX
#	define GEN_ISIZE_MIN GEN_I64_MIN
#	define GEN_ISIZE_MAX GEN_I64_MAX
#else
#	error Unknown architecture size. This library only supports 32 bit and 64 bit architectures.
#endif

#define GEN_F32_MIN 1.17549435e-38f
#define GEN_F32_MAX 3.40282347e+38f
#define GEN_F64_MIN 2.2250738585072014e-308
#define GEN_F64_MAX 1.7976931348623157e+308

#if defined( GEN_COMPILER_MSVC )
#	if _MSC_VER < 1300
typedef unsigned char  u8;
typedef signed char    s8;
typedef unsigned short u16;
typedef signed short   s16;
typedef unsigned int   u32;
typedef signed int     s32;
#    else
typedef unsigned __int8  u8;
typedef signed __int8    s8;
typedef unsigned __int16 u16;
typedef signed __int16   s16;
typedef unsigned __int32 u32;
typedef signed __int32   s32;
#    endif
typedef unsigned __int64 u64;
typedef signed __int64   s64;
#else
#	include <stdint.h>

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;
#endif

static_assert( sizeof( u8 )  == sizeof( s8 ),  "sizeof(u8) != sizeof(s8)" );
static_assert( sizeof( u16 ) == sizeof( s16 ), "sizeof(u16) != sizeof(s16)" );
static_assert( sizeof( u32 ) == sizeof( s32 ), "sizeof(u32) != sizeof(s32)" );
static_assert( sizeof( u64 ) == sizeof( s64 ), "sizeof(u64) != sizeof(s64)" );

static_assert( sizeof( u8 )  == 1, "sizeof(u8) != 1" );
static_assert( sizeof( u16 ) == 2, "sizeof(u16) != 2" );
static_assert( sizeof( u32 ) == 4, "sizeof(u32) != 4" );
static_assert( sizeof( u64 ) == 8, "sizeof(u64) != 8" );

typedef size_t    uw;
typedef ptrdiff_t sw;

static_assert( sizeof( uw ) == sizeof( sw ), "sizeof(uw) != sizeof(sw)" );

// NOTE: (u)zpl_intptr is only here for semantic reasons really as this library will only support 32/64 bit OSes.
#if defined( _WIN64 )
typedef signed __int64   sptr;
typedef unsigned __int64 uptr;
#elif defined( _WIN32 )
// NOTE; To mark types changing their size, e.g. zpl_intptr
#	ifndef _W64
#		if ! defined( __midl ) && ( defined( _X86_ ) || defined( _M_IX86 ) ) && _MSC_VER >= 1300
#			define _W64 __w64
#		else
#			define _W64
#		endif
#	endif
typedef _W64 signed int   sptr;
typedef _W64 unsigned int uptr;
#else
typedef uintptr_t uptr;
typedef intptr_t  sptr;
#endif

static_assert( sizeof( uptr ) == sizeof( sptr ), "sizeof(uptr) != sizeof(sptr)" );

typedef float  f32;
typedef double f64;

static_assert( sizeof( f32 ) == 4, "sizeof(f32) != 4" );
static_assert( sizeof( f64 ) == 8, "sizeof(f64) != 8" );

typedef s8  b8;
typedef s16 b16;
typedef s32 b32;

#pragma endregion Basic Types
