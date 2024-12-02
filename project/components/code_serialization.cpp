#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "ast.cpp"
#endif

String to_string(Code self)
{
	if ( self.ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return to_string( self.ast );
}

String to_string(CodeAttributes attributes) {
	return GEN_NS duplicate( attributes->Content, GlobalAllocator );
}

String to_string(CodeBody body)
{
	GEN_ASSERT(body.ast != nullptr);
	String result = string_make( GlobalAllocator, "" );
	switch ( body.ast->Type )
	{
		using namespace ECode;
		case Untyped:
		case Execution:
			append( & result, rcast(AST*, body.ast)->Content );
		break;

		case Enum_Body:
		case Class_Body:
		case Extern_Linkage_Body:
		case Function_Body:
		case Global_Body:
		case Namespace_Body:
		case Struct_Body:
		case Union_Body:
			to_string( body, & result );
		break;

		case Export_Body:
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
		append_fmt( result, "%S", GEN_NS to_string(curr) );
		++curr;
	}
}

void to_string_export( CodeBody body, String* result )
{
	GEN_ASSERT(body.ast != nullptr);
	GEN_ASSERT(result   != nullptr);
	append_fmt( result, "export\n{\n" );

	Code curr = body;
	s32  left = body.ast->NumEntries;
	while ( left-- )
	{
		append_fmt( result, "%S", to_string(curr) );
		++curr;
	}

	append_fmt( result, "};\n" );
}

String to_string(CodeComment comment)
{
	return GEN_NS duplicate( comment->Content, GlobalAllocator );
}

String to_string(CodeConstructor self)
{
	String result = string_make( GlobalAllocator, "" );
	switch (self->Type)
	{
		using namespace ECode;
		case Constructor:
			to_string_def( self, & result );
		break;
		case Constructor_Fwd:
			to_string_fwd( self, & result );
		break;
	}
	return result;
}

void to_string_def(CodeConstructor self, String* result )
{
	AST* ClassStructParent = self->Parent->Parent;
	if (ClassStructParent) {
		append( result, ClassStructParent->Name );
	}
	else {
		append( result, self->Name );
	}

	if ( self->Params )
		append_fmt( result, "( %S )", GEN_NS to_string(self->Params) );
	else
		append( result, "()" );

	if ( self->InitializerList )
		append_fmt( result, " : %S", GEN_NS to_string(self->InitializerList) );

	if ( self->InlineCmt )
		append_fmt( result, " // %S", self->InlineCmt->Content );

	append_fmt( result, "\n{\n%S\n}\n", GEN_NS to_string(self->Body) );
}

void to_string_fwd(CodeConstructor self, String* result )
{
	AST* ClassStructParent = self->Parent->Parent;
	if (ClassStructParent) {
		append( result, ClassStructParent->Name );
	}
	else {
		append( result, self->Name );
	}

	if ( self->Params )
		append_fmt( result, "( %S )", GEN_NS to_string(self->Params) );
	else
		append_fmt( result, "()");

	if (self->Body)
		append_fmt( result, " = %S", GEN_NS to_string(self->Body) );

	if ( self->InlineCmt )
		append_fmt( result, "; // %S\n", self->InlineCmt->Content );
	else
		append( result, ";\n" );
}

String to_string( CodeClass self )
{
	String result = string_make( GlobalAllocator, "" );
	switch ( self->Type )
	{
		using namespace ECode;
		case Class:
			to_string_def(self, & result );
		break;
		case Class_Fwd:
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
		append( result, "export " );

	append( result, "class " );

	if ( ast->Attributes )
	{
		append_fmt( result, "%S ", GEN_NS to_string(ast->Attributes) );
	}

	if ( ast->ParentType )
	{
		char const* access_level = to_str( ast->ParentAccess );

		append_fmt( result, "%S : %s %S", ast->Name, access_level, ast->ParentType.to_string() );

		CodeType interface = cast(CodeType, ast->ParentType->Next);
		if ( interface )
			append( result, "\n" );

		while ( interface )
		{
			append_fmt( result, ", %S", interface.to_string() );
			interface = interface->Next ? cast(CodeType, interface->Next) : CodeType { nullptr };
		}
	}
	else if ( ast->Name )
	{
		append( result, ast->Name );
	}

	if ( ast->InlineCmt )
	{
		append_fmt( result, " // %S", ast->InlineCmt->Content );
	}

	append_fmt( result, "\n{\n%S\n}", GEN_NS to_string(ast->Body) );

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
		append( result, ";\n");
}

void to_string_fwd( CodeClass self, String* result )
{
	GEN_ASSERT(self.ast != nullptr);
	AST_Class* ast = self.ast;

	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( result, "export " );

	if ( ast->Attributes )
		append_fmt( result, "class %S %S", to_string(ast->Attributes), ast->Name );

	else append_fmt( result, "class %S", ast->Name );

	// Check if it can have an end-statement
	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
	{
		if ( ast->InlineCmt )
			append_fmt( result, "; // %S\n", ast->InlineCmt->Content );
		else
			append( result,";\n");
	}
}

String to_string(CodeDefine define)
{
	return string_fmt_buf( GlobalAllocator, "#define %S %S\n", define->Name, define->Content );
}

void to_string(CodeDefine define, String* result )
{
	append_fmt( result, "#define %S %S\n", define->Name, define->Content );
}

String to_string(CodeDestructor self)
{
	String result = string_make( GlobalAllocator, "" );
	switch ( self->Type )
	{
		using namespace ECode;
		case Destructor:
			to_string_def( self, & result );
		break;
		case Destructor_Fwd:
			to_string_fwd( self, & result );
		break;
	}
	return result;
}

void to_string_def(CodeDestructor self, String* result )
{
	if ( self->Name )
	{
		append_fmt( result, "%S()", self->Name );
	}
	else if ( self->Specs )
	{
		if ( has(self->Specs, ESpecifier::Virtual ) )
			append_fmt( result, "virtual ~%S()", self->Parent->Name );
		else
			append_fmt( result, "~%S()", self->Parent->Name );
	}
	else
		append_fmt( result, "~%S()", self->Parent->Name );

	append_fmt( result, "\n{\n%S\n}\n", GEN_NS to_string(self->Body) );
}

void to_string_fwd(CodeDestructor self, String* result )
{
	if ( self->Specs )
	{
		if ( has(self->Specs, ESpecifier::Virtual ) )
			append_fmt( result, "virtual ~%S();\n", self->Parent->Name );
		else
			append_fmt( result, "~%S()", self->Parent->Name );

		if ( has(self->Specs, ESpecifier::Pure ) )
			append( result, " = 0;" );
		else if (self->Body)
			append_fmt( result, " = %S;", GEN_NS to_string(self->Body) );
	}
	else
		append_fmt( result, "~%S();", self->Parent->Name );

	if ( self->InlineCmt )
		append_fmt( result, "  %S", self->InlineCmt->Content );
	else
		append( result, "\n");
}

String to_string(CodeEnum self)
{
	String result = string_make( GlobalAllocator, "" );
	switch ( self->Type )
	{
		using namespace ECode;
		case Enum:
			to_string_def(self, & result );
		break;
		case Enum_Fwd:
			to_string_fwd(self, & result );
		break;
		case Enum_Class:
			to_string_class_def(self, & result );
		break;
		case Enum_Class_Fwd:
			to_string_class_fwd(self, & result );
		break;
	}
	return result;
}

void to_string_def(CodeEnum self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		append( result, "export " );

	if ( self->Attributes || self->UnderlyingType )
	{
		append( result, "enum " );

		if ( self->Attributes )
			append_fmt( result, "%S ", GEN_NS to_string(self->Attributes) );

		if ( self->UnderlyingType )
			append_fmt( result, "%S : %S\n{\n%S\n}"
				, self->Name
				, self->UnderlyingType.to_string()
				, GEN_NS to_string(self->Body)
			);
		else if ( self->UnderlyingTypeMacro )
			append_fmt( result, "%S : %S\n{\n%S\n}"
				, self->Name
				, GEN_NS to_string(self->UnderlyingTypeMacro)
				, GEN_NS to_string(self->Body)
			);

		else append_fmt( result, "%S\n{\n%S\n}", self->Name, GEN_NS to_string(self->Body) );
	}
	else append_fmt( result, "enum %S\n{\n%S\n}", self->Name, GEN_NS to_string(self->Body) );

	if ( self->Parent.ast == nullptr || ( self->Parent->Type != ECode::Typedef && self->Parent->Type != ECode::Variable ) )
		append( result, ";\n");
}

void to_string_fwd(CodeEnum self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		append( result, "export " );

	if ( self->Attributes )
		append_fmt( result, "%S ",GEN_NS to_string(self->Attributes) );

	if ( self->UnderlyingType )
		append_fmt( result, "enum %S : %S", self->Name, self->UnderlyingType.to_string() );
	else
		append_fmt( result, "enum %S", self->Name );

	if ( self->Parent.ast == nullptr || ( self->Parent->Type != ECode::Typedef && self->Parent->Type != ECode::Variable ) )
	{
		if ( self->InlineCmt )
			append_fmt( result, ";  %S", self->InlineCmt->Content );
		else
			append( result, ";\n");
	}
}

void to_string_class_def(CodeEnum self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		append( result, "export " );

	if ( self->Attributes || self->UnderlyingType )
	{
		append( result, "enum class " );

		if ( self->Attributes )
		{
			append_fmt( result, "%S ",GEN_NS to_string(self->Attributes) );
		}

		if ( self->UnderlyingType )
		{
			append_fmt( result, "%S : %S\n{\n%S\n}", self->Name, self->UnderlyingType.to_string(), GEN_NS to_string(self->Body) );
		}
		else
		{
			append_fmt( result, "%S\n{\n%S\n}", self->Name, GEN_NS to_string(self->Body) );
		}
	}
	else
	{
		append_fmt( result, "enum class %S\n{\n%S\n}", GEN_NS to_string(self->Body) );
	}

	if ( self->Parent.ast == nullptr || ( self->Parent->Type != ECode::Typedef && self->Parent->Type != ECode::Variable ) )
		append( result, ";\n");
}

void to_string_class_fwd(CodeEnum self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		append( result, "export " );

	append( result, "enum class " );

	if ( self->Attributes )
		append_fmt( result, "%S ",GEN_NS to_string(self->Attributes) );

	append_fmt( result, "%S : %S", self->Name, self->UnderlyingType.to_string() );

	if ( self->Parent.ast == nullptr || ( self->Parent->Type != ECode::Typedef && self->Parent->Type != ECode::Variable ) )
	{
		if ( self->InlineCmt )
			append_fmt( result, ";  %S", self->InlineCmt->Content );
		else
			append( result, ";\n");
	}
}

String to_string(CodeExec exec)
{
	return GEN_NS duplicate( exec->Content, GlobalAllocator );
}

void to_string(CodeExtern self, String* result )
{
	if ( self->Body )
		append_fmt( result, "extern \"%S\"\n{\n%S\n}\n", self->Name, GEN_NS to_string(self->Body) );
	else
		append_fmt( result, "extern \"%S\"\n{}\n", self->Name );
}

String to_string(CodeInclude include)
{
	return string_fmt_buf( GlobalAllocator, "#include %S\n", include->Content );
}

void to_string( CodeInclude include, String* result )
{
	append_fmt( result, "#include %S\n", include->Content );
}

String to_string(CodeFriend self)
{
	String result = string_make( GlobalAllocator, "" );
	to_string( self, & result );
	return result;
}

void to_string(CodeFriend self, String* result )
{
	append_fmt( result, "friend %S", GEN_NS to_string(self->Declaration) );

	if ( self->Declaration->Type != ECode::Function && (* result)[ length(* result) - 1 ] != ';' )
	{
		append( result, ";" );
	}

	if ( self->InlineCmt )
		append_fmt( result, "  %S", self->InlineCmt->Content );
	else
		append( result, "\n");
}

String to_string(CodeFn self)
{
	String result = string_make( GlobalAllocator, "" );
	switch ( self->Type )
	{
		using namespace ECode;
		case Function:
			to_string_def(self, & result );
		break;
		case Function_Fwd:
			to_string_fwd(self, & result );
		break;
	}
	return result;
}

void to_string_def(CodeFn self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		append( result, "export" );

	if ( self->Attributes )
		append_fmt( result, " %S ",GEN_NS to_string(self->Attributes) );

	bool prefix_specs = false;
	if ( self->Specs )
	{
		for ( SpecifierT spec : self->Specs )
		{
			if ( ! ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );

				prefix_specs = true;
			}
		}
	}

	if ( self->Attributes || prefix_specs )
		append( result, "\n" );

	if ( self->ReturnType )
		append_fmt( result, "%S %S(", self->ReturnType.to_string(), self->Name );

	else
		append_fmt( result, "%S(", self->Name );

	if ( self->Params )
		append_fmt( result, "%S)", GEN_NS to_string(self->Params) );

	else
		append( result, ")" );

	if ( self->Specs )
	{
		for ( SpecifierT spec : self->Specs )
		{
			if ( ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	append_fmt( result, "\n{\n%S\n}\n", GEN_NS to_string(self->Body) );
}

void to_string_fwd(CodeFn self, String* result )
{
	if ( bitfield_is_equal( u32, self->ModuleFlags, ModuleFlag_Export ))
		append( result, "export " );

	if ( self->Attributes )
		append_fmt( result, "%S ",GEN_NS to_string(self->Attributes) );

	b32 prefix_specs = false;
	if ( self->Specs )
	{
		for ( SpecifierT spec : self->Specs )
		{
			if ( ! ESpecifier::is_trailing( spec ) || ! (spec != ESpecifier::Pure) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );

				prefix_specs = true;
			}
		}
	}

	if ( self->Attributes || prefix_specs )
	{
		append( result, "\n" );
	}

	if ( self->ReturnType )
		append_fmt( result, "%S %S(", self->ReturnType.to_string(), self->Name );

	else
		append_fmt( result, "%S(", self->Name );

	if ( self->Params )
		append_fmt( result, "%S)", GEN_NS to_string(self->Params) );

	else
		append( result, ")" );

	if ( self->Specs )
	{
		for ( SpecifierT spec : self->Specs )
		{
			if ( ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( self->Specs && has(self->Specs, ESpecifier::Pure ) >= 0 )
		append( result, " = 0;" );
	else if (self->Body)
		append_fmt( result, " = %S;", GEN_NS to_string(self->Body) );

	if ( self->InlineCmt )
		append_fmt( result, ";  %S", self->InlineCmt->Content );
	else
		append( result, ";\n" );
}

String CodeModule::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeModule::to_string( String& result )
{
	if (((u32(ModuleFlag_Export) & u32(ast->ModuleFlags)) == u32(ModuleFlag_Export)))
		append( & result, "export ");

	if (((u32(ModuleFlag_Import) & u32(ast->ModuleFlags)) == u32(ModuleFlag_Import)))
		append( & result, "import ");

	append_fmt( & result, "%S;\n", ast->Name );
}

String CodeNS::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeNS::to_string( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	append_fmt( & result, "namespace %S\n{\n%S\n}\n", ast->Name , GEN_NS to_string(ast->Body) );
}

String CodeOperator::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	switch ( ast->Type )
	{
		using namespace ECode;
		case Operator:
		case Operator_Member:
			to_string_def( result );
		break;
		case Operator_Fwd:
		case Operator_Member_Fwd:
			to_string_fwd( result );
		break;
	}
	return result;
}

void CodeOperator::to_string_def( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	if ( ast->Attributes )
		append_fmt( & result, "%S ",GEN_NS to_string(ast->Attributes) );

	if ( ast->Attributes )
		append_fmt( & result, "%S ",GEN_NS to_string(ast->Attributes) );

	if ( ast->Specs )
	{
		for ( SpecifierT spec : ast->Specs )
		{
			if ( ! ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( & result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( ast->Attributes || ast->Specs )
	{
		append( & result, "\n" );
	}

	if ( ast->ReturnType )
		append_fmt( & result, "%S %S (", ast->ReturnType.to_string(), ast->Name );

	if ( ast->Params )
		append_fmt( & result, "%S)", GEN_NS to_string(ast->Params) );

	else
		append( & result, ")" );

	if ( ast->Specs )
	{
		for ( SpecifierT spec : ast->Specs )
		{
			if ( ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( & result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	append_fmt( & result, "\n{\n%S\n}\n"
		, GEN_NS to_string(ast->Body)
	);
}

void CodeOperator::to_string_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	if ( ast->Attributes )
		append_fmt( & result, "%S\n",GEN_NS to_string(ast->Attributes) );

	if ( ast->Specs )
	{
		for ( SpecifierT spec : ast->Specs )
		{
			if ( ! ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( & result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( ast->Attributes || ast->Specs )
	{
		append( & result, "\n" );
	}

	append_fmt( & result, "%S %S (", ast->ReturnType.to_string(), ast->Name );

	if ( ast->Params )
		append_fmt( & result, "%S)", GEN_NS to_string(ast->Params) );

	else
		append_fmt( & result, ")" );

	if ( ast->Specs )
	{
		for ( SpecifierT spec : ast->Specs )
		{
			if ( ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( & result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( ast->InlineCmt )
		append_fmt( & result, ";  %S", ast->InlineCmt->Content );
	else
		append( & result, ";\n" );
}

String CodeOpCast::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	switch ( ast->Type )
	{
		using namespace ECode;
		case Operator_Cast:
			to_string_def( result );
		break;
		case Operator_Cast_Fwd:
			to_string_fwd( result );
		break;
	}
	return result;
}

void CodeOpCast::to_string_def( String& result )
{
	if ( ast->Specs )
	{
		for ( SpecifierT spec : ast->Specs )
		{
			if ( ! ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( & result, "%*s ", spec_str.Len, spec_str.Ptr );
			}
		}

		if ( ast->Name && length(ast->Name) )
			append_fmt( & result, "%Soperator %S()", ast->Name, ast->ValueType.to_string() );
		else
			append_fmt( & result, "operator %S()", ast->ValueType.to_string() );

		for ( SpecifierT spec : ast->Specs )
		{
			if ( ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( & result, " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}

		append_fmt( & result, "\n{\n%S\n}\n", GEN_NS to_string(ast->Body) );
		return;
	}

	if ( ast->Name && length(ast->Name) )
		append_fmt( & result, "%Soperator %S()\n{\n%S\n}\n", ast->Name, ast->ValueType.to_string(), GEN_NS to_string(ast->Body) );
	else
		append_fmt( & result, "operator %S()\n{\n%S\n}\n", ast->ValueType.to_string(), GEN_NS to_string(ast->Body) );
}

void CodeOpCast::to_string_fwd( String& result )
{
	if ( ast->Specs )
	{
		for ( SpecifierT spec : ast->Specs )
		{
			if ( ! ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( & result, "%*s ", spec_str.Len, spec_str.Ptr );
			}
		}

		append_fmt( & result, "operator %S()", ast->ValueType.to_string() );

		for ( SpecifierT spec : ast->Specs )
		{
			if ( ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( & result, " %*s", spec_str.Len, spec_str.Ptr );
			}
		}

		if ( ast->InlineCmt )
			append_fmt( & result, ";  %S", ast->InlineCmt->Content );
		else
			append( & result, ";\n" );
		return;
	}

	if ( ast->InlineCmt )
		append_fmt( & result, "operator %S();  %S", ast->ValueType.to_string() );
	else
		append_fmt( & result, "operator %S();\n", ast->ValueType.to_string() );
}

String to_string(CodeParam self)
{
	String result = string_make( GlobalAllocator, "" );
	to_string( self, & result );
	return result;
}

void to_string( CodeParam self, String* result )
{
	AST_Param* ast = self.ast;
	if ( ast->Macro )
	{
		// Related to parsing: ( <macro>, ... )
		GEN_NS append( result, ast->Macro.ast->Content );
		// Could also be: ( <macro> <type <name>, ... )
	}

	if ( ast->Name )
	{
		if ( ast->ValueType.ast == nullptr )
			append_fmt( result, " %S", ast->Name );
		else
			append_fmt( result, " %S %S", ast->ValueType.to_string(), ast->Name );

	}
	else if ( ast->ValueType )
		append_fmt( result, " %S", ast->ValueType.to_string() );

	if ( ast->PostNameMacro )
	{
		append_fmt( result, " %S", GEN_NS to_string(ast->PostNameMacro) );
	}

	if ( ast->Value )
		append_fmt( result, " = %S", GEN_NS to_string(ast->Value) );

	if ( ast->NumEntries - 1 > 0 )
	{
		for ( CodeParam param : ast->Next )
		{
			append_fmt( result, ", %S", to_string(param) );
		}
	}
}

String CodePreprocessCond::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	switch ( ast->Type )
	{
		using namespace ECode;
		case Preprocess_If:
			to_string_if( result );
		break;
		case Preprocess_IfDef:
			to_string_ifdef( result );
		break;
		case Preprocess_IfNotDef:
			to_string_ifndef( result );
		break;
		case Preprocess_ElIf:
			to_string_elif( result );
		break;
		case Preprocess_Else:
			to_string_else( result );
		break;
		case Preprocess_EndIf:
			to_string_endif( result );
		break;
	}
	return result;
}

void CodePreprocessCond::to_string_if( String& result )
{
	append_fmt( & result, "#if %S\n", ast->Content );
}

void CodePreprocessCond::to_string_ifdef( String& result )
{
	append_fmt( & result, "#ifdef %S\n", ast->Content );
}

void CodePreprocessCond::to_string_ifndef( String& result )
{
	append_fmt( & result, "#ifndef %S\n", ast->Content );
}

void CodePreprocessCond::to_string_elif( String& result )
{
	append_fmt( & result, "#elif %S\n", ast->Content );
}

void CodePreprocessCond::to_string_else( String& result )
{
	append_fmt( & result, "#else\n" );
}

void CodePreprocessCond::to_string_endif( String& result )
{
	append_fmt( & result, "#endif\n" );
}

String CodePragma::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodePragma::to_string( String& result )
{
	append_fmt( & result, "#pragma %S\n", ast->Content );
}

String to_string(CodeSpecifiers self)
{
	String result = string_make( GlobalAllocator, "" );
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
		StrC spec = ESpecifier::to_str( self->ArrSpecs[idx] );
		append_fmt( result, "%.*s ", spec.Len, spec.Ptr );
		idx++;
	}
}

String to_string(CodeStruct self)
{
	GEN_ASSERT(self.ast != nullptr);
	String result = string_make( GlobalAllocator, "" );
	switch ( self->Type )
	{
		using namespace ECode;
		case Struct:
			to_string_def( self, & result );
		break;
		case Struct_Fwd:
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
		append( result, "export " );

	append( result, "struct " );

	if ( ast->Attributes )
	{
		append_fmt( result, "%S ",GEN_NS to_string(ast->Attributes) );
	}

	if ( ast->ParentType )
	{
		char const* access_level = to_str( ast->ParentAccess );

		append_fmt( result, "%S : %s %S", ast->Name, access_level, ast->ParentType.to_string() );

		CodeType interface = cast(CodeType, ast->ParentType->Next);
		if ( interface )
			append( result, "\n" );

		while ( interface )
		{
			append_fmt( result, ", %S", interface.to_string() );
			interface = interface->Next ? cast( CodeType, interface->Next) : CodeType { nullptr };
		}
	}
	else if ( ast->Name )
	{
		append( result, ast->Name );
	}

	if ( ast->InlineCmt )
	{
		append_fmt( result, " // %S", ast->InlineCmt->Content );
	}

	append_fmt( result, "\n{\n%S\n}", GEN_NS to_string(ast->Body) );

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
		append( result, ";\n");
}

void to_string_fwd( CodeStruct self, String* result )
{
	GEN_ASSERT(self.ast != nullptr);
	AST_Struct* ast = self.ast;

	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( result, "export " );

	if ( ast->Attributes )
		append_fmt( result, "struct %S %S",GEN_NS to_string(ast->Attributes), ast->Name );

	else append_fmt( result, "struct %S", ast->Name );

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
	{
		if ( ast->InlineCmt )
			append_fmt( result, ";  %S", ast->InlineCmt->Content );
		else
			append( result, ";\n");
	}
}

String CodeTemplate::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeTemplate::to_string( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	if ( ast->Params )
		append_fmt( & result, "template< %S >\n%S", GEN_NS to_string(ast->Params), GEN_NS to_string(ast->Declaration) );
	else
		append_fmt( & result, "template<>\n%S", GEN_NS to_string(ast->Declaration) );
}

String CodeTypedef::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeTypedef::to_string( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	append( & result, "typedef ");

	// Determines if the typedef is a function typename
	if ( ast->UnderlyingType->ReturnType )
		append( & result, GEN_NS to_string(ast->UnderlyingType) );
	else
		append_fmt( & result, "%S %S", GEN_NS to_string(ast->UnderlyingType), ast->Name );

	if ( ast->UnderlyingType->Type == ECode::Typename && ast->UnderlyingType->ArrExpr )
	{
		append_fmt( & result, "[ %S ];", GEN_NS to_string(ast->UnderlyingType->ArrExpr) );

		AST* next_arr_expr = ast->UnderlyingType->ArrExpr->Next;
		while ( next_arr_expr )
		{
			append_fmt( & result, "[ %S ];", GEN_NS to_string(next_arr_expr) );
			next_arr_expr = next_arr_expr->Next;
		}
	}
	else
	{
		append( & result, ";" );
	}

	if ( ast->InlineCmt )
		append_fmt( & result, "  %S", ast->InlineCmt->Content);
	else
		append( & result, "\n");
}

String CodeType::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeType::to_string( String& result )
{
	#if defined(GEN_USE_NEW_TYPENAME_PARSING)
		if ( ast->ReturnType && ast->Params )
		{
			if ( ast->Attributes )
				append_fmt( result, "%S ",GEN_NS to_string(ast->Attributes) );
			else
			{
				if ( ast->Specs )
					append_fmt( & result, "%S ( %S ) ( %S ) %S", ast->ReturnType.to_string(), ast->Name, ast->Params.to_string(), ast->Specs.to_string() );
				else
					append_fmt( & result, "%S ( %S ) ( %S )", ast->ReturnType.to_string(), ast->Name, ast->Params.to_string() );
			}

			break;
		}
	#else
		if ( ast->ReturnType && ast->Params )
		{
			if ( ast->Attributes )
				append_fmt( & result, "%S ",GEN_NS to_string(ast->Attributes) );
			else
			{
				if ( ast->Specs )
					append_fmt( & result, "%S %S ( %S ) %S", ast->ReturnType.to_string(), ast->Name, GEN_NS to_string(ast->Params), GEN_NS to_string(ast->Specs) );
				else
					append_fmt( & result, "%S %S ( %S )", ast->ReturnType.to_string(), ast->Name, GEN_NS to_string(ast->Params) );
			}

			return;
		}
	#endif

	if ( ast->Attributes )
		append_fmt( & result, "%S ",GEN_NS to_string(ast->Attributes) );

	if ( ast->Specs )
		append_fmt( & result, "%S %S", ast->Name, GEN_NS to_string(ast->Specs) );
	else
		append_fmt( & result, "%S", ast->Name );

	if ( ast->IsParamPack )
		append( & result, "...");
}

String CodeUnion::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeUnion::to_string( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	append( & result, "union " );

	if ( ast->Attributes )
		append_fmt( & result, "%S ",GEN_NS to_string(ast->Attributes) );

	if ( ast->Name )
	{
		append_fmt( & result, "%S\n{\n%S\n}"
			, ast->Name
			, GEN_NS to_string(ast->Body)
		);
	}
	else
	{
		// Anonymous union
		append_fmt( & result, "\n{\n%S\n}"
			, GEN_NS to_string(ast->Body)
		);
	}

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
		append( & result, ";\n");
}

String CodeUsing::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	switch ( ast->Type )
	{
		using namespace ECode;
		case Using:
			to_string( result );
		break;
		case Using_Namespace:
			to_string_ns( result );
		break;
	}
	return result;
}

void CodeUsing::to_string( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	if ( ast->Attributes )
		append_fmt( & result, "%S ",GEN_NS to_string(ast->Attributes) );

	if ( ast->UnderlyingType )
	{
		append_fmt( & result, "using %S = %S", ast->Name, ast->UnderlyingType.to_string() );

		if ( ast->UnderlyingType->ArrExpr )
		{
			append_fmt( & result, "[ %S ]", GEN_NS to_string(ast->UnderlyingType->ArrExpr) );

			AST* next_arr_expr = ast->UnderlyingType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				append_fmt( & result, "[ %S ]", GEN_NS to_string(next_arr_expr) );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		append( & result, ";" );
	}
	else
		append_fmt( & result, "using %S;", ast->Name );

	if ( ast->InlineCmt )
		append_fmt( & result, "  %S\n", ast->InlineCmt->Content );
	else
		append( & result, "\n");
}

void CodeUsing::to_string_ns( String& result )
{
	if ( ast->InlineCmt )
		append_fmt( & result, "using namespace $S;  %S", ast->Name, ast->InlineCmt->Content );
	else
		append_fmt( & result, "using namespace %s;\n", ast->Name );
}

String CodeVar::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeVar::to_string( String& result )
{
	if ( ast->Parent && ast->Parent->Type == ECode::Variable )
	{
		// Its a comma-separated variable ( a NextVar )

		if ( ast->Specs )
			append_fmt( & result, "%S ", GEN_NS to_string(ast->Specs) );

		append( & result, ast->Name );

		if ( ast->ValueType->ArrExpr )
		{
			append_fmt( & result, "[ %S ]", GEN_NS to_string(ast->ValueType->ArrExpr) );

			AST* next_arr_expr = ast->ValueType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				append_fmt( & result, "[ %S ]", GEN_NS to_string(next_arr_expr) );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		if ( ast->Value )
		{
			if ( ast->VarConstructorInit )
				append_fmt( & result, "( %S ", GEN_NS to_string(ast->Value) );
			else
				append_fmt( & result, " = %S", GEN_NS to_string(ast->Value) );
		}

		// Keep the chain going...
		if ( ast->NextVar )
			append_fmt( & result, ", %S", ast->NextVar.to_string() );

		if ( ast->VarConstructorInit )
			append( & result, " )");

		return;
	}

	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	if ( ast->Attributes || ast->Specs )
	{
		if ( ast->Attributes )
			append_fmt( & result, "%S ", GEN_NS to_string(ast->Specs) );

		if ( ast->Specs )
			append_fmt( & result, "%S\n", GEN_NS to_string(ast->Specs) );

		append_fmt( & result, "%S %S", ast->ValueType.to_string(), ast->Name );

		if ( ast->ValueType->ArrExpr )
		{
			append_fmt( & result, "[ %S ]", GEN_NS to_string(ast->ValueType->ArrExpr) );

			AST* next_arr_expr = ast->ValueType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				append_fmt( & result, "[ %S ]", GEN_NS to_string(next_arr_expr) );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		if ( ast->BitfieldSize )
			append_fmt( & result, " : %S", GEN_NS to_string(ast->BitfieldSize) );

		if ( ast->Value )
		{
			if ( ast->VarConstructorInit )
				append_fmt( & result, "( %S ", GEN_NS to_string(ast->Value) );
			else
				append_fmt( & result, " = %S", GEN_NS to_string(ast->Value) );
		}

		if ( ast->NextVar )
			append_fmt( & result, ", %S", ast->NextVar.to_string() );

		if ( ast->VarConstructorInit )
			append( & result, " )");

		if ( ast->InlineCmt )
			append_fmt( & result, ";  %S", ast->InlineCmt->Content);
		else
			append( & result, ";\n" );

		return;
	}

	if ( ast->BitfieldSize )
		append_fmt( & result, "%S %S : %S", ast->ValueType.to_string(), ast->Name, GEN_NS to_string(ast->BitfieldSize) );

	else if ( ast->ValueType->ArrExpr )
	{
		append_fmt( & result, "%S %S[ %S ]", ast->ValueType.to_string(), ast->Name, GEN_NS to_string(ast->ValueType->ArrExpr) );

		AST* next_arr_expr = ast->ValueType->ArrExpr->Next;
		while ( next_arr_expr )
		{
			append_fmt( & result, "[ %S ]", GEN_NS to_string(next_arr_expr) );
			next_arr_expr = next_arr_expr->Next;
		}
	}

	else
		append_fmt( & result, "%S %S", ast->ValueType.to_string(), ast->Name );

	if ( ast->Value )
	{
		if ( ast->VarConstructorInit )
			append_fmt( & result, "( %S ", GEN_NS to_string(ast->Value) );
		else
			append_fmt( & result, " = %S", GEN_NS to_string(ast->Value) );
	}

	if ( ast->NextVar )
		append_fmt( & result, ", %S", ast->NextVar.to_string() );

	if ( ast->VarConstructorInit )
		append( & result, " )");

	append( & result, ";" );

	if ( ast->InlineCmt )
		append_fmt( & result, "  %S", ast->InlineCmt->Content);
	else
		append( & result, "\n");
}
