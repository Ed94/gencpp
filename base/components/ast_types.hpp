#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "code_types.hpp"
#endif

#pragma region AST Types

/*
  ______   ______  ________      ________
 /      \ /      \|        \    |        \
|  ▓▓▓▓▓▓\  ▓▓▓▓▓▓\\▓▓▓▓▓▓▓▓     \▓▓▓▓▓▓▓▓__    __  ______   ______   _______
| ▓▓__| ▓▓ ▓▓___\▓▓  | ▓▓          | ▓▓  |  \  |  \/      \ /      \ /       \
| ▓▓    ▓▓\▓▓    \   | ▓▓          | ▓▓  | ▓▓  | ▓▓  ▓▓▓▓▓▓\  ▓▓▓▓▓▓\  ▓▓▓▓▓▓▓
| ▓▓▓▓▓▓▓▓_\▓▓▓▓▓▓\  | ▓▓          | ▓▓  | ▓▓  | ▓▓ ▓▓  | ▓▓ ▓▓    ▓▓\▓▓    \
| ▓▓  | ▓▓  \__| ▓▓  | ▓▓          | ▓▓  | ▓▓__/ ▓▓ ▓▓__/ ▓▓ ▓▓▓▓▓▓▓▓_\▓▓▓▓▓▓\
| ▓▓  | ▓▓\▓▓    ▓▓  | ▓▓          | ▓▓   \▓▓    ▓▓ ▓▓    ▓▓\▓▓     \       ▓▓
 \▓▓   \▓▓ \▓▓▓▓▓▓    \▓▓           \▓▓   _\▓▓▓▓▓▓▓ ▓▓▓▓▓▓▓  \▓▓▓▓▓▓▓\▓▓▓▓▓▓▓
                                         |  \__| ▓▓ ▓▓
                                          \▓▓    ▓▓ ▓▓
                                           \▓▓▓▓▓▓ \▓▓
*/

/*
	Show only relevant members of the AST for its type.
	AST* fields are replaced with Code types.
		- Guards assignemnts to AST* fields to ensure the AST is duplicated if assigned to another parent.
*/

struct AST_Body
{
	union {
		char  _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached Name;
	Code      Front;
	Code      Back;
	Token*    Tok;
	Code      Parent;
	CodeType  Type;
	char      _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	s32       NumEntries;
};
static_assert( sizeof(AST_Body) == sizeof(AST), "ERROR: AST_Body is not the same size as AST");

// TODO(Ed): Support chaining attributes (Use parameter linkage pattern)
struct AST_Attributes
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		StrCached     Content;
	};
	StrCached         Name;
	Code              Prev;
	Code              Next;
	Token*            Tok;
	Code              Parent;
	CodeType          Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Attributes) == sizeof(AST), "ERROR: AST_Attributes is not the same size as AST");

#if 0
struct AST_BaseClass
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached         Name;
	Code              Prev;
	Code              Next;
	Token*            Tok;
	Code              Parent;
	CodeType          Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_BaseClass) == sizeof(AST), "ERROR: AST_BaseClass is not the same size as AST");
#endif

struct AST_Comment
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		StrCached  Content;
	};
	StrCached         Name;
	Code              Prev;
	Code              Next;
	Token*            Tok;
	Code              Parent;
	CodeType          Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Comment) == sizeof(AST), "ERROR: AST_Comment is not the same size as AST");

struct AST_Class
{
	union {
		char                _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			CodeComment     InlineCmt; // Only supported by forward declarations
			CodeAttributes  Attributes;
			CodeSpecifiers  Specs; // Support for final
			CodeTypename    ParentType;
			char            _PAD_PARAMS_[ sizeof(AST*) ];
			CodeBody        Body;
			char            _PAD_PROPERTIES_2_[ sizeof(AST*) ];
		};
	};
	StrCached               Name;
	CodeTypename            Prev;
	CodeTypename            Next;
	Token*                  Tok;
	Code                    Parent;
	CodeType                Type;
	ModuleFlag              ModuleFlags;
	AccessSpec              ParentAccess;
};
static_assert( sizeof(AST_Class) == sizeof(AST), "ERROR: AST_Class is not the same size as AST");

