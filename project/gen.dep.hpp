// This file is intended to be included within gen.hpp (There is pragma diagnostic ignores)
#pragma once

#pragma region Platform Detection

/* Platform architecture */

#if defined( _WIN64 ) || defined( __x86_64__ ) || defined( _M_X64 ) || defined( __64BIT__ ) || defined( __powerpc64__ ) || defined( __ppc64__ ) || defined( __aarch64__ )
#	ifndef GEN_ARCH_64_BIT
#		define GEN_ARCH_64_BIT 1
#	endif
#else
#	ifndef GEN_ARCH_32_BItxt_StrCaT
#		define GEN_ARCH_32_BIT 1
#	endif
#endif

/* Platform OS */

#if defined( _WIN32 ) || defined( _WIN64 )
#	ifndef GEN_SYSTEM_WINDOWS
#		define GEN_SYSTEM_WINDOWS 1
#	endif
#elif defined( __APPLE__ ) && defined( __MACH__ )
#	ifndef GEN_SYSTEM_OSX
#		define GEN_SYSTEM_OSX 1
#	endif
#	ifndef GEN_SYSTEM_MACOS
#		define GEN_SYSTEM_MACOS 1
#	endif
#	include <TargetConditionals.h>
#	if TARGET_IPHONE_SIMULATOR == 1 || TARGET_OS_IPHONE == 1
#		ifndef GEN_SYSTEM_IOS
#			define GEN_SYSTEM_IOS 1
#		endif
#	endif
#elif defined( __unix__ )
#	ifndef GEN_SYSTEM_UNIX
#		define GEN_SYSTEM_UNIX 1
#	endif
#	if defined( ANDROID ) || defined( __ANDROID__ )
#		ifndef GEN_SYSTEM_ANDROID
#			define GEN_SYSTEM_ANDROID 1
#		endif
#		ifndef GEN_SYSTEM_LINUX
#			define GEN_SYSTEM_LINUX 1
#		endif
#	elif defined( __linux__ )
#		ifndef GEN_SYSTEM_LINUX
#			define GEN_SYSTEM_LINUX 1
#		endif
#	elif defined( __FreeBSD__ ) || defined( __FreeBSD_kernel__ )
#		ifndef GEN_SYSTEM_FREEBSD
#			define GEN_SYSTEM_FREEBSD 1
#		endif
#	elif defined( __OpenBSD__ )
#		ifndef GEN_SYSTEM_OPENBSD
#			define GEN_SYSTEM_OPENBSD 1
#		endif
#	elif defined( __EMSCRIPTEN__ )
#		ifndef GEN_SYSTEM_EMSCRIPTEN
#			define GEN_SYSTEM_EMSCRIPTEN 1
#		endif
#	elif defined( __CYGWIN__ )
#		ifndef GEN_SYSTEM_CYGWIN
#			define GEN_SYSTEM_CYGWIN 1
#		endif
#	else
#		error This UNIX operating system is not supported
#	endif
#else
#	error This operating system is not supported
#endif

/* Platform compiler */

#if defined( _MSC_VER )
#	define GEN_COMPILER_MSVC 1
#elif defined( __GNUC__ )
#	define GEN_COMPILER_GCC 1
#elif defined( __clang__ )
#	define GEN_COMPILER_CLANG 1
#elif defined( __MINGW32__ )
#	define GEN_COMPILER_MINGW 1
#	error Unknown compiler
#endif

#if defined( __has_attribute )
#	define GEN_HAS_ATTRIBUTE( attribute ) __has_attribute( attribute )
#else
#	define GEN_HAS_ATTRIBUTE( attribute ) ( 0 )
#endif

#if defined(GEN_GCC_VERSION_CHECK)
#  undef GEN_GCC_VERSION_CHECK
#endif
#if defined(GEN_GCC_VERSION)
#  define GEN_GCC_VERSION_CHECK(major,minor,patch) (GEN_GCC_VERSION >= GEN_VERSION_ENCODE(major, minor, patch))
#else
#  define GEN_GCC_VERSION_CHECK(major,minor,patch) (0)
#endif

#define GEN_DEF_INLINE  static
#define GEN_IMPL_INLINE static inline

#ifdef GEN_COMPILER_MSVC
#	define forceinline __forceinline
#	define neverinline __declspec( noinline )
#elif defined(GEN_COMPILER_GCC)
#	define forceinline inline __attribute__((__always_inline__))
#	define neverinline __attribute__( ( __noinline__ ) )
#elif defined(GEN_COMPILER_CLANG)
#if __has_attribute(__always_inline__)
#	define forceinline inline __attribute__((__always_inline__))
#	define neverinline __attribute__( ( __noinline__ ) )
#else
#	define forceinline
#	define neverinline
#endif
#else
#	define forceinline
#	define neverinline
#endif

#pragma endregion Platform Detection

#pragma region Mandatory Includes
#	include <stdarg.h>
#	include <stddef.h>

#	if defined( GEN_SYSTEM_WINDOWS )
#		include <intrin.h>
#	endif
#pragma endregion Mandatory Includes

namespace gen {

#pragma region Macros
#define zpl_cast( Type ) ( Type )

// Keywords

#define global        static    // Global variables
#define internal      static    // Internal linkage
#define local_persist static    // Local Persisting variables

// Bits

#define bit( Value )                             ( 1 << Value )
#define bitfield_is_equal( Type, Field, Mask ) ( (Type(Mask) & Type(Field)) == Type(Mask) )

// Casting
#define ccast( Type, Value ) ( * const_cast< Type* >( & (Value) ) )
#define pcast( Type, Value ) ( * reinterpret_cast< Type* >( & ( Value ) ) )
#define rcast( Type, Value ) reinterpret_cast< Type >( Value )
#define scast( Type, Value ) static_cast< Type >( Value )

// Num Arguments (Varadics)
#if defined(__GNUC__) || defined(__clang__)
	// Supports 0-10 arguments
	#define num_args_impl( _0,                            \
		_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,  \
		_11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
		N, ...                                            \
	) N
	// _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
	// _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
	// _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,

	// ## deletes preceding comma if _VA_ARGS__ is empty (GCC, Clang)
	#define num_args(...)                       \
	num_args_impl(_, ## __VA_ARGS__,            \
		20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
		10,  9,  8,  7,  6,  5,  4,  3,  2,  1, \
		0                                       \
	)
	// 50, 49, 48, 47, 46, 45, 44, 43, 42, 41,              \
	// 40, 39, 38, 37, 36, 35, 34, 33, 32, 31,              \
	// 30, 29, 28, 27, 26, 25, 24, 23, 22, 21,
#else
	// Supports 1-10 arguments
	#define num_args_impl(                                \
		_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,  \
		_11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
		N, ...                                            \
	) N

	#define num_args(...)                       \
	num_args_impl( __VA_ARGS__,                 \
		20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
		10,  9,  8,  7,  6,  5,  4,  3,  2,  1  \
	)
#endif

// Stringizing
#define stringize_va( ... ) #__VA_ARGS__
#define stringize( ... )    stringize_va( __VA_ARGS__ )

// Function do once

#define do_once()      \
do                     \
{                      \
	static             \
	bool Done = false; \
	if ( Done )        \
		return;        \
	Done = true;       \
}                      \
while(0)

#define do_once_start  \
do                     \
{                      \
	static             \
	bool Done = false; \
	if ( Done )        \
		break;         \
	Done = true;

#define do_once_end    \
}                      \
while(0);

#define clamp( x, lower, upper )      min( max( ( x ), ( lower ) ), ( upper ) )
#define count_of( x )                 ( ( size_of( x ) / size_of( 0 [ x ] ) ) / ( ( sw )( ! ( size_of( x ) % size_of( 0 [ x ] ) ) ) ) )
#define is_between( x, lower, upper ) ( ( ( lower ) <= ( x ) ) && ( ( x ) <= ( upper ) ) )
#define max( a, b )                   ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define min( a, b )                   ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define size_of( x )                  ( sw )( sizeof( x ) )

template< class Type >
void swap( Type a, Type b )
{
	Type tmp = a;
	a = b;
	b = tmp;
}
#pragma endregion Macros

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

#pragma region Debug
#if defined( _MSC_VER )
#	if _MSC_VER < 1300
#		define GEN_DEBUG_TRAP() __asm int 3 /* Trap to debugger! */
#	else
#		define GEN_DEBUG_TRAP() __debugbreak()
#	endif
#elif defined( GEN_COMPILER_TINYC )
#	define GEN_DEBUG_TRAP() process_exit( 1 )
#else
#	define GEN_DEBUG_TRAP() __builtin_trap()
#endif

#define GEN_ASSERT( cond ) GEN_ASSERT_MSG( cond, NULL )

