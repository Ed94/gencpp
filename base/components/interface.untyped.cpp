#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#include "interface.parsing.cpp"
#endif

ssize token_fmt_va( char* buf, usize buf_size, s32 num_tokens, va_list va )
{
	char const* buf_begin = buf;
	ssize       remaining = buf_size;

	local_persist
	TokenMap_FixedArena tok_map_arena;
	fixed_arena_init( & tok_map_arena);

	local_persist
	StringTable tok_map;
	{
		tok_map = hashtable_init(StrC, fixed_arena_allocator_info(& tok_map_arena) );

		s32 left = num_tokens - 1;

		while ( left-- )
		{
			char const* token = va_arg( va, char const* );
			StrC        value = va_arg( va, StrC );

			u32 key = crc32( token, str_len(token) );
			hashtable_set( tok_map, key, value );
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

			u32       key   = crc32( token, tok_len );
			StrC*     value = hashtable_get(tok_map, key );

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

	hashtable_clear(tok_map);
	fixed_arena_free(& tok_map_arena);

	ssize result = buf_size - remaining;

	return result;
}

Code untyped_str( StrC content )
{
	if ( content.Len == 0 )
	{
		log_failure( "untyped_str: empty string" );
		return InvalidCode;
	}

	Code
	result          = make_code();
	result->Name    = get_cached_string( content );
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
	ssize length = str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va);
	va_end(va);

	StrC buf_str      = { str_len_capped(fmt, MaxNameLength), fmt };
    StrC uncapped_str = { length, buf };

	Code
	result          = make_code();
	result->Name    = get_cached_string( buf_str );
	result->Type    = CT_Untyped;
	result->Content = get_cached_string( uncapped_str );

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

	StrC buf_str = { length, buf };

	Code
	result          = make_code();
	result->Name    = get_cached_string( buf_str );
	result->Type    = CT_Untyped;
	result->Content = result->Name;

	if ( result->Name.Len == 0 )
	{
		log_failure( "untyped_fmt: could not cache string" );
		return InvalidCode;
	}

	return result;
}
