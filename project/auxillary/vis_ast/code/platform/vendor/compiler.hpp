// Platform compiler
#pragma once

#if defined( _MSC_VER )
#	define Compiler_MSVC 1
#elif defined( __clang__ )
#	define Compiler_Clang 1
#else
#	error "Unknown compiler"
#endif

#if defined( __has_attribute )
#	define HAS_ATTRIBUTE( attribute ) __has_attribute( attribute )
#else
#	define HAS_ATTRIBUTE( attribute ) ( 0 )
#endif

#ifdef Compiler_Clang
#	define compiler_decorated_func_name __PRETTY_NAME__
#elif defined(Compiler_MSVC)
#	define compiler_decorated_func_name __FUNCDNAME__
#endif
