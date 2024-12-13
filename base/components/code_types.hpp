#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "ast.hpp"
#endif

/*
  ______                 __               ______            __                        ______
 /      \               |  \             |      \          |  \                      /      \
|  ▓▓▓▓▓▓\ ______   ____| ▓▓ ______       \▓▓▓▓▓▓_______  _| ▓▓_    ______   ______ |  ▓▓▓▓▓▓\ ______   _______  ______
| ▓▓   \▓▓/      \ /      ▓▓/      \       | ▓▓ |       \|   ▓▓ \  /      \ /      \| ▓▓_  \▓▓|      \ /       \/      \
| ▓▓     |  ▓▓▓▓▓▓\  ▓▓▓▓▓▓▓  ▓▓▓▓▓▓\      | ▓▓ | ▓▓▓▓▓▓▓\\▓▓▓▓▓▓ |  ▓▓▓▓▓▓\  ▓▓▓▓▓▓\ ▓▓ \     \▓▓▓▓▓▓\  ▓▓▓▓▓▓▓  ▓▓▓▓▓▓\
| ▓▓   __| ▓▓  | ▓▓ ▓▓  | ▓▓ ▓▓    ▓▓      | ▓▓ | ▓▓  | ▓▓ | ▓▓ __| ▓▓    ▓▓ ▓▓   \▓▓ ▓▓▓▓    /      ▓▓ ▓▓     | ▓▓    ▓▓
| ▓▓__/  \ ▓▓__/ ▓▓ ▓▓__| ▓▓ ▓▓▓▓▓▓▓▓     _| ▓▓_| ▓▓  | ▓▓ | ▓▓|  \ ▓▓▓▓▓▓▓▓ ▓▓     | ▓▓     |  ▓▓▓▓▓▓▓ ▓▓_____| ▓▓▓▓▓▓▓▓
 \▓▓    ▓▓\▓▓    ▓▓\▓▓    ▓▓\▓▓     \    |   ▓▓ \ ▓▓  | ▓▓  \▓▓  ▓▓\▓▓     \ ▓▓     | ▓▓      \▓▓    ▓▓\▓▓     \\▓▓     \
  \▓▓▓▓▓▓  \▓▓▓▓▓▓  \▓▓▓▓▓▓▓ \▓▓▓▓▓▓▓     \▓▓▓▓▓▓\▓▓   \▓▓   \▓▓▓▓  \▓▓▓▓▓▓▓\▓▓      \▓▓       \▓▓▓▓▓▓▓ \▓▓▓▓▓▓▓ \▓▓▓▓▓▓▓
*/

#pragma region Code Type C-Interface

void   body_append          ( CodeBody body, Code     other );
void   body_append_body     ( CodeBody body, CodeBody other );
StrBuilder body_to_string       ( CodeBody body );
void   body_to_strbuilder_ref   ( CodeBody body, StrBuilder* result );
void   body_to_strbuilder_export( CodeBody body, StrBuilder* result );

Code begin_CodeBody( CodeBody body);
Code end_CodeBody  ( CodeBody body );
Code next_CodeBody ( CodeBody body, Code entry_iter );

void   class_add_interface( CodeClass self, CodeTypename interface );
StrBuilder class_to_string    ( CodeClass self );
void   class_to_strbuilder_def( CodeClass self, StrBuilder* result );
void   class_to_strbuilder_fwd( CodeClass self, StrBuilder* result );

void       params_append       (CodeParams params, CodeParams param );
CodeParams params_get          (CodeParams params, s32 idx);
bool       params_has_entries  (CodeParams params );
StrBuilder     params_to_string    (CodeParams params );
void       params_to_strbuilder_ref(CodeParams params, StrBuilder* result );

CodeParams begin_CodeParams(CodeParams params);
CodeParams end_CodeParams  (CodeParams params);
CodeParams next_CodeParams (CodeParams params, CodeParams entry_iter);

bool   specifiers_append       (CodeSpecifiers specifiers, Specifier spec);
s32    specifiers_has          (CodeSpecifiers specifiers, Specifier spec);
s32    specifiers_remove       (CodeSpecifiers specifiers, Specifier to_remove );
StrBuilder specifiers_to_string    (CodeSpecifiers specifiers);
void   specifiers_to_strbuilder_ref(CodeSpecifiers specifiers, StrBuilder* result);

Specifier* begin_CodeSpecifiers(CodeSpecifiers specifiers);
Specifier* end_CodeSpecifiers  (CodeSpecifiers specifiers);
Specifier* next_CodeSpecifiers (CodeSpecifiers specifiers, Specifier* spec_iter);

void   struct_add_interface(CodeStruct self, CodeTypename interface);
StrBuilder struct_to_string    (CodeStruct self);
void   struct_to_strbuilder_fwd(CodeStruct self, StrBuilder* result);
void   struct_to_strbuilder_def(CodeStruct self, StrBuilder* result);

StrBuilder attributes_to_string    (CodeAttributes attributes);
void   attributes_to_strbuilder_ref(CodeAttributes attributes, StrBuilder* result);

StrBuilder comment_to_string    (CodeComment comment );
void   comment_to_strbuilder_ref(CodeComment comment, StrBuilder* result );

StrBuilder constructor_to_string    (CodeConstructor constructor);
void   constructor_to_strbuilder_def(CodeConstructor constructor, StrBuilder* result );
void   constructor_to_strbuilder_fwd(CodeConstructor constructor, StrBuilder* result );