struct AST_Constructor
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			CodeComment    InlineCmt; // Only supported by forward declarations
			char           _PAD_PROPERTIES_ [ sizeof(AST*) * 1 ];
			CodeSpecifiers Specs;
			Code           InitializerList;
			CodeParams     Params;
			Code           Body;
			char           _PAD_PROPERTIES_2_ [ sizeof(AST*) * 2 ];
		};
	};
	StrCached         Name;
	Code              Prev;
	Code              Next;
	Token*            Tok;
	Code              Parent;
	CodeType          Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Constructor) == sizeof(AST), "ERROR: AST_Constructor is not the same size as AST");

struct AST_Define
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			char              _PAD_PROPERTIES_ [ sizeof(AST*) * 4 ];
			CodeDefineParams  Params;
			Code              Body; // Should be completely serialized for now to a: StrCached Content.
			char              _PAD_PROPERTIES_2_ [ sizeof(AST*) * 1 ];
		};
	};
	StrCached Name;
	Code      Prev;
	Code      Next;
	Token*    Tok;
	Code      Parent;
	CodeType  Type;
	char      _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Define) == sizeof(AST), "ERROR: AST_Define is not the same size as AST");

struct AST_DefineParams
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached         Name;
	CodeDefineParams  Last;
	CodeDefineParams  Next;
	Token*            Tok;
	Code              Parent;
	CodeType          Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	s32               NumEntries;
};
static_assert( sizeof(AST_DefineParams) == sizeof(AST), "ERROR: AST_DefineParams is not the same size as AST");

struct AST_Destructor
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			CodeComment    InlineCmt;
			char           _PAD_PROPERTIES_ [ sizeof(AST*) * 1 ];
			CodeSpecifiers Specs;
			char           _PAD_PROPERTIES_2_ [ sizeof(AST*) * 2 ];
			Code           Body;
			char           _PAD_PROPERTIES_3_ [ sizeof(AST*) ];
		};
	};
	StrCached              Name;
	Code                   Prev;
	Code                   Next;
	Token*                 Tok;
	Code                   Parent;
	CodeType               Type;
	char                   _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Destructor) == sizeof(AST), "ERROR: AST_Destructor is not the same size as AST");

struct AST_Enum
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			CodeComment    InlineCmt;
			CodeAttributes Attributes;
			char           _PAD_SPEC_  [ sizeof(AST*) ];
			CodeTypename   UnderlyingType;
			Code           UnderlyingTypeMacro;
			CodeBody       Body;
			char           _PAD_PROPERTIES_2_[ sizeof(AST*) ];
		};
	};
	StrCached              Name;
	Code                   Prev;
	Code                   Next;
	Token*                 Tok;
	Code                   Parent;
	CodeType               Type;
	ModuleFlag             ModuleFlags;
	char                   _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Enum) == sizeof(AST), "ERROR: AST_Enum is not the same size as AST");

struct AST_Exec
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		StrCached  Content;
	};
	StrCached         Name;
	Code              Prev;
	Code              Next;
	Token*            Tok;
	Code              Parent;
	CodeType          Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Exec) == sizeof(AST), "ERROR: AST_Exec is not the same size as AST");

#ifdef GEN_EXECUTION_EXPRESSION_SUPPORT
struct AST_Expr
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr) == sizeof(AST), "ERROR: AST_Expr is not the same size as AST");

struct AST_Expr_Assign
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_Assign) == sizeof(AST), "ERROR: AST_Expr_Assign is not the same size as AST");

struct AST_Expr_Alignof
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_Alignof) == sizeof(AST), "ERROR: AST_Expr_Alignof is not the same size as AST");

struct AST_Expr_Binary
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_Binary) == sizeof(AST), "ERROR: AST_Expr_Binary is not the same size as AST");

