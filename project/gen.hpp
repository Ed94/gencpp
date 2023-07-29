/*
	gencpp: An attempt at "simple" staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Public Address:
	https://github.com/Ed94/gencpp
*/
#pragma once

#include "helpers/push_ignores.inline.hpp"
#include "components/header_start.hpp"

#ifdef GEN_DONT_USE_NAMESPACE
#	define GEN_NS_BEGIN
#	define GEN_NS_END
#else
#	define GEN_NS_BEGIN namespace gen {
#	define GEN_NS_END   }
#endif

GEN_NS_BEGIN

#include "components/types.hpp"
#include "components/ecode.hpp"
#include "components/eoperator.hpp"
#include "components/especifier.hpp"
#include "components/data_structures.hpp"
#include "components/interface.hpp"
#include "components/header_end.hpp"

#include "file_processors/builder.hpp"

GEN_NS_END

#include "helpers/pop_ignores.inline.hpp"
