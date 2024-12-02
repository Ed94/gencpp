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
	return rcast( AST*, self.ast )->to_string();
}

String CodeAttributes::to_string()
{
	return GEN_NS duplicate( ast->Content, GlobalAllocator );
}

String CodeBody::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	switch ( ast->Type )
	{
		using namespace ECode;
		case Untyped:
		case Execution:
			GEN_NS append( & result, raw()->Content );
		break;

		case Enum_Body:
		case Class_Body:
		case Extern_Linkage_Body:
		case Function_Body:
		case Global_Body:
		case Namespace_Body:
		case Struct_Body:
		case Union_Body:
			to_string( result );
		break;

		case Export_Body:
			to_string_export( result );
		break;
	}
	return result;
}

void CodeBody::to_string( String& result )
{
	Code curr = ast->Front;
	s32  left = ast->NumEntries;
	while ( left -- )
	{
		append_fmt( & result, "%S", curr.to_string() );
		++curr;
	}
}

void CodeBody::to_string_export( String& result )
{
	append_fmt( & result, "export\n{\n" );

	Code curr = *this;
	s32  left = ast->NumEntries;
	while ( left-- )
	{
		append_fmt( & result, "%S", curr.to_string() );
		++curr;
	}

	append_fmt( & result, "};\n" );
}

String CodeComment::to_string()
{
	return GEN_NS duplicate( ast->Content, GlobalAllocator );
}

String CodeConstructor::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	switch (ast->Type)
	{
		using namespace ECode;
		case Constructor:
			to_string_def( result );
		break;
		case Constructor_Fwd:
			to_string_fwd( result );
		break;
	}
	return result;
}

void CodeConstructor::to_string_def( String& result )
{
	AST* ClassStructParent = ast->Parent->Parent;
	if (ClassStructParent) {
		append( & result, ClassStructParent->Name );
	}
	else {
		append( & result, ast->Name );
	}

	if ( ast->Params )
		append_fmt( & result, "( %S )", ast->Params.to_string() );
	else
		append( & result, "()" );

	if ( ast->InitializerList )
		append_fmt( & result, " : %S", ast->InitializerList.to_string() );

	if ( ast->InlineCmt )
		append_fmt( & result, " // %S", ast->InlineCmt->Content );

	append_fmt( & result, "\n{\n%S\n}\n", ast->Body.to_string() );
}

void CodeConstructor::to_string_fwd( String& result )
{
	AST* ClassStructParent = ast->Parent->Parent;
	if (ClassStructParent) {
		append( & result, ClassStructParent->Name );
	}
	else {
		append( & result, ast->Name );
	}

	if ( ast->Params )
		append_fmt( & result, "( %S )", ast->Params.to_string() );
	else
		append_fmt( & result, "()");

	if (ast->Body)
		append_fmt( & result, " = %S", ast->Body.to_string() );

	if ( ast->InlineCmt )
		append_fmt( & result, "; // %S\n", ast->InlineCmt->Content );
	else
		append( & result, ";\n" );
}

String CodeClass::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	switch ( ast->Type )
	{
		using namespace ECode;
		case Class:
			to_string_def( result );
		break;
		case Class_Fwd:
			to_string_fwd( result );
		break;
	}
	return result;
}

void CodeClass::to_string_def( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	append( & result, "class " );

	if ( ast->Attributes )
	{
		append_fmt( & result, "%S ", ast->Attributes.to_string() );
	}

	if ( ast->ParentType )
	{
		char const* access_level = to_str( ast->ParentAccess );

		append_fmt( & result, "%S : %s %S", ast->Name, access_level, ast->ParentType.to_string() );

		CodeType interface = ast->ParentType->Next->code_cast< CodeType >();
		if ( interface )
			append( & result, "\n" );

		while ( interface )
		{
			append_fmt( & result, ", %S", interface.to_string() );
			interface = interface->Next ? interface->Next->code_cast< CodeType >() : CodeType { nullptr };
		}
	}
	else if ( ast->Name )
	{
		append( & result, ast->Name );
	}

	if ( ast->InlineCmt )
	{
		append_fmt( & result, " // %S", ast->InlineCmt->Content );
	}

	append_fmt( & result, "\n{\n%S\n}", ast->Body.to_string() );

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
		append( & result, ";\n");
}

void CodeClass::to_string_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	if ( ast->Attributes )
		append_fmt( & result, "class %S %S", ast->Attributes.to_string(), ast->Name );

	else append_fmt( & result, "class %S", ast->Name );

	// Check if it can have an end-statement
	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
	{
		if ( ast->InlineCmt )
			append_fmt( & result, "; // %S\n", ast->InlineCmt->Content );
		else
			append( & result,";\n");
	}
}

