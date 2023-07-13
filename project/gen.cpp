// ReSharper disable CppClangTidyClangDiagnosticSwitchEnum

#ifdef gen_time
#include "gen.hpp"

#pragma region GENCPP DEPENDENCIES
//! If its desired to roll your own dependencies, define GENCPP_PROVIDE_DEPENDENCIES before including this file.
//! Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES

// NOTE: Ensure we use standard methods for these calls if we use GEN_PICO
#pragma region Macros
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
#pragma endregion Macros

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
		*end =  ( uptr )  0;

		pool.PhysicalStart = data;
		pool.FreeList      = data;

		return pool;
	}
#pragma endregion Memory

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

	namespace Memory
	{
		global AllocatorInfo GlobalAllocator;
		global Array<Arena>  Global_AllocatorBuckets;

		void* Global_Allocator_Proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
		{
			Arena& last = Global_AllocatorBuckets.back();

			switch ( type )
			{
				case EAllocation_ALLOC:
				{
					if ( last.TotalUsed + size > last.TotalSize )
					{
						Arena bucket = Arena::init_from_allocator( heap(), BucketSize );

						if ( bucket.PhysicalStart == nullptr )
							fatal( "Failed to create bucket for Global_AllocatorBuckets");

						if ( ! Global_AllocatorBuckets.append( bucket ) )
							fatal( "Failed to append bucket to Global_AllocatorBuckets");

						last = Global_AllocatorBuckets.back();
					}

					return alloc_align( last, size, alignment );
				}
				case EAllocation_FREE:
				{
					// Doesn't recycle.
				}
				break;
				case EAllocation_FREE_ALL:
				{
					// Memory::cleanup instead.
				}
				break;
				case EAllocation_RESIZE:
				{
					if ( last.TotalUsed + size > last.TotalSize )
					{
						Arena bucket = Arena::init_from_allocator( heap(), BucketSize );

						if ( bucket.PhysicalStart == nullptr )
							fatal( "Failed to create bucket for Global_AllocatorBuckets");

						if ( ! Global_AllocatorBuckets.append( bucket ) )
							fatal( "Failed to append bucket to Global_AllocatorBuckets");

						last = Global_AllocatorBuckets.back();
					}

					void* result = alloc_align( last.Backing, size, alignment );

					if ( result != nullptr && old_memory != nullptr )
					{
						mem_copy( result, old_memory, old_size );
					}

					return result;
				}
			}

			return nullptr;
		}

		void setup()
		{
			GlobalAllocator = AllocatorInfo { & Global_Allocator_Proc, nullptr };

			Global_AllocatorBuckets = Array<Arena>::init_reserve( heap(), 128 );

			if ( Global_AllocatorBuckets == nullptr )
				fatal( "Failed to reserve memory for Global_AllocatorBuckets");

			Arena bucket = Arena::init_from_allocator( heap(), BucketSize );

			if ( bucket.PhysicalStart == nullptr )
				fatal( "Failed to create first bucket for Global_AllocatorBuckets");

			Global_AllocatorBuckets.append( bucket );
		}

		void cleanup()
		{
			s32 index = 0;
			s32 left  = Global_AllocatorBuckets.num();
			do
			{
				Arena* bucket = & Global_AllocatorBuckets[ index ];
				bucket->free();
				index++;
			}
			while ( left--, left );

			Global_AllocatorBuckets.free();
		}

	// namespace Memory
	}
// namespace gen
}
#endif
#pragma endregion GENCPP DEPENDENCIES

namespace gen
{
	namespace StaticData
	{
		global Array< Pool >  CodePools         = { nullptr };
		global Array< Arena > CodeEntriesArenas = { nullptr };
		global Array< Arena > StringArenas      = { nullptr };

		global StringTable StringCache;

		global AllocatorInfo Allocator_DataArrays       = heap();
		global AllocatorInfo Allocator_CodePool         = heap();
		global AllocatorInfo Allocator_CodeEntriesArena = heap();
		global AllocatorInfo Allocator_StringArena      = heap();
		global AllocatorInfo Allocator_StringTable      = heap();
		global AllocatorInfo Allocator_TypeTable        = heap();
	}

#pragma region Constants
	global Code t_auto;
	global Code t_void;
	global Code t_int;
	global Code t_bool;
	global Code t_char;
	global Code t_wchar_t;
	global Code t_class;
	global Code t_typename;

	#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
	global Code t_b32;

	global Code t_s8;
	global Code t_s16;
	global Code t_s32;
	global Code t_s64;

	global Code t_u8;
	global Code t_u16;
	global Code t_u32;
	global Code t_u64;

	global Code t_sw;
	global Code t_uw;

	global Code t_f32;
	global Code t_f64;
	#endif

	global Code access_public;
	global Code access_protected;
	global Code access_private;

	global Code module_global_fragment;
	global Code module_private_fragment;

	global Code pragma_once;

	global Code spec_const;
	global Code spec_consteval;
	global Code spec_constexpr;
	global Code spec_constinit;
	global Code spec_extern_linkage;
	global Code spec_global;
	global Code spec_inline;
	global Code spec_internal_linkage;
	global Code spec_local_persist;
	global Code spec_mutable;
	global Code spec_ptr;
	global Code spec_ref;
	global Code spec_register;
	global Code spec_rvalue;
	global Code spec_static_member;
	global Code spec_thread_local;
	global Code spec_volatile;
#pragma endregion Constants

#pragma region AST Body Case Macros
#	define AST_BODY_CLASS_UNALLOWED_TYPES \
	case Attributes:                      \
	case Class_Body:                      \
	case Enum_Body:                       \
	case Extern_Linkage:                  \
	case Function_Body:                   \
	case Function_Fwd:                    \
	case Global_Body:                     \
	case Namespace:                       \
	case Namespace_Body:                  \
	case Operator:                        \
	case Operator_Fwd:                    \
	case Parameters:                      \
	case Specifiers:                      \
	case Struct_Body:                     \
	case Typename:

#	define AST_BODY_FUNCTION_UNALLOWED_TYPES \
	case Access_Public:                      \
	case Access_Protected:                   \
	case Access_Private:                     \
	case Attributes:                         \
	case Class_Body:                         \
	case Enum_Body:                          \
	case Extern_Linkage:                     \
	case Friend:                             \
	case Function_Body:                      \
	case Function_Fwd:                       \
	case Global_Body:                        \
	case Namespace:                          \
	case Namespace_Body:                     \
	case Operator:                           \
	case Operator_Fwd:                       \
	case Operator_Member:                    \
	case Operator_Member_Fwd:                \
	case Parameters:                         \
	case Specifiers:                         \
	case Struct_Body:                        \
	case Typename:

#	define AST_BODY_GLOBAL_UNALLOWED_TYPES \
	case Access_Public: 				   \
	case Access_Protected: 				   \
	case Access_Private: 				   \
	case Attributes:                       \
	case Class_Body: 					   \
	case Enum_Body: 					   \
	case Execution: 					   \
	case Friend: 						   \
	case Function_Body: 				   \
	case Global_Body: 					   \
	case Namespace_Body: 				   \
	case Operator_Member: 				   \
	case Operator_Member_Fwd: 			   \
	case Parameters: 					   \
	case Specifiers: 					   \
	case Struct_Body: 					   \
	case Typename:

#	define AST_BODY_EXPORT_UNALLOWED_TYPES         AST_BODY_GLOBAL_UNALLOWED_TYPES
#	define AST_BODY_NAMESPACE_UNALLOWED_TYPES      AST_BODY_GLOBAL_UNALLOWED_TYPES
#	define AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES AST_BODY_GLOBAL_UNALLOWED_TYPES

#	define AST_BODY_STRUCT_UNALLOWED_TYPES         AST_BODY_CLASS_UNALLOWED_TYPES
#pragma endregion AST Body Case Macros

#pragma region AST
	Code Code::Global;
	Code Code::Invalid;

	AST* AST::duplicate()
	{
		using namespace ECode;

		Code
		result           = make_code();
		result->Parent   = Parent;
		result->Name     = Name;
		result->Type     = Type;
		result->Op       = Op;

		switch ( Type )
		{
			case Invalid:
				log_failure( "AST::duplicate: Cannot duplicate an invalid AST." );
				return nullptr;

			case Access_Public:
			case Access_Protected:
			case Access_Private:
			case Attributes:
			case Comment:
			case Execution:
			case Module:
			case Preprocessor_Include:
			case Untyped:
				// Can just be the same, as its a cached string.
				result->Content = Content;
				return result;

			case Specifiers:
				// Need to memcpy the specifiers.
				mem_copy( result->ArrSpecs, ArrSpecs, sizeof( ArrSpecs ) );
				result->StaticIndex = StaticIndex;
				return result;

			// The main purpose of this is to make sure entires in the AST are unique,
			// So that we can assign the new parent without corrupting the existing AST.
			case Class:
			case Class_Fwd:
			case Class_Body:
			case Enum:
			case Enum_Fwd:
			case Enum_Body:
			case Enum_Class:
			case Enum_Class_Fwd:
			case Export_Body:
			case Extern_Linkage:
			case Extern_Linkage_Body:
			case Friend:
			case Function:
			case Function_Fwd:
			case Function_Body:
			case Global_Body:
			case Namespace:
			case Namespace_Body:
			case Operator:
			case Operator_Fwd:
			case Operator_Cast:
			case Operator_Cast_Fwd:
			case Operator_Member:
			case Operator_Member_Fwd:
			case Parameters:
			case Struct:
			case Struct_Fwd:
			case Struct_Body:
			case Template:
			case Typedef:
			case Typename:
			case Union:
			case Union_Body:
			case Using:
			case Using_Namespace:
			case Variable:
				s32 index = 0;
				s32 left  = num_entries();
				while ( left -- )
				{
					// This will naturally duplicate the entire chain duplicate all of the ast nodes.
					// It may not be the most optimal way for memory reasons, however figuring out the heuristic
					// For when it should reparent all nodes or not is not within the simple scope of this library.
					result->add_entry( entry(index)->duplicate() );
					index++;
				}
				return result;
		}

		log_failure( "AST::duplicate: Unknown AST type %s.", type_str() );
		return nullptr;
	}

