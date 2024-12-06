#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "ast.hpp"
#endif

#pragma region Code Type Interface
void   append           ( CodeBody body, Code     other );
void   append           ( CodeBody body, CodeBody other );
String to_string        ( CodeBody body );
void   to_string        ( CodeBody body, String* result );
void   to_string_export ( CodeBody body, String* result );

Code begin( CodeBody body);
Code end  ( CodeBody body );
Code next ( CodeBody body );

void   add_interface( CodeClass self, CodeType interface );
String to_string    ( CodeClass self );
void   to_string_def( CodeClass self, String* result );
void   to_string_fwd( CodeClass self, String* result );

void      append     (CodeParam params, CodeParam param );
CodeParam get        (CodeParam params, s32 idx);
bool      has_entries(CodeParam params );
String    to_string  (CodeParam params );
void      to_string  (CodeParam params, String* result );

CodeParam begin(CodeParam params);
CodeParam end  (CodeParam params);

bool   append   (CodeSpecifiers specifiers, Specifier spec);
s32    has      (CodeSpecifiers specifiers, Specifier spec);
s32    remove   (CodeSpecifiers specifiers, Specifier to_remove );
String to_string(CodeSpecifiers specifiers);
void   to_string(CodeSpecifiers specifiers, String* result);

Specifier* begin(CodeSpecifiers specifiers );
Specifier* end  (CodeSpecifiers specifiers);

void   add_interface(CodeStruct self, CodeType interface);
String to_string    (CodeStruct self);
void   to_string_fwd(CodeStruct self, String* result);
void   to_string_def(CodeStruct self, String* result);

String to_string(CodeAttributes attributes);
String to_string(CodeComment    comment );

String to_string    (CodeConstructor constructor);
void   to_string_def(CodeConstructor constructor, String* result );
void   to_string_fwd(CodeConstructor constructor, String* result );

String to_string(CodeDefine define);
void   to_string(CodeDefine define, String* result);

String to_string    (CodeDestructor destructor);
void   to_string_def(CodeDestructor destructor, String* result );
void   to_string_fwd(CodeDestructor destructor, String* result );

String to_string          (CodeEnum self);
void   to_string_def      (CodeEnum self, String* result );
void   to_string_fwd      (CodeEnum self, String* result );
void   to_string_class_def(CodeEnum self, String* result );
void   to_string_class_fwd(CodeEnum self, String* result );

String to_string(CodeExec exec);

void to_string(CodeExtern self, String* result);

String to_string(CodeInclude include);
void   to_string(CodeInclude include, String* result);

String to_string(CodeFriend self);
void   to_string(CodeFriend self, String* result);

String to_string    (CodeFn self);
void   to_string_def(CodeFn self, String* result);
void   to_string_fwd(CodeFn self, String* result);

String to_string(CodeModule self);
void   to_string(CodeModule self, String* result);

String to_string(CodeNS self);
void   to_string(CodeNS self, String* result);

String to_string    (CodeOperator self);
void   to_string_fwd(CodeOperator self, String* result );
void   to_string_def(CodeOperator self, String* result );

String to_string    (CodeOpCast op_cast );
void   to_string_def(CodeOpCast op_cast, String* result );
void   to_string_fwd(CodeOpCast op_cast, String* result );

String to_string(CodePragma self);
void   to_string(CodePragma self, String* result);

String to_string       (CodePreprocessCond cond);
void   to_string_if    (CodePreprocessCond cond, String* result );
void   to_string_ifdef (CodePreprocessCond cond, String* result );
void   to_string_ifndef(CodePreprocessCond cond, String* result );
void   to_string_elif  (CodePreprocessCond cond, String* result );
void   to_string_else  (CodePreprocessCond cond, String* result );
void   to_string_endif (CodePreprocessCond cond, String* result );

String to_string(CodeTemplate self);
void   to_string(CodeTemplate self, String* result);

