#pragma region String

String String::fmt( AllocatorInfo allocator, char* buf, sw buf_size, char const* fmt, ... )
{
	va_list va;
	va_start( va, fmt );
	str_fmt_va( buf, buf_size, fmt, va );
	va_end( va );

	return make( allocator, buf );
}

String String::fmt_buf( AllocatorInfo allocator, char const* fmt, ... )
{
	local_persist thread_local
	char buf[ GEN_PRINTF_MAXLEN ] = { 0 };

	va_list va;
	va_start( va, fmt );
	str_fmt_va( buf, GEN_PRINTF_MAXLEN, fmt, va );
	va_end( va );

	return make( allocator, buf );
}

bool String::append_fmt( char const* fmt, ... )
{
	sw   res;
	char buf[ GEN_PRINTF_MAXLEN ] = { 0 };

	va_list va;
	va_start( va, fmt );
	res = str_fmt_va( buf, count_of( buf ) - 1, fmt, va ) - 1;
	va_end( va );

	return append( buf, res );
}

#pragma endregion String
