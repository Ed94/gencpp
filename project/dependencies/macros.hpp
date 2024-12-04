#ifdef GEN_INTELLISENSE_DIRECTIVES
#   pragma once
#endif

#pragma region Macros

#ifndef global
#define global        static    // Global variables
#endif
#ifndef internal
#define internal      static    // Internal linkage
#endif
#ifndef local_persist
#define local_persist static    // Local Persisting variables
#endif

#ifndef bit
#define bit( Value )                             ( 1 << Value )
#define bitfield_is_equal( Type, Field, Mask ) ( (Type(Mask) & Type(Field)) == Type(Mask) )
#endif

#if GEN_COMPILER_CPP
#	ifndef cast
#	define cast( type, value ) (tmpl_cast<type>( value ))
#	endif
#	ifndef ccast
#	define ccast( type, value ) ( const_cast< type >( (value) ) )
#	endif
#	ifndef pcast
#	define pcast( type, value ) ( * reinterpret_cast< type* >( & ( value ) ) )
#	endif
#	ifndef rcast
#	define rcast( type, value ) reinterpret_cast< type >( value )
#	endif
#	ifndef scast
#	define scast( type, value ) static_cast< type >( value )
#	endif
#else
#	ifndef cast
#	define cast( type, value )  ( (type)(value) )
#	endif
#	ifndef ccast
#	define ccast( type, value ) ( (type)(value) )
#	endif
#	ifndef pcast
#	define pcast( type, value ) ( * (type*)(value) )
#	endif
#	ifndef rcast
#	define rcast( type, value ) ( (type)(value) )
#	endif
#	ifndef scast
#	define scast( type, value ) ( (type)(value) )
#	endif
#endif

#ifndef stringize
#define stringize_va( ... ) #__VA_ARGS__
#define stringize( ... )    stringize_va( __VA_ARGS__ )
#endif

#ifndef do_once
#define do_once( statement ) for ( local_persist b32 once = true; once; once = false, (statement) )

#define do_once_start      \
	do                     \
	{                      \
		local_persist      \
		bool done = false; \
		if ( done )        \
			break;         \
		done = true;

#define do_once_end        \
	}                      \
	while(0);
#endif

#ifndef labeled_scope_start
#define labeled_scope_start if ( false ) {
#define labeled_scope_end   }
#endif

#ifndef         compiler_decorated_func_name
#   ifdef COMPILER_CLANG
#       define  compiler_decorated_func_name __PRETTY_NAME__
#   elif defined(COMPILER_MSVC)
#   	define compiler_decorated_func_name __FUNCDNAME__
#   endif
#endif

#ifndef num_args_impl
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
#endif

#ifndef clamp
#define clamp( x, lower, upper )      min( max( ( x ), ( lower ) ), ( upper ) )
#endif
#ifndef count_of
#define count_of( x )                 ( ( size_of( x ) / size_of( 0 [ x ] ) ) / ( ( ssize )( ! ( size_of( x ) % size_of( 0 [ x ] ) ) ) ) )
#endif
#ifndef is_between
#define is_between( x, lower, upper ) ( ( ( lower ) <= ( x ) ) && ( ( x ) <= ( upper ) ) )
#endif
#ifndef size_of
#define size_of( x )                  ( ssize )( sizeof( x ) )
#endif

#ifndef max
#define max( a, b ) ( (a > b) ? (a) : (b) )
#endif
#ifndef min
#define min( a, b ) ( (a < b) ? (a) : (b) )
#endif

#if GEN_COMPILER_MSVC || GEN_COMPILER_TINYC
#	define offset_of( Type, element ) ( ( GEN_NS( ssize ) ) & ( ( ( Type* )0 )->element ) )
#else
#	define offset_of( Type, element ) __builtin_offsetof( Type, element )
#endif

#ifndef        forceinline
#	if GEN_COMPILER_MSVC
#		define forceinline __forceinline
#		define neverinline __declspec( noinline )
#	elif GEN_COMPILER_GCC
#		define forceinline inline __attribute__((__always_inline__))
#		define neverinline __attribute__( ( __noinline__ ) )
#	elif GEN_COMPILER_CLANG
#	if __has_attribute(__always_inline__)
#		define forceinline inline __attribute__((__always_inline__))
#		define neverinline __attribute__( ( __noinline__ ) )
#	else
#		define forceinline
#		define neverinline
#	endif
#	else
#		define forceinline
#		define neverinline
#	endif
#endif

#ifndef        neverinline
#	if GEN_COMPILER_MSVC
#		define neverinline __declspec( noinline )
#	elif GEN_COMPILER_GCC
#		define neverinline __attribute__( ( __noinline__ ) )
#	elif GEN_COMPILER_CLANG
#	if __has_attribute(__always_inline__)
#		define neverinline __attribute__( ( __noinline__ ) )
#	else
#		define neverinline
#	endif
#	else
#		define neverinline
#	endif
#endif

#if !defined(GEN_SUPPORT_CPP_REFERENCES) && (GEN_COMPILER_C || __STDC_VERSION__ < 202311L)
#	undef    GEN_SUPPORT_CPP_REFERENCES
#	define   GEN_SUPPORT_CPP_REFERENCES 0
#endif

#if !defined(GEN_SUPPORT_CPP_MEMBER_FEATURES) && (GEN_COMPILER_C || __STDC_VERSION__ < 202311L)
#	undef    GEN_SUPPORT_CPP_MEMBER_FEATURES
#	define   GEN_SUPPORT_CPP_MEMBER_FEATURES 0
#endif

#if ! defined(typeof) && (!GEN_COMPILER_C || __STDC_VERSION__ < 202311L)
#	if ! GEN_COMPILER_C
#		define typeof decltype
#	elif defined(_MSC_VER)
#		define typeof(x) __typeof(x)
#	elif defined(__GNUC__) || defined(__clang__)
#		define typeof(x) __typeof__(x)
#	else
#		error "Compiler not supported"
#	endif
#endif

#ifndef GEN_API_C_BEGIN
#	if GEN_COMPILER_C || (GEN_COMPILER_CPP && GEN_SUPPORT_CPP_REFERENCES)
#		define GEN_API_C_BEGIN
#		define GEN_API_C_END
#	else
#		define GEN_API_C_BEGIN extern "C" {
#		define GEN_API_C_END }
#	endif
#endif

#if GEN_COMPILER_C
#	if __STDC_VERSION__ >= 202311L
#		define enum_underlying(type) : type
#	else
#		define enum_underlying(type)
#   endif
#else
#	define enum_underlying(type) : type
#endif

#if GEN_COMPILER_C
#	ifndef nullptr
#		define nullptr NULL
#	endif
#endif

#if ! defined(GEN_PARAM_DEFAULT) && GEN_COMPILER_CPP
#	define GEN_PARAM_DEFAULT = {}
#else
#	define GEN_PARAM_DEFAULT
#endif

#pragma endregion Macros