String to_string(CodeTypename self);
void   to_string(CodeTypename self, String* result);

String to_string(CodeTypedef self);
void   to_string(CodeTypedef self, String* result);

String to_string(CodeUnion self);
void   to_string_def(CodeUnion self, String* result);
void   to_string_fwd(CodeUnion self, String* result);

String to_string   (CodeUsing op_cast );
void   to_string   (CodeUsing op_cast, String* result );
void   to_string_ns(CodeUsing op_cast, String* result );

String to_string(CodeVar self);
void   to_string(CodeVar self, String* result);
#pragma endregion Code Type Interface

#pragma region Code Types
// These structs are not used at all by the C vairant.
#if ! GEN_COMPILER_C
// stati_assert( GEN_COMPILER_C, "This should not be compiled with the C-library" );

#define Verify_POD(Type) static_assert(size_of(Code##Type) == size_of(AST_##Type), "ERROR: Code##Type is not a POD")

struct CodeBody
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeBody );

	void append( Code other )    { return GEN_NS append( *this, other ); }
	void append( CodeBody body ) { return GEN_NS append(*this, body); }
	bool has_entries()           { return GEN_NS code_has_entries(* this); }

	String to_string()                        { return GEN_NS to_string(* this); }
	void   to_string( String& result )        { return GEN_NS to_string(* this, & result ); }
	void   to_string_export( String& result ) { return GEN_NS to_string_export(* this, & result); }

	Code begin() { return GEN_NS begin(* this); }
	Code end()   { return GEN_NS end(* this); }
#endif

	Using_CodeOps( CodeBody );
	operator Code() { return * rcast( Code*, this ); }
	AST_Body* operator->() { return ast; }

	AST_Body* ast;
};

struct CodeClass
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeClass );

	void add_interface( CodeType interface );

	String to_string();
	void   to_string_def( String& result );
	void   to_string_fwd( String& result );
#endif

	Using_CodeOps( CodeClass );
	operator Code() { return * rcast( Code*, this ); }
	AST_Class* operator->()
	{
		if ( ast == nullptr )
		{
			log_failure("Attempt to dereference a nullptr");
			return nullptr;
		}
		return ast;
	}
	AST_Class* ast;
};

struct CodeParam
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeParam );

	void      append( CodeParam other );
	CodeParam get( s32 idx );
	bool      has_entries();
	String    to_string();
	void      to_string( String& result );
#endif

	Using_CodeOps( CodeParam );
	AST_Param* operator->()
	{
		if ( ast == nullptr )
		{
			log_failure("Attempt to dereference a nullptr!");
			return nullptr;
		}
		return ast;
	}
	operator Code()       { return { (AST*)ast }; }
	CodeParam operator*() { return * this; }
	CodeParam& operator++();

	AST_Param* ast;
};

struct CodeSpecifiers
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeSpecifiers );

	bool   append( Specifier spec )       { return GEN_NS append(* this, spec); }
	s32    has( Specifier spec )          { return GEN_NS has(* this, spec); }
	s32    remove( Specifier to_remove )  { return GEN_NS remove(* this, to_remove); }
	String to_string()                    { return GEN_NS to_string(* this ); }
	void   to_string( String& result )    { return GEN_NS to_string(* this, & result); }
#endif

	Using_CodeOps(CodeSpecifiers);
	operator Code() { return { (AST*) ast }; }
	AST_Specifiers* operator->()
	{
		if ( ast == nullptr )
		{
			log_failure("Attempt to dereference a nullptr!");
			return nullptr;
		}
		return ast;
	}

	AST_Specifiers* ast;
};

struct CodeStruct
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeStruct );

	void add_interface( CodeType interface );

	String to_string();
	void   to_string_fwd( String& result );
	void   to_string_def( String& result );
#endif

	Using_CodeOps( CodeStruct );
	operator Code() { return * rcast( Code*, this ); }
	AST_Struct* operator->()
	{
		if ( ast == nullptr )
		{
			log_failure("Attempt to dereference a nullptr");
			return nullptr;
		}
		return ast;
	}
	AST_Struct* ast;
};

