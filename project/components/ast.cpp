#pragma once
#include "static_data.cpp"

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
			log_failure("Attempted to serialize invalid code! - %S", Parent ? Parent->debug_str() : Name );
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
			if ( Prev && Prev->Type != Comment && Prev->Type != NewLine )
				result.append( "\n" );

			static char line[ MaxCommentLineLength ];

			char const* end       = & scast(String, Content).back();
			char*       scanner   = Content.Data;
			s32         curr      = 0;
			do
			{
				char const* next   = scanner;
				s32         length = 0;
				while ( next != end && scanner[ length ] != '\n' )
				{
					next = scanner + length;
					length++;
				}
				length++;

				str_copy( line, scanner, length );
				result.append_fmt( "//%.*s", length, line );
				mem_set( line, 0, MaxCommentLineLength );

				scanner += length;
			}
			while ( scanner <= end );

			if ( result.back() != '\n' )
				result.append( "\n" );
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
					result.append_fmt( "%S ", Attributes->to_string() );
				}

				if ( ParentType )
				{
					char const* access_level = to_str( ParentAccess );

					result.append_fmt( "%S : %s %S", Name, access_level, ParentType );

					CodeType interface = Next->cast< CodeType >();
					if ( interface )
						result.append( "\n" );

					while ( interface )
					{
						result.append_fmt( ", %S", interface.to_string() );
						interface = interface->Next ? interface->Next->cast< CodeType >() : Code { nullptr };
					}

					result.append_fmt( "\n{\n%S\n}", Body->to_string() );
				}
				else
				{
					result.append_fmt( "%S \n{\n%S\n}", Name, Body->to_string() );
				}
			}
			else
			{
				result.append_fmt( "class %S\n{\n%S\n}", Name, Body->to_string() );
			}

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";\n");
		}
		break;

		case Class_Fwd:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "class %S %S", Attributes->to_string(), Name );

			else result.append_fmt( "class %S", Name );

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";\n");
		}
		break;

		case Constructor:
		{
			result.append( Parent->Name );

			if ( Params )
				result.append_fmt( "( %S )", Params->to_string() );
			else
				result.append( "(void)" );

			if ( InitializerList )
				result.append_fmt( " : %S", InitializerList->to_string() );

			result.append_fmt( "\n{\n%S\n}\n", Body->to_string() );
		}
		break;

		case Constructor_Fwd:
		{
			result.append( Parent->Name );

			if ( Params )
				result.append_fmt( "( %S )", Params->to_string() );
			else
				result.append( "(void);\n" );
		}
		break;

		case Destructor:
		{
			if ( Specs )
			{
				CodeSpecifiers specs = Specs->cast<CodeSpecifiers>();

				if ( specs.has( ESpecifier::Virtual ) )
					result.append_fmt( "virtual ~%S()", Parent->Name );
				else
					result.append_fmt( "~%S()", Parent->Name );
			}
			else
				result.append_fmt( "~%S()", Parent->Name );

			result.append_fmt( "\n{\n%S\n}\n", Body->to_string() );
		}
		break;

		case Destructor_Fwd:
		{
			if ( Specs )
			{
				CodeSpecifiers specs = Specs->cast<CodeSpecifiers>();

				if ( specs.has( ESpecifier::Virtual ) )
					result.append_fmt( "virtual ~%S();\n", Parent->Name );
				else
					result.append_fmt( "~%S()", Parent->Name );

				if ( specs.has( ESpecifier::Pure ) )
					result.append( " = 0;\n" );
			}
			else
				result.append_fmt( "~%S();\n", Parent->Name );
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
					result.append_fmt( "%S ", Attributes->to_string() );

				if ( UnderlyingType )
					result.append_fmt( "%S : %S\n{\n%S\n}"
						, Name
						, UnderlyingType->to_string()
						, Body->to_string()
					);

				else result.append_fmt( "%S\n{\n%S\n}", Name, Body->to_string() );
			}
			else result.append_fmt( "enum %S\n{\n%S\n}", Name, Body->to_string() );

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";\n");
		}
		break;

		case Enum_Fwd:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "%S ", Attributes->to_string() );

			result.append_fmt( "enum %S : %S", Name, UnderlyingType->to_string() );

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";\n");
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
					result.append_fmt( "%S ", Attributes->to_string() );
				}

				if ( UnderlyingType )
				{
					result.append_fmt( "%S : %S\n{\n%S\n}", Name, UnderlyingType->to_string(), Body->to_string() );
				}
				else
				{
					result.append_fmt( "%S\n{\n%S\n}", Name, Body->to_string() );
				}
			}
			else
			{
				result.append_fmt( "enum class %S\n{\n%S\n}", Body->to_string() );
			}

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";\n");
		}
		break;

		case Enum_Class_Fwd:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			result.append( "enum class " );

			if ( Attributes )
				result.append_fmt( "%S ", Attributes->to_string() );

			result.append_fmt( "%S : %S", Name, UnderlyingType->to_string() );

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";\n");
		}
		break;

		case Export_Body:
		{
			result.append_fmt( "export\n{\n" );

			Code curr = { this };
			s32  left = NumEntries;
			while ( left-- )
			{
				result.append_fmt( "%S", curr.to_string() );
				++curr;
			}

			result.append_fmt( "};\n" );
		}
		break;

		case Extern_Linkage:
			result.append_fmt( "extern \"%S\"\n{\n%S\n}\n", Name, Body->to_string() );
		break;

		case Friend:
			result.append_fmt( "friend %S", Declaration->to_string() );

			if ( result[ result.length() -1 ] != ';' )
				result.append( ";\n" );
		break;

		case Function:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "%S ", Attributes->to_string() );

			if ( Specs )
				result.append_fmt( "%S", Specs->to_string() );

			if ( ReturnType )
				result.append_fmt( "%S %S(", ReturnType->to_string(), Name );

			else
				result.append_fmt( "%S(", Name );

			if ( Params )
				result.append_fmt( "%S)", Params->to_string() );

			else
				result.append( "void)" );

			if ( Specs )
			{
				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
					{
						StrC spec_str = ESpecifier::to_str( spec );
						result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
					}
				}
			}

			result.append_fmt( "\n{\n%S\n}\n", Body->to_string() );
		}
		break;

		case Function_Fwd:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "%S ", Attributes->to_string() );

			if ( Specs )
				result.append_fmt( "%S", Specs->to_string() );

			if ( ReturnType )
				result.append_fmt( "%S %S(", ReturnType->to_string(), Name );

			else
				result.append_fmt( "%S(", Name );

			if ( Params )
				result.append_fmt( "%S)", Params->to_string() );

			else
				result.append( "void)" );

			if ( Specs )
			{
				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
					{
						StrC spec_str = ESpecifier::to_str( spec );
						result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
					}
				}
			}

			result.append( ";\n" );
		}
		break;

		case Module:
			if (((u32(ModuleFlag::Export) & u32(ModuleFlags)) == u32(ModuleFlag::Export)))
				result.append("export ");

			if (((u32(ModuleFlag::Import) & u32(ModuleFlags)) == u32(ModuleFlag::Import)))
				result.append("import ");

			result.append_fmt( "%S;\n", Name );
			break;

		case Namespace:
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			result.append_fmt( "namespace %S\n{\n%S\n}\n", Name , Body->to_string() );
		break;

		case Operator:
		case Operator_Member:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "%S ", Attributes->to_string() );

			if ( Attributes )
				result.append_fmt( "%S\n", Attributes->to_string() );

			if ( ReturnType )
				result.append_fmt( "%S %S (", ReturnType->to_string(), Name );

			if ( Params )
				result.append_fmt( "%S)", Params->to_string() );

			else
				result.append( "void)" );

			if ( Specs )
			{
				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
					{
						StrC spec_str = ESpecifier::to_str( spec );
						result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
					}
				}
			}

			result.append_fmt( "\n{\n%S\n}\n"
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
				result.append_fmt( "%S\n", Attributes->to_string() );

			if ( Specs )
				result.append_fmt( "%S\n", Specs->to_string() );

			result.append_fmt( "%S %S (", ReturnType->to_string(), Name );

			if ( Params )
				result.append_fmt( "%S)", Params->to_string() );

			else
				result.append_fmt( "void)" );

			if ( Specs )
			{
				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
					{
						StrC spec_str = ESpecifier::to_str( spec );
						result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
					}
				}
			}

			result.append( ";\n" );
		}
		break;

		case Operator_Cast:
		{
			if ( Specs )
			{
				if ( Name && Name.length() )
					result.append_fmt( "%Soperator %S()", Name, ValueType->to_string() );
				else
					result.append_fmt( "operator %S()", ValueType->to_string() );

				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
					{
						StrC spec_str = ESpecifier::to_str( spec );
						result.append_fmt( " %.*s", spec_str.Len, spec_str.Ptr );
					}
				}

				result.append_fmt( "\n{\n%S\n}\n", Body->to_string() );
				break;
			}

			if ( Name && Name.length() )
				result.append_fmt("%Soperator %S()\n{\n%S\n}\n", Name, ValueType->to_string(), Body->to_string() );
			else
				result.append_fmt("operator %S()\n{\n%S\n}\n", ValueType->to_string(), Body->to_string() );
		}
		break;

		case Operator_Cast_Fwd:
			if ( Specs )
			{
				result.append_fmt( "operator %S()", ValueType->to_string() );

				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
					{
						StrC spec_str = ESpecifier::to_str( spec );
						result.append_fmt( " %*s", spec_str.Len, spec_str.Ptr );
					}
				}

				result.append( ";" );
				break;
			}

			result.append_fmt("operator %S();\n", ValueType->to_string() );
		break;

		case Parameters:
		{
			if ( ValueType == nullptr )
			{
				result.append_fmt( "%S", Name );
				break;
			}

			if ( Name )
				result.append_fmt( "%S %S", ValueType->to_string(), Name );

			else
				result.append_fmt( "%S", ValueType->to_string() );

			if ( Value )
				result.append_fmt( "= %S", Value->to_string() );

			if ( NumEntries - 1 > 0)
			{
				for ( CodeParam param :  CodeParam { (AST_Param*) Next } )
				{
					result.append_fmt( ", %S", param.to_string() );
				}
			}
		}
		break;
		case Preprocess_Define:
			result.append_fmt( "#define %S %S\n", Name, Content );
		break;

		case Preprocess_If:
			result.append_fmt( "#if %S\n", Content );
		break;

		case Preprocess_IfDef:
			result.append_fmt( "#ifdef %S\n", Content );
		break;

		case Preprocess_IfNotDef:
			result.append_fmt( "#ifndef %S\n", Content );
		break;

		case Preprocess_Include:
			result.append_fmt( "#include \"%S\"\n", Content );
		break;

		case Preprocess_ElIf:
			result.append_fmt( "#elif %S\n", Content );
		break;

		case Preprocess_Else:
			result.append_fmt( "#else\n" );
		break;

		case Preprocess_EndIf:
			result.append_fmt( "#endif\n" );
		break;

		case Preprocess_Pragma:
			result.append_fmt( "#pragma %S\n", Content );
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

				StrC spec = ESpecifier::to_str( ArrSpecs[idx] );
				result.append_fmt( "%.*s ", spec.Len, spec.Ptr );
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
				result.append_fmt( "struct\n{\n%S\n};\n", Body->to_string() );
				break;
			}

			if ( Attributes || ParentType )
			{
				result.append( "struct " );

				if ( Attributes )
					result.append_fmt( "%S ", Attributes->to_string() );

				if ( ParentType )
				{
					char const* access_level = to_str( ParentAccess );

					result.append_fmt( "%S : %s %S", Name, access_level, ParentType );

					CodeType interface = Next->cast< CodeType >();
					if ( interface )
						result.append( "\n" );

					while ( interface )
					{
						result.append_fmt( ", %S", interface.to_string() );

						interface = interface->Next ? interface->Next->cast< CodeType >() : Code { nullptr };
					}

					result.append_fmt( "\n{\n%S\n}", Body->to_string() );
				}
				else
				{
					if ( Name )
						result.append_fmt( "%S \n{\n%S\n}", Name, Body->to_string() );
				}
			}
			else
			{
				result.append_fmt( "struct %S\n{\n%S\n}", Name, Body->to_string() );
			}

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";\n");
		}
		break;

		case Struct_Fwd:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "struct %S %S", Attributes->to_string(), Name );

			else result.append_fmt( "struct %S", Name );

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";\n");
		}
		break;

		case Template:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			result.append_fmt( "template< %S >\n%S", Params->to_string(), Declaration->to_string() );
		}
		break;

		case Typedef:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			result.append( "typedef ");

			if ( IsFunction )
				result.append( UnderlyingType->to_string() );
			else
				result.append_fmt( "%S %S", UnderlyingType->to_string(), Name );

			if ( UnderlyingType->Type == Typename && UnderlyingType->ArrExpr )
			{
				result.append_fmt( "[%S];", UnderlyingType->ArrExpr->to_string() );
			}
			else
			{
				result.append( ";\n" );
			}
		}
		break;

		case Typename:
		{
			if ( Attributes || Specs )
			{
				if ( Attributes )
					result.append_fmt( "%S ", Attributes->to_string() );

				if ( Specs )
					result.append_fmt( "%S %S", Name, Specs->to_string() );

				else
					result.append_fmt( "%S", Name );
			}
			else
			{
				result.append_fmt( "%S", Name );
			}
		}
		break;

		case Union:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			result.append( "union " );

			if ( Attributes )
				result.append_fmt( "%S ", Attributes->to_string() );

			if ( Name )
			{
				result.append_fmt( "%S\n{\n%S\n}"
					, Name
					, Body->to_string()
				);
			}
			else
			{
				// Anonymous union
				result.append_fmt( "\n{\n%S\n}"
					, Body->to_string()
				);
			}

			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
				result.append(";\n");
		}
		break;

		case Using:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes )
				result.append_fmt( "%S ", Attributes->to_string() );

			if ( UnderlyingType )
			{
				result.append_fmt( "using %S = %S", Name, UnderlyingType->to_string() );

				if ( UnderlyingType->ArrExpr )
					result.append_fmt( "[%S]", UnderlyingType->ArrExpr->to_string() );

				result.append( ";\n" );
			}
			else
				result.append_fmt( "using %S;\n", Name );
		}
		break;

		case Using_Namespace:
			result.append_fmt( "using namespace %s;\n", Name );
		break;

		case Variable:
		{
			if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))
				result.append( "export " );

			if ( Attributes || Specs )
			{
				if ( Attributes )
					result.append_fmt( "%S ", Specs->to_string() );

				if ( Specs )
					result.append_fmt( "%S\n", Specs->to_string() );

				result.append_fmt( "%S %S", ValueType->to_string(), Name );

				if ( ValueType->ArrExpr )
					result.append_fmt( "[%S]", ValueType->ArrExpr->to_string() );

				if ( BitfieldSize )
					result.append_fmt( " : %S", BitfieldSize->to_string() );

				if ( Value )
					result.append_fmt( " = %S", Value->to_string() );

				result.append( ";\n" );

				break;
			}

			if ( BitfieldSize )
				result.append_fmt( "%S %S : %S;\n", ValueType->to_string(), Name, BitfieldSize->to_string() );

			else if ( UnderlyingType->ArrExpr )
				result.append_fmt( "%S %S[%S];\n", UnderlyingType->to_string(), Name, UnderlyingType->ArrExpr->to_string() );

			else
				result.append_fmt( "%S %S;\n", UnderlyingType->to_string(), Name );
		}
		break;

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
				result.append_fmt( "%S", curr.to_string() );
				++curr;
			}
		}
		break;
	}

	return result;
}

