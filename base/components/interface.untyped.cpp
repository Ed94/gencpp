#ifdef INTELLISENSE_DIRECTIVES
#pragma once
#include "interface.parsing.cpp"
#endif

ssize token_fmt_va( char* buf, usize buf_size, s32 num_tokens, va_list va )
{
	char const* buf_begin = buf;
	ssize       remaining = buf_size;

	if (_ctx->token_fmt_map.Hashes == nullptr) {
		_ctx->token_fmt_map = hashtable_init(Str, _ctx->Allocator_DyanmicContainers );
	}
	// Populate token pairs
	{
		s32 left = num_tokens - 1;

		while ( left-- )
		{
			char const* token = va_arg( va, char const* );
			Str         value = va_arg( va, Str );

			u32 key = crc32( token, c_str_len(token) );
			hashtable_set( _ctx->token_fmt_map, key, value );
		}
	}

	char const* fmt     = va_arg( va, char const* );
	char        current = *fmt;

	while ( current )
	{
		ssize len = 0;

		while ( current && current != '<' && remaining )
		{
			* buf = * fmt;
			buf++;
			fmt++;
			remaining--;

			current = * fmt;
		}

		if ( current == '<' )
		{
			char const* scanner = fmt + 1;

			s32 tok_len = 0;

			while ( *scanner != '>' )
			{
				tok_len++;
				scanner++;
			}

			char const* token = fmt + 1;

			u32      key   = crc32( token, tok_len );
			Str*     value = hashtable_get(_ctx->token_fmt_map, key );

			if ( value )
			{
				ssize          left = value->Len;
				char const* str  = value->Ptr;

				while ( left-- )
				{
					* buf = * str;
					buf++;
					str++;
					remaining--;
				}

				scanner++;
				fmt     = scanner;
				current = * fmt;
				continue;
			}

			* buf = * fmt;
			buf++;
			fmt++;
			remaining--;

			current = * fmt;
		}
	}
	hashtable_clear(_ctx->token_fmt_map);
	ssize result = buf_size - remaining;
	return result;
}

Code untyped_str( Str content )
{
	if ( content.Len == 0 )
	{
		log_failure( "untyped_str: empty string" );
		return InvalidCode;
	}

	Code
	result          = make_code();
	result->Name    = cache_str( content );
	result->Type    = CT_Untyped;
	result->Content = result->Name;

	if ( result->Name.Len == 0 )
	{
		log_failure( "untyped_str: could not cache string" );
		return InvalidCode;
	}

	return result;
}

Code untyped_fmt( char const* fmt, ...)
{
	if ( fmt == nullptr )
	{
		log_failure( "untyped_fmt: null format string" );
		return InvalidCode;
	}

	local_persist thread_local
	char buf[GEN_PRINTF_MAXLEN] = { 0 };

	va_list va;
	va_start(va, fmt);
	ssize length = c_str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va);
	va_end(va);
    Str content = { buf, length };

	Code
	result          = make_code();
	result->Type    = CT_Untyped;
	result->Content = cache_str( content );

	if ( result->Name.Len == 0 )
	{
		log_failure( "untyped_fmt: could not cache string" );
		return InvalidCode;
	}

	return result;
}

Code untyped_token_fmt( s32 num_tokens, char const* fmt, ... )
{
	if ( num_tokens == 0 )
	{
		log_failure( "untyped_token_fmt: zero tokens" );
		return InvalidCode;
	}

	local_persist thread_local
	char buf[GEN_PRINTF_MAXLEN] = { 0 };

	va_list va;
	va_start(va, fmt);
	ssize length = token_fmt_va(buf, GEN_PRINTF_MAXLEN, num_tokens, va);
	va_end(va);

	Str buf_str = { buf, length };

	Code
	result          = make_code();
	result->Type    = CT_Untyped;
	result->Content = cache_str( buf_str );

	if ( result->Name.Len == 0 )
	{
		log_failure( "untyped_fmt: could not cache string" );
		return InvalidCode;
	}

	return result;
}

Code untyped_toks( TokenSlice tokens )
{
	if ( tokens.Num == 0 ) {
		log_failure( "untyped_toks: empty token slice" );
		return InvalidCode;
	}
	Code
	result              = make_code();
	result->Type        = CT_Untyped;
	result->ContentToks = tokens;
	return result;
}