struct CodeAttributes
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code(CodeAttributes);
	String to_string();
#endif

	Using_CodeOps(CodeAttributes);
	operator Code();
	AST_Attributes *operator->();
	AST_Attributes *ast;
};

// Define_CodeType( BaseClass );

struct CodeComment
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code(CodeComment);
	String to_string() { return GEN_NS to_string(* this); }
#endif

	Using_CodeOps(CodeComment);
	operator Code();
	AST_Comment *operator->();
	AST_Comment *ast;
};

struct CodeConstructor
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeConstructor );

	String to_string()                     { return GEN_NS to_string(* this); }
	void   to_string_def( String& result ) { return GEN_NS to_string_def(* this, & result); }
	void   to_string_fwd( String& result ) { return GEN_NS to_string_fwd(* this, & result); }
#endif

	Using_CodeOps(CodeConstructor);
	operator         Code();
	AST_Constructor* operator->();
	AST_Constructor* ast;
};

struct CodeDefine
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeDefine );

	String to_string()                 { return GEN_NS to_string(* this); }
	void   to_string( String& result ) { return GEN_NS to_string(* this, & result); }
#endif

	Using_CodeOps(CodeDefine);
	operator    Code();
	AST_Define* operator->();
	AST_Define* ast;
};

struct CodeDestructor
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeDestructor );

	String to_string()                     { return GEN_NS to_string(* this); }
	void   to_string_def( String& result ) { return GEN_NS to_string_def(* this, & result); }
	void   to_string_fwd( String& result ) { return GEN_NS to_string_fwd(* this, & result); }
#endif

	Using_CodeOps(CodeDestructor);
	operator         Code();
	AST_Destructor* operator->();
	AST_Destructor* ast;
};

struct CodeEnum
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeEnum );

	String to_string()                            { return GEN_NS to_string(* this); }
	void   to_string_def( String& result )        { return GEN_NS to_string_def(* this, & result); }
	void   to_string_fwd( String& result )        { return GEN_NS to_string_fwd(* this, & result); }
	void   to_string_class_def( String& result )  { return GEN_NS to_string_class_def(* this, & result); }
	void   to_string_class_fwd( String& result )  { return GEN_NS to_string_class_fwd(* this, & result); }
#endif

	Using_CodeOps(CodeEnum);
	operator  Code();
	AST_Enum* operator->();
	AST_Enum* ast;
};

struct CodeExec
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code(CodeExec);
	String to_string() { return GEN_NS to_string(* this); }
#endif

	Using_CodeOps(CodeExec);
	operator Code();
	AST_Exec *operator->();
	AST_Exec *ast;
};

#if GEN_EXECUTION_EXPRESSION_SUPPORT
struct CodeExpr
{
	Using_Code( CodeExpr );

	void to_string( String& result );

	AST*      raw();
	operator  Code();
	AST_Expr* operator->();
	AST_Expr* ast;
};

struct CodeExpr_Assign
{
	Using_Code( CodeExpr_Assign );

	void to_string( String& result );

	AST*             raw();
	operator         Code();
	AST_Expr_Assign* operator->();
	AST_Expr_Assign* ast;
};

struct CodeExpr_Alignof
{
	Using_Code( CodeExpr_Alignof );

	void to_string( String& result );

	AST*              raw();
	operator          Code();
	AST_Expr_Alignof* operator->();
	AST_Expr_Alignof* ast;
};

struct CodeExpr_Binary
{
	Using_Code( CodeExpr_Binary );

	void to_string( String& result );

	AST*             raw();
	operator         Code();
	AST_Expr_Binary* operator->();
	AST_Expr_Binary* ast;
};

struct CodeExpr_CStyleCast
{
	Using_Code( CodeExpr_CStyleCast );

	void to_string( String& result );