StrBuilder define_to_string    (CodeDefine self);
void   define_to_strbuilder_ref(CodeDefine self, StrBuilder* result);

StrBuilder destructor_to_string    (CodeDestructor destructor);
void   destructor_to_strbuilder_def(CodeDestructor destructor, StrBuilder* result );
void   destructor_to_strbuilder_fwd(CodeDestructor destructor, StrBuilder* result );

StrBuilder enum_to_string          (CodeEnum self);
void   enum_to_strbuilder_def      (CodeEnum self, StrBuilder* result );
void   enum_to_strbuilder_fwd      (CodeEnum self, StrBuilder* result );
void   enum_to_strbuilder_class_def(CodeEnum self, StrBuilder* result );
void   enum_to_strbuilder_class_fwd(CodeEnum self, StrBuilder* result );

StrBuilder exec_to_string    (CodeExec exec);
void   exec_to_strbuilder_ref(CodeExec exec, StrBuilder* result);

void extern_to_string(CodeExtern self, StrBuilder* result);

StrBuilder include_to_string    (CodeInclude self);
void   include_to_strbuilder_ref(CodeInclude self, StrBuilder* result);

StrBuilder friend_to_string     (CodeFriend self);
void   friend_to_strbuilder_ref(CodeFriend self, StrBuilder* result);

StrBuilder fn_to_string    (CodeFn self);
void   fn_to_strbuilder_def(CodeFn self, StrBuilder* result);
void   fn_to_strbuilder_fwd(CodeFn self, StrBuilder* result);

StrBuilder module_to_string    (CodeModule self);
void   module_to_strbuilder_ref(CodeModule self, StrBuilder* result);

StrBuilder namespace_to_string    (CodeNS self);
void   namespace_to_strbuilder_ref(CodeNS self, StrBuilder* result);

StrBuilder code_op_to_string    (CodeOperator self);
void   code_op_to_strbuilder_fwd(CodeOperator self, StrBuilder* result );
void   code_op_to_strbuilder_def(CodeOperator self, StrBuilder* result );

StrBuilder opcast_to_string     (CodeOpCast op_cast );
void   opcast_to_strbuilder_def(CodeOpCast op_cast, StrBuilder* result );
void   opcast_to_strbuilder_fwd(CodeOpCast op_cast, StrBuilder* result );

StrBuilder pragma_to_string    (CodePragma self);
void   pragma_to_strbuilder_ref(CodePragma self, StrBuilder* result);

StrBuilder preprocess_to_string       (CodePreprocessCond cond);
void   preprocess_to_strbuilder_if    (CodePreprocessCond cond, StrBuilder* result );
void   preprocess_to_strbuilder_ifdef (CodePreprocessCond cond, StrBuilder* result );
void   preprocess_to_strbuilder_ifndef(CodePreprocessCond cond, StrBuilder* result );
void   preprocess_to_strbuilder_elif  (CodePreprocessCond cond, StrBuilder* result );
void   preprocess_to_strbuilder_else  (CodePreprocessCond cond, StrBuilder* result );
void   preprocess_to_strbuilder_endif (CodePreprocessCond cond, StrBuilder* result );

StrBuilder template_to_string    (CodeTemplate self);
void   template_to_strbuilder_ref(CodeTemplate self, StrBuilder* result);

StrBuilder typename_to_string    (CodeTypename self);
void   typename_to_strbuilder_ref(CodeTypename self, StrBuilder* result);

StrBuilder typedef_to_string    (CodeTypedef self);
void   typedef_to_strbuilder_ref(CodeTypedef self, StrBuilder* result );

StrBuilder union_to_string    (CodeUnion self);
void   union_to_strbuilder_def(CodeUnion self, StrBuilder* result);
void   union_to_strbuilder_fwd(CodeUnion self, StrBuilder* result);

StrBuilder using_to_string    (CodeUsing op_cast );
void   using_to_strbuilder_ref(CodeUsing op_cast, StrBuilder* result );
void   using_to_strbuilder_ns (CodeUsing op_cast, StrBuilder* result );

StrBuilder var_to_string    (CodeVar self);
void   var_to_strbuilder_ref(CodeVar self, StrBuilder* result);

#pragma endregion Code Type C-Interface

#if GEN_COMPILER_CPP
#pragma region Code Types C++

// These structs are not used at all by the C vairant.
static_assert( GEN_COMPILER_CPP, "This should not be compiled with the C-library" );

#define Verify_POD(Type) static_assert(size_of(Code##Type) == size_of(AST_##Type), "ERROR: Code##Type is not a POD")

struct CodeBody
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeBody );
	forceinline void   append( Code other )               { return body_append( *this, other ); }
	forceinline void   append( CodeBody body )            { return body_append(*this, body); }
	forceinline bool   has_entries()                      { return code_has_entries(* this); }
	forceinline StrBuilder to_string()                        { return body_to_string(* this); }
	forceinline void   to_string( StrBuilder& result )        { return body_to_strbuilder_ref(* this, & result ); }
	forceinline void   to_strbuilder_export( StrBuilder& result ) { return body_to_strbuilder_export(* this, & result); }

#endif
	forceinline Code begin() { return begin_CodeBody(* this); }
	forceinline Code end()   { return end_CodeBody(* this); }
	Using_CodeOps( CodeBody );
	forceinline operator Code() { return * rcast( Code*, this ); }
	forceinline AST_Body* operator->() { return ast; }
	AST_Body* ast;
};

