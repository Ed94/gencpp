#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "ast.cpp"
#endif

String to_string(CodeAttributes attributes) {
	return {(char*) strc_duplicate( attributes->Content, GlobalAllocator ).Ptr};
}

String to_string(CodeBody body)
{
	GEN_ASSERT(body.ast != nullptr);
	String result = string_make_reserve( GlobalAllocator, 128 );
	switch ( body.ast->Type )
	{
		case CT_Untyped:
		case CT_Execution:
			string_append_strc( & result, rcast(AST*, body.ast)->Content );
		break;

		case CT_Enum_Body:
		case CT_Class_Body:
		case CT_Extern_Linkage_Body:
		case CT_Function_Body:
		case CT_Global_Body:
		case CT_Namespace_Body:
		case CT_Struct_Body:
		case CT_Union_Body:
			to_string( body, & result );
		break;

		case CT_Export_Body:
			to_string_export( body, & result );
		break;
	}
	return result;
}

void to_string( CodeBody body, String* result )
{
	GEN_ASSERT(body.ast != nullptr);
	GEN_ASSERT(result   != nullptr);
	Code curr = body.ast->Front;
	s32  left = body.ast->NumEntries;
	while ( left -- )
	{
		string_append_fmt( result, "%S", GEN_NS to_string(curr) );
		++curr;
	}
}

void to_string_export( CodeBody body, String* result )
{
	GEN_ASSERT(body.ast != nullptr);
	GEN_ASSERT(result   != nullptr);
	string_append_fmt( result, "export\n{\n" );

	Code curr = body;
	s32  left = body.ast->NumEntries;
	while ( left-- )
	{
		string_append_fmt( result, "%S", to_string(curr) );
		++curr;
	}

	string_append_fmt( result, "};\n" );
}

String to_string(CodeComment comment)
{
	return {(char*) strc_duplicate( comment->Content, GlobalAllocator ).Ptr};
}

String to_string(CodeConstructor self)
{
	String result = string_make_reserve( GlobalAllocator, 128 );
	switch (self->Type)
	{
		case CT_Constructor:
			to_string_def( self, & result );
		break;
		case CT_Constructor_Fwd:
			to_string_fwd( self, & result );
		break;
	}
	return result;
}

void to_string_def(CodeConstructor self, String* result )
{
	Code ClassStructParent = self->Parent->Parent;
	if (ClassStructParent) {
		string_append_strc( result, ClassStructParent->Name );
	}
	else {
		string_append_strc( result, self->Name );
	}

	if ( self->Params )
		string_append_fmt( result, "( %S )", to_string(self->Params) );
	else
		string_append_strc( result, txt("()") );

	if ( self->InitializerList )
		string_append_fmt( result, " : %S", to_string(self->InitializerList) );

	if ( self->InlineCmt )
		string_append_fmt( result, " // %SC", self->InlineCmt->Content );

	string_append_fmt( result, "\n{\n%S\n}\n", to_string(self->Body) );
}

void to_string_fwd(CodeConstructor self, String* result )
{
	Code ClassStructParent = self->Parent->Parent;
	if (ClassStructParent) {
		string_append_strc( result, ClassStructParent->Name );
	}
	else {
		string_append_strc( result, self->Name );
	}

	if ( self->Params )
		string_append_fmt( result, "( %S )", to_string(self->Params) );
	else
		string_append_fmt( result, "()");

	if (self->Body)
		string_append_fmt( result, " = %S", to_string(self->Body) );

	if ( self->InlineCmt )
		string_append_fmt( result, "; // %SC\n", self->InlineCmt->Content );
	else
		string_append_strc( result, txt(";\n") );
}

String to_string( CodeClass self )
{
	String result = string_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Class:
			to_string_def(self, & result );
		break;
		case CT_Class_Fwd:
			to_string_fwd(self, & result );
		break;
	}
	return result;
}

