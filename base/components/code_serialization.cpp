#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "ast.cpp"
#endif

inline
String attributes_to_string(CodeAttributes attributes) {
	GEN_ASSERT(attributes);
	char* raw = ccast(char*, strc_duplicate( attributes->Content, GlobalAllocator ).Ptr);
	String result = { raw };
	return result;
}

inline
void attributes_to_string_ref(CodeAttributes attributes, String* result) {
	GEN_ASSERT(attributes);
	GEN_ASSERT(result);
	string_append_strc(result, attributes->Content);
}

String body_to_string(CodeBody body)
{
	GEN_ASSERT(body);
	String result = string_make_reserve( GlobalAllocator, 128 );
	switch ( body->Type )
	{
		case CT_Untyped:
		case CT_Execution:
			string_append_strc( & result, cast(Code, body)->Content );
		break;

		case CT_Enum_Body:
		case CT_Class_Body:
		case CT_Extern_Linkage_Body:
		case CT_Function_Body:
		case CT_Global_Body:
		case CT_Namespace_Body:
		case CT_Struct_Body:
		case CT_Union_Body:
			body_to_string_ref( body, & result );
		break;

		case CT_Export_Body:
			body_to_string_export( body, & result );
		break;
	}
	return result;
}

void body_to_string_ref( CodeBody body, String* result )
{
	GEN_ASSERT(body   != nullptr);
	GEN_ASSERT(result != nullptr);
	Code curr = body->Front;
	s32  left = body->NumEntries;
	while ( left -- )
	{
		code_to_string_ptr(curr, result);
		// string_append_fmt( result, "%S", code_to_string(curr) );
		++curr;
	}
}

void body_to_string_export( CodeBody body, String* result )
{
	GEN_ASSERT(body   != nullptr);
	GEN_ASSERT(result != nullptr);
	string_append_fmt( result, "export\n{\n" );

	Code curr = cast(Code, body);
	s32  left = body->NumEntries;
	while ( left-- )
	{
		code_to_string_ptr(curr, result);
		// string_append_fmt( result, "%S", code_to_string(curr) );
		++curr;
	}

	string_append_fmt( result, "};\n" );
}

inline
String comment_to_string(CodeComment comment) {
	GEN_ASSERT(comment);
	char* raw = ccast(char*, strc_duplicate( comment->Content, GlobalAllocator ).Ptr);
	String result = { raw };
	return result;
}

inline
void comment_to_string_ref(CodeComment comment, String* result) {
	GEN_ASSERT(comment);
	GEN_ASSERT(result);
	string_append_strc(result, comment->Content);
}

String constructor_to_string(CodeConstructor self)
{
	String result = string_make_reserve( GlobalAllocator, 128 );
	switch (self->Type)
	{
		case CT_Constructor:
			constructor_to_string_def( self, & result );
		break;
		case CT_Constructor_Fwd:
			constructor_to_string_fwd( self, & result );
		break;
	}
	return result;
}

void constructor_to_string_def(CodeConstructor self, String* result )
{
	Code ClassStructParent = self->Parent->Parent;
	if (ClassStructParent) {
		string_append_strc( result, ClassStructParent->Name );
	}
	else {
		string_append_strc( result, self->Name );
	}

	if ( self->Params )
		string_append_fmt( result, "( %S )", params_to_string(self->Params) );
	else
		string_append_strc( result, txt("()") );

	if ( self->InitializerList )
		string_append_fmt( result, " : %S", code_to_string(self->InitializerList) );

	if ( self->InlineCmt )
		string_append_fmt( result, " // %SC", self->InlineCmt->Content );

	string_append_fmt( result, "\n{\n%S\n}\n", code_to_string(self->Body) );
}

void constructor_to_string_fwd(CodeConstructor self, String* result )
{
	Code ClassStructParent = self->Parent->Parent;
	if (ClassStructParent) {
		string_append_strc( result, ClassStructParent->Name );
	}
	else {
		string_append_strc( result, self->Name );
	}

	if ( self->Params )
		string_append_fmt( result, "( %S )", params_to_string(self->Params) );
	else
		string_append_fmt( result, "()");

	if (self->Body)
		string_append_fmt( result, " = %S", code_to_string(self->Body) );

	if ( self->InlineCmt )
		string_append_fmt( result, "; // %SC\n", self->InlineCmt->Content );
	else
		string_append_strc( result, txt(";\n") );
}

String class_to_string( CodeClass self )
{
	String result = string_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Class:
			class_to_string_def(self, & result );
		break;
		case CT_Class_Fwd:
			class_to_string_fwd(self, & result );
		break;
	}
	return result;
}