bool AST::is_equal( AST* other )
{
/*
	AST values are either some u32 value, a cached string, or a pointer to another AST.

	u32 values are compared by value.
	Cached strings are compared by pointer.
	AST nodes are compared with AST::is_equal.
*/
	if ( other == nullptr )
		return false;

	if ( Type != other->Type )
		return false;

	switch ( Type )
	{
		using namespace ECode;

		case NewLine:
		case Access_Public:
		case Access_Protected:
		case Access_Private:
		case Preprocess_Else:
		case Preprocess_EndIf:
			return true;

		case Comment:
		case Execution:
		case PlatformAttributes:
		case Untyped:
		{
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return Content == other->Content;
		}

		case Class_Fwd:
		case Struct_Fwd:
		{
			if ( Name != other->Name )
				return false;
			if ( ParentType != other->ParentType )
				return false;
			if ( ParentAccess != other->ParentAccess )
				return false;
			if ( Attributes ? ! Attributes->is_equal( other->Attributes ) : other->Attributes != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Class:
		case Struct:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( ParentType != other->ParentType )
				return false;
			if ( ParentAccess != other->ParentAccess )
				return false;
			if ( Attributes ? ! Attributes->is_equal( other->Attributes ) : other->Attributes != nullptr )
				return false;
			if ( Body ? ! Body->is_equal( other->Body ) : other->Body != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;

			return true;
		}

		case Constructor:
		{
			if ( InitializerList ? ! InitializerList->is_equal( other->InitializerList ) : other->InitializerList != nullptr )
				return false;
			if ( Params ? ! Params->is_equal( other->Params ) : other->Params != nullptr )
				return false;
			if ( Body ? ! Body->is_equal( other->Body ) : other->Body != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Constructor_Fwd:
		{
			if ( InitializerList ? ! InitializerList->is_equal( other->InitializerList ) : other->InitializerList != nullptr )
				return false;
			if ( Params ? ! Params->is_equal( other->Params ) : other->Params != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Destructor:
		{
			if ( Specs ? ! Specs->is_equal( other->Specs ) : other->Specs != nullptr )
				return false;
			if ( Body ? ! Body->is_equal( other->Body ) : other->Body != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Destructor_Fwd:
		{
			if ( Specs ? ! Specs->is_equal( other->Specs ) : other->Specs != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Enum:
		case Enum_Class:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( Attributes ? ! Attributes->is_equal( other->Attributes ) : other->Attributes != nullptr )
				return false;
			if ( UnderlyingType ? ! UnderlyingType->is_equal( other->UnderlyingType ) : other->UnderlyingType != nullptr )
				return false;
			if ( Body ? ! Body->is_equal( other->Body ) : other->Body != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Enum_Fwd:
		case Enum_Class_Fwd:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( Attributes ? ! Attributes->is_equal( other->Attributes ) : other->Attributes != nullptr )
				return false;
			if ( UnderlyingType ? ! UnderlyingType->is_equal( other->UnderlyingType ) : other->UnderlyingType != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Extern_Linkage:
		{
			if ( Name != other->Name )
				return false;
			if ( Body ? ! Body->is_equal( other->Body ) : other->Body != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Friend:
		{
			if ( Name != other->Name )
				return false;
			if ( Declaration ? ! Declaration->is_equal( other->Declaration ) : other->Declaration != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Function:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( ReturnType ? ! ReturnType->is_equal( other->ReturnType ) : other->ReturnType != nullptr )
				return false;
			if ( Attributes ? ! Attributes->is_equal( other->Attributes ) : other->Attributes != nullptr )
				return false;
			if ( Specs ? ! Specs->is_equal( other->Specs ) : other->Specs != nullptr )
				return false;
			if ( Params ? ! Params->is_equal( other->Params ) : other->Params != nullptr )
				return false;
			if ( Body ? ! Body->is_equal( other->Body ) : other->Body != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Function_Fwd:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( ReturnType ? ! ReturnType->is_equal( other->ReturnType ) : other->ReturnType != nullptr )
				return false;
			if ( Attributes ? ! Attributes->is_equal( other->Attributes ) : other->Attributes != nullptr )
				return false;
			if ( Specs ? ! Specs->is_equal( other->Specs ) : other->Specs != nullptr )
				return false;
			if ( Params ? ! Params->is_equal( other->Params ) : other->Params != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Module:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Namespace:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( Body ? ! Body->is_equal( other->Body ) : other->Body != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Operator:
		case Operator_Member:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( ReturnType ? ! ReturnType->is_equal( other->ReturnType ) : other->ReturnType != nullptr )
				return false;
			if ( Attributes ? ! Attributes->is_equal( other->Attributes ) : other->Attributes != nullptr )
				return false;
			if ( Specs ? ! Specs->is_equal( other->Specs ) : other->Specs != nullptr )
				return false;
			if ( Params ? ! Params->is_equal( other->Params ) : other->Params != nullptr )
				return false;
			if ( Body ? ! Body->is_equal( other->Body ) : other->Body != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Operator_Fwd:
		case Operator_Member_Fwd:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( ReturnType ? ! ReturnType->is_equal( other->ReturnType ) : other->ReturnType != nullptr )
				return false;
			if ( Attributes ? ! Attributes->is_equal( other->Attributes ) : other->Attributes != nullptr )
				return false;
			if ( Specs ? ! Specs->is_equal( other->Specs ) : other->Specs != nullptr )
				return false;
			if ( Params ? ! Params->is_equal( other->Params ) : other->Params != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Operator_Cast:
		{
			if ( Name != other->Name )
				return false;
			if ( Specs ? ! Specs->is_equal( other->Specs ) : other->Specs != nullptr )
				return false;
			if ( ValueType ? ! ValueType->is_equal( other->ValueType ) : other->ValueType != nullptr )
				return false;
			if ( Body ? ! Body->is_equal( other->Body ) : other->Body != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Operator_Cast_Fwd:
		{
			if ( Name != other->Name )
				return false;
			if ( Specs ? ! Specs->is_equal( other->Specs ) : other->Specs != nullptr )
				return false;
			if ( ValueType ? ! ValueType->is_equal( other->ValueType ) : other->ValueType != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Parameters:
		{
			if ( NumEntries > 1 )
			{
				if ( NumEntries != other->NumEntries )
					return false;

				AST* curr       = this;
				AST* curr_other = other;
				while ( curr != nullptr  )
				{
					if ( ! curr->is_equal( curr_other ) )
						return false;

					curr       = curr->Next;
					curr_other = curr_other->Next;
				}

				return true;
			}

			if ( Name != other->Name )
				return false;
			if ( ValueType ? ! ValueType->is_equal( other->ValueType ) : other->ValueType != nullptr )
				return false;
			if ( Value ? ! Value->is_equal( other->Value ) : other->Value != nullptr )
				return false;
			if ( ArrExpr ? ! ArrExpr->is_equal( other->ArrExpr ) : other->ArrExpr != nullptr )
				return false;

			return true;
		}

		case Preprocess_Define:
		{
			if ( Name != other->Name )
				return false;
			if ( Content != other->Content )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Preprocess_If:
		case Preprocess_IfDef:
		case Preprocess_IfNotDef:
		case Preprocess_ElIf:
		{
			if ( Content != other->Content )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Preprocess_Include:
		case Preprocess_Pragma:
		{
			if ( Content != other->Content )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Specifiers:
		{
			if ( NumEntries != other->NumEntries )
				return false;
			if ( Name != other->Name )
				return false;
			for ( s32 idx = 0; idx < NumEntries; ++idx )
			{
				if ( ArrSpecs[ idx ] != other->ArrSpecs[ idx ] )
					return false;
			}
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Template:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( Params ? ! Params->is_equal( other->Params ) : other->Params != nullptr )
				return false;
			if ( Declaration ? ! Declaration->is_equal( other->Declaration ) : other->Declaration != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Typedef:
		{
			if ( IsFunction != other->IsFunction )
				return false;
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( UnderlyingType != other->UnderlyingType )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}
		case Typename:
		{
			if ( Name != other->Name )
				return false;
			if ( Specs ? ! Specs->is_equal( other->Specs ) : other->Specs != nullptr )
				return false;
			if ( ArrExpr ? ! ArrExpr->is_equal( other->ArrExpr ) : other->ArrExpr != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Union:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( Attributes ? ! Attributes->is_equal( other->Attributes ) : other->Attributes != nullptr )
				return false;
			if ( Body ? ! Body->is_equal( other->Body ) : other->Body != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Using:
		case Using_Namespace:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( Attributes ? ! Attributes->is_equal( other->Attributes ) : other->Attributes != nullptr )
				return false;
			if ( UnderlyingType ? ! UnderlyingType->is_equal( other->UnderlyingType ) : other->UnderlyingType != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Variable:
		{
			if ( ModuleFlags != other->ModuleFlags )
				return false;
			if ( Name != other->Name )
				return false;
			if ( Attributes ? ! Attributes->is_equal( other->Attributes ) : other->Attributes != nullptr )
				return false;
			if ( Specs ? ! Specs->is_equal( other->Specs ) : other->Specs != nullptr )
				return false;
			if ( ValueType ? ! ValueType->is_equal( other->ValueType ) : other->ValueType != nullptr )
				return false;
			if ( BitfieldSize ? ! BitfieldSize->is_equal( other->BitfieldSize ) : other->BitfieldSize != nullptr )
				return false;
			if ( Value ? ! Value->is_equal( other->Value ) : other->Value != nullptr )
				return false;
			if ( Prev ? ! Prev->is_equal( other->Prev ) : other->Prev != nullptr )
				return false;
			if ( Next ? ! Next->is_equal( other->Next ) : other->Next != nullptr )
				return false;

			return true;
		}

		case Class_Body:
		case Enum_Body:
		case Export_Body:
		case Global_Body:
		case Namespace_Body:
		case Struct_Body:
		case Union_Body:
		{
			if ( NumEntries != other->NumEntries )
				return false;

			if ( Front != other->Front )
				return false;

			if ( Back != other->Back )
				return false;

			AST* curr       = Front;
			AST* curr_other = other->Front;
			while ( curr != nullptr )
			{
				if ( ! curr->is_equal( curr_other ) )
					return false;

				curr       = curr->Next;
				curr_other = curr_other->Next;
			}

			return true;
		}
	}

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