void to_string_def( CodeClass self, String* result )
{
	GEN_ASSERT(self.ast != nullptr);
	AST_Class* ast = self.ast;

	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_strc( result, txt("class ") );

	if ( ast->Attributes )
	{
		string_append_fmt( result, "%S ", GEN_NS to_string(ast->Attributes) );
	}

	if ( ast->ParentType )
	{
		char const* access_level = to_str( ast->ParentAccess );

		string_append_fmt( result, "%SC : %s %S", ast->Name, access_level, to_string(ast->ParentType) );

		CodeTypename interface = cast(CodeTypename, ast->ParentType->Next);
		if ( interface )
			string_append_strc( result, txt("\n") );

		while ( interface )
		{
			string_append_fmt( result, ", %S", to_string(interface) );
			interface = interface->Next ? cast(CodeTypename, interface->Next) : CodeTypename { nullptr };
		}
	}
	else if ( ast->Name )
	{
		string_append_strc( result, ast->Name );
	}

	if ( ast->InlineCmt )
	{
		string_append_fmt( result, " // %SC", ast->InlineCmt->Content );
	}

	string_append_fmt( result, "\n{\n%S\n}", to_string(ast->Body) );

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != CT_Typedef && ast->Parent->Type != CT_Variable ) )
		string_append_strc( result, txt(";\n") );
}

void to_string_fwd( CodeClass self, String* result )
{
	GEN_ASSERT(self.ast != nullptr);
	AST_Class* ast = self.ast;

	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( ast->Attributes )
		string_append_fmt( result, "class %S %SC", to_string(ast->Attributes), ast->Name );

	else string_append_fmt( result, "class %SC", ast->Name );

	// Check if it can have an end-statement
	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != CT_Typedef && ast->Parent->Type != CT_Variable ) )
	{
		if ( ast->InlineCmt )
			string_append_fmt( result, "; // %SC\n", ast->InlineCmt->Content );
		else
			string_append_strc( result, txt(";\n") );
	}
}

String to_string(CodeDefine define)
{
	return string_fmt_buf( GlobalAllocator, "#define %SC %SC\n", define->Name, define->Content );
}

void to_string(CodeDefine define, String* result )
{
	string_append_fmt( result, "#define %SC %SC\n", define->Name, define->Content );
}

String to_string(CodeDestructor self)
{
	String result = string_make_reserve( GlobalAllocator, 128 );
	switch ( self->Type )
	{
		case CT_Destructor:
			to_string_def( self, & result );
		break;
		case CT_Destructor_Fwd:
			to_string_fwd( self, & result );
		break;
	}
	return result;
}

void to_string_def(CodeDestructor self, String* result )
{
	if ( self->Name )
	{
		string_append_fmt( result, "%SC()", self->Name );
	}
	else if ( self->Specs )
	{
		if ( has(self->Specs, Spec_Virtual ) )
			string_append_fmt( result, "virtual ~%SC()", self->Parent->Name );
		else
			string_append_fmt( result, "~%SC()", self->Parent->Name );
	}
	else
		string_append_fmt( result, "~%SC()", self->Parent->Name );

	string_append_fmt( result, "\n{\n%S\n}\n", to_string(self->Body) );
}

void to_string_fwd(CodeDestructor self, String* result )
{
	if ( self->Specs )
	{
		if ( has(self->Specs, Spec_Virtual ) )
			string_append_fmt( result, "virtual ~%SC();\n", self->Parent->Name );
		else
			string_append_fmt( result, "~%SC()", self->Parent->Name );

		if ( has(self->Specs, Spec_Pure ) )
			string_append_strc( result, txt(" = 0;") );
		else if (self->Body)
			string_append_fmt( result, " = %S;", to_string(self->Body) );
	}
	else
		string_append_fmt( result, "~%SC();", self->Parent->Name );

	if ( self->InlineCmt )
		string_append_fmt( result, "  %SC", self->InlineCmt->Content );
	else
		string_append_strc( result, txt("\n"));
}

String to_string(CodeEnum self)
{
	String result = string_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Enum:
			to_string_def(self, & result );
		break;
		case CT_Enum_Fwd:
			to_string_fwd(self, & result );
		break;
		case CT_Enum_Class:
			to_string_class_def(self, & result );
		break;
		case CT_Enum_Class_Fwd:
			to_string_class_fwd(self, & result );
		break;
	}
	return result;
}

void to_string_def(CodeEnum self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes || self->UnderlyingType )
	{
		string_append_strc( result, txt("enum ") );

		if ( self->Attributes )
			string_append_fmt( result, "%S ", to_string(self->Attributes) );

		if ( self->UnderlyingType )
			string_append_fmt( result, "%SC : %S\n{\n%S\n}"
				, self->Name
				, to_string(self->UnderlyingType)
				, to_string(self->Body)
			);
		else if ( self->UnderlyingTypeMacro )
			string_append_fmt( result, "%SC : %S\n{\n%S\n}"
				, self->Name
				, to_string(self->UnderlyingTypeMacro)
				, to_string(self->Body)
			);

		else string_append_fmt( result, "%SC\n{\n%S\n}", self->Name, to_string(self->Body) );
	}
	else string_append_fmt( result, "enum %SC\n{\n%S\n}", self->Name, to_string(self->Body) );

	if ( self->Parent.ast == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		string_append_strc( result, txt(";\n"));
}

