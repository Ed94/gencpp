#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "types.hpp"
#include "gen/ecode.hpp"
#include "gen/eoperator.hpp"
#include "gen/especifier.hpp"
#endif

struct AST;
struct AST_Body;
struct AST_Attributes;
struct AST_Comment;
struct AST_Constructor;
// struct AST_BaseClass;
struct AST_Class;
struct AST_Define;
struct AST_Destructor;
struct AST_Enum;
struct AST_Exec;
struct AST_Extern;
struct AST_Include;
struct AST_Friend;
struct AST_Fn;
struct AST_Module;
struct AST_NS;
struct AST_Operator;
struct AST_OpCast;
struct AST_Param;
struct AST_Pragma;
struct AST_PreprocessCond;
struct AST_Specifiers;

#if GEN_EXECUTION_EXPRESSION_SUPPORT
struct AST_Expr;
struct AST_Expr_Assign;
struct AST_Expr_Alignof;
struct AST_Expr_Binary;
struct AST_Expr_CStyleCast;
struct AST_Expr_FunctionalCast;
struct AST_Expr_CppCast;
struct AST_Expr_ProcCall;
struct AST_Expr_Decltype;
struct AST_Expr_Comma;  // TODO(Ed) : This is a binary op not sure if it needs its own AST...
struct AST_Expr_AMS;    // Access Member Symbol
struct AST_Expr_Sizeof;
struct AST_Expr_Subscript;
struct AST_Expr_Ternary;
struct AST_Expr_UnaryPrefix;
struct AST_Expr_UnaryPostfix;
struct AST_Expr_Element;

struct AST_Stmt;
struct AST_Stmt_Break;
struct AST_Stmt_Case;
struct AST_Stmt_Continue;
struct AST_Stmt_Decl;
struct AST_Stmt_Do;
struct AST_Stmt_Expr;  // TODO(Ed) : Is this distinction needed? (Should it be a flag instead?)
struct AST_Stmt_Else;
struct AST_Stmt_If;
struct AST_Stmt_For;
struct AST_Stmt_Goto;
struct AST_Stmt_Label;
struct AST_Stmt_Switch;
struct AST_Stmt_While;
#endif

struct AST_Struct;
struct AST_Template;
struct AST_Type;
struct AST_Typedef;
struct AST_Union;
struct AST_Using;
struct AST_Var;

struct Code;
struct CodeBody;
// These are to offer ease of use and optionally strong type safety for the AST.
struct CodeAttributes;
// struct CodeBaseClass;
struct CodeComment;
struct CodeClass;
struct CodeConstructor;
struct CodeDefine;
struct CodeDestructor;
struct CodeEnum;
struct CodeExec;
struct CodeExtern;
struct CodeInclude;
struct CodeFriend;
struct CodeFn;
struct CodeModule;
struct CodeNS;
struct CodeOperator;
struct CodeOpCast;
struct CodeParam;
struct CodePreprocessCond;
struct CodePragma;
struct CodeSpecifiers;

#if GEN_EXECUTION_EXPRESSION_SUPPORT
struct CodeExpr;
struct CodeExpr_Assign;
struct CodeExpr_Alignof;
struct CodeExpr_Binary;
struct CodeExpr_CStyleCast;
struct CodeExpr_FunctionalCast;
struct CodeExpr_CppCast;
struct CodeExpr_Element;
struct CodeExpr_ProcCall;
struct CodeExpr_Decltype;
struct CodeExpr_Comma;
struct CodeExpr_AMS; // Access Member Symbol
struct CodeExpr_Sizeof;
struct CodeExpr_Subscript;
struct CodeExpr_Ternary;
struct CodeExpr_UnaryPrefix;
struct CodeExpr_UnaryPostfix;

struct CodeStmt;
struct CodeStmt_Break;
struct CodeStmt_Case;
struct CodeStmt_Continue;
struct CodeStmt_Decl;
struct CodeStmt_Do;
struct CodeStmt_Expr;
struct CodeStmt_Else;
struct CodeStmt_If;
struct CodeStmt_For;
struct CodeStmt_Goto;
struct CodeStmt_Label;
struct CodeStmt_Switch;
struct CodeStmt_While;
#endif