#define GEN_ASSERT_MSG( cond, msg, ... )                                                     \
	do                                                                                       \
	{                                                                                        \
		if ( ! ( cond ) )                                                                    \
		{                                                                                    \
			assert_handler( #cond, __FILE__, zpl_cast( s64 ) __LINE__, msg, ##__VA_ARGS__ ); \
			GEN_DEBUG_TRAP();                                                                \
		}                                                                                    \
	} while ( 0 )

#define GEN_ASSERT_NOT_NULL( ptr ) GEN_ASSERT_MSG( ( ptr ) != NULL, #ptr " must not be NULL" )

// NOTE: Things that shouldn't happen with a message!
#define GEN_PANIC( msg, ... ) GEN_ASSERT_MSG( 0, msg, ##__VA_ARGS__ )

void assert_handler( char const* condition, char const* file, s32 line, char const* msg, ... );
s32  assert_crash( char const* condition );
void process_exit( u32 code );
#pragma endregion Debug

#pragma region Memory
#define kilobytes( x ) ( ( x ) * ( s64 )( 1024 ) )
#define megabytes( x ) ( kilobytes( x ) * ( s64 )( 1024 ) )
#define gigabytes( x ) ( megabytes( x ) * ( s64 )( 1024 ) )
#define terabytes( x ) ( gigabytes( x ) * ( s64 )( 1024 ) )

#define GEN__ONES          ( zpl_cast( uw ) - 1 / GEN_U8_MAX )
#define GEN__HIGHS         ( GEN__ONES * ( GEN_U8_MAX / 2 + 1 ) )
#define GEN__HAS_ZERO( x ) ( ( ( x )-GEN__ONES ) & ~( x )&GEN__HIGHS )

//! Checks if value is power of 2.
GEN_DEF_INLINE b32 is_power_of_two( sw x );

//! Aligns address to specified alignment.
GEN_DEF_INLINE void* align_forward( void* ptr, sw alignment );

//! Aligns value to a specified alignment.
GEN_DEF_INLINE s64 align_forward_i64( s64 value, sw alignment );

//! Moves pointer forward by bytes.
GEN_DEF_INLINE void* pointer_add( void* ptr, sw bytes );

//! Moves pointer forward by bytes.
GEN_DEF_INLINE void const* pointer_add_const( void const* ptr, sw bytes );

//! Calculates difference between two addresses.
GEN_DEF_INLINE sw pointer_diff( void const* begin, void const* end );

//! Copy non-overlapping memory from source to destination.
void* mem_copy( void* dest, void const* source, sw size );

//! Search for a constant value within the size limit at memory location.
void const* mem_find( void const* data, u8 byte_value, sw size );

//! Copy memory from source to destination.
GEN_DEF_INLINE void* mem_move( void* dest, void const* source, sw size );

//! Set constant value at memory location with specified size.
GEN_DEF_INLINE void* mem_set( void* data, u8 byte_value, sw size );

//! @param ptr Memory location to clear up.
//! @param size The size to clear up with.
GEN_DEF_INLINE void zero_size( void* ptr, sw size );

//! Clears up an item.
#define zero_item( t ) zero_size( ( t ), size_of( *( t ) ) )    // NOTE: Pass pointer of struct

//! Clears up an array.
#define zero_array( a, count ) zero_size( ( a ), size_of( *( a ) ) * count )

enum AllocType : u8
{
	EAllocation_ALLOC,
	EAllocation_FREE,
	EAllocation_FREE_ALL,
	EAllocation_RESIZE,
};

using AllocatorProc = void* ( void* allocator_data, AllocType type
	, sw size, sw alignment
	, void* old_memory, sw old_size
	, u64 flags );

struct AllocatorInfo
{
	AllocatorProc* Proc;
	void*          Data;
};

enum AllocFlag
{
	ALLOCATOR_FLAG_CLEAR_TO_ZERO = bit( 0 ),
};

#ifndef GEN_DEFAULT_MEMORY_ALIGNMENT
#	define GEN_DEFAULT_MEMORY_ALIGNMENT ( 2 * size_of( void* ) )
#endif

#ifndef GEN_DEFAULT_ALLOCATOR_FLAGS
#	define GEN_DEFAULT_ALLOCATOR_FLAGS ( ALLOCATOR_FLAG_CLEAR_TO_ZERO )
#endif

//! Allocate memory with default alignment.
GEN_DEF_INLINE void* alloc( AllocatorInfo a, sw size );

//! Allocate memory with specified alignment.
GEN_DEF_INLINE void* alloc_align( AllocatorInfo a, sw size, sw alignment );

//! Free allocated memory.
GEN_DEF_INLINE void free( AllocatorInfo a, void* ptr );

//! Free all memory allocated by an allocator.
GEN_DEF_INLINE void free_all( AllocatorInfo a );

//! Resize an allocated memory.
GEN_DEF_INLINE void* resize( AllocatorInfo a, void* ptr, sw old_size, sw new_size );

//! Resize an allocated memory with specified alignment.
GEN_DEF_INLINE void* resize_align( AllocatorInfo a, void* ptr, sw old_size, sw new_size, sw alignment );

//! Allocate memory for an item.
#define alloc_item( allocator_, Type ) ( Type* )alloc( allocator_, size_of( Type ) )

//! Allocate memory for an array of items.
#define alloc_array( allocator_, Type, count ) ( Type* )alloc( allocator_, size_of( Type ) * ( count ) )

/* heap memory analysis tools */
/* define GEN_HEAP_ANALYSIS to enable this feature */
/* call zpl_heap_stats_init at the beginning of the entry point */
/* you can call zpl_heap_stats_check near the end of the execution to validate any possible leaks */
void heap_stats_init( void );
sw   heap_stats_used_memory( void );
sw   heap_stats_alloc_count( void );
void heap_stats_check( void );

//! Allocate/Resize memory using default options.

//! Use this if you don't need a "fancy" resize allocation
GEN_DEF_INLINE void* default_resize_align( AllocatorInfo a, void* ptr, sw old_size, sw new_size, sw alignment );

void* heap_allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags );

//! The heap allocator backed by operating system's memory manager.
constexpr AllocatorInfo heap( void ) { return { heap_allocator_proc, nullptr }; }

//! Helper to allocate memory using heap allocator.
#define malloc( sz ) alloc( heap(), sz )

//! Helper to free memory allocated by heap allocator.
#define mfree( ptr ) free( heap(), ptr )

GEN_IMPL_INLINE b32 is_power_of_two( sw x )
{
	if ( x <= 0 )
		return false;
	return ! ( x & ( x - 1 ) );
}

GEN_IMPL_INLINE void* align_forward( void* ptr, sw alignment )
{
	uptr p;

	GEN_ASSERT( is_power_of_two( alignment ) );

	p = zpl_cast( uptr ) ptr;
	return zpl_cast( void* )( ( p + ( alignment - 1 ) ) & ~( alignment - 1 ) );
}

GEN_IMPL_INLINE s64 align_forward_i64( s64 value, sw alignment )
{
	return value + ( alignment - value % alignment ) % alignment;
}

GEN_IMPL_INLINE void* pointer_add( void* ptr, sw bytes )
{
	return zpl_cast( void* )( zpl_cast( u8* ) ptr + bytes );
}

GEN_IMPL_INLINE void const* pointer_add_const( void const* ptr, sw bytes )
{
	return zpl_cast( void const* )( zpl_cast( u8 const* ) ptr + bytes );
}

GEN_IMPL_INLINE sw pointer_diff( void const* begin, void const* end )
{
	return zpl_cast( sw )( zpl_cast( u8 const* ) end - zpl_cast( u8 const* ) begin );
}

GEN_IMPL_INLINE void* mem_move( void* dest, void const* source, sw n )
{
	if ( dest == NULL )
	{
		return NULL;
	}

	u8*       d = zpl_cast( u8* ) dest;
	u8 const* s = zpl_cast( u8 const* ) source;

	if ( d == s )
		return d;
	if ( s + n <= d || d + n <= s )    // NOTE: Non-overlapping
		return mem_copy( d, s, n );

	if ( d < s )
	{
		if ( zpl_cast( uptr ) s % size_of( sw ) == zpl_cast( uptr ) d % size_of( sw ) )
		{
			while ( zpl_cast( uptr ) d % size_of( sw ) )
			{
				if ( ! n-- )
					return dest;
				*d++ = *s++;
			}
			while ( n >= size_of( sw ) )
			{
				*zpl_cast( sw* ) d  = *zpl_cast( sw* ) s;
				n                  -= size_of( sw );
				d                  += size_of( sw );
				s                  += size_of( sw );
			}
		}
		for ( ; n; n-- )
			*d++ = *s++;
	}
	else
	{
		if ( ( zpl_cast( uptr ) s % size_of( sw ) ) == ( zpl_cast( uptr ) d % size_of( sw ) ) )
		{
			while ( zpl_cast( uptr )( d + n ) % size_of( sw ) )
			{
				if ( ! n-- )
					return dest;
				d[ n ] = s[ n ];
			}
			while ( n >= size_of( sw ) )
			{
				n                         -= size_of( sw );
				*zpl_cast( sw* )( d + n )  = *zpl_cast( sw* )( s + n );
			}
		}
		while ( n )
			n--, d[ n ] = s[ n ];
	}

	return dest;
}

