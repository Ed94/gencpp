// ReSharper disable CppClangTidyClangDiagnosticSwitchEnum
#include "Bloat.hpp"
#include "gen.hpp"

#ifdef gen_time
namespace gen
{
	ZPL_TABLE_DEFINE( StringTable, str_tbl_,   String );
	ZPL_TABLE_DEFINE( TypeTable,   type_tbl_ , Code   );

	namespace StaticData
	{
		static Array(Pool)  CodePools         = nullptr;
		static Array(Arena) CodeEntriesArenas = nullptr;
		static Array(Arena) StringArenas      =  nullptr;

		static StringTable StringMap;
		static TypeTable   TypeMap;

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

	Code type_ns(bool);
	Code type_ns(char);
	Code type_ns(char_wide);

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

	Code spec_constexpr;
	Code spec_const;
	Code spec_inline;
	Code sepc_ptr;
	Code spec_ref;
#pragma endregion Constants

#pragma region AST Body Case Macros
#	define AST_BODY_CLASS_UNALLOWED_TYPES \
	case Class_Body:                      \
	case Enum_Body:                       \
	case Friend:                          \
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
	case Class_Body:                         \
	case Enum_Body:                          \
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

#	define AST_BODY_NAMESPACE_UNALLOWED_TYPES \
	case Access_Public: 					  \
	case Access_Protected: 					  \
	case Access_Private: 					  \
	case Class_Body: 						  \
	case Enum_Body: 						  \
	case Execution: 						  \
	case Friend: 							  \
	case Function_Body: 					  \
	case Global_Body: 						  \
	case Namespace_Body: 					  \
	case Operator_Member: 					  \
	case Operator_Member_Fwd: 				  \
	case Parameters: 						  \
	case Specifiers: 						  \
	case Struct_Body: 						  \
	case Typename:

#	define AST_BODY_STRUCT_UNALLOWED_TYPES \
	case Enum_Body: 					   \
	case Execution: 					   \
	case Function_Body: 				   \
	case Global_Body: 					   \
	case Namespace: 					   \
	case Namespace_Body: 				   \
	case Operator: 						   \
	case Operator_Fwd: 					   \
	case Parameters: 					   \
	case Specifiers: 					   \
	case Struct_Body: 					   \
	case Typename: 						   \
	case Using_Namespace:
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

			case Access_Public:
				log_failure( "AST::add: Cannot add an AST to a public access specifier." );
				return false;

			case Access_Protected:
				log_failure( "AST::add: Cannot add an AST to a protected access specifier." );
				return false;

			case Access_Private:
				log_failure( "AST::add: Cannot add an AST to a private access specifier." );
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
						log_failure( "AST::add: Cannot add an AST to a class body." );
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
						log_failure( "AST::add: Cannot add an AST to a function body." );
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
						log_failure( "AST::add: Cannot add an AST to a global body." );
						return false;
					}

