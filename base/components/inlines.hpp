#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "interface.hpp"
#endif

#pragma region Serialization
inline
StrBuilder attributes_to_strbuilder(CodeAttributes attributes) {
	GEN_ASSERT(attributes);
	char* raw = ccast(char*, str_duplicate( attributes->Content, get_context()->Allocator_Temp ).Ptr);
	StrBuilder result = { raw };
	return result;
}

inline
void attributes_to_strbuilder_ref(CodeAttributes attributes, StrBuilder* result) {
	GEN_ASSERT(attributes);
	GEN_ASSERT(result);
	strbuilder_append_str(result, attributes->Content);
}

inline
StrBuilder comment_to_strbuilder(CodeComment comment) {
	GEN_ASSERT(comment);
	char* raw = ccast(char*, str_duplicate( comment->Content, get_context()->Allocator_Temp ).Ptr);
	StrBuilder result = { raw };
	return result;
}

inline
void body_to_strbuilder_ref( CodeBody body, StrBuilder* result )
{
	GEN_ASSERT(body   != nullptr);
	GEN_ASSERT(result != nullptr);
	Code curr = body->Front;
	s32  left = body->NumEntries;
	while ( left -- )
	{
		code_to_strbuilder_ref(curr, result);
		// strbuilder_append_fmt( result, "%SB", code_to_strbuilder(curr) );
		++curr;
	}
}

inline
void comment_to_strbuilder_ref(CodeComment comment, StrBuilder* result) {
	GEN_ASSERT(comment);
	GEN_ASSERT(result);
	strbuilder_append_str(result, comment->Content);
}

inline
StrBuilder define_to_strbuilder(CodeDefine define)
{
	GEN_ASSERT(define)
	StrBuilder result = strbuilder_make_reserve( _ctx->Allocator_Temp, 512 );
	define_to_strbuilder_ref(define, & result);
	return result;
}

inline
StrBuilder define_params_to_strbuilder(CodeDefineParams params)
{
	GEN_ASSERT(params);
	StrBuilder result = strbuilder_make_reserve( _ctx->Allocator_Temp, 128 );
	define_params_to_strbuilder_ref( params, & result );
	return result;
}

inline
StrBuilder exec_to_strbuilder(CodeExec exec)
{
	GEN_ASSERT(exec);
	char* raw = ccast(char*, str_duplicate( exec->Content, _ctx->Allocator_Temp ).Ptr);
	StrBuilder result = { raw };
	return result;
}

inline
StrBuilder exec_to_strbuilder_ref(CodeExec exec, StrBuilder* result) {
	GEN_ASSERT(exec);
	GEN_ASSERT(result);
	char* raw = ccast(char*, str_duplicate( exec->Content, get_context()->Allocator_Temp ).Ptr);
	StrBuilder result = { raw };
	return result;
}

inline
void extern_to_strbuilder(CodeExtern self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( self->Body )
		strbuilder_append_fmt( result, "extern \"%S\"\n{\n%SB\n}\n", self->Name, body_to_strbuilder(self->Body) );
	else
		strbuilder_append_fmt( result, "extern \"%S\"\n{}\n", self->Name );
}

inline
StrBuilder friend_to_strbuilder(CodeFriend self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( _ctx->Allocator_Temp, 256 );
	friend_to_strbuilder_ref( self, & result );
	return result;
}

inline
void friend_to_strbuilder_ref(CodeFriend self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	strbuilder_append_fmt( result, "friend %SB", code_to_strbuilder(self->Declaration) );

	if ( self->Declaration->Type != CT_Function && self->Declaration->Type != CT_Operator && (* result)[ strbuilder_length(* result) - 1 ] != ';' )
	{
		strbuilder_append_str( result, txt(";") );
	}

	if ( self->InlineCmt )
		strbuilder_append_fmt( result, "  %S", self->InlineCmt->Content );
	else
		strbuilder_append_str( result, txt("\n"));
}

inline
StrBuilder include_to_strbuilder(CodeInclude include)
{
	GEN_ASSERT(include);
	return strbuilder_fmt_buf( _ctx->Allocator_Temp, "#include %S\n", include->Content );
}

inline
void include_to_strbuilder_ref( CodeInclude include, StrBuilder* result )
{
	GEN_ASSERT(include);
	GEN_ASSERT(result);
	strbuilder_append_fmt( result, "#include %S\n", include->Content );
}

