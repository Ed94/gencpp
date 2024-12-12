#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "ast.cpp"
#endif

inline
StrBuilder attributes_to_string(CodeAttributes attributes) {
	GEN_ASSERT(attributes);
	char* raw = ccast(char*, str_duplicate( attributes->Content, GlobalAllocator ).Ptr);
	StrBuilder result = { raw };
	return result;
}

inline
void attributes_to_strbuilder_ref(CodeAttributes attributes, StrBuilder* result) {
	GEN_ASSERT(attributes);
	GEN_ASSERT(result);
	strbuilder_append_str(result, attributes->Content);
}

StrBuilder body_to_string(CodeBody body)
{
	GEN_ASSERT(body);
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 128 );
	switch ( body->Type )
	{
		case CT_Untyped:
		case CT_Execution:
			strbuilder_append_str( & result, cast(Code, body)->Content );
		break;

		case CT_Enum_Body:
		case CT_Class_Body:
		case CT_Extern_Linkage_Body:
		case CT_Function_Body:
		case CT_Global_Body:
		case CT_Namespace_Body:
		case CT_Struct_Body:
		case CT_Union_Body:
			body_to_strbuilder_ref( body, & result );
		break;

		case CT_Export_Body:
			body_to_strbuilder_export( body, & result );
		break;
	}
	return result;
}

void body_to_strbuilder_ref( CodeBody body, StrBuilder* result )
{
	GEN_ASSERT(body   != nullptr);
	GEN_ASSERT(result != nullptr);
	Code curr = body->Front;
	s32  left = body->NumEntries;
	while ( left -- )
	{
		code_to_strbuilder_ptr(curr, result);
		// strbuilder_append_fmt( result, "%SB", code_to_string(curr) );
		++curr;
	}
}

void body_to_strbuilder_export( CodeBody body, StrBuilder* result )
{
	GEN_ASSERT(body   != nullptr);
	GEN_ASSERT(result != nullptr);
	strbuilder_append_fmt( result, "export\n{\n" );

	Code curr = cast(Code, body);
	s32  left = body->NumEntries;
	while ( left-- )
	{
		code_to_strbuilder_ptr(curr, result);
		// strbuilder_append_fmt( result, "%SB", code_to_string(curr) );
		++curr;
	}

	strbuilder_append_fmt( result, "};\n" );
}

inline
StrBuilder comment_to_string(CodeComment comment) {
	GEN_ASSERT(comment);
	char* raw = ccast(char*, str_duplicate( comment->Content, GlobalAllocator ).Ptr);
	StrBuilder result = { raw };
	return result;
}

inline
void comment_to_strbuilder_ref(CodeComment comment, StrBuilder* result) {
	GEN_ASSERT(comment);
	GEN_ASSERT(result);
	strbuilder_append_str(result, comment->Content);
}

StrBuilder constructor_to_string(CodeConstructor self)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 128 );
	switch (self->Type)
	{
		case CT_Constructor:
			constructor_to_strbuilder_def( self, & result );
		break;
		case CT_Constructor_Fwd:
			constructor_to_strbuilder_fwd( self, & result );
		break;
	}
	return result;
}

void constructor_to_strbuilder_def(CodeConstructor self, StrBuilder* result )
{
	Code ClassStructParent = self->Parent->Parent;
	if (ClassStructParent) {
		strbuilder_append_str( result, ClassStructParent->Name );
	}
	else {
		strbuilder_append_str( result, self->Name );
	}

	if ( self->Params )
		strbuilder_append_fmt( result, "( %SB )", params_to_string(self->Params) );
	else
		strbuilder_append_str( result, txt("()") );

	if ( self->InitializerList )
		strbuilder_append_fmt( result, " : %SB", code_to_string(self->InitializerList) );

	if ( self->InlineCmt )
		strbuilder_append_fmt( result, " // %S", self->InlineCmt->Content );

	strbuilder_append_fmt( result, "\n{\n%SB\n}\n", code_to_string(self->Body) );
}

void constructor_to_strbuilder_fwd(CodeConstructor self, StrBuilder* result )
{
	Code ClassStructParent = self->Parent->Parent;
	if (ClassStructParent) {
		strbuilder_append_str( result, ClassStructParent->Name );
	}
	else {
		strbuilder_append_str( result, self->Name );
	}

	if ( self->Params )
		strbuilder_append_fmt( result, "( %SB )", params_to_string(self->Params) );
	else
		strbuilder_append_fmt( result, "()");

	if (self->Body)
		strbuilder_append_fmt( result, " = %SB", code_to_string(self->Body) );

	if ( self->InlineCmt )
		strbuilder_append_fmt( result, "; // %S\n", self->InlineCmt->Content );
	else
		strbuilder_append_str( result, txt(";\n") );
}

StrBuilder class_to_string( CodeClass self )
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Class:
			class_to_strbuilder_def(self, & result );
		break;
		case CT_Class_Fwd:
			class_to_strbuilder_fwd(self, & result );
		break;
	}
	return result;
}