struct CodeClass
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeClass );
	forceinline void   add_interface( CodeType interface );
	forceinline StrBuilder to_string();
	forceinline void   to_strbuilder_def( StrBuilder& result );
	forceinline void   to_strbuilder_fwd( StrBuilder& result );
#endif
	Using_CodeOps( CodeClass );
	forceinline operator Code() { return * rcast( Code*, this ); }
	forceinline AST_Class* operator->() {
		GEN_ASSERT(ast);
		return ast;
	}
	AST_Class* ast;
};

struct CodeParams
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeParams );
	forceinline void      append( CodeParams other );
	forceinline CodeParams get( s32 idx );
	forceinline bool      has_entries();
	forceinline StrBuilder    to_string();
	forceinline void      to_string( StrBuilder& result );

	forceinline CodeParams begin() { return begin_CodeParams(* this); }
	forceinline CodeParams end()   { return end_CodeParams(* this); }
#endif
	Using_CodeOps( CodeParams );
	forceinline operator Code()       { return { (AST*)ast }; }
	forceinline CodeParams operator*() { return * this; }
	forceinline AST_Params* operator->() {
		GEN_ASSERT(ast);
		return ast;
	}
	CodeParams& operator++();
	AST_Params* ast;
};

struct CodeSpecifiers
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeSpecifiers );
	bool   append( Specifier spec )       { return specifiers_append(* this, spec); }
	s32    has( Specifier spec )          { return specifiers_has(* this, spec); }
	s32    remove( Specifier to_remove )  { return specifiers_remove(* this, to_remove); }
	StrBuilder to_string()                    { return specifiers_to_string(* this ); }
	void   to_string( StrBuilder& result )    { return specifiers_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps(CodeSpecifiers);
	forceinline operator Code() { return { (AST*) ast }; }
	forceinline AST_Specifiers* operator->() {
		GEN_ASSERT(ast);
		return ast;
	}
	AST_Specifiers* ast;
};

struct CodeAttributes
{
#if ! GEN_C_LIKE_CPP
	Using_Code(CodeAttributes);
	forceinline StrBuilder to_string()               { return attributes_to_string(* this); }
	forceinline void   to_string(StrBuilder& result) { return attributes_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps(CodeAttributes);
	operator Code();
	AST_Attributes *operator->();
	AST_Attributes *ast;
};

// Define_CodeType( BaseClass );

struct CodeComment
{
#if ! GEN_C_LIKE_CPP
	Using_Code(CodeComment);
	forceinline StrBuilder to_string()               { return comment_to_string    (* this); }
	forceinline void   to_string(StrBuilder& result) { return comment_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps(CodeComment);
	operator Code();
	AST_Comment *operator->();
	AST_Comment *ast;
};

struct CodeConstructor
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeConstructor );
	forceinline StrBuilder to_string()                     { return constructor_to_string(* this); }
	forceinline void   to_strbuilder_def( StrBuilder& result ) { return constructor_to_strbuilder_def(* this, & result); }
	forceinline void   to_strbuilder_fwd( StrBuilder& result ) { return constructor_to_strbuilder_fwd(* this, & result); }
#endif
	Using_CodeOps(CodeConstructor);
	operator         Code();
	AST_Constructor* operator->();
	AST_Constructor* ast;
};

struct CodeDefine
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeDefine );
	forceinline StrBuilder to_string()                 { return define_to_string(* this); }
	forceinline void   to_string( StrBuilder& result ) { return define_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps(CodeDefine);
	operator    Code();
	AST_Define* operator->();
	AST_Define* ast;
};

struct CodeDestructor
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeDestructor );
	forceinline StrBuilder to_string()                     { return destructor_to_string(* this); }
	forceinline void   to_strbuilder_def( StrBuilder& result ) { return destructor_to_strbuilder_def(* this, & result); }
	forceinline void   to_strbuilder_fwd( StrBuilder& result ) { return destructor_to_strbuilder_fwd(* this, & result); }
#endif
	Using_CodeOps(CodeDestructor);
	operator         Code();
	AST_Destructor* operator->();
	AST_Destructor* ast;
};

struct CodeEnum
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeEnum );
	forceinline StrBuilder to_string()                            { return enum_to_string(* this); }
	forceinline void   to_strbuilder_def( StrBuilder& result )        { return enum_to_strbuilder_def(* this, & result); }
	forceinline void   to_strbuilder_fwd( StrBuilder& result )        { return enum_to_strbuilder_fwd(* this, & result); }
	forceinline void   to_strbuilder_class_def( StrBuilder& result )  { return enum_to_strbuilder_class_def(* this, & result); }
	forceinline void   to_strbuilder_class_fwd( StrBuilder& result )  { return enum_to_strbuilder_class_fwd(* this, & result); }
#endif
	Using_CodeOps(CodeEnum);
	operator  Code();
	AST_Enum* operator->();
	AST_Enum* ast;
};

struct CodeExec
{
#if ! GEN_C_LIKE_CPP
	Using_Code(CodeExec);
	forceinline StrBuilder to_string()               { return exec_to_string(* this); }
	forceinline void   to_string(StrBuilder& result) { return exec_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps(CodeExec);
	operator Code();
	AST_Exec *operator->();
	AST_Exec *ast;
};

#if GEN_EXECUTION_EXPRESSION_SUPPORT
struct CodeExpr
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr );
	forceinline void to_string( StrBuilder& result );
#endif
	operator  Code();
	AST_Expr* operator->();
	AST_Expr* ast;
};

