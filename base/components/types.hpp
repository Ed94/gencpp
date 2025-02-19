#ifdef INTELLISENSE_DIRECTIVES
#pragma once
#include "dependencies/platform.hpp"
#include "dependencies/macros.hpp"
#include "dependencies/basic_types.hpp"
#include "dependencies/debug.hpp"
#include "dependencies/memory.hpp"
#include "dependencies/string_ops.hpp"
#include "dependencies/printing.hpp"
#include "dependencies/containers.hpp"
#include "dependencies/hashing.hpp"
#include "dependencies/strings.hpp"
#include "dependencies/filesystem.hpp"
#include "dependencies/timing.hpp"
#include "dependencies/parsing.hpp"
#endif

/*
 ________                                              __    __      ________
|        \                                            |  \  |  \    |        \
| ▓▓▓▓▓▓▓▓_______  __    __ ______ ____   _______     | ▓▓\ | ▓▓     \▓▓▓▓▓▓▓▓__    __  ______   ______   _______
| ▓▓__   |       \|  \  |  \      \    \ /       \    | ▓▓▓\| ▓▓       | ▓▓  |  \  |  \/      \ /      \ /       \
| ▓▓  \  | ▓▓▓▓▓▓▓\ ▓▓  | ▓▓ ▓▓▓▓▓▓\▓▓▓▓\  ▓▓▓▓▓▓▓    | ▓▓▓▓\ ▓▓       | ▓▓  | ▓▓  | ▓▓  ▓▓▓▓▓▓\  ▓▓▓▓▓▓\  ▓▓▓▓▓▓▓
| ▓▓▓▓▓  | ▓▓  | ▓▓ ▓▓  | ▓▓ ▓▓ | ▓▓ | ▓▓\▓▓    \     | ▓▓\▓▓ ▓▓       | ▓▓  | ▓▓  | ▓▓ ▓▓  | ▓▓ ▓▓    ▓▓\▓▓    \
| ▓▓_____| ▓▓  | ▓▓ ▓▓__/ ▓▓ ▓▓ | ▓▓ | ▓▓_\▓▓▓▓▓▓\    | ▓▓ \▓▓▓▓       | ▓▓  | ▓▓__/ ▓▓ ▓▓__/ ▓▓ ▓▓▓▓▓▓▓▓_\▓▓▓▓▓▓\
| ▓▓     \ ▓▓  | ▓▓\▓▓    ▓▓ ▓▓ | ▓▓ | ▓▓       ▓▓    | ▓▓  \▓▓▓       | ▓▓   \▓▓    ▓▓ ▓▓    ▓▓\▓▓     \       ▓▓
 \▓▓▓▓▓▓▓▓\▓▓   \▓▓ \▓▓▓▓▓▓ \▓▓  \▓▓  \▓▓\▓▓▓▓▓▓▓      \▓▓   \▓▓        \▓▓   _\▓▓▓▓▓▓▓ ▓▓▓▓▓▓▓  \▓▓▓▓▓▓▓\▓▓▓▓▓▓▓
                                                                             |  \__| ▓▓ ▓▓
                                                                              \▓▓    ▓▓ ▓▓
                                                                               \▓▓▓▓▓▓ \▓▓

*/

enum AccessSpec : u32
{
	AccessSpec_Default,
	AccessSpec_Private,
	AccessSpec_Protected,
	AccessSpec_Public,

	AccessSpec_Num_AccessSpec,
	AccessSpec_Invalid,

	AccessSpec_SizeDef = GEN_U32_MAX,
};
static_assert( size_of(AccessSpec) == size_of(u32), "AccessSpec not u32 size" );

inline
Str access_spec_to_str( AccessSpec type )
{
	local_persist
	Str lookup[ (u32)AccessSpec_Num_AccessSpec ] = {
		{ "",        sizeof( "" )        - 1 },
		{ "private", sizeof("prviate")   - 1 },
		{ "private", sizeof("protected") - 1 },
		{ "public",  sizeof("public")    - 1 },
	};

	Str invalid = { "Invalid", sizeof("Invalid") - 1 };
	if ( type > AccessSpec_Public )
		return invalid;

	return lookup[ (u32)type ];
}

enum CodeFlag : u32
{
	CodeFlag_None          = 0,
	CodeFlag_FunctionType  = bit(0),
	CodeFlag_ParamPack     = bit(1),
	CodeFlag_Module_Export = bit(2),
	CodeFlag_Module_Import = bit(3),

	CodeFlag_SizeDef = GEN_U32_MAX,
};
static_assert( size_of(CodeFlag) == size_of(u32), "CodeFlag not u32 size" );

// Used to indicate if enum definitoin is an enum class or regular enum.
enum EnumDecl : u8
{
	EnumDecl_Regular,
	EnumDecl_Class,

	EnumT_SizeDef = GEN_U8_MAX,
};
typedef u8 EnumT;

enum ModuleFlag : u32
{
	ModuleFlag_None    = 0,
	ModuleFlag_Export  = bit(0),
	ModuleFlag_Import  = bit(1),

	Num_ModuleFlags,
	ModuleFlag_Invalid,

	ModuleFlag_SizeDef = GEN_U32_MAX,
};
static_assert( size_of(ModuleFlag) == size_of(u32), "ModuleFlag not u32 size" );

inline
Str module_flag_to_str( ModuleFlag flag )
{
	local_persist
	Str lookup[ (u32)Num_ModuleFlags ] = {
		{ "__none__", sizeof("__none__") - 1 },
		{ "export",   sizeof("export")   - 1 },
		{ "import",   sizeof("import")   - 1 },
	};

	local_persist
	Str invalid_flag = { "invalid", sizeof("invalid") };
	if ( flag > ModuleFlag_Import )
		return invalid_flag;

	return lookup[ (u32)flag ];
}

enum EPreprocessCond : u32
{
	PreprocessCond_If,
	PreprocessCond_IfDef,
	PreprocessCond_IfNotDef,
	PreprocessCond_ElIf,

	EPreprocessCond_SizeDef = GEN_U32_MAX,
};
static_assert( size_of(EPreprocessCond) == size_of(u32), "EPreprocessCond not u32 size" );

enum ETypenameTag : u16
{
	Tag_None,
	Tag_Class,
	Tag_Enum,
	Tag_Struct,
	Tag_Union,

	Tag_UnderlyingType = GEN_U16_MAX,
};
static_assert( size_of(ETypenameTag) == size_of(u16), "ETypenameTag is not u16 size");
