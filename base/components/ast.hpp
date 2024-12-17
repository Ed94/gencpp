#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "types.hpp"
#include "gen/ecode.hpp"
#include "gen/eoperator.hpp"
#include "gen/especifier.hpp"
#endif

/*
  ______   ______  ________      __    __       ______                 __
 /      \ /      \|        \    |  \  |  \     /      \               |  \
|  ▓▓▓▓▓▓\  ▓▓▓▓▓▓\\▓▓▓▓▓▓▓▓    | ▓▓\ | ▓▓    |  ▓▓▓▓▓▓\ ______   ____| ▓▓ ______
| ▓▓__| ▓▓ ▓▓___\▓▓  | ▓▓       | ▓▓▓\| ▓▓    | ▓▓   \▓▓/      \ /      ▓▓/      \
| ▓▓    ▓▓\▓▓    \   | ▓▓       | ▓▓▓▓\ ▓▓    | ▓▓     |  ▓▓▓▓▓▓\  ▓▓▓▓▓▓▓  ▓▓▓▓▓▓\
| ▓▓▓▓▓▓▓▓_\▓▓▓▓▓▓\  | ▓▓       | ▓▓\▓▓ ▓▓    | ▓▓   __| ▓▓  | ▓▓ ▓▓  | ▓▓ ▓▓    ▓▓
| ▓▓  | ▓▓  \__| ▓▓  | ▓▓       | ▓▓ \▓▓▓▓    | ▓▓__/  \ ▓▓__/ ▓▓ ▓▓__| ▓▓ ▓▓▓▓▓▓▓▓
| ▓▓  | ▓▓\▓▓    ▓▓  | ▓▓       | ▓▓  \▓▓▓     \▓▓    ▓▓\▓▓    ▓▓\▓▓    ▓▓\▓▓     \
 \▓▓   \▓▓ \▓▓▓▓▓▓    \▓▓        \▓▓   \▓▓      \▓▓▓▓▓▓  \▓▓▓▓▓▓  \▓▓▓▓▓▓▓ \▓▓▓▓▓▓▓
*/

struct AST;
struct AST_Body;
struct AST_Attributes;
struct AST_Comment;
struct AST_Constructor;
// struct AST_BaseClass;
struct AST_Class;
struct AST_Define;
struct AST_DefineParams;
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
struct AST_Params;
struct AST_Pragma;
struct AST_PreprocessCond;
struct AST_Specifiers;

#ifdef GEN_EXECUTION_EXPRESSION_SUPPORT
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
typedef AST* Code;
#else
struct Code;
#endif

#if GEN_COMPILER_C
typedef AST_Body*           CodeBody;
typedef AST_Attributes*     CodeAttributes;
typedef AST_Comment*        CodeComment;
typedef AST_Class*          CodeClass;
typedef AST_Constructor*    CodeConstructor;
typedef AST_Define*         CodeDefine;
typedef AST_DefineParams*   CodeDefineParams;
typedef AST_Destructor*     CodeDestructor;
typedef AST_Enum*           CodeEnum;
typedef AST_Exec*           CodeExec;
typedef AST_Extern*         CodeExtern;
typedef AST_Include*        CodeInclude;
typedef AST_Friend*         CodeFriend;
typedef AST_Fn*             CodeFn;
typedef AST_Module*         CodeModule;
typedef AST_NS*             CodeNS;
typedef AST_Operator*       CodeOperator;
typedef AST_OpCast*         CodeOpCast;
typedef AST_Params*         CodeParams;
typedef AST_PreprocessCond* CodePreprocessCond;
typedef AST_Pragma*         CodePragma;
typedef AST_Specifiers*     CodeSpecifiers;
#else
struct CodeBody;
struct CodeAttributes;
struct CodeComment;
struct CodeClass;
struct CodeConstructor;
struct CodeDefine;
struct CodeDefineParams;
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
struct CodeParams;
struct CodePreprocessCond;
struct CodePragma;
struct CodeSpecifiers;
#endif

#ifdef GEN_EXECUTION_EXPRESSION_SUPPORT

