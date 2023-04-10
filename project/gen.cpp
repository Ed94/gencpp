#include "Bloat.hpp"
#include "gen.hpp"

#ifdef gentime
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
#	ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
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
#	endif

	Code access_public;
	Code access_protected;
	Code access_private;

	Code spec_constexpr;
	Code spec_const;
	Code spec_inline;
	Code sepc_ptr;
	Code spec_ref;
#pragma endregion Constants

#	pragma region AST
	const Code Code::Invalid;

	bool AST::add( AST* other )
	{
		switch ( Type )
		{
			using namespace ECode;

			case Invalid:
			break;

			case Untyped:
			break;

			case Access_Public:
			break;

			case Access_Protected:
			break;

			case Access_Private:
			break;

			case Class:
			break;

			case Class_FwdDecl:
			break;

			case Class_Body:
			break;

			case Enum:
			break;

			case Enum_FwdDecl:
			break;

			case Enum_Body:
			break;

			case Enum_Class:
			break;

			case Enum_Class_FwdDecl:
			break;

			case Friend:
			break;

			case Function:
			break;

			case Function_Body:
			break;

			case Function_FwdDecl:
			break;

			case Global_Body:
			break;

			case Namespace:
			break;

			case Namespace_Body:
			break;

			case Operator:
			break;

			case Operator_FwdDecl:
			break;

			case Parameters:
			break;

			case Specifiers:
			break;

			case Struct:
			break;

			case Struct_Body:
			break;

			case Variable:
			break;

			case Typedef:
			break;

			case Typename:
			break;

			case Using:
			break;
		}

		array_append( Entries, other );

		other->Parent = this;
		return true;
	}

	bool AST::check()
	{
		switch ( Type )
		{
			using namespace ECode;

			case Invalid:
			break;

			case Untyped:
			break;

			case Access_Public:
			break;

			case Access_Protected:
			break;

			case Access_Private:
			break;

			case Enum:
			break;

			case Enum_FwdDecl:
			break;

			case Enum_Body:
			break;

			case Friend:
			break;

			case Function:
			break;

			case Function_Body:
			break;

			case Function_FwdDecl:
			break;

			case Global_Body:
			break;

			case Namespace:
			break;

			case Namespace_Body:
			break;

			case Operator:
			break;

			case Operator_FwdDecl:
			break;

			case Parameters:
			break;

			case Specifiers:
			break;

			case Struct:
			break;

			case Struct_Body:
			break;

			case Variable:
			break;

			case Typedef:
			break;

			case Typename:
			break;

			case Using:
			break;
		}

		return true;
	}

	AST* AST::duplicate()
	{
		using namespace ECode;

		Code
		result = make_code();

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
			case Class_FwdDecl:
			case Enum_FwdDecl:
			case Function_FwdDecl:
			case Specifiers:
				// Can just be the same, as its a cached string.
				result->Content = Content;
			return;

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
			case Struct_FwdDecl:
			case Struct_Body:
			case Variable:
			case Typedef:
			case Typename:
			case Using:
				result->Entries = make_code_entries();
				s32     index = 0;
				s32     left  = array_count( result->Entries );
				while ( left -- )
				{
					// This will naturally duplicate the entire chain duplicate all of the ast nodes.
					// It may not be the most optimal way for memory reasons, however figuring out the heuristic
					// For when it should reparent all nodes or not is not within the simple scope of this library.
					result->add_entry( Entries[index]->duplicate() );
					result->Entries[index]->Parent = this;
					index++;
				}
			return;
		}
	}

	String AST::to_string() const
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
			break;

			case Class_FwdDecl:
			break;

			case Class_Body:
			break;

			case Enum:
			break;

			case Enum_FwdDecl:
			break;

			case Enum_Body:
			break;

			case Enum_Class:
			break;

			case Enum_Class_FwdDecl:
			break;

			case Friend:
			break;

			case Function:
			{
				u32 index = 0;
				u32 left  = array_count( Entries );

				if ( left <= 0 )
					log_failure( "Code::to_string - Name: %s Type: %s, expected definition", Name, Type );

				if ( Entries[index]->Type == Specifiers )
				{
					result = string_append_fmt( result, "%s", Entries[index]->to_string() );
					index++;
					left--;
				}

				if ( left <= 0 )
					log_failure( "Code::to_string - Name: %s Type: %s, expected return type", Name, Type );

				result = string_append_fmt( result, "\n%s %s(", Entries[index]->to_string(), Name );
				index++;
				left--;

				if ( left && Entries[index]->Type == Parameters )
				{
					result = string_append_fmt( result, "%s", Entries[index]->to_string() );
					index++;
					left--;
				}

				result = string_append_fmt( result, ")\n{\n%s\n}", Entries[index]->to_string() );
			}
			break;

			case Function_FwdDecl:
			{
				u32 index = 0;
				u32 left  = array_count( Entries );

				if ( left <= 0 )
					log_failure( "Code::to_string - Name: %s Type: %s, expected definition", Name, Type );

				if ( Entries[index]->Type == Specifiers )
				{
					result = string_append_fmt( result, "%s\n", Entries[index]->to_string() );
					index++;
					left--;
				}

				if ( left <= 0 )
					log_failure( "Code::to_string - Name: %s Type: %s, expected return type", Name, Type );

				result = string_append_fmt( result, "\n%s %s(", Entries[index]->to_string(), Name );
				index++;
				left--;

				if ( left && Entries[index]->Type == Parameters )
				{
					result = string_append_fmt( result, "%s", Entries[index]->to_string() );
					index++;
					left--;
				}

				result = string_appendc( result, ");\n" );
			}
			break;

			case Function_Body:
				log_failure("NOT SUPPORTED YET");
			break;

			case Global_Body:
			break;

			case Namespace:
				log_failure("NOT SUPPORTED YET");
			break;

			case Namespace_Body:
				log_failure("NOT SUPPORTED YET");
			break;

			case Operator:
			break;

			case Operator_FwdDecl:
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
				result = string_append_fmt( result, "%s", Content );
			break;

			case Struct:
				log_failure("NOT SUPPORTED YET");
			break;

			case Struct_FwdDecl:
			break;

			case Struct_Body:
				log_failure("NOT SUPPORTED YET");
			break;

			case Variable:
				log_failure("NOT SUPPORTED YET");
			break;

			case Typedef:
				log_failure("NOT SUPPORTED YET");
			break;

			case Typename:
				result = string_append_fmt( result, "%s", Name );
			break;

			case Using:
				log_failure("NOT SUPPORTED YET");
			break;
		}

		return result;
	}
