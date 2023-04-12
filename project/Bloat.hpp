/*
	BLOAT.

	This contians all definitions not directly related to the project.
*/
#pragma once

#ifdef BLOAT_IMPL
#	define ZPL_IMPLEMENTATION
#endif

// TODO: This will be removed when making the library have zero dependencies.
#pragma region 									ZPL INCLUDE
#if __clang__
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wmissing-braces"
#	pragma clang diagnostic ignored "-Wbraced-scalar-init"
#endif

// #   define ZPL_HEAP_ANALYSIS
#	define ZPL_WRAP_IN_NAMESPACE
#	define ZPL_NO_MATH_H
#	define ZPL_CUSTOM_MODULES
#		define ZPL_MODULE_ESSENTIALS
#		define ZPL_MODULE_CORE
#		define ZPL_MODULE_TIMER
#		define ZPL_MODULE_HASHING
// #		define ZPL_MODULE_REGEX
// #		define ZPL_MODULE_EVENT
// #		define ZPL_MODULE_DLL
// #		define ZPL_MODULE_OPTS
// #		define ZPL_MODULE_PROCESS
// #		define ZPL_MODULE_MAT
// #		define ZPL_MODULE_THREADING
// #		define ZPL_MODULE_JOBS
// #		define ZPL_MODULE_PARSER
#include "zpl.h"

using zpl::s8;
using zpl::s16;
using zpl::s32;
using zpl::s64;
using zpl::u8;
using zpl::u32;
using zpl::u64;
using zpl::uw;
using zpl::sw;

using zpl::Arena;
using zpl::AllocatorInfo;
using zpl::ArrayHeader;
using zpl::FileInfo;
using zpl::FileError;
using zpl::Pool;
using zpl::String;

using zpl::EFileMode_WRITE;
using zpl::EFileError_NONE;

using zpl::alloc;
using zpl::arena_allocator;
using zpl::arena_init_from_memory;
using zpl::arena_free;
using zpl::bprintf;
using zpl::char_is_alpha;
using zpl::char_is_alphanumeric;
using zpl::char_is_space;
using zpl::crc32;
using zpl::free_all;
using zpl::memcopy;
using zpl::memset;
using zpl::pool_allocator;
using zpl::pool_init;
using zpl::pool_free;
using zpl::printf_va;
using zpl::printf_err_va;
using zpl::str_compare;
using zpl::snprintf_va;
using zpl::string_appendc;
using zpl::string_append_fmt;
using zpl::string_append_length;
using zpl::string_length;
using zpl::string_make;
using zpl::strnlen;
using zpl::exit;


#if __clang__
#	pragma clang diagnostic pop
#endif
#pragma endregion 								ZPL INCLUDE


#if __clang__
#	pragma clang diagnostic ignored "-Wunused-const-variable"
#	pragma clang diagnostic ignored "-Wswitch"
#	pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wunknown-pragmas"
#	pragma clang diagnostic ignored "-Wvarargs"
#endif


#if defined(__GNUC__) || defined(__clang__) || true
	// Supports 0-10 arguments
	#define macro_num_args_impl( _0,                      \
		_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,  \
		_11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
		N, ...                                            \
	) N
	// _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
	// _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
	// _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,

	// ## deletes preceding comma if _VA_ARGS__ is empty (GCC, Clang)
	#define macro_num_args(...)                 \
 	macro_num_args_impl(_, ## __VA_ARGS__,      \
		20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
		10,  9,  8,  7,  6,  5,  4,  3,  2,  1, \
		0                                       \
	)
  	// 50, 49, 48, 47, 46, 45, 44, 43, 42, 41,              \
  	// 40, 39, 38, 37, 36, 35, 34, 33, 32, 31,              \
  	// 30, 29, 28, 27, 26, 25, 24, 23, 22, 21,
#else
	// Supports 1-10 arguments
  	#define macro_num_args_impl(                          \
  		 _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
		_11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
 		 N, ...                                           \
	) N

	#define macro_num_args(...)                 \
	macro_num_args_impl( __VA_ARGS__,           \
		20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
		10,  9,  8,  7,  6,  5,  4,  3,  2,  1  \
	)
#endif

#define macro_expand( Expanded_ ) Expanded_

#define bit( Value_ )                      ( 1 << Value_ )
#define bitfield_is_equal( Field_, Mask_ ) ( ( (Mask_) & (Field_) ) == (Mask_) )
#define ct                                 constexpr
#define forceinline                        ZPL_ALWAYS_INLINE
#define print_nl( _)                       zpl_printf("\n")
#define ccast( Type_, Value_ )             * const_cast< Type_* >( & (Value_) )
#define scast( Type_, Value_ )			   static_cast< Type_ >( Value_ )
#define rcast( Type_, Value_ )			   reinterpret_cast< Type_ >( Value_ )
#define pcast( Type_, Value_ )             ( * (Type_*)( & (Value_) ) )
#define txt_impl( Value_ )                 #Value_
#define txt( Value_ )                      txt_impl( Value_ )
#define txt_n_len( Value_ )		           sizeof( txt_impl( Value_ ) ), txt_impl( Value_ )
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

ct char const* Msg_Invalid_Value = "INVALID VALUE PROVIDED";

namespace Global
{
	extern bool ShouldShowDebug;
}

namespace Memory
{
	ct uw Initial_Reserve = megabytes(10);

	extern Arena Global_Arena;
	// #define g_allocator arena_allocator( & Memory::Global_Arena)

	// Heap allocator is being used for now to isolate errors from being memory related (tech debt till ready to address)
	#define g_allocator heap()

	void setup();
	void resize( uw new_size );
	void cleanup();
}

sw token_fmt_va( char* buf, uw buf_size, char const* fmt, s32 num_tokens, va_list va );

inline
char const* token_fmt( char const* fmt, sw num_tokens, ... )
{
	local_persist thread_local
	char buf[ZPL_PRINTF_MAXLEN] = { 0 };

	va_list va;
	va_start(va, fmt);
	token_fmt_va(buf, ZPL_PRINTF_MAXLEN, fmt, num_tokens, va);
	va_end(va);

	return buf;
}

inline
sw log_fmt(char const *fmt, ...)
{
	if ( Global::ShouldShowDebug == false )
		return 0;

	sw res;
	va_list va;

	va_start(va, fmt);
	res = printf_va(fmt, va);
	va_end(va);

	return res;
}

inline
sw fatal(char const *fmt, ...)
{
	local_persist thread_local
	char buf[ZPL_PRINTF_MAXLEN] = { 0 };

	va_list va;

#if Build_Debug
	va_start(va, fmt);
	zpl::snprintf_va(buf, ZPL_PRINTF_MAXLEN, fmt, va);
	va_end(va);

	assert_crash(buf);
	return -1;
#else
	va_start(va, fmt);
	printf_err_va( fmt, va);
	va_end(va);

	exit(1);
	return -1;
#endif
}

