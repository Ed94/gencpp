Code Code::Global;
Code Code::Invalid;

AST* AST::duplicate()
{
	using namespace ECode;

	AST*
	result = make_code().ast;
#ifndef GEN_USE_RECURSIVE_AST_DUPLICATION
	mem_copy( result, this, sizeof( AST ) );
	result->Parent = nullptr;
#else
	// TODO : Stress test this...
	switch ( Type )
	{
		case Invalid:
			log_failure("Attempted to duplicate invalid code! - \n%s", Parent ? Parent->debug_str() : Name );
			return nullptr
		case Untyped:
		case Comment:
		case Execution:
		case Access_Private:
		case Access_Protected:
		case Access_Public:
		case PlatformAttributes:
		case Preprocessor_Include:
		case Module:
		case Specifiers:
		case Using_Namespace:
			mem_copy( result, this, sizeof( AST ) );
		break;

		case Extern_Linkage:
		case Friend:
			mem_copy( result, this, sizeof( AST ) );

			if (Value)
				result->Value = Value->duplicate();
		break;

		case Class:
		case Struct:
		case Enum:
			mem_copy( result, this, sizeof( AST ) );

			if ( Attributes)
				result->Attributes = Attributes->duplicate();

			if ( ParentType )
				result->ParentType = ParentType->duplicate();

			result->Body = Body->duplicate();
		break;

		case Enum_Fwd:
		case Class_Fwd:
		case Struct_Fwd:
			mem_copy( result, this, sizeof( AST ) );

			if ( Attributes)
				result->Attributes = Attributes->duplicate();

			if ( ParentType )
				result->ParentType = ParentType->duplicate();
		break;

		case Function:
		case Operator:
		case Operator_Member:
			mem_copy( result, this, sizeof( AST ) );

			if ( Attributes)
				result->Attributes = Attributes->duplicate();

			if ( Specs )
				result->ParentType = ParentType->duplicate();

			if ( ReturnType )
				result->ReturnType = ReturnType->duplicate();

			if ( Params )
				result->Params = Params->duplicate();

			result->Body = Body->duplicate();
		break;

		case Function_Fwd:
		case Operator_Fwd:
		case Operator_Member_Fwd:
			mem_copy( result, this, sizeof( AST ) );

			if ( Attributes)
				result->Attributes = Attributes->duplicate();

			if ( Specs )
				result->ParentType = ParentType->duplicate();

			if ( ReturnType )
				result->ReturnType = ReturnType->duplicate();

			if ( Params )
				result->Params = Params->duplicate();
		break;

		case Namespace:
			mem_copy( result, this, sizeof( AST ) );

			result->Body = Body->duplicate();
		break;

		case Operator_Cast:
			mem_copy( result, this, sizeof( AST ) );

			result->ValueType = ValueType->duplicate();
			result->Body      = Body->duplicate();
		break;
		case Operator_Cast_Fwd:
			mem_copy( result, this, sizeof( AST ) );

			result->ValueType = ValueType->duplicate();
		break;

		case Parameters:
			mem_copy( result, this, sizeof( AST ) );

			result->NumEntries = 0;
			result->Last = nullptr;
			result->Next = nullptr;

			if ( NumEntries - 1 > 0 )
			{
				CodeParam parent = result->cast<CodeParam>();
				for ( CodeParam param : Next->cast<CodeParam>() )
				{
					parent.append( param );
				}
			}
		break;

		case Template:
			mem_copy( result, this, sizeof( AST ) );

			result->Params      = Params->duplicate();
			result->Declaration = Declaration->duplicate();
		break;

		case Typename:
			mem_copy( result, this, sizeof( AST ) );

			if (Attributes)
				result->Attributes = Attributes->duplicate();

			if ( Specs )
				result->Specs = Specs->duplicate();

			if ( ArrExpr )
				result->ArrExpr = ArrExpr->duplicate();
		break;

		case Typedef:
		case Using:
			mem_copy( result, this, sizeof( AST ) );

			if (Attributes)
				result->Attributes = Attributes->duplicate();

			if ( UnderlyingType )
				result->UnderlyingType = UnderlyingType->duplicate();
		break;

		case Union:
			mem_copy( result, this, sizeof( AST ) );

			if ( Attributes)
				result->Attributes = Attributes->duplicate();

			result->Body = Body->duplicate();
		break;

		case Variable:
			mem_copy( result, this, sizeof( AST ) );

			if (Attributes)
				result->Attributes = Attributes->duplicate();

			if ( Specs )
				result->Specs = Specs->duplicate();

			result->ValueType = UnderlyingType->duplicate();

			if ( Value )
				result->Value = Value->duplicate();
		break;

		case Class_Body:
		case Enum_Body:
		case Export_Body:
		case Extern_Linkage_Body:
		case Function_Body:
		case Global_Body:
		case Namespace_Body:
		case Struct_Body:
		case Union_Body:
			CodeBody
			body         = cast<CodeBody>();
			body->Name   = Name;
			body->Type   = Type;
			for ( Code entry : cast<CodeBody>() )
			{
				result->append( entry.ast );
			}
		break;
	}
#endif

	return result;
}