struct CodeExpr_Assign
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_Assign );
	forceinline void to_string( StrBuilder& result );
#endif
	operator         Code();
	AST_Expr_Assign* operator->();
	AST_Expr_Assign* ast;
};

struct CodeExpr_Alignof
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_Alignof );
	forceinline void to_string( StrBuilder& result );
#endif
	operator          Code();
	AST_Expr_Alignof* operator->();
	AST_Expr_Alignof* ast;
};

struct CodeExpr_Binary
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_Binary );
	forceinline void to_string( StrBuilder& result );
#endif
	operator         Code();
	AST_Expr_Binary* operator->();
	AST_Expr_Binary* ast;
};

struct CodeExpr_CStyleCast
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_CStyleCast );
	forceinline void to_string( StrBuilder& result );
#endif
	operator             Code();
	AST_Expr_CStyleCast* operator->();
	AST_Expr_CStyleCast* ast;
};

struct CodeExpr_FunctionalCast
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_FunctionalCast );
	forceinline void to_string( StrBuilder& result );
#endif
	operator                 Code();
	AST_Expr_FunctionalCast* operator->();
	AST_Expr_FunctionalCast* ast;
};

struct CodeExpr_CppCast
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_CppCast );
	forceinline void to_string( StrBuilder& result );
#endif
	operator          Code();
	AST_Expr_CppCast* operator->();
	AST_Expr_CppCast* ast;
};

struct CodeExpr_Element
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_Element );
	forceinline void to_string( StrBuilder& result );
#endif
	operator          Code();
	AST_Expr_Element* operator->();
	AST_Expr_Element* ast;
};

struct CodeExpr_ProcCall
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_ProcCall );
	forceinline void to_string( StrBuilder& result );
#endif
	operator           Code();
	AST_Expr_ProcCall* operator->();
	AST_Expr_ProcCall* ast;
};

struct CodeExpr_Decltype
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_Decltype );
	forceinline void to_string( StrBuilder& result );
#endif
	operator  Code();
	AST_Expr_Decltype* operator->();
	AST_Expr_Decltype* ast;
};

struct CodeExpr_Comma
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_Comma );
	forceinline void to_string( StrBuilder& result );
#endif
	operator  Code();
	AST_Expr_Comma* operator->();
	AST_Expr_Comma* ast;
};

struct CodeExpr_AMS
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_AMS );
	forceinline void to_string( StrBuilder& result );
#endif
	operator      Code();
	AST_Expr_AMS* operator->();
	AST_Expr_AMS* ast;
};

struct CodeExpr_Sizeof
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_Sizeof );
	forceinline void to_string( StrBuilder& result );
#endif
	operator         Code();
	AST_Expr_Sizeof* operator->();
	AST_Expr_Sizeof* ast;
};

struct CodeExpr_Subscript
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_Subscript );
	forceinline void to_string( StrBuilder& result );
#endif
	operator            Code();
	AST_Expr_Subscript* operator->();
	AST_Expr_Subscript* ast;
};

struct CodeExpr_Ternary
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_Ternary );
	forceinline void to_string( StrBuilder& result );
#endif
	operator          Code();
	AST_Expr_Ternary* operator->();
	AST_Expr_Ternary* ast;
};

struct CodeExpr_UnaryPrefix
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_UnaryPrefix );
	forceinline void to_string( StrBuilder& result );
#endif
	operator              Code();
	AST_Expr_UnaryPrefix* operator->();
	AST_Expr_UnaryPrefix* ast;
};

struct CodeExpr_UnaryPostfix
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExpr_UnaryPostfix );
	forceinline void to_string( StrBuilder& result );
#endif
	AST*                   raw();
	operator               Code();
	AST_Expr_UnaryPostfix* operator->();
	AST_Expr_UnaryPostfix* ast;
};
#endif

struct CodeExtern
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeExtern );
	forceinline void to_string( StrBuilder& result ) { return extern_to_string(* this, & result); }
#endif
	Using_CodeOps(CodeExtern);
	operator    Code();
	AST_Extern* operator->();
	AST_Extern* ast;
};

struct CodeInclude
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeInclude );
	StrBuilder to_string()                  { return include_to_string(* this); }
	forceinline void   to_string( StrBuilder& result )  { return include_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps(CodeInclude);
	operator     Code();
	AST_Include* operator->();
	AST_Include* ast;
};

struct CodeFriend
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeFriend );
	forceinline StrBuilder to_string()                 { return friend_to_string(* this); }
	forceinline void   to_string( StrBuilder& result ) { return friend_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps(CodeFriend);
	operator    Code();
	AST_Friend* operator->();
	AST_Friend* ast;
};

struct CodeFn
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeFn );
	forceinline StrBuilder to_string()                     { return fn_to_string(* this); }
	forceinline void   to_strbuilder_def( StrBuilder& result ) { return fn_to_strbuilder_def(* this, & result); }
	forceinline void   to_strbuilder_fwd( StrBuilder& result ) { return fn_to_strbuilder_fwd(* this, & result); }
#endif
	Using_CodeOps(CodeFn);
	operator Code();
	AST_Fn*  operator->();
	AST_Fn*  ast;
};

struct CodeModule
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeModule );
	forceinline StrBuilder to_string()                 { return module_to_string(* this); }
	forceinline void   to_string( StrBuilder& result ) { return module_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps(CodeModule);
	operator    Code();
	AST_Module* operator->();
	AST_Module* ast;
};