	String AST::to_string()
	{
	#	define ProcessModuleFlags() \
		if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Export ))  \
			result.append( "export " );             \
                                                                         \
		if ( bitfield_is_equal( u32, ModuleFlags, ModuleFlag::Import ))  \
			result.append( "import " );             \

		String result = String::make( Memory::GlobalAllocator, "" );

		switch ( Type )
		{
			using namespace ECode;

			case Invalid:
				log_failure("Attempted to serialize invalid code! - %s", Name);
			break;

			case Untyped:
			case Execution:
				result.append( Content );
			break;

			case Comment:
			{
				static char line[MaxCommentLineLength];

				s32 left  = Content.length();
				s32 index = 0;
				do
				{
					s32 length = 0;
					while ( left && Content[index] != '\n' )
					{
						length++;
						left--;
					}

					str_copy( line, Content, length );
					line[length] = '\0';

					result.append_fmt( "// %s", line );
				}
				while ( left--, left > 0 );
			}
			break;

			case Access_Private:
			case Access_Protected:
			case Access_Public:
				result.append( Name );
			break;

			case Attributes:
				result.append( Content );

			case Class:
			{
				ProcessModuleFlags();

				if ( num_entries() > 1 )
				{
					result.append( "class " );

					s32 idx = 1;

					if ( entry( idx )->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
					}

					result.append( Name );

					AST const* parent = entry( idx );

					if ( parent )
					{
						char const* access_level = to_str( ParentAccess );

						result.append_fmt( ": %s %s\n{\n"
							, access_level
							, parent
						);
					}
					else
					{
						result.append( "\n{\n" );
					}

					result.append_fmt( "%s\n};", body()->to_string() );
				}
				else
				{
					result.append_fmt( "class %s\n{\n%s\n};", Name, body()->to_string() );
				}
			}
			break;

			case Class_Fwd:
			{
				ProcessModuleFlags();

				s32 idx = 1;
				if ( entry(idx) && entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "class %s;", Name );
			}
			break;

			case Enum:
			{
				ProcessModuleFlags();

				result.append( "enum " );

				if ( num_entries() > 1 )
				{
					s32 idx = 1;

					AST const* current = entry( idx);

					if ( current->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
					}

					if ( current->Type == Typename)
					{
						result.append_fmt( "%s : %s\n{\n"
							, Name
							, entry( idx )->to_string()
						);
					}
					else
					{
						result.append_fmt( "%s\n{\n"
							, Name
						);
					}
				}
				else
				{
					result.append_fmt( "%s\n{\n"
						, Name
					);
				}

				result.append_fmt( "%s};"
					, body()->to_string()
				);
			}
			break;

			case Enum_Fwd:
			{
				ProcessModuleFlags();

				s32 idx = 1;
				if ( entry(idx) && entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "enum %s : %s;", Name, entry( 0 )->to_string() );
			}
			break;

			case Enum_Class:
			{
				ProcessModuleFlags();

				result.append( "enum class " );

				if ( num_entries()  > 1 )
				{
					s32 idx = 1;

					if ( entry( idx )->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
					}

					if ( entry( idx )->Type == Typename )
					{
						result.append_fmt( "%s : %s\n{\n"
							, Name
							, entry( idx )->to_string()
						);
					}
					else
					{
						result.append_fmt( "%s\n{\n"
							, Name
						);
					}
				}

				result.append_fmt( "%s\n};"
					, body()->to_string()
				);
			}
			break;

			case Enum_Class_Fwd:
			{
				ProcessModuleFlags();

				result.append( "enum class " );

				s32 idx = 0;

				if ( entry(idx) && entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s : %s;", Name, entry( idx )->to_string() );
			}
			break;

			case Export_Body:
			{
				result.append_fmt( "export\n{\n" );

				s32 index = 0;
				s32 left  = num_entries();
				while ( left -- )
				{
					result.append_fmt( "%s\n", entry( index )->to_string() );
					index++;
				}

				result.append_fmt( "};" );
			}
			break;

			case Extern_Linkage:
				ProcessModuleFlags();

				result.append_fmt( "extern \"%s\"\n{\n%s\n}"
					, Name
					, body()->to_string()
				);
			break;

			case Friend:
				result.append_fmt( "friend %s;", entry( 0 )->to_string() );
			break;

			case Function:
			{
				ProcessModuleFlags();

				u32 idx  = 1;
				u32 left = num_entries();

				AST* current = entry( idx );

				if ( current && current->Type == Attributes )
				{
					result.append_fmt( "%s ", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}

				if ( current && current->Type == Specifiers )
				{
					result.append_fmt( "%s\n", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}

				if ( current && current->Type == Typename )
				{
					result.append_fmt( "%s ", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}

				result.append_fmt( "%s(", Name );

				if ( left && current && current->Type == Parameters )
				{
					result.append_fmt("%s", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}
				else
				{
					result.append( "void" );
				}

				result.append_fmt( ")\n{\n%s\n}"
					, body()->to_string()
				);
			}
			break;

			case Function_Fwd:
			{
				ProcessModuleFlags();

				u32 idx  = 0;
				u32 left = num_entries();

				AST* current = entry( idx );

				if ( current && current->Type == Attributes )
				{
					result.append_fmt( "%s ", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}

				if ( current && current->Type == Specifiers )
				{
					result.append_fmt( "%s\n", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}

				if ( current && current->Type == Typename )
				{
					result.append_fmt( "%s ", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}

				result.append_fmt( "%s(", Name );

				if ( left && current && current->Type == Parameters )
				{
					result.append_fmt("%s", current->to_string() );
					idx++;
					left--;
					current = entry( idx );
				}
				else
				{
					result.append( "void" );
				}

				result.append( ");" );
			}
			break;

			case Module:
				if (((u32(ModuleFlag::Export) & u32(ModuleFlags)) == u32(ModuleFlag::Export)))
					result.append("export ");

				if (((u32(ModuleFlag::Import) & u32(ModuleFlags)) == u32(ModuleFlag::Import)))
					result.append("import ");

				result.append_fmt( "%s;", Name );
				break;

			case Namespace:
				ProcessModuleFlags();

				result.append_fmt( "namespace %s\n{\n%s}"
					, Name
					, body()->to_string()
				);
			break;

			case Operator:
			case Operator_Member:
			{
				ProcessModuleFlags();

				s32 idx = 1;

				if ( entry(idx) && entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				if ( entry(idx) && entry( idx )->Type == Specifiers )
				{
					result.append_fmt( "%s\n", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s %s (", entry( idx )->to_string(), Name );
				idx++;

				if ( entry(idx) && entry( idx )->Type == Parameters )
				{
					result.append_fmt( "%s", entry( idx )->to_string() );
					idx++;
				}
				else
				{
					result.append_fmt( "void" );
				}

				result.append_fmt( ")\n{\n%s\n}"
					, body()->to_string()
				);
			}
			break;

			case Operator_Fwd:
			case Operator_Member_Fwd:
			{
				ProcessModuleFlags();

				s32 idx = 0;

				if ( entry(idx) && entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				if ( entry(idx) && entry( idx )->Type == Specifiers )
				{
					result.append_fmt( "%s", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s %s (", entry( idx )->to_string(), Name );
				idx++;

				if ( entry(idx) && entry( idx )->Type == Parameters )
				{
					result.append_fmt( "%s);", entry( idx )->to_string() );
					idx++;
				}
				else
				{
					result.append_fmt( "void);" );
				}
			}
			break;

			case Operator_Cast:
				result.append_fmt("operator %s(){\n%s\n}", entry(1)->to_string(), body()->to_string() );
			break;

			case Operator_Cast_Fwd:
				result.append_fmt("operator %s();", entry(0)->to_string() );
			break;

			case Parameters:
			{
				if ( Name )
				{
					result.append_fmt( "%s %s", entry( 0 )->to_string(), Name );
				}
				else
					result.append_fmt( "%s", entry(0)->to_string() );

				s32 index = 1;
				s32 left  = num_entries() - 1;

				while ( left-- )
				{
					if ( Name )
						result.append_fmt( ", %s %s"
							, entry( index )->entry( 0 )->to_string()
							, entry( index )->Name
						);

					else
						result.append_fmt( "%s", entry(0)->to_string() );

					index++;
				}
			}
			break;

			case Preprocessor_Include:
				result.append_fmt( "#include \"%s\"", Name );
			break;

			case Specifiers:
			{
				s32 idx  = 0;
				s32 left = StaticIndex;
				while ( left-- )
				{
					result.append_fmt( "%s ", (char const*)ESpecifier::to_str( ArrSpecs[idx]) );
					idx++;
				}
			}
			break;

			case Struct:
			{
				ProcessModuleFlags();

				result.append( "struct ");

				if ( num_entries() > 1 )
				{
					s32 idx = 1;

					AST* current = entry( idx );

					if ( current && current->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
						current = entry( idx );
					}

					result.append_fmt( "%s", Name );

					if ( current )
					{
						switch ( ParentAccess )
						{
							case AccessSpec::Private:
							case AccessSpec::Protected:
							case AccessSpec::Public:
								result.append_fmt( " : %s %s", to_str(ParentAccess), current->to_string() );
								idx++;
								break;

							default:
								result.append_fmt( " : %s", current->to_string() );
								break;
						}
					}

					result.append_fmt( "\n{\n%s\n};", body()->to_string() );
					break;
				}

				result.append_fmt( "%s\n{\n%s\n};", Name, body()->to_string() );
			}
			break;

			case Struct_Fwd:
			{
				ProcessModuleFlags();

				result.append( "struct ");

				s32 idx = 0;

				if ( entry(idx) && entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				result.append_fmt( "%s;", Name );
			}
			break;

			case Variable:
			{
				ProcessModuleFlags();

				s32 idx = 1;

				if ( num_entries() > 1 )
				{
					if ( entry( idx )->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
					}

					if ( entry( idx )->Type == Specifiers )
					{
						result.append_fmt( "%s", entry( idx )->to_string() );
						idx++;
					}

					result.append_fmt( "%s %s", entry( 0 )->to_string(), Name );

					AST* type     = entry( 0);
					AST* type_arr = type->entry( 0 );

					// TODO : This problably needs to be an iteration for all entries of type.
					if ( type->num_entries() && type_arr->Type == ECode::Untyped )
						result.append_fmt( "[%s]", type_arr->to_string() );

					if ( entry( idx ) )
						result.append_fmt( " = %s;", entry( idx )->to_string() );

					break;
				}

				AST* type     = entry( 0);
				AST* type_arr = type->entry( 0 );

				// TODO : This problably needs to be an iteration for all entries of type.
				if ( type->num_entries() && type_arr->Type == ECode::Untyped )
					result.append_fmt( "%s %s[%s];", type->to_string(), Name, type_arr->to_string() );

				else
					result.append_fmt( "%s %s;", entry( 0 )->to_string(), Name );
			}
			break;

			case Template:
			{
				ProcessModuleFlags();

				result.append_fmt( "template< %s >\n%s", entry( 1 )->to_string(), body()->to_string() );
			}
			break;

			case Typedef:
			{
				ProcessModuleFlags();

				AST* type = entry( 0 );

				result.append( "typedef ");

				if ( entry( 1 ) && entry( 1 )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( 1 )->to_string() );
				}

				result.append_fmt( "%s %s", type->to_string(), Name );

				if ( type->entry( 1 ) )
				{
					result.append_fmt( "[%s];", type->entry( 1 )->to_string() );
				}
				else
				{
					result.append( ";" );
				}
			}
			break;

			case Typename:
				if ( num_entries() )
				{
					s32 idx = 0;

					if ( entry(idx)->Type == Attributes )
					{
						result.append_fmt( "%s ", entry( idx )->to_string() );
						idx++;
					}

					if ( entry(idx)->Type == Specifiers )
					{
						result.append_fmt( "%s %s", Name, entry( idx )->to_string() );
					}
					else
					{
						result.append_fmt( "%s", Name );
					}

				}
				else
				{
					result.append_fmt( "%s", Name );
				}
			break;

			case Union:
			{
				ProcessModuleFlags();

				s32 idx = 0;

				result.append( "union " );

				if ( entry( idx )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( idx )->to_string() );
					idx++;
				}

				if ( Name )
				{
					result.append_fmt( "%s\n{\n%s\n};"
						, Name
						, body()->to_string()
					);
				}
				else
				{
					// Anonymous union
					result.append_fmt( "\n{\n%s\n};"
						, body()->to_string()
					);
				}
			}
			break;

			case Using:
			{
				ProcessModuleFlags();

				AST* type = entry( 0 );

				if ( entry( 1 ) && entry( 1 )->Type == Attributes )
				{
					result.append_fmt( "%s ", entry( 1 )->to_string() );
				}

				if ( type )
				{
					result.append_fmt( "using %s = %s", Name, type->to_string() );

					if ( type->entry( 1 ) )
						result.append_fmt( "[%s]", type->entry( 1 )->to_string() );

					result.append( ";" );
				}
				else
					result.append_fmt( "using %s;", Name );
			}
			break;

			case Using_Namespace:
				result.append_fmt( "using namespace %s;", Name );
			break;

			case Class_Body:
			case Enum_Body:
			case Extern_Linkage_Body:
			case Function_Body:
			case Global_Body:
			case Namespace_Body:
			case Struct_Body:
			case Union_Body:
			{
				s32 index = 0;
				s32 left  = num_entries();
				while ( left -- )
				{
					result.append_fmt( "%s\n", entry( index )->to_string() );
					index++;
				}
			}
			break;
		}

		return result;

	#undef ProcessModuleFlags
	}

	bool AST::is_equal( AST* other )
	{
		if ( Type != other->Type )
			return false;

		switch ( Type )
		{
			case ECode::Typedef:
			case ECode::Typename:
			{
				if ( Name != other->Name )
					return false;

				if ( num_entries() != other->num_entries() )
					return false;

				for ( s32 i = 0; i < num_entries(); ++i )
				{
					if ( entry( i ) != other->entry( i ) )
						return false;
				}

				return true;
			}
		}

		if ( Name != other->Name )
			return false;

		return true;
	}

	bool AST::validate_body()
	{
		using namespace ECode;
	#define CheckBodyType( BodyType )                                               \
		if ( Type != BodyType )                                                     \
		{                                                                           \
			log_failure( "AST::validate_body: Invalid body type %s", debug_str() ); \
			return false;                                                           \
		}

	#define CheckEntries( Unallowed_Types )                                                               \
		do                                                                                                \
		{                                                                                                 \
			for ( s32 idx = 0; idx < num_entries(); idx++ )                                               \
			{                                                                                             \
				AST* elem = entry( idx );                                                                 \
                                                                                                          \
				switch ( elem->Type )                                                                     \
				{                                                                                         \
					Unallowed_Types                                                                       \
						log_failure( "AST::validate_body: Invalid entry in body %s", elem->debug_str() ); \
						return false;                                                                     \
				}                                                                                         \
			}                                                                                             \
		}                                                                                                 \
		while (0);

		switch ( Type )
		{
			case Class_Body:
				CheckBodyType( Class_Body );
				CheckEntries( AST_BODY_CLASS_UNALLOWED_TYPES );
			break;
			case Enum_Body:
				CheckBodyType( Enum_Body );
				for ( s32 idx = 0; idx < body()->num_entries(); idx++ )
				{
					AST* elem = entry( idx );

					if ( elem->Type != Untyped )
					{
						log_failure( "AST::validate_body: Invalid entry in enum body (needs to be untyped or comment) %s", elem->debug_str() );
						return false;
					}
				}
			break;
			case Export_Body:
				for ( s32 idx = 0; idx < num_entries(); idx++ )
				{
					AST* elem = entry( idx );

					if ( elem->Type != Untyped )
					{
						log_failure( "AST::validate_body: Invalid entry in export body %s", elem->debug_str() );
						return false;
					}
				}
			break;
			case Extern_Linkage:
				CheckBodyType( Extern_Linkage );
				CheckEntries( AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES );
			break;
			case Function_Body:
				CheckBodyType( Function_Body );
				CheckEntries( AST_BODY_FUNCTION_UNALLOWED_TYPES );
			break;
			case Global_Body:
				for ( s32 idx = 0; idx < num_entries(); idx++ )
				{
					AST* elem = entry( idx );

					if ( elem->Type != Untyped )
					{
						log_failure( "AST::validate_body: Invalid entry in global body %s", elem->debug_str() );
						return false;
					}
				}
			break;
			case Namespace_Body:
				CheckBodyType( Namespace_Body );
				CheckEntries( AST_BODY_NAMESPACE_UNALLOWED_TYPES );
			break;
			case Struct_Body:
				CheckBodyType( Struct_Body );
				CheckEntries( AST_BODY_STRUCT_UNALLOWED_TYPES );
			break;
			case Union_Body:
				CheckBodyType( Union_Body );
				for ( s32 idx = 0; idx < body()->num_entries(); idx++ )
				{
					AST* elem = entry( idx );

					if ( elem->Type != Untyped )
					{
						log_failure( "AST::validate_body: Invalid entry in union body (needs to be untyped or comment) %s", elem->debug_str() );
						return false;
					}
				}
			break;

			default:
				log_failure( "AST::validate_body: Invalid this AST does not have a body %s", debug_str() );
				return false;
		}

		return false;
	}
#pragma endregion AST

#pragma region Gen Interface
	void init()
	{
		using namespace StaticData;

		// Setup the arrays
		{
			CodePools = Array<Pool>::init_reserve( Allocator_DataArrays, InitSize_DataArrays );

			if ( CodePools == nullptr )
				fatal( "gen::init: Failed to initialize the CodePools array" );

			CodeEntriesArenas = Array<Arena>::init_reserve( Allocator_DataArrays, InitSize_DataArrays );

			if ( CodeEntriesArenas == nullptr )
				fatal( "gen::init: Failed to initialize the CodeEntriesPools array" );

			StringArenas = Array<Arena>::init_reserve( Allocator_DataArrays, InitSize_DataArrays );

			if ( StringArenas == nullptr )
				fatal( "gen::init: Failed to initialize the StringArenas array" );
		}

		// Setup the code pool and code entries arena.
		{
			Pool code_pool = Pool::init( Allocator_CodePool, CodePool_NumBlocks, sizeof(AST) );

			if ( code_pool.PhysicalStart == nullptr )
				fatal( "gen::init: Failed to initialize the code pool" );

			CodePools.append( code_pool );

			Arena code_entires_arena = Arena::init_from_allocator( Allocator_CodeEntriesArena, SizePer_CodeEntriresArena );

			if ( code_entires_arena.PhysicalStart == nullptr )
				fatal( "gen::init: Failed to initialize the code entries arena" );

			CodeEntriesArenas.append( code_entires_arena );

			Arena string_arena = Arena::init_from_allocator( Allocator_StringArena, SizePer_StringArena );

			if ( string_arena.PhysicalStart == nullptr )
				fatal( "gen::init: Failed to initialize the string arena" );

			StringArenas.append( string_arena );
		}

		// Setup the hash tables
		{
			StringCache = StringTable::init( Allocator_StringTable );

			if ( StringCache.Entries == nullptr )
				fatal( "gen::init: Failed to initialize the StringCache");
		}

		Code::Global          = make_code();
		Code::Global->Name    = get_cached_string( txt_StrC("Global Code") );
		Code::Global->Content = Code::Global->Name;

		Code::Invalid = make_code();
		Code::Invalid.set_global();

	#	define def_constant_code_type( Type_ )    \
		t_##Type_ = def_type( name(Type_) ); \
		t_##Type_.set_global();

		def_constant_code_type( auto );
		def_constant_code_type( void );
		def_constant_code_type( int );
		def_constant_code_type( bool );
		def_constant_code_type( char );
		def_constant_code_type( wchar_t );
		def_constant_code_type( class );
		def_constant_code_type( typename );

	#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
		t_b32 = def_type( name(b32) );

		def_constant_code_type( s8 );
		def_constant_code_type( s16 );
		def_constant_code_type( s32 );
		def_constant_code_type( s64 );

		def_constant_code_type( u8 );
		def_constant_code_type( u16 );
		def_constant_code_type( u32 );
		def_constant_code_type( u64 );

		def_constant_code_type( sw );
		def_constant_code_type( uw );

		def_constant_code_type( f32 );
		def_constant_code_type( f64 );
	#endif
	#	undef def_constant_code_type

		access_private       = make_code();
		access_private->Type = ECode::Access_Private;
		access_private->Name = get_cached_string( txt_StrC("private:") );
		access_private.set_global();

		access_protected = make_code();
		access_protected->Type = ECode::Access_Protected;
		access_protected->Name = get_cached_string( txt_StrC("protected:") );
		access_protected.set_global();

		access_public = make_code();
		access_public->Type = ECode::Access_Public;
		access_public->Name = get_cached_string( txt_StrC("public:") );
		access_public.set_global();

		module_global_fragment          = make_code();
		module_global_fragment->Type    = ECode::Untyped;
		module_global_fragment->Name    = get_cached_string( txt_StrC("module;") );
		module_global_fragment->Content = module_global_fragment->Name;
		module_global_fragment.set_global();

		module_private_fragment          = make_code();
		module_private_fragment->Type    = ECode::Untyped;
		module_private_fragment->Name    = get_cached_string( txt_StrC("module : private;") );
		module_private_fragment->Content = module_private_fragment->Name;
		module_private_fragment.set_global();

		pragma_once          = make_code();
		pragma_once->Type    = ECode::Untyped;
		pragma_once->Name    = get_cached_string( txt_StrC("#pragma once") );
		pragma_once->Content = pragma_once->Name;
		pragma_once.set_global();

	#	pragma push_macro( "global" )
	#	pragma push_macro( "internal" )
	#	pragma push_macro( "local_persist" )
	#	define global        global
	#	define internal      internal
	#	define local_persist local_persist

	#	define def_constant_spec( Type_, ... )                                    \
		spec_##Type_ = def_specifiers( num_args(__VA_ARGS__), __VA_ARGS__); \
		spec_##Type_.set_global();

		def_constant_spec( const,            ESpecifier::Const );
		def_constant_spec( consteval,        ESpecifier::Consteval );
		def_constant_spec( constexpr,        ESpecifier::Constexpr );
		def_constant_spec( constinit,        ESpecifier::Constinit );
		def_constant_spec( extern_linkage,   ESpecifier::External_Linkage );
		def_constant_spec( global,           ESpecifier::Global );
		def_constant_spec( inline,           ESpecifier::Inline );
		def_constant_spec( internal_linkage, ESpecifier::Internal_Linkage );
		def_constant_spec( local_persist,    ESpecifier::Local_Persist );
		def_constant_spec( mutable,          ESpecifier::Mutable );
		def_constant_spec( ptr,              ESpecifier::Ptr );
		def_constant_spec( ref,              ESpecifier::Ref );
		def_constant_spec( register,         ESpecifier::Register );
		def_constant_spec( rvalue,           ESpecifier::RValue );
		def_constant_spec( static_member,    ESpecifier::Static_Member );
		def_constant_spec( thread_local,     ESpecifier::Thread_Local );
		def_constant_spec( volatile, 	     ESpecifier::Volatile)

		spec_local_persist = def_specifiers( 1, ESpecifier::Local_Persist );
		spec_local_persist.set_global();

	#	pragma pop_macro( "global" )
	#	pragma pop_macro( "internal" )
	#	pragma pop_macro( "local_persist" )

	#	undef def_constant_spec
	}

	void deinit()
	{
		using namespace StaticData;

		s32 index = 0;
		s32 left  = CodePools.num();
		do
		{
			Pool* code_pool = & CodePools[index];
			code_pool->free();
			index++;
		}
		while ( left--, left );

		index = 0;
		left  = CodeEntriesArenas.num();
		do
		{
			Arena* code_entries_arena = & CodeEntriesArenas[index];
			code_entries_arena->free();
			index++;
		}
		while ( left--, left );

		index = 0;
		left  = StringArenas.num();
		do
		{
			Arena* string_arena = & StringArenas[index];
			string_arena->free();
			index++;
		}
		while ( left--, left );

		StringCache.destroy();

		CodePools.free();
		CodeEntriesArenas.free();
		StringArenas.free();
	}

	inline
	AllocatorInfo get_string_allocator( s32 str_length )
	{
		using namespace StaticData;

		Arena& last = StringArenas.back();

		if ( last.TotalUsed + str_length > last.TotalSize )
		{
			Arena new_arena = Arena::init_from_allocator( Allocator_StringArena, SizePer_StringArena );

			if ( ! StringArenas.append( new_arena ) )
				fatal( "gen::get_string_allocator: Failed to allocate a new string arena" );

			last = StringArenas.back();
		}

		return last;
	}

	// Will either make or retrive a code string.
	StringCached get_cached_string( StrC str )
	{
		using namespace StaticData;

		s32 hash_length = str.Len > kilobytes(1) ? kilobytes(1) : str.Len;
		u32 key         = crc32( str.Ptr, hash_length );
		{
			StringCached* result = StringCache.get( key );

			if ( result )
				return * result;
		}

		String result = String::make( get_string_allocator( str.Len ), str );

		StringCache.set( key, result );

		return result;
	}

	/*
		Used internally to retireve a Code object form the CodePool.
	*/
	Code make_code()
	{
		using namespace StaticData;

		AllocatorInfo allocator = CodePools.back();

		s32 index = 0;
		s32 left  = CodePools.num();
		do
		{
			if ( CodePools[index].FreeList != nullptr  )
			{
				allocator = CodePools[index];
				break;
			}
			index++;
		}
		while ( left--, left );

		if ( allocator.Data == nullptr )
		{
			Pool code_pool = Pool::init( Allocator_CodePool, CodePool_NumBlocks, sizeof(AST) );

			if ( code_pool.PhysicalStart == nullptr )
				fatal( "gen::make_code: Failed to allocate a new code pool - CodePool allcoator returned nullptr." );

			if ( ! CodePools.append( code_pool ) )
				fatal( "gen::make_code: Failed to allocate a new code pool - CodePools failed to append new pool." );

			allocator = * CodePools;
		}

		Code result { rcast( AST*, alloc( allocator, sizeof(AST) )) };

		result->Content        = { nullptr };
		result->Parent         = nullptr;
		result->Name           = { nullptr };
		result->Type           = ECode::Invalid;
		result->Op             = EOperator::Invalid;
		result->ModuleFlags    = ModuleFlag::Invalid;
		result->ParentAccess   = AccessSpec::Invalid;
		result->StaticIndex    = 0;
		result->DynamicEntries = false;

		return result;
	}

	Array< AST* > make_code_entries()
	{
		using namespace StaticData;

		AllocatorInfo allocator = { nullptr, nullptr };

		s32 index = 0;
		s32 left  = CodeEntriesArenas.num();
		do
		{
			if ( CodeEntriesArenas[index].size_remaining( GEN_DEFAULT_MEMORY_ALIGNMENT) >= InitSize_CodeEntiresArray )
				allocator = CodeEntriesArenas[index];
			index++;
		}
		while( left--, left );

		if ( allocator.Data == nullptr )
		{
			Arena arena = Arena::init_from_allocator( Allocator_CodeEntriesArena, SizePer_CodeEntriresArena );

			if ( arena.PhysicalStart == nullptr )
				fatal( "gen::make_code: Failed to allocate a new code entries arena - CodeEntriesArena allcoator returned nullptr." );

			allocator = arena;
			CodeEntriesArenas.append( arena );
		}

		Array< AST* > entry_array = Array< AST* >::init( allocator );
		return entry_array;
	}

	enum class OpValidateResult : u32
	{
		Fail,
		Global,
		Member
	};

	inline
	OpValidateResult operator__validate( OperatorT op, Code params_code, Code ret_type, Code specifier )
	{
		using namespace EOperator;

		if ( op == EOperator::Invalid )
		{
			log_failure("gen::def_operator: op cannot be invalid");
			return OpValidateResult::Fail;
		}

	#pragma region Helper Macros
	#	define check_params()                                                                                  \
		if ( ! params_code )                                                                                   \
		{                                                                                                      \
			log_failure("gen::def_operator: params is null and operator%s requires it", to_str(op));           \
			return OpValidateResult::Fail;                                                                     \
		}                                                                                                      \
		if ( params_code->Type != ECode::Parameters )                                                          \
		{                                                                                                      \
			log_failure("gen::def_operator: params is not of Parameters type - %s", params_code->debug_str()); \
			return OpValidateResult::Fail;                                                                     \
		}

	#	define check_param_eq_ret()                                                                    \
		if ( ! is_member_symbol && params_code->param_type() != ret_type )                             \
		{                                                                                              \
			log_failure("gen_def_operator: operator%s requires first parameter to equal return type\n" \
				"param types: %s\n"                                                                    \
				"return type: %s",                                                                     \
				to_str(op),                                                                            \
				params_code->debug_str(),                                                              \
				ret_type->debug_str()                                                                  \
			);                                                                                         \
			return OpValidateResult::Fail;                                                             \
		}
	#pragma endregion Helper Macros

		if ( ! ret_type )
		{
			log_failure("gen::def_operator: ret_type is null but is required by operator%s", to_str(op));
		}

		if ( ret_type->Type != ECode::Typename )
		{
			log_failure("gen::def_operator: ret_type is not of typename type - %s", ret_type->debug_str());
			return OpValidateResult::Fail;
		}

		bool is_member_symbol = false;

		switch ( op )
		{
		#	define specs( ... ) num_args( __VA_ARGS__ ), __VA_ARGS__
			case Assign:
				check_params();

				if ( params_code->param_count() > 1 )
				{
					log_failure("gen::def_operator: "
						"operator%s does not support non-member definition (more than one parameter provided) - %s",
						to_str(op),
						params_code->debug_str()
					);
					return OpValidateResult::Fail;
				}

				is_member_symbol = true;
			break;

			case Assign_Add:
			case Assign_Subtract:
			case Assign_Multiply:
			case Assign_Divide:
			case Assign_Modulo:
			case Assign_BAnd:
			case Assign_BOr:
			case Assign_BXOr:
			case Assign_LShift:
			case Assign_RShift:
				check_params();

				if ( params_code->param_count() == 1 )
					is_member_symbol = true;

				else
					check_param_eq_ret();

				if (params_code->param_count() > 2 )
				{
					log_failure("gen::def_operator: operator%s may not be defined with more than two parametes - param count; %d\n%s"
						, to_str(op)
						, params_code->param_count()
						, params_code->debug_str()
					);
					return OpValidateResult::Fail;
				}
			break;

			case Increment:
			case Decrement:
				// If its not set, it just means its a prefix member op.
				if ( params_code )
				{
					if ( params_code->Type != ECode::Parameters )
					{
						log_failure("gen::def_operator: operator%s params code provided is not of Parameters type - %s"
							, to_str(op)
							, params_code->debug_str()
						);
						return OpValidateResult::Fail;
					}

					switch ( params_code->param_count() )
					{
						case 1:
							if ( params_code->param_type()->is_equal( t_int ) )
								is_member_symbol = true;

							else
								check_param_eq_ret();
						break;

						case 2:
							check_param_eq_ret();

							if ( ! params_code->get_param(1)->is_equal( t_int ) )
							{
								log_failure("gen::def_operator: "
									"operator%s requires second parameter of non-member definition to be int for post-decrement",
									to_str(op)
								);
								return OpValidateResult::Fail;
							}
						break;

						default:
							log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-2"
								, to_str(op)
								, params_code->param_count()
							);
							return OpValidateResult::Fail;
					}
				}
			break;

			case Unary_Plus:
			case Unary_Minus:
			case BNot:
				if ( ! params_code )
					is_member_symbol = true;

				else
				{
					if ( params_code->Type != ECode::Parameters )
					{
						log_failure("gen::def_operator: params is not of Parameters type - %s", params_code->debug_str());
						return OpValidateResult::Fail;
					}

					if ( params_code->param_type()->is_equal( ret_type ) )
					{
						log_failure("gen::def_operator: "
							"operator%s is non-member symbol yet first paramter does not equal return type\n"
							"param type: %s\n"
							"return type: %s\n"
							, params_code->debug_str()
							, ret_type->debug_str()
						);
						return OpValidateResult::Fail;
					}

					if ( params_code->param_count() > 1 )
					{
						log_failure("gen::def_operator: operator%s may not have more than one parameter - param count: %d"
							, to_str(op)
							, params_code->param_count()
						);
						return OpValidateResult::Fail;
					}
				}
			break;

			case Add:
			case Subtract:
			case Multiply:
			case Divide:
			case Modulo:
			case BAnd:
			case BOr:
			case BXOr:
			case LShift:
			case RShift:
				check_params();

				switch ( params_code->param_count() )
				{
					case 1:
						is_member_symbol = true;
					break;

					case 2:
						if ( ! params_code->param_type()->is_equal( ret_type ) )
						{
							log_failure("gen::def_operator: "
								"operator%s is non-member symbol yet first paramter does not equal return type\n"
								"param type: %s\n"
								"return type: %s\n"
								, params_code->debug_str()
								, ret_type->debug_str()
							);
							return OpValidateResult::Fail;
						}
					break;

					default:
						log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-2"
							, to_str(op)
							, params_code->param_count()
						);
						return OpValidateResult::Fail;
				}
			break;

			case UnaryNot:
				if ( ! params_code )
					is_member_symbol = true;

				else
				{
					if ( params_code->Type != ECode::Parameters )
					{
						log_failure("gen::def_operator: params is not of Parameters type - %s", params_code->debug_str());
						return OpValidateResult::Fail;
					}

					if ( params_code->param_count() != 1 )
					{
						log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-1"
							, to_str(op)
							, params_code->param_count()
						);
						return OpValidateResult::Fail;
					}
				}

				if ( ! ret_type->is_equal( t_bool ))
				{
					log_failure("gen::def_operator: operator%s return type must be of type bool - %s"
						, to_str(op)
						, ret_type->debug_str()
					);
					return OpValidateResult::Fail;
				}
			break;

			case LAnd:
			case LOr:
			case LEqual:
			case LNot:
			case Lesser:
			case Greater:
			case LesserEqual:
			case GreaterEqual:
				check_params();

				switch ( params_code->param_count() )
				{
					case 1:
						is_member_symbol = true;
					break;

					case 2:
					break;

					default:
						log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 1-2"
							, to_str(op)
							, params_code->param_count()
						);
						return OpValidateResult::Fail;
				}
			break;

			case Indirection:
			case AddressOf:
			case MemberOfPointer:
				if ( params_code && params_code->param_count() > 1)
				{
					log_failure("gen::def_operator: operator%s recieved unexpected number of paramters recived %d instead of 0-1"
						, to_str(op)
						, params_code->param_count()
					);
					return OpValidateResult::Fail;
				}
				else
				{
					is_member_symbol = true;
				}
			break;

			case PtrToMemOfPtr:
				if ( params_code )
				{
					log_failure("gen::def_operator: operator%s expects no paramters - %s", to_str(op), params_code->debug_str());
					return OpValidateResult::Fail;
				}
			break;

			case Subscript:
			case FunctionCall:
			case Comma:
				check_params();
			break;
		#	undef specs
		}

		return is_member_symbol ? OpValidateResult::Member : OpValidateResult::Global;
	#	undef check_params
	#	undef check_ret_type
	#	undef check_param_eq_ret
	}

	void set_allocator_data_arrays( AllocatorInfo allocator )
	{
		StaticData::Allocator_DataArrays = allocator;
	}

	void set_allocator_code_pool( AllocatorInfo allocator )
	{
		StaticData::Allocator_CodePool = allocator;
	}

	void set_allocator_code_entries_arena( AllocatorInfo allocator )
	{
		StaticData::Allocator_CodeEntriesArena = allocator;
	}

	void set_allocator_string_arena( AllocatorInfo allocator )
	{
		StaticData::Allocator_StringArena = allocator;
	}

	void set_allocator_string_table( AllocatorInfo allocator )
	{
		StaticData::Allocator_StringArena = allocator;
	}

#pragma region Helper Marcos
	// This snippet is used in nearly all the functions.
#	define name_check( Context_, Name_ )                                                            \
	{                                                                                               \
		if ( Name_.Len <= 0 )                                                                       \
		{                                                                                           \
			log_failure( "gen::" stringize(Context_) ": Invalid name length provided - %d",  Name_.Len ); \
			return Code::Invalid;                                                                   \
		}                                                                                           \
                                                                                                    \
		if ( Name_.Ptr == nullptr )                                                                 \
		{                                                                                           \
			log_failure( "gen::" stringize(Context_) ": name is null" );                                  \
			return Code::Invalid;                                                                   \
		}                                                                                           \
	}

#	define null_check( Context_, Code_ )                                              \
		if ( ! Code_ )                                                                \
		{                                                                             \
			log_failure( "gen::" stringize(Context_) ": " stringize(Code_) " provided is null" ); \
			return Code::Invalid;                                                     \
		}

#	define null_or_invalid_check( Context_, Code_ )                                     \
	{                                                                                   \
		if ( ! Code_ )                                                                  \
		{                                                                               \
			log_failure( "gen::" stringize(Context_) ": " stringize(Code_) " provided is null" );   \
			return Code::Invalid;                                                       \
		}                                                                               \
                                                                                        \
		if ( Code_->is_invalid() )                                                      \
		{                                                                               \
			log_failure("gen::" stringize(Context_) ": " stringize(Code_) " provided is invalid" ); \
			return Code::Invalid;                                                       \
		}                                                                               \
	}

#	define not_implemented( Context_ )                              \
		log_failure( "gen::%s: This function is not implemented" ); \
		return Code::Invalid;
#pragma endregion Helper Marcos

#pragma region Upfront Constructors
/*
	The implementaiton of the upfront constructors involves bascially doing three things:
	* Validate the arguments given to construct the intended type of AST is valid.
	* Construct said AST type.
	* Lock the AST (set to readonly) and return the valid object.

	If any of the validation fails, it triggers a call to log_failure with as much info the give the user so that they can hopefully
	identify the issue without having to debug too much (at least they can debug though...)

	The largest of the functions is related to operator overload definitions.
	The library validates a good protion of their form and thus the argument processing for is quite a bit.
*/
	Code def_attributes( StrC content )
	{
		if ( content.Len <= 0 || content.Ptr == nullptr )
		{
			log_failure( "gen::def_attributes: Invalid attributes provided" );
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Type    = ECode::Attributes;
		result->Name    = get_cached_string( content );
		result->Content = result->Name;

		return result;
	}

	Code def_comment( StrC content )
	{
		if ( content.Len <= 0 || content.Ptr == nullptr )
		{
			log_failure( "gen::def_comment: Invalid comment provided:" );
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Type    = ECode::Comment;
		result->Name    = get_cached_string( content );
		result->Content = result->Name;

		return result;
	}

	Code def_class( StrC name
		, Code body
		, Code parent, AccessSpec parent_access
		, Code attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_class, name );

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::def_class: attributes was not a 'Attributes' type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		if ( parent && (parent->Type != Class || parent->Type != Struct || parent->Type != Typename || parent->Type != Untyped) )
		{
			log_failure( "gen::def_class: parent provided is not type 'Class', 'Struct', 'Typeanme', or 'Untyped': %s", parent->debug_str() );
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		if ( body )
		{
			switch ( body->Type )
			{
				case Class_Body:
				case Untyped:
				break;

				default:
					log_failure("gen::def_class: body must be either of Class_Body or Untyped type - %s", body->debug_str());
					return Code::Invalid;
			}

			result->Type = Class;
			result->add_entry( body );
		}
		else
		{
			result->Type = Class_Fwd;
		}

		if ( attributes )
			result->add_entry( attributes );

		if ( parent )
		{
			result->ParentAccess = parent_access;
			result->add_entry( parent );
		}

		return result;
	}

	Code def_enum( StrC name
		, Code body, Code type
		, EnumT specifier, Code attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_enum, name );

		if ( type && type->Type != Typename )
		{
			log_failure( "gen::def_enum: enum underlying type provided was not of type Typename: %s", type->debug_str() );
			return Code::Invalid;
		}

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::def_enum: attributes was not a 'Attributes' type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		if ( body )
		{
			switch ( body->Type )
			{
				case Enum_Body:
				case Untyped:
				break;

				default:
					log_failure( "gen::def_enum: body must be of Enum_Body or Untyped type %s", body->debug_str());
					return Code::Invalid;
			}

			result->Type = specifier == EnumClass ?
				Enum_Class : Enum;

			result->add_entry( body );
		}
		else
		{
			result->Type = specifier == EnumClass ?
				Enum_Class_Fwd : Enum_Fwd;
		}

		if ( attributes )
			result->add_entry( attributes );

		if ( type )
		{
			result->add_entry( type );
		}
		else if ( result->Type != Enum_Class_Fwd && result->Type != Enum_Fwd )
		{
			log_failure( "gen::def_enum: enum forward declaration must have an underlying type" );
			return Code::Invalid;
		}

		return result;
	}

	Code def_execution( StrC content )
	{
		if ( content.Len <= 0 || content.Ptr == nullptr )
		{
			log_failure( "gen::def_execution: Invalid execution provided" );
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Type    = ECode::Execution;
		result->Name    = get_cached_string( content );
		result->Content = result->Name;

		return result;
	}

	Code def_extern_link( StrC name, Code body, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_extern_linkage, name );
		null_check( def_extern_linkage, body );

		if ( body->Type != Extern_Linkage_Body && body->Type != Untyped )
		{
			log_failure("gen::def_extern_linkage: body is not of extern_linkage or untyped type %s", body->debug_str());
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Type        = Extern_Linkage;
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		result->add_entry( body );

		return result;
	}

	Code def_friend( Code declaration )
	{
		using namespace ECode;

		null_check( def_friend, declaration );

		switch ( declaration->Type )
		{
			case Class_Fwd:
			case Function_Fwd:
			case Operator_Fwd:
			case Struct_Fwd:
			case Class:
			case Function:
			case Operator:
			case Struct:
			break;

			default:
				log_failure("gen::def_friend: requires declartion to have class, function, operator, or struct - %s", declaration->debug_str());
				return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Type = Friend;

		result->add_entry( declaration );

		return result;
	}

	Code def_function( StrC name
		, Code params , Code ret_type, Code body
		, Code specifiers, Code attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_function, name );

		if ( params && params->Type != Parameters )
		{
			log_failure( "gen::def_function: params was not a `Parameters` type: %s", params->debug_str() );
			return Code::Invalid;
		}

		if ( ret_type && ret_type->Type != Typename )
		{
			log_failure( "gen::def_function: ret_type was not a Typename: %s", ret_type->debug_str() );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_function: specifiers was not a `Specifiers` type: %s", specifiers->debug_str() );
			return Code::Invalid;
		}

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::def_function: attributes was not a `Attributes` type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		if ( body )
		{
			switch ( body->Type )
			{
				case Function_Body:
				case Execution:
				case Untyped:
					break;

				default:
				{
					log_failure("gen::def_function: body must be either of Function_Body, Execution, or Untyped type. %s", body->debug_str());
					return Code::Invalid;
				}
			}

			result->Type = Function;
			result->add_entry( body );
		}
		else
		{
			result->Type = Function_Fwd;
		}

		if ( attributes )
			result->add_entry( attributes );

		if ( specifiers )
			result->add_entry( specifiers );

		if ( ret_type )
		{
			result->add_entry( ret_type );
		}
		else
		{
			result->add_entry( t_void );
		}

		if ( params )
			result->add_entry( params );

		return result;
	}

	Code def_include ( StrC path )
	{
		if ( path.Len <= 0 || path.Ptr == nullptr )
		{
			log_failure( "gen::def_include: Invalid path provided - %d" );
			return Code::Invalid;
		}

		Code
		result          = make_code();
		result->Type    = ECode::Preprocessor_Include;
		result->Name    = get_cached_string( path );
		result->Content = result->Name;

		return result;
	}

	Code def_module( StrC name, ModuleFlag mflags )
	{
		name_check( def_module, name );

		Code
		result = make_code();
		result->Type        = ECode::Module;
		result->Name        = get_cached_string( name );
		result->Content     = result->Name;
		result->ModuleFlags = mflags;

		return result;
	}

	Code def_namespace( StrC name, Code body, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_namespace, name );
		null_check( def_namespace, body );

		if ( body->Type != Namespace_Body && body->Type != Untyped )
		{
			log_failure("gen::def_namespace: body is not of namespace or untyped type %s", body->debug_str());
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Type        = Namespace;
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		result->add_entry( body );

		return result;
	}

	Code def_operator( OperatorT op
		, Code params_code, Code ret_type, Code body
		, Code specifiers, Code attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::def_operator: Attributes was provided but its not of attributes type: %s", attributes->debug_str() );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != Specifiers )
		{
			log_failure( "gen::def_operator: Specifiers was provided but its not of specifiers type: %s", specifiers->debug_str() );
			return Code::Invalid;
		}

		OpValidateResult check_result = operator__validate( op, params_code, ret_type, specifiers );

		if ( check_result == OpValidateResult::Fail )
		{
			return Code::Invalid;
		}

		char const* name = str_fmt_buf( "operator %s", to_str(op) );

		Code
		result              = make_code();
		result->Name        = get_cached_string( { str_len(name), name } );
		result->ModuleFlags = mflags;

		if ( body )
		{
			switch ( body->Type )
			{
				case Function_Body:
				case Execution:
				case Untyped:
					break;

				default:
				{
					log_failure("gen::def_operator: body must be either of Function_Body, Execution, or Untyped type. %s", body->debug_str());
					return Code::Invalid;
				}
			}

			result->Type = check_result == OpValidateResult::Global ?
				Operator : Operator_Member;

			result->add_entry( body );
		}
		else
		{
			result->Type = check_result == OpValidateResult::Global ?
				Operator_Fwd : Operator_Member_Fwd;
		}

		if ( attributes )
			result->add_entry( attributes );

		if ( specifiers )
			result->add_entry( specifiers );

		result->add_entry( ret_type );

		if (params_code)
			result->add_entry( params_code );

		return result;
	}

	Code def_operator_cast( Code type, Code body )
	{
		using namespace ECode;
		null_check( def_operator_cast, type );

		if ( type->Type != Typename )
		{
			log_failure( "gen::def_operator_cast: type is not a typename - %s", type->debug_str() );
			return Code::Invalid;
		}

		Code result = make_code();

		if (body)
		{
			result->Type = Operator_Cast;

			if ( body->Type != Function_Body && body->Type != Execution )
			{
				log_failure( "gen::def_operator_cast: body is not of function body or execution type - %s", body->debug_str() );
				return Code::Invalid;
			}

			result->add_entry( body );
		}
		else
		{
			result->Type = Operator_Cast_Fwd;
		}

		result->add_entry( type );
		return result;
	}

	Code def_param( Code type, StrC name, Code value )
	{
		using namespace ECode;

		name_check( def_param, name );
		null_check( def_param, type );

		if ( type->Type != Typename )
		{
			log_failure( "gen::def_param: type is not a typename - %s", type->debug_str() );
			return Code::Invalid;
		}

		if ( value && value->Type != Untyped )
		{
			log_failure( "gen::def_param: value is not untyped - %s", value->debug_str() );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Type = Parameters;
		result->Name = get_cached_string( name );

		result->add_entry( type );

		if ( value )
			result->add_entry( value );

		return result;
	}

	Code def_specifier( SpecifierT spec )
	{
		Code
		result       = make_code();
		result->Type = ECode::Specifiers;

		result->add_specifier( spec );

		return result;
	}

	Code def_struct( StrC name
		, Code body
		, Code parent, AccessSpec parent_access
		, Code attributes
		, ModuleFlag mflags )
	{
		using namespace ECode;

		name_check( def_struct, name );

		if ( attributes && attributes->Type != Attributes )
		{
			log_failure( "gen::def_struct: attributes was not a `Attributes` type" );
			return Code::Invalid;
		}

		if ( parent && parent->Type != Typename )
		{
			log_failure( "gen::def_struct: parent was not a `Struct` type" );
			return Code::Invalid;
		}

		if ( body && body->Type != Struct_Body )
		{
			log_failure( "gen::def_struct: body was not a Struct_Body type" );
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		if ( body )
		{
			result->Type = Struct;
			result->add_entry( body );
		}
		else
		{
			result->Type = Struct_Fwd;
		}

		if ( attributes )
			result->add_entry( attributes );

		if ( parent )
		{
			result->ParentAccess = parent_access;
			result->add_entry( parent );
		}

		return result;
	}

	Code def_template( Code params, Code definition, ModuleFlag mflags )
	{
		null_check( def_template, definition );

		if ( params && params->Type != ECode::Parameters )
		{
			log_failure( "gen::def_template: params is not of parameters type - %s", params->debug_str() );
			return Code::Invalid;
		}

		switch (definition->Type )
		{
			case ECode::Class:
			case ECode::Function:
			case ECode::Struct:
			case ECode::Variable:
			case ECode::Using:
			break;

			default:
				log_failure( "gen::def_template: definition is not of class, function, struct, variable, or using type - %s", definition->debug_str() );
		}

		Code
		result              = make_code();
		result->Type        = ECode::Template;
		result->ModuleFlags = mflags;

		result->add_entry( definition );
		result->add_entry( params );

		return result;
	}

	Code def_type( StrC name, Code arrayexpr, Code specifiers, Code attributes )
	{
		name_check( def_type, name );

		if ( attributes && attributes->Type != ECode::Attributes )
		{
			log_failure( "gen::def_type: attributes is not of attributes type - %s", attributes->debug_str() );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != ECode::Specifiers )
		{
			log_failure( "gen::def_type: specifiers is not of specifiers type - %s", specifiers->debug_str() );
			return Code::Invalid;
		}

		if ( arrayexpr && arrayexpr->Type != ECode::Untyped )
		{
			log_failure( "gen::def_type: arrayexpr is not of untyped type - %s", arrayexpr->debug_str() );
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Name = get_cached_string( name );
		result->Type = ECode::Typename;

		if ( attributes )
			result->add_entry( attributes );

		if ( specifiers )
			result->add_entry( specifiers );

		if ( arrayexpr )
			result->add_entry( arrayexpr );

		return result;
	}

	Code def_typedef( StrC name, Code type, Code attributes, ModuleFlag mflags )
	{
		name_check( def_typedef, name );
		null_check( def_typedef, type );

		if ( type->Type != ECode::Typename )
		{
			log_failure( "gen::def_typedef: type was not a Typename" );
			return Code::Invalid;
		}

		if ( attributes && attributes->Type != ECode::Attributes )
		{
			log_failure( "gen::def_typedef: attributes was not a Attributes" );
			return Code::Invalid;
		}

		// Registering the type.
		Code registered_type = def_type( name );

		if ( ! registered_type )
		{
			log_failure( "gen::def_typedef: failed to register type" );
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->Name        = get_cached_string( name );
		result->Type        = ECode::Typedef;
		result->ModuleFlags = mflags;

		result->add_entry( type );

		if ( attributes )
			result->add_entry( attributes );

		return result;
	}

	Code def_union( StrC name, Code body, Code attributes, ModuleFlag mflags )
	{
		null_check( def_union, body );

		if ( body->Type != ECode::Union_Body )
		{
			log_failure( "gen::def_union: body was not a Union_Body type - %s", body->debug_str() );
			return Code::Invalid;
		}

		if ( attributes && attributes->Type != ECode::Attributes )
		{
			log_failure( "gen::def_union: attributes was not a Attributes type - %s", attributes->debug_str() );
			return Code::Invalid;
		}

		Code
		result = make_code();
		result->ModuleFlags = mflags;
		result->Type        = ECode::Union;

		if ( name.Ptr )
			result->Name = get_cached_string( name );

		result->add_entry( body );

		if ( attributes )
			result->add_entry( attributes );

		return result;
	}

	Code def_using( StrC name, Code type
		, Code attributes
		, ModuleFlag mflags )
	{
		name_check( def_using, name );
		null_check( def_using, type );

		Code register_type = def_type( name );

		if ( ! register_type )
		{
			log_failure( "gen::def_using: failed to register type" );
			return Code::Invalid;
		}

		if ( attributes && attributes->Type != ECode::Attributes )
		{
			log_failure( "gen::def_using: attributes was not a Attributes type - %s", attributes->debug_str() );
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;
		result->Type = ECode::Using;

		if ( type )
			result->add_entry( type );

		if ( attributes )
			result->add_entry( attributes );

		return result;
	}

	Code def_using_namespace( StrC name )
	{
		name_check( def_using_namespace, name );

		Code
		result          = make_code();
		result->Name    = get_cached_string( name );
		result->Content = result->Name;
		result->Type    = ECode::Using_Namespace;

		return result;
	}

	Code def_variable( Code type, StrC name, Code value
		, Code specifiers, Code attributes
		, ModuleFlag mflags )
	{
		name_check( def_variable, name );
		null_check( def_variable, type );

		if ( attributes && attributes->Type != ECode::Attributes )
		{
			log_failure( "gen::def_variable: attributes was not a `Attributes` type" );
			return Code::Invalid;
		}

		if ( specifiers && specifiers->Type != ECode::Specifiers )
		{
			log_failure( "gen::def_variable: specifiers was not a `Specifiers` type" );
			return Code::Invalid;
		}

		if ( type->Type != ECode::Typename )
		{
			log_failure( "gen::def_variable: type was not a Typename" );
			return Code::Invalid;
		}

		if ( value && value->Type != ECode::Untyped )
		{
			log_failure( "gen::def_variable: value was not a `Untyped` type" );
			return Code::Invalid;
		}

		Code
		result              = make_code();
		result->Name        = get_cached_string( name );
		result->Type        = ECode::Variable;
		result->ModuleFlags = mflags;

		result->add_entry( type );

		if ( attributes )
			result->add_entry( attributes );

		if ( specifiers )
			result->add_entry( specifiers );

		if ( value )
			result->add_entry( value );

		return result;
	}

/*
	Body related functions typically follow the same implementation pattern.
	Opted to use inline helper macros to get the implementaiton done.

	The implementation pattern is as follows:
	* Validate a valid parameter num was provided, or code array
		def_body_start or def_body_code_array_start( <name of function >)

	* Begin the code entry do-while loop, make sure each entry is valid processing its type in the switc
		def_body_code_validation_start( <name of function> )

	* Define the switch case statements between the macros.

	* Add the code entry, finish the closing implemenation for the do-while loop.
		def_body_code_validation_end( <name of function> )

	* Lock the body AST and return it.

	If a function's implementation deviates from the macros then its just writen it out.
*/

#pragma region Helper Macros for def_**_body functions
#	define def_body_start( Name_ )                                          \
	using namespace ECode;                                                  \
																            \
	if ( num <= 0 )                                                         \
	{                                                                       \
		log_failure("gen::" stringize(Name_) ": num cannot be zero or negative"); \
		return Code::Invalid;                                               \
	}

#	define def_body_code_array_start( Name_ )                                \
	using namespace ECode;                                                   \
																             \
	if ( num <= 0 )                                                          \
	{                                                                        \
		log_failure("gen::" stringize(Name_) ": num cannot be zero or negative");  \
		return Code::Invalid;                                                \
	}                                                                        \
                                                                             \
	if ( codes == nullptr )                                                  \
	{                                                                        \
		log_failure("gen::" stringize(Name_)" : Provided a null array of codes");  \
		return Code::Invalid;                                                \
	}

#	define def_body_code_validation_start( Name_ )                          \
	do                                                                      \
	{                                                                       \
		Code_POD pod   = va_arg(va, Code_POD);                              \
		Code     entry = pcast(Code, pod);                                  \
                                                                            \
		if ( ! entry )                                                      \
		{                                                                   \
			log_failure("gen::" stringize(Name_) ": Provided an null entry");     \
			return Code::Invalid;                                           \
		}                                                                   \
                                                                            \
		switch ( entry->Type )                                              \
		{

#	define def_body_code_array_validation_start( Name_ )                    \
	do                                                                      \
	{                                                                       \
		Code entry = *codes; codes++;                                       \
                                                                            \
		if ( ! entry )                                                      \
		{                                                                   \
			log_failure("gen::" stringize(Name_) ": Provided an null entry");     \
			return Code::Invalid;                                           \
		}                                                                   \
                                                                            \
		switch ( entry->Type )                                              \
		{

#	define def_body_code_validation_end( Name_ )                                                        \
				log_failure("gen::" stringize(Name_) ": Entry type is not allowed: %s", entry->debug_str() ); \
				return Code::Invalid;                                                                   \
                                                                                                        \
			default:                                                                                    \
				break;                                                                                  \
		}                                                                                               \
                                                                                                        \
		result->add_entry( entry );                                                                     \
	}                                                                                                   \
	while ( num--, num > 0 )
#pragma endregion Helper Macros for def_**_body functions

	Code def_class_body( s32 num, ... )
	{
		def_body_start( def_class_body );

		Code
		result       = make_code();
		result->Type = Class_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_class_body );
			AST_BODY_CLASS_UNALLOWED_TYPES
		def_body_code_validation_end( def_class_body );
		va_end(va);

		return result;
	}

	Code def_class_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_class_body );

		Code
		result       = make_code();
		result->Type = Function_Body;

		def_body_code_array_validation_start( def_class_body );
			AST_BODY_CLASS_UNALLOWED_TYPES
		def_body_code_validation_end( def_class_body );

		return result;
	}

	Code def_enum_body( s32 num, ... )
	{
		def_body_start( def_enum_body );

		Code
		result       = make_code();
		result->Type = Enum_Body;

		va_list va;
		va_start(va, num);
		do
		{
			Code_POD pod   = va_arg(va, Code_POD);
			Code     entry = pcast(Code, pod);

			if ( ! entry )
			{
				log_failure("gen::def_enum_body: Provided a null entry");
				return Code::Invalid;
			}

			if ( entry->Type != Untyped && entry->Type != Comment )
			{
				log_failure("gen::def_enum_body: Entry type is not allowed - %s. Must be of untyped or comment type.", entry->debug_str() ); \
				return Code::Invalid;
			}

			result->add_entry( entry );
		}
		while ( num--, num > 0 );
		va_end(va);

		return result;
	}

	Code def_enum_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_enum_body );

		Code
		result       = make_code();
		result->Type = Enum_Body;

		do
		{
			Code entry = *codes;

			if ( ! entry )
			{
				log_failure("gen::def_enum_body: Provided a null entry");
				return Code::Invalid;
			}

			if ( entry->Type != Untyped && entry->Type != Comment )
			{
				log_failure("gen::def_enum_body: Entry type is not allowed: %s", entry->debug_str() ); \
				return Code::Invalid;
			}

			result->add_entry( entry );
		}
		while ( codes++, num--, num > 0 );

		return result;
	}

	Code def_export_body( s32 num, ... )
	{
		def_body_start( def_export_body );

		Code
		result       = make_code();
		result->Type = Export_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_export_body );
			AST_BODY_EXPORT_UNALLOWED_TYPES
		def_body_code_validation_end( def_export_body );
		va_end(va);

		return result;
	}

	Code def_export_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_export_body );

		Code
		result       = make_code();
		result->Type = Export_Body;

		def_body_code_array_validation_start( def_export_body );
			AST_BODY_EXPORT_UNALLOWED_TYPES
		def_body_code_validation_end( def_export_body );

		return result;
	}

	Code def_extern_link_body( s32 num, ... )
	{
		def_body_start( def_extern_linkage_body );

		Code
		result       = make_code();
		result->Type = Extern_Linkage_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_extern_linkage_body );
			AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES
		def_body_code_validation_end( def_extern_linkage_body );
		va_end(va);

		return result;
	}

	Code def_extern_link_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_extern_linkage_body );

		Code
		result       = make_code();
		result->Type = Extern_Linkage_Body;

		def_body_code_array_validation_start( def_extern_linkage_body );
			AST_BODY_EXTERN_LINKAGE_UNALLOWED_TYPES
		def_body_code_validation_end( def_extern_linkage_body );

		return result;
	}

	Code def_function_body( s32 num, ... )
	{
		def_body_start( def_function_body );

		Code
		result       = make_code();
		result->Type = Function_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_function_body );
			AST_BODY_FUNCTION_UNALLOWED_TYPES
		def_body_code_validation_end( def_function_body );
		va_end(va);

		return result;
	}

	Code def_function_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_function_body );

		Code
		result       = make_code();
		result->Type = Function_Body;

		def_body_code_array_validation_start( def_function_body );
			AST_BODY_FUNCTION_UNALLOWED_TYPES
		def_body_code_validation_end( def_function_body );

		return result;
	}

	Code def_global_body( s32 num, ... )
	{
		def_body_start( def_global_body );

		Code
		result       = make_code();
		result->Type = Global_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_global_body );
			AST_BODY_GLOBAL_UNALLOWED_TYPES
		def_body_code_validation_end( def_global_body );
		va_end(va);

		return result;
	}

	Code def_global_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_global_body );

		Code
		result       = make_code();
		result->Type = Global_Body;

		def_body_code_array_validation_start( def_global_body );
			AST_BODY_GLOBAL_UNALLOWED_TYPES
		def_body_code_validation_end( def_global_body );

		return result;
	}

	Code def_namespace_body( s32 num, ... )
	{
		def_body_start( def_namespace_body );

		Code
		result = make_code();
		result->Type = Namespace_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_namespace_body );
			AST_BODY_NAMESPACE_UNALLOWED_TYPES
		def_body_code_validation_end( def_namespace_body );
		va_end(va);

		return result;
	}

	Code def_namespace_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_namespace_body );

		Code
		result       = make_code();
		result->Type = Global_Body;

		def_body_code_array_validation_start( def_namespace_body );
			AST_BODY_NAMESPACE_UNALLOWED_TYPES
		def_body_code_validation_end( def_namespace_body );

		return result;
	}

	Code def_params( s32 num, ... )
	{
		def_body_start( def_params );

		va_list va;
		va_start(va, num);

		Code_POD pod   = va_arg(va, Code_POD);
		Code     param = pcast( Code, pod );

		null_check( def_params, param );

		if ( param->Type != Parameters )
		{
			log_failure( "gen::def_params: param %d is not a Parameters", num - num + 1 );
			return Code::Invalid;
		}

		Code result = { param->duplicate() };

		while ( -- num )
		{
			pod   = va_arg(va, Code_POD);
			param = pcast( Code, pod );

			if ( param->Type != Parameters )
			{
				log_failure( "gen::def_params: param %d is not a Parameters", num - num + 1 );
				return Code::Invalid;
			}

			result->add_entry( param );
		}
		va_end(va);

		return result;
	}

	Code def_params( s32 num, Code* codes )
	{
		def_body_code_array_start( def_params );

	#	define check_current()                                                                                      \
		if ( current == nullptr )                                                                                   \
		{                                                                                                           \
			log_failure("gen::def_params: Provide a null code in codes array");                                     \
			return Code::Invalid;                                                                                   \
		}                                                                                                           \
                                                                                                                    \
		if (current->Type != Parameters )                                                                           \
		{                                                                                                           \
			log_failure("gen::def_params: Code in coes array is not of paramter type - %s", current->debug_str() ); \
			return Code::Invalid;                                                                                   \
		}

		Code current = * codes;
		check_current();

		Code
		result = make_code();
		result->Name = current->Name;
		result->Type = current->Type;

		result->add_entry( current->entry( 0 ) );

		while( codes++, current = * codes, num--, num > 0 )
		{
			check_current();
			result->add_entry( current );
		}
	#	undef check_current

		return result;
	}

	Code def_specifiers( s32 num, ... )
	{
		if ( num <= 0 )
		{
			log_failure("gen::def_specifiers: num cannot be zero or less");
			return Code::Invalid;
		}

		if ( num > AST::ArrSpecs_Cap )
		{
			log_failure("gen::def_specifiers: num of speciifers to define AST larger than AST specicifier capacity - %d", num);
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Type = ECode::Specifiers;

		va_list va;
		va_start(va, num);
		do
		{
			SpecifierT type = (SpecifierT)va_arg(va, int);

			result->add_specifier( type );
		}
		while ( --num, num );
		va_end(va);

		return result;
	}

	Code def_specifiers( s32 num, SpecifierT* specs )
	{
		if ( num <= 0 )
		{
			log_failure("gen::def_specifiers: num cannot be zero or less");
			return Code::Invalid;
		}

		if ( num > AST::ArrSpecs_Cap )
		{
			log_failure("gen::def_specifiers: num of speciifers to define AST larger than AST specicifier capacity - %d", num);
			return Code::Invalid;
		}

		Code
		result       = make_code();
		result->Type = ECode::Specifiers;

		s32 idx = 0;
		do
		{
			result->add_specifier( specs[idx] );
			idx++;
		}
		while ( --num, num );

		return result;
	}

	Code def_struct_body( s32 num, ... )
	{
		def_body_start( def_struct_body );

		Code
		result       = make_code();
		result->Type = Struct_Body;

		va_list va;
		va_start(va, num);
		def_body_code_validation_start( def_struct_body );
			AST_BODY_STRUCT_UNALLOWED_TYPES
		def_body_code_validation_end( def_struct_body );
		va_end(va);

		return result;
	}

	Code def_struct_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_struct_body );

		Code
		result       = make_code();
		result->Type = Struct_Body;

		def_body_code_array_validation_start( def_struct_body );
			AST_BODY_STRUCT_UNALLOWED_TYPES
		def_body_code_validation_end( def_struct_body );

		return result;
	}

	Code def_union_body( s32 num, ... )
	{
		def_body_start( def_union_body );

		Code
		result       = make_code();
		result->Type = Union_Body;

		va_list va;
		va_start(va, num);
		do
		{
			Code_POD pod   = va_arg(va, Code_POD);
			Code     entry = pcast( Code, pod );

			if ( ! entry )
			{
				log_failure("gen::def_union_body: Provided a null entry");
				return Code::Invalid;
			}

			if ( entry->Type != Untyped && entry->Type != Comment )
			{
				log_failure("gen::def_union_body: Entry type is not allowed - %s. Must be of untyped or comment type.", entry->debug_str() ); \
				return Code::Invalid;
			}

			result->add_entry( entry );
		}
		while ( num--, num > 0 );
		va_end(va);

		return result;
	}

	Code def_union_body( s32 num, Code* codes )
	{
		def_body_code_array_start( def_union_body );

		Code
		result       = make_code();
		result->Type = Union_Body;

		do
		{
			Code entry = *codes;

			if ( ! entry )
			{
				log_failure("gen::def_union_body: Provided a null entry");
				return Code::Invalid;
			}

			if ( entry->Type != Untyped && entry->Type != Comment )
			{
				log_failure("gen::def_union_body: Entry type is not allowed: %s", entry->debug_str() ); \
				return Code::Invalid;
			}

			result->add_entry( entry );
		}
		while ( codes++, num--, num > 0 );

		return result;
	}

