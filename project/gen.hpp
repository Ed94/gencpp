/*
	gencpp: An attempt at "simple" staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Public Address:
	https://github.com/Ed94/gencpp
*/
#pragma once

#if ! defined(GEN_DONT_ENFORCE_GEN_TIME_GUARD) && ! defined(GEN_TIME)
#error Gen.hpp : GEN_TIME not defined
#endif

#include "helpers/gen.push_ignores.inline.hpp"

//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
// Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES
#	include "dependencies/gen.dep.hpp"
#endif

#if defined(GEN_DONT_USE_NAMESPACE) && ! defined(GEN_NS_BEGIN)
#	define GEN_NS_BEGIN
#	define GEN_NS_END
#elif ! defined(GEN_NS_BEGIN)
#  define GEN_NS_BEGIN namespace gen {
#  define GEN_NS_END   }
#endif

GEN_NS_BEGIN

#include "components/gen.types.hpp"
#include "components/gen.data_structures.hpp"
#include "components/gen.interface.hpp"
#include "components/gen.header_end.hpp"

#include "filesystem/gen.builder.hpp"

GEN_NS_END

#include "helpers/gen.pop_ignores.inline.hpp"
