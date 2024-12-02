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

#pragma region Code
inline
char const* debug_str( Code code )
{
	if ( code.ast == nullptr )
		return "Code::debug_str: AST is null!";

	return debug_str( code.ast );
}
inline
Code duplicate( Code code )
{
	if ( code.ast == nullptr )
	{
		log_failure("Code::duplicate: Cannot duplicate code, AST is null!");
		return Code_Invalid;
	}

	return { duplicate(code.ast) };
}
inline
bool is_body(Code code)
{
	if ( code.ast == nullptr )
	{
		return is_body(code.ast);
	}
	return false;
}
inline
bool is_equal( Code self, Code other )
{
	if ( self.ast == nullptr || other.ast == nullptr )
	{
		// Just check if they're both null.
		// log_failure( "Code::is_equal: Cannot compare code, AST is null!" );
		return self.ast == nullptr && other.ast == nullptr;
	}
	return is_equal( self.ast, other.ast );
}
inline
bool is_valid(Code self)
{
	return self.ast != nullptr && self.ast->Type != CodeT::Invalid;
}
inline
void set_global(Code self)
{
	if ( self.ast == nullptr )
	{
		log_failure("Code::set_global: Cannot set code as global, AST is null!");
		return;
	}

	self->Parent = Code_Global.ast;
}
inline
Code& Code::operator ++()
{
	if ( ast )
		ast = ast->Next;

	return *this;
}
#pragma endregion Code

#pragma region CodeBody
inline
void append( CodeBody self, Code other )
{
	GEN_ASSERT(other.ast != nullptr);

	if (is_body(other)) {
		append( self, cast(CodeBody, other) );
		return;
	}

	append( rcast(AST*, self.ast), other.ast );
}
inline
void append( CodeBody self, CodeBody body )
{
	GEN_ASSERT(self.ast != nullptr);

	for ( Code entry : body ) {
		append( self, entry );
	}
}
inline
Code begin( CodeBody body) {
	if ( body.ast )
		return { rcast( AST*, body.ast)->Front };
	return { nullptr };
}
inline
Code end(CodeBody body ){
	return { rcast(AST*, body.ast)->Back->Next };
}
#pragma endregion CodeBody

#pragma region CodeClass
inline
void add_interface( CodeClass self, CodeType type )
{
	GEN_ASSERT(self.ast !=nullptr);
	CodeType possible_slot = self->ParentType;
	if ( possible_slot.ast )
	{
		// Were adding an interface to parent type, so we need to make sure the parent type is public.
		self->ParentAccess = AccessSpec_Public;
		// If your planning on adding a proper parent,
		// then you'll need to move this over to ParentType->next and update ParentAccess accordingly.
	}

	while ( possible_slot.ast != nullptr )
	{
		possible_slot.ast = (AST_Type*) possible_slot->Next.ast;
	}

	possible_slot.ast = type.ast;
}
#pragma endregion CodeClass

#pragma region CodeParam
inline
void append( CodeParam appendee, CodeParam other )
{
	GEN_ASSERT(appendee.ast != nullptr);
	AST* self  = cast(Code, appendee).ast;
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
CodeParam get(CodeParam self, s32 idx )
{
	GEN_ASSERT(self.ast != nullptr);
	CodeParam param = * self;
	do
	{
		if ( ! ++ param )
			return { nullptr };

		param = { (AST_Param*) cast(Code, param)->Next };
	}
	while ( --idx );

	return param;
}
inline
bool has_entries(CodeParam self)
{
	GEN_ASSERT(self.ast != nullptr);
	return self->NumEntries > 0;
}
inline
CodeParam& CodeParam::operator ++()
{
	ast = ast->Next.ast;
	return * this;
}
inline
CodeParam begin(CodeParam params)
{
	if ( params.ast )
		return { params.ast };

	return { nullptr };
}
inline
CodeParam end(CodeParam params)
{
	// return { (AST_Param*) rcast( AST*, ast)->Last };
	return { nullptr };
}
#pragma endregion CodeParam

#pragma region CodeSpecifiers
inline
bool append(CodeSpecifiers self, SpecifierT spec )
{
	if ( self.ast == nullptr )
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
s32 has(CodeSpecifiers self, SpecifierT spec)
{
	GEN_ASSERT(self.ast != nullptr);
	for ( s32 idx = 0; idx < self->NumEntries; idx++ ) {
		if ( self->ArrSpecs[ idx ] == spec )
			return idx;
	}
	return -1;
}
inline
s32 remove( CodeSpecifiers self, SpecifierT to_remove )
{
	AST_Specifiers* ast = self.ast;
	if ( ast == nullptr )
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
		SpecifierT spec = self->ArrSpecs[next];
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
inline
SpecifierT* begin(CodeSpecifiers self)
{
	if ( self.ast )
		return & self->ArrSpecs[0];

	return nullptr;
}
inline
SpecifierT* end(CodeSpecifiers self)
{
	return self->ArrSpecs + self->NumEntries;
}
#pragma endregion CodeSpecifiers

#pragma region CodeStruct
inline
void add_interface(CodeStruct self, CodeType type )
{
	CodeType possible_slot = self->ParentType;
	if ( possible_slot.ast )
	{
		// Were adding an interface to parent type, so we need to make sure the parent type is public.
		self->ParentAccess = AccessSpec_Public;
		// If your planning on adding a proper parent,
		// then you'll need to move this over to ParentType->next and update ParentAccess accordingly.
	}

	while ( possible_slot.ast != nullptr )
	{
		possible_slot.ast = (AST_Type*) possible_slot->Next.ast;
	}

	possible_slot.ast = type.ast;
}
#pragma endregion Code

#pragma region Interface
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

	return { result, buf };
}
#pragma endregion Interface