void class_to_string_def( CodeClass self, String* result )
{
	GEN_ASSERT(self);

	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_strc( result, txt("class ") );

	if ( self->Attributes )
	{
		string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );
	}

	if ( self->ParentType )
	{
		StrC access_level = access_spec_to_str( self->ParentAccess );
		string_append_fmt( result, "%SC : %SC %S", self->Name, access_level, typename_to_string(self->ParentType) );

		CodeTypename interface = cast(CodeTypename, self->ParentType->Next);
		if ( interface )
			string_append_strc( result, txt("\n") );

		while ( interface )
		{
			string_append_fmt( result, ", %S", typename_to_string(interface) );
			interface = interface->Next ? cast(CodeTypename, interface->Next) : NullCode;
		}
	}
	else if ( self->Name.Len )
	{
		string_append_strc( result, self->Name );
	}

	if ( self->InlineCmt )
	{
		string_append_fmt( result, " // %SC", self->InlineCmt->Content );
	}

	string_append_fmt( result, "\n{\n%S\n}", body_to_string(self->Body) );

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		string_append_strc( result, txt(";\n") );
}

void class_to_string_fwd( CodeClass self, String* result )
{
	GEN_ASSERT(self);

	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes )
		string_append_fmt( result, "class %S %SC", attributes_to_string(self->Attributes), self->Name );

	else string_append_fmt( result, "class %SC", self->Name );

	// Check if it can have an end-statement
	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
	{
		if ( self->InlineCmt )
			string_append_fmt( result, "; // %SC\n", self->InlineCmt->Content );
		else
			string_append_strc( result, txt(";\n") );
	}
}

String define_to_string(CodeDefine define)
{
	return string_fmt_buf( GlobalAllocator, "#define %SC %SC", define->Name, define->Content );
}

void define_to_string_ref(CodeDefine define, String* result )
{
	string_append_fmt( result, "#define %SC %SC", define->Name, define->Content );
}

String destructor_to_string(CodeDestructor self)
{
	String result = string_make_reserve( GlobalAllocator, 128 );
	switch ( self->Type )
	{
		case CT_Destructor:
			destructor_to_string_def( self, & result );
		break;
		case CT_Destructor_Fwd:
			destructor_to_string_fwd( self, & result );
		break;
	}
	return result;
}

void destructor_to_string_def(CodeDestructor self, String* result )
{
	if ( self->Name.Len )
	{
		string_append_fmt( result, "%SC()", self->Name );
	}
	else if ( self->Specs )
	{
		if ( specifiers_has(self->Specs, Spec_Virtual ) )
			string_append_fmt( result, "virtual ~%SC()", self->Parent->Name );
		else
			string_append_fmt( result, "~%SC()", self->Parent->Name );
	}
	else
		string_append_fmt( result, "~%SC()", self->Parent->Name );

	string_append_fmt( result, "\n{\n%S\n}\n", code_to_string(self->Body) );
}

void destructor_to_string_fwd(CodeDestructor self, String* result )
{
	if ( self->Specs )
	{
		if ( specifiers_has(self->Specs, Spec_Virtual ) )
			string_append_fmt( result, "virtual ~%SC();\n", self->Parent->Name );
		else
			string_append_fmt( result, "~%SC()", self->Parent->Name );

		if ( specifiers_has(self->Specs, Spec_Pure ) )
			string_append_strc( result, txt(" = 0;") );
		else if (self->Body)
			string_append_fmt( result, " = %S;", code_to_string(self->Body) );
	}
	else
		string_append_fmt( result, "~%SC();", self->Parent->Name );

	if ( self->InlineCmt )
		string_append_fmt( result, "  %SC", self->InlineCmt->Content );
	else
		string_append_strc( result, txt("\n"));
}

String enum_to_string(CodeEnum self)
{
	String result = string_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Enum:
			enum_to_string_def(self, & result );
		break;
		case CT_Enum_Fwd:
			enum_to_string_fwd(self, & result );
		break;
		case CT_Enum_Class:
			enum_to_string_class_def(self, & result );
		break;
		case CT_Enum_Class_Fwd:
			enum_to_string_class_fwd(self, & result );
		break;
	}
	return result;
}

void enum_to_string_def(CodeEnum self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes || self->UnderlyingType || self->UnderlyingTypeMacro )
	{
		string_append_strc( result, txt("enum ") );

		if ( self->Attributes )
			string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );

		if ( self->UnderlyingType )
			string_append_fmt( result, "%SC : %S\n{\n%S\n}"
				, self->Name
				, typename_to_string(self->UnderlyingType)
				, body_to_string(self->Body)
			);
		else if ( self->UnderlyingTypeMacro )
			string_append_fmt( result, "%SC %S\n\n{\n%S\n}"
				, self->Name
				, code_to_string(self->UnderlyingTypeMacro)
				, body_to_string(self->Body)
			);

		else string_append_fmt( result, "%SC\n{\n%S\n}", self->Name, body_to_string(self->Body) );
	}
	else string_append_fmt( result, "enum %SC\n{\n%S\n}", self->Name, body_to_string(self->Body) );

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		string_append_strc( result, txt(";\n"));
}

