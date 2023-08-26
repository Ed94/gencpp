#pragma once
#include "ast.hpp"

#pragma region AST Types
/*
	Show only relevant members of the AST for its type.
	AST* fields are replaced with Code types.
		- Guards assignemnts to AST* fields to ensure the AST is duplicated if assigned to another parent.
*/

struct AST_Body
{
	char              _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
	Code              Front;
	Code              Back;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	s32 			  NumEntries;
	s32               Token;
};
static_assert( sizeof(AST_Body) == sizeof(AST), "ERROR: AST_Filtered is not the same size as AST");

struct AST_Attributes
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		StringCached  Content;
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_Attributes) == sizeof(AST), "ERROR: AST_Attributes is not the same size as AST");

struct AST_Comment
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		StringCached  Content;
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_Comment) == sizeof(AST), "ERROR: AST_Comment is not the same size as AST");

struct AST_Class
{
	union {
		char                _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeComment     InlineCmt; // Only supported by forward declarations
			CodeAttributes  Attributes;
			char 	        _PAD_SPECS_ [ sizeof(AST*) ];
			CodeType        ParentType;
			char 	        _PAD_PARAMS_[ sizeof(AST*) ];
			CodeBody        Body;
		};
	};
	CodeType                Last;
	CodeType                Next;
	Code                    Parent;
	StringCached            Name;
	CodeT                   Type;
	ModuleFlag              ModuleFlags;
	AccessSpec              ParentAccess;
	s32                     Token;
};
static_assert( sizeof(AST_Class) == sizeof(AST), "ERROR: AST_Class is not the same size as AST");

struct AST_Constructor
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeComment    InlineCmt; // Only supported by forward declarations
			char           _PAD_PROPERTIES_ [ sizeof(AST*) * 1 ];
			CodeSpecifiers Specs;
			Code           InitializerList;
			CodeParam      Params;
			Code           Body;
		};
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	char              _PAD_NAME_[ sizeof(StringCached) ];
	CodeT             Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_Constructor) == sizeof(AST), "ERROR: AST_Constructor is not the same size as AST");

struct AST_Define
{
	union {
		char 		  _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		StringCached  Content;
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_Define) == sizeof(AST), "ERROR: AST_Define is not the same size as AST");

struct AST_Destructor
{
	union {
		char               _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeComment    InlineCmt;
			char           _PAD_PROPERTIES_ [ sizeof(AST*) * 1 ];
			CodeSpecifiers Specs;
			char           _PAD_PROPERTIES_2_ [ sizeof(AST*) * 2 ];
			Code           Body;
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	char                   _PAD_NAME_[ sizeof(StringCached) ];
	CodeT                  Type;
	char                   _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
	s32                    Token;
};
static_assert( sizeof(AST_Destructor) == sizeof(AST), "ERROR: AST_Destructor is not the same size as AST");

struct AST_Enum
{
	union {
		char                _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeComment    InlineCmt;
			CodeAttributes Attributes;
			char           _PAD_SPEC_  [ sizeof(AST*) ];
			CodeType       UnderlyingType;
			char	       _PAD_PARAMS_[ sizeof(AST*) ];
			CodeBody       Body;
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	ModuleFlag             ModuleFlags;
	char 			       _PAD_UNUSED_[ sizeof(u32) ];
	s32                    Token;
};
static_assert( sizeof(AST_Enum) == sizeof(AST), "ERROR: AST_Enum is not the same size as AST");

struct AST_Exec
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		StringCached  Content;
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char              _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_Exec) == sizeof(AST), "ERROR: AST_Exec is not the same size as AST");

struct AST_Extern
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			char      _PAD_PROPERTIES_[ sizeof(AST*) * 5 ];
			CodeBody  Body;
		};
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_Extern) == sizeof(AST), "ERROR: AST_Extern is not the same size as AST");

struct AST_Include
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		StringCached  Content;
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_Include) == sizeof(AST), "ERROR: AST_Include is not the same size as AST");

