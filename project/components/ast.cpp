#pragma once
#include "static_data.cpp"

Code Code::Global;
Code Code::Invalid;

char const* AST::debug_str()
{
	if ( Parent )
	{
		String
		result = String::make_reserve( GlobalAllocator, kilobytes(1) );
		result.append_fmt(
				"\n\tType    : %s"
				"\n\tParent  : %s %s"
				"\n\tName    : %s"
			, type_str()
			, Parent->type_str()
			, Parent->Name, Name ? Name : ""
		);

		return result;
	}

	String
	result = String::make_reserve( GlobalAllocator, kilobytes(1) );
	result.append_fmt(
			"\n\tType    : %s"
			"\n\tName    : %s"
		, type_str()
		, Name ? Name : ""
	);

	return result;
}

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
		case Comment:
			result.append( Content );
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

			// Check if it can have an end-statement
			if ( Parent == nullptr || ( Parent->Type != ECode::Typedef && Parent->Type != ECode::Variable ) )
			{
				if ( InlineCmt )
					result.append_fmt( "; // %S\n", InlineCmt->Content );
				else
					result.append(";\n");
			}
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
			{
				if ( InlineCmt )
					result.append_fmt( "(void); // %S\n", InlineCmt->Content );
				else
					result.append( "(void);\n" );
			}
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
					result.append( " = 0;" );
			}
			else
				result.append_fmt( "~%S();", Parent->Name );

			if ( InlineCmt )
				result.append_fmt( "  %S", InlineCmt->Content );
			else
				result.append("\n");
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
			{
				if ( InlineCmt )
					result.append_fmt(";  %S", InlineCmt->Content );
				else
					result.append(";\n");
			}
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
			{
				if ( InlineCmt )
					result.append_fmt(";  %S", InlineCmt->Content );
				else
					result.append(";\n");
			}
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
			{
				result.append( ";" );
			}

			if ( InlineCmt )
				result.append_fmt("  %S", InlineCmt->Content );
			else
				result.append("\n");
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
				result.append( ")" );

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
				result.append( ")" );

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

			if ( InlineCmt )
				result.append_fmt( ";  %S", InlineCmt->Content );
			else
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
				result.append( ")" );

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
				result.append_fmt( ")" );

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

			if ( InlineCmt )
				result.append_fmt( ";  %S", InlineCmt->Content );
			else
				result.append( ";\n" );
		}
		break;

		case Operator_Cast:
		{
			if ( Specs )
			{
				// TODO : Add support for specifies before the operator keyword

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
				// TODO : Add support for specifies before the operator keyword

				result.append_fmt( "operator %S()", ValueType->to_string() );

				for ( SpecifierT spec : Specs->cast<CodeSpecifiers>() )
				{
					if ( ESpecifier::is_trailing( spec ) )
					{
						StrC spec_str = ESpecifier::to_str( spec );
						result.append_fmt( " %*s", spec_str.Len, spec_str.Ptr );
					}
				}

				if ( InlineCmt )
					result.append_fmt( ";  %S", InlineCmt->Content );
				else
					result.append( ";\n" );
				break;
			}

			if ( InlineCmt )
				result.append_fmt("operator %S();  %S", ValueType->to_string() );
			else
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
			result.append_fmt( "#include %S\n", Content );
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
			{
				if ( InlineCmt )
					result.append_fmt(";  %S", InlineCmt->Content );
				else
					result.append(";\n");
			}
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
			{
				if ( InlineCmt )
					result.append_fmt(";  %S", InlineCmt->Content );
				else
					result.append(";\n");
			}
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
				result.append( ";" );
			}

			if ( InlineCmt )
				result.append_fmt("  %S", InlineCmt->Content);
			else
				result.append("\n");
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

			if ( IsParamPack )
				result.append("...");
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

				result.append( ";" );
			}
			else
				result.append_fmt( "using %S;", Name );

			if ( InlineCmt )
				result.append_fmt("  %S\n", InlineCmt->Content );
			else
				result.append("\n");
		}
		break;

		case Using_Namespace:
			if ( InlineCmt )
				result.append_fmt( "using namespace $S;  %S", Name, InlineCmt->Content );
			else
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

				if ( InlineCmt )
					result.append_fmt(";  %S", InlineCmt->Content);
				else
					result.append( ";\n" );

				break;
			}

			if ( BitfieldSize )
				result.append_fmt( "%S %S : %S;", ValueType->to_string(), Name, BitfieldSize->to_string() );

			else if ( UnderlyingType->ArrExpr )
				result.append_fmt( "%S %S[%S];", UnderlyingType->to_string(), Name, UnderlyingType->ArrExpr->to_string() );

			else
				result.append_fmt( "%S %S;", UnderlyingType->to_string(), Name );

			if ( InlineCmt )
				result.append_fmt("  %S", InlineCmt->Content);
			else
				result.append("\n");
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
	{
		log_fmt( "AST::is_equal: other is null\nAST: %S", debug_str() );
		return false;
	}

	if ( Type != other->Type )
	{
		log_fmt("AST::is_equal: Type check failure with other\nAST: %S\nOther: %S"
			, debug_str()
			, other->debug_str()
		);

		return false;
	}

	switch ( Type )
	{
		using namespace ECode;

	#define check_member_val( val )                         \
	if ( val != other->val )                                \
	{                                                       \
		log_fmt("AST::is_equal: Member - " #val " failed\n" \
		        "AST  : %S\n"                               \
		        "Other: %S\n"                               \
		    , debug_str()                                   \
		    , other->debug_str()                            \
		);                                                  \
	                                                        \
		return false;                                       \
	}

	#define check_member_str( str )                               \
	if ( str != other->str )                                      \
	{                                                             \
		log_fmt("AST::is_equal: Member string - "#str " failed\n" \
				"AST  : %S\n"                                     \
				"Other: %S\n"                                     \
			, debug_str()                                         \
			, other->debug_str()                                  \
		);                                                        \
	                                                              \
		return false;                                             \
	}

	#define check_member_content( content ) \
	if ( content != other->content )       \
	{                                      \
		log_fmt("AST::is_equal: Member content - "#content " failed\n"    \
				"AST  : %S\n"                                             \
				"Other: %S\n"                                             \
			, debug_str()                                                 \
			, other->debug_str()                                          \
		);                                                                \
                                                                          \
		log_fmt("Content cannot be trusted to be unique with this check " \
			"so it must be verified by eye for now\n"                     \
			"AST   Content:\n%S\n"                                        \
			"Other Content:\n%S\n"                                        \
			, content                                                     \
			, other->content                                              \
		);                                                                \
	}

	#define check_member_ast( ast )                                                              \
	if ( ast )                                                                                   \
	{                                                                                            \
		if ( other->ast == nullptr )                                                             \
		{                                                                                        \
			log_fmt("AST::is_equal: Failed for member " #ast " other equivalent param is null\n" \
					"AST  : %s\n"                                                                \
					"Other: %s\n"                                                                \
					"For ast member: %s\n"                                                       \
				, debug_str()                                                                    \
				, other->debug_str()                                                             \
				, ast->debug_str()                                                               \
			);                                                                                   \
                                                                                                 \
			return false;                                                                        \
		}                                                                                        \
                                                                                                 \
		if ( ! ast->is_equal( other->ast ) )                                                     \
		{                                                                                        \
			log_fmt( "AST::is_equal: Failed for " #ast"\n"                                       \
			         "AST  : %S\n"                                                               \
			         "Other: %S\n"                                                               \
			         "For     ast member: %S\n"                                                  \
			         "other's ast member: %S\n"                                                  \
				, debug_str()                                                                    \
				, other->debug_str()                                                             \
				, ast->debug_str()                                                               \
				, other->ast->debug_str()                                                        \
			);                                                                                   \
		                                                                                         \
			return false;                                                                        \
		}                                                                                        \
	}

		case NewLine:
		case Access_Public:
		case Access_Protected:
		case Access_Private:
		case Preprocess_Else:
		case Preprocess_EndIf:
			return true;


		// Comments are not validated.
		case Comment:
			// return true;

		case Execution:
		case PlatformAttributes:
		case Untyped:
		{
			check_member_content( Content );
			// check_member_ast( Prev );
			// check_member_ast( Next );
		}

		case Class_Fwd:
		case Struct_Fwd:
		{
			check_member_str( Name );
			check_member_ast( ParentType );
			check_member_val( ParentAccess );
			check_member_ast( Attributes );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Class:
		case Struct:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ParentType );
			check_member_val( ParentAccess );
			check_member_ast( Attributes );
			check_member_ast( Body );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Constructor:
		{
			check_member_ast( InitializerList );
			check_member_ast( Params );
			check_member_ast( Body );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Constructor_Fwd:
		{
			check_member_ast( InitializerList );
			check_member_ast( Params );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Destructor:
		{
			check_member_ast( Specs );
			check_member_ast( Body );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Destructor_Fwd:
		{
			check_member_ast( Specs );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Enum:
		case Enum_Class:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Attributes );
			check_member_ast( UnderlyingType );
			check_member_ast( Body );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Enum_Fwd:
		case Enum_Class_Fwd:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Attributes );
			check_member_ast( UnderlyingType );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Extern_Linkage:
		{
			check_member_str( Name );
			check_member_ast( Body );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Friend:
		{
			check_member_str( Name );
			check_member_ast( Declaration );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Function:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ReturnType );
			check_member_ast( Attributes );
			check_member_ast( Specs );
			check_member_ast( Params );
			check_member_ast( Body );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Function_Fwd:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ReturnType );
			check_member_ast( Attributes );
			check_member_ast( Specs );
			check_member_ast( Params );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Module:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Namespace:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Body );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Operator:
		case Operator_Member:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ReturnType );
			check_member_ast( Attributes );
			check_member_ast( Specs );
			check_member_ast( Params );
			check_member_ast( Body );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Operator_Fwd:
		case Operator_Member_Fwd:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ReturnType );
			check_member_ast( Attributes );
			check_member_ast( Specs );
			check_member_ast( Params );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Operator_Cast:
		{
			check_member_str( Name );
			check_member_ast( Specs );
			check_member_ast( ValueType );
			check_member_ast( Body );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Operator_Cast_Fwd:
		{
			check_member_str( Name );
			check_member_ast( Specs );
			check_member_ast( ValueType );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Parameters:
		{
			if ( NumEntries > 1 )
			{
				AST* curr       = this;
				AST* curr_other = other;
				while ( curr != nullptr  )
				{
					if ( curr )
					{
						if ( curr_other == nullptr )
						{
							log_fmt("AST::is_equal: Failed for parameter, other equivalent param is null\n"
							        "AST  : %S\n"
							        "Other: %S\n"
							        "For ast member: %S\n"
							    , curr->debug_str()
							);

							return false;
						}

						if ( ! curr->is_equal( curr_other ) )
						{
							log_fmt( "AST::is_equal: Failed for parameter\n"
									"AST  : %S\n"
									"Other: %S\n"
									"For     ast member: %S\n"
									"other's ast member: %S\n"
								, debug_str()
								, other->debug_str()
								, curr->debug_str()
								, curr_other->debug_str()
							);
							return false;
						}
					}

					curr       = curr->Next;
					curr_other = curr_other->Next;
				}

				check_member_val( NumEntries );

				return true;
			}

			check_member_str( Name );
			check_member_ast( ValueType );
			check_member_ast( Value );
			check_member_ast( ArrExpr );

			return true;
		}

		case Preprocess_Define:
		{
			check_member_str( Name );
			check_member_content( Content );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Preprocess_If:
		case Preprocess_IfDef:
		case Preprocess_IfNotDef:
		case Preprocess_ElIf:
		{
			check_member_content( Content );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Preprocess_Include:
		case Preprocess_Pragma:
		{
			check_member_content( Content );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Specifiers:
		{
			check_member_val( NumEntries );
			check_member_str( Name );
			for ( s32 idx = 0; idx < NumEntries; ++idx )
			{
				check_member_val( ArrSpecs[ idx ] );
			}
			// check_member_ast( Prev );
			// check_member_ast( Next );
			return true;
		}

		case Template:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Params );
			check_member_ast( Declaration );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Typedef:
		{
			check_member_val( IsFunction );
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Specs );
			check_member_ast( UnderlyingType );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}
		case Typename:
		{
			check_member_val( IsParamPack );
			check_member_str( Name );
			check_member_ast( Specs );
			check_member_ast( ArrExpr );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Union:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( Attributes );
			check_member_ast( Body );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Using:
		case Using_Namespace:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( UnderlyingType );
			check_member_ast( Attributes );
			// check_member_ast( Prev );
			// check_member_ast( Next );

			return true;
		}

		case Variable:
		{
			check_member_val( ModuleFlags );
			check_member_str( Name );
			check_member_ast( ValueType );
			check_member_ast( BitfieldSize );
			check_member_ast( Value );
			check_member_ast( Attributes );
			check_member_ast( Specs );
			// check_member_ast( Prev );
			// check_member_ast( Next );

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
			check_member_ast( Front );
			check_member_ast( Back );

			AST* curr       = Front;
			AST* curr_other = other->Front;
			while ( curr != nullptr )
			{
				if ( curr_other == nullptr )
				{
					log_fmt("AST::is_equal: Failed for body, other equivalent param is null\n"
					        "AST  : %S\n"
					        "Other: %S\n"
					        "For ast member: %S\n"
					    , curr->debug_str()
					);

					return false;
				}

				if ( ! curr->is_equal( curr_other ) )
				{
					log_fmt( "AST::is_equal: Failed for body\n"
							"AST  : %S\n"
							"Other: %S\n"
							"For     ast member: %S\n"
							"other's ast member: %S\n"
						, debug_str()
						, other->debug_str()
						, curr->debug_str()
						, curr_other->debug_str()
					);

					return false;
				}

				curr       = curr->Next;
				curr_other = curr_other->Next;
			}

			check_member_val( NumEntries );

			return true;
		}

	#undef check_member_val
	#undef check_member_str
	#undef check_member_ast
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
