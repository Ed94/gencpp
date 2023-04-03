#include "Bloat.hpp"
#include "gen.hpp"
#define gen_time

#ifdef gen_time
namespace gen
{
	namespace StaticData
	{
		static array(CodePOD) CodePool = nullptr;
	}

	/*
		Used internally to retireve a Code object form the CodePool.
	*/
	Code make()
	{
		using namespace StaticData;

		array_append( CodePool, InvalidCode );

		return * (Code*) & array_back( CodePool );
	}



	void init()
	{
		array_init( StaticData::CodePool, g_allocator );
	}

	Code decl_type( char const* name, Code type, Code specifiers )
	{
		using namespace ECode;

		if ( type->Type != Specifiers )
			fatal( "gen::decl_type: type is not a Typename");

		if ( type->Type != Typename )
			fatal( "gen::decl_type: specifiers is not a 'Specfiers' type");

		Code 
		result       = make();
		result->Type = Decl_Type;
		result->Name = string_make( g_allocator, name );

		array_init( result->Entries, g_allocator );
		result->add( specifiers );
		result->add( type );

		return result;
	}

	Code decl_fn( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
	)
	{
		using namespace ECode;

		if ( specifiers->Type != Specifiers )
			fatal( "gen::decl_fn: specifiers was not a `Specifiers` type" );

		if ( params->Type != Parameters )
			fatal( "gen::decl_fn: params was not a `Parameters` type" );

		if ( ret_type->Type != Typename )
			fatal( "gen::decl_fn: ret_type was not a Typename" );

		Code
		result       = make();
		result->Type = Decl_Function;
		result->Name = string_make( g_allocator, name );
		
		array_init( result->Entries, g_allocator );

		if ( specifiers )
			result->add( specifiers );

		result->add( ret_type );

		if ( params )
			result->add( params );

		return result;
	}

	Code def_parameters( s32 num, ... )
	{
		using namespace ECode;

		if (num <= 0)
			fatal( "TT::make_paramters: num cannot be zero or neg" );

		Code
		result       = make();
		result->Type = Parameters;

		va_list va;
		va_start(va, num);

		result->Name = string_make( g_allocator, va_arg(va, char const*) );

		array_init( result->Entries, g_allocator );

		Code type = va_arg(va, Code);

		if ( type->Type != Typename )
			fatal( "gen::def_parameters: type of param %d is not a Typename", num - num + 1 );

		result->add( type );

		while( num -= 2, num && num % 2 == 0 )
		{
			type = va_arg(va, Code);

			Code
			param       = make();
			param->Type = Parameters;
			param->Name = string_make( g_allocator, va_arg(va, char const*) );

			array_init( param->Entries, g_allocator );

			if ( type->Type != Typename )
				fatal( "gen::def_parameters: type of param %d is not a Typename", num - num + 1 );

			param->add( type );
			result->add(param);
		}
		va_end(va);

		return result;
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
			fatal( "gen::def_function: specifiers was not a `Specifiers` type" );

		if ( params && params->Type != Parameters )
			fatal( "gen::def_function: params was not a `Parameters` type" );

		if ( ret_type == nullptr || ret_type->Type != Typename )
			fatal( "gen::def_function: ret_type was not a Typename" );

		switch ( body->Type )
		{
			case Function_Body:
			case Untyped:
				break;

			default:
				fatal("gen::def_function: body must be either of Function_Body or Untyped type.");
		}

		Code 
		result       = make();
		result->Name = string_make( g_allocator, name );
		result->Type = Function;
		
		array_init( result->Entries, g_allocator );

		if ( specifiers )
			result->add( specifiers );

		result->add( ret_type );

		if ( params )
			result->add( params );

		result->add( body );

		body->Parent = result;

		return result;
	}

	Code def_function_body( s32 num, ... )
	{
		using namespace ECode;

		if ( num <= 0 )
			fatal("gen::def_function_body: num cannot zero or neg");

		Code result = make();

		array_init( result->Entries, g_allocator );

		va_list va;
		va_start(va, num);
		do
		{
			Code entry = va_arg(va, Code);

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
					fatal("gen::def_function_body: Entry type is not allowed: %s", entry->type_str() );

				default:
					break;
			}

			result->add( entry );
		} 
		while ( num--, num > 0 );
		va_end(va);
		
