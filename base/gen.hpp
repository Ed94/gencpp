/*
	gencpp: An attempt at "simple" staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Public Address:
	https://github.com/Ed94/gencpp
*/
#pragma once

#include "helpers/push_ignores.inline.hpp"
#include "components/header_start.hpp"

// Has container defines pushed
#include "gen.dep.hpp"

GEN_NS_BEGIN

#include "components/types.hpp"
#include "components/gen/ecodetypes.hpp"
#include "components/gen/eoperator.hpp"
#include "components/gen/especifier.hpp"
#include "components/gen/etoktype.hpp"
#include "components/parser_types.hpp"

#include "components/ast.hpp"
#include "components/code_types.hpp"
#include "components/ast_types.hpp"

#include "components/interface.hpp"

#include "components/inlines.hpp"
#include "components/gen/ast_inlines.hpp"
#include "components/header_end.hpp"

#include "auxillary/builder.hpp"
#include "auxillary/scanner.hpp"

GEN_NS_END

#include "helpers/pop_container_defines.inline.hpp"
#include "helpers/pop_ignores.inline.hpp"