GEN_IMPL_INLINE void* mem_set( void* dest, u8 c, sw n )
{
	if ( dest == NULL )
	{
		return NULL;
	}

	u8* s = zpl_cast( u8* ) dest;
	sw  k;
	u32 c32 = ( ( u32 )-1 ) / 255 * c;

	if ( n == 0 )
		return dest;
	s[ 0 ] = s[ n - 1 ] = c;
	if ( n < 3 )
		return dest;
	s[ 1 ] = s[ n - 2 ] = c;
	s[ 2 ] = s[ n - 3 ] = c;
	if ( n < 7 )
		return dest;
	s[ 3 ] = s[ n - 4 ] = c;
	if ( n < 9 )
		return dest;

	k  = -zpl_cast( sptr ) s & 3;
	s += k;
	n -= k;
	n &= -4;

	*zpl_cast( u32* )( s + 0 )     = c32;
	*zpl_cast( u32* )( s + n - 4 ) = c32;
	if ( n < 9 )
		return dest;
	*zpl_cast( u32* )( s + 4 )      = c32;
	*zpl_cast( u32* )( s + 8 )      = c32;
	*zpl_cast( u32* )( s + n - 12 ) = c32;
	*zpl_cast( u32* )( s + n - 8 )  = c32;
	if ( n < 25 )
		return dest;
	*zpl_cast( u32* )( s + 12 )     = c32;
	*zpl_cast( u32* )( s + 16 )     = c32;
	*zpl_cast( u32* )( s + 20 )     = c32;
	*zpl_cast( u32* )( s + 24 )     = c32;
	*zpl_cast( u32* )( s + n - 28 ) = c32;
	*zpl_cast( u32* )( s + n - 24 ) = c32;
	*zpl_cast( u32* )( s + n - 20 ) = c32;
	*zpl_cast( u32* )( s + n - 16 ) = c32;

	k  = 24 + ( zpl_cast( uptr ) s & 4 );
	s += k;
	n -= k;

	{
		u64 c64 = ( zpl_cast( u64 ) c32 << 32 ) | c32;
		while ( n > 31 )
		{
			*zpl_cast( u64* )( s + 0 )  = c64;
			*zpl_cast( u64* )( s + 8 )  = c64;
			*zpl_cast( u64* )( s + 16 ) = c64;
			*zpl_cast( u64* )( s + 24 ) = c64;

			n -= 32;
			s += 32;
		}
	}

	return dest;
}

GEN_IMPL_INLINE void* alloc_align( AllocatorInfo a, sw size, sw alignment )
{
	return a.Proc( a.Data, EAllocation_ALLOC, size, alignment, nullptr, 0, GEN_DEFAULT_ALLOCATOR_FLAGS );
}

GEN_IMPL_INLINE void* alloc( AllocatorInfo a, sw size )
{
	return alloc_align( a, size, GEN_DEFAULT_MEMORY_ALIGNMENT );
}

GEN_IMPL_INLINE void free( AllocatorInfo a, void* ptr )
{
	if ( ptr != nullptr )
		a.Proc( a.Data, EAllocation_FREE, 0, 0, ptr, 0, GEN_DEFAULT_ALLOCATOR_FLAGS );
}

GEN_IMPL_INLINE void free_all( AllocatorInfo a )
{
	a.Proc( a.Data, EAllocation_FREE_ALL, 0, 0, nullptr, 0, GEN_DEFAULT_ALLOCATOR_FLAGS );
}

GEN_IMPL_INLINE void* resize( AllocatorInfo a, void* ptr, sw old_size, sw new_size )
{
	return resize_align( a, ptr, old_size, new_size, GEN_DEFAULT_MEMORY_ALIGNMENT );
}

GEN_IMPL_INLINE void* resize_align( AllocatorInfo a, void* ptr, sw old_size, sw new_size, sw alignment )
{
	return a.Proc( a.Data, EAllocation_RESIZE, new_size, alignment, ptr, old_size, GEN_DEFAULT_ALLOCATOR_FLAGS );
}

GEN_IMPL_INLINE void* default_resize_align( AllocatorInfo a, void* old_memory, sw old_size, sw new_size, sw alignment )
{
	if ( ! old_memory )
		return alloc_align( a, new_size, alignment );

	if ( new_size == 0 )
	{
		free( a, old_memory );
		return nullptr;
	}

	if ( new_size < old_size )
		new_size = old_size;

	if ( old_size == new_size )
	{
		return old_memory;
	}
	else
	{
		void* new_memory = alloc_align( a, new_size, alignment );
		if ( ! new_memory )
			return nullptr;
		mem_move( new_memory, old_memory, min( new_size, old_size ) );
		free( a, old_memory );
		return new_memory;
	}
}

GEN_IMPL_INLINE void zero_size( void* ptr, sw size )
{
	mem_set( ptr, 0, size );
}

struct Arena
{
	static
	void* allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags );

	static
	Arena init_from_memory( void* start, sw size )
	{
		return
		{
			{ nullptr, nullptr },
			start,
			size,
			0,
			0
		};
	}

	static
	Arena init_from_allocator( AllocatorInfo backing, sw size )
	{
		Arena result =
		{
			backing,
			alloc( backing, size),
			size,
			0,
			0
		};
		return result;
	}

	static
	Arena init_sub( Arena& parent, sw size )
	{
		return init_from_allocator( parent.Backing, size );
	}

	sw alignment_of( sw alignment )
	{
		sw alignment_offset, result_pointer, mask;
		GEN_ASSERT( is_power_of_two( alignment ) );

		alignment_offset = 0;
		result_pointer   = (sw) PhysicalStart + TotalUsed;
		mask             = alignment - 1;

		if ( result_pointer & mask )
			alignment_offset = alignment - ( result_pointer & mask );

		return alignment_offset;
	}

	void check()
	{
		GEN_ASSERT( TempCount == 0 );
	}

	void free()
	{
		if ( Backing.Proc )
		{
			gen::free( Backing, PhysicalStart );
			PhysicalStart = nullptr;
		}
	}

	sw size_remaining( sw alignment )
	{
		sw result = TotalSize - ( TotalUsed + alignment_of( alignment ) );
		return result;
	}

	AllocatorInfo Backing;
	void*         PhysicalStart;
	sw            TotalSize;
	sw            TotalUsed;
	sw            TempCount;

	operator AllocatorInfo()
	{
		return { allocator_proc, this };
	}
};

struct Pool
{
	static
	void* allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags );

	static
	Pool init( AllocatorInfo backing, sw num_blocks, sw block_size )
	{
		return init_align( backing, num_blocks, block_size, GEN_DEFAULT_MEMORY_ALIGNMENT );
	}

	static
	Pool init_align( AllocatorInfo backing, sw num_blocks, sw block_size, sw block_align );

	void clear();

	void free()
	{
		if ( Backing.Proc )
		{
			gen::free( Backing, PhysicalStart );
		}
	}

	AllocatorInfo Backing;
	void*         PhysicalStart;
	void*         FreeList;
	sw            BlockSize;
	sw            BlockAlign;
	sw            TotalSize;
	sw            NumBlocks;

	operator AllocatorInfo()
	{
		return { allocator_proc, this };
	}
};
#pragma endregion Memory

#pragma region String Ops
GEN_DEF_INLINE const char* char_first_occurence( const char* str, char c );
constexpr auto str_find = &char_first_occurence;

GEN_DEF_INLINE b32   char_is_alpha( char c );
GEN_DEF_INLINE b32   char_is_alphanumeric( char c );
GEN_DEF_INLINE b32   char_is_digit( char c );
GEN_DEF_INLINE b32   char_is_hex_digit( char c );
GEN_DEF_INLINE b32   char_is_space( char c );
GEN_DEF_INLINE char  char_to_lower( char c );
GEN_DEF_INLINE char  char_to_upper( char c );

GEN_DEF_INLINE s32  digit_to_int( char c );
GEN_DEF_INLINE s32  hex_digit_to_int( char c );

GEN_DEF_INLINE s32         str_compare( const char* s1, const char* s2 );
GEN_DEF_INLINE s32         str_compare( const char* s1, const char* s2, sw len );
GEN_DEF_INLINE char*       str_copy( char* dest, const char* source, sw len );
GEN_DEF_INLINE sw          str_copy_nulpad( char* dest, const char* source, sw len );
GEN_DEF_INLINE sw          str_len( const char* str );
GEN_DEF_INLINE sw          str_len( const char* str, sw max_len );
GEN_DEF_INLINE char*       str_reverse( char* str );    // NOTE: ASCII only
GEN_DEF_INLINE char const* str_skip( char const* str, char c );
GEN_DEF_INLINE char const* str_skip_any( char const* str, char const* char_list );
GEN_DEF_INLINE char const* str_trim( char const* str, b32 catch_newline );

// NOTE: ASCII only
GEN_DEF_INLINE void str_to_lower( char* str );
GEN_DEF_INLINE void str_to_upper( char* str );

s64  str_to_i64( const char* str, char** end_ptr, s32 base );
void i64_to_str( s64 value, char* string, s32 base );
void u64_to_str( u64 value, char* string, s32 base );
f64  str_to_f64( const char* str, char** end_ptr );

GEN_IMPL_INLINE const char* char_first_occurence( const char* s, char c )
{
	char ch = c;
	for ( ; *s != ch; s++ )
	{
		if ( *s == '\0' )
			return NULL;
	}
	return s;
}

GEN_IMPL_INLINE b32 char_is_alpha( char c )
{
	if ( ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) )
		return true;
	return false;
}

GEN_IMPL_INLINE b32 char_is_alphanumeric( char c )
{
	return char_is_alpha( c ) || char_is_digit( c );
}

GEN_IMPL_INLINE b32 char_is_digit( char c )
{
	if ( c >= '0' && c <= '9' )
		return true;
	return false;
}

GEN_IMPL_INLINE b32 char_is_hex_digit( char c )
{
	if ( char_is_digit( c ) || ( c >= 'a' && c <= 'f' ) || ( c >= 'A' && c <= 'F' ) )
		return true;
	return false;
}

