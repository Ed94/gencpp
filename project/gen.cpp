#include "Bloat.hpp"
#include "gen.hpp"

#ifdef gen_time
namespace gen
{
	namespace StaticData
	{
		static array(CodePOD) CodePool = nullptr;
	}

#pragma region CONSTANTS
#	ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
	const Code t_void;

	const Code t_bool;
	const Code t_char;
	const Code t_char_wide;

	const Code t_s8;
	const Code t_s16;
	const Code t_s32;
	const Code t_s64;

	const Code t_u8;
	const Code t_u16;
	const Code t_u32;
	const Code t_u64;

	const Code t_sw;
	const Code t_uw;

	const Code t_f32;
	const Code t_f64;

	const Code spec_constexpr;
	const Code spec_inline;
#	endif
#pragma endregion CONSTANTS


	/*
		Used internally to retireve a Code object form the CodePool.
	*/
	static Code make_code()
	{
		using namespace StaticData;

		ct CodePOD Invalid = { ECode::Invalid, false, nullptr, nullptr, nullptr, { nullptr } };

		array_append( CodePool, Invalid );

		return pcast( Code, array_back( CodePool ));
	}

	void init()
	{
		array_init( StaticData::CodePool, g_allocator );

		Code& 
		InvalidCode_write = ccast( Code, InvalidCode );
		InvalidCode_write = make_code();

	#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
		Code& 
		t_bool_write = ccast( Code, t_void );
		t_bool_write = def_type( txt(void) );

	#	define def_constant_code_type( Type_ )        \
		Code&                                         \
		t_##Type_##_write = ccast( Code, t_##Type_ ); \
		t_##Type_##_write = def_type( txt(Type_) )    \

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
		
		Code&
		spec_constexpr_write = ccast( Code, spec_constexpr );
		spec_constexpr_write = def_specifiers( 1, ESpecifier::Constexpr );

	#	define def_constant_spec( Type_, ... ) \
		Code& \
		spec_##Type_##_write = ccast( Code, spec_##Type_ );                        \
		spec_##Type_##_write = def_specifiers( VA_NARGS(__VA_ARGS__), __VA_ARGS__)

		def_constant_spec( inline, ESpecifier::Inline );

	#	undef def_constant_spec
		
	#endif
	}

	Code decl_type( Code type, char const* name, Code specifiers )
	{
		using namespace ECode;

		if ( type->Type != Specifiers )
		{
			log_failure( "gen::decl_type: type is not a Typename");
			return InvalidCode;
		}

		if ( type->Type != Typename )
		{
			log_failure( "gen::decl_type: specifiers is not a 'Specfiers' type");
			return InvalidCode;
		}

		Code 
		result       = make_code();
		result->Type = Decl_Type;
		result->Name = string_make( g_allocator, name );

		array_init( result->Entries, g_allocator );
		result->add( specifiers );
		result->add( type );
		result.lock();

		return result;
	}

	Code decl_proc( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
	)
	{
		using namespace ECode;

		if ( specifiers->Type != Specifiers )
		{
			log_failure( "gen::decl_fn: specifiers was not a `Specifiers` type" );
			return InvalidCode;
		}

		if ( params->Type != Parameters )
		{
			log_failure( "gen::decl_fn: params was not a `Parameters` type" );
			return InvalidCode;
		}

		if ( ret_type->Type != Typename )
		{
			log_failure( "gen::decl_fn: ret_type was not a Typename" );
			return InvalidCode;
		}

		Code
		result       = make_code();
		result->Type = Decl_Function;
		result->Name = string_make( g_allocator, name );
		
		array_init( result->Entries, g_allocator );

		if ( specifiers )
			result->add( specifiers );

		result->add( ret_type );

		if ( params )
			result->add( params );

		result.lock();
		return result;
	}

