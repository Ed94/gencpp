/*
	gencpp: An attempt at "simple" staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Public Address:
	https://github.com/Ed94/gencpp

	This is a single header variant of the library.
	Define GEN_IMPLEMENTATION before including this file in a single compilation unit.
*/
#if ! defined(GEN_DONT_ENFORCE_GEN_TIME_GUARD) && ! defined(GEN_TIME)
#	error Gen.hpp : GEN_TIME not defined
#endif

#ifdef GEN_DONT_USE_NAMESPACE
#	define GEN_NS_BEGIN
#	define GEN_NS_END
#else
#	define GEN_NS_BEGIN namespace gen {
#	define GEN_NS_END   }
#endif

