/* Platform architecture */

#if defined( _WIN64 ) || defined( __x86_64__ ) || defined( _M_X64 ) || defined( __64BIT__ ) || defined( __powerpc64__ ) || defined( __ppc64__ ) || defined( __aarch64__ )
#	ifndef ARCH_64_BIT
#		define ARCH_64_BIT 1
#	endif
#else
#	error A 32-bit architecture is not supported
#endif