struct CodeStruct;
struct CodeTemplate;
struct CodeType;
struct CodeTypedef;
struct CodeUnion;
struct CodeUsing;
struct CodeVar;

namespace parser
{
	struct Token;
}

template< class Type> forceinline Type tmpl_cast( Code* self ) { return * rcast( Type*, self ); }
#if ! GEN_COMPILER_C && 0
template< class Type> forceinline Type tmpl_cast( Code& self ) { return * rcast( Type*, & self ); }
#endif

/*
	AST* wrapper
	- Not constantly have to append the '*' as this is written often..
	- Allows for implicit conversion to any of the ASTs (raw or filtered).
*/
struct Code
{
#	pragma region Statics
	// Used to identify ASTs that should always be duplicated. (Global constant ASTs)
	static Code Global;

	// Used to identify invalid generated code.
	static Code Invalid;
#	pragma endregion Statics

#	define Using_Code( Typename )          \
	char const* debug_str();               \
	Code        duplicate();			   \
	bool        is_equal( Code other );    \
	bool        is_body();                 \
	bool        is_valid();                \
	void        set_global();              \
	String      to_string();               \
	Typename&   operator = ( AST* other ); \
	Typename&   operator = ( Code other ); \
	bool        operator ==( Code other ); \
	bool        operator !=( Code other ); \
	operator bool();

	Using_Code( Code );

	template< class Type >
	forceinline Type code_cast()
	{
		return * rcast( Type*, this );
	}

	AST* operator ->()
	{
		return ast;
	}
	Code& operator ++();

	// TODO(Ed) : Remove this overload.
	auto& operator*()
	{
		local_persist thread_local
		Code NullRef = { nullptr };

		if ( ast == nullptr )
			return NullRef;

		return *this;
	}

	AST* ast;

#ifdef GEN_ENFORCE_STRONG_CODE_TYPES
#	define operator explicit operator
#endif
	operator CodeBody() const;
	operator CodeAttributes() const;
	// operator CodeBaseClass() const;
	operator CodeComment() const;
	operator CodeClass() const;
	operator CodeConstructor() const;
	operator CodeDefine() const;
	operator CodeDestructor() const;
	operator CodeExec() const;
	operator CodeEnum() const;
	operator CodeExtern() const;
	operator CodeInclude() const;
	operator CodeFriend() const;
	operator CodeFn() const;
	operator CodeModule() const;
	operator CodeNS() const;
	operator CodeOperator() const;
	operator CodeOpCast() const;
	operator CodeParam() const;
	operator CodePragma() const;
	operator CodePreprocessCond() const;
	operator CodeSpecifiers() const;
	operator CodeStruct() const;
	operator CodeTemplate() const;
	operator CodeType() const;
	operator CodeTypedef() const;
	operator CodeUnion() const;
	operator CodeUsing() const;
	operator CodeVar() const;
	#undef operator
};

struct Code_POD
{
	AST* ast;
};

static_assert( sizeof(Code) == sizeof(Code_POD), "ERROR: Code is not POD" );

// Desired width of the AST data structure.
constexpr int const AST_POD_Size = 128;

void        append     ( AST* self, AST* other );
char const* debug_str  ( AST* self );
AST*        duplicate  ( AST* self );
Code*       entry      ( AST* self, u32 idx );
bool        has_entries( AST* self );
bool        is_body    ( AST* self );
String      to_string  ( AST* self );
char const* type_str   ( AST* self );

#if GEN_CPP_SUPPORT_REFERENCES
void        append   ( AST& self, AST& other ) { return append(& self, & other); }
bool        is_body  ( AST& self )             { return is_body(& self); }
char const* debug_str( AST& self )             { return debug_str( & self ); }
String      to_string( AST& self )             { return to_string( & self ); }
char const* type_str ( AST& self )             { return type_str( & self ); }
#endif

