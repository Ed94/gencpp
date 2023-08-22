#include "helpers/push_ignores.inline.hpp"

// ReSharper disable CppClangTidyClangDiagnosticSwitchEnum

#if ! defined(GEN_DONT_ENFORCE_GEN_TIME_GUARD) && ! defined(GEN_TIME)
#	error Gen.hpp : GEN_TIME not defined
#endif

#include "gen.hpp"

//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
//! Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES
#	include "gen.dep.cpp"
#endif

namespace gen {

#include "components/static_data.cpp"

#include "components/ast_case_macros.cpp"
#include "components/ast.cpp"

#include "components/interface.cpp"
#include "components/interface.upfront.cpp"
#include "components/temp/etoktype.cpp"
#include "components/interface.parsing.cpp"
#include "components/interface.untyped.cpp"

}

#include "helpers/pop_ignores.inline.hpp"
