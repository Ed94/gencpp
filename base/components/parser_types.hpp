#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "types.hpp"
#include "gen/ecode.hpp"
#include "gen/eoperator.hpp"
#include "gen/especifier.hpp"
#include "gen/etoktype.hpp"
#endif

enum TokFlags : u32
{
	TF_Operator		         = bit(0),
	TF_Assign                = bit(1),
	TF_Preprocess            = bit(2),
	TF_Preprocess_Cond       = bit(3),
	TF_Attribute             = bit(6),
	TF_AccessOperator        = bit(7),
	TF_AccessSpecifier       = bit(8),
	TF_Specifier             = bit(9),
	TF_EndDefinition         = bit(10),    // Either ; or }
	TF_Formatting            = bit(11),
	TF_Literal               = bit(12),
	TF_Macro_Functional      = bit(13),
	TF_Macro_Expects_Body    = bit(14),

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
Str tok_to_str(Token tok) {
	return tok.Text;
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

struct StackNode
{
	StackNode* Prev;

	Token* Start;
	Str    Name;          // The name of the AST node (if parsed)
	Str    ProcName;    // The name of the procedure
};

struct ParseContext
{
	TokArray   Tokens;
	StackNode* Scope;
};

enum MacroType : u16
{
	MT_Expression,     // A macro is assumed to be a expression if not resolved.
	MT_Statement,      
	MT_Typename,
	MT_Attribute,      // More of a note to the parser than anythign else (attributes should be defined in the user attribues def).
	MT_Specifier,      // More of a note to the parser than anythign else (specifiers should be defined in the user attribues def).
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

Str macrotype_to_str( MacroType type )
{
	local_persist
	Str lookup[] = {
		{ "Statement",        sizeof("Statement")        - 1 },
		{ "Expression",       sizeof("Expression")       - 1 },
		{ "Typename",         sizeof("Typename")         - 1 },
		{ "Attribute(Macro)", sizeof("Attribute(Macro)") - 1 },
		{ "Specifier(Macro)", sizeof("Specifier(Macro)") - 1 },
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
	MF_Functional          = bit(0), // Macro has parameters (args expected to be passed)
	MF_Expects_Body        = bit(1), // Expects to assign a braced scope to its body.
	MF_Allow_As_Identifier = bit(2), // lex__eat wil treat this macro as an identifier if the parser attempts to consume it as one.
                                     //  ^^^ This is a sort of kludge because we don't support push/pop macro programs rn. ^^^

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

typedef HashTable(Macro) MacroTable;
