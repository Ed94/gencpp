#ifdef GEN_INTELLISENSE_DIRECTIVES
#	include "builder.hpp"
#endif

Builder Builder::open( char const* path )
{
	Builder result;

	FileError error = file_open_mode( & result.File, EFileMode_WRITE, path );
	if ( error != EFileError_NONE )
	{
		log_failure( "gen::File::open - Could not open file: %s", path);
		return result;
	}

	result.Buffer = string_make_reserve( GlobalAllocator, Builder_StrBufferReserve );

	// log_fmt("$Builder - Opened file: %s\n", result.File.filename );
	return result;
}

void Builder::pad_lines( s32 num )
{
	append( Buffer,  "\n" );
}

void Builder::print( Code code )
{
	String   str = code->to_string();
	// const ssize len = str.length();
	// log_fmt( "%s - print: %.*s\n", File.filename, len > 80 ? 80 : len, str.Data );
	append( Buffer, str );
}

void Builder::print_fmt( char const* fmt, ... )
{
	ssize   res;
	char buf[ GEN_PRINTF_MAXLEN ] = { 0 };

	va_list va;
	va_start( va, fmt );
	res = str_fmt_va( buf, count_of( buf ) - 1, fmt, va ) - 1;
	va_end( va );

	// log_fmt( "$%s - print_fmt: %.*s\n", File.filename, res > 80 ? 80 : res, buf );
	append( Buffer, buf, res );
}

void Builder::write()
{
	b32 result = file_write( & File, Buffer, length(Buffer) );

	if ( result == false )
		log_failure("gen::File::write - Failed to write to file: %s\n", file_name( & File ) );

	log_fmt( "Generated: %s\n", File.filename );
	file_close( & File );
	free(Buffer);
}