String AST::to_string()
{
#	define ProcessModuleFlags()                                      \
	if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))  \
		result.append( "export " );                                  \
																     \
	if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Import ))  \
		result.append( "import " );                                  \

	local_persist thread_local
	char SerializationLevel = 0;

#if defined(GEN_BENCHMARK) && defined(GEN_BENCHMARK_SERIALIZATION)
	u64 time_start = time_rel_ms();
#endif

	// TODO : Need to refactor so that intermeidate strings are freed conviently.
	String result = String::make( GlobalAllocator, "" );

	switch ( Type )
	{
		using namespace ECode;

		case Invalid:
			log_failure("Attempted to serialize invalid code! - %s", Parent ? Parent->debug_str() : Name );
		break;

		case Untyped:
		case Execution:
			result.append( Content );
		break;

		case Comment:
		{
			static char line[MaxCommentLineLength];

			s32 left  = Content.length();
			s32 index = 0;
			do
			{
				s32 length = 0;
				while ( left && Content[index] != '\n' )
				{
					length++;
					left--;
				}

				str_copy( line, Content, length );
				line[length] = '\0';

				result.append_fmt( "// %s", line );
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
			ProcessModuleFlags();

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
					result.append_fmt( "%s \n{\n%s\n};", Name, Body->to_string() );
				}
			}
			else
			{
				result.append_fmt( "class %s\n{\n%s\n};", Name, Body->to_string() );
			}
		}
		break;

		case Class_Fwd:
		{
			ProcessModuleFlags();

			if ( Attributes )
				result.append_fmt( "class %s %s;", Attributes->to_string(), Name );

			else result.append_fmt( "class %s;", Name );
		}
		break;

		case Enum:
		{
			ProcessModuleFlags();

			if ( Attributes || UnderlyingType )
			{
				result.append( "enum " );

				if ( Attributes )
					result.append_fmt( "%s ", Attributes->to_string() );

				if ( UnderlyingType )
					result.append_fmt( "%s : %s\n{\n%s\n};"
						, Name
						, UnderlyingType->to_string()
						, Body->to_string()
					);

				else result.append_fmt( "%s\n{\n%s\n};"
					, Name
					, Body->to_string()
				);
			}
			else result.append_fmt( "enum %s\n{\n%s\n};"
				, Name
				, Body->to_string()
			);
		}
		break;

		case Enum_Fwd:
		{
			ProcessModuleFlags();

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			result.append_fmt( "enum %s : %s;", Name, UnderlyingType->to_string() );
		}
		break;

		case Enum_Class:
		{
			ProcessModuleFlags();

			if ( Attributes || UnderlyingType )
			{
				result.append( "enum class " );

				if ( Attributes )
				{
					result.append_fmt( "%s ", Attributes->to_string() );
				}

				if ( UnderlyingType )
				{
					result.append_fmt( "%s : %s\n{\n%s\n};"
						, Name
						, UnderlyingType->to_string()
						, Body->to_string()
					);
				}
				else
				{
					result.append_fmt( "%s\n{\n%s\n};"
						, Name
						, Body->to_string()
					);
				}
			}
			else
			{
				result.append_fmt( "enum class %s\n{\n%s\n};"
					, Body->to_string()
				);
			}
		}
		break;

		case Enum_Class_Fwd:
		{
			ProcessModuleFlags();

			result.append( "enum class " );

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			result.append_fmt( "%s : %s;", Name, UnderlyingType->to_string() );
		}
		break;

		case Export_Body:
		{
			result.append_fmt( "export\n{\n" );

			Code curr = { this };
			s32  left = NumEntries;
			while ( left-- )
			{
				result.append_fmt( "%s\n", curr.to_string() );
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
			ProcessModuleFlags();

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			if ( Specs )
				result.append_fmt( "%s\n", Specs->to_string() );

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
				CodeSpecifiers specs = cast<CodeSpecifiers>();

				for ( SpecifierT spec : specs )
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
			ProcessModuleFlags();

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			if ( Specs )
				result.append_fmt( "%s\n", Specs->to_string() );

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
				CodeSpecifiers specs = cast<CodeSpecifiers>();

				for ( SpecifierT spec : specs )
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
			ProcessModuleFlags();

			result.append_fmt( "namespace %s\n{\n%s}"
				, Name
				, Body->to_string()
			);
		break;

		case Operator:
		case Operator_Member:
		{
			ProcessModuleFlags();

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
				CodeSpecifiers specs = cast<CodeSpecifiers>();

				for ( SpecifierT spec : specs )
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
			ProcessModuleFlags();

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
				CodeSpecifiers specs = cast<CodeSpecifiers>();

				for ( SpecifierT spec : specs )
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
				result.append_fmt( "operator %s()" );

				CodeSpecifiers specs = cast<CodeSpecifiers>();

				for ( SpecifierT spec : specs )
				{
					if ( ESpecifier::is_trailing( spec ) )
						result.append_fmt( " %s", (char const*)ESpecifier::to_str( spec ) );
				}

				result.append_fmt( "\n{\n%s\n}", Body->to_string() );
				break;
			}

			result.append_fmt("operator %s()\n{\n%s\n}", ValueType->to_string(), Body->to_string() );
		}
		break;

		case Operator_Cast_Fwd:
			if ( Specs )
			{
				result.append_fmt( "operator %s()" );

				CodeSpecifiers specs = cast<CodeSpecifiers>();

				for ( SpecifierT spec : specs )
				{
					if ( ESpecifier::is_trailing( spec ) )
						result.append_fmt( " %s", (char const*)ESpecifier::to_str( spec ) );
				}

				result.append_fmt( ";", Body->to_string() );
				break;
			}

			result.append_fmt("operator %s();", ValueType->to_string() );
		break;

		case Parameters:
		{
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

		case Preprocessor_Include:
			result.append_fmt( "#include \"%s\"", Name );
		break;

		case Specifiers:
		{
			s32 idx  = 0;
			s32 left = NumEntries;
			while ( left-- )
			{
				if ( ESpecifier::is_trailing( ArrSpecs[idx])  )
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
			ProcessModuleFlags();

			if ( Name == nullptr)
			{
				result.append( "struct\n{\n%s\n};", Body->to_string() );
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

					result.append_fmt( "%s \n{\n%s\n};", Name, Body->to_string() );
				}
			}
			else
			{
				result.append_fmt( "struct %s\n{\n%s\n};", Name, Body->to_string() );
			}
		}
		break;

		case Struct_Fwd:
		{
			ProcessModuleFlags();

			if ( Attributes )
				result.append_fmt( "struct %s %s;", Attributes->to_string(), Name );

			else result.append_fmt( "struct %s;", Name );
		}
		break;

		case Template:
		{
			ProcessModuleFlags();

			result.append_fmt( "template< %s >\n%s", Params->to_string(), Declaration->to_string() );
		}
		break;

		case Typedef:
		{
			ProcessModuleFlags();

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
			ProcessModuleFlags();

			result.append( "union " );

			if ( Attributes )
				result.append_fmt( "%s ", Attributes->to_string() );

			if ( Name )
			{
				result.append_fmt( "%s\n{\n%s\n};"
					, Name
					, Body->to_string()
				);
			}
			else
			{
				// Anonymous union
				result.append_fmt( "\n{\n%s\n};"
					, Body->to_string()
				);
			}
		}
		break;

		case Using:
		{
			ProcessModuleFlags();

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
			ProcessModuleFlags();

			if ( Attributes || Specs )
			{
				if ( Attributes )
					result.append_fmt( "%s ", Specs->to_string() );

				if ( Specs )
					result.append_fmt( "%s\n", Specs->to_string() );

				result.append_fmt( "%s %s", ValueType->to_string(), Name );

				if ( ValueType->ArrExpr )
					result.append_fmt( "[%s]", ValueType->ArrExpr->to_string() );

				if ( Value )
					result.append_fmt( " = %s", Value->to_string() );

				result.append( ";" );

				break;
			}

			if ( UnderlyingType->ArrExpr )
				result.append_fmt( "%s %s[%s];", UnderlyingType->to_string(), Name, UnderlyingType->ArrExpr->to_string() );

			else
				result.append_fmt( "%s %s;", UnderlyingType->to_string(), Name );
		}
		break;

		case Class_Body:
		case Enum_Body:
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
				result.append_fmt( "%s\n", curr.to_string() );
				++curr;
			}
		}
		break;
	}

