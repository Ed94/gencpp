#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "types.hpp"
#include "gen/ecode.hpp"
#include "gen/eoperator.hpp"
#include "gen/especifier.hpp"
#endif

enum MacroTypes : u16
{
	MT_Block_Start,    // Not Supported yet
	MT_Block_End,      // Not Supported yet
	MT_Case_Statement, // Not Supported yet
	MT_Expression,
	MT_Statement,
	MT_Typename,

	MF_UnderlyingType = GEN_U16_Max,
};

enum MacroFlags : u16
{
	MF_Functional     = bit(0), // Macro has parameters (args expected to be passed)
	MF_Expects_Body   = bit(1), // Expects to assign a braced scope to its body.

	MF_Null           = 0,
	MF_UnderlyingType = GEN_U16_Max,
};

struct PreprocessorMacro
{
	StrCached  Name;
	MacroTypes Type;
	MacroFlags Flags;
};

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
	return bitfield_is_equal( u32, tok.Flags, TF_AccessOperator );
}

forceinline
bool tok_is_access_specifier(Token tok) {
	return bitfield_is_equal( u32, tok.Flags, TF_AccessSpecifier );
}

forceinline
bool tok_is_attribute(Token tok) {
	return bitfield_is_equal( u32, tok.Flags, TF_Attribute );
}

forceinline
bool tok_is_operator(Token tok) {
	return bitfield_is_equal( u32, tok.Flags, TF_Operator );
}

forceinline
bool tok_is_preprocessor(Token tok) {
	return bitfield_is_equal( u32, tok.Flags, TF_Preprocess );
}

forceinline
bool tok_is_preprocess_cond(Token tok) {
	return bitfield_is_equal( u32, tok.Flags, TF_Preprocess_Cond );
}

forceinline
bool tok_is_specifier(Token tok) {
	return bitfield_is_equal( u32, tok.Flags, TF_Specifier );
}

forceinline
bool tok_is_end_definition(Token tok) {
	return bitfield_is_equal( u32, tok.Flags, TF_EndDefinition );
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
