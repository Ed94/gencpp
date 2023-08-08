using LogFailType = sw(*)(char const*, ...);

// By default this library will either crash or exit if an error is detected while generating codes.
// Even if set to not use fatal, fatal will still be used for memory failures as the library is unusable when they occur.
#ifdef GEN_DONT_USE_FATAL
	#define log_failure log_fmt
#else
	#define log_failure fatal
#endif

enum class AccessSpec : u32
{
	Default,
	Public,
	Protected,
	Private,

	Num_AccessSpec,
	Invalid,
};

inline
char const* to_str( AccessSpec type )
{
	local_persist
	char const* lookup[ (u32)AccessSpec::Num_AccessSpec ] = {
		"",
		"public",
		"protected",
		"private",
	};

	if ( type > AccessSpec::Public )
		return "Invalid";

	return lookup[ (u32)type ];
}

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
	// Private = bit(2),

	Num_ModuleFlags,
	Invalid,
};

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

/*
	Predefined attributes
	Used for the parser constructors to identify non-standard attributes

	Override these to change the attribute to your own unique identifier convention.

	The tokenizer identifies attribute defines with the GEN_DEFINE_ATTRIBUTE_TOKENS macros.
	See the example below and the Define_TokType macro used in gen.cpp to know the format.
	While the library can parse raw attributes, most projects use defines to wrap them for compiler
	platform indendence. The token define allows support for them without having to modify the library.
*/
#if defined(GEN_SYSTEM_WINDOWS) || defined( __CYGWIN__ )
#ifndef GEN_Attribute_Keyword
#	define GEN_API_Export_Code   __declspec(dllexport)
#	define GEN_API_Import_Code   __declspec(dllimport)
#	define GEN_Attribute_Keyword __declspec
#endif

constexpr char const* Attribute_Keyword = stringize( GEN_Attribute_Keyword);

#elif GEN_HAS_ATTRIBUTE( visibility ) || GEN_GCC_VERSION_CHECK( 3, 3, 0 )
#ifndef GEN_Attribute_Keyword
#	define GEN_API_Export_Code   __attribute__ ((visibility ("default")))
#	define GEN_API_Import_Code   __attribute__ ((visibility ("default")))
#	define GEN_Attribute_Keyword __attribute__
#endif

constexpr char const* Attribute_Keyword = stringize( GEN_Attribute_Keyword );

#else
#ifndef GEN_Attribute_Keyword
#	define GEN_API_Export_Code
#	define GEN_API_Import_Code
#	define GEN_Attribute_Keyword
#endif

constexpr char const* Attribute_Keyword = "";

#endif

// Implements basic string interning. Data structure is based off the ZPL Hashtable.
using StringTable = HashTable<String const>;

// Represents strings cached with the string table.
// Should never be modified, if changed string is desired, cache_string( str ) another.
using StringCached = String const;

