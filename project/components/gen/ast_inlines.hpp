// This file was generated automatially by gen.bootstrap.cpp (See: https://github.com/Ed94/gencpp)

#pragma once
#pragma region generated code inline implementation

char const* Code::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code Code::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool Code::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool Code::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void Code::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String Code::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

Code& Code::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool Code::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool Code::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

Code::operator bool()
{
	return ast != nullptr;
}

char const* CodeBody::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeBody::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeBody::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeBody::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeBody::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeBody::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeBody& CodeBody::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeBody::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeBody::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeBody::operator bool()
{
	return ast != nullptr;
}

char const* CodeAttributes::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeAttributes::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeAttributes::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeAttributes::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeAttributes::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeAttributes::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeAttributes& CodeAttributes::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeAttributes::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeAttributes::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeAttributes::operator bool()
{
	return ast != nullptr;
}

AST* CodeAttributes::raw( void )
{
	return rcast( AST*, ast );
}

CodeAttributes::operator Code()
{
	return *rcast( Code*, this );
}

AST_Attributes* CodeAttributes::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeComment::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeComment::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeComment::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeComment::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeComment::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeComment::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeComment& CodeComment::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeComment::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeComment::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeComment::operator bool()
{
	return ast != nullptr;
}

AST* CodeComment::raw( void )
{
	return rcast( AST*, ast );
}

CodeComment::operator Code()
{
	return *rcast( Code*, this );
}

AST_Comment* CodeComment::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeConstructor::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeConstructor::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeConstructor::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeConstructor::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeConstructor::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeConstructor::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeConstructor& CodeConstructor::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeConstructor::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeConstructor::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeConstructor::operator bool()
{
	return ast != nullptr;
}

AST* CodeConstructor::raw( void )
{
	return rcast( AST*, ast );
}

CodeConstructor::operator Code()
{
	return *rcast( Code*, this );
}

AST_Constructor* CodeConstructor::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeClass::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeClass::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeClass::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeClass::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeClass::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeClass::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeClass& CodeClass::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeClass::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeClass::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeClass::operator bool()
{
	return ast != nullptr;
}

char const* CodeDefine::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeDefine::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeDefine::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeDefine::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeDefine::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeDefine::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeDefine& CodeDefine::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeDefine::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeDefine::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeDefine::operator bool()
{
	return ast != nullptr;
}

AST* CodeDefine::raw( void )
{
	return rcast( AST*, ast );
}

CodeDefine::operator Code()
{
	return *rcast( Code*, this );
}

AST_Define* CodeDefine::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeDestructor::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeDestructor::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeDestructor::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeDestructor::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeDestructor::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeDestructor::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeDestructor& CodeDestructor::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeDestructor::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeDestructor::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeDestructor::operator bool()
{
	return ast != nullptr;
}

AST* CodeDestructor::raw( void )
{
	return rcast( AST*, ast );
}

CodeDestructor::operator Code()
{
	return *rcast( Code*, this );
}

AST_Destructor* CodeDestructor::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeEnum::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeEnum::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeEnum::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeEnum::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeEnum::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeEnum::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeEnum& CodeEnum::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeEnum::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeEnum::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeEnum::operator bool()
{
	return ast != nullptr;
}

AST* CodeEnum::raw( void )
{
	return rcast( AST*, ast );
}

CodeEnum::operator Code()
{
	return *rcast( Code*, this );
}

AST_Enum* CodeEnum::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeExec::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeExec::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeExec::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeExec::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeExec::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeExec::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeExec& CodeExec::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeExec::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeExec::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeExec::operator bool()
{
	return ast != nullptr;
}

AST* CodeExec::raw( void )
{
	return rcast( AST*, ast );
}

CodeExec::operator Code()
{
	return *rcast( Code*, this );
}

AST_Exec* CodeExec::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeExtern::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeExtern::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeExtern::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeExtern::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeExtern::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeExtern::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeExtern& CodeExtern::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeExtern::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeExtern::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeExtern::operator bool()
{
	return ast != nullptr;
}

AST* CodeExtern::raw( void )
{
	return rcast( AST*, ast );
}

CodeExtern::operator Code()
{
	return *rcast( Code*, this );
}