struct CodeNS
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeNS );
	forceinline StrBuilder to_string()                 { return namespace_to_string(* this); }
	forceinline void    to_string( StrBuilder& result ) { return namespace_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps(CodeNS);
	operator Code();
	AST_NS*  operator->();
	AST_NS*  ast;
};

struct CodeOperator
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeOperator );
	forceinline StrBuilder to_string()                     { return code_op_to_string(* this); }
	forceinline void   to_strbuilder_def( StrBuilder& result ) { return code_op_to_strbuilder_def(* this, & result); }
	forceinline void   to_strbuilder_fwd( StrBuilder& result ) { return code_op_to_strbuilder_fwd(* this, & result); }
#endif
	Using_CodeOps(CodeOperator);
	operator      Code();
	AST_Operator* operator->();
	AST_Operator* ast;
};

struct CodeOpCast
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeOpCast );
	forceinline StrBuilder to_string()                     { return opcast_to_string(* this); }
	forceinline void   to_strbuilder_def( StrBuilder& result ) { return opcast_to_strbuilder_def(* this, & result); }
	forceinline void   to_strbuilder_fwd( StrBuilder& result ) { return opcast_to_strbuilder_fwd(* this, & result); }
#endif
	Using_CodeOps(CodeOpCast);
	operator    Code();
	AST_OpCast* operator->();
	AST_OpCast* ast;
};

struct CodePragma
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodePragma );
	forceinline StrBuilder to_string()                 { return pragma_to_string(* this); }
	forceinline void   to_string( StrBuilder& result ) { return pragma_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps( CodePragma );
	operator    Code();
	AST_Pragma* operator->();
	AST_Pragma* ast;
};

struct CodePreprocessCond
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodePreprocessCond );
	forceinline StrBuilder to_string()                        { return preprocess_to_string(* this); }
	forceinline void   to_strbuilder_if( StrBuilder& result )     { return preprocess_to_strbuilder_if(* this, & result); }
	forceinline void   to_strbuilder_ifdef( StrBuilder& result )  { return preprocess_to_strbuilder_ifdef(* this, & result); }
	forceinline void   to_strbuilder_ifndef( StrBuilder& result ) { return preprocess_to_strbuilder_ifndef(* this, & result); }
	forceinline void   to_strbuilder_elif( StrBuilder& result )   { return preprocess_to_strbuilder_elif(* this, & result); }
	forceinline void   to_strbuilder_else( StrBuilder& result )   { return preprocess_to_strbuilder_else(* this, & result); }
	forceinline void   to_strbuilder_endif( StrBuilder& result )  { return preprocess_to_strbuilder_endif(* this, & result); }
#endif
	Using_CodeOps( CodePreprocessCond );
	operator            Code();
	AST_PreprocessCond* operator->();
	AST_PreprocessCond* ast;
};

#if GEN_EXECUTION_EXPRESSION_SUPPORT
struct CodeStmt
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator  Code();
	AST_Stmt* operator->();
	AST_Stmt* ast;
};

struct CodeStmt_Break
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_Break );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator        Code();
	AST_Stmt_Break* operator->();
	AST_Stmt_Break* ast;
};

struct CodeStmt_Case
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_Case );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator       Code();
	AST_Stmt_Case* operator->();
	AST_Stmt_Case* ast;
};

struct CodeStmt_Continue
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_Continue );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator           Code();
	AST_Stmt_Continue* operator->();
	AST_Stmt_Continue* ast;
};

struct CodeStmt_Decl
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_Decl );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator       Code();
	AST_Stmt_Decl* operator->();
	AST_Stmt_Decl* ast;
};

struct CodeStmt_Do
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_Do );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator     Code();
	AST_Stmt_Do* operator->();
	AST_Stmt_Do* ast;
};

struct CodeStmt_Expr
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_Expr );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator       Code();
	AST_Stmt_Expr* operator->();
	AST_Stmt_Expr* ast;
};

struct CodeStmt_Else
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_Else );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator       Code();
	AST_Stmt_Else* operator->();
	AST_Stmt_Else* ast;
};

struct CodeStmt_If
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_If );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator     Code();
	AST_Stmt_If* operator->();
	AST_Stmt_If* ast;
};

struct CodeStmt_For
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_For );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator      Code();
	AST_Stmt_For* operator->();
	AST_Stmt_For* ast;
};

struct CodeStmt_Goto
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_Goto );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator       Code();
	AST_Stmt_Goto* operator->();
	AST_Stmt_Goto* ast;
};

struct CodeStmt_Label
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_Label );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator        Code();
	AST_Stmt_Label* operator->();
	AST_Stmt_Label* ast;
};

struct CodeStmt_Switch
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_Switch );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator       Code();
	AST_Stmt_Switch* operator->();
	AST_Stmt_Switch* ast;
};

struct CodeStmt_While
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStmt_While );
	forceinline StrBuilder to_string();
	forceinline void   to_string( StrBuilder& result );
#endif
	operator       Code();
	AST_Stmt_While* operator->();
	AST_Stmt_While* ast;
};
#endif

struct CodeTemplate
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeTemplate );
	forceinline StrBuilder to_string()                 { return template_to_string(* this); }
	forceinline void   to_string( StrBuilder& result ) { return template_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps( CodeTemplate );
	operator      Code();
	AST_Template* operator->();
	AST_Template* ast;
};

