#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "interface.hpp"
#endif

inline
void append( AST* self, AST* other )
{
	GEN_ASSERT(self  != nullptr);
	GEN_ASSERT(other != nullptr);

	if ( other->Parent )
		other = duplicate(other);

	other->Parent = self;

	if ( self->Front == nullptr )
	{
		self->Front = other;
		self->Back  = other;

		self->NumEntries++;
		return;
	}

	AST*
	Current       = self->Back;
	Current->Next = other;
	other->Prev   = Current;
	self->Back    = other;
	self->NumEntries++;
}

inline
Code* entry( AST* self, u32 idx )
{
	GEN_ASSERT(self != nullptr);
	AST** current = & self->Front;
	while ( idx >= 0 && current != nullptr )
	{
		if ( idx == 0 )
			return rcast( Code*, current);

		current = & ( * current )->Next;
		idx--;
	}

	return rcast( Code*, current);
}

inline
bool has_entries(AST* self)
{
	GEN_ASSERT(self != nullptr);
	return self->NumEntries > 0;
}

inline
bool is_body(AST* self)
{
	GEN_ASSERT(self != nullptr);
	switch (self->Type)
	{
		case ECode::Enum_Body:
		case ECode::Class_Body:
		case ECode::Union_Body:
		case ECode::Export_Body:
		case ECode::Global_Body:
		case ECode::Struct_Body:
		case ECode::Function_Body:
		case ECode::Namespace_Body:
		case ECode::Extern_Linkage_Body:
			return true;
	}
	return false;
}

inline
char const* type_str(AST* self)
{
	GEN_ASSERT(self != nullptr);
	return ECode::to_str( self->Type );
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
		ast->ParentAccess = AccessSpec_Public;
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
		entry = GEN_NS duplicate( entry );

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

		param = { (AST_Param*) param.raw()->Next };
	}
	while ( --idx );

	return param;
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
		ast->ParentAccess = AccessSpec_Public;
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
StrC token_fmt_impl( ssize num, ... )
{
	local_persist thread_local
	char buf[GEN_PRINTF_MAXLEN] = { 0 };
	mem_set( buf, 0, GEN_PRINTF_MAXLEN );

	va_list va;
	va_start(va, num );
	ssize result = token_fmt_va(buf, GEN_PRINTF_MAXLEN, num, va);
	va_end(va);

	return { result, buf };
}
