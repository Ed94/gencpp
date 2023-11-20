#if GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "ast.cpp"
#endif

String Code::to_string()
{
	if ( ast == nullptr )
	{
		log_failure( "Code::to_string: Cannot convert code to string, AST is null!" );
		return { nullptr };
	}
	return rcast( AST*, ast )->to_string();
}

String CodeAttributes::to_string()
{
	return ast->Content.duplicate( GlobalAllocator );
}

String CodeBody::to_string()
{
	String result = String::make( GlobalAllocator, "" );
	switch ( ast->Type )
	{
		using namespace ECode;
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
		result.append_fmt( "%S", curr.to_string() );
		++curr;
	}
}

void CodeBody::to_string_export( String& result )
{
	result.append_fmt( "export\n{\n" );

	Code curr = { this };
	s32  left = ast->NumEntries;
	while ( left-- )
	{
		result.append_fmt( "%S", curr.to_string() );
		++curr;
	}

	result.append_fmt( "};\n" );
}

String CodeComment::to_string()
{
	return ast->Content.duplicate( GlobalAllocator );
}

String CodeConstructor::to_string()
{
	String result = String::make( GlobalAllocator, "" );
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
	result.append( ast->Parent->name );
	
	if ( ast->Params )
		result.append_fmt( "( %S )", ast->Params.to_string() );
	else
		result.append( "(void)" );
	
	if ( ast->InitializerList )
		result.append_fmt( " : %S", ast->InitializerList.to_string() );
	
	if ( ast->InlineCmt )
		result.append_fmt( " // %S", ast->InlineCmt->Content );
	
	result.append_fmt( "\n{\n%S\n}\n", ast->Body.to_string() );
}

void CodeConstructor::to_string_fwd( String& result )
{
	result.append( ast->Parent->Name );
	
	if ( ast->Params )
		result.append_fmt( "( %S )", ast->Params.to_string() );
	else
	{
		if ( ast->InlineCmt )
			result.append_fmt( "(void); // %S\n", ast->InlineCmt->Content );
		else
			result.append( "(void);\n" );
	}
}

