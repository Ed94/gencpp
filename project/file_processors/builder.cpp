void Builder::print( Code code )
{
	Buffer.append( code->to_string() );
}

void Builder::print_fmt( char const* fmt, ... )
{
	sw   res;
	char buf[ GEN_PRINTF_MAXLEN ] = { 0 };

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

	Buffer = String::make_reserve( GlobalAllocator, Builder_StrBufferReserve );

	return true;
}

void Builder::write()
{
	bool result = file_write( & File, Buffer, Buffer.length() );

	if ( result == false )
		log_failure("gen::File::write - Failed to write to file: %s", file_name( & File ) );

	file_close( & File );
	Buffer.free();
}
