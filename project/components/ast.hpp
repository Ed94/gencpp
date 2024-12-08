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
struct AST_Typename;
struct AST_Typedef;
struct AST_Union;
struct AST_Using;
struct AST_Var;

#if GEN_COMPILER_C
#define Define_Code(Type) typedef AST_##Type* Code##Type
#else
#define Define_Code(Type) struct Code##Type
#endif

#if GEN_COMPILER_C
typedef AST* Code;
#else
struct Code;
#endif
Define_Code(Body);
// These are to offer ease of use and optionally strong type safety for the AST.
Define_Code(Attributes);
// struct CodeBaseClass;
Define_Code(Comment);
Define_Code(Class);
Define_Code(Constructor);
Define_Code(Define);
Define_Code(Destructor);
Define_Code(Enum);
Define_Code(Exec);
Define_Code(Extern);
Define_Code(Include);
Define_Code(Friend);
Define_Code(Fn);
Define_Code(Module);
Define_Code(NS);
Define_Code(Operator);
Define_Code(OpCast);
Define_Code(Param);
Define_Code(PreprocessCond);
Define_Code(Pragma);
Define_Code(Specifiers);

#if GEN_EXECUTION_EXPRESSION_SUPPORT
Define_Code(Expr);
Define_Code(Expr_Assign);
Define_Code(Expr_Alignof);
Define_Code(Expr_Binary);
Define_Code(Expr_CStyleCast);
Define_Code(Expr_FunctionalCast);
Define_Code(Expr_CppCast);
Define_Code(Expr_Element);
Define_Code(Expr_ProcCall);
Define_Code(Expr_Decltype);
Define_Code(Expr_Comma);
Define_Code(Expr_AMS); // Access Member Symbol
Define_Code(Expr_Sizeof);
Define_Code(Expr_Subscript);
Define_Code(Expr_Ternary);
Define_Code(Expr_UnaryPrefix);
Define_Code(Expr_UnaryPostfix);

Define_Code(Stmt);
Define_Code(Stmt_Break);
Define_Code(Stmt_Case);
Define_Code(Stmt_Continue);
Define_Code(Stmt_Decl);
Define_Code(Stmt_Do);
Define_Code(Stmt_Expr);
Define_Code(Stmt_Else);
Define_Code(Stmt_If);
Define_Code(Stmt_For);
Define_Code(Stmt_Goto);
Define_Code(Stmt_Label);
Define_Code(Stmt_Switch);
Define_Code(Stmt_While);
#endif

Define_Code(Struct);
Define_Code(Template);
Define_Code(Typename);
Define_Code(Typedef);
Define_Code(Union);
Define_Code(Using);
Define_Code(Var);
#undef Define_Code

GEN_NS_PARSER_BEGIN

struct Token;

GEN_NS_PARSER_END

#if GEN_COMPILER_CPP
// Note(Ed): This is to alleviate an edge case with parsing usings or typedefs where I don't really have it setup
// to parse a 'namespace' macro or a type with a macro.
// I have ideas for ways to pack that into the typedef/using ast, but for now just keeping it like this
#define ParserTokenType GEN_NS_PARSER Token
typedef ParserTokenType Token;
#undef ParserTokenType
#endif

#if GEN_COMPILER_CPP
template< class Type> forceinline Type tmpl_cast( Code self ) { return * rcast( Type*, & self ); }
#endif

#pragma region Code C-Interface
void        code_append       (Code code, Code other );
char const* code_debug_str    (Code code);
Code        code_duplicate    (Code code);
Code*       code_entry        (Code code, u32 idx );
bool        code_has_entries  (Code code);
bool        code_is_body      (Code code);
bool        code_is_equal     (Code code, Code other);
bool        code_is_valid     (Code code);
void        code_set_global   (Code code);
String      code_to_string    (Code self );
void        code_to_string_ptr(Code self, String* result );
char const* code_type_str     (Code self );
bool        code_validate_body(Code self );
#pragma endregion Code C-Interface

#if GEN_COMPILER_CPP
/*
	AST* wrapper
	- Not constantly have to append the '*' as this is written often..
	- Allows for implicit conversion to any of the ASTs (raw or filtered).
*/
struct Code
{
	AST* ast;

#	define Using_Code( Typename )                                                               \
	forceinline char const* debug_str()                { return code_debug_str(* this); }       \
	forceinline Code        duplicate()                { return code_duplicate(* this); }	    \
	forceinline bool        is_equal( Code other )     { return code_is_equal(* this, other); } \
	forceinline bool        is_body()                  { return code_is_body(* this); }         \
	forceinline bool        is_valid()                 { return code_is_valid(* this); }        \
	forceinline void        set_global()               { return code_set_global(* this); }

#	define Using_CodeOps( Typename )                                                                           \
	forceinline Typename&  operator = ( Code other );                                                          \
	forceinline bool       operator ==( Code other )                        { return (AST*)ast == other.ast; } \
	forceinline bool       operator !=( Code other )                        { return (AST*)ast != other.ast; } \
	forceinline bool       operator ==(std::nullptr_t) const                { return ast == nullptr; }         \
	forceinline bool       operator !=(std::nullptr_t) const                { return ast != nullptr;  }        \
	operator bool();

#if ! GEN_C_LIKE_CPP
	Using_Code( Code );
	forceinline void        append(Code other)        { return code_append(* this, other); }
	forceinline Code*       entry(u32 idx)            { return code_entry(* this, idx); }
	forceinline bool        has_entries()             { return code_has_entries(* this); }
	forceinline String      to_string()               { return code_to_string(* this); }
	forceinline void        to_string(String& result) { return code_to_string_ptr(* this, & result); }
	forceinline char const* type_str()                { return code_type_str(* this); }
	forceinline bool        validate_body()           { return code_validate_body(*this); }
#endif

