// ReSharper disable CppClangTidyClangDiagnosticSwitchEnum
#include "Bloat.hpp"
#include "gen.hpp"

#ifdef gen_time
namespace gen
{
	ZPL_TABLE_DEFINE( StringTable, str_tbl_,   String );
	// ZPL_TABLE_DEFINE( TypeTable,   type_tbl_ , Code   );

	namespace StaticData
	{
		static Array(Pool)  CodePools         = nullptr;
		static Array(Arena) CodeEntriesArenas = nullptr;
		static Array(Arena) StringArenas      =  nullptr;

		static StringTable StringMap;
		// static TypeTable   TypeMap;

		static AllocatorInfo Allocator_DataArrays       = heap();
		static AllocatorInfo Allocator_CodePool         = heap();
		static AllocatorInfo Allocator_CodeEntriesArena = heap();
		static AllocatorInfo Allocator_StringArena      = heap();
		static AllocatorInfo Allocator_StringTable      = heap();
		static AllocatorInfo Allocator_TypeTable        = heap();
	}

#pragma region Constants
	#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
	Code type_ns(void);
	Code type_ns(int);
	Code type_ns(bool);
	Code type_ns(char);
	Code type_ns(wchar_t);

	Code type_ns(s8);
	Code type_ns(s16);
	Code type_ns(s32);
	Code type_ns(s64);

	Code type_ns(u8);
	Code type_ns(u16);
	Code type_ns(u32);
	Code type_ns(u64);

	Code type_ns(sw);
	Code type_ns(uw);

	Code type_ns(f32);
	Code type_ns(f64);
	#endif

	Code access_public;
	Code access_protected;
	Code access_private;

	Code module_global_fragment;
	Code module_private_fragment;

	Code pragma_once;

	Code spec_const;
	Code spec_consteval;
	Code spec_constexpr;
	Code spec_constinit;
	Code spec_extern_linkage;
	Code spec_inline;
	Code spec_internal_linkage;
	Code spec_local_persist;
	Code spec_mutable;
	Code spec_ptr;
	Code spec_ref;
	Code spec_register;
	Code spec_rvalue;
	Code spec_static_member;
	Code spec_thread_local;
	Code spec_volatile;
	Code spec_type_signed;
	Code spec_type_unsigned;
	Code spec_type_short;
	Code spec_type_long;
#pragma endregion Constants

#pragma region AST Body Case Macros
#	define AST_BODY_CLASS_UNALLOWED_TYPES \
	case Attributes:                      \
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
	case Attributes:                         \
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
	case Attributes:                       \
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
	Code Code::Invalid;

	bool AST::add( AST* other )
	{
	#ifdef GEN_FEATURE_INCREMENTAL
		if ( other == nullptr )
		{
			log_failure( "AST::add: Provided a null AST" );
			return false;
		}

		if ( other->Type == ECode::Invalid )
		{
			log_failure( "AST::add: Provided an invalid AST" );
			return false;
		}

		switch ( Type )
		{
			using namespace ECode;

			case Invalid:
				log_failure( "AST::add: Cannot add an AST to an invalid AST." );
				return false;

			case Untyped:
				log_failure( "AST::add: Cannot add an AST to an untyped AST." );
				return false;

			case Comment:
				log_failure( "AST::add: Cannot add an AST to a comment." );
				return false;

			case Access_Private:
				log_failure( "AST::add: Cannot add an AST to a private access specifier." );
				return false;

			case Access_Protected:
				log_failure( "AST::add: Cannot add an AST to a protected access specifier." );
				return false;

			case Access_Public:
				log_failure( "AST::add: Cannot add an AST to a public access specifier." );
				return false;

			case Attributes:
				log_failure( "AST::add: Cannot add an AST to an attribute." );
				return false;

			case Class:
				log_failure( "AST::add: Cannot add an AST to a class, only to its body" );
				return false;

			case Class_Fwd:
				log_failure( "AST::add: Cannot add an AST to a class forward declaration." );
				return false;

			case Class_Body:
				switch ( other->Type )
				{
					AST_BODY_CLASS_UNALLOWED_TYPES
					{
						log_failure( "AST::add: Cannot add %s to a class body.", other->type_str() );
						return false;
					}

					default:
						break;
				}
			break;

			case Enum:
				log_failure( "AST::add: Cannot add an AST to an enum, only to its body" );
				return false;

			case Enum_Fwd:
				log_failure( "AST::add: Cannot add an AST to an enum forward declaration." );
				return false;

			case Enum_Body:
				if ( other->Type != Untyped )
				{
					log_failure( "AST::add: Cannot add an AST which is not untyped to an enum body." );
					return false;
				}
			break;

			case Execution:
				log_failure( "AST::add: Cannot add an AST to an execution block." );
				return false;
			break;

			case Export_Body:
				switch ( other->Type )
				{
					AST_BODY_EXPORT_UNALLOWED_TYPES
					{
						log_failure( "AST::add: Cannot add %s to an export body.", other->type_str() );
						return false;
					}

					default:
						break;
				}
			break;

			case Extern_Linkage:
				log_failure( "AST::add: Cannot add an AST to an extern linkage, only to its body." );
				return false;

			case Extern_Linkage_Body:
				switch ( other->Type )
				{
					AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES
					{
						log_failure( "AST::add: Cannot add %s to an extern linkage body.", other->type_str() );
						return false;
					}

					default:
						break;
				}

			case Enum_Class:
				log_failure( "AST::add: Cannot add an AST to an enum class, only to its body" );
				return false;

			case Enum_Class_Fwd:
				log_failure( "AST::add: Cannot add an AST to an enum class forward declaration." );
				return false;

			case Friend:
				log_failure( "AST::add: Cannot add an AST to a friend declaration." );
				return false;

			case Function:
				log_failure( "AST::add: Cannot add an AST to a function, only to its body" );
				return false;

			case Function_Body:
				switch ( other->Type )
				{
					AST_BODY_FUNCTION_UNALLOWED_TYPES
					{
						log_failure( "AST::add: Cannot add %s to a function body.", other->type_str() );
						return false;
					}

					default:
						break;
				}
			break;

			case Function_Fwd:
				log_failure( "AST::add: Cannot add an AST to a function forward declaration." );
				return false;

			case Global_Body:
				switch ( other->Type )
				{
					AST_BODY_GLOBAL_UNALLOWED_TYPES
					{
						log_failure( "AST::add: Cannot add %s to a global body.", other->type_str() );
						return false;
					}

					default:
						break;
				}
			break;

			case Module:
				log_failure( "AST::add: Cannot add an AST to a module, only to its body" );
				return false;

			case Namespace:
				if ( Type != Global_Body )
				{
					log_failure( "AST::add: Cannot add a namespace to a non-global body." );
					return false;
				}

			case Namespace_Body:
				switch ( other-> Type )
				{
					AST_BODY_NAMESPACE_UNALLOWED_TYPES
					{
						log_failure( "AST::add: Cannot add %s to a namespace body.", other->type_str() );
						return false;
					}

					default:
						break;
				}
			break;

			case Operator:
				log_failure( "AST::add: Cannot add an operator, only to its body" );
				return false;

			case Operator_Fwd:
				log_failure( "AST::add: Cannot add an operator forward declaration." );
				return false;

			case Parameters:
				log_failure( "AST::add: Cannot add to a parameter list, use AST::add_param instead" );
				return false;

			case Preprocessor_Include:
				log_failure( "AST::add: Cannot add an AST to a preprocessor include." );
				return false;

			case Specifiers:
				log_failure( "AST::add: Cannot add to a specifier, use AST::add_specifier instead." );
				return false;

			case Struct:
				log_failure( "AST::add: Cannot add to a struct, only to its body." );
				return false;

			case Struct_Body:
				switch ( other->Type )
				{
					AST_BODY_STRUCT_UNALLOWED_TYPES
					{
						log_failure( "AST::add: Cannot add %s to a struct body.", other->type_str() );
						return false;
					}

					default:
						break;
				}
			break;

			case Typedef:
				log_failure( "AST::add: Cannot add to a typedef." );
				return false;

			case Typename:
				log_failure( "AST::add: Cannot add to a typename." );
				return false;

			case Union:
				log_failure( "AST::add: Cannot add to a union, only to its body." );
				return false;

			case Union_Body:
				if ( other->Type != Untyped )
				{
					log_failure( "AST::add: Cannot add an AST which is not untyped to a union body." );
					return false;
				}

			case Using:
				log_failure( "AST::add: Cannot add to a using statement." );
				return false;

			case Using_Namespace:
				log_failure( "AST::add: Cannot add to a using namespace statement." );
				return false;

			case Variable:
				log_failure( "AST::add: Cannot add to a variable." );
				return false;
		}

		add_entry( other );
		return true;
	#else
		log_failure( "AST::add: Incremental AST building is not enabled." );
		return false;
	#endif
	}

	AST* AST::duplicate()
	{
		using namespace ECode;

		Code
		result           = make_code();
		result->Parent   = Parent;
		result->Name     = Name;
		result->Type     = Type;
		result->Op       = Op;
		result->Readonly = Readonly;

		switch ( Type )
		{
			case Invalid:
				log_failure( "AST::duplicate: Cannot duplicate an invalid AST." );
				return nullptr;

			case Untyped:
			case Comment:
			case Preprocessor_Include:
			case Access_Public:
			case Access_Protected:
			case Access_Private:
			case Attributes:
			case Module:
				// Can just be the same, as its a cached string.
				result->Content = Content;
				return result;

			// The main purpose of this is to make sure entires in the AST are unique,
			// So that we can assign the new parent without corrupting the existing AST.
			case Class:
			case Class_Fwd:
			case Class_Body:
			case Enum:
			case Enum_Fwd:
			case Enum_Body:
			case Enum_Class:
			case Enum_Class_Fwd:
			case Export_Body:
			case Execution:
			case Extern_Linkage:
			case Extern_Linkage_Body:
			case Friend:
			case Function:
			case Function_Fwd:
			case Function_Body:
			case Global_Body:
			case Namespace:
			case Namespace_Body:
			case Operator:
			case Operator_Fwd:
			case Operator_Member:
			case Operator_Member_Fwd:
			case Parameters:
			case Specifiers:
			case Struct:
			case Struct_Fwd:
			case Struct_Body:
			case Typedef:
			case Typename:
			case Union:
			case Union_Body:
			case Using:
			case Using_Namespace:
			case Variable:
				s32 index = 0;
				s32 left  = num_entries();
				while ( left -- )
				{
					// This will naturally duplicate the entire chain duplicate all of the ast nodes.
					// It may not be the most optimal way for memory reasons, however figuring out the heuristic
					// For when it should reparent all nodes or not is not within the simple scope of this library.
					result->add_entry( entry(index)->duplicate() );
					index++;
				}
				return result;
		}

		log_failure( "AST::duplicate: Unknown AST type %s.", type_str() );
		return nullptr;
	}

