// ReSharper disable CppClangTidyClangDiagnosticSwitchEnum

#ifdef gen_time

#include "Bloat.hpp"
#include "gen.hpp"

namespace gen
{
	ZPL_TABLE_DEFINE( StringTable, str_tbl_, String );

	namespace StaticData
	{
		global Array(Pool)  CodePools         = nullptr;
		global Array(Arena) CodeEntriesArenas = nullptr;
		global Array(Arena) StringArenas      = nullptr;

		global StringTable StringMap;

		global AllocatorInfo Allocator_DataArrays       = heap();
		global AllocatorInfo Allocator_CodePool         = heap();
		global AllocatorInfo Allocator_CodeEntriesArena = heap();
		global AllocatorInfo Allocator_StringArena      = heap();
		global AllocatorInfo Allocator_StringTable      = heap();
		global AllocatorInfo Allocator_TypeTable        = heap();
	}

#pragma region Constants
	global Code t_auto;
	global Code t_void;
	global Code t_int;
	global Code t_bool;
	global Code t_char;
	global Code t_wchar_t;

	#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
	global Code t_b32;

	global Code t_s8;
	global Code t_s16;
	global Code t_s32;
	global Code t_s64;

	global Code t_u8;
	global Code t_u16;
	global Code t_u32;
	global Code t_u64;

	global Code t_sw;
	global Code t_uw;

	global Code t_f32;
	global Code t_f64;
	#endif

	global Code access_public;
	global Code access_protected;
	global Code access_private;

	global Code module_global_fragment;
	global Code module_private_fragment;

	global Code pragma_once;

	global Code spec_const;
	global Code spec_consteval;
	global Code spec_constexpr;
	global Code spec_constinit;
	global Code spec_extern_linkage;
	global Code spec_global;
	global Code spec_inline;
	global Code spec_internal_linkage;
	global Code spec_local_persist;
	global Code spec_mutable;
	global Code spec_ptr;
	global Code spec_ref;
	global Code spec_register;
	global Code spec_rvalue;
	global Code spec_static_member;
	global Code spec_thread_local;
	global Code spec_volatile;
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
	Code Code::Global;
	Code Code::Invalid;

