#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "components/types.hpp"
#endif

// This file was generated automatially by gencpp's bootstrap.cpp (See: https://github.com/Ed94/gencpp)

#pragma region generated code inline implementation

inline char const* Code::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code Code::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool Code::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool Code::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void Code::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
}

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

inline bool Code::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool Code::operator!=( Code other )
{
	return (AST*)ast != other.ast;
}

inline Code::operator bool()
{
	return ast != nullptr;
}

inline char const* CodeBody::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeBody::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeBody::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeBody::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeBody::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeBody::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeBody::operator!=( Code other )
{
	return (AST*)ast != other.ast;
}

inline CodeBody::operator bool()
{
	return ast != nullptr;
}

inline char const* CodeAttributes::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeAttributes::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeAttributes::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeAttributes::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeAttributes::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeAttributes::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeAttributes::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeComment::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeComment::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeComment::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeComment::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeComment::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeComment::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeComment::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeConstructor::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeConstructor::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeConstructor::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeConstructor::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeConstructor::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeConstructor::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeConstructor::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeClass::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeClass::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeClass::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeClass::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeClass::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeClass::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeClass::operator!=( Code other )
{
	return (AST*)ast != other.ast;
}

inline CodeClass::operator bool()
{
	return ast != nullptr;
}

inline char const* CodeDefine::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeDefine::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeDefine::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeDefine::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeDefine::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeDefine::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeDefine::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeDestructor::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeDestructor::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeDestructor::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeDestructor::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeDestructor::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeDestructor::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeDestructor::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeEnum::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeEnum::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeEnum::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeEnum::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeEnum::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeEnum::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeEnum::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeExec::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeExec::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeExec::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeExec::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeExec::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeExec::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeExec::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeExtern::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeExtern::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeExtern::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeExtern::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeExtern::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeExtern::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeExtern::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeFriend::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeFriend::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeFriend::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeFriend::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeFriend::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeFriend::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeFriend::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeFn::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeFn::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeFn::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeFn::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeFn::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeFn::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeFn::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeInclude::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeInclude::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeInclude::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeInclude::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeInclude::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeInclude::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeInclude::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeModule::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeModule::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeModule::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeModule::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeModule::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeModule::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeModule::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeNS::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeNS::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeNS::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeNS::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeNS::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeNS::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeNS::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeOperator::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeOperator::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeOperator::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeOperator::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeOperator::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeOperator::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeOperator::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeOpCast::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeOpCast::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeOpCast::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeOpCast::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeOpCast::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeOpCast::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeOpCast::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeParam::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeParam::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeParam::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeParam::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeParam::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeParam::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeParam::operator!=( Code other )
{
	return (AST*)ast != other.ast;
}

inline CodeParam::operator bool()
{
	return ast != nullptr;
}

inline char const* CodePragma::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodePragma::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodePragma::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodePragma::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodePragma::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodePragma::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodePragma::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodePreprocessCond::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodePreprocessCond::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodePreprocessCond::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodePreprocessCond::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodePreprocessCond::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodePreprocessCond::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodePreprocessCond::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeSpecifiers::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeSpecifiers::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeSpecifiers::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeSpecifiers::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeSpecifiers::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeSpecifiers::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeSpecifiers::operator!=( Code other )
{
	return (AST*)ast != other.ast;
}

inline CodeSpecifiers::operator bool()
{
	return ast != nullptr;
}

inline char const* CodeStruct::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeStruct::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeStruct::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeStruct::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeStruct::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeStruct::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeStruct::operator!=( Code other )
{
	return (AST*)ast != other.ast;
}

inline CodeStruct::operator bool()
{
	return ast != nullptr;
}

inline char const* CodeTemplate::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeTemplate::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeTemplate::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeTemplate::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeTemplate::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeTemplate::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeTemplate::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeType::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeType::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeType::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeType::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeType::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeType::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeType::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeTypedef::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeTypedef::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeTypedef::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeTypedef::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeTypedef::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeTypedef::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeTypedef::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeUnion::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeUnion::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeUnion::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeUnion::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeUnion::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeUnion::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeUnion::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeUsing::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeUsing::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeUsing::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeUsing::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeUsing::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeUsing::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeUsing::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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

inline char const* CodeVar::debug_str()
{
	if ( ast == nullptr )
		return "Code::debug_str: AST is null!";
	return rcast( AST*, ast )->debug_str();
}

inline Code CodeVar::duplicate()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::duplicate: Cannot duplicate code, AST is null!" );
		return Code::Invalid;
	}
	return { rcast( AST*, ast )->duplicate() };
}

inline bool CodeVar::is_equal( Code other )
{
	if ( ast == nullptr || other.ast == nullptr )
	{
		return ast == nullptr && other.ast == nullptr;
	}
	return rcast( AST*, ast )->is_equal( other.ast );
}

inline bool CodeVar::is_valid()
{
	return (AST*)ast != nullptr && rcast( AST*, ast )->Type != CodeT::Invalid;
}

inline void CodeVar::set_global()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::set_global: Cannot set code as global, AST is null!" );
		return;
	}
	rcast( AST*, ast )->Parent = Code::Global.ast;
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

inline bool CodeVar::operator==( Code other )
{
	return (AST*)ast == other.ast;
}

inline bool CodeVar::operator!=( Code other )
{
	return (AST*)ast != other.ast;
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