void class_to_strbuilder_def( CodeClass self, StrBuilder* result )
{
	GEN_ASSERT(self);

	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	strbuilder_append_str( result, txt("class ") );

	if ( self->Attributes )
	{
		strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );
	}

	if ( self->ParentType )
	{
		Str access_level = access_spec_to_str( self->ParentAccess );
		strbuilder_append_fmt( result, "%S : %S %SB", self->Name, access_level, typename_to_string(self->ParentType) );

		CodeTypename interface = cast(CodeTypename, self->ParentType->Next);
		if ( interface )
			strbuilder_append_str( result, txt("\n") );

		while ( interface )
		{
			strbuilder_append_fmt( result, ", public %SB", typename_to_string(interface) );
			interface = interface->Next ? cast(CodeTypename, interface->Next) : NullCode;
		}
	}
	else if ( self->Name.Len )
	{
		strbuilder_append_str( result, self->Name );
	}

	if ( self->InlineCmt )
	{
		strbuilder_append_fmt( result, " // %S", self->InlineCmt->Content );
	}

	strbuilder_append_fmt( result, "\n{\n%SB\n}", body_to_string(self->Body) );

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		strbuilder_append_str( result, txt(";\n") );
}

void class_to_strbuilder_fwd( CodeClass self, StrBuilder* result )
{
	GEN_ASSERT(self);

	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	if ( self->Attributes )
		strbuilder_append_fmt( result, "class %SB %S", attributes_to_string(self->Attributes), self->Name );

	else strbuilder_append_fmt( result, "class %S", self->Name );

	// Check if it can have an end-statement
	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
	{
		if ( self->InlineCmt )
			strbuilder_append_fmt( result, "; // %S\n", self->InlineCmt->Content );
		else
			strbuilder_append_str( result, txt(";\n") );
	}
}

StrBuilder define_to_string(CodeDefine define)
{
	return strbuilder_fmt_buf( GlobalAllocator, "#define %S %S", define->Name, define->Content );
}

void define_to_strbuilder_ref(CodeDefine define, StrBuilder* result )
{
	strbuilder_append_fmt( result, "#define %S %S", define->Name, define->Content );
}

StrBuilder destructor_to_string(CodeDestructor self)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 128 );
	switch ( self->Type )
	{
		case CT_Destructor:
			destructor_to_strbuilder_def( self, & result );
		break;
		case CT_Destructor_Fwd:
			destructor_to_strbuilder_fwd( self, & result );
		break;
	}
	return result;
}

void destructor_to_strbuilder_def(CodeDestructor self, StrBuilder* result )
{
	if ( self->Name.Len )
	{
		strbuilder_append_fmt( result, "%S()", self->Name );
	}
	else if ( self->Specs )
	{
		if ( specifiers_has(self->Specs, Spec_Virtual ) )
			strbuilder_append_fmt( result, "virtual ~%S()", self->Parent->Name );
		else
			strbuilder_append_fmt( result, "~%S()", self->Parent->Name );
	}
	else
		strbuilder_append_fmt( result, "~%S()", self->Parent->Name );

	strbuilder_append_fmt( result, "\n{\n%SB\n}\n", code_to_string(self->Body) );
}

void destructor_to_strbuilder_fwd(CodeDestructor self, StrBuilder* result )
{
	if ( self->Specs )
	{
		if ( specifiers_has(self->Specs, Spec_Virtual ) )
			strbuilder_append_fmt( result, "virtual ~%S();\n", self->Parent->Name );
		else
			strbuilder_append_fmt( result, "~%S()", self->Parent->Name );

		if ( specifiers_has(self->Specs, Spec_Pure ) )
			strbuilder_append_str( result, txt(" = 0;") );
		else if (self->Body)
			strbuilder_append_fmt( result, " = %SB;", code_to_string(self->Body) );
	}
	else
		strbuilder_append_fmt( result, "~%S();", self->Parent->Name );

	if ( self->InlineCmt )
		strbuilder_append_fmt( result, "  %S", self->InlineCmt->Content );
	else
		strbuilder_append_str( result, txt("\n"));
}

StrBuilder enum_to_string(CodeEnum self)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Enum:
			enum_to_strbuilder_def(self, & result );
		break;
		case CT_Enum_Fwd:
			enum_to_strbuilder_fwd(self, & result );
		break;
		case CT_Enum_Class:
			enum_to_strbuilder_class_def(self, & result );
		break;
		case CT_Enum_Class_Fwd:
			enum_to_strbuilder_class_fwd(self, & result );
		break;
	}
	return result;
}

void enum_to_strbuilder_def(CodeEnum self, StrBuilder* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	if ( self->Attributes || self->UnderlyingType || self->UnderlyingTypeMacro )
	{
		strbuilder_append_str( result, txt("enum ") );

		if ( self->Attributes )
			strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );

		if ( self->UnderlyingType )
			strbuilder_append_fmt( result, "%S : %SB\n{\n%SB\n}"
				, self->Name
				, typename_to_string(self->UnderlyingType)
				, body_to_string(self->Body)
			);
		else if ( self->UnderlyingTypeMacro )
			strbuilder_append_fmt( result, "%S %SB\n{\n%SB\n}"
				, self->Name
				, code_to_string(self->UnderlyingTypeMacro)
				, body_to_string(self->Body)
			);

		else strbuilder_append_fmt( result, "%S\n{\n%SB\n}", self->Name, body_to_string(self->Body) );
	}
	else strbuilder_append_fmt( result, "enum %S\n{\n%SB\n}", self->Name, body_to_string(self->Body) );

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		strbuilder_append_str( result, txt(";\n"));
}