void to_string_fwd(CodeEnum self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", to_string(self->Attributes) );

	if ( self->UnderlyingType )
		string_append_fmt( result, "enum %SC : %S", self->Name, to_string(self->UnderlyingType) );
	else
		string_append_fmt( result, "enum %SC", self->Name );

	if ( self->Parent.ast == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
	{
		if ( self->InlineCmt )
			string_append_fmt( result, ";  %SC", self->InlineCmt->Content );
		else
			string_append_strc( result, txt(";\n"));
	}
}

void to_string_class_def(CodeEnum self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes || self->UnderlyingType )
	{
		string_append_strc( result, txt("enum class ") );

		if ( self->Attributes )
		{
			string_append_fmt( result, "%S ", to_string(self->Attributes) );
		}

		if ( self->UnderlyingType )
		{
			string_append_fmt( result, "%SC : %S\n{\n%S\n}", self->Name, to_string(self->UnderlyingType), to_string(self->Body) );
		}
		else
		{
			string_append_fmt( result, "%SC\n{\n%S\n}", self->Name, to_string(self->Body) );
		}
	}
	else
	{
		string_append_fmt( result, "enum %SC\n{\n%S\n}", self->Name, to_string(self->Body) );
	}

	if ( self->Parent.ast == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		string_append_strc( result, txt(";\n"));
}

void to_string_class_fwd(CodeEnum self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_strc( result, txt("enum class ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", to_string(self->Attributes) );

	string_append_fmt( result, "%SC : %S", self->Name, to_string(self->UnderlyingType) );

	if ( self->Parent.ast == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
	{
		if ( self->InlineCmt )
			string_append_fmt( result, ";  %SC", self->InlineCmt->Content );
		else
			string_append_strc( result, txt(";\n"));
	}
}

String to_string(CodeExec exec)
{
	return {(char*) strc_duplicate( exec->Content, GlobalAllocator ).Ptr};
}

void to_string(CodeExtern self, String* result )
{
	if ( self->Body )
		string_append_fmt( result, "extern \"%SC\"\n{\n%S\n}\n", self->Name, to_string(self->Body) );
	else
		string_append_fmt( result, "extern \"%SC\"\n{}\n", self->Name );
}

String to_string(CodeInclude include)
{
	return string_fmt_buf( GlobalAllocator, "#include %SC\n", include->Content );
}

void to_string( CodeInclude include, String* result )
{
	string_append_fmt( result, "#include %SC\n", include->Content );
}

String to_string(CodeFriend self)
{
	String result = string_make_reserve( GlobalAllocator, 256 );
	to_string( self, & result );
	return result;
}

void to_string(CodeFriend self, String* result )
{
	string_append_fmt( result, "friend %S", to_string(self->Declaration) );

	if ( self->Declaration->Type != CT_Function && self->Declaration->Type != CT_Operator && (* result)[ string_length(* result) - 1 ] != ';' )
	{
		string_append_strc( result, txt(";") );
	}

	if ( self->InlineCmt )
		string_append_fmt( result, "  %SC", self->InlineCmt->Content );
	else
		string_append_strc( result, txt("\n"));
}

String to_string(CodeFn self)
{
	String result = string_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Function:
			to_string_def(self, & result );
		break;
		case CT_Function_Fwd:
			to_string_fwd(self, & result );
		break;
	}
	return result;
}

void to_string_def(CodeFn self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export") );

	if ( self->Attributes )
		string_append_fmt( result, " %S ", to_string(self->Attributes) );

	bool prefix_specs = false;
	if ( self->Specs )
	{
		for ( Specifier spec : self->Specs )
		{
			if ( ! is_trailing( spec ) )
			{
				StrC spec_str = to_str( spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );

				prefix_specs = true;
			}
		}
	}

	if ( self->Attributes || prefix_specs )
		string_append_strc( result, txt("\n") );

	if ( self->ReturnType )
		string_append_fmt( result, "%S %SC(", to_string(self->ReturnType), self->Name );

	else
		string_append_fmt( result, "%SC(", self->Name );

	if ( self->Params )
		string_append_fmt( result, "%S)", to_string(self->Params) );

	else
		string_append_strc( result, txt(")") );

	if ( self->Specs )
	{
		for ( Specifier spec : self->Specs )
		{
			if ( is_trailing( spec ) )
			{
				StrC spec_str = to_str( spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	string_append_fmt( result, "\n{\n%S\n}\n", to_string(self->Body) );
}

void to_string_fwd(CodeFn self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", to_string(self->Attributes) );

	b32 prefix_specs = false;
	if ( self->Specs )
	{
		for ( Specifier spec : self->Specs )
		{
			if ( ! is_trailing( spec ) || ! (spec != Spec_Pure) )
			{
				StrC spec_str = to_str( spec );
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
		string_append_fmt( result, "%S %SC(", to_string(self->ReturnType), self->Name );

	else
		string_append_fmt( result, "%SC(", self->Name );

	if ( self->Params )
		string_append_fmt( result, "%S)", to_string(self->Params) );

	else
		string_append_strc( result, txt(")") );

	if ( self->Specs )
	{
		for ( Specifier spec : self->Specs )
		{
			if ( is_trailing( spec ) )
			{
				StrC spec_str = to_str( spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->Specs && has(self->Specs, Spec_Pure ) >= 0 )
		string_append_strc( result, txt(" = 0;") );
	else if (self->Body)
		string_append_fmt( result, " = %S;", to_string(self->Body) );

	if ( self->InlineCmt )
		string_append_fmt( result, ";  %SC", self->InlineCmt->Content );
	else
		string_append_strc( result, txt(";\n") );
}

String to_string(CodeModule self)
{
	String result = string_make_reserve( GlobalAllocator, 64 );
	to_string( self, & result );
	return result;
}

void to_string(CodeModule self, String* result )
{
	if (((u32(ModuleFlag_Export) & u32(self->ModuleFlags)) == u32(ModuleFlag_Export)))
		string_append_strc( result, txt("export "));

	if (((u32(ModuleFlag_Import) & u32(self->ModuleFlags)) == u32(ModuleFlag_Import)))
		string_append_strc( result, txt("import "));

	string_append_fmt( result, "%SC;\n", self->Name );
}

String to_string(CodeNS self)
{
	String result = string_make_reserve( GlobalAllocator, 512 );
	to_string( self, & result );
	return result;
}

void to_string(CodeNS self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_fmt( result, "namespace %SC\n{\n%S\n}\n", self->Name, to_string(self->Body) );
}

String to_string(CodeOperator self)
{
	String result = string_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Operator:
		case CT_Operator_Member:
			to_string_def( self, & result );
		break;
		case CT_Operator_Fwd:
		case CT_Operator_Member_Fwd:
			to_string_fwd( self, & result );
		break;
	}
	return result;
}

void to_string_def(CodeOperator self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", to_string(self->Attributes) );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", to_string(self->Attributes) );

	if ( self->Specs )
	{
		for ( Specifier spec : self->Specs )
		{
			if ( ! is_trailing( spec ) )
			{
				StrC spec_str = to_str( spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->Attributes || self->Specs )
	{
		string_append_strc( result, txt("\n") );
	}

	if ( self->ReturnType )
		string_append_fmt( result, "%S %SC (", to_string(self->ReturnType), self->Name );

	if ( self->Params )
		string_append_fmt( result, "%S)", to_string(self->Params) );

	else
		string_append_strc( result, txt(")") );

	if ( self->Specs )
	{
		for ( Specifier spec : self->Specs )
		{
			if ( is_trailing( spec ) )
			{
				StrC spec_str = to_str( spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	string_append_fmt( result, "\n{\n%S\n}\n"
		, to_string(self->Body)
	);
}

void to_string_fwd(CodeOperator self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S\n", to_string(self->Attributes) );

	if ( self->Specs )
	{
		for ( Specifier spec : self->Specs )
		{
			if ( ! is_trailing( spec ) )
			{
				StrC spec_str = to_str( spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->Attributes || self->Specs )
	{
		string_append_strc( result, txt("\n") );
	}

	string_append_fmt( result, "%S %SC (", to_string(self->ReturnType), self->Name );

	if ( self->Params )
		string_append_fmt( result, "%S)", to_string(self->Params) );

	else
		string_append_fmt( result, ")" );

	if ( self->Specs )
	{
		for ( Specifier spec : self->Specs )
		{
			if ( is_trailing( spec ) )
			{
				StrC spec_str = to_str( spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->InlineCmt )
		string_append_fmt( result, ";  %SC", self->InlineCmt->Content );
	else
		string_append_strc( result, txt(";\n") );
}

String to_string(CodeOpCast self)
{
	String result = string_make_reserve( GlobalAllocator, 128 );
	switch ( self->Type )
	{
		case CT_Operator_Cast:
			to_string_def(self, & result );
		break;
		case CT_Operator_Cast_Fwd:
			to_string_fwd(self, & result );
		break;
	}
	return result;
}

void to_string_def(CodeOpCast self, String* result )
{
	if ( self->Specs )
	{
		for ( Specifier spec : self->Specs )
		{
			if ( ! is_trailing( spec ) )
			{
				StrC spec_str = to_str( spec );
				string_append_fmt( result, "%*s ", spec_str.Len, spec_str.Ptr );
			}
		}

		if ( self->Name && self->Name.Len )
			string_append_fmt( result, "%SC operator %S()", self->Name, to_string(self->ValueType) );
		else
			string_append_fmt( result, "operator %S()", to_string(self->ValueType) );

		for ( Specifier spec : self->Specs )
		{
			if ( is_trailing( spec ) )
			{
				StrC spec_str = to_str( spec );
				string_append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}

		string_append_fmt( result, "\n{\n%S\n}\n", to_string(self->Body) );
		return;
	}

	if ( self->Name && self->Name.Len )
		string_append_fmt( result, "%SC operator %S()\n{\n%S\n}\n", self->Name, to_string(self->ValueType), to_string(self->Body) );
	else
		string_append_fmt( result, "operator %S()\n{\n%S\n}\n", to_string(self->ValueType), to_string(self->Body) );
}

void to_string_fwd(CodeOpCast self, String* result )
{
	if ( self->Specs )
	{
		for ( Specifier spec : self->Specs )
		{
			if ( ! is_trailing( spec ) )
			{
				StrC spec_str = to_str( spec );
				string_append_fmt( result, "%*s ", spec_str.Len, spec_str.Ptr );
			}
		}

		string_append_fmt( result, "operator %S()", to_string(self->ValueType) );

		for ( Specifier spec : self->Specs )
		{
			if ( is_trailing( spec ) )
			{
				StrC spec_str = to_str( spec );
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
		string_append_fmt( result, "operator %S();  %S", to_string(self->ValueType) );
	else
		string_append_fmt( result, "operator %S();\n", to_string(self->ValueType) );
}

String to_string(CodeParam self)
{
	String result = string_make_reserve( GlobalAllocator, 128 );
	to_string( self, & result );
	return result;
}

void to_string( CodeParam self, String* result )
{
	AST_Param* ast = self.ast;
	if ( ast->Macro )
	{
		// Related to parsing: ( <macro>, ... )
		string_append_strc( result, ast->Macro.ast->Content );
		// Could also be: ( <macro> <type <name>, ... )
	}

	if ( ast->Name )
	{
		if ( ast->ValueType.ast == nullptr )
			string_append_fmt( result, " %SC", ast->Name );
		else
			string_append_fmt( result, " %S %SC", to_string(ast->ValueType), ast->Name );

	}
	else if ( ast->ValueType )
		string_append_fmt( result, " %S", to_string(ast->ValueType) );

	if ( ast->PostNameMacro )
	{
		string_append_fmt( result, " %S", to_string(ast->PostNameMacro) );
	}

	if ( ast->Value )
		string_append_fmt( result, " = %S", to_string(ast->Value) );

	if ( ast->NumEntries - 1 > 0 )
	{
		for ( CodeParam param : ast->Next )
		{
			string_append_fmt( result, ", %S", to_string(param) );
		}
	}
}

String to_string(CodePreprocessCond self)
{
	String result = string_make_reserve( GlobalAllocator, 256 );
	switch ( self->Type )
	{
		case CT_Preprocess_If:
			to_string_if( self, & result );
		break;
		case CT_Preprocess_IfDef:
			to_string_ifdef( self, & result );
		break;
		case CT_Preprocess_IfNotDef:
			to_string_ifndef( self, & result );
		break;
		case CT_Preprocess_ElIf:
			to_string_elif( self, & result );
		break;
		case CT_Preprocess_Else:
			to_string_else( self, & result );
		break;
		case CT_Preprocess_EndIf:
			to_string_endif( self, & result );
		break;
	}
	return result;
}

void to_string_if(CodePreprocessCond cond, String* result )
{
	string_append_fmt( result, "#if %SC\n", cond->Content );
}

void to_string_ifdef(CodePreprocessCond cond, String* result )
{
	string_append_fmt( result, "#ifdef %SC\n", cond->Content );
}

void to_string_ifndef(CodePreprocessCond cond, String* result )
{
	string_append_fmt( result, "#ifndef %SC\n", cond->Content );
}

void to_string_elif(CodePreprocessCond cond, String* result )
{
	string_append_fmt( result, "#elif %SC\n", cond->Content );
}

void to_string_else(CodePreprocessCond cond, String* result )
{
	string_append_strc( result, txt("#else\n") );
}

void to_string_endif(CodePreprocessCond cond, String* result )
{
	string_append_strc( result, txt("#endif\n") );
}

String to_string(CodePragma self)
{
	String result = string_make_reserve( GlobalAllocator, 256 );
	to_string( self, & result );
	return result;
}

void to_string(CodePragma self, String* result )
{
	string_append_fmt( result, "#pragma %SC\n", self->Content );
}

String to_string(CodeSpecifiers self)
{
	String result = string_make_reserve( GlobalAllocator, 64 );
	to_string( self, & result );
	return result;
}

void to_string( CodeSpecifiers self, String* result )
{
	GEN_ASSERT(self.ast != nullptr);
	GEN_ASSERT(result   != nullptr);
	s32 idx  = 0;
	s32 left = self->NumEntries;
	while ( left-- )
	{
		StrC spec = to_str( self->ArrSpecs[idx] );
		string_append_fmt( result, "%.*s ", spec.Len, spec.Ptr );
		idx++;
	}
}

String to_string(CodeStruct self)
{
	GEN_ASSERT(self.ast != nullptr);
	String result = string_make_reserve( GlobalAllocator, 512 );
	switch ( self->Type )
	{
		case CT_Struct:
			to_string_def( self, & result );
		break;
		case CT_Struct_Fwd:
			to_string_fwd( self, & result );
		break;
	}
	return result;
}

void to_string_def( CodeStruct self, String* result )
{
	GEN_ASSERT(self.ast != nullptr);
	AST_Struct* ast = self.ast;

	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_strc( result, txt("struct ") );

	if ( ast->Attributes )
	{
		string_append_fmt( result, "%S ", to_string(ast->Attributes) );
	}

	if ( ast->ParentType )
	{
		char const* access_level = to_str( ast->ParentAccess );

		string_append_fmt( result, "%SC : %s %S", ast->Name, access_level, to_string(ast->ParentType) );

		CodeTypename interface = cast(CodeTypename, ast->ParentType->Next);
		if ( interface )
			string_append_strc( result, txt("\n") );

		while ( interface )
		{
			string_append_fmt( result, ", %S", to_string(interface) );
			interface = interface->Next ? cast( CodeTypename, interface->Next) : CodeTypename { nullptr };
		}
	}
	else if ( ast->Name )
	{
		string_append_strc( result, ast->Name );
	}

	if ( ast->InlineCmt )
	{
		string_append_fmt( result, " // %SC", ast->InlineCmt->Content );
	}

	string_append_fmt( result, "\n{\n%S\n}", to_string(ast->Body) );

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != CT_Typedef && ast->Parent->Type != CT_Variable ) )
		string_append_strc( result, txt(";\n"));
}

void to_string_fwd( CodeStruct self, String* result )
{
	GEN_ASSERT(self.ast != nullptr);
	AST_Struct* ast = self.ast;

	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( ast->Attributes )
		string_append_fmt( result, "struct %S %SC", to_string(ast->Attributes), ast->Name );

	else string_append_fmt( result, "struct %SC", ast->Name );

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != CT_Typedef && ast->Parent->Type != CT_Variable ) )
	{
		if ( ast->InlineCmt )
			string_append_fmt( result, ";  %SC", ast->InlineCmt->Content );
		else
			string_append_strc( result, txt( ";\n") );
	}
}

String to_string(CodeTemplate self)
{
	String result = string_make_reserve( GlobalAllocator, 1024 );
	to_string( self, & result );
	return result;
}

void to_string(CodeTemplate self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Params )
		string_append_fmt( result, "template< %S >\n%S", to_string(self->Params), to_string(self->Declaration) );
	else
		string_append_fmt( result, "template<>\n%S", to_string(self->Declaration) );
}

String to_string(CodeTypedef self)
{
	String result = string_make_reserve( GlobalAllocator, 128 );
	to_string( self, & result );
	return result;
}

void to_string(CodeTypedef self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_strc( result, txt("typedef "));

	// Determines if the typedef is a function typename
	if ( self->UnderlyingType->ReturnType )
		string_append_string( result, to_string(self->UnderlyingType) );
	else
		string_append_fmt( result, "%S %SC", to_string(self->UnderlyingType), self->Name );

	if ( self->UnderlyingType->Type == CT_Typename && self->UnderlyingType->ArrExpr )
	{
		string_append_fmt( result, "[ %S ];", to_string(self->UnderlyingType->ArrExpr) );

		Code next_arr_expr = self->UnderlyingType->ArrExpr->Next;
		while ( next_arr_expr )
		{
			string_append_fmt( result, "[ %S ];", to_string(next_arr_expr) );
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

String to_string(CodeTypename self)
{
	String result = string_make_strc( GlobalAllocator, txt("") );
	to_string( self, & result );
	return result;
}

void to_string(CodeTypename self, String* result )
{
	#if defined(GEN_USE_NEW_TYPENAME_PARSING)
		if ( self->ReturnType && self->Params )
		{
			if ( self->Attributes )
				string_append_fmt( result, "%S ", to_string(self->Attributes) );
			else
			{
				if ( self->Specs )
					string_append_fmt( result, "%S ( %SC ) ( %S ) %S", to_string(self->ReturnType), self->Name, to_string(self->Params), to_string(self->Specs) );
				else
					string_append_fmt( result, "%S ( %SC ) ( %S )", to_string(self->ReturnType), self->Name, to_string(self->Params) );
			}

			break;
		}
	#else
		if ( self->ReturnType && self->Params )
		{
			if ( self->Attributes )
				string_append_fmt( result, "%S ", to_string(self->Attributes) );
			else
			{
				if ( self->Specs )
					string_append_fmt( result, "%S %SC ( %S ) %S", to_string(self->ReturnType), self->Name, to_string(self->Params), to_string(self->Specs) );
				else
					string_append_fmt( result, "%S %SC ( %S )", to_string(self->ReturnType), self->Name, to_string(self->Params) );
			}

			return;
		}
	#endif

	if ( self->Attributes )
		string_append_fmt( result, "%S ", to_string(self->Attributes) );

	if ( self->Specs )
		string_append_fmt( result, "%SC %S", self->Name, to_string(self->Specs) );
	else
		string_append_fmt( result, "%SC", self->Name );

	if ( self->IsParamPack )
		string_append_strc( result, txt("..."));
}

String to_string(CodeUnion self)
{
	String result = string_make_reserve( GlobalAllocator, 512 );
	to_string( self, & result );
	return result;
}

void to_string(CodeUnion self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	string_append_strc( result, txt("union ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", to_string(self->Attributes) );

	if ( self->Name )
	{
		string_append_fmt( result, "%SC\n{\n%S\n}"
			, self->Name
			, GEN_NS to_string(self->Body)
		);
	}
	else
	{
		// Anonymous union
		string_append_fmt( result, "\n{\n%S\n}"
			, GEN_NS to_string(self->Body)
		);
	}

	if ( self->Parent.ast == nullptr || ( self->Parent->Type != CT_Typedef && self->Parent->Type != CT_Variable ) )
		string_append_strc( result, txt(";\n"));
}

String to_string(CodeUsing self)
{
	String result = string_make_reserve( GlobalAllocator, 128 );
	switch ( self->Type )
	{
		case CT_Using:
			to_string( self, & result );
		break;
		case CT_Using_Namespace:
			to_string_ns( self, & result );
		break;
	}
	return result;
}

void to_string(CodeUsing self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes )
		string_append_fmt( result, "%S ", to_string(self->Attributes) );

	if ( self->UnderlyingType )
	{
		string_append_fmt( result, "using %SC = %S", self->Name, to_string(self->UnderlyingType) );

		if ( self->UnderlyingType->ArrExpr )
		{
			string_append_fmt( result, "[ %S ]", to_string(self->UnderlyingType->ArrExpr) );

			Code next_arr_expr = self->UnderlyingType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				string_append_fmt( result, "[ %S ]", to_string(next_arr_expr) );
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

void to_string_ns(CodeUsing self, String* result )
{
	if ( self->InlineCmt )
		string_append_fmt( result, "using namespace $SC;  %SC", self->Name, self->InlineCmt->Content );
	else
		string_append_fmt( result, "using namespace %SC;\n", self->Name );
}

String to_string(CodeVar self)
{
	String result = string_make_reserve( GlobalAllocator, 256 );
	to_string( self, & result );
	return result;
}

void to_string(CodeVar self, String* result )
{
	if ( self->Parent && self->Parent->Type == CT_Variable )
	{
		// Its a comma-separated variable ( a NextVar )

		if ( self->Specs )
			string_append_fmt( result, "%S ", to_string(self->Specs) );

		string_append_strc( result, self->Name );

		if ( self->ValueType->ArrExpr )
		{
			string_append_fmt( result, "[ %S ]", to_string(self->ValueType->ArrExpr) );

			Code next_arr_expr = self->ValueType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				string_append_fmt( result, "[ %S ]", to_string(next_arr_expr) );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		if ( self->Value )
		{
			if ( self->VarConstructorInit )
				string_append_fmt( result, "( %S ", to_string(self->Value) );
			else
				string_append_fmt( result, " = %S", to_string(self->Value) );
		}

		// Keep the chain going...
		if ( self->NextVar )
			string_append_fmt( result, ", %S", to_string(self->NextVar) );

		if ( self->VarConstructorInit )
			string_append_strc( result, txt(" )"));

		return;
	}

	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		string_append_strc( result, txt("export ") );

	if ( self->Attributes || self->Specs )
	{
		if ( self->Attributes )
			string_append_fmt( result, "%S ", to_string(self->Specs) );

		if ( self->Specs )
			string_append_fmt( result, "%S\n", to_string(self->Specs) );

		string_append_fmt( result, "%S %SC", to_string(self->ValueType), self->Name );

		if ( self->ValueType->ArrExpr )
		{
			string_append_fmt( result, "[ %S ]", to_string(self->ValueType->ArrExpr) );

			Code next_arr_expr = self->ValueType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				string_append_fmt( result, "[ %S ]", to_string(next_arr_expr) );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		if ( self->BitfieldSize )
			string_append_fmt( result, " : %S", to_string(self->BitfieldSize) );

		if ( self->Value )
		{
			if ( self->VarConstructorInit )
				string_append_fmt( result, "( %S ", to_string(self->Value) );
			else
				string_append_fmt( result, " = %S", to_string(self->Value) );
		}

		if ( self->NextVar )
			string_append_fmt( result, ", %S", to_string(self->NextVar) );

		if ( self->VarConstructorInit )
			string_append_strc( result, txt(" )"));

		if ( self->InlineCmt )
			string_append_fmt( result, ";  %SC", self->InlineCmt->Content);
		else
			string_append_strc( result, txt(";\n") );

		return;
	}

	if ( self->BitfieldSize )
		string_append_fmt( result, "%S %SC : %S", to_string(self->ValueType), self->Name, to_string(self->BitfieldSize) );

	else if ( self->ValueType->ArrExpr )
	{
		string_append_fmt( result, "%S %SC[ %S ]", to_string(self->ValueType), self->Name, to_string(self->ValueType->ArrExpr) );

		Code next_arr_expr = self->ValueType->ArrExpr->Next;
		while ( next_arr_expr )
		{
			string_append_fmt( result, "[ %S ]", to_string(next_arr_expr) );
			next_arr_expr = next_arr_expr->Next;
		}
	}

	else
		string_append_fmt( result, "%S %SC", to_string(self->ValueType), self->Name );

	if ( self->Value )
	{
		if ( self->VarConstructorInit )
			string_append_fmt( result, "( %S ", to_string(self->Value) );
		else
			string_append_fmt( result, " = %S", to_string(self->Value) );
	}

	if ( self->NextVar )
		string_append_fmt( result, ", %S", to_string( self->NextVar) );

	if ( self->VarConstructorInit )
		string_append_strc( result, txt(" )"));

	string_append_strc( result, txt(";") );

	if ( self->InlineCmt )
		string_append_fmt( result, "  %SC", self->InlineCmt->Content);
	else
		string_append_strc( result, txt("\n"));
}