	Using_CodeOps( Code );
	forceinline AST* operator ->() { return ast; }

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
	operator CodeTypename() const;
	operator CodeTypedef() const;
	operator CodeUnion() const;
	operator CodeUsing() const;
	operator CodeVar() const;
	#undef operator
};
#endif

#pragma region Statics
// Used to identify ASTs that should always be duplicated. (Global constant ASTs)
extern Code Code_Global;

// Used to identify invalid generated code.
extern Code Code_Invalid;
#pragma endregion Statics

struct Code_POD
{
	AST* ast;
};
static_assert( sizeof(Code) == sizeof(Code_POD), "ERROR: Code is not POD" );

// Desired width of the AST data structure.
constexpr int const AST_POD_Size = 128;

constexpr static
int AST_ArrSpecs_Cap =
(
		AST_POD_Size
		- sizeof(StringCached)
		- sizeof(AST*) * 3
		- sizeof(Token*)
		- sizeof(AST*)
		- sizeof(CodeType)
		- sizeof(ModuleFlag)
		- sizeof(u32)
)
/ sizeof(Specifier) - 1;

/*
	Simple AST POD with functionality to seralize into C++ syntax.
*/
struct AST
{
	union {
		struct
		{
			Code      InlineCmt;       // Class, Constructor, Destructor, Enum, Friend, Functon, Operator, OpCast, Struct, Typedef, Using, Variable
			Code      Attributes;      // Class, Enum, Function, Struct, Typedef, Union, Using, Variable
			Code      Specs;           // Destructor, Function, Operator, Typename, Variable
			union {
				Code  InitializerList; // Constructor
				Code  ParentType;      // Class, Struct, ParentType->Next has a possible list of interfaces.
				Code  ReturnType;      // Function, Operator, Typename
				Code  UnderlyingType;  // Enum, Typedef
				Code  ValueType;       // Parameter, Variable
			};
			union {
				Code  Macro;               // Parameter
				Code  BitfieldSize;        // Variable (Class/Struct Data Member)
				Code  Params;              // Constructor, Function, Operator, Template, Typename
				Code  UnderlyingTypeMacro; // Enum
			};
			union {
				Code  ArrExpr;          // Typename
				Code  Body;             // Class, Constructor, Destructor, Enum, Friend, Function, Namespace, Struct, Union
				Code  Declaration;      // Friend, Template
				Code  Value;            // Parameter, Variable
			};
			union {
				Code  NextVar;          // Variable; Possible way to handle comma separated variables declarations. ( , NextVar->Specs NextVar->Name NextVar->ArrExpr = NextVar->Value )
				Code  SuffixSpecs;      // Only used with typenames, to store the function suffix if typename is function signature. ( May not be needed )
				Code  PostNameMacro;     // Only used with parameters for specifically UE_REQUIRES (Thanks Unreal)
			};
		};
		StringCached  Content;          // Attributes, Comment, Execution, Include
		struct {
			Specifier  ArrSpecs[AST_ArrSpecs_Cap]; // Specifiers
			Code       NextSpecs;              // Specifiers; If ArrSpecs is full, then NextSpecs is used.
		};
	};
	StringCached      Name;
	union {
		Code Prev;
		Code Front;
		Code Last;
	};
	union {
		Code Next;
		Code Back;
	};
	Token*            Token; // Reference to starting token, only avaialble if it was derived from parsing.
	Code              Parent;
	CodeType          Type;
//	CodeFlag          CodeFlags;
	ModuleFlag        ModuleFlags;
	union {
		b32           IsFunction;  // Used by typedef to not serialize the name field.
		b32           IsParamPack; // Used by typename to know if type should be considered a parameter pack.
		Operator      Op;
		AccessSpec    ParentAccess;
		s32           NumEntries;
		s32           VarConstructorInit;  // Used by variables to know that initialization is using a constructor expression instead of an assignment expression.
	};
};
static_assert( sizeof(AST) == AST_POD_Size, "ERROR: AST is not size of AST_POD_Size" );

#if GEN_COMPILER_CPP
// Uses an implicitly overloaded cast from the AST to the desired code type.
// Necessary if the user wants GEN_ENFORCE_STRONG_CODE_TYPES
struct  InvalidCode_ImplictCaster;
#define InvalidCode (InvalidCode_ImplictCaster{})
#else
#define InvalidCode { (void*)Code_Invalid }
#endif

#if GEN_COMPILER_CPP
struct NullCode_ImplicitCaster;
// Used when the its desired when omission is allowed in a definition.
#define NullCode    (NullCode_ImplicitCaster{})
#else
#define NullCode    nullptr
#endif