AST_Extern* CodeExtern::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeFriend::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeFriend::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeFriend::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeFriend::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeFriend::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeFriend::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeFriend& CodeFriend::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeFriend::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeFriend::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeFriend::operator bool()
{
	return ast != nullptr;
}

AST* CodeFriend::raw( void )
{
	return rcast( AST*, ast );
}

CodeFriend::operator Code()
{
	return *rcast( Code*, this );
}

AST_Friend* CodeFriend::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeFn::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeFn::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeFn::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeFn::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeFn::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeFn::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeFn& CodeFn::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeFn::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeFn::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeFn::operator bool()
{
	return ast != nullptr;
}

AST* CodeFn::raw( void )
{
	return rcast( AST*, ast );
}

CodeFn::operator Code()
{
	return *rcast( Code*, this );
}

AST_Fn* CodeFn::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeInclude::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeInclude::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeInclude::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeInclude::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeInclude::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeInclude::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeInclude& CodeInclude::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeInclude::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeInclude::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeInclude::operator bool()
{
	return ast != nullptr;
}

AST* CodeInclude::raw( void )
{
	return rcast( AST*, ast );
}

CodeInclude::operator Code()
{
	return *rcast( Code*, this );
}

AST_Include* CodeInclude::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeModule::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeModule::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeModule::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeModule::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeModule::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeModule::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeModule& CodeModule::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeModule::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeModule::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeModule::operator bool()
{
	return ast != nullptr;
}

AST* CodeModule::raw( void )
{
	return rcast( AST*, ast );
}

CodeModule::operator Code()
{
	return *rcast( Code*, this );
}

AST_Module* CodeModule::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeNS::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeNS::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeNS::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeNS::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeNS::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeNS::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeNS& CodeNS::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeNS::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeNS::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeNS::operator bool()
{
	return ast != nullptr;
}

AST* CodeNS::raw( void )
{
	return rcast( AST*, ast );
}

CodeNS::operator Code()
{
	return *rcast( Code*, this );
}

AST_NS* CodeNS::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeOperator::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeOperator::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeOperator::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeOperator::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeOperator::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeOperator::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeOperator& CodeOperator::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeOperator::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeOperator::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeOperator::operator bool()
{
	return ast != nullptr;
}

AST* CodeOperator::raw( void )
{
	return rcast( AST*, ast );
}

CodeOperator::operator Code()
{
	return *rcast( Code*, this );
}

AST_Operator* CodeOperator::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeOpCast::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeOpCast::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeOpCast::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeOpCast::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeOpCast::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeOpCast::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeOpCast& CodeOpCast::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeOpCast::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeOpCast::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeOpCast::operator bool()
{
	return ast != nullptr;
}

AST* CodeOpCast::raw( void )
{
	return rcast( AST*, ast );
}

CodeOpCast::operator Code()
{
	return *rcast( Code*, this );
}

AST_OpCast* CodeOpCast::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeParam::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeParam::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeParam::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeParam::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeParam::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeParam::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeParam& CodeParam::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeParam::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeParam::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeParam::operator bool()
{
	return ast != nullptr;
}

char const* CodePragma::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodePragma::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodePragma::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodePragma::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodePragma::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodePragma::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodePragma& CodePragma::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodePragma::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodePragma::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodePragma::operator bool()
{
	return ast != nullptr;
}

AST* CodePragma::raw( void )
{
	return rcast( AST*, ast );
}

CodePragma::operator Code()
{
	return *rcast( Code*, this );
}

AST_Pragma* CodePragma::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodePreprocessCond::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodePreprocessCond::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodePreprocessCond::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodePreprocessCond::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodePreprocessCond::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodePreprocessCond::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodePreprocessCond& CodePreprocessCond::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodePreprocessCond::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodePreprocessCond::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodePreprocessCond::operator bool()
{
	return ast != nullptr;
}

AST* CodePreprocessCond::raw( void )
{
	return rcast( AST*, ast );
}

CodePreprocessCond::operator Code()
{
	return *rcast( Code*, this );
}

AST_PreprocessCond* CodePreprocessCond::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeSpecifiers::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeSpecifiers::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeSpecifiers::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeSpecifiers::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeSpecifiers::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeSpecifiers::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeSpecifiers& CodeSpecifiers::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeSpecifiers::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeSpecifiers::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeSpecifiers::operator bool()
{
	return ast != nullptr;
}