GEN_IMPL_INLINE b32 char_is_space( char c )
{
	if ( c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v' )
		return true;
	return false;
}

GEN_IMPL_INLINE char char_to_lower( char c )
{
	if ( c >= 'A' && c <= 'Z' )
		return 'a' + ( c - 'A' );
	return c;
}

GEN_IMPL_INLINE char char_to_upper( char c )
{
	if ( c >= 'a' && c <= 'z' )
		return 'A' + ( c - 'a' );
	return c;
}

GEN_IMPL_INLINE s32 digit_to_int( char c )
{
	return char_is_digit( c ) ? c - '0' : c - 'W';
}

GEN_IMPL_INLINE s32 hex_digit_to_int( char c )
{
	if ( char_is_digit( c ) )
		return digit_to_int( c );
	else if ( is_between( c, 'a', 'f' ) )
		return c - 'a' + 10;
	else if ( is_between( c, 'A', 'F' ) )
		return c - 'A' + 10;
	return -1;
}

GEN_IMPL_INLINE s32 str_compare( const char* s1, const char* s2 )
{
	while ( *s1 && ( *s1 == *s2 ) )
	{
		s1++, s2++;
	}
	return *( u8* )s1 - *( u8* )s2;
}

GEN_IMPL_INLINE s32 str_compare( const char* s1, const char* s2, sw len )
{
	for ( ; len > 0; s1++, s2++, len-- )
	{
		if ( *s1 != *s2 )
			return ( ( s1 < s2 ) ? -1 : +1 );
		else if ( *s1 == '\0' )
			return 0;
	}
	return 0;
}

GEN_IMPL_INLINE char* str_copy( char* dest, const char* source, sw len )
{
	GEN_ASSERT_NOT_NULL( dest );
	if ( source )
	{
		char* str = dest;
		while ( len > 0 && *source )
		{
			*str++ = *source++;
			len--;
		}
		while ( len > 0 )
		{
			*str++ = '\0';
			len--;
		}
	}
	return dest;
}

GEN_IMPL_INLINE sw str_copy_nulpad( char* dest, const char* source, sw len )
{
	sw result = 0;
	GEN_ASSERT_NOT_NULL( dest );
	if ( source )
	{
		const char* source_start = source;
		char*       str          = dest;
		while ( len > 0 && *source )
		{
			*str++ = *source++;
			len--;
		}
		while ( len > 0 )
		{
			*str++ = '\0';
			len--;
		}

		result = source - source_start;
	}
	return result;
}

GEN_IMPL_INLINE sw str_len( const char* str )
{
	if ( str == NULL )
	{
		return 0;
	}
	const char* p = str;
	while ( *str )
		str++;
	return str - p;
}

GEN_IMPL_INLINE sw str_len( const char* str, sw max_len )
{
	const char* end = zpl_cast( const char* ) mem_find( str, 0, max_len );
	if ( end )
		return end - str;
	return max_len;
}

GEN_IMPL_INLINE char* str_reverse( char* str )
{
	sw    len  = str_len( str );
	char* a    = str + 0;
	char* b    = str + len - 1;
	len       /= 2;
	while ( len-- )
	{
		swap( *a, *b );
		a++, b--;
	}
	return str;
}

GEN_IMPL_INLINE char const* str_skip( char const* str, char c )
{
	while ( *str && *str != c )
	{
		++str;
	}
	return str;
}

GEN_IMPL_INLINE char const* str_skip_any( char const* str, char const* char_list )
{
	char const* closest_ptr     = zpl_cast( char const* ) pointer_add( ( void* )str, str_len( str ) );
	sw          char_list_count = str_len( char_list );
	for ( sw i = 0; i < char_list_count; i++ )
	{
		char const* p = str_skip( str, char_list[ i ] );
		closest_ptr   = min( closest_ptr, p );
	}
	return closest_ptr;
}

GEN_IMPL_INLINE char const* str_trim( char const* str, b32 catch_newline )
{
	while ( *str && char_is_space( *str ) && ( ! catch_newline || ( catch_newline && *str != '\n' ) ) )
	{
		++str;
	}
	return str;
}

GEN_IMPL_INLINE void str_to_lower( char* str )
{
	if ( ! str )
		return;
	while ( *str )
	{
		*str = char_to_lower( *str );
		str++;
	}
}

GEN_IMPL_INLINE void str_to_upper( char* str )
{
	if ( ! str )
		return;
	while ( *str )
	{
		*str = char_to_upper( *str );
		str++;
	}
}
#pragma endregion String Ops

#pragma region Printing
struct FileInfo;

#ifndef GEN_PRINTF_MAXLEN
#	define GEN_PRINTF_MAXLEN 65536
#endif

// NOTE: A locally persisting buffer is used internally
char* str_fmt_buf       ( char const* fmt, ... );
char* str_fmt_buf_va    ( char const* fmt, va_list va );
sw    str_fmt_va        ( char* str, sw n, char const* fmt, va_list va );
sw    str_fmt_file       ( FileInfo* f, char const* fmt, ... );
sw    str_fmt_file_va    ( FileInfo* f, char const* fmt, va_list va );
sw    str_fmt_out_va    ( char const* fmt, va_list va );
sw    str_fmt_out_err   ( char const* fmt, ... );
sw    str_fmt_out_err_va( char const* fmt, va_list va );

constexpr
char const* Msg_Invalid_Value = "INVALID VALUE PROVIDED";

inline
sw log_fmt(char const* fmt, ...)
{
	sw res;
	va_list va;

	va_start(va, fmt);
	res = str_fmt_out_va(fmt, va);
	va_end(va);

	return res;
}

inline
sw fatal(char const* fmt, ...)
{
	local_persist thread_local
	char buf[GEN_PRINTF_MAXLEN] = { 0 };

	va_list va;

#if Build_Debug
	va_start(va, fmt);
	str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va);
	va_end(va);

	assert_crash(buf);
	return -1;
#else
	va_start(va, fmt);
	str_fmt_out_err_va( fmt, va);
	va_end(va);

	exit(1);
	return -1;
#endif
}
#pragma endregion Printing

#pragma region Containers
template<class Type>
struct Array
{
	struct Header
	{
		AllocatorInfo Allocator;
		uw            Capacity;
		uw            Num;
	};

	static
	Array init( AllocatorInfo allocator )
	{
		return init_reserve( allocator, grow_formula(0) );
	}

	static
	Array init_reserve( AllocatorInfo allocator, sw capacity )
	{
		Header* header = rcast( Header*, alloc( allocator, sizeof(Header) + sizeof(Type) * capacity ));

		if ( header == nullptr )
			return { nullptr };

		header->Allocator = allocator;
		header->Capacity  = capacity;
		header->Num       = 0;

		return { rcast( Type*, header + 1) };
	}

	static
	uw grow_formula( uw value )
	{
		return 2 * value + 8;
	}

	bool append( Type value )
	{
		Header* header = get_header();

		if ( header->Num == header->Capacity )
		{
			if ( ! grow( header->Capacity ))
				return false;

			header = get_header();
		}

		Data[ header->Num ] = value;
		header->Num++;

		return true;
	}

	bool append( Type* items, uw item_num )
	{
		Header* header = get_header();

		if ( header->Num + item_num > header->Capacity )
		{
			if ( ! grow( header->Capacity + item_num ))
				return false;

			header = get_header();
		}

		mem_copy( Data + header->Num, items, item_num * sizeof(Type) );
		header->Num += item_num;

		return true;
	}

	bool append_at( Type item, sw idx )
	{
		Header* header = get_header();

		if ( idx >= header->Num )
			idx = header->Num - 1;

		if ( idx < 0 )
			idx = 0;

		if ( header->Capacity < header->Num + 1 )
		{
			if ( ! grow( header->Capacity + 1 ))
				return false;

			header = get_header();
		}

		Type* target = Data + idx;

		mem_move( target + 1, target, (header->Num - idx) * sizeof(Type) );
		header->Num++;

		return true;
	}

	bool append_at( Type* items, uw item_num, sw idx )
	{
		Header* header = get_header();

		if ( idx >= header->Num )
		{
			return append( items, item_num );
		}

		if ( item_num > header->Capacity )
		{
			if ( ! grow( header->Capacity + item_num ) )
				return false;

			header = get_header();
		}

		Type* target = Data + idx + item_num;
		Type* src    = Data + idx;

		mem_move( target, src, (header->Num - idx) * sizeof(Type) );
		mem_copy( src, items, item_num * sizeof(Type) );
		header->Num += item_num;

		return true;
	}

	Type& back( void )
	{
		Header& header = * get_header();
		return Data[ header.Num - 1 ];
	}

	void clear( void )
	{
		Header& header = * get_header();
		header.Num     = 0;
	}

	bool fill( uw begin, uw end, Type value )
	{
		Header& header = * get_header();

		if ( begin < 0 || end >= header.Num )
			return false;

		for ( sw idx = begin; idx < end; idx++ )
		{
			Data[ idx ] = value;
		}

		return true;
	}

	void free( void )
	{
		Header& header = * get_header();
		gen::free( header.Allocator, &header );
		Data = nullptr;
	}

	Header* get_header( void )
	{
		return rcast( Header*, Data ) - 1 ;
	}

	bool grow( uw min_capacity )
	{
		Header& header       = * get_header();
		uw      new_capacity = grow_formula( header.Capacity );

		if ( new_capacity < min_capacity )
			new_capacity = min_capacity;

		return set_capacity( new_capacity );
	}

