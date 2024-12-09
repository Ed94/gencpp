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

// Mainly intended for forcing the base library to utilize only C-valid constructs or type coercion
#ifndef GEN_C_LIKE_CPP
#define GEN_C_LIKE_CPP 0
#endif

#if GEN_COMPILER_CPP
#	ifndef cast
#	define cast( type, value ) (tmpl_cast<type>( value ))
#	endif
#else
#	ifndef cast
#	define cast( type, value )  ( (type)(value) )
#	endif
#endif

#if GEN_COMPILER_CPP
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

// This is essentially an arg couneter version of GEN_SELECT_ARG macros
// See section : _Generic function overloading for that usage (explains this heavier case)

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

#if GEN_COMPILER_C
#ifndef static_assert
#undef  static_assert
    #if GEN_COMPILER_C && __STDC_VERSION__ >= 201112L
        #define static_assert(condition, message) _Static_assert(condition, message)
    #else
        #define static_assert(condition, message) typedef char static_assertion_##__LINE__[(condition)?1:-1]
	#endif
#endif
#endif

#if GEN_COMPILER_CPP
// Already Defined
#elif GEN_COMPILER_C && __STDC_VERSION__ >= 201112L
#	define thread_local _Thread_local
#elif GEN_COMPILER_MSVC
#	define thread_local __declspec(thread)
#elif GEN_COMPILER_CLANG
#	define thread_local __thread
#else
#	error "No thread local support"
#endif

#if ! defined(typeof) && (!GEN_COMPILER_C || __STDC_VERSION__ < 202311L)
#	if ! GEN_COMPILER_C
#		define typeof decltype
#	elif defined(_MSC_VER)
#		define typeof(x) __typeof__(x)
#	elif defined(__GNUC__) || defined(__clang__)
#		define typeof(x) __typeof__(x)
#	else
#		error "Compiler not supported"
#	endif
#endif

#ifndef GEN_API_C_BEGIN
#	if GEN_COMPILER_C
#		define GEN_API_C_BEGIN
#		define GEN_API_C_END
#	else
#		define GEN_API_C_BEGIN extern "C" {
#		define GEN_API_C_END   }
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

#	ifndef GEN_REMOVE_PTR
#		define GEN_REMOVE_PTR(type) typeof(* ( (type) NULL) )
#	endif
#endif

#if ! defined(GEN_PARAM_DEFAULT) && GEN_COMPILER_CPP
#	define GEN_PARAM_DEFAULT = {}
#else
#	define GEN_PARAM_DEFAULT
#endif

#if GEN_COMPILER_CPP
    #define struct_init(type, value) {value}
#else
    #define struct_init(type, value) {value}
#endif

#if 0
#ifndef GEN_OPTIMIZE_MAPPINGS_BEGIN
#	define GEN_OPTIMIZE_MAPPINGS_BEGIN _pragma(optimize("gt", on))
#	define GEN_OPITMIZE_MAPPINGS_END   _pragma(optimize("", on))
#endif
#else
#	define GEN_OPTIMIZE_MAPPINGS_BEGIN
#	define GEN_OPITMIZE_MAPPINGS_END
#endif