void enum_to_strbuilder_fwd(CodeEnum self, StrBuilder* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	if ( self->Attributes )
		strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );

	if ( self->UnderlyingType )
		strbuilder_append_fmt( result, "enum %S : %SB", self->Name, typename_to_string(self->UnderlyingType) );
	else if (self->UnderlyingTypeMacro)
	{
		log_fmt("IDENTIFIED A UNDERLYING ENUM MACRO");
		strbuilder_append_fmt( result, "enum %S %SB", self->Name, code_to_string(self->UnderlyingTypeMacro) );
	}
	else
		strbuilder_append_fmt( result, "enum %S", self->Name );

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
	{
		if ( self->InlineCmt )
			strbuilder_append_fmt( result, ";  %S", self->InlineCmt->Content );
		else
			strbuilder_append_str( result, txt(";\n"));
	}
}

void enum_to_strbuilder_class_def(CodeEnum self, StrBuilder* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	if ( self->Attributes || self->UnderlyingType )
	{
		strbuilder_append_str( result, txt("enum class ") );

		if ( self->Attributes )
		{
			strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );
		}

		if ( self->UnderlyingType )
		{
			strbuilder_append_fmt( result, "%S : %SB\n{\n%SB\n}", self->Name, typename_to_string(self->UnderlyingType), body_to_string(self->Body) );
		}
		else
		{
			strbuilder_append_fmt( result, "%S\n{\n%SB\n}", self->Name, body_to_string(self->Body) );
		}
	}
	else
	{
		strbuilder_append_fmt( result, "enum %S\n{\n%SB\n}", self->Name, body_to_string(self->Body) );
	}

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		strbuilder_append_str( result, txt(";\n"));
}

void enum_to_strbuilder_class_fwd(CodeEnum self, StrBuilder* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	strbuilder_append_str( result, txt("enum class ") );

	if ( self->Attributes )
		strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );

	strbuilder_append_fmt( result, "%S : %SB", self->Name, typename_to_string(self->UnderlyingType) );

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
	{
		if ( self->InlineCmt )
			strbuilder_append_fmt( result, ";  %S", self->InlineCmt->Content );
		else
			strbuilder_append_str( result, txt(";\n"));
	}
}

StrBuilder exec_to_string(CodeExec exec)
{
	GEN_ASSERT(exec);
	char* raw = ccast(char*, str_duplicate( exec->Content, GlobalAllocator ).Ptr);
	StrBuilder result = { raw };
	return result;
}

void extern_to_string(CodeExtern self, StrBuilder* result )
{
	if ( self->Body )
		strbuilder_append_fmt( result, "extern \"%S\"\n{\n%SB\n}\n", self->Name, body_to_string(self->Body) );
	else
		strbuilder_append_fmt( result, "extern \"%S\"\n{}\n", self->Name );
}

StrBuilder include_to_string(CodeInclude include)
{
	return strbuilder_fmt_buf( GlobalAllocator, "#include %S\n", include->Content );
}

void include_to_strbuilder_ref( CodeInclude include, StrBuilder* result )
{
	strbuilder_append_fmt( result, "#include %S\n", include->Content );
}

StrBuilder friend_to_string(CodeFriend self)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 256 );
	friend_to_strbuilder_ref( self, & result );
	return result;
}

void friend_to_strbuilder_ref(CodeFriend self, StrBuilder* result )
{
	strbuilder_append_fmt( result, "friend %SB", code_to_string(self->Declaration) );

	if ( self->Declaration->Type != CT_Function && self->Declaration->Type != CT_Operator && (* result)[ strbuilder_length(* result) - 1 ] != ';' )
	{
		strbuilder_append_str( result, txt(";") );
	}

	if ( self->InlineCmt )
		strbuilder_append_fmt( result, "  %S", self->InlineCmt->Content );
	else
		strbuilder_append_str( result, txt("\n"));
}

StrBuilder fn_to_string(CodeFn self)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Function:
			fn_to_strbuilder_def(self, & result );
		break;
		case CT_Function_Fwd:
			fn_to_strbuilder_fwd(self, & result );
		break;
	}
	return result;
}

void fn_to_strbuilder_def(CodeFn self, StrBuilder* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export") );

	if ( self->Attributes )
		strbuilder_append_fmt( result, " %SB ", attributes_to_string(self->Attributes) );

	bool prefix_specs = false;
	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( ! spec_is_trailing( * spec ) )
			{
				Str spec_str = spec_to_str( * spec );
				strbuilder_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );

				prefix_specs = true;
			}
		}
	}

	if ( self->Attributes || prefix_specs )
		strbuilder_append_str( result, txt("\n") );

	if ( self->ReturnType )
		strbuilder_append_fmt( result, "%SB %S(", typename_to_string(self->ReturnType), self->Name );

	else
		strbuilder_append_fmt( result, "%S(", self->Name );

	if ( self->Params )
		strbuilder_append_fmt( result, "%SB)", params_to_string(self->Params) );

	else
		strbuilder_append_str( result, txt(")") );

	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( spec_is_trailing( * spec ) )
			{
				Str spec_str = spec_to_str( * spec );
				strbuilder_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	strbuilder_append_fmt( result, "\n{\n%SB\n}\n", body_to_string(self->Body) );
}

