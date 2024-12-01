#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "ast.hpp"
#endif

#pragma region Code Types

struct CodeBody
{
	Using_Code( CodeBody );

	void append( Code other )
	{
		GEN_ASSERT(other.ast != nullptr);

		if (other.is_body()) {
			append( cast(CodeBody, & other) );
		}

		GEN_NS append( raw(), other.ast );
	}
	void append( CodeBody body )
	{
		for ( Code entry : body ) {
			append( entry );
		}
	}
	bool has_entries() { return GEN_NS has_entries(rcast( AST*, ast )); }
	AST* raw()         { return rcast( AST*, ast ); }

	void to_string( String& result );
	void to_string_export( String& result );

	AST_Body* operator->() { return ast; }

	operator Code() { return * rcast( Code*, this ); }

#pragma region Iterator
	Code begin()
	{
		if ( ast )
			return { rcast( AST*, ast)->Front };

		return { nullptr };
	}
	Code end()
	{
		return { rcast(AST*, ast)->Back->Next };
	}
#pragma endregion Iterator

	AST_Body* ast;
};

struct CodeClass
{
	Using_Code( CodeClass );

	void add_interface( CodeType interface );

	void to_string_def( String& result );
	void to_string_fwd( String& result );

	AST* raw()
	{
		return rcast( AST*, ast );
	}
	operator Code()
	{
		return * rcast( Code*, this );
	}
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
	Using_Code( CodeParam );

	void append( CodeParam other );

	CodeParam get( s32 idx );
	bool has_entries();
	void to_string( String& result );
	AST* raw()
	{
		return rcast( AST*, ast );
	}
	AST_Param* operator->()
	{
		if ( ast == nullptr )
		{
			log_failure("Attempt to dereference a nullptr!");
			return nullptr;
		}
		return ast;
	}
	operator Code()
	{
		return { (AST*)ast };
	}
#pragma region Iterator
	CodeParam begin()
	{
		if ( ast )
			return { ast };

		return { nullptr };
	}
	CodeParam end()
	{
		// return { (AST_Param*) rcast( AST*, ast)->Last };
		return { nullptr };
	}
	CodeParam& operator++();
	CodeParam operator*()
	{
		return * this;
	}
#pragma endregion Iterator

	AST_Param* ast;
};

struct CodeSpecifiers
{
	Using_Code( CodeSpecifiers );

	bool append( SpecifierT spec )
	{
		if ( ast == nullptr )
		{
			log_failure("CodeSpecifiers: Attempted to append to a null specifiers AST!");
			return false;
		}

		if ( raw()->NumEntries == AST::ArrSpecs_Cap )
		{
			log_failure("CodeSpecifiers: Attempted to append over %d specifiers to a specifiers AST!", AST::ArrSpecs_Cap );
			return false;
		}

		raw()->ArrSpecs[ raw()->NumEntries ] = spec;
		raw()->NumEntries++;
		return true;
	}
	s32 has( SpecifierT spec )
	{
		for ( s32 idx = 0; idx < raw()->NumEntries; idx++ )
		{
			if ( raw()->ArrSpecs[ idx ] == spec )
				return idx;
		}

		return -1;
	}
	s32 remove( SpecifierT to_remove )
	{
		if ( ast == nullptr )
		{
			log_failure("CodeSpecifiers: Attempted to append to a null specifiers AST!");
			return -1;
		}

		if ( raw()->NumEntries == AST::ArrSpecs_Cap )
		{
			log_failure("CodeSpecifiers: Attempted to append over %d specifiers to a specifiers AST!", AST::ArrSpecs_Cap );
			return -1;
		}

		s32 result = -1;

		s32 curr = 0;
		s32 next = 0;
		for(; next < raw()->NumEntries; ++ curr, ++ next)
		{
			SpecifierT spec = raw()->ArrSpecs[next];
			if (spec == to_remove)
			{
				result = next;

				next ++;
				if (next >= raw()->NumEntries)
					break;

				spec = raw()->ArrSpecs[next];
			}

			raw()->ArrSpecs[ curr ] = spec;
		}

		if (result > -1) {
			raw()->NumEntries --;
		}
		return result;
	}
	void to_string( String& result );
	AST* raw()
	{
		return rcast( AST*, ast );
	}
	AST_Specifiers* operator->()
	{
		if ( ast == nullptr )
		{
			log_failure("Attempt to dereference a nullptr!");
			return nullptr;
		}
		return ast;
	}
	operator Code()
	{
		return { (AST*) ast };
	}
#pragma region Iterator
	SpecifierT* begin()
	{
		if ( ast )
			return & raw()->ArrSpecs[0];

		return nullptr;
	}
	SpecifierT* end()
	{
		return raw()->ArrSpecs + raw()->NumEntries;
	}
#pragma endregion Iterator