/*
	Simple AST POD with functionality to seralize into C++ syntax.
*/
struct AST
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
#	pragma region Member Functions
	void        append     ( AST* other ) { GEN_NS append(this, other); }
	char const* debug_str  ()             { return GEN_NS debug_str(this); }
	AST*        duplicate  ()             { return GEN_NS duplicate(this); }
	Code*       entry      ( u32 idx )    { return GEN_NS entry(this, idx); }
	bool        has_entries();
	bool        is_equal   ( AST* other );
	bool        is_body()                  { return GEN_NS is_body(this); }
	char const* type_str()                 { return GEN_NS type_str(this); }
	bool        validate_body();

	String to_string(); //{ return GEN_NS to_string(this); }

	template< class Type >
	forceinline Type code_cast()
	{
		return * this;
	}

	neverinline
	void to_string( String& result );
#	pragma endregion Member Functions
#endif

	operator Code();
	operator CodeBody();
	operator CodeAttributes();
	// operator CodeBaseClass();
	operator CodeComment();
	operator CodeConstructor();
	operator CodeDestructor();
	operator CodeClass();
	operator CodeDefine();
	operator CodeEnum();
	operator CodeExec();
	operator CodeExtern();
	operator CodeInclude();
	operator CodeFriend();
	operator CodeFn();
	operator CodeModule();
	operator CodeNS();
	operator CodeOperator();
	operator CodeOpCast();
	operator CodeParam();
	operator CodePragma();
	operator CodePreprocessCond();
	operator CodeSpecifiers();
	operator CodeStruct();
	operator CodeTemplate();
	operator CodeType();
	operator CodeTypedef();
	operator CodeUnion();
	operator CodeUsing();
	operator CodeVar();

	constexpr static
	int ArrSpecs_Cap =
	(
			AST_POD_Size
			- sizeof(AST*) * 3
			- sizeof(parser::Token*)
			- sizeof(AST*)
			- sizeof(StringCached)
			- sizeof(CodeT)
			- sizeof(ModuleFlag)
			- sizeof(int)
	)
	/ sizeof(int) - 1; // -1 for 4 extra bytes

	union {
		struct
		{
			AST*      InlineCmt;       // Class, Constructor, Destructor, Enum, Friend, Functon, Operator, OpCast, Struct, Typedef, Using, Variable
			AST*      Attributes;      // Class, Enum, Function, Struct, Typedef, Union, Using, Variable
			AST*      Specs;           // Destructor, Function, Operator, Typename, Variable
			union {
				AST*  InitializerList; // Constructor
				AST*  ParentType;      // Class, Struct, ParentType->Next has a possible list of interfaces.
				AST*  ReturnType;      // Function, Operator, Typename
				AST*  UnderlyingType;  // Enum, Typedef
				AST*  ValueType;       // Parameter, Variable
			};
			union {
				AST*  Macro;           // Parameter
				AST*  BitfieldSize;    // Variable (Class/Struct Data Member)
				AST*  Params;          // Constructor, Function, Operator, Template, Typename
			};
			union {
				AST*  ArrExpr;          // Typename
				AST*  Body;             // Class, Constructor, Destructor, Enum, Friend, Function, Namespace, Struct, Union
				AST*  Declaration;      // Friend, Template
				AST*  Value;            // Parameter, Variable
			};
			union {
				AST*  NextVar;          // Variable; Possible way to handle comma separated variables declarations. ( , NextVar->Specs NextVar->Name NextVar->ArrExpr = NextVar->Value )
				AST*  SuffixSpecs;      // Only used with typenames, to store the function suffix if typename is function signature. ( May not be needed )
				AST*  PostNameMacro;     // Only used with parameters for specifically UE_REQUIRES (Thanks Unreal)
			};
		};
		StringCached  Content;          // Attributes, Comment, Execution, Include
		struct {
			SpecifierT ArrSpecs[ArrSpecs_Cap]; // Specifiers
			AST*       NextSpecs;              // Specifiers; If ArrSpecs is full, then NextSpecs is used.
		};
	};
	union {
		AST* Prev;
		AST* Front;
		AST* Last;
	};
	union {
		AST* Next;
		AST* Back;
	};
	parser::Token*    Token; // Reference to starting token, only avaialble if it was derived from parsing.
	AST*              Parent;
	StringCached      Name;
	CodeT             Type;