void fn_to_strbuilder_fwd(CodeFn self, StrBuilder* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	if ( self->Attributes )
		strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );

	b32 prefix_specs = false;
	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( ! spec_is_trailing( * spec ) || ! ( * spec != Spec_Pure) )
			{
				Str spec_str = spec_to_str( * spec );
				strbuilder_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );

				prefix_specs = true;
			}
		}
	}

	if ( self->Attributes || prefix_specs )
	{
		strbuilder_append_str( result, txt("\n") );
	}

	if ( self->ReturnType )
		strbuilder_append_fmt( result, "%SB %S(", typename_to_string(self->ReturnType), self->Name );

	else
		strbuilder_append_fmt( result, "%S(", self->Name );

	if ( self->Params )
		strbuilder_append_fmt( result, "%SB)", params_to_string(self->Params) );

	else
		strbuilder_append_str( result, txt(")") );

	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( spec_is_trailing( * spec ) )
			{
				Str spec_str = spec_to_str( * spec );
				strbuilder_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->Specs && specifiers_has(self->Specs, Spec_Pure ) >= 0 )
		strbuilder_append_str( result, txt(" = 0;") );
	else if (self->Body)
		strbuilder_append_fmt( result, " = %SB;", body_to_string(self->Body) );

	if ( self->InlineCmt )
		strbuilder_append_fmt( result, ";  %S", self->InlineCmt->Content );
	else
		strbuilder_append_str( result, txt(";\n") );
}

StrBuilder module_to_string(CodeModule self)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 64 );
	module_to_strbuilder_ref( self, & result );
	return result;
}

void module_to_strbuilder_ref(CodeModule self, StrBuilder* result )
{
	if (((scast(u32, ModuleFlag_Export) & scast(u32, self->ModuleFlags)) == scast(u32, ModuleFlag_Export)))
		strbuilder_append_str( result, txt("export "));

	if (((scast(u32, ModuleFlag_Import) & scast(u32, self->ModuleFlags)) == scast(u32, ModuleFlag_Import)))
		strbuilder_append_str( result, txt("import "));

	strbuilder_append_fmt( result, "%S;\n", self->Name );
}

StrBuilder namespace_to_string(CodeNS self)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 512 );
	namespace_to_strbuilder_ref( self, & result );
	return result;
}

void namespace_to_strbuilder_ref(CodeNS self, StrBuilder* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	strbuilder_append_fmt( result, "namespace %S\n{\n%SB\n}\n", self->Name, body_to_string(self->Body) );
}

StrBuilder code_op_to_string(CodeOperator self)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Operator:
		case CT_Operator_Member:
			code_op_to_strbuilder_def( self, & result );
		break;
		case CT_Operator_Fwd:
		case CT_Operator_Member_Fwd:
			code_op_to_strbuilder_fwd( self, & result );
		break;
	}
	return result;
}

void code_op_to_strbuilder_def(CodeOperator self, StrBuilder* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	if ( self->Attributes )
		strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );

	if ( self->Attributes )
		strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );

	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( ! spec_is_trailing( * spec ) )
			{
				Str spec_str = spec_to_str( * spec );
				strbuilder_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->Attributes || self->Specs )
	{
		strbuilder_append_str( result, txt("\n") );
	}

	if ( self->ReturnType )
		strbuilder_append_fmt( result, "%SB %S (", typename_to_string(self->ReturnType), self->Name );

	if ( self->Params )
		strbuilder_append_fmt( result, "%SB)", params_to_string(self->Params) );

	else
		strbuilder_append_str( result, txt(")") );

	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( spec_is_trailing( * spec ) )
			{
				Str spec_str = spec_to_str( * spec );
				strbuilder_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	strbuilder_append_fmt( result, "\n{\n%SB\n}\n"
		, body_to_string(self->Body)
	);
}

void code_op_to_strbuilder_fwd(CodeOperator self, StrBuilder* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	if ( self->Attributes )
		strbuilder_append_fmt( result, "%SB\n", attributes_to_string(self->Attributes) );

	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( ! spec_is_trailing( * spec ) )
			{
				Str spec_str = spec_to_str( * spec );
				strbuilder_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->Attributes || self->Specs )
	{
		strbuilder_append_str( result, txt("\n") );
	}

	strbuilder_append_fmt( result, "%SB %S (", typename_to_string(self->ReturnType), self->Name );

	if ( self->Params )
		strbuilder_append_fmt( result, "%SB)", params_to_string(self->Params) );

	else
		strbuilder_append_fmt( result, ")" );

	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( spec_is_trailing( * spec ) )
			{
				Str spec_str = spec_to_str( * spec );
				strbuilder_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->InlineCmt )
		strbuilder_append_fmt( result, ";  %S", self->InlineCmt->Content );
	else
		strbuilder_append_str( result, txt(";\n") );
}