#	undef name_check
#	undef null_check
#	undef null_or_invalid_check
#pragma endregion Upfront Constructors

#pragma region Parsing Constructors
#ifdef GEN_FEATURE_PARSING
/*
	These constructors are the most implementation intensive other than the edtior or scanner.
*/

	namespace Parser
	{

	/*
		This is a simple lexer that focuses on tokenizing only tokens relevant to the library.
		It will not be capable of lexing C++ code with unsupported features.
	*/

	#	define Define_TokType \
		Entry( Access_Private,        "private" )         \
		Entry( Access_Protected,      "protected" )       \
		Entry( Access_Public,         "public" )          \
		Entry( Access_MemberSymbol,   "." )               \
		Entry( Access_StaticSymbol,   "::")               \
		Entry( Ampersand,             "&" )               \
		Entry( Ampersand_DBL,         "&&" )              \
		Entry( Assign_Classifer,      ":" )               \
		Entry( BraceCurly_Open,       "{" )               \
		Entry( BraceCurly_Close,      "}" )               \
		Entry( BraceSquare_Open,      "[" )               \
		Entry( BraceSquare_Close,     "]" )               \
		Entry( Capture_Start,         "(" )               \
		Entry( Capture_End,           ")" )               \
		Entry( Comment,               "__comment__" )     \
		Entry( Char,                  "__char__" )        \
		Entry( Comma,                 "," )               \
		Entry( Decl_Class,            "class" )           \
		Entry( Decl_Enum,             "enum" )            \
		Entry( Decl_Extern_Linkage,   "extern" )          \
		Entry( Decl_Friend,           "friend" )          \
		Entry( Decl_Module,           "module" )          \
		Entry( Decl_Namespace,        "namespace" )       \
		Entry( Decl_Operator,         "operator" )        \
		Entry( Decl_Struct,           "struct" )          \
		Entry( Decl_Template,         "template" )        \
		Entry( Decl_Typedef,          "typedef" )         \
		Entry( Decl_Using,            "using" )           \
		Entry( Decl_Union,            "union" )           \
		Entry( Identifier,            "__SymID__" )       \
		Entry( Module_Import,         "import" )          \
		Entry( Module_Export,         "export" )          \
		Entry( Number,                "number" )          \
		Entry( Operator,              "operator" )        \
		Entry( Spec_Alignas,          "alignas" )         \
		Entry( Spec_Const,            "const" )           \
		Entry( Spec_Consteval,        "consteval" )       \
		Entry( Spec_Constexpr,        "constexpr" )       \
		Entry( Spec_Constinit,        "constinit" )       \
		Entry( Spec_Extern,           "extern" )          \
		Entry( Spec_Global, 		  "global" )          \
		Entry( Spec_Inline,           "inline" )          \
		Entry( Spec_Internal_Linkage, "internal" )        \
		Entry( Spec_LocalPersist,     "local_persist" )   \
		Entry( Spec_Mutable,          "mutable" )         \
		Entry( Spec_Static,           "static" )          \
		Entry( Spec_ThreadLocal,      "thread_local" )    \
		Entry( Spec_Volatile,         "volatile")         \
		Entry( Star,                  "*" )               \
		Entry( Statement_End,         ";" )               \
		Entry( String,                "__String__" )      \
		Entry( Type_Unsigned, 	      "unsigned" )        \
		Entry( Type_Signed,           "signed" )          \
		Entry( Type_Short,            "short" )           \
		Entry( Type_Long,             "long" )            \
		Entry( Type_char, 			  "char" )            \
		Entry( Type_int, 			  "int" )             \
		Entry( Type_double, 		  "double" )

		enum class TokType : u32
		{
		#	define Entry( Name_, Str_ ) Name_,
			Define_TokType
		#	undef Entry
			Attr_Keyword,

			Num,
			Invalid
		};

		struct Token
		{
			char const* Text;
			sptr        Length;
			TokType     Type;
			bool 	    IsAssign;

			operator bool()
			{
				return Text && Length && Type != TokType::Invalid;
			}

			operator StrC()
			{
				return { Length, Text };
			}
		};

		internal inline
		TokType get_tok_type( char const* word, s32 length )
		{
			local_persist
			StrC lookup[(u32)TokType::Num] =
			{
			#	define Entry( Name_, Str_ ) { sizeof(Str_), Str_ },
				Define_TokType
			#	undef Entry

				{ sizeof(Attribute::Keyword), Attribute::Keyword },
			};

			for ( u32 index = 0; index < (u32)TokType::Num; index++ )
			{
				s32         lookup_len = lookup[index].Len - 1;
				char const* lookup_str = lookup[index].Ptr;

				if ( lookup_len != length )
					continue;

				if ( str_compare( word, lookup_str, lookup_len ) == 0 )
					return scast(TokType, index);
			}

			return TokType::Invalid;
		}

		internal inline
		char const* str_tok_type( TokType type )
		{
			local_persist
			char const* lookup[(u32)TokType::Num] =
			{
			#	define Entry( Name_, Str_ ) Str_,
				Define_TokType
			#	undef Entry

				Attribute::Keyword,
			};

			return lookup[(u32)type];
		}

	#	undef Define_TokType

		internal inline
		bool tok_is_specifier( Token const& tok )
		{
			return (tok.Type <= TokType::Star && tok.Type >= TokType::Spec_Alignas)
				|| tok.Type == TokType::Ampersand
				|| tok.Type == TokType::Ampersand_DBL
			;
		}

		internal inline
		bool tok_is_access_specifier( Token const& tok )
		{
			return tok.Type >= TokType::Access_Private && tok.Type <= TokType::Access_Public;
		}

		internal inline
		AccessSpec tok_to_access_specifier( Token const& tok )
		{
			return scast(AccessSpec, tok.Type);
		}

		Arena LexAllocator;

		struct TokArray
		{
			Array<Token> Arr;
			s32          Idx;

			bool __eat( TokType type, char const* context )
			{
				if ( Arr.num() - Idx <= 0 )
				{
					log_failure( "gen::%s: No tokens left", context );
					return Code::Invalid;
				}

				if ( Arr[Idx].Type != type )
				{
					String token_str = String::make( Memory::GlobalAllocator, { Arr[Idx].Length, Arr[Idx].Text } );

					log_failure( "gen::%s: expected %s, got %s", context, str_tok_type(type), str_tok_type(Arr[Idx].Type) );

					return Code::Invalid;
				}

				Idx++;
				return true;
			}

			Token& current()
			{
				return Arr[Idx];
			}

			Token& previous()
			{
				return Arr[Idx - 1];
			}

			Token* next()
			{
				return Idx + 1 < Arr.num() ? &Arr[Idx + 1] : nullptr;
			}
		};

		TokArray lex( StrC content )
		{
		#	define current ( * scanner )

		#	define move_forward() \
			left--;               \
			scanner++

		#	define SkipWhitespace()                     \
			while ( left && char_is_space( current ) )  \
			{                                           \
				move_forward();                         \
			}

		#	define SkipWhitespace_Checked( Context_, Msg_, ... )             \
			while ( left && char_is_space( current ) )                       \
			{                                                                \
				move_forward();                                              \
			}                                                                \
			if ( left <= 0 )                                                 \
			{                                                                \
				log_failure( "gen::" txt(Context_) ": " Msg_, __VA_ARGS__ ); \
				return { 0, nullptr };                                       \
			}

			do_once_start
				// TODO : Use the global memory allocator for this...
				LexAllocator = Arena::init_from_allocator( heap(), megabytes(10) );

				if ( LexAllocator.PhysicalStart == nullptr )
				{
					log_failure( "gen::lex: failed to allocate memory for parsing constructor's lexer");
					return { { nullptr }, 0  };
				}
			do_once_end

			local_persist thread_local
			Array<Token> Tokens = { nullptr };

			s32         left    = content.Len;
			char const* scanner = content.Ptr;

			char const* word        = scanner;
			s32         word_length = 0;

			SkipWhitespace();
			if ( left <= 0 )
			{
				log_failure( "gen::lex: no tokens found (only whitespace provided)" );
				return { { nullptr }, 0 };
			}

			if ( Tokens )
				Tokens.clear();

			Tokens = Array<Token>::init_reserve( LexAllocator, content.Len / 8 );

			while (left )
			{
				Token token = { nullptr, 0, TokType::Invalid, false };

				SkipWhitespace();
				if ( left <= 0 )
					break;

				switch ( current )
				{
					case '.':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Access_MemberSymbol;

						if (left)
							move_forward();
						goto FoundToken;

					case '&' :
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Ampersand;

						if (left)
							move_forward();

						if ( current == '&' )	// &&
						{
							token.Length  = 2;
							token.Type    = TokType::Ampersand_DBL;

							if (left)
								move_forward();
						}

						goto FoundToken;

					case ':':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Assign_Classifer;

						if (left)
							move_forward();

						if ( current == ':' )
						{
							move_forward();
							token.Type  = TokType::Access_StaticSymbol;
							token.Length++;
						}
						goto FoundToken;

					case '{':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::BraceCurly_Open;

						if (left)
							move_forward();
						goto FoundToken;

					case '}':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::BraceCurly_Close;

						if (left)
							move_forward();
						goto FoundToken;

					case '[':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::BraceSquare_Open;
						if ( left )
						{
							move_forward();

							if ( current == ']' )
							{
								token.Length = 2;
								token.Type   = TokType::Operator;
								move_forward();
							}
						}
						goto FoundToken;

					case ']':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::BraceSquare_Close;

						if (left)
							move_forward();
						goto FoundToken;

					case '(':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Capture_Start;

						if (left)
							move_forward();
						goto FoundToken;

					case ')':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Capture_End;

						if (left)
							move_forward();
						goto FoundToken;

					case '\'':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Char;

						move_forward();

						while ( left && current != '\'' )
						{
							move_forward();
							token.Length++;
						}

						if ( left )
						{
							move_forward();
							token.Length++;
						}
						goto FoundToken;

					case ',':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Comma;

						if (left)
							move_forward();
						goto FoundToken;

					case '*':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Star;

						if (left)
							move_forward();
						goto FoundToken;

					case ';':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Statement_End;

						if (left)
							move_forward();
						goto FoundToken;

					case '"':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::String;

						move_forward();
						while ( left )
						{
							if ( current == '"' )
							{
								move_forward();
								break;
							}

							if ( current == '\\' )
							{
								move_forward();
								token.Length++;

								if ( left )
								{
									move_forward();
									token.Length++;
								}
								continue;
							}

							move_forward();
							token.Length++;
						}
						goto FoundToken;

					// All other operators we just label as an operator and move forward.
					case '=':
						token.Text     = scanner;
						token.Length   = 1;
						token.Type     = TokType::Operator;
						token.IsAssign = true;

						if (left)
							move_forward();

						goto FoundToken;

					case '+':
					case '%':
					case '^':
					case '~':
					case '!':
					case '<':
					case '>':
					case '|':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Operator;

						if (left)
							move_forward();

						if ( current == '=' )
						{
							token.Length++;
							token.IsAssign = true;

							if (left)
								move_forward();
						}
						else while ( left && current == *(scanner - 1) && token.Length < 3 )
						{
							token.Length++;

							if (left)
								move_forward();
						}
						goto FoundToken;

					// Dash is unfortunatlly a bit more complicated...
					case '-':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Operator;
						if ( left )
						{
							move_forward();

							if ( current == '>'  )
							{
								token.Length++;
								move_forward();

								if ( current == '*' )
								{
									token.Length++;
									move_forward();
								}
							}
							else if ( current == '=' )
							{
								token.Length++;
								token.IsAssign = true;

								if (left)
									move_forward();
							}
							else while ( left && current == *(scanner - 1) && token.Length < 3 )
							{
								token.Length++;

								if (left)
									move_forward();
							}
						}
						goto FoundToken;

					case '/':
						token.Text   = scanner;
						token.Length = 1;
						token.Type   = TokType::Operator;

						if ( left )
						{
							move_forward();

							if ( current == '/' )
							{
								token.Type = TokType::Comment;

								move_forward();
								token.Text   = scanner;
								token.Length = 0;

								while ( left && current != '\n' )
								{
									move_forward();
									token.Length++;
								}
							}
							else if ( current == '*' )
							{
								token.Type = TokType::Comment;

								move_forward();
								token.Text   = scanner;
								token.Length = 0;

								while ( left && ( current != '*' && *(scanner + 1) != '/' ) )
								{
									move_forward();
									token.Length++;
								}
								move_forward();
								move_forward();
							}
						}
						goto FoundToken;
				}

				if ( char_is_alpha( current ) || current == '_' )
				{
					token.Text   = scanner;
					token.Length = 1;
					move_forward();

					while ( left && ( char_is_alphanumeric(current) || current == '_' ) )
					{
						move_forward();
						token.Length++;
					}

					goto FoundToken;
				}
				else if ( char_is_digit(current) )
				{
					// This is a very brute force lex, no checks are done for validity of literal.

					token.Text   = scanner;
					token.Length = 1;
					token.Type   = TokType::Number;
					move_forward();

					if (left
					&& (	current == 'x' || current == 'X'
						||	current == 'b' || current == 'B'
						||  current == 'o' || current == 'O' )
					)
					{
						move_forward();
						token.Length++;

						while ( left && char_is_hex_digit(current) )
						{
							move_forward();
							token.Length++;
						}

						goto FoundToken;
					}

					while ( left && char_is_digit(current) )
					{
						move_forward();
						token.Length++;
					}

					if ( left && current == '.' )
					{
						move_forward();
						token.Length++;

						while ( left && char_is_digit(current) )
						{
							move_forward();
							token.Length++;
						}
					}

					goto FoundToken;
				}
				else
				{
					String context_str = String::fmt_buf( Memory::GlobalAllocator, "%s", scanner, min( 100, left ) );

					log_failure( "Failed to lex token %s", context_str );

					// Skip to next whitespace since we can't know if anything else is valid until then.
					while ( left && ! char_is_space( current ) )
					{
						move_forward();
					}
				}

			FoundToken:

				if ( token.Type != TokType::Invalid )
				{
					Tokens.append( token );
					continue;
				}

				TokType type = get_tok_type( token.Text, token.Length );

				if ( type == TokType::Invalid)
				{
					// Its most likely an identifier...
					type = TokType::Identifier;
				}

				token.Type = type;
				Tokens.append( token );
			}

			if ( Tokens.num() == 0 )
			{
				log_failure( "Failed to lex any tokens" );
				return { { nullptr }, 0 };
			}

			return { Tokens, 0 };
		#	undef current
		#	undef move_forward
		#	undef SkipWhitespace
		#	undef SkipWhitespace_Checked
		}
	}

