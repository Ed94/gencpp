/*
	gencpp: An attempt at "simple" staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Define GEN_IMPLEMENTATION before including this file in a single compilation unit.

	Public Address:
	https://github.com/Ed94/gencpp  --------------------------------------------------------------.
	|   _____                               _____ _                       _                        |
	|  / ____)                             / ____} |                     | |                       |
	| | / ___  ___ _ __   ___ _ __  _ __  | {___ | |__ _ _, __ _, ___  __| |                       |
	| | |{_  |/ _ \ '_ \ / __} '_ l| '_ l `\___ \| __/ _` |/ _` |/ _ \/ _` |                       |
	| | l__j | ___/ | | | {__; |+l } |+l | ____) | l| (_| | {_| | ___/ (_| |                       |
	|  \_____|\___}_l |_|\___} ,__/| ,__/ (_____/ \__\__/_|\__, |\___}\__,_l                       |
	|     Singleheader       | |   | |                      __} |                                  |
	|                        l_l   l_l                     {___/                                   |
	! ----------------------------------------------------------------------- VERSION: v0.25-Alpha |
	! ============================================================================================ |
	! WARNING: THIS IS AN ALPHA VERSION OF THE LIBRARY, USE AT YOUR OWN DISCRETION                 |
	! NEVER DO CODE GENERATION WITHOUT AT LEAST HAVING CONTENT IN A CODEBASE UNDER VERSION CONTROL |
	! ============================================================================================ /
*/
#if ! defined(GEN_DONT_ENFORCE_GEN_TIME_GUARD) && ! defined(GEN_TIME)
#	error Gen.hpp : GEN_TIME not defined
#endif
