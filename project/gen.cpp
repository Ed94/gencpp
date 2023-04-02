#include "Bloat.hpp"
#include "gen.hpp"

#ifdef gen_time
namespace gen
{
	static Code Unused()
	{
		static Code value = {
			Code::Unused, 
			string_make( g_allocator, "Unused" )
		};

		return value;
	} 

	Code decl_type( char const* name, Code specifiers, Code type )
	{
		Code 
		result;
		result.Type = Code::Decl_Type;
		result.Name = string_make( g_allocator, name );
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
		result;
		result.Type = Code::Decl_Function;
		result.Name = string_make( g_allocator, name );
		
		array_init( result.Content, g_allocator );

		if ( specifiers )
			result.add( specifiers );

		result.add( ret_type );

		if ( params )
			result.add( params );

		return result;
	}

	Code make_parameters( s32 num, ... )
	{
		if (num <= 0)
			fatal("TT::make_paramters: num is %d", num);

		Code result;

		va_list va;
		va_start(va, num);

		result.Name = string_make( g_allocator, va_arg(va, char const*) );
		result.add( va_arg(va, Code) );

		while( num -= 2, num % 2 )
		{
			Code
			param;
			param.Name = string_make( g_allocator, va_arg(va, char const*) );
			param.add( va_arg(va, Code) );

			result.add(param);
		}
		
		va_end(va);

		return result;
	}

	Code make_fmt(char const* fmt, ...)
	{
		local_persist thread_local 
		char buf[ZPL_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		zpl_snprintf_va(buf, ZPL_PRINTF_MAXLEN, fmt, va);
		va_end(va);
         
		Code 
		code;
		code.Name    = string_make( g_allocator, fmt );
		code.Type    = Code::Untyped;
		code.Content = string_make( g_allocator, buf );

		return code;
	}

	Code make_function( char const* name
		, Code specifiers
		, Code params
		, Code ret_type
		, Code body )
	{
		Code 
		result;
		result.Name = string_make( g_allocator, name );
		result.Type = Code::Function;
		
		array_init( result.Content, g_allocator );

		if ( specifiers )
			result.add( specifiers );

		result.add( ret_type );

		if ( params )
			result.add( params );

		result.add( body );

		return result;
	}

	Code make_specifier( u32 num, ... )
	{
		if ( num <= 0 )
			fatal("gen::make_specifier: num cannot be zero.");

		Code 
		result;

		va_list va;
		va_start(va, num);
		do
		{
			Specifier type = va_arg(va, Specifier);

			switch ( type )
			{
				case Alignas:
					result.Content = string_sprintf_buf( g_allocator, "%s(%d)", specifier_str(type), va_arg(va, u32) );
				break;

				default:
					result.Content = string_make( g_allocator, specifier_str(type) );
				break;
			}
		}
		while ( num-- );
		va_end(va);

		return result;
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
					result = string_append_fmt("%s\n", Entries[0].to_string());

				result = string_append_fmt( result, "%s %s;\n", Entries[1].to_string(), Name );
			break;

			case Decl_Function:
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
					result = string_append_fmt( result, "%s, ", Entries[index].to_string() );
					index++;
					left--;
				}

				result = string_appendc( result, ");\n" );
			break;

			case Parameters:
				result = string_append_fmt( result, "%s %s", Entries[0], Name );

				u32 index = 1;
				u32 left  = array_count( Entries ) - 1;

				while ( left-- )
					result = string_append_fmt( result, ", %s %s", Entries[index].Entries[0], Entries[index].Name );
			break;

			case Struct:
				fatal("NOT SUPPORTED YET");
			break;

			case Function:
				u32 index = 0;
				u32 left  = array_count( Entries );

				if ( left <= 0 )
					fatal( "Code::to_string - Name: %s Type: %s, expected definition", Name, Type );

				while ( Entries[index].Type == EType::Specifiers )
				{
					result = string_append_fmt( result, "%s ", Entries[index] );
					index++;
				}

				if ( left <= 0 )
					fatal( "Code::to_string - Name: %s Type: %s, expected return type", Name, Type );

				result = string_append_fmt( result, "\n%s %s(", Entries[index], Name );
				index++;

				while ( left && Entries[index].Type == Parameters )
				{
					result = string_append_fmt( result, "%s, ", Entries[index] );
					index++;
				}

				result = string_append_fmt( result, ")\n{\n%s\n}", Entries[index] );
			break;

			case Specifiers:
				result = string_append_fmt( result, "%s", Content );
			break;

			case Variable:
				// result = string_append_fmt( result, "%s", )
			break;

			case Typename:
			break;
		}

		return result;
	}
}


int main()
{
	gen_main();

	return 0;
}
#endif gen_time
