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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

Code::operator bool()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeBody::operator bool()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeAttributes::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeAttributes::raw()
{
	return rcast( AST*, ast );
}

CodeAttributes::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeComment::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeComment::raw()
{
	return rcast( AST*, ast );
}

CodeComment::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeConstructor::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeConstructor::raw()
{
	return rcast( AST*, ast );
}

CodeConstructor::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeClass::operator bool()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeDefine::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeDefine::raw()
{
	return rcast( AST*, ast );
}

CodeDefine::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeDestructor::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeDestructor::raw()
{
	return rcast( AST*, ast );
}

CodeDestructor::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeEnum::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeEnum::raw()
{
	return rcast( AST*, ast );
}

CodeEnum::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeExec::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeExec::raw()
{
	return rcast( AST*, ast );
}

CodeExec::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeExtern::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeExtern::raw()
{
	return rcast( AST*, ast );
}

CodeExtern::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeFriend::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeFriend::raw()
{
	return rcast( AST*, ast );
}

CodeFriend::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeFn::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeFn::raw()
{
	return rcast( AST*, ast );
}

CodeFn::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeInclude::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeInclude::raw()
{
	return rcast( AST*, ast );
}

CodeInclude::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeModule::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeModule::raw()
{
	return rcast( AST*, ast );
}

CodeModule::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeNS::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeNS::raw()
{
	return rcast( AST*, ast );
}

CodeNS::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeOperator::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeOperator::raw()
{
	return rcast( AST*, ast );
}

CodeOperator::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeOpCast::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeOpCast::raw()
{
	return rcast( AST*, ast );
}

CodeOpCast::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeParam::operator bool()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodePragma::operator bool()
{
	return ast != nullptr;
}

inline AST* CodePragma::raw()
{
	return rcast( AST*, ast );
}

CodePragma::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodePreprocessCond::operator bool()
{
	return ast != nullptr;
}

inline AST* CodePreprocessCond::raw()
{
	return rcast( AST*, ast );
}

CodePreprocessCond::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeSpecifiers::operator bool()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeStruct::operator bool()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeTemplate::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeTemplate::raw()
{
	return rcast( AST*, ast );
}

CodeTemplate::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeType::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeType::raw()
{
	return rcast( AST*, ast );
}

CodeType::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeTypedef::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeTypedef::raw()
{
	return rcast( AST*, ast );
}

CodeTypedef::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeUnion::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeUnion::raw()
{
	return rcast( AST*, ast );
}

CodeUnion::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeUsing::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeUsing::raw()
{
	return rcast( AST*, ast );
}

CodeUsing::operator Code()
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
		log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return false;
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

CodeVar::operator bool()
{
	return ast != nullptr;
}

inline AST* CodeVar::raw()
{
	return rcast( AST*, ast );
}

CodeVar::operator Code()
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

AST::operator CodeBody()
{
	return { rcast( AST_Body*, this ) };
}

Code::operator CodeBody() const
{
	return { (AST_Body*)ast };
}

AST::operator CodeAttributes()
{
	return { rcast( AST_Attributes*, this ) };
}

Code::operator CodeAttributes() const
{
	return { (AST_Attributes*)ast };
}

AST::operator CodeComment()
{
	return { rcast( AST_Comment*, this ) };
}

Code::operator CodeComment() const
{
	return { (AST_Comment*)ast };
}

AST::operator CodeConstructor()
{
	return { rcast( AST_Constructor*, this ) };
}

Code::operator CodeConstructor() const
{
	return { (AST_Constructor*)ast };
}

AST::operator CodeClass()
{
	return { rcast( AST_Class*, this ) };
}

Code::operator CodeClass() const
{
	return { (AST_Class*)ast };
}

AST::operator CodeDefine()
{
	return { rcast( AST_Define*, this ) };
}

Code::operator CodeDefine() const
{
	return { (AST_Define*)ast };
}

AST::operator CodeDestructor()
{
	return { rcast( AST_Destructor*, this ) };
}

