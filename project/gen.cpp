// ReSharper disable CppClangTidyClangDiagnosticSwitchEnum

#if ! defined(GEN_DONT_ENFORCE_GEN_TIME_GUARD) && ! defined(GEN_TIME)
#error Gen.hpp : GEN_TIME not defined
#endif

#include "gen.push_ignores.inline.hpp"

//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
//! Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES
#	include "gen.dep.cpp"
#endif

#include "gen.hpp"

namespace gen
{
#pragma region StaticData
	// TODO : Convert global allocation strategy to use the dual-scratch allocator for a contextual scope.
	global AllocatorInfo  GlobalAllocator;
	global Array<Arena>   Global_AllocatorBuckets;

	global Array< Pool >  CodePools         = { nullptr };
	global Array< Arena > StringArenas      = { nullptr };

	global StringTable StringCache;

	global Arena LexArena;

	global AllocatorInfo Allocator_DataArrays       = heap();
	global AllocatorInfo Allocator_CodePool         = heap();
	global AllocatorInfo Allocator_Lexer            = heap();
	global AllocatorInfo Allocator_StringArena      = heap();
	global AllocatorInfo Allocator_StringTable      = heap();
	global AllocatorInfo Allocator_TypeTable        = heap();
#pragma endregion StaticData

#pragma region Constants
	global CodeType t_auto;
	global CodeType t_void;
	global CodeType t_int;
	global CodeType t_bool;
	global CodeType t_char;
	global CodeType t_wchar_t;
	global CodeType t_class;
	global CodeType t_typename;

	#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
	global CodeType t_b32;

	global CodeType t_s8;
	global CodeType t_s16;
	global CodeType t_s32;
	global CodeType t_s64;

	global CodeType t_u8;
	global CodeType t_u16;
	global CodeType t_u32;
	global CodeType t_u64;

	global CodeType t_sw;
	global CodeType t_uw;

	global CodeType t_f32;
	global CodeType t_f64;
	#endif

	global Code access_public;
	global Code access_protected;
	global Code access_private;

	global Code module_global_fragment;
	global Code module_private_fragment;

	global Code pragma_once;

	global CodeSpecifier spec_const;
	global CodeSpecifier spec_consteval;
	global CodeSpecifier spec_constexpr;
	global CodeSpecifier spec_constinit;
	global CodeSpecifier spec_extern_linkage;
	global CodeSpecifier spec_global;
	global CodeSpecifier spec_inline;
	global CodeSpecifier spec_internal_linkage;
	global CodeSpecifier spec_local_persist;
	global CodeSpecifier spec_mutable;
	global CodeSpecifier spec_ptr;
	global CodeSpecifier spec_ref;
	global CodeSpecifier spec_register;
	global CodeSpecifier spec_rvalue;
	global CodeSpecifier spec_static_member;
	global CodeSpecifier spec_thread_local;
	global CodeSpecifier spec_volatile;
#pragma endregion Constants

#pragma region AST Body Case Macros
#	define AST_BODY_CLASS_UNALLOWED_TYPES \
	case PlatformAttributes:                      \
	case Class_Body:                      \
	case Enum_Body:                       \
	case Extern_Linkage:                  \
	case Function_Body:                   \
	case Function_Fwd:                    \
	case Global_Body:                     \
	case Namespace:                       \
	case Namespace_Body:                  \
	case Operator:                        \
	case Operator_Fwd:                    \
	case Parameters:                      \
	case Specifiers:                      \
	case Struct_Body:                     \
	case Typename:

#	define AST_BODY_FUNCTION_UNALLOWED_TYPES \
	case Access_Public:                      \
	case Access_Protected:                   \
	case Access_Private:                     \
	case PlatformAttributes:                         \
	case Class_Body:                         \
	case Enum_Body:                          \
	case Extern_Linkage:                     \
	case Friend:                             \
	case Function_Body:                      \
	case Function_Fwd:                       \
	case Global_Body:                        \
	case Namespace:                          \
	case Namespace_Body:                     \
	case Operator:                           \
	case Operator_Fwd:                       \
	case Operator_Member:                    \
	case Operator_Member_Fwd:                \
	case Parameters:                         \
	case Specifiers:                         \
	case Struct_Body:                        \
	case Typename:

#	define AST_BODY_GLOBAL_UNALLOWED_TYPES \
	case Access_Public: 				   \
	case Access_Protected: 				   \
	case Access_Private: 				   \
	case PlatformAttributes:                       \
	case Class_Body: 					   \
	case Enum_Body: 					   \
	case Execution: 					   \
	case Friend: 						   \
	case Function_Body: 				   \
	case Global_Body: 					   \
	case Namespace_Body: 				   \
	case Operator_Member: 				   \
	case Operator_Member_Fwd: 			   \
	case Parameters: 					   \
	case Specifiers: 					   \
	case Struct_Body: 					   \
	case Typename:

#	define AST_BODY_EXPORT_UNALLOWED_TYPES         AST_BODY_GLOBAL_UNALLOWED_TYPES
#	define AST_BODY_NAMESPACE_UNALLOWED_TYPES      AST_BODY_GLOBAL_UNALLOWED_TYPES
#	define AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES AST_BODY_GLOBAL_UNALLOWED_TYPES

#	define AST_BODY_STRUCT_UNALLOWED_TYPES         AST_BODY_CLASS_UNALLOWED_TYPES
#pragma endregion AST Body Case Macros

#pragma region AST
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
					result.append_fmt( "%s", Params->to_string() );

				else
					result.append( "void" );

				result.append_fmt( ")\n{\n%s\n}"
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
					result.append_fmt( "%s", Params->to_string() );

				else
					result.append( "void" );

				result.append( ");" );
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
					result.append_fmt( "%s", Params->to_string() );

				else
					result.append( "void" );

				result.append_fmt( ")\n{\n%s\n}"
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
					result.append_fmt( "%s);", Params->to_string() );

				else
					result.append_fmt( "void);" );
			}
			break;

			case Operator_Cast:
			{
				result.append_fmt("operator %s(){\n%s\n}", ValueType->to_string(), Body->to_string() );
			}
			break;

			case Operator_Cast_Fwd:
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

				if ( Attributes )
				{
					result.append_fmt( "%s ", Attributes->to_string() );
				}

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
#pragma endregion AST

#pragma region Gen Interface
	internal void* Global_Allocator_Proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
	{
		Arena& last = Global_AllocatorBuckets.back();

		switch ( type )
		{
			case EAllocation_ALLOC:
			{
				if ( last.TotalUsed + size > last.TotalSize )
				{
					Arena bucket = Arena::init_from_allocator( heap(), Global_BucketSize );

					if ( bucket.PhysicalStart == nullptr )
						fatal( "Failed to create bucket for Global_AllocatorBuckets");

					if ( ! Global_AllocatorBuckets.append( bucket ) )
						fatal( "Failed to append bucket to Global_AllocatorBuckets");

					last = Global_AllocatorBuckets.back();
				}

				return alloc_align( last, size, alignment );
			}
			case EAllocation_FREE:
			{
				// Doesn't recycle.
			}
			break;
			case EAllocation_FREE_ALL:
			{
				// Memory::cleanup instead.
			}
			break;
			case EAllocation_RESIZE:
			{
				if ( last.TotalUsed + size > last.TotalSize )
				{
					Arena bucket = Arena::init_from_allocator( heap(), Global_BucketSize );

					if ( bucket.PhysicalStart == nullptr )
						fatal( "Failed to create bucket for Global_AllocatorBuckets");

					if ( ! Global_AllocatorBuckets.append( bucket ) )
						fatal( "Failed to append bucket to Global_AllocatorBuckets");

					last = Global_AllocatorBuckets.back();
				}

				void* result = alloc_align( last.Backing, size, alignment );

				if ( result != nullptr && old_memory != nullptr )
				{
					mem_copy( result, old_memory, old_size );
				}

				return result;
			}
		}

		return nullptr;
	}

	internal void define_constants()
	{
		Code::Global          = make_code();
		Code::Global->Name    = get_cached_string( txt_StrC("Global Code") );
		Code::Global->Content = Code::Global->Name;

		Code::Invalid = make_code();
		Code::Invalid.set_global();

	#	define def_constant_code_type( Type_ )    \
		t_##Type_ = def_type( name(Type_) ); \
		t_##Type_.set_global();

		def_constant_code_type( auto );
		def_constant_code_type( void );
		def_constant_code_type( int );
		def_constant_code_type( bool );
		def_constant_code_type( char );
		def_constant_code_type( wchar_t );
		def_constant_code_type( class );
		def_constant_code_type( typename );

	#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
		t_b32 = def_type( name(b32) );

		def_constant_code_type( s8 );
		def_constant_code_type( s16 );
		def_constant_code_type( s32 );
		def_constant_code_type( s64 );

		def_constant_code_type( u8 );
		def_constant_code_type( u16 );
		def_constant_code_type( u32 );
		def_constant_code_type( u64 );

		def_constant_code_type( sw );
		def_constant_code_type( uw );

		def_constant_code_type( f32 );
		def_constant_code_type( f64 );
	#endif
	#	undef def_constant_code_type

		access_private       = make_code();
		access_private->Type = ECode::Access_Private;
		access_private->Name = get_cached_string( txt_StrC("private:") );
		access_private.set_global();

		access_protected = make_code();
		access_protected->Type = ECode::Access_Protected;
		access_protected->Name = get_cached_string( txt_StrC("protected:") );
		access_protected.set_global();

		access_public = make_code();
		access_public->Type = ECode::Access_Public;
		access_public->Name = get_cached_string( txt_StrC("public:") );
		access_public.set_global();

		module_global_fragment          = make_code();
		module_global_fragment->Type    = ECode::Untyped;
		module_global_fragment->Name    = get_cached_string( txt_StrC("module;") );
		module_global_fragment->Content = module_global_fragment->Name;
		module_global_fragment.set_global();

		module_private_fragment          = make_code();
		module_private_fragment->Type    = ECode::Untyped;
		module_private_fragment->Name    = get_cached_string( txt_StrC("module : private;") );
		module_private_fragment->Content = module_private_fragment->Name;
		module_private_fragment.set_global();

		pragma_once          = make_code();
		pragma_once->Type    = ECode::Untyped;
		pragma_once->Name    = get_cached_string( txt_StrC("#pragma once") );
		pragma_once->Content = pragma_once->Name;
		pragma_once.set_global();

	#	pragma push_macro( "global" )
	#	pragma push_macro( "internal" )
	#	pragma push_macro( "local_persist" )
	#	define global        global
	#	define internal      internal
	#	define local_persist local_persist

	#	define def_constant_spec( Type_, ... )                                    \
		spec_##Type_ = def_specifiers( num_args(__VA_ARGS__), __VA_ARGS__); \
		spec_##Type_.set_global();

		def_constant_spec( const,            ESpecifier::Const );
		def_constant_spec( consteval,        ESpecifier::Consteval );
		def_constant_spec( constexpr,        ESpecifier::Constexpr );
		def_constant_spec( constinit,        ESpecifier::Constinit );
		def_constant_spec( extern_linkage,   ESpecifier::External_Linkage );
		def_constant_spec( global,           ESpecifier::Global );
		def_constant_spec( inline,           ESpecifier::Inline );
		def_constant_spec( internal_linkage, ESpecifier::Internal_Linkage );
		def_constant_spec( local_persist,    ESpecifier::Local_Persist );
		def_constant_spec( mutable,          ESpecifier::Mutable );
		def_constant_spec( ptr,              ESpecifier::Ptr );
		def_constant_spec( ref,              ESpecifier::Ref );
		def_constant_spec( register,         ESpecifier::Register );
		def_constant_spec( rvalue,           ESpecifier::RValue );
		def_constant_spec( static_member,    ESpecifier::Static );
		def_constant_spec( thread_local,     ESpecifier::Thread_Local );
		def_constant_spec( volatile, 	     ESpecifier::Volatile)

		spec_local_persist = def_specifiers( 1, ESpecifier::Local_Persist );
		spec_local_persist.set_global();

	#	pragma pop_macro( "global" )
	#	pragma pop_macro( "internal" )
	#	pragma pop_macro( "local_persist" )

	#	undef def_constant_spec
	}

	void init()
	{
		// Setup global allocator
		{
			GlobalAllocator = AllocatorInfo { & Global_Allocator_Proc, nullptr };

			Global_AllocatorBuckets = Array<Arena>::init_reserve( heap(), 128 );

			if ( Global_AllocatorBuckets == nullptr )
				fatal( "Failed to reserve memory for Global_AllocatorBuckets");

			Arena bucket = Arena::init_from_allocator( heap(), Global_BucketSize );

			if ( bucket.PhysicalStart == nullptr )
				fatal( "Failed to create first bucket for Global_AllocatorBuckets");

			Global_AllocatorBuckets.append( bucket );

		}

		// Setup the arrays
		{
			CodePools = Array<Pool>::init_reserve( Allocator_DataArrays, InitSize_DataArrays );

			if ( CodePools == nullptr )
				fatal( "gen::init: Failed to initialize the CodePools array" );

			StringArenas = Array<Arena>::init_reserve( Allocator_DataArrays, InitSize_DataArrays );

			if ( StringArenas == nullptr )
				fatal( "gen::init: Failed to initialize the StringArenas array" );
		}

		// Setup the code pool and code entries arena.
		{
			Pool code_pool = Pool::init( Allocator_CodePool, CodePool_NumBlocks, sizeof(AST) );

			if ( code_pool.PhysicalStart == nullptr )
				fatal( "gen::init: Failed to initialize the code pool" );

			CodePools.append( code_pool );

		#ifdef GEN_FEATURE_PARSING
			LexArena = Arena::init_from_allocator( Allocator_Lexer, LexAllocator_Size );
		#endif

			Arena string_arena = Arena::init_from_allocator( Allocator_StringArena, SizePer_StringArena );

			if ( string_arena.PhysicalStart == nullptr )
				fatal( "gen::init: Failed to initialize the string arena" );

			StringArenas.append( string_arena );
		}

		// Setup the hash tables
		{
			StringCache = StringTable::init( Allocator_StringTable );

			if ( StringCache.Entries == nullptr )
				fatal( "gen::init: Failed to initialize the StringCache");
		}

		define_constants();
	}

	void deinit()
	{
		s32 index = 0;
		s32 left  = CodePools.num();
		do
		{
			Pool* code_pool = & CodePools[index];
			code_pool->free();
			index++;
		}
		while ( left--, left );

		index = 0;
		left  = StringArenas.num();
		do
		{
			Arena* string_arena = & StringArenas[index];
			string_arena->free();
			index++;
		}
		while ( left--, left );

		StringCache.destroy();

		CodePools.free();
		StringArenas.free();

	#ifdef GEN_FEATURE_PARSING
		LexArena.free();
	#endif

		index = 0;
		left  = Global_AllocatorBuckets.num();
		do
		{
			Arena* bucket = & Global_AllocatorBuckets[ index ];
			bucket->free();
			index++;
		}
		while ( left--, left );

		Global_AllocatorBuckets.free();
	}

	void reset()
	{
		s32 index = 0;
		s32 left  = CodePools.num();
		do
		{
			Pool* code_pool = & CodePools[index];
			code_pool->clear();
			index++;
		}
		while ( left--, left );

		index = 0;
		left  = StringArenas.num();
		do
		{
			Arena* string_arena = & StringArenas[index];
			string_arena->TotalUsed = 0;;
			index++;
		}
		while ( left--, left );

		StringCache.clear();

		define_constants();
	}

	AllocatorInfo get_string_allocator( s32 str_length )
	{
		Arena* last = & StringArenas.back();

		uw size_req = str_length + sizeof(String::Header) + sizeof(char*);

		if ( last->TotalUsed + size_req > last->TotalSize )
		{
			Arena new_arena = Arena::init_from_allocator( Allocator_StringArena, SizePer_StringArena );

			if ( ! StringArenas.append( new_arena ) )
				fatal( "gen::get_string_allocator: Failed to allocate a new string arena" );

			last = & StringArenas.back();
		}

		return * last;
	}

	// Will either make or retrive a code string.
	StringCached get_cached_string( StrC str )
	{
		s32 hash_length = str.Len > kilobytes(1) ? kilobytes(1) : str.Len;
		u64 key         = crc32( str.Ptr, hash_length );
		{
			StringCached* result = StringCache.get( key );

			if ( result )
				return * result;
		}

		String result = String::make( get_string_allocator( str.Len ), str );

		StringCache.set( key, result );

		return result;
	}

	/*
		Used internally to retireve a Code object form the CodePool.
	*/
	Code make_code()
	{
		Pool* allocator = & CodePools.back();
		if ( allocator->FreeList == nullptr )
		{
			Pool code_pool = Pool::init( Allocator_CodePool, CodePool_NumBlocks, sizeof(AST) );

			if ( code_pool.PhysicalStart == nullptr )
				fatal( "gen::make_code: Failed to allocate a new code pool - CodePool allcoator returned nullptr." );

			if ( ! CodePools.append( code_pool ) )
				fatal( "gen::make_code: Failed to allocate a new code pool - CodePools failed to append new pool." );

			allocator = & CodePools.back();
		}

		Code result { rcast( AST*, alloc( * allocator, sizeof(AST) )) };

		result->Content         = { nullptr };
		result->Prev            = { nullptr };
		result->Next			= { nullptr };
		result->Parent          = { nullptr };
		result->Name            = { nullptr };
		result->Type            = ECode::Invalid;
		result->ModuleFlags     = ModuleFlag::Invalid;
		result->NumEntries      = 0;

		return result;
	}

	enum class OpValidateResult : u32
	{
		Fail,
		Global,
		Member
	};

	inline
	OpValidateResult operator__validate( OperatorT op, CodeParam params_code, CodeType ret_type, CodeSpecifier specifier )
	{
		using namespace EOperator;

		if ( op == EOperator::Invalid )
		{
			log_failure("gen::def_operator: op cannot be invalid");
			return OpValidateResult::Fail;
		}

	#pragma region Helper Macros
	#	define check_params()                                                                                 \
		if ( ! params_code )                                                                                  \
		{                                                                                                     \
			log_failure("gen::def_operator: params is null and operator%s requires it", to_str(op));          \
			return OpValidateResult::Fail;                                                                    \
		}                                                                                                     \
		if ( params_code->Type != ECode::Parameters )                                                         \
		{                                                                                                     \
			log_failure("gen::def_operator: params is not of Parameters type - %s", params_code.debug_str()); \
			return OpValidateResult::Fail;                                                                    \
		}

	#	define check_param_eq_ret()                                                                    \
		if ( ! is_member_symbol && params_code->ValueType != ret_type )                                \
		{                                                                                              \
			log_failure("gen_def_operator: operator%s requires first parameter to equal return type\n" \
				"param types: %s\n"                                                                    \
				"return type: %s",                                                                     \
				to_str(op),                                                                            \
				params_code.debug_str(),                                                               \
				ret_type.debug_str()                                                                   \
			);                                                                                         \
			return OpValidateResult::Fail;                                                             \
		}
	#pragma endregion Helper Macros

		if ( ! ret_type )
		{
			log_failure("gen::def_operator: ret_type is null but is required by operator%s", to_str(op));
		}

		if ( ret_type->Type != ECode::Typename )
		{
			log_failure("gen::def_operator: ret_type is not of typename type - %s", ret_type.debug_str());
			return OpValidateResult::Fail;
		}

		bool is_member_symbol = false;

		switch ( op )
		{
		#	define specs( ... ) num_args( __VA_ARGS__ ), __VA_ARGS__
			case Assign:
				check_params();

				if ( params_code->NumEntries > 1 )
				{
					log_failure("gen::def_operator: "
						"operator%s does not support non-member definition (more than one parameter provided) - %s",
						to_str(op),
						params_code.debug_str()
					);
					return OpValidateResult::Fail;
				}

				is_member_symbol = true;
			break;

			case Assign_Add:
			case Assign_Subtract:
			case Assign_Multiply:
			case Assign_Divide:
			case Assign_Modulo:
			case Assign_BAnd:
			case Assign_BOr:
			case Assign_BXOr:
			case Assign_LShift:
			case Assign_RShift:
				check_params();

				if ( params_code->NumEntries == 1 )
					is_member_symbol = true;

				else
					check_param_eq_ret();

				if (params_code->NumEntries > 2 )
				{
					log_failure("gen::def_operator: operator%s may not be defined with more than two parametes - param count; %d\n%s"
						, to_str(op)
						, params_code->NumEntries
						, params_code.debug_str()
					);
					return OpValidateResult::Fail;
				}
			break;

			case Increment:
			case Decrement:
				// If its not set, it just means its a prefix member op.
				if ( params_code )
				{
					if ( params_code->Type != ECode::Parameters )
					{
						log_failure("gen::def_operator: operator%s params code provided is not of Parameters type - %s"
							, to_str(op)
							, params_code.debug_str()
						);
						return OpValidateResult::Fail;
					}

					switch ( params_code->NumEntries )
					{
						case 1:
							if ( params_code->ValueType.is_equal( t_int ) )
								is_member_symbol = true;

							else
								check_param_eq_ret();
						break;

						case 2:
							check_param_eq_ret();

							if ( ! params_code.get(1).is_equal( t_int ) )
							{
								log_failure("gen::def_operator: "
									"operator%s requires second parameter of non-member definition to be int for post-decrement",
									to_str(op)
								);
								return OpValidateResult::Fail;
							}
						break;

						default:
							log_failure("gen::def_operator: operator%s recieved unexpected number of parameters recived %d instead of 0-2"
								, to_str(op)
								, params_code->NumEntries
							);
							return OpValidateResult::Fail;
					}
				}
			break;

			case Unary_Plus:
			case Unary_Minus:
			case BNot:
				if ( ! params_code )
					is_member_symbol = true;

				else
				{
					if ( params_code->Type != ECode::Parameters )
					{
						log_failure("gen::def_operator: params is not of Parameters type - %s", params_code.debug_str());
						return OpValidateResult::Fail;
					}

					if ( params_code->ValueType.is_equal( ret_type ) )
					{
						log_failure("gen::def_operator: "
							"operator%s is non-member symbol yet first paramter does not equal return type\n"
							"param type: %s\n"
							"return type: %s\n"
							, params_code.debug_str()
							, ret_type.debug_str()
						);
						return OpValidateResult::Fail;
					}

					if ( params_code->NumEntries > 1 )
					{
						log_failure("gen::def_operator: operator%s may not have more than one parameter - param count: %d"
							, to_str(op)
							, params_code->NumEntries
						);
						return OpValidateResult::Fail;
					}
				}
			break;

			case Add:
			case Subtract:
			case Multiply:
			case Divide:
			case Modulo:
			case BAnd:
			case BOr:
			case BXOr:
			case LShift:
			case RShift:
				check_params();

				switch ( params_code->NumEntries )
				{
					case 1:
						is_member_symbol = true;
					break;

					case 2:
						if ( ! params_code->ValueType.is_equal( ret_type ) )
						{
							log_failure("gen::def_operator: "
								"operator%s is non-member symbol yet first paramter does not equal return type\n"
								"param type: %s\n"
								"return type: %s\n"
								, params_code.debug_str()
								, ret_type.debug_str()
							);
							return OpValidateResult::Fail;
						}
					break;

					default:
						log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-2"
							, to_str(op)
							, params_code->NumEntries
						);
						return OpValidateResult::Fail;
				}
			break;

			case UnaryNot:
				if ( ! params_code )
					is_member_symbol = true;

				else
				{
					if ( params_code->Type != ECode::Parameters )
					{
						log_failure("gen::def_operator: params is not of Parameters type - %s", params_code.debug_str());
						return OpValidateResult::Fail;
					}

					if ( params_code->NumEntries != 1 )
					{
						log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-1"
							, to_str(op)
							, params_code->NumEntries
						);
						return OpValidateResult::Fail;
					}
				}

				if ( ! ret_type.is_equal( t_bool ))
				{
					log_failure("gen::def_operator: operator%s return type must be of type bool - %s"
						, to_str(op)
						, ret_type.debug_str()
					);
					return OpValidateResult::Fail;
				}
			break;

			case LAnd:
			case LOr:
			case LEqual:
			case LNot:
			case Lesser:
			case Greater:
			case LesserEqual:
			case GreaterEqual:
				check_params();

				switch ( params_code->NumEntries )
				{
					case 1:
						is_member_symbol = true;
					break;

					case 2:
					break;

					default:
						log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 1-2"
							, to_str(op)
							, params_code->NumEntries
						);
						return OpValidateResult::Fail;
				}
			break;