	String AST::to_string()
	{
#	define ProcessModuleFlags() \
		if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))  \
			result.append( "export " );             \
                                                                         \
		if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Import ))  \
			result.append( "import " );             \

		String result = String::make( g_allocator, "" );

		// Scope indentation.
		char indent_str[128] = {0};   // Should be more than enough...
		s32  tab_count       = 0;
		{
			AST* curr_parent = Parent;
			while ( curr_parent )
			{
				switch ( curr_parent->Type )
				{
					using namespace ECode;

					case Class_Body:
					case Enum_Body:
					case Extern_Linkage_Body:
					case Function_Body:
					case Global_Body:
					case Namespace_Body:
					case Struct_Body:
					case Union_Body:
						break;

					default:
					{
						curr_parent = curr_parent->Parent;
						continue;
					}
				}

				indent_str[tab_count] = '\t';
				tab_count++;

				curr_parent = curr_parent->Parent;
			}
		}
		StrC indent = { tab_count, indent_str };

		switch ( Type )
		{
			using namespace ECode;

			case Invalid:
				log_failure("Attempted to serialize invalid code! - %s", Name);
			break;

			case Untyped:
				// result = string_append_length( result, Content, string_length( ccast(String, Content)) );
				result.append( Content );
			break;

			case Comment:
			{
				result.append( indent );

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
				result.append( indent );
				result.append( Name );
			break;

			case Attributes:
				result.append( Content );

			case Class:
			{
				result.append( indent );

				ProcessModuleFlags();

				if ( num_entries() > 1 )
				{
					result.append( "class " );

					s32 idx = 1;

					if ( entry( idx )->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
					}

					result.append( Name );

					AST* parent = entry( idx );

					if ( parent )
					{
						char const* access_level = to_str( ParentAccess );

						result.append_fmt( ": %s %s\n%s{\n"
							, access_level
							, parent
							, indent_str
						);
					}
					else
					{
						result.append( "\n{\n" );
					}

					result.append_fmt( "%s\n%s};", body()->to_string(), indent_str );
				}
				else
				{
					result.append_fmt( "class %s\n{\n%s\n%s};", Name, body()->to_string(), indent_str );
				}
			}
			break;

			case Class_Fwd:
			{
				result.append( indent );

				ProcessModuleFlags();

				result.append_fmt( "class %s;", Name );
			}
			break;

			case Enum:
			{
				result.append( indent );

				ProcessModuleFlags();

				result.append( "enum " );

				if ( num_entries() > 1 )
				{
					s32 idx = 1;

					AST* Entry = entry( idx);

					if ( Entry->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
					}

					if ( Entry->Type == Typename)
					{
						result.append_fmt( "%s : %s\n{\n"
							, Name
							, entry( idx )->to_string()
						);
					}
					else
					{
						result.append_fmt( "%s\n{\n"
							, Name
						);
					}
				}
				else
				{
					result.append_fmt( "%s\n{\n"
						, Name
					);
				}

				result.append_fmt( "%s};"
					, body()->to_string()
				);
			}
			break;

			case Enum_Fwd:
			{
				result.append( indent );

				ProcessModuleFlags();

				result.append_fmt( "enum %s : %s;", Name, entry( 0 )->to_string() );
			}
			break;

			case Enum_Class:
			{
				result.append( indent );

				ProcessModuleFlags();

				result.append( "enum class " );

				s32 idx = 0;

				if ( entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				if ( entry( idx )->Type == Typename )
				{
					result.append_fmt( "%s : %s\n%s{\n"
						, Name
						, entry( idx )->to_string()
						, indent_str
					);
				}
				else
				{
					result.append_fmt( "%s\n%s{\n"
						, Name
						, indent_str
					);
				}

				result.append_fmt( "%s\n%s};"
					, body()->to_string()
					, indent_str
				);
			}
			break;

			case Enum_Class_Fwd:
			{
				result.append( indent );

				ProcessModuleFlags();

				result.append( "enum class " );

				s32 idx = 0;

				if ( entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s : %s;", Name, entry( idx )->to_string() );
			}
			break;

			case Execution:
				result.append( Content );
			break;

			case Export_Body:
			{
				result.append_fmt( "%sexport\n%s{\n", indent_str, indent_str );

				s32 index = 0;
				s32 left  = num_entries();
				while ( left -- )
				{
					result.append_fmt( "%s\t%s\n", indent_str, entry( index )->to_string() );
					index++;
				}

				result.append_fmt( "%s};", indent_str );
			}
			break;

			case Extern_Linkage:
				result.append( indent );

				ProcessModuleFlags();

				result.append_fmt( "extern \"%s\"\n%s{\n%s\n%s}"
					, Name
					, indent_str
					, body()->to_string()
					, indent_str
				);
			break;

			case Friend:
				result.append_fmt( "%sfriend %s", indent_str, entry( 0 )->to_string() );
			break;

			case Function:
			{
				result.append( indent );

				ProcessModuleFlags();

				u32 idx  = 1;
				u32 left = num_entries();

				AST* Entry = entry( idx );

				if ( Entry && Entry->Type == Attributes )
				{
					result.append_fmt( "%s ", Entry->to_string() );
					idx++;
					left--;
					Entry = entry( idx );
				}

				if ( Entry && Entry->Type == Specifiers )
				{
					result.append_fmt( "%s\n", Entry->to_string() );
					idx++;
					left--;
					Entry = entry( idx );
				}

				if ( Entry && Entry->Type == Typename )
				{
					result.append_fmt( "%s ", Entry->to_string() );
					idx++;
					left--;
					Entry = entry( idx );
				}

				result.append_fmt( "%s(", Name );

				if ( left && Entry && Entry->Type == Parameters )
				{
					result.append_fmt("%s", Entry->to_string() );
					idx++;
					left--;
					Entry = entry( idx );
				}
				else
				{
					result.append( "void" );
				}

				result.append_fmt( ")\n%s{\n%s\n%s}"
					, indent_str
					, body()->to_string()
					, indent_str
				);
			}
			break;

			case Function_Fwd:
			{
				result.append( indent );

				ProcessModuleFlags();

				u32 idx  = 0;
				u32 left = num_entries();

				AST* Entry = entry( idx );

				if ( Entry && Entry->Type == Attributes )
				{
					result.append_fmt( "%s ", Entry->to_string() );
					idx++;
					left--;
					Entry = entry( idx );
				}

				if ( Entry && Entry->Type == Specifiers )
				{
					result.append_fmt( "%s\n", Entry->to_string() );
					idx++;
					left--;
					Entry = entry( idx );
				}

				if ( Entry && Entry->Type == Typename )
				{
					result.append_fmt( "%s ", Entry->to_string() );
					idx++;
					left--;
					Entry = entry( idx );
				}

				result.append_fmt( "%s(", Name );

				if ( left && Entry && Entry->Type == Parameters )
				{
					result.append_fmt("%s", Entry->to_string() );
					idx++;
					left--;
					Entry = entry( idx );
				}
				else
				{
					result.append( "void" );
				}

				result.append( ");" );
			}
			break;

			case Module:
				result.append( indent );

				if (((u32(ModuleFlag::Export) & u32(ModuleFlags)) == u32(ModuleFlag::Export)))
					result.append("export ");

				if (((u32(ModuleFlag::Import) & u32(ModuleFlags)) == u32(ModuleFlag::Import)))
					result.append("import ");

				result.append_fmt( "%s;", Name );
				break;

			case Namespace:
				result.append( indent );

				ProcessModuleFlags();

				result.append_fmt( "namespace %s\n%s{\n%s\n%s};"
					, Name
					, indent_str
					, body()->to_string()
					, indent_str
				);
			break;

			case Operator:
			case Operator_Member:
			{
				result.append( indent );

				ProcessModuleFlags();

				s32 idx = 1;

				if ( entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				if ( entry( idx )->Type == Specifiers )
				{
					result.append_fmt( "%s\n", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s operator %s (", entry( idx )->to_string(), Name );
				idx++;

				if ( entry( idx )->Type == Parameters )
				{
					result.append_fmt( "%s", entry( idx )->to_string() );
					idx++;
				}
				else
				{
					result.append_fmt( "void" );
				}

				result.append_fmt( ")\n%s{\n%s\n%s}"
					, indent_str
					, body()->to_string()
					, indent_str
				);
			}
			break;

			case Operator_Fwd:
			case Operator_Member_Fwd:
			{
				result.append( indent );

				ProcessModuleFlags();

				s32 idx = 0;

				if ( entry( idx )->Type == Specifiers )
				{
					result.append_fmt( "%s", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s operator%s(", entry( idx )->to_string(), Name );
				idx++;

				if ( entry( idx )->Type == Parameters )
				{
					result.append_fmt( "%s);", entry( idx )->to_string() );
					idx++;
				}
				else
				{
					result.append_fmt( "void);" );
				}
			}
			break;

			case Parameters:
			{
				result.append_fmt( "%s %s", entry( 0 )->to_string(), Name );

				s32 index = 1;
				s32 left  = num_entries();

				while ( left--, left > 0 )
					result.append_fmt( ", %s %s"
						, entry( index )->entry( 0 )->to_string()
						, entry( index )->Name
					);
			}
			break;

			case Preprocessor_Include:
				result.append_fmt( "#include \"%s\"", Name );
			break;

			case Specifiers:
			{
				s32 idx  = 0;
				s32 left = StaticIndex;
				while ( left--, left > 0 )
					result.append_fmt( "%s ", ESpecifier::to_str( ArrSpecs[idx]) );
			}
			break;

			case Struct:
			{
				result.append( indent );

				ProcessModuleFlags();

				result.append( "struct ");

				s32 idx = 1;

				if ( entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s ", Name );

				if ( entry( idx ) )
				{
					char const* access_str = to_str( ParentAccess );

					result.append_fmt( ": %s %s", access_str, entry( idx )->to_string() );
				}

				result.append_fmt( "\n{\n%s\n};", body()->to_string() );
			}
			break;

			case Struct_Fwd:
			{
				result.append( indent );

				ProcessModuleFlags();

				result.append( "struct ");

				s32 idx = 1;

				if ( entry( idx )->Type == Specifiers )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s;", Name );
			}
			break;

			case Variable:
			{
				result.append( indent );

				ProcessModuleFlags();

				s32 idx = 1;

				if ( num_entries() > 1 )
				{
					if ( entry( idx )->Type == Specifiers )
					{
						result.append_fmt( "%s", entry( idx )->to_string() );
						idx++;
					}

					result.append_fmt( "%s %s", entry( 0 )->to_string(), Name );

					if ( entry( idx ) )
						result.append_fmt( " = %s;", entry( idx )->to_string() );

					break;
				}

				result.append_fmt( "%s %s;", entry( 0 )->to_string(), Name );
			}
			break;

			case Typedef:
			{
				result.append( indent );

				ProcessModuleFlags();

				AST* type = entry( 0 );

				if ( entry( 1 ) && entry( 1 )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( 1 )->to_string() );
				}

				result.append_fmt( "typedef %s %s", type->to_string(), Name );

				if ( type->entry( 1 ) )
				{
					result.append_fmt( "[%s];", type->entry( 1 )->to_string() );
				}
				else
				{
					result.append( ";" );
				}
			}
			break;

			case Typename:
				if ( num_entries() && entry( 0 ) )
				{
					result.append_fmt( "%s %s", Name, entry( 0 )->to_string() );
				}
				else
				{
					result.append_fmt( "%s", Name );
				}
			break;

			case Union:
			{
				result.append( indent );

				ProcessModuleFlags();

				s32 idx = 0;

				result.append( "union " );

				if ( entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s\n%s{\n%s\n%s};"
					, Name
					, indent_str
					, body()->to_string()
					, indent_str
				);
			}
			break;

			case Using:
			{
				result.append( indent );

				ProcessModuleFlags();

				AST* type = entry( 0 );

				if ( entry( 1 ) && entry( 1 )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( 1 )->to_string() );
				}

				if ( type )
				{
					result.append_fmt( "using %s = %s", Name, type->to_string() );

					if ( type->entry( 1 ) )
						result.append_fmt( "[%s]", type->entry( 1 )->to_string() );

					result.append( ";" );
				}
				else
					result.append_fmt( "using %s;", Name );
			}
			break;

			case Using_Namespace:
				result.append_fmt( "%susing namespace %s;", indent_str, Name );
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
				s32 index = 0;
				s32 left  = num_entries();
				while ( left -- )
				{
					result.append_fmt( "%s%s\n", indent_str, entry( index )->to_string() );
					index++;
				}
			}
			break;
		}

		return result;

	#undef ProcessModuleFlags
	}
#pragma endregion AST

#pragma region Gen Interface
	void init()
	{
		using namespace StaticData;

		// Setup the arrays
		{
			if (! array_init_reserve( CodePools, Allocator_DataArrays, InitSize_DataArrays ) )
				fatal( "gen::init: Failed to initialize the CodePools array" );

			if ( ! array_init_reserve( CodeEntriesArenas, Allocator_DataArrays, InitSize_DataArrays ) )
				fatal( "gen::init: Failed to initialize the CodeEntriesPools array" );

			if ( ! array_init_reserve( StringArenas, Allocator_DataArrays, InitSize_DataArrays ) )
				fatal( "gen::init: Failed to initialize the StringArenas array" );
		}

		// Setup the code pool and code entries arena.
		{
			Pool code_pool;
			pool_init( & code_pool, Allocator_CodePool, CodePool_NumBlocks, sizeof(AST) );

			if ( code_pool.physical_start == nullptr )
				fatal( "gen::init: Failed to initialize the code pool" );

			array_append( CodePools, code_pool );

			Arena code_entires_arena;
			arena_init_from_allocator( & code_entires_arena, Allocator_CodeEntriesArena, SizePer_CodeEntriresArena );

			if ( code_entires_arena.physical_start == nullptr )
				fatal( "gen::init: Failed to initialize the code entries arena" );

			array_append( CodeEntriesArenas, code_entires_arena );

			Arena string_arena;
			arena_init_from_allocator( & string_arena, Allocator_StringArena, SizePer_StringArena );

			if ( string_arena.physical_start == nullptr )
				fatal( "gen::init: Failed to initialize the string arena" );

			array_append( StringArenas, string_arena );
		}

		// Setup the hash tables
		{
			str_tbl_init ( & StringMap, Allocator_StringTable );
			if ( StringMap.entries == nullptr )
				fatal( "gen::init: Failed to initialize the StringMap");

			//type_tbl_init( & TypeMap,   Allocator_TypeTable );
			//if ( TypeMap.entries == nullptr )
			//	fatal( "gen::init: Failed to initialize the TypeMap" );
		}

		Code::Invalid = make_code();
		Code::Invalid.lock();

		Code&
		t_void_write = ccast( Code, t_void );
		t_void_write = def_type( name(void) );

	#	define def_constant_code_type( Type_ )             \
		Code&                                              \
		t_##Type_##_write = ccast( Code, type_ns(Type_) ); \
		t_##Type_##_write = def_type( name(Type_) );       \

		def_constant_code_type( int );
		def_constant_code_type( bool );
		def_constant_code_type( char );
		def_constant_code_type( wchar_t );

	#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
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

		Code&
		access_private_write = ccast( Code, access_private );
		access_private_write = make_code();
		access_private_write->Type = ECode::Access_Private;
		access_private_write->Name = get_cached_string( StrC::from("private") );
		access_private_write.lock();

		Code&
		access_protected_write = ccast( Code, access_protected );
		access_protected_write = make_code();
		access_protected_write->Type = ECode::Access_Protected;
		access_protected_write->Name = get_cached_string( StrC::from("protected") );
		access_protected_write.lock();

		Code&
		access_public_write = ccast( Code, access_public );
		access_public_write = make_code();
		access_public_write->Type = ECode::Access_Public;
		access_public_write->Name = get_cached_string( StrC::from("public") );
		access_public_write.lock();

		module_global_fragment          = make_code();
		module_global_fragment->Type    = ECode::Untyped;
		module_global_fragment->Name    = get_cached_string( StrC::from("module;") );
		module_global_fragment->Content = module_global_fragment->Name;
		module_global_fragment.lock();

		module_private_fragment          = make_code();
		module_private_fragment->Type    = ECode::Untyped;
		module_private_fragment->Name    = get_cached_string( StrC::from("module : private;") );
		module_private_fragment->Content = module_private_fragment->Name;
		module_private_fragment.lock();

		pragma_once          = make_code();
		pragma_once->Type    = ECode::Untyped;
		pragma_once->Name    = get_cached_string( StrC::from("#pragma once") );
		pragma_once->Content = pragma_once->Name;
		pragma_once.lock();

		Code&
		spec_local_persist_write = ccast( Code, spec_local_persist );
		spec_local_persist_write = def_specifiers( 1, ESpecifier::Local_Persist );

	#	define def_constant_spec( Type_, ... )                                            \
		Code&                                                                             \
		spec_##Type_##_write = ccast( Code, spec_##Type_);                                \
		spec_##Type_##_write = def_specifiers( macro_num_args(__VA_ARGS__), __VA_ARGS__); \

		def_constant_spec( consteval,        ESpecifier::Consteval );
		def_constant_spec( constexpr,        ESpecifier::Constexpr );
		def_constant_spec( constinit,        ESpecifier::Constinit );
		def_constant_spec( extern_linkage,   ESpecifier::External_Linkage );
		def_constant_spec( const,            ESpecifier::Const );
		def_constant_spec( inline,           ESpecifier::Inline );
		def_constant_spec( internal_linkage, ESpecifier::Internal_Linkage );
		def_constant_spec( mutable,          ESpecifier::Mutable );
		def_constant_spec( ptr,              ESpecifier::Ptr );
		def_constant_spec( ref,              ESpecifier::Ref );
		def_constant_spec( register,         ESpecifier::Register );
		def_constant_spec( rvalue,           ESpecifier::RValue );
		def_constant_spec( static_member,    ESpecifier::Static_Member );
		def_constant_spec( thread_local,     ESpecifier::Thread_Local );
		def_constant_spec( volatile, 	     ESpecifier::Volatile)
		def_constant_spec( type_signed,      ESpecifier::Type_Signed );
		def_constant_spec( type_unsigned,    ESpecifier::Type_Unsigned );
		def_constant_spec( type_short,       ESpecifier::Type_Short );
		def_constant_spec( type_long,        ESpecifier::Type_Long );
	#	undef def_constant_spec
	}

	void deinit()
	{
		using namespace StaticData;

		s32 index = 0;
		s32 left  = array_count( CodePools );
		do
		{
			Pool* code_pool = & CodePools[index];
			pool_free( code_pool );
			index++;
		}
		while ( left--, left );

		index = 0;
		left  = array_count( CodeEntriesArenas );
		do
		{
			Arena* code_entries_arena = & CodeEntriesArenas[index];
			arena_free( code_entries_arena );
			index++;
		}
		while ( left--, left );

		index = 0;
		left  = array_count( StringArenas );
		do
		{
			Arena* string_arena = & StringArenas[index];
			arena_free( string_arena );
			index++;
		}
		while ( left--, left );

		str_tbl_destroy( & StringMap );
		// type_tbl_destroy( & TypeMap );

		array_free( CodePools );
		array_free( CodeEntriesArenas );
		array_free( StringArenas );
	}

	void clear_code_memory()
	{
		using namespace StaticData;

		// Clear the code pools
		{
			s32 index = 0;
			s32 left  = array_count( CodePools );
			while ( left-- )
			{
				Pool* code_pool = & CodePools[index];
				pool_free( code_pool );
				index++;
			}

			array_clear( CodePools );
		}

		// Clear the code entries arenas
		{
			s32 index = 0;
			s32 left = array_count( CodeEntriesArenas );
			while ( left-- )
			{
				Arena* code_entries_arena = & CodeEntriesArenas[index];
				arena_free( code_entries_arena );
				index++;
			}

			array_clear( CodeEntriesArenas );
		}

		// type_tbl_clear( & StaticData::TypeMap );
	}

	inline
	AllocatorInfo get_string_allocator( s32 str_length )
	{
		using namespace StaticData;

		if ( StringArenas->total_allocated + str_length > StringArenas->total_size )
		{
			Arena new_arena;
			arena_init_from_allocator( & new_arena, Allocator_StringArena, SizePer_StringArena );

			if ( array_append( StringArenas, new_arena ) )
				fatal( "gen::get_string_allocator: Failed to allocate a new string arena" );

			return arena_allocator( StringArenas );
		}

		return arena_allocator( StringArenas );
	}

	// Will either make or retrive a code string.
	StringCached get_cached_string( StrC str )
	{
		s32 hash_length = str.Len > kilobytes(1) ? kilobytes(1) : str.Len;
		u32 key         = crc32( str.Ptr, hash_length );
		{
			String* result = str_tbl_get( & StaticData::StringMap, key );

			if ( result )
				return * result;
		}

		String result = String::make( get_string_allocator( str.Len ), str.Ptr );

		str_tbl_set( & StaticData::StringMap, key, result );

		return result;
	}

	/*
		Used internally to retireve a Code object form the CodePool.
	*/
	Code make_code()
	{
		using namespace StaticData;

		AllocatorInfo allocator = { nullptr, nullptr };

		s32 index = 0;
		s32 left  = array_count( CodePools );
		do
		{
			if ( CodePools[index].free_list != nullptr  )
			{
				allocator = zpl::pool_allocator( & CodePools[index] );
				break;
			}
			index++;
		}
		while ( left--, left );

		if ( allocator.data == nullptr )
		{
			Pool code_pool;
			pool_init( & code_pool, Allocator_CodePool, CodePool_NumBlocks, sizeof(AST) );

			if ( code_pool.physical_start == nullptr )
				fatal( "gen::make_code: Failed to allocate a new code pool - CodePool allcoator returned nullptr." );

			if ( ! array_append( CodePools, code_pool ) )
				fatal( "gen::make_code: Failed to allocate a new code pool - CodePools failed to append new pool." );

			allocator = pool_allocator( CodePools );
		}

		Code result { rcast( AST*, alloc( allocator, sizeof(AST) )) };

		result->Content        = { nullptr };
		result->Parent         = nullptr;
		result->Name           = { nullptr };
		result->Type           = ECode::Invalid;
		result->Op             = EOperator::Invalid;
		result->ModuleFlags    = ModuleFlag::Invalid;
		result->ParentAccess   = AccessSpec::Invalid;
		result->StaticIndex    = 0;
		result->Readonly       = false;
		result->DynamicEntries = false;

		return result;
	}

	Array(AST*) make_code_entries()
	{
		using namespace StaticData;

		AllocatorInfo allocator = { nullptr, nullptr };

		s32 index = 0;
		s32 left  = array_count( CodeEntriesArenas );
		do
		{
			if ( arena_size_remaining( & CodeEntriesArenas[index], ZPL_DEFAULT_MEMORY_ALIGNMENT) >= InitSize_CodeEntiresArray )
				allocator = arena_allocator( & CodeEntriesArenas[index] );
			index++;
		}
		while( left--, left );

		if ( allocator.data == nullptr )
		{
			Arena arena;
			arena_init_from_allocator( & arena, Allocator_CodeEntriesArena, SizePer_CodeEntriresArena );

			if ( arena.physical_start == nullptr )
				fatal( "gen::make_code: Failed to allocate a new code entries arena - CodeEntriesArena allcoator returned nullptr." );

			allocator = arena_allocator( & arena );
			array_append( CodeEntriesArenas, arena );
		}

		Array(AST*) entry_array;
		array_init( entry_array, allocator );

		return entry_array;
	}

	enum class OpValidateResult : u32
	{
		Fail,
		Global,
		Member
	};

	inline
	OpValidateResult operator__validate( OperatorT op, Code params_code, Code ret_type, Code specifier )
	{
		using namespace EOperator;

		if ( op == EOperator::Invalid )
		{
			log_failure("gen::def_operator: op cannot be invalid");
			return OpValidateResult::Fail;
		}

	#pragma region Helper Macros
	#	define check_params()                                                                                  \
		if ( ! params_code )                                                                                   \
		{                                                                                                      \
			log_failure("gen::def_operator: params is null and operator%s requires it", to_str(op));           \
			return OpValidateResult::Fail;                                                                     \
		}                                                                                                      \
		if ( params_code->Type != ECode::Parameters )                                                          \
		{                                                                                                      \
			log_failure("gen::def_operator: params is not of Parameters type - %s", params_code->debug_str()); \
			return OpValidateResult::Fail;                                                                     \
		}

	#	define check_param_eq_ret()                                                                    \
		if ( ! is_member_symbol && params_code->param_type() != ret_type )                             \
		{                                                                                              \
			log_failure("gen_def_operator: operator%s requires first parameter to equal return type\n" \
				"param types: %s\n"                                                                    \
				"return type: %s",                                                                     \
				to_str(op),                                                                            \
				params_code->debug_str(),                                                              \
				ret_type->debug_str()                                                                  \
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
			log_failure("gen::def_operator: ret_type is not of typename type - %s", ret_type->debug_str());
			return OpValidateResult::Fail;
		}

		bool is_member_symbol = false;

		switch ( op )
		{
		#	define specs( ... ) macro_num_args( __VA_ARGS__ ), __VA_ARGS__
			case Assign:
				check_params();

				if ( params_code->param_count() > 1 )
				{
					log_failure("gen::def_operator: "
						"operator%s does not support non-member definition (more than one parameter provided) - %s",
						to_str(op),
						params_code->debug_str()
					);
					return OpValidateResult::Fail;
				}

				is_member_symbol = true;
			break;

			case Assign_Add:
			case Assign_Subtract:
			case Assgin_Multiply:
			case Assgin_Divide:
			case Assgin_Modulo:
			case Assgin_BAnd:
			case Assgin_BOr:
			case Assign_BXOr:
			case Assign_LShift:
			case Assign_RShift:
				check_params();

				if ( params_code->param_count() == 1 )
					is_member_symbol = true;

				else
					check_param_eq_ret();

				if (params_code->param_count() > 2 )
				{
					log_failure("gen::def_operator: operator%s may not be defined with more than two parametes - param count; %d\n%s"
						, to_str(op)
						, params_code->param_count()
						, params_code->debug_str()
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
							, params_code->debug_str()
						);
						return OpValidateResult::Fail;
					}

					switch ( params_code->param_count() )
					{
						case 1:
							if ( params_code->param_type() == type_ns(int) )
								is_member_symbol = true;

							else
								check_param_eq_ret();
						break;

						case 2:
							check_param_eq_ret();

							if ( params_code->get_param(1) != type_ns(int) )
							{
								log_failure("gen::def_operator: "
									"operator%s requires second parameter of non-member definition to be int for post-decrement",
									to_str(op)
								);
								return OpValidateResult::Fail;
							}
						break;

						default:
							log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-2"
								, to_str(op)
								, params_code->param_count()
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
						log_failure("gen::def_operator: params is not of Parameters type - %s", params_code->debug_str());
						return OpValidateResult::Fail;
					}

					if ( params_code->param_type() == ret_type )
					{
						log_failure("gen::def_operator: "
							"operator%s is non-member symbol yet first paramter does not equal return type\n"
							"param type: %s\n"
							"return type: %s\n"
							, params_code->debug_str()
							, ret_type->debug_str()
						);
						return OpValidateResult::Fail;
					}

					if ( params_code->param_count() > 1 )
					{
						log_failure("gen::def_operator: operator%s may not have more than one parameter - param count: %d"
							, to_str(op)
							, params_code->param_count()
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

				switch ( params_code->param_count() )
				{
					case 1:
						is_member_symbol = true;
					break;

					case 2:
						if ( params_code->param_type() != ret_type )
						{
							log_failure("gen::def_operator: "
								"operator%s is non-member symbol yet first paramter does not equal return type\n"
								"param type: %s\n"
								"return type: %s\n"
								, params_code->debug_str()
								, ret_type->debug_str()
							);
							return OpValidateResult::Fail;
						}
					break;

					default:
						log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-2"
							, to_str(op)
							, params_code->param_count()
						);
						return OpValidateResult::Fail;
				}
			break;

			case LNot:
				if ( ! params_code )
					is_member_symbol = true;

				else
				{
					if ( params_code->Type != ECode::Parameters )
					{
						log_failure("gen::def_operator: params is not of Parameters type - %s", params_code->debug_str());
						return OpValidateResult::Fail;
					}

					if ( params_code->param_count() != 1 )
					{
						log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-1"
							, to_str(op)
							, params_code->param_count()
						);
						return OpValidateResult::Fail;
					}
				}

				if ( ret_type != type_ns(bool) )
				{
					log_failure("gen::def_operator: operator%s return type must be of type bool - %s"
						, to_str(op)
						, ret_type->debug_str()
					);
					return OpValidateResult::Fail;
				}
			break;

			case LAnd:
			case LOr:
			case Equals:
			case NotEquals:
			case Lesser:
			case Greater:
			case LesserEqual:
			case GreaterEqual:
				check_params();

				switch ( params_code->param_count() )
				{
					case 1:
						is_member_symbol = true;
					break;

					case 2:
					break;

					default:
						log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 1-2"
							, to_str(op)
							, params_code->param_count()
						);
						return OpValidateResult::Fail;
				}
			break;

			case Indirection:
			case AddressOf:
			case MemberOfPointer:
				if ( params_code && params_code->param_count() > 1)
				{
					log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-1"
						, to_str(op)
						, params_code->param_count()
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
					log_failure("gen::def_operator: operator%s expects no paramters - %s", to_str(op), params_code->debug_str());
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
		StaticData::Allocator_DataArrays = allocator;
	}

	void set_allocator_code_pool( AllocatorInfo allocator )
	{
		StaticData::Allocator_CodePool = allocator;
	}

	void set_allocator_code_entries_arena( AllocatorInfo allocator )
	{
		StaticData::Allocator_CodeEntriesArena = allocator;
	}

	void set_allocator_string_arena( AllocatorInfo allocator )
	{
		StaticData::Allocator_StringArena = allocator;
	}

	void set_allocator_string_table( AllocatorInfo allocator )
	{
		StaticData::Allocator_StringArena = allocator;
	}

#pragma region Helper Marcos
	// This snippet is used in nearly all the functions.
#	define name_check( Context_, Name_ )                                                            \
	{                                                                                               \
		if ( Name_.Len <= 0 )                                                                       \
		{                                                                                           \
			log_failure( "gen::" txt(Context_) ": Invalid name length provided - %d",  Name_.Len ); \
			return Code::Invalid;                                                                   \
		}                                                                                           \
                                                                                                    \
		if ( Name_.Ptr == nullptr )                                                                 \
		{                                                                                           \
			log_failure( "gen::" txt(Context_) ": name is null" );                                  \
			return Code::Invalid;                                                                   \
		}                                                                                           \
	}

#	define null_check( Context_, Code_ )                                              \
		if ( ! Code_ )                                                                \
		{                                                                             \
			log_failure( "gen::" txt(Context_) ": " txt(Code_) " provided is null" ); \
			return Code::Invalid;                                                     \
		}

#	define null_or_invalid_check( Context_, Code_ )                                     \
	{                                                                                   \
		if ( ! Code_ )                                                                  \
		{                                                                               \
			log_failure( "gen::" txt(Context_) ": " txt(Code_) " provided is null" );   \
			return Code::Invalid;                                                       \
		}                                                                               \
                                                                                        \
		if ( Code_->is_invalid() )                                                      \
		{                                                                               \
			log_failure("gen::" txt(Context_) ": " txt(Code_) " provided is invalid" ); \
			return Code::Invalid;                                                       \
		}                                                                               \
	}

#	define not_implemented( Context_ )                              \
		log_failure( "gen::%s: This function is not implemented" ); \
		return Code::Invalid;
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
	I decided to validate a good protion of their form and thus the argument processing for is quite a bit.
*/
	Code def_attributes( StrC content )
	{
		if ( content.Len <= 0 || content.Ptr == nullptr )
		{
			log_failure( "gen::def_attributes: Invalid attributes provided" );
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Type    = ECode::Attributes;
		result->Name    = get_cached_string( content );
		result->Content = result->Name;

		result.lock();
		return result;
	}

	Code def_comment( StrC content )
	{
		if ( content.Len <= 0 || content.Ptr == nullptr )
		{
			log_failure( "gen::def_comment: Invalid comment provided:" );
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Type    = ECode::Comment;
		result->Name    = get_cached_string( content );
		result->Content = result->Name;

		result.lock();
		return result;
	}

	Code def_class( StrC name
		, Code body
		, Code parent, AccessSpec parent_access
		, Code attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_class, name );

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::def_class: attributes was not a 'Attributes' type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		if ( parent && (parent->Type != Class || parent->Type != Struct || parent->Type != Typename || parent->Type != Untyped) )
		{
			log_failure( "gen::def_class: parent provided is not type 'Class', 'Struct', 'Typeanme', or 'Untyped': %s", parent->debug_str() );
			return Code::Invalid;
		}

		Code
		result              = make_code();
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
					log_failure("gen::def_class: body must be either of Class_Body or Untyped type - %s", body->debug_str());
					return Code::Invalid;
			}

			result->Type = Class;
			result->add_entry( body );
		}
		else
		{
			result->Type = Class_Fwd;
		}

		if ( attributes )
			result->add_entry( attributes );

		if ( parent )
		{
			result->ParentAccess = parent_access;
			result->add_entry( parent );
		}

		result.lock();
		return result;
	}

	Code def_enum( StrC name
		, Code body, Code type
		, EnumT specifier, Code attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_enum, name );

		if ( type && type->Type != Typename )
		{
			log_failure( "gen::def_enum: enum underlying type provided was not of type Typename: %s", type->debug_str() );
			return Code::Invalid;
		}

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::def_enum: attributes was not a 'Attributes' type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		Code
		result              = make_code();
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
					log_failure( "gen::def_enum: body must be of Enum_Body or Untyped type %s", body->debug_str());
					return Code::Invalid;
			}

			result->Type = specifier == EnumClass ?
				Enum_Class : Enum;

			result->add_entry( body );
		}
		else
		{
			result->Type = specifier == EnumClass ?
				Enum_Class_Fwd : Enum_Fwd;
		}

		if ( attributes )
			result->add_entry( attributes );

		if ( type )
		{
			result->add_entry( type );
		}
		else if ( result->Type != Enum_Class_Fwd && result->Type != Enum_Fwd )
		{
			log_failure( "gen::def_enum: enum forward declaration must have an underlying type" );
			return Code::Invalid;
		}

		result.lock();
		return result;
	}

	Code def_execution( StrC content )
	{
		if ( content.Len <= 0 || content.Ptr == nullptr )
		{
			log_failure( "gen::def_execution: Invalid execution provided" );
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Type    = ECode::Execution;
		result->Name    = get_cached_string( content );
		result->Content = result->Name;

		result.lock();
		return result;
	}

	Code def_extern_link( StrC name, Code body, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_extern_linkage, name );
		null_check( def_extern_linkage, body );

		if ( body->Type != Extern_Linkage_Body && body->Type != Untyped )
		{
			log_failure("gen::def_extern_linkage: body is not of extern_linkage or untyped type %s", body->debug_str());
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Type        = Extern_Linkage;
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		result->add_entry( body );

		result.lock();
		return result;
	}

	Code def_friend( Code declaration )
	{
		using namespace ECode;

		null_check( def_friend, declaration );

		switch ( declaration->Type )
		{
			case Class_Fwd:
			case Function_Fwd:
			case Operator_Fwd:
			case Struct_Fwd:
			break;

			default:
				// Technically friends can have a function body. I will not support it so if you want that add it yourself. - Ed.
				log_failure("gen::def_friend: declaration cannot be used with friend, must be a forward declare - %s", declaration->debug_str());
				return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Type = Friend;

		result->add_entry( declaration );

		result.lock();
		return result;
	}

	Code def_function( StrC name
		, Code params , Code ret_type, Code body
		, Code specifiers, Code attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_function, name );

		if ( params && params->Type != Parameters )
		{
			log_failure( "gen::def_function: params was not a `Parameters` type: %s", params->debug_str() );
			return Code::Invalid;
		}

		if ( ret_type && ret_type->Type != Typename )
		{
			log_failure( "gen::def_function: ret_type was not a Typename: %s", ret_type->debug_str() );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_function: specifiers was not a `Specifiers` type: %s", specifiers->debug_str() );
			return Code::Invalid;
		}

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::def_function: attributes was not a `Attributes` type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		Code
		result              = make_code();
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
					log_failure("gen::def_function: body must be either of Function_Body or Untyped type. %s", body->debug_str());
					return Code::Invalid;
				}
			}

			result->Type = Function;
			result->add_entry( body );
		}
		else
		{
			result->Type = Function_Fwd;
		}

		if ( attributes )
			result->add_entry( attributes );

		if ( specifiers )
			result->add_entry( specifiers );

		if ( ret_type )
		{
			result->add_entry( ret_type );
		}
		else
		{
			result->add_entry( type_ns(void) );
		}

		if ( params )
			result->add_entry( params );

		result.lock();
		return result;
	}

	Code def_include ( StrC path )
	{
		if ( path.Len <= 0 || path.Ptr == nullptr )
		{
			log_failure( "gen::def_include: Invalid path provided - %d" );
			return Code::Invalid;
		}

		Code
		result          = make_code();
		result->Type    = ECode::Preprocessor_Include;
		result->Name    = get_cached_string( path );
		result->Content = result->Name;

		result.lock();
		return result;
	}

	Code def_module( StrC name, ModuleFlag mflags )
	{
		name_check( def_module, name );

		Code
		result = make_code();
		result->Type        = ECode::Module;
		result->Name        = get_cached_string( name );
		result->Content     = result->Name;
		result->ModuleFlags = mflags;

		result.lock();
		return result;
	}

	Code def_namespace( StrC name, Code body, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_namespace, name );
		null_check( def_namespace, body );

		if ( body->Type != Namespace_Body && body->Type != Untyped )
		{
			log_failure("gen::def_namespace: body is not of namespace or untyped type %s", body->debug_str());
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Type        = Namespace;
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		result->add_entry( body );

		result.lock();
		return result;
	}

	Code def_operator( OperatorT op
		, Code params_code, Code ret_type, Code body
		, Code specifiers, Code attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

		if ( body && body->Type != Function_Body )
		{
			log_failure( "gen::def_operator: Body was provided but its not of function body type: %s", body->debug_str() );
			return Code::Invalid;
		}

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::def_operator: Attributes was provided but its not of attributes type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_operator: Specifiers was provided but its not of specifiers type: %s", specifiers->debug_str() );
			return Code::Invalid;
		}

		OpValidateResult check_result = operator__validate( op, params_code, ret_type, specifiers );

		if ( check_result == OpValidateResult::Fail )
		{
			return Code::Invalid;
		}

		char const* name = str_fmt_buf( "operator%s", to_str(op) );

		Code
		result              = make_code();
		result->Name        = get_cached_string( { str_len(name), name } );
		result->ModuleFlags = mflags;

		if ( body )
		{
			result->Type = check_result == OpValidateResult::Global ?
				Operator : Operator_Member;

			result->add_entry( body );
		}
		else
		{
			result->Type = check_result == OpValidateResult::Global ?
				Operator_Fwd : Operator_Member_Fwd;
		}

		if ( attributes )
			result->add_entry( attributes );

		if ( specifiers )
			result->add_entry( specifiers );

		result->add_entry( ret_type );

		if (params_code)
			result->add_entry( params_code );

		result.lock();
		return result;
	}

	Code def_param( Code type, StrC name, Code value )
	{
		using namespace ECode;

		name_check( def_param, name );
		null_check( def_param, type );

		if ( type->Type != Typename )
		{
			log_failure( "gen::def_param: type is not a typename - %s", type->debug_str() );
			return Code::Invalid;
		}

		if ( value && value->Type != Untyped )
		{
			log_failure( "gen::def_param: value is not untyped - %s", value->debug_str() );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Type = Parameters;
		result->Name = get_cached_string( name );

		result->add_entry( type );

		if ( value )
			result->add_entry( value );

		result.lock();
		return result;
	}

	Code def_struct( StrC name
		, Code body
		, Code parent, AccessSpec parent_access
		, Code attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_struct, name );

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::def_struct: attributes was not a `Attributes` type" );
			return Code::Invalid;
		}

		if ( parent && parent->Type != Struct )
		{
			log_failure( "gen::def_struct: parent was not a `Struct` type" );
			return Code::Invalid;
		}

		if ( body && body->Type != Struct_Body )
		{
			log_failure( "gen::def_struct: body was not a Struct_Body type" );
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		if ( body )
		{
			result->Type = Struct;
			result->add_entry( body );
		}
		else
		{
			result->Type = Struct_Fwd;
		}

		if ( attributes )
			result->add_entry( attributes );

		if ( parent )
		{
			result->ParentAccess = parent_access;
			result->add_entry( parent );
		}

		result.lock();
		return result;
	}

	Code def_type( StrC name, Code ArrayExpr, Code specifiers )
	{
		name_check( def_type, name );

		if ( specifiers && specifiers->Type != ECode::Specifiers )
		{
			log_failure( "gen::def_type: specifiers is not of specifiers type - %s", specifiers->debug_str() );
			return Code::Invalid;
		}

		// Code cached = get_cached_type( name );
		// if ( cached )
		// 	return cached;

		Code
		result       = make_code();
		result->Name = get_cached_string( name );
		result->Type = ECode::Typename;

		if ( specifiers )
			result->add_entry( specifiers );

		if ( ArrayExpr )
			result->add_entry( ArrayExpr );

		result.lock();

		// s32 hash_length = name.Len > kilobytes(1) ? kilobytes(1) : name.Len;
		// s32 key         = crc32( name.Ptr, hash_length );

		// type_tbl_set( & StaticData::TypeMap, key, result );

		return result;
	}

	Code def_typedef( StrC name, Code type, Code attributes, ModuleFlag mflags )
	{
		name_check( def_typedef, name );
		null_check( def_typedef, type );

		if ( type->Type != ECode::Typename )
		{
			log_failure( "gen::def_typedef: type was not a Typename" );
			return Code::Invalid;
		}

		// Registering the type.
		Code registered_type = def_type( name );

		if ( ! registered_type )
		{
			log_failure( "gen::def_typedef: failed to register type" );
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Name        = get_cached_string( name );
		result->Type        = ECode::Typedef;
		result->ModuleFlags = mflags;

		result->add_entry( type );

		if ( attributes )
			result->add_entry( attributes );

		result.lock();
		return result;
	}

	Code def_union( StrC name, Code body, Code attributes, ModuleFlag mflags )
	{
		name_check( def_union, name );
		null_check( def_union, body );

		if ( body->Type != ECode::Union_Body )
		{
			log_failure( "gen::def_union: body was not a Union_Body type" );
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;
		result->Type        = ECode::Union;

		result->add_entry( body );

		if ( attributes )
			result->add_entry( attributes );

		result.lock();
		return result;
	}

	Code def_using( StrC name, Code type
		, Code attributes
		, ModuleFlag mflags )
	{
		name_check( def_using, name );
		null_check( def_using, type );

		Code register_type = def_type( name );

		if ( ! register_type )
		{
			log_failure( "gen::def_using: failed to register type" );
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;
		result->Type = ECode::Using;

		if ( type )
			result->add_entry( type );

		if ( attributes )
			result->add_entry( attributes );

		result.lock();
		return result;
	}

	Code def_using_namespace( StrC name )
	{
		name_check( def_using_namespace, name );

		Code
		result          = make_code();
		result->Name    = get_cached_string( name );
		result->Content = result->Name;
		result->Type    = ECode::Using_Namespace;

		result.lock();
		return result;
	}

	Code def_variable( Code type, StrC name, Code value
		, Code specifiers, Code attributes
		, ModuleFlag mflags )
	{
		name_check( def_variable, name );
		null_check( def_variable, type );

		if ( attributes && attributes->Type != ECode::Attributes )
		{
			log_failure( "gen::def_variable: attributes was not a `Attributes` type" );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != ECode::Specifiers )
		{
			log_failure( "gen::def_variable: specifiers was not a `Specifiers` type" );
			return Code::Invalid;
		}

		if ( type->Type != ECode::Typename )
		{
			log_failure( "gen::def_variable: type was not a Typename" );
			return Code::Invalid;
		}

		if ( value && value->Type != ECode::Untyped )
		{
			log_failure( "gen::def_variable: value was not a `Untyped` type" );
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Name        = get_cached_string( name );
		result->Type        = ECode::Variable;
		result->ModuleFlags = mflags;

		result->add_entry( type );

		if ( attributes )
			result->add_entry( attributes );

		if ( specifiers )
			result->add_entry( specifiers );

		if ( value )
			result->add_entry( value );

		result.lock();
		return result;
	}

/*
	Body related functions typically follow the same implementation pattern.
	So I opted to use inline helper macros to get the implementaiton done.

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
#	define def_body_start( Name_ )                                          \
	using namespace ECode;                                                  \
																            \
	if ( num <= 0 )                                                         \
	{                                                                       \
		log_failure("gen::" txt(Name_) ": num cannot be zero or negative"); \
		return Code::Invalid;                                               \
	}

#	define def_body_code_array_start( Name_ )                                \
	using namespace ECode;                                                   \
																             \
	if ( num <= 0 )                                                          \
	{                                                                        \
		log_failure("gen::" txt(Name_) ": num cannot be zero or negative");  \
		return Code::Invalid;                                                \
	}                                                                        \
                                                                             \
	if ( codes == nullptr )                                                  \
	{                                                                        \
		log_failure("gen::def_class_body: Provided a null array of codes");  \
		return Code::Invalid;                                                \
	}

#	define def_body_code_validation_start( Name_ )                          \
	do                                                                      \
	{                                                                       \
		Code_POD pod   = va_arg(va, Code_POD);                              \
		Code     entry = pcast(Code, pod);                                  \
                                                                            \
		if ( ! entry )                                                      \
		{                                                                   \
			log_failure("gen::" txt(Name_) ": Provided an null entry");     \
			return Code::Invalid;                                           \
		}                                                                   \
                                                                            \
		switch ( entry->Type )                                              \
		{

#	define def_body_code_array_validation_start( Name_ )                    \
	do                                                                      \
	{                                                                       \
		Code entry = *codes; codes++;                                       \
                                                                            \
		if ( ! entry )                                                      \
		{                                                                   \
			log_failure("gen::" txt(Name_) ": Provided an null entry");     \
			return Code::Invalid;                                           \
		}                                                                   \
                                                                            \
		switch ( entry->Type )                                              \
		{

#	define def_body_code_validation_end( Name_ )                                                        \
				log_failure("gen::" txt(Name_) ": Entry type is not allowed: %s", entry->debug_str() ); \
				return Code::Invalid;                                                                   \
                                                                                                        \
			default:                                                                                    \
				break;                                                                                  \
		}                                                                                               \
                                                                                                        \
		result->add_entry( entry );                                                                     \
	}                                                                                                   \
	while ( num--, num > 0 )
#pragma endregion Helper Macros for def_**_body functions

	Code def_class_body( s32 num, ... )
	{
		def_body_start( def_class_body );

		Code
		result       = make_code();
		result->Type = Class_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_class_body );
			AST_BODY_CLASS_UNALLOWED_TYPES
		def_body_code_validation_end( def_class_body );
		va_end(va);

		result.lock();
		return result;
	}

	Code def_class_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_class_body );

		Code
		result       = make_code();
		result->Type = Function_Body;

		def_body_code_array_validation_start( def_class_body );
			AST_BODY_CLASS_UNALLOWED_TYPES
		def_body_code_validation_end( def_class_body );

		result.lock();
		return result;
	}

	Code def_enum_body( s32 num, ... )
	{
		def_body_start( def_enum_body );

		Code
		result       = make_code();
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
				return Code::Invalid;
			}

			if ( entry->Type != Untyped )
			{
				log_failure("gen::def_enum_body: Entry type is not allowed - %s. Must be of untyped type.", entry->debug_str() ); \
				return Code::Invalid;
			}

			result->add_entry( entry );
		}
		while ( num--, num > 0 );
		va_end(va);

		result.lock();
		return result;
	}

	Code def_enum_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_enum_body );

		Code
		result       = make_code();
		result->Type = Enum_Body;

		do
		{
			Code entry = *codes;

			if ( ! entry )
			{
				log_failure("gen::def_enum_body: Provided a null entry");
				return Code::Invalid;
			}

			if ( entry->Type != Untyped )
			{
				log_failure("gen::def_enum_body: Entry type is not allowed: %s", entry->debug_str() ); \
				return Code::Invalid;
			}

			result->add_entry( entry );
		}
		while ( codes++, num--, num > 0 );

		result.lock();
		return result;
	}

	Code def_export_body( s32 num, ... )
	{
		def_body_start( def_export_body );

		Code
		result       = make_code();
		result->Type = Export_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_export_body );
			AST_BODY_EXPORT_UNALLOWED_TYPES
		def_body_code_validation_end( def_export_body );
		va_end(va);

		result.lock();
		return result;
	}

	Code def_export_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_export_body );

		Code
		result       = make_code();
		result->Type = Export_Body;

		def_body_code_array_validation_start( def_export_body );
			AST_BODY_EXPORT_UNALLOWED_TYPES
		def_body_code_validation_end( def_export_body );

		result.lock();
		return result;
	}

	Code def_extern_link_body( s32 num, ... )
	{
		def_body_start( def_extern_linkage_body );

		Code
		result       = make_code();
		result->Type = Extern_Linkage_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_extern_linkage_body );
			AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES
		def_body_code_validation_end( def_extern_linkage_body );
		va_end(va);

		result.lock();
		return result;
	}

	Code def_extern_link_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_extern_linkage_body );

		Code
		result       = make_code();
		result->Type = Extern_Linkage_Body;

		def_body_code_array_validation_start( def_extern_linkage_body );
			AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES
		def_body_code_validation_end( def_extern_linkage_body );

		result.lock();
		return result;
	}

	Code def_function_body( s32 num, ... )
	{
		def_body_start( def_function_body );

		Code
		result       = make_code();
		result->Type = Function_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_function_body );
			AST_BODY_FUNCTION_UNALLOWED_TYPES
		def_body_code_validation_end( def_function_body );
		va_end(va);

		result.lock();
		return result;
	}

	Code def_function_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_function_body );

		Code
		result       = make_code();
		result->Type = Function_Body;

		def_body_code_array_validation_start( def_function_body );
			AST_BODY_FUNCTION_UNALLOWED_TYPES
		def_body_code_validation_end( def_function_body );

		result.lock();
		return result;
	}

	Code def_global_body( s32 num, ... )
	{
		def_body_start( def_global_body );

		Code
		result       = make_code();
		result->Type = Global_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_global_body );
			AST_BODY_GLOBAL_UNALLOWED_TYPES
		def_body_code_validation_end( def_global_body );
		va_end(va);

		result.lock();
		return result;
	}

	Code def_global_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_global_body );

		Code
		result       = make_code();
		result->Type = Global_Body;

		def_body_code_array_validation_start( def_global_body );
			AST_BODY_GLOBAL_UNALLOWED_TYPES
		def_body_code_validation_end( def_global_body );

		result.lock();
		return result;
	}

	Code def_namespace_body( s32 num, ... )
	{
		def_body_start( def_namespace_body );

		Code
		result = make_code();
		result->Type = Namespace_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_namespace_body );
			AST_BODY_NAMESPACE_UNALLOWED_TYPES
		def_body_code_validation_end( def_namespace_body );
		va_end(va);

		result.lock();
		return result;
	}

	Code def_namespace_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_namespace_body );

		Code
		result       = make_code();
		result->Type = Global_Body;

		def_body_code_array_validation_start( def_namespace_body );
			AST_BODY_NAMESPACE_UNALLOWED_TYPES
		def_body_code_validation_end( def_namespace_body );

		result.lock();
		return result;
	}

	Code def_params( s32 num, ... )
	{
		def_body_start( def_params );

		if ( num % 3 != 0 )
		{
			log_failure("gen::def_params: number of arguments must be a multiple of 3 (Code, s32, char const*) - %d", num);
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Type = Parameters;

		va_list va;
		va_start(va, num);

		Code_POD    pod         = va_arg(va, Code_POD);
		Code        type        = pcast( Code, pod );
		s32         name_length = va_arg(va, s32 );
		char const* name        = va_arg(va, char const*);

		result->Name = get_cached_string( { name_length, name } );

		if ( type->Type != Typename )
		{
			log_failure( "gen::def_params: type of param %d is not a Typename", num - num + 1 );
			return Code::Invalid;
		}

		result->add_entry( type );

		while( num -= 3, num && num % 3 == 0 )
		{
			pod         = va_arg(va, Code_POD);
			type        = pcast( Code, pod );
			name_length = va_arg(va, u32);
			name        = va_arg(va, char const*);

			Code
			param       = make_code();
			param->Type = Parameters;
			param->Name = get_cached_string( { name_length, name } );

			if ( type->Type != Typename )
			{
				log_failure( "gen::def_params: type of param %d is not a Typename", num - num + 1 );
				return Code::Invalid;
			}

			param->add_entry( type );
			param.lock();

			result->add_entry(param);
		}
		va_end(va);

		result.lock();
		return result;
	}

	Code def_params( s32 num, Code* codes )
	{
		def_body_code_array_start( def_params );

	#	define check_current()                                                                                      \
		if ( current == nullptr )                                                                                   \
		{                                                                                                           \
			log_failure("gen::def_params: Provide a null code in codes array");                                     \
			return Code::Invalid;                                                                                   \
		}                                                                                                           \
                                                                                                                    \
		if (current->Type != Parameters )                                                                           \
		{                                                                                                           \
			log_failure("gen::def_params: Code in coes array is not of paramter type - %s", current->debug_str() ); \
			return Code::Invalid;                                                                                   \
		}

		Code current = * codes;
		check_current();

		Code
		result = make_code();
		result->Name = current->Name;
		result->Type = current->Type;

		result->add_entry( current->entry( 0) );

		while( codes++, current = * codes, num--, num > 0 )
		{
			check_current();
			result->add_entry( current );
		}
	#	undef check_current

		result.lock();
		return result;
	}

	Code def_specifiers( s32 num, ... )
	{
		if ( num <= 0 )
		{
			log_failure("gen::def_specifiers: num cannot be zero or less");
			return Code::Invalid;
		}

		if ( num > AST::ArrSpecs_Cap )
		{
			log_failure("gen::def_specifiers: num of speciifers to define AST larger than AST specicifier capacity - %d", num);
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Type = ECode::Specifiers;

		va_list va;
		va_start(va, num);
		do
		{
			SpecifierT type = (SpecifierT)va_arg(va, int);

			result->add_specifier( type );
		}
		while ( --num, num );
		va_end(va);

		result.lock();
		return result;
	}

	Code def_struct_body( s32 num, ... )
	{
		def_body_start( def_struct_body );

		Code
		result       = make_code();
		result->Type = Struct_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_struct_body );
			AST_BODY_STRUCT_UNALLOWED_TYPES
		def_body_code_validation_end( def_struct_body );
		va_end(va);

		result.lock();
		return result;
	}

	Code def_struct_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_struct_body );

		Code
		result       = make_code();
		result->Type = Struct_Body;

		def_body_code_array_validation_start( def_struct_body );
			AST_BODY_STRUCT_UNALLOWED_TYPES
		def_body_code_validation_end( def_struct_body );

		result.lock();
		return result;
	}

	Code def_union_body( s32 num, ... )
	{
		def_body_start( def_union_body );

		Code
		result       = make_code();
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
				return Code::Invalid;
			}

			if ( entry->Type != Untyped && entry->Type != Comment )
			{
				log_failure("gen::def_union_body: Entry type is not allowed - %s. Must be of untyped type.", entry->debug_str() ); \
				return Code::Invalid;
			}

			result->add_entry( entry );
		}
		while ( num--, num > 0 );
		va_end(va);

		result.lock();
		return result;
	}

	Code def_union_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_union_body );

		Code
		result       = make_code();
		result->Type = Union_Body;

		do
		{
			Code entry = *codes;

			if ( ! entry )
			{
				log_failure("gen::def_union_body: Provided a null entry");
				return Code::Invalid;
			}

			if ( entry->Type != Untyped )
			{
				log_failure("gen::def_union_body: Entry type is not allowed: %s", entry->debug_str() ); \
				return Code::Invalid;
			}

			result->add_entry( entry );
		}
		while ( codes++, num--, num > 0 );

		result.lock();
		return result;
	}
#pragma endregion Upfront Constructors

#pragma region Incremetnal Constructors
#ifdef	GEN_FEATURE_INCREMENTAL
	Code make_class( StrC name
		, Code parent, AccessSpec parent_access
		, Code specifiers, Code attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( make_struct, name );

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::make_class: attributes was not a `Attributes` type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::make_class: specifiers was not a `Specifiers` type: %s", specifiers->debug_str() );
			return Code::Invalid;
		}

		if ( parent && parent->Type != Struct )
		{
			log_failure( "gen::make_class: parent was not a `Struct` type: %s", parent->debug_str() );
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Type        = Struct;
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		Code
		body       = make_code();
		body->Type = Struct_Body;

		result->add_entry( body );

		if ( attributes )
			result->add_entry( attributes );

		if ( specifiers )
			result->add_entry( specifiers );

		if ( parent )
			result->add_entry( parent );

		return result;
	}

	Code make_enum( StrC name, Code type, EnumT specifier, Code attributes, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( make_enum, name );

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::make_enum: attributes was not a `Attributes` type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		if ( type && type->Type != Typename )
		{
			log_failure("gen::make_enum: type provided is not of code type typename - %s", type->debug_str() );
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Type        = specifier == EnumClass ? Enum_Class : Enum;
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		Code
		body       = make_code();
		body->Type = Enum_Body;

		result->add_entry( body );

		if ( type )
			result->add_entry( type );

		return result;
	}

	Code make_export_body( StrC name )
	{
		using namespace ECode;

		Code
		result       = make_code();
		result->Type = Export_Body;

		if ( name && name.Len > 0 )
			result->Name = get_cached_string( name );

		return result;
	}

	Code make_extern_link( StrC name, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( make_extern_linkage, name);

		Code
		result              = make_code();
		result->Type        = Extern_Linkage;
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		return result;
	}

	Code make_function( StrC name
		, Code params, Code ret_type
		, Code specifiers, Code attributes
		, ModuleFlag mflags
	)
	{
		using namespace ECode;

		name_check( make_function, name );

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::make_function: attributes was not a `Attributes` type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_function: specifiers was not a `Specifiers` type" );
			return Code::Invalid;
		}

		if ( params && params->Type != Parameters )
		{
			log_failure( "gen::def_function: params was not a `Parameters` type" );
			return Code::Invalid;
		}

		if ( ret_type == nullptr || ret_type->Type != Typename )
		{
			log_failure( "gen::def_function: ret_type was not a Typename" );
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Name        = get_cached_string( name );
		result->Type        = Function;
		result->ModuleFlags = mflags;

		Code
		body       = make_code();
		body->Type = Function_Body;

		result->add_entry( body );

		if ( attributes )
			result->add_entry( attributes );

		if ( specifiers )
			result->add_entry( specifiers );

		if ( ret_type )
			result->add_entry( ret_type );

		if ( params )
			result->add_entry( params );

		return result;
	}

	Code make_global_body( StrC name )
	{
		name_check( make_global_body, name );

		Code
		result       = make_code();
		result->Type = ECode::Global_Body;

		if ( name.Len > 0 )
			result->Name = get_cached_string( name );

		return result;
	}

	Code make_namespace( StrC name, Code parent, ModuleFlag mflags )
	{
		name_check( make_namespace, name );

		Code
		result = make_code();
		result->Type        = ECode::Namespace;
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		Code
		body = make_code();
		body->Type = ECode::Namespace_Body;

		result->add_entry( body );

		return result;
	}

	Code make_operator( OperatorT op, Code params_code, Code ret_type, Code specifiers, Code attributes, ModuleFlag mflags )
	{
		using namespace ECode;

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::make_operator: attributes was not a `Attributes` type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		OpValidateResult check_result = operator__validate( op, params_code, ret_type, specifiers );

		if ( check_result == OpValidateResult::Fail )
		{
			return Code::Invalid;
		}

		char const* name = str_fmt_buf( "operator%s", to_str(op) );

		Code
		result              = make_code();
		result->Name        = get_cached_string( { str_len(name, MaxNameLength), name } );
		result->ModuleFlags = mflags;

		if ( attributes )
			result->add_entry( attributes );

		if ( specifiers )
			result->add_entry( specifiers );

		if (params_code)
			result->add_entry( params_code );

		result->add_entry( ret_type );

		return result;
	}

	Code make_params()
	{
		Code
		result       = make_code();
		result->Type = ECode::Parameters;

		return result;
	}

	Code make_specifiers()
	{
		Code
		result       = make_code();
		result->Type = ECode::Specifiers;

		return result;
	}

	Code make_struct( StrC name, Code parent, Code specifiers, Code attributes, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( make_struct, name );

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::make_struct: attributes was not a `Attributes` type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::make_struct: specifiers was not a `Specifiers` type" );
			return Code::Invalid;
		}

		if ( parent && parent->Type != Struct )
		{
			log_failure( "gen::make_struct: parent was not a `Struct` type" );
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Type        = Struct;
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		Code
		body       = make_code();
		body->Type = Function_Body;

		result->add_entry( make_code() );

		if ( attributes )
			result->add_entry( attributes );

		if ( specifiers )
			result->add_entry( specifiers );

		if ( parent )
			result->add_entry( parent );

		return result;
	}

#	undef name_check
#	undef null_check
#	undef null_or_invalid_check
#endif // GEN_FEATURE_INCREMENTAL
#pragma endregion Incremetnal Constructions

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
	*/

	// Angle brackets not supported as they are used for template arguments outside of expressions
	// Any angle brackets found will be considered an operator token.

	#	define Define_TokType \
		Entry( Access_Private,      "private" )         \
		Entry( Access_Protected,    "protected" )       \
		Entry( Access_Public,       "public" )          \
		Entry( Access_MemberSymbol, "." )               \
		Entry( Access_StaticSymbol, "::")               \
		Entry( Ampersand,           "&" )               \
		Entry( Ampersand_DBL,       "&&" )              \
		Entry( Assign_Classifer,    ":" )               \
		Entry( BraceCurly_Open,     "{" )               \
		Entry( BraceCurly_Close,    "}" )               \
		Entry( BraceSquare_Open,    "[" )               \
		Entry( BraceSquare_Close,   "]" )               \
		Entry( Capture_Start,       "(" )               \
		Entry( Capture_End,         ")" )               \
		Entry( Comment,             "__comment__" )     \
		Entry( Char,                "__char__" )        \
		Entry( Comma,               "," )               \
		Entry( Decl_Class,          "class" )           \
		Entry( Decl_Enum,           "enum" )            \
		Entry( Decl_Friend,         "friend" )          \
		Entry( Decl_Module,         "module" )          \
		Entry( Decl_Namespace,      "namespace" )       \
		Entry( Decl_Struct,         "struct" )          \
		Entry( Decl_Typedef,        "typedef" )         \
		Entry( Decl_Using,          "using" )           \
		Entry( Decl_Union,          "union" )           \
		Entry( Identifier,          "__SymID__" )       \
		Entry( Module_Import,       "import" )          \
		Entry( Module_Export,       "export" )          \
		Entry( Number,              "number" )          \
		Entry( Operator,            "operator" )        \
		Entry( Spec_Alignas,        "alignas" )         \
		Entry( Spec_CLinkage,       "extern \"C\"" )    \
		Entry( Spec_Const,          "const" )           \
		Entry( Spec_Consteval,      "consteval" )       \
		Entry( Spec_Constexpr,      "constexpr" )       \
		Entry( Spec_Constinit,      "constinit" )       \
		Entry( Spec_Extern,         "extern" )          \
		Entry( Spec_Inline,         "inline" )          \
		Entry( Spec_Static,         "static" )          \
		Entry( Spec_ThreadLocal,    "thread_local" )    \
		Entry( Spec_Volatile,       "volatile")         \
		Entry( Star,                "*" )               \
		Entry( Statement_End,       ";" )               \
		Entry( String,              "__String__" )      \
		Entry( Type_Unsigned, 	    "unsigned" )        \
		Entry( Type_Signed,         "signed" )          \
		Entry( Type_Short,          "short" )           \
		Entry( Type_Long,           "long" )

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

		inline
		TokType get_tok_type( char const* word, s32 length )
		{
			local_persist
			char const* lookup[(u32)TokType::Num] =
			{
			#	define Entry( Name_, Str_ ) Str_,
				Define_TokType
			#	undef Entry

				Attribute::Keyword,
			};

			for ( u32 index = 0; index < (u32)TokType::Num; index++ )
			{
				if ( str_compare( word, lookup[index], length ) == 0 )
					return scast(TokType, index);
			}

			return TokType::Invalid;
		}

		inline
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

		inline
		bool tok_is_specifier( Token const& tok )
		{
			return tok.Type <= TokType::Star
				|| tok.Type >= TokType::Spec_Alignas
				|| tok.Type == TokType::Ampersand
				|| tok.Type == TokType::Ampersand_DBL
			;
		}

		inline
		bool tok_is_access_specifier( Token const& tok )
		{
			return tok.Type >= TokType::Access_Private && tok.Type <= TokType::Access_Public;
		}

		inline
		AccessSpec tok_to_access_specifier( Token const& tok )
		{
			return scast(AccessSpec, tok.Type);
		}

		Arena LexAllocator;

		struct TokArray
		{
			Array(Token) Arr;
			s32          Idx;

			inline
			bool __eat( TokType type, char const* context )
			{
				if ( array_count(Arr) - Idx <= 0 )
				{
					log_failure( "gen::%s: No tokens left", context );
					return Code::Invalid;
				}

				if ( Arr[0].Type != type )
				{
					String token_str = String::make( g_allocator, { Arr[Idx].Length, Arr[Idx].Text } );

					log_failure( "gen::%s: expected %s, got %s", context, str_tok_type(type), str_tok_type(Arr[Idx].Type) );

					return Code::Invalid;
				}

				Idx++;
			}
		};

		TokArray lex( StrC content )
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

			do_once_start
				arena_init_from_allocator( & LexAllocator, heap(), megabytes(10) );

				if ( LexAllocator.physical_start == nullptr )
				{
					log_failure( "gen::lex: failed to allocate memory for parsing constructor's lexer");
					return { nullptr, 0  };
				}
			do_once_end

			local_persist thread_local
			Array(Token) Tokens = nullptr;

			s32         left    = content.Len;
			char const* scanner = content.Ptr;

			char const* word        = scanner;
			s32         word_length = 0;

			SkipWhitespace();
			if ( left <= 0 )
			{
				log_failure( "gen::lex: no tokens found (only whitespace provided)" );
				return { nullptr, 0 };
			}

			if ( Tokens )
				array_clear( Tokens );

			array_init_reserve( Tokens, arena_allocator( & LexAllocator), content.Len / 8 );

			while (left )
			{
				Token token = { nullptr, 0, TokType::Invalid, false };

				switch ( current )
				{
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
						goto FoundToken;

					case '}':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::BraceCurly_Close;
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
						goto FoundToken;

					case '(':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Capture_Start;
						if ( left )
						{
							move_forward();

							if ( current == ')' )
							{
								token.Length = 2;
								token.Type   = TokType::Operator;
								move_forward();
							}
						}
						goto FoundToken;

					case ')':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Capture_End;
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
						goto FoundToken;

					case '*':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Star;
						goto FoundToken;

					case ';':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Statement_End;
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

				SkipWhitespace();
				if ( left <= 0 )
					break;

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
				else
				{
					String context_str = String::fmt_buf( g_allocator, "%s", scanner, min( 100, left ) );

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
					array_append( Tokens, token );
					continue;
				}

				TokType type = get_tok_type( token.Text, token.Length );

				if ( type != TokType::Invalid )
				{
					token.Type = type;
					array_append( Tokens, token );
				}
				else
				{
					// Its most likely an identifier...


					String tok_str = String::fmt_buf( g_allocator, "%s", token.Text, token.Length );

					log_failure( "Failed to lex token %s", tok_str );

					// Skip to next whitespace since we can't know if anything else is valid until then.
					while ( left && ! char_is_space( current ) )
					{
						move_forward();
					}
				}
			}

			if ( array_count(Tokens) == 0 )
			{
				log_failure( "Failed to lex any tokens" );
				return { nullptr, 0 };
			}

			return { Tokens, 0 };
		#	undef current
		#	undef move_forward
		#	undef SkipWhitespace
		#	undef SkipWhitespace_Checked
		}
	}

#pragma region Helper Macros
#	define check_parse_args( func, def )                                 \
	if ( def.Len <= 0 )                                                  \
	{                                                                    \
		log_failure( "gen::" txt(func) ": length must greater than 0" ); \
		return Code::Invalid;                                            \
	}                                                                    \
	if ( def.Ptr == nullptr )                                            \
	{                                                                    \
		log_failure( "gen::" txt(func) ": def was null" );               \
		return Code::Invalid;                                            \
	}

#	define currtok      toks.Arr[toks.Idx]
#	define eat( Type_ ) toks.__eat( Type_, context )
#	define left         array_count(toks.Arr) - toks.Idx

#	define check( Type_ ) left && currtok.Type == Type_
#pragma endregion Helper Macros

	Code parse_function_body( Parser::TokArray& toks, char const* context );
	Code parse_global_nspace( Parser::TokArray& toks, char const* context );

	Code parse_class        ( Parser::TokArray& toks, char const* context );
	Code parse_enum         ( Parser::TokArray& toks, char const* context );
	Code parse_export_body  ( Parser::TokArray& toks, char const* context );
	Code parse_exten_link   ( Parser::TokArray& toks, char const* context );
	Code parse_friend       ( Parser::TokArray& toks, char const* context );
	Code parse_function     ( Parser::TokArray& toks, char const* context );
	Code parse_struct       ( Parser::TokArray& toks, char const* context );
	Code parse_variable     ( Parser::TokArray& toks, char const* context );
	Code parse_type         ( Parser::TokArray& toks, char const* context );
	Code parse_typedef      ( Parser::TokArray& toks, char const* context );
	Code parse_union        ( Parser::TokArray& toks, char const* context );
	Code parse_using        ( Parser::TokArray& toks, char const* context );

	inline
	Code parse_array_decl( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		if ( check( TokType::BraceSquare_Open ) )
		{
			eat( TokType::BraceSquare_Open );

			if ( left == 0 )
			{
				log_failure( "%s: Error, unexpected end of typedef definition ( '[]' scope started )", txt(parse_typedef) );
				return Code::Invalid;
			}

			if ( currtok.Type == TokType::BraceSquare_Close )
			{
				log_failure( "%s: Error, empty array expression in typedef definition", txt(parse_typedef) );
				return Code::Invalid;
			}

			Token untyped_tok = currtok;

			while ( left && currtok.Type != TokType::BraceSquare_Close )
			{
				untyped_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)untyped_tok.Text;
			}

			Code array_expr = untyped_str( untyped_tok );

			if ( left == 0 )
			{
				log_failure( "%s: Error, unexpected end of type definition, expected ]", txt(parse_typedef) );
				return Code::Invalid;
			}

			if ( currtok.Type != TokType::BraceSquare_Close )
			{
				log_failure( "%s: Error, expected ] in type definition, not %s", txt(parse_typedef), str_tok_type( currtok.Type ) );
				return Code::Invalid;
			}

			eat( TokType::BraceSquare_Close );
			return array_expr;
		}

		return Code::Invalid;
	}

	inline
	Parser::Token parse_identifier( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		Token name = currtok;

		eat( TokType::Identifier );

		while ( left && currtok.Type == TokType::Access_StaticSymbol )
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

	Code parse_params( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		eat( TokType::Capture_Start );

		if ( check(TokType::Capture_End) )
		{
			eat( TokType::Capture_End );
			return { nullptr };
		}

		Code
		result = make_code();
		result->Type = Parameters;

		while ( left && currtok.Type != TokType::Capture_End)
		{
			Code type  = { nullptr };
			Code value = { nullptr };

			type = parse_type( toks, context );
			if ( type == Code::Invalid )
				return Code::Invalid;

			Token name = currtok;
			eat( TokType::Identifier );

			if ( currtok.IsAssign )
			{
				eat( TokType::Operator );

				Token value_tok = currtok;

				if ( currtok.Type == TokType::Statement_End )
				{
					log_failure( "gen::%s: Expected value after assignment operator", context );
					return Code::Invalid;
				}

				while ( left && currtok.Type != TokType::Statement_End )
				{
					value_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)value_tok.Text;
					eat( currtok.Type );
				}

				value = parse_type( toks, context );
			}

			Code
			param = make_code();
			param->Type = Parameters;
			param->Name = get_cached_string( name );

			param->add_entry( type );

			if ( value )
				param->add_entry( value );

			param.lock();

			result->add_entry( param );
			eat( TokType::Comma );
		}
		eat( TokType::Capture_End );

		result.lock();
		return result;
	#	undef context
	}

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

	Code parse_class_struct( Parser::TokType which, Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		return Code::Invalid;
	}

	Code parse_class_struct_body( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		eat( TokType::BraceCurly_Open );

		Code
		result = make_code();
		result->Type = Class_Body;

		while ( left && currtok.Type != TokType::BraceCurly_Close )
		{
			Code member = Code::Invalid;

			switch ( currtok.Type )
			{
				case TokType::Comment:
					member = def_comment( currtok );
					eat( TokType::Comment );
				break;

				case TokType::Access_Public:
					member = access_public;
					eat( TokType::Access_Public );
				break;

				case TokType::Access_Protected:
					member = access_protected;
					eat( TokType::Access_Protected );
				break;

				case TokType::Access_Private:
					member = access_private;
					eat( TokType::Access_Private );
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

				case TokType::Decl_Struct:
					member = parse_struct( toks, context );
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

				case TokType::Spec_Consteval:
				case TokType::Spec_Constexpr:
				case TokType::Spec_Constinit:
				case TokType::Spec_Inline:
				case TokType::Spec_Static:
				case TokType::Spec_ThreadLocal:
				case TokType::Spec_Volatile:
					Code specifiers = Code::Invalid;

					SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
					s32        num_specifiers = 0;

					while ( left && tok_is_specifier( currtok ) )
					{
						SpecifierT spec = ESpecifier::to_type( currtok );

						switch ( spec )
						{
							case ESpecifier::Constexpr:
							case ESpecifier::Constinit:
							case ESpecifier::External_Linkage:
							case ESpecifier::Local_Persist:
							case ESpecifier::Mutable:
							case ESpecifier::Static_Member:
							case ESpecifier::Thread_Local:
							case ESpecifier::Volatile:
							break;

							default:
								log_failure( "gen::parse_variable: invalid specifier " txt(spec) " for variable" );
								return Code::Invalid;
						}

						specs_found[num_specifiers] = spec;
						num_specifiers++;
						eat( currtok.Type );
					}

					if ( num_specifiers )
					{
						specifiers = def_specifiers( num_specifiers, specs_found );
					}

				case TokType::Identifier:
				case TokType::Spec_Const:
				case TokType::Type_Unsigned:
				case TokType::Type_Signed:
				case TokType::Type_Short:
				case TokType::Type_Long:
				{
					Code type = parse_type( toks, context );
					if ( type == Code::Invalid )
						return Code::Invalid;

					Token name = currtok;

					if ( check( TokType::Identifier ) )
					{
						name = currtok;
					}

					// Parsing a member function
					if ( check( TokType::Capture_Start ))
					{
						Code params = parse_params( toks, context );
						if ( params == Code::Invalid )
							return Code::Invalid;

						if ( check( TokType::BraceCurly_Open ) )
						{
							Code body = parse_function_body( toks, context);
							if ( body == Code::Invalid )
								return Code::Invalid;

							Code
							member       = make_code();
							member->Name = get_cached_string( name );

							if ( body )
							{
								switch ( body->Type )
								{
									case Function_Body:
									case Untyped:
										break;

									default:
									{
										log_failure("gen::def_function: body must be either of Function_Body or Untyped type. %s", body->debug_str());
										return Code::Invalid;
									}
								}

								member->Type = Function;
								member->add_entry( body );
							}
							else
							{
								member->Type = Function_Fwd;
							}

							member->add_entry( type );

							if ( params )
								member->add_entry( params );

							break;
						}
					}

					// Parsing a member variable
					Code array_expr = parse_array_decl( toks, context );
					Code expr       = parse_variable_assignment( toks, context );

					member       = make_code();
					member->Type = Variable;
					member->Name = get_cached_string( name );

					member->add_entry( type );

					if (array_expr)
						type->add_entry( array_expr );

					if (specifiers)
						member->add_entry( specifiers );

					if ( expr )
						member->add_entry( expr );
				}

				if ( member == Code::Invalid )
					return Code::Invalid;

				result->add_entry( member );
			}

			eat( currtok.Type );
		}

		eat( TokType::BraceCurly_Close );
		return result;
	}

	Code parse_function_body( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		eat( TokType::BraceCurly_Open );

		Code
		result = make_code();
		result->Type = Function_Body;

		while ( left && currtok.Type != TokType::BraceCurly_Close )
		{
			Code member = Code::Invalid;

			switch ( currtok.Type )
			{
				case TokType::Comment:
					member = def_comment( currtok );
					eat( TokType::Comment );
				break;

				case TokType::Decl_Class:
					member = parse_class( toks, context );
				break;

				case TokType::Decl_Enum:
					member = parse_enum( toks, context );
				break;

				case TokType::Decl_Struct:
					member = parse_struct( toks, context );
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

				case TokType::Identifier:
				break;

				case TokType::Spec_Const:
				case TokType::Spec_Constexpr:
				case TokType::Spec_Constinit:
				case TokType::Spec_Static:
				case TokType::Spec_ThreadLocal:
				case TokType::Spec_Volatile:
					// Can be all kinds of crap

				case TokType::Type_Unsigned:
				case TokType::Type_Signed:
				case TokType::Type_Short:
				case TokType::Type_Long:
					member = parse_variable( toks, context );
				break;

				default:
					Token untyped_tok = currtok;

					while ( left && currtok.Type != TokType::Statement_End )
					{
						untyped_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)untyped_tok.Text;
						eat( currtok.Type );
					}

					Code member = untyped_str( untyped_tok );
				break;
			}

			if ( member == Code::Invalid )
				return Code::Invalid;

			result->add_entry( member );

			eat( currtok.Type );
		}

		eat( TokType::BraceCurly_Close );
		return result;
	}

	Code parse_class( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Token name { nullptr, 0, TokType::Invalid };

		Code parent       = { nullptr };
		Code specifiers   = { nullptr };
		Code body         = { nullptr };
		Code lang_linkage = { nullptr };

		Code result = Code::Invalid;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		// Parse module specifiers
		eat( TokType::Decl_Class );

		// Parse specifiers
		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			switch ( spec )
			{
				case ESpecifier::External_Linkage:
				case ESpecifier::Local_Persist:
				case ESpecifier::Mutable:
				case ESpecifier::Static_Member:
				case ESpecifier::Thread_Local:
				case ESpecifier::Volatile:
				break;

				default:
					log_failure( "gen::parse_variable: invalid specifier " txt(spec) " for variable" );
					return Code::Invalid;
			}

			if ( spec == ESpecifier::External_Linkage )
			{
				specs_found[num_specifiers] = spec;
				num_specifiers++;
				eat( TokType::Spec_Extern );

				if ( currtok.Type == TokType::String )
				{
					lang_linkage = untyped_str( currtok );
					eat( TokType::String );
				}

				continue;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( num_specifiers )
		{
			specifiers = def_specifiers( num_specifiers, specs_found );
		}

		// Parse alignment

		Token name = parse_identifier( toks, context );

		if ( check( TokType::Assign_Classifer ) )
		{
			eat( TokType::Assign_Classifer );

			AccessSpec access = AccessSpec::Invalid;

			if ( tok_is_access_specifier( currtok ) )
			{
				access = tok_to_access_specifier( currtok );
			}

			Token parent_tok = parse_identifier( toks, context );
		}

		not_implemented();
	}

	Code parse_class( StrC def )
	{
		check_parse_args( parse_class, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_class( toks, txt(parse_class) );
	}

	Code parse_enum( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Token name       = { nullptr, 0, TokType::Invalid };
		Code  array_expr = { nullptr };
		Code  type       = { nullptr };
		Token body       = { nullptr, 0, TokType::Invalid };

		char  entries_code[ kilobytes(128) ] { 0 };
		s32   entries_length = 0;

		bool is_enum_class = false;

		eat( TokType::Decl_Enum );

		if ( currtok.Type == TokType::Decl_Class )
		{
			eat( TokType::Decl_Class);
			is_enum_class = true;
		}

		// TODO: Parse attributes

		if ( currtok.Type != TokType::Identifier )
		{
			log_failure( "gen::parse_enum: expected identifier for enum name" );
			return Code::Invalid;
		}

		name = currtok;
		eat( TokType::Identifier );

		if ( currtok.Type == TokType::Assign_Classifer )
		{
			eat( TokType::Assign_Classifer );

			type = parse_type( toks, txt(parse_enum) );
			if ( type == Code::Invalid )
				return Code::Invalid;
		}

		if ( currtok.Type == TokType::BraceCurly_Open )
		{
			eat( TokType::BraceCurly_Open );

			body = currtok;

			while ( currtok.Type != TokType::BraceCurly_Close )
			{
				body.Length += currtok.Length;

				eat( currtok.Type );
			}

			eat( TokType::BraceCurly_Close );
		}
		else
		{
			eat( TokType::Statement_End );
		}

		using namespace ECode;

		Code
		result = make_code();

		if ( body.Length )
		{
			mem_copy( entries_code, body.Text, body.Length );

			Code untyped_body = untyped_str( { entries_length, entries_code } );

			result->Type = is_enum_class ? Enum_Class : Enum;
			result->add_entry( untyped_body );
		}
		else
		{
			result->Type = is_enum_class ? Enum_Class_Fwd : Enum_Fwd;
		}

		result->Name = get_cached_string( name );

		if ( type )
			result->add_entry( type );

		result.lock();
		return result;
	}

	Code parse_enum( StrC def )
	{
		check_parse_args( parse_enum, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_enum( toks, txt(parse_enum) );
	}

	Code parse_export_body( Parser::TokArray& toks, char const* context )
	{
		not_implemented();
		return Code::Invalid;
	}

	Code parse_export_body( StrC def )
	{
		not_implemented();
		return Code::Invalid;
	}

	Code parse_extern_link( Parser::TokArray& toks, char const* context )
	{
		not_implemented();
		return Code::Invalid;
	}

	Code parse_extern_link( StrC def )
	{
		not_implemented();
		return Code::Invalid;
	}

	Code parse_friend( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		eat( TokType::Decl_Friend );

		Code function = { nullptr };

		// Type declaration or return type
		Code type = parse_type( toks, txt(parse_friend) );
		if ( type == Code::Invalid )
			return Code::Invalid;

		// Funciton declaration
		if ( currtok.Type == TokType::Identifier )
		{
			// Name
			Token name = parse_identifier( toks, txt(parse_friend) );

			// Parameter list
			Code params = parse_params( toks, txt(parse_friend) );

			function       = make_code();
			function->Type = Function_Fwd;
			function->Name = get_cached_string( name );
			function->add_entry( type );

			if ( params )
				function->add_entry( params );

			function.lock();
		}

		eat( TokType::Statement_End );

		Code
		result       = make_code();
		result->Type = Friend;

		if ( function )
			result->add_entry( function );

		else
			result->add_entry( type );

		result.lock();
		return result;
	}

	Code parse_friend( StrC def )
	{
		check_parse_args( parse_friend, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_friend( toks, txt(parse_friend) );
	}

	Code parse_functon( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		// TODO: Add attribute parsing support

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Code lang_linkage = Code::Invalid;
		Code attributes   = Code::Invalid;
		Code array_expr   = Code::Invalid;
		Code specifiers   = Code::Invalid;

		// TODO: Parse module specifiers

		// TODO: Parse attributes

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			switch ( spec )
			{
				case ESpecifier::Constexpr:
				case ESpecifier::External_Linkage:
				case ESpecifier::Local_Persist:
				case ESpecifier::Mutable:
				case ESpecifier::Static_Member:
				case ESpecifier::Thread_Local:
				case ESpecifier::Volatile:
				break;

				default:
					log_failure( "gen::parse_variable: invalid specifier " txt(spec) " for variable" );
					return Code::Invalid;
			}

			if ( spec == ESpecifier::External_Linkage )
			{
				specs_found[num_specifiers] = spec;
				num_specifiers++;
				eat( TokType::Spec_Extern );

				if ( currtok.Type == TokType::String )
				{
					lang_linkage = untyped_str( currtok );
					eat( TokType::String );
				}

				continue;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( num_specifiers )
		{
			specifiers = def_specifiers( num_specifiers, specs_found );
		}

		Code ret_type = parse_type( toks, txt(parse_function) );
		if ( ret_type == Code::Invalid )
			return Code::Invalid;

		Token name = parse_identifier( toks, txt(parse_function) );
		if ( ! name )
			return Code::Invalid;

		Code params = parse_params( toks, txt(parse_function) );

		Code body = Code::Invalid;
		if ( check( TokType::BraceCurly_Open ) )
		{
			body = parse_function_body( toks, txt(parse_function) );
			if ( body == Code::Invalid )
				return Code::Invalid;
		}
		else
		{
			eat( TokType::Statement_End );
		}

		using namespace ECode;

		Code
		result       = make_code();
		result->Name = get_cached_string( name );

		if ( body )
		{
			switch ( body->Type )
			{
				case Function_Body:
				case Untyped:
					break;

				default:
				{
					log_failure("gen::def_function: body must be either of Function_Body or Untyped type. %s", body->debug_str());
					return Code::Invalid;
				}
			}

			result->Type = Function;
			result->add_entry( body );
		}
		else
		{
			result->Type = Function_Fwd;
		}

		if ( specifiers )
			result->add_entry( specifiers );

		result->add_entry( ret_type );

		if ( params )
			result->add_entry( params );

		result.lock();
		return result;
	}

	Code parse_function( StrC def )
	{
		using namespace Parser;

		check_parse_args( parse_function, def );

		Arena mem;
		do_once_start
			arena_init_from_allocator( & mem, heap(), kilobytes( 10 ) );
		do_once_end

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_functon( toks, txt(parse_function) );
	}

	Code parse_global_body( Parser::TokArray& toks, char const* context )
	{
		not_implemented();
	}

	Code parse_global_body( StrC def )
	{
		using namespace Parser;
		using namespace ECode;

		Code result = make_code();
		result->Type = Global_Body;

		// while ( left )
		// {

		// }

		result.lock();
		return result;
	}

	Code parse_namespace( Parser::TokArray& toks, char const* context )
	{
		not_implemented();
	}

	Code parse_namespace( StrC def )
	{
		not_implemented( parse_namespace );
	}

	Code parse_operator( Parser::TokArray& toks, char const* context )
	{
		not_implemented();
	}

	Code parse_operator( StrC def )
	{
		not_implemented( parse_operator );
	}

	Code parse_struct( Parser::TokArray& toks, char const* context )
	{
		not_implemented();
	}

	Code parse_struct( StrC def )
	{
		Arena mem;
		do_once_start
			arena_init_from_allocator( & mem, heap(), kilobytes( 10 ) );
		do_once_end

		// Pretty sure its impossible to have more than this.
		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };

		u8 num_specifiers;

		// Making all significant tokens have a max length of 128 for this parser.
		constexpr sw LengthID = 128;

		char const name  [LengthID] { 0 };
		char const parent[LengthID] { 0 };

		return Code::Invalid;
	}

	Code parse_type( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Token name = { nullptr, 0, TokType::Invalid };

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			if (   spec != ESpecifier::Const
				&& spec < ESpecifier::Type_Signed )
			{
				log_failure( "gen::parse_type: Error, invalid specifier used in type definition: %s", currtok.Text );
				return Code::Invalid;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( left == 0 )
		{
			log_failure( "%s: Error, unexpected end of type definition", context );
			return Code::Invalid;
		}

		if (   currtok.Type == TokType::Decl_Class
			|| currtok.Type == TokType::Decl_Struct )
		{
			name = currtok;
			eat( currtok.Type );

			name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
			eat( TokType::Identifier );
		}
		else
		{
			name = parse_identifier( toks, context );
			if ( ! name )
				return Code::Invalid;
		}

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			if (   spec != ESpecifier::Const
			    && spec != ESpecifier::Ptr
			    && spec != ESpecifier::Ref
			    && spec != ESpecifier::RValue
				&& spec < ESpecifier::Type_Signed )
			{
				log_failure( "%s: Error, invalid specifier used in type definition: %s", context, currtok.Text );
				return Code::Invalid;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		using namespace ECode;

		// TODO: Need to figure out type code caching wiht the type table.

		Code
		result       = make_code();
		result->Type = Typename;
		result->Name = get_cached_string( name );

		if (num_specifiers)
		{
			Code specifiers = def_specifiers( num_specifiers, specs_found );

			result->add_entry( specifiers );
		}

		return result;
	}

	Code parse_type( StrC def )
	{
		check_parse_args( parse_type, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		Code result = parse_type( toks, txt(parse_type) );

		result.lock();
		return result;
	}

	Code parse_typedef( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Token name       = { nullptr, 0, TokType::Invalid };
		Code  array_expr = { nullptr };
		Code  type       = { nullptr };

		eat( TokType::Decl_Typedef );

		type = parse_type( toks, txt(parse_typedef) );

		if ( check( TokType::Identifier ) )
		{
			log_failure( "gen::parse_typedef: Error, expected identifier for typedef" );
			return Code::Invalid;
		}

		name = currtok;
		eat( TokType::Identifier );

		array_expr = parse_array_decl( toks, txt(parse_typedef) );

		eat( TokType::Statement_End );

		using namespace ECode;

		Code
		result       = make_code();
		result->Type = Typedef;
		result->Name = get_cached_string( name );

		result->add_entry( type );

		if ( array_expr )
			type->add_entry( array_expr );

		result.lock();
		return result;
	}

	Code parse_typedef( StrC def )
	{
		check_parse_args( parse_typedef, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_typedef( toks, txt(parse_typedef) );
	}

	Code parse_union( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		not_implemented();

		return Code::Invalid;
	}

	Code parse_union( s32 length, char const* def )
	{
		not_implemented( parse_union );
	}

	Code parse_using( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Token* name       = nullptr;
		Code   array_expr = { nullptr };
		Code   type       = { nullptr };

		bool is_namespace = false;

		eat( TokType::Decl_Using );

		if ( currtok.Type == TokType::Decl_Namespace )
		{
			is_namespace = true;
			eat( TokType::Decl_Namespace );
		}

		// TODO: Parse Attributes

		eat( TokType::Identifier );

		if ( currtok.IsAssign )
		{
			eat( TokType::Operator );

			type = parse_type( toks, txt(parse_typedef) );
		}

		array_expr = parse_array_decl( toks, txt(parse_typedef) );

		eat( TokType::Statement_End );

		using namespace ECode;

		Code
		result       = make_code();
		result->Type = is_namespace ? Using : Using_Namespace;
		result->Name = get_cached_string( * name );

		result->add_entry( type );

		if ( array_expr )
			type->add_entry( array_expr );

		result.lock();
		return result;
	}

	Code parse_using( StrC def )
	{
		check_parse_args( parse_using, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_using( toks, txt(parse_using) );
	}

	Code parse_variable( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Token name = { nullptr, 0, TokType::Invalid };

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Code lang_linkage = Code::Invalid;
		Code attributes   = Code::Invalid;
		Code array_expr   = Code::Invalid;
		Code specifiers   = Code::Invalid;

		// TODO: Parse module specifiers

		// TODO: Parse attributes
		if ( check( TokType::BraceSquare_Open ) )
		{
			eat( TokType::BraceSquare_Open );

		// TODO: Need to have a parser for attributes, it gets complicated...
		#if 0
			Token attris_tok = currtok;

			while ( left && currtok.Type != TokType::BraceSquare_Close )
			{

				eat( TokType::Identifier )
			}
		#endif

			attributes = untyped_str( currtok );

			eat( TokType::BraceSquare_Close );
		}

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			switch ( spec )
			{
				case ESpecifier::Constexpr:
				case ESpecifier::Constinit:
				case ESpecifier::External_Linkage:
				case ESpecifier::Local_Persist:
				case ESpecifier::Mutable:
				case ESpecifier::Static_Member:
				case ESpecifier::Thread_Local:
				case ESpecifier::Volatile:
				break;

				default:
					log_failure( "gen::parse_variable: invalid specifier " txt(spec) " for variable" );
					return Code::Invalid;
			}

			if ( spec == ESpecifier::External_Linkage )
			{
				specs_found[num_specifiers] = spec;
				num_specifiers++;
				eat( TokType::Spec_Extern );

				if ( currtok.Type == TokType::String )
				{
					lang_linkage = untyped_str( currtok );
					eat( TokType::String );
				}

				continue;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( num_specifiers )
		{
			specifiers = def_specifiers( num_specifiers, specs_found );
		}

		Code type = parse_type( toks, txt(parse_variable) );

		if ( type == Code::Invalid )
			return Code::Invalid;

		if ( currtok.Type != TokType::Identifier )
		{
			log_failure( "gen::parse_variable: expected identifier, recieved " txt(currtok.Type) );
			return Code::Invalid;
		}

		name = currtok;
		eat( TokType::Identifier );

		array_expr = parse_array_decl( toks, txt(parse_variable) );

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

		eat( TokType::Statement_End );

		using namespace ECode;

		Code result  = make_code();
		result->Type = Variable;
		result->Name = get_cached_string( name );

		result->add_entry( type );

		if (array_expr)
			type->add_entry( array_expr );

		if ( attributes )
			result->add_entry( attributes );

		if (specifiers)
			result->add_entry( specifiers );

		if ( expr )
			result->add_entry( expr );

		result.lock();
		return result;
	}

	Code parse_variable( StrC def )
	{
		check_parse_args( parse_variable, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_variable( toks, txt(parse_variable) );
	}

	// Undef helper macros
#	undef check_parse_args
#	undef curr_tok
#	undef eat
#	undef left
//	End GEN_FEATURE_PARSING
#	endif
#pragma endregion Parsing Constructors

#pragma region Untyped Constructors
	struct TokEntry
	{
		char const* Str;
		sw          Length;
	};

	ZPL_TABLE( static, TokMap, tokmap_, TokEntry )

	sw token_fmt_va( char* buf, uw buf_size, char const* fmt, s32 num_tokens, va_list va )
	{
		char const* buf_begin = buf;
		sw          remaining = buf_size;

		TokMap tok_map;
		{
			tokmap_init( & tok_map, g_allocator );

			s32 left = num_tokens;

			while ( left-- )
			{
				char const* token = va_arg( va, char const* );
				char const* value = va_arg( va, char const* );

				TokEntry entry
				{
					value,
					str_len(value, (sw)128)
				};

				u32 key = crc32( token, str_len(token, 32) );

				tokmap_set( & tok_map, key, entry );
			}
		}

		sw   result  = 0;
		char current = *fmt;

		while ( current )
		{
			sw len = 0;

			while ( current && current != '{' && remaining )
			{
				* buf = * fmt;
				buf++;
				fmt++;

				current = * fmt;
			}

			if ( current == '{' )
			{
				char const* scanner = fmt;

				s32 tok_len = 0;

				while ( *scanner != '}' )
				{
					tok_len++;
					scanner++;
				}

				char const* token = fmt;

				u32      key   = crc32( token, tok_len );
				TokEntry value = * tokmap_get( & tok_map, key );
				sw       left  = value.Length;

				while ( left-- )
				{
					* buf = *value.Str;
					buf++;
					value.Str++;
				}

				scanner++;
				fmt     = scanner;
				current = * fmt;
			}
		}

		tokmap_clear( & tok_map );

		return result;
	}

	Code untyped_str( StrC content )
	{
		Code
		result          = make_code();
		result->Name    = get_cached_string( content );
		result->Type    = ECode::Untyped;
		result->Content = result->Name;

		result.lock();
		return result;
	}

	Code untyped_fmt( char const* fmt, ...)
	{
		local_persist thread_local
		char buf[ZPL_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		sw length = str_fmt_va(buf, ZPL_PRINTF_MAXLEN, fmt, va);
		va_end(va);

		Code
		result          = make_code();
		result->Name    = get_cached_string( { str_len(fmt, MaxNameLength), fmt } );
		result->Type    = ECode::Untyped;
		result->Content = get_cached_string( { length, buf } );

		result.lock();
		return result;
	}

	Code untyped_token_fmt( char const* fmt, s32 num_tokens, ... )
	{
		local_persist thread_local
		char buf[ZPL_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		sw length = token_fmt_va(buf, ZPL_PRINTF_MAXLEN, fmt, num_tokens, va);
		va_end(va);

		Code
		result          = make_code();
		result->Name    = get_cached_string( { str_len(fmt, MaxNameLength), fmt } );
		result->Type    = ECode::Untyped;
		result->Content = get_cached_string( { length, buf } );

		result.lock();
		return result;
	}
#pragma endregion Untyped Constructors
#pragma endregion Gen Interface

#pragma region Builder
	void Builder::print( Code code )
	{
		Buffer.append_fmt( "%s\n", code.to_string() );
	}

	void Builder::print_fmt( char const* fmt, ... )
	{
		sw   res;
		char buf[ ZPL_PRINTF_MAXLEN ] = { 0 };

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

		Buffer = String::make( g_allocator, "" );

		return true;
	}

	void Builder::write()
	{
		bool result = file_write( & File, Buffer, Buffer.length() );

		if ( result == false )
			log_failure("gen::File::write - Failed to write to file: %s", file_name( & File ) );

		// file_seek( & File, 0 );
		file_close( & File );
	}
#pragma endregion Builder

#pragma region File Lexer
// The Editor and Scanner require this lexer.
#if defined(GEN_FEATURE_EDITOR) || defined(GEN_FEATURE_SCANNER)
/*
	This is a more robust lexer than the ones used for the lexer in the parse constructors interface.
	Its needed to scan a C++ file and have awareness to skip content unsupported by the library.
*/
struct FileLexer
{

};
#endif
#pragma endregion File Lexer

#pragma region Editor
#ifdef GEN_FEATURE_EDITOR
#endif
#pragma endregion Editor

#pragma region Scanner
#ifdef GEN_FEATURE_SCANNER
#endif
#pragma endregion Scanner
}
// End: gen_time
#endif
