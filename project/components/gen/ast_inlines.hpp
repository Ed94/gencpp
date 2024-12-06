#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once #include "components/types.hpp"
#endif

// This file was generated automatially by gencpp's bootstrap.cpp (See: https://github.com/Ed94/gencpp)

#pragma region generated code inline implementation

inline Code& Code::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeAttributes::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeComment::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeConstructor::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeDefine::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeDestructor::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeEnum::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeExec::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeExtern::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeFriend::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeFn::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeInclude::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeModule::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeNS::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeOperator::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeOpCast::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodePragma::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodePreprocessCond::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeTemplate::operator bool()
{
	return ast != nullptr;
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

inline CodeTypename& CodeTypename::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeTypename::operator bool()
{
	return ast != nullptr;
}

inline CodeTypename::operator Code()
{
	return *rcast( Code*, this );
}

inline AST_Typename* CodeTypename::operator->()
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeTypedef::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeUnion::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeUsing::operator bool()
{
	return ast != nullptr;
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
		ast         = rcast( decltype( ast ), code_duplicate( other ).ast );
		ast->Parent = { nullptr };
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

inline CodeVar::operator bool()
{
	return ast != nullptr;
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

inline Code::operator CodeBody() const
{
	return { (AST_Body*)ast };
}

inline Code::operator CodeAttributes() const
{
	return { (AST_Attributes*)ast };
}

inline Code::operator CodeComment() const
{
	return { (AST_Comment*)ast };
}

inline Code::operator CodeConstructor() const
{
	return { (AST_Constructor*)ast };
}

inline Code::operator CodeClass() const
{
	return { (AST_Class*)ast };
}

inline Code::operator CodeDefine() const
{
	return { (AST_Define*)ast };
}

inline Code::operator CodeDestructor() const
{
	return { (AST_Destructor*)ast };
}

inline Code::operator CodeEnum() const
{
	return { (AST_Enum*)ast };
}

inline Code::operator CodeExec() const
{
	return { (AST_Exec*)ast };
}

inline Code::operator CodeExtern() const
{
	return { (AST_Extern*)ast };
}

inline Code::operator CodeFriend() const
{
	return { (AST_Friend*)ast };
}

inline Code::operator CodeFn() const
{
	return { (AST_Fn*)ast };
}

inline Code::operator CodeInclude() const
{
	return { (AST_Include*)ast };
}

inline Code::operator CodeModule() const
{
	return { (AST_Module*)ast };
}

inline Code::operator CodeNS() const
{
	return { (AST_NS*)ast };
}

inline Code::operator CodeOperator() const
{
	return { (AST_Operator*)ast };
}

inline Code::operator CodeOpCast() const
{
	return { (AST_OpCast*)ast };
}

inline Code::operator CodeParam() const
{
	return { (AST_Param*)ast };
}

inline Code::operator CodePragma() const
{
	return { (AST_Pragma*)ast };
}

inline Code::operator CodePreprocessCond() const
{
	return { (AST_PreprocessCond*)ast };
}

inline Code::operator CodeSpecifiers() const
{
	return { (AST_Specifiers*)ast };
}

inline Code::operator CodeStruct() const
{
	return { (AST_Struct*)ast };
}

inline Code::operator CodeTemplate() const
{
	return { (AST_Template*)ast };
}

inline Code::operator CodeTypename() const
{
	return { (AST_Typename*)ast };
}

inline Code::operator CodeTypedef() const
{
	return { (AST_Typedef*)ast };
}

inline Code::operator CodeUnion() const
{
	return { (AST_Union*)ast };
}

inline Code::operator CodeUsing() const
{
	return { (AST_Using*)ast };
}

inline Code::operator CodeVar() const
{
	return { (AST_Var*)ast };
}

#pragma endregion generated AST / Code cast implementation