StrBuilder opcast_to_string(CodeOpCast self)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 128 );
	switch ( self->Type )
	{
		case CT_Operator_Cast:
			opcast_to_strbuilder_def(self, & result );
		break;
		case CT_Operator_Cast_Fwd:
			opcast_to_strbuilder_fwd(self, & result );
		break;
	}
	return result;
}

void opcast_to_strbuilder_def(CodeOpCast self, StrBuilder* result )
{
	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( ! spec_is_trailing( * spec ) )
			{
				Str spec_str = spec_to_str( * spec );
				strbuilder_append_fmt( result, "%*s ", spec_str.Len, spec_str.Ptr );
			}
		}

		if ( self->Name.Ptr && self->Name.Len )
			strbuilder_append_fmt( result, "%S operator %SB()", self->Name, typename_to_string(self->ValueType) );
		else
			strbuilder_append_fmt( result, "operator %SB()", typename_to_string(self->ValueType) );

		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( spec_is_trailing( * spec ) )
			{
				Str spec_str = spec_to_str( * spec );
				strbuilder_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}

		strbuilder_append_fmt( result, "\n{\n%SB\n}\n", body_to_string(self->Body) );
		return;
	}

	if ( self->Name.Ptr && self->Name.Len )
		strbuilder_append_fmt( result, "%S operator %SB()\n{\n%SB\n}\n", self->Name, typename_to_string(self->ValueType), body_to_string(self->Body) );
	else
		strbuilder_append_fmt( result, "operator %SB()\n{\n%SB\n}\n", typename_to_string(self->ValueType), body_to_string(self->Body) );
}

void opcast_to_strbuilder_fwd(CodeOpCast self, StrBuilder* result )
{
	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( ! spec_is_trailing( * spec ) )
			{
				Str spec_str = spec_to_str( * spec );
				strbuilder_append_fmt( result, "%*s ", spec_str.Len, spec_str.Ptr );
			}
		}

		strbuilder_append_fmt( result, "operator %SB()", typename_to_string(self->ValueType) );

		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( spec_is_trailing( * spec ) )
			{
				Str spec_str = spec_to_str( * spec );
				strbuilder_append_fmt( result, " %*s", spec_str.Len, spec_str.Ptr );
			}
		}

		if ( self->InlineCmt )
			strbuilder_append_fmt( result, ";  %S", self->InlineCmt->Content );
		else
			strbuilder_append_str( result, txt(";\n") );
		return;
	}

	if ( self->InlineCmt )
		strbuilder_append_fmt( result, "operator %SB();  %SB", typename_to_string(self->ValueType) );
	else
		strbuilder_append_fmt( result, "operator %SB();\n", typename_to_string(self->ValueType) );
}

StrBuilder params_to_string(CodeParams self)
{
	GEN_ASSERT(self);
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 128 );
	params_to_strbuilder_ref( self, & result );
	return result;
}

void params_to_strbuilder_ref( CodeParams self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( self->Macro )
	{
		// Related to parsing: ( <macro>, ... )
		strbuilder_append_str( result, self->Macro->Content );
		// Could also be: ( <macro> <type <name>, ... )
	}

	if ( self->Name.Ptr && self->Name.Len )
	{
		if ( self->ValueType == nullptr )
			strbuilder_append_fmt( result, " %S", self->Name );
		else
			strbuilder_append_fmt( result, " %SB %S", typename_to_string(self->ValueType), self->Name );

	}
	else if ( self->ValueType )
		strbuilder_append_fmt( result, " %SB", typename_to_string(self->ValueType) );

	if ( self->PostNameMacro )
	{
		strbuilder_append_fmt( result, " %SB", code_to_string(self->PostNameMacro) );
	}

	if ( self->Value )
		strbuilder_append_fmt( result, " = %SB", code_to_string(self->Value) );

	if ( self->NumEntries - 1 > 0 )
	{
		for ( CodeParams param = begin_CodeParams(self->Next); param != end_CodeParams(self->Next); param = next_CodeParams(self->Next, param) )
		{
			strbuilder_append_fmt( result, ", %SB", params_to_string(param) );
		}
	}
}

StrBuilder preprocess_to_string(CodePreprocessCond self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 256 );
	switch ( self->Type )
	{
		case CT_Preprocess_If:
			preprocess_to_strbuilder_if( self, & result );
		break;
		case CT_Preprocess_IfDef:
			preprocess_to_strbuilder_ifdef( self, & result );
		break;
		case CT_Preprocess_IfNotDef:
			preprocess_to_strbuilder_ifndef( self, & result );
		break;
		case CT_Preprocess_ElIf:
			preprocess_to_strbuilder_elif( self, & result );
		break;
		case CT_Preprocess_Else:
			preprocess_to_strbuilder_else( self, & result );
		break;
		case CT_Preprocess_EndIf:
			preprocess_to_strbuilder_endif( self, & result );
		break;
	}
	return result;
}

