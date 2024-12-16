/*
	gencpp: An attempt at "simple" staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Public Address:
	https://github.com/Ed94/gencpp

	This is a variant intended for use with Unreal Engine 5
	https://github.com/Ed94/gencpp  --------------------------------------------------------------.
	|   _____                               _____ _                       _                        |
	|  / ____)                             / ____} |                     | |                       |
	| | / ___  ___ _ __   ___ _ __  _ __  | {___ | |__ _ _, __ _, ___  __| |                       |
	| | |{_  |/ _ \ '_ \ / __} '_ l| '_ l `\___ \| __/ _` |/ _` |/ _ \/ _` |                       |
	| | l__j | ___/ | | | {__; |+l } |+l | ____) | l| (_| | {_| | ___/ (_| |                       |
	|  \_____|\___}_l |_|\___} ,__/| ,__/ (_____/ \__\__/_|\__, |\___}\__,_l                       |
	|  Unreal Engine         | |   | |                      __} |                                  |
	|                        l_l   l_l                     {___/                                   |
	! ----------------------------------------------------------------------- VERSION: v0.23-Alpha |
	! ============================================================================================ |
	! WARNING: THIS IS AN ALPHA VERSION OF THE LIBRARY, USE AT YOUR OWN DISCRETION                 |
	! NEVER DO CODE GENERATION WITHOUT AT LEAST HAVING CONTENT IN A CODEBASE UNDER VERSION CONTROL |
	! ============================================================================================ /
*/
#if ! defined(GEN_DONT_ENFORCE_GEN_TIME_GUARD) && ! defined(GEN_TIME)
#	error Gen.hpp : GEN_TIME not defined
#endif

//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
// Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES
#	include "gen.dep.hpp"
#endif

#ifndef GEN_NS_BEGIN
#	ifdef GEN_DONT_USE_NAMESPACE
#		define GEN_NS
#		define GEN_NS_BEGIN
#		define GEN_NS_END
#	else
#		define GEN_NS       gen::
#		define GEN_NS_BEGIN namespace gen {
#		define GEN_NS_END   }
#	endif
#endif