void enum_to_string_fwd(CodeEnum self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );

	if ( self->UnderlyingType )
		string_append_fmt( result, "enum %SC : %S", self->Name, typename_to_string(self->UnderlyingType) );
	else if (self->UnderlyingTypeMacro)
	{
		log_fmt("IDENTIFIED A UNDERLYING ENUM MACRO");
		string_append_fmt( result, "enum %SC %S", self->Name, code_to_string(self->UnderlyingTypeMacro) );
	}
	else
		string_append_fmt( result, "enum %SC", self->Name );

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
	{
		if ( self->InlineCmt )
			string_append_fmt( result, ";  %SC", self->InlineCmt->Content );
		else
			string_append_strc( result, txt(";\n"));
	}
}

void enum_to_string_class_def(CodeEnum self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes || self->UnderlyingType )
	{
		string_append_strc( result, txt("enum class ") );

		if ( self->Attributes )
		{
			string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );
		}

		if ( self->UnderlyingType )
		{
			string_append_fmt( result, "%SC : %S\n{\n%S\n}", self->Name, typename_to_string(self->UnderlyingType), body_to_string(self->Body) );
		}
		else
		{
			string_append_fmt( result, "%SC\n{\n%S\n}", self->Name, body_to_string(self->Body) );
		}
	}
	else
	{
		string_append_fmt( result, "enum %SC\n{\n%S\n}", self->Name, body_to_string(self->Body) );
	}

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		string_append_strc( result, txt(";\n"));
}

void enum_to_string_class_fwd(CodeEnum self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_strc( result, txt("enum class ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );

	string_append_fmt( result, "%SC : %S", self->Name, typename_to_string(self->UnderlyingType) );

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
	{
		if ( self->InlineCmt )
			string_append_fmt( result, ";  %SC", self->InlineCmt->Content );
		else
			string_append_strc( result, txt(";\n"));
	}
}

String exec_to_string(CodeExec exec)
{
	GEN_ASSERT(exec);
	char* raw = ccast(char*, strc_duplicate( exec->Content, GlobalAllocator ).Ptr);
	String result = { raw };
	return result;
}

void extern_to_string(CodeExtern self, String* result )
{
	if ( self->Body )
		string_append_fmt( result, "extern \"%SC\"\n{\n%S\n}\n", self->Name, body_to_string(self->Body) );
	else
		string_append_fmt( result, "extern \"%SC\"\n{}\n", self->Name );
}

String include_to_string(CodeInclude include)
{
	return string_fmt_buf( GlobalAllocator, "#include %SC\n", include->Content );
}

void include_to_string_ref( CodeInclude include, String* result )
{
	string_append_fmt( result, "#include %SC\n", include->Content );
}

String friend_to_string(CodeFriend self)
{
	String result = string_make_reserve( GlobalAllocator, 256 );
	friend_to_string_ref( self, & result );
	return result;
}

void friend_to_string_ref(CodeFriend self, String* result )
{
	string_append_fmt( result, "friend %S", code_to_string(self->Declaration) );

	if ( self->Declaration->Type != CT_Function && self->Declaration->Type != CT_Operator && (* result)[ string_length(* result) - 1 ] != ';' )
	{
		string_append_strc( result, txt(";") );
	}

	if ( self->InlineCmt )
		string_append_fmt( result, "  %SC", self->InlineCmt->Content );
	else
		string_append_strc( result, txt("\n"));
}

String fn_to_string(CodeFn self)
{
	String result = string_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Function:
			fn_to_string_def(self, & result );
		break;
		case CT_Function_Fwd:
			fn_to_string_fwd(self, & result );
		break;
	}
	return result;
}

void fn_to_string_def(CodeFn self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export") );

	if ( self->Attributes )
		string_append_fmt( result, " %S ", attributes_to_string(self->Attributes) );

	bool prefix_specs = false;
	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( ! spec_is_trailing( * spec ) )
			{
				StrC spec_str = spec_to_str( * spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );

				prefix_specs = true;
			}
		}
	}

	if ( self->Attributes || prefix_specs )
		string_append_strc( result, txt("\n") );

	if ( self->ReturnType )
		string_append_fmt( result, "%S %SC(", typename_to_string(self->ReturnType), self->Name );

	else
		string_append_fmt( result, "%SC(", self->Name );

	if ( self->Params )
		string_append_fmt( result, "%S)", params_to_string(self->Params) );

	else
		string_append_strc( result, txt(")") );

	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( spec_is_trailing( * spec ) )
			{
				StrC spec_str = spec_to_str( * spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	string_append_fmt( result, "\n{\n%S\n}\n", body_to_string(self->Body) );
}

void fn_to_string_fwd(CodeFn self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );

	b32 prefix_specs = false;
	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( ! spec_is_trailing( * spec ) || ! ( * spec != Spec_Pure) )
			{
				StrC spec_str = spec_to_str( * spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );

				prefix_specs = true;
			}
		}
	}

	if ( self->Attributes || prefix_specs )
	{
		string_append_strc( result, txt("\n") );
	}

	if ( self->ReturnType )
		string_append_fmt( result, "%S %SC(", typename_to_string(self->ReturnType), self->Name );

	else
		string_append_fmt( result, "%SC(", self->Name );

	if ( self->Params )
		string_append_fmt( result, "%S)", params_to_string(self->Params) );

	else
		string_append_strc( result, txt(")") );

	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( spec_is_trailing( * spec ) )
			{
				StrC spec_str = spec_to_str( * spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->Specs && specifiers_has(self->Specs, Spec_Pure ) >= 0 )
		string_append_strc( result, txt(" = 0;") );
	else if (self->Body)
		string_append_fmt( result, " = %S;", body_to_string(self->Body) );

	if ( self->InlineCmt )
		string_append_fmt( result, ";  %SC", self->InlineCmt->Content );
	else
		string_append_strc( result, txt(";\n") );
}

