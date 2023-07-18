#include "gen_dep.hpp"

#ifdef gen_time

// NOTE: Ensure we use standard methods for these calls if we use GEN_PICO
#pragma region Macros & Includes
#	include <stdio.h>
#	if ! defined( GEN_PICO_CUSTOM_ROUTINES )
#		if ! defined( GEN_MODULE_CORE )
#			define _strlen                   strlen
#			define _printf_err( fmt, ... )   fprintf( stderr, fmt, __VA_ARGS__ )
#			define _printf_err_va( fmt, va ) vfprintf( stderr, fmt, va )
#		else
#			define _strlen                   str_len
#			define _printf_err( fmt, ... )   str_fmt_out_err( fmt, __VA_ARGS__ )
#			define _printf_err_va( fmt, va ) str_fmt_out_err_va( fmt, va )
#		endif
#	endif
#
#	include <errno.h>
#
#	if defined( GEN_SYSTEM_UNIX ) || defined( GEN_SYSTEM_MACOS )
#		include <unistd.h>
#	elif defined( GEN_SYSTEM_WINDOWS )
#		if ! defined( GEN_NO_WINDOWS_H )
#			ifndef WIN32_LEAN_AND_MEAN
#				ifndef NOMINMAX
#					define NOMINMAX
#				endif
#
#				define WIN32_LEAN_AND_MEAN
#				define WIN32_MEAN_AND_LEAN
#				define VC_EXTRALEAN
#			endif
#			include <windows.h>
#			undef NOMINMAX
#			undef WIN32_LEAN_AND_MEAN
#			undef WIN32_MEAN_AND_LEAN
#			undef VC_EXTRALEAN
#		endif
#	endif
#pragma endregion Macros & Includes

#ifdef GEN_BENCHMARK
// Timing includes
#if defined( GEN_SYSTEM_MACOS ) || GEN_SYSTEM_UNIX
#	include <time.h>
#	include <sys/time.h>
#endif

#if defined( GEN_SYSTEM_MACOS )
#	include <mach/mach.h>
#	include <mach/mach_time.h>
#	include <mach/clock.h>
#endif

#if defined( GEN_SYSTEM_EMSCRIPTEN )
#	include <emscripten.h>
#endif

#if defined( GEN_SYSTEM_WINDOWS )
#	include <timezoneapi.h>
#endif
#endif

namespace gen
{
#pragma region Debug
	void assert_handler( char const* condition, char const* file, s32 line, char const* msg, ... )
	{
		_printf_err( "%s:(%d): Assert Failure: ", file, line );

		if ( condition )
			_printf_err( "`%s` ", condition );

		if ( msg )
		{
			va_list va;
			va_start( va, msg );
			_printf_err_va( msg, va );
			va_end( va );
		}

		_printf_err( "%s", "\n" );
	}

	s32 assert_crash( char const* condition )
	{
		GEN_PANIC( condition );
		return 0;
	}

	#if defined( GEN_SYSTEM_WINDOWS )
	void process_exit( u32 code )
	{
		ExitProcess( code );
	}
	#else
	#	include <stdlib.h>

	void process_exit( u32 code )
	{
		exit( code );
	}
	#endif
#pragma endregion Debug

#pragma region String Ops
	internal sw _scan_zpl_i64( const char* text, s32 base, s64* value )
	{
		const char* text_begin = text;
		s64         result     = 0;
		b32         negative   = false;

		if ( *text == '-' )
		{
			negative = true;
			text++;
		}

		if ( base == 16 && str_compare( text, "0x", 2 ) == 0 )
			text += 2;

		for ( ;; )
		{
			s64 v;
			if ( char_is_digit( *text ) )
				v = *text - '0';
			else if ( base == 16 && char_is_hex_digit( *text ) )
				v = hex_digit_to_int( *text );
			else
				break;

			result *= base;
			result += v;
			text++;
		}

		if ( value )
		{
			if ( negative )
				result = -result;
			*value = result;
		}

		return ( text - text_begin );
	}