		return result;
	}

	Code def_namespace( char const* name, Code body )
	{
		using namespace ECode;

		Code 
		result       = make();
		result->Type = Namespace;

		array_init( result->Entries, g_allocator );

		if ( body->Type != Namespace_Body || body->Type != Untyped )
			fatal("gen::def_namespace: body is not of namespace or untyped type");

		result->add( body );

		return result;
	}

	Code def_namespace_body( s32 num, ... )
	{
		using namespace ECode;

		if ( num <= 0 )
			fatal("gen::make_specifier: num cannot be zero or less");

		Code
		result = make();
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
					fatal("gen::def_function_body: Entry type is not allowed: %s", ECode::str(entry->Type) );

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
		result          = make();
		result->Type    = ECode::Specifiers;
		result->Content = string_make( g_allocator, "" );

		va_list va;
		va_start(va, num);
		do
		{
			Specifier type = (Specifier)va_arg(va, int);

			switch ( type )
			{
				case Alignas:
					result->Content = string_append_fmt( result->Content, "%s(%d)", specifier_str(type), va_arg(va, u32) );
				break;

				default:
					const char* str = specifier_str(type);

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
			fatal( "gen::def_struct: specifiers was not a `Specifiers` type" );

		if ( parent && parent->Type != Struct )
			fatal( "gen::def_struct: parent was not a `Struct` type" );

		if ( body && body->Type != Struct_Body )
			fatal( "gen::def_struct: body was not a Struct_Body type" );

		Code
		result      = make();
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
			fatal("gen::def_struct_body: num cannot be zero");

		Code result = make();

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
					fatal("gen::def_struct_body: Entry type is not allowed: %s", ECode::str(entry->Type) );
			}

			result->add( entry );
		} 
		while ( num--, num > 0 );
		va_end(va);
		
		return result;
	}

	Code def_variable( char const* name, Code type, Code value, Code specifiers )
	{
		if ( specifiers && specifiers->Type != ECode::Specifiers )
			fatal( "gen::def_variable: specifiers was not a `Specifiers` type" );

		if ( type->Type != ECode::Typename )
			fatal( "gen::def_variable: type was not a Typename" );

		if ( value && value->Type != ECode::Untyped )
			fatal( "gen::def_variable: value was not a `Untyped` type" );

		Code 
		result       = make();
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
		result       = make();
		result->Name = string_make( g_allocator, name );
		result->Type = ECode::Typename;

		return result;
	}

	Code def_using( char const* name, Code type )
	{
		Code
		result       = make();
		result->Name = string_make( g_allocator, name );
		result->Type = ECode::Using;

		array_init( result->Entries, g_allocator );

		type->Parent = result;
		result->add( type );

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
		result          = make();
		result->Name    = string_make( g_allocator, fmt );
		result->Type    = ECode::Untyped;
		result->Content = string_make( g_allocator, buf );

		return result;
	}


	struct TokEntry
	{
		char const* Str;
		s32         Length;
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
					zpl_strnlen(value, 128) 
				};

				u32 key = crc32( token, zpl_strnlen(token, 32) );

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
				*buf = *fmt;
				buf++;
				fmt++;

				current = *fmt;
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

				s32      key   = crc32( token, tok_len );
				TokEntry value = *tokmap_get( & tok_map, key );
				s32      left  = value.Length;

				while ( left-- )
				{
					*buf = *value.Str;
					buf++;
					value.Str++;
				}

				scanner++;
				fmt     = scanner;				
				current = *fmt;
			}
		}

		return result;
	}
	
	Code token_fmt( char const* fmt, s32 num_tokens, ... )
	{
		local_persist thread_local 
		char buf[ZPL_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		token_fmt_va(buf, ZPL_PRINTF_MAXLEN, fmt, num_tokens, va);
		va_end(va);

		Code
		result         = make();
		result->Name    = string_make( g_allocator, fmt );
		result->Type    = ECode::Untyped;
		result->Content = string_make( g_allocator, buf );

		return result;
	}



	string AST::to_string()
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