String CodeClass::to_string()
{
	String result = String::make( GlobalAllocator, "" );
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
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	result.append( "class " );

	if ( ast->Attributes )
	{
		result.append_fmt( "%S ", ast->Attributes.to_string() );
	}

	if ( ast->ParentType )
	{
		char const* access_level = to_str( ast->ParentAccess );

		result.append_fmt( "%S : %s %S", ast->Name, access_level, ast->ParentType );

		CodeType interface = ast->ParentType->Next->cast< CodeType >();
		if ( interface )
			result.append( "\n" );

		while ( interface )
		{
			result.append_fmt( ", %S", interface.to_string() );
			interface = interface->Next ? interface->Next->cast< CodeType >() : Code { nullptr };
		}
	}
	else if ( ast->Name )
	{
		result.append( ast->Name );
	}

	if ( ast->InlineCmt )
	{
		result.append_fmt( " // %S", ast->InlineCmt->Content );
	}

	result.append_fmt( "\n{\n%S\n}", ast->Body.to_string() );

	if ( ast->Parent == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
		result.append(";\n");
}

void CodeClass::to_string_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	if ( ast->Attributes )
		result.append_fmt( "class %S %S", ast->Attributes.to_string(), ast->Name );

	else result.append_fmt( "class %S", ast->Name );

	// Check if it can have an end-statement
	if ( ast->Parent == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
	{
		if ( ast->InlineCmt )
			result.append_fmt( "; // %S\n", ast->InlineCmt->Content );
		else
			result.append(";\n");
	}
}

String CodeDefine::to_string()
{
	return String::fmt_buf( GlobalAllocator, "#define %S %S\n", ast->Name, ast->Content );
}

void CodeDefine::to_string( String& result )
{
	return result.append_fmt( "#define %S %S\n", ast->Name, ast->Content );
}

String CodeDestructor::to_string()
{
	String result = String::make( GlobalAllocator, "" );
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
	if ( ast->Specs )
	{
		if ( ast->Specs.has( ESpecifier::Virtual ) )
			result.append_fmt( "virtual ~%S()", ast->Parent->Name );
		else
			result.append_fmt( "~%S()", ast->Parent->Name );
	}
	else
		result.append_fmt( "~%S()", ast->Parent->Name );
	
	result.append_fmt( "\n{\n%S\n}\n", ast->Body.to_string() );
}

void CodeDestructor::to_string_fwd( String& result )
{
	if ( ast->Specs )
	{
		if ( ast->Specs.has( ESpecifier::Virtual ) )
			result.append_fmt( "virtual ~%S();\n", ast->Parent->Name );
		else
			result.append_fmt( "~%S()", ast->Parent->Name );

		if ( ast->Specs.has( ESpecifier::Pure ) )
			result.append( " = 0;" );
	}
	else
		result.append_fmt( "~%S();", ast->Parent->Name );

	if ( ast->InlineCmt )
		result.append_fmt( "  %S", ast->InlineCmt->Content );
	else
		result.append("\n");
}

String CodeEnum::to_string()
{
	String result = String::make( GlobalAllocator, "" );
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
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );
	
	if ( ast->Attributes || ast->UnderlyingType )
	{
		result.append( "enum " );
	
		if ( ast->Attributes )
			result.append_fmt( "%S ", ast->Attributes.to_string() );
	
		if ( UnderlyingType )
			result.append_fmt( "%S : %S\n{\n%S\n}"
				, ast->Name
				, ast->UnderlyingType.to_string()
				, ast->Body.to_string()
			);
	
		else result.append_fmt( "%S\n{\n%S\n}", ast->Name, ast->Body.to_string() );
	}
	else result.append_fmt( "enum %S\n{\n%S\n}", ast->Name, ast->Body.to_string() );
	
	if ( ast->Parent == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
		result.append(";\n");
}

void CodeEnum::to_string_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	if ( ast->Attributes )
		result.append_fmt( "%S ", ast->Attributes.to_string() );

	result.append_fmt( "enum %S : %S", ast->Name, ast->UnderlyingType.to_string() );

	if ( ast->Parent == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
	{
		if ( ast->InlineCmt )
			result.append_fmt(";  %S", ast->InlineCmt->Content );
		else
			result.append(";\n");
	}
}

void CodeEnum::to_string_class_def( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );
	
	if ( ast->Attributes || ast->UnderlyingType )
	{
		result.append( "enum class " );
	
		if ( ast->Attributes )
		{
			result.append_fmt( "%S ", ast->Attributes.to_string() );
		}
	
		if ( ast->UnderlyingType )
		{
			result.append_fmt( "%S : %S\n{\n%S\n}", ast->Name, ast->UnderlyingType.to_string(), ast->Body.to_string() );
		}
		else
		{
			result.append_fmt( "%S\n{\n%S\n}", ast->Name, ast->Body.to_string() );
		}
	}
	else
	{
		result.append_fmt( "enum class %S\n{\n%S\n}", ast->Body.to_string() );
	}
	
	if ( ast->Parent == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
		result.append(";\n");
}

void CodeEnum::to_string_class_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	result.append( "enum class " );

	if ( ast->Attributes )
		result.append_fmt( "%S ", ast->Attributes.to_string() );

	result.append_fmt( "%S : %S", ast->Name, ast->UnderlyingType.to_string() );

	if ( ast->Parent == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
	{
		if ( ast->InlineCmt )
			result.append_fmt(";  %S", ast->InlineCmt->Content );
		else
			result.append(";\n");
	}
}

String CodeExec::to_string()
{
	return ast->Content.duplicate( GlobalAllocator );
}

String CodeExtern::to_string()
{
	String result = String::make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeExtern::to_string( String& result )
{
	if ( ast->Body )
		result.append_fmt( "extern \"%S\"\n{\n%S\n}\n", ast->Name, ast->Body.to_string() );
	else
		result.append_fmt( "extern \"%S\"\n{}\n", ast->Name );
}

String CodeInclude::to_string()
{
	return String::fmt( GlobalAllocator, "#include %S\n", ast->Content );
}

void CodeInclude::to_string( String& result )
{
	result.append_fmt( "#include %S\n", ast->Content );
}

String CodeFriend::to_string( String& result )
{
	String result = String::make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeFriend::to_string( String& result )
{
	result.append_fmt( "friend %S", ast->Declaration->to_string() );

	if ( result[ result.length() -1 ] != ';' )
	{
		result.append( ";" );
	}

	if ( ast->InlineCmt )
		result.append_fmt("  %S", ast->InlineCmt->Content );
	else
		result.append("\n");
}

String CodeFn::to_string( String& result )
{
	String result = String::make( GlobalAllocator, "" );
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
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export" );

	if ( ast->Attributes )
		result.append_fmt( " %S ", ast->Attributes.to_string() );

	if ( ast->Specs )
	{
		for ( SpecifierT spec : ast->Specs )
		{
			if ( ! ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( ast->Attributes || ast->Specs )
		result.append( "\n" );

	if ( ast->ReturnType )
		result.append_fmt( "%S %S(", ast->ReturnType.to_string(), ast->Name );

	else
		result.append_fmt( "%S(", ast->Name );

	if ( Params )
		result.append_fmt( "%S)", ast->Params.to_string() );

	else
		result.append( ")" );

	if ( ast->Specs )
	{
		for ( SpecifierT spec : ast->Specs )
		{
			if ( ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	result.append_fmt( "\n{\n%S\n}\n", ast->Body.to_string() );
}

void CodeFn::to_string_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	if ( ast->Attributes )
		result.append_fmt( "%S ", ast->Attributes.to_string() );

	if ( Specs )
	{
		for ( SpecifierT spec : Specs )
		{
			if ( ! ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( ast->Attributes || ast->Specs )
	{
		result.append("\n" );
	}

	if ( ast->ReturnType )
		result.append_fmt( "%S %S(", ast->ReturnType.to_string(), ast->Name );

	else
		result.append_fmt( "%S(", ast->Name );

	if ( ast->Params )
		result.append_fmt( "%S)", ast->Params.to_string() );

	else
		result.append( ")" );

	if ( ast->Specs )
	{
		for ( SpecifierT spec : Specs )
		{
			if ( ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( ast->InlineCmt )
		result.append_fmt( ";  %S", ast->InlineCmt->Content );
	else
		result.append( ";\n" );
}

String CodeModule::to_string()
{
	String result = String::make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeModule::to_string( String& result )
{
	if (((u32(ModuleFlag::Export) & u32(ast->ModuleFlags)) == u32(ModuleFlag::Export)))
		result.append("export ");

	if (((u32(ModuleFlag::Import) & u32(ast->ModuleFlags)) == u32(ModuleFlag::Import)))
		result.append("import ");

	result.append_fmt( "%S;\n", ast->Name );
}

String CodeNS::to_string()
{
	String result = String::make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeNS::to_string( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	result.append_fmt( "namespace %S\n{\n%S\n}\n", ast->Name , ast->Body.to_string() );
}

String CodeOperator::to_string()
{
	String result = String::make( GlobalAllocator, "" );
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
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	if ( ast->Attributes )
		result.append_fmt( "%S ", ast->Attributes.to_string() );

	if ( ast->Attributes )
		result.append_fmt( "%S ", ast->Attributes.to_string() );

	if ( ast->Specs )
	{
		for ( SpecifierT spec : ast->Specs )
		{
			if ( ! ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( ast->Attributes || ast->Specs )
	{
		result.append("\n" );
	}

	if ( ast->ReturnType )
		result.append_fmt( "%S %S (", ast->ReturnType.to_string(), ast->Name );

	if ( ast->Params )
		result.append_fmt( "%S)", ast->Params.to_string() );

	else
		result.append( ")" );

	if ( ast->Specs )
	{
		for ( SpecifierT spec : ast->Specs )
		{
			if ( ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	result.append_fmt( "\n{\n%S\n}\n"
		, ast->Body.to_string()
	);
}

void CodeOperator::to_string_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	if ( ast->Attributes )
		result.append_fmt( "%S\n", ast->Attributes.to_string() );

	if ( ast->Specs )
	{
		for ( SpecifierT spec : Specs )
		{
			if ( ! ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( ast->Attributes || ast->Specs )
	{
		result.append("\n" );
	}

	result.append_fmt( "%S %S (", ast->ReturnType.to_string(), ast->Name );

	if ( ast->Params )
		result.append_fmt( "%S)", ast->Params.to_string() );

	else
		result.append_fmt( ")" );

	if ( ast->Specs )
	{
		for ( SpecifierT spec : Specs )
		{
			if ( ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}
	}

	if ( ast->InlineCmt )
		result.append_fmt( ";  %S", ast->InlineCmt->Content );
	else
		result.append( ";\n" );
}

String CodeOpCast::to_string()
{
	String result = String::make( GlobalAllocator, "" );
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
		// TODO : Add support for specifies before the operator keyword

		if ( ast->Name && ast->Name.length() )
			result.append_fmt( "%Soperator %S()", ast->Name, ast->ValueType.to_string() );
		else
			result.append_fmt( "operator %S()", ast->ValueType.to_string() );

		for ( SpecifierT spec : Specs )
		{
			if ( ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
			}
		}

		result.append_fmt( "\n{\n%S\n}\n", ast->Body.to_string() );
		break;
	}

	if ( ast->Name && ast->Name.length() )
		result.append_fmt("%Soperator %S()\n{\n%S\n}\n", ast->Name, ast->ValueType.to_string(), ast->Body.to_string() );
	else
		result.append_fmt("operator %S()\n{\n%S\n}\n", ast->ValueType.to_string(), ast->Body.to_string() );
}

void CodeOpCast::to_string_fwd( String& result )
{
	if ( ast->Specs )
	{
		// TODO : Add support for specifies before the operator keyword
	
		result.append_fmt( "operator %S()", ast->ValueType.to_string() );
	
		for ( SpecifierT spec : Specs )
		{
			if ( ESpecifier::is_trailing( spec ) )
			{
				StrC spec_str = ESpecifier::to_str( spec );
				result.append_fmt( " %*s", spec_str.Len, spec_str.Ptr );
			}
		}
	
		if ( ast->InlineCmt )
			result.append_fmt( ";  %S", ast->InlineCmt->Content );
		else
			result.append( ";\n" );
		break;
	}
	
	if ( ast->InlineCmt )
		result.append_fmt("operator %S();  %S", ast->ValueType.to_string() );
	else
		result.append_fmt("operator %S();\n", ast->ValueType.to_string() );
}

String CodeParam::to_string()
{
	String result = String::make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeParam::to_string( String& result )
{
	if ( ast->ValueType == nullptr )
	{
		result.append_fmt( "%S", ast->Name );
		break;
	}

	if ( ast->Name )
		result.append_fmt( "%S %S", ast->ValueType.to_string(), ast->Name );

	else
		result.append_fmt( "%S", ast->ValueType.to_string() );

	if ( ast->Value )
		result.append_fmt( "= %S", ast->Value.to_string() );

	if ( ast->NumEntries - 1 > 0)
	{
		for ( CodeParam param :  ast->Next )
		{
			result.append_fmt( ", %S", param.to_string() );
		}
	}
}

String CodePreprocessCond::to_string()
{
	String result = String::make( GlobalAllocator, "" );
	switch ( ast->Type )
	{
		using namespace ECode;
		case Preprocess_If:
			to_string_if( result );
		break;
		case Preprocess_IfDef:
			to_string_ifdef( result );
		break;
		case Preprocess_IfNotDef
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
	result.append_fmt( "#if %S\n", ast->Content );
}

void CodePreprocessCond::to_string_ifdef( String& result )
{
	result.append_fmt( "#ifdef %S\n", ast->Content );
}

void CodePreprocessCond::to_string_ifndef( String& result )
{
	result.append_fmt( "#ifndef %S\n", ast->Content );
}

void CodePreprocessCond::to_string_elif( String& result )
{
	result.append_fmt( "#elif %S\n", ast->Content );
}

void CodePreprocessCond::to_string_else( String& result )
{
	result.append_fmt( "#else\n" );
}

void CodePreprocessCond::to_string_endif( String& result )
{
	result.append_fmt( "#endif\n" );
}

String CodePragma::to_string()
{
	String result = String::make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodePragma::to_string( String& result )
{
	result.append_fmt( "#pragma %S\n", ast->Content );
}

String CodeSpecifiers::to_string()
{
	String result = String::make( GlobalAllocator, "" );
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
		result.append_fmt( "%.*s ", spec.Len, spec.Ptr );
		idx++;
	}
}

String CodeStruct::to_string()
{
	String result = String::make( GlobalAllocator, "" );
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
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	result.append( "struct " );

	if ( ast->Attributes )
	{
		result.append_fmt( "%S ", ast->Attributes.to_string() );
	}

	if ( ast->ParentType )
	{
		char const* access_level = to_str( ast->ParentAccess );

		result.append_fmt( "%S : %s %S", ast->Name, access_level, ast->ParentType );

		CodeType interface = ast->ParentType->Next->cast< CodeType >();
		if ( interface )
			result.append( "\n" );

		while ( interface )
		{
			result.append_fmt( ", %S", interface.to_string() );
			interface = interface->Next ? interface->Next->cast< CodeType >() : Code { nullptr };
		}
	}
	else if ( ast->Name )
	{
		result.append( ast->Name );
	}

	if ( ast->InlineCmt )
	{
		result.append_fmt( " // %S", ast->InlineCmt->Content );
	}

	result.append_fmt( "\n{\n%S\n}", ast->Body.to_string() );

	if ( ast->Parent == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
		result.append(";\n");
}

void CodeStruct::to_string_fwd( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	if ( ast->Attributes )
		result.append_fmt( "struct %S %S", ast->Attributes.to_string(), ast->Name );

	else result.append_fmt( "struct %S", ast->Name );

	if ( ast->Parent == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
	{
		if ( ast->InlineCmt )
			result.append_fmt(";  %S", ast->InlineCmt->Content );
		else
			result.append(";\n");
	}
}

String CodeTemplate::to_string()
{
	String result = String::make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeTemplate::to_string( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	if ( ast->Params )
		result.append_fmt( "template< %S >\n%S", ast->Params.to_string(), ast->Declaration.to_string() );
	else
		result.append_fmt( "template<>\n%S", ast->Declaration.to_string() );
}

String CodeTypedef::to_string()
{
	String result = String::make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeTypedef::to_string( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	result.append( "typedef ");

	// Determines if the typedef is a function typename
	if ( ast->UnderlyingType->ReturnType )
		result.append( ast->UnderlyingType.to_string() );
	else
		result.append_fmt( "%S %S", ast->UnderlyingType.to_string(), ast->Name );

	if ( ast->UnderlyingType->Type == Typename && ast->UnderlyingType->ArrExpr )
	{
		result.append_fmt( "[ %S ];", ast->UnderlyingType->ArrExpr->to_string() );

		AST* next_arr_expr = ast->UnderlyingType->ArrExpr->Next;
		while ( next_arr_expr )
		{
			result.append_fmt( "[ %S ];", next_arr_expr.to_string() );
			next_arr_expr = next_arr_expr->Next;
		}
	}
	else
	{
		result.append( ";" );
	}

	if ( ast->InlineCmt )
		result.append_fmt("  %S", ast->InlineCmt->Content);
	else
		result.append("\n");
}

String CodeType::to_string()
{
	String result = String::make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeType::to_string( String& result )
{
	#if GEN_USE_NEW_TYPENAME_PARSING
		if ( ast->ReturnType && ast->Params )
		{
			if ( ast->Attributes )
				result.append_fmt( "%S ", ast->Attributes.to_string() );
			else
			{
				if ( ast->Specs )
					result.append_fmt( "%S ( %S ) ( %S ) %S", ast->ReturnType.to_string(), ast->Name, ast->Params.to_string(), ast->Specs.to_string() );
				else
					result.append_fmt( "%S ( %S ) ( %S )", ast->ReturnType.to_string(), ast->Name, ast->Params.to_string() );
			}

			break;
		}
	#else
		if ( ast->ReturnType && ast->Params )
		{
			if ( ast->Attributes )
				result.append_fmt( "%S ", ast->Attributes.to_string() );
			else
			{
				if ( Specs )
					result.append_fmt( "%S %S ( %S ) %S", ast->ReturnType.to_string(), ast->Name, ast->Params.to_string(), ast->Specs.to_string() );
				else
					result.append_fmt( "%S %S ( %S )", ast->ReturnType.to_string(), ast->Name, ast->Params.to_string() );
			}

			break;
		}
	#endif

	if ( ast->Attributes )
		result.append_fmt( "%S ", ast->Attributes.to_string() );

	if ( ast->Specs )
		result.append_fmt( "%S %S", ast->Name, ast->Specs.to_string() );
	else
		result.append_fmt( "%S", ast->Name );

	if ( ast->IsParamPack )
		result.append("...");
}

String CodeUnion::to_string()
{
	String result = String::make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeUnion::to_string( String& result )
{
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	result.append( "union " );

	if ( ast->Attributes )
		result.append_fmt( "%S ", ast->Attributes.to_string() );

	if ( ast->Name )
	{
		result.append_fmt( "%S\n{\n%S\n}"
			, ast->Name
			, ast->Body.to_string()
		);
	}
	else
	{
		// Anonymous union
		result.append_fmt( "\n{\n%S\n}"
			, ast->Body.to_string()
		);
	}

	if ( ast->Parent == nullptr || ( ast->Parent->Type != ECode::Typedef && ast->Parent->Type != ECode::Variable ) )
		result.append(";\n");
}

String CodeUsing::to_string()
{
	String result = String::make( GlobalAllocator, "" );
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
	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	if ( ast->Attributes )
		result.append_fmt( "%S ", ast->Attributes.to_string() );

	if ( ast->UnderlyingType )
	{
		result.append_fmt( "using %S = %S", ast->Name, ast->UnderlyingType.to_string() );

		if ( ast->UnderlyingType->ArrExpr )
		{
			result.append_fmt( "[ %S ]", ast->UnderlyingType->ArrExpr.to_string() );

			AST* next_arr_expr = ast->UnderlyingType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				result.append_fmt( "[ %S ]", next_arr_expr->to_string() );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		result.append( ";" );
	}
	else
		result.append_fmt( "using %S;", ast->Name );

	if ( ast->InlineCmt )
		result.append_fmt("  %S\n", ast->InlineCmt->Content );
	else
		result.append("\n");
}

void CodeUsing::to_string_ns( String& result )
{
	if ( ast->InlineCmt )
		result.append_fmt( "using namespace $S;  %S", ast->Name, ast->InlineCmt->Content );
	else
		result.append_fmt( "using namespace %s;\n", ast->Name );
}

String CodeVar::to_string()
{
	String result = String::make( GlobalAllocator, "" );
	to_string( result );
	return result;
}

void CodeVar::to_string( String& result )
{
	if ( ast->Parent && ast->Parent->Type == ECode::Variable )
	{
		// Its a comma-separated variable ( a NextVar )

		if ( ast->Specs )
			result.append_fmt( "%S ", ast->Specs.to_string() );

		result.append( ast->Name );

		if ( ast->ValueType->ArrExpr )
		{
			result.append_fmt( "[ %S ]", ast->ValueType->ArrExpr.to_string() );

			AST* next_arr_expr = ast->ValueType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				result.append_fmt( "[ %S ]", next_arr_expr->to_string() );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		if ( ast->Value )
			result.append_fmt( " = %S", ast->Value.to_string() );

		// Keep the chain going...
		if ( ast->NextVar )
			result.append_fmt( ", %S", ast->NextVar.to_string() );

		break;
	}

	if ( bitfield_is_equal( u32, ast->ModuleFlags, ModuleFlag::Export ))
		result.append( "export " );

	if ( ast->Attributes || ast->Specs )
	{
		if ( ast->Attributes )
			result.append_fmt( "%S ", ast->Specs.to_string() );

		if ( ast->Specs )
			result.append_fmt( "%S\n", ast->Specs.to_string() );

		result.append_fmt( "%S %S", ast->ValueType.to_string(), ast->Name );

		if ( ast->ValueType->ArrExpr )
		{
			result.append_fmt( "[ %S ]", ast->ValueType->ArrExpr.to_string() );

			AST* next_arr_expr = ast->ValueType->ArrExpr->Next;
			while ( next_arr_expr )
			{
				result.append_fmt( "[ %S ]", next_arr_expr->to_string() );
				next_arr_expr = next_arr_expr->Next;
			}
		}

		if ( ast->BitfieldSize )
			result.append_fmt( " : %S", ast->BitfieldSize.to_string() );

		if ( ast->Value )
			result.append_fmt( " = %S", ast->Value.to_string() );

		if ( ast->NextVar )
			result.append_fmt( ", %S", ast->NextVar.to_string() );

		if ( ast->InlineCmt )
			result.append_fmt(";  %S", ast->InlineCmt->Content);
		else
			result.append( ";\n" );

		break;
	}

	if ( ast->BitfieldSize )
		result.append_fmt( "%S %S : %S", ast->ValueType.to_string(), ast->Name, ast->BitfieldSize.to_string() );

	else if ( ValueType->ArrExpr )
	{
		result.append_fmt( "%S %S[ %S ]", ast->ValueType.to_string(), ast->Name, ast->ValueType->ArrExpr.to_string() );

		AST* next_arr_expr = ast->ValueType->ArrExpr->Next;
		while ( next_arr_expr )
		{
			result.append_fmt( "[ %S ]", next_arr_expr->to_string() );
			next_arr_expr = next_arr_expr->Next;
		}
	}

	else
		result.append_fmt( "%S %S", ast->ValueType.to_string(), ast->Name );

	if ( Value )
		result.append_fmt( " = %S", ast->Value.to_string() );

	if ( NextVar )
		result.append_fmt( ", %S", ast->NextVar.to_string() );

	result.append( ";" );

	if ( InlineCmt )
		result.append_fmt("  %S", ast->InlineCmt->Content);
	else
		result.append("\n");
}