String CodeDefine::to_string()
{
	return string_fmt_buf( GlobalAllocator, "#define %S %S\n", ast->Name, ast->Content );
}

void CodeDefine::to_string( String& result )
{
	append_fmt( & result, "#define %S %S\n", ast->Name, ast->Content );
}

String CodeDestructor::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	switch ( ast->Type )
	{
		using namespace ECode;
		case Destructor:
			to_string_def( result );
		break;
		case Destructor_Fwd:
			to_string_fwd( result );
		break;
	}
	return result;
}

void CodeDestructor::to_string_def( String& result )
{
	if ( ast->Name )
	{
		append_fmt( & result, "%S()", ast->Name );
	}
	else if ( ast->Specs )
	{
		if ( ast->Specs.has( ESpecifier::Virtual ) )
			append_fmt( & result, "virtual ~%S()", ast->Parent->Name );
		else
			append_fmt( & result, "~%S()", ast->Parent->Name );
	}
	else
		append_fmt( & result, "~%S()", ast->Parent->Name );

	append_fmt( & result, "\n{\n%S\n}\n", ast->Body.to_string() );
}

void CodeDestructor::to_string_fwd( String& result )
{
	if ( ast->Specs )
	{
		if ( ast->Specs.has( ESpecifier::Virtual ) )
			append_fmt( & result, "virtual ~%S();\n", ast->Parent->Name );
		else
			append_fmt( & result, "~%S()", ast->Parent->Name );

		if ( ast->Specs.has( ESpecifier::Pure ) )
			append( & result, " = 0;" );
		else if (ast->Body)
			append_fmt( & result, " = %S;", ast->Body.to_string() );
	}
	else
		append_fmt( & result, "~%S();", ast->Parent->Name );

	if ( ast->InlineCmt )
		append_fmt( & result, "  %S", ast->InlineCmt->Content );
	else
		append( & result, "\n");
}

String CodeEnum::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	switch ( ast->Type )
	{
		using namespace ECode;
		case Enum:
			to_string_def( result );
		break;
		case Enum_Fwd:
			to_string_fwd( result );
		break;
		case Enum_Class:
			to_string_class_def( result );
		break;
		case Enum_Class_Fwd:
			to_string_class_fwd( result );
		break;
	}
	return result;
}

void CodeEnum::to_string_def( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	if ( ast->Attributes || ast->UnderlyingType )
	{
		append( & result, "enum " );

		if ( ast->Attributes )
			append_fmt( & result, "%S ", ast->Attributes.to_string() );

		if ( ast->UnderlyingType )
			append_fmt( & result, "%S : %S\n{\n%S\n}"
				, ast->Name
				, ast->UnderlyingType.to_string()
				, ast->Body.to_string()
			);
		else if ( ast->UnderlyingTypeMacro )
			append_fmt( & result, "%S : %S\n{\n%S\n}"
				, ast->Name
				, ast->UnderlyingTypeMacro.to_string()
				, ast->Body.to_string()
			);

		else append_fmt( & result, "%S\n{\n%S\n}", ast->Name, ast->Body.to_string() );
	}
	else append_fmt( & result, "enum %S\n{\n%S\n}", ast->Name, ast->Body.to_string() );

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
		append( & result, ";\n");
}

void CodeEnum::to_string_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	if ( ast->Attributes )
		append_fmt( & result, "%S ", ast->Attributes.to_string() );

	if ( ast->UnderlyingType )
		append_fmt( & result, "enum %S : %S", ast->Name, ast->UnderlyingType.to_string() );
	else
		append_fmt( & result, "enum %S", ast->Name );

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
	{
		if ( ast->InlineCmt )
			append_fmt( & result, ";  %S", ast->InlineCmt->Content );
		else
			append( & result, ";\n");
	}
}

void CodeEnum::to_string_class_def( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	if ( ast->Attributes || ast->UnderlyingType )
	{
		append( & result, "enum class " );

		if ( ast->Attributes )
		{
			append_fmt( & result, "%S ", ast->Attributes.to_string() );
		}

		if ( ast->UnderlyingType )
		{
			append_fmt( & result, "%S : %S\n{\n%S\n}", ast->Name, ast->UnderlyingType.to_string(), ast->Body.to_string() );
		}
		else
		{
			append_fmt( & result, "%S\n{\n%S\n}", ast->Name, ast->Body.to_string() );
		}
	}
	else
	{
		append_fmt( & result, "enum class %S\n{\n%S\n}", ast->Body.to_string() );
	}

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
		append( & result, ";\n");
}

