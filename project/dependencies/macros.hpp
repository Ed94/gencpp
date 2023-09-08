#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "header_start.hpp"
#endif

#pragma region Macros

#define zpl_cast( Type ) ( Type )

// Keywords

#define global        static    // Global variables
#define internal      static    // Internal linkage
#define local_persist static    // Local Persisting variables

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

// Bits

#define bit( Value )                             ( 1 << Value )
#define bitfield_is_equal( Type, Field, Mask ) ( (Type(Mask) & Type(Field)) == Type(Mask) )

// Casting

#define ccast( Type, Value ) ( * const_cast< Type* >( & (Value) ) )
#define pcast( Type, Value ) ( * reinterpret_cast< Type* >( & ( Value ) ) )
#define rcast( Type, Value ) reinterpret_cast< Type >( Value )
#define scast( Type, Value ) static_cast< Type >( Value )

// Num Arguments (Varadics)
// #if defined(__GNUC__) || defined(__clang__)
// Supports 0-50 arguments
#define num_args_impl( _0,                                 \
		_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,   \
		_11, _12, _13, _14, _15, _16, _17, _18, _19, _20,  \
		_21, _22, _23, _24, _25, _26, _27, _28, _29, _30,  \
		_31, _32, _33, _34, _35, _36, _37, _38, _39, _40,  \
		_41, _42, _43, _44, _45, _46, _47, _48, _49, _50,  \
		_51, _52, _53, _54, _55, _56, _57, _58, _59, _60,  \
		_61, _62, _63, _64, _65, _66, _67, _68, _69, _70,  \
		_71, _72, _73, _74, _75, _76, _77, _78, _79, _80,  \
		_81, _82, _83, _84, _85, _86, _87, _88, _89, _90,  \
		_91, _92, _93, _94, _95, _96, _97, _98, _99, _100, \
		N, ...                                             \
	) N

	// ## deletes preceding comma if _VA_ARGS__ is empty (GCC, Clang)
#define num_args(...)                            \
	num_args_impl(_, ## __VA_ARGS__,             \
		100, 99, 98, 97, 96, 95, 94, 93, 92, 91, \
		 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, \
		 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, \
		 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, \
		 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, \
		 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, \
		 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, \
		 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, \
		 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
		 10,  9,  8,  7,  6,  5,  4,  3,  2,  1, \
		 0                                       \
	)

// #else
// This doesn't work on latest msvc so I had to use /Zc:preprocessor flag.

// Supports 1-50 arguments
// #define num_args_impl(                                  \
// 		_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,   \
// 		_11, _12, _13, _14, _15, _16, _17, _18, _19, _20,  \
// 		_21, _22, _23, _24, _25, _26, _27, _28, _29, _30,  \
// 		_31, _32, _33, _34, _35, _36, _37, _38, _39, _40,  \
// 		_41, _42, _43, _44, _45, _46, _47, _48, _49, _50,  \
// 		_51, _52, _53, _54, _55, _56, _57, _58, _59, _60,  \
// 		_61, _62, _63, _64, _65, _66, _67, _68, _69, _70,  \
// 		_71, _72, _73, _74, _75, _76, _77, _78, _79, _80,  \
// 		_81, _82, _83, _84, _85, _86, _87, _88, _89, _90,  \
// 		_91, _92, _93, _94, _95, _96, _97, _98, _99, _100, \
// 		N, ...                                             \
// 	) N

// #define num_args(...)                         \
// 	num_args_impl( __VA_ARGS__,                  \
// 		100, 99, 98, 97, 96, 95, 94, 93, 92, 91, \
// 		 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, \
// 		 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, \
// 		 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, \
// 		 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, \
// 		 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, \
// 		 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, \
// 		 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, \
// 		 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, \
// 		 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
// 		 10,  9,  8,  7,  6,  5,  4,  3,  2,  1  \
// 	)
// #endif

// Stringizing
#define stringize_va( ... ) #__VA_ARGS__
#define stringize( ... )    stringize_va( __VA_ARGS__ )

// Function do once

#define do_once()          \
	do                     \
	{                      \
		static             \
		bool Done = false; \
		if ( Done )        \
			return;        \
		Done = true;       \
	}                      \
	while(0)

#define do_once_start      \
	do                     \
	{                      \
		static             \
		bool Done = false; \
		if ( Done )        \
			break;         \
		Done = true;

#define do_once_end        \
	}                      \
	while(0);

#define labeled_scope_start if ( false ) {
#define labeled_scope_end   }

#define clamp( x, lower, upper )      min( max( ( x ), ( lower ) ), ( upper ) )
#define count_of( x )                 ( ( size_of( x ) / size_of( 0 [ x ] ) ) / ( ( sw )( ! ( size_of( x ) % size_of( 0 [ x ] ) ) ) ) )
#define is_between( x, lower, upper ) ( ( ( lower ) <= ( x ) ) && ( ( x ) <= ( upper ) ) )
#define max( a, b )                   ( ( a ) > ( b ) ? ( a ) : ( b ) )
#define min( a, b )                   ( ( a ) < ( b ) ? ( a ) : ( b ) )
#define size_of( x )                  ( sw )( sizeof( x ) )

#if defined( _MSC_VER ) || defined( GEN_COMPILER_TINYC )
#	define offset_of( Type, element ) ( ( GEN_NS( gen_sw ) ) & ( ( ( Type* )0 )->element ) )
#else
#	define offset_of( Type, element ) __builtin_offsetof( Type, element )
#endif

template< class Type >
void swap( Type& a, Type& b )
{
	Type tmp = a;
	a = b;
	b = tmp;
}

#pragma endregion Macros