	uw num( void )
	{
		return get_header()->Num;
	}

	bool pop( void )
	{
		Header& header = * get_header();

		GEN_ASSERT( header.Num > 0 );
		header.Num--;
	}

	void remove_at( uw idx )
	{
		Header* header = get_header();
		GEN_ASSERT( idx < header->Num );

		mem_move( header + idx, header + idx + 1, sizeof( Type ) * ( header->Num - idx - 1 ) );
		header->Num--;
	}

	bool reserve( uw new_capacity )
	{
		Header& header = * get_header();

		if ( header.Capacity < new_capacity )
			return set_capacity( new_capacity );

		return true;
	}

	bool resize( uw num )
	{
		Header* header = get_header();

		if ( header->Capacity < num )
		{
			if ( ! grow( num ) )
				return false;

			header = get_header();
		}

		header->Num = num;
		return true;
	}

	bool set_capacity( uw new_capacity )
	{
		Header& header = * get_header();

		if ( new_capacity == header.Capacity )
			return true;

		if ( new_capacity < header.Num )
			header.Num = new_capacity;

		sw      size       = sizeof( Header ) + sizeof( Type ) * new_capacity;
		Header* new_header = rcast( Header*, alloc( header.Allocator, size ) );

		if ( new_header == nullptr )
			return false;

		mem_move( new_header, &header, sizeof( Header ) + sizeof( Type ) * header.Num );

		new_header->Capacity = new_capacity;

		gen::free( header.Allocator, &header );

		Data = rcast( Type*, new_header + 1);
		return true;
	}

	Type* Data;

	operator Type*()
	{
		return Data;
	}

	operator Type const*() const
	{
		return Data;
	}

	// For-range based support

	Type* begin()
	{
		return Data;
	}

	Type* end()
	{
		return Data + get_header()->Num;
	}
};

template<typename Type>
struct HashTable
{
	struct FindResult
	{
		sw HashIndex;
		sw PrevIndex;
		sw EntryIndex;
	};

	struct Entry
	{
		u64  Key;
		sw   Next;
		Type Value;
	};

	static
	HashTable init( AllocatorInfo allocator )
	{
		HashTable<Type> result = { { nullptr }, { nullptr } };

		result.Hashes  = Array<sw>::init( allocator );
		result.Entries = Array<Entry>::init( allocator );

		return result;
	}

	static
	HashTable init_reserve( AllocatorInfo allocator, sw num )
	{
		HashTable<Type> result = { { nullptr }, { nullptr } };

		result.Hashes  = Array<sw>::init_reserve( allocator, num );
		result.Hashes.get_header()->Num = num;

		result.Entries = Array<Entry>::init_reserve( allocator, num );

		return result;
	}

	void clear( void )
	{
		for ( sw idx = 0; idx < Hashes.num(); idx++ )
			Hashes[ idx ] = -1;

		Hashes.clear();
		Entries.clear();
	}

	void destroy( void )
	{
		if ( Hashes && Hashes.get_header()->Capacity )
		{
			Hashes.free();
			Entries.free();
		}
	}

	Type* get( u64 key )
	{
		sw idx = find( key ).EntryIndex;
		if ( idx >= 0 )
			return & Entries[ idx ].Value;

		return nullptr;
	}

	using MapProc = void (*)( u64 key, Type  value );

	void map( MapProc map_proc )
	{
		GEN_ASSERT_NOT_NULL( map_proc );

		for ( sw idx = 0; idx < Entries.num(); idx++ )
		{
			map_proc( Entries[ idx ].Key, Entries[ idx ].Value );
		}
	}

	using MapMutProc = void (*)( u64 key, Type* value );

	void map_mut( MapMutProc map_proc )
	{
		GEN_ASSERT_NOT_NULL( map_proc );

		for ( sw idx = 0; idx < Entries.num(); idx++ )
		{
			map_proc( Entries[ idx ].Key, & Entries[ idx ].Value );
		}
	}

	void grow()
	{
		sw new_num = Array<Entry>::grow_formula( Entries.num() );
		rehash( new_num );
	}

	void rehash( sw new_num )
	{
		sw idx;
		sw last_added_index;

		HashTable<Type> new_ht = init_reserve( Hashes.get_header()->Allocator, new_num );

		Array<sw>::Header* hash_header = new_ht.Hashes.get_header();

		for ( idx = 0; idx < new_ht.Hashes.num(); ++idx )
			new_ht.Hashes[ idx ] = -1;

		for ( idx = 0; idx < Entries.num(); ++idx )
		{
			Entry& entry = Entries[ idx ];

			FindResult find_result;

			if ( new_ht.Hashes.num() == 0 )
				new_ht.grow();

			entry            = Entries[ idx ];
			find_result      = new_ht.find( entry.Key );
			last_added_index = new_ht.add_entry( entry.Key );

			if ( find_result.PrevIndex < 0 )
				new_ht.Hashes[ find_result.HashIndex ] = last_added_index;

			else
				new_ht.Entries[ find_result.PrevIndex ].Next = last_added_index;

			new_ht.Entries[ last_added_index ].Next  = find_result.EntryIndex;
			new_ht.Entries[ last_added_index ].Value = entry.Value;
		}

		destroy();
		*this = new_ht;
	}

	void rehash_fast()
	{
		sw idx;

		for ( idx = 0; idx < Entries.num(); idx++ )
			Entries[ idx ].Next = -1;

		for ( idx = 0; idx < Hashes.num(); idx++ )
			Hashes[ idx ] = -1;

		for ( idx = 0; idx < Entries.num(); idx++ )
		{
			Entry* entry;

			FindResult find_result;
		}
	}

	void remove( u64 key )
	{
		FindResult find_result = find( key);

		if ( find_result.EntryIndex >= 0 )
		{
			Entries.remove_at( find_result.EntryIndex );
			rehash_fast();
		}
	}

	void remove_entry( sw idx )
	{
		Entries.remove_at( idx );
	}

	void set( u64 key, Type value )
	{
		sw idx;
		FindResult find_result;

		if ( Hashes.num() == 0 )
			grow();

		find_result = find( key );

		if ( find_result.EntryIndex >= 0 )
		{
			idx = find_result.EntryIndex;
		}
		else
		{
			idx = add_entry( key );

			if ( find_result.PrevIndex >= 0 )
			{
				Entries[ find_result.PrevIndex ].Next = idx;
			}
			else
			{
				Hashes[ find_result.HashIndex ] = idx;
			}
		}

		Entries[ idx ].Value = value;

		if ( full() )
			grow();
	}

	sw slot( u64 key )
	{
		for ( sw idx = 0; idx < Hashes.num(); ++idx )
			if ( Hashes[ idx ] == key )
				return idx;

		return -1;
	}

	Array< sw>    Hashes;
	Array< Entry> Entries;

protected:

	sw add_entry( u64 key )
	{
		sw idx;
		Entry entry = { key, -1 };

		idx = Entries.num();
		Entries.append( entry );
		return idx;
	}

	FindResult find( u64 key )
	{
		FindResult result = { -1, -1, -1 };

		if ( Hashes.num() > 0 )
		{
			result.HashIndex    = key % Hashes.num();
			result.EntryIndex  = Hashes[ result.HashIndex ];

			while ( result.EntryIndex >= 0 )
			{
				if ( Entries[ result.EntryIndex ].Key == key )
					break;

				result.PrevIndex  = result.EntryIndex;
				result.EntryIndex = Entries[ result.EntryIndex ].Next;
			}
		}

		return result;
	}

	b32 full()
	{
		return 0.75f * Hashes.num() < Entries.num();
	}
};
#pragma endregion Containers

#pragma region Hashing

u32 crc32( void const* data, sw len );
u64 crc64( void const* data, sw len );

#pragma endregion Hashing

#pragma region String
// Constant string with length.
struct StrC
{
	sw          Len;
	char const* Ptr;

	operator char const* () const
	{
		return Ptr;
	}
};

#define txt_StrC( text ) \
	StrC { sizeof( text ) - 1, text }

StrC to_StrC( char const* str )
{
	return { str_len( str ), str };
}

sw StrC_len( char const* str )
{
	return (sw) ( str - 1 );
}

// Dynamic String
// This is directly based off the ZPL string api.
// They used a header pattern
// I kept it for simplicty of porting but its not necessary to keep it that way.
struct String
{
	struct Header
	{
		AllocatorInfo Allocator;
		sw            Length;
		sw            Capacity;
	};

	static
	uw grow_formula( uw value )
	{
		// Using a very aggressive growth formula to reduce time mem_copying with recursive calls to append in this library.
		return 4 * value + 8;
	}

	static
	String make( AllocatorInfo allocator, char const* str )
	{
		sw length = str ? str_len( str ) : 0;
		return make_length( allocator, str, length );
	}

	static
	String make( AllocatorInfo allocator, StrC str )
	{
		return make_length( allocator, str.Ptr, str.Len );
	}

	static
	String make_reserve( AllocatorInfo allocator, sw capacity )
	{
		constexpr sw header_size = sizeof( Header );

		s32   alloc_size = header_size + capacity + 1;
		void* allocation = alloc( allocator, alloc_size );

		if ( allocation == nullptr )
			return { nullptr };

		mem_set( allocation, 0, alloc_size );

		Header*
		header            = rcast(Header*, allocation);
		header->Allocator = allocator;
		header->Capacity  = capacity;
		header->Length    = 0;

		String result = { (char*)allocation + header_size };
		return result;
	}

