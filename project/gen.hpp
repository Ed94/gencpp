/*
	gencpp: An attempt at "simple" staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Public Address:
	https://github.com/Ed94/gencpp
*/
#pragma once

#include "helpers/gen.push_ignores.inline.hpp"
#include "components/gen.header_start.hpp"

GEN_NS_BEGIN

#include "components/gen.types.hpp"
#include "components/gen.data_structures.hpp"
#include "components/gen.interface.hpp"
#include "components/gen.header_end.hpp"

#include "filesystem/gen.builder.hpp"

GEN_NS_END

#include "helpers/gen.pop_ignores.inline.hpp"
