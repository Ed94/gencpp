#include "Bloat.hpp"
#include "gen.hpp"

#ifdef gen_time
namespace gen
{
	ZPL_TABLE_DEFINE( StringTable, str_tbl_,   string );
	ZPL_TABLE_DEFINE( TypeTable,   type_tbl_ , Code   );

	namespace StaticData
	{
	#ifndef GEN_CODE_USE_SOA
		static array(CodePOD) CodePool = nullptr;

	#else
		using DataUnion = union
		{
			array(AST*)   Entries;
			string        Content;
		};

		namespace CodePool
		{
			array(CodeT)     Type;
			array(bool)      Readonly;
			array(AST*)      Parent;
			array(string)    Name;
			array(string)    Comment;
			array(DataUnion) Data;

			forceinline
			CodeT& type( sw index )
			{
				return Type[index];
			}

			forceinline
			bool& readonly( sw index )
			{
				return Readonly[index];
			}

			forceinline
			AST*& parent( sw index )
			{
				return Parent[index];
			}

			forceinline
			string& name( sw index )
			{
				return Name[index];
			}

			forceinline
			string& comment( sw index )
			{
				return Comment[index];
			}

			forceinline
			DataUnion& data( sw index )
			{
				return Data[index];
			}
		}
	#endif

		static array(arena) StringArenas =  nullptr;

		static StringTable StringMap;
		static TypeTable   TypeMap;

		static sw InitSize_CodePool       = megabytes(64);
		static sw InitSize_StringArena    = megabytes(32);
		static sw InitSize_StringTable    = megabytes(4);
		static sw InitSize_TypeTable      = megabytes(4);

		static allocator Allocator_CodePool    = heap();
		static allocator Allocator_StringArena = heap();
		static allocator Allocator_StringTable = heap();
		static allocator Allocator_TypeTable   = heap();
	}

#pragma region CONSTANTS
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

	Code spec_constexpr;
	Code spec_inline;
#pragma endregion CONSTANTS