#if GEN_COMPILER_C
//   ____                       _        ______                _   _                ____                  _                 __ _
//  / ___}                     (_)      |  ____}              | | (_)              / __ \                | |               | |(_)
// | | ___  ___ _ __   ___ _ __ _  ___  | |__ _   _ _ __   ___| |_ _  ___  _ __   | |  | |_   _____ _ __ | | ___   __ _  __| | _ _ __   __ _
// | |{__ |/ _ \ '_ \ / _ \ '__} |/ __| |  __} | | | '_ \ / __} __} |/ _ \| '_ \  | |  | \ \ / / _ \ '_ \| |/ _ \ / _` |/ _` || | '_ \ / _` |
// | |__j |  __/ | | |  __/ |  | | (__  | |  | |_| | | | | (__| l_| | (_) | | | | | l__| |\ V /  __/ | | | | (_) | (_| | (_| || | | | | (_| |
//  \____/ \___}_l l_l\___}_l  l_l\___| l_l   \__,_l_l l_l\___}\__}_l\___/l_l l_l  \____/  \_/ \___}_l l_l_l\___/ \__,_l\__,_l|_|_| |_|\__, |
// This implemnents macros for utilizing "The Naive Extendible _Generic Macro" explained in:                                            __| |
// https://github.com/JacksonAllan/CC/blob/main/articles/Better_C_Generics_Part_1_The_Extendible_Generic.md                            {___/
// Since gencpp is used to generate the c-library, it was choosen over the more novel implementations to keep the macros as easy to understand and unobfuscated as possible.

#define GEN_COMMA_OPERATOR , // The comma operator is used by preprocessor macros to delimit arguments, so we have to represent it via a macro to prevent parsing incorrectly.

// Helper macros for argument selection
#define GEN_SELECT_ARG_1( _1, ... ) _1 // <-- Of all th args passed pick _1.
#define GEN_SELECT_ARG_2( _1, _2, ... ) _2 // <-- Of all the args passed pick _2.
#define GEN_SELECT_ARG_3( _1, _2, _3, ... ) _3 // etc..

#define GEN_GENERIC_SEL_ENTRY_TYPE             GEN_SELECT_ARG_1 // Use the arg expansion macro to select arg 1 which should have the type.
#define GEN_GENERIC_SEL_ENTRY_FUNCTION         GEN_SELECT_ARG_2 // Use the arg expansion macro to select arg 2 which should have the function.
#define GEN_GENERIC_SEL_ENTRY_COMMA_DELIMITER  GEN_SELECT_ARG_3 // Use the arg expansion macro to select arg 3 which should have the comma delimiter ','.

#define GEN_RESOLVED_FUNCTION_CALL // Just used to indicate where the call "occurs"

// ----------------------------------------------------------------------------------------------------------------------------------
// GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( macro ) includes a _Generic slot only if the specified macro is defined (as type, function_name).
// It takes advantage of the fact that if the macro is defined, then the expanded text will contain a comma.
// Expands to ',' if it can find (type): (function) <comma_operator: ',' >
// Where GEN_GENERIC_SEL_ENTRY_COMMA_DELIMITER is specifically looking for that <comma> ,
#define GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( slot_exp ) GEN_GENERIC_SEL_ENTRY_COMMA_DELIMITER( slot_exp, GEN_GENERIC_SEL_ENTRY_TYPE( slot_exp, ): GEN_GENERIC_SEL_ENTRY_FUNCTION( slot_exp, ) GEN_COMMA_OPERATOR, , )
//                                                          ^ Selects the comma                              ^ is the type                             ^ is the function                             ^ Insert a comma
// The slot won't exist if that comma is not found.                                                                                                                                                  |

// For the occastion where an expression didn't resolve to a selection option the "default: <value>" wilbe set to:
typedef struct GENCPP_NO_RESOLVED_GENERIC_SELECTION GENCPP_NO_RESOLVED_GENERIC_SELECTION;
struct GENCPP_NO_RESOLVED_GENERIC_SELECTION {
	void* _THE_VOID_SLOT_;
};
GENCPP_NO_RESOLVED_GENERIC_SELECTION const gen_generic_selection_fail = {0};
// Which will provide the message:  error: called object type 'struct NO_RESOLVED_GENERIC_SELECTION' is not a function or function pointer
// ----------------------------------------------------------------------------------------------------------------------------------