	AST*                 raw();
	operator             Code();
	AST_Expr_CStyleCast* operator->();
	AST_Expr_CStyleCast* ast;
};

struct CodeExpr_FunctionalCast
{
	Using_Code( CodeExpr_FunctionalCast );

	void to_string( String& result );

	AST*                     raw();
	operator                 Code();
	AST_Expr_FunctionalCast* operator->();
	AST_Expr_FunctionalCast* ast;
};

struct CodeExpr_CppCast
{
	Using_Code( CodeExpr_CppCast );

	void to_string( String& result );

	AST*              raw();
	operator          Code();
	AST_Expr_CppCast* operator->();
	AST_Expr_CppCast* ast;
};

struct CodeExpr_Element
{
	Using_Code( CodeExpr_Element );

	void to_string( String& result );

	AST*              raw();
	operator          Code();
	AST_Expr_Element* operator->();
	AST_Expr_Element* ast;
};

struct CodeExpr_ProcCall
{
	Using_Code( CodeExpr_ProcCall );

	void to_string( String& result );

	AST*               raw();
	operator           Code();
	AST_Expr_ProcCall* operator->();
	AST_Expr_ProcCall* ast;
};

struct CodeExpr_Decltype
{
	Using_Code( CodeExpr_Decltype );

	void to_string( String& result );

	AST*      raw();
	operator  Code();
	AST_Expr_Decltype* operator->();
	AST_Expr_Decltype* ast;
};

struct CodeExpr_Comma
{
	Using_Code( CodeExpr_Comma );

	void to_string( String& result );

	AST*      raw();
	operator  Code();
	AST_Expr_Comma* operator->();
	AST_Expr_Comma* ast;
};

struct CodeExpr_AMS
{
	Using_Code( CodeExpr_AMS );

	void to_string( String& result );

	AST*          raw();
	operator      Code();
	AST_Expr_AMS* operator->();
	AST_Expr_AMS* ast;
};

struct CodeExpr_Sizeof
{
	Using_Code( CodeExpr_Sizeof );

	void to_string( String& result );

	AST*             raw();
	operator         Code();
	AST_Expr_Sizeof* operator->();
	AST_Expr_Sizeof* ast;
};

struct CodeExpr_Subscript
{
	Using_Code( CodeExpr_Subscript );

	void to_string( String& result );

	AST*                raw();
	operator            Code();
	AST_Expr_Subscript* operator->();
	AST_Expr_Subscript* ast;
};

struct CodeExpr_Ternary
{
	Using_Code( CodeExpr_Ternary );

	void to_string( String& result );

	AST*              raw();
	operator          Code();
	AST_Expr_Ternary* operator->();
	AST_Expr_Ternary* ast;
};

struct CodeExpr_UnaryPrefix
{
	Using_Code( CodeExpr_UnaryPrefix );

	void to_string( String& result );

	AST*                  raw();
	operator              Code();
	AST_Expr_UnaryPrefix* operator->();
	AST_Expr_UnaryPrefix* ast;
};

struct CodeExpr_UnaryPostfix
{
	Using_Code( CodeExpr_UnaryPostfix );

	void to_string( String& result );

	AST*                   raw();
	operator               Code();
	AST_Expr_UnaryPostfix* operator->();
	AST_Expr_UnaryPostfix* ast;
};
#endif

struct CodeExtern
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeExtern );

	void to_string( String& result ) { return GEN_NS to_string(* this, & result); }
#endif

	Using_CodeOps(CodeExtern);
	operator    Code();
	AST_Extern* operator->();
	AST_Extern* ast;
};

struct CodeInclude
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeInclude );

	String to_string()                  { return GEN_NS to_string(* this); }
	void   to_string( String& result )  { return GEN_NS to_string(* this, & result); }
#endif

	Using_CodeOps(CodeInclude);
	operator     Code();
	AST_Include* operator->();
	AST_Include* ast;
};