#if defined(GEN_BENCHMARK) && defined(GEN_BENCHMARK_SERIALIZATION)
	log_fmt("AST::to_string() time taken: %llu for: %s\n", time_rel_ms() - time_start, result );
#endif
	return result;
#undef ProcessModuleFlags
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

#define CheckEntries( Unallowed_Types )                                                                \
	do                                                                                                 \
	{                                                                                                  \
		for ( Code entry : cast<CodeBody>() )                                                          \
		{                                                                                              \
			switch ( entry->Type )                                                                     \
			{                                                                                          \
				Unallowed_Types                                                                        \
					log_failure( "AST::validate_body: Invalid entry in body %s", entry.debug_str() );  \
					return false;                                                                      \
			}                                                                                          \
		}                                                                                              \
	}                                                                                                  \
	while (0);

	switch ( Type )
	{
		case Class_Body:
			CheckEntries( AST_BODY_CLASS_UNALLOWED_TYPES );
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
			CheckEntries( AST_BODY_CLASS_UNALLOWED_TYPES );
		break;
		case Extern_Linkage:
			CheckEntries( AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES );
		break;
		case Function_Body:
			CheckEntries( AST_BODY_FUNCTION_UNALLOWED_TYPES );
		break;
		case Global_Body:
			CheckEntries( AST_BODY_GLOBAL_UNALLOWED_TYPES );
		break;
		case Namespace_Body:
			CheckEntries( AST_BODY_NAMESPACE_UNALLOWED_TYPES );
		break;
		case Struct_Body:
			CheckEntries( AST_BODY_STRUCT_UNALLOWED_TYPES );
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
}