void preprocess_to_strbuilder_if(CodePreprocessCond cond, StrBuilder* result )
{
	GEN_ASSERT(cond);
	strbuilder_append_fmt( result, "#if %S", cond->Content );
}

void preprocess_to_strbuilder_ifdef(CodePreprocessCond cond, StrBuilder* result )
{
	GEN_ASSERT(cond);
	strbuilder_append_fmt( result, "#ifdef %S\n", cond->Content );
}

void preprocess_to_strbuilder_ifndef(CodePreprocessCond cond, StrBuilder* result )
{
	GEN_ASSERT(cond);
	strbuilder_append_fmt( result, "#ifndef %S", cond->Content );
}

void preprocess_to_strbuilder_elif(CodePreprocessCond cond, StrBuilder* result )
{
	GEN_ASSERT(cond);
	strbuilder_append_fmt( result, "#elif %S\n", cond->Content );
}

void preprocess_to_strbuilder_else(CodePreprocessCond cond, StrBuilder* result )
{
	GEN_ASSERT(cond);
	strbuilder_append_str( result, txt("#else\n") );
}

void preprocess_to_strbuilder_endif(CodePreprocessCond cond, StrBuilder* result )
{
	GEN_ASSERT(cond);
	strbuilder_append_str( result, txt("#endif\n") );
}

StrBuilder pragma_to_string(CodePragma self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 256 );
	pragma_to_strbuilder_ref( self, & result );
	return result;
}

void pragma_to_strbuilder_ref(CodePragma self, StrBuilder* result )
{
	strbuilder_append_fmt( result, "#pragma %S\n", self->Content );
}

StrBuilder specifiers_to_string(CodeSpecifiers self)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 64 );
	specifiers_to_strbuilder_ref( self, & result );
	return result;
}

void specifiers_to_strbuilder_ref( CodeSpecifiers self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	s32 idx  = 0;
	s32 left = self->NumEntries;
	while ( left-- )
	{
		Str spec = spec_to_str( self->ArrSpecs[idx] );
		strbuilder_append_fmt( result, "%.*s ", spec.Len, spec.Ptr );
		idx++;
	}
}

StrBuilder struct_to_string(CodeStruct self)
{
	GEN_ASSERT(self);
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Struct:
			struct_to_strbuilder_def( self, & result );
		break;
		case CT_Struct_Fwd:
			struct_to_strbuilder_fwd( self, & result );
		break;
	}
	return result;
}

void struct_to_strbuilder_def( CodeStruct self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	strbuilder_append_str( result, txt("struct ") );

	if ( self->Attributes )
	{
		strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );
	}

	if ( self->ParentType )
	{
		Str access_level = access_spec_to_str( self->ParentAccess );

		strbuilder_append_fmt( result, "%S : %S %SB", self->Name, access_level, typename_to_string(self->ParentType) );

		CodeTypename interface = cast(CodeTypename, self->ParentType->Next);
		if ( interface )
			strbuilder_append_str( result, txt("\n") );

		while ( interface )
		{
			strbuilder_append_fmt( result, ", %SB", typename_to_string(interface) );
			interface = interface->Next ? cast( CodeTypename, interface->Next) : NullCode;
		}
	}
	else if ( self->Name.Len )
	{
		strbuilder_append_str( result, self->Name );
	}

	if ( self->InlineCmt )
	{
		strbuilder_append_fmt( result, " // %S", self->InlineCmt->Content );
	}

	strbuilder_append_fmt( result, "\n{\n%SB\n}", body_to_string(self->Body) );

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		strbuilder_append_str( result, txt(";\n"));
}

void struct_to_strbuilder_fwd( CodeStruct self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	if ( self->Attributes )
		strbuilder_append_fmt( result, "struct %SB %S", attributes_to_string(self->Attributes), self->Name );

	else strbuilder_append_fmt( result, "struct %S", self->Name );

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
	{
		if ( self->InlineCmt )
			strbuilder_append_fmt( result, ";  %S", self->InlineCmt->Content );
		else
			strbuilder_append_str( result, txt( ";\n") );
	}
}

StrBuilder template_to_string(CodeTemplate self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 1024 );
	template_to_strbuilder_ref( self, & result );
	return result;
}

void template_to_strbuilder_ref(CodeTemplate self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	if ( self->Params )
		strbuilder_append_fmt( result, "template< %SB >\n%SB", params_to_string(self->Params), code_to_string(self->Declaration) );
	else
		strbuilder_append_fmt( result, "template<>\n%SB", code_to_string(self->Declaration) );
}

StrBuilder typedef_to_string(CodeTypedef self)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 128 );
	typedef_to_strbuilder_ref( self, & result );
	return result;
}