			case Indirection:
			case AddressOf:
			case MemberOfPointer:
				if ( params_code && params_code->NumEntries > 1)
				{
					log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-1"
						, to_str(op)
						, params_code->NumEntries
					);
					return OpValidateResult::Fail;
				}
				else
				{
					is_member_symbol = true;
				}
			break;

			case PtrToMemOfPtr:
				if ( params_code )
				{
					log_failure("gen::def_operator: operator%s expects no paramters - %s", to_str(op), params_code.debug_str());
					return OpValidateResult::Fail;
				}
			break;

			case Subscript:
			case FunctionCall:
			case Comma:
				check_params();
			break;
		#	undef specs
		}

		return is_member_symbol ? OpValidateResult::Member : OpValidateResult::Global;
	#	undef check_params
	#	undef check_ret_type
	#	undef check_param_eq_ret
	}

	void set_allocator_data_arrays( AllocatorInfo allocator )
	{
		Allocator_DataArrays = allocator;
	}

	void set_allocator_code_pool( AllocatorInfo allocator )
	{
		Allocator_CodePool = allocator;
	}

	void set_allocator_lexer( AllocatorInfo allocator )
	{
		Allocator_Lexer = allocator;
	}

	void set_allocator_string_arena( AllocatorInfo allocator )
	{
		Allocator_StringArena = allocator;
	}

	void set_allocator_string_table( AllocatorInfo allocator )
	{
		Allocator_StringArena = allocator;
	}

#pragma region Helper Marcojs
	// This snippet is used in nearly all the functions.
#	define name_check( Context_, Name_ )                                                                  \
	{                                                                                                     \
		if ( Name_.Len <= 0 )                                                                             \
		{                                                                                                 \
			log_failure( "gen::" stringize(Context_) ": Invalid name length provided - %d",  Name_.Len ); \
			return CodeInvalid;                                                                           \
		}                                                                                                 \
                                                                                                          \
		if ( Name_.Ptr == nullptr )                                                                       \
		{                                                                                                 \
			log_failure( "gen::" stringize(Context_) ": name is null" );                                  \
			return CodeInvalid;                                                                           \
		}                                                                                                 \
	}

#	define null_check( Context_, Code_ )                                                          \
		if ( ! Code_ )                                                                            \
		{                                                                                         \
			log_failure( "gen::" stringize(Context_) ": " stringize(Code_) " provided is null" ); \
			return CodeInvalid;                                                                   \
		}

#	define null_or_invalid_check( Context_, Code_ )                                                 \
	{                                                                                               \
		if ( ! Code_ )                                                                              \
		{                                                                                           \
			log_failure( "gen::" stringize(Context_) ": " stringize(Code_) " provided is null" );   \
			return CodeInvalid;                                                                     \
		}                                                                                           \
                                                                                                    \
		if ( Code_->is_invalid() )                                                                  \
		{                                                                                           \
			log_failure("gen::" stringize(Context_) ": " stringize(Code_) " provided is invalid" ); \
			return CodeInvalid;                                                                     \
		}                                                                                           \
	}

#	define not_implemented( Context_ )                              \
		log_failure( "gen::%s: This function is not implemented" ); \
		return CodeInvalid;
#pragma endregion Helper Marcos

