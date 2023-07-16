#include "gen_dep.hpp"

#ifdef gen_time

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

// gen_time
#endif