	/*
		Used internally to retireve a Code object form the CodePool.
	*/
	static Code make_code()
	{
		using namespace StaticData;

#	ifndef GEN_CODE_USE_SOA
		ct CodePOD Invalid = { ECode::Invalid, false, nullptr, nullptr, nullptr, { nullptr } };

		array_append( CodePool, Invalid );

		return pcast( Code, array_back( CodePool ));
#	else

		array_append( CodePool::Type, ECode::Invalid );
		array_append( CodePool::Readonly, false );
		array_append( CodePool::Name, nullptr );
		array_append( CodePool::Comment, nullptr );
		array_append( CodePool::Data, { nullptr } );

		Code code { array_count( CodePool::Type) - 1 };

		return code;
#	endif
	}

#	pragma region AST
	bool AST::add( AST* other )
	{
		switch ( Type )
		{
			using namespace ECode;

			case Untyped:
			break;

			case Global_Body:
			break;

			case Function:
			break;

			case Function_Body:
			break;

			case Function_FwdDecl:
			break;

			case Namespace:
			break;

			case Namespace_Body:
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

	}

	string AST::to_string() const
	{
		string result = string_make( g_allocator, "" );

		if ( Comment )
			result = string_append_fmt( result, "// %s\n", Comment );

		switch ( Type )
		{
			using namespace ECode;

			case Invalid:
				fatal("Attempted to serialize invalid code! - %s", Name);
			break;

			case Untyped:
				result = string_append_length( result, Content, string_length(Content) );
			break;

			case Function_FwdDecl:
			{
				u32 index = 0;
				u32 left  = array_count( Entries );

				if ( left <= 0 )
					fatal( "Code::to_string - Name: %s Type: %s, expected definition", Name, Type );

				if ( Entries[index]->Type == Specifiers )
				{
					result = string_append_fmt( result, "%s\n", Entries[index]->to_string() );
					index++;
					left--;
				}

				if ( left <= 0 )
					fatal( "Code::to_string - Name: %s Type: %s, expected return type", Name, Type );

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

			case Function:
			{
				u32 index = 0;
				u32 left  = array_count( Entries );

				if ( left <= 0 )
					fatal( "Code::to_string - Name: %s Type: %s, expected definition", Name, Type );

				if ( Entries[index]->Type == Specifiers )
				{
					result = string_append_fmt( result, "%s", Entries[index]->to_string() );
					index++;
					left--;
				}

				if ( left <= 0 )
					fatal( "Code::to_string - Name: %s Type: %s, expected return type", Name, Type );

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

			case Function_Body:
				fatal("NOT SUPPORTED YET");
			break;

			case Namespace:
				fatal("NOT SUPPORTED YET");
			break;

			case Namespace_Body:
				fatal("NOT SUPPORTED YET");
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
				fatal("NOT SUPPORTED YET");
			break;

			case Struct_Body:
				fatal("NOT SUPPORTED YET");
			break;

			case Variable:
				fatal("NOT SUPPORTED YET");
			break;

			case Typedef:
				fatal("NOT SUPPORTED YET");
			break;

			case Typename:
				result = string_append_fmt( result, "%s", Name );
			break;

			case Using:
				fatal("NOT SUPPORTED YET");
			break;
		}

		return result;
	}

	const Code Code::Invalid;
#	pragma endregion AST

#pragma region Gen Interface
	void init()
	{
		array_init( StaticData::CodePool, StaticData::Allocator_CodePool );
		array_init( StaticData::StringArenas, heap() );

		arena string_arena;
		arena_init_from_allocator( & string_arena, StaticData::Allocator_StringArena, StaticData::InitSize_StringArena );

		str_tbl_init(  & StaticData::StringMap, StaticData::Allocator_StringTable );
		type_tbl_init( & StaticData::TypeMap,   StaticData::Allocator_TypeTable );

		Code&
		InvalidCode_write = ccast( Code, Code::Invalid );
		InvalidCode_write = make_code();

	#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
		Code&
		t_bool_write = ccast( Code, t_void );
		t_bool_write = def_type( txt(void) );

	#	define def_constant_code_type( Type_ ) \
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
	#	undef def_constant_code_type
	#endif

		Code&
		spec_constexpr_write = ccast( Code, spec_constexpr );
		spec_constexpr_write = def_specifiers( 1, ESpecifier::Constexpr );

	#	define def_constant_spec( Type_, ... )                              \
		Code&                                                               \
		spec_##Type_ = def_specifiers( macro_num_args(__VA_ARGS__), __VA_ARGS__); \
		spec_##Type_.lock();

		def_constant_spec( const, ESpecifier::Const );
		def_constant_spec( inline, ESpecifier::Inline );
	#	undef def_constant_spec
	}

	void clear_code_pool()
	{
		array_clear( StaticData::CodePool );

		sw size = array_capacity( StaticData::CodePool );

		zpl_memset( StaticData::CodePool, 0, size );
	}

	allocator get_string_allocator( s32 str_length )
	{
		using namespace StaticData;

		if ( StringArenas->total_allocated + str_length > StringArenas->total_size )
		{
			arena new_arena;
			arena_init_from_allocator( & new_arena, Allocator_StringArena, InitSize_StringArena );

			array_append( StringArenas, new_arena );

			return arena_allocator( StringArenas );
		}

		return arena_allocator( StringArenas );
	}

	// Will either make or retrive a code string.
	string code_string( char const* cstr, s32 length )
	{
		s32 hash_length = length > kilobytes(1) ? kilobytes(1) : length;

		u32 key = crc32( cstr, hash_length );

		string* result = str_tbl_get( & StaticData::StringMap, key );

		if ( result )
		{
			return * result;
		}

		str_tbl_set( & StaticData::StringMap, key, * result );

		return * result;
	}

	void set_init_reserve_code_pool( sw size )
	{
		StaticData::InitSize_CodePool = size;
	}

	void set_init_reserve_string_arena( sw size )
	{
		StaticData::InitSize_StringArena = size;
	}

	void set_init_reserve_string_table( sw size )
	{
		StaticData::InitSize_StringTable = size;
	}

	void set_init_reserve_type_table( sw size )
	{
		StaticData::InitSize_TypeTable = size;
	}

	void set_allocator_code_pool( allocator pool_allocator )
	{
		StaticData::Allocator_CodePool = pool_allocator;
	}

	void set_allocator_string_arena( allocator string_allocator )
	{
		StaticData::Allocator_StringArena = string_allocator;
	}

	void set_allocator_string_table( allocator string_allocator )
	{
		StaticData::Allocator_StringArena = string_allocator;
	}

	void set_allocator_type_table( allocator type_reg_allocator )
	{
		StaticData::Allocator_TypeTable = type_reg_allocator;
	}

#	pragma region Upfront Constructors
	Code def_class( s32 length, char const* name, Code parent, Code specifiers, Code body )
	{
		using namespace ECode;

		if ( name == nullptr )
		{
			log_failure( "gen::def_class: name is null");
			return Code::Invalid;
		}

		if ( parent && parent->Type != Class || parent->Type != Struct )
		{
			log_failure( "gen::def_class: parent provided is not type 'Class' or 'Struct' - Type: %s", parent->type_str() );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_class: specifiers was not a 'Specifiers' type - Type: %s", specifiers->type_str() );
			return Code::Invalid;
		}

		switch ( body->Type )
		{
			case Class_Body:
			case Untyped:
			break;

			default:
				log_failure("gen_def_class: body must be either of Function_Body or Untyped type.");
				return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Name = code_string( name, length );

		array_init( result->Entries, StaticData::Allocator_CodePool );

		if ( body )
		{
			result->Type = Class;
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

	Code def_enum( s32 length, char const* name, Code type, Code body )
	{
		if ( length <= 0 )
		{
			log_failure( "gen_def_enum: Invalid name length provided - %d", length );
		}

		if ( name == nullptr )
		{
			log_failure( "gen::def_class: name is null");
			return Code::Invalid;
		}


	}

	Code def_function( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
		, Code body
	)
	{
		using namespace ECode;

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

		switch ( body->Type )
		{
			case Function_Body:
			case Untyped:
				break;

			default:
			{
				log_failure("gen::def_function: body must be either of Function_Body or Untyped type.");
				return Code::Invalid;
			}
		}

		s32 name_length = zpl_strnlen( name, MaxNameLength );

		Code
		result       = make_code();
		result->Name = code_string( name, name_length );
		result->Type = Function;

		array_init( result->Entries, StaticData::Allocator_CodePool );

		result->add_entry( body );

		if ( specifiers )
			result->add_entry( specifiers );

		result->add_entry( ret_type );

		if ( params )
			result->add_entry( params );

		result.lock();
		return result;
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
				case Function_FwdDecl:
				case Namespace:
				case Namespace_Body:
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
				case Function_FwdDecl:
				case Namespace:
				case Namespace_Body:
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
		s32         name_length = zpl_strnlen(name, MaxNameLength);

		result->Name = code_string( name, name_length );

		array_init( result->Entries, g_allocator );

		if ( type->Type != Typename )
		{
			log_failure( "gen::def_parameters: type of param %d is not a Typename", num - num + 1 );
			return Code::Invalid;
		}

		result->add_entry( type );

		while( num -= 2, num && num % 2 == 0 )
		{
			type = va_arg(va, Code);

			name        = va_arg(va, char const*);
			name_length = zpl_strnlen(name, MaxNameLength);

			Code
			param       = make_code();
			param->Type = Parameters;
			param->Name = code_string(name, name_length);

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

	Code def_namespace( char const* name, Code body )
	{
		using namespace ECode;

		Code
		result       = make_code();
		result->Type = Namespace;

		array_init( result->Entries, g_allocator );

		if ( body->Type != Namespace_Body || body->Type != Untyped )
		{
			log_failure("gen::def_namespace: body is not of namespace or untyped type");
			return Code::Invalid;
		}

		result->add_entry( body );

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

	Code def_specifiers( s32 num, ... )
	{
		if ( num <= 0 )
			fatal("gen::make_specifier: num cannot be zero or less");

		Code
		result          = make_code();
		result->Type    = ECode::Specifiers;
		result->Content = string_make( g_allocator, "" );

		va_list va;
		va_start(va, num);
		do
		{
			SpecifierT type = (SpecifierT)va_arg(va, int);

			switch ( type )
			{
				case ESpecifier::Alignas:
					result->Content = string_append_fmt( result->Content, "%s(%d)", ESpecifier::to_str(type), va_arg(va, u32) );
				break;

				default:
					const char* str = ESpecifier::to_str(type);

					result->Content = string_append_fmt( result->Content, "%s", str );
				break;
			}
		}
		while ( --num, num );
		va_end(va);

		return result;
	}

	Code def_struct( char const* name, Code body, Code parent, Code specifiers )
	{
		using namespace ECode;

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
		result->Type = Struct;
		result->Name = string_make( g_allocator, name );

		array_init( result->Entries, g_allocator );

		if ( body )
			result->add_entry( body );

		if ( parent )
			result->add_entry( parent );

		if ( specifiers )
			result->add_entry( specifiers );

		return result;
	}

	Code def_struct_body( s32 num, ... )
	{
		using namespace ECode;

		if ( num == 0 )
		{
			log_failure("gen::def_struct_body: num cannot be zero");
			return Code::Invalid;
		}

		Code result = make_code();

		array_init( result->Entries, g_allocator );

		va_list va;
		va_start(va, num);
		do
		{
			Code entry = va_arg(va, Code);

			switch ( entry->Type )
			{
				case Namespace:
				case Namespace_Body:
				case Parameters:
				case Specifiers:
				case Struct_Body:
				case Typename:
				{
					log_failure("gen::def_struct_body: Entry type is not allowed: %s", ECode::str(entry->Type) );
					return Code::Invalid;
				}
			}

			result->add_entry( entry );
		}
		while ( num--, num > 0 );
		va_end(va);

		return result;
	}

	Code def_variable( Code type, char const* name, Code value, Code specifiers )
	{
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
		result->Name = string_make( g_allocator, name );
		result->Type = ECode::Variable;

		array_init( result->Entries, g_allocator );

		if ( specifiers )
			result->add_entry( specifiers );

		result->add_entry( type );

		if ( value )
			result->add_entry( value );

		return result;
	}

	Code def_type( char const* name,  Code specifiers )
	{
		Code
		result       = make_code();
		result->Name = string_make( g_allocator, name );
		result->Type = ECode::Typename;

		return result;
	}

	Code def_using( char const* name, Code type )
	{
		Code
		result       = make_code();
		result->Name = string_make( g_allocator, name );
		result->Type = ECode::Using;

		array_init( result->Entries, g_allocator );

		type->Parent = result;
		result->add_entry( type );

		return result;
	}
#	pragma endregion Upfront Constructors

#	pragma region Incremetnal Constructors
	Code make_function( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
	)
	{
		using namespace ECode;

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
		result->Name = string_make( g_allocator, name );
		result->Type = Function;

		array_init( result->Entries, g_allocator );

		// Making body at entry 0.
		result->add_entry( make_code() );

		if ( specifiers )
			result->add_entry( specifiers );

		if ( ret_type )
			result->add_entry( ret_type );

		if ( params )
			result->add_entry( params );

		return result;
	}

	Code make_struct( char const* name, Code parent, Code specifiers )
	{
		using namespace ECode;

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
		result       = make_code();
		result->Type = Struct;
		result->Name = string_make( g_allocator, name );

		array_init( result->Entries, g_allocator );

		// Making body at entry 0.
		result->add_entry( make_code() );

		if ( parent )
			result->add_entry( parent );

		if ( specifiers )
			result->add_entry( specifiers );

		return result;
	}

	Code make_global_body( char const* name = "", s32 num = 0, ... )
	{
		Code
		result = make_code();
		result->Type = ECode::Global_Body;
		result->Name = string_make( g_allocator, "");

		array_init( result->Entries, g_allocator );

		// Making body at entry 0;
		result->add_entry( make_code() );

		return result;
	}
#	pragma endregion Incremetnal Constructions

#	pragma region Parsing Constructors
	Code parse_proc( char const* def, s32 length )
	{
		if ( def == nullptr )
		{
			log_failure("gen::parse_proc: def is null!");
			return Code::Invalid;
		}

		arena mem;
		do_once_start
		{
			arena_init_from_allocator( & mem, heap(), kilobytes( 10 ) );
		}
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
		zpl_memset( Params, 0, sizeof( Params ));

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
		}

		Code specifiers = def_specifiers( num_specifiers, specs_found );

		Code params   = make_code();
		Code ret_type = def_type( ret_type_str );
		Code body     = untyped_str( body_length, body_str );

		Code
		result       = make_code();
		result->Name = string_make( g_allocator, name );
		result->Type = ECode::Function;

		array_init( result->Entries, g_allocator );

		result->add_entry( body );

		if ( specifiers )
			result->add_entry( specifiers );

		result->add_entry( ret_type );

		if ( params )
			result->add_entry( params );

		result.lock();
		return result;
	}

	Code parse_struct( char const* def, s32 length )
	{
		arena mem;
		do_once_start
			arena_init_from_allocator( & mem, heap(), kilobytes( 10 ) );
		do_once_end

		// Pretty sure its impossible to have more than this.
		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };

		u8 num_specifiers;

		// Making all significant tokens have a max length of 128 for this parser.
		ct sw LengthID = 128;

		char const name[LengthID] { 0 };
		char const parent[LengthID] { 0 };


	}
#	pragma endregion Parsing Constructors

#	pragma region Untyped Constructors
	Code untyped_str(char const* str)
	{
		Code
		result          = make_code();
		result->Name    = string_make( g_allocator, str );
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
		zpl_snprintf_va(buf, ZPL_PRINTF_MAXLEN, fmt, va);
		va_end(va);

		Code
		result          = make_code();
		result->Name    = string_make( g_allocator, fmt );
		result->Type    = ECode::Untyped;
		result->Content = string_make( g_allocator, buf );

		return result;
	}

	Code untyped_token_fmt( char const* fmt, s32 num_tokens, ... )
	{
		local_persist thread_local
		char buf[ZPL_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		token_fmt_va(buf, ZPL_PRINTF_MAXLEN, fmt, num_tokens, va);
		va_end(va);

		Code
		result           = make_code();
		result->Name     = string_make( g_allocator, fmt );
		result->Type     = ECode::Untyped;
		result->Content  = string_make( g_allocator, buf );

		result.lock();

		return result;
	}
#	pragma endregion Untyped Constructors
#pragma endregion Gen Interface

#	pragma region Builder
	void Builder::print( Code code )
	{
		Buffer = string_append_fmt( Buffer, "%s\n\n", code->to_string() );
	}

	bool Builder::open( char const* path )
	{
		file_error error = file_open_mode( & File, ZPL_FILE_MODE_WRITE, path );

		if ( error != ZPL_FILE_ERROR_NONE )
		{
			fatal( "gen::File::open - Could not open file: %s", path);
			return false;
		}

		Buffer = string_make( g_allocator, "" );

		return true;
	}

	void Builder::write()
	{
		bool result = file_write( & File, Buffer, string_length(Buffer) );

		if ( result == false )
			fatal("gen::File::write - Failed to write to file: %s", file_name( & File ) );

		// file_seek( & File, 0 );
		file_close( & File );
	}
#	pragma endregion Builder
}
#endif