struct CodeTypename
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeTypename );
	forceinline StrBuilder to_string()                 { return typename_to_string(* this); }
	forceinline void   to_string( StrBuilder& result ) { return typename_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps( CodeTypename );
	operator      Code();
	AST_Typename* operator->();
	AST_Typename* ast;
};

struct CodeTypedef
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeTypedef );
	forceinline StrBuilder to_string()                 { return typedef_to_string(* this); }
	forceinline void   to_string( StrBuilder& result ) { return typedef_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps( CodeTypedef );
	operator     Code();
	AST_Typedef* operator->();
	AST_Typedef* ast;
};

struct CodeUnion
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeUnion );
	forceinline StrBuilder to_string()                     { return union_to_string(* this); }
	forceinline void   to_strbuilder_def( StrBuilder& result ) { return union_to_strbuilder_def(* this, & result); }
	forceinline void   to_strbuilder_fwd( StrBuilder& result ) { return union_to_strbuilder_fwd(* this, & result); }
#endif
	Using_CodeOps(CodeUnion);
	operator   Code();
	AST_Union* operator->();
	AST_Union* ast;
};

struct CodeUsing
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeUsing );
	forceinline StrBuilder to_string()                    { return using_to_string(* this); }
	forceinline void   to_string( StrBuilder& result )    { return using_to_strbuilder_ref(* this, & result); }
	forceinline void   to_strbuilder_ns( StrBuilder& result ) { return using_to_strbuilder_ns(* this, & result); }
#endif
	Using_CodeOps(CodeUsing);
	operator   Code();
	AST_Using* operator->();
	AST_Using* ast;
};

struct CodeVar
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeVar );
	forceinline StrBuilder to_string()                 { return var_to_string(* this); }
	forceinline void   to_string( StrBuilder& result ) { return var_to_strbuilder_ref(* this, & result); }
#endif
	Using_CodeOps(CodeVar);
	operator Code();
	AST_Var* operator->();
	AST_Var* ast;
};

struct CodeStruct
{
#if ! GEN_C_LIKE_CPP
	Using_Code( CodeStruct );
	forceinline void   add_interface( CodeTypename interface ) { return struct_add_interface(* this, interface); }
	forceinline StrBuilder to_string()                             { return struct_to_string(* this); }
	forceinline void   to_strbuilder_fwd( StrBuilder& result )         { return struct_to_strbuilder_fwd(* this, & result); }
	forceinline void   to_strbuilder_def( StrBuilder& result )         { return struct_to_strbuilder_def(* this, & result); }
#endif
	Using_CodeOps( CodeStruct );
	forceinline operator Code() { return * rcast( Code*, this ); }
	forceinline AST_Struct* operator->() {
		GEN_ASSERT(ast);
		return ast;
	}
	AST_Struct* ast;
};

#undef Define_CodeType
#undef Using_Code
#undef Using_CodeOps

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
    operator CodeParams        () const { return cast(CodeParams,          Code_Invalid); }
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

struct NullCode_ImplicitCaster
{
    operator Code              () const { return {nullptr}; }
    operator CodeBody          () const { return {(AST_Body*)      nullptr}; }
    operator CodeAttributes    () const { return {(AST_Attributes*)nullptr}; }
    operator CodeComment       () const { return {nullptr}; }
    operator CodeClass         () const { return {nullptr}; }
    operator CodeConstructor   () const { return {nullptr}; }
    operator CodeDefine        () const { return {nullptr}; }
    operator CodeDestructor    () const { return {nullptr}; }
    operator CodeExec          () const { return {nullptr}; }
    operator CodeEnum          () const { return {nullptr}; }
    operator CodeExtern        () const { return {nullptr}; }
    operator CodeInclude       () const { return {nullptr}; }
    operator CodeFriend        () const { return {nullptr}; }
    operator CodeFn            () const { return {nullptr}; }
    operator CodeModule        () const { return {nullptr}; }
    operator CodeNS            () const { return {nullptr}; }
    operator CodeOperator      () const { return {nullptr}; }
    operator CodeOpCast        () const { return {nullptr}; }
    operator CodeParams        () const { return {nullptr}; }
    operator CodePragma        () const { return {nullptr}; }
    operator CodePreprocessCond() const { return {nullptr}; }
    operator CodeSpecifiers    () const { return {nullptr}; }
    operator CodeStruct        () const { return {nullptr}; }
    operator CodeTemplate      () const { return {nullptr}; }
    operator CodeTypename      () const { return CodeTypename{(AST_Typename*)nullptr}; }
    operator CodeTypedef       () const { return {nullptr}; }
    operator CodeUnion         () const { return {nullptr}; }
    operator CodeUsing         () const { return {nullptr}; }
    operator CodeVar           () const { return {nullptr}; }
};

#if ! GEN_C_LIKE_CPP
GEN_OPTIMIZE_MAPPINGS_BEGIN

forceinline void   append          ( CodeBody body, Code     other ) { return body_append(body, other); }
forceinline void   append          ( CodeBody body, CodeBody other ) { return body_append_body(body, other); }
forceinline StrBuilder to_string       ( CodeBody body )                 { return body_to_string(body); }
forceinline void   to_string       ( CodeBody body, StrBuilder& result ) { return body_to_strbuilder_ref(body, & result); }
forceinline void   to_strbuilder_export( CodeBody body, StrBuilder& result ) { return body_to_strbuilder_export(body, & result); }