	AST_Specifiers* ast;
};

struct CodeStruct
{
	Using_Code( CodeStruct );

	void add_interface( CodeType interface );

	void to_string_def( String& result );
	void to_string_fwd( String& result );

	AST* raw()
	{
		return rcast( AST*, ast );
	}
	operator Code()
	{
		return * rcast( Code*, this );
	}
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

#define Define_CodeType( Typename )     \
	struct Code##Typename               \
	{                                   \
		Using_Code( Code ## Typename ); \
		AST*            raw();          \
		operator        Code();         \
		AST_##Typename* operator->();   \
		AST_##Typename* ast;            \
	}

Define_CodeType( Attributes );
// Define_CodeType( BaseClass );
Define_CodeType( Comment );

struct CodeConstructor
{
	Using_Code( CodeConstructor );

	void to_string_def( String& result );
	void to_string_fwd( String& result );

	AST*             raw();
	operator         Code();
	AST_Constructor* operator->();
	AST_Constructor* ast;
};

struct CodeDefine
{
	Using_Code( CodeDefine );

	void to_string( String& result );

	AST*        raw();
	operator    Code();
	AST_Define* operator->();
	AST_Define* ast;
};

struct CodeDestructor
{
	Using_Code( CodeDestructor );

	void to_string_def( String& result );
	void to_string_fwd( String& result );

	AST*             raw();
	operator         Code();
	AST_Destructor* operator->();
	AST_Destructor* ast;
};

struct CodeEnum
{
	Using_Code( CodeEnum );

	void to_string_def( String& result );
	void to_string_fwd( String& result );
	void to_string_class_def( String& result );
	void to_string_class_fwd( String& result );

	AST*      raw();
	operator  Code();
	AST_Enum* operator->();
	AST_Enum* ast;
};

Define_CodeType( Exec );

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
	Using_Code( CodeExtern );

	void to_string( String& result );

	AST*        raw();
	operator    Code();
	AST_Extern* operator->();
	AST_Extern* ast;
};

struct CodeInclude
{
	Using_Code( CodeInclude );

	void to_string( String& result );

	AST*         raw();
	operator     Code();
	AST_Include* operator->();
	AST_Include* ast;
};

struct CodeFriend
{
	Using_Code( CodeFriend );

	void to_string( String& result );

	AST*        raw();
	operator    Code();
	AST_Friend* operator->();
	AST_Friend* ast;
};

struct CodeFn
{
	Using_Code( CodeFn );

	void to_string_def( String& result );
	void to_string_fwd( String& result );

	AST*     raw();
	operator Code();
	AST_Fn*  operator->();
	AST_Fn*  ast;
};

struct CodeModule
{
	Using_Code( CodeModule );

	void to_string( String& result );

	AST*        raw();
	operator    Code();
	AST_Module* operator->();
	AST_Module* ast;
};

struct CodeNS
{
	Using_Code( CodeNS );

	void to_string( String& result );

	AST*     raw();
	operator Code();
	AST_NS*  operator->();
	AST_NS*  ast;
};

struct CodeOperator
{
	Using_Code( CodeOperator );

	void to_string_def( String& result );
	void to_string_fwd( String& result );

	AST*          raw();
	operator      Code();
	AST_Operator* operator->();
	AST_Operator* ast;
};

struct CodeOpCast
{
	Using_Code( CodeOpCast );

	void to_string_def( String& result );
	void to_string_fwd( String& result );

	AST*        raw();
	operator    Code();
	AST_OpCast* operator->();
	AST_OpCast* ast;
};

struct CodePragma
{
	Using_Code( CodePragma );

	void to_string( String& result );

	AST*        raw();
	operator    Code();
	AST_Pragma* operator->();
	AST_Pragma* ast;
};

struct CodePreprocessCond
{
	Using_Code( CodePreprocessCond );

	void to_string_if( String& result );
	void to_string_ifdef( String& result );
	void to_string_ifndef( String& result );
	void to_string_elif( String& result );
	void to_string_else( String& result );
	void to_string_endif( String& result );

