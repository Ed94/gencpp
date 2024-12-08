#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "interface.hpp"
#endif

#pragma region Code
inline
void code_append( Code self, Code other )
{
	GEN_ASSERT(self);
	GEN_ASSERT(other);
	GEN_ASSERT_MSG(self != other, "Attempted to recursively append Code AST to itself.");

	if ( other->Parent != nullptr )
		other = code_duplicate(other);

	other->Parent = self;

	if ( self->Front == nullptr )
	{
		self->Front = other;
		self->Back  = other;

		self->NumEntries++;
		return;
	}

	Code
	Current       = self->Back;
	Current->Next = other;
	other->Prev   = Current;
	self->Back    = other;
	self->NumEntries++;
}
inline
bool code_is_body(Code self)
{
	GEN_ASSERT(self);
	switch (self->Type)
	{
		case CT_Enum_Body:
		case CT_Class_Body:
		case CT_Union_Body:
		case CT_Export_Body:
		case CT_Global_Body:
		case CT_Struct_Body:
		case CT_Function_Body:
		case CT_Namespace_Body:
		case CT_Extern_Linkage_Body:
			return true;
	}
	return false;
}
inline
Code* code_entry( Code self, u32 idx )
{
	GEN_ASSERT(self != nullptr);
	Code* current = & self->Front;
	while ( idx >= 0 && current != nullptr )
	{
		if ( idx == 0 )
			return rcast( Code*, current);

		current = & ( * current )->Next;
		idx--;
	}

	return rcast( Code*, current);
}
forceinline
bool code_is_valid(Code self)
{
	GEN_ASSERT(self);
	return self != nullptr && self->Type != CT_Invalid;
}
forceinline
bool code_has_entries(AST* self)
{
	GEN_ASSERT(self);
	return self->NumEntries > 0;
}
forceinline
void code_set_global(Code self)
{
	if ( self == nullptr )
	{
		log_failure("Code::set_global: Cannot set code as global, AST is null!");
		return;
	}

	self->Parent = Code_Global;
}
#if GEN_COMPILER_CPP
forceinline
Code& Code::operator ++()
{
	if ( ast )
		ast = ast->Next.ast;

	return * this;
}
#endif
forceinline
char const* code_type_str(Code self)
{
	GEN_ASSERT(self != nullptr);
	return codetype_to_str( self->Type ).Ptr;
}
#pragma endregion Code

#pragma region CodeBody
inline
void body_append( CodeBody self, Code other )
{
	GEN_ASSERT(self);
	GEN_ASSERT(other);

	if (code_is_body(other)) {
		body_append_body( self, cast(CodeBody, other) );
		return;
	}

	code_append( cast(Code, self), other );
}
inline
void body_append_body( CodeBody self, CodeBody body )
{
	GEN_ASSERT(self);
	GEN_ASSERT(body);
	GEN_ASSERT_MSG(self != body, "Attempted to append body to itself.");

	for ( Code entry = begin_CodeBody(body); entry != end_CodeBody(body); entry = next_CodeBody(body, entry) ) {
		body_append( self, entry );
	}
}
inline
Code begin_CodeBody( CodeBody body) {
	GEN_ASSERT(body);
	if ( body != nullptr )
		return body->Front;

	return NullCode;
}
forceinline
Code end_CodeBody(CodeBody body ){
	GEN_ASSERT(body);
	return body->Back->Next;
}
inline
Code next_CodeBody(CodeBody body, Code entry) {
	GEN_ASSERT(body);
	GEN_ASSERT(entry);
	return entry->Next;
}
#pragma endregion CodeBody

#pragma region CodeClass
inline
void class_add_interface( CodeClass self, CodeTypename type )
{
	GEN_ASSERT(self);
	GEN_ASSERT(type);
	CodeTypename possible_slot = self->ParentType;
	if ( possible_slot != nullptr )
	{
		// Were adding an interface to parent type, so we need to make sure the parent type is public.
		self->ParentAccess = AccessSpec_Public;
		// If your planning on adding a proper parent,
		// then you'll need to move this over to ParentType->next and update ParentAccess accordingly.
	}

	while ( possible_slot != nullptr )
	{
		possible_slot = cast(CodeTypename, possible_slot->Next);
	}

	possible_slot = type;
}
#pragma endregion CodeClass