#	pragma endregion AST

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

		Code&
		InvalidCode_write = ccast( Code, Code::Invalid );
		InvalidCode_write = make_code();

#	ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
		Code&
		t_bool_write = ccast( Code, t_void );
		t_bool_write = def_type( txt(void) );

#		define def_constant_code_type( Type_ ) \
		Code&                                  \
		t_##Type_ = def_type( txt(Type_) );    \
		t_##Type_.lock()

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
#		undef def_constant_code_type
#	endif

		Code&
		spec_constexpr_write = ccast( Code, spec_constexpr );
		spec_constexpr_write = def_specifiers( 1, ESpecifier::Constexpr );

#		define def_constant_spec( Type_, ... )                                    \
		Code&                                                                     \
		spec_##Type_ = def_specifiers( macro_num_args(__VA_ARGS__), __VA_ARGS__); \
		spec_##Type_.lock();

		def_constant_spec( const, ESpecifier::Const );
		def_constant_spec( inline, ESpecifier::Inline );
#		undef def_constant_spec
	}

	void clear_code_pools()
	{
		using namespace StaticData;

		// Clear the code pools
		{
			s32 index = 0;
			s32 left  = 0;
			while (( left-- ))
			{
				Pool* code_pool = & CodePools[index];
				pool_free( code_pool );
			}

			array_clear( CodePools );
		}

		// Clear the code entries pools
		{
			s32 index = 0;
			s32 left  = 0;
			while (( left-- ))
			{
				Arena* code_entries_arena = & CodeEntriesArenas[index];
				arena_free( code_entries_arena );
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

		ct CodePOD Invalid = { nullptr, nullptr, nullptr, nullptr, ECode::Invalid, EOperator::Invalid, false, {0} };

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

		* result = pcast( AST, Invalid);

		return result;
	}

	Array(AST*) make_code_entries()
	{
		using namespace StaticData;

		AllocatorInfo allocator = { nullptr, nullptr };

		s32 left = array_count( CodeEntriesArenas );
		do
		{
			if ( arena_size_remaining(CodeEntriesArenas, ZPL_DEFAULT_MEMORY_ALIGNMENT) > ZPL_ARRAY_GROW_FORMULA(0) )
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

	forceinline
	bool operator_member_symbol_check( Code entry )
	{
		using namespace ECode;

		// Is assumed by the time this is called, entry has been valided to not be null or invalid.
		switch ( entry->Type )
		{
			case Access_Public:
			case Access_Protected:
			case Access_Private:
			case Class_Body:
			case Class_FwdDecl:
			case Enum_FwdDecl:
			case Enum_Body:
			case Enum_Class_FwdDecl:
			case Friend:
			case Function:
			case Function_FwdDecl:
			case Global_Body:
			case Namespace:
			case Namespace_Body:
			case Operator:
			case Operator_FwdDecl:
			case Parameters:
			case Specifiers:
			case Struct_FwdDecl:
			case Struct_Body:
			case Typename:
				return false;
		}

		return true;
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

#	pragma region Helper Functions
	// This snippet is used in nearly all the functions.
#	define name_check( Context_, Length_, Name_ )                                               \
	{                                                                                           \
		if ( Length_ <= 0 )                                                                     \
		{                                                                                       \
			log_failure( "gen::%s: Invalid name length provided - %d", txt(Context_), length ); \
			return Code::Invalid;                                                               \
		}                                                                                       \
                                                                                                \
		if ( Name_ == nullptr )                                                                 \
		{                                                                                       \
			log_failure( "gen::%s: name is null", txt(Context_) );                              \
			return Code::Invalid;                                                               \
		}                                                                                       \
	}

#	define null_check( Context_, Code_ )                                           \
	if ( ! Code_ )                                                                 \
	{                                                                              \
		log_failure( "gen::%s: %s provided is null!", txt(Context_), txt(Code_) ); \
		return Code::Invalid;                                                      \
	}

#	define null_or_invalid_check( Context_, Code_ )                                      \
	{                                                                                    \
		if ( ! Code_ )                                                                   \
		{                                                                                \
			log_failure( "gen::%s: %s provided is null!", txt(Context_) );               \
			return Code::Invalid;                                                        \
		}                                                                                \
                                                                                         \
		if ( Code_->is_invalid() )                                                       \
		{                                                                                \
			log_failure("gen::%s: %s provided is invalid!", txt(Context_), txt(Code_) ); \
			return Code::Invalid;                                                        \
		}                                                                                \
	}

#	define not_implemented( Context_ )                              \
		log_failure( "gen::%s: This function is not implemented" ); \
		return Code::Invalid;
#	pragma endregion Helper Functions

#	pragma region Upfront Constructors
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

			result->Type    = Class;
			result->Entries = make_code_entries();
			result->add_entry( body );
		}
		else
		{
			result->Type = Class_FwdDecl;
		}

		if ( parent )
			result->add_entry( parent );

		if ( specifiers )
			result->add_entry( specifiers );

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

			result->Entries = make_code_entries();
			result->add_entry( body );
		}
		else if ( specifier == EnumClass )
		{
			result->Type = specifier == EnumClass ?
				Enum_Class_FwdDecl : Enum_FwdDecl;
		}

		if ( type )
		{
			result->add_entry( type );
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
			case Class_FwdDecl:
			case Function_FwdDecl:
			case Operator_FwdDecl:
			case Struct_FwdDecl:
			break;

			default:
				// Technically friends can have a function body. I will not support it so if you want that add it yourself. - Ed.
				log_failure("gen::def_friend: declaration cannot be used with friend, must be a forward declare - %s", declaration->debug_str());
				return Code::Invalid;
		}

		Code
		result          = make_code();
		result->Type    = Friend;
		result->Entries = make_code_entries();

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
		result          = make_code();
		result->Name    = get_cached_string( name, length );
		result->Entries = make_code_entries();

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
			result->Type = Function_FwdDecl;
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

		return result;
	}

	Code def_operator( OperatorT op, Code params, Code ret_type, Code specifiers, Code body )
	{
		using namespace EOperator;

		if ( op == Invalid )
		{
			log_failure("gen::def_operator: op cannot be invalid");
			return Code::Invalid;
		}

		switch ( op )
		{
			case Assign:
			case Assign_Add:
			case Assign_Subtract:
			case Assgin_Multiply:
			case Assgin_Divide:
			case Assgin_Modulo:

			break;
		}

		Code
		result       = make_code();
		result->Type = ECode::Operator;

		if ( body )
		{

		}
		else
		{

		}

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
		result          = make_code();
		result->Type    = Parameters;
		result->Name    = get_cached_string( name, length );
		result->Entries = make_code_entries();

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
		result          = make_code();
		result->Name    = get_cached_string( name, length );
		result->Entries = make_code_entries();

		if ( body )
		{
			result->Type = Struct;
			result->add_entry( body );
		}
		else
		{
			result->Type = Struct_FwdDecl;
		}

		if ( parent )
			result->add_entry( parent );

		if ( specifiers )
			result->add_entry( specifiers );

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
		result          = make_code();
		result->Name    = get_cached_string( name, length );
		result->Type    = ECode::Variable;
		result->Entries = make_code_entries();

		if ( specifiers )
			result->add_entry( specifiers );

		result->add_entry( type );

		if ( value )
			result->add_entry( value );

		return result;
	}

	Code def_type( u32 length, char const* name,  Code specifiers )
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
		{
			result->Entries = make_code_entries();
			result->add_entry( specifiers );
		}

		result.lock();
		return result;
	}

	Code def_using( u32 length, char const* name, Code type, UsingT specifier )
	{
		name_check( def_using, length, name );
		null_check( def_using, type );

		Code
		result       = make_code();
		result->Name = get_cached_string( name, length );

		switch ( specifier )
		{
			case UsingRegular:
				result->Entries = make_code_entries();
				result->Type    = ECode::Using;

				result->add_entry( type );
			break;

			case UsingNamespace:
				result->Type = ECode::Using_Namespace;
			break;
		}

		result.lock();
		return result;
	}

	Code def_class_body( s32 num, ... )
	{
		using namespace ECode;

		if ( num == 0 )
		{
			log_failure("gen::def_class_body: num cannot be zero");
			return Code::Invalid;
		}

		Code
		result          = make_code();
		result->Type    = Class_Body;
		result->Entries = make_code_entries();

		va_list va;
		va_start(va, num);
		do
		{
			Code entry = va_arg(va, Code);

			switch ( entry->Type )
			{
				case Enum_Body:
				case Execution:
				case Function_Body:
				case Global_Body:
				case Namespace:
				case Namespace_Body:
				case Parameters:
				case Specifiers:
				case Struct_Body:
				case Typename:
				case Using_Namespace:
				{
					log_failure("gen::def_class_body: Entry type is not allowed: %s", ECode::str(entry->Type) );
					return Code::Invalid;
				}

				case Operator:
					// If an operator is getting added, we need to verify
					// the definition conforms to the format required for member symbols.
					if ( ! operator_member_symbol_check( entry ) )
					{
						log_failure( "gen::def_class_body: Operator entry was not a valid member symbol.");
						return Code::Invalid;
					}
				break;
			}

			result->add_entry( entry );
		}
		while ( num--, num > 0 );
		va_end(va);

		result.lock();
		return result;
	}

	Code def_enum_body( s32 num, ... )
	{
		not_implemented( def_enum_body );
	}

	Code def_function_body( s32 num, ... )
	{
		using namespace ECode;

		if ( num <= 0 )
		{
			log_failure("gen::def_function_body: num cannot zero or neg");
			return Code::Invalid;
		}

		Code result = make_code();

		array_init( result->Entries, g_allocator );

		va_list va;
		va_start(va, num);
		do
		{
			Code entry = va_arg(va, Code);

			if ( ! entry )
			{
				log_failure("gen::def_function_body: Provided an invalid entry!");
				return Code::Invalid;
			}

			switch ( entry->Type )
			{
				case Access_Public:
				case Access_Protected:
				case Access_Private:
				case Class_Body:
				case Enum_Body:
				case Friend:
				case Function_Body:
				case Function_FwdDecl:
				case Global_Body:
				case Namespace:
				case Namespace_Body:
				case Operator:
				case Parameters:
				case Specifiers:
				case Struct_Body:
				case Typename:
				{
					log_failure("gen::def_function_body: Entry type is not allowed: %s", entry->type_str() );
					return Code::Invalid;
				}

				default:
					break;
			}

			result->add_entry( entry );
		}
		while ( num--, num > 0 );
		va_end(va);

		return result;
	}

	Code def_function_body( s32 num, Code* codes )
	{
		using namespace ECode;

		if ( num <= 0 )
		{
			log_failure("gen::def_function_body: num cannot zero or neg");
			return Code::Invalid;
		}

		if ( codes == nullptr )
		{
			log_failure("gen::def_function_body: Provided a null array of codes!");
			return Code::Invalid;
		}

		Code result = make_code();

		array_init( result->Entries, g_allocator );
		do
		{
			Code entry = *codes;

			if ( ! entry )
			{
				log_failure("gen::def_function_body: Provided an invalid entry!");
				return Code::Invalid;
			}

			switch ( entry->Type )
			{
				case Access_Public:
				case Access_Protected:
				case Access_Private:
				case Class_Body:
				case Enum_Body:
				case Friend:
				case Function_Body:
				case Function_FwdDecl:
				case Global_Body:
				case Namespace:
				case Namespace_Body:
				case Operator:
				case Parameters:
				case Specifiers:
				case Struct_Body:
				case Typename:
				{
					log_failure("gen::def_function_body: Entry type is not allowed: %s", entry->type_str() );
					return Code::Invalid;
				}

				default:
					break;
			}

			result->add_entry( entry );
		}
		while ( num--, num > 0 );

		return result;
	}

	Code def_global_body( s32 num, ... )
	{
		using namespace ECode;

		if ( num <= 0 )
		{
			log_failure("gen::def_global_body: num cannot zero or neg");
			return Code::Invalid;
		}

		Code result = make_code();

		array_init( result->Entries, g_allocator );

		va_list va;
		va_start(va, num);
		do
		{
			Code entry = va_arg(va, Code);

			if ( ! entry )
			{
				log_failure("gen::def_global_body: Provided an invalid entry!");
				return Code::Invalid;
			}

			switch ( entry->Type )
			{
				case Access_Public:
				case Access_Protected:
				case Access_Private:
				case Class_Body:
				case Enum_Body:
				case Execution:
				case Friend:
				case Function_Body:
				case Global_Body:
				case Namespace_Body:
				case Parameters:
				case Specifiers:
				case Struct_Body:
				case Typename:
				{
					log_failure("gen::def_global_body: Entry type is not allowed: %s", entry->type_str() );
					return Code::Invalid;
				}

				default:
					break;
			}

			result->add_entry( entry );
		}
		while ( num--, num > 0 );
		va_end(va);

		return result;
	}

	Code def_namespace_body( s32 num, ... )
	{
		using namespace ECode;

		if ( num <= 0 )
		{
			log_failure("gen::make_specifier: num cannot be zero or less");
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Type = Namespace_Body;

		va_list va;
		va_start(va, num);
		do
		{
			Code entry = va_arg(va, Code);

			switch ( entry->Type )
			{
				case Namespace_Body:
				case Parameters:
				case Specifiers:
				case Struct_Body:
				case Typename:
					log_failure("gen::def_function_body: Entry type is not allowed: %s", ECode::str(entry->Type) );
					return Code::Invalid;

				default:
					break;
			}

			result->add_entry( entry );
		}
		while ( num--, num > 0 );
		va_end(va);

		return result;
	}

	Code def_params( s32 num, ... )
	{
		using namespace ECode;

		if (num <= 0)
		{
			log_failure( "TT::make_paramters: num cannot be zero or neg" );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Type = Parameters;

		va_list va;
		va_start(va, num);

		Code type = va_arg(va, Code);

		char const* name        = va_arg(va, char const*);
		s32         name_length = strnlen(name, MaxNameLength);

		result->Name = get_cached_string( name, name_length );

		array_init( result->Entries, g_allocator );

		if ( type->Type != Typename )
		{
			log_failure( "gen::def_parameters: type of param %d is not a Typename", num - num + 1 );
			return Code::Invalid;
		}

		result->add_entry( type );

		while( num -= 2, num && num % 2 == 0 )
		{
			type        = va_arg(va, Code);
			name_length = va_arg(va, u32);
			name        = va_arg(va, char const*);

			Code
			param       = make_code();
			param->Type = Parameters;
			param->Name = get_cached_string(name, name_length);

			array_init( param->Entries, StaticData::Allocator_CodePool );

			if ( type->Type != Typename )
			{
				log_failure( "gen::def_parameters: type of param %d is not a Typename", num - num + 1 );
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

	Code def_specifiers( s32 num, ... )
	{
		if ( num <= 0 )
			log_failure("gen::make_specifier: num cannot be zero or less");

		// This should be more than enough...
		static u8 FixedSizedBuffer[kilobytes(1024)];

		static Arena str_arena;
		do_once_start
			arena_init_from_memory( & str_arena, FixedSizedBuffer, kilobytes(1024) );
		do_once_end

		Code
		result       = make_code();
		result->Type = ECode::Specifiers;

		String crafted = string_make( arena_allocator( & str_arena ), "" );

		va_list va;
		va_start(va, num);
		do
		{
			SpecifierT type = (SpecifierT)va_arg(va, int);

			switch ( type )
			{
				case ESpecifier::Alignas:
					crafted = string_append_fmt( (String)result->Content, "%s(%d)", ESpecifier::to_str(type), va_arg(va, u32) );
				break;

				default:
					const char* str = ESpecifier::to_str(type);

					crafted = string_append_fmt( (String)result->Content, "%s", str );
				break;
			}
		}
		while ( --num, num );
		va_end(va);

		result->Content = get_cached_string( crafted, string_length( crafted ) );

		arena_free( & str_arena );

		return result;
	}

	Code def_struct_body( s32 num, ... )
	{
		using namespace ECode;

		if ( num <= 0 )
		{
			log_failure("gen::def_struct_body: num must be at least 1");
			return Code::Invalid;
		}

		Code
		result          = make_code();
		result->Entries = make_code_entries();

		va_list va;
		va_start(va, num);
		do
		{
			Code entry = va_arg(va, Code);

			switch ( entry->Type )
			{
				case Enum_Body:
				case Execution:
				case Function_Body:
				case Global_Body:
				case Namespace:
				case Namespace_Body:
				case Parameters:
				case Specifiers:
				case Struct_Body:
				case Typename:
				case Using_Namespace:
				{
					log_failure("gen::def_struct_body: Entry type is not allowed: %s", ECode::str(entry->Type) );
					return Code::Invalid;
				}

				case Operator:
					// If an operator is getting added, we need to verify
					// the definition conforms to the format required for member symbols.
					if ( ! operator_member_symbol_check( entry ) )
					{
						log_failure( "gen::def_struct_body: Operator entry was not a valid member symbol.");
						return Code::Invalid;
					}
				break;
			}

			result->add_entry( entry );
		}
		while ( num--, num > 0 );
		va_end(va);

		return result;
	}
#	pragma endregion Upfront Constructors

#	pragma region Incremetnal Constructors
	Code make_class( s32 length, char const* name, Code parent, Code specifiers )
	{
		not_implemented( make_class );
	}

	Code make_enum( s32 length, char const* name, Code type, EnumT specifier )
	{
		not_implemented( make_class );
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
		result          = make_code();
		result->Name    = get_cached_string( name, length );
		result->Type    = Function;
		result->Entries = make_code_entries();

		Code
		body = make_code();
		body->Type    = Function_Body;
		body->Entries = make_code_entries();

		result->add_entry( make_code() );

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
		result          = make_code();
		result->Type    = ECode::Global_Body;
		result->Name    = get_cached_string( name, length );
		result->Entries = make_code_entries();

		// Making body at entry 0;
		result->add_entry( make_code() );

		return result;
	}

	Code make_namespace( s32 length, char const* name, Code parent, Code specifiers )
	{
		not_implemented( make_namespace );
	}

	Code make_operator( OperatorT op, Code params, Code ret_type, Code specifiers )
	{
		not_implemented( make_operator );
	}

	Code make_params()
	{
		not_implemented( make_params );
	}

	Code make_specifiers()
	{
		not_implemented( make_specifiers );
	}

	Code make_struct( s32 length, char const* name, Code parent, Code specifiers )
	{
		using namespace ECode;

		name_check( make_struct, length, name );

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

		Code
		result          = make_code();
		result->Type    = Struct;
		result->Name    = get_cached_string( name, length );
		result->Entries = make_code_entries();

		Code
		body          = make_code();
		body->Type    = Function_Body;
		body->Entries = make_code_entries();

		result->add_entry( make_code() );

		if ( parent )
			result->add_entry( parent );

		if ( specifiers )
			result->add_entry( specifiers );

		return result;
	}
#	pragma endregion Incremetnal Constructions

#	pragma region Parsing Constructors
	Code parse_class( s32 length, char const* def )
	{
		not_implemented( parse_class );
	}

	Code parse_enum( s32 length, char const* def )
	{
		not_implemented( parse_enum );
	}

	Code parse_execution( s32 length, char const* exec_def )
	{
		not_implemented( parse_execution );
	}

	Code parse_friend( s32 length, char const* def )
	{
		not_implemented( parse_friend );
	}

	Code parse_global_body( s32 length, char const* def )
	{
		not_implemented( parse_global_body );
	}

	Code parse_function( s32 length, char const* def )
	{
		if ( def == nullptr )
		{
			log_failure("gen::parse_proc: def is null!");
			return Code::Invalid;
		}

		Arena mem;
		do_once_start
			arena_init_from_allocator( & mem, heap(), kilobytes( 10 ) );
		do_once_end

		// Pretty sure its impossible to have more than this.
		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };

		u8 num_specifiers;

		// Making all significant tokens have a max length of 128 for this parser.
		ct sw LengthID = 128;

		struct Param
		{
			char const Type[LengthID];
			char const Name[LengthID];
		};

		static
		Param Params[ 64 ] { 0 };

		// Zero out params before a run of this func.
		memset( Params, 0, sizeof( Params ));

		char const* name;
		s32         name_length = 0;

		char const* body_str     = nullptr;
		s32         body_length  = 0;

		char const* ret_type_str = nullptr;
		s32         ret_length   = 0;

		s32         left         = length;
		char const* scanner      = def;

		while ( left -- )
		{
			#define SkipWhitespace() \
			while ( left && char_is_space( * scanner ) ) \
			{                                            \
				left--;                                  \
				scanner++ ;                              \
			}

			#define Get

			// Find all specifiers (if any) and the return type
			do
			{
				SkipWhitespace();

				s32         length = 0;
				char const* spec_or_type = scanner;

				while ( char_is_alpha( * scanner) || * scanner == '_')
				{
					length++;
					left--;
					scanner++;
				}

				SpecifierT type = ESpecifier::to_type( spec_or_type, length );

				if ( type != ESpecifier::Invalid )
				{
					specs_found[num_specifiers] = type;
					num_specifiers++;
					continue;
				}

				// Should be return type
				ret_type_str = scanner;
				ret_length   = length;
				scanner++;
				break;
			}
			while( left );

			SkipWhitespace();

			// Next up should be the name if before there were specifiers
			name = scanner;

			while ( char_is_alpha( * scanner) || * scanner == '_')
			{
				ret_length++;
				left--;
				scanner++;
			}

			// Parameters
			SkipWhitespace();

			if ( * scanner != '(' )
			{
				log_failure("gen::parse_proc: Error, expected a ( in %s", string_make_length( g_allocator, scanner - 4, 100) );
				return Code::Invalid;
			}

			scanner++;

			do
			{

			}
			while (0);

			// Get past ')'
			scanner++;

			SkipWhitespace()

			// get end specifiers




			if ( * scanner != '{')
			{

			}

			not_implemented( parse_function );
		}

		Code specifiers = def_specifiers( num_specifiers, specs_found );

		Code params   = make_code();
		Code ret_type = def_type( ret_length, ret_type_str );
		Code body     = untyped_str( body_length, body_str );

		Code
		result          = make_code();
		result->Name    = get_cached_string( name, name_length );
		result->Type    = ECode::Function;
		result->Entries = make_code_entries();

		result->add_entry( body );

		if ( specifiers )
			result->add_entry( specifiers );

		result->add_entry( ret_type );

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
		ct sw LengthID = 128;

		char const name  [LengthID] { 0 };
		char const parent[LengthID] { 0 };
	}

	Code parse_variable( s32 length, char const* def )
	{
		not_implemented( parse_variable );
	}

	Code parse_type( s32 length, char const* def )
	{
		not_implemented( parse_type );
	}

	Code parse_typdef( s32 length, char const* def )
	{
		not_implemented( parse_typedef );
	}

	Code parse_using( s32 length, char const* def )
	{
		not_implemented( parse_using );
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
#	pragma endregion Parsing Constructors

#	pragma region Untyped Constructors
	Code untyped_str( s32 length, char const* str )
	{
		Code
		result          = make_code();
		result->Name    = get_cached_string( str, length );
		result->Type    = ECode::Untyped;
		result->Content = result->Name;

		return result;
	}

	Code untyped_fmt(char const* fmt, ...)
	{
		local_persist thread_local
		char buf[ZPL_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		sw length = snprintf_va(buf, ZPL_PRINTF_MAXLEN, fmt, va);
		va_end(va);

		Code
		result          = make_code();
		result->Name    = get_cached_string( fmt, strnlen(fmt, MaxNameLength) );
		result->Type    = ECode::Untyped;
		result->Content = get_cached_string( buf, length );

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
		result->Name    = get_cached_string( fmt, strnlen(fmt, MaxNameLength) );
		result->Type    = ECode::Untyped;
		result->Content = get_cached_string( buf, length );

		result.lock();

		return result;
	}
#	pragma endregion Untyped Constructors
#pragma endregion Gen Interface

#pragma region Builder
	void Builder::print( Code code )
	{
		Buffer = string_append_fmt( Buffer, "%s\n\n", code->to_string() );
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

#pragma region Editor
#pragma endregion Editor

#pragma region Scanner
#pragma endregion Scanner
}
// end gentime
#endif
