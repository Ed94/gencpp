#pragma once
#if INTELLISENSE_DIRECTIVES
#include "compiler.hpp"
#endif

#ifdef Compiler_MSVC
#pragma warning( disable: 4201 ) // Support for non-standard nameless struct or union extesnion
#pragma warning( disable: 4100 ) // Support for unreferenced formal parameters
#pragma warning( disable: 4800 ) // Support implicit conversion to bools
#pragma warning( disable: 4365 ) // Support for signed/unsigned mismatch auto-conversion
#pragma warning( disable: 4189 ) // Support for unused variables
#pragma warning( disable: 4514 ) // Support for unused inline functions
#pragma warning( disable: 4505 ) // Support for unused static functions
#pragma warning( disable: 5045 ) // Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
#pragma warning( disable: 5264 ) // Support for 'const' variables unused
#pragma warning( disable: 4820 ) // Support auto-adding padding to structs
#pragma warning( disable: 4711 ) // Support automatic inline expansion
#pragma warning( disable: 4710 ) // Support automatic inline expansion
#pragma warning( disable: 4805 ) // Support comparisons of s32 to bool.
#pragma warning( disable: 5246 ) // Support for initialization of subobject without braces.
#endif

#ifdef Compiler_Clang
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wvarargs"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif
