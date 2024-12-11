#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#   include "src_start.cpp"
#endif

#pragma region Debug

void assert_handler( char const* condition, char const* file, char const* function, s32 line, char const* msg, ... )
{
	_printf_err( "%s - %s:(%d): Assert Failure: ", file, function, line );

	if ( condition )
		_printf_err( "`%s` \n", condition );

	if ( msg )
	{
		va_list va;
		va_start( va, msg );
		_printf_err_va( msg, va );
		va_end( va );
	}

	_printf_err( "%s", "\n" );
}

s32 assert_crash( char const* condition )
{
	GEN_PANIC( condition );
	return 0;
}

#if defined( GEN_SYSTEM_WINDOWS )
	void process_exit( u32 code )
	{
		ExitProcess( code );
	}
#else
#	include <stdlib.h>

	void process_exit( u32 code )
	{
		exit( code );
	}
#endif

#pragma endregion Debug