struct AST_Friend
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeComment InlineCmt;
			char        _PAD_PROPERTIES_[ sizeof(AST*) * 4 ];
			Code        Declaration;
		};
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_Friend) == sizeof(AST), "ERROR: AST_Friend is not the same size as AST");

struct AST_Fn
{
	union {
		char                _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeComment     InlineCmt;
			CodeAttributes  Attributes;
			CodeSpecifiers  Specs;
			CodeType        ReturnType;
			CodeParam 	    Params;
			CodeBody        Body;
		};
	};
	Code                    Prev;
	Code                    Parent;
	Code                    Next;
	StringCached            Name;
	CodeT                   Type;
	ModuleFlag              ModuleFlags;
	char 			        _PAD_UNUSED_[ sizeof(u32) ];
	s32                     Token;
};
static_assert( sizeof(AST_Fn) == sizeof(AST), "ERROR: AST_Fn is not the same size as AST");

struct AST_Module
{
	char              _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	ModuleFlag        ModuleFlags;
	char 			  _PAD_UNUSED_[ sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_Module) == sizeof(AST), "ERROR: AST_Module is not the same size as AST");

struct AST_NS
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct {
			char 	  _PAD_PROPERTIES_[ sizeof(AST*) * 5 ];
			CodeBody  Body;
		};
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	ModuleFlag        ModuleFlags;
	char 			  _PAD_UNUSED_[ sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_NS) == sizeof(AST), "ERROR: AST_NS is not the same size as AST");

struct AST_Operator
{
	union {
		char                _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeComment     InlineCmt;
			CodeAttributes  Attributes;
			CodeSpecifiers  Specs;
			CodeType        ReturnType;
			CodeParam 	    Params;
			CodeBody        Body;
		};
	};
	Code                    Prev;
	Code                    Next;
	Code                    Parent;
	StringCached            Name;
	CodeT                   Type;
	ModuleFlag              ModuleFlags;
	OperatorT               Op;
	s32                     Token;
};
static_assert( sizeof(AST_Operator) == sizeof(AST), "ERROR: AST_Operator is not the same size as AST");

struct AST_OpCast
{
	union {
		char          _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeComment    InlineCmt;
			char 	       _PAD_PROPERTIES_[ sizeof(AST*)  ];
			CodeSpecifiers Specs;
			CodeType       ValueType;
			char 	       _PAD_PROPERTIES_2_[ sizeof(AST*) ];
			CodeBody       Body;
		};
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_OpCast) == sizeof(AST), "ERROR: AST_OpCast is not the same size as AST");

struct AST_Param
{
	union {
		char 		  _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			char 	  _PAD_PROPERTIES_2_[ sizeof(AST*) * 3 ];
			CodeType  ValueType;
			char 	  _PAD_PROPERTIES_[ sizeof(AST*) ];
			Code      Value;
		};
	};
	CodeParam         Last;
	CodeParam         Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	s32               NumEntries;
	s32               Token;
};
static_assert( sizeof(AST_Param) == sizeof(AST), "ERROR: AST_Param is not the same size as AST");

struct AST_Pragma
{
	union {
		char 		  _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		StringCached  Content;
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_Pragma) == sizeof(AST), "ERROR: AST_Pragma is not the same size as AST");

struct AST_PreprocessCond
{
	union {
		char 		  _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		StringCached  Content;
	};
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) + sizeof(u32) ];
	s32               Token;
};
static_assert( sizeof(AST_PreprocessCond) == sizeof(AST), "ERROR: AST_PreprocessCond is not the same size as AST");

struct AST_Specifiers
{
	SpecifierT        ArrSpecs[ AST::ArrSpecs_Cap ];
	Code              Prev;
	Code              Next;
	Code              Parent;
	StringCached      Name;
	CodeT             Type;
	char 			  _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	s32               NumEntries;
	s32               Token;
};
	static_assert( sizeof(AST_Specifiers) == sizeof(AST), "ERROR: AST_Specifier is not the same size as AST");