inline
StrBuilder module_to_strbuilder(CodeModule self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( _ctx->Allocator_Temp, 64 );
	module_to_strbuilder_ref( self, & result );
	return result;
}

inline
StrBuilder namespace_to_strbuilder(CodeNS self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( _ctx->Allocator_Temp, 512 );
	namespace_to_strbuilder_ref( self, & result );
	return result;
}

inline
void namespace_to_strbuilder_ref(CodeNS self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( bitfield_is_set( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	strbuilder_append_fmt( result, "namespace %S\n{\n%SB\n}\n", self->Name, body_to_strbuilder(self->Body) );
}

inline
StrBuilder params_to_strbuilder(CodeParams self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( _ctx->Allocator_Temp, 128 );
	params_to_strbuilder_ref( self, & result );
	return result;
}

inline
void preprocess_to_strbuilder_if(CodePreprocessCond cond, StrBuilder* result )
{
	GEN_ASSERT(cond);
	GEN_ASSERT(result);
	strbuilder_append_fmt( result, "#if %S", cond->Content );
}

inline
void preprocess_to_strbuilder_ifdef(CodePreprocessCond cond, StrBuilder* result )
{
	GEN_ASSERT(cond);
	GEN_ASSERT(result);
	strbuilder_append_fmt( result, "#ifdef %S\n", cond->Content );
}

inline
void preprocess_to_strbuilder_ifndef(CodePreprocessCond cond, StrBuilder* result )
{
	GEN_ASSERT(cond);
	GEN_ASSERT(result);
	strbuilder_append_fmt( result, "#ifndef %S", cond->Content );
}

inline
void preprocess_to_strbuilder_elif(CodePreprocessCond cond, StrBuilder* result )
{
	GEN_ASSERT(cond);
	GEN_ASSERT(result);
	strbuilder_append_fmt( result, "#elif %S\n", cond->Content );
}

inline
void preprocess_to_strbuilder_else(CodePreprocessCond cond, StrBuilder* result )
{
	GEN_ASSERT(cond);
	GEN_ASSERT(result);
	strbuilder_append_str( result, txt("#else\n") );
}

inline
void preprocess_to_strbuilder_endif(CodePreprocessCond cond, StrBuilder* result )
{
	GEN_ASSERT(cond);
	GEN_ASSERT(result);
	strbuilder_append_str( result, txt("#endif\n") );
}

inline
StrBuilder pragma_to_strbuilder(CodePragma self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( _ctx->Allocator_Temp, 256 );
	pragma_to_strbuilder_ref( self, & result );
	return result;
}

inline
void pragma_to_strbuilder_ref(CodePragma self, StrBuilder* result )
{
	GEN_ASSERT(self)
	GEN_ASSERT(result)
	strbuilder_append_fmt( result, "#pragma %S\n", self->Content );
}

inline
StrBuilder specifiers_to_strbuilder(CodeSpecifiers self)
{
	GEN_ASSERT(self)
	StrBuilder result = strbuilder_make_reserve( _ctx->Allocator_Temp, 64 );
	specifiers_to_strbuilder_ref( self, & result );
	return result;
}

inline
StrBuilder template_to_strbuilder(CodeTemplate self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( _ctx->Allocator_Temp, 1024 );
	template_to_strbuilder_ref( self, & result );
	return result;
}

inline
StrBuilder typedef_to_strbuilder(CodeTypedef self)
{
	GEN_ASSERT(self)
	StrBuilder result = strbuilder_make_reserve( _ctx->Allocator_Temp, 128 );
	typedef_to_strbuilder_ref( self, & result );
	return result;
}

inline
StrBuilder typename_to_strbuilder(CodeTypename self)
{
	GEN_ASSERT(self)
	StrBuilder result = strbuilder_make_str( _ctx->Allocator_Temp, txt("") );
	typename_to_strbuilder_ref( self, & result );
	return result;
}

inline
StrBuilder using_to_strbuilder(CodeUsing self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( _ctx->Allocator_Temp, 128 );
	switch ( self->Type )
	{
		case CT_Using:
			using_to_strbuilder_ref( self, & result );
		break;
		case CT_Using_Namespace:
			using_to_strbuilder_ns( self, & result );
		break;
	}
	return result;
}

inline
void using_to_strbuilder_ns(CodeUsing self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( self->InlineCmt )
		strbuilder_append_fmt( result, "using namespace $S;  %S", self->Name, self->InlineCmt->Content );
	else
		strbuilder_append_fmt( result, "using namespace %S;\n", self->Name );
}

inline
StrBuilder var_to_strbuilder(CodeVar self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( get_context()->Allocator_Temp, 256 );
	var_to_strbuilder_ref( self, & result );
	return result;
}
#pragma endregion Serialization

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
bool code_has_entries(Code self)
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
Str code_type_str(Code self)
{
	GEN_ASSERT(self != nullptr);
	return codetype_to_str( self->Type );
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

	while ( possible_slot->Next != nullptr )
	{
		possible_slot = cast(CodeTypename, possible_slot->Next);
	}

	possible_slot->Next = type;
}
#pragma endregion CodeClass

#pragma region CodeParams
inline
void params_append( CodeParams appendee, CodeParams other )
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
CodeParams params_get(CodeParams self, s32 idx )
{
	GEN_ASSERT(self);
	CodeParams param = self;
	do
	{
		if ( ++ param != nullptr )
			return NullCode;

		param = cast(CodeParams, cast(Code, param)->Next);
	}
	while ( --idx );

	return param;
}
forceinline
bool params_has_entries(CodeParams self)
{
	GEN_ASSERT(self);
	return self->NumEntries > 0;
}
#if GEN_COMPILER_CPP
forceinline
CodeParams& CodeParams::operator ++()
{
	* this = ast->Next;
	return * this;
}
#endif
forceinline
CodeParams begin_CodeParams(CodeParams params)
{
	if ( params != nullptr )
		return params;

	return NullCode;
}
forceinline
CodeParams end_CodeParams(CodeParams params)
{
	// return { (AST_Params*) rcast( AST*, ast)->Last };
	return NullCode;
}
forceinline
CodeParams next_CodeParams(CodeParams params, CodeParams param_iter)
{
	GEN_ASSERT(param_iter);
	return param_iter->Next;
}
#pragma endregion CodeParams

#pragma region CodeDefineParams
forceinline void             define_params_append     (CodeDefineParams appendee, CodeDefineParams other ) { params_append( cast(CodeParams, appendee), cast(CodeParams, other) ); }
forceinline CodeDefineParams define_params_get        (CodeDefineParams self, s32 idx )                    { return (CodeDefineParams) (Code) params_get( cast(CodeParams, self), idx); }
forceinline bool             define_params_has_entries(CodeDefineParams self)                              { return params_has_entries( cast(CodeParams, self)); }

forceinline CodeDefineParams begin_CodeDefineParams(CodeDefineParams params)                              { return (CodeDefineParams) (Code) begin_CodeParams( cast(CodeParams, (Code)params)); }
forceinline CodeDefineParams end_CodeDefineParams  (CodeDefineParams params)                              { return (CodeDefineParams) (Code) end_CodeParams  ( cast(CodeParams, (Code)params)); }
forceinline CodeDefineParams next_CodeDefineParams (CodeDefineParams params, CodeDefineParams entry_iter) { return (CodeDefineParams) (Code) next_CodeParams ( cast(CodeParams, (Code)params), cast(CodeParams, (Code)entry_iter)); }

#if GEN_COMPILER_CPP
forceinline
CodeDefineParams& CodeDefineParams::operator ++()
{
	* this = ast->Next;
	return * this;
}
#endif
#pragma endregion CodeDefineParams

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
bool specifiers_has(CodeSpecifiers self, Specifier spec)
{
	GEN_ASSERT(self != nullptr);
	for ( s32 idx = 0; idx < self->NumEntries; idx++ ) {
		if ( self->ArrSpecs[ idx ] == spec )
			return true;
	}
	return false;
}
inline
s32 specifiers_index_of(CodeSpecifiers self, Specifier spec)
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

	while ( possible_slot->Next != nullptr )
	{
		possible_slot->Next = cast(CodeTypename, possible_slot->Next);
	}

	possible_slot->Next = type;
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
Str token_fmt_impl( ssize num, ... )
{
	local_persist thread_local
	char buf[GEN_PRINTF_MAXLEN] = { 0 };
	mem_set( buf, 0, GEN_PRINTF_MAXLEN );

	va_list va;
	va_start(va, num );
	ssize result = token_fmt_va(buf, GEN_PRINTF_MAXLEN, num, va);
	va_end(va);

	Str str = { buf, result };
	return str;
}
#pragma endregion Interface