struct CodeFriend
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeFriend );

	String to_string()                 { return GEN_NS to_string(* this); }
	void   to_string( String& result ) { return GEN_NS to_string(* this, & result); }
#endif

	Using_CodeOps(CodeFriend);
	operator    Code();
	AST_Friend* operator->();
	AST_Friend* ast;
};

struct CodeFn
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeFn );

	String to_string()                     { return GEN_NS to_string(* this); }
	void   to_string_def( String& result ) { return GEN_NS to_string_def(* this, & result); }
	void   to_string_fwd( String& result ) { return GEN_NS to_string_fwd(* this, & result); }
#endif

	Using_CodeOps(CodeFn);
	operator Code();
	AST_Fn*  operator->();
	AST_Fn*  ast;
};

struct CodeModule
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeModule );

	String to_string()                 { return GEN_NS to_string(* this); }
	void   to_string( String& result ) { return GEN_NS to_string(* this, & result); }
#endif

	Using_CodeOps(CodeModule);
	operator    Code();
	AST_Module* operator->();
	AST_Module* ast;
};

struct CodeNS
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeNS );

	String to_string()                 { return GEN_NS to_string(* this); }
	void   to_string( String& result ) { return GEN_NS to_string(* this, & result); }
#endif

	Using_CodeOps(CodeNS);
	operator Code();
	AST_NS*  operator->();
	AST_NS*  ast;
};

struct CodeOperator
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeOperator );

	String to_string()                     { return GEN_NS to_string(* this); }
	void   to_string_def( String& result ) { return GEN_NS to_string_def(* this, & result); }
	void   to_string_fwd( String& result ) { return GEN_NS to_string_fwd(* this, & result); }
#endif

	Using_CodeOps(CodeOperator);
	operator      Code();
	AST_Operator* operator->();
	AST_Operator* ast;
};

struct CodeOpCast
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeOpCast );

	String to_string()                     { return GEN_NS to_string(* this); }
	void   to_string_def( String& result ) { return GEN_NS to_string_def(* this, & result); }
	void   to_string_fwd( String& result ) { return GEN_NS to_string_fwd(* this, & result); }
#endif

	Using_CodeOps(CodeOpCast);
	operator    Code();
	AST_OpCast* operator->();
	AST_OpCast* ast;
};

struct CodePragma
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodePragma );

	String to_string()                 { return GEN_NS to_string(* this); }
	void   to_string( String& result ) { return GEN_NS to_string(* this, & result); }
#endif

	Using_CodeOps( CodePragma );
	operator    Code();
	AST_Pragma* operator->();
	AST_Pragma* ast;
};

struct CodePreprocessCond
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodePreprocessCond );

	String to_string()                        { return GEN_NS to_string(* this); }
	void   to_string_if( String& result )     { return GEN_NS to_string_if(* this, & result); }
	void   to_string_ifdef( String& result )  { return GEN_NS to_string_ifdef(* this, & result); }
	void   to_string_ifndef( String& result ) { return GEN_NS to_string_ifndef(* this, & result); }
	void   to_string_elif( String& result )   { return GEN_NS to_string_elif(* this, & result); }
	void   to_string_else( String& result )   { return GEN_NS to_string_else(* this, & result); }
	void   to_string_endif( String& result )  { return GEN_NS to_string_endif(* this, & result); }
#endif

	Using_CodeOps( CodePreprocessCond );
	operator            Code();
	AST_PreprocessCond* operator->();
	AST_PreprocessCond* ast;
};

#if GEN_EXECUTION_EXPRESSION_SUPPORT
struct CodeStmt
{
	Using_Code( CodeStmt );

	String to_string();
	void   to_string( String& result );

	AST*      raw();
	operator  Code();
	AST_Stmt* operator->();
	AST_Stmt* ast;
};

struct CodeStmt_Break
{
	Using_Code( CodeStmt_Break );

	String to_string();
	void   to_string( String& result );