String module_to_string(CodeModule self)
{
	String result = string_make_reserve( GlobalAllocator, 64 );
	module_to_string_ref( self, & result );
	return result;
}

void module_to_string_ref(CodeModule self, String* result )
{
	if (((scast(u32, ModuleFlag_Export) & scast(u32, self->ModuleFlags)) == scast(u32, ModuleFlag_Export)))
		string_append_strc( result, txt("export "));

	if (((scast(u32, ModuleFlag_Import) & scast(u32, self->ModuleFlags)) == scast(u32, ModuleFlag_Import)))
		string_append_strc( result, txt("import "));

	string_append_fmt( result, "%SC;\n", self->Name );
}

String namespace_to_string(CodeNS self)
{
	String result = string_make_reserve( GlobalAllocator, 512 );
	namespace_to_string_ref( self, & result );
	return result;
}

void namespace_to_string_ref(CodeNS self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_fmt( result, "namespace %SC\n{\n%S\n}\n", self->Name, body_to_string(self->Body) );
}

String code_op_to_string(CodeOperator self)
{
	String result = string_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Operator:
		case CT_Operator_Member:
			code_op_to_string_def( self, & result );
		break;
		case CT_Operator_Fwd:
		case CT_Operator_Member_Fwd:
			code_op_to_string_fwd( self, & result );
		break;
	}
	return result;
}