	static
	String make_length( AllocatorInfo allocator, char const* str, sw length )
	{
		constexpr sw header_size = sizeof( Header );

		s32   alloc_size = header_size + length + 1;
		void* allocation = alloc( allocator, alloc_size );

		if ( allocation == nullptr )
			return { nullptr };

		Header&
		header = * rcast(Header*, allocation);
		header = { allocator, length, length };

		String  result = { rcast( char*, allocation) + header_size };

		if ( length && str )
			mem_copy( result, str, length );
		else
			mem_set( result, 0, alloc_size - header_size );

		result[ length ] = '\0';

		return result;
	}

	static
	String fmt( AllocatorInfo allocator, char* buf, sw buf_size, char const* fmt, ... );

	static
	String fmt_buf( AllocatorInfo allocator, char const* fmt, ... );

	static
	String join( AllocatorInfo allocator, char const** parts, sw num_parts, char const* glue )
	{
		String result = make( allocator, "" );

		for ( sw idx = 0; idx < num_parts; ++idx )
		{
			result.append( parts[ idx ] );

			if ( idx < num_parts - 1 )
				result.append( glue );
		}

		return result;
	}

	static
	bool are_equal( String lhs, String rhs )
	{
		if ( lhs.length() != rhs.length() )
			return false;

		for ( sw idx = 0; idx < lhs.length(); ++idx )
			if ( lhs[ idx ] != rhs[ idx ] )
				return false;

		return true;
	}

	bool make_space_for( char const* str, sw add_len )
	{
		sw available = avail_space();

		// NOTE: Return if there is enough space left
		if ( available >= add_len )
		{
			return true;
		}
		else
		{
			sw new_len, old_size, new_size;

			void* ptr;
			void* new_ptr;

			AllocatorInfo allocator = get_header().Allocator;
			Header*       header	= nullptr;

			new_len  = grow_formula( length() + add_len );
			ptr      = & get_header();
			old_size = size_of( Header ) + length() + 1;
			new_size = size_of( Header ) + new_len + 1;

			new_ptr = resize( allocator, ptr, old_size, new_size );

			if ( new_ptr == nullptr )
				return false;

			header            = zpl_cast( Header* ) new_ptr;
			header->Allocator = allocator;
			header->Capacity  = new_len;

			Data = rcast( char*, header + 1 );

			return str;
		}
	}

	bool append( char const* str )
	{
		return append( str, str_len( str ) );
	}

	bool append( char const* str, sw length )
	{
		if ( sptr(str) > 0 )
		{
			sw curr_len = this->length();

			if ( ! make_space_for( str, length ) )
				return false;

			Header& header = get_header();

			mem_copy( Data + curr_len, str, length );

			Data[ curr_len + length ] = '\0';

			header.Length = curr_len + length;
		}
		return str;
	}

	bool append( StrC str)
	{
		return append( str.Ptr, str.Len );
	}

	bool append( const String other )
	{
		return append( other.Data, other.length() );;
	}

	bool append_fmt( char const* fmt, ... );

	sw avail_space() const
	{
		Header const&
		header = * rcast( Header const*, Data - sizeof( Header ));

		return header.Capacity - header.Length;
	}

	sw capacity() const
	{
		Header const&
		header = * rcast( Header const*, Data - sizeof( Header ));

		return header.Capacity;
	}

	void clear()
	{
		get_header().Length = 0;
	}

	String duplicate( AllocatorInfo allocator )
	{
		return make_length( allocator, Data, length() );
	}

	void free()
	{
		if ( ! Data )
			return;

		Header& header = get_header();

		gen::free( header.Allocator, & header );
	}

	Header& get_header()
	{
		return *(Header*)(Data - sizeof(Header));
	}

	sw length() const
	{
		Header const&
		header = * rcast( Header const*, Data - sizeof( Header ));

		return header.Length;
	}

	void trim( char const* cut_set )
	{
		sw len = 0;

		char* start_pos = Data;
		char* end_pos   = Data + length() - 1;

		while ( start_pos <= end_pos && char_first_occurence( cut_set, *start_pos ) )
			start_pos++;

		while ( end_pos > start_pos && char_first_occurence( cut_set, *end_pos ) )
			end_pos--;

		len = scast( sw, ( start_pos > end_pos ) ? 0 : ( ( end_pos - start_pos ) + 1 ) );

		if ( Data != start_pos )
			mem_move( Data, start_pos, len );

		Data[ len ] = '\0';

		get_header().Length = len;
	}

	void trim_space()
	{
		return trim( " \t\r\n\v\f" );
	}

	// For-range support

	char* begin()
	{
		return Data;
	}

	char* end()
	{
		Header const&
		header = * rcast( Header const*, Data - sizeof( Header ));

		return Data + header.Length;
	}

	operator bool()
	{
		return Data;
	}

	operator char* ()
	{
		return Data;
	}

	operator char const* () const
	{
		return Data;
	}

	operator StrC() const
	{
		return
		{
			length(),
			Data
		};
	}

	// Used with cached strings
	// Essentially makes the string a string view.
	String const& operator = ( String const& other ) const
	{
		if ( this == & other )
			return *this;

		String& this_ = ccast( String, *this );

		this_.Data = other.Data;

		return this_;
	}

	char& operator [] ( sw index )
	{
		return Data[ index ];
	}

	char const& operator [] ( sw index ) const
	{
		return Data[ index ];
	}

	char* Data = nullptr;
};

struct String_POD
{
	char* Data;

	operator String()
	{
		return * rcast(String*, this);
	}
};
static_assert( sizeof( String_POD ) == sizeof( String ), "String is not a POD" );
#pragma endregion String

#pragma region File Handling

typedef u32 FileMode;

enum FileModeFlag
{
	EFileMode_READ   = bit( 0 ),
	EFileMode_WRITE  = bit( 1 ),
	EFileMode_APPEND = bit( 2 ),
	EFileMode_RW     = bit( 3 ),
	GEN_FILE_MODES   = EFileMode_READ | EFileMode_WRITE | EFileMode_APPEND | EFileMode_RW,
};

// NOTE: Only used internally and for the file operations
enum SeekWhenceType
{
	ESeekWhence_BEGIN   = 0,
	ESeekWhence_CURRENT = 1,
	ESeekWhence_END     = 2,
};

enum FileError
{
	EFileError_NONE,
	EFileError_INVALID,
	EFileError_INVALID_FILENAME,
	EFileError_EXISTS,
	EFileError_NOT_EXISTS,
	EFileError_PERMISSION,
	EFileError_TRUNCATION_FAILURE,
	EFileError_NOT_EMPTY,
	EFileError_NAME_TOO_LONG,
	EFileError_UNKNOWN,
};

union FileDescriptor
{
	void* p;
	sptr  i;
	uptr  u;
};

typedef struct FileOperations FileOperations;

#define GEN_FILE_OPEN_PROC( name )     FileError name( FileDescriptor* fd, FileOperations* ops, FileMode mode, char const* filename )
#define GEN_FILE_READ_AT_PROC( name )  b32 name( FileDescriptor fd, void* buffer, sw size, s64 offset, sw* bytes_read, b32 stop_at_newline )
#define GEN_FILE_WRITE_AT_PROC( name ) b32 name( FileDescriptor fd, void const* buffer, sw size, s64 offset, sw* bytes_written )
#define GEN_FILE_SEEK_PROC( name )     b32 name( FileDescriptor fd, s64 offset, SeekWhenceType whence, s64* new_offset )
#define GEN_FILE_CLOSE_PROC( name )    void name( FileDescriptor fd )

typedef GEN_FILE_OPEN_PROC( file_open_proc );
typedef GEN_FILE_READ_AT_PROC( FileReadProc );
typedef GEN_FILE_WRITE_AT_PROC( FileWriteProc );
typedef GEN_FILE_SEEK_PROC( FileSeekProc );
typedef GEN_FILE_CLOSE_PROC( FileCloseProc );

struct FileOperations
{
	FileReadProc*  read_at;
	FileWriteProc* write_at;
	FileSeekProc*  seek;
	FileCloseProc* close;
};

extern FileOperations const default_file_operations;

typedef u64 FileTime;

enum DirType
{
	GEN_DIR_TYPE_FILE,
	GEN_DIR_TYPE_FOLDER,
	GEN_DIR_TYPE_UNKNOWN,
};

struct DirInfo;

struct DirEntry
{
	char const*     filename;
	struct DirInfo* dir_info;
	u8              type;
};

struct DirInfo
{
	char const* fullpath;
	DirEntry*   entries;    // zpl_array

	// Internals
	char** filenames;    // zpl_array
	String buf;
};

struct FileInfo
{
	FileOperations ops;
	FileDescriptor fd;
	b32            is_temp;

	char const* filename;
	FileTime    last_write_time;
	DirEntry*   dir;
};

enum FileStandardType
{
	EFileStandard_INPUT,
	EFileStandard_OUTPUT,
	EFileStandard_ERROR,

	EFileStandard_COUNT,
};

/**
 * Get standard file I/O.
 * @param  std Check zpl_file_standard_type
 * @return     File handle to standard I/O
 */
FileInfo* file_get_standard( FileStandardType std );