void typedef_to_strbuilder_ref(CodeTypedef self, StrBuilder* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	strbuilder_append_str( result, txt("typedef "));

	// Determines if the typedef is a function typename
	if ( self->UnderlyingType->ReturnType )
		strbuilder_append_string( result, code_to_string(self->UnderlyingType) );
	else
		strbuilder_append_fmt( result, "%SB %S", code_to_string(self->UnderlyingType), self->Name );

	if ( self->UnderlyingType->Type == CT_Typename && self->UnderlyingType->ArrExpr )
	{
		strbuilder_append_fmt( result, "[ %SB ];", code_to_string(self->UnderlyingType->ArrExpr) );

		Code next_arr_expr = self->UnderlyingType->ArrExpr->Next;
		while ( next_arr_expr )
		{
			strbuilder_append_fmt( result, "[ %SB ];", code_to_string(next_arr_expr) );
			next_arr_expr = next_arr_expr->Next;
		}
	}
	else
	{
		strbuilder_append_str( result, txt(";") );
	}

	if ( self->InlineCmt )
		strbuilder_append_fmt( result, "  %S", self->InlineCmt->Content);
	else
		strbuilder_append_str( result, txt("\n"));
}

StrBuilder typename_to_string(CodeTypename self)
{
	StrBuilder result = strbuilder_make_str( GlobalAllocator, txt("") );
	typename_to_strbuilder_ref( self, & result );
	return result;
}

void typename_to_strbuilder_ref(CodeTypename self, StrBuilder* result )
{
	#if defined(GEN_USE_NEW_TYPENAME_PARSING)
		if ( self->ReturnType && self->Params )
		{
			if ( self->Attributes )
				strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );
			else
			{
				if ( self->Specs )
					strbuilder_append_fmt( result, "%SB ( %S ) ( %SB ) %SB", typename_to_string(self->ReturnType), self->Name, params_to_string(self->Params), specifiers_to_string(self->Specs) );
				else
					strbuilder_append_fmt( result, "%SB ( %S ) ( %SB )", typename_to_string(self->ReturnType), self->Name, params_to_string(self->Params) );
			}

			break;
		}
	#else
		if ( self->ReturnType && self->Params )
		{
			if ( self->Attributes )
				strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );
			else
			{
				if ( self->Specs )
					strbuilder_append_fmt( result, "%SB %S ( %SB ) %SB", typename_to_string(self->ReturnType), self->Name, params_to_string(self->Params), specifiers_to_string(self->Specs) );
				else
					strbuilder_append_fmt( result, "%SB %S ( %SB )", typename_to_string(self->ReturnType), self->Name, params_to_string(self->Params) );
			}

			return;
		}
	#endif

	if ( self->Attributes )
		strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );

	switch ( self->TypeTag )
	{
		case Tag_Class  : strbuilder_append_str( result, txt("class "));  break;
		case Tag_Enum   : strbuilder_append_str( result, txt("enum "));   break;
		case Tag_Struct : strbuilder_append_str( result, txt("struct ")); break;
		case Tag_Union  : strbuilder_append_str( result, txt("union "));  break;
		default:
			break;
	}

	if ( self->Specs )
		strbuilder_append_fmt( result, "%S %SB", self->Name, specifiers_to_string(self->Specs) );
	else
		strbuilder_append_fmt( result, "%S", self->Name );

	if ( self->IsParamPack )
		strbuilder_append_str( result, txt("..."));
}

StrBuilder union_to_string(CodeUnion self)
{
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Union:
			union_to_strbuilder_def( self, & result );
		break;
		case CT_Union_Fwd:
			union_to_strbuilder_fwd( self, & result );
		break;
	}
	return result;
}

void union_to_strbuilder_def(CodeUnion self, StrBuilder* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	strbuilder_append_str( result, txt("union ") );

	if ( self->Attributes )
		strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );

	if ( self->Name.Len )
	{
		strbuilder_append_fmt( result, "%S\n{\n%SB\n}"
			, self->Name
			, body_to_string(self->Body)
		);
	}
	else
	{
		// Anonymous union
		strbuilder_append_fmt( result, "\n{\n%SB\n}"
			, body_to_string(self->Body)
		);
	}

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		strbuilder_append_str( result, txt(";\n"));
}

void union_to_strbuilder_fwd(CodeUnion self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	strbuilder_append_str( result, txt("union ") );

	if ( self->Attributes )
		strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );

	if ( self->Name.Len )
	{
		strbuilder_append_fmt( result, "%S", self->Name);
	}

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		strbuilder_append_str( result, txt(";\n"));
}

StrBuilder using_to_string(CodeUsing self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 128 );
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

