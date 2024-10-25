#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "interface.hpp"
#endif

inline
void AST::append( AST* other )
{
	if ( other->Parent )
		other = other->duplicate();

	other->Parent = this;

	if ( Front == nullptr )
	{
		Front = other;
		Back  = other;

		NumEntries++;
		return;
	}

	AST*
		Current       = Back;
	Current->Next = other;
	other->Prev   = Current;
	Back          = other;
	NumEntries++;
}

inline
Code& AST::entry( u32 idx )
{
	AST** current = & Front;
	while ( idx >= 0 && current != nullptr )
	{
		if ( idx == 0 )
			return * rcast( Code*, current);

		current = & ( * current )->Next;
		idx--;
	}

	return * rcast( Code*, current);
}

inline
bool AST::has_entries()
{
	return NumEntries > 0;
}

inline
char const* AST::type_str()
{
	return ECode::to_str( Type );
}

inline
AST::operator Code()
{
	return { this };
}

inline
Code& Code::operator ++()
{
	if ( ast )
		ast = ast->Next;

	return *this;
}

inline
void CodeClass::add_interface( CodeType type )
{
	CodeType possible_slot = ast->ParentType;
	if ( possible_slot.ast )
	{
		// Were adding an interface to parent type, so we need to make sure the parent type is public.
		ast->ParentAccess = AccessSpec::Public;
		// If your planning on adding a proper parent,
		// then you'll need to move this over to ParentType->next and update ParentAccess accordingly.
	}

	while ( possible_slot.ast != nullptr )
	{
		possible_slot.ast = (AST_Type*) possible_slot->Next.ast;
	}

	possible_slot.ast = type.ast;
}

inline
void CodeParam::append( CodeParam other )
{
	AST* self  = (AST*) ast;
	AST* entry = (AST*) other.ast;

	if ( entry->Parent )
		entry = entry->duplicate();

	entry->Parent = self;

	if ( self->Last == nullptr )
	{
		self->Last = entry;
		self->Next = entry;
		self->NumEntries++;
		return;
	}

	self->Last->Next = entry;
	self->Last       = entry;
	self->NumEntries++;
}

inline
CodeParam CodeParam::get( s32 idx )
{
	CodeParam param = *this;
	do
	{
		if ( ! ++ param )
			return { nullptr };

		return { (AST_Param*) param.raw()->Next };
	}
	while ( --idx );

	return { nullptr };
}

inline
bool CodeParam::has_entries()
{
	return ast->NumEntries > 0;
}

inline
CodeParam& CodeParam::operator ++()
{
	ast = ast->Next.ast;
	return * this;
}

inline
void CodeStruct::add_interface( CodeType type )
{
	CodeType possible_slot = ast->ParentType;
	if ( possible_slot.ast )
	{
		// Were adding an interface to parent type, so we need to make sure the parent type is public.
		ast->ParentAccess = AccessSpec::Public;
		// If your planning on adding a proper parent,
		// then you'll need to move this over to ParentType->next and update ParentAccess accordingly.
	}

	while ( possible_slot.ast != nullptr )
	{
		possible_slot.ast = (AST_Type*) possible_slot->Next.ast;
	}

	possible_slot.ast = type.ast;
}

inline
CodeBody def_body( CodeT type )
{
	switch ( type )
	{
		using namespace ECode;
		case Class_Body:
		case Enum_Body:
		case Export_Body:
		case Extern_Linkage:
		case Function_Body:
		case Global_Body:
		case Namespace_Body:
		case Struct_Body:
		case Union_Body:
			break;

		default:
			log_failure( "def_body: Invalid type %s", (char const*)ECode::to_str(type) );
			return (CodeBody)Code::Invalid;
	}

	Code
	result       = make_code();
	result->Type = type;
	return (CodeBody)result;
}

inline
StrC token_fmt_impl( sw num, ... )
{
	local_persist thread_local
	char buf[GEN_PRINTF_MAXLEN] = { 0 };
	mem_set( buf, 0, GEN_PRINTF_MAXLEN );

	va_list va;
	va_start(va, num );
	sw result = token_fmt_va(buf, GEN_PRINTF_MAXLEN, num, va);
	va_end(va);

	return { result, buf };
}