	// TODO : Are these good enough for characters?
	global const char _num_to_char_table[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"@$";

	s64 str_to_i64( const char* str, char** end_ptr, s32 base )
	{
		sw  len;
		s64 value;

		if ( ! base )
		{
			if ( ( str_len( str ) > 2 ) && ( str_compare( str, "0x", 2 ) == 0 ) )
				base = 16;
			else
				base = 10;
		}

		len = _scan_zpl_i64( str, base, &value );
		if ( end_ptr )
			*end_ptr = ( char* )str + len;
		return value;
	}

	void i64_to_str( s64 value, char* string, s32 base )
	{
		char* buf      = string;
		b32   negative = false;
		u64   v;

		if ( value < 0 )
		{
			negative = true;
			value    = -value;
		}

		v = zpl_cast( u64 ) value;
		if ( v != 0 )
		{
			while ( v > 0 )
			{
				*buf++  = _num_to_char_table[ v % base ];
				v      /= base;
			}
		}
		else
		{
			*buf++ = '0';
		}
		if ( negative )
			*buf++ = '-';
		*buf = '\0';
		str_reverse( string );
	}

	void u64_to_str( u64 value, char* string, s32 base )
	{
		char* buf = string;

		if ( value )
		{
			while ( value > 0 )
			{
				*buf++  = _num_to_char_table[ value % base ];
				value  /= base;
			}
		}
		else
		{
			*buf++ = '0';
		}
		*buf = '\0';

		str_reverse( string );
	}
#pragma endregion String Ops

#pragma region Printing
	enum
	{
		GEN_FMT_MINUS = bit( 0 ),
		GEN_FMT_PLUS  = bit( 1 ),
		GEN_FMT_ALT   = bit( 2 ),
		GEN_FMT_SPACE = bit( 3 ),
		GEN_FMT_ZERO  = bit( 4 ),

		GEN_FMT_CHAR   = bit( 5 ),
		GEN_FMT_SHORT  = bit( 6 ),
		GEN_FMT_INT    = bit( 7 ),
		GEN_FMT_LONG   = bit( 8 ),
		GEN_FMT_LLONG  = bit( 9 ),
		GEN_FMT_SIZE   = bit( 10 ),
		GEN_FMT_INTPTR = bit( 11 ),

		GEN_FMT_UNSIGNED = bit( 12 ),
		GEN_FMT_LOWER    = bit( 13 ),
		GEN_FMT_UPPER    = bit( 14 ),
		GEN_FMT_WIDTH    = bit( 15 ),

		GEN_FMT_DONE = bit( 30 ),

		GEN_FMT_INTS = GEN_FMT_CHAR | GEN_FMT_SHORT | GEN_FMT_INT | GEN_FMT_LONG | GEN_FMT_LLONG | GEN_FMT_SIZE | GEN_FMT_INTPTR
	};

	struct _format_info
	{
		s32 base;
		s32 flags;
		s32 width;
		s32 precision;
	};

	internal sw _print_string( char* text, sw max_len, _format_info* info, char const* str )
	{
		sw    res = 0, len = 0;
		sw    remaining = max_len;
		char* begin     = text;

		if ( str == NULL && max_len >= 6 )
		{
			res += str_copy_nulpad( text, "(null)", 6 );
			return res;
		}

		if ( info && info->precision >= 0 )
			len = str_len( str, info->precision );
		else
			len = str_len( str );

		if ( info && ( info->width == 0 && info->flags & GEN_FMT_WIDTH ) )
		{
			return res;
		}

		if ( info && ( info->width == 0 || info->flags & GEN_FMT_MINUS ) )
		{
			if ( info->precision > 0 )
				len = info->precision < len ? info->precision : len;
			if ( res + len > max_len )
				return res;
			res  += str_copy_nulpad( text, str, len );
			text += res;

			if ( info->width > res )
			{
				sw padding = info->width - len;

				char pad = ( info->flags & GEN_FMT_ZERO ) ? '0' : ' ';
				while ( padding-- > 0 && remaining-- > 0 )
					*text++ = pad, res++;
			}
		}
		else
		{
			if ( info && ( info->width > res ) )
			{
				sw   padding = info->width - len;
				char pad     = ( info->flags & GEN_FMT_ZERO ) ? '0' : ' ';
				while ( padding-- > 0 && remaining-- > 0 )
					*text++ = pad, res++;
			}

			if ( res + len > max_len )
				return res;
			res += str_copy_nulpad( text, str, len );
		}

		if ( info )
		{
			if ( info->flags & GEN_FMT_UPPER )
				str_to_upper( begin );
			else if ( info->flags & GEN_FMT_LOWER )
				str_to_lower( begin );
		}

		return res;
	}

	internal sw _print_char( char* text, sw max_len, _format_info* info, char arg )
	{
		char str[ 2 ] = "";
		str[ 0 ]      = arg;
		return _print_string( text, max_len, info, str );
	}

	internal sw _print_repeated_char( char* text, sw max_len, _format_info* info, char arg )
	{
		sw  res = 0;
		s32 rem = ( info ) ? ( info->width > 0 ) ? info->width : 1 : 1;
		res     = rem;
		while ( rem-- > 0 )
			*text++ = arg;

		return res;
	}

	internal sw _print_i64( char* text, sw max_len, _format_info* info, s64 value )
	{
		char num[ 130 ];
		i64_to_str( value, num, info ? info->base : 10 );
		return _print_string( text, max_len, info, num );
	}

	internal sw _print_u64( char* text, sw max_len, _format_info* info, u64 value )
	{
		char num[ 130 ];
		u64_to_str( value, num, info ? info->base : 10 );
		return _print_string( text, max_len, info, num );
	}

	internal sw _print_f64( char* text, sw max_len, _format_info* info, b32 is_hexadecimal, f64 arg )
	{
		// TODO: Handle exponent notation
		sw    width, len, remaining = max_len;
		char* text_begin = text;

		if ( arg )
		{
			u64 value;
			if ( arg < 0 )
			{
				if ( remaining > 1 )
					*text = '-', remaining--;
				text++;
				arg = -arg;
			}
			else if ( info->flags & GEN_FMT_MINUS )
			{
				if ( remaining > 1 )
					*text = '+', remaining--;
				text++;
			}

			value  = zpl_cast( u64 ) arg;
			len    = _print_u64( text, remaining, NULL, value );
			text  += len;

			if ( len >= remaining )
				remaining = min( remaining, 1 );
			else
				remaining -= len;
			arg -= value;

			if ( info->precision < 0 )
				info->precision = 6;

			if ( ( info->flags & GEN_FMT_ALT ) || info->precision > 0 )
			{
				s64 mult = 10;
				if ( remaining > 1 )
					*text = '.', remaining--;
				text++;
				while ( info->precision-- > 0 )
				{
					value  = zpl_cast( u64 )( arg * mult );
					len    = _print_u64( text, remaining, NULL, value );
					text  += len;
					if ( len >= remaining )
						remaining = min( remaining, 1 );
					else
						remaining -= len;
					arg  -= zpl_cast( f64 ) value / mult;
					mult *= 10;
				}
			}
		}
		else
		{
			if ( remaining > 1 )
				*text = '0', remaining--;
			text++;
			if ( info->flags & GEN_FMT_ALT )
			{
				if ( remaining > 1 )
					*text = '.', remaining--;
				text++;
			}
		}

		width = info->width - ( text - text_begin );
		if ( width > 0 )
		{
			char  fill = ( info->flags & GEN_FMT_ZERO ) ? '0' : ' ';
			char* end  = text + remaining - 1;
			len        = ( text - text_begin );

			for ( len = ( text - text_begin ); len--; )
			{
				if ( ( text_begin + len + width ) < end )
					*( text_begin + len + width ) = *( text_begin + len );
			}

			len   = width;
			text += len;
			if ( len >= remaining )
				remaining = min( remaining, 1 );
			else
				remaining -= len;

			while ( len-- )
			{
				if ( text_begin + len < end )
					text_begin[ len ] = fill;
			}
		}

		return ( text - text_begin );
	}

	neverinline sw str_fmt_va( char* text, sw max_len, char const* fmt, va_list va )
	{
		char const* text_begin = text;
		sw          remaining  = max_len, res;

		while ( *fmt )
		{
			_format_info info = { 0 };
			sw           len  = 0;
			info.precision    = -1;

			while ( *fmt && *fmt != '%' && remaining )
				*text++ = *fmt++;

			if ( *fmt == '%' )
			{
				do
				{
					switch ( *++fmt )
					{
						case '-' :
							{
								info.flags |= GEN_FMT_MINUS;
								break;
							}
						case '+' :
							{
								info.flags |= GEN_FMT_PLUS;
								break;
							}
						case '#' :
							{
								info.flags |= GEN_FMT_ALT;
								break;
							}
						case ' ' :
							{
								info.flags |= GEN_FMT_SPACE;
								break;
							}
						case '0' :
							{
								info.flags |= ( GEN_FMT_ZERO | GEN_FMT_WIDTH );
								break;
							}
						default :
							{
								info.flags |= GEN_FMT_DONE;
								break;
							}
					}
				} while ( ! ( info.flags & GEN_FMT_DONE ) );
			}

			// NOTE: Optional Width
			if ( *fmt == '*' )
			{
				int width = va_arg( va, int );
				if ( width < 0 )
				{
					info.flags |= GEN_FMT_MINUS;
					info.width  = -width;
				}
				else
				{
					info.width = width;
				}
				info.flags |= GEN_FMT_WIDTH;
				fmt++;
			}
			else
			{
				info.width = zpl_cast( s32 ) str_to_i64( fmt, zpl_cast( char** ) & fmt, 10 );
				if ( info.width != 0 )
				{
					info.flags |= GEN_FMT_WIDTH;
				}
			}

			// NOTE: Optional Precision
			if ( *fmt == '.' )
			{
				fmt++;
				if ( *fmt == '*' )
				{
					info.precision = va_arg( va, int );
					fmt++;
				}
				else
				{
					info.precision = zpl_cast( s32 ) str_to_i64( fmt, zpl_cast( char** ) & fmt, 10 );
				}
				info.flags &= ~GEN_FMT_ZERO;
			}

			switch ( *fmt++ )
			{
				case 'h' :
					if ( *fmt == 'h' )
					{    // hh => char
						info.flags |= GEN_FMT_CHAR;
						fmt++;
					}
					else
					{    // h => short
						info.flags |= GEN_FMT_SHORT;
					}
					break;

				case 'l' :
					if ( *fmt == 'l' )
					{    // ll => long long
						info.flags |= GEN_FMT_LLONG;
						fmt++;
					}
					else
					{    // l => long
						info.flags |= GEN_FMT_LONG;
					}
					break;

					break;

				case 'z' :    // NOTE: zpl_usize
					info.flags |= GEN_FMT_UNSIGNED;
					// fallthrough
				case 't' :    // NOTE: zpl_isize
					info.flags |= GEN_FMT_SIZE;
					break;

				default :
					fmt--;
					break;
			}

			switch ( *fmt )
			{
				case 'u' :
					info.flags |= GEN_FMT_UNSIGNED;
					// fallthrough
				case 'd' :
				case 'i' :
					info.base = 10;
					break;

				case 'o' :
					info.base = 8;
					break;

				case 'x' :
					info.base   = 16;
					info.flags |= ( GEN_FMT_UNSIGNED | GEN_FMT_LOWER );
					break;

				case 'X' :
					info.base   = 16;
					info.flags |= ( GEN_FMT_UNSIGNED | GEN_FMT_UPPER );
					break;

				case 'f' :
				case 'F' :
				case 'g' :
				case 'G' :
					len = _print_f64( text, remaining, &info, 0, va_arg( va, f64 ) );
					break;

				case 'a' :
				case 'A' :
					len = _print_f64( text, remaining, &info, 1, va_arg( va, f64 ) );
					break;

				case 'c' :
					len = _print_char( text, remaining, &info, zpl_cast( char ) va_arg( va, int ) );
					break;

				case 's' :
					len = _print_string( text, remaining, &info, va_arg( va, char* ) );
					break;

				case 'r' :
					len = _print_repeated_char( text, remaining, &info, va_arg( va, int ) );
					break;

				case 'p' :
					info.base   = 16;
					info.flags |= ( GEN_FMT_LOWER | GEN_FMT_UNSIGNED | GEN_FMT_ALT | GEN_FMT_INTPTR );
					break;

				case '%' :
					len = _print_char( text, remaining, &info, '%' );
					break;

				default :
					fmt--;
					break;
			}

			fmt++;

			if ( info.base != 0 )
			{
				if ( info.flags & GEN_FMT_UNSIGNED )
				{
					u64 value = 0;
					switch ( info.flags & GEN_FMT_INTS )
					{
						case GEN_FMT_CHAR :
							value = zpl_cast( u64 ) zpl_cast( u8 ) va_arg( va, int );
							break;
						case GEN_FMT_SHORT :
							value = zpl_cast( u64 ) zpl_cast( u16 ) va_arg( va, int );
							break;
						case GEN_FMT_LONG :
							value = zpl_cast( u64 ) va_arg( va, unsigned long );
							break;
						case GEN_FMT_LLONG :
							value = zpl_cast( u64 ) va_arg( va, unsigned long long );
							break;
						case GEN_FMT_SIZE :
							value = zpl_cast( u64 ) va_arg( va, uw );
							break;
						case GEN_FMT_INTPTR :
							value = zpl_cast( u64 ) va_arg( va, uptr );
							break;
						default :
							value = zpl_cast( u64 ) va_arg( va, unsigned int );
							break;
					}

					len = _print_u64( text, remaining, &info, value );
				}
				else
				{
					s64 value = 0;
					switch ( info.flags & GEN_FMT_INTS )
					{
						case GEN_FMT_CHAR :
							value = zpl_cast( s64 ) zpl_cast( s8 ) va_arg( va, int );
							break;
						case GEN_FMT_SHORT :
							value = zpl_cast( s64 ) zpl_cast( s16 ) va_arg( va, int );
							break;
						case GEN_FMT_LONG :
							value = zpl_cast( s64 ) va_arg( va, long );
							break;
						case GEN_FMT_LLONG :
							value = zpl_cast( s64 ) va_arg( va, long long );
							break;
						case GEN_FMT_SIZE :
							value = zpl_cast( s64 ) va_arg( va, uw );
							break;
						case GEN_FMT_INTPTR :
							value = zpl_cast( s64 ) va_arg( va, uptr );
							break;
						default :
							value = zpl_cast( s64 ) va_arg( va, int );
							break;
					}

					len = _print_i64( text, remaining, &info, value );
				}
			}

			text += len;
			if ( len >= remaining )
				remaining = min( remaining, 1 );
			else
				remaining -= len;
		}

		*text++ = '\0';
		res     = ( text - text_begin );
		return ( res >= max_len || res < 0 ) ? -1 : res;
	}

	char* str_fmt_buf_va( char const* fmt, va_list va )
	{
		local_persist thread_local char buffer[ GEN_PRINTF_MAXLEN ];
		str_fmt_va( buffer, size_of( buffer ), fmt, va );
		return buffer;
	}

	char* str_fmt_buf( char const* fmt, ... )
	{
		va_list va;
		char*   str;
		va_start( va, fmt );
		str = str_fmt_buf_va( fmt, va );
		va_end( va );
		return str;
	}

	sw str_fmt_file_va( struct FileInfo* f, char const* fmt, va_list va )
	{
		local_persist thread_local char buf[ GEN_PRINTF_MAXLEN ];
		sw                              len = str_fmt_va( buf, size_of( buf ), fmt, va );
		b32                             res = file_write( f, buf, len - 1 );    // NOTE: prevent extra whitespace
		return res ? len : -1;
	}

	sw str_fmt_out_va( char const* fmt, va_list va )
	{
		return str_fmt_file_va( file_get_standard( EFileStandard_OUTPUT ), fmt, va );
	}

	sw str_fmt_out_err_va( char const* fmt, va_list va )
	{
		return str_fmt_file_va( file_get_standard( EFileStandard_ERROR ), fmt, va );
	}

	sw str_fmt_out_err( char const* fmt, ... )
	{
		sw      res;
		va_list va;
		va_start( va, fmt );
		res = str_fmt_out_err_va( fmt, va );
		va_end( va );
		return res;
	}
#pragma endregion Printing

#pragma region Memory
	void* mem_copy( void* dest, void const* source, sw n )
	{
		if ( dest == NULL )
		{
			return NULL;
		}

		return memcpy( dest, source, n );
	}

	void const* mem_find( void const* data, u8 c, sw n )
	{
		u8 const* s = zpl_cast( u8 const* ) data;
		while ( ( zpl_cast( uptr ) s & ( sizeof( uw ) - 1 ) ) && n && *s != c )
		{
			s++;
			n--;
		}
		if ( n && *s != c )
		{
			sw const* w;
			sw        k = GEN__ONES * c;
			w           = zpl_cast( sw const* ) s;
			while ( n >= size_of( sw ) && ! GEN__HAS_ZERO( *w ^ k ) )
			{
				w++;
				n -= size_of( sw );
			}
			s = zpl_cast( u8 const* ) w;
			while ( n && *s != c )
			{
				s++;
				n--;
			}
		}

		return n ? zpl_cast( void const* ) s : NULL;
	}

	#define GEN_HEAP_STATS_MAGIC 0xDEADC0DE

	struct _heap_stats
	{
		u32 magic;
		sw  used_memory;
		sw  alloc_count;
	};

	global _heap_stats _heap_stats_info;

	void heap_stats_init( void )
	{
		zero_item( &_heap_stats_info );
		_heap_stats_info.magic = GEN_HEAP_STATS_MAGIC;
	}

	sw heap_stats_used_memory( void )
	{
		GEN_ASSERT_MSG( _heap_stats_info.magic == GEN_HEAP_STATS_MAGIC, "heap_stats is not initialised yet, call heap_stats_init first!" );
		return _heap_stats_info.used_memory;
	}

	sw heap_stats_alloc_count( void )
	{
		GEN_ASSERT_MSG( _heap_stats_info.magic == GEN_HEAP_STATS_MAGIC, "heap_stats is not initialised yet, call heap_stats_init first!" );
		return _heap_stats_info.alloc_count;
	}

	void heap_stats_check( void )
	{
		GEN_ASSERT_MSG( _heap_stats_info.magic == GEN_HEAP_STATS_MAGIC, "heap_stats is not initialised yet, call heap_stats_init first!" );
		GEN_ASSERT( _heap_stats_info.used_memory == 0 );
		GEN_ASSERT( _heap_stats_info.alloc_count == 0 );
	}

	struct _heap_alloc_info
	{
		sw    size;
		void* physical_start;
	};

	void* heap_allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
	{
		void* ptr = NULL;
		// unused( allocator_data );
		// unused( old_size );
		if ( ! alignment )
			alignment = GEN_DEFAULT_MEMORY_ALIGNMENT;

	#ifdef GEN_HEAP_ANALYSIS
		sw alloc_info_size      = size_of( _heap_alloc_info );
		sw alloc_info_remainder = ( alloc_info_size % alignment );
		sw track_size           = max( alloc_info_size, alignment ) + alloc_info_remainder;
		switch ( type )
		{
			case EAllocation_FREE :
				{
					if ( ! old_memory )
						break;
					_heap_alloc_info* alloc_info  = zpl_cast( _heap_alloc_info* ) old_memory - 1;
					_heap_stats_info.used_memory -= alloc_info->size;
					_heap_stats_info.alloc_count--;
					old_memory = alloc_info->physical_start;
				}
				break;
			case EAllocation_ALLOC :
				{
					size += track_size;
				}
				break;
			default :
				break;
		}
	#endif

		switch ( type )
		{
	#if defined( GEN_COMPILER_MSVC ) || ( defined( GEN_COMPILER_GCC ) && defined( GEN_SYSTEM_WINDOWS ) ) || ( defined( GEN_COMPILER_TINYC ) && defined( GEN_SYSTEM_WINDOWS ) )
			case EAllocation_ALLOC :
				ptr = _aligned_malloc( size, alignment );
				if ( flags & ALLOCATOR_FLAG_CLEAR_TO_ZERO )
					zero_size( ptr, size );
				break;
			case EAllocation_FREE :
				_aligned_free( old_memory );
				break;
			case EAllocation_RESIZE :
				{
					AllocatorInfo a = heap();
					ptr             = default_resize_align( a, old_memory, old_size, size, alignment );
				}
				break;

	#elif defined( GEN_SYSTEM_LINUX ) && ! defined( GEN_CPU_ARM ) && ! defined( GEN_COMPILER_TINYC )
			case EAllocation_ALLOC :
				{
					ptr = aligned_alloc( alignment, ( size + alignment - 1 ) & ~( alignment - 1 ) );

					if ( flags & GEN_ALLOCATOR_FLAG_CLEAR_TO_ZERO )
					{
						zero_size( ptr, size );
					}
				}
				break;

			case EAllocation_FREE :
				{
					free( old_memory );
				}
				break;

			case EAllocation_RESIZE :
				{
					AllocatorInfo a = heap();
					ptr             = default_resize_align( a, old_memory, old_size, size, alignment );
				}
				break;
	#else
			case EAllocation_ALLOC :
				{
					posix_memalign( &ptr, alignment, size );

					if ( flags & GEN_ALLOCATOR_FLAG_CLEAR_TO_ZERO )
					{
						zero_size( ptr, size );
					}
				}
				break;

			case EAllocation_FREE :
				{
					free( old_memory );
				}
				break;

			case EAllocation_RESIZE :
				{
					AllocatorInfo a = heap();
					ptr             = default_resize_align( a, old_memory, old_size, size, alignment );
				}
				break;
	#endif

			case EAllocation_FREE_ALL :
				break;
		}

	#ifdef GEN_HEAP_ANALYSIS
		if ( type == EAllocation_ALLOC )
		{
			_heap_alloc_info* alloc_info = zpl_cast( _heap_alloc_info* )( zpl_cast( char* ) ptr + alloc_info_remainder );
			zero_item( alloc_info );
			alloc_info->size              = size - track_size;
			alloc_info->physical_start    = ptr;
			ptr                           = zpl_cast( void* )( alloc_info + 1 );
			_heap_stats_info.used_memory += alloc_info->size;
			_heap_stats_info.alloc_count++;
		}
	#endif

		return ptr;
	}

	void* Arena::allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
	{
		Arena* arena = rcast(Arena*, allocator_data);
		void*      ptr   = NULL;

		// unused( old_size );

		switch ( type )
		{
			case EAllocation_ALLOC :
				{
					void* end        = pointer_add( arena->PhysicalStart, arena->TotalUsed );
					sw    total_size = align_forward_i64( size, alignment );

					// NOTE: Out of memory
					if ( arena->TotalUsed + total_size > (sw) arena->TotalSize )
					{
						// zpl__printf_err("%s", "Arena out of memory\n");
						fatal("Arena out of memory! (Possibly could not fit for the largest size Arena!!)");
						return nullptr;
					}

					ptr              = align_forward( end, alignment );
					arena->TotalUsed += total_size;

					if ( flags & ALLOCATOR_FLAG_CLEAR_TO_ZERO )
						zero_size( ptr, size );
				}
				break;

			case EAllocation_FREE :
				// NOTE: Free all at once
				// Use Temp_Arena_Memory if you want to free a block
				break;

			case EAllocation_FREE_ALL :
				arena->TotalUsed = 0;
				break;

			case EAllocation_RESIZE :
				{
					// TODO : Check if ptr is on top of stack and just extend
					AllocatorInfo a = arena->Backing;
					ptr             = default_resize_align( a, old_memory, old_size, size, alignment );
				}
				break;
		}
		return ptr;
	}

	void* Pool::allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
	{
		Pool* pool = zpl_cast( Pool* ) allocator_data;
		void* ptr  = NULL;

		// unused( old_size );

		switch ( type )
		{
			case EAllocation_ALLOC :
				{
					uptr next_free;

					GEN_ASSERT( size == pool->BlockSize );
					GEN_ASSERT( alignment == pool->BlockAlign );
					GEN_ASSERT( pool->FreeList != NULL );

					next_free        = *zpl_cast( uptr* ) pool->FreeList;
					ptr              = pool->FreeList;
					pool->FreeList   = zpl_cast( void* ) next_free;
					pool->TotalSize += pool->BlockSize;

					if ( flags & ALLOCATOR_FLAG_CLEAR_TO_ZERO )
						zero_size( ptr, size );
				}
				break;

			case EAllocation_FREE :
				{
					uptr* next;
					if ( old_memory == NULL )
						return NULL;

					next             = zpl_cast( uptr* ) old_memory;
					*next            = zpl_cast( uptr ) pool->FreeList;
					pool->FreeList   = old_memory;
					pool->TotalSize -= pool->BlockSize;
				}
				break;

			case EAllocation_FREE_ALL :
				{
					sw    actual_block_size, block_index;
					void* curr;
					uptr* end;

					actual_block_size = pool->BlockSize + pool->BlockAlign;
					pool->TotalSize   = 0;

					// NOTE: Init intrusive freelist
					curr = pool->PhysicalStart;
					for ( block_index = 0; block_index < pool->NumBlocks - 1; block_index++ )
					{
						uptr* next = zpl_cast( uptr* ) curr;
						*next      = zpl_cast( uptr ) curr + actual_block_size;
						curr       = pointer_add( curr, actual_block_size );
					}

					end            = zpl_cast( uptr* ) curr;
					*end           = zpl_cast( uptr ) NULL;
					pool->FreeList = pool->PhysicalStart;
				}
				break;

			case EAllocation_RESIZE :
				// NOTE: Cannot resize
				GEN_PANIC( "You cannot resize something allocated by with a pool." );
				break;
		}

		return ptr;
	}

	Pool Pool::init_align( AllocatorInfo backing, sw num_blocks, sw block_size, sw block_align )
	{
		Pool pool = {};

		sw    actual_block_size, pool_size, block_index;
		void *data, *curr;
		uptr* end;

		zero_item( &pool );

		pool.Backing     = backing;
		pool.BlockSize   = block_size;
		pool.BlockAlign  = block_align;
		pool.NumBlocks   = num_blocks;

		actual_block_size = block_size + block_align;
		pool_size         = num_blocks * actual_block_size;

		data = alloc_align( backing, pool_size, block_align );

		// NOTE: Init intrusive freelist
		curr = data;
		for ( block_index = 0; block_index < num_blocks - 1; block_index++ )
		{
			uptr* next = ( uptr* ) curr;
			*next      = ( uptr  ) curr + actual_block_size;
			curr       = pointer_add( curr, actual_block_size );
		}

		end  =  ( uptr* ) curr;
		*end =  ( uptr )  NULL;

		pool.PhysicalStart = data;
		pool.FreeList      = data;

		return pool;
	}

	void Pool::clear()
	{
		sw    actual_block_size, block_index;
		void* curr;
		uptr* end;

		actual_block_size = BlockSize + BlockAlign;

		curr = PhysicalStart;
		for ( block_index = 0; block_index < NumBlocks - 1; block_index++ )
		{
			uptr* next = ( uptr* ) curr;
			*next      = ( uptr  ) curr + actual_block_size;
			curr       = pointer_add( curr, actual_block_size );
		}

		end  =  ( uptr* ) curr;
		*end =  ( uptr )  NULL;

		FreeList = PhysicalStart;
	}
#pragma endregion Memory

#pragma region ADT

#pragma endregion ADT

#pragma region CSV

#pragma endregion CSV

#pragma region Hashing
	global u32 const _crc32_table[ 256 ] = {
		0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd,
		0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
		0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
		0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
		0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
		0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
		0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce,
		0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
		0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
		0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
		0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0,
		0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
		0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703,
		0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
		0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
		0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
		0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5,
		0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
		0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
	};

	u32 crc32( void const* data, sw len )
	{
		sw        remaining;
		u32       result = ~( zpl_cast( u32 ) 0 );
		u8 const* c      = zpl_cast( u8 const* ) data;
		for ( remaining = len; remaining--; c++ )
			result = ( result >> 8 ) ^ ( _crc32_table[ ( result ^ *c ) & 0xff ] );
		return ~result;
	}

	global u64 const _crc64_table[ 256 ] = {
	0x0000000000000000ull, 0x7ad870c830358979ull, 0xf5b0e190606b12f2ull, 0x8f689158505e9b8bull, 0xc038e5739841b68full, 0xbae095bba8743ff6ull, 0x358804e3f82aa47dull,
	0x4f50742bc81f2d04ull, 0xab28ecb46814fe75ull, 0xd1f09c7c5821770cull, 0x5e980d24087fec87ull, 0x24407dec384a65feull, 0x6b1009c7f05548faull, 0x11c8790fc060c183ull,
	0x9ea0e857903e5a08ull, 0xe478989fa00bd371ull, 0x7d08ff3b88be6f81ull, 0x07d08ff3b88be6f8ull, 0x88b81eabe8d57d73ull, 0xf2606e63d8e0f40aull, 0xbd301a4810ffd90eull,
	0xc7e86a8020ca5077ull, 0x4880fbd87094cbfcull, 0x32588b1040a14285ull, 0xd620138fe0aa91f4ull, 0xacf86347d09f188dull, 0x2390f21f80c18306ull, 0x594882d7b0f40a7full,
	0x1618f6fc78eb277bull, 0x6cc0863448deae02ull, 0xe3a8176c18803589ull, 0x997067a428b5bcf0ull, 0xfa11fe77117cdf02ull, 0x80c98ebf2149567bull, 0x0fa11fe77117cdf0ull,
	0x75796f2f41224489ull, 0x3a291b04893d698dull, 0x40f16bccb908e0f4ull, 0xcf99fa94e9567b7full, 0xb5418a5cd963f206ull, 0x513912c379682177ull, 0x2be1620b495da80eull,
	0xa489f35319033385ull, 0xde51839b2936bafcull, 0x9101f7b0e12997f8ull, 0xebd98778d11c1e81ull, 0x64b116208142850aull, 0x1e6966e8b1770c73ull, 0x8719014c99c2b083ull,
	0xfdc17184a9f739faull, 0x72a9e0dcf9a9a271ull, 0x08719014c99c2b08ull, 0x4721e43f0183060cull, 0x3df994f731b68f75ull, 0xb29105af61e814feull, 0xc849756751dd9d87ull,
	0x2c31edf8f1d64ef6ull, 0x56e99d30c1e3c78full, 0xd9810c6891bd5c04ull, 0xa3597ca0a188d57dull, 0xec09088b6997f879ull, 0x96d1784359a27100ull, 0x19b9e91b09fcea8bull,
	0x636199d339c963f2ull, 0xdf7adabd7a6e2d6full, 0xa5a2aa754a5ba416ull, 0x2aca3b2d1a053f9dull, 0x50124be52a30b6e4ull, 0x1f423fcee22f9be0ull, 0x659a4f06d21a1299ull,
	0xeaf2de5e82448912ull, 0x902aae96b271006bull, 0x74523609127ad31aull, 0x0e8a46c1224f5a63ull, 0x81e2d7997211c1e8ull, 0xfb3aa75142244891ull, 0xb46ad37a8a3b6595ull,
	0xceb2a3b2ba0eececull, 0x41da32eaea507767ull, 0x3b024222da65fe1eull, 0xa2722586f2d042eeull, 0xd8aa554ec2e5cb97ull, 0x57c2c41692bb501cull, 0x2d1ab4dea28ed965ull,
	0x624ac0f56a91f461ull, 0x1892b03d5aa47d18ull, 0x97fa21650afae693ull, 0xed2251ad3acf6feaull, 0x095ac9329ac4bc9bull, 0x7382b9faaaf135e2ull, 0xfcea28a2faafae69ull,
	0x8632586aca9a2710ull, 0xc9622c4102850a14ull, 0xb3ba5c8932b0836dull, 0x3cd2cdd162ee18e6ull, 0x460abd1952db919full, 0x256b24ca6b12f26dull, 0x5fb354025b277b14ull,
	0xd0dbc55a0b79e09full, 0xaa03b5923b4c69e6ull, 0xe553c1b9f35344e2ull, 0x9f8bb171c366cd9bull, 0x10e3202993385610ull, 0x6a3b50e1a30ddf69ull, 0x8e43c87e03060c18ull,
	0xf49bb8b633338561ull, 0x7bf329ee636d1eeaull, 0x012b592653589793ull, 0x4e7b2d0d9b47ba97ull, 0x34a35dc5ab7233eeull, 0xbbcbcc9dfb2ca865ull, 0xc113bc55cb19211cull,
	0x5863dbf1e3ac9decull, 0x22bbab39d3991495ull, 0xadd33a6183c78f1eull, 0xd70b4aa9b3f20667ull, 0x985b3e827bed2b63ull, 0xe2834e4a4bd8a21aull, 0x6debdf121b863991ull,
	0x1733afda2bb3b0e8ull, 0xf34b37458bb86399ull, 0x8993478dbb8deae0ull, 0x06fbd6d5ebd3716bull, 0x7c23a61ddbe6f812ull, 0x3373d23613f9d516ull, 0x49aba2fe23cc5c6full,
	0xc6c333a67392c7e4ull, 0xbc1b436e43a74e9dull, 0x95ac9329ac4bc9b5ull, 0xef74e3e19c7e40ccull, 0x601c72b9cc20db47ull, 0x1ac40271fc15523eull, 0x5594765a340a7f3aull,
	0x2f4c0692043ff643ull, 0xa02497ca54616dc8ull, 0xdafce7026454e4b1ull, 0x3e847f9dc45f37c0ull, 0x445c0f55f46abeb9ull, 0xcb349e0da4342532ull, 0xb1eceec59401ac4bull,
	0xfebc9aee5c1e814full, 0x8464ea266c2b0836ull, 0x0b0c7b7e3c7593bdull, 0x71d40bb60c401ac4ull, 0xe8a46c1224f5a634ull, 0x927c1cda14c02f4dull, 0x1d148d82449eb4c6ull,
	0x67ccfd4a74ab3dbfull, 0x289c8961bcb410bbull, 0x5244f9a98c8199c2ull, 0xdd2c68f1dcdf0249ull, 0xa7f41839ecea8b30ull, 0x438c80a64ce15841ull, 0x3954f06e7cd4d138ull,
	0xb63c61362c8a4ab3ull, 0xcce411fe1cbfc3caull, 0x83b465d5d4a0eeceull, 0xf96c151de49567b7ull, 0x76048445b4cbfc3cull, 0x0cdcf48d84fe7545ull, 0x6fbd6d5ebd3716b7ull,
	0x15651d968d029fceull, 0x9a0d8ccedd5c0445ull, 0xe0d5fc06ed698d3cull, 0xaf85882d2576a038ull, 0xd55df8e515432941ull, 0x5a3569bd451db2caull, 0x20ed197575283bb3ull,
	0xc49581ead523e8c2ull, 0xbe4df122e51661bbull, 0x3125607ab548fa30ull, 0x4bfd10b2857d7349ull, 0x04ad64994d625e4dull, 0x7e7514517d57d734ull, 0xf11d85092d094cbfull,
	0x8bc5f5c11d3cc5c6ull, 0x12b5926535897936ull, 0x686de2ad05bcf04full, 0xe70573f555e26bc4ull, 0x9ddd033d65d7e2bdull, 0xd28d7716adc8cfb9ull, 0xa85507de9dfd46c0ull,
	0x273d9686cda3dd4bull, 0x5de5e64efd965432ull, 0xb99d7ed15d9d8743ull, 0xc3450e196da80e3aull, 0x4c2d9f413df695b1ull, 0x36f5ef890dc31cc8ull, 0x79a59ba2c5dc31ccull,
	0x037deb6af5e9b8b5ull, 0x8c157a32a5b7233eull, 0xf6cd0afa9582aa47ull, 0x4ad64994d625e4daull, 0x300e395ce6106da3ull, 0xbf66a804b64ef628ull, 0xc5bed8cc867b7f51ull,
	0x8aeeace74e645255ull, 0xf036dc2f7e51db2cull, 0x7f5e4d772e0f40a7ull, 0x05863dbf1e3ac9deull, 0xe1fea520be311aafull, 0x9b26d5e88e0493d6ull, 0x144e44b0de5a085dull,
	0x6e963478ee6f8124ull, 0x21c640532670ac20ull, 0x5b1e309b16452559ull, 0xd476a1c3461bbed2ull, 0xaeaed10b762e37abull, 0x37deb6af5e9b8b5bull, 0x4d06c6676eae0222ull,
	0xc26e573f3ef099a9ull, 0xb8b627f70ec510d0ull, 0xf7e653dcc6da3dd4ull, 0x8d3e2314f6efb4adull, 0x0256b24ca6b12f26ull, 0x788ec2849684a65full, 0x9cf65a1b368f752eull,
	0xe62e2ad306bafc57ull, 0x6946bb8b56e467dcull, 0x139ecb4366d1eea5ull, 0x5ccebf68aecec3a1ull, 0x2616cfa09efb4ad8ull, 0xa97e5ef8cea5d153ull, 0xd3a62e30fe90582aull,
	0xb0c7b7e3c7593bd8ull, 0xca1fc72bf76cb2a1ull, 0x45775673a732292aull, 0x3faf26bb9707a053ull, 0x70ff52905f188d57ull, 0x0a2722586f2d042eull, 0x854fb3003f739fa5ull,
	0xff97c3c80f4616dcull, 0x1bef5b57af4dc5adull, 0x61372b9f9f784cd4ull, 0xee5fbac7cf26d75full, 0x9487ca0fff135e26ull, 0xdbd7be24370c7322ull, 0xa10fceec0739fa5bull,
	0x2e675fb4576761d0ull, 0x54bf2f7c6752e8a9ull, 0xcdcf48d84fe75459ull, 0xb71738107fd2dd20ull, 0x387fa9482f8c46abull, 0x42a7d9801fb9cfd2ull, 0x0df7adabd7a6e2d6ull,
	0x772fdd63e7936bafull, 0xf8474c3bb7cdf024ull, 0x829f3cf387f8795dull, 0x66e7a46c27f3aa2cull, 0x1c3fd4a417c62355ull, 0x935745fc4798b8deull, 0xe98f353477ad31a7ull,
	0xa6df411fbfb21ca3ull, 0xdc0731d78f8795daull, 0x536fa08fdfd90e51ull, 0x29b7d047efec8728ull,
};

	u64 crc64( void const* data, sw len )
	{
		sw        remaining;
		u64       result = ( zpl_cast( u64 ) 0 );
		u8 const* c      = zpl_cast( u8 const* ) data;
		for ( remaining = len; remaining--; c++ )
			result = ( result >> 8 ) ^ ( _crc64_table[ ( result ^ *c ) & 0xff ] );
		return result;
	}
#pragma endregion Hashing

#pragma region File Handling
	#if defined( GEN_SYSTEM_WINDOWS ) || defined( GEN_SYSTEM_CYGWIN )

	internal wchar_t* _alloc_utf8_to_ucs2( AllocatorInfo a, char const* text, sw* w_len_ )
	{
		wchar_t* w_text = NULL;
		sw       len = 0, w_len = 0, w_len1 = 0;
		if ( text == NULL )
		{
			if ( w_len_ )
				*w_len_ = w_len;
			return NULL;
		}
		len = str_len( text );
		if ( len == 0 )
		{
			if ( w_len_ )
				*w_len_ = w_len;
			return NULL;
		}
		w_len = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, text, zpl_cast( int ) len, NULL, 0 );
		if ( w_len == 0 )
		{
			if ( w_len_ )
				*w_len_ = w_len;
			return NULL;
		}
		w_text = alloc_array( a, wchar_t, w_len + 1 );
		w_len1 = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, text, zpl_cast( int ) len, w_text, zpl_cast( int ) w_len );
		if ( w_len1 == 0 )
		{
			free( a, w_text );
			if ( w_len_ )
				*w_len_ = 0;
			return NULL;
		}
		w_text[ w_len ] = 0;
		if ( w_len_ )
			*w_len_ = w_len;
		return w_text;
	}

	internal GEN_FILE_SEEK_PROC( _win32_file_seek )
	{
		LARGE_INTEGER li_offset;
		li_offset.QuadPart = offset;
		if ( ! SetFilePointerEx( fd.p, li_offset, &li_offset, whence ) )
		{
			return false;
		}

		if ( new_offset )
			*new_offset = li_offset.QuadPart;
		return true;
	}

	internal GEN_FILE_READ_AT_PROC( _win32_file_read )
	{
		// unused( stop_at_newline );
		b32 result = false;
		_win32_file_seek( fd, offset, ESeekWhence_BEGIN, NULL );
		DWORD size_ = zpl_cast( DWORD )( size > GEN_I32_MAX ? GEN_I32_MAX : size );
		DWORD bytes_read_;
		if ( ReadFile( fd.p, buffer, size_, &bytes_read_, NULL ) )
		{
			if ( bytes_read )
				*bytes_read = bytes_read_;
			result = true;
		}

		return result;
	}

	internal GEN_FILE_WRITE_AT_PROC( _win32_file_write )
	{
		DWORD size_ = zpl_cast( DWORD )( size > GEN_I32_MAX ? GEN_I32_MAX : size );
		DWORD bytes_written_;
		_win32_file_seek( fd, offset, ESeekWhence_BEGIN, NULL );
		if ( WriteFile( fd.p, buffer, size_, &bytes_written_, NULL ) )
		{
			if ( bytes_written )
				*bytes_written = bytes_written_;
			return true;
		}
		return false;
	}

	internal GEN_FILE_CLOSE_PROC( _win32_file_close )
	{
		CloseHandle( fd.p );
	}

	FileOperations const default_file_operations = { _win32_file_read, _win32_file_write, _win32_file_seek, _win32_file_close };

	neverinline GEN_FILE_OPEN_PROC( _win32_file_open )
	{
		DWORD    desired_access;
		DWORD    creation_disposition;
		void*    handle;
		wchar_t* w_text;

		switch ( mode & GEN_FILE_MODES )
		{
			case EFileMode_READ :
				desired_access       = GENERIC_READ;
				creation_disposition = OPEN_EXISTING;
				break;
			case EFileMode_WRITE :
				desired_access       = GENERIC_WRITE;
				creation_disposition = CREATE_ALWAYS;
				break;
			case EFileMode_APPEND :
				desired_access       = GENERIC_WRITE;
				creation_disposition = OPEN_ALWAYS;
				break;
			case EFileMode_READ | EFileMode_RW :
				desired_access       = GENERIC_READ | GENERIC_WRITE;
				creation_disposition = OPEN_EXISTING;
				break;
			case EFileMode_WRITE | EFileMode_RW :
				desired_access       = GENERIC_READ | GENERIC_WRITE;
				creation_disposition = CREATE_ALWAYS;
				break;
			case EFileMode_APPEND | EFileMode_RW :
				desired_access       = GENERIC_READ | GENERIC_WRITE;
				creation_disposition = OPEN_ALWAYS;
				break;
			default :
				GEN_PANIC( "Invalid file mode" );
				return EFileError_INVALID;
		}

		w_text = _alloc_utf8_to_ucs2( heap(), filename, NULL );
		handle = CreateFileW( w_text, desired_access, FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, creation_disposition, FILE_ATTRIBUTE_NORMAL, NULL );

		free( heap(), w_text );

		if ( handle == INVALID_HANDLE_VALUE )
		{
			DWORD err = GetLastError();
			switch ( err )
			{
				case ERROR_FILE_NOT_FOUND :
					return EFileError_NOT_EXISTS;
				case ERROR_FILE_EXISTS :
					return EFileError_EXISTS;
				case ERROR_ALREADY_EXISTS :
					return EFileError_EXISTS;
				case ERROR_ACCESS_DENIED :
					return EFileError_PERMISSION;
			}
			return EFileError_INVALID;
		}

		if ( mode & EFileMode_APPEND )
		{
			LARGE_INTEGER offset = { { 0 } };
			if ( ! SetFilePointerEx( handle, offset, NULL, ESeekWhence_END ) )
			{
				CloseHandle( handle );
				return EFileError_INVALID;
			}
		}

		fd->p = handle;
		*ops  = default_file_operations;
		return EFileError_NONE;
	}

	#else    // POSIX
	#	include <fcntl.h>

	internal GEN_FILE_SEEK_PROC( _posix_file_seek )
	{
	#	if defined( GEN_SYSTEM_OSX )
		s64 res = lseek( fd.i, offset, whence );
	#	else    // TODO(ZaKlaus): @fixme lseek64
		s64 res = lseek( fd.i, offset, whence );
	#	endif
		if ( res < 0 )
			return false;
		if ( new_offset )
			*new_offset = res;
		return true;
	}

	internal GEN_FILE_READ_AT_PROC( _posix_file_read )
	{
		unused( stop_at_newline );
		sw res = pread( fd.i, buffer, size, offset );
		if ( res < 0 )
			return false;
		if ( bytes_read )
			*bytes_read = res;
		return true;
	}

	internal GEN_FILE_WRITE_AT_PROC( _posix_file_write )
	{
		sw  res;
		s64 curr_offset = 0;
		_posix_file_seek( fd, 0, ESeekWhence_CURRENT, &curr_offset );
		if ( curr_offset == offset )
		{
			// NOTE: Writing to stdout et al. doesn't like pwrite for numerous reasons
			res = write( zpl_cast( int ) fd.i, buffer, size );
		}
		else
		{
			res = pwrite( zpl_cast( int ) fd.i, buffer, size, offset );
		}
		if ( res < 0 )
			return false;
		if ( bytes_written )
			*bytes_written = res;
		return true;
	}

	internal GEN_FILE_CLOSE_PROC( _posix_file_close )
	{
		close( fd.i );
	}

	FileOperations const default_file_operations = { _posix_file_read, _posix_file_write, _posix_file_seek, _posix_file_close };

	GEN_NEVER_INLINE GEN_FILE_OPEN_PROC( _posix_file_open )
	{
		s32 os_mode;
		switch ( mode & GEN_FILE_MODES )
		{
			case EFileMode_READ :
				os_mode = O_RDONLY;
				break;
			case EFileMode_WRITE :
				os_mode = O_WRONLY | O_CREAT | O_TRUNC;
				break;
			case EFileMode_APPEND :
				os_mode = O_WRONLY | O_APPEND | O_CREAT;
				break;
			case EFileMode_READ | EFileMode_RW :
				os_mode = O_RDWR;
				break;
			case EFileMode_WRITE | EFileMode_RW :
				os_mode = O_RDWR | O_CREAT | O_TRUNC;
				break;
			case EFileMode_APPEND | EFileMode_RW :
				os_mode = O_RDWR | O_APPEND | O_CREAT;
				break;
			default :
				GEN_PANIC( "Invalid file mode" );
				return EFileError_INVALID;
		}

		fd->i = open( filename, os_mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
		if ( fd->i < 0 )
		{
			// TODO : More file errors
			return EFileError_INVALID;
		}

		*ops = default_file_operations;
		return EFileError_NONE;
	}

	// POSIX
	#endif

	internal void _dirinfo_free_entry( DirEntry* entry );

	// TODO : Is this a bad idea?
	global b32      _std_file_set                     = false;
	global FileInfo _std_files[ EFileStandard_COUNT ] = {
	{
		{ nullptr, nullptr, nullptr, nullptr },
		{ nullptr },
		0,
		nullptr,
		0,
		nullptr
	} };

	#if defined( GEN_SYSTEM_WINDOWS ) || defined( GEN_SYSTEM_CYGWIN )

	FileInfo* file_get_standard( FileStandardType std )
	{
		if ( ! _std_file_set )
		{
	#	define GEN__SET_STD_FILE( type, v ) \
			_std_files[ type ].FD.p = v;     \
			_std_files[ type ].Ops  = default_file_operations
			GEN__SET_STD_FILE( EFileStandard_INPUT, GetStdHandle( STD_INPUT_HANDLE ) );
			GEN__SET_STD_FILE( EFileStandard_OUTPUT, GetStdHandle( STD_OUTPUT_HANDLE ) );
			GEN__SET_STD_FILE( EFileStandard_ERROR, GetStdHandle( STD_ERROR_HANDLE ) );
	#	undef GEN__SET_STD_FILE
			_std_file_set = true;
		}
		return &_std_files[ std ];
	}

	#else    // POSIX

	FileInfo* file_get_standard( FileStandardType std )
	{
		if ( ! _std_file_set )
		{
	#	define GEN__SET_STD_FILE( type, v ) \
			_std_files[ type ].fd.i = v;     \
			_std_files[ type ].ops  = default_file_operations
			GEN__SET_STD_FILE( EFileStandard_INPUT, 0 );
			GEN__SET_STD_FILE( EFileStandard_OUTPUT, 1 );
			GEN__SET_STD_FILE( EFileStandard_ERROR, 2 );
	#	undef GEN__SET_STD_FILE
			_std_file_set = true;
		}
		return &_std_files[ std ];
	}

	#endif

	FileError file_close( FileInfo* f )
	{
		if ( ! f )
			return EFileError_INVALID;

		if ( f->Filename )
			free( heap(), zpl_cast( char* ) f->Filename );

	#if defined( GEN_SYSTEM_WINDOWS )
		if ( f->FD.p == INVALID_HANDLE_VALUE )
			return EFileError_INVALID;
	#else
		if ( f->fd.i < 0 )
			return EFileError_INVALID;
	#endif

		if ( f->IsTemp )
		{
			f->Ops.close( f->FD );
			return EFileError_NONE;
		}

		if ( ! f->Ops.read_at )
			f->Ops = default_file_operations;
		f->Ops.close( f->FD );

	#if 0
		if ( f->Dir )
		{
			_dirinfo_free_entry( f->Dir );
			mfree( f->Dir );
			f->Dir = NULL;
		}
	#endif

		return EFileError_NONE;
	}

	FileError file_new( FileInfo* f, FileDescriptor fd, FileOperations ops, char const* filename )
	{
		FileError err = EFileError_NONE;
		sw        len = str_len( filename );

		f->Ops             = ops;
		f->FD              = fd;
		f->Dir             = nullptr;
		f->LastWriteTime   = 0;
		f->Filename        = alloc_array( heap(), char, len + 1 );
		mem_copy( zpl_cast( char* ) f->Filename, zpl_cast( char* ) filename, len + 1 );

		return err;
	}

	FileError file_open_mode( FileInfo* f, FileMode mode, char const* filename )
	{
		FileInfo file_ =
		{
			{ nullptr, nullptr, nullptr, nullptr },
			{ nullptr },
			0,
			nullptr,
			0,
			nullptr
		 };

		*f = file_;
		FileError err;

	#if defined( GEN_SYSTEM_WINDOWS ) || defined( GEN_SYSTEM_CYGWIN )
		err = _win32_file_open( &f->FD, &f->Ops, mode, filename );
	#else
		err = _posix_file_open( &f->fd, &f->ops, mode, filename );
	#endif

		if ( err == EFileError_NONE )
			return file_new( f, f->FD, f->Ops, filename );

		return err;
	}

	s64 file_size( FileInfo* f )
	{
		s64 size        = 0;
		s64 prev_offset = file_tell( f );

		file_seek_to_end( f );
		size = file_tell( f );

		file_seek( f, prev_offset );

		return size;
	}
#pragma endregion File Handling

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

#ifdef GEN_BENCHMARK
#pragma region Timing
	#if defined( GEN_COMPILER_MSVC ) && ! defined( __clang__ )
	u64 read_cpu_time_stamp_counter( void )
	{
		return __rdtsc();
	}
	#elif defined( __i386__ )
	u64 read_cpu_time_stamp_counter( void )
	{
		u64 x;
		__asm__ volatile( ".byte 0x0f, 0x31" : "=A"( x ) );
		return x;
	}
	#elif defined( __x86_64__ )
	u64 read_cpu_time_stamp_counter( void )
	{
		u32 hi, lo;
		__asm__ __volatile__( "rdtsc" : "=a"( lo ), "=d"( hi ) );
		return ( zpl_cast( u64 ) lo ) | ( ( zpl_cast( u64 ) hi ) << 32 );
	}
	#elif defined( __powerpc__ )
	u64 read_cpu_time_stamp_counter( void )
	{
		u64 result = 0;
		u32 upper, lower, tmp;
		__asm__ volatile(
			"0:                   \n"
			"\tmftbu   %0         \n"
			"\tmftb    %1         \n"
			"\tmftbu   %2         \n"
			"\tcmpw    %2,%0      \n"
			"\tbne     0b         \n"
			: "=r"( upper ), "=r"( lower ), "=r"( tmp )
		);
		result = upper;
		result = result << 32;
		result = result | lower;

		return result;
	}
	#elif defined( GEN_SYSTEM_EMSCRIPTEN )
	u64 read_cpu_time_stamp_counter( void )
	{
		return ( u64 )( emscripten_get_now() * 1e+6 );
	}
	#elif defined( GEN_CPU_ARM ) && ! defined( GEN_COMPILER_TINYC )
	u64 read_cpu_time_stamp_counter( void )
	{
	#	if defined( __aarch64__ )
		int64_t r = 0;
		asm volatile( "mrs %0, cntvct_el0" : "=r"( r ) );
	#	elif ( __ARM_ARCH >= 6 )
		uint32_t r = 0;
		uint32_t pmccntr;
		uint32_t pmuseren;
		uint32_t pmcntenset;

		// Read the user mode perf monitor counter access permissions.
		asm volatile( "mrc p15, 0, %0, c9, c14, 0" : "=r"( pmuseren ) );
		if ( pmuseren & 1 )
		{    // Allows reading perfmon counters for user mode code.
			asm volatile( "mrc p15, 0, %0, c9, c12, 1" : "=r"( pmcntenset ) );
			if ( pmcntenset & 0x80000000ul )
			{    // Is it counting?
				asm volatile( "mrc p15, 0, %0, c9, c13, 0" : "=r"( pmccntr ) );
				// The counter is set up to count every 64th cycle
				return ( ( int64_t )pmccntr ) * 64;    // Should optimize to << 6
			}
		}
	#	else
	#		error "No suitable method for read_cpu_time_stamp_counter for this cpu type"
	#	endif

		return r;
	}
	#else
	u64 read_cpu_time_stamp_counter( void )
	{
		GEN_PANIC( "read_cpu_time_stamp_counter is not supported on this particular setup" );
		return -0;
	}
	#endif

	#if defined( GEN_SYSTEM_WINDOWS ) || defined( GEN_SYSTEM_CYGWIN )

	u64 time_rel_ms( void )
	{
		local_persist LARGE_INTEGER win32_perf_count_freq = {};
		u64                         result;
		LARGE_INTEGER               counter;
		local_persist LARGE_INTEGER win32_perf_counter = {};
		if ( ! win32_perf_count_freq.QuadPart )
		{
			QueryPerformanceFrequency( &win32_perf_count_freq );
			GEN_ASSERT( win32_perf_count_freq.QuadPart != 0 );
			QueryPerformanceCounter( &win32_perf_counter );
		}

		QueryPerformanceCounter( &counter );

		result = ( counter.QuadPart - win32_perf_counter.QuadPart ) * 1000 / ( win32_perf_count_freq.QuadPart );
		return result;
	}

	#else

	#	if defined( GEN_SYSTEM_LINUX ) || defined( GEN_SYSTEM_FREEBSD ) || defined( GEN_SYSTEM_OPENBSD ) || defined( GEN_SYSTEM_EMSCRIPTEN )
	u64 _unix_gettime( void )
	{
		struct timespec t;
		u64             result;

		clock_gettime( 1 /*CLOCK_MONOTONIC*/, &t );
		result = 1000 * t.tv_sec + 1.0e-6 * t.tv_nsec;
		return result;
	}
	#	endif

	u64 time_rel_ms( void )
	{
	#	if defined( GEN_SYSTEM_OSX )
		u64 result;

		local_persist u64 timebase  = 0;
		local_persist u64 timestart = 0;

		if ( ! timestart )
		{
			mach_timebase_info_data_t tb = { 0 };
			mach_timebase_info( &tb );
			timebase   = tb.numer;
			timebase  /= tb.denom;
			timestart  = mach_absolute_time();
		}

		// NOTE: mach_absolute_time() returns things in nanoseconds
		result = 1.0e-6 * ( mach_absolute_time() - timestart ) * timebase;
		return result;
	#	else
		local_persist u64 unix_timestart = 0.0;

		if ( ! unix_timestart )
		{
			unix_timestart = _unix_gettime();
		}

		u64 now = _unix_gettime();

		return ( now - unix_timestart );
	#	endif
	}
	#endif

	f64 time_rel( void )
	{
		return ( f64 )( time_rel_ms() * 1e-3 );
	}
#pragma endregion Timing
#endif

// namespace gen
}

// gen_time
#endif