void CodeEnum::to_string_class_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	append( & result, "enum class " );

	if ( ast->Attributes )
		append_fmt( & result, "%S ", ast->Attributes.to_string() );

	append_fmt( & result, "%S : %S", ast->Name, ast->UnderlyingType.to_string() );

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
	{
		if ( ast->InlineCmt )
			append_fmt( & result, ";  %S", ast->InlineCmt->Content );
		else
			append( & result, ";\n");
	}
}

String CodeExec::to_string()
{
	return GEN_NS duplicate( ast->Content, GlobalAllocator );
}

void CodeExtern::to_string( String& result )
{
	if ( ast->Body )
		append_fmt( & result, "extern \"%S\"\n{\n%S\n}\n", ast->Name, ast->Body.to_string() );
	else
		append_fmt( & result, "extern \"%S\"\n{}\n", ast->Name );
}

String CodeInclude::to_string()
{
	return string_fmt_buf( GlobalAllocator, "#include %S\n", ast->Content );
}

void CodeInclude::to_string( String& result )
{
	append_fmt( & result, "#include %S\n", ast->Content );
}

String CodeFriend::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeFriend::to_string( String& result )
{
	append_fmt( & result, "friend %S", ast->Declaration->to_string() );

	if ( ast->Declaration->Type != ECode::Function && result[ length(result) - 1 ] != ';' )
	{
		append( & result, ";" );
	}

	if ( ast->InlineCmt )
		append_fmt( & result, "  %S", ast->InlineCmt->Content );
	else
		append( & result, "\n");
}

String CodeFn::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	switch ( ast->Type )
	{
		using namespace ECode;
		case Function:
			to_string_def( result );
		break;
		case Function_Fwd:
			to_string_fwd( result );
		break;
	}
	return result;
}

void CodeFn::to_string_def( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export" );

	if ( ast->Attributes )
		append_fmt( & result, " %S ", ast->Attributes.to_string() );

	bool prefix_specs = false;
	if ( ast->Specs )
	{
		for ( SpecifierT spec : ast->Specs )
		{
			if ( ! ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( & result, " %.*s", spec_str.Len, spec_str.Ptr );

				prefix_specs = true;
			}
		}
	}

	if ( ast->Attributes || prefix_specs )
		append( & result, "\n" );

	if ( ast->ReturnType )
		append_fmt( & result, "%S %S(", ast->ReturnType.to_string(), ast->Name );

	else
		append_fmt( & result, "%S(", ast->Name );

	if ( ast->Params )
		append_fmt( & result, "%S)", ast->Params.to_string() );

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

	append_fmt( & result, "\n{\n%S\n}\n", ast->Body.to_string() );
}

void CodeFn::to_string_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	if ( ast->Attributes )
		append_fmt( & result, "%S ", ast->Attributes.to_string() );

	b32 prefix_specs = false;
	if ( ast->Specs )
	{
		for ( SpecifierT spec : ast->Specs )
		{
			if ( ! ESpecifier::is_trailing( spec ) || ! (spec != ESpecifier::Pure) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				append_fmt( & result, " %.*s", spec_str.Len, spec_str.Ptr );

				prefix_specs = true;
			}
		}
	}

	if ( ast->Attributes || prefix_specs )
	{
		append( & result, "\n" );
	}

	if ( ast->ReturnType )
		append_fmt( & result, "%S %S(", ast->ReturnType.to_string(), ast->Name );

	else
		append_fmt( & result, "%S(", ast->Name );

	if ( ast->Params )
		append_fmt( & result, "%S)", ast->Params.to_string() );

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

	if ( ast->Specs && ast->Specs.has( ESpecifier::Pure ) >= 0 )
		append( & result, " = 0;" );
	else if (ast->Body)
		append_fmt( & result, " = %S;", ast->Body.to_string() );

	if ( ast->InlineCmt )
		append_fmt( & result, ";  %S", ast->InlineCmt->Content );
	else
		append( & result, ";\n" );
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

	append_fmt( & result, "namespace %S\n{\n%S\n}\n", ast->Name , ast->Body.to_string() );
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
		append_fmt( & result, "%S ", ast->Attributes.to_string() );

	if ( ast->Attributes )
		append_fmt( & result, "%S ", ast->Attributes.to_string() );

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
		append_fmt( & result, "%S)", ast->Params.to_string() );

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
		, ast->Body.to_string()
	);
}

