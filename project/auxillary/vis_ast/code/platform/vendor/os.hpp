// Platform OS detection
#pragma once

#if defined( _WIN32 ) || defined( _WIN64 )
#	ifndef System_Windows
#		define System_Windows 1
#	endif
#elif defined( __APPLE__ ) && defined( __MACH__ )
#	ifndef System_MacOS
#		define System_MacOS 1
#	endif
#elif defined( __unix__ )
#	if defined( __linux__ )
#		ifndef System_Linux
#			define System_linux 1
#		endif
#	else
#		error This UNIX operating system is not supported
#	endif
#else
#	error This operating system is not supported
#endif