void code_op_to_string_def(CodeOperator self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );

	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( ! spec_is_trailing( * spec ) )
			{
				StrC spec_str = spec_to_str( * spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->Attributes || self->Specs )
	{
		string_append_strc( result, txt("\n") );
	}

	if ( self->ReturnType )
		string_append_fmt( result, "%S %SC (", typename_to_string(self->ReturnType), self->Name );

	if ( self->Params )
		string_append_fmt( result, "%S)", params_to_string(self->Params) );

	else
		string_append_strc( result, txt(")") );

	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( spec_is_trailing( * spec ) )
			{
				StrC spec_str = spec_to_str( * spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	string_append_fmt( result, "\n{\n%S\n}\n"
		, body_to_string(self->Body)
	);
}

void code_op_to_string_fwd(CodeOperator self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S\n", attributes_to_string(self->Attributes) );

	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( ! spec_is_trailing( * spec ) )
			{
				StrC spec_str = spec_to_str( * spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->Attributes || self->Specs )
	{
		string_append_strc( result, txt("\n") );
	}

	string_append_fmt( result, "%S %SC (", typename_to_string(self->ReturnType), self->Name );

	if ( self->Params )
		string_append_fmt( result, "%S)", params_to_string(self->Params) );

	else
		string_append_fmt( result, ")" );

	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( spec_is_trailing( * spec ) )
			{
				StrC spec_str = spec_to_str( * spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->InlineCmt )
		string_append_fmt( result, ";  %SC", self->InlineCmt->Content );
	else
		string_append_strc( result, txt(";\n") );
}

String opcast_to_string(CodeOpCast self)
{
	String result = string_make_reserve( GlobalAllocator, 128 );
	switch ( self->Type )
	{
		case CT_Operator_Cast:
			opcast_to_string_def(self, & result );
		break;
		case CT_Operator_Cast_Fwd:
			opcast_to_string_fwd(self, & result );
		break;
	}
	return result;
}

void opcast_to_string_def(CodeOpCast self, String* result )
{
	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( ! spec_is_trailing( * spec ) )
			{
				StrC spec_str = spec_to_str( * spec );
				string_append_fmt( result, "%*s ", spec_str.Len, spec_str.Ptr );
			}
		}

		if ( self->Name.Ptr && self->Name.Len )
			string_append_fmt( result, "%SC operator %S()", self->Name, typename_to_string(self->ValueType) );
		else
			string_append_fmt( result, "operator %S()", typename_to_string(self->ValueType) );

		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( spec_is_trailing( * spec ) )
			{
				StrC spec_str = spec_to_str( * spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}

		string_append_fmt( result, "\n{\n%S\n}\n", body_to_string(self->Body) );
		return;
	}

	if ( self->Name.Ptr && self->Name.Len )
		string_append_fmt( result, "%SC operator %S()\n{\n%S\n}\n", self->Name, typename_to_string(self->ValueType), body_to_string(self->Body) );
	else
		string_append_fmt( result, "operator %S()\n{\n%S\n}\n", typename_to_string(self->ValueType), body_to_string(self->Body) );
}

void opcast_to_string_fwd(CodeOpCast self, String* result )
{
	if ( self->Specs )
	{
		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( ! spec_is_trailing( * spec ) )
			{
				StrC spec_str = spec_to_str( * spec );
				string_append_fmt( result, "%*s ", spec_str.Len, spec_str.Ptr );
			}
		}

		string_append_fmt( result, "operator %S()", typename_to_string(self->ValueType) );

		for ( Specifier* spec = begin_CodeSpecifiers(self->Specs); spec != end_CodeSpecifiers(self->Specs); spec = next_CodeSpecifiers(self->Specs, spec) )
		{
			if ( spec_is_trailing( * spec ) )
			{
				StrC spec_str = spec_to_str( * spec );
				string_append_fmt( result, " %*s", spec_str.Len, spec_str.Ptr );
			}
		}

		if ( self->InlineCmt )
			string_append_fmt( result, ";  %SC", self->InlineCmt->Content );
		else
			string_append_strc( result, txt(";\n") );
		return;
	}

	if ( self->InlineCmt )
		string_append_fmt( result, "operator %S();  %S", typename_to_string(self->ValueType) );
	else
		string_append_fmt( result, "operator %S();\n", typename_to_string(self->ValueType) );
}

String params_to_string(CodeParam self)
{
	GEN_ASSERT(self);
	GEN_ASSERT(self);
	String result = string_make_reserve( GlobalAllocator, 128 );
	params_to_string_ref( self, & result );
	return result;
}

void params_to_string_ref( CodeParam self, String* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( self->Macro )
	{
		// Related to parsing: ( <macro>, ... )
		string_append_strc( result, self->Macro->Content );
		// Could also be: ( <macro> <type <name>, ... )
	}

	if ( self->Name.Ptr && self->Name.Len )
	{
		if ( self->ValueType == nullptr )
			string_append_fmt( result, " %SC", self->Name );
		else
			string_append_fmt( result, " %S %SC", typename_to_string(self->ValueType), self->Name );

	}
	else if ( self->ValueType )
		string_append_fmt( result, " %S", typename_to_string(self->ValueType) );

	if ( self->PostNameMacro )
	{
		string_append_fmt( result, " %S", code_to_string(self->PostNameMacro) );
	}

	if ( self->Value )
		string_append_fmt( result, " = %S", code_to_string(self->Value) );

	if ( self->NumEntries - 1 > 0 )
	{
		for ( CodeParam param = begin_CodeParam(self->Next); param != end_CodeParam(self->Next); param = next_CodeParam(self->Next, param) )
		{
			string_append_fmt( result, ", %S", params_to_string(param) );
		}
	}
}

String preprocess_to_string(CodePreprocessCond self)
{
	GEN_ASSERT(self);
	String result = string_make_reserve( GlobalAllocator, 256 );
	switch ( self->Type )
	{
		case CT_Preprocess_If:
			preprocess_to_string_if( self, & result );
		break;
		case CT_Preprocess_IfDef:
			preprocess_to_string_ifdef( self, & result );
		break;
		case CT_Preprocess_IfNotDef:
			preprocess_to_string_ifndef( self, & result );
		break;
		case CT_Preprocess_ElIf:
			preprocess_to_string_elif( self, & result );
		break;
		case CT_Preprocess_Else:
			preprocess_to_string_else( self, & result );
		break;
		case CT_Preprocess_EndIf:
			preprocess_to_string_endif( self, & result );
		break;
	}
	return result;
}

void preprocess_to_string_if(CodePreprocessCond cond, String* result )
{
	GEN_ASSERT(cond);
	string_append_fmt( result, "#if %SC", cond->Content );
}

void preprocess_to_string_ifdef(CodePreprocessCond cond, String* result )
{
	GEN_ASSERT(cond);
	string_append_fmt( result, "#ifdef %SC\n", cond->Content );
}

void preprocess_to_string_ifndef(CodePreprocessCond cond, String* result )
{
	GEN_ASSERT(cond);
	string_append_fmt( result, "#ifndef %SC", cond->Content );
}

void preprocess_to_string_elif(CodePreprocessCond cond, String* result )
{
	GEN_ASSERT(cond);
	string_append_fmt( result, "#elif %SC\n", cond->Content );
}

void preprocess_to_string_else(CodePreprocessCond cond, String* result )
{
	GEN_ASSERT(cond);
	string_append_strc( result, txt("#else\n") );
}

void preprocess_to_string_endif(CodePreprocessCond cond, String* result )
{
	GEN_ASSERT(cond);
	string_append_strc( result, txt("#endif\n") );
}

String pragma_to_string(CodePragma self)
{
	GEN_ASSERT(self);
	String result = string_make_reserve( GlobalAllocator, 256 );
	pragma_to_string_ref( self, & result );
	return result;
}

void pragma_to_string_ref(CodePragma self, String* result )
{
	string_append_fmt( result, "#pragma %SC\n", self->Content );
}

String specifiers_to_string(CodeSpecifiers self)
{
	String result = string_make_reserve( GlobalAllocator, 64 );
	specifiers_to_string_ref( self, & result );
	return result;
}

void specifiers_to_string_ref( CodeSpecifiers self, String* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	s32 idx  = 0;
	s32 left = self->NumEntries;
	while ( left-- )
	{
		StrC spec = spec_to_str( self->ArrSpecs[idx] );
		string_append_fmt( result, "%.*s ", spec.Len, spec.Ptr );
		idx++;
	}
}

String struct_to_string(CodeStruct self)
{
	GEN_ASSERT(self);
	GEN_ASSERT(self);
	String result = string_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Struct:
			struct_to_string_def( self, & result );
		break;
		case CT_Struct_Fwd:
			struct_to_string_fwd( self, & result );
		break;
	}
	return result;
}

void struct_to_string_def( CodeStruct self, String* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_strc( result, txt("struct ") );

	if ( self->Attributes )
	{
		string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );
	}

	if ( self->ParentType )
	{
		StrC access_level = access_spec_to_str( self->ParentAccess );

		string_append_fmt( result, "%SC : %SC %S", self->Name, access_level, typename_to_string(self->ParentType) );

		CodeTypename interface = cast(CodeTypename, self->ParentType->Next);
		if ( interface )
			string_append_strc( result, txt("\n") );

		while ( interface )
		{
			string_append_fmt( result, ", %S", typename_to_string(interface) );
			interface = interface->Next ? cast( CodeTypename, interface->Next) : NullCode;
		}
	}
	else if ( self->Name.Len )
	{
		string_append_strc( result, self->Name );
	}

	if ( self->InlineCmt )
	{
		string_append_fmt( result, " // %SC", self->InlineCmt->Content );
	}

	string_append_fmt( result, "\n{\n%S\n}", body_to_string(self->Body) );

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		string_append_strc( result, txt(";\n"));
}