forceinline Code begin( CodeBody body)                   { return begin_CodeBody(body); }
forceinline Code end  ( CodeBody body )                  { return end_CodeBody(body); }
forceinline Code next ( CodeBody body, Code entry_iter ) { return next_CodeBody(body, entry_iter); }

forceinline void   add_interface( CodeClass self, CodeTypename interface ) { return class_add_interface(self, interface); }
forceinline StrBuilder to_string    ( CodeClass self )                         { return class_to_string(self); }
forceinline void   to_strbuilder_def( CodeClass self, StrBuilder& result )         { return class_to_strbuilder_def(self, & result); }
forceinline void   to_strbuilder_fwd( CodeClass self, StrBuilder& result )         { return class_to_strbuilder_fwd(self, & result); }

forceinline void      append     (CodeParams params, CodeParams param ) { return params_append(params, param); }
forceinline CodeParams get        (CodeParams params, s32 idx)          { return params_get(params, idx); }
forceinline bool      has_entries(CodeParams params )                   { return params_has_entries(params); }
forceinline StrBuilder    to_string  (CodeParams params )                   { return params_to_string(params); }
forceinline void      to_string  (CodeParams params, StrBuilder& result )   { return params_to_strbuilder_ref(params, & result); }
  
forceinline CodeParams begin(CodeParams params)                        { return begin_CodeParams(params); }
forceinline CodeParams end  (CodeParams params)                        { return end_CodeParams(params); }
forceinline CodeParams next (CodeParams params, CodeParams entry_iter) { return next_CodeParams(params, entry_iter); }

forceinline bool   append   (CodeSpecifiers specifiers, Specifier spec)       { return specifiers_append(specifiers, spec); }
forceinline s32    has      (CodeSpecifiers specifiers, Specifier spec)       { return specifiers_has(specifiers, spec); }
forceinline s32    remove   (CodeSpecifiers specifiers, Specifier to_remove ) { return specifiers_remove(specifiers, to_remove); }
forceinline StrBuilder to_string(CodeSpecifiers specifiers)                       { return specifiers_to_string(specifiers); }
forceinline void   to_string(CodeSpecifiers specifiers, StrBuilder& result)       { return specifiers_to_strbuilder_ref(specifiers, & result);  }

forceinline Specifier* begin(CodeSpecifiers specifiers)                       { return begin_CodeSpecifiers(specifiers); }
forceinline Specifier* end  (CodeSpecifiers specifiers)                       { return end_CodeSpecifiers(specifiers); }
forceinline Specifier* next (CodeSpecifiers specifiers, Specifier& spec_iter) { return next_CodeSpecifiers(specifiers, & spec_iter); }

forceinline void   add_interface(CodeStruct self, CodeTypename interface) { return struct_add_interface(self, interface); }
forceinline StrBuilder to_string    (CodeStruct self)                         { return struct_to_string(self); }
forceinline void   to_strbuilder_fwd(CodeStruct self, StrBuilder& result)         { return struct_to_strbuilder_fwd(self, & result); }
forceinline void   to_strbuilder_def(CodeStruct self, StrBuilder& result)         { return struct_to_strbuilder_def(self, & result); }

forceinline StrBuilder to_string(CodeAttributes attributes)                 { return attributes_to_string(attributes); }
forceinline void   to_string(CodeAttributes attributes, StrBuilder& result) { return attributes_to_strbuilder_ref(attributes, & result); }

forceinline StrBuilder to_string(CodeComment comment )                  { return comment_to_string(comment); }
forceinline void   to_string(CodeComment comment, StrBuilder& result )  { return comment_to_strbuilder_ref(comment, & result); }

forceinline StrBuilder to_string    (CodeConstructor constructor)                  { return constructor_to_string(constructor); }
forceinline void   to_strbuilder_def(CodeConstructor constructor, StrBuilder& result ) { return constructor_to_strbuilder_def(constructor, & result); }
forceinline void   to_strbuilder_fwd(CodeConstructor constructor, StrBuilder& result ) { return constructor_to_strbuilder_fwd(constructor, & result); }

forceinline StrBuilder to_string(CodeDefine self)                 { return define_to_string(self); }
forceinline void   to_string(CodeDefine self, StrBuilder& result) { return define_to_strbuilder_ref(self, & result); }

forceinline StrBuilder to_string    (CodeDestructor destructor)                  { return destructor_to_string(destructor); }
forceinline void   to_strbuilder_def(CodeDestructor destructor, StrBuilder& result ) { return destructor_to_strbuilder_def(destructor, & result); }
forceinline void   to_strbuilder_fwd(CodeDestructor destructor, StrBuilder& result ) { return destructor_to_strbuilder_fwd(destructor, & result); }

forceinline StrBuilder to_string          (CodeEnum self)                  { return enum_to_string(self); }
forceinline void   to_strbuilder_def      (CodeEnum self, StrBuilder& result ) { return enum_to_strbuilder_def(self, & result); }
forceinline void   to_strbuilder_fwd      (CodeEnum self, StrBuilder& result ) { return enum_to_strbuilder_fwd(self, & result); }
forceinline void   to_strbuilder_class_def(CodeEnum self, StrBuilder& result ) { return enum_to_strbuilder_class_def(self, & result); }
forceinline void   to_strbuilder_class_fwd(CodeEnum self, StrBuilder& result ) { return enum_to_strbuilder_class_fwd(self, & result); }

forceinline StrBuilder to_string(CodeExec exec)                 { return exec_to_string(exec); }
forceinline void   to_string(CodeExec exec, StrBuilder& result) { return exec_to_strbuilder_ref(exec, & result); }