struct AST_Expr_CStyleCast
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_CStyleCast) == sizeof(AST), "ERROR: AST_Expr_CStyleCast is not the same size as AST");

struct AST_Expr_FunctionalCast
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_FunctionalCast) == sizeof(AST), "ERROR: AST_Expr_FunctionalCast is not the same size as AST");

struct AST_Expr_CppCast
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_CppCast) == sizeof(AST), "ERROR: AST_Expr_CppCast is not the same size as AST");

struct AST_Expr_ProcCall
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_ProcCall) == sizeof(AST), "ERROR: AST_Expr_Identifier is not the same size as AST");

struct AST_Expr_Decltype
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_Decltype) == sizeof(AST), "ERROR: AST_Expr_Decltype is not the same size as AST");

struct AST_Expr_Comma
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_Comma) == sizeof(AST), "ERROR: AST_Expr_Comma is not the same size as AST");

struct AST_Expr_AMS
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_AMS) == sizeof(AST), "ERROR: AST_Expr_AMS is not the same size as AST");

struct AST_Expr_Sizeof
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_Sizeof) == sizeof(AST), "ERROR: AST_Expr_Sizeof is not the same size as AST");

struct AST_Expr_Subscript
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_Subscript) == sizeof(AST), "ERROR: AST_Expr_Subscript is not the same size as AST");

struct AST_Expr_Ternary
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_Ternary) == sizeof(AST), "ERROR: AST_Expr_Ternary is not the same size as AST");

struct AST_Expr_UnaryPrefix
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_UnaryPrefix) == sizeof(AST), "ERROR: AST_Expr_UnaryPrefix is not the same size as AST");

struct AST_Expr_UnaryPostfix
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_UnaryPostfix) == sizeof(AST), "ERROR: AST_Expr_UnaryPostfix is not the same size as AST");

struct AST_Expr_Element
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached   Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Expr_Element) == sizeof(AST), "ERROR: AST_Expr_Element is not the same size as AST");
#endif

struct AST_Extern
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			char      _PAD_PROPERTIES_[ sizeof(AST*) * 5 ];
			CodeBody  Body;
			char      _PAD_PROPERTIES_2_[ sizeof(AST*) ];
		};
	};
	StrCached         Name;
	Code              Prev;
	Code              Next;
	Token*            Tok;
	Code              Parent;
	CodeType          Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Extern) == sizeof(AST), "ERROR: AST_Extern is not the same size as AST");

struct AST_Include
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		StrCached  Content;
	};
	StrCached Name;
	Code      Prev;
	Code      Next;
	Token*    Tok;
	Code      Parent;
	CodeType  Type;
	char      _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Include) == sizeof(AST), "ERROR: AST_Include is not the same size as AST");

struct AST_Friend
{
	union {
		char            _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			CodeComment InlineCmt;
			char        _PAD_PROPERTIES_[ sizeof(AST*) * 4 ];
			Code        Declaration;
			char        _PAD_PROPERTIES_2_[ sizeof(AST*) ];
		};
	};
	StrCached Name;
	Code      Prev;
	Code      Next;
	Token*    Tok;
	Code      Parent;
	CodeType  Type;
	char      _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Friend) == sizeof(AST), "ERROR: AST_Friend is not the same size as AST");

struct AST_Fn
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			CodeComment     InlineCmt;
			CodeAttributes  Attributes;
			CodeSpecifiers  Specs;
			CodeTypename    ReturnType;
			CodeParams      Params;
			CodeBody        Body;
			Code            SuffixSpecs;  // Thanks Unreal
		};
	};
	StrCached  Name;
	Code       Prev;
	Code       Next;
	Token*     Tok;
	Code       Parent;
	CodeType   Type;
	ModuleFlag ModuleFlags;
	char       _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Fn) == sizeof(AST), "ERROR: AST_Fn is not the same size as AST");