void struct_to_string_fwd( CodeStruct self, String* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes )
		string_append_fmt( result, "struct %S %SC", attributes_to_string(self->Attributes), self->Name );

	else string_append_fmt( result, "struct %SC", self->Name );

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
	{
		if ( self->InlineCmt )
			string_append_fmt( result, ";  %SC", self->InlineCmt->Content );
		else
			string_append_strc( result, txt( ";\n") );
	}
}

String template_to_string(CodeTemplate self)
{
	GEN_ASSERT(self);
	String result = string_make_reserve( GlobalAllocator, 1024 );
	template_to_string_ref( self, & result );
	return result;
}

void template_to_string_ref(CodeTemplate self, String* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Params )
		string_append_fmt( result, "template< %S >\n%S", params_to_string(self->Params), code_to_string(self->Declaration) );
	else
		string_append_fmt( result, "template<>\n%S", code_to_string(self->Declaration) );
}

String typedef_to_string(CodeTypedef self)
{
	String result = string_make_reserve( GlobalAllocator, 128 );
	typedef_to_string_ref( self, & result );
	return result;
}

void typedef_to_string_ref(CodeTypedef self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_strc( result, txt("typedef "));

	// Determines if the typedef is a function typename
	if ( self->UnderlyingType->ReturnType )
		string_append_string( result, code_to_string(self->UnderlyingType) );
	else
		string_append_fmt( result, "%S %SC", code_to_string(self->UnderlyingType), self->Name );

	if ( self->UnderlyingType->Type == CT_Typename && self->UnderlyingType->ArrExpr )
	{
		string_append_fmt( result, "[ %S ];", code_to_string(self->UnderlyingType->ArrExpr) );

		Code next_arr_expr = self->UnderlyingType->ArrExpr->Next;
		while ( next_arr_expr )
		{
			string_append_fmt( result, "[ %S ];", code_to_string(next_arr_expr) );
			next_arr_expr = next_arr_expr->Next;
		}
	}
	else
	{
		string_append_strc( result, txt(";") );
	}

	if ( self->InlineCmt )
		string_append_fmt( result, "  %SC", self->InlineCmt->Content);
	else
		string_append_strc( result, txt("\n"));
}