	Code def_params( s32 num, ... )
	{
		using namespace ECode;

		if (num <= 0)
		{
			log_failure( "TT::make_paramters: num cannot be zero or neg" );
			return InvalidCode;
		}

		Code
		result       = make_code();
		result->Type = Parameters;

		va_list va;
		va_start(va, num);

		result->Name = string_make( g_allocator, va_arg(va, char const*) );

		array_init( result->Entries, g_allocator );

		Code type = va_arg(va, Code);

		if ( type->Type != Typename )
		{
			log_failure( "gen::def_parameters: type of param %d is not a Typename", num - num + 1 );
			return InvalidCode;
		}

		result->add( type );

		while( num -= 2, num && num % 2 == 0 )
		{
			type = va_arg(va, Code);

			Code
			param       = make_code();
			param->Type = Parameters;
			param->Name = string_make( g_allocator, va_arg(va, char const*) );

			array_init( param->Entries, g_allocator );

			if ( type->Type != Typename )
			{
				log_failure( "gen::def_parameters: type of param %d is not a Typename", num - num + 1 );
				return InvalidCode;
			}

			param->add( type );
			param.lock();
			
			result->add(param);
		}
		va_end(va);

		result.lock();
		return result;
	}

	Code def_proc( char const* name
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
			return InvalidCode;
		}

		if ( params && params->Type != Parameters )
		{
			log_failure( "gen::def_function: params was not a `Parameters` type" );
			return InvalidCode;
		}

		if ( ret_type == nullptr || ret_type->Type != Typename )
		{
			log_failure( "gen::def_function: ret_type was not a Typename" );
			return InvalidCode;
		}

		switch ( body->Type )
		{
			case Function_Body:
			case Untyped:
				break;

			default:
			{
				log_failure("gen::def_function: body must be either of Function_Body or Untyped type.");
				return InvalidCode;
			}
		}

		Code 
		result       = make_code();
		result->Name = string_make( g_allocator, name );
		result->Type = Function;
		
		array_init( result->Entries, g_allocator );

		result->add( body );

		if ( specifiers )
			result->add( specifiers );

		result->add( ret_type );

		if ( params )
			result->add( params );

