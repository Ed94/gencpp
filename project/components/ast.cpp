Code Code::Global;
Code Code::Invalid;

AST* AST::duplicate()
{
	using namespace ECode;

	AST* result = make_code().ast;

	mem_copy( result, this, sizeof( AST ) );

	result->Parent = nullptr;
	return result;
}

String AST::to_string()
{
	local_persist thread_local
	char SerializationLevel = 0;

	// TODO : Need to refactor so that intermeidate strings are freed conviently.
	String result = String::make( GlobalAllocator, "" );

	switch ( Type )
	{
		using namespace ECode;

		case Invalid:
			log_failure("Attempted to serialize invalid code! - %s", Parent ? Parent->debug_str() : Name );
		break;

		case NewLine:
			result.append("\n");
		break;

		case Untyped:
		case Execution:
			result.append( Content );
		break;

		case Comment:
		{
			result.append("\n");

			static char line[MaxCommentLineLength];

			s32 left  = Content.length();
			s32 index = 0;
			s32 curr  = 0;
			do
			{
				s32 length = 0;
				while ( left && Content[index] != '\n' )
				{
					length++;
					left--;
					index++;
				}
				index++;

				str_copy( line, Content + curr, length );
				result.append_fmt( "//%.*s", length, line );
				mem_set( line, 0, MaxCommentLineLength);

				length++;
				left--;
				curr = index;
			}
			while ( left--, left > 0 );
		}
		break;

		case Access_Private:
		case Access_Protected:
		case Access_Public:
			result.append( Name );
		break;

		case PlatformAttributes:
			result.append( Content );

		case Class:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes || ParentType )
			{
				result.append( "class " );

				if ( Attributes )
				{
					result.append_fmt( "%s ", Attributes->to_string() );
				}

				if ( ParentType )
				{
					char const* access_level = to_str( ParentAccess );

					result.append_fmt( "%s : %s %s\n{\n%s\n};"
						, Name
						, access_level
						, ParentType->to_string()
						, Body->to_string()
					);

					CodeType interface = Next->cast<CodeType>();
					if ( interface )
						result.append("\n");

					while ( interface )
					{
						result.append_fmt( ", %s", interface.to_string() );

						interface = interface->Next->cast<CodeType>();
					}
				}
				else
				{
					result.append_fmt( "%s \n{\n%s\n}", Name, Body->to_string() );
				}
			}
			else
			{
				result.append_fmt( "class %s\n{\n%s\n}", Name, Body->to_string() );
			}

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";");
		}
		break;

		case Class_Fwd:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "class %s %s", Attributes->to_string(), Name );

			else result.append_fmt( "class %s", Name );

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";");
		}
		break;

		case Enum:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes || UnderlyingType )
			{
				result.append( "enum " );

				if ( Attributes )
					result.append_fmt( "%s ", Attributes->to_string() );

				if ( UnderlyingType )
					result.append_fmt( "%s : %s\n{\n%s\n}"
						, Name
						, UnderlyingType->to_string()
						, Body->to_string()
					);

				else result.append_fmt( "%s\n{\n%s\n}"
					, Name
					, Body->to_string()
				);
			}
			else result.append_fmt( "enum %s\n{\n%s\n}"
				, Name
				, Body->to_string()
			);

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";");
		}
		break;

		case Enum_Fwd:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			result.append_fmt( "enum %s : %s", Name, UnderlyingType->to_string() );

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";");
		}
		break;

		case Enum_Class:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes || UnderlyingType )
			{
				result.append( "enum class " );

				if ( Attributes )
				{
					result.append_fmt( "%s ", Attributes->to_string() );
				}

				if ( UnderlyingType )
				{
					result.append_fmt( "%s : %s\n{\n%s\n}"
						, Name
						, UnderlyingType->to_string()
						, Body->to_string()
					);
				}
				else
				{
					result.append_fmt( "%s\n{\n%s\n}"
						, Name
						, Body->to_string()
					);
				}
			}
			else
			{
				result.append_fmt( "enum class %s\n{\n%s\n}"
					, Body->to_string()
				);
			}

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";");
		}
		break;

		case Enum_Class_Fwd:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			result.append( "enum class " );

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			result.append_fmt( "%s : %s", Name, UnderlyingType->to_string() );

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";");
		}
		break;

		case Export_Body:
		{
			result.append_fmt( "export\n{\n" );

			Code curr = { this };
			s32  left = NumEntries;
			while ( left-- )
			{
				result.append_fmt( "%s", curr.to_string() );
				++curr;
			}

			result.append_fmt( "};" );
		}
		break;

		case Extern_Linkage:
			result.append_fmt( "extern \"%s\"\n{\n%s\n}"
				, Name
				, Body->to_string()
			);
		break;

		case Friend:
			result.append_fmt( "friend %s", Declaration->to_string() );

			if ( result[ result.length() -1 ] != ';' )
				result.append( ";" );
		break;

		case Function:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			if ( Specs )
				result.append_fmt( "%s", Specs->to_string() );

			if ( ReturnType )
				result.append_fmt( "%s %s(", ReturnType->to_string(), Name );

			else
				result.append_fmt( "%s(", Name );

			if ( Params )
				result.append_fmt( "%s)", Params->to_string() );

			else
				result.append( "void)" );

			if ( Specs )
			{
				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
						result.append_fmt( " %s", (char const*)ESpecifier::to_str( spec ) );
				}
			}

			result.append_fmt( "\n{\n%s\n}"
				, Body->to_string()
			);
		}
		break;

		case Function_Fwd:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			if ( Specs )
				result.append_fmt( "%s", Specs->to_string() );

			if ( ReturnType )
				result.append_fmt( "%s %s(", ReturnType->to_string(), Name );

			else
				result.append_fmt( "%s(", Name );

			if ( Params )
				result.append_fmt( "%s)", Params->to_string() );

			else
				result.append( "void)" );

			if ( Specs )
			{
				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
						result.append_fmt( " %s", (char const*)ESpecifier::to_str( spec ) );
				}
			}

			result.append( ";" );
		}
		break;

		case Module:
			if (((u32(ModuleFlag::Export) & u32(ModuleFlags)) == u32(ModuleFlag::Export)))
				result.append("export ");

			if (((u32(ModuleFlag::Import) & u32(ModuleFlags)) == u32(ModuleFlag::Import)))
				result.append("import ");

			result.append_fmt( "%s;", Name );
			break;

		case Namespace:
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			result.append_fmt( "namespace %s\n{\n%s\n}"
				, Name
				, Body->to_string()
			);
		break;

		case Operator:
		case Operator_Member:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			if ( Attributes )
				result.append_fmt( "%s\n", Attributes->to_string() );

			if ( ReturnType )
				result.append_fmt( "%s %s (", ReturnType->to_string(), Name );

			if ( Params )
				result.append_fmt( "%s)", Params->to_string() );

			else
				result.append( "void)" );

			if ( Specs )
			{
				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
						result.append_fmt( " %s", (char const*)ESpecifier::to_str( spec ) );
				}
			}

			result.append_fmt( "\n{\n%s\n}"
				, Body->to_string()
			);
		}
		break;

		case Operator_Fwd:
		case Operator_Member_Fwd:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			if ( Specs )
				result.append_fmt( "%s", Specs->to_string() );

			result.append_fmt( "%s %s (", ReturnType->to_string(), Name );

			if ( Params )
				result.append_fmt( "%s)", Params->to_string() );

			else
				result.append_fmt( "void)" );

			if ( Specs )
			{
				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
						result.append_fmt( " %s", (char const*)ESpecifier::to_str( spec ) );
				}
			}

			result.append( ";" );
		}
		break;

		case Operator_Cast:
		{
			if ( Specs )
			{
				if ( Name && Name.length() )
					result.append_fmt( "%.*soperator %s()", Name.length(), Name, ValueType->to_string() );
				else
					result.append_fmt( "operator %s()", ValueType->to_string() );

				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
						result.append_fmt( " %s", (char const*)ESpecifier::to_str( spec ) );
				}

				result.append_fmt( "\n{\n%s\n}", Body->to_string() );
				break;
			}

			if ( Name && Name.length() )
				result.append_fmt("%.*soperator %s()\n{\n%s\n}", Name.length(), Name, ValueType->to_string(), Body->to_string() );
			else
				result.append_fmt("operator %s()\n{\n%s\n}", ValueType->to_string(), Body->to_string() );
		}
		break;

		case Operator_Cast_Fwd:
			if ( Specs )
			{
				result.append_fmt( "operator %s()", ValueType->to_string() );

				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
						result.append_fmt( " %s", (char const*)ESpecifier::to_str( spec ) );
				}

				result.append( ";" );
				break;
			}

			result.append_fmt("operator %s();", ValueType->to_string() );
		break;

		case Parameters:
		{
			if ( ValueType == nullptr )
			{
				result.append_fmt( "%s", Name );
				break;
			}

			if ( Name )
				result.append_fmt( "%s %s", ValueType->to_string(), Name );

			else
				result.append_fmt( "%s", ValueType->to_string() );

			if ( Value )
				result.append_fmt( "= %s", Value->to_string() );

			if ( NumEntries - 1 > 0)
			{
				for ( CodeParam param :  CodeParam { (AST_Param*) Next } )
				{
					result.append_fmt( ", %s", param.to_string() );
				}
			}
		}
		break;
		case Preprocess_Define:
			result.append_fmt( "#define %s %s", Name, Content );
		break;

		case Preprocess_If:
			result.append_fmt( "#if %s", Content );
		break;

		case Preprocess_IfDef:
			result.append_fmt( "#ifdef %s", Content );
		break;

		case Preprocess_IfNotDef:
			result.append_fmt( "#ifndef %s", Content );
		break;

		case Preprocess_Include:
			result.append_fmt( "#include \"%s\"\n", Content );
		break;

		case Preprocess_ElIf:
			result.append_fmt( "#elif %s", Content );
		break;

		case Preprocess_Else:
			result.append_fmt( "\n#else" );
		break;

		case Preprocess_EndIf:
			result.append_fmt( "#endif" );
		break;

		case Preprocess_Pragma:
			result.append_fmt( "#pragma %s", Content );
		break;

		case Specifiers:
		{
			s32 idx  = 0;
			s32 left = NumEntries;
			while ( left-- )
			{
				if ( ESpecifier::is_trailing( ArrSpecs[idx]) && ArrSpecs[idx] != ESpecifier::Const )
				{
					idx++;
					continue;
				}

				result.append_fmt( "%s ", (char const*)ESpecifier::to_str( ArrSpecs[idx]) );
				idx++;
			}
		}
		break;

		case Struct:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Name == nullptr)
			{
				result.append_fmt( "struct\n{\n%s\n};", Body->to_string() );
				break;
			}

			if ( Attributes || ParentType )
			{
				result.append( "struct " );

				if ( Attributes )
					result.append_fmt( "%s ", Attributes->to_string() );

				if ( ParentType )
				{
					char const* access_level = to_str( ParentAccess );

					result.append_fmt( "%s : %s %s\n{\n%s\n};"
						, Name
						, access_level
						, ParentType->to_string()
						, Body->to_string()
					);

					CodeType interface = Next->cast<CodeType>();
					if ( interface )
						result.append("\n");

					while ( interface )
					{
						result.append_fmt( ", public %s", interface.to_string() );

						interface = interface->Next->cast<CodeType>();
					}
				}
				else
				{
					if ( Name )

					result.append_fmt( "%s \n{\n%s\n}", Name, Body->to_string() );
				}
			}
			else
			{
				result.append_fmt( "struct %s\n{\n%s\n}", Name, Body->to_string() );
			}

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";");
		}
		break;

		case Struct_Fwd:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "struct %s %s", Attributes->to_string(), Name );

			else result.append_fmt( "struct %s", Name );

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";");
		}
		break;

		case Template:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			result.append_fmt( "template< %s >\n%s", Params->to_string(), Declaration->to_string() );
		}
		break;

		case Typedef:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			result.append( "typedef ");

			result.append_fmt( "%s %s", UnderlyingType->to_string(), Name );

			if ( UnderlyingType->Type == Typename && UnderlyingType->ArrExpr )
			{
				result.append_fmt( "[%s];", UnderlyingType->ArrExpr->to_string() );
			}
			else
			{
				result.append( ";" );
			}
		}
		break;

		case Typename:
		{
			if ( Attributes || Specs )
			{
				if ( Attributes )
					result.append_fmt( "%s ", Attributes->to_string() );

				if ( Specs )
					result.append_fmt( "%s %s", Name, Specs->to_string() );

				else
					result.append_fmt( "%s", Name );
			}
			else
			{
				result.append_fmt( "%s", Name );
			}
		}
		break;

		case Union:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			result.append( "union " );

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			if ( Name )
			{
				result.append_fmt( "%s\n{\n%s\n}"
					, Name
					, Body->to_string()
				);
			}
			else
			{
				// Anonymous union
				result.append_fmt( "\n{\n%s\n}"
					, Body->to_string()
				);
			}

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";");
		}
		break;

		case Using:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			if ( UnderlyingType )
			{
				result.append_fmt( "using %s = %s", Name, UnderlyingType->to_string() );

				if ( UnderlyingType->ArrExpr )
					result.append_fmt( "[%s]", UnderlyingType->ArrExpr->to_string() );

				result.append( ";" );
			}
			else
				result.append_fmt( "using %s;", Name );
		}
		break;

		case Using_Namespace:
			result.append_fmt( "using namespace %s;", Name );
		break;

		case Variable:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes || Specs )
			{
				if ( Attributes )
					result.append_fmt( "%s ", Specs->to_string() );

				if ( Specs )
					result.append_fmt( "%s\n", Specs->to_string() );

				result.append_fmt( "%s %s", ValueType->to_string(), Name );

				if ( ValueType->ArrExpr )
					result.append_fmt( "[%s]", ValueType->ArrExpr->to_string() );

				if ( BitfieldSize )
					result.append_fmt( " : %s", BitfieldSize->to_string() );

				if ( Value )
					result.append_fmt( " = %s", Value->to_string() );

				result.append( ";" );

				break;
			}

			if ( BitfieldSize )
				result.append_fmt( "%s : %s", ValueType->to_string(), BitfieldSize->to_string() );

			else if ( UnderlyingType->ArrExpr )
				result.append_fmt( "%s %s[%s];", UnderlyingType->to_string(), Name, UnderlyingType->ArrExpr->to_string() );

			else
				result.append_fmt( "%s %s;", UnderlyingType->to_string(), Name );
		}
		break;

