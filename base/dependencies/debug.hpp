#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dependencies/platform.hpp"
#	include "dependencies/macros.hpp"
#	include "basic_types.hpp"
#	include "macros.hpp"
#endif

#pragma region Debug

#if GEN_BUILD_DEBUG
#	if defined( GEN_COMPILER_MSVC )
#		if _MSC_VER < 1300
// #pragma message("GEN_BUILD_DEBUG: __asm int 3")
#			define GEN_DEBUG_TRAP() __asm int 3 /* Trap to debugger! */
#		else
// #pragma message("GEN_BUILD_DEBUG: __debugbreak()")
#			define GEN_DEBUG_TRAP() __debugbreak()
#		endif
#	elif defined( GEN_COMPILER_TINYC )
#		define GEN_DEBUG_TRAP() process_exit( 1 )
#	else
#		define GEN_DEBUG_TRAP() __builtin_trap()
#	endif
#else
// #pragma message("GEN_BUILD_DEBUG: omitted")
#	define GEN_DEBUG_TRAP()
#endif

#define GEN_ASSERT( cond ) GEN_ASSERT_MSG( cond, NULL )

#define GEN_ASSERT_MSG( cond, msg, ... )                                                              \
	do                                                                                                \
	{                                                                                                 \
		if ( ! ( cond ) )                                                                             \
		{                                                                                             \
			assert_handler( #cond, __FILE__, __func__, scast( s64, __LINE__ ), msg, ##__VA_ARGS__ );  \
			GEN_DEBUG_TRAP();                                                                         \
		}                                                                                             \
	} while ( 0 )

#define GEN_ASSERT_NOT_NULL( ptr ) GEN_ASSERT_MSG( ( ptr ) != NULL, #ptr " must not be NULL" )

// NOTE: Things that shouldn't happen with a message!
#define GEN_PANIC( msg, ... ) GEN_ASSERT_MSG( 0, msg, ##__VA_ARGS__ )

#if GEN_BUILD_DEBUG
	#define GEN_FATAL( ... )                               \
	do                                                     \
	{                                                      \
		local_persist thread_local                         \
		char buf[GEN_PRINTF_MAXLEN] = { 0 };               \
		                                                   \
		c_str_fmt(buf, GEN_PRINTF_MAXLEN, __VA_ARGS__);    \
		GEN_PANIC(buf);                                    \
	}                                                      \
	while (0)
#else

#	define GEN_FATAL( ... )                  \
	do                                       \
	{                                        \
		c_str_fmt_out_err( __VA_ARGS__ );    \
		GEN_DEBUG_TRAP();                    \
		process_exit(1);                     \
	}                                        \
	while (0)
#endif

GEN_API void assert_handler( char const* condition, char const* file, char const* function, s32 line, char const* msg, ... );
GEN_API s32  assert_crash( char const* condition );
GEN_API void process_exit( u32 code );

#pragma endregion Debug