struct AST_Module
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached         Name;
	Code              Prev;
	Code              Next;
	Token*            Tok;
	Code              Parent;
	CodeType          Type;
	ModuleFlag        ModuleFlags;
	char             _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Module) == sizeof(AST), "ERROR: AST_Module is not the same size as AST");

struct AST_NS
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct {
			char      _PAD_PROPERTIES_[ sizeof(AST*) * 5 ];
			CodeBody  Body;
			char      _PAD_PROPERTIES_2_[ sizeof(AST*) ];
		};
	};
	StrCached  Name;
	Code       Prev;
	Code       Next;
	Token*     Tok;
	Code       Parent;
	CodeType   Type;
	ModuleFlag ModuleFlags;
	char       _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_NS) == sizeof(AST), "ERROR: AST_NS is not the same size as AST");

struct AST_Operator
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			CodeComment    InlineCmt;
			CodeAttributes Attributes;
			CodeSpecifiers Specs;
			CodeTypename   ReturnType;
			CodeParams 	   Params;
			CodeBody       Body;
			char           _PAD_PROPERTIES_ [ sizeof(AST*) ];
		};
	};
	StrCached  Name;
	Code       Prev;
	Code       Next;
	Token*     Tok;
	Code       Parent;
	CodeType   Type;
	ModuleFlag ModuleFlags;
	Operator   Op;
};
static_assert( sizeof(AST_Operator) == sizeof(AST), "ERROR: AST_Operator is not the same size as AST");

struct AST_OpCast
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			CodeComment    InlineCmt;
			char           _PAD_PROPERTIES_[ sizeof(AST*)  ];
			CodeSpecifiers Specs;
			CodeTypename   ValueType;
			char           _PAD_PROPERTIES_2_[ sizeof(AST*) ];
			CodeBody       Body;
			char           _PAD_PROPERTIES_3_[ sizeof(AST*) ];
		};
	};
	StrCached Name;
	Code      Prev;
	Code      Next;
	Token*    Tok;
	Code      Parent;
	CodeType  Type;
	char      _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_OpCast) == sizeof(AST), "ERROR: AST_OpCast is not the same size as AST");

struct AST_Params
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			// TODO(Ed): Support attributes for parameters (Some prefix macros can be converted to that...)
			char         _PAD_PROPERTIES_2_[ sizeof(AST*) * 3 ];
			CodeTypename ValueType;
			Code         Macro;
			Code         Value;
			Code         PostNameMacro; // Thanks Unreal
			// char     _PAD_PROPERTIES_3_[sizeof( AST* )];
		};
	};
	StrCached  Name;
	CodeParams Last;
	CodeParams Next;
	Token*     Tok;
	Code       Parent;
	CodeType   Type;
	char       _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	s32        NumEntries;
};
static_assert( sizeof(AST_Params) == sizeof(AST), "ERROR: AST_Params is not the same size as AST");

struct AST_Pragma
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		StrCached  Content;
	};
	StrCached Name;
	Code      Prev;
	Code      Next;
	Token*    Tok;
	Code      Parent;
	CodeType  Type;
	char      _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Pragma) == sizeof(AST), "ERROR: AST_Pragma is not the same size as AST");

struct AST_PreprocessCond
{
	union {
		char          _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		StrCached     Content;
	};
	StrCached Name;
	Code      Prev;
	Code      Next;
	Token*    Tok;
	Code      Parent;
	CodeType  Type;
	char      _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_PreprocessCond) == sizeof(AST), "ERROR: AST_PreprocessCond is not the same size as AST");

struct AST_Specifiers
{
	Specifier      ArrSpecs[ AST_ArrSpecs_Cap ];
	StrCached      Name;
	CodeSpecifiers NextSpecs;
	Code           Prev;
	Code           Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	s32            NumEntries;
};
static_assert( sizeof(AST_Specifiers) == sizeof(AST), "ERROR: AST_Specifier is not the same size as AST");

#ifdef GEN_EXECUTION_EXPRESSION_SUPPORT
struct AST_Stmt
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt) == sizeof(AST), "ERROR: AST_Stmt is not the same size as AST");