	AST* AST::duplicate()
	{
		using namespace ECode;

		Code
		result           = make_code();
		result->Parent   = Parent;
		result->Name     = Name;
		result->Type     = Type;
		result->Op       = Op;

		switch ( Type )
		{
			case Invalid:
				log_failure( "AST::duplicate: Cannot duplicate an invalid AST." );
				return nullptr;

			case Access_Public:
			case Access_Protected:
			case Access_Private:
			case Attributes:
			case Comment:
			case Execution:
			case Module:
			case Preprocessor_Include:
			case Untyped:
				// Can just be the same, as its a cached string.
				result->Content = Content;
				return result;

			case Specifiers:
				// Need to memcpy the specifiers.
				mem_copy( result->ArrSpecs, ArrSpecs, sizeof( ArrSpecs ) );
				result->StaticIndex = StaticIndex;
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

		String result = String::make( Memory::GlobalAllocator, "" );

		switch ( Type )
		{
			using namespace ECode;

			case Invalid:
				log_failure("Attempted to serialize invalid code! - %s", Name);
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

			case Attributes:
				result.append( Content );

			case Class:
			{
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

					AST const* parent = entry( idx );

					if ( parent )
					{
						char const* access_level = to_str( ParentAccess );

						result.append_fmt( ": %s %s\n{\n"
							, access_level
							, parent
						);
					}
					else
					{
						result.append( "\n{\n" );
					}

					result.append_fmt( "%s\n};", body()->to_string() );
				}
				else
				{
					result.append_fmt( "class %s\n{\n%s\n};", Name, body()->to_string() );
				}
			}
			break;

			case Class_Fwd:
			{
				ProcessModuleFlags();

				s32 idx = 1;
				if ( entry(idx) && entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "class %s;", Name );
			}
			break;

			case Enum:
			{
				ProcessModuleFlags();

				result.append( "enum " );

				if ( num_entries() > 1 )
				{
					s32 idx = 1;

					AST const* current = entry( idx);

					if ( current->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
					}

					if ( current->Type == Typename)
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

				result.append_fmt( "%s};\n"
					, body()->to_string()
				);
			}
			break;

			case Enum_Fwd:
			{
				ProcessModuleFlags();

				s32 idx = 1;
				if ( entry(idx) && entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "enum %s : %s;", Name, entry( 0 )->to_string() );
			}
			break;

			case Enum_Class:
			{
				ProcessModuleFlags();

				result.append( "enum class " );

				if ( num_entries()  > 1 )
				{
					s32 idx = 1;

					if ( entry( idx )->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
					}

					if ( entry( idx )->Type == Typename )
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

				result.append_fmt( "%s\n};"
					, body()->to_string()
				);
			}
			break;

			case Enum_Class_Fwd:
			{
				ProcessModuleFlags();

				result.append( "enum class " );

				s32 idx = 0;

				if ( entry(idx) && entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s : %s;", Name, entry( idx )->to_string() );
			}
			break;

			case Export_Body:
			{
				result.append_fmt( "export\n{\n" );

				s32 index = 0;
				s32 left  = num_entries();
				while ( left -- )
				{
					result.append_fmt( "%s\n", entry( index )->to_string() );
					index++;
				}

				result.append_fmt( "};" );
			}
			break;

			case Extern_Linkage:
				ProcessModuleFlags();

				result.append_fmt( "extern \"%s\"\n{\n%s\n}"
					, Name
					, body()->to_string()
				);
			break;

			case Friend:
				result.append_fmt( "friend %s;", entry( 0 )->to_string() );
			break;

			case Function:
			{
				ProcessModuleFlags();

				u32 idx  = 1;
				u32 left = num_entries();

				AST* current = entry( idx );

				if ( current && current->Type == Attributes )
				{
					result.append_fmt( "%s ", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}

				if ( current && current->Type == Specifiers )
				{
					result.append_fmt( "%s\n", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}

				if ( current && current->Type == Typename )
				{
					result.append_fmt( "%s ", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}

				result.append_fmt( "%s(", Name );

				if ( left && current && current->Type == Parameters )
				{
					result.append_fmt("%s", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}
				else
				{
					result.append( "void" );
				}

				result.append_fmt( ")\n{\n%s\n}"
					, body()->to_string()
				);
			}
			break;

			case Function_Fwd:
			{
				ProcessModuleFlags();

				u32 idx  = 0;
				u32 left = num_entries();

				AST* current = entry( idx );

				if ( current && current->Type == Attributes )
				{
					result.append_fmt( "%s ", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}

				if ( current && current->Type == Specifiers )
				{
					result.append_fmt( "%s\n", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}

				if ( current && current->Type == Typename )
				{
					result.append_fmt( "%s ", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}

				result.append_fmt( "%s(", Name );

				if ( left && current && current->Type == Parameters )
				{
					result.append_fmt("%s", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}
				else
				{
					result.append( "void" );
				}

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
					, body()->to_string()
				);
			break;

			case Operator:
			case Operator_Member:
			{
				ProcessModuleFlags();

				s32 idx = 1;

				if ( entry(idx) && entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				if ( entry(idx) && entry( idx )->Type == Specifiers )
				{
					result.append_fmt( "%s\n", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s %s (", entry( idx )->to_string(), Name );
				idx++;

				if ( entry(idx) && entry( idx )->Type == Parameters )
				{
					result.append_fmt( "%s", entry( idx )->to_string() );
					idx++;
				}
				else
				{
					result.append_fmt( "void" );
				}

				result.append_fmt( ")\n{\n%s\n}"
					, body()->to_string()
				);
			}
			break;

			case Operator_Fwd:
			case Operator_Member_Fwd:
			{
				ProcessModuleFlags();

				s32 idx = 0;

				if ( entry(idx) && entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				if ( entry(idx) && entry( idx )->Type == Specifiers )
				{
					result.append_fmt( "%s", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s %s (", entry( idx )->to_string(), Name );
				idx++;

				if ( entry(idx) && entry( idx )->Type == Parameters )
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
				s32 left  = num_entries() - 1;

				while ( left-- )
				{
					result.append_fmt( ", %s %s"
						, entry( index )->entry( 0 )->to_string()
						, entry( index )->Name
					);
					index++;
				}
			}
			break;

			case Preprocessor_Include:
				result.append_fmt( "#include \"%s\"", Name );
			break;

			case Specifiers:
			{
				s32 idx  = 0;
				s32 left = StaticIndex;
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

				result.append( "struct ");

				if ( num_entries() > 1 )
				{
					s32 idx = 1;

					AST* current = entry( idx );

					if ( current && current->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
						current = entry( idx );
					}

					result.append_fmt( "%s", Name );

					if ( current )
					{
						switch ( ParentAccess )
						{
							case AccessSpec::Private:
							case AccessSpec::Protected:
							case AccessSpec::Public:
								result.append_fmt( " : %s %s", to_str(ParentAccess), current->to_string() );
								idx++;
								break;

							default:
								result.append_fmt( " : %s", current->to_string() );
								break;
						}
					}

					result.append_fmt( "\n{\n%s\n};", body()->to_string() );
					break;
				}

				result.append_fmt( "%s\n{\n%s\n};", Name, body()->to_string() );
			}
			break;

			case Struct_Fwd:
			{
				ProcessModuleFlags();

				result.append( "struct ");

				s32 idx = 0;

				if ( entry(idx) && entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s;", Name );
			}
			break;

			case Variable:
			{
				ProcessModuleFlags();

				s32 idx = 1;

				if ( num_entries() > 1 )
				{
					if ( entry( idx )->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
					}

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
				ProcessModuleFlags();

				AST* type = entry( 0 );

				result.append( "typedef ");

				if ( entry( 1 ) && entry( 1 )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( 1 )->to_string() );
				}

				result.append_fmt( "%s %s", type->to_string(), Name );

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
				if ( num_entries() )
				{
					s32 idx = 0;

					if ( entry(idx)->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
					}

					if ( entry(idx)->Type == Specifiers )
					{
						result.append_fmt( "%s %s", Name, entry( idx )->to_string() );
						idx++;
					}

					result.append_fmt( "%s", Name );
				}
				else
				{
					result.append_fmt( "%s", Name );
				}
			break;

			case Union:
			{
				ProcessModuleFlags();

				s32 idx = 0;

				result.append( "union " );

				if ( entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s\n{\n%s\n};"
					, Name
					, body()->to_string()
				);
			}
			break;

			case Using:
			{
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
				result.append_fmt( "using namespace %s;", Name );
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
					result.append_fmt( "%s\n", entry( index )->to_string() );
					index++;
				}
			}
			break;
		}

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

				if ( num_entries() != other->num_entries() )
					return false;

				for ( s32 i = 0; i < num_entries(); ++i )
				{
					if ( entry( i ) != other->entry( i ) )
						return false;
				}

				return true;
			}
		}

		if ( Name != other->Name )
			return false;

		return true;
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
		}

		Code::Global          = make_code();
		Code::Global->Name    = get_cached_string( name("Global Code") );
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
		access_private->Name = get_cached_string( StrC::from("private:") );
		access_private.set_global();

		access_protected = make_code();
		access_protected->Type = ECode::Access_Protected;
		access_protected->Name = get_cached_string( StrC::from("protected:") );
		access_protected.set_global();

		access_public = make_code();
		access_public->Type = ECode::Access_Public;
		access_public->Name = get_cached_string( StrC::from("public:") );
		access_public.set_global();

		module_global_fragment          = make_code();
		module_global_fragment->Type    = ECode::Untyped;
		module_global_fragment->Name    = get_cached_string( StrC::from("module;") );
		module_global_fragment->Content = module_global_fragment->Name;
		module_global_fragment.set_global();

		module_private_fragment          = make_code();
		module_private_fragment->Type    = ECode::Untyped;
		module_private_fragment->Name    = get_cached_string( StrC::from("module : private;") );
		module_private_fragment->Content = module_private_fragment->Name;
		module_private_fragment.set_global();

		pragma_once          = make_code();
		pragma_once->Type    = ECode::Untyped;
		pragma_once->Name    = get_cached_string( StrC::from("#pragma once") );
		pragma_once->Content = pragma_once->Name;
		pragma_once.set_global();

	#	pragma push_macro( "global" )
	#	pragma push_macro( "internal" )
	#	pragma push_macro( "local_persist" )
	#	define global        global
	#	define internal      internal
	#	define local_persist local_persist

	#	define def_constant_spec( Type_, ... )                                    \
		spec_##Type_ = def_specifiers( macro_num_args(__VA_ARGS__), __VA_ARGS__); \
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
		def_constant_spec( static_member,    ESpecifier::Static_Member );
		def_constant_spec( thread_local,     ESpecifier::Thread_Local );
		def_constant_spec( volatile, 	     ESpecifier::Volatile)

		spec_local_persist = def_specifiers( 1, ESpecifier::Local_Persist );
		spec_local_persist.set_global();

	#	pragma pop_macro( "global" )
	#	pragma pop_macro( "internal" )
	#	pragma pop_macro( "local_persist" )

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

	inline
	AllocatorInfo get_string_allocator( s32 str_length )
	{
		using namespace StaticData;

		Arena* last = & array_back( StringArenas );

		if ( last->total_allocated + str_length > last->total_size )
		{
			Arena new_arena;
			arena_init_from_allocator( & new_arena, Allocator_StringArena, SizePer_StringArena );

			if ( ! array_append( StringArenas, new_arena ) )
				fatal( "gen::get_string_allocator: Failed to allocate a new string arena" );

			last = & array_back( StringArenas );
		}

		return arena_allocator( last );
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

		String result = String::make( get_string_allocator( str.Len ), str );

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
							if ( params_code->param_type()->is_equal( t_int ) )
								is_member_symbol = true;

							else
								check_param_eq_ret();
						break;

						case 2:
							check_param_eq_ret();

							if ( ! params_code->get_param(1)->is_equal( t_int ) )
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

					if ( params_code->param_type()->is_equal( ret_type ) )
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
						if ( ! params_code->param_type()->is_equal( ret_type ) )
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

				if ( ! ret_type->is_equal( t_bool ))
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
	The library validates a good protion of their form and thus the argument processing for is quite a bit.
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
				case Execution:
				case Untyped:
					break;

				default:
				{
					log_failure("gen::def_function: body must be either of Function_Body, Execution, or Untyped type. %s", body->debug_str());
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
			result->add_entry( t_void );
		}

		if ( params )
			result->add_entry( params );

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

		return result;
	}

	Code def_operator( OperatorT op
		, Code params_code, Code ret_type, Code body
		, Code specifiers, Code attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

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

		char const* name = str_fmt_buf( "operator %s", to_str(op) );

		Code
		result              = make_code();
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
					return Code::Invalid;
				}
			}

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

		return result;
	}

	Code def_specifier( SpecifierT spec )
	{
		Code
		result       = make_code();
		result->Type = ECode::Specifiers;

		result->add_specifier( spec );

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

		if ( parent && parent->Type != Typename )
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

		return result;
	}

	Code def_type( StrC name, Code arrayexpr, Code specifiers, Code attributes )
	{
		name_check( def_type, name );

		if ( attributes && attributes->Type != ECode::Attributes )
		{
			log_failure( "gen::def_type: attributes is not of attributes type - %s", attributes->debug_str() );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != ECode::Specifiers )
		{
			log_failure( "gen::def_type: specifiers is not of specifiers type - %s", specifiers->debug_str() );
			return Code::Invalid;
		}

		if ( arrayexpr && arrayexpr->Type != ECode::Untyped )
		{
			log_failure( "gen::def_type: arrayexpr is not of untyped type - %s", arrayexpr->debug_str() );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Name = get_cached_string( name );
		result->Type = ECode::Typename;

		if ( attributes )
			result->add_entry( attributes );

		if ( specifiers )
			result->add_entry( specifiers );

		if ( arrayexpr )
			result->add_entry( arrayexpr );

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

		if ( attributes && attributes->Type != ECode::Attributes )
		{
			log_failure( "gen::def_typedef: attributes was not a Attributes" );
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

		return result;
	}

	Code def_union( StrC name, Code body, Code attributes, ModuleFlag mflags )
	{
		name_check( def_union, name );
		null_check( def_union, body );

		if ( body->Type != ECode::Union_Body )
		{
			log_failure( "gen::def_union: body was not a Union_Body type - %s", body->debug_str() );
			return Code::Invalid;
		}

		if ( attributes && attributes->Type != ECode::Attributes )
		{
			log_failure( "gen::def_union: attributes was not a Attributes type - %s", attributes->debug_str() );
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

		if ( attributes && attributes->Type != ECode::Attributes )
		{
			log_failure( "gen::def_using: attributes was not a Attributes type - %s", attributes->debug_str() );
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

		return result;
	}

	Code def_params( s32 num, ... )
	{
		def_body_start( def_params );

		va_list va;
		va_start(va, num);

		Code_POD pod   = va_arg(va, Code_POD);
		Code     param = pcast( Code, pod );

		null_check( def_params, param );

		if ( param->Type != Parameters )
		{
			log_failure( "gen::def_params: param %d is not a Parameters", num - num + 1 );
			return Code::Invalid;
		}

		Code result = { param->duplicate() };

		while ( -- num )
		{
			pod   = va_arg(va, Code_POD);
			param = pcast( Code, pod );

			if ( param->Type != Parameters )
			{
				log_failure( "gen::def_params: param %d is not a Parameters", num - num + 1 );
				return Code::Invalid;
			}

			result->add_entry( param );
		}
		va_end(va);

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

		result->add_entry( current->entry( 0 ) );

		while( codes++, current = * codes, num--, num > 0 )
		{
			check_current();
			result->add_entry( current );
		}
	#	undef check_current

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

		return result;
	}

	Code def_specifiers( s32 num, SpecifierT* specs )
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

		s32 idx = 0;
		do
		{
			result->add_specifier( specs[idx] );
			idx++;
		}
		while ( --num, num );

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

		return result;
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
	*/

	// Angle brackets not supported as they are used for template arguments outside of expressions
	// Any angle brackets found will be considered an operator token.

	#	define Define_TokType \
		Entry( Access_Private,        "private" )         \
		Entry( Access_Protected,      "protected" )       \
		Entry( Access_Public,         "public" )          \
		Entry( Access_MemberSymbol,   "." )               \
		Entry( Access_StaticSymbol,   "::")               \
		Entry( Ampersand,             "&" )               \
		Entry( Ampersand_DBL,         "&&" )              \
		Entry( Assign_Classifer,      ":" )               \
		Entry( BraceCurly_Open,       "{" )               \
		Entry( BraceCurly_Close,      "}" )               \
		Entry( BraceSquare_Open,      "[" )               \
		Entry( BraceSquare_Close,     "]" )               \
		Entry( Capture_Start,         "(" )               \
		Entry( Capture_End,           ")" )               \
		Entry( Comment,               "__comment__" )     \
		Entry( Char,                  "__char__" )        \
		Entry( Comma,                 "," )               \
		Entry( Decl_Class,            "class" )           \
		Entry( Decl_Enum,             "enum" )            \
		Entry( Decl_Extern_Linkage,   "extern" )          \
		Entry( Decl_Friend,           "friend" )          \
		Entry( Decl_Module,           "module" )          \
		Entry( Decl_Namespace,        "namespace" )       \
		Entry( Decl_Operator,         "operator" )        \
		Entry( Decl_Struct,           "struct" )          \
		Entry( Decl_Typedef,          "typedef" )         \
		Entry( Decl_Using,            "using" )           \
		Entry( Decl_Union,            "union" )           \
		Entry( Identifier,            "__SymID__" )       \
		Entry( Module_Import,         "import" )          \
		Entry( Module_Export,         "export" )          \
		Entry( Number,                "number" )          \
		Entry( Operator,              "operator" )        \
		Entry( Spec_Alignas,          "alignas" )         \
		Entry( Spec_Const,            "const" )           \
		Entry( Spec_Consteval,        "consteval" )       \
		Entry( Spec_Constexpr,        "constexpr" )       \
		Entry( Spec_Constinit,        "constinit" )       \
		Entry( Spec_Extern,           "extern" )          \
		Entry( Spec_Global, 		  "global" )          \
		Entry( Spec_Inline,           "inline" )          \
		Entry( Spec_Internal_Linkage, "internal" )        \
		Entry( Spec_LocalPersist,     "local_persist" )   \
		Entry( Spec_Mutable,          "mutable" )         \
		Entry( Spec_Static,           "static" )          \
		Entry( Spec_ThreadLocal,      "thread_local" )    \
		Entry( Spec_Volatile,         "volatile")         \
		Entry( Star,                  "*" )               \
		Entry( Statement_End,         ";" )               \
		Entry( String,                "__String__" )      \
		Entry( Type_Unsigned, 	      "unsigned" )        \
		Entry( Type_Signed,           "signed" )          \
		Entry( Type_Short,            "short" )           \
		Entry( Type_Long,             "long" )            \
		Entry( Type_char, 			  "char" )            \
		Entry( Type_int, 			  "int" )             \
		Entry( Type_double, 		  "double" )

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

				if ( Arr[Idx].Type != type )
				{
					String token_str = String::make( Memory::GlobalAllocator, { Arr[Idx].Length, Arr[Idx].Text } );

					log_failure( "gen::%s: expected %s, got %s", context, str_tok_type(type), str_tok_type(Arr[Idx].Type) );

					return Code::Invalid;
				}

				Idx++;
				return true;
			}

			inline
			Token& current()
			{
				return Arr[Idx];
			}

			inline
			Token& previous()
			{
				return Arr[Idx - 1];
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

			s32         left    = content.Len -1;
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

				SkipWhitespace();
				if ( left <= 0 )
					break;

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
					String context_str = String::fmt_buf( Memory::GlobalAllocator, "%s", scanner, min( 100, left ) );

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

				if ( type == TokType::Invalid)
				{
					// Its most likely an identifier...
					type = TokType::Identifier;
				}

				token.Type = type;
				array_append( Tokens, token );
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

#	define currtok      toks.current()
#	define prevtok      toks.previous()
#	define eat( Type_ ) toks.__eat( Type_, context )
#	define left         ( array_count(toks.Arr) - toks.Idx )

#	define check( Type_ ) ( left && currtok.Type == Type_ )
#pragma endregion Helper Macros

	internal Code parse_class_struct_body( Parser::TokArray& toks, char const* context );
	internal Code parse_function_body    ( Parser::TokArray& toks, char const* context );
	internal Code parse_global_nspace    ( Parser::TokArray& toks, char const* context );

	internal Code parse_class            ( Parser::TokArray& toks, char const* context );
	internal Code parse_enum             ( Parser::TokArray& toks, char const* context );
	internal Code parse_export_body      ( Parser::TokArray& toks, char const* context );
	internal Code parse_extern_link_body ( Parser::TokArray& toks, char const* context );
	internal Code parse_exten_link       ( Parser::TokArray& toks, char const* context );
	internal Code parse_friend           ( Parser::TokArray& toks, char const* context );
	internal Code parse_function         ( Parser::TokArray& toks, char const* context );
	internal Code parse_namespace        ( Parser::TokArray& toks, char const* context );
	internal Code parse_struct           ( Parser::TokArray& toks, char const* context );
	internal Code parse_variable         ( Parser::TokArray& toks, char const* context );
	internal Code parse_type             ( Parser::TokArray& toks, char const* context );
	internal Code parse_typedef          ( Parser::TokArray& toks, char const* context );
	internal Code parse_union            ( Parser::TokArray& toks, char const* context );
	internal Code parse_using            ( Parser::TokArray& toks, char const* context );

	internal inline
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

			result->add_entry( param );

			if ( check( TokType::Comma ) )
				eat( TokType::Comma );
		}
		eat( TokType::Capture_End );

		return result;
	#	undef context
	}

	internal
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

	internal
	Code parse_class_struct( Parser::TokType which, Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		if ( which != TokType::Decl_Class && which != TokType::Decl_Struct )
		{
			log_failure( "%s: Error, expected class or struct, not %s", context, str_tok_type( which ) );
			return Code::Invalid;
		}

		Token name { nullptr, 0, TokType::Invalid };

		Code       parent     = { nullptr };
		Code       body       = { nullptr };
		Code       attributes = { nullptr };
		ModuleFlag mflags     = ModuleFlag::None;

		Code result = Code::Invalid;

		// TODO: Parse module specifiers

		eat( which );

		// TODO: Parse attributes

		name = parse_identifier( toks, context );

		AccessSpec access     = AccessSpec::Invalid;
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
			body = parse_class_struct_body( toks, context );
		}

		eat( TokType::Statement_End );

		if ( parent_tok )
			parent = def_type( parent_tok );

		if ( which == TokType::Decl_Class )
			result = def_class( name, body, parent, access
				// TODO: Set these up later
				, NoCode // Attributes
				, ModuleFlag::None
			);

		else
			result = def_struct( name, body, parent, access
				// TODO: Set these up later
				, NoCode // Attributes
				, ModuleFlag::None
			);

		return result;
	}

	internal
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
			Code member     = Code::Invalid;
			Code specifiers = Code::Invalid;

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
					{
						log_failure( "gen::parse_variable: failed to parse type" );
						return Code::Invalid;
					}

					Token name = currtok;
					eat( TokType::Identifier );

					// Parsing a member function
					if ( check( TokType::Capture_Start ))
					{
						Code params = parse_params( toks, context );
						if ( params == Code::Invalid )
						{
							log_failure( "gen::parse_variable: failed to parse function parameters" );
							return Code::Invalid;
						}

						if ( check( TokType::BraceCurly_Open ) )
						{
							Code body = parse_function_body( toks, context);
							if ( body == Code::Invalid )
							{
								log_failure( "gen::parse_variable: failed to parse function body" );
								return Code::Invalid;
							}

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
			}

			if ( member == Code::Invalid )
			{
				log_failure( "gen::parse_variable: failed to parse member" );
				return Code::Invalid;
			}

			result->add_entry( member );
		}

		eat( TokType::BraceCurly_Close );
		return result;
	}

	internal
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

	internal
	Code parse_global_nspace( CodeT which, Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		if ( which != Namespace_Body && which != Global_Body && which != Export_Body && which != Extern_Linkage_Body )
			return Code::Invalid;

		eat( TokType::BraceCurly_Open );

		Code
		result = make_code();
		result->Type = which;

		while ( left && currtok.Type != TokType::BraceCurly_Close )
		{
			Code member     = Code::Invalid;
			Code specifiers = Code::Invalid;

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

				case TokType::Module_Export:
					if ( which == Export_Body )
						log_failure( "gen::parse_extern_link_body: nested export declaration" );

					member = parse_export_body( toks, context );
				break;

				case TokType::Decl_Extern_Linkage:
					if ( which == Extern_Linkage_Body )
						log_failure( "gen::parse_extern_link_body: nested extern linkage" );

					member = parse_extern_link_body( toks, context );
				break;

				case TokType::Decl_Namespace:
					member = parse_namespace( toks, context );
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

				// TODO: Module support.
				// case TokType::Module_Export:
				// case TokType::Module_Import:

				case TokType::Spec_Extern:
				case TokType::Spec_Global:
				case TokType::Spec_Inline:
				case TokType::Spec_Internal_Linkage:
				case TokType::Spec_Static:
				{
					SpecifierT specs_found[16] { ESpecifier::Invalid };
					s32         num_specs = 0;

					while ( left && tok_is_specifier( currtok ) )
					{
						SpecifierT spec = ESpecifier::to_type( currtok );

						switch ( spec )
						{
							case ESpecifier::Const:
							case ESpecifier::External_Linkage:
							case ESpecifier::Global:
							case ESpecifier::Inline:
							case ESpecifier::Internal_Linkage:
							break;

							default:
								log_failure( "gen::%s: invalid specifier " txt(spec) " for a global scope", context );
								return Code::Invalid;
						}

						specs_found[num_specs] = spec;
						num_specs++;
						eat( currtok.Type );
					}

					if ( num_specs )
					{
						specifiers = def_specifiers( num_specs, specs_found );
					}
				}
				case TokType::Identifier:
				case TokType::Spec_Const:
				case TokType::Type_Long:
				case TokType::Type_Short:
				case TokType::Type_Signed:
				case TokType::Type_Unsigned:
				{
					Code type = parse_type( toks, context );
					if ( type == Code::Invalid )
					{
						log_failure( "gen::%s: failed to parse type", context );
						return Code::Invalid;
					}
					if ( check( TokType::Decl_Operator ) )
					{
						// Already parsed the return type...

						eat( TokType::Decl_Operator );

						// Parse op type

						// Parse parameters

						// Parse possible body
					}

					Token name = currtok;
					eat( TokType::Identifier );

					// Parsing a function
					if ( check( TokType::Capture_Start ))
					{
						Code params = parse_params( toks, context );
						if ( params == Code::Invalid )
						{
							log_failure( "gen::%s: failed to parse function params", context );
							return Code::Invalid;
						}

						if ( check( TokType::BraceCurly_Open ))
						{
							Code body = parse_function_body( toks, context );
							if ( body == Code::Invalid )
							{
								log_failure( "gen::%s: failed to parse function body", context );
								return Code::Invalid;
							}

							member = make_code();
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
										log_failure( "gen::%s: invalid function body", context );
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

					// Parsing a variable
					Code array_expr = parse_array_decl( toks, context );
					Code expr       = parse_variable_assignment( toks, context );

					member = make_code();
					member->Type = Variable;
					member->Name = get_cached_string( name );

					member->add_entry( type );

					if ( array_expr )
						type->add_entry( array_expr );

					if ( specifiers )
						member->add_entry( specifiers );

					if ( expr )
						member->add_entry( expr );
				}
			}

			if ( member == Code::Invalid )
			{
				log_failure( "gen::%s: failed to parse extern linkage member", context );
				return Code::Invalid;
			}

			result->add_entry( member );
		}

		eat( TokType::BraceCurly_Close );
		return result;
	}

	internal
	Code parse_class( Parser::TokArray& toks, char const* context )
	{
		return parse_class_struct( Parser::TokType::Decl_Class, toks, context );
	}

	Code parse_class( StrC def )
	{
		check_parse_args( parse_class, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_class_struct( TokType::Decl_Class, toks, txt(parse_class) );
	}

	internal
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
			eat( TokType::Statement_End );
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
			// mem_copy( entries_code, body.Text, body.Length );

			Code untyped_body = untyped_str( body );

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

	internal
	Code parse_export_body( Parser::TokArray& toks, char const* context )
	{
		return parse_global_nspace( ECode::Export_Body, toks, context );
	}

	Code parse_export_body( StrC def )
	{
		check_parse_args( parse_export_body, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_export_body( toks, txt(parse_export_body) );
	}

	internal
	Code parse_extern_link_body( Parser::TokArray& toks, char const* context )
	{
		return parse_global_nspace( ECode::Extern_Linkage_Body, toks, context );
	}

	internal
	Code parse_extern_link( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		eat( TokType::Decl_Extern_Linkage );

		Token name = currtok;
		eat( TokType::String );

		name.Text   += 1;
		name.Length -= 1;

		Code
		result       = make_code();
		result->Type = ECode::Extern_Linkage;
		result->Name = get_cached_string( name );

		Code entry = parse_extern_link_body( toks, context );
		if ( entry == Code::Invalid )
		{
			log_failure( "gen::parse_extern_link: failed to parse body" );
			return result;
		}

		result->add_entry( entry );

		return result;
	}

	Code parse_extern_link( StrC def )
	{
		check_parse_args( parse_extern_link, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_extern_link( toks, txt(parse_extern_link) );
	}

	internal
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
		}

		eat( TokType::Statement_End );

		Code
		result       = make_code();
		result->Type = Friend;

		if ( function )
			result->add_entry( function );

		else
			result->add_entry( type );

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

	internal
	Code parse_functon( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Code lang_linkage = { nullptr };
		Code attributes   = { nullptr };
		Code array_expr   = { nullptr };
		Code specifiers   = { nullptr };

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

		Code body = { nullptr };
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

		return result;
	}

	Code parse_function( StrC def )
	{
		using namespace Parser;

		check_parse_args( parse_function, def );

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_functon( toks, txt(parse_function) );
	}

	Code parse_global_body( StrC def )
	{
		check_parse_args( parse_global_body, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_global_nspace( ECode::Global_Body, toks, txt(parse_global_body) );
	}

	internal
	Code parse_namespace( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		eat( TokType::Decl_Namespace );

		Token name = parse_identifier( toks, txt(parse_namespace) );

		Code body = parse_global_nspace( ECode::Namespace_Body, toks, txt(parse_namespace) );
		if ( body == Code::Invalid )
			return Code::Invalid;

		Code
		result = make_code();
		result->Type = ECode::Namespace;
		result->Name = get_cached_string( name );

		result->add_entry( body );

		return result;
	}

	Code parse_namespace( StrC def )
	{
		check_parse_args( parse_namespace, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_namespace( toks, txt(parse_namespace) );
	}

	internal
	Code parse_operator( Parser::TokArray& toks, char const* context )
	{
		// Parse Module specifier

		// Parse Attributes

		// Parse Speciifers

		// Parse Return Type

		// Parse Operator

		// Parse Params

		// Parse Body

		return Code::Invalid;
	}

	Code parse_operator( StrC def )
	{
		check_parse_args( parse_operator, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_operator( toks, txt(parse_operator) );
	}

	internal
	Code parse_struct( Parser::TokArray& toks, char const* context )
	{
		return parse_class_struct( Parser::TokType::Decl_Struct, toks, txt(parse_struct) );
	}

	Code parse_struct( StrC def )
	{
		check_parse_args( parse_struct, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_class_struct( TokType::Decl_Struct, toks, txt(parse_struct) );
	}

	internal
	Code parse_type( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Token name = { nullptr, 0, TokType::Invalid };

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			if ( spec != ESpecifier::Const )
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
				return Code::Invalid;
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
				return Code::Invalid;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		Token func_sig = { nullptr, 0, TokType::Invalid };

		if ( check( TokType::Capture_Start ) )
		{
			// Its a function type

			func_sig = currtok;
			eat( TokType::Capture_Start );

			while ( check( TokType::Star ) || currtok.Type == TokType::Spec_Const )
			{
				eat( currtok.Type );
			}

			eat(TokType::Identifier);

			eat( TokType::Capture_End );

			// Parameters

			eat( TokType::Capture_Start );

			// TODO: Change this to validate the parameters...
			// Bruteforce lex the parameters, no validation.
			while ( ! check( TokType::Capture_End ))
			{
				eat( currtok.Type );
			}

			eat(TokType::Capture_End);

			func_sig.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)func_sig.Text;
		}

		using namespace ECode;

		Code
		result       = make_code();
		result->Type = Typename;

		if ( func_sig )
		{
			// Brute force name to the entire signature.

			String custom_name = String::make( Memory::GlobalAllocator, name );

			if ( num_specifiers )
				custom_name.append( def_specifiers( num_specifiers, (SpecifierT*)specs_found ).to_string() );

			custom_name.append( " " );
			custom_name.append(func_sig );
		}
		else
		{
			if (num_specifiers)
			{
				Code specifiers = def_specifiers( num_specifiers, (SpecifierT*)specs_found );
			}
		}

		result->Name = get_cached_string( name );

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

		return result;
	}

	internal
	Code parse_typedef( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Token name       = { nullptr, 0, TokType::Invalid };
		Code  array_expr = { nullptr };
		Code  type       = { nullptr };

		eat( TokType::Decl_Typedef );

		type = parse_type( toks, txt(parse_typedef) );

		if ( ! check( TokType::Identifier ) )
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

		if ( array_expr && array_expr->Type != Invalid )
			type->add_entry( array_expr );

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

	internal
	Code parse_union( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		not_implemented();

		return Code::Invalid;
	}

	Code parse_union( StrC def )
	{
		check_parse_args( parse_union, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_typedef( toks, txt(parse_union) );
	}

	internal
	Code parse_using( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Token  name       = { nullptr, 0, TokType::Invalid };
		Code   array_expr = { nullptr };
		Code   type       = { nullptr };

		bool is_namespace = false;

		// TODO: Parse module specs

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
			// TODO: Parse Attributes (using type-alias)

			eat( TokType::Operator );

			type = parse_type( toks, txt(parse_typedef) );
		}

		array_expr = parse_array_decl( toks, txt(parse_typedef) );

		eat( TokType::Statement_End );

		using namespace ECode;

		Code
		result       = make_code();
		result->Type = is_namespace ? Using : Using_Namespace;
		result->Name = get_cached_string( name );

		result->add_entry( type );

		if ( array_expr )
			type->add_entry( array_expr );

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

	internal
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
#endif
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
			// TODO: Switch this to use an arena that makes use of the stack (cap the size of the token table to around 4096 bytes)
			tokmap_init( & tok_map, Memory::GlobalAllocator );

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

		char current = *fmt;

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
				TokEntry* value = tokmap_get( & tok_map, key );

				if ( value )
				{
					sw          left = value->Length;
					char const* str  = value->Str;

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

		tokmap_clear( & tok_map );

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

		Buffer = String::make( Memory::GlobalAllocator, "" );

		return true;
	}

	void Builder::write()
	{
		bool result = file_write( & File, Buffer, Buffer.length() );

		if ( result == false )
			log_failure("gen::File::write - Failed to write to file: %s", file_name( & File ) );

		file_close( & File );
	}
#pragma endregion Builder

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
