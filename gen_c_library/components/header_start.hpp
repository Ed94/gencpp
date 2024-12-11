/*
	gencpp: An attempt at "simple" staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Public Address:
	https://github.com/Ed94/gencpp  ---------------------------------------------------------------.
	|   _____                               _____ _                       _      ___   __     __    |
	|  / ____)                             / ____} |                     | |   / ,__} /  |   /  |   |
	| | / ___  ___ _ __   ___ _ __  _ __  | {___ | l_ __ _  __ _, ___  __| |  | |    '-l |  '-l |   |
	| | |{_  \/ __\ '_ \ / __} '_ l| '_ l  \___ \| __/ _` |/ _` |/ __\/ _` |  | |      | |    | |   |
	| | l__j | ___/ | | | {__; ;_l } ;_l } ____} | l| (_} | {_| | ___j {_; |  | l___  _J l_  _J l_  |
	|  \_____|\___}_l |_|\___} .__/| .__/ {_____/ \__\__/_l\__. |\___/\__,_l   \____}{_____}{_____} |
	|                        | |   | |                      __} |                                   |
	|                        l_l   l_l                     {___/                                    |
	! ----------------------------------------------------------------------- VERSION: v0.20-Alpha  |
	! ============================================================================================= |
	! WARNING: THIS IS AN ALPHA VERSION OF THE LIBRARY, USE AT YOUR OWN DISCRETION                  |
	! NEVER DO CODE GENERATION WITHOUT AT LEAST HAVING CONTENT IN A CODEBASE UNDER VERSION CONTROL  |
	! ============================================================================================= /
*/
#if ! defined(GEN_DONT_ENFORCE_GEN_TIME_GUARD) && ! defined(GEN_TIME)
#	error Gen.hpp : GEN_TIME not defined
#endif