struct AST_Stmt_Break
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_Break) == sizeof(AST), "ERROR: AST_Stmt_Break is not the same size as AST");

struct AST_Stmt_Case
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_Case) == sizeof(AST), "ERROR: AST_Stmt_Case is not the same size as AST");

struct AST_Stmt_Continue
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_Continue) == sizeof(AST), "ERROR: AST_Stmt_Continue is not the same size as AST");

struct AST_Stmt_Decl
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_Decl) == sizeof(AST), "ERROR: AST_Stmt_Decl is not the same size as AST");

struct AST_Stmt_Do
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_Do) == sizeof(AST), "ERROR: AST_Stmt_Do is not the same size as AST");

struct AST_Stmt_Expr
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_Expr) == sizeof(AST), "ERROR: AST_Stmt_Expr is not the same size as AST");

struct AST_Stmt_Else
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_Else) == sizeof(AST), "ERROR: AST_Stmt_Else is not the same size as AST");

struct AST_Stmt_If
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_If) == sizeof(AST), "ERROR: AST_Stmt_If is not the same size as AST");

struct AST_Stmt_For
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_For) == sizeof(AST), "ERROR: AST_Stmt_For is not the same size as AST");

struct AST_Stmt_Goto
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_Goto) == sizeof(AST), "ERROR: AST_Stmt_Goto is not the same size as AST");

struct AST_Stmt_Label
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_Label) == sizeof(AST), "ERROR: AST_Stmt_Label is not the same size as AST");

struct AST_Stmt_Switch
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_Switch) == sizeof(AST), "ERROR: AST_Stmt_Switch is not the same size as AST");

struct AST_Stmt_While
{
	union {
		char _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
	};
	StrCached      Name;
	CodeExpr       Prev;
	CodeExpr       Next;
	Token*         Tok;
	Code           Parent;
	CodeType       Type;
	char           _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
};
static_assert( sizeof(AST_Stmt_While) == sizeof(AST), "ERROR: AST_Stmt_While is not the same size as AST");
#endif

struct AST_Struct
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			CodeComment    InlineCmt;
			CodeAttributes Attributes;
			CodeSpecifiers Specs; // Support for final
			CodeTypename   ParentType;
			char           _PAD_PARAMS_[ sizeof(AST*) ];
			CodeBody       Body;
			char          _PAD_PROPERTIES_2_[ sizeof(AST*) ];
		};
	};
	StrCached              Name;
	CodeTypename           Prev;
	CodeTypename           Next;
	Token*                 Tok;
	Code                   Parent;
	CodeType               Type;
	ModuleFlag             ModuleFlags;
	AccessSpec             ParentAccess;
};
static_assert( sizeof(AST_Struct) == sizeof(AST), "ERROR: AST_Struct is not the same size as AST");

struct AST_Template
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			char           _PAD_PROPERTIES_[ sizeof(AST*) * 4 ];
			CodeParams 	   Params;
			Code           Declaration;
			char           _PAD_PROPERTIES_2_[ sizeof(AST*) ];
		};
	};
	StrCached  Name;
	Code       Prev;
	Code       Next;
	Token*     Tok;
	Code       Parent;
	CodeType   Type;
	ModuleFlag ModuleFlags;
	char       _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Template) == sizeof(AST), "ERROR: AST_Template is not the same size as AST");

#if 0
// WIP... The type ast is going to become more advanced and lead to a major change to AST design.
struct AST_Type
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			char           _PAD_INLINE_CMT_[ sizeof(AST*) ];
			CodeAttributes  Attributes;
			CodeSpecifiers  Specs;
			Code            QualifierID;
			// CodeTypename ReturnType;      // Only used for function signatures
			// CodeParams    Params;          // Only used for function signatures
			Code            ArrExpr;
			// CodeSpecifiers SpecsFuncSuffix; // Only used for function signatures
		};
	};
	StrCached              Name;
	Code                   Prev;
	Code                   Next;
	Token*                 Tok;
	Code                   Parent;
	CodeType               Type;
	char                   _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	b32                    IsParamPack;
};
static_assert( sizeof(AST_Type) == sizeof(AST), "ERROR: AST_Type is not the same size as AST");
#endif