String typename_to_string(CodeTypename self)
{
	String result = string_make_strc( GlobalAllocator, txt("") );
	typename_to_string_ref( self, & result );
	return result;
}

void typename_to_string_ref(CodeTypename self, String* result )
{
	#if defined(GEN_USE_NEW_TYPENAME_PARSING)
		if ( self->ReturnType && self->Params )
		{
			if ( self->Attributes )
				string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );
			else
			{
				if ( self->Specs )
					string_append_fmt( result, "%S ( %SC ) ( %S ) %S", typename_to_string(self->ReturnType), self->Name, params_to_string(self->Params), specifiers_to_string(self->Specs) );
				else
					string_append_fmt( result, "%S ( %SC ) ( %S )", typename_to_string(self->ReturnType), self->Name, params_to_string(self->Params) );
			}

			break;
		}
	#else
		if ( self->ReturnType && self->Params )
		{
			if ( self->Attributes )
				string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );
			else
			{
				if ( self->Specs )
					string_append_fmt( result, "%S %SC ( %S ) %S", typename_to_string(self->ReturnType), self->Name, params_to_string(self->Params), specifiers_to_string(self->Specs) );
				else
					string_append_fmt( result, "%S %SC ( %S )", typename_to_string(self->ReturnType), self->Name, params_to_string(self->Params) );
			}

			return;
		}
	#endif

	if ( self->Attributes )
		string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );

	switch ( self->TypeTag )
	{
		case Tag_Class  : string_append_strc( result, txt("class "));  break;
		case Tag_Enum   : string_append_strc( result, txt("enum "));   break;
		case Tag_Struct : string_append_strc( result, txt("struct ")); break;
		case Tag_Union  : string_append_strc( result, txt("union "));  break;
		default:
			break;
	}

	if ( self->Specs )
		string_append_fmt( result, "%SC %S", self->Name, specifiers_to_string(self->Specs) );
	else
		string_append_fmt( result, "%SC", self->Name );

	if ( self->IsParamPack )
		string_append_strc( result, txt("..."));
}

String union_to_string(CodeUnion self)
{
	String result = string_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Union:
			union_to_string_def( self, & result );
		break;
		case CT_Union_Fwd:
			union_to_string_fwd( self, & result );
		break;
	}
	return result;
}

