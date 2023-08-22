/*
	gencpp: An attempt at "simple" staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Public Address:
	https://github.com/Ed94/gencpp
*/
#pragma once

#include "helpers/push_ignores.inline.hpp"
#include "components/header_start.hpp"

namespace gen {

#include "components/types.hpp"
#include "components/temp/ecode.hpp"
#include "components/temp/eoperator.hpp"
#include "components/temp/especifier.hpp"

#include "components/ast.hpp"
#include "components/ast_types.hpp"

#include "components/interface.hpp"

#include "components/inlines.hpp"
#include "components/temp/ast_inlines.hpp"
#include "components/header_end.hpp"

}

#include "helpers/pop_ignores.inline.hpp"
