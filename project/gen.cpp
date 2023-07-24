// ReSharper disable CppClangTidyClangDiagnosticSwitchEnum

#if ! defined(GEN_DONT_ENFORCE_GEN_TIME_GUARD) && ! defined(GEN_TIME)
#error Gen.hpp : GEN_TIME not defined
#endif

#include "helpers/gen.push_ignores.inline.hpp"

//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
//! Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES
#	include "dependencies/gen.dep.cpp"
#endif

#include "gen.hpp"

GEN_NS_BEGIN

#include "components/gen.data.cpp"

#include "components/gen.ast_case_macros.cpp"
#include "components/gen.ast.cpp"

#include "components/gen.interface.cpp"
#include "components/gen.interface.upfront.cpp"
#include "components/gen.interface.parsing.cpp"
#include "components/gen.untyped.cpp"

#include "filesystem/gen.builder.cpp"

GEN_NS_END

#include "helpers/gen.pop_ignores.inline.hpp"