//	CodeFlag          CodeFlags;
	ModuleFlag        ModuleFlags;
	union {
		b32           IsFunction;  // Used by typedef to not serialize the name field.
		b32           IsParamPack; // Used by typename to know if type should be considered a parameter pack.
		OperatorT     Op;
		AccessSpec    ParentAccess;
		s32           NumEntries;
		s32           VarConstructorInit;  // Used by variables to know that initialization is using a constructor expression instead of an assignment expression.
		b32           EnumUnderlyingMacro; // Used by enums incase the user wants to wrap underlying type specification in a macro
	};
};

struct AST_POD
{
	union {
		struct
		{
			AST*      InlineCmt;       // Class, Constructor, Destructor, Enum, Friend, Functon, Operator, OpCast, Struct, Typedef, Using, Variable
			AST*      Attributes;      // Class, Enum, Function, Struct, Typedef, Union, Using, Variable
			AST*      Specs;           // Destructor, Function, Operator, Typename, Variable
			union {
				AST*  InitializerList; // Constructor
				AST*  ParentType;      // Class, Struct, ParentType->Next has a possible list of interfaces.
				AST*  ReturnType;      // Function, Operator, Typename
				AST*  UnderlyingType;  // Enum, Typedef
				AST*  ValueType;       // Parameter, Variable
			};
			union {
				AST*  Macro;           // Parameter
				AST*  BitfieldSize;    // Variable (Class/Struct Data Member)
				AST*  Params;          // Constructor, Function, Operator, Template, Typename
			};
			union {
				AST*  ArrExpr;          // Typename
				AST*  Body;             // Class, Constructr, Destructor, Enum, Friend, Function, Namespace, Struct, Union
				AST*  Declaration;      // Friend, Template
				AST*  Value;            // Parameter, Variable
			};
			union {
				AST*  NextVar;          // Variable; Possible way to handle comma separated variables declarations. ( , NextVar->Specs NextVar->Name NextVar->ArrExpr = NextVar->Value )
				AST*  SuffixSpecs;      // Only used with typenames, to store the function suffix if typename is function signature. ( May not be needed )
				AST*  PostNameMacro;    // Only used with parameters for specifically UE_REQUIRES (Thanks Unreal)
			};
		};
		StringCached  Content;          // Attributes, Comment, Execution, Include
		struct {
			SpecifierT ArrSpecs[AST::ArrSpecs_Cap]; // Specifiers
			AST*       NextSpecs;                   // Specifiers; If ArrSpecs is full, then NextSpecs is used.
		};
	};
	union {
		AST* Prev;
		AST* Front;
		AST* Last;
	};
	union {
		AST* Next;
		AST* Back;
	};
	parser::Token*    Token; // Reference to starting token, only avaialble if it was derived from parsing.
	AST*              Parent;
	StringCached      Name;
	CodeT             Type;
	CodeFlag          CodeFlags;
	ModuleFlag        ModuleFlags;
	union {
		b32           IsFunction;  // Used by typedef to not serialize the name field.
		b32           IsParamPack; // Used by typename to know if type should be considered a parameter pack.
		OperatorT     Op;
		AccessSpec    ParentAccess;
		s32           NumEntries;
		s32           VarConstructorInit; // Used by variables to know that initialization is using a constructor expression instead of an assignment expression.
	};
};


// TODO(Ed): Convert
String      to_string  ( AST* self ) { return self->to_string(); }

// Its intended for the AST to have equivalent size to its POD.
// All extra functionality within the AST namespace should just be syntatic sugar.
static_assert( sizeof(AST)     == sizeof(AST_POD), "ERROR: AST IS NOT POD" );
static_assert( sizeof(AST_POD) == AST_POD_Size,    "ERROR: AST POD is not size of AST_POD_Size" );

// Used when the its desired when omission is allowed in a definition.
#define NoCode      { nullptr }
#define CodeInvalid (* Code::Invalid.ast) // Uses an implicitly overloaded cast from the AST to the desired code type.