					default:
						break;
				}
			break;


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
						log_failure( "AST::add: Cannot add an AST to a namespace body." );
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
						log_failure( "AST::add: Cannot add to a struct body." );
						return false;
					}

					default:
						break;
				}
			break;

			case Variable:
				log_failure( "AST::add: Cannot add to a variable." );
				return false;

			case Typedef:
				log_failure( "AST::add: Cannot add to a typedef." );
				return false;

			case Typename:
				log_failure( "AST::add: Cannot add to a typename." );
				return false;

			case Using:
				log_failure( "AST::add: Cannot add to a using statement." );
				return false;
		}

		array_append( Entries, other );

		other->Parent = this;
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
		result->Comment  = Comment;
		result->Type     = Type;
		result->Op       = Op;
		result->Readonly = Readonly;

		switch ( Type )
		{
			case Invalid:
			break;

			case Untyped:
			case Access_Public:
			case Access_Protected:
			case Access_Private:
			case Class_Fwd:
			case Enum_Fwd:
			case Function_Fwd:
			case Specifiers:
				// Can just be the same, as its a cached string.
				result->Content = Content;
				return result;

			// The main purpose of this is to make sure entires in the AST are unique,
			// So that we can assign the new parent without corrupting the existing AST.
			case Class:
			case Class_Body:
			case Enum:
			case Enum_Body:
			case Friend:
			case Global_Body:
			case Namespace:
			case Namespace_Body:
			case Parameters:
			case Function:
			case Function_Body:
			case Struct:
			case Struct_Fwd:
			case Struct_Body:
			case Variable:
			case Typedef:
			case Typename:
			case Using:
				s32 index = 0;
				s32 left  = num_entries();
				while ( left -- )
				{
					// This will naturally duplicate the entire chain duplicate all of the ast nodes.
					// It may not be the most optimal way for memory reasons, however figuring out the heuristic
					// For when it should reparent all nodes or not is not within the simple scope of this library.
					result->add_entry( Entries[index]->duplicate() );
					index++;
				}
				return result;
		}
	}

	String AST::to_string()
	{
		String result = string_make( g_allocator, "" );

		if ( Comment )
			result = string_append_fmt( result, "// %s\n", Comment );

		switch ( Type )
		{
			using namespace ECode;

			case Invalid:
				log_failure("Attempted to serialize invalid code! - %s", Name);
			break;

			case Untyped:
				result = string_append_length( result, Content, string_length( ccast(String, Content)) );
			break;

			case Access_Public:
			case Access_Protected:
			case Access_Private:
				result = string_append_length( result, Name, string_length( ccast(String, Name)) ) ;
			break;

			case Class:
			{
				result = string_append_fmt( result, "class");

				s32 idx = 1;

				if ( Entries[idx]->Type == Specifiers )
				{
					result = string_append_fmt( result, " %s", Entries[idx]->to_string() );
					idx++;
				}

				result = string_append_fmt( result, " %s", Name );

				if ( parent() )
					result = string_append_fmt( result, " : %s", parent()->to_string() );

				result = string_append_fmt( result, "\n{\n%s\n};\n", body()->to_string() );
			}
			break;

			case Class_Fwd:
			{
				result = string_append_fmt( result, "class");

				s32 idx = 1;

				if ( Entries[idx]->Type == Specifiers )
				{
					result = string_append_fmt( result, " %s", Entries[idx]->to_string() );
					idx++;
				}

				result = string_append_fmt( result, " %s;", Name );
			}
			break;

			case Enum:
				if ( underlying_type() )
				{
					result = string_append_fmt( result, "enum %s : %s\n{\n%s\n};\n"
						, Name
						, underlying_type()->to_string()
						, body()->to_string()
					);
				}
				else
				{
					result = string_append_fmt( result, "enum %s\n{\n%s\n};\n"
						, Name
						, body()->to_string()
					);
				}
			break;

			case Enum_Fwd:
				result = string_append_fmt( result, "enum %s : %s;\n", Name, underlying_type()->to_string() );
			break;

			case Enum_Class:
				if ( underlying_type() )
				{
					result = string_append_fmt( result, "enum class %s : %s\n{\n%s\n};\n"
						, Name
						, underlying_type()->to_string()
						, body()->to_string()
					);
				}
				else
				{
					result = string_append_fmt( result, "enum class %s\n{\n%s\n};\n"
						, Name
						, body()->to_string()
					);
				}
			break;

			case Enum_Class_Fwd:
				result = string_append_fmt( result, "enum class %s : %s;\n", Name, underlying_type()->to_string() );
			break;

			case Friend:
				result = string_append_fmt( result, "friend %s;\n", Entries[0]->to_string() );
			break;

			case Function:
			{
				u32 idx  = 1;
				u32 left = num_entries();

				if ( left <= 0 )
					log_failure( "Code::to_string - Name: %s Type: %s, expected definition", Name, Type );

				if ( Entries[idx]->Type == Specifiers )
				{
					result = string_append_fmt( result, "%s", Entries[idx]->to_string() );
					idx++;
					left--;
				}

				if ( left <= 0 )
					log_failure( "Code::to_string - Name: %s Type: %s, expected return type", Name, Type );

				result = string_append_fmt( result, "\n%s %s(", Entries[idx]->to_string(), Name );
				idx++;
				left--;

				if ( left && Entries[idx]->Type == Parameters )
				{
					result = string_append_fmt( result, "%s", Entries[idx]->to_string() );
					idx++;
					left--;
				}
				else
				{
					result = string_append_fmt( result, "void" );
				}

				result = string_append_fmt( result, ")\n{\n%s\n}", body()->to_string() );
			}
			break;

			case Function_Fwd:
			{
				u32 idx  = 0;
				u32 left = array_count( Entries );

				if ( left <= 0 )
					log_failure( "Code::to_string - Name: %s Type: %s, expected definition", Name, Type );

				if ( Entries[idx]->Type == Specifiers )
				{
					result = string_append_fmt( result, "%s\n", Entries[idx]->to_string() );
					idx++;
					left--;
				}

				if ( left <= 0 )
					log_failure( "Code::to_string - Name: %s Type: %s, expected return type", Name, Type );

				result = string_append_fmt( result, "\n%s %s(", Entries[idx]->to_string(), Name );
				idx++;
				left--;

				if ( left && Entries[idx]->Type == Parameters )
				{
					result = string_append_fmt( result, "%s", Entries[idx]->to_string() );
					idx++;
					left--;
				}
				else
				{
					result = string_append_fmt( result, "void" );
				}

				result = string_appendc( result, ");\n" );
			}
			break;

			case Namespace:
				result = string_append_fmt( result, "namespace %s\n{\n%s\n};\n", Name, body()->to_string() );
			break;

			case Operator:
			case Operator_Member:
			{
				s32 idx = 1;

				if ( Entries[idx]->Type == Specifiers )
				{
					result = string_append_fmt( result, "%s", Entries[idx]->to_string() );
					idx++;
				}

				result = string_append_fmt( result, "%s operator%s(", Entries[idx]->to_string(), Name );
				idx++;

				if ( Entries[idx]->Type == Parameters )
				{
					result = string_append_fmt( result, "%s", Entries[idx]->to_string() );
					idx++;
				}
				else
				{
					result = string_append_fmt( result, "void" );
				}

				result = string_append_fmt( result, ")\n{\n%s\n}", body()->to_string() );
			}
			break;

			case Operator_Fwd:
			case Operator_Member_Fwd:
			{
				s32 idx;

				if ( Entries[idx]->Type == Specifiers )
				{
					result = string_append_fmt( result, "%s", Entries[idx]->to_string() );
					idx++;
				}

				result = string_append_fmt( result, "%s operator%s(", Entries[idx]->to_string(), Name );
				idx++;

				if ( Entries[idx]->Type == Parameters )
				{
					result = string_append_fmt( result, "%s);\n", Entries[idx]->to_string() );
					idx++;
				}
				else
				{
					result = string_append_fmt( result, "void);\n" );
				}
			}
			break;

			case Parameters:
			{
				result = string_append_fmt( result, "%s %s", Entries[0]->to_string(), Name );

				s32 index = 1;
				s32 left  = array_count( Entries ) - 1;

				while ( left--, left > 0 )
					result = string_append_fmt( result, ", %s %s"
						, Entries[index]->Entries[0]->to_string()
						, Entries[index]->Name
					);
			}
			break;

			case Specifiers:
			{
				s32 idx  = 0;
				s32 left = StaticIndex;
				while ( left--, left > 0 )
					result = string_append_fmt( result, "%s ", ESpecifier::to_str( ArrSpecs[idx]) );
			}
			break;

			case Struct:
			{
				result = string_append_fmt( result, "struct");

				s32 idx = 1;

				if ( Entries[idx]->Type == Specifiers )
				{
					result = string_append_fmt( result, " %s", Entries[idx]->to_string() );
					idx++;
				}

				result = string_append_fmt( result, " %s", Name );

				if ( parent() )
					result = string_append_fmt( result, " : %s", parent()->to_string() );

				result = string_append_fmt( result, "\n{\n%s\n};\n", body()->to_string() );
			}
			break;

			case Struct_Fwd:
			{
				result = string_append_fmt( result, "struct");

				s32 idx = 1;

				if ( Entries[idx]->Type == Specifiers )
				{
					result = string_append_fmt( result, " %s", Entries[idx]->to_string() );
					idx++;
				}

				result = string_append_fmt( result, " %s;", Name );
			}
			break;

			case Variable:
			{
				s32 idx = 1;

				if ( Entries[idx]->Type == Specifiers )
				{
					result = string_append_fmt( result, "%s", Entries[idx]->to_string() );
					idx++;
				}

				result = string_append_fmt( result, "%s %s", Entries[0]->to_string(), Name );

				if ( Entries[idx] )
					result = string_append_fmt( result, " = %s", Entries[idx]->to_string() );
			}
			break;

			case Typedef:
				// TODO: Check for array expression

				result = string_append_fmt( result, "typedef %s %s", Entries[0]->to_string(), Name );
			break;

			case Typename:
				result = string_append_fmt( result, "%s %s", Name, Entries[0]->to_string() );
			break;

			case Using:
				// TODO: Check for array expression

				if ( Entries[0] )
					result = string_append_fmt( result, "using %s = %s", Name, Entries[0] );

				else
					result = string_append_fmt( result, "using %s", Name );
			break;

			case Using_Namespace:
				result = string_append_fmt( result, "using namespace %s", Name );
			break;


			case Class_Body:
			case Enum_Body:
			case Function_Body:
			case Global_Body:
			case Namespace_Body:
			case Struct_Body:
			{
				s32 index = 0;
				s32 left  = num_entries();
				while ( left -- )
				{
					result = string_append_fmt( result, "%s\n", Entries[index]->to_string() );
					index++;
				}
			}
			break;
		}

		return result;
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

			type_tbl_init( & TypeMap,   Allocator_TypeTable );
			if ( TypeMap.entries == nullptr )
				fatal( "gen::init: Failed to initialize the TypeMap" );
		}

		Code::Invalid = make_code();
		Code::Invalid.lock();

	#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
		Code&
		t_bool_write = ccast( Code, t_void );
		t_bool_write = def_type( name(void) );

	#	define def_constant_code_type( Type_ ) \
		Code&                                  \
		t_##Type_ = def_type( name(Type_) );   \
		t_##Type_->Readonly = true;

		def_constant_code_type( int );

		def_constant_code_type( bool );
		def_constant_code_type( char );
		def_constant_code_type( wchar_t );

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
	#	undef def_constant_code_type
	#endif

		Code&
		spec_constexpr_write = ccast( Code, spec_constexpr );
		spec_constexpr_write = def_specifiers( 1, ESpecifier::Constexpr );

	#	define def_constant_spec( Type_, ... )                                    \
		Code&                                                                     \
		spec_##Type_ = def_specifiers( macro_num_args(__VA_ARGS__), __VA_ARGS__); \
		spec_##Type_.lock();

		def_constant_spec( const, ESpecifier::Const );
		def_constant_spec( inline, ESpecifier::Inline );
	#	undef def_constant_spec
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

		type_tbl_clear( & StaticData::TypeMap );
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
	StringCached get_cached_string( char const* cstr, s32 length )
	{
		s32     hash_length = length > kilobytes(1) ? kilobytes(1) : length;
		u32     key         = crc32( cstr, hash_length );
		String* result      = str_tbl_get( & StaticData::StringMap, key );

		if ( result )
			return * result;

		* result = string_make( get_string_allocator( length ), cstr );

		str_tbl_set( & StaticData::StringMap, key, * result );

		return * result;
	}

	/*
		Used internally to retireve a Code object form the CodePool.
	*/
	Code make_code()
	{
		using namespace StaticData;

		AllocatorInfo allocator = { nullptr, nullptr };

		s32 left = array_count( CodePools );
		do
		{
			if ( CodePools[left].free_list != nullptr  )
				allocator = zpl::pool_allocator( & CodePools[left] );
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

		result->Content        = nullptr;
		result->Parent         = nullptr;
		result->Name           = nullptr;
		result->Comment        = nullptr;
		result->Type           = ECode::Invalid;
		result->Op             = EOperator::Invalid;
		result->Readonly       = false;
		result->DynamicEntries = false;
		result->StaticIndex    = 0;

		return result;
	}

	Array(AST*) make_code_entries()
	{
		using namespace StaticData;

		AllocatorInfo allocator = { nullptr, nullptr };

		s32 left = array_count( CodeEntriesArenas );
		do
		{
			if ( arena_size_remaining(CodeEntriesArenas, ZPL_DEFAULT_MEMORY_ALIGNMENT) >= InitSize_CodeEntiresArray )
				allocator = arena_allocator( & CodeEntriesArenas[left] );
		}
		while( left--, left );

		if ( allocator.data == nullptr )
		{
			Arena arena;
			arena_init_from_allocator( & arena, Allocator_CodeEntriesArena, SizePer_CodeEntriresArena );

			if ( arena.physical_start == nullptr )
				fatal( "gen::make_code: Failed to allocate a new code entries arena - CodeEntriesArena allcoator returned nullptr." );

			allocator = arena_allocator( CodeEntriesArenas );
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
#	define name_check( Context_, Length_, Name_ )                                                \
	{                                                                                            \
		if ( Length_ <= 0 )                                                                      \
		{                                                                                        \
			log_failure( "gen::" txt(Context_) ": Invalid name length provided - %d",  length ); \
			return Code::Invalid;                                                                \
		}                                                                                        \
                                                                                                 \
		if ( Name_ == nullptr )                                                                  \
		{                                                                                        \
			log_failure( "gen::" txt(Context_) ": name is null" );                               \
			return Code::Invalid;                                                                \
		}                                                                                        \
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

	Code def_class( s32 length, char const* name, Code parent, Code specifiers, Code body )
	{
		using namespace ECode;

		name_check( def_class, length, name );

		if ( parent && parent->Type != Class || parent->Type != Struct )
		{
			log_failure( "gen::def_class: parent provided is not type 'Class' or 'Struct' - Type: %s", parent->debug_str() );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_class: specifiers was not a 'Specifiers' type - Type: %s", specifiers->debug_str() );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Name = get_cached_string( name, length );

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

		if ( specifiers )
			result->add_entry( specifiers );

		if ( parent )
			result->add_entry( parent );

		result.lock();
		return result;
	}

	Code def_enum( s32 length, char const* name, Code type, EnumT specifier, Code body )
	{
		using namespace ECode;

		name_check( def_enum, length, name );

		if ( type && type->Type != Typename )
		{
			log_failure( "gen::def_enum: enum underlying type provided was not of type Typename: %s", type->debug_str() );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Name = get_cached_string( name, length );

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
		else if ( specifier == EnumClass )
		{
			result->Type = specifier == EnumClass ?
				Enum_Class_Fwd : Enum_Fwd;
		}

		if ( type )
		{
			result->add_entry( type );
		}
		else if ( result->Type == Enum_Class_Fwd || result->Type == Enum_Fwd )
		{
			log_failure( "gen::def_enum: enum forward declaration must have an underlying type" );
			return Code::Invalid;
		}

		result.lock();
		return result;
	}

	Code def_execution( Code untyped_code )
	{
		null_check( def_execution, untyped_code );

		if ( untyped_code->Type != ECode::Untyped )
		{
			log_failure( "gen::def_execution: untyped_code is not of untyped type - %s", untyped_code->debug_str() );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Type = ECode::Execution;

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

	Code def_function( s32 length, char const* name
		, Code specifiers
		, Code params
		, Code ret_type
		, Code body
	)
	{
		using namespace ECode;

		name_check( def_function, length, name );

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_function: specifiers was not a `Specifiers` type %s", specifiers->debug_str() );
			return Code::Invalid;
		}

		if ( params && params->Type != Parameters )
		{
			log_failure( "gen::def_function: params was not a `Parameters` type %s", params->debug_str() );
			return Code::Invalid;
		}

		if ( ret_type == nullptr || ret_type->Type != Typename )
		{
			log_failure( "gen::def_function: ret_type was not a Typename %s", ret_type->debug_str() );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Name = get_cached_string( name, length );

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

	Code def_namespace( s32 length, char const* name, Code body )
	{
		using namespace ECode;

		name_check( def_namespace, length, name );
		null_check( def_namespace, body );

		Code
		result          = make_code();
		result->Type    = Namespace;
		result->Name    = get_cached_string( name, length );
		result->Entries = make_code_entries();

		if ( body->Type != Namespace_Body || body->Type != Untyped )
		{
			log_failure("gen::def_namespace: body is not of namespace or untyped type %s", body->debug_str());
			return Code::Invalid;
		}

		result->add_entry( body );

		result.lock();
		return result;
	}

	Code def_operator( OperatorT op, Code params_code, Code ret_type, Code specifiers, Code body )
	{
		using namespace ECode;

		if ( body && body->Type != Function_Body )
		{
			log_failure( "gen::def_operator: Body was provided but its not of function body type - %s", body->debug_str() );
			return Code::Invalid;
		}

		OpValidateResult check_result = operator__validate( op, params_code, ret_type, specifiers );

		if ( check_result == OpValidateResult::Fail )
		{
			return Code::Invalid;
		}

		char const* name = str_fmt_buf( "operator%s", to_str(op) );

		Code
		result       = make_code();
		result->Name = get_cached_string( name, str_len(name, MaxNameLength) );

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

		if ( specifiers )
			result->add_entry( specifiers );

		result->add_entry( ret_type );

		if (params_code)
			result->add_entry( params_code );

		result.lock();
		return result;
	}

	Code def_param( Code type, s32 length, char const* name )
	{
		using namespace ECode;

		name_check( def_param, length, name );
		null_check( def_param, type );

		if ( type->Type != Typename )
		{
			log_failure( "gen::def_param: type is not a typename - %s", type->debug_str() );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Type = Parameters;
		result->Name = get_cached_string( name, length );

		result->add_entry( type );

		result.lock();
		return result;
	}

	Code def_struct( u32 length, char const* name, Code body, Code parent, Code specifiers )
	{
		using namespace ECode;

		name_check( def_struct, length, name );

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_struct: specifiers was not a `Specifiers` type" );
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
		result       = make_code();
		result->Name = get_cached_string( name, length );

		if ( body )
		{
			result->Type = Struct;
			result->add_entry( body );
		}
		else
		{
			result->Type = Struct_Fwd;
		}

		if ( parent )
			result->add_entry( parent );

		if ( specifiers )
			result->add_entry( specifiers );

		result.lock();
		return result;
	}

	Code def_variable( Code type, u32 length, char const* name, Code value, Code specifiers )
	{
		name_check( def_variable, length, name );
		null_check( def_variable, type );

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
		result       = make_code();
		result->Name = get_cached_string( name, length );
		result->Type = ECode::Variable;

		result->add_entry( type );

		if ( specifiers )
			result->add_entry( specifiers );

		if ( value )
			result->add_entry( value );

		result.lock();
		return result;
	}

	Code def_typedef( u32 length, char const* name, Code type )
	{
		name_check( def_typedef, length, name );
		null_check( def_typedef, type );

		if ( type->Type != ECode::Typename )
		{
			log_failure( "gen::def_typedef: type was not a Typename" );
			return Code::Invalid;
		}

		// Registering the type.
		Code registered_type = def_type( length, name );

		if ( ! registered_type )
		{
			log_failure( "gen::def_typedef: failed to register type" );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Name = get_cached_string( name, length );
		result->Type = ECode::Typedef;

		result->add_entry( type );
		result.lock();

		return result;
	}

	Code def_type( u32 length, char const* name, Code specifiers, Code ArrayExpr )
	{
		name_check( def_type, length, name );

		if ( specifiers && specifiers->Type != ECode::Specifiers )
		{
			log_failure( "gen::def_type: specifiers is not of specifiers type - %s", specifiers->debug_str() );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Name = get_cached_string( name, length );
		result->Type = ECode::Typename;

		if ( specifiers )
			result->add_entry( specifiers );

		if ( ArrayExpr )
			result->add_entry( ArrayExpr );

		result.lock();
		return result;
	}

	Code def_using( u32 length, char const* name, Code type, UsingT specifier )
	{
		name_check( def_using, length, name );
		null_check( def_using, type );

		Code register_type = def_type( length, name );

		if ( ! register_type )
		{
			log_failure( "gen::def_using: failed to register type" );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Name = get_cached_string( name, length );

		switch ( specifier )
		{
			case UsingRegular:
				result->Type = ECode::Using;

				if ( type )
					result->add_entry( type );
			break;

			case UsingNamespace:
				result->Type = ECode::Using_Namespace;
			break;
		}

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

#	define def_body_code_validation_start( Name_, Get_Code_Expr_ )          \
	do                                                                      \
	{                                                                       \
		Code entry = Get_Code_Expr_;                                        \
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
		def_body_code_validation_start( def_class_body, va_arg( va, Code ) );
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

		def_body_code_validation_start( def_class_body, *codes; codes++ );
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
			Code entry = va_arg(va, Code);

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
		result->Type = Class_Body;

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

	Code def_function_body( s32 num, ... )
	{
		def_body_start( def_function_body );

		Code
		result       = make_code();
		result->Type = Function_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_function_body, va_arg(va, Code) );
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

		def_body_code_validation_start( def_function_body, *codes; codes++ );
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
		def_body_code_validation_start( def_global_body, va_arg(va, Code) );
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

		def_body_code_validation_start( def_global_body, *codes; codes++ );
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
		def_body_code_validation_start( def_namespace_body, va_arg(va, Code) );
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

		def_body_code_validation_start( def_namespace_body, *codes; codes++ );
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

		Code        type        = va_arg(va, Code);
		s32         name_length = va_arg(va, s32 );
		char const* name        = va_arg(va, char const*);

		result->Name    = get_cached_string( name, name_length );
		result->Entries = make_code_entries();

		if ( type->Type != Typename )
		{
			log_failure( "gen::def_params: type of param %d is not a Typename", num - num + 1 );
			return Code::Invalid;
		}

		result->add_entry( type );

		while( num -= 3, num && num % 3 == 0 )
		{
			type        = va_arg(va, Code);
			name_length = va_arg(va, u32);
			name        = va_arg(va, char const*);

			Code
			param       = make_code();
			param->Type = Parameters;
			param->Name = get_cached_string(name, name_length);

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
		def_body_code_validation_start( def_struct_body, va_arg(va, Code) );
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

		def_body_code_validation_start( def_struct_body, *codes; codes++ );
			AST_BODY_STRUCT_UNALLOWED_TYPES
		def_body_code_validation_end( def_struct_body );

		result.lock();
		return result;
	}
#pragma endregion Upfront Constructors

#pragma region Incremetnal Constructors
#ifdef	GEN_FEATURE_INCREMENTAL
	Code make_class( s32 length, char const* name, Code parent, Code specifiers )
	{
		using namespace ECode;

		name_check( make_struct, length, name );

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::make_class: specifiers was not a `Specifiers` type" );
			return Code::Invalid;
		}

		if ( parent && parent->Type != Struct )
		{
			log_failure( "gen::make_class: parent was not a `Struct` type" );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Type = Struct;
		result->Name = get_cached_string( name, length );

		Code
		body       = make_code();
		body->Type = Struct_Body;

		result->add_entry( body );

		if ( parent )
			result->add_entry( parent );

		if ( specifiers )
			result->add_entry( specifiers );

		return result;
	}

	Code make_enum( s32 length, char const* name, Code type, EnumT specifier )
	{
		using namespace ECode;

		name_check( make_enum, length, name );

		if ( type && type != Typename )
		{
			log_failure("gen::make_enum: type provided is not of code type typename - %s", type->debug_str() );
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Type = specifier == EnumClass ? Enum_Class : Enum;
		result->Name = get_cached_string( name, length );

		Code
		body = make_code();
		body->Type = Enum_Body;

		result->add_entry( body );

		if ( type )
			result->add_entry( type );

		return result;
	}

	Code make_function( s32 length, char const* name
		, Code specifiers
		, Code params
		, Code ret_type
	)
	{
		using namespace ECode;

		name_check( make_function, length, name );

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
		result       = make_code();
		result->Name = get_cached_string( name, length );
		result->Type = Function;

		Code
		body       = make_code();
		body->Type = Function_Body;

		result->add_entry( body );

		if ( specifiers )
			result->add_entry( specifiers );

		if ( ret_type )
			result->add_entry( ret_type );

		if ( params )
			result->add_entry( params );

		return result;
	}

	Code make_global_body( s32 length, char const* name, s32 num, ... )
	{
		name_check( make_global_body, length, name );

		Code
		result       = make_code();
		result->Type = ECode::Global_Body;
		result->Name = get_cached_string( name, length );

		return result;
	}

	Code make_namespace( s32 length, char const* name, Code parent, Code specifiers )
	{
		name_check( make_namespace, length, name );

		Code
		result = make_code();
		result->Type = ECode::Namespace;
		result->Name = get_cached_string( name, length );

		Code
		body = make_code();
		body->Type = ECode::Namespace_Body;

		result->add_entry( body );

		return result;
	}

	Code make_operator( OperatorT op, Code params_code, Code ret_type, Code specifiers )
	{
		using namespace ECode;

		OpValidateResult check_result = operator__validate( op, params_code, ret_type, specifiers );

		if ( check_result == OpValidateResult::Fail )
		{
			return Code::Invalid;
		}

		char const* name = str_fmt_buf( "operator%s", to_str(op) );

		Code
		result       = make_code();
		result->Name = get_cached_string( name, str_len(name, MaxNameLength) );

		if (params_code)
			result->add_entry( params_code );

		result->add_entry( ret_type );

		if ( specifiers )
			result->add_entry( specifiers );

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

	Code make_struct( s32 length, char const* name, Code parent, Code specifiers )
	{
		using namespace ECode;

		name_check( make_struct, length, name );

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
		result       = make_code();
		result->Type = Struct;
		result->Name = get_cached_string( name, length );

		Code
		body       = make_code();
		body->Type = Function_Body;

		result->add_entry( make_code() );

		if ( parent )
			result->add_entry( parent );

		if ( specifiers )
			result->add_entry( specifiers );

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

	The current implementation is very convervative and does not use a lexer to process the input first.
	Instead, it just sifts through the content directly looking for the syntax pattern for the code type
	and then constructs the AST after.

	Eventually I will problably end up using a dedicated lexer for parser constructors to lower the sloc.

	It uses the upfront constructors to help keep code from getitng to large since the target is to keep the sloc low
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
		Entry( Access_Public,       "public" )         \
		Entry( Access_Protected,    "protected" )      \
		Entry( Access_Private,      "private" )        \
		Entry( Access_MemberSymbol, "." )              \
		Entry( Access_StaticSymbol, "::")              \
		Entry( Ampersand,           "&" )              \
		Entry( Ampersand_DBL,       "&&" )             \
		Entry( Assign_Classifer,    ":" )              \
		Entry( BraceCurly_Open,     "{" )              \
		Entry( BraceCurly_Close,    "}" )              \
		Entry( BraceSquare_Open,    "[" )              \
		Entry( BraceSquare_Close,   "]" )              \
		Entry( Capture_Start,       "(" )              \
		Entry( Capture_End,         ")" )              \
		Entry( Comment,             "__comment__" )    \
		Entry( Char,                "__char__" )       \
		Entry( Comma,               "," )              \
		Entry( Decl_Class,          "class" )          \
		Entry( Decl_Enum,           "enum" )           \
		Entry( Decl_Friend,         "friend" )         \
		Entry( Decl_Namespace,      "namespace" )      \
		Entry( Decl_Struct,         "struct" )         \
		Entry( Decl_Typedef,        "typedef" )        \
		Entry( Decl_Using,          "using" )          \
		Entry( Decl_Union,          "union" )          \
		Entry( Identifier,          "__SymID__" )      \
		Entry( Number,              "number" )         \
		Entry( Operator,            "operator" )       \
		Entry( Spec_API,            txt(API_Keyword) ) \
		Entry( Spec_Alignas,        "alignas" )        \
		Entry( Spec_CLinkage,       "extern \"C\"" )   \
		Entry( Spec_Const,          "const" )          \
		Entry( Spec_Consteval,      "consteval" )      \
		Entry( Spec_Constexpr,      "constexpr" )      \
		Entry( Spec_Constinit,      "constinit" )      \
		Entry( Spec_Export,         "export" )         \
		Entry( Spec_Extern,         "extern" )         \
		Entry( Spec_Import,         "import" )         \
		Entry( Spec_Inline,         "inline" )         \
		Entry( Spec_Module,         "module" )         \
		Entry( Spec_Static,         "static" )         \
		Entry( Spec_ThreadLocal,    "thread_local" )   \
		Entry( Spec_Volatile,       "volatile")        \
		Entry( Star,                "*" )              \
		Entry( Statement_End,       ";" )              \
		Entry( String,              "__String__" )     \
		Entry( Type_Unsigned, 	    "unsigned" )       \
		Entry( Type_Signed,         "signed" )         \
		Entry( Type_Short,          "short" )          \
		Entry( Type_Long,           "long" )

		enum class TokType : u32
		{
		#	define Entry( Name_, Str_ ) Name_,
			Define_TokType
		#	undef Entry

			Num,
			Invalid
		};

		struct Token
		{
			char const* Text;
			sptr        Length;
			TokType     Type;
			bool 	    IsAssign;
		};

		TokType get_tok_type( char const* word, s32 length )
		{
			local_persist
			char const* lookup[(u32)TokType::Num] =
			{
			#	define Entry( Name_, Str_ ) Str_,
				Define_TokType
			#	undef Entry
			};

			for ( u32 index = 0; index < (u32)TokType::Num; index++ )
			{
				if ( str_compare( word, lookup[index], length ) == 0 )
					return scast(TokType, index);
			}

			return TokType::Invalid;
		}

		char const* str_tok_type( TokType type )
		{
			local_persist
			char const* lookup[(u32)TokType::Num] =
			{
			#	define Entry( Name_, Str_ ) Str_,
				Define_TokType
			#	undef Entry
			};

			return lookup[(u32)type];
		}

	#	undef Define_TokType

		inline
		bool tok_is_specifier( Token const& tok )
		{
			return tok.Type >= TokType::Spec_API && tok.Type <= TokType::Star
				|| tok.Type == TokType::Ampersand
				|| tok.Type == TokType::Ampersand_DBL
			;
		}

		Arena LexAllocator;

		struct TokArray
		{
			s32          Idx;
			Array(Token) Arr;

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
					String token_str = string_make_length( g_allocator, Arr[Idx].Text, Arr[Idx].Length );

					log_failure( "gen::%s: expected %s, got %s", context, str_tok_type(type), str_tok_type(Arr[Idx].Type) );

					return Code::Invalid;
				}

				Idx++;
			}
		};

		TokArray lex( s32 length, char const* content)
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
					return { 0, nullptr };
				}
			do_once_end

			local_persist thread_local
			Array(Token) Tokens = nullptr;

			s32         left    = length;
			char const* scanner = content;

			char const* word        = scanner;
			s32         word_length = 0;

			SkipWhitespace();
			if ( left <= 0 )
			{
				log_failure( "gen::lex: no tokens found (only whitespace provided)" );
				return { 0, nullptr };
			}

			if ( Tokens )
				array_clear( Tokens );

			array_init_reserve( Tokens, arena_allocator( & LexAllocator), length / 8 );

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
						else while ( left && current == *(scanner - 1) && length < 3 )
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
							else while ( left && current == *(scanner - 1) && length < 3 )
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

								while ( left && current != '\n' )
								{
									move_forward();
									token.Length++;
								}
							}
							else if ( current == '*' )
							{
								token.Type = TokType::Comment;

								while ( left && ( current != '*' || *(scanner + 1) != '/' ) )
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
					String context_str = zpl::string_sprintf_buf( g_allocator, "%s", scanner, min( 100, left ) );

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


					String tok_str = zpl::string_sprintf_buf( g_allocator, "%s", token.Text, token.Length );

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
				return { 0, nullptr };
			}

			return { 0, Tokens };
		#	undef current
		#	undef move_forward
		#	undef SkipWhitespace
		#	undef SkipWhitespace_Checked
		}
	}

#pragma region Helper Macros
#	define check_parse_args( func, length, def )                         \
	if ( length <= 0 )                                                   \
	{                                                                    \
		log_failure( "gen::" txt(func) ": length must greater than 0" ); \
		return Code::Invalid;                                            \
	}                                                                    \
	if ( def == nullptr )                                                \
	{                                                                    \
		log_failure( "gen::" txt(func) ": def was null" );               \
		return Code::Invalid;                                            \
	}

#	define currtok      toks.Arr[toks.Idx]
#	define eat( Type_ ) toks.__eat( Type_, txt(context) )
#	define left         array_count(toks.Arr) - toks.Idx

#	define check( Type_ ) left && currtok.Type == Type_
#pragma endregion Helper Macros

	Code parse_class( s32 length, char const* def )
	{
		using namespace Parser;

	#	define context parse_class

		TokArray toks = lex( length, def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		Token name { nullptr, 0, TokType::Invalid };

		Code parent    = { nullptr };
		Code speciifes = { nullptr };
		Code body      = { nullptr };

		do
		{

		}
		while ( left--, left > 0 );

		return Code::Invalid;
	#	undef context
	}

	Code parse_enum( s32 length, char const* def )
	{
		using namespace Parser;

	#	define context parse_enum
		check_parse_args( parse_enum, length, def );

		TokArray toks = lex( length, def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

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

			Code untyped_body = untyped_str( entries_length, entries_code );

			result->Type = is_enum_class ? Enum_Class : Enum;
			result->add_entry( untyped_body );
		}
		else
		{
			result->Type = is_enum_class ? Enum_Class_Fwd : Enum_Fwd;
		}

		result->Name = get_cached_string( name.Text, name.Length );

		if ( type )
			result->add_entry( type );

		result.lock();
		return result;
	#	undef context
	}

	Code parse_execution( s32 length, char const* exec_def )
	{
		not_implemented( parse_execution );
	}

	Code parse_friend( s32 length, char const* def )
	{
		using namespace Parser;

#		define context parse_friend
		check_parse_args( parse_friend, length, def );

		TokArray toks = lex( length, def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		eat( TokType::Decl_Friend );

		// This can either be a simple type, or a function declaration.
		// If its a function declaration, it will have a return type, followed by a name, followed by a parameter list.
		// If its a simple type, it will have a type, followed by a name.




		using namespace ECode;

		Code result = make_code();



		return result;
	}

	Code parse_global_body( s32 length, char const* def )
	{
		not_implemented( parse_global_body );
	}

	Code parse_function( s32 length, char const* def )
	{
		using namespace Parser;

		check_parse_args( parse_function, length, def );

		Arena mem;
		do_once_start
			arena_init_from_allocator( & mem, heap(), kilobytes( 10 ) );
		do_once_end

		// Pretty sure its impossible to have more than this.
		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };

		u8 num_specifiers;

		// Making all significant tokens have a max length of 128 for this parser.
		constexpr sw LengthID = 128;

		struct Param
		{
			char const Type[LengthID];
			char const Name[LengthID];
		};

		static
		Param Params[ 64 ] { 0 };




		using namespace ECode;

		Code specifiers = def_specifiers( num_specifiers, specs_found );

		Code params   = make_code();
		// Code ret_type = def_type( ret_length, ret_type_str );
		// Code body     = untyped_str( body_length, body_str );

		Code
		result       = make_code();
		// result->Name = get_cached_string( name, name_length );
		result->Type = Function;

		// result->add_entry( body );

		if ( specifiers )
			result->add_entry( specifiers );

		// result->add_entry( ret_type );

		if ( params )
			result->add_entry( params );

		result.lock();
		return result;
	}

	Code parse_namespace( s32 length, char const* def )
	{
		not_implemented( parse_namespace );
	}

	Code parse_operator( s32 length, char const* def )
	{
		not_implemented( parse_operator );
	}

	Code parse_struct( s32 length, char const* def )
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

	Code parse_variable( s32 length, char const* def )
	{
		using namespace Parser;

	#	define context parse_variable
		check_parse_args( parse_variable, length, def );

		TokArray toks = lex( length, def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		Token* name = nullptr;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Code lang_linkage;
		Code attributes;
		Code array_expr = { nullptr };

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

			attributes = untyped_str( currtok.Length, currtok.Text );

			eat( TokType::BraceSquare_Close );
		}

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok.Text, currtok.Length );

			switch ( spec )
			{
				case ESpecifier::Constexpr:
				case ESpecifier::Constinit:
				case ESpecifier::Export:
				case ESpecifier::External_Linkage:
				case ESpecifier::Import:
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
					lang_linkage = untyped_str( currtok.Length, currtok.Text );
					eat( TokType::String );
				}

				continue;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		Code type = parse_type( toks, txt(parse_variable) );

		if ( type == Code::Invalid )
			return Code::Invalid;

		if ( currtok.Type != TokType::Identifier )
		{
			log_failure( "gen::parse_variable: expected identifier, recieved " txt(currtok.Type) );
			return Code::Invalid;
		}

		name = & currtok;
		eat( TokType::Identifier );

		Code expr = { nullptr };

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

			expr = untyped_str( expr_tok.Length, expr_tok.Text );
		}

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

			Token
			untyped_tok = currtok;

			while ( left && currtok.Type != TokType::BraceSquare_Close )
			{
				untyped_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)untyped_tok.Text;
			}

			array_expr = untyped_str( untyped_tok.Length, untyped_tok.Text );

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
		}

		eat( TokType::Statement_End );

		using namespace ECode;

		Code result  = make_code();
		result->Type = Variable;
		result->Name = get_cached_string( name->Text, name->Length );

		result->add_entry( type );

		if (array_expr)
			type->add_entry( array_expr );

		if ( attributes )
			result->add_entry( attributes );

		if ( expr )
			result->add_entry( expr );

		return Code::Invalid;
	#	undef context
	}

	Code parse_type( Parser::TokArray& toks, char const* func_name )
	{
		using namespace Parser;

	#	define context parse_type

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Token name = { nullptr, 0, TokType::Invalid };

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok.Text, currtok.Length );

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
			log_failure( "%s: Error, unexpected end of type definition", func_name );
			return Code::Invalid;
		}

		if (   currtok.Type == TokType::Decl_Class
			|| currtok.Type == TokType::Decl_Struct )
		{
			name = currtok;
			eat( currtok.Type );

			name.Length = (  (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
			eat( TokType::Identifier );
		}
		else
		{
			name = currtok;
			eat( TokType::Identifier );
		}

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok.Text, currtok.Length );

			if (   spec != ESpecifier::Const
			    && spec != ESpecifier::Ptr
			    && spec != ESpecifier::Ref
			    && spec != ESpecifier::RValue
				&& spec < ESpecifier::Type_Signed )
			{
				log_failure( "%s: Error, invalid specifier used in type definition: %s", func_name, currtok.Text );
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
		result->Name = get_cached_string( name.Text, name.Length );

		if (num_specifiers)
		{
			Code specifiers = def_specifiers( num_specifiers, specs_found );

			result->add_entry( specifiers );
		}

		return result;
	#	undef context
	}

	Code parse_type( s32 length, char const* def )
	{
		using namespace Parser;

	#	define context parse_type
		check_parse_args( parse_type, length, def );

		TokArray toks = lex( length, def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		Code result = parse_type( toks, txt(parse_type) );

		result.lock();
		return result;
	#	undef context
	}

	Code parse_typedef( s32 length, char const* def )
	{
		using namespace Parser;

	#	define context parse_typedef
		check_parse_args( parse_typedef, length, def );

		TokArray toks = lex( length, def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

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

			array_expr = untyped_str( untyped_tok.Length, untyped_tok.Text );

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
		}

		eat( TokType::Statement_End );

		using namespace ECode;

		Code
		result       = make_code();
		result->Type = Typedef;
		result->Name = get_cached_string( name.Text, name.Length );

		result->add_entry( type );

		if ( array_expr )
			type->add_entry( array_expr );

		result.lock();
		return result;
	#	undef context
	}

	Code parse_using( s32 length, char const* def )
	{
		using namespace Parser;

	#	define context parse_using
		check_parse_args( parse_using, length, def );

		TokArray toks = lex( length, def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

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

		eat( TokType::Identifier );

		if ( currtok.IsAssign )
		{
			eat( TokType::Operator );

			type = parse_type( toks, txt(parse_typedef) );
		}

		if ( ! is_namespace && check( TokType::BraceSquare_Open ) )
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

			Token
			untyped_tok = currtok;

			while ( left && currtok.Type != TokType::BraceSquare_Close )
			{
				untyped_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)untyped_tok.Text;
			}

			array_expr = untyped_str( untyped_tok.Length, untyped_tok.Text );

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
		}

		eat( TokType::Statement_End );

		using namespace ECode;

		Code
		result       = make_code();
		result->Type = is_namespace ? Using : Using_Namespace;
		result->Name = get_cached_string( name->Text, name->Length );

		result->add_entry( type );

		if ( array_expr )
			type->add_entry( array_expr );

		result.lock();
		return result;
	#	undef context
	}

	s32 parse_classes( s32 length, char const* class_defs, Code* out_class_codes )
	{
		not_implemented( parse_classes );
	}

	s32 parse_enums( s32 length, char const* enum_defs, Code* out_enum_codes )
	{
		not_implemented( parse_enums );
	}

	s32 parse_friends( s32 length, char const* friend_defs, Code* out_friend_codes )
	{
		not_implemented( parse_friends );
	}

	s32 parse_functions ( s32 length, char const* fn_defs, Code* out_fn_codes )
	{
		not_implemented( parse_functions );
	}

	s32 parse_namespaces( s32 length, char const* namespace_defs, Code* out_namespaces_codes )
	{
		not_implemented( parse_namespaces );
	}

	s32 parse_operators( s32 length, char const* operator_defs, Code* out_operator_codes )
	{
		not_implemented( parse_operators );
	}

	s32 parse_structs( s32 length, char const* struct_defs, Code* out_struct_codes )
	{
		not_implemented( parse_structs );
	}

	s32 parse_variables( s32 length, char const* vars_def, Code* out_var_codes )
	{
		not_implemented( parse_variables );
	}

	s32 parse_typedefs( s32 length, char const* typedef_def, Code* out_typedef_codes )
	{
		not_implemented( parse_typedefs );
	}

	s32 parse_usings( s32 length, char const* usings_def, Code* out_using_codes )
	{
		not_implemented( parse_usings );
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

	Code untyped_str( s32 length, char const* str )
	{
		Code
		result          = make_code();
		result->Name    = get_cached_string( str, length );
		result->Type    = ECode::Untyped;
		result->Content = result->Name;

		result.lock();
		return result;
	}

	Code untyped_fmt(char const* fmt, ...)
	{
		local_persist thread_local
		char buf[ZPL_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		sw length = str_fmt_va(buf, ZPL_PRINTF_MAXLEN, fmt, va);
		va_end(va);

		Code
		result          = make_code();
		result->Name    = get_cached_string( fmt, str_len(fmt, MaxNameLength) );
		result->Type    = ECode::Untyped;
		result->Content = get_cached_string( buf, length );

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
		result->Name    = get_cached_string( fmt, str_len(fmt, MaxNameLength) );
		result->Type    = ECode::Untyped;
		result->Content = get_cached_string( buf, length );

		result.lock();
		return result;
	}
#pragma endregion Untyped Constructors
#pragma endregion Gen Interface

#pragma region Builder
	void Builder::print( Code code )
	{
		Buffer = string_append_fmt( Buffer, "%s\n\n", code.to_string() );
	}

	bool Builder::open( char const* path )
	{
		FileError error = file_open_mode( & File, EFileMode_WRITE, path );

		if ( error != EFileError_NONE )
		{
			log_failure( "gen::File::open - Could not open file: %s", path);
			return false;
		}

		Buffer = string_make( g_allocator, "" );

		return true;
	}

	void Builder::write()
	{
		bool result = file_write( & File, Buffer, string_length(Buffer) );

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
