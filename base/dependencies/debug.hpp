#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dependencies/platform.hpp"
#	include "dependencies/macros.hpp"
#	include "basic_types.hpp"
#	include "macros.hpp"
#endif

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

#if Build_Debug
	#define GEN_FATAL( ... )                               \
	do                                                     \
	{                                                      \
		local_persist thread_local                         \
		char buf[GEN_PRINTF_MAXLEN] = { 0 };               \
		                                                   \
		str_fmt(buf, GEN_PRINTF_MAXLEN, __VA_ARGS__);      \
		GEN_PANIC(buf);                                    \
	}                                                      \
	while (0)
#else

#	define GEN_FATAL( ... )                  \
	do                                       \
	{                                        \
		str_fmt_out_err( __VA_ARGS__ );      \
		process_exit(1);                     \
	}                                        \
	while (0)
#endif

void assert_handler( char const* condition, char const* file, char const* function, s32 line, char const* msg, ... );
s32  assert_crash( char const* condition );
void process_exit( u32 code );

#pragma endregion Debug
