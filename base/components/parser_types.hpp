#ifdef INTELLISENSE_DIRECTIVES
#pragma once
#include "types.hpp"
#include "gen/ecode.hpp"
#include "gen/eoperator.hpp"
#include "gen/especifier.hpp"
#include "gen/etoktype.hpp"
#endif

enum TokFlags : u32
{
	TF_Operator              = bit(0),
	TF_Assign                = bit(1),
	TF_Identifier            = bit(2),
	TF_Preprocess            = bit(3),
	TF_Preprocess_Cond       = bit(4),
	TF_Attribute             = bit(5),
	TF_AccessOperator        = bit(6),
	TF_AccessSpecifier       = bit(7),
	TF_Specifier             = bit(8),
	TF_EndDefinition         = bit(9),    // Either ; or }
	TF_Formatting            = bit(10),
	TF_Literal               = bit(11),
	TF_Macro_Functional      = bit(12),
	TF_Macro_Expects_Body    = bit(13),

	TF_Null = 0,
	TF_UnderlyingType = GEN_U32_MAX,
};

struct Token
{
	Str     Text;
	TokType Type;
	s32     Line;
	s32     Column;
	u32     Flags;
};

constexpr Token NullToken { {}, Tok_Invalid, 0, 0, TF_Null };

forceinline
AccessSpec tok_to_access_specifier(Token tok) {
	return scast(AccessSpec, tok.Type);
}

forceinline
bool tok_is_valid( Token tok ) {
	return tok.Text.Ptr && tok.Text.Len && tok.Type != Tok_Invalid;
}

forceinline
bool tok_is_access_operator(Token tok) {
	return bitfield_is_set( u32, tok.Flags, TF_AccessOperator );
}

forceinline
bool tok_is_access_specifier(Token tok) {
	return bitfield_is_set( u32, tok.Flags, TF_AccessSpecifier );
}

forceinline
bool tok_is_attribute(Token tok) {
	return bitfield_is_set( u32, tok.Flags, TF_Attribute );
}

forceinline
bool tok_is_operator(Token tok) {
	return bitfield_is_set( u32, tok.Flags, TF_Operator );
}

forceinline
bool tok_is_preprocessor(Token tok) {
	return bitfield_is_set( u32, tok.Flags, TF_Preprocess );
}

forceinline
bool tok_is_preprocess_cond(Token tok) {
	return bitfield_is_set( u32, tok.Flags, TF_Preprocess_Cond );
}

forceinline
bool tok_is_specifier(Token tok) {
	return bitfield_is_set( u32, tok.Flags, TF_Specifier );
}

forceinline
bool tok_is_end_definition(Token tok) {
	return bitfield_is_set( u32, tok.Flags, TF_EndDefinition );
}

StrBuilder tok_to_strbuilder(Token tok);

struct TokArray 
{
	Array(Token) Arr;
	s32          Idx;
};

struct TokenSlice
{
	Token* Ptr;
	s32    Num;
};

struct LexContext
{
	Str             content;
	s32             left;
	char const*     scanner;
	s32             line;
	s32             column;
	// StringTable     defines;
	Token           token;
};

struct LexedInfo
{
	Str        text;
	TokenSlice tokens;
};

typedef struct ParseStackNode ParseStackNode;

struct ParseContext
{
	TokArray        Tokens;
	ParseStackNode* Scope;
};

enum MacroType : u16
{
	MT_Expression,     // A macro is assumed to be a expression if not resolved.
	MT_Statement,      
	MT_Typename,
	MT_Block_Start,    // Not Supported yet
	MT_Block_End,      // Not Supported yet
	MT_Case_Statement, // Not Supported yet

	MT_UnderlyingType = GEN_U16_MAX,
};

forceinline
TokType macrotype_to_toktype( MacroType type ) {
	switch ( type ) {
		case MT_Statement  : return Tok_Preprocess_Macro_Stmt;
		case MT_Expression : return Tok_Preprocess_Macro_Expr;
		case MT_Typename   : return Tok_Preprocess_Macro_Typename;
	}
	// All others unsupported for now.
	return Tok_Invalid;
}

inline
Str macrotype_to_str( MacroType type )
{
	local_persist
	Str lookup[] = {
		{ "Statement",        sizeof("Statement")        - 1 },
		{ "Expression",       sizeof("Expression")       - 1 },
		{ "Typename",         sizeof("Typename")         - 1 },
		{ "Block_Start",      sizeof("Block_Start")      - 1 },
		{ "Block_End",        sizeof("Block_End")        - 1 },
		{ "Case_Statement",   sizeof("Case_Statement")   - 1 },
	};
	local_persist
	Str invalid = { "Invalid", sizeof("Invalid") };
	if ( type > MT_Case_Statement )
		return invalid;

	return lookup[ type ];
}

enum EMacroFlags : u16
{
	// Macro has parameters (args expected to be passed)
	MF_Functional          = bit(0), 

	// Expects to assign a braced scope to its body.
	MF_Expects_Body        = bit(1), 

	// lex__eat wil treat this macro as an identifier if the parser attempts to consume it as one.
	// This is a kludge because we don't support push/pop macro pragmas rn.
	MF_Allow_As_Identifier = bit(2), 

	// When parsing identifiers, it will allow the consumption of the macro parameters (as its expected to be a part of constructing the identifier)
	// Example of a decarator macro from stb_sprintf.h: 
	// STBSP__PUBLICDEC int STB_SPRINTF_DECORATE(sprintf)(char* buf, char const *fmt, ...) STBSP__ATTRIBUTE_FORMAT(2,3);
	//                       ^^ STB_SPRINTF_DECORATE is decorating sprintf
	MF_Identifier_Decorator = bit(3), 

	// lex__eat wil treat this macro as an attribute if the parser attempts to consume it as one.
	// This a kludge because unreal has a macro that behaves as both a 'statement' and an attribute (UE_DEPRECATED, PRAGMA_ENABLE_DEPRECATION_WARNINGS, etc)
	// TODO(Ed): We can keep the MF_Allow_As_Attribute flag for macros, however, we need to add the ability of AST_Attributes to chain themselves.
	// Its thats already a thing in the standard language anyway
	// & it would allow UE_DEPRECATED, (UE_PROPERTY / UE_FUNCTION) to chain themselves as attributes of a resolved member function/variable definition
	MF_Allow_As_Attribute  = bit(4),

	// When a macro is encountered after attributes and specifiers while parsing a function, or variable:
	// It will consume the macro and treat it as resolving the definition.
	// (MUST BE OF MT_Statement TYPE)
	MF_Allow_As_Definition = bit(5),

	// Created for Unreal's PURE_VIRTUAL
	MF_Allow_As_Specifier = bit(6),

	MF_Null           = 0,
	MF_UnderlyingType = GEN_U16_MAX,
};
typedef u16 MacroFlags;

struct Macro
{
	StrCached  Name;
	MacroType  Type;
	MacroFlags Flags;
};

forceinline
b32 macro_is_functional( Macro macro ) {
	return bitfield_is_set( b16, macro.Flags, MF_Functional );
}

forceinline
b32 macro_expects_body( Macro macro ) {
	return bitfield_is_set( b16, macro.Flags, MF_Expects_Body );
}

#if GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP
forceinline b32 is_functional( Macro macro ) { return bitfield_is_set( b16, macro.Flags, MF_Functional ); }
forceinline b32 expects_body ( Macro macro ) { return bitfield_is_set( b16, macro.Flags, MF_Expects_Body ); }
#endif

typedef HashTable(Macro) MacroTable;