char const* CodeStruct::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeStruct::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeStruct::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeStruct::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeStruct::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeStruct::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeStruct& CodeStruct::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeStruct::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeStruct::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeStruct::operator bool()
{
	return ast != nullptr;
}

char const* CodeTemplate::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeTemplate::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeTemplate::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeTemplate::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeTemplate::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeTemplate::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeTemplate& CodeTemplate::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeTemplate::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeTemplate::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeTemplate::operator bool()
{
	return ast != nullptr;
}

AST* CodeTemplate::raw( void )
{
	return rcast( AST*, ast );
}

CodeTemplate::operator Code()
{
	return *rcast( Code*, this );
}

AST_Template* CodeTemplate::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeType::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeType::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeType::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeType::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeType::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeType::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeType& CodeType::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeType::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeType::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeType::operator bool()
{
	return ast != nullptr;
}

AST* CodeType::raw( void )
{
	return rcast( AST*, ast );
}

CodeType::operator Code()
{
	return *rcast( Code*, this );
}

AST_Type* CodeType::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeTypedef::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeTypedef::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeTypedef::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeTypedef::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeTypedef::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeTypedef::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeTypedef& CodeTypedef::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeTypedef::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeTypedef::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeTypedef::operator bool()
{
	return ast != nullptr;
}

AST* CodeTypedef::raw( void )
{
	return rcast( AST*, ast );
}

CodeTypedef::operator Code()
{
	return *rcast( Code*, this );
}

AST_Typedef* CodeTypedef::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeUnion::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeUnion::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeUnion::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeUnion::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeUnion::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeUnion::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeUnion& CodeUnion::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeUnion::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeUnion::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeUnion::operator bool()
{
	return ast != nullptr;
}

AST* CodeUnion::raw( void )
{
	return rcast( AST*, ast );
}

CodeUnion::operator Code()
{
	return *rcast( Code*, this );
}

AST_Union* CodeUnion::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeUsing::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeUsing::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeUsing::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeUsing::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeUsing::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeUsing::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeUsing& CodeUsing::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeUsing::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeUsing::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeUsing::operator bool()
{
	return ast != nullptr;
}

AST* CodeUsing::raw( void )
{
	return rcast( AST*, ast );
}

CodeUsing::operator Code()
{
	return *rcast( Code*, this );
}

AST_Using* CodeUsing::operator->( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Attempt to dereference a nullptr!" );
		return nullptr;
	}
	return ast;
}

char const* CodeVar::debug_str( void )
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