#pragma region Helper Macros
#	define check_parse_args( func, def )                                           \
	if ( def.Len <= 0 )                                                            \
	{                                                                              \
		log_failure( "gen::" stringize(func) ": length must greater than 0" ); \
		return Code::Invalid;                                                      \
	}                                                                              \
	if ( def.Ptr == nullptr )                                                      \
	{                                                                              \
		log_failure( "gen::" stringize(func) ": def was null" );               \
		return Code::Invalid;                                                      \
	}

#	define nexttok 	    toks.next()
#	define currtok      toks.current()
#	define prevtok      toks.previous()
#	define eat( Type_ ) toks.__eat( Type_, context )
#	define left         ( toks.Arr.num() - toks.Idx )

#	define check( Type_ ) ( left && currtok.Type == Type_ )
#pragma endregion Helper Macros

	internal Code parse_function_body    ( Parser::TokArray& toks, char const* context );
	internal Code parse_global_nspace    ( Parser::TokArray& toks, char const* context );

	internal Code parse_class            ( Parser::TokArray& toks, char const* context );
	internal Code parse_enum             ( Parser::TokArray& toks, char const* context );
	internal Code parse_export_body      ( Parser::TokArray& toks, char const* context );
	internal Code parse_extern_link_body ( Parser::TokArray& toks, char const* context );
	internal Code parse_exten_link       ( Parser::TokArray& toks, char const* context );
	internal Code parse_friend           ( Parser::TokArray& toks, char const* context );
	internal Code parse_function         ( Parser::TokArray& toks, char const* context );
	internal Code parse_namespace        ( Parser::TokArray& toks, char const* context );
	internal Code parse_operator_cast    ( Parser::TokArray& toks, char const* context );
	internal Code parse_struct           ( Parser::TokArray& toks, char const* context );
	internal Code parse_variable         ( Parser::TokArray& toks, char const* context );
	internal Code parse_template         ( Parser::TokArray& toks, char const* context );
	internal Code parse_type             ( Parser::TokArray& toks, char const* context );
	internal Code parse_typedef          ( Parser::TokArray& toks, char const* context );
	internal Code parse_union            ( Parser::TokArray& toks, char const* context );
	internal Code parse_using            ( Parser::TokArray& toks, char const* context );

	internal inline
	Code parse_array_decl( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		if ( check( TokType::BraceSquare_Open ) )
		{
			eat( TokType::BraceSquare_Open );

			if ( left == 0 )
			{
				log_failure( "%s: Error, unexpected end of typedef definition ( '[]' scope started )", stringize(parse_typedef) );
				return Code::Invalid;
			}

			if ( currtok.Type == TokType::BraceSquare_Close )
			{
				log_failure( "%s: Error, empty array expression in typedef definition", stringize(parse_typedef) );
				return Code::Invalid;
			}

			Token untyped_tok = currtok;

			while ( left && currtok.Type != TokType::BraceSquare_Close )
			{
				eat( currtok.Type );
			}

			untyped_tok.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)untyped_tok.Text;

			Code array_expr = untyped_str( untyped_tok );

			if ( left == 0 )
			{
				log_failure( "%s: Error, unexpected end of type definition, expected ]", stringize(parse_typedef) );
				return Code::Invalid;
			}

			if ( currtok.Type != TokType::BraceSquare_Close )
			{
				log_failure( "%s: Error, expected ] in type definition, not %s", stringize(parse_typedef), str_tok_type( currtok.Type ) );
				return Code::Invalid;
			}

			eat( TokType::BraceSquare_Close );
			return array_expr;
		}

		return { nullptr };
	}

	internal inline
	Parser::Token parse_identifier( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		Token name = currtok;

		eat( TokType::Identifier );

		while ( check( TokType::Access_StaticSymbol ) )
		{
			eat( TokType::Access_StaticSymbol );

			if ( left == 0 )
			{
				log_failure( "%s: Error, unexpected end of type definition, expected identifier", context );
				return { nullptr, 0, TokType::Invalid };
			}

			if ( currtok.Type != TokType::Identifier )
			{
				log_failure( "%s: Error, expected identifier in type definition, not %s", context, str_tok_type( currtok.Type ) );
				return { nullptr, 0, TokType::Invalid };
			}

			name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
			eat( TokType::Identifier );
		}

		return name;
	}

	internal
	Code parse_params( Parser::TokArray& toks, char const* context, bool use_template_capture = false )
	{
		using namespace Parser;
		using namespace ECode;

		if ( ! use_template_capture )
			eat( TokType::Capture_Start );

		else
		{
			if ( check ( TokType::Operator ) && currtok.Text[0] == '<' )
				eat( TokType::Operator );
		}

		if ( ! use_template_capture &&  check(TokType::Capture_End) )
		{
			eat( TokType::Capture_End );
			return { nullptr };
		}

		Code type  = { nullptr };
		Code value = { nullptr };

		type = parse_type( toks, context );
		if ( type == Code::Invalid )
			return Code::Invalid;

		Token name = { nullptr, 0, TokType::Invalid, false };

		if ( check( TokType::Identifier )  )
		{
			name = currtok;
			eat( TokType::Identifier );

			if ( currtok.IsAssign )
			{
				eat( TokType::Operator );

				Token value_tok = currtok;

				if ( currtok.Type == TokType::Statement_End )
				{
					log_failure( "gen::%s: Expected value after assignment operator", context );
					return Code::Invalid;
				}

				while ( left && currtok.Type != TokType::Statement_End )
				{
					value_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)value_tok.Text;
					eat( currtok.Type );
				}

				value = parse_type( toks, context );
			}
		}

		Code
		result       = make_code();
		result->Type = Parameters;

		if ( name.Length > 0 )
			result->Name = get_cached_string( name );

		result->add_entry( type );

		if ( value )
			result->add_entry( value );

		while ( left
			&& use_template_capture ?
					currtok.Type != TokType::Operator && currtok.Text[0] !=  '>'
				:	currtok.Type != TokType::Capture_End )
		{
			eat( TokType::Comma );

			Code type  = { nullptr };
			Code value = { nullptr };

			type = parse_type( toks, context );
			if ( type == Code::Invalid )
				return Code::Invalid;

			name = { nullptr, 0, TokType::Invalid, false };

			if ( check( TokType::Identifier )  )
			{
				name = currtok;
				eat( TokType::Identifier );

				if ( currtok.IsAssign )
				{
					eat( TokType::Operator );

					Token value_tok = currtok;

					if ( currtok.Type == TokType::Statement_End )
					{
						log_failure( "gen::%s: Expected value after assignment operator", context );
						return Code::Invalid;
					}

					while ( left && currtok.Type != TokType::Statement_End )
					{
						value_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)value_tok.Text;
						eat( currtok.Type );
					}

					value = parse_type( toks, context );
				}
			}

			Code
			param       = make_code();
			param->Type = Parameters;

			if ( name.Length > 0 )
				param->Name = get_cached_string( name );

			param->add_entry( type );

			if ( value )
				param->add_entry( value );

			result->add_entry( param );
		}

		if ( ! use_template_capture )
			eat( TokType::Capture_End );

		else
		{
			if ( ! check( TokType::Operator) || currtok.Text[0] != '>' )
			{
				log_failure("gen::parse_template: expected '<' after 'template' keyword. %s", str_tok_type( currtok.Type ));
				return Code::Invalid;
			}
			eat( TokType::Operator );
		}

		return result;
	#	undef context
	}

	// Function parsing is handled in multiple places because its initial signature is shared with variable parsing
	internal inline
	Code parse_function_after_name(
		  ModuleFlag mflags
		, Code attributes
		, Code specifiers
		, Code ret_type
		, StrC name
		, Parser::TokArray& toks
		, char const* context
	)
	{
		using namespace Parser;

		Code params = parse_params( toks, stringize(parse_function) );

		Code body = { nullptr };
		if ( check( TokType::BraceCurly_Open ) )
		{
			body = parse_function_body( toks, stringize(parse_function) );
			if ( body == Code::Invalid )
				return Code::Invalid;
		}
		else
		{
			eat( TokType::Statement_End );
		}

		using namespace ECode;

		Code
		result              = make_code();
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		if ( body )
		{
			switch ( body->Type )
			{
				case Function_Body:
				case Untyped:
					break;

				default:
				{
					log_failure("gen::def_function: body must be either of Function_Body or Untyped type. %s", body->debug_str());
					return Code::Invalid;
				}
			}

			result->Type = Function;
			result->add_entry( body );
		}
		else
		{
			result->Type = Function_Fwd;
		}

		if ( specifiers )
			result->add_entry( specifiers );

		result->add_entry( ret_type );

		if ( params )
			result->add_entry( params );

		return result;
	}

	internal inline
	Code parse_operator_after_ret_type( ModuleFlag mflags, Code attributes, Code specifiers, Code ret_type, Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace EOperator;

		// Parse Operator
		eat( TokType::Decl_Operator );

		if ( ! check( TokType::Operator ) )
		{
			log_failure( "gen::%s: Expected operator after 'operator' keyword", context );
			return Code::Invalid;
		}

		OperatorT op = Invalid;
		switch ( currtok.Text[0] )
		{
			case '+':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_Add;

				else
					op = Add;
			}
			break;
			case '-':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_Subtract;

				else
					op = Subtract;
			}
			break;
			case '*':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_Multiply;

				else
				{
					Token& finder = prevtok;
					while ( finder.Type != TokType::Decl_Operator )
					{
						if ( finder.Type == TokType::Identifier)
						{
							op = Indirection;
							break;
						}
					}

					if ( op == Invalid)
						op = Multiply;
				}
			}
			break;
			case '/':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_Divide;

				else
					op = Divide;
			}
			break;
			case '%':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_Modulo;

				else
					op = Modulo;
			}
			break;
			case '&':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_BAnd;

				else if ( currtok.Text[1] == '&' )
					op = LAnd;

				else
				{


					if ( op == Invalid )
					op = BAnd;
				}
			}
			break;
			case '|':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_BOr;

				else if ( currtok.Text[1] == '|' )
					op = LOr;

				else
					op = BOr;
			}
			break;
			case '^':
			{
				if ( currtok.Text[1] == '=' )
					op = Assign_BXOr;

				else
					op = BXOr;
			}
			break;
			case '~':
			{
				op = BNot;
			}
			break;
			case '!':
			{
				if ( currtok.Text[1] == '=' )
					op = LNot;

				else
					op = UnaryNot;
			}
			break;
			case '=':
			{
				if ( currtok.Text[1] == '=' )
					op = LEqual;

				else
					op = Assign;
			}
			break;
			case '<':
			{
				if ( currtok.Text[1] == '=' )
					op = LEqual;

				else if ( currtok.Text[1] == '<' )
				{
					if ( currtok.Text[2] == '=' )
						op = Assign_LShift;

					else
						op = LShift;
				}
				else
					op = Lesser;
			}
			break;
			case '>':
			{
				if ( currtok.Text[1] == '=' )
					op = GreaterEqual;

				else if ( currtok.Text[1] == '>' )
				{
					if ( currtok.Text[2] == '=' )
						op = Assign_RShift;

					else
						op = RShift;
				}
				else
					op = Greater;
			}
			break;
			case '(':
			{
				if ( currtok.Text[1] == ')' )
					op = FunctionCall;

				else
					op = Invalid;
			}
			break;
			case '[':
			{
				if ( currtok.Text[1] == ']' )
					op = Subscript;

				else
					op = Invalid;
			}
			break;
			default:
			{
				break;
			}
		}

		if ( op == Invalid )
		{
			log_failure( "gen::%s: Invalid operator '%s'", context, currtok.Text );
			return Code::Invalid;
		}

		eat( TokType::Operator );

		// Parse Params
		Code params = parse_params( toks, stringize(parse_operator) );

		// Parse Body
		Code body = { nullptr };
		if ( check( TokType::BraceCurly_Open ) )
		{
			body = parse_function_body( toks, stringize(parse_function) );
			if ( body == Code::Invalid )
				return Code::Invalid;
		}
		else
		{
			eat( TokType::Statement_End );
		}

		// OpValidateResult check_result = operator__validate( op, params, ret_type, specifiers );
		Code result = def_operator( op, params, ret_type, body, specifiers, attributes, mflags );
		return result;
	}

	// Variable parsing is handled in multiple places because its initial signature is shared with function parsing
	internal inline
	Code parse_variable_after_name(
		  ModuleFlag mflags
		, Code attributes
		, Code specifiers
		, Code type
		, StrC name
		, Parser::TokArray& toks
		, char const* context )
	{
		using namespace Parser;

		Code array_expr = parse_array_decl( toks, stringize(parse_variable) );

		Code expr = Code::Invalid;

		if ( currtok.IsAssign )
		{
			eat( TokType::Operator );

			Token expr_tok = currtok;

			if ( currtok.Type == TokType::Statement_End )
			{
				log_failure( "gen::parse_variable: expected expression after assignment operator" );
				return Code::Invalid;
			}

			while ( left && currtok.Type != TokType::Statement_End )
			{
				eat( currtok.Type );
			}

			expr_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)expr_tok.Text;
			expr            = untyped_str( expr_tok );
		}

		eat( TokType::Statement_End );

		using namespace ECode;

		Code
		result              = make_code();
		result->Type        = Variable;
		result->Name        = get_cached_string( name );
		result->ModuleFlags = mflags;

		result->add_entry( type );

		if (array_expr.is_valid() )
			type->add_entry( array_expr );

		if ( attributes.is_valid() )
			result->add_entry( attributes );

		if (specifiers.is_valid() )
			result->add_entry( specifiers );

		if ( expr.is_valid() )
			result->add_entry( expr );

		return result;
	}

	internal inline
	Code parse_variable_assignment( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Code expr = Code::Invalid;

		if ( currtok.IsAssign )
		{
			eat( TokType::Operator );

			Token expr_tok = currtok;

			if ( currtok.Type == TokType::Statement_End )
			{
				log_failure( "gen::parse_variable: expected expression after assignment operator" );
				return Code::Invalid;
			}

			while ( left && currtok.Type != TokType::Statement_End )
			{
				expr_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)expr_tok.Text;
				eat( currtok.Type );
			}

			expr = untyped_str( expr_tok );
		}

		return expr;
	}

	internal inline
	Code parse_operator_function_or_variable( bool expects_function, Code attributes, Code specifiers, Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Code result = Code::Invalid;

		Code type = parse_type( toks, stringize(parse_variable) );

		if ( type == Code::Invalid )
			return Code::Invalid;

		if ( check( TokType::Operator) )
		{
			// Dealing with an operator overload
			result = parse_operator_after_ret_type( ModuleFlag::None, attributes, specifiers, type, toks, stringize(parse_template) );
		}
		else
		{
			StrC name = currtok;
			eat( TokType::Identifier );

			if ( check( TokType::Capture_Start) )
			{
				// Dealing with a function

				result = parse_function_after_name( ModuleFlag::None, attributes, specifiers, type, name, toks, stringize(parse_template) );
			}
			else
			{
				if ( expects_function )
				{
					log_failure( "gen::parse_template: expected function declaration (consteval was used)" );
					return Code::Invalid;
				}

				// Dealing with a variable
				result = parse_variable_after_name( ModuleFlag::None, attributes, specifiers, type, name, toks, stringize(parse_template) );
			}
		}

		return result;
	}

	internal
	Code parse_class_struct_body( Parser::TokType which, Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		eat( TokType::BraceCurly_Open );

		Code
		result = make_code();

		if ( which == TokType::Decl_Class )
			result->Type = Class_Body;

		else
			result->Type = Struct_Body;

		while ( left && currtok.Type != TokType::BraceCurly_Close )
		{
			Code member     = Code::Invalid;
			Code attributes = { nullptr };
			Code specifiers = { nullptr };

			bool expects_function = false;

			switch ( currtok.Type )
			{
				case TokType::Comment:
					member = def_comment( currtok );
					eat( TokType::Comment );
				break;

				case TokType::Access_Public:
					member = access_public;
					eat( TokType::Access_Public );
					eat( TokType::Assign_Classifer );
				break;

				case TokType::Access_Protected:
					member = access_protected;
					eat( TokType::Access_Protected );
					eat( TokType::Assign_Classifer );
				break;

				case TokType::Access_Private:
					member = access_private;
					eat( TokType::Access_Private );
					eat( TokType::Assign_Classifer );
				break;

				case TokType::Decl_Class:
					member = parse_class( toks, context );
				break;

				case TokType::Decl_Enum:
					member = parse_enum( toks, context );
				break;

				case TokType::Decl_Friend:
					member = parse_friend( toks, context );
				break;

				case TokType::Decl_Operator:
					member = parse_operator_cast( toks, context );
				break;

				case TokType::Decl_Struct:
					member = parse_struct( toks, context );
				break;

				case TokType::Decl_Template:
					member = parse_template( toks, context );
				break;

				case TokType::Decl_Typedef:
					member = parse_typedef( toks, context );
				break;

				case TokType::Decl_Union:
					member = parse_variable( toks, context );
				break;

				case TokType::Decl_Using:
					member = parse_using( toks, context );
				break;

				case TokType::BraceSquare_Open:
				case TokType::Attr_Keyword:
				{
					// Standard attribute
					if ( currtok.Type == TokType::BraceSquare_Open  )
					{
						eat( TokType::BraceSquare_Open );

						if ( currtok.Type != TokType::BraceSquare_Open )
						{
							log_failure( "%s: Error, expected attribute name", context );
							return result;
						}

						while ( left && currtok.Type != TokType::BraceSquare_Close )
						{
							// TODO : Parse attributes
						}

						eat( TokType::BraceSquare_Close );
						eat( TokType::BraceSquare_Close );
					}

					// Platform Specific attribute
					eat( TokType::Attr_Keyword );
					eat( TokType::Capture_Start );

					// TODO : Parse attributes

					eat( TokType::Capture_End );
				}
				//! Fallthrough intended
				case TokType::Spec_Consteval:
				case TokType::Spec_Constexpr:
				case TokType::Spec_Constinit:
				case TokType::Spec_Inline:
				case TokType::Spec_Mutable:
				case TokType::Spec_Static:
				case TokType::Spec_Volatile:
				{
					SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
					s32        num_specifiers = 0;

					while ( left && tok_is_specifier( currtok ) )
					{
						SpecifierT spec = ESpecifier::to_type( currtok );

						switch ( spec )
						{
							case ESpecifier::Constexpr:
							case ESpecifier::Constinit:
							case ESpecifier::Inline:
							case ESpecifier::Mutable:
							case ESpecifier::Static_Member:
							case ESpecifier::Volatile:
							break;

							case ESpecifier::Consteval:
								expects_function = true;
							break;

							default:
								log_failure( "gen::parse_class_struct_body: invalid specifier " "%s" " for variable", ESpecifier::to_str(spec) );
								return Code::Invalid;
						}

						specs_found[num_specifiers] = spec;
						num_specifiers++;
						eat( currtok.Type );
					}

					if ( num_specifiers )
					{
						specifiers = def_specifiers( num_specifiers, specs_found );
					}
				}
				//! Fallthrough intentional
				case TokType::Identifier:
				case TokType::Spec_Const:
				case TokType::Type_Unsigned:
				case TokType::Type_Signed:
				case TokType::Type_Short:
				case TokType::Type_Long:
				{
					member = parse_operator_function_or_variable( expects_function, attributes, specifiers, toks, context );
				}
				break;

				default:
					Token untyped_tok = currtok;

					while ( left && currtok.Type != TokType::BraceCurly_Close )
					{
						untyped_tok.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)untyped_tok.Text;
						eat( currtok.Type );
					}

					member = untyped_str( untyped_tok );
				break;
			}

			if ( member == Code::Invalid )
			{
				log_failure( "gen::parse_variable: failed to parse member" );
				return Code::Invalid;
			}

			result->add_entry( member );
		}

		eat( TokType::BraceCurly_Close );
		return result;
	}

	internal
	Code parse_class_struct( Parser::TokType which, Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		if ( which != TokType::Decl_Class && which != TokType::Decl_Struct )
		{
			log_failure( "%s: Error, expected class or struct, not %s", context, str_tok_type( which ) );
			return Code::Invalid;
		}

		Token name { nullptr, 0, TokType::Invalid };

		Code       parent     = { nullptr };
		Code       body       = { nullptr };
		Code       attributes = { nullptr };
		ModuleFlag mflags     = ModuleFlag::None;

		Code result = Code::Invalid;

		// TODO : Parse module specifiers

		eat( which );

		// TODO : Parse attributes

		name = parse_identifier( toks, context );

		AccessSpec access     = AccessSpec::Invalid;
		Token      parent_tok = { nullptr, 0, TokType::Invalid };

		if ( check( TokType::Assign_Classifer ) )
		{
			eat( TokType::Assign_Classifer );

			if ( tok_is_access_specifier( currtok ) )
			{
				access = tok_to_access_specifier( currtok );
			}

			parent_tok = parse_identifier( toks, context );
		}

		if ( check( TokType::BraceCurly_Open ) )
		{
			body = parse_class_struct_body( which, toks, context );
		}

		eat( TokType::Statement_End );

		if ( parent_tok )
			parent = def_type( parent_tok );

		if ( which == TokType::Decl_Class )
			result = def_class( name, body, parent, access
				// TODO : Set these up later
				, NoCode // Attributes
				, ModuleFlag::None
			);

		else
			result = def_struct( name, body, parent, access
				// TODO : Set these up later
				, NoCode // Attributes
				, ModuleFlag::None
			);

		return result;
	}

	internal
	Code parse_function_body( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		eat( TokType::BraceCurly_Open );

		Code
		result = make_code();
		result->Type = Function_Body;

		Token start = currtok;

		s32 level = 0;
		while ( left && ( currtok.Type != TokType::BraceCurly_Close || level > 0 ) )
		{
			if ( currtok.Type == TokType::BraceCurly_Open )
				level++;

			else if ( currtok.Type == TokType::BraceCurly_Close && level > 0 )
				level--;

			eat( currtok.Type );
		}

		Token previous = prevtok;

		s32 len = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)start.Text;

		if ( len > 0 )
		{
			result->add_entry( def_execution( { len, start.Text } ));
		}

		eat( TokType::BraceCurly_Close );
		return result;
	}

	internal
	Code parse_global_nspace( CodeT which, Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		if ( which != Namespace_Body && which != Global_Body && which != Export_Body && which != Extern_Linkage_Body )
			return Code::Invalid;

		if ( which != Global_Body )
			eat( TokType::BraceCurly_Open );

		Code
		result = make_code();
		result->Type = which;

		while ( left && currtok.Type != TokType::BraceCurly_Close )
		{
			Code member     = Code::Invalid;
			Code attributes = { nullptr };
			Code specifiers = { nullptr };

			bool expects_function = false;

			switch ( currtok.Type )
			{
				case TokType::Comment:
					member = def_comment( currtok );
					eat( TokType::Comment );
				break;

				case TokType::Decl_Enum:
					member = parse_enum( toks, context);
				break;

				case TokType::Decl_Class:
					member = parse_class( toks, context );
				break;

				case TokType::Decl_Extern_Linkage:
					if ( which == Extern_Linkage_Body )
						log_failure( "gen::parse_global_nspace: nested extern linkage" );

					member = parse_extern_link_body( toks, context );
				break;

				case TokType::Decl_Namespace:
					member = parse_namespace( toks, context );
				break;

				case TokType::Decl_Struct:
					member = parse_struct( toks, context );
				break;

				case TokType::Decl_Template:
					member = parse_template( toks, context );
				break;

				case TokType::Decl_Typedef:
					member = parse_typedef( toks, context );
				break;

				case TokType::Decl_Union:
					member = parse_union( toks, context );
				break;

				case TokType::Decl_Using:
					member = parse_using( toks, context );
				break;

				case TokType::Module_Export:
					if ( which == Export_Body )
						log_failure( "gen::parse_global_nspace: nested export declaration" );

					member = parse_export_body( toks, context );
				break;

				case TokType::Module_Import:
				{
					not_implemented();
				}
				//! Fallthrough intentional
				case TokType::BraceSquare_Open:
				case TokType::Attr_Keyword:
				{
					not_implemented();

					// Standard attribute
					if ( currtok.Type == TokType::BraceSquare_Open  )
					{
						eat( TokType::BraceSquare_Open );

						if ( currtok.Type != TokType::BraceSquare_Open )
						{
							log_failure( "%s: Error, expected attribute name", context );
							return result;
						}

						while ( left && currtok.Type != TokType::BraceSquare_Close )
						{
							// TODO : Parse attributes
						}

						eat( TokType::BraceSquare_Close );
						eat( TokType::BraceSquare_Close );
					}

					// Platform Specific attribute
					eat( TokType::Attr_Keyword );
					eat( TokType::Capture_Start );

					// TODO : Parse attributes

					eat( TokType::Capture_End );
				}
				//! Fallthrough intentional
				case TokType::Spec_Consteval:
				case TokType::Spec_Constexpr:
				case TokType::Spec_Constinit:
				case TokType::Spec_Extern:
				case TokType::Spec_Global:
				case TokType::Spec_Inline:
				case TokType::Spec_Internal_Linkage:
				case TokType::Spec_Static:
				{
					SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
					s32        num_specifiers = 0;

					while ( left && tok_is_specifier( currtok ) )
					{
						SpecifierT spec = ESpecifier::to_type( currtok );

						switch ( spec )
						{
							case ESpecifier::Constexpr:
							case ESpecifier::Constinit:
							case ESpecifier::Inline:
							case ESpecifier::Mutable:
							case ESpecifier::Static_Member:
							case ESpecifier::Volatile:
							break;

							case ESpecifier::Consteval:
								expects_function = true;
							break;

							default:
								log_failure( "gen::parse_global_nspace: invalid specifier " "%s" " for variable", ESpecifier::to_str(spec) );
								return Code::Invalid;
						}

						specs_found[num_specifiers] = spec;
						num_specifiers++;
						eat( currtok.Type );
					}

					if ( num_specifiers )
					{
						specifiers = def_specifiers( num_specifiers, specs_found );
					}
				}
				//! Fallthrough intentional
				case TokType::Identifier:
				case TokType::Spec_Const:
				case TokType::Type_Long:
				case TokType::Type_Short:
				case TokType::Type_Signed:
				case TokType::Type_Unsigned:
				{
					member = parse_operator_function_or_variable( expects_function, attributes, specifiers, toks, context );
				}
			}

			if ( member == Code::Invalid )
			{
				log_failure( "gen::%s: failed to parse extern linkage member", context );
				return Code::Invalid;
			}

			result->add_entry( member );
		}

		if ( which != Global_Body )
			eat( TokType::BraceCurly_Close );

		return result;
	}

	internal
	Code parse_class( Parser::TokArray& toks, char const* context )
	{
		return parse_class_struct( Parser::TokType::Decl_Class, toks, context );
	}

	Code parse_class( StrC def )
	{
		check_parse_args( parse_class, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_class_struct( TokType::Decl_Class, toks, stringize(parse_class) );
	}

	internal
	Code parse_enum( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Token name       = { nullptr, 0, TokType::Invalid };
		Code  array_expr = { nullptr };
		Code  type       = { nullptr };
		Token body       = { nullptr, 0, TokType::Invalid };

		char  entries_code[ kilobytes(128) ] { 0 };
		s32   entries_length = 0;

		bool is_enum_class = false;

		eat( TokType::Decl_Enum );

		if ( currtok.Type == TokType::Decl_Class )
		{
			eat( TokType::Decl_Class);
			is_enum_class = true;
		}

		// TODO : Parse attributes

		if ( currtok.Type != TokType::Identifier )
		{
			log_failure( "gen::parse_enum: expected identifier for enum name" );
			return Code::Invalid;
		}

		name = currtok;
		eat( TokType::Identifier );

		if ( currtok.Type == TokType::Assign_Classifer )
		{
			eat( TokType::Assign_Classifer );

			type = parse_type( toks, stringize(parse_enum) );
			if ( type == Code::Invalid )
				return Code::Invalid;
		}

		if ( currtok.Type == TokType::BraceCurly_Open )
		{
			eat( TokType::BraceCurly_Open );

			body = currtok;

			while ( currtok.Type != TokType::BraceCurly_Close )
			{
				eat( TokType::Identifier);

				if ( currtok.Type == TokType::Operator && currtok.Text[0] == '=' )
				{
					eat( TokType::Operator );

					while ( currtok.Type != TokType::Comma && currtok.Type != TokType::BraceCurly_Close )
					{
						eat( currtok.Type );
					}
				}

				if ( currtok.Type == TokType::Comma )
				{
					eat( TokType::Comma );
				}
			}

			body.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)body.Text;

			eat( TokType::BraceCurly_Close );
		}

		eat( TokType::Statement_End );

		using namespace ECode;

		Code
		result = make_code();

		if ( body.Length )
		{
			// mem_copy( entries_code, body.Text, body.Length );

			Code untyped_body = untyped_str( body );

			result->Type = is_enum_class ? Enum_Class : Enum;
			result->add_entry( untyped_body );
		}
		else
		{
			result->Type = is_enum_class ? Enum_Class_Fwd : Enum_Fwd;
		}

		result->Name = get_cached_string( name );

		if ( type )
			result->add_entry( type );

		return result;
	}

	Code parse_enum( StrC def )
	{
		check_parse_args( parse_enum, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_enum( toks, stringize(parse_enum) );
	}

	internal inline
	Code parse_export_body( Parser::TokArray& toks, char const* context )
	{
		return parse_global_nspace( ECode::Export_Body, toks, context );
	}

	Code parse_export_body( StrC def )
	{
		check_parse_args( parse_export_body, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_export_body( toks, stringize(parse_export_body) );
	}

	internal inline
	Code parse_extern_link_body( Parser::TokArray& toks, char const* context )
	{
		return parse_global_nspace( ECode::Extern_Linkage_Body, toks, context );
	}

	internal
	Code parse_extern_link( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		eat( TokType::Decl_Extern_Linkage );

		Token name = currtok;
		eat( TokType::String );

		name.Text   += 1;
		name.Length -= 1;

		Code
		result       = make_code();
		result->Type = ECode::Extern_Linkage;
		result->Name = get_cached_string( name );

		Code entry = parse_extern_link_body( toks, context );
		if ( entry == Code::Invalid )
		{
			log_failure( "gen::parse_extern_link: failed to parse body" );
			return result;
		}

		result->add_entry( entry );

		return result;
	}

	Code parse_extern_link( StrC def )
	{
		check_parse_args( parse_extern_link, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_extern_link( toks, stringize(parse_extern_link) );
	}

	internal
	Code parse_friend( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		using namespace ECode;

		eat( TokType::Decl_Friend );

		Code function = { nullptr };

		// Type declaration or return type
		Code type = parse_type( toks, stringize(parse_friend) );
		if ( type == Code::Invalid )
			return Code::Invalid;

		// Funciton declaration
		if ( currtok.Type == TokType::Identifier )
		{
			// Name
			Token name = parse_identifier( toks, stringize(parse_friend) );

			// Parameter list
			Code params = parse_params( toks, stringize(parse_friend) );

			function       = make_code();
			function->Type = Function_Fwd;
			function->Name = get_cached_string( name );
			function->add_entry( type );

			if ( params )
				function->add_entry( params );
		}

		eat( TokType::Statement_End );

		Code
		result       = make_code();
		result->Type = Friend;

		if ( function )
			result->add_entry( function );

		else
			result->add_entry( type );

		return result;
	}

	Code parse_friend( StrC def )
	{
		check_parse_args( parse_friend, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_friend( toks, stringize(parse_friend) );
	}

	internal
	Code parse_functon( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Code attributes = { nullptr };
		Code specifiers = { nullptr };

		// TODO : Parse module specifiers

		// TODO : Parse attributes

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			switch ( spec )
			{
				case ESpecifier::Const:
				case ESpecifier::Consteval:
				case ESpecifier::Constexpr:
				case ESpecifier::External_Linkage:
				case ESpecifier::Inline:
				case ESpecifier::Static_Member:
				break;

				default:
					log_failure( "gen::parse_functon: invalid specifier " "%s" " for functon", ESpecifier::to_str(spec) );
					return Code::Invalid;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( num_specifiers )
		{
			specifiers = def_specifiers( num_specifiers, specs_found );
		}

		Code ret_type = parse_type( toks, stringize(parse_function) );
		if ( ret_type == Code::Invalid )
			return Code::Invalid;

		Token name = parse_identifier( toks, stringize(parse_function) );
		if ( ! name )
			return Code::Invalid;

		Code result = parse_function_after_name( ModuleFlag::None, attributes, specifiers, ret_type, name, toks, context );

		return result;
	}

	Code parse_function( StrC def )
	{
		using namespace Parser;

		check_parse_args( parse_function, def );

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_functon( toks, stringize(parse_function) );
	}

	Code parse_global_body( StrC def )
	{
		check_parse_args( parse_global_body, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_global_nspace( ECode::Global_Body, toks, stringize(parse_global_body) );
	}

	internal
	Code parse_namespace( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;
		eat( TokType::Decl_Namespace );

		Token name = parse_identifier( toks, stringize(parse_namespace) );

		Code body = parse_global_nspace( ECode::Namespace_Body, toks, stringize(parse_namespace) );
		if ( body == Code::Invalid )
			return Code::Invalid;

		Code
		result = make_code();
		result->Type = ECode::Namespace;
		result->Name = get_cached_string( name );

		result->add_entry( body );

		return result;
	}

	Code parse_namespace( StrC def )
	{
		check_parse_args( parse_namespace, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_namespace( toks, stringize(parse_namespace) );
	}

	internal
	Code parse_operator( Parser::TokArray& toks, char const* context )
	{
		// TODO : Parse Module specifier

		// TODO : Parse Attributes
		Code attributes = { nullptr };

		Code specifiers = { nullptr };

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			switch ( spec )
			{
				case ESpecifier::Const:
				case ESpecifier::Constexpr:
				case ESpecifier::Inline:
				case ESpecifier::Static_Member:
				break;

				default:
					log_failure( "gen::parse_operator: invalid specifier " "%s" " for operator", ESpecifier::to_str(spec) );
					return Code::Invalid;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( num_specifiers )
		{
			specifiers = def_specifiers( num_specifiers, specs_found );
		}

		// Parse Return Type
		Code ret_type = parse_type( toks, stringize(parse_operator) );

		Code result = parse_operator_after_ret_type( ModuleFlag::None, attributes, specifiers, ret_type, toks, context );
		return result;
	}

	Code parse_operator( StrC def )
	{
		check_parse_args( parse_operator, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_operator( toks, stringize(parse_operator) );
	}

	Code parse_operator_cast( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		eat( TokType::Decl_Operator );

		Code type = parse_type( toks, stringize(parse_operator_cast) );

		eat( TokType::Capture_Start );
		eat( TokType::Capture_End );

		Code body = { nullptr };

		if ( check( TokType::BraceCurly_Open) )
		{
			eat( TokType::BraceCurly_Open );

			Token body_str = currtok;

			s32 level = 0;
			while ( left && ( currtok.Type != TokType::BraceCurly_Close || level > 0 ) )
			{
				if ( currtok.Type == TokType::BraceCurly_Open )
					level++;

				else if ( currtok.Type == TokType::BraceCurly_Close && level > 0 )
					level--;

				eat( currtok.Type );
			}

			body_str.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)body_str.Text;

			body = untyped_str( body_str );

			eat( TokType::BraceCurly_Close );
		}

		Code result = make_code();

		if (body)
		{
			result->Type = ECode::Operator_Cast;
			result->add_entry( body );
		}
		else
		{
			result->Type = ECode::Operator_Cast_Fwd;
		}

		result->add_entry( type );

		return result;
	}

	Code parse_operator_cast( StrC def )
	{
		check_parse_args( parse_operator_cast, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_operator_cast( toks, stringize(parse_operator_cast) );
	}

	internal inline
	Code parse_struct( Parser::TokArray& toks, char const* context )
	{
		return parse_class_struct( Parser::TokType::Decl_Struct, toks, stringize(parse_struct) );
	}

	Code parse_struct( StrC def )
	{
		check_parse_args( parse_struct, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_class_struct( TokType::Decl_Struct, toks, stringize(parse_struct) );
	}

	internal
	Code parse_template( Parser::TokArray& toks, char const* context )
	{
	#	define UseTemplateCapture true

		using namespace Parser;

		// TODO : Parse Module specifier

		eat( TokType::Decl_Template );

		Code params = parse_params( toks, stringize(parse_template), UseTemplateCapture );
		if ( params == Code::Invalid )
			return Code::Invalid;

		Code definition = { nullptr };

		while ( left )
		{
			if ( check( TokType::Decl_Class ) )
			{
				definition = parse_class( toks, stringize(parse_template) );
				break;
			}

			if ( check( TokType::Decl_Struct ) )
			{
				definition = parse_enum( toks, stringize(parse_template) );
				break;
			}

			if ( check( TokType::Decl_Using ))
			{
				definition = parse_using( toks, stringize(parse_template) );
				break;
			}

			// Its either a function or a variable
			Token name = { nullptr, 0, TokType::Invalid };


			Code attributes   = { nullptr };
			Code specifiers   = { nullptr };

			// TODO : Parse attributes

			bool expects_function = false;

			SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
			s32        num_specifiers = 0;

			while ( left && tok_is_specifier( currtok ) )
			{
				SpecifierT spec = ESpecifier::to_type( currtok );

				switch ( spec )
				{
					case ESpecifier::Const:
					case ESpecifier::Constexpr:
					case ESpecifier::Constinit:
					case ESpecifier::External_Linkage:
					case ESpecifier::Global:
					case ESpecifier::Inline:
					case ESpecifier::Local_Persist:
					case ESpecifier::Mutable:
					case ESpecifier::Static_Member:
					case ESpecifier::Thread_Local:
					case ESpecifier::Volatile:
					break;

					case ESpecifier::Consteval:
						expects_function = true;
					break;

					default:
						log_failure( "gen::parse_template: invalid specifier %s for variable or function", ESpecifier::to_str( spec ) );
						return Code::Invalid;
				}

				// Ignore const it will be handled by the type
				if ( spec == ESpecifier::Const )
					continue;

				specs_found[num_specifiers] = spec;
				num_specifiers++;
				eat( currtok.Type );
			}

			if ( num_specifiers )
			{
				specifiers = def_specifiers( num_specifiers, specs_found );
			}

			definition = parse_operator_function_or_variable( expects_function, attributes, specifiers, toks, stringize(parse_template) );
			break;
		}

		Code
		result = make_code();
		result->Type = ECode::Template;
		result->add_entry( definition );
		result->add_entry( params );

		return result;
	#	undef UseTemplateCapture
	}

	Code parse_template( StrC def )
	{
		check_parse_args( parse_template, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_template( toks, stringize(parse_template) );
	}

	internal
	Code parse_type( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Token context_tok = prevtok;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Token name      = { nullptr, 0, TokType::Invalid };
		Token brute_sig = { currtok.Text, 0, TokType::Invalid };

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			if ( spec != ESpecifier::Const )
			{
				log_failure( "gen::parse_type: Error, invalid specifier used in type definition: %s", currtok.Text );
				return Code::Invalid;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( left == 0 )
		{
			log_failure( "%s: Error, unexpected end of type definition", context );
			return Code::Invalid;
		}

		if (   currtok.Type == TokType::Decl_Class
			|| currtok.Type == TokType::Decl_Struct )
		{
			name = currtok;
			eat( currtok.Type );

			name.Length = ( (sptr)currtok.Text + currtok.Length ) - (sptr)name.Text;
			eat( TokType::Identifier );
		}
		else if ( currtok.Type >= TokType::Type_Unsigned )
		{
			name = currtok;
			eat( currtok.Type );

			while (currtok.Type >= TokType::Type_Unsigned)
			{
				eat( currtok.Type );
			}

			name.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)name.Text;
		}
		else
		{
			name = parse_identifier( toks, context );
			if ( ! name )
				return Code::Invalid;

			// Problably dealing with a templated symbol
			if ( currtok.Type == TokType::Operator && currtok.Text[0] == '<' && currtok.Length == 1 )
			{
				eat( TokType::Operator );

				s32 level = 0;
				while ( left && ( currtok.Text[0] != '>' || level > 0 ))
				{
					if ( currtok.Text[0] == '<' )
						level++;

					if ( currtok.Text[0] == '>' )
						level--;

					eat( currtok.Type );
				}

				eat( TokType::Operator );

				// Extend length of name to last token
				name.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)name.Text;
			}
		}

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			if (   spec != ESpecifier::Const
				&& spec != ESpecifier::Ptr
				&& spec != ESpecifier::Ref
				&& spec != ESpecifier::RValue )
			{
				log_failure( "%s: Error, invalid specifier used in type definition: %s", context, currtok.Text );
				return Code::Invalid;
			}

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		// Not sure if its technically possible to cast ot a function pointer user defined operator cast...
		// Supporting it is not worth the effort.
		if ( check( TokType::Capture_Start ) && context_tok.Type != TokType::Decl_Operator )
		{
			// Its a function type
			eat( TokType::Capture_Start );

			while ( check( TokType::Star ) || currtok.Type == TokType::Spec_Const )
			{
				eat( currtok.Type );
			}

			// if its a using statement there will not be an ID.
			if ( check( TokType::Identifier ) )
				eat(TokType::Identifier);

			eat( TokType::Capture_End );

			// Parameters

			eat( TokType::Capture_Start );

			// TODO : Change this to validate the parameters...
			// Bruteforce lex the parameters, no validation.
			s32 level = 0;
			while ( ! check( TokType::Capture_End ) || level > 0 )
			{
				if ( check( TokType::Capture_Start ) )
					level++;

				if ( check( TokType::Capture_End ) )
					level--;

				eat( currtok.Type );
			}

			eat(TokType::Capture_End);

			brute_sig.Length = ( (sptr)prevtok.Text + prevtok.Length ) - (sptr)brute_sig.Text;
		}

		using namespace ECode;

		Code
		result       = make_code();
		result->Type = Typename;

		if ( brute_sig.Length > 0 )
		{
			// Bruteforce all tokens together.
			name = brute_sig;
		}
		else
		{
			if (num_specifiers)
			{
				Code specifiers = def_specifiers( num_specifiers, (SpecifierT*)specs_found );
				result->add_entry( specifiers );
			}
		}

		result->Name = get_cached_string( name );

		return result;
	}

	Code parse_type( StrC def )
	{
		check_parse_args( parse_type, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		Code result = parse_type( toks, stringize(parse_type) );

		return result;
	}

	internal
	Code parse_typedef( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Token name       = { nullptr, 0, TokType::Invalid };
		Code  array_expr = { nullptr };
		Code  type       = { nullptr };

		eat( TokType::Decl_Typedef );

		type = parse_type( toks, stringize(parse_typedef) );

		if ( ! check( TokType::Identifier ) )
		{
			log_failure( "gen::parse_typedef: Error, expected identifier for typedef" );
			return Code::Invalid;
		}

		name = currtok;
		eat( TokType::Identifier );

		array_expr = parse_array_decl( toks, stringize(parse_typedef) );

		eat( TokType::Statement_End );

		using namespace ECode;

		Code
		result       = make_code();
		result->Type = Typedef;
		result->Name = get_cached_string( name );

		result->add_entry( type );

		if ( array_expr && array_expr->Type != Invalid )
			type->add_entry( array_expr );

		return result;
	}

	Code parse_typedef( StrC def )
	{
		check_parse_args( parse_typedef, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_typedef( toks, stringize(parse_typedef) );
	}

	internal
	Code parse_union( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		// TODO : Parse module spec

		eat( TokType::Decl_Union );

		// TODO : Parse attributes
		Code attributes = { nullptr };

		StrC name = { 0, nullptr };

		if ( check( TokType::Identifier ) )
		{
			name = currtok;
			eat( TokType::Identifier );
		}

		Code body = { nullptr };

		eat( TokType::BraceCurly_Open );

		body = make_code();
		body->Type = ECode::Union_Body;

		while ( ! check( TokType::BraceCurly_Close ) )
		{
			Code entry = parse_variable( toks, stringize(parse_union) );

			if ( entry )
				body->add_entry( entry );
		}

		eat( TokType::BraceCurly_Close );
		eat( TokType::Statement_End );

		Code
		result = make_code();
		result->Type = ECode::Union;

		if ( name )
			result->Name = get_cached_string( name );

		if ( body )
			result->add_entry( body );

		if ( attributes )
			result->add_entry( attributes );

		return result;
	}

	Code parse_union( StrC def )
	{
		check_parse_args( parse_union, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_union( toks, stringize(parse_union) );
	}

	internal
	Code parse_using( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Token  name       = { nullptr, 0, TokType::Invalid };
		Code   array_expr = { nullptr };
		Code   type       = { nullptr };

		bool is_namespace = false;

		// TODO : Parse module specs

		eat( TokType::Decl_Using );

		if ( currtok.Type == TokType::Decl_Namespace )
		{
			is_namespace = true;
			eat( TokType::Decl_Namespace );
		}

		name = currtok;
		eat( TokType::Identifier );

		if ( currtok.IsAssign )
		{
			// TODO : Parse Attributes (using type-alias)

			eat( TokType::Operator );

			type = parse_type( toks, stringize(parse_typedef) );
		}

		array_expr = parse_array_decl( toks, stringize(parse_typedef) );

		eat( TokType::Statement_End );

		using namespace ECode;

		Code
		result       = make_code();
		result->Type = is_namespace ? Using_Namespace : Using;
		result->Name = get_cached_string( name );

		if ( type.is_valid() )
			result->add_entry( type );

		if ( array_expr.is_valid() )
			type->add_entry( array_expr );

		return result;
	}

	Code parse_using( StrC def )
	{
		check_parse_args( parse_using, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_using( toks, stringize(parse_using) );
	}

	internal
	Code parse_variable( Parser::TokArray& toks, char const* context )
	{
		using namespace Parser;

		Token name = { nullptr, 0, TokType::Invalid };

		SpecifierT specs_found[16] { ESpecifier::Num_Specifiers };
		s32        num_specifiers = 0;

		Code attributes = Code::Invalid;
		Code specifiers = Code::Invalid;

		// TODO : Parse module specifiers

		// TODO : Parse attributes

		while ( left && tok_is_specifier( currtok ) )
		{
			SpecifierT spec = ESpecifier::to_type( currtok );

			switch ( spec )
			{
				case ESpecifier::Const:
				case ESpecifier::Constexpr:
				case ESpecifier::Constinit:
				case ESpecifier::External_Linkage:
				case ESpecifier::Global:
				case ESpecifier::Inline:
				case ESpecifier::Local_Persist:
				case ESpecifier::Mutable:
				case ESpecifier::Static_Member:
				case ESpecifier::Thread_Local:
				case ESpecifier::Volatile:
				break;

				default:
					log_failure( "gen::parse_variable: invalid specifier %s for variable", ESpecifier::to_str( spec ) );
					return Code::Invalid;
			}

			// Ignore const specifiers, they're handled by the type
			if ( spec == ESpecifier::Const )
				continue;

			specs_found[num_specifiers] = spec;
			num_specifiers++;
			eat( currtok.Type );
		}

		if ( num_specifiers )
		{
			specifiers = def_specifiers( num_specifiers, specs_found );
		}

		Code type = parse_type( toks, stringize(parse_variable) );

		if ( type == Code::Invalid )
			return Code::Invalid;

		name = currtok;
		eat( TokType::Identifier );

		Code result = parse_variable_after_name( ModuleFlag::None, attributes, specifiers, type, name, toks, stringize(parse_variable) );

		return result;
	}

	Code parse_variable( StrC def )
	{
		check_parse_args( parse_variable, def );
		using namespace Parser;

		TokArray toks = lex( def );
		if ( toks.Arr == nullptr )
			return Code::Invalid;

		return parse_variable( toks, stringize(parse_variable) );
	}

	// Undef helper macros
#	undef check_parse_args
#	undef curr_tok
#	undef eat
#	undef left
//	End GEN_FEATURE_PARSING
#endif
#pragma endregion Parsing Constructors

#pragma region Untyped Constructors
	sw token_fmt_va( char* buf, uw buf_size, s32 num_tokens, va_list va )
	{
		char const* buf_begin = buf;
		sw          remaining = buf_size;

		HashTable<StrC> tok_map;
		{
			// TODO : Switch this to use an arena that makes use of the stack (cap the size of the token table to around 4096 bytes)
			tok_map = HashTable<StrC>::init( Memory::GlobalAllocator );

			s32 left = num_tokens - 1;

			while ( left-- )
			{
				char const* token = va_arg( va, char const* );
				StrC        value = va_arg( va, StrC );

				u32 key = crc32( token, str_len(token) );

				tok_map.set( key, value );
			}
		}

		char const* fmt     = va_arg( va, char const* );
		char        current = *fmt;

		while ( current )
		{
			sw len = 0;

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
				StrC*     value = tok_map.get( key );

				if ( value )
				{
					sw          left = value->Len;
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

		tok_map.clear();

		sw result = buf_size - remaining;

		return result;
	}

	Code untyped_str( StrC content )
	{
		Code
		result          = make_code();
		result->Name    = get_cached_string( content );
		result->Type    = ECode::Untyped;
		result->Content = result->Name;

		return result;
	}

	Code untyped_fmt( char const* fmt, ...)
	{
		local_persist thread_local
		char buf[GEN_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		sw length = str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va);
		va_end(va);

		Code
		result          = make_code();
		result->Name    = get_cached_string( { str_len(fmt, MaxNameLength), fmt } );
		result->Type    = ECode::Untyped;
		result->Content = get_cached_string( { length, buf } );

		return result;
	}

	Code untyped_token_fmt( s32 num_tokens, ... )
	{
		local_persist thread_local
		char buf[GEN_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, num_tokens);
		sw length = token_fmt_va(buf, GEN_PRINTF_MAXLEN, num_tokens, va);
		va_end(va);

		Code
		result          = make_code();
		result->Name    = get_cached_string( { length, buf } );
		result->Type    = ECode::Untyped;
		result->Content = result->Name;

		return result;
	}
#pragma endregion Untyped Constructors
#pragma endregion Gen Interface

#pragma region Builder
	void Builder::print( Code code )
	{
		Buffer.append_fmt( "%s\n", code->to_string() );
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

		Buffer = String::make( Memory::GlobalAllocator, "" );

		return true;
	}

	void Builder::write()
	{
		bool result = file_write( & File, Buffer, Buffer.length() );

		if ( result == false )
			log_failure("gen::File::write - Failed to write to file: %s", file_name( & File ) );

		file_close( & File );
	}
#pragma endregion Builder

#pragma region Editor
#ifdef GEN_FEATURE_EDITOR
#endif
#pragma endregion Editor

#pragma region Scanner
#ifdef GEN_FEATURE_SCANNER
#endif
#pragma endregion Scanner
}
// End: gen_time
#endif
