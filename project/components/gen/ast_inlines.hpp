#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "components/types.hpp"
#endif

// This file was generated automatially by gencpp's bootstrap.cpp (See: https://github.com/Ed94/gencpp)

#pragma region generated code inline implementation

inline Code& Code::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline Code::operator bool()
{
	return ast != nullptr;
}

inline CodeBody& CodeBody::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeBody::operator bool()
{
	return ast != nullptr;
}

inline CodeAttributes& CodeAttributes::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeAttributes::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeAttributes::raw()
{
	return rcast( AST*, ast );
}

inline CodeAttributes::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Attributes* CodeAttributes::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeComment& CodeComment::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeComment::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeComment::raw()
{
	return rcast( AST*, ast );
}

inline CodeComment::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Comment* CodeComment::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeConstructor& CodeConstructor::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeConstructor::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeConstructor::raw()
{
	return rcast( AST*, ast );
}

inline CodeConstructor::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Constructor* CodeConstructor::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeClass& CodeClass::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeClass::operator bool()
{
	return ast != nullptr;
}

inline CodeDefine& CodeDefine::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeDefine::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeDefine::raw()
{
	return rcast( AST*, ast );
}

inline CodeDefine::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Define* CodeDefine::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeDestructor& CodeDestructor::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeDestructor::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeDestructor::raw()
{
	return rcast( AST*, ast );
}

inline CodeDestructor::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Destructor* CodeDestructor::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeEnum& CodeEnum::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeEnum::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeEnum::raw()
{
	return rcast( AST*, ast );
}

inline CodeEnum::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Enum* CodeEnum::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeExec& CodeExec::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeExec::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeExec::raw()
{
	return rcast( AST*, ast );
}

inline CodeExec::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Exec* CodeExec::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeExtern& CodeExtern::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeExtern::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeExtern::raw()
{
	return rcast( AST*, ast );
}

inline CodeExtern::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Extern* CodeExtern::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeFriend& CodeFriend::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeFriend::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeFriend::raw()
{
	return rcast( AST*, ast );
}

inline CodeFriend::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Friend* CodeFriend::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeFn& CodeFn::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeFn::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeFn::raw()
{
	return rcast( AST*, ast );
}

inline CodeFn::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Fn* CodeFn::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeInclude& CodeInclude::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeInclude::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeInclude::raw()
{
	return rcast( AST*, ast );
}

inline CodeInclude::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Include* CodeInclude::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeModule& CodeModule::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeModule::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeModule::raw()
{
	return rcast( AST*, ast );
}

inline CodeModule::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Module* CodeModule::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeNS& CodeNS::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeNS::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeNS::raw()
{
	return rcast( AST*, ast );
}

inline CodeNS::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_NS* CodeNS::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeOperator& CodeOperator::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeOperator::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeOperator::raw()
{
	return rcast( AST*, ast );
}

inline CodeOperator::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Operator* CodeOperator::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeOpCast& CodeOpCast::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeOpCast::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeOpCast::raw()
{
	return rcast( AST*, ast );
}

inline CodeOpCast::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_OpCast* CodeOpCast::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeParam& CodeParam::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeParam::operator bool()
{
	return ast != nullptr;
}

inline CodePragma& CodePragma::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodePragma::operator bool()
{
	return ast != nullptr;
}

inline AST* CodePragma::raw()
{
	return rcast( AST*, ast );
}

inline CodePragma::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Pragma* CodePragma::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodePreprocessCond& CodePreprocessCond::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodePreprocessCond::operator bool()
{
	return ast != nullptr;
}

inline AST* CodePreprocessCond::raw()
{
	return rcast( AST*, ast );
}

inline CodePreprocessCond::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_PreprocessCond* CodePreprocessCond::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeSpecifiers& CodeSpecifiers::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeSpecifiers::operator bool()
{
	return ast != nullptr;
}

inline CodeStruct& CodeStruct::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeStruct::operator bool()
{
	return ast != nullptr;
}

inline CodeTemplate& CodeTemplate::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeTemplate::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeTemplate::raw()
{
	return rcast( AST*, ast );
}

inline CodeTemplate::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Template* CodeTemplate::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeType& CodeType::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeType::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeType::raw()
{
	return rcast( AST*, ast );
}

inline CodeType::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Type* CodeType::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeTypedef& CodeTypedef::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeTypedef::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeTypedef::raw()
{
	return rcast( AST*, ast );
}

inline CodeTypedef::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Typedef* CodeTypedef::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeUnion& CodeUnion::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeUnion::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeUnion::raw()
{
	return rcast( AST*, ast );
}

inline CodeUnion::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Union* CodeUnion::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeUsing& CodeUsing::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeUsing::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeUsing::raw()
{
	return rcast( AST*, ast );
}

inline CodeUsing::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Using* CodeUsing::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

inline CodeVar& CodeVar::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeVar::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeVar::raw()
{
	return rcast( AST*, ast );
}

inline CodeVar::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Var* CodeVar::operator->()
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

#pragma endregion generated code inline implementation

#pragma region generated AST/Code cast implementation

inline AST::operator CodeBody()
{
	return { rcast( AST_Body*, this ) };
}