void union_to_string_def(CodeUnion self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_strc( result, txt("union ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );

	if ( self->Name.Len )
	{
		string_append_fmt( result, "%SC\n{\n%S\n}"
			, self->Name
			, body_to_string(self->Body)
		);
	}
	else
	{
		// Anonymous union
		string_append_fmt( result, "\n{\n%S\n}"
			, body_to_string(self->Body)
		);
	}

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		string_append_strc( result, txt(";\n"));
}

void union_to_string_fwd(CodeUnion self, String* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_strc( result, txt("union ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );

	if ( self->Name.Len )
	{
		string_append_fmt( result, "%SC", self->Name);
	}

	if ( self->Parent == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		string_append_strc( result, txt(";\n"));
}

String using_to_string(CodeUsing self)
{
	GEN_ASSERT(self);
	String result = string_make_reserve( GlobalAllocator, 128 );
	switch ( self->Type )
	{
		case CT_Using:
			using_to_string_ref( self, & result );
		break;
		case CT_Using_Namespace:
			using_to_string_ns( self, & result );
		break;
	}
	return result;
}

void using_to_string_ref(CodeUsing self, String* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", attributes_to_string(self->Attributes) );

	if ( self->UnderlyingType )
	{
		string_append_fmt( result, "using %SC = %S", self->Name, typename_to_string(self->UnderlyingType) );

		if ( self->UnderlyingType->ArrExpr )
		{
			string_append_fmt( result, "[ %S ]", code_to_string(self->UnderlyingType->ArrExpr) );

			Code next_arr_expr = self->UnderlyingType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				string_append_fmt( result, "[ %S ]", code_to_string(next_arr_expr) );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		string_append_strc( result, txt(";") );
	}
	else
		string_append_fmt( result, "using %SC;", self->Name );

	if ( self->InlineCmt )
		string_append_fmt( result, "  %SC\n", self->InlineCmt->Content );
	else
		string_append_strc( result, txt("\n"));
}

inline
void using_to_string_ns(CodeUsing self, String* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( self->InlineCmt )
		string_append_fmt( result, "using namespace $SC;  %SC", self->Name, self->InlineCmt->Content );
	else
		string_append_fmt( result, "using namespace %SC;\n", self->Name );
}

inline
String var_to_string(CodeVar self)
{
	GEN_ASSERT(self);
	String result = string_make_reserve( GlobalAllocator, 256 );
	var_to_string_ref( self, & result );
	return result;
}

neverinline
void var_to_string_ref(CodeVar self, String* result )
{
	GEN_ASSERT(self);
	GEN_ASSERT(result);
	if ( self->Parent && self->Parent->Type == CT_Variable )
	{
		// Its a comma-separated variable ( a NextVar )

		if ( self->Specs )
			string_append_fmt( result, "%S ", specifiers_to_string(self->Specs) );

		string_append_strc( result, self->Name );

		if ( self->ValueType->ArrExpr )
		{
			string_append_fmt( result, "[ %S ]", code_to_string(self->ValueType->ArrExpr) );

			Code next_arr_expr = self->ValueType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				string_append_fmt( result, "[ %S ]", code_to_string(next_arr_expr) );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		if ( self->Value )
		{
			if ( self->VarConstructorInit )
				string_append_fmt( result, "( %S ", code_to_string(self->Value) );
			else
				string_append_fmt( result, " = %S", code_to_string(self->Value) );
		}

		// Keep the chain going...
		if ( self->NextVar )
			string_append_fmt( result, ", %S", var_to_string(self->NextVar) );

		if ( self->VarConstructorInit )
			string_append_strc( result, txt(" )"));

		return;
	}

	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes || self->Specs )
	{
		if ( self->Attributes )
			string_append_fmt( result, "%S ", specifiers_to_string(self->Specs) );

		if ( self->Specs )
			string_append_fmt( result, "%S\n", specifiers_to_string(self->Specs) );

		string_append_fmt( result, "%S %SC", typename_to_string(self->ValueType), self->Name );

		if ( self->ValueType->ArrExpr )
		{
			string_append_fmt( result, "[ %S ]", code_to_string(self->ValueType->ArrExpr) );

			Code next_arr_expr = self->ValueType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				string_append_fmt( result, "[ %S ]", code_to_string(next_arr_expr) );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		if ( self->BitfieldSize )
			string_append_fmt( result, " : %S", code_to_string(self->BitfieldSize) );

		if ( self->Value )
		{
			if ( self->VarConstructorInit )
				string_append_fmt( result, "( %S ", code_to_string(self->Value) );
			else
				string_append_fmt( result, " = %S", code_to_string(self->Value) );
		}

		if ( self->NextVar )
			string_append_fmt( result, ", %S", var_to_string(self->NextVar) );

		if ( self->VarConstructorInit )
			string_append_strc( result, txt(" )"));

		if ( self->InlineCmt )
			string_append_fmt( result, ";  %SC", self->InlineCmt->Content);
		else
			string_append_strc( result, txt(";\n") );

		return;
	}

	if ( self->BitfieldSize )
		string_append_fmt( result, "%S %SC : %S", typename_to_string(self->ValueType), self->Name, code_to_string(self->BitfieldSize) );

	else if ( self->ValueType->ArrExpr )
	{
		string_append_fmt( result, "%S %SC[ %S ]", typename_to_string(self->ValueType), self->Name, code_to_string(self->ValueType->ArrExpr) );

		Code next_arr_expr = self->ValueType->ArrExpr->Next;
		while ( next_arr_expr )
		{
			string_append_fmt( result, "[ %S ]", code_to_string(next_arr_expr) );
			next_arr_expr = next_arr_expr->Next;
		}
	}

	else
		string_append_fmt( result, "%S %SC", typename_to_string(self->ValueType), self->Name );

	if ( self->Value )
	{
		if ( self->VarConstructorInit )
			string_append_fmt( result, "( %S ", code_to_string(self->Value) );
		else
			string_append_fmt( result, " = %S", code_to_string(self->Value) );
	}

	if ( self->NextVar )
		string_append_fmt( result, ", %S", var_to_string( self->NextVar) );

	if ( self->VarConstructorInit )
		string_append_strc( result, txt(" )"));

	string_append_strc( result, txt(";") );

	if ( self->InlineCmt )
		string_append_fmt( result, "  %SC", self->InlineCmt->Content);
	else
		string_append_strc( result, txt("\n"));
}
