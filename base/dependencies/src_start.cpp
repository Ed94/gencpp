#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "header_start.hpp"
#endif

#pragma region Macros and Includes

#	include <stdio.h>
// NOTE: Ensure we use standard methods for these calls if we use GEN_PICO
#	if ! defined( GEN_PICO_CUSTOM_ROUTINES )
#		if ! defined( GEN_MODULE_CORE )
#			define _strlen                   strlen
#			define _printf_err( fmt, ... )   fprintf( stderr, fmt, __VA_ARGS__ )
#			define _printf_err_va( fmt, va ) vfprintf( stderr, fmt, va )
#		else
#			define _strlen                   c_str_len
#			define _printf_err( fmt, ... )   c_str_fmt_out_err( fmt, __VA_ARGS__ )
#			define _printf_err_va( fmt, va ) c_str_fmt_out_err_va( fmt, va )
#		endif
#	endif
#
#	include <errno.h>
#
#	if defined( GEN_SYSTEM_UNIX ) || defined( GEN_SYSTEM_MACOS )
#		include <unistd.h>
#	elif defined( GEN_SYSTEM_WINDOWS )
#		if ! defined( GEN_NO_WINDOWS_H )
#			ifndef WIN32_LEAN_AND_MEAN
#				ifndef NOMINMAX
#					define NOMINMAX
#				endif
#
#				define WIN32_LEAN_AND_MEAN
#				define WIN32_MEAN_AND_LEAN
#				define VC_EXTRALEAN
#			endif
#			include <windows.h>
#			undef NOMINMAX
#			undef WIN32_LEAN_AND_MEAN
#			undef WIN32_MEAN_AND_LEAN
#			undef VC_EXTRALEAN
#		endif
#	endif

#include <sys/stat.h>

#ifdef GEN_SYSTEM_MACOS
#	include <copyfile.h>
#endif

#ifdef GEN_SYSTEM_CYGWIN
#	include <windows.h>
#endif

#if defined( GEN_SYSTEM_WINDOWS ) && ! defined( GEN_COMPILER_GCC )
#	include <io.h>
#endif

#if defined( GEN_SYSTEM_LINUX )
#	include <sys/types.h>
#endif

#ifdef GEN_BENCHMARK
// Timing includes
#if defined( GEN_SYSTEM_MACOS ) || GEN_SYSTEM_UNIX
#	include <time.h>
#	include <sys/time.h>
#endif

#if defined( GEN_SYSTEM_MACOS )
#	include <mach/mach.h>
#	include <mach/mach_time.h>
#	include <mach/clock.h>
#endif

#if defined( GEN_SYSTEM_EMSCRIPTEN )
#	include <emscripten.h>
#endif

#if defined( GEN_SYSTEM_WINDOWS )
#	include <timezoneapi.h>
#endif
#endif

#pragma endregion Macros and Includes
