#ifdef GEN_INTELLISENSE_DIRECTIVES
#	include "builder.hpp"
#endif

Builder builder_open( char const* path )
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

void builder_pad_lines( Builder* builder, s32 num )
{
	string_append_strc( & builder->Buffer, txt("\n") );
}

void builder_print( Builder* builder, Code code )
{
	String   str = code_to_string(code);
	// const ssize len = str.length();
	// log_fmt( "%s - print: %.*s\n", File.filename, len > 80 ? 80 : len, str.Data );
	string_append_string( & builder->Buffer, str );
}

void builder_print_fmt_va( Builder* builder, char const* fmt, va_list va )
{
	ssize   res;
	char buf[ GEN_PRINTF_MAXLEN ] = { 0 };

	res = str_fmt_va( buf, count_of( buf ) - 1, fmt, va ) - 1;

	string_append_c_str_len( (String*) & (builder->Buffer), (char const*)buf, res);
}

void builder_write(Builder* builder)
{
	b32 result = file_write( & builder->File, builder->Buffer, string_length(builder->Buffer) );

	if ( result == false )
		log_failure("gen::File::write - Failed to write to file: %s\n", file_name( & builder->File ) );

	log_fmt( "Generated: %s\n", builder->File.filename );
	file_close( & builder->File );
	string_free(& builder->Buffer);
}

