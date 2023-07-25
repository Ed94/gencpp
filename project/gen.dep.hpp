// This file is intended to be included within gen.hpp (There is no pragma diagnostic ignores)
#pragma once

#include "dependencies/gen.header_start.hpp"

#ifdef GEN_DONT_USE_NAMESPACE
#	define GEN_NS_BEGIN
#	define GEN_NS_END
#else
#	define GEN_NS_BEGIN namespace gen {
#	define GEN_NS_END   }
#endif

GEN_NS_BEGIN

#include "dependencies/gen.macros.hpp"
#include "dependencies/gen.basic_types.hpp"
#include "dependencies/gen.debug.hpp"
#include "dependencies/gen.memory.hpp"
#include "dependencies/gen.string_ops.hpp"
#include "dependencies/gen.printing.hpp"
#include "dependencies/gen.containers.hpp"
#include "dependencies/gen.string.hpp"
#include "dependencies/gen.file_handling.hpp"
#include "dependencies/gen.parsing.hpp"
#include "dependencies/gen.timing.hpp"

GEN_NS_END