#if GEN_COMPILER_C
typedef AST_Expr*                CodeExpr;
typedef AST_Expr_Assign*         CodeExpr_Assign;
typedef AST_Expr_Alignof*        CodeExpr_Alignof;
typedef AST_Expr_Binary*         CodeExpr_Binary;
typedef AST_Expr_CStyleCast*     CodeExpr_CStyleCast;
typedef AST_Expr_FunctionalCast* CodeExpr_FunctionalCast;
typedef AST_Expr_CppCast*        CodeExpr_CppCast;
typedef AST_Expr_Element*        CodeExpr_Element;
typedef AST_Expr_ProcCall*       CodeExpr_ProcCall;
typedef AST_Expr_Decltype*       CodeExpr_Decltype;
typedef AST_Expr_Comma*          CodeExpr_Comma;
typedef AST_Expr_AMS*            CodeExpr_AMS; // Access Member Symbol
typedef AST_Expr_Sizeof*         CodeExpr_Sizeof;
typedef AST_Expr_Subscript*      CodeExpr_Subscript;
typedef AST_Expr_Ternary*        CodeExpr_Ternary;
typedef AST_Expr_UnaryPrefix*    CodeExpr_UnaryPrefix;
typedef AST_Expr_UnaryPostfix*   CodeExpr_UnaryPostfix;
#else
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
#endif

#if GEN_COMPILER_C
typedef AST_Stmt*          CodeStmt;
typedef AST_Stmt_Break*    CodeStmt_Break;
typedef AST_Stmt_Case*     CodeStmt_Case;
typedef AST_Stmt_Continue* CodeStmt_Continue;
typedef AST_Stmt_Decl*     CodeStmt_Decl;
typedef AST_Stmt_Do*       CodeStmt_Do;
typedef AST_Stmt_Expr*     CodeStmt_Expr;
typedef AST_Stmt_Else*     CodeStmt_Else;
typedef AST_Stmt_If*       CodeStmt_If;
typedef AST_Stmt_For*      CodeStmt_For;
typedef AST_Stmt_Goto*     CodeStmt_Goto;
typedef AST_Stmt_Label*    CodeStmt_Label;
typedef AST_Stmt_Switch*   CodeStmt_Switch;
typedef AST_Stmt_While*    CodeStmt_While;
#else
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

// GEN_EXECUTION_EXPRESSION_SUPPORT
#endif

#if GEN_COMPILER_C
typedef AST_Struct*   CodeStruct;
typedef AST_Template* CodeTemplate;
typedef AST_Typename* CodeTypename;
typedef AST_Typedef*  CodeTypedef;
typedef AST_Union*    CodeUnion;
typedef AST_Using*    CodeUsing;
typedef AST_Var*      CodeVar;
#else
struct CodeStruct;
struct CodeTemplate;
struct CodeTypename;
struct CodeTypedef;
struct CodeUnion;
struct CodeUsing;
struct CodeVar;
#endif

#if GEN_COMPILER_CPP
template< class Type> forceinline Type tmpl_cast( Code self ) { return * rcast( Type*, & self ); }
#endif

#pragma region Code C-Interface

GEN_API void       code_append           (Code code, Code other );
GEN_API Str        code_debug_str        (Code code);
GEN_API Code       code_duplicate        (Code code);
GEN_API Code*      code_entry            (Code code, u32 idx );
GEN_API bool       code_has_entries      (Code code);
GEN_API bool       code_is_body          (Code code);
GEN_API bool       code_is_equal         (Code code, Code other);
GEN_API bool       code_is_valid         (Code code);
GEN_API void       code_set_global       (Code code);
GEN_API StrBuilder code_to_strbuilder    (Code self );
GEN_API void       code_to_strbuilder_ptr(Code self, StrBuilder* result );
GEN_API Str        code_type_str         (Code self );
GEN_API bool       code_validate_body    (Code self );

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

#	define Using_Code( Typename )                                                        \
	forceinline Str  debug_str()                { return code_debug_str(* this); }       \
	forceinline Code duplicate()                { return code_duplicate(* this); }	     \
	forceinline bool is_equal( Code other )     { return code_is_equal(* this, other); } \
	forceinline bool is_body()                  { return code_is_body(* this); }         \
	forceinline bool is_valid()                 { return code_is_valid(* this); }        \
	forceinline void set_global()               { return code_set_global(* this); }

#	define Using_CodeOps( Typename )                                                                           \
	forceinline Typename&  operator = ( Code other );                                                          \
	forceinline bool       operator ==( Code other )                        { return (AST*)ast == other.ast; } \
	forceinline bool       operator !=( Code other )                        { return (AST*)ast != other.ast; } \
	forceinline bool       operator ==(std::nullptr_t) const                { return ast == nullptr; }         \
	forceinline bool       operator !=(std::nullptr_t) const                { return ast != nullptr;  }        \
	operator bool();