/**
 * Closes the file
 * @param  file
 */
FileError file_close( FileInfo* file );

/**
 * Returns the currently opened file's name
 * @param  file
 */
inline
char const* file_name( FileInfo* file )
{
	return file->filename ? file->filename : "";
}

/**
 * Opens a file
 * @param  file
 * @param  filename
 */
FileError file_open( FileInfo* file, char const* filename );

/**
 * Opens a file using a specified mode
 * @param  file
 * @param  mode     Access mode to use
 * @param  filename
 */
FileError file_open_mode( FileInfo* file, FileMode mode, char const* filename );

/**
 * Reads from a file
 * @param  file
 * @param  buffer Buffer to read to
 * @param  size   Size to read
 */
GEN_DEF_INLINE b32 file_read( FileInfo* file, void* buffer, sw size );

/**
 * Reads file at a specific offset
 * @param  file
 * @param  buffer     Buffer to read to
 * @param  size       Size to read
 * @param  offset     Offset to read from
 * @param  bytes_read How much data we've actually read
 */
GEN_DEF_INLINE b32 file_read_at( FileInfo* file, void* buffer, sw size, s64 offset );

/**
 * Reads file safely
 * @param  file
 * @param  buffer     Buffer to read to
 * @param  size       Size to read
 * @param  offset     Offset to read from
 * @param  bytes_read How much data we've actually read
 */
GEN_DEF_INLINE b32 file_read_at_check( FileInfo* file, void* buffer, sw size, s64 offset, sw* bytes_read );

struct FileContents
{
	AllocatorInfo allocator;
	void*         data;
	sw            size;
};

constexpr b32 zero_terminate    = true;
constexpr b32 no_zero_terminate = false;

/**
 * Reads the whole file contents
 * @param  a              Allocator to use
 * @param  zero_terminate End the read data with null terminator
 * @param  filepath       Path to the file
 * @return                File contents data
 */
FileContents file_read_contents( AllocatorInfo a, b32 zero_terminate, char const* filepath );

/**
 * Returns a size of the file
 * @param  file
 * @return      File size
 */
s64 file_size( FileInfo* file );

/**
 * Seeks the file cursor from the beginning of file to a specific position
 * @param  file
 * @param  offset Offset to seek to
 */
GEN_DEF_INLINE s64 file_seek( FileInfo* file, s64 offset );

/**
 * Seeks the file cursor to the end of the file
 * @param  file
 */
GEN_DEF_INLINE s64 file_seek_to_end( FileInfo* file );

/**
 * Returns the length from the beginning of the file we've read so far
 * @param  file
 * @return      Our current position in file
 */
GEN_DEF_INLINE s64 file_tell( FileInfo* file );

/**
 * Writes to a file
 * @param  file
 * @param  buffer Buffer to read from
 * @param  size   Size to read
 */
GEN_DEF_INLINE b32 file_write( FileInfo* file, void const* buffer, sw size );

/**
 * Writes to file at a specific offset
 * @param  file
 * @param  buffer        Buffer to read from
 * @param  size          Size to write
 * @param  offset        Offset to write to
 * @param  bytes_written How much data we've actually written
 */
GEN_DEF_INLINE b32 file_write_at( FileInfo* file, void const* buffer, sw size, s64 offset );

/**
 * Writes to file safely
 * @param  file
 * @param  buffer        Buffer to read from
 * @param  size          Size to write
 * @param  offset        Offset to write to
 * @param  bytes_written How much data we've actually written
 */
GEN_DEF_INLINE b32 file_write_at_check( FileInfo* file, void const* buffer, sw size, s64 offset, sw* bytes_written );

GEN_IMPL_INLINE s64 file_seek( FileInfo* f, s64 offset )
{
	s64 new_offset = 0;

	if ( ! f->ops.read_at )
		f->ops = default_file_operations;

	f->ops.seek( f->fd, offset, ESeekWhence_BEGIN, &new_offset );

	return new_offset;
}

GEN_IMPL_INLINE s64 file_seek_to_end( FileInfo* f )
{
	s64 new_offset = 0;

	if ( ! f->ops.read_at )
		f->ops = default_file_operations;

	f->ops.seek( f->fd, 0, ESeekWhence_END, &new_offset );

	return new_offset;
}

GEN_IMPL_INLINE s64 file_tell( FileInfo* f )
{
	s64 new_offset = 0;

	if ( ! f->ops.read_at )
		f->ops = default_file_operations;

	f->ops.seek( f->fd, 0, ESeekWhence_CURRENT, &new_offset );

	return new_offset;
}

GEN_IMPL_INLINE b32 file_read( FileInfo* f, void* buffer, sw size )
{
	s64 cur_offset = file_tell( f );
	b32 result     = file_read_at( f, buffer, size, file_tell( f ) );
	file_seek( f, cur_offset + size );
	return result;
}

GEN_IMPL_INLINE b32 file_read_at( FileInfo* f, void* buffer, sw size, s64 offset )
{
	return file_read_at_check( f, buffer, size, offset, NULL );
}

GEN_IMPL_INLINE b32 file_read_at_check( FileInfo* f, void* buffer, sw size, s64 offset, sw* bytes_read )
{
	if ( ! f->ops.read_at )
		f->ops = default_file_operations;
	return f->ops.read_at( f->fd, buffer, size, offset, bytes_read, false );
}

GEN_IMPL_INLINE b32 file_write( FileInfo* f, void const* buffer, sw size )
{
	s64 cur_offset = file_tell( f );
	b32 result     = file_write_at( f, buffer, size, file_tell( f ) );

	file_seek( f, cur_offset + size );

	return result;
}

GEN_IMPL_INLINE b32 file_write_at( FileInfo* f, void const* buffer, sw size, s64 offset )
{
	return file_write_at_check( f, buffer, size, offset, NULL );
}

GEN_IMPL_INLINE b32 file_write_at_check( FileInfo* f, void const* buffer, sw size, s64 offset, sw* bytes_written )
{
	if ( ! f->ops.read_at )
		f->ops = default_file_operations;

	return f->ops.write_at( f->fd, buffer, size, offset, bytes_written );
}



enum FileStreamFlags : u32
{
	/* Allows us to write to the buffer directly. Beware: you can not append a new data! */
	EFileStream_WRITABLE = bit( 0 ),

	/* Clones the input buffer so you can write (zpl_file_write*) data into it. */
	/* Since we work with a clone, the buffer size can dynamically grow as well. */
	EFileStream_CLONE_WRITABLE = bit( 1 ),
};

/**
 * Opens a new memory stream
 * @param file
 * @param allocator
 */
b8 file_stream_new( FileInfo* file, AllocatorInfo allocator );

/**
 * Opens a memory stream over an existing buffer
 * @param  file
 * @param  allocator
 * @param  buffer   Memory to create stream from
 * @param  size     Buffer's size
 * @param  flags
 */
b8 file_stream_open( FileInfo* file, AllocatorInfo allocator, u8* buffer, sw size, FileStreamFlags flags );

/**
 * Retrieves the stream's underlying buffer and buffer size.
 * @param file memory stream
 * @param size (Optional) buffer size
 */
u8* file_stream_buf( FileInfo* file, sw* size );

extern FileOperations const memory_file_operations;
#pragma endregion File Handling

#pragma region ADT
enum ADT_Type : u32
{
	EADT_TYPE_UNINITIALISED, /* node was not initialised, this is a programming error! */
	EADT_TYPE_ARRAY,
	EADT_TYPE_OBJECT,
	EADT_TYPE_STRING,
	EADT_TYPE_MULTISTRING,
	EADT_TYPE_INTEGER,
	EADT_TYPE_REAL,
};

enum ADT_Props : u32
{
	EADT_PROPS_NONE,
	EADT_PROPS_NAN,
	EADT_PROPS_NAN_NEG,
	EADT_PROPS_INFINITY,
	EADT_PROPS_INFINITY_NEG,
	EADT_PROPS_FALSE,
	EADT_PROPS_TRUE,
	EADT_PROPS_NULL,
	EADT_PROPS_IS_EXP,
	EADT_PROPS_IS_HEX,

	// Used internally so that people can fill in real numbers they plan to write.
	EADT_PROPS_IS_PARSED_REAL,
};

enum ADT_NamingStyle : u32
{
	EADT_NAME_STYLE_DOUBLE_QUOTE,
	EADT_NAME_STYLE_SINGLE_QUOTE,
	EADT_NAME_STYLE_NO_QUOTES,
};

enum ADT_AssignStyle : u32
{
	EADT_ASSIGN_STYLE_COLON,
	EADT_ASSIGN_STYLE_EQUALS,
	EADT_ASSIGN_STYLE_LINE,
};

enum ADT_DelimStyle : u32
{
	EADT_DELIM_STYLE_COMMA,
	EADT_DELIM_STYLE_LINE,
	EADT_DELIM_STYLE_NEWLINE,
};

enum ADT_Error : u32
{
	EADT_ERROR_NONE,
	EADT_ERROR_INTERNAL,
	EADT_ERROR_ALREADY_CONVERTED,
	EADT_ERROR_INVALID_TYPE,
	EADT_ERROR_OUT_OF_MEMORY,
};

struct ADT_Node
{
	char const*      name;
	struct ADT_Node* parent;