	AST*            raw();
	operator        Code();
	AST_Stmt_Break* operator->();
	AST_Stmt_Break* ast;
};

struct CodeStmt_Case
{
	Using_Code( CodeStmt_Case );

	String to_string();
	void   to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_Case* operator->();
	AST_Stmt_Case* ast;
};

struct CodeStmt_Continue
{
	Using_Code( CodeStmt_Continue );

	String to_string();
	void   to_string( String& result );

	AST*               raw();
	operator           Code();
	AST_Stmt_Continue* operator->();
	AST_Stmt_Continue* ast;
};

struct CodeStmt_Decl
{
	Using_Code( CodeStmt_Decl );

	String to_string();
	void   to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_Decl* operator->();
	AST_Stmt_Decl* ast;
};

struct CodeStmt_Do
{
	Using_Code( CodeStmt_Do );

	String to_string();
	void   to_string( String& result );

	AST*         raw();
	operator     Code();
	AST_Stmt_Do* operator->();
	AST_Stmt_Do* ast;
};

struct CodeStmt_Expr
{
	Using_Code( CodeStmt_Expr );

	String to_string();
	void   to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_Expr* operator->();
	AST_Stmt_Expr* ast;
};

struct CodeStmt_Else
{
	Using_Code( CodeStmt_Else );

	String to_string();
	void   to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_Else* operator->();
	AST_Stmt_Else* ast;
};

struct CodeStmt_If
{
	Using_Code( CodeStmt_If );

	String to_string();
	void   to_string( String& result );

	AST*         raw();
	operator     Code();
	AST_Stmt_If* operator->();
	AST_Stmt_If* ast;
};

struct CodeStmt_For
{
	Using_Code( CodeStmt_For );

	String to_string();
	void   to_string( String& result );

	AST*          raw();
	operator      Code();
	AST_Stmt_For* operator->();
	AST_Stmt_For* ast;
};

struct CodeStmt_Goto
{
	Using_Code( CodeStmt_Goto );

	String to_string();
	void   to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_Goto* operator->();
	AST_Stmt_Goto* ast;
};

struct CodeStmt_Label
{
	Using_Code( CodeStmt_Label );

	String to_string();
	void   to_string( String& result );

	AST*            raw();
	operator        Code();
	AST_Stmt_Label* operator->();
	AST_Stmt_Label* ast;
};

struct CodeStmt_Switch
{
	Using_Code( CodeStmt_Switch );

	String to_string();
	void   to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_Switch* operator->();
	AST_Stmt_Switch* ast;
};

struct CodeStmt_While
{
	Using_Code( CodeStmt_While );

	String to_string();
	void   to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_While* operator->();
	AST_Stmt_While* ast;
};
#endif

struct CodeTemplate
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeTemplate );

	String to_string()                 { return GEN_NS to_string(* this); }
	void   to_string( String& result ) { return GEN_NS to_string(* this, & result); }
#endif

	Using_CodeOps( CodeTemplate );
	operator      Code();
	AST_Template* operator->();
	AST_Template* ast;
};

struct CodeTypename
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeTypename );

	String to_string()                 { return GEN_NS to_string(* this); }
	void   to_string( String& result ) { return GEN_NS to_string(* this, & result); }
#endif

	Using_CodeOps( CodeTypename );
	operator      Code();
	AST_Typename* operator->();
	AST_Typename* ast;
};

struct CodeTypedef
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeTypedef );

	String to_string()                 { return GEN_NS to_string(* this); }
	void   to_string( String& result ) { return GEN_NS to_string(* this, & result); }
#endif

	Using_CodeOps( CodeTypedef );
	operator     Code();
	AST_Typedef* operator->();
	AST_Typedef* ast;
};

struct CodeUnion
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeUnion );

	String to_string()                     { return GEN_NS to_string(* this); }
	void   to_string_def( String& result ) { return GEN_NS to_string_def(* this, & result); }
	void   to_string_fwd( String& result ) { return GEN_NS to_string_fwd(* this, & result); }