#pragma region Upfront Constructors
/*
	The implementaiton of the upfront constructors involves bascially doing three things:
	* Validate the arguments given to construct the intended type of AST is valid.
	* Construct said AST type.
	* Lock the AST (set to readonly) and return the valid object.

	If any of the validation fails, it triggers a call to log_failure with as much info the give the user so that they can hopefully
	identify the issue without having to debug too much (at least they can debug though...)

	The largest of the functions is related to operator overload definitions.
	The library validates a good protion of their form and thus the argument processing for is quite a bit.
*/
	CodeAttributes def_attributes( StrC content )
	{
		if ( content.Len <= 0 || content.Ptr == nullptr )
		{
			log_failure( "gen::def_attributes: Invalid attributes provided" );
			return CodeInvalid;
		}

		Code
		result          = make_code();
		result->Type    = ECode::PlatformAttributes;
		result->Name    = get_cached_string( content );
		result->Content = result->Name;

		return (CodeAttributes) result;
	}

	CodeComment def_comment( StrC content )
	{
		if ( content.Len <= 0 || content.Ptr == nullptr )
		{
			log_failure( "gen::def_comment: Invalid comment provided:" );
			return CodeInvalid;
		}

		Code
		result          = make_code();
		result->Type    = ECode::Comment;
		result->Name    = get_cached_string( content );
		result->Content = result->Name;

		return (CodeComment) result;
	}

	CodeClass def_class( StrC name
		, Code           body
		, CodeType       parent, AccessSpec parent_access
		, CodeAttributes attributes
		, ModuleFlag     mflags )
	{
		using namespace ECode;

		name_check( def_class, name );

		if ( attributes && attributes->Type != PlatformAttributes )
		{
			log_failure( "gen::def_class: attributes was not a 'PlatformAttributes' type: %s", attributes.debug_str() );
			return CodeInvalid;
		}

		if ( parent && (parent->Type != Class || parent->Type != Struct || parent->Type != Typename || parent->Type != Untyped) )
		{
			log_failure( "gen::def_class: parent provided is not type 'Class', 'Struct', 'Typeanme', or 'Untyped': %s", parent.debug_str() );
			return CodeInvalid;
		}

		CodeClass
		result              = (CodeClass) make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		if ( body )
		{
			switch ( body->Type )
			{
				case Class_Body:
				case Untyped:
				break;

				default:
					log_failure("gen::def_class: body must be either of Class_Body or Untyped type - %s", body.debug_str());
					return CodeInvalid;
			}

			result->Type = Class;
			result->Body = body;
		}
		else
		{
			result->Type = Class_Fwd;
		}

		if ( attributes )
			result->Attributes = attributes;

		if ( parent )
		{
			result->ParentAccess = parent_access;
			result->ParentType   = parent;
		}

		return result;
	}

	CodeEnum def_enum( StrC name
		, Code       body,      CodeType       type
		, EnumT      specifier, CodeAttributes attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_enum, name );

		if ( type && type->Type != Typename )
		{
			log_failure( "gen::def_enum: enum underlying type provided was not of type Typename: %s", type.debug_str() );
			return CodeInvalid;
		}

		if ( attributes && attributes->Type != PlatformAttributes )
		{
			log_failure( "gen::def_enum: attributes was not a 'PlatformAttributes' type: %s", attributes.debug_str() );
			return CodeInvalid;
		}

		CodeEnum
		result              = (CodeEnum) make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		if ( body )
		{
			switch ( body->Type )
			{
				case Enum_Body:
				case Untyped:
				break;

				default:
					log_failure( "gen::def_enum: body must be of Enum_Body or Untyped type %s", body.debug_str());
					return CodeInvalid;
			}

			result->Type = specifier == EnumClass ?
				Enum_Class : Enum;

			result->Body = body;
		}
		else
		{
			result->Type = specifier == EnumClass ?
				Enum_Class_Fwd : Enum_Fwd;
		}

		if ( attributes )
			result->Attributes = attributes;

		if ( type )
		{
			result->UnderlyingType = type;
		}
		else if ( result->Type != Enum_Class_Fwd && result->Type != Enum_Fwd )
		{
			log_failure( "gen::def_enum: enum forward declaration must have an underlying type" );
			return CodeInvalid;
		}

		return result;
	}

	CodeExec def_execution( StrC content )
	{
		if ( content.Len <= 0 || content.Ptr == nullptr )
		{
			log_failure( "gen::def_execution: Invalid execution provided" );
			return CodeInvalid;
		}

		Code
		result          = make_code();
		result->Type    = ECode::Execution;
		result->Name    = get_cached_string( content );
		result->Content = result->Name;

		return (CodeExec) result;
	}

	CodeExtern def_extern_link( StrC name, Code body )
	{
		using namespace ECode;

		name_check( def_extern_linkage, name );
		null_check( def_extern_linkage, body );

		if ( body->Type != Extern_Linkage_Body && body->Type != Untyped )
		{
			log_failure("gen::def_extern_linkage: body is not of extern_linkage or untyped type %s", body->debug_str());
			return CodeInvalid;
		}

		CodeExtern
		result        = (CodeExtern)make_code();
		result->Type  = Extern_Linkage;
		result->Name  = get_cached_string( name );
		result->Body  = body;

		return (CodeExtern) result;
	}

	CodeFriend def_friend( Code declaration )
	{
		using namespace ECode;

		null_check( def_friend, declaration );

		switch ( declaration->Type )
		{
			case Class_Fwd:
			case Function_Fwd:
			case Operator_Fwd:
			case Struct_Fwd:
			case Class:
			case Function:
			case Operator:
			case Struct:
			break;

			default:
				log_failure("gen::def_friend: requires declartion to have class, function, operator, or struct - %s", declaration->debug_str());
				return CodeInvalid;
		}

		CodeFriend
		result       = (CodeFriend) make_code();
		result->Type = Friend;

		result->Declaration = declaration;

		return result;
	}

	CodeFn def_function( StrC name
		, CodeParam      params ,   CodeType       ret_type, Code body
		, CodeSpecifier specifiers, CodeAttributes attributes
		, ModuleFlag     mflags )
	{
		using namespace ECode;

		name_check( def_function, name );

		if ( params && params->Type != Parameters )
		{
			log_failure( "gen::def_function: params was not a `Parameters` type: %s", params.debug_str() );
			return CodeInvalid;
		}

		if ( ret_type && ret_type->Type != Typename )
		{
			log_failure( "gen::def_function: ret_type was not a Typename: %s", ret_type.debug_str() );
			return CodeInvalid;
		}

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_function: specifiers was not a `Specifiers` type: %s", specifiers.debug_str() );
			return CodeInvalid;
		}

		if ( attributes && attributes->Type != PlatformAttributes )
		{
			log_failure( "gen::def_function: attributes was not a `PlatformAttributes` type: %s", attributes.debug_str() );
			return CodeInvalid;
		}

		CodeFn
		result              = (CodeFn) make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		if ( body )
		{
			switch ( body->Type )
			{
				case Function_Body:
				case Execution:
				case Untyped:
					break;

				default:
				{
					log_failure("gen::def_function: body must be either of Function_Body, Execution, or Untyped type. %s", body->debug_str());
					return CodeInvalid;
				}
			}

			result->Type = Function;
			result->Body = body;
		}
		else
		{
			result->Type = Function_Fwd;
		}

		if ( attributes )
			result->Attributes = attributes;

		if ( specifiers )
			result->Specs = specifiers;

		if ( ret_type )
		{
			result->ReturnType = ret_type;
		}
		else
		{
			result->ReturnType = t_void;
		}

		if ( params )
			result->Params = params;

		return result;
	}

	CodeInclude def_include ( StrC path )
	{
		if ( path.Len <= 0 || path.Ptr == nullptr )
		{
			log_failure( "gen::def_include: Invalid path provided - %d" );
			return CodeInvalid;
		}

		Code
		result          = make_code();
		result->Type    = ECode::Preprocessor_Include;
		result->Name    = get_cached_string( path );
		result->Content = result->Name;

		return (CodeInclude) result;
	}

	CodeModule def_module( StrC name, ModuleFlag mflags )
	{
		name_check( def_module, name );

		Code
		result              = make_code();
		result->Type        = ECode::Module;
		result->Name        = get_cached_string( name );
		result->Content     = result->Name;
		result->ModuleFlags = mflags;

		return (CodeModule) result;
	}

	CodeNamespace def_namespace( StrC name, Code body, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_namespace, name );
		null_check( def_namespace, body );

		if ( body->Type != Namespace_Body && body->Type != Untyped )
		{
			log_failure("gen::def_namespace: body is not of namespace or untyped type %s", body.debug_str());
			return CodeInvalid;
		}

		CodeNamespace
		result              = (CodeNamespace) make_code();
		result->Type        = Namespace;
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;
		result->Body        = body;

		return result;
	}

	CodeOperator def_operator( OperatorT op
		, CodeParam      params_code, CodeType       ret_type, Code body
		, CodeSpecifier specifiers,   CodeAttributes attributes
		, ModuleFlag     mflags )
	{
		using namespace ECode;

		if ( attributes && attributes->Type != PlatformAttributes )
		{
			log_failure( "gen::def_operator: PlatformAttributes was provided but its not of attributes type: %s", attributes.debug_str() );
			return CodeInvalid;
		}

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_operator: Specifiers was provided but its not of specifiers type: %s", specifiers.debug_str() );
			return CodeInvalid;
		}

		OpValidateResult check_result = operator__validate( op, params_code, ret_type, specifiers );

		if ( check_result == OpValidateResult::Fail )
		{
			return CodeInvalid;
		}

		char const* name = str_fmt_buf( "operator %s", to_str(op) );

		CodeOperator
		result              = (CodeOperator) make_code();
		result->Name        = get_cached_string( { str_len(name), name } );
		result->ModuleFlags = mflags;

		if ( body )
		{
			switch ( body->Type )
			{
				case Function_Body:
				case Execution:
				case Untyped:
					break;

				default:
				{
					log_failure("gen::def_operator: body must be either of Function_Body, Execution, or Untyped type. %s", body->debug_str());
					return CodeInvalid;
				}
			}

			result->Type = check_result == OpValidateResult::Global ?
				Operator : Operator_Member;

			result->Body = body;
		}
		else
		{
			result->Type = check_result == OpValidateResult::Global ?
				Operator_Fwd : Operator_Member_Fwd;
		}

		if ( attributes )
			result->Attributes = attributes;

		if ( specifiers )
			result->Specs = specifiers;

		result->ReturnType = ret_type;

		if (params_code)
			result->Params = params_code;

		return result;
	}

	CodeOpCast def_operator_cast( CodeType type, Code body )
	{
		using namespace ECode;
		null_check( def_operator_cast, type );

		if ( type->Type != Typename )
		{
			log_failure( "gen::def_operator_cast: type is not a typename - %s", type.debug_str() );
			return CodeInvalid;
		}

		CodeOpCast result = (CodeOpCast) make_code();

		if (body)
		{
			result->Type = Operator_Cast;

			if ( body->Type != Function_Body && body->Type != Execution )
			{
				log_failure( "gen::def_operator_cast: body is not of function body or execution type - %s", body.debug_str() );
				return CodeInvalid;
			}

			result->Body = body;
		}
		else
		{
			result->Type = Operator_Cast_Fwd;
		}

		result->ValueType = type;
		return result;
	}

	CodeParam def_param( CodeType type, StrC name, Code value )
	{
		using namespace ECode;

		name_check( def_param, name );
		null_check( def_param, type );

		if ( type->Type != Typename )
		{
			log_failure( "gen::def_param: type is not a typename - %s", type.debug_str() );
			return CodeInvalid;
		}

		if ( value && value->Type != Untyped )
		{
			log_failure( "gen::def_param: value is not untyped - %s", value.debug_str() );
			return CodeInvalid;
		}

		CodeParam
		result       = (CodeParam) make_code();
		result->Type = Parameters;
		result->Name = get_cached_string( name );

		result->ValueType = type;

		if ( value )
			result->Value = value;

		result->NumEntries++;

		return result;
	}

	CodeSpecifier def_specifier( SpecifierT spec )
	{
		CodeSpecifier
		result       = (CodeSpecifier) make_code();
		result->Type = ECode::Specifiers;
		result.append( spec );

		return result;
	}

	CodeStruct def_struct( StrC name
		, Code           body
		, CodeType       parent, AccessSpec parent_access
		, CodeAttributes attributes
		, ModuleFlag     mflags )
	{
		using namespace ECode;

		if ( attributes && attributes->Type != PlatformAttributes )
		{
			log_failure( "gen::def_struct: attributes was not a `PlatformAttributes` type - %s", attributes.debug_str() );
			return CodeInvalid;
		}

		if ( parent && parent->Type != Typename )
		{
			log_failure( "gen::def_struct: parent was not a `Struct` type - %s", parent.debug_str() );
			return CodeInvalid;
		}

		if ( body && body->Type != Struct_Body )
		{
			log_failure( "gen::def_struct: body was not a Struct_Body type - %s", body.debug_str() );
			return CodeInvalid;
		}

		CodeStruct
		result              = (CodeStruct) make_code();
		result->ModuleFlags = mflags;

		if ( name )
			result->Name = get_cached_string( name );

		if ( body )
		{
			result->Type = Struct;
			result->Body = body;
		}
		else
		{
			result->Type = Struct_Fwd;
		}

		if ( attributes )
			result->Attributes = attributes;

		if ( parent )
		{
			result->ParentAccess = parent_access;
			result->ParentType   = parent;
		}

		return result;
	}

	CodeTemplate def_template( CodeParam params, Code declaration, ModuleFlag mflags )
	{
		null_check( def_template, declaration );

		if ( params && params->Type != ECode::Parameters )
		{
			log_failure( "gen::def_template: params is not of parameters type - %s", params.debug_str() );
			return CodeInvalid;
		}

		switch (declaration->Type )
		{
			case ECode::Class:
			case ECode::Function:
			case ECode::Struct:
			case ECode::Variable:
			case ECode::Using:
			break;

			default:
				log_failure( "gen::def_template: declaration is not of class, function, struct, variable, or using type - %s", declaration.debug_str() );
		}

		CodeTemplate
		result              = (CodeTemplate) make_code();
		result->Type        = ECode::Template;
		result->ModuleFlags = mflags;
		result->Params      = params;
		result->Declaration = declaration;

		return result;
	}

	CodeType def_type( StrC name, Code arrayexpr, CodeSpecifier specifiers, CodeAttributes attributes )
	{
		name_check( def_type, name );

		if ( attributes && attributes->Type != ECode::PlatformAttributes )
		{
			log_failure( "gen::def_type: attributes is not of attributes type - %s", attributes.debug_str() );
			return CodeInvalid;
		}

		if ( specifiers && specifiers->Type != ECode::Specifiers )
		{
			log_failure( "gen::def_type: specifiers is not of specifiers type - %s", specifiers.debug_str() );
			return CodeInvalid;
		}

		if ( arrayexpr && arrayexpr->Type != ECode::Untyped )
		{
			log_failure( "gen::def_type: arrayexpr is not of untyped type - %s", arrayexpr->debug_str() );
			return CodeInvalid;
		}

		CodeType
		result       = (CodeType) make_code();
		result->Name = get_cached_string( name );
		result->Type = ECode::Typename;

		if ( attributes )
			result->Attributes = attributes;

		if ( specifiers )
			result->Specs = specifiers;

		if ( arrayexpr )
			result->ArrExpr = arrayexpr;

		return result;
	}

	CodeTypedef def_typedef( StrC name, Code type, CodeAttributes attributes, ModuleFlag mflags )
	{
		name_check( def_typedef, name );
		null_check( def_typedef, type );

		if ( type->Type != ECode::Typename )
		{
			log_failure( "gen::def_typedef: type was not a Typename - %s", type.debug_str() );
			return CodeInvalid;
		}

		if ( attributes && attributes->Type != ECode::PlatformAttributes )
		{
			log_failure( "gen::def_typedef: attributes was not a PlatformAttributes - %s", attributes.debug_str() );
			return CodeInvalid;
		}

		// Registering the type.
		Code registered_type = def_type( name );

		if ( ! registered_type )
		{
			log_failure( "gen::def_typedef: failed to register type" );
			return CodeInvalid;
		}

		CodeTypedef
		result              = (CodeTypedef) make_code();
		result->Name        = get_cached_string( name );
		result->Type        = ECode::Typedef;
		result->ModuleFlags = mflags;

		result->UnderlyingType = type;

		if ( attributes )
			result->Attributes = attributes;

		return result;
	}

	CodeUnion def_union( StrC name, Code body, CodeAttributes attributes, ModuleFlag mflags )
	{
		null_check( def_union, body );

		if ( body->Type != ECode::Union_Body )
		{
			log_failure( "gen::def_union: body was not a Union_Body type - %s", body.debug_str() );
			return CodeInvalid;
		}

		if ( attributes && attributes->Type != ECode::PlatformAttributes )
		{
			log_failure( "gen::def_union: attributes was not a PlatformAttributes type - %s", attributes.debug_str() );
			return CodeInvalid;
		}

		CodeUnion
		result              = (CodeUnion) make_code();
		result->ModuleFlags = mflags;
		result->Type        = ECode::Union;

		if ( name.Ptr )
			result->Name = get_cached_string( name );

		result->Body = body;

		if ( attributes )
			result->Attributes = attributes;

		return result;
	}

	CodeUsing def_using( StrC name, CodeType type
		, CodeAttributes attributes
		, ModuleFlag     mflags )
	{
		name_check( def_using, name );
		null_check( def_using, type );

		Code register_type = def_type( name );

		if ( ! register_type )
		{
			log_failure( "gen::def_using: failed to register type" );
			return CodeInvalid;
		}

		if ( attributes && attributes->Type != ECode::PlatformAttributes )
		{
			log_failure( "gen::def_using: attributes was not a PlatformAttributes type - %s", attributes.debug_str() );
			return CodeInvalid;
		}

		CodeUsing
		result              = (CodeUsing) make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;
		result->Type = ECode::Using;

		result->UnderlyingType = type;

		if ( attributes )
			result->Attributes = attributes;

		return result;
	}

	CodeUsing def_using_namespace( StrC name )
	{
		name_check( def_using_namespace, name );

		Code
		result          = make_code();
		result->Name    = get_cached_string( name );
		result->Content = result->Name;
		result->Type    = ECode::Using_Namespace;

		return (CodeUsing) result;
	}

	CodeVar def_variable( CodeType type, StrC name, Code value
		, CodeSpecifier specifiers, CodeAttributes attributes
		, ModuleFlag     mflags )
	{
		name_check( def_variable, name );
		null_check( def_variable, type );

		if ( attributes && attributes->Type != ECode::PlatformAttributes )
		{
			log_failure( "gen::def_variable: attributes was not a `PlatformAttributes` type - %s", attributes.debug_str() );
			return CodeInvalid;
		}

		if ( specifiers && specifiers->Type != ECode::Specifiers )
		{
			log_failure( "gen::def_variable: specifiers was not a `Specifiers` type - %s", specifiers.debug_str() );
			return CodeInvalid;
		}

		if ( type->Type != ECode::Typename )
		{
			log_failure( "gen::def_variable: type was not a Typename - %s", type.debug_str() );
			return CodeInvalid;
		}

		if ( value && value->Type != ECode::Untyped )
		{
			log_failure( "gen::def_variable: value was not a `Untyped` type - %s", value.debug_str() );
			return CodeInvalid;
		}

		CodeVar
		result              = (CodeVar) make_code();
		result->Name        = get_cached_string( name );
		result->Type        = ECode::Variable;
		result->ModuleFlags = mflags;

		result->ValueType = type;

		if ( attributes )
			result->Attributes = attributes;

		if ( specifiers )
			result->Specs = specifiers;

		if ( value )
			result->Value = value;

		return result;
	}

/*
	Body related functions typically follow the same implementation pattern.
	Opted to use inline helper macros to get the implementaiton done.

	The implementation pattern is as follows:
	* Validate a valid parameter num was provided, or code array
		def_body_start or def_body_code_array_start( <name of function >)

	* Begin the code entry do-while loop, make sure each entry is valid processing its type in the switc
		def_body_code_validation_start( <name of function> )

	* Define the switch case statements between the macros.

	* Add the code entry, finish the closing implemenation for the do-while loop.
		def_body_code_validation_end( <name of function> )

	* Lock the body AST and return it.

	If a function's implementation deviates from the macros then its just writen it out.
*/