#if 0
		{
			Code curr = Front->cast<Code>();
			s32  left = NumEntries;
			while ( left -- )
			{
				result.append_fmt( "%s", curr.to_string() );
				++curr;
			}
		}
		break;
#endif

		case Enum_Body:
		case Class_Body:
		case Extern_Linkage_Body:
		case Function_Body:
		case Global_Body:
		case Namespace_Body:
		case Struct_Body:
		case Union_Body:
		{
			Code curr = Front->cast<Code>();
			s32  left = NumEntries;
			while ( left -- )
			{
				result.append_fmt( "%s", curr.to_string() );

				if ( curr->Type != ECode::NewLine )
					result.append( "\n" );

				++curr;
			}
		}
		break;
	}

	return result;
}

bool AST::is_equal( AST* other )
{
	if ( Type != other->Type )
		return false;

	switch ( Type )
	{
		case ECode::Typedef:
		case ECode::Typename:
		{
			if ( Name != other->Name )
				return false;

			return true;
		}
	}

	if ( Name != other->Name )
		return false;

	return true;
}

bool AST::validate_body()
{
	using namespace ECode;

#define CheckEntries( Unallowed_Types )                                                               \
	do                                                                                                \
	{                                                                                                 \
		for ( Code entry : cast<CodeBody>() )                                                         \
		{                                                                                             \
			switch ( entry->Type )                                                                    \
			{                                                                                         \
				Unallowed_Types                                                                       \
					log_failure( "AST::validate_body: Invalid entry in body %s", entry.debug_str() ); \
					return false;                                                                     \
			}                                                                                         \
		}                                                                                             \
	}                                                                                                 \
	while (0);

	switch ( Type )
	{
		case Class_Body:
			CheckEntries( GEN_AST_BODY_CLASS_UNALLOWED_TYPES );
		break;
		case Enum_Body:
			for ( Code entry : cast<CodeBody>() )
			{
				if ( entry->Type != Untyped )
				{
					log_failure( "AST::validate_body: Invalid entry in enum body (needs to be untyped or comment) %s", entry.debug_str() );
					return false;
				}
			}
		break;
		case Export_Body:
			CheckEntries( GEN_AST_BODY_CLASS_UNALLOWED_TYPES );
		break;
		case Extern_Linkage:
			CheckEntries( GEN_AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES );
		break;
		case Function_Body:
			CheckEntries( GEN_AST_BODY_FUNCTION_UNALLOWED_TYPES );
		break;
		case Global_Body:
			for (Code entry : cast<CodeBody>())
			{
				switch (entry->Type)
				{
					case Access_Public:
					case Access_Protected:
					case Access_Private:
					case PlatformAttributes:
					case Class_Body:
					case Enum_Body:
					case Execution:
					case Friend:
					case Function_Body:
					case Global_Body:
					case Namespace_Body:
					case Operator_Member:
					case Operator_Member_Fwd:
					case Parameters:
					case Specifiers:
					case Struct_Body:
					case Typename:
						log_failure("AST::validate_body: Invalid entry in body %s", entry.debug_str());
					return false;
				}
			}
		break;
		case Namespace_Body:
			CheckEntries( GEN_AST_BODY_NAMESPACE_UNALLOWED_TYPES );
		break;
		case Struct_Body:
			CheckEntries( GEN_AST_BODY_STRUCT_UNALLOWED_TYPES );
		break;
		case Union_Body:
			for ( Code entry : Body->cast<CodeBody>() )
			{
				if ( entry->Type != Untyped )
				{
					log_failure( "AST::validate_body: Invalid entry in union body (needs to be untyped or comment) %s", entry.debug_str() );
					return false;
				}
			}
		break;

		default:
			log_failure( "AST::validate_body: Invalid this AST does not have a body %s", debug_str() );
			return false;
	}

	return false;

#undef CheckEntries
}