#endif

	Using_CodeOps(CodeUnion);
	operator   Code();
	AST_Union* operator->();
	AST_Union* ast;
};

struct CodeUsing
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeUsing );

	String to_string()                    { return GEN_NS to_string(* this); }
	void   to_string( String& result )    { return GEN_NS to_string(* this, & result); }
	void   to_string_ns( String& result ) { return GEN_NS to_string_ns(* this, & result); }
#endif

	Using_CodeOps(CodeUsing);
	operator   Code();
	AST_Using* operator->();
	AST_Using* ast;
};

struct CodeVar
{
#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	Using_Code( CodeVar );

	String to_string()                 { return GEN_NS to_string(* this); }
	void   to_string( String& result ) { return GEN_NS to_string(* this, & result); }
#endif

	Using_CodeOps(CodeVar);
	operator Code();
	AST_Var* operator->();
	AST_Var* ast;
};

#undef Define_CodeType
#undef Using_Code
#undef Using_CodeOps

#if GEN_SUPPORT_CPP_REFERENCES
void to_string_export( CodeBody body, String& result ) { return to_string_export(body, & result); };
#endif

#undef Verify_POD

struct InvalidCode_ImplictCaster
{
	// operator CodeBaseClass() const;
    operator Code              () const { return Code_Invalid; }
    operator CodeBody          () const { return cast(CodeBody,           Code_Invalid); }
    operator CodeAttributes    () const { return cast(CodeAttributes,     Code_Invalid); }
    operator CodeComment       () const { return cast(CodeComment,        Code_Invalid); }
    operator CodeClass         () const { return cast(CodeClass,          Code_Invalid); }
    operator CodeConstructor   () const { return cast(CodeConstructor,    Code_Invalid); }
    operator CodeDefine        () const { return cast(CodeDefine,         Code_Invalid); }
    operator CodeDestructor    () const { return cast(CodeDestructor,     Code_Invalid); }
    operator CodeExec          () const { return cast(CodeExec,           Code_Invalid); }
    operator CodeEnum          () const { return cast(CodeEnum,           Code_Invalid); }
    operator CodeExtern        () const { return cast(CodeExtern,         Code_Invalid); }
    operator CodeInclude       () const { return cast(CodeInclude,        Code_Invalid); }
    operator CodeFriend        () const { return cast(CodeFriend,         Code_Invalid); }
    operator CodeFn            () const { return cast(CodeFn,             Code_Invalid); }
    operator CodeModule        () const { return cast(CodeModule,         Code_Invalid); }
    operator CodeNS            () const { return cast(CodeNS,             Code_Invalid); }
    operator CodeOperator      () const { return cast(CodeOperator,       Code_Invalid); }
    operator CodeOpCast        () const { return cast(CodeOpCast,         Code_Invalid); }
    operator CodeParam         () const { return cast(CodeParam,          Code_Invalid); }
    operator CodePragma        () const { return cast(CodePragma,         Code_Invalid); }
    operator CodePreprocessCond() const { return cast(CodePreprocessCond, Code_Invalid); }
    operator CodeSpecifiers    () const { return cast(CodeSpecifiers,     Code_Invalid); }
    operator CodeStruct        () const { return cast(CodeStruct,         Code_Invalid); }
    operator CodeTemplate      () const { return cast(CodeTemplate,       Code_Invalid); }
    operator CodeTypename      () const { return cast(CodeTypename,       Code_Invalid); }
    operator CodeTypedef       () const { return cast(CodeTypedef,        Code_Invalid); }
    operator CodeUnion         () const { return cast(CodeUnion,          Code_Invalid); }
    operator CodeUsing         () const { return cast(CodeUsing,          Code_Invalid); }
    operator CodeVar           () const { return cast(CodeVar,            Code_Invalid); }
};

#endif //if ! GEN_COMPILER_C

#pragma endregion Code Types