forceinline void to_string(CodeExtern self, StrBuilder& result) { return extern_to_string(self, & result); }

forceinline StrBuilder to_string(CodeInclude self)                 { return include_to_string(self); }
forceinline void   to_string(CodeInclude self, StrBuilder& result) { return include_to_strbuilder_ref(self, & result); }

forceinline StrBuilder to_string(CodeFriend self)                 { return friend_to_string(self); }
forceinline void   to_string(CodeFriend self, StrBuilder& result) { return friend_to_strbuilder_ref(self, & result); }

forceinline StrBuilder to_string    (CodeFn self)                 { return fn_to_string(self); }
forceinline void   to_strbuilder_def(CodeFn self, StrBuilder& result) { return fn_to_strbuilder_def(self, & result); }
forceinline void   to_strbuilder_fwd(CodeFn self, StrBuilder& result) { return fn_to_strbuilder_fwd(self, & result); }

forceinline StrBuilder to_string(CodeModule self)                 { return module_to_string(self); }
forceinline void   to_string(CodeModule self, StrBuilder& result) { return module_to_strbuilder_ref(self, & result); }

forceinline StrBuilder to_string(CodeNS self)                 { return namespace_to_string(self); }
forceinline void   to_string(CodeNS self, StrBuilder& result) { return namespace_to_strbuilder_ref(self,  & result); }

forceinline StrBuilder to_string    (CodeOperator self)                  { return code_op_to_string(self); }
forceinline void   to_strbuilder_fwd(CodeOperator self, StrBuilder& result ) { return code_op_to_strbuilder_fwd(self, & result); }
forceinline void   to_strbuilder_def(CodeOperator self, StrBuilder& result ) { return code_op_to_strbuilder_def(self, & result); }

forceinline StrBuilder to_string    (CodeOpCast op_cast )                 { return opcast_to_string(op_cast); }
forceinline void   to_strbuilder_def(CodeOpCast op_cast, StrBuilder& result ) { return opcast_to_strbuilder_def(op_cast, & result); }
forceinline void   to_strbuilder_fwd(CodeOpCast op_cast, StrBuilder& result ) { return opcast_to_strbuilder_fwd(op_cast, & result); }

forceinline StrBuilder to_string(CodePragma self)                 { return pragma_to_string(self); }
forceinline void   to_string(CodePragma self, StrBuilder& result) { return pragma_to_strbuilder_ref(self, & result); }

forceinline StrBuilder to_string       (CodePreprocessCond cond)                  { return preprocess_to_string(cond); }
forceinline void   to_strbuilder_if    (CodePreprocessCond cond, StrBuilder& result ) { return preprocess_to_strbuilder_if(cond, & result); }
forceinline void   to_strbuilder_ifdef (CodePreprocessCond cond, StrBuilder& result ) { return preprocess_to_strbuilder_ifdef(cond, & result); }
forceinline void   to_strbuilder_ifndef(CodePreprocessCond cond, StrBuilder& result ) { return preprocess_to_strbuilder_ifndef(cond, & result); }
forceinline void   to_strbuilder_elif  (CodePreprocessCond cond, StrBuilder& result ) { return preprocess_to_strbuilder_elif(cond, & result); }
forceinline void   to_strbuilder_else  (CodePreprocessCond cond, StrBuilder& result ) { return preprocess_to_strbuilder_else(cond, & result); }
forceinline void   to_strbuilder_endif (CodePreprocessCond cond, StrBuilder& result ) { return preprocess_to_strbuilder_endif(cond, & result); }

forceinline StrBuilder to_string(CodeTemplate self)                 { return template_to_string(self); }
forceinline void   to_string(CodeTemplate self, StrBuilder& result) { return template_to_strbuilder_ref(self, & result); }

forceinline StrBuilder to_string(CodeTypename self)                 { return typename_to_string(self); }
forceinline void   to_string(CodeTypename self, StrBuilder& result) { return typename_to_strbuilder_ref(self, & result); }

forceinline StrBuilder to_string(CodeTypedef self)                  { return typedef_to_string(self); }
forceinline void   to_string(CodeTypedef self, StrBuilder& result ) { return typedef_to_strbuilder_ref(self, & result); }

forceinline StrBuilder to_string    (CodeUnion self)                 { return union_to_string(self); }
forceinline void   to_strbuilder_def(CodeUnion self, StrBuilder& result) { return union_to_strbuilder_def(self, & result); }
forceinline void   to_strbuilder_fwd(CodeUnion self, StrBuilder& result) { return union_to_strbuilder_fwd(self, & result); }

forceinline StrBuilder to_string   (CodeUsing op_cast )                 { return using_to_string(op_cast); }
forceinline void   to_string   (CodeUsing op_cast, StrBuilder& result ) { return using_to_strbuilder_ref(op_cast, & result); }
forceinline void   to_strbuilder_ns(CodeUsing op_cast, StrBuilder& result ) { return using_to_strbuilder_ns(op_cast, & result); }

forceinline StrBuilder to_string(CodeVar self)                 { return var_to_string(self); }
forceinline void   to_string(CodeVar self, StrBuilder& result) { return var_to_strbuilder_ref(self, & result); }

GEN_OPITMIZE_MAPPINGS_END
#endif //if GEN_C_LIKE_CPP

#pragma endregion Code Types C++
#endif //if GEN_COMPILER_CPP
