#define BLOAT_IMPL
#include "Bloat.hpp"


namespace Global
{
	bool ShouldShowDebug = false;
}

namespace Memory
{
	using namespace zpl;

	Arena Global_Arena {};

	void setup()
	{
		arena_init_from_allocator( & Global_Arena, heap(), Initial_Reserve );

		if ( Global_Arena.total_size == 0 )
		{
			assert_crash( "Failed to reserve memory for Tests:: Global_Arena" );
		}
	}

	void resize( uw new_size )
	{
		void* new_memory = resize( heap(), Global_Arena.physical_start, Global_Arena.total_size, new_size );

		if ( new_memory == nullptr )
		{
			fatal("Failed to resize global arena!");
		}

		Global_Arena.physical_start = new_memory;
		Global_Arena.total_size     = new_size;
	}

	void cleanup()
	{
		arena_free( & Global_Arena);
	}
}


struct TokEntry
{
	char const* Str;
	sw          Length;
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
				str_len(value, (sw)128)
			};

			u32 key = crc32( token, str_len(token, 32) );

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

			u32      key   = crc32( token, tok_len );
			TokEntry value = * tokmap_get( & tok_map, key );
			sw       left  = value.Length;

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

	tokmap_clear( & tok_map );

	return result;
}