#if ! GEN_C_LIKE_CPP
	Using_Code( Code );
	forceinline void       append(Code other)                { return code_append(* this, other); }
	forceinline Code*      entry(u32 idx)                    { return code_entry(* this, idx); }
	forceinline bool       has_entries()                     { return code_has_entries(* this); }
	forceinline StrBuilder to_strbuilder()                   { return code_to_strbuilder(* this); }
	forceinline void       to_strbuilder(StrBuilder& result) { return code_to_strbuilder_ptr(* this, & result); }
	forceinline Str        type_str()                        { return code_type_str(* this); }
	forceinline bool       validate_body()                   { return code_validate_body(*this); }
#endif

	Using_CodeOps( Code );
	forceinline Code operator *() { return * this; } // Required to support for-range iteration.
	forceinline AST* operator ->() { return ast; }

	Code& operator ++();

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
	operator CodeDefineParams() const;
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
	operator CodeParams() const;
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
	- sizeof(Code)
	- sizeof(StrCached)
	- sizeof(Code) * 2
	- sizeof(Token*)
	- sizeof(Code)
	- sizeof(CodeType)
	- sizeof(ModuleFlag)
	- sizeof(u32)
)
/ sizeof(Specifier) - 1;

/*
	Simple AST POD with functionality to seralize into C++ syntax.
	TODO(Ed): Eventually haven't a transparent AST like this will longer be viable once statements & expressions are in (most likely....)
*/
struct AST
{
	union {
		struct
		{
			Code      InlineCmt;       // Class, Constructor, Destructor, Enum, Friend, Functon, Operator, OpCast, Struct, Typedef, Using, Variable
			Code      Attributes;      // Class, Enum, Function, Struct, Typedef, Union, Using, Variable // TODO(Ed): Parameters can have attributes
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
				Code  Params;              // Constructor, Define, Function, Operator, Template, Typename
				Code  UnderlyingTypeMacro; // Enum
			};
			union {
				Code  ArrExpr;          // Typename
				Code  Body;             // Class, Constructor, Define, Destructor, Enum, Friend, Function, Namespace, Struct, Union
				Code  Declaration;      // Friend, Template
				Code  Value;            // Parameter, Variable
			};
			union {
				Code  NextVar;          // Variable; Possible way to handle comma separated variables declarations. ( , NextVar->Specs NextVar->Name NextVar->ArrExpr = NextVar->Value )
				Code  SuffixSpecs;      // Only used with typenames, to store the function suffix if typename is function signature. ( May not be needed )
				Code  PostNameMacro;    // Only used with parameters for specifically UE_REQUIRES (Thanks Unreal)
			};
		};
		StrCached  Content;          // Attributes, Comment, Execution, Include
		struct {
			Specifier  ArrSpecs[AST_ArrSpecs_Cap]; // Specifiers
			Code       NextSpecs;              // Specifiers; If ArrSpecs is full, then NextSpecs is used.
		};
	};
	StrCached      Name;
	union {
		Code Prev;
		Code Front;
		Code Last;
	};
	union {
		Code Next;
		Code Back;
	};
	Token*            Token; // Reference to starting token, only available if it was derived from parsing.
	Code              Parent;
	CodeType          Type;
//	CodeFlag          CodeFlags;
	ModuleFlag        ModuleFlags;
	union {
		b32           IsFunction;  // Used by typedef to not serialize the name field.
		struct {
			b16           IsParamPack;   // Used by typename to know if type should be considered a parameter pack.
			ETypenameTag  TypeTag;       // Used by typename to keep track of explicitly declared tags for the identifier (enum, struct, union)
		};
		Operator      Op;
		AccessSpec    ParentAccess;
		s32           NumEntries;
		s32           VarParenthesizedInit;  // Used by variables to know that initialization is using a constructor expression instead of an assignment expression.
	};
};
static_assert( sizeof(AST) == AST_POD_Size, "ERROR: AST is not size of AST_POD_Size" );

#if GEN_COMPILER_CPP
// Uses an implicitly overloaded cast from the AST to the desired code type.
// Necessary if the user wants GEN_ENFORCE_STRONG_CODE_TYPES
struct  InvalidCode_ImplictCaster;
#define InvalidCode (InvalidCode_ImplictCaster{})
#else
#define InvalidCode (void*){ (void*)Code_Invalid }
#endif

#if GEN_COMPILER_CPP
struct NullCode_ImplicitCaster;
// Used when the its desired when omission is allowed in a definition.
#define NullCode    (NullCode_ImplicitCaster{})
#else
#define NullCode    nullptr
#endif
