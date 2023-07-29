// This file is intended to be included within gen.hpp (There is no pragma diagnostic ignores)
#pragma once

#include "dependencies/header_start.hpp"

#ifdef GEN_DONT_USE_NAMESPACE
#	define GEN_NS_BEGIN
#	define GEN_NS_END
#else
#	define GEN_NS_BEGIN namespace gen {
#	define GEN_NS_END   }
#endif

GEN_NS_BEGIN

#include "dependencies/macros.hpp"
#include "dependencies/basic_types.hpp"
#include "dependencies/debug.hpp"
#include "dependencies/memory.hpp"
#include "dependencies/string_ops.hpp"
#include "dependencies/printing.hpp"
#include "dependencies/containers.hpp"
#include "dependencies/string.hpp"
#include "dependencies/parsing.hpp"
#include "dependencies/timing.hpp"

#include "dependencies/file_handling.hpp"

GEN_NS_END