void using_to_strbuilder_ref(CodeUsing self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	if ( self->Attributes )
		strbuilder_append_fmt( result, "%SB ", attributes_to_string(self->Attributes) );

	if ( self->UnderlyingType )
	{
		strbuilder_append_fmt( result, "using %S = %SB", self->Name, typename_to_string(self->UnderlyingType) );

		if ( self->UnderlyingType->ArrExpr )
		{
			strbuilder_append_fmt( result, "[ %SB ]", code_to_string(self->UnderlyingType->ArrExpr) );

			Code next_arr_expr = self->UnderlyingType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				strbuilder_append_fmt( result, "[ %SB ]", code_to_string(next_arr_expr) );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		strbuilder_append_str( result, txt(";") );
	}
	else
		strbuilder_append_fmt( result, "using %S;", self->Name );

	if ( self->InlineCmt )
		strbuilder_append_fmt( result, "  %S\n", self->InlineCmt->Content );
	else
		strbuilder_append_str( result, txt("\n"));
}

inline
void using_to_strbuilder_ns(CodeUsing self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( self->InlineCmt )
		strbuilder_append_fmt( result, "using namespace $SC;  %S", self->Name, self->InlineCmt->Content );
	else
		strbuilder_append_fmt( result, "using namespace %S;\n", self->Name );
}

inline
StrBuilder var_to_string(CodeVar self)
{
	GEN_ASSERT(self);
	StrBuilder result = strbuilder_make_reserve( GlobalAllocator, 256 );
	var_to_strbuilder_ref( self, & result );
	return result;
}

neverinline
void var_to_strbuilder_ref(CodeVar self, StrBuilder* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( self->Parent && self->Parent->Type == CT_Variable )
	{
		// Its a comma-separated variable ( a NextVar )

		if ( self->Specs )
			strbuilder_append_fmt( result, "%SB ", specifiers_to_string(self->Specs) );

		strbuilder_append_str( result, self->Name );

		if ( self->ValueType->ArrExpr )
		{
			strbuilder_append_fmt( result, "[ %SB ]", code_to_string(self->ValueType->ArrExpr) );

			Code next_arr_expr = self->ValueType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				strbuilder_append_fmt( result, "[ %SB ]", code_to_string(next_arr_expr) );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		if ( self->Value )
		{
			if ( self->VarParenthesizedInit )
				strbuilder_append_fmt( result, "( %SB ", code_to_string(self->Value) );
			else
				strbuilder_append_fmt( result, " = %SB", code_to_string(self->Value) );
		}

		// Keep the chain going...
		if ( self->NextVar )
			strbuilder_append_fmt( result, ", %SB", var_to_string(self->NextVar) );

		if ( self->VarParenthesizedInit )
			strbuilder_append_str( result, txt(" )"));

		return;
	}

	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		strbuilder_append_str( result, txt("export ") );

	if ( self->Attributes || self->Specs )
	{
		if ( self->Attributes )
			strbuilder_append_fmt( result, "%SB ", specifiers_to_string(self->Specs) );

		if ( self->Specs )
			strbuilder_append_fmt( result, "%SB\n", specifiers_to_string(self->Specs) );

		strbuilder_append_fmt( result, "%SB %S", typename_to_string(self->ValueType), self->Name );

		if ( self->ValueType->ArrExpr )
		{
			strbuilder_append_fmt( result, "[ %SB ]", code_to_string(self->ValueType->ArrExpr) );

			Code next_arr_expr = self->ValueType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				strbuilder_append_fmt( result, "[ %SB ]", code_to_string(next_arr_expr) );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		if ( self->BitfieldSize )
			strbuilder_append_fmt( result, " : %SB", code_to_string(self->BitfieldSize) );

		if ( self->Value )
		{
			if ( self->VarParenthesizedInit )
				strbuilder_append_fmt( result, "( %SB ", code_to_string(self->Value) );
			else
				strbuilder_append_fmt( result, " = %SB", code_to_string(self->Value) );
		}

		if ( self->NextVar )
			strbuilder_append_fmt( result, ", %SB", var_to_string(self->NextVar) );

		if ( self->VarParenthesizedInit )
			strbuilder_append_str( result, txt(" )"));

		if ( self->InlineCmt )
			strbuilder_append_fmt( result, ";  %S", self->InlineCmt->Content);
		else
			strbuilder_append_str( result, txt(";\n") );

		return;
	}

	if ( self->BitfieldSize )
		strbuilder_append_fmt( result, "%SB %S : %SB", typename_to_string(self->ValueType), self->Name, code_to_string(self->BitfieldSize) );

	else if ( self->ValueType->ArrExpr )
	{
		strbuilder_append_fmt( result, "%SB %S[ %SB ]", typename_to_string(self->ValueType), self->Name, code_to_string(self->ValueType->ArrExpr) );

		Code next_arr_expr = self->ValueType->ArrExpr->Next;
		while ( next_arr_expr )
		{
			strbuilder_append_fmt( result, "[ %SB ]", code_to_string(next_arr_expr) );
			next_arr_expr = next_arr_expr->Next;
		}
	}

	else
		strbuilder_append_fmt( result, "%SB %S", typename_to_string(self->ValueType), self->Name );

	if ( self->Value )
	{
		if ( self->VarParenthesizedInit )
			strbuilder_append_fmt( result, "( %SB ", code_to_string(self->Value) );
		else
			strbuilder_append_fmt( result, " = %SB", code_to_string(self->Value) );
	}

	if ( self->NextVar )
		strbuilder_append_fmt( result, ", %SB", var_to_string( self->NextVar) );

	if ( self->VarParenthesizedInit )
		strbuilder_append_str( result, txt(" )"));

	strbuilder_append_str( result, txt(";") );

	if ( self->InlineCmt )
		strbuilder_append_fmt( result, "  %S", self->InlineCmt->Content);
	else
		strbuilder_append_str( result, txt("\n"));
}