		result.lock();
		return result;
	}

	Code def_proc_body( s32 num, ... )
	{
		using namespace ECode;

		if ( num <= 0 )
		{
			log_failure("gen::def_function_body: num cannot zero or neg");
			return InvalidCode;
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
				return InvalidCode;
			}

			switch ( entry->Type )
			{
				case Decl_Function:
				case Decl_Type:
				case Namespace:
				case Namespace_Body:
				case Parameters:
				case Specifiers:
				case Struct_Body:
				case Typename:
				{
					log_failure("gen::def_function_body: Entry type is not allowed: %s", entry->type_str() );
					return InvalidCode;
				}

				default:
					break;
			}

			result->add( entry );
		} 
		while ( num--, num > 0 );
		va_end(va);
		
		return result;
	}

	Code def_proc_body( s32 num, Code* codes )
	{
		using namespace ECode;

		if ( num <= 0 )
		{
			log_failure("gen::def_function_body: num cannot zero or neg");
			return InvalidCode;
		}

		if ( codes == nullptr )
		{
			log_failure("gen::def_function_body: Provided a null array of codes!");
			return InvalidCode;
		}

		Code result = make_code();

		array_init( result->Entries, g_allocator );
		do
		{
			Code entry = *codes;

			if ( ! entry )
			{
				log_failure("gen::def_function_body: Provided an invalid entry!");
				return InvalidCode;
			}

			switch ( entry->Type )
			{
				case Decl_Function:
				case Decl_Type:
				case Namespace:
				case Namespace_Body:
				case Parameters:
				case Specifiers:
				case Struct_Body:
				case Typename:
				{
					log_failure("gen::def_function_body: Entry type is not allowed: %s", entry->type_str() );
					return InvalidCode;
				}

				default:
					break;
			}

			result->add( entry );
		} 
		while ( num--, num > 0 );
		
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
			return InvalidCode;
		}

		result->add( body );

		return result;
	}

	Code def_namespace_body( s32 num, ... )
	{
		using namespace ECode;

		if ( num <= 0 )
		{
			log_failure("gen::make_specifier: num cannot be zero or less");
			return InvalidCode;
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
					return InvalidCode;

				default:
					break;
			}

			result->add( entry );
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
			return InvalidCode;
		}

		if ( parent && parent->Type != Struct )
		{
			log_failure( "gen::def_struct: parent was not a `Struct` type" );
			return InvalidCode;
		}

		if ( body && body->Type != Struct_Body )
		{
			log_failure( "gen::def_struct: body was not a Struct_Body type" );
			return InvalidCode;
		}

		Code
		result       = make_code();
		result->Type = Struct;
		result->Name = string_make( g_allocator, name );

		array_init( result->Entries, g_allocator );

		if ( body )
			result->add( body );

		if ( parent )
			result->add( parent );

		if ( specifiers )
			result->add( specifiers );

		return result;
	}

	Code def_struct_body( s32 num, ... )
	{
		using namespace ECode;

		if ( num == 0 )
		{
			log_failure("gen::def_struct_body: num cannot be zero");
			return InvalidCode;
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
					return InvalidCode;
				}
			}

			result->add( entry );
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
			return InvalidCode;
		}

		if ( type->Type != ECode::Typename )
		{
			log_failure( "gen::def_variable: type was not a Typename" );
			return InvalidCode;
		}

		if ( value && value->Type != ECode::Untyped )
		{
			log_failure( "gen::def_variable: value was not a `Untyped` type" );
			return InvalidCode;
		}

		Code 
		result       = make_code();
		result->Name = string_make( g_allocator, name );
		result->Type = ECode::Variable;
		
		array_init( result->Entries, g_allocator );

		if ( specifiers )
			result->add( specifiers );

		result->add( type );

		if ( value )
			result->add( value );

		return result;
	}

	Code def_type( char const* name )
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
		result->add( type );

		return result;
	}

	Code untyped_str(char const* fmt)
	{
		Code 
		result       = make_code();
		result->Name = string_make( g_allocator, fmt );
		result->Type = ECode::Untyped;

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

	Code make_proc( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
	)
	{
		using namespace ECode;

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_function: specifiers was not a `Specifiers` type" );
			return InvalidCode;
		}

		if ( params && params->Type != Parameters )
		{
			log_failure( "gen::def_function: params was not a `Parameters` type" );
			return InvalidCode;
		}

		if ( ret_type == nullptr || ret_type->Type != Typename )
		{
			log_failure( "gen::def_function: ret_type was not a Typename" );
			return InvalidCode;
		}

		Code 
		result       = make_code();
		result->Name = string_make( g_allocator, name );
		result->Type = Function;
		
		array_init( result->Entries, g_allocator );

		// Making body at entry 0.
		result->add( make_code() );

		if ( specifiers )
			result->add( specifiers );

		if ( ret_type )
			result->add( ret_type );

		if ( params )
			result->add( params );

		return result;
	}

	Code make_struct( char const* name, Code parent = UnusedCode, Code specifiers = UnusedCode )
	{
		using namespace ECode;

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_struct: specifiers was not a `Specifiers` type" );
			return InvalidCode;
		}

		if ( parent && parent->Type != Struct )
		{
			log_failure( "gen::def_struct: parent was not a `Struct` type" );
			return InvalidCode;
		}

		Code
		result       = make_code();
		result->Type = Struct;
		result->Name = string_make( g_allocator, name );

		array_init( result->Entries, g_allocator );

		// Making body at entry 0.
		result->add( make_code() );

		if ( parent )
			result->add( parent );

		if ( specifiers )
			result->add( specifiers );

		return result;
	}

	Code make_unit( char const* name )
	{
		Code
		result = make_code();
		result->Type = ECode::Unit;
		result->Name = string_make( g_allocator, name );

		array_init( result->Entries, g_allocator );

		// Making body at entry 0;
		result->add( make_code() );

		return result;
	}

	Code parse_proc( char const* def, s32 length )
	{
		if ( def == nullptr )
		{
			log_failure("gen::parse_proc: def is null!");
			return InvalidCode;
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
				scanner++ ;
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
				return InvalidCode;
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
		Code body     = untyped_str( body_str, body_length );

		Code 
		result       = make_code();
		result->Name = string_make( g_allocator, name );
		result->Type = ECode::Function;
		
		array_init( result->Entries, g_allocator );

		result->add( body );

		if ( specifiers )
			result->add( specifiers );

		result->add( ret_type );

		if ( params )
			result->add( params );

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

			case Decl_Function:
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

			case Decl_Type:
				if ( Entries[0]->Type == Specifiers )
					result = string_append_fmt( result, "%s\n", Entries[0]->to_string());

				result = string_append_fmt( result, "%s %s;\n", Entries[1]->to_string(), Name );
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
}
#endif