Code CodeVar::duplicate( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

bool CodeVar::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

bool CodeVar::is_valid( void )
{
	return ( AST* )ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

void CodeVar::set_global( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

String CodeVar::to_string( void )
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

CodeVar& CodeVar::operator=( Code other )
{
	if ( other.ast && other->Parent )
	{
		ast                        = rcast( decltype( ast ), other.ast->duplicate() );
		rcast( AST*, ast )->Parent = nullptr;
	}
	ast = rcast( decltype( ast ), other.ast );
	return *this;
}

bool CodeVar::operator==( Code other )
{
	return ( AST* )ast == other.ast;
}

bool CodeVar::operator!=( Code other )
{
	return ( AST* )ast != other.ast;
}

CodeVar::operator bool()
{
	return ast != nullptr;
}

AST* CodeVar::raw( void )
{
	return rcast( AST*, ast );
}

CodeVar::operator Code()
{
	return *rcast( Code*, this );
}

AST_Var* CodeVar::operator->( void )
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

AST::operator CodeBody()
{
	return { rcast( AST_Body*, this ) };
}

Code::operator CodeBody() const
{
	return { ( AST_Body* )ast };
}

AST::operator CodeAttributes()
{
	return { rcast( AST_Attributes*, this ) };
}

Code::operator CodeAttributes() const
{
	return { ( AST_Attributes* )ast };
}

AST::operator CodeComment()
{
	return { rcast( AST_Comment*, this ) };
}

Code::operator CodeComment() const
{
	return { ( AST_Comment* )ast };
}

AST::operator CodeConstructor()
{
	return { rcast( AST_Constructor*, this ) };
}

Code::operator CodeConstructor() const
{
	return { ( AST_Constructor* )ast };
}

AST::operator CodeClass()
{
	return { rcast( AST_Class*, this ) };
}

Code::operator CodeClass() const
{
	return { ( AST_Class* )ast };
}

AST::operator CodeDefine()
{
	return { rcast( AST_Define*, this ) };
}

Code::operator CodeDefine() const
{
	return { ( AST_Define* )ast };
}

AST::operator CodeDestructor()
{
	return { rcast( AST_Destructor*, this ) };
}

Code::operator CodeDestructor() const
{
	return { ( AST_Destructor* )ast };
}

AST::operator CodeEnum()
{
	return { rcast( AST_Enum*, this ) };
}

Code::operator CodeEnum() const
{
	return { ( AST_Enum* )ast };
}

AST::operator CodeExec()
{
	return { rcast( AST_Exec*, this ) };
}

Code::operator CodeExec() const
{
	return { ( AST_Exec* )ast };
}

AST::operator CodeExtern()
{
	return { rcast( AST_Extern*, this ) };
}

Code::operator CodeExtern() const
{
	return { ( AST_Extern* )ast };
}

AST::operator CodeFriend()
{
	return { rcast( AST_Friend*, this ) };
}

Code::operator CodeFriend() const
{
	return { ( AST_Friend* )ast };
}

AST::operator CodeFn()
{
	return { rcast( AST_Fn*, this ) };
}

Code::operator CodeFn() const
{
	return { ( AST_Fn* )ast };
}

AST::operator CodeInclude()
{
	return { rcast( AST_Include*, this ) };
}

Code::operator CodeInclude() const
{
	return { ( AST_Include* )ast };
}

AST::operator CodeModule()
{
	return { rcast( AST_Module*, this ) };
}

Code::operator CodeModule() const
{
	return { ( AST_Module* )ast };
}

AST::operator CodeNS()
{
	return { rcast( AST_NS*, this ) };
}

Code::operator CodeNS() const
{
	return { ( AST_NS* )ast };
}

AST::operator CodeOperator()
{
	return { rcast( AST_Operator*, this ) };
}

Code::operator CodeOperator() const
{
	return { ( AST_Operator* )ast };
}

AST::operator CodeOpCast()
{
	return { rcast( AST_OpCast*, this ) };
}

Code::operator CodeOpCast() const
{
	return { ( AST_OpCast* )ast };
}

AST::operator CodeParam()
{
	return { rcast( AST_Param*, this ) };
}

Code::operator CodeParam() const
{
	return { ( AST_Param* )ast };
}

AST::operator CodePragma()
{
	return { rcast( AST_Pragma*, this ) };
}

Code::operator CodePragma() const
{
	return { ( AST_Pragma* )ast };
}

AST::operator CodePreprocessCond()
{
	return { rcast( AST_PreprocessCond*, this ) };
}

Code::operator CodePreprocessCond() const
{
	return { ( AST_PreprocessCond* )ast };
}

AST::operator CodeSpecifiers()
{
	return { rcast( AST_Specifiers*, this ) };
}

Code::operator CodeSpecifiers() const
{
	return { ( AST_Specifiers* )ast };
}

AST::operator CodeStruct()
{
	return { rcast( AST_Struct*, this ) };
}

Code::operator CodeStruct() const
{
	return { ( AST_Struct* )ast };
}

AST::operator CodeTemplate()
{
	return { rcast( AST_Template*, this ) };
}

Code::operator CodeTemplate() const
{
	return { ( AST_Template* )ast };
}

AST::operator CodeType()
{
	return { rcast( AST_Type*, this ) };
}

Code::operator CodeType() const
{
	return { ( AST_Type* )ast };
}

AST::operator CodeTypedef()
{
	return { rcast( AST_Typedef*, this ) };
}

Code::operator CodeTypedef() const
{
	return { ( AST_Typedef* )ast };
}

AST::operator CodeUnion()
{
	return { rcast( AST_Union*, this ) };
}

Code::operator CodeUnion() const
{
	return { ( AST_Union* )ast };
}

AST::operator CodeUsing()
{
	return { rcast( AST_Using*, this ) };
}

Code::operator CodeUsing() const
{
	return { ( AST_Using* )ast };
}

AST::operator CodeVar()
{
	return { rcast( AST_Var*, this ) };
}

Code::operator CodeVar() const
{
	return { ( AST_Var* )ast };
}

#pragma endregion generated AST / Code cast implementation