struct AST_Struct
{
	union {
		char               _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeComment    InlineCmt;
			CodeAttributes Attributes;
			char 	       _PAD_SPECS_ [ sizeof(AST*) ];
			CodeType       ParentType;
			char 	       _PAD_PARAMS_[ sizeof(AST*) ];
			CodeBody       Body;
		};
	};
	CodeType               Last;
	CodeType               Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	ModuleFlag             ModuleFlags;
	AccessSpec             ParentAccess;
	s32                    Token;
};
static_assert( sizeof(AST_Struct) == sizeof(AST), "ERROR: AST_Struct is not the same size as AST");

struct AST_Template
{
	union {
		char 		       _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			char 	       _PAD_PROPERTIES_[ sizeof(AST*) * 4 ];
			CodeParam 	   Params;
			Code           Declaration;
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	ModuleFlag             ModuleFlags;
	char 			       _PAD_UNUSED_[ sizeof(u32) ];
	s32                    Token;
};
static_assert( sizeof(AST_Template) == sizeof(AST), "ERROR: AST_Template is not the same size as AST");

struct AST_Type
{
	union {
		char 		       _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			char           _PAD_CMT_[ sizeof(AST*) ];
			CodeAttributes Attributes;
			CodeSpecifiers Specs;
			char 	       _PAD_PROPERTIES_[ sizeof(AST*) * 2 ];
			Code           ArrExpr;
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	char 			       _PAD_UNUSED_[ sizeof(ModuleFlag) ];
	b32                    IsParamPack;
	s32                    Token;
};
static_assert( sizeof(AST_Type) == sizeof(AST), "ERROR: AST_Type is not the same size as AST");

struct AST_Typedef
{
	union {
		char 		       _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeComment    InlineCmt;
			char 	       _PAD_PROPERTIES_[ sizeof(AST*) * 2 ];
			Code           UnderlyingType;
			char 	       _PAD_PROPERTIES_2_[ sizeof(AST*) * 2 ];
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	ModuleFlag             ModuleFlags;
	b32                    IsFunction;
	s32                    Token;
};
static_assert( sizeof(AST_Typedef) == sizeof(AST), "ERROR: AST_Typedef is not the same size as AST");

struct AST_Union
{
	union {
		char 		       _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			char           _PAD_INLINE_CMT_[ sizeof(AST*) ];
			CodeAttributes Attributes;
			char 	       _PAD_PROPERTIES_[ sizeof(AST*) * 3 ];
			CodeBody       Body;
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	ModuleFlag             ModuleFlags;
	char 			       _PAD_UNUSED_[ sizeof(u32) ];
	s32                    Token;
};
static_assert( sizeof(AST_Union) == sizeof(AST), "ERROR: AST_Union is not the same size as AST");

struct AST_Using
{
	union {
		char 		        _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeComment     InlineCmt;
			CodeAttributes  Attributes;
			char 	        _PAD_SPECS_     [ sizeof(AST*) ];
			CodeType        UnderlyingType;
			char 	        _PAD_PROPERTIES_[ sizeof(AST*) * 2 ];
		};
	};
	Code                    Prev;
	Code                    Next;
	Code                    Parent;
	StringCached            Name;
	CodeT                   Type;
	ModuleFlag              ModuleFlags;
	char 			        _PAD_UNUSED_[ sizeof(u32) ];
	s32                     Token;
};
static_assert( sizeof(AST_Using) == sizeof(AST), "ERROR: AST_Using is not the same size as AST");

struct AST_Var
{
	union {
		char               _PAD_[ sizeof(SpecifierT) * AST::ArrSpecs_Cap ];
		struct
		{
			CodeComment    InlineCmt;
			CodeAttributes Attributes;
			CodeSpecifiers Specs;
			CodeType       ValueType;
			Code           BitfieldSize;
			Code           Value;
		};
	};
	Code                   Prev;
	Code                   Next;
	Code                   Parent;
	StringCached           Name;
	CodeT                  Type;
	ModuleFlag             ModuleFlags;
	char 			       _PAD_UNUSED_[ sizeof(u32) ];
	s32                    Token;
};
static_assert( sizeof(AST_Var) == sizeof(AST), "ERROR: AST_Var is not the same size as AST");

#pragma endregion AST Types