Code::operator CodeDestructor() const
{
	return { (AST_Destructor*)ast };
}

AST::operator CodeEnum()
{
	return { rcast( AST_Enum*, this ) };
}

Code::operator CodeEnum() const
{
	return { (AST_Enum*)ast };
}

AST::operator CodeExec()
{
	return { rcast( AST_Exec*, this ) };
}

Code::operator CodeExec() const
{
	return { (AST_Exec*)ast };
}

AST::operator CodeExtern()
{
	return { rcast( AST_Extern*, this ) };
}

Code::operator CodeExtern() const
{
	return { (AST_Extern*)ast };
}

AST::operator CodeFriend()
{
	return { rcast( AST_Friend*, this ) };
}

Code::operator CodeFriend() const
{
	return { (AST_Friend*)ast };
}

AST::operator CodeFn()
{
	return { rcast( AST_Fn*, this ) };
}

Code::operator CodeFn() const
{
	return { (AST_Fn*)ast };
}

AST::operator CodeInclude()
{
	return { rcast( AST_Include*, this ) };
}

Code::operator CodeInclude() const
{
	return { (AST_Include*)ast };
}

AST::operator CodeModule()
{
	return { rcast( AST_Module*, this ) };
}

Code::operator CodeModule() const
{
	return { (AST_Module*)ast };
}

AST::operator CodeNS()
{
	return { rcast( AST_NS*, this ) };
}

Code::operator CodeNS() const
{
	return { (AST_NS*)ast };
}

AST::operator CodeOperator()
{
	return { rcast( AST_Operator*, this ) };
}

Code::operator CodeOperator() const
{
	return { (AST_Operator*)ast };
}

AST::operator CodeOpCast()
{
	return { rcast( AST_OpCast*, this ) };
}

Code::operator CodeOpCast() const
{
	return { (AST_OpCast*)ast };
}

AST::operator CodeParam()
{
	return { rcast( AST_Param*, this ) };
}

Code::operator CodeParam() const
{
	return { (AST_Param*)ast };
}

AST::operator CodePragma()
{
	return { rcast( AST_Pragma*, this ) };
}

Code::operator CodePragma() const
{
	return { (AST_Pragma*)ast };
}

AST::operator CodePreprocessCond()
{
	return { rcast( AST_PreprocessCond*, this ) };
}

Code::operator CodePreprocessCond() const
{
	return { (AST_PreprocessCond*)ast };
}

AST::operator CodeSpecifiers()
{
	return { rcast( AST_Specifiers*, this ) };
}

Code::operator CodeSpecifiers() const
{
	return { (AST_Specifiers*)ast };
}

AST::operator CodeStruct()
{
	return { rcast( AST_Struct*, this ) };
}

Code::operator CodeStruct() const
{
	return { (AST_Struct*)ast };
}

AST::operator CodeTemplate()
{
	return { rcast( AST_Template*, this ) };
}

Code::operator CodeTemplate() const
{
	return { (AST_Template*)ast };
}

AST::operator CodeType()
{
	return { rcast( AST_Type*, this ) };
}

Code::operator CodeType() const
{
	return { (AST_Type*)ast };
}

AST::operator CodeTypedef()
{
	return { rcast( AST_Typedef*, this ) };
}

Code::operator CodeTypedef() const
{
	return { (AST_Typedef*)ast };
}

AST::operator CodeUnion()
{
	return { rcast( AST_Union*, this ) };
}

Code::operator CodeUnion() const
{
	return { (AST_Union*)ast };
}

AST::operator CodeUsing()
{
	return { rcast( AST_Using*, this ) };
}

Code::operator CodeUsing() const
{
	return { (AST_Using*)ast };
}

AST::operator CodeVar()
{
	return { rcast( AST_Var*, this ) };
}

Code::operator CodeVar() const
{
	return { (AST_Var*)ast };
}

#pragma endregion generated AST / Code cast implementation