	/* properties */
	ADT_Type type  : 4;
	u8 props : 4;
#ifndef GEN_PARSER_DISABLE_ANALYSIS
	u8 cfg_mode          : 1;
	u8 name_style        : 2;
	u8 assign_style      : 2;
	u8 delim_style       : 2;
	u8 delim_line_width  : 4;
	u8 assign_line_width : 4;
#endif

	/* adt data */
	union
	{
		char const*     string;
		Array<ADT_Node> nodes;    ///< zpl_array

		struct
		{
			union
			{
				f64 real;
				s64 integer;
			};

#ifndef GEN_PARSER_DISABLE_ANALYSIS
			/* number analysis */
			s32 base;
			s32 base2;
			u8  base2_offset : 4;
			s8  exp          : 4;
			u8  neg_zero     : 1;
			u8  lead_digit   : 1;
#endif
		};
	};
};

/* ADT NODE LIMITS
* delimiter and assignment segment width is limited to 128 whitespace symbols each.
* real number limits decimal position to 128 places.
* real number exponent is limited to 64 digits.
*/

/**
 * @brief Initialise an ADT object or array
 *
 * @param node
 * @param backing Memory allocator used for descendants
 * @param name Node's name
 * @param is_array
 * @return error code
 */
u8 adt_make_branch( ADT_Node* node, AllocatorInfo backing, char const* name, b32 is_array );

/**
 * @brief Destroy an ADT branch and its descendants
 *
 * @param node
 * @return error code
 */
u8 adt_destroy_branch( ADT_Node* node );

/**
 * @brief Initialise an ADT leaf
 *
 * @param node
 * @param name Node's name
 * @param type Node's type (use zpl_adt_make_branch for container nodes)
 * @return error code
 */
u8 adt_make_leaf( ADT_Node* node, char const* name, ADT_Type type );


/**
 * @brief Fetch a node using provided URI string.
 *
 * This method uses a basic syntax to fetch a node from the ADT. The following features are available
 * to retrieve the data:
 *
 * - "a/b/c" navigates through objects "a" and "b" to get to "c"
 * - "arr/[foo=123]/bar" iterates over "arr" to find any object with param "foo" that matches the value "123", then gets its field called "bar"
 * - "arr/3" retrieves the 4th element in "arr"
 * - "arr/[apple]" retrieves the first element of value "apple" in "arr"
 *
 * @param node ADT node
 * @param uri Locator string as described above
 * @return zpl_adt_node*
 *
 * @see code/apps/examples/json_get.c
 */
ADT_Node* adt_query( ADT_Node* node, char const* uri );

/**
 * @brief Find a field node within an object by the given name.
 *
 * @param node
 * @param name
 * @param deep_search Perform search recursively
 * @return zpl_adt_node * node
 */
	ADT_Node* adt_find( ADT_Node* node, char const* name, b32 deep_search );

/**
 * @brief Allocate an unitialised node within a container at a specified index.
 *
 * @param parent
 * @param index
 * @return zpl_adt_node * node
 */
ADT_Node* adt_alloc_at( ADT_Node* parent, sw index );

/**
 * @brief Allocate an unitialised node within a container.
 *
 * @param parent
 * @return zpl_adt_node * node
 */
ADT_Node* adt_alloc( ADT_Node* parent );

/**
 * @brief Move an existing node to a new container at a specified index.
 *
 * @param node
 * @param new_parent
 * @param index
 * @return zpl_adt_node * node
 */
ADT_Node* adt_move_node_at( ADT_Node* node, ADT_Node* new_parent, sw index );

/**
 * @brief Move an existing node to a new container.
 *
 * @param node
 * @param new_parent
 * @return zpl_adt_node * node
 */
ADT_Node* adt_move_node( ADT_Node* node, ADT_Node* new_parent );

/**
 * @brief Swap two nodes.
 *
 * @param node
 * @param other_node
 * @return
 */
void adt_swap_nodes( ADT_Node* node, ADT_Node* other_node );

/**
 * @brief Remove node from container.
 *
 * @param node
 * @return
 */
void adt_remove_node( ADT_Node* node );

/**
 * @brief Initialise a node as an object
 *
 * @param obj
 * @param name
 * @param backing
 * @return
 */
b8 adt_set_obj( ADT_Node* obj, char const* name, AllocatorInfo backing );

/**
 * @brief Initialise a node as an array
 *
 * @param obj
 * @param name
 * @param backing
 * @return
 */
b8 adt_set_arr( ADT_Node* obj, char const* name, AllocatorInfo backing );

/**
 * @brief Initialise a node as a string
 *
 * @param obj
 * @param name
 * @param value
 * @return
 */
b8 adt_set_str( ADT_Node* obj, char const* name, char const* value );

/**
 * @brief Initialise a node as a float
 *
 * @param obj
 * @param name
 * @param value
 * @return
 */
b8 adt_set_flt( ADT_Node* obj, char const* name, f64 value );

/**
 * @brief Initialise a node as a signed integer
 *
 * @param obj
 * @param name
 * @param value
 * @return
 */
	b8 adt_set_int( ADT_Node* obj, char const* name, s64 value );

/**
 * @brief Append a new node to a container as an object
 *
 * @param parent
 * @param name
 * @return*
 */
ADT_Node* adt_append_obj( ADT_Node* parent, char const* name );

/**
 * @brief Append a new node to a container as an array
 *
 * @param parent
 * @param name
 * @return*
 */
ADT_Node* adt_append_arr( ADT_Node* parent, char const* name );

/**
 * @brief Append a new node to a container as a string
 *
 * @param parent
 * @param name
 * @param value
 * @return*
 */
ADT_Node* adt_append_str( ADT_Node* parent, char const* name, char const* value );

/**
 * @brief Append a new node to a container as a float
 *
 * @param parent
 * @param name
 * @param value
 * @return*
 */
ADT_Node* adt_append_flt( ADT_Node* parent, char const* name, f64 value );

/**
 * @brief Append a new node to a container as a signed integer
 *
 * @param parent
 * @param name
 * @param value
 * @return*
 */
ADT_Node* adt_append_int( ADT_Node* parent, char const* name, s64 value );

/* parser helpers */

/**
 * @brief Parses a text and stores the result into an unitialised node.
 *
 * @param node
 * @param base
 * @return*
 */
char* adt_parse_number( ADT_Node* node, char* base );

/**
 * @brief Parses a text and stores the result into an unitialised node.
 * This function expects the entire input to be a number.
 *
 * @param node
 * @param base
 * @return*
 */
char* adt_parse_number_strict( ADT_Node* node, char* base_str );

/**
 * @brief Parses and converts an existing string node into a number.
 *
 * @param node
 * @return
 */
ADT_Error adt_str_to_number( ADT_Node* node );

/**
 * @brief Parses and converts an existing string node into a number.
 * This function expects the entire input to be a number.
 *
 * @param node
 * @return
 */
ADT_Error adt_str_to_number_strict( ADT_Node* node );

/**
 * @brief Prints a number into a file stream.
 *
 * The provided file handle can also be a memory mapped stream.
 *
 * @see zpl_file_stream_new
 * @param file
 * @param node
 * @return
 */
ADT_Error adt_print_number( FileInfo* file, ADT_Node* node );

/**
 * @brief Prints a string into a file stream.
 *
 * The provided file handle can also be a memory mapped stream.
 *
 * @see zpl_file_stream_new
 * @param file
 * @param node
 * @param escaped_chars
 * @param escape_symbol
 * @return
 */
ADT_Error adt_print_string( FileInfo* file, ADT_Node* node, char const* escaped_chars, char const* escape_symbol );
#pragma endregion ADT

#pragma region CSV
enum CSV_Error : u32
{
	ECSV_Error__NONE,
	ECSV_Error__INTERNAL,
	ECSV_Error__UNEXPECTED_END_OF_INPUT,
	ECSV_Error__MISMATCHED_ROWS,
};

typedef ADT_Node CSV_Object;

GEN_DEF_INLINE u8 csv_parse( CSV_Object* root, char* text, AllocatorInfo allocator, b32 has_header );
u8                csv_parse_delimiter( CSV_Object* root, char* text, AllocatorInfo allocator, b32 has_header, char delim );
void              csv_free( CSV_Object* obj );

GEN_DEF_INLINE void   csv_write( FileInfo* file, CSV_Object* obj );
GEN_DEF_INLINE String csv_write_string( AllocatorInfo a, CSV_Object* obj );
void                  csv_write_delimiter( FileInfo* file, CSV_Object* obj, char delim );
String                csv_write_string_delimiter( AllocatorInfo a, CSV_Object* obj, char delim );

/* inline */

GEN_IMPL_INLINE u8 csv_parse( CSV_Object* root, char* text, AllocatorInfo allocator, b32 has_header )
{
	return csv_parse_delimiter( root, text, allocator, has_header, ',' );
}

GEN_IMPL_INLINE void csv_write( FileInfo* file, CSV_Object* obj )
{
	csv_write_delimiter( file, obj, ',' );
}

GEN_IMPL_INLINE String csv_write_string( AllocatorInfo a, CSV_Object* obj )
{
	return csv_write_string_delimiter( a, obj, ',' );
}
#pragma endregion CSV

#ifdef GEN_BENCHMARK
//! Return CPU timestamp.
u64 read_cpu_time_stamp_counter( void );

//! Return relative time (in seconds) since the application start.
f64 time_rel( void );

//! Return relative time since the application start.
u64 time_rel_ms( void );
#endif

// gen namespace
}

