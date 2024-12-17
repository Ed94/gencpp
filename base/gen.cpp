#include "helpers/push_ignores.inline.hpp"

// ReSharper disable CppClangTidyClangDiagnosticSwitchEnum

#if ! defined(GEN_DONT_ENFORCE_GEN_TIME_GUARD) && ! defined(GEN_TIME)
#	error Gen.hpp : GEN_TIME not defined
#endif

#include "gen.hpp"

// These are intended for use in the base library of gencpp and the C-variant of the library
// It provides a interoperability between the C++ and C interfacing for containers. (not letting these do any crazy substiution though)
// They are undefined in gen.hpp and gen.cpp at the end of the files.
// We cpp library expects the user to use the regular calls as they can resolve the type fine.

#include "helpers/push_container_defines.inline.hpp"

//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
//! Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES
#	include "gen.dep.cpp"
#endif

GEN_NS_BEGIN

#include "components/static_data.cpp"

#include "components/ast_case_macros.cpp"
#include "components/ast.cpp"
#include "components/code_serialization.cpp"

#include "components/interface.cpp"
#include "components/interface.upfront.cpp"
#include "components/lexer.cpp"
#include "components/parser_case_macros.cpp"
#include "components/parser.cpp"
#include "components/interface.parsing.cpp"
#include "components/interface.untyped.cpp"

#include "auxiliary/builder.cpp"
#include "auxiliary/scanner.cpp"

GEN_NS_END

#include "helpers/pop_container_defines.inline.hpp"
#include "helpers/pop_ignores.inline.hpp"