	AST*                raw();
	operator            Code();
	AST_PreprocessCond* operator->();
	AST_PreprocessCond* ast;
};

#if GEN_EXECUTION_EXPRESSION_SUPPORT
struct CodeStmt
{
	Using_Code( CodeStmt );

	void to_string( String& result );

	AST*      raw();
	operator  Code();
	AST_Stmt* operator->();
	AST_Stmt* ast;
};

struct CodeStmt_Break
{
	Using_Code( CodeStmt_Break );

	void to_string( String& result );

	AST*            raw();
	operator        Code();
	AST_Stmt_Break* operator->();
	AST_Stmt_Break* ast;
};

struct CodeStmt_Case
{
	Using_Code( CodeStmt_Case );

	void to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_Case* operator->();
	AST_Stmt_Case* ast;
};

struct CodeStmt_Continue
{
	Using_Code( CodeStmt_Continue );

	void to_string( String& result );

	AST*               raw();
	operator           Code();
	AST_Stmt_Continue* operator->();
	AST_Stmt_Continue* ast;
};

struct CodeStmt_Decl
{
	Using_Code( CodeStmt_Decl );

	void to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_Decl* operator->();
	AST_Stmt_Decl* ast;
};

struct CodeStmt_Do
{
	Using_Code( CodeStmt_Do );

	void to_string( String& result );

	AST*         raw();
	operator     Code();
	AST_Stmt_Do* operator->();
	AST_Stmt_Do* ast;
};

struct CodeStmt_Expr
{
	Using_Code( CodeStmt_Expr );

	void to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_Expr* operator->();
	AST_Stmt_Expr* ast;
};

struct CodeStmt_Else
{
	Using_Code( CodeStmt_Else );

	void to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_Else* operator->();
	AST_Stmt_Else* ast;
};

struct CodeStmt_If
{
	Using_Code( CodeStmt_If );

	void to_string( String& result );

	AST*         raw();
	operator     Code();
	AST_Stmt_If* operator->();
	AST_Stmt_If* ast;
};

struct CodeStmt_For
{
	Using_Code( CodeStmt_For );

	void to_string( String& result );

	AST*          raw();
	operator      Code();
	AST_Stmt_For* operator->();
	AST_Stmt_For* ast;
};

struct CodeStmt_Goto
{
	Using_Code( CodeStmt_Goto );

	void to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_Goto* operator->();
	AST_Stmt_Goto* ast;
};

struct CodeStmt_Label
{
	Using_Code( CodeStmt_Label );

	void to_string( String& result );

	AST*            raw();
	operator        Code();
	AST_Stmt_Label* operator->();
	AST_Stmt_Label* ast;
};

struct CodeStmt_Switch
{
	Using_Code( CodeStmt_Switch );

	void to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_Switch* operator->();
	AST_Stmt_Switch* ast;
};

struct CodeStmt_While
{
	Using_Code( CodeStmt_While );

	void to_string( String& result );

	AST*           raw();
	operator       Code();
	AST_Stmt_While* operator->();
	AST_Stmt_While* ast;
};
#endif

struct CodeTemplate
{
	Using_Code( CodeTemplate );

	void to_string( String& result );

	AST*          raw();
	operator      Code();
	AST_Template* operator->();
	AST_Template* ast;
};

struct CodeType
{
	Using_Code( CodeType );

	void to_string( String& result );

	AST*      raw();
	operator  Code();
	AST_Type* operator->();
	AST_Type* ast;
};

struct CodeTypedef
{
	Using_Code( CodeTypedef );

	void to_string( String& result );

	AST*         raw();
	operator     Code();
	AST_Typedef* operator->();
	AST_Typedef* ast;
};

struct CodeUnion
{
	Using_Code( CodeUnion );

	void to_string( String& result );

	AST*       raw();
	operator   Code();
	AST_Union* operator->();
	AST_Union* ast;
};

struct CodeUsing
{
	Using_Code( CodeUsing );

	void to_string( String& result );
	void to_string_ns( String& result );

	AST*       raw();
	operator   Code();
	AST_Using* operator->();
	AST_Using* ast;
};

struct CodeVar
{
	Using_Code( CodeVar );

	void to_string( String& result );

	AST*     raw();
	operator Code();
	AST_Var* operator->();
	AST_Var* ast;
};

#undef Define_CodeType
#undef Using_Code

#pragma endregion Code Types
