#include "Bloat.hpp"
#include "gen.hpp"
#define gen_time

#ifdef gen_time
namespace gen
{
	void init()
	{

	}

	ct Code make()
	{
		return { Code::Invalid, nullptr, nullptr, { nullptr } };
	}

	Code decl_type( char const* name, Code type, Code specifiers )
	{
		Code 
		result      = make();
		result.Type = Code::Decl_Type;
		result.Name = string_make( g_allocator, name );

		array_init( result.Entries, g_allocator );
		result.add( specifiers );
		result.add( type );

		return result;
	}

	Code decl_fn( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
	)
	{
		Code 
		result      = make();
		result.Type = Code::Decl_Function;
		result.Name = string_make( g_allocator, name );
		
		array_init( result.Entries, g_allocator );

		if ( specifiers )
			result.add( specifiers );

		result.add( ret_type );

		if ( params )
			result.add( params );

		return result;
	}

	Code def_parameters( s32 num, ... )
	{
		if (num <= 0)
			fatal("TT::make_paramters: num is %d", num);

		Code 
		result      = make();
		result.Type = Code::Parameters;

		va_list va;
		va_start(va, num);

		result.Name = string_make( g_allocator, va_arg(va, char const*) );

		array_init( result.Entries, g_allocator );

		Code type = va_arg(va, Code);
		result.add( type );

		while( num -= 2, num && num % 2 == 0 )
		{
			Code
			param      = make();
			param.Name = string_make( g_allocator, va_arg(va, char const*) );

			array_init( param.Entries, g_allocator );

			type = va_arg(va, Code);
			param.add( type );

			result.add(param);
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
		Code 
		result      = make();
		result.Name = string_make( g_allocator, name );
		result.Type = Code::Function;
		
		array_init( result.Entries, g_allocator );

		if ( specifiers )
			result.add( specifiers );

		result.add( ret_type );

		if ( params )
			result.add( params );

		result.add( body );

		return result;
	}

	Code def_function_body( u32 num, ... )
	{

	}

	Code def_namespace( char const* name, Code body )
	{

	}

	Code def_namespace_body( u32 num, ... )
	{

	}

	Code def_specifiers( u32 num, ... )
	{
		if ( num <= 0 )
			fatal("gen::make_specifier: num cannot be zero.");

		Code 
		result         = make();
		result.Type    = Code::Specifiers;
		result.Content = string_make( g_allocator, "" );

		va_list va;
		va_start(va, num);
		do
		{
			Specifier type = (Specifier)va_arg(va, int);

			switch ( type )
			{
				case Alignas:
					result.Content = string_append_fmt( result.Content, "%s(%d)", specifier_str(type), va_arg(va, u32) );
				break;

				default:
					const char* str = specifier_str(type);

					result.Content = string_append_fmt( result.Content, "%s", str );
				break;
			}
		}
		while ( --num, num );
		va_end(va);

		return result;
	}

	Code def_struct( char const* name, Code body, Code parent, Code specifiers )
	{

	}

	Code def_struct_body( u32 num, ... )
	{

	}

	Code def_variable( char const* name, Code type, Code value, Code specifiers )
	{
		
	}

	Code def_type( char const* name )
	{
		Code 
		result      = make();
		result.Name = string_make( g_allocator, name );
		result.Type = Code::Typename;

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
		result         = make();
		result.Name    = string_make( g_allocator, fmt );
		result.Type    = Code::Untyped;
		result.Content = string_make( g_allocator, buf );

		return result;
	}
	
	Code token_fmt( char const* fmt, ... )
	{

	}



	string Code::to_string()
	{
		string result = string_make( g_allocator, "" );

		if ( Comment )
			result = string_append_fmt( result, "// %s\n", Comment );

		switch ( Type )
		{
			case Invalid:
				fatal("Attempted to serialize invalid code! - %s", Name);
			break;

			case Untyped:
				result = string_append_length( result, Content, string_length(Content) );
			break;

			case Decl_Type:
				if ( Entries[0].Type == Specifiers )
					result = string_append_fmt( result, "%s\n", Entries[0].to_string());

				result = string_append_fmt( result, "%s %s;\n", Entries[1].to_string(), Name );
			break;

			case Decl_Function:
			{
				u32 index = 0;
				u32 left  = array_count( Entries );

				if ( left <= 0 )
					fatal( "Code::to_string - Name: %s Type: %s, expected definition", Name, Type );

				if ( Entries[index].Type == Specifiers )
				{
					result = string_append_fmt( result, "%s\n", Entries[index].to_string() );
					index++;
					left--;
				}

				if ( left <= 0 )
					fatal( "Code::to_string - Name: %s Type: %s, expected return type", Name, Type );

				result = string_append_fmt( result, "\n%s %s(", Entries[index].to_string(), Name );
				index++;
				left--;

				if ( left && Entries[index].Type == Parameters )
				{
					result = string_append_fmt( result, "%s", Entries[index].to_string() );
					index++;
					left--;
				}

				result = string_appendc( result, ");\n" );
			}
			break;

			case Function_Body:
			break;

			case Parameters: 
			{
				result = string_append_fmt( result, "%s %s", Entries[0].to_string(), Name );

				s32 index = 1;
				s32 left  = array_count( Entries ) - 1;

				while ( left--, left > 0 )
					result = string_append_fmt( result, ", %s %s"
						, Entries[index].Entries[0].to_string()
						, Entries[index].Name 
					);
			}
			break;

			case Function:
			{
				u32 index = 0;
				u32 left  = array_count( Entries );

				if ( left <= 0 )
					fatal( "Code::to_string - Name: %s Type: %s, expected definition", Name, Type );

				if ( Entries[index].Type == Specifiers )
				{
					result = string_append_fmt( result, "%s", Entries[index].to_string() );
					index++;
					left--;
				}

				if ( left <= 0 )
					fatal( "Code::to_string - Name: %s Type: %s, expected return type", Name, Type );

				result = string_append_fmt( result, "\n%s %s(", Entries[index].to_string(), Name );
				index++;
				left--;

				if ( left && Entries[index].Type == Parameters )
				{
					result = string_append_fmt( result, "%s", Entries[index].to_string() );
					index++;
					left--;
				}

				result = string_append_fmt( result, ")\n{\n%s\n}", Entries[index].to_string() );
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

			case Typename:
				result = string_append_fmt( result, "%s", Name );
			break;
		}

		return result;
	}



	void Builder::print( Code code )
	{
		Buffer = string_append_fmt( Buffer, "%s\n\n", code.to_string() );
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
