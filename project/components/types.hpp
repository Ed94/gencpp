#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "header_start.hpp"
#endif

using LogFailType = ssize(*)(char const*, ...);

// By default this library will either crash or exit if an error is detected while generating codes.
// Even if set to not use GEN_FATAL, GEN_FATAL will still be used for memory failures as the library is unusable when they occur.
#ifdef GEN_DONT_USE_FATAL
	#define log_failure log_fmt
#else
	#define log_failure GEN_FATAL
#endif

enum AccessSpec enum_underlying(u32)
{
	AccessSpec_Default,
	AccessSpec_Private,
	AccessSpec_Protected,
	AccessSpec_Public,

	AccessSpec_Num_AccessSpec,
	AccessSpec_Invalid,

	AccessSpec_SizeDef = GEN_U32_MAX,
};
static_assert( size_of(AccessSpec) == size_of(u32));

inline
char const* to_str( AccessSpec type )
{
	local_persist
	char const* lookup[ (u32)AccessSpec_Num_AccessSpec ] = {
		"",
		"private",
		"protected",
		"public",
	};

	if ( type > AccessSpec_Public )
		return "Invalid";

	return lookup[ (u32)type ];
}

enum CodeFlag enum_underlying(u32)
{
	CodeFlag_None          = 0,
	CodeFlag_FunctionType  = bit(0),
	CodeFlag_ParamPack     = bit(1),
	CodeFlag_Module_Export = bit(2),
	CodeFlag_Module_Import = bit(3),

	CodeFlag_SizeDef = GEN_U32_MAX,
};
static_assert( size_of(CodeFlag) == size_of(u32));

// Used to indicate if enum definitoin is an enum class or regular enum.
enum EnumDecl enum_underlying(u8)
{
	EnumDecl_Regular,
	EnumDecl_Class,

	EnumT_SizeDef = GEN_U8_MAX,
};
typedef u8 EnumT;

enum ModuleFlag enum_underlying(u32)
{
	ModuleFlag_None    = 0,
	ModuleFlag_Export  = bit(0),
	ModuleFlag_Import  = bit(1),

	Num_ModuleFlags,
	ModuleFlag_Invalid,

	ModuleFlag_SizeDef = GEN_U32_MAX,
};
static_assert( size_of(ModuleFlag) == size_of(u32));

inline
StrC to_str( ModuleFlag flag )
{
	local_persist
	StrC lookup[ (u32)ModuleFlag::Num_ModuleFlags ] = {
		{ sizeof("__none__"), "__none__" },
		{ sizeof("export"), "export" },
		{ sizeof("import"), "import" },
	};

	if ( flag > ModuleFlag_Import )
		return { sizeof("invalid"), "invalid" };

	return lookup[ (u32)flag ];
}

inline
ModuleFlag operator|( ModuleFlag A, ModuleFlag B)
{
	return (ModuleFlag)( (u32)A | (u32)B );
}

enum EPreprocessCond enum_underlying(u32)
{
	PreprocessCond_If,
	PreprocessCond_IfDef,
	PreprocessCond_IfNotDef,
	PreprocessCond_ElIf,

	EPreprocessCond_SizeDef = GEN_U32_MAX,
};
static_assert( size_of(EPreprocessCond) == size_of(u32));