#pragma region CodeParam
inline
void params_append( CodeParam appendee, CodeParam other )
{
	GEN_ASSERT(appendee);
	GEN_ASSERT(other);
	GEN_ASSERT_MSG(appendee != other, "Attempted to append parameter to itself.");
	Code self  = cast(Code, appendee);
	Code entry = cast(Code, other);

	if ( entry->Parent != nullptr )
		entry = code_duplicate( entry );

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
CodeParam params_get(CodeParam self, s32 idx )
{
	GEN_ASSERT(self);
	CodeParam param = self;
	do
	{
		if ( ++ param != nullptr )
			return NullCode;

		param = cast(CodeParam, cast(Code, param)->Next);
	}
	while ( --idx );

	return param;
}
forceinline
bool params_has_entries(CodeParam self)
{
	GEN_ASSERT(self);
	return self->NumEntries > 0;
}
#if GEN_COMPILER_CPP
forceinline
CodeParam& CodeParam::operator ++()
{
	* this = ast->Next;
	return * this;
}
#endif
forceinline
CodeParam begin_CodeParam(CodeParam params)
{
	if ( params != nullptr )
		return params;

	return NullCode;
}
forceinline
CodeParam end_CodeParam(CodeParam params)
{
	// return { (AST_Param*) rcast( AST*, ast)->Last };
	return NullCode;
}
forceinline
CodeParam next_CodeParam(CodeParam params, CodeParam param_iter)
{
	GEN_ASSERT(param_iter);
	return param_iter->Next;
}
#pragma endregion CodeParam

#pragma region CodeSpecifiers
inline
bool specifiers_append(CodeSpecifiers self, Specifier spec )
{
	if ( self == nullptr )
	{
		log_failure("CodeSpecifiers: Attempted to append to a null specifiers AST!");
		return false;
	}
	if ( self->NumEntries == AST_ArrSpecs_Cap )
	{
		log_failure("CodeSpecifiers: Attempted to append over %d specifiers to a specifiers AST!", AST_ArrSpecs_Cap );
		return false;
	}

	self->ArrSpecs[ self->NumEntries ] = spec;
	self->NumEntries++;
	return true;
}
inline
s32 specifiers_has(CodeSpecifiers self, Specifier spec)
{
	GEN_ASSERT(self != nullptr);
	for ( s32 idx = 0; idx < self->NumEntries; idx++ ) {
		if ( self->ArrSpecs[ idx ] == spec )
			return idx;
	}
	return -1;
}
inline
s32 specifiers_remove( CodeSpecifiers self, Specifier to_remove )
{
	if ( self == nullptr )
	{
		log_failure("CodeSpecifiers: Attempted to append to a null specifiers AST!");
		return -1;
	}
	if ( self->NumEntries == AST_ArrSpecs_Cap )
	{
		log_failure("CodeSpecifiers: Attempted to append over %d specifiers to a specifiers AST!", AST_ArrSpecs_Cap );
		return -1;
	}

	s32 result = -1;

	s32 curr = 0;
	s32 next = 0;
	for(; next < self->NumEntries; ++ curr, ++ next)
	{
		Specifier spec = self->ArrSpecs[next];
		if (spec == to_remove)
		{
			result = next;

			next ++;
			if (next >= self->NumEntries)
				break;

			spec = self->ArrSpecs[next];
		}

		self->ArrSpecs[ curr ] = spec;
	}

	if (result > -1) {
		self->NumEntries --;
	}
	return result;
}
forceinline
Specifier* begin_CodeSpecifiers(CodeSpecifiers self)
{
	if ( self != nullptr )
		return & self->ArrSpecs[0];

	return nullptr;
}
forceinline
Specifier* end_CodeSpecifiers(CodeSpecifiers self)
{
	return self->ArrSpecs + self->NumEntries;
}
forceinline
Specifier* next_CodeSpecifiers(CodeSpecifiers self, Specifier* spec_iter)
{
	return spec_iter + 1;
}
#pragma endregion CodeSpecifiers

#pragma region CodeStruct
inline
void struct_add_interface(CodeStruct self, CodeTypename type )
{
	CodeTypename possible_slot = self->ParentType;
	if ( possible_slot != nullptr )
	{
		// Were adding an interface to parent type, so we need to make sure the parent type is public.
		self->ParentAccess = AccessSpec_Public;
		// If your planning on adding a proper parent,
		// then you'll need to move this over to ParentType->next and update ParentAccess accordingly.
	}

	while ( possible_slot != nullptr )
	{
		possible_slot = cast(CodeTypename, possible_slot->Next);
	}

	possible_slot = type;
}
#pragma endregion Code

#pragma region Interface
inline
CodeBody def_body( CodeType type )
{
	switch ( type )
	{
		case CT_Class_Body:
		case CT_Enum_Body:
		case CT_Export_Body:
		case CT_Extern_Linkage:
		case CT_Function_Body:
		case CT_Global_Body:
		case CT_Namespace_Body:
		case CT_Struct_Body:
		case CT_Union_Body:
			break;

		default:
			log_failure( "def_body: Invalid type %s", codetype_to_str(type).Ptr );
			return (CodeBody)Code_Invalid;
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

	StrC str = { result, buf };
	return str;
}
#pragma endregion Interface
