#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "header_start.hpp"
#endif

using LogFailType = sw(*)(char const*, ...);

// By default this library will either crash or exit if an error is detected while generating codes.
// Even if set to not use GEN_FATAL, GEN_FATAL will still be used for memory failures as the library is unusable when they occur.
#ifdef GEN_DONT_USE_FATAL
	#define log_failure log_fmt
#else
	#define log_failure GEN_FATAL
#endif

enum class AccessSpec : u32
{
	Default,
	Private,
	Protected,
	Public,

	Num_AccessSpec,
	Invalid,
};

inline
char const* to_str( AccessSpec type )
{
	local_persist
	char const* lookup[ (u32)AccessSpec::Num_AccessSpec ] = {
		"",
		"private",
		"protected",
		"public",
	};

	if ( type > AccessSpec::Public )
		return "Invalid";

	return lookup[ (u32)type ];
}


enum CodeFlag : u32
{
	None          = 0,
	FunctionType  = bit(0),
	ParamPack     = bit(1),
	Module_Export = bit(2),
	Module_Import = bit(3),
};

// Used to indicate if enum definitoin is an enum class or regular enum.
enum class EnumT : u8
{
	Regular,
	Class
};

constexpr EnumT EnumClass   = EnumT::Class;
constexpr EnumT EnumRegular = EnumT::Regular;

enum class ModuleFlag : u32
{
	None    = 0,
	Export  = bit(0),
	Import  = bit(1),

	Num_ModuleFlags,
	Invalid,
};

inline
StrC to_str( ModuleFlag flag )
{
	local_persist
	StrC lookup[ (u32)ModuleFlag::Num_ModuleFlags ] = {
		{ sizeof("__none__"), "__none__" },
		{ sizeof("export"), "export" },
		{ sizeof("import"), "import" },
	};

	if ( flag > ModuleFlag::Import )
		return { sizeof("invalid"), "invalid" };

	return lookup[ (u32)flag ];
}

inline
ModuleFlag operator|( ModuleFlag A, ModuleFlag B)
{
	return (ModuleFlag)( (u32)A | (u32)B );
}

enum class EPreprocessCond : u32
{
	If,
	IfDef,
	IfNotDef,
	ElIf
};

constexpr EPreprocessCond PreprocessCond_If       = EPreprocessCond::If;
constexpr EPreprocessCond PreprocessCond_IfDef    = EPreprocessCond::IfDef;
constexpr EPreprocessCond PreprocessCond_IfNotDef = EPreprocessCond::IfNotDef;
constexpr EPreprocessCond PreprocessCond_ElIf     = EPreprocessCond::ElIf;