// Below are generated on demand for an overlaod depdendent on a type:
// -----------------------------------------------------------------------------------------------------#define GEN_FUNCTION_GENERIC_EXAMPLE( selector_arg ) _Generic(       k
#define GEN_FUNCTION_GENERIC_EXAMPLE( selector_arg ) _Generic(       \
(selector_arg), /* Select Via Expression*/                           \
  /* Extendibility slots: */                                         \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( FunctionID__ARGS_SIG_1 ) \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( FunctionID__ARGS_SIG_1 ) \
	default: gen_generic_selection_fail                              \
) GEN_RESOLVED_FUNCTION_CALL( selector_arg )
// ----------------------------------------------------------------------------------------------------------------------------------

// Then each definiton of a function has an associated define:
// #define <function_id_macro> GEN_GENERIC_FUNCTION_ARG_SIGNATURE( <function_id>, <arguments> )
#define GEN_GENERIC_FUNCTION_ARG_SIGNATURE( name_of_function, type_delimiter ) type_delimiter name_of_function

// Then somehwere later on
// <etc> <return_type> <function_id> ( <arguments> ) { <implementation> }

// Concrete example:

// To add support for long:
#define GEN_EXAMPLE_HASH__ARGS_SIG_1 GEN_GENERIC_FUNCTION_ARG_SIGNATURE( hash__P_long, long long )
size_t gen_example_hash__P_long( long val ) { return val * 2654435761ull; }

// To add support for long long:
#define GEN_EXAMPLE_HASH__ARGS_SIG_2 GEN_GENERIC_FUNCTION_ARG_SIGNATURE( hash__P_long_long, long long )
size_t gen_example_hash__P_long_long( long long val ) { return val * 2654435761ull; }

// If using an Editor with support for syntax hightlighting macros: HASH__ARGS_SIG_1 and HASH_ARGS_SIG_2 should show color highlighting indicating the slot is enabled,
// or, "defined" for usage during the compilation pass that handles the _Generic instrinsic.
#define hash( function_arguments ) _Generic(                   \
(function_arguments), /* Select Via Expression*/               \
  /* Extendibility slots: */                                   \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( HASH__ARGS_SIG_1 ) \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( HASH__ARGS_SIG_2 ) \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( HASH__ARGS_SIG_3 ) \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( HASH__ARGS_SIG_4 ) \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( HASH__ARGS_SIG_5 ) \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( HASH__ARGS_SIG_6 ) \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( HASH__ARGS_SIG_7 ) \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( HASH__ARGS_SIG_8 ) \
	default: gen_generic_selection_fail                        \
) GEN_RESOLVED_FUNCTION_CALL( function_arguments )

// Additional Variations:

// If the function takes more than one argument the following is used:
#define GEN_FUNCTION_GENERIC_EXAMPLE_VARADIC( selector_arg, ... ) _Generic( \
(selector_arg),                                                             \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( FunctionID__ARGS_SIG_1 )        \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( FunctionID__ARGS_SIG_2 )        \
	...                                                                     \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT(FunctionID__ARGS_SIG_N )         \
	default: gen_generic_selection_fail                                     \
) GEN_RESOLVED_FUNCTION_CALL( selector_arg, __VA_ARG__ )

// If the function does not take the arugment as a parameter:
#define GEN_FUNCTION_GENERIC_EXAMPLE_DIRECT_TYPE( selector_arg ) _Generic( \
( GEN_TYPE_TO_EXP(selector_arg) ),                                         \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( FunctionID__ARGS_SIG_1 )       \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( FunctionID__ARGS_SIG_2 )       \
	/* ... */                                                              \
	GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT(FunctionID__ARGS_SIG_N )        \
	default: gen_generic_selection_fail                                    \
) GEN_RESOLVED_FUNCTION_CALL()

// Used to keep the _Generic keyword happy as bare types are not considered "expressions"
#define GEN_TYPE_TO_EXP(type) (* (type*)NULL)

// typedef void* GEN_GenericExampleType;
// GEN_FUNCTION_GENERIC_EXAMPLE_DIRECT_TYPE( GEN_GenericExampleType );

// END OF ------------------------ _Generic function overloading ----------------------------------------- END OF
#endif

#pragma endregion Macros