inline Code::operator CodeBody() const
{
	return { (AST_Body*)ast };
}

inline AST::operator CodeAttributes()
{
	return { rcast( AST_Attributes*, this ) };
}

inline Code::operator CodeAttributes() const
{
	return { (AST_Attributes*)ast };
}

inline AST::operator CodeComment()
{
	return { rcast( AST_Comment*, this ) };
}

inline Code::operator CodeComment() const
{
	return { (AST_Comment*)ast };
}

inline AST::operator CodeConstructor()
{
	return { rcast( AST_Constructor*, this ) };
}

inline Code::operator CodeConstructor() const
{
	return { (AST_Constructor*)ast };
}

inline AST::operator CodeClass()
{
	return { rcast( AST_Class*, this ) };
}

inline Code::operator CodeClass() const
{
	return { (AST_Class*)ast };
}

inline AST::operator CodeDefine()
{
	return { rcast( AST_Define*, this ) };
}

inline Code::operator CodeDefine() const
{
	return { (AST_Define*)ast };
}

inline AST::operator CodeDestructor()
{
	return { rcast( AST_Destructor*, this ) };
}

inline Code::operator CodeDestructor() const
{
	return { (AST_Destructor*)ast };
}

inline AST::operator CodeEnum()
{
	return { rcast( AST_Enum*, this ) };
}

inline Code::operator CodeEnum() const
{
	return { (AST_Enum*)ast };
}

inline AST::operator CodeExec()
{
	return { rcast( AST_Exec*, this ) };
}

inline Code::operator CodeExec() const
{
	return { (AST_Exec*)ast };
}

inline AST::operator CodeExtern()
{
	return { rcast( AST_Extern*, this ) };
}

inline Code::operator CodeExtern() const
{
	return { (AST_Extern*)ast };
}

inline AST::operator CodeFriend()
{
	return { rcast( AST_Friend*, this ) };
}

inline Code::operator CodeFriend() const
{
	return { (AST_Friend*)ast };
}

inline AST::operator CodeFn()
{
	return { rcast( AST_Fn*, this ) };
}

inline Code::operator CodeFn() const
{
	return { (AST_Fn*)ast };
}

inline AST::operator CodeInclude()
{
	return { rcast( AST_Include*, this ) };
}

inline Code::operator CodeInclude() const
{
	return { (AST_Include*)ast };
}

inline AST::operator CodeModule()
{
	return { rcast( AST_Module*, this ) };
}

inline Code::operator CodeModule() const
{
	return { (AST_Module*)ast };
}

inline AST::operator CodeNS()
{
	return { rcast( AST_NS*, this ) };
}

inline Code::operator CodeNS() const
{
	return { (AST_NS*)ast };
}

inline AST::operator CodeOperator()
{
	return { rcast( AST_Operator*, this ) };
}

inline Code::operator CodeOperator() const
{
	return { (AST_Operator*)ast };
}

inline AST::operator CodeOpCast()
{
	return { rcast( AST_OpCast*, this ) };
}

inline Code::operator CodeOpCast() const
{
	return { (AST_OpCast*)ast };
}

inline AST::operator CodeParam()
{
	return { rcast( AST_Param*, this ) };
}

inline Code::operator CodeParam() const
{
	return { (AST_Param*)ast };
}

inline AST::operator CodePragma()
{
	return { rcast( AST_Pragma*, this ) };
}

inline Code::operator CodePragma() const
{
	return { (AST_Pragma*)ast };
}

inline AST::operator CodePreprocessCond()
{
	return { rcast( AST_PreprocessCond*, this ) };
}

inline Code::operator CodePreprocessCond() const
{
	return { (AST_PreprocessCond*)ast };
}

inline AST::operator CodeSpecifiers()
{
	return { rcast( AST_Specifiers*, this ) };
}

inline Code::operator CodeSpecifiers() const
{
	return { (AST_Specifiers*)ast };
}

inline AST::operator CodeStruct()
{
	return { rcast( AST_Struct*, this ) };
}

inline Code::operator CodeStruct() const
{
	return { (AST_Struct*)ast };
}

inline AST::operator CodeTemplate()
{
	return { rcast( AST_Template*, this ) };
}

inline Code::operator CodeTemplate() const
{
	return { (AST_Template*)ast };
}

inline AST::operator CodeType()
{
	return { rcast( AST_Type*, this ) };
}

inline Code::operator CodeType() const
{
	return { (AST_Type*)ast };
}

inline AST::operator CodeTypedef()
{
	return { rcast( AST_Typedef*, this ) };
}

inline Code::operator CodeTypedef() const
{
	return { (AST_Typedef*)ast };
}

inline AST::operator CodeUnion()
{
	return { rcast( AST_Union*, this ) };
}

inline Code::operator CodeUnion() const
{
	return { (AST_Union*)ast };
}

inline AST::operator CodeUsing()
{
	return { rcast( AST_Using*, this ) };
}

inline Code::operator CodeUsing() const
{
	return { (AST_Using*)ast };
}

inline AST::operator CodeVar()
{
	return { rcast( AST_Var*, this ) };
}

inline Code::operator CodeVar() const
{
	return { (AST_Var*)ast };
}

#pragma endregion generated AST / Code cast implementation