#pragma region Helper Macros for def_**_body functions
#	define def_body_start( Name_ )                                                \
	using namespace ECode;                                                        \
																                  \
	if ( num <= 0 )                                                               \
	{                                                                             \
		log_failure("gen::" stringize(Name_) ": num cannot be zero or negative"); \
		return CodeInvalid;                                                       \
	}

#	define def_body_code_array_start( Name_ )                                      \
	using namespace ECode;                                                         \
																                   \
	if ( num <= 0 )                                                                \
	{                                                                              \
		log_failure("gen::" stringize(Name_) ": num cannot be zero or negative");  \
		return CodeInvalid;                                                        \
	}                                                                              \
                                                                                   \
	if ( codes == nullptr )                                                        \
	{                                                                              \
		log_failure("gen::" stringize(Name_)" : Provided a null array of codes");  \
		return CodeInvalid;                                                        \
	}

#	define def_body_code_validation_start( Name_ )                                \
	do                                                                            \
	{                                                                             \
		Code_POD pod   = va_arg(va, Code_POD);                                    \
		Code     entry = pcast(Code, pod);                                        \
                                                                                  \
		if ( ! entry )                                                            \
		{                                                                         \
			log_failure("gen::" stringize(Name_) ": Provided an null entry");     \
			return CodeInvalid;                                                   \
		}                                                                         \
                                                                                  \
		switch ( entry->Type )                                                    \
		{

#	define def_body_code_array_validation_start( Name_ )                          \
	do                                                                            \
	{                                                                             \
		Code entry = *codes; codes++;                                             \
                                                                                  \
		if ( ! entry )                                                            \
		{                                                                         \
			log_failure("gen::" stringize(Name_) ": Provided an null entry");     \
			return CodeInvalid;                                                   \
		}                                                                         \
                                                                                  \
		switch ( entry->Type )                                                    \
		{

#	define def_body_code_validation_end( Name_ )                                                              \
				log_failure("gen::" stringize(Name_) ": Entry type is not allowed: %s", entry.debug_str() );  \
				return CodeInvalid;                                                                           \
                                                                                                              \
			default:                                                                                          \
				break;                                                                                        \
		}                                                                                                     \
                                                                                                              \
		result.append( entry );                                                                               \
	}                                                                                                         \
	while ( num--, num > 0 )
#pragma endregion Helper Macros for def_**_body functions

	CodeBody def_class_body( s32 num, ... )
	{
		def_body_start( def_class_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Class_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_class_body );
			AST_BODY_CLASS_UNALLOWED_TYPES
		def_body_code_validation_end( def_class_body );
		va_end(va);

		return result;
	}

	CodeBody def_class_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_class_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Function_Body;

		def_body_code_array_validation_start( def_class_body );
			AST_BODY_CLASS_UNALLOWED_TYPES
		def_body_code_validation_end( def_class_body );

		return result;
	}

	CodeBody def_enum_body( s32 num, ... )
	{
		def_body_start( def_enum_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Enum_Body;

		va_list va;
		va_start(va, num);
		do
		{
			Code_POD pod   = va_arg(va, Code_POD);
			Code     entry = pcast(Code, pod);

			if ( ! entry )
			{
				log_failure("gen::def_enum_body: Provided a null entry");
				return CodeInvalid;
			}

			if ( entry->Type != Untyped && entry->Type != Comment )
			{
				log_failure("gen::def_enum_body: Entry type is not allowed - %s. Must be of untyped or comment type.", entry.debug_str() ); \
				return CodeInvalid;
			}

			result.append( entry );
		}
		while ( num--, num > 0 );
		va_end(va);

		return (CodeBody) result;
	}

	CodeBody def_enum_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_enum_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Enum_Body;

		do
		{
			Code entry = *codes;

			if ( ! entry )
			{
				log_failure("gen::def_enum_body: Provided a null entry");
				return CodeInvalid;
			}

			if ( entry->Type != Untyped && entry->Type != Comment )
			{
				log_failure("gen::def_enum_body: Entry type is not allowed: %s", entry.debug_str() ); \
				return CodeInvalid;
			}

			result.append( entry );
		}
		while ( codes++, num--, num > 0 );

		return result;
	}

	CodeBody def_export_body( s32 num, ... )
	{
		def_body_start( def_export_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Export_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_export_body );
			AST_BODY_EXPORT_UNALLOWED_TYPES
		def_body_code_validation_end( def_export_body );
		va_end(va);

		return result;
	}

	CodeBody def_export_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_export_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Export_Body;

		def_body_code_array_validation_start( def_export_body );
			AST_BODY_EXPORT_UNALLOWED_TYPES
		def_body_code_validation_end( def_export_body );

		return result;
	}

	CodeBody def_extern_link_body( s32 num, ... )
	{
		def_body_start( def_extern_linkage_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Extern_Linkage_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_extern_linkage_body );
			AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES
		def_body_code_validation_end( def_extern_linkage_body );
		va_end(va);

		return result;
	}

	CodeBody def_extern_link_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_extern_linkage_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Extern_Linkage_Body;

		def_body_code_array_validation_start( def_extern_linkage_body );
			AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES
		def_body_code_validation_end( def_extern_linkage_body );

		return result;
	}

	CodeBody def_function_body( s32 num, ... )
	{
		def_body_start( def_function_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Function_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_function_body );
			AST_BODY_FUNCTION_UNALLOWED_TYPES
		def_body_code_validation_end( def_function_body );
		va_end(va);

		return result;
	}

	CodeBody def_function_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_function_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Function_Body;

		def_body_code_array_validation_start( def_function_body );
			AST_BODY_FUNCTION_UNALLOWED_TYPES
		def_body_code_validation_end( def_function_body );

		return result;
	}

	CodeBody def_global_body( s32 num, ... )
	{
		def_body_start( def_global_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Global_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_global_body );
			AST_BODY_GLOBAL_UNALLOWED_TYPES
		def_body_code_validation_end( def_global_body );
		va_end(va);

		return result;
	}

	CodeBody def_global_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_global_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Global_Body;

		def_body_code_array_validation_start( def_global_body );
			AST_BODY_GLOBAL_UNALLOWED_TYPES
		def_body_code_validation_end( def_global_body );

		return result;
	}

	CodeBody def_namespace_body( s32 num, ... )
	{
		def_body_start( def_namespace_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Namespace_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_namespace_body );
			AST_BODY_NAMESPACE_UNALLOWED_TYPES
		def_body_code_validation_end( def_namespace_body );
		va_end(va);

		return result;
	}

	CodeBody def_namespace_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_namespace_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Global_Body;

		def_body_code_array_validation_start( def_namespace_body );
			AST_BODY_NAMESPACE_UNALLOWED_TYPES
		def_body_code_validation_end( def_namespace_body );

		return result;
	}

	CodeParam def_params( s32 num, ... )
	{
		def_body_start( def_params );

		va_list va;
		va_start(va, num);

		Code_POD  pod   = va_arg(va, Code_POD);
		CodeParam param = pcast( CodeParam, pod );

		null_check( def_params, param );

		if ( param->Type != Parameters )
		{
			log_failure( "gen::def_params: param %d is not a Parameters", num - num + 1 );
			return CodeInvalid;
		}

		CodeParam result = (CodeParam) param.duplicate();

		while ( -- num )
		{
			pod   = va_arg(va, Code_POD);
			param = pcast( CodeParam, pod );

			if ( param->Type != Parameters )
			{
				log_failure( "gen::def_params: param %d is not a Parameters", num - num + 1 );
				return CodeInvalid;
			}

			result.append( param );
		}
		va_end(va);

		return result;
	}

	CodeParam def_params( s32 num, CodeParam* codes )
	{
		def_body_code_array_start( def_params );

	#	define check_current()                                                                                      \
		if ( current.ast == nullptr )                                                                               \
		{                                                                                                           \
			log_failure("gen::def_params: Provide a null code in codes array");                                     \
			return CodeInvalid;                                                                                     \
		}                                                                                                           \
                                                                                                                    \
		if (current->Type != Parameters )                                                                           \
		{                                                                                                           \
			log_failure("gen::def_params: Code in coes array is not of paramter type - %s", current.debug_str() );  \
			return CodeInvalid;                                                                                     \
		}

		CodeParam current = (CodeParam) codes->duplicate();
		check_current();

		CodeParam
		result            = (CodeParam) make_code();
		result->Name      = current->Name;
		result->Type      = current->Type;
		result->ValueType = current->ValueType;

		while( codes++, current = * codes, num--, num > 0 )
		{
			check_current();
			result.append( current );
		}
	#	undef check_current

		return result;
	}

	CodeSpecifier def_specifiers( s32 num, ... )
	{
		if ( num <= 0 )
		{
			log_failure("gen::def_specifiers: num cannot be zero or less");
			return CodeInvalid;
		}

		if ( num > AST::ArrSpecs_Cap )
		{
			log_failure("gen::def_specifiers: num of speciifers to define AST larger than AST specicifier capacity - %d", num);
			return CodeInvalid;
		}

		CodeSpecifier
		result       = (CodeSpecifier) make_code();
		result->Type = ECode::Specifiers;

		va_list va;
		va_start(va, num);
		do
		{
			SpecifierT type = (SpecifierT)va_arg(va, int);

			result.append( type );
		}
		while ( --num, num );
		va_end(va);

		return result;
	}

	CodeSpecifier def_specifiers( s32 num, SpecifierT* specs )
	{
		if ( num <= 0 )
		{
			log_failure("gen::def_specifiers: num cannot be zero or less");
			return CodeInvalid;
		}

		if ( num > AST::ArrSpecs_Cap )
		{
			log_failure("gen::def_specifiers: num of speciifers to define AST larger than AST specicifier capacity - %d", num);
			return CodeInvalid;
		}

		CodeSpecifier
		result       = (CodeSpecifier) make_code();
		result->Type = ECode::Specifiers;

		s32 idx = 0;
		do
		{
			result.append( specs[idx] );
			idx++;
		}
		while ( --num, num );

		return result;
	}

	CodeBody def_struct_body( s32 num, ... )
	{
		def_body_start( def_struct_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Struct_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_struct_body );
			AST_BODY_STRUCT_UNALLOWED_TYPES
		def_body_code_validation_end( def_struct_body );
		va_end(va);

		return result;
	}

	CodeBody def_struct_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_struct_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Struct_Body;

		def_body_code_array_validation_start( def_struct_body );
			AST_BODY_STRUCT_UNALLOWED_TYPES
		def_body_code_validation_end( def_struct_body );

		return result;
	}

	CodeBody def_union_body( s32 num, ... )
	{
		def_body_start( def_union_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Union_Body;

		va_list va;
		va_start(va, num);
		do
		{
			Code_POD pod   = va_arg(va, Code_POD);
			Code     entry = pcast( Code, pod );

			if ( ! entry )
			{
				log_failure("gen::def_union_body: Provided a null entry");
				return CodeInvalid;
			}

			if ( entry->Type != Untyped && entry->Type != Comment )
			{
				log_failure("gen::def_union_body: Entry type is not allowed - %s. Must be of untyped or comment type.", entry.debug_str() ); \
				return CodeInvalid;
			}

			result.append( entry );
		}
		while ( num--, num > 0 );
		va_end(va);

		return result;
	}

	CodeBody def_union_body( s32 num, CodeUnion* codes )
	{
		def_body_code_array_start( def_union_body );

		CodeBody
		result       = (CodeBody) make_code();
		result->Type = Union_Body;

		do
		{
			Code entry = *codes;

			if ( ! entry )
			{
				log_failure("gen::def_union_body: Provided a null entry");
				return CodeInvalid;
			}

			if ( entry->Type != Untyped && entry->Type != Comment )
			{
				log_failure("gen::def_union_body: Entry type is not allowed: %s", entry.debug_str() );
				return CodeInvalid;
			}

			result.append( entry );
		}
		while ( codes++, num--, num > 0 );

		return (CodeBody) result;
	}

#	undef name_check
#	undef null_check
#	undef null_or_invalid_check
#pragma endregion Upfront Constructors

#pragma region Parsing Constructors
#ifdef GEN_FEATURE_PARSING
/*
	These constructors are the most implementation intensive other than the edtior or scanner.
*/

	namespace Parser
	{

	/*
		This is a simple lexer that focuses on tokenizing only tokens relevant to the library.
		It will not be capable of lexing C++ code with unsupported features.

		For the sake of scanning files, it can scan preprocessor directives
	*/

	#	define Define_TokType \
		Entry( Access_Private,         "private" )         \
		Entry( Access_Protected,       "protected" )       \
		Entry( Access_Public,          "public" )          \
		Entry( Access_MemberSymbol,    "." )               \
		Entry( Access_StaticSymbol,    "::")               \
		Entry( Ampersand,              "&" )               \
		Entry( Ampersand_DBL,          "&&" )              \
		Entry( Assign_Classifer,       ":" )               \
		Entry( BraceCurly_Open,        "{" )               \
		Entry( BraceCurly_Close,       "}" )               \
		Entry( BraceSquare_Open,       "[" )               \
		Entry( BraceSquare_Close,      "]" )               \
		Entry( Capture_Start,          "(" )               \
		Entry( Capture_End,            ")" )               \
		Entry( Comment,                "__comment__" )     \
		Entry( Char,                   "__char__" )        \
		Entry( Comma,                  "," )               \
		Entry( Decl_Class,             "class" )           \
		Entry( Decl_Enum,              "enum" )            \
		Entry( Decl_Extern_Linkage,    "extern" )          \
		Entry( Decl_Friend,            "friend" )          \
		Entry( Decl_Module,            "module" )          \
		Entry( Decl_Namespace,         "namespace" )       \
		Entry( Decl_Operator,          "operator" )        \
		Entry( Decl_Struct,            "struct" )          \
		Entry( Decl_Template,          "template" )        \
		Entry( Decl_Typedef,           "typedef" )         \
		Entry( Decl_Using,             "using" )           \
		Entry( Decl_Union,             "union" )           \
		Entry( Identifier,             "__identifier__" )  \
		Entry( Module_Import,          "import" )          \
		Entry( Module_Export,          "export" )          \
		Entry( Number,                 "number" )          \
		Entry( Operator,               "operator" )        \
		Entry( Preprocessor_Directive, "#")                \
		Entry( Preprocessor_Include,   "include" )         \
		Entry( Spec_Alignas,           "alignas" )         \
		Entry( Spec_Const,             "const" )           \
		Entry( Spec_Consteval,         "consteval" )       \
		Entry( Spec_Constexpr,         "constexpr" )       \
		Entry( Spec_Constinit,         "constinit" )       \
		Entry( Spec_Extern,            "extern" )          \
		Entry( Spec_Global, 		   "global" )          \
		Entry( Spec_Inline,            "inline" )          \
		Entry( Spec_Internal_Linkage,  "internal" )        \
		Entry( Spec_LocalPersist,      "local_persist" )   \
		Entry( Spec_Mutable,           "mutable" )         \
		Entry( Spec_Static,            "static" )          \
		Entry( Spec_ThreadLocal,       "thread_local" )    \
		Entry( Spec_Volatile,          "volatile")         \
		Entry( Star,                   "*" )               \
		Entry( Statement_End,          ";" )               \
		Entry( String,                 "__string__" )      \
		Entry( Type_Unsigned, 	       "unsigned" )        \
		Entry( Type_Signed,            "signed" )          \
		Entry( Type_Short,             "short" )           \
		Entry( Type_Long,              "long" )            \
		Entry( Type_char, 			   "char" )            \
		Entry( Type_int, 			   "int" )             \
		Entry( Type_double, 		   "double" )

		enum class TokType : u32
		{
		#	define Entry( Name_, Str_ ) Name_,
			Define_TokType
		#	undef Entry
			Attr_Keyword,

			Num,
			Invalid
		};

		struct Token
		{
			char const* Text;
			sptr        Length;
			TokType     Type;
			bool 	    IsAssign;

			operator bool()
			{
				return Text && Length && Type != TokType::Invalid;
			}

			operator StrC()
			{
				return { Length, Text };
			}
		};

		internal inline
		TokType get_tok_type( char const* word, s32 length )
		{
			local_persist
			StrC lookup[(u32)TokType::Num] =
			{
			#	define Entry( Name_, Str_ ) { sizeof(Str_), Str_ },
				Define_TokType
			#	undef Entry

				{ sizeof(Attribute::Keyword), Attribute::Keyword },
			};

			for ( u32 index = 0; index < (u32)TokType::Num; index++ )
			{
				s32         lookup_len = lookup[index].Len - 1;
				char const* lookup_str = lookup[index].Ptr;

				if ( lookup_len != length )
					continue;

				if ( str_compare( word, lookup_str, lookup_len ) == 0 )
					return scast(TokType, index);
			}

			return TokType::Invalid;
		}

		internal inline
		char const* str_tok_type( TokType type )
		{
			local_persist
			char const* lookup[(u32)TokType::Num] =
			{
			#	define Entry( Name_, Str_ ) Str_,
				Define_TokType
			#	undef Entry

				Attribute::Keyword,
			};

			return lookup[(u32)type];
		}

	#	undef Define_TokType

		internal inline
		bool tok_is_specifier( Token const& tok )
		{
			return (tok.Type <= TokType::Star && tok.Type >= TokType::Spec_Alignas)
				|| tok.Type == TokType::Ampersand
				|| tok.Type == TokType::Ampersand_DBL
			;
		}

		internal inline
		bool tok_is_access_specifier( Token const& tok )
		{
			return tok.Type >= TokType::Access_Private && tok.Type <= TokType::Access_Public;
		}

		internal inline
		AccessSpec tok_to_access_specifier( Token const& tok )
		{
			return scast(AccessSpec, tok.Type);
		}


		struct TokArray
		{
			Array<Token> Arr;
			s32          Idx;

			bool __eat( TokType type, char const* context )
			{
				if ( Arr.num() - Idx <= 0 )
				{
					log_failure( "gen::%s: No tokens left", context );
					return Code::Invalid;
				}

				if ( Arr[Idx].Type != type )
				{
					String token_str = String::make( GlobalAllocator, { Arr[Idx].Length, Arr[Idx].Text } );

					log_failure( "gen::%s: expected %s, got %s", context, str_tok_type(type), str_tok_type(Arr[Idx].Type) );

					return Code::Invalid;
				}

				Idx++;
				return true;
			}

			Token& current()
			{
				return Arr[Idx];
			}

			Token& previous()
			{
				return Arr[Idx - 1];
			}

			Token* next()
			{
				return Idx + 1 < Arr.num() ? &Arr[Idx + 1] : nullptr;
			}
		};

		TokArray lex( StrC content, bool keep_preprocess_directives = false )
		{
		#	define current ( * scanner )

		#	define move_forward() \
			left--;               \
			scanner++

		#	define SkipWhitespace()                     \
			while ( left && char_is_space( current ) )  \
			{                                           \
				move_forward();                         \
			}

		#	define SkipWhitespace_Checked( Context_, Msg_, ... )             \
			while ( left && char_is_space( current ) )                       \
			{                                                                \
				move_forward();                                              \
			}                                                                \
			if ( left <= 0 )                                                 \
			{                                                                \
				log_failure( "gen::" txt(Context_) ": " Msg_, __VA_ARGS__ ); \
				return { 0, nullptr };                                       \
			}

			local_persist thread_local
			Array<Token> Tokens = { nullptr };

			s32         left    = content.Len;
			char const* scanner = content.Ptr;

			char const* word        = scanner;
			s32         word_length = 0;

			SkipWhitespace();
			if ( left <= 0 )
			{
				log_failure( "gen::lex: no tokens found (only whitespace provided)" );
				return { { nullptr }, 0 };
			}

			if ( Tokens )
			{
				Tokens.free();
			}

			Tokens = Array<Token>::init_reserve( LexArena, content.Len / 6 );

			while (left )
			{
				Token token = { nullptr, 0, TokType::Invalid, false };

				SkipWhitespace();
				if ( left <= 0 )
					break;

				switch ( current )
				{
					case '#':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Preprocessor_Directive;
						move_forward();

						while (left && current != '\n' )
						{
							if ( current == '\\'  )
							{
								move_forward();

								if ( current != '\n' && keep_preprocess_directives )
								{
									log_failure( "gen::lex: invalid preprocessor directive, will still grab but will not compile %s", token.Text );
								}
							}

							move_forward();
							token.Length++;
						}
						goto FoundToken;

					case '.':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Access_MemberSymbol;

						if (left)
							move_forward();
						goto FoundToken;

					case '&' :
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Ampersand;

						if (left)
							move_forward();

						if ( current == '&' )	// &&
						{
							token.Length  = 2;
							token.Type    = TokType::Ampersand_DBL;

							if (left)
								move_forward();
						}

						goto FoundToken;

					case ':':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Assign_Classifer;

						if (left)
							move_forward();

						if ( current == ':' )
						{
							move_forward();
							token.Type  = TokType::Access_StaticSymbol;
							token.Length++;
						}
						goto FoundToken;

					case '{':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::BraceCurly_Open;

						if (left)
							move_forward();
						goto FoundToken;

					case '}':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::BraceCurly_Close;

						if (left)
							move_forward();
						goto FoundToken;

					case '[':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::BraceSquare_Open;
						if ( left )
						{
							move_forward();

							if ( current == ']' )
							{
								token.Length = 2;
								token.Type   = TokType::Operator;
								move_forward();
							}
						}
						goto FoundToken;

					case ']':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::BraceSquare_Close;

						if (left)
							move_forward();
						goto FoundToken;

					case '(':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Capture_Start;

						if (left)
							move_forward();
						goto FoundToken;

					case ')':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Capture_End;

						if (left)
							move_forward();
						goto FoundToken;

					case '\'':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Char;

						move_forward();

						while ( left && current != '\'' )
						{
							move_forward();
							token.Length++;
						}

						if ( left )
						{
							move_forward();
							token.Length++;
						}
						goto FoundToken;

					case ',':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Comma;

						if (left)
							move_forward();
						goto FoundToken;

					case '*':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Star;

						if (left)
							move_forward();
						goto FoundToken;

					case ';':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Statement_End;

						if (left)
							move_forward();
						goto FoundToken;

					case '"':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::String;

						move_forward();
						while ( left )
						{
							if ( current == '"' )
							{
								move_forward();
								break;
							}

							if ( current == '\\' )
							{
								move_forward();
								token.Length++;

								if ( left )
								{
									move_forward();
									token.Length++;
								}
								continue;
							}

							move_forward();
							token.Length++;
						}
						goto FoundToken;

					// All other operators we just label as an operator and move forward.
					case '=':
						token.Text     = scanner;
						token.Length   = 1;
						token.Type     = TokType::Operator;
						token.IsAssign = true;

						if (left)
							move_forward();

						goto FoundToken;

					case '+':
					case '%':
					case '^':
					case '~':
					case '!':
					case '<':
					case '>':
					case '|':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Operator;

						if (left)
							move_forward();

						if ( current == '=' )
						{
							token.Length++;
							token.IsAssign = true;

							if (left)
								move_forward();
						}
						else while ( left && current == *(scanner - 1) && token.Length < 3 )
						{
							token.Length++;

							if (left)
								move_forward();
						}
						goto FoundToken;

					// Dash is unfortunatlly a bit more complicated...
					case '-':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Operator;
						if ( left )
						{
							move_forward();

							if ( current == '>'  )
							{
								token.Length++;
								move_forward();

								if ( current == '*' )
								{
									token.Length++;
									move_forward();
								}
							}
							else if ( current == '=' )
							{
								token.Length++;
								token.IsAssign = true;

								if (left)
									move_forward();
							}
							else while ( left && current == *(scanner - 1) && token.Length < 3 )
							{
								token.Length++;

								if (left)
									move_forward();
							}
						}
						goto FoundToken;

					case '/':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Operator;

						if ( left )
						{
							move_forward();

							if ( current == '/' )
							{
								token.Type = TokType::Comment;

								move_forward();
								token.Text   = scanner;
								token.Length = 0;

								while ( left && current != '\n' )
								{
									move_forward();
									token.Length++;
								}
							}
							else if ( current == '*' )
							{
								token.Type = TokType::Comment;

								move_forward();
								token.Text   = scanner;
								token.Length = 0;

								while ( left && ( current != '*' && *(scanner + 1) != '/' ) )
								{
									move_forward();
									token.Length++;
								}
								move_forward();
								move_forward();
							}
						}
						goto FoundToken;
				}

				if ( char_is_alpha( current ) || current == '_' )
				{
					token.Text   = scanner;
					token.Length = 1;
					move_forward();

					while ( left && ( char_is_alphanumeric(current) || current == '_' ) )
					{
						move_forward();
						token.Length++;
					}

					goto FoundToken;
				}
				else if ( char_is_digit(current) )
				{
					// This is a very brute force lex, no checks are done for validity of literal.

					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Number;
					move_forward();

					if (left
					&& (	current == 'x' || current == 'X'
						||	current == 'b' || current == 'B'
						||  current == 'o' || current == 'O' )
					)
					{
						move_forward();
						token.Length++;

						while ( left && char_is_hex_digit(current) )
						{
							move_forward();
							token.Length++;
						}

						goto FoundToken;
					}

					while ( left && char_is_digit(current) )
					{
						move_forward();
						token.Length++;
					}

					if ( left && current == '.' )
					{
						move_forward();
						token.Length++;

						while ( left && char_is_digit(current) )
						{
							move_forward();
							token.Length++;
						}
					}

					goto FoundToken;
				}
				else
				{
					String context_str = String::fmt_buf( GlobalAllocator, "%s", scanner, min( 100, left ) );

					log_failure( "Failed to lex token %s", context_str );

					// Skip to next whitespace since we can't know if anything else is valid until then.
					while ( left && ! char_is_space( current ) )
					{
						move_forward();
					}
				}

			FoundToken:

				if ( token.Type != TokType::Invalid )
				{
					if ( token.Type == TokType::Preprocessor_Directive && keep_preprocess_directives == false )
						continue;

					Tokens.append( token );
					continue;
				}

				TokType type = get_tok_type( token.Text, token.Length );

				if ( type == TokType::Invalid)
					type = TokType::Identifier;

				token.Type = type;
				Tokens.append( token );
			}

			if ( Tokens.num() == 0 )
			{
				log_failure( "Failed to lex any tokens" );
				return { { nullptr }, 0 };
			}

			return { Tokens, 0 };
		#	undef current
		#	undef move_forward
		#	undef SkipWhitespace
		#	undef SkipWhitespace_Checked
		}
	}

#pragma region Helper Macros
#	define check_parse_args( func, def )                                       \
	if ( def.Len <= 0 )                                                        \
	{                                                                          \
		log_failure( "gen::" stringize(func) ": length must greater than 0" ); \
		return CodeInvalid;                                                    \
	}                                                                          \
	if ( def.Ptr == nullptr )                                                  \
	{                                                                          \
		log_failure( "gen::" stringize(func) ": def was null" );               \
		return CodeInvalid;                                                    \
	}

#	define nexttok 	    toks.next()
#	define currtok      toks.current()
#	define prevtok      toks.previous()
#	define eat( Type_ ) toks.__eat( Type_, context )
#	define left         ( toks.Arr.num() - toks.Idx )

#	define check( Type_ ) ( left && currtok.Type == Type_ )
#pragma endregion Helper Macros

	struct ParseContext
	{
		ParseContext* Parent;
		char const*   Fn;
	};

	internal Code parse_function_body    ( Parser::TokArray& toks, char const* context );
	internal Code parse_global_nspace    ( Parser::TokArray& toks, char const* context );

	internal CodeClass     parse_class            ( Parser::TokArray& toks, char const* context );
	internal CodeEnum      parse_enum             ( Parser::TokArray& toks, char const* context );
	internal CodeBody      parse_export_body      ( Parser::TokArray& toks, char const* context );
	internal CodeBody      parse_extern_link_body ( Parser::TokArray& toks, char const* context );
	internal CodeExtern    parse_exten_link       ( Parser::TokArray& toks, char const* context );
	internal CodeFriend    parse_friend           ( Parser::TokArray& toks, char const* context );
	internal CodeFn        parse_function         ( Parser::TokArray& toks, char const* context );
	internal CodeNamespace parse_namespace        ( Parser::TokArray& toks, char const* context );
	internal CodeOpCast    parse_operator_cast    ( Parser::TokArray& toks, char const* context );
	internal CodeStruct    parse_struct           ( Parser::TokArray& toks, char const* context );
	internal CodeVar       parse_variable         ( Parser::TokArray& toks, char const* context );
	internal CodeTemplate  parse_template         ( Parser::TokArray& toks, char const* context );
	internal CodeType      parse_type             ( Parser::TokArray& toks, char const* context );
	internal CodeTypedef   parse_typedef          ( Parser::TokArray& toks, char const* context );
	internal CodeUnion     parse_union            ( Parser::TokArray& toks, char const* context );
	internal CodeUsing     parse_using            ( Parser::TokArray& toks, char const* context );

	internal inline
	Code parse_array_decl( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		if ( check( TokType::BraceSquare_Open ) )
		{
			eat( TokType::BraceSquare_Open );

			if ( left == 0 )
			{
				log_failure( "%s: Error, unexpected end of typedef definition ( '[]' scope started )", stringize(parse_typedef) );
				return Code::Invalid;
			}

			if ( currtok.Type == TokType::BraceSquare_Close )
			{
				log_failure( "%s: Error, empty array expression in typedef definition", stringize(parse_typedef) );
				return Code::Invalid;
			}

			Token untyped_tok = currtok;

			while ( left && currtok.Type != TokType::BraceSquare_Close )
			{
				eat( currtok.Type );
			}

			untyped_tok.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)untyped_tok.Text;

			Code array_expr = untyped_str( untyped_tok );

			if ( left == 0 )
			{
				log_failure( "%s: Error, unexpected end of type definition, expected ]", stringize(parse_typedef) );
				return Code::Invalid;
			}

			if ( currtok.Type != TokType::BraceSquare_Close )
			{
				log_failure( "%s: Error, expected ] in type definition, not %s", stringize(parse_typedef), str_tok_type( currtok.Type ) );
				return Code::Invalid;
			}

			eat( TokType::BraceSquare_Close );
			return array_expr;
		}

		return { nullptr };
	}

	internal inline
	Parser::Token parse_identifier( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		Token name = currtok;

		eat( TokType::Identifier );

		while ( check( TokType::Access_StaticSymbol ) )
		{
			eat( TokType::Access_StaticSymbol );

			if ( left == 0 )
			{
				log_failure( "%s: Error, unexpected end of type definition, expected identifier", context );
				return { nullptr, 0, TokType::Invalid };
			}

			if ( currtok.Type != TokType::Identifier )
			{
				log_failure( "%s: Error, expected identifier in type definition, not %s", context, str_tok_type( currtok.Type ) );
				return { nullptr, 0, TokType::Invalid };
			}

			name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
			eat( TokType::Identifier );
		}

		return name;
	}

	internal
	CodeParam parse_params( Parser::TokArray& toks, char const* context, bool use_template_capture = false )
	{
		using namespace Parser;
		using namespace ECode;

		if ( ! use_template_capture )
			eat( TokType::Capture_Start );

		else
		{
			if ( check ( TokType::Operator ) && currtok.Text[0] == '<' )
				eat( TokType::Operator );
		}

		if ( ! use_template_capture &&  check(TokType::Capture_End) )
		{
			eat( TokType::Capture_End );
			return { nullptr };
		}

		CodeType type  = { nullptr };
		Code     value = { nullptr };

		type = parse_type( toks, context );
		if ( type == Code::Invalid )
			return CodeInvalid;

		Token name = { nullptr, 0, TokType::Invalid, false };

		if ( check( TokType::Identifier )  )
		{
			name = currtok;
			eat( TokType::Identifier );

			if ( currtok.IsAssign )
			{
				eat( TokType::Operator );

				Token value_tok = currtok;

				if ( currtok.Type == TokType::Statement_End )
				{
					log_failure( "gen::%s: Expected value after assignment operator", context );
					return CodeInvalid;
				}

				while ( left && currtok.Type != TokType::Statement_End )
				{
					value_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)value_tok.Text;
					eat( currtok.Type );
				}

				value = parse_type( toks, context );
			}
		}

		CodeParam
		result       = (CodeParam) make_code();
		result->Type = Parameters;

		if ( name.Length > 0 )
			result->Name = get_cached_string( name );

		result->ValueType = type;

		if ( value )
			result->Value = value;

		result->NumEntries++;

		while ( left
			&& use_template_capture ?
					currtok.Type != TokType::Operator && currtok.Text[0] !=  '>'
				:	currtok.Type != TokType::Capture_End )
		{
			eat( TokType::Comma );

			Code type  = { nullptr };
			Code value = { nullptr };

			type = parse_type( toks, context );
			if ( type == Code::Invalid )
				return CodeInvalid;

			name = { nullptr, 0, TokType::Invalid, false };

			if ( check( TokType::Identifier )  )
			{
				name = currtok;
				eat( TokType::Identifier );

				if ( currtok.IsAssign )
				{
					eat( TokType::Operator );

					Token value_tok = currtok;

					if ( currtok.Type == TokType::Statement_End )
					{
						log_failure( "gen::%s: Expected value after assignment operator", context );
						return CodeInvalid;
					}

					while ( left && currtok.Type != TokType::Statement_End )
					{
						value_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)value_tok.Text;
						eat( currtok.Type );
					}

					value = parse_type( toks, context );
				}
			}

			CodeParam
			param       = (CodeParam) make_code();
			param->Type = Parameters;

			if ( name.Length > 0 )
				param->Name = get_cached_string( name );

			param->ValueType = type;

			if ( value )
				param->Value = value;

			result.append( param );
		}

		if ( ! use_template_capture )
			eat( TokType::Capture_End );

		else
		{
			if ( ! check( TokType::Operator) || currtok.Text[0] != '>' )
			{
				log_failure("gen::parse_template: expected '<' after 'template' keyword. %s", str_tok_type( currtok.Type ));
				return CodeInvalid;
			}
			eat( TokType::Operator );
		}

		return result;
	#	undef context
	}

	// Function parsing is handled in multiple places because its initial signature is shared with variable parsing
	internal inline
	CodeFn parse_function_after_name(
		  ModuleFlag        mflags
		, CodeAttributes    attributes
		, CodeSpecifier     specifiers
		, CodeType          ret_type
		, StrC              name
		, Parser::TokArray& toks
		, char const*        context
	)
	{
		using namespace Parser;

		CodeParam params = parse_params( toks, stringize(parse_function) );

		CodeBody body = { nullptr };
		if ( check( TokType::BraceCurly_Open ) )
		{
			body = parse_function_body( toks, stringize(parse_function) );
			if ( body == Code::Invalid )
				return CodeInvalid;
		}
		else
		{
			eat( TokType::Statement_End );
		}

		using namespace ECode;

		CodeFn
		result              = (CodeFn) make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		if ( body )
		{
			switch ( body->Type )
			{
				case Function_Body:
				case Untyped:
					break;

				default:
				{
					log_failure("gen::def_function: body must be either of Function_Body or Untyped type. %s", body.debug_str());
					return CodeInvalid;
				}
			}

			result->Type = Function;
			result->Body = body;
		}
		else
		{
			result->Type = Function_Fwd;
		}

		if ( specifiers )
			result->Specs = specifiers;

		result->ReturnType = ret_type;

		if ( params )
			result->Params = params;

		return result;
	}

	internal inline
	CodeOperator parse_operator_after_ret_type( ModuleFlag mflags
		, CodeAttributes attributes
		, CodeSpecifier  specifiers
		, CodeType       ret_type
		, Parser::TokArray& toks
		, char const* context )
	{
		using namespace Parser;
		using namespace EOperator;

		// Parse Operator
		eat( TokType::Decl_Operator );

		if ( ! check( TokType::Operator ) )
		{
			log_failure( "gen::%s: Expected operator after 'operator' keyword", context );
			return CodeInvalid;
		}

		OperatorT op = Invalid;
		switch ( currtok.Text[0] )
		{
			case '+':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_Add;

				else
					op = Add;
			}
			break;
			case '-':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_Subtract;

				else
					op = Subtract;
			}
			break;
			case '*':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_Multiply;

				else
				{
					Token& finder = prevtok;
					while ( finder.Type != TokType::Decl_Operator )
					{
						if ( finder.Type == TokType::Identifier)
						{
							op = Indirection;
							break;
						}
					}

					if ( op == Invalid)
						op = Multiply;
				}
			}
			break;
			case '/':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_Divide;

				else
					op = Divide;
			}
			break;
			case '%':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_Modulo;

				else
					op = Modulo;
			}
			break;
			case '&':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_BAnd;

				else if ( currtok.Text[1] == '&' )
					op = LAnd;

				else
				{


					if ( op == Invalid )
					op = BAnd;
				}
			}
			break;
			case '|':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_BOr;

				else if ( currtok.Text[1] == '|' )
					op = LOr;

				else
					op = BOr;
			}
			break;
			case '^':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_BXOr;

				else
					op = BXOr;
			}
			break;
			case '~':
			{
				op = BNot;
			}
			break;
			case '!':
			{
				if ( currtok.Text[1] == '=' )
					op = LNot;

				else
					op = UnaryNot;
			}
			break;
			case '=':
			{
				if ( currtok.Text[1] == '=' )
					op = LEqual;

				else
					op = Assign;
			}
			break;
			case '<':
			{
				if ( currtok.Text[1] == '=' )
					op = LEqual;

				else if ( currtok.Text[1] == '<' )
				{
					if ( currtok.Text[2] == '=' )
						op = Assign_LShift;

					else
						op = LShift;
				}
				else
					op = Lesser;
			}
			break;
			case '>':
			{
				if ( currtok.Text[1] == '=' )
					op = GreaterEqual;

				else if ( currtok.Text[1] == '>' )
				{
					if ( currtok.Text[2] == '=' )
						op = Assign_RShift;

					else
						op = RShift;
				}
				else
					op = Greater;
			}
			break;
			case '(':
			{
				if ( currtok.Text[1] == ')' )
					op = FunctionCall;

				else
					op = Invalid;
			}
			break;
			case '[':
			{
				if ( currtok.Text[1] == ']' )
					op = Subscript;

				else
					op = Invalid;
			}
			break;
			default:
			{
				break;
			}
		}

		if ( op == Invalid )
		{
			log_failure( "gen::%s: Invalid operator '%s'", context, currtok.Text );
			return CodeInvalid;
		}

		eat( TokType::Operator );

		// Parse Params
		CodeParam params = parse_params( toks, stringize(parse_operator) );

		// Parse Body
		CodeBody body = { nullptr };
		if ( check( TokType::BraceCurly_Open ) )
		{
			body = parse_function_body( toks, stringize(parse_function) );
			if ( body == Code::Invalid )
				return CodeInvalid;
		}
		else
		{
			eat( TokType::Statement_End );
		}

		// OpValidateResult check_result = operator__validate( op, params, ret_type, specifiers );
		CodeOperator result = def_operator( op, params, ret_type, body, specifiers, attributes, mflags );
		return result;
	}

	// Variable parsing is handled in multiple places because its initial signature is shared with function parsing
	internal inline
	CodeVar parse_variable_after_name(
		  ModuleFlag        mflags
		, CodeAttributes    attributes
		, CodeSpecifier     specifiers
		, CodeType          type
		, StrC              name
		, Parser::TokArray& toks
		, char const*       context )
	{
		using namespace Parser;

		Code array_expr = parse_array_decl( toks, stringize(parse_variable) );

		Code expr = { nullptr };

		if ( currtok.IsAssign )
		{
			eat( TokType::Operator );

			Token expr_tok = currtok;

			if ( currtok.Type == TokType::Statement_End )
			{
				log_failure( "gen::parse_variable: expected expression after assignment operator" );
				return CodeInvalid;
			}

			while ( left && currtok.Type != TokType::Statement_End )
			{
				eat( currtok.Type );
			}

			expr_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)expr_tok.Text;
			expr            = untyped_str( expr_tok );
		}

		eat( TokType::Statement_End );

		using namespace ECode;

		CodeVar
		result              = (CodeVar) make_code();
		result->Type        = Variable;
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		result->ValueType = type;

		if (array_expr )
			type->ArrExpr = array_expr;

		if ( attributes )
			result->Attributes = attributes;

		if ( specifiers )
			result->Specs = specifiers;

		if ( expr )
			result->Value = expr;

		return result;
	}

	internal inline
	Code parse_variable_assignment( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Code expr = Code::Invalid;

		if ( currtok.IsAssign )
		{
			eat( TokType::Operator );

			Token expr_tok = currtok;

			if ( currtok.Type == TokType::Statement_End )
			{
				log_failure( "gen::parse_variable: expected expression after assignment operator" );
				return Code::Invalid;
			}

			while ( left && currtok.Type != TokType::Statement_End )
			{
				expr_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)expr_tok.Text;
				eat( currtok.Type );
			}

			expr = untyped_str( expr_tok );
		}

		return expr;
	}

	internal inline
	Code parse_operator_function_or_variable( bool expects_function, CodeAttributes attributes, CodeSpecifier specifiers, Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Code result = Code::Invalid;

		CodeType type = parse_type( toks, stringize(parse_variable) );

		if ( type == Code::Invalid )
			return CodeInvalid;

		if ( check( TokType::Operator) )
		{
			// Dealing with an operator overload
			result = parse_operator_after_ret_type( ModuleFlag::None, attributes, specifiers, type, toks, stringize(parse_template) );
		}
		else
		{
			StrC name = currtok;
			eat( TokType::Identifier );

			if ( check( TokType::Capture_Start) )
			{
				// Dealing with a function

				result = parse_function_after_name( ModuleFlag::None, attributes, specifiers, type, name, toks, stringize(parse_template) );
			}
			else
			{
				if ( expects_function )
				{
					log_failure( "gen::parse_template: expected function declaration (consteval was used)" );
					return Code::Invalid;
				}

				// Dealing with a variable
				result = parse_variable_after_name( ModuleFlag::None, attributes, specifiers, type, name, toks, stringize(parse_template) );
			}
		}

		return result;
	}

	internal
	CodeBody parse_class_struct_body( Parser::TokType which, Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		eat( TokType::BraceCurly_Open );

		CodeBody
		result = (CodeBody) make_code();

		if ( which == TokType::Decl_Class )
			result->Type = Class_Body;

		else
			result->Type = Struct_Body;

		while ( left && currtok.Type != TokType::BraceCurly_Close )
		{
			Code           member     = Code::Invalid;
			CodeAttributes attributes = { nullptr };
			CodeSpecifier  specifiers = { nullptr };

			bool expects_function = false;

			switch ( currtok.Type )
			{
				case TokType::Comment:
					member = def_comment( currtok );
					eat( TokType::Comment );
				break;

				case TokType::Access_Public:
					member = access_public;
					eat( TokType::Access_Public );
					eat( TokType::Assign_Classifer );
				break;

				case TokType::Access_Protected:
					member = access_protected;
					eat( TokType::Access_Protected );
					eat( TokType::Assign_Classifer );
				break;

				case TokType::Access_Private:
					member = access_private;
					eat( TokType::Access_Private );
					eat( TokType::Assign_Classifer );
				break;

				case TokType::Decl_Class:
					member = parse_class( toks, context );
				break;

				case TokType::Decl_Enum:
					member = parse_enum( toks, context );
				break;

				case TokType::Decl_Friend:
					member = parse_friend( toks, context );
				break;

				case TokType::Decl_Operator:
					member = parse_operator_cast( toks, context );
				break;

				case TokType::Decl_Struct:
					member = parse_struct( toks, context );
				break;

				case TokType::Decl_Template:
					member = parse_template( toks, context );
				break;

				case TokType::Decl_Typedef:
					member = parse_typedef( toks, context );
				break;

				case TokType::Decl_Union:
					member = parse_variable( toks, context );
				break;

				case TokType::Decl_Using:
					member = parse_using( toks, context );
				break;

				case TokType::BraceSquare_Open:
				case TokType::Attr_Keyword:
				{
					// Standard attribute
					if ( currtok.Type == TokType::BraceSquare_Open  )
					{
						eat( TokType::BraceSquare_Open );

						if ( currtok.Type != TokType::BraceSquare_Open )
						{
							log_failure( "%s: Error, expected attribute name", context );
							return result;
						}

						while ( left && currtok.Type != TokType::BraceSquare_Close )
						{
							// TODO : Parse attributes
						}

						eat( TokType::BraceSquare_Close );
						eat( TokType::BraceSquare_Close );
					}

					// Platform Specific attribute
					eat( TokType::Attr_Keyword );
					eat( TokType::Capture_Start );

					// TODO : Parse attributes

					eat( TokType::Capture_End );
				}
				//! Fallthrough intended
				case TokType::Spec_Consteval:
				case TokType::Spec_Constexpr:
				case TokType::Spec_Constinit:
				case TokType::Spec_Inline:
				case TokType::Spec_Mutable:
				case TokType::Spec_Static:
				case TokType::Spec_Volatile:
				{
					SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
					s32        num_specifiers = 0;

					while ( left && tok_is_specifier( currtok ) )
					{
						SpecifierT spec = ESpecifier::to_type( currtok );

						switch ( spec )
						{
							case ESpecifier::Constexpr:
							case ESpecifier::Constinit:
							case ESpecifier::Inline:
							case ESpecifier::Mutable:
							case ESpecifier::Static:
							case ESpecifier::Volatile:
							break;

							case ESpecifier::Consteval:
								expects_function = true;
							break;

							default:
								log_failure( "gen::parse_class_struct_body: invalid specifier " "%s" " for variable", ESpecifier::to_str(spec) );
								return CodeInvalid;
						}

						specs_found[num_specifiers] = spec;
						num_specifiers++;
						eat( currtok.Type );
					}

					if ( num_specifiers )
					{
						specifiers = def_specifiers( num_specifiers, specs_found );
					}
				}
				//! Fallthrough intentional
				case TokType::Identifier:
				case TokType::Spec_Const:
				case TokType::Type_Unsigned:
				case TokType::Type_Signed:
				case TokType::Type_Short:
				case TokType::Type_Long:
				{
					member = parse_operator_function_or_variable( expects_function, attributes, specifiers, toks, context );
				}
				break;

				default:
					Token untyped_tok = currtok;

					while ( left && currtok.Type != TokType::BraceCurly_Close )
					{
						untyped_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)untyped_tok.Text;
						eat( currtok.Type );
					}

					member = untyped_str( untyped_tok );
				break;
			}

			if ( member == Code::Invalid )
			{
				log_failure( "gen::parse_variable: failed to parse member" );
				return CodeInvalid;
			}

			result.append( member );
		}

		eat( TokType::BraceCurly_Close );
		return result;
	}

	internal
	Code parse_class_struct( Parser::TokType which, Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		if ( which != TokType::Decl_Class && which != TokType::Decl_Struct )
		{
			log_failure( "%s: Error, expected class or struct, not %s", context, str_tok_type( which ) );
			return CodeInvalid;
		}

		Token name { nullptr, 0, TokType::Invalid };

		CodeType       parent     = { nullptr };
		CodeBody       body       = { nullptr };
		CodeAttributes attributes = { nullptr };
		ModuleFlag     mflags     = ModuleFlag::None;

		CodeClass result = CodeInvalid;

		// TODO : Parse module specifiers

		eat( which );

		// TODO : Parse attributes

		if ( check( TokType::Identifier ) )
			name = parse_identifier( toks, context );

		AccessSpec access     = AccessSpec::Default;
		Token      parent_tok = { nullptr, 0, TokType::Invalid };

		if ( check( TokType::Assign_Classifer ) )
		{
			eat( TokType::Assign_Classifer );

			if ( tok_is_access_specifier( currtok ) )
			{
				access = tok_to_access_specifier( currtok );
			}

			parent_tok = parse_identifier( toks, context );
		}

		if ( check( TokType::BraceCurly_Open ) )
		{
			body = parse_class_struct_body( which, toks, context );
		}

		eat( TokType::Statement_End );

		if ( parent_tok )
			parent = def_type( parent_tok );

		if ( which == TokType::Decl_Class )
			result = def_class( name, body, parent, access
				// TODO : Set these up later
				, NoCode // Attributes
				, ModuleFlag::None
			);

		else
			result = def_struct( name, body, (CodeType)parent, access
				// TODO : Set these up later
				, NoCode // Attributes
				, ModuleFlag::None
			);

		return result;
	}

	internal
	Code parse_function_body( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		eat( TokType::BraceCurly_Open );

		CodeBody
		result = (CodeBody) make_code();
		result->Type = Function_Body;

		Token start = currtok;

		s32 level = 0;
		while ( left && ( currtok.Type != TokType::BraceCurly_Close || level > 0 ) )
		{
			if ( currtok.Type == TokType::BraceCurly_Open )
				level++;

			else if ( currtok.Type == TokType::BraceCurly_Close && level > 0 )
				level--;

			eat( currtok.Type );
		}

		Token previous = prevtok;

		s32 len = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)start.Text;

		if ( len > 0 )
		{
			result.append( def_execution( { len, start.Text } ) );
		}

		eat( TokType::BraceCurly_Close );
		return result;
	}

	internal
	CodeBody parse_global_nspace( CodeT which, Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		if ( which != Namespace_Body && which != Global_Body && which != Export_Body && which != Extern_Linkage_Body )
			return CodeInvalid;

		if ( which != Global_Body )
			eat( TokType::BraceCurly_Open );

		CodeBody
		result = (CodeBody) make_code();
		result->Type = which;

		while ( left && currtok.Type != TokType::BraceCurly_Close )
		{
			Code           member     = Code::Invalid;
			CodeAttributes attributes = { nullptr };
			CodeSpecifier  specifiers = { nullptr };

			bool expects_function = false;

			switch ( currtok.Type )
			{
				case TokType::Comment:
					member = def_comment( currtok );
					eat( TokType::Comment );
				break;

				case TokType::Decl_Enum:
					member = parse_enum( toks, context);
				break;

				case TokType::Decl_Class:
					member = parse_class( toks, context );
				break;

				case TokType::Decl_Extern_Linkage:
					if ( which == Extern_Linkage_Body )
						log_failure( "gen::parse_global_nspace: nested extern linkage" );

					member = parse_extern_link_body( toks, context );
				break;

				case TokType::Decl_Namespace:
					member = parse_namespace( toks, context );
				break;

				case TokType::Decl_Struct:
					member = parse_struct( toks, context );
				break;

				case TokType::Decl_Template:
					member = parse_template( toks, context );
				break;

				case TokType::Decl_Typedef:
					member = parse_typedef( toks, context );
				break;

				case TokType::Decl_Union:
					member = parse_union( toks, context );
				break;

				case TokType::Decl_Using:
					member = parse_using( toks, context );
				break;

				case TokType::Module_Export:
					if ( which == Export_Body )
						log_failure( "gen::parse_global_nspace: nested export declaration" );

					member = parse_export_body( toks, context );
				break;

				case TokType::Module_Import:
				{
					not_implemented();
				}
				//! Fallthrough intentional
				case TokType::BraceSquare_Open:
				case TokType::Attr_Keyword:
				{
					not_implemented();

					// Standard attribute
					if ( currtok.Type == TokType::BraceSquare_Open  )
					{
						eat( TokType::BraceSquare_Open );

						if ( currtok.Type != TokType::BraceSquare_Open )
						{
							log_failure( "%s: Error, expected attribute name", context );
							return result;
						}

						while ( left && currtok.Type != TokType::BraceSquare_Close )
						{
							// TODO : Parse attributes
						}

						eat( TokType::BraceSquare_Close );
						eat( TokType::BraceSquare_Close );
					}

					// Platform Specific attribute
					eat( TokType::Attr_Keyword );
					eat( TokType::Capture_Start );

					// TODO : Parse attributes

					eat( TokType::Capture_End );
				}
				//! Fallthrough intentional
				case TokType::Spec_Consteval:
				case TokType::Spec_Constexpr:
				case TokType::Spec_Constinit:
				case TokType::Spec_Extern:
				case TokType::Spec_Global:
				case TokType::Spec_Inline:
				case TokType::Spec_Internal_Linkage:
				case TokType::Spec_Static:
				{
					SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
					s32        num_specifiers = 0;

					while ( left && tok_is_specifier( currtok ) )
					{
						SpecifierT spec = ESpecifier::to_type( currtok );

						switch ( spec )
						{
							case ESpecifier::Constexpr:
							case ESpecifier::Constinit:
							case ESpecifier::Inline:
							case ESpecifier::Mutable:
							case ESpecifier::Static:
							case ESpecifier::Volatile:
							break;

							case ESpecifier::Consteval:
								expects_function = true;
							break;

							default:
								log_failure( "gen::parse_global_nspace: invalid specifier " "%s" " for variable", ESpecifier::to_str(spec) );
								return CodeInvalid;
						}

						specs_found[num_specifiers] = spec;
						num_specifiers++;
						eat( currtok.Type );
					}

					if ( num_specifiers )
					{
						specifiers = def_specifiers( num_specifiers, specs_found );
					}
				}
				//! Fallthrough intentional
				case TokType::Identifier:
				case TokType::Spec_Const:
				case TokType::Type_Long:
				case TokType::Type_Short:
				case TokType::Type_Signed:
				case TokType::Type_Unsigned:
				{
					member = parse_operator_function_or_variable( expects_function, attributes, specifiers, toks, context );
				}
			}

			if ( member == Code::Invalid )
			{
				log_failure( "gen::%s: failed to parse extern linkage member", context );
				return CodeInvalid;
			}

			result.append( member );
		}

		if ( which != Global_Body )
			eat( TokType::BraceCurly_Close );

		return result;
	}

	internal
	CodeClass parse_class( Parser::TokArray& toks, char const* context )
	{
		return (CodeClass) parse_class_struct( Parser::TokType::Decl_Class, toks, context );
	}

	CodeClass parse_class( StrC def )
	{
		check_parse_args( parse_class, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return (CodeClass) parse_class_struct( TokType::Decl_Class, toks, stringize(parse_class) );
	}

	internal
	CodeEnum parse_enum( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Token    name       = { nullptr, 0, TokType::Invalid };
		Code     array_expr = { nullptr };
		CodeType type       = { nullptr };
		Token    body       = { nullptr, 0, TokType::Invalid };

		char  entries_code[ kilobytes(128) ] { 0 };
		s32   entries_length = 0;

		bool is_enum_class = false;

		eat( TokType::Decl_Enum );

		if ( currtok.Type == TokType::Decl_Class )
		{
			eat( TokType::Decl_Class);
			is_enum_class = true;
		}

		// TODO : Parse attributes

		if ( currtok.Type != TokType::Identifier )
		{
			log_failure( "gen::parse_enum: expected identifier for enum name" );
			return CodeInvalid;
		}

		name = currtok;
		eat( TokType::Identifier );

		if ( currtok.Type == TokType::Assign_Classifer )
		{
			eat( TokType::Assign_Classifer );

			type = parse_type( toks, stringize(parse_enum) );
			if ( type == Code::Invalid )
				return CodeInvalid;
		}

		if ( currtok.Type == TokType::BraceCurly_Open )
		{
			eat( TokType::BraceCurly_Open );

			body = currtok;

			while ( currtok.Type != TokType::BraceCurly_Close )
			{
				eat( TokType::Identifier);

				if ( currtok.Type == TokType::Operator && currtok.Text[0] == '=' )
				{
					eat( TokType::Operator );

					while ( currtok.Type != TokType::Comma && currtok.Type != TokType::BraceCurly_Close )
					{
						eat( currtok.Type );
					}
				}

				if ( currtok.Type == TokType::Comma )
				{
					eat( TokType::Comma );
				}
			}

			body.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)body.Text;

			eat( TokType::BraceCurly_Close );
		}

		eat( TokType::Statement_End );

		using namespace ECode;

		CodeEnum
		result = (CodeEnum) make_code();

		if ( body.Length )
		{
			// mem_copy( entries_code, body.Text, body.Length );

			Code untyped_body = untyped_str( body );

			result->Type = is_enum_class ? Enum_Class : Enum;
			result->Body = untyped_body;
		}
		else
		{
			result->Type = is_enum_class ? Enum_Class_Fwd : Enum_Fwd;
		}

		result->Name = get_cached_string( name );

		if ( type )
			result->UnderlyingType = type;

		return result;
	}

	CodeEnum parse_enum( StrC def )
	{
		check_parse_args( parse_enum, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return parse_enum( toks, stringize(parse_enum) );
	}

	internal inline
	CodeBody parse_export_body( Parser::TokArray& toks, char const* context )
	{
		return parse_global_nspace( ECode::Export_Body, toks, context );
	}

	CodeBody parse_export_body( StrC def )
	{
		check_parse_args( parse_export_body, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return parse_export_body( toks, stringize(parse_export_body) );
	}

	internal inline
	CodeBody parse_extern_link_body( Parser::TokArray& toks, char const* context )
	{
		return parse_global_nspace( ECode::Extern_Linkage_Body, toks, context );
	}

	internal
	CodeExtern parse_extern_link( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		eat( TokType::Decl_Extern_Linkage );

		Token name = currtok;
		eat( TokType::String );

		name.Text   += 1;
		name.Length -= 1;

		CodeExtern
		result       = (CodeExtern) make_code();
		result->Type = ECode::Extern_Linkage;
		result->Name = get_cached_string( name );

		Code entry = parse_extern_link_body( toks, context );
		if ( entry == Code::Invalid )
		{
			log_failure( "gen::parse_extern_link: failed to parse body" );
			return result;
		}

		result->Body = entry;

		return result;
	}

	CodeExtern parse_extern_link( StrC def )
	{
		check_parse_args( parse_extern_link, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return parse_extern_link( toks, stringize(parse_extern_link) );
	}

	internal
	CodeFriend parse_friend( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		eat( TokType::Decl_Friend );

		CodeFn function = { nullptr };

		// Type declaration or return type
		CodeType type = parse_type( toks, stringize(parse_friend) );
		if ( type == Code::Invalid )
			return CodeInvalid;

		// Funciton declaration
		if ( currtok.Type == TokType::Identifier )
		{
			// Name
			Token name = parse_identifier( toks, stringize(parse_friend) );

			// Parameter list
			CodeParam params = parse_params( toks, stringize(parse_friend) );

			function             = make_code();
			function->Type       = Function_Fwd;
			function->Name       = get_cached_string( name );
			function->ReturnType = type;

			if ( params )
				function->Params = params;
		}

		eat( TokType::Statement_End );

		CodeFriend
		result       = (CodeFriend) make_code();
		result->Type = Friend;

		if ( function )
			result->Declaration = function;

		else
			result->Declaration = type;

		return result;
	}

	CodeFriend parse_friend( StrC def )
	{
		check_parse_args( parse_friend, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return parse_friend( toks, stringize(parse_friend) );
	}

	internal
	CodeFn parse_functon( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		CodeAttributes attributes = { nullptr };
		CodeSpecifier  specifiers = { nullptr };

		// TODO : Parse module specifiers

		// TODO : Parse attributes

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			switch ( spec )
			{
				case ESpecifier::Const:
				case ESpecifier::Consteval:
				case ESpecifier::Constexpr:
				case ESpecifier::External_Linkage:
				case ESpecifier::Inline:
				case ESpecifier::Static:
				break;

				default:
					log_failure( "gen::parse_functon: invalid specifier " "%s" " for functon", ESpecifier::to_str(spec) );
					return CodeInvalid;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( num_specifiers )
		{
			specifiers = def_specifiers( num_specifiers, specs_found );
		}

		CodeType ret_type = parse_type( toks, stringize(parse_function) );
		if ( ret_type == Code::Invalid )
			return CodeInvalid;

		Token name = parse_identifier( toks, stringize(parse_function) );
		if ( ! name )
			return CodeInvalid;

		CodeFn result = parse_function_after_name( ModuleFlag::None, attributes, specifiers, ret_type, name, toks, context );

		return result;
	}

	CodeFn parse_function( StrC def )
	{
		using namespace Parser;

		check_parse_args( parse_function, def );

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return (CodeFn) parse_functon( toks, stringize(parse_function) );
	}

	CodeBody parse_global_body( StrC def )
	{
		check_parse_args( parse_global_body, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return parse_global_nspace( ECode::Global_Body, toks, stringize(parse_global_body) );
	}

	internal
	CodeNamespace parse_namespace( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		eat( TokType::Decl_Namespace );

		Token name = parse_identifier( toks, stringize(parse_namespace) );

		CodeBody body = parse_global_nspace( ECode::Namespace_Body, toks, stringize(parse_namespace) );
		if ( body == Code::Invalid )
			return CodeInvalid;

		CodeNamespace
		result       = (CodeNamespace) make_code();
		result->Type = ECode::Namespace;
		result->Name = get_cached_string( name );

		result->Body = body;

		return result;
	}

	CodeNamespace parse_namespace( StrC def )
	{
		check_parse_args( parse_namespace, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return parse_namespace( toks, stringize(parse_namespace) );
	}

	internal
	CodeOperator parse_operator( Parser::TokArray& toks, char const* context )
	{
		// TODO : Parse Module specifier

		// TODO : Parse Attributes
		CodeAttributes attributes = { nullptr };

		CodeSpecifier specifiers = { nullptr };

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			switch ( spec )
			{
				case ESpecifier::Const:
				case ESpecifier::Constexpr:
				case ESpecifier::Inline:
				case ESpecifier::Static:
				break;

				default:
					log_failure( "gen::parse_operator: invalid specifier " "%s" " for operator", ESpecifier::to_str(spec) );
					return CodeInvalid;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( num_specifiers )
		{
			specifiers = def_specifiers( num_specifiers, specs_found );
		}

		// Parse Return Type
		CodeType ret_type = parse_type( toks, stringize(parse_operator) );

		CodeOperator result = parse_operator_after_ret_type( ModuleFlag::None, attributes, specifiers, ret_type, toks, context );
		return result;
	}

	CodeOperator parse_operator( StrC def )
	{
		check_parse_args( parse_operator, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return (CodeOperator) parse_operator( toks, stringize(parse_operator) );
	}

	CodeOpCast parse_operator_cast( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		eat( TokType::Decl_Operator );

		Code type = parse_type( toks, stringize(parse_operator_cast) );

		eat( TokType::Capture_Start );
		eat( TokType::Capture_End );

		Code body = { nullptr };

		if ( check( TokType::BraceCurly_Open) )
		{
			eat( TokType::BraceCurly_Open );

			Token body_str = currtok;

			s32 level = 0;
			while ( left && ( currtok.Type != TokType::BraceCurly_Close || level > 0 ) )
			{
				if ( currtok.Type == TokType::BraceCurly_Open )
					level++;

				else if ( currtok.Type == TokType::BraceCurly_Close && level > 0 )
					level--;

				eat( currtok.Type );
			}

			body_str.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)body_str.Text;

			body = untyped_str( body_str );

			eat( TokType::BraceCurly_Close );
		}

		CodeOpCast result = (CodeOpCast) make_code();

		if (body)
		{
			result->Type = ECode::Operator_Cast;
			result->Body = body;
		}
		else
		{
			result->Type = ECode::Operator_Cast_Fwd;
		}

		result->ValueType = type;

		return result;
	}

	CodeOpCast parse_operator_cast( StrC def )
	{
		check_parse_args( parse_operator_cast, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return parse_operator_cast( toks, stringize(parse_operator_cast) );
	}

	internal inline
	CodeStruct parse_struct( Parser::TokArray& toks, char const* context )
	{
		return (CodeStruct) parse_class_struct( Parser::TokType::Decl_Struct, toks, stringize(parse_struct) );
	}

	CodeStruct parse_struct( StrC def )
	{
		check_parse_args( parse_struct, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return (CodeStruct) parse_class_struct( TokType::Decl_Struct, toks, stringize(parse_struct) );
	}

	internal
	CodeTemplate parse_template( Parser::TokArray& toks, char const* context )
	{
	#	define UseTemplateCapture true

		using namespace Parser;

		// TODO : Parse Module specifier

		eat( TokType::Decl_Template );

		Code params = parse_params( toks, stringize(parse_template), UseTemplateCapture );
		if ( params == Code::Invalid )
			return CodeInvalid;

		Code definition = { nullptr };

		while ( left )
		{
			if ( check( TokType::Decl_Class ) )
			{
				definition = parse_class( toks, stringize(parse_template) );
				break;
			}

			if ( check( TokType::Decl_Struct ) )
			{
				definition = parse_enum( toks, stringize(parse_template) );
				break;
			}

			if ( check( TokType::Decl_Using ))
			{
				definition = parse_using( toks, stringize(parse_template) );
				break;
			}

			// Its either a function or a variable
			Token name = { nullptr, 0, TokType::Invalid };


			CodeAttributes attributes   = { nullptr };
			CodeSpecifier  specifiers   = { nullptr };

			// TODO : Parse attributes

			bool expects_function = false;

			SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
			s32        num_specifiers = 0;

			while ( left && tok_is_specifier( currtok ) )
			{
				SpecifierT spec = ESpecifier::to_type( currtok );

				switch ( spec )
				{
					case ESpecifier::Const:
					case ESpecifier::Constexpr:
					case ESpecifier::Constinit:
					case ESpecifier::External_Linkage:
					case ESpecifier::Global:
					case ESpecifier::Inline:
					case ESpecifier::Local_Persist:
					case ESpecifier::Mutable:
					case ESpecifier::Static:
					case ESpecifier::Thread_Local:
					case ESpecifier::Volatile:
					break;

					case ESpecifier::Consteval:
						expects_function = true;
					break;

					default:
						log_failure( "gen::parse_template: invalid specifier %s for variable or function", ESpecifier::to_str( spec ) );
						return CodeInvalid;
				}

				// Ignore const it will be handled by the type
				if ( spec == ESpecifier::Const )
					continue;

				specs_found[num_specifiers] = spec;
				num_specifiers++;
				eat( currtok.Type );
			}

			if ( num_specifiers )
			{
				specifiers = def_specifiers( num_specifiers, specs_found );
			}

			definition = parse_operator_function_or_variable( expects_function, attributes, specifiers, toks, stringize(parse_template) );
			break;
		}

		CodeTemplate
		result               = (CodeTemplate) make_code();
		result->Type        = ECode::Template;
		result->Params      = params;
		result->Declaration = definition;

		return result;
	#	undef UseTemplateCapture
	}

	CodeTemplate parse_template( StrC def )
	{
		check_parse_args( parse_template, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return parse_template( toks, stringize(parse_template) );
	}

	internal
	CodeType parse_type( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Token context_tok = prevtok;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Token name      = { nullptr, 0, TokType::Invalid };
		Token brute_sig = { currtok.Text, 0, TokType::Invalid };

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			if ( spec != ESpecifier::Const )
			{
				log_failure( "gen::parse_type: Error, invalid specifier used in type definition: %s", currtok.Text );
				return CodeInvalid;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( left == 0 )
		{
			log_failure( "%s: Error, unexpected end of type definition", context );
			return CodeInvalid;
		}

		if (   currtok.Type == TokType::Decl_Class
			|| currtok.Type == TokType::Decl_Enum
			|| currtok.Type == TokType::Decl_Struct )
		{
			name = currtok;
			eat( currtok.Type );

			name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
			eat( TokType::Identifier );
		}
		else if ( currtok.Type >= TokType::Type_Unsigned )
		{
			name = currtok;
			eat( currtok.Type );

			while (currtok.Type >= TokType::Type_Unsigned)
			{
				eat( currtok.Type );
			}

			name.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)name.Text;
		}
		else
		{
			name = parse_identifier( toks, context );
			if ( ! name )
				return CodeInvalid;

			// Problably dealing with a templated symbol
			if ( currtok.Type == TokType::Operator && currtok.Text[0] == '<' && currtok.Length == 1 )
			{
				eat( TokType::Operator );

				s32 level = 0;
				while ( left && ( currtok.Text[0] != '>' || level > 0 ))
				{
					if ( currtok.Text[0] == '<' )
						level++;

					if ( currtok.Text[0] == '>' )
						level--;

					eat( currtok.Type );
				}

				eat( TokType::Operator );

				// Extend length of name to last token
				name.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)name.Text;
			}
		}

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			if (   spec != ESpecifier::Const
				&& spec != ESpecifier::Ptr
				&& spec != ESpecifier::Ref
				&& spec != ESpecifier::RValue )
			{
				log_failure( "%s: Error, invalid specifier used in type definition: %s", context, currtok.Text );
				return CodeInvalid;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		// Not sure if its technically possible to cast ot a function pointer user defined operator cast...
		// Supporting it is not worth the effort.
		if ( check( TokType::Capture_Start ) && context_tok.Type != TokType::Decl_Operator )
		{
			// Its a function type
			eat( TokType::Capture_Start );

			while ( check( TokType::Star ) || currtok.Type == TokType::Spec_Const )
			{
				eat( currtok.Type );
			}

			// if its a using statement there will not be an ID.
			if ( check( TokType::Identifier ) )
				eat(TokType::Identifier);

			eat( TokType::Capture_End );

			// Parameters

			eat( TokType::Capture_Start );

			// TODO : Change this to validate the parameters...
			// Bruteforce lex the parameters, no validation.
			s32 level = 0;
			while ( ! check( TokType::Capture_End ) || level > 0 )
			{
				if ( check( TokType::Capture_Start ) )
					level++;

				if ( check( TokType::Capture_End ) )
					level--;

				eat( currtok.Type );
			}

			eat(TokType::Capture_End);

			brute_sig.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)brute_sig.Text;
		}

		using namespace ECode;

		CodeType
		result       = (CodeType) make_code();
		result->Type = Typename;

		if ( brute_sig.Length > 0 )
		{
			// Bruteforce all tokens together.
			name = brute_sig;
		}
		else
		{
			if (num_specifiers)
			{
				Code specifiers = def_specifiers( num_specifiers, (SpecifierT*)specs_found );
				result->Specs = specifiers;
			}
		}

		result->Name = get_cached_string( name );

		return result;
	}

	CodeType parse_type( StrC def )
	{
		check_parse_args( parse_type, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		CodeType result = parse_type( toks, stringize(parse_type) );

		return result;
	}

	internal
	CodeTypedef parse_typedef( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Token name       = { nullptr, 0, TokType::Invalid };
		Code  array_expr = { nullptr };
		Code  type       = { nullptr };

		eat( TokType::Decl_Typedef );

		if ( check( TokType::Decl_Enum ) )
			type = parse_enum( toks, context );

		else if ( check(TokType::Decl_Struct ) )
			type = parse_enum( toks, context );

		else if ( check(TokType::Decl_Union) )
			type = parse_union( toks, context );

		else
			type = parse_type( toks, context );

		if ( ! check( TokType::Identifier ) )
		{
			log_failure( "gen::parse_typedef: Error, expected identifier for typedef" );
			return CodeInvalid;
		}

		name = currtok;
		eat( TokType::Identifier );

		array_expr = parse_array_decl( toks, context );

		eat( TokType::Statement_End );

		using namespace ECode;

		CodeTypedef
		result       = (CodeTypedef) make_code();
		result->Type = Typedef;
		result->Name = get_cached_string( name );

		result->UnderlyingType = type;

		if ( type->Type == Typename && array_expr && array_expr->Type != Invalid )
			type.cast<CodeType>()->ArrExpr = array_expr;

		return result;
	}

	CodeTypedef parse_typedef( StrC def )
	{
		check_parse_args( parse_typedef, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return parse_typedef( toks, stringize(parse_typedef) );
	}

	internal
	CodeUnion parse_union( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		// TODO : Parse module spec

		eat( TokType::Decl_Union );

		// TODO : Parse attributes
		CodeAttributes attributes = { nullptr };

		StrC name = { 0, nullptr };

		if ( check( TokType::Identifier ) )
		{
			name = currtok;
			eat( TokType::Identifier );
		}

		CodeBody body = { nullptr };

		eat( TokType::BraceCurly_Open );

		body = make_code();
		body->Type = ECode::Union_Body;

		while ( ! check( TokType::BraceCurly_Close ) )
		{
			Code entry = parse_variable( toks, context );

			if ( entry )
				body.append( entry );
		}

		eat( TokType::BraceCurly_Close );
		eat( TokType::Statement_End );

		CodeUnion
		result = (CodeUnion) make_code();
		result->Type = ECode::Union;

		if ( name )
			result->Name = get_cached_string( name );

		if ( body )
			result->Body = body;

		if ( attributes )
			result->Attributes = attributes;

		return result;
	}

	CodeUnion parse_union( StrC def )
	{
		check_parse_args( parse_union, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return parse_union( toks, stringize(parse_union) );
	}

	internal
	CodeUsing parse_using( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Token    name       = { nullptr, 0, TokType::Invalid };
		Code     array_expr = { nullptr };
		CodeType type       = { nullptr };

		bool is_namespace = false;

		// TODO : Parse module specs

		eat( TokType::Decl_Using );

		if ( currtok.Type == TokType::Decl_Namespace )
		{
			is_namespace = true;
			eat( TokType::Decl_Namespace );
		}

		name = currtok;
		eat( TokType::Identifier );

		if ( currtok.IsAssign )
		{
			// TODO : Parse Attributes (using type-alias)

			eat( TokType::Operator );

			type = parse_type( toks, context );
		}

		array_expr = parse_array_decl( toks, context );

		eat( TokType::Statement_End );

		using namespace ECode;

		CodeUsing
		result       = (CodeUsing) make_code();
		result->Name = get_cached_string( name );

		if ( is_namespace)
		{
			result->Type = Using_Namespace;
		}
		else
		{
			result->Type = Using;

			if ( type )
				result->UnderlyingType = type;

			if ( array_expr )
				type->ArrExpr = array_expr;
		}
		return result;
	}

	CodeUsing parse_using( StrC def )
	{
		check_parse_args( parse_using, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return parse_using( toks, stringize(parse_using) );
	}

	internal
	CodeVar parse_variable( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Token name = { nullptr, 0, TokType::Invalid };

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		CodeAttributes attributes = { nullptr };
		CodeSpecifier  specifiers = { nullptr };

		// TODO : Parse module specifiers

		// TODO : Parse attributes

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			switch ( spec )
			{
				case ESpecifier::Const:
				case ESpecifier::Constexpr:
				case ESpecifier::Constinit:
				case ESpecifier::External_Linkage:
				case ESpecifier::Global:
				case ESpecifier::Inline:
				case ESpecifier::Local_Persist:
				case ESpecifier::Mutable:
				case ESpecifier::Static:
				case ESpecifier::Thread_Local:
				case ESpecifier::Volatile:
				break;

				default:
					log_failure( "gen::parse_variable: invalid specifier %s for variable", ESpecifier::to_str( spec ) );
					return CodeInvalid;
			}

			// Ignore const specifiers, they're handled by the type
			if ( spec == ESpecifier::Const )
				continue;

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( num_specifiers )
		{
			specifiers = def_specifiers( num_specifiers, specs_found );
		}

		CodeType type = parse_type( toks, context );

		if ( type == Code::Invalid )
			return CodeInvalid;

		name = currtok;
		eat( TokType::Identifier );

		CodeVar result = parse_variable_after_name( ModuleFlag::None, attributes, specifiers, type, name, toks, context );

		return result;
	}

	CodeVar parse_variable( StrC def )
	{
		check_parse_args( parse_variable, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return CodeInvalid;

		return parse_variable( toks, stringize(parse_variable) );
	}

	// Undef helper macros
#	undef check_parse_args
#	undef curr_tok
#	undef eat
#	undef left
//	End GEN_FEATURE_PARSING
#endif
#pragma endregion Parsing Constructors

#pragma region Untyped Constructors
	sw token_fmt_va( char* buf, uw buf_size, s32 num_tokens, va_list va )
	{
		char const* buf_begin = buf;
		sw          remaining = buf_size;

		static Arena tok_map_arena;

		HashTable<StrC> tok_map;
		{
			static char tok_map_mem[ TokenFmt_TokenMap_MemSize ];

			tok_map_arena = Arena::init_from_memory( tok_map_mem, sizeof(tok_map_mem) );

			tok_map = HashTable<StrC>::init( tok_map_arena );

			s32 left = num_tokens - 1;

			while ( left-- )
			{
				char const* token = va_arg( va, char const* );
				StrC        value = va_arg( va, StrC );

				u32 key = crc32( token, str_len(token) );

				tok_map.set( key, value );
			}
		}

		char const* fmt     = va_arg( va, char const* );
		char        current = *fmt;

		while ( current )
		{
			sw len = 0;

			while ( current && current != '<' && remaining )
			{
				* buf = * fmt;
				buf++;
				fmt++;
				remaining--;

				current = * fmt;
			}

			if ( current == '<' )
			{
				char const* scanner = fmt + 1;

				s32 tok_len = 0;

				while ( *scanner != '>' )
				{
					tok_len++;
					scanner++;
				}

				char const* token = fmt + 1;

				u32       key   = crc32( token, tok_len );
				StrC*     value = tok_map.get( key );

				if ( value )
				{
					sw          left = value->Len;
					char const* str  = value->Ptr;

					while ( left-- )
					{
						* buf = * str;
						buf++;
						str++;
						remaining--;
					}

					scanner++;
					fmt     = scanner;
					current = * fmt;
					continue;
				}

				* buf = * fmt;
				buf++;
				fmt++;
				remaining--;

				current = * fmt;
			}
		}

		tok_map.clear();
		tok_map_arena.free();

		sw result = buf_size - remaining;

		return result;
	}

	Code untyped_str( StrC content )
	{
		Code
		result          = make_code();
		result->Name    = get_cached_string( content );
		result->Type    = ECode::Untyped;
		result->Content = result->Name;

		return result;
	}

	Code untyped_fmt( char const* fmt, ...)
	{
		local_persist thread_local
		char buf[GEN_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		sw length = str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va);
		va_end(va);

		Code
		result          = make_code();
		result->Name    = get_cached_string( { str_len(fmt, MaxNameLength), fmt } );
		result->Type    = ECode::Untyped;
		result->Content = get_cached_string( { length, buf } );

		return result;
	}

	Code untyped_token_fmt( s32 num_tokens, ... )
	{
		local_persist thread_local
		char buf[GEN_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, num_tokens);
		sw length = token_fmt_va(buf, GEN_PRINTF_MAXLEN, num_tokens, va);
		va_end(va);

		Code
		result          = make_code();
		result->Name    = get_cached_string( { length, buf } );
		result->Type    = ECode::Untyped;
		result->Content = result->Name;

		return result;
	}
#pragma endregion Untyped Constructors
#pragma endregion Gen Interface

#pragma region Builder
	void Builder::print( Code code )
	{
		Buffer.append_fmt( "%s\n", code->to_string() );
	}

	void Builder::print_fmt( char const* fmt, ... )
	{
		sw   res;
		char buf[ GEN_PRINTF_MAXLEN ] = { 0 };

		va_list va;
		va_start( va, fmt );
		res = str_fmt_va( buf, count_of( buf ) - 1, fmt, va ) - 1;
		va_end( va );

		Buffer.append( buf, res );
	}

	bool Builder::open( char const* path )
	{
		FileError error = file_open_mode( & File, EFileMode_WRITE, path );

		if ( error != EFileError_NONE )
		{
			log_failure( "gen::File::open - Could not open file: %s", path);
			return false;
		}

		Buffer = String::make_reserve( GlobalAllocator, Builder_StrBufferReserve );

		return true;
	}

	void Builder::write()
	{
		bool result = file_write( & File, Buffer, Buffer.length() );

		if ( result == false )
			log_failure("gen::File::write - Failed to write to file: %s", file_name( & File ) );

		file_close( & File );
		Buffer.free();
	}
#pragma endregion Builder
}

#include "gen.pop_ignores.inline.hpp"