struct AST_Typename
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			char           _PAD_INLINE_CMT_[ sizeof(AST*) ];
			CodeAttributes Attributes;
			CodeSpecifiers Specs;
			CodeTypename   ReturnType;      // Only used for function signatures
			CodeParams     Params;          // Only used for function signatures
			Code           ArrExpr;
			CodeSpecifiers SpecsFuncSuffix; // Only used for function signatures
		};
	};
	StrCached           Name;
	Code                   Prev;
	Code                   Next;
	Token*                 Tok;
	Code                   Parent;
	CodeType               Type;
	char                   _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	struct {
		b16                IsParamPack;   // Used by typename to know if type should be considered a parameter pack.
		ETypenameTag       TypeTag;       // Used by typename to keep track of explicitly declared tags for the identifier (enum, struct, union)
	};
};
static_assert( sizeof(AST_Typename) == sizeof(AST), "ERROR: AST_Type is not the same size as AST");

struct AST_Typedef
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			CodeComment    InlineCmt;
			char           _PAD_PROPERTIES_[ sizeof(AST*) * 2 ];
			Code           UnderlyingType;
			char           _PAD_PROPERTIES_2_[ sizeof(AST*) * 3 ];
		};
	};
	StrCached           Name;
	Code                   Prev;
	Code                   Next;
	Token*                 Tok;
	Code                   Parent;
	CodeType               Type;
	ModuleFlag             ModuleFlags;
	b32                    IsFunction;
};
static_assert( sizeof(AST_Typedef) == sizeof(AST), "ERROR: AST_Typedef is not the same size as AST");

struct AST_Union
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			char           _PAD_INLINE_CMT_[ sizeof(AST*) ];
			CodeAttributes Attributes;
			char           _PAD_PROPERTIES_[ sizeof(AST*) * 3 ];
			CodeBody       Body;
			char           _PAD_PROPERTIES_2_[ sizeof(AST*) ];
		};
	};
	StrCached  Name;
	Code       Prev;
	Code       Next;
	Token*     Tok;
	Code       Parent;
	CodeType   Type;
	ModuleFlag ModuleFlags;
	char       _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Union) == sizeof(AST), "ERROR: AST_Union is not the same size as AST");

struct AST_Using
{
	union {
		char                _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			CodeComment     InlineCmt;
			CodeAttributes  Attributes;
			char            _PAD_SPECS_     [ sizeof(AST*) ];
			CodeTypename    UnderlyingType;
			char            _PAD_PROPERTIES_[ sizeof(AST*) * 3 ];
		};
	};
	StrCached  Name;
	Code       Prev;
	Code       Next;
	Token*     Tok;
	Code       Parent;
	CodeType   Type;
	ModuleFlag ModuleFlags;
	char       _PAD_UNUSED_[ sizeof(u32) ];
};
static_assert( sizeof(AST_Using) == sizeof(AST), "ERROR: AST_Using is not the same size as AST");

struct AST_Var
{
	union {
		char               _PAD_[ sizeof(Specifier) * AST_ArrSpecs_Cap + sizeof(AST*) ];
		struct
		{
			CodeComment    InlineCmt;
			CodeAttributes Attributes;
			CodeSpecifiers Specs;
			CodeTypename   ValueType;
			Code           BitfieldSize;
			Code           Value;
			CodeVar        NextVar;
		};
	};
	StrCached  Name;
	Code       Prev;
	Code       Next;
	Token*     Tok;
	Code       Parent;
	CodeType   Type;
	ModuleFlag ModuleFlags;
	s32        VarParenthesizedInit;
};
static_assert( sizeof(AST_Var) == sizeof(AST), "ERROR: AST_Var is not the same size as AST");

#pragma endregion AST Types