void CodeOperator::to_string_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	if ( ast->Attributes )
		append_fmt( & result, "%S\n", ast->Attributes.to_string() );

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
		append_fmt( & result, "%S)", ast->Params.to_string() );

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

		append_fmt( & result, "\n{\n%S\n}\n", ast->Body.to_string() );
		return;
	}

	if ( ast->Name && length(ast->Name) )
		append_fmt( & result, "%Soperator %S()\n{\n%S\n}\n", ast->Name, ast->ValueType.to_string(), ast->Body.to_string() );
	else
		append_fmt( & result, "operator %S()\n{\n%S\n}\n", ast->ValueType.to_string(), ast->Body.to_string() );
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

String CodeParam::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeParam::to_string( String& result )
{
	if ( ast->Macro )
	{
		// Related to parsing: ( <macro>, ... )
		GEN_NS append( & result, ast->Macro.ast->Content );
		// Could also be: ( <macro> <type <name>, ... )
	}

	if ( ast->Name )
	{
		if ( ast->ValueType.ast == nullptr )
			append_fmt( & result, " %S", ast->Name );
		else
			append_fmt( & result, " %S %S", ast->ValueType.to_string(), ast->Name );

	}
	else if ( ast->ValueType )
		append_fmt( & result, " %S", ast->ValueType.to_string() );

	if ( ast->PostNameMacro )
	{
		append_fmt( & result, " %S", ast->PostNameMacro.to_string() );
	}

	if ( ast->Value )
		append_fmt( & result, " = %S", ast->Value.to_string() );

	if ( ast->NumEntries - 1 > 0 )
	{
		for ( CodeParam param : ast->Next )
		{
			append_fmt( & result, ", %S", param.to_string() );
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

String CodeSpecifiers::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeSpecifiers::to_string( String& result )
{
	s32 idx  = 0;
	s32 left = ast->NumEntries;
	while ( left-- )
	{
		StrC spec = ESpecifier::to_str( ast->ArrSpecs[idx] );
		append_fmt( & result, "%.*s ", spec.Len, spec.Ptr );
		idx++;
	}
}

String CodeStruct::to_string()
{
	String result = string_make( GlobalAllocator, "" );
	switch ( ast->Type )
	{
		using namespace ECode;
		case Struct:
			to_string_def( result );
		break;
		case Struct_Fwd:
			to_string_fwd( result );
		break;
	}
	return result;
}

void CodeStruct::to_string_def( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	append( & result, "struct " );

	if ( ast->Attributes )
	{
		append_fmt( & result, "%S ", ast->Attributes.to_string() );
	}

	if ( ast->ParentType )
	{
		char const* access_level = to_str( ast->ParentAccess );

		append_fmt( & result, "%S : %s %S", ast->Name, access_level, ast->ParentType.to_string() );

		CodeType interface = ast->ParentType->Next->code_cast< CodeType >();
		if ( interface )
			append( & result, "\n" );

		while ( interface )
		{
			append_fmt( & result, ", %S", interface.to_string() );
			interface = interface->Next ? interface->Next->code_cast< CodeType >() : CodeType { nullptr };
		}
	}
	else if ( ast->Name )
	{
		append( & result, ast->Name );
	}

	if ( ast->InlineCmt )
	{
		append_fmt( & result, " // %S", ast->InlineCmt->Content );
	}

	append_fmt( & result, "\n{\n%S\n}", ast->Body.to_string() );

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
		append( & result, ";\n");
}

void CodeStruct::to_string_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag_Export ))
		append( & result, "export " );

	if ( ast->Attributes )
		append_fmt( & result, "struct %S %S", ast->Attributes.to_string(), ast->Name );

	else append_fmt( & result, "struct %S", ast->Name );

	if ( ast->Parent.ast == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
	{
		if ( ast->InlineCmt )
			append_fmt( & result, ";  %S", ast->InlineCmt->Content );
		else
			append( & result, ";\n");
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
		append_fmt( & result, "template< %S >\n%S", ast->Params.to_string(), ast->Declaration.to_string() );
	else
		append_fmt( & result, "template<>\n%S", ast->Declaration.to_string() );
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
		append( & result, ast->UnderlyingType.to_string() );
	else
		append_fmt( & result, "%S %S", ast->UnderlyingType.to_string(), ast->Name );

	if ( ast->UnderlyingType->Type == ECode::Typename && ast->UnderlyingType->ArrExpr )
	{
		append_fmt( & result, "[ %S ];", ast->UnderlyingType->ArrExpr->to_string() );

		AST* next_arr_expr = ast->UnderlyingType->ArrExpr->Next;
		while ( next_arr_expr )
		{
			append_fmt( & result, "[ %S ];", next_arr_expr->to_string() );
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
				append_fmt( result, "%S ", ast->Attributes.to_string() );
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
				append_fmt( & result, "%S ", ast->Attributes.to_string() );
			else
			{
				if ( ast->Specs )
					append_fmt( & result, "%S %S ( %S ) %S", ast->ReturnType.to_string(), ast->Name, ast->Params.to_string(), ast->Specs.to_string() );
				else
					append_fmt( & result, "%S %S ( %S )", ast->ReturnType.to_string(), ast->Name, ast->Params.to_string() );
			}

			return;
		}
	#endif

	if ( ast->Attributes )
		append_fmt( & result, "%S ", ast->Attributes.to_string() );

	if ( ast->Specs )
		append_fmt( & result, "%S %S", ast->Name, ast->Specs.to_string() );
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
		append_fmt( & result, "%S ", ast->Attributes.to_string() );

	if ( ast->Name )
	{
		append_fmt( & result, "%S\n{\n%S\n}"
			, ast->Name
			, ast->Body.to_string()
		);
	}
	else
	{
		// Anonymous union
		append_fmt( & result, "\n{\n%S\n}"
			, ast->Body.to_string()
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
		append_fmt( & result, "%S ", ast->Attributes.to_string() );

	if ( ast->UnderlyingType )
	{
		append_fmt( & result, "using %S = %S", ast->Name, ast->UnderlyingType.to_string() );

		if ( ast->UnderlyingType->ArrExpr )
		{
			append_fmt( & result, "[ %S ]", ast->UnderlyingType->ArrExpr.to_string() );

			AST* next_arr_expr = ast->UnderlyingType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				append_fmt( & result, "[ %S ]", next_arr_expr->to_string() );
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
			append_fmt( & result, "%S ", ast->Specs.to_string() );

		append( & result, ast->Name );

		if ( ast->ValueType->ArrExpr )
		{
			append_fmt( & result, "[ %S ]", ast->ValueType->ArrExpr.to_string() );

			AST* next_arr_expr = ast->ValueType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				append_fmt( & result, "[ %S ]", next_arr_expr->to_string() );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		if ( ast->Value )
		{
			if ( ast->VarConstructorInit )
				append_fmt( & result, "( %S ", ast->Value.to_string() );
			else
				append_fmt( & result, " = %S", ast->Value.to_string() );
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
			append_fmt( & result, "%S ", ast->Specs.to_string() );

		if ( ast->Specs )
			append_fmt( & result, "%S\n", ast->Specs.to_string() );

		append_fmt( & result, "%S %S", ast->ValueType.to_string(), ast->Name );

		if ( ast->ValueType->ArrExpr )
		{
			append_fmt( & result, "[ %S ]", ast->ValueType->ArrExpr.to_string() );

			AST* next_arr_expr = ast->ValueType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				append_fmt( & result, "[ %S ]", next_arr_expr->to_string() );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		if ( ast->BitfieldSize )
			append_fmt( & result, " : %S", ast->BitfieldSize.to_string() );

		if ( ast->Value )
		{
			if ( ast->VarConstructorInit )
				append_fmt( & result, "( %S ", ast->Value.to_string() );
			else
				append_fmt( & result, " = %S", ast->Value.to_string() );
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
		append_fmt( & result, "%S %S : %S", ast->ValueType.to_string(), ast->Name, ast->BitfieldSize.to_string() );

	else if ( ast->ValueType->ArrExpr )
	{
		append_fmt( & result, "%S %S[ %S ]", ast->ValueType.to_string(), ast->Name, ast->ValueType->ArrExpr.to_string() );

		AST* next_arr_expr = ast->ValueType->ArrExpr->Next;
		while ( next_arr_expr )
		{
			append_fmt( & result, "[ %S ]", next_arr_expr->to_string() );
			next_arr_expr = next_arr_expr->Next;
		}
	}

	else
		append_fmt( & result, "%S %S", ast->ValueType.to_string(), ast->Name );

	if ( ast->Value )
	{
		if ( ast->VarConstructorInit )
			append_fmt( & result, "( %S ", ast->Value.to_string() );
		else
			append_fmt( & result, " = %S", ast->Value.to_string() );
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
