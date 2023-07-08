/*
	BLOAT.

	This contians all definitions not directly related to the project.
*/
#pragma once

#ifdef BLOAT_IMPL
#	define ZPL_IMPLEMENTATION
#endif

// TODO: This will be removed when making the library have zero dependencies.
#pragma region 									ZPL INCLUDE
#if __clang__
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wmissing-braces"
#	pragma clang diagnostic ignored "-Wbraced-scalar-init"
#endif

// #   define ZPL_HEAP_ANALYSIS
#	define ZPL_WRAP_IN_NAMESPACE
#	define ZPL_NO_MATH_H
#	define ZPL_CUSTOM_MODULES
#		define ZPL_MODULE_ESSENTIALS
#		define ZPL_MODULE_CORE
#		define ZPL_MODULE_TIMER
#		define ZPL_MODULE_HASHING
// #		define ZPL_MODULE_REGEX
// #		define ZPL_MODULE_EVENT
// #		define ZPL_MODULE_DLL
// #		define ZPL_MODULE_OPTS
// #		define ZPL_MODULE_PROCESS
// #		define ZPL_MODULE_MAT
// #		define ZPL_MODULE_THREADING
// #		define ZPL_MODULE_JOBS
// #		define ZPL_MODULE_PARSER
#include "zpl.h"

using zpl::b32;
using zpl::s8;
using zpl::s16;
using zpl::s32;
using zpl::s64;
using zpl::u8;
using zpl::u32;
using zpl::u64;
using zpl::uw;
using zpl::sw;
using zpl::sptr;
using zpl::uptr;

using zpl::Arena;
using zpl::AllocatorInfo;
using zpl::ArrayHeader;
using zpl::FileInfo;
using zpl::FileError;
using zpl::Pool;
// using zpl::String;

using zpl::EFileMode_WRITE;
using zpl::EFileError_NONE;

using zpl::alloc;
using zpl::arena_allocator;
using zpl::arena_init_from_memory;
using zpl::arena_init_from_allocator;
using zpl::arena_free;
using zpl::assert_crash;
using zpl::str_fmt_buf;
using zpl::char_first_occurence;
using zpl::char_is_alpha;
using zpl::char_is_alphanumeric;
using zpl::char_is_space;
using zpl::crc32;
using zpl::free_all;
using zpl::mem_copy;
using zpl::mem_move;
using zpl::mem_set;
using zpl::pool_allocator;
using zpl::pool_init;
using zpl::pool_free;
using zpl::process_exit;
using zpl::str_copy;
using zpl::str_fmt_va;
using zpl::str_fmt_out_va;
using zpl::str_fmt_out_err_va;
using zpl::str_compare;
using zpl::str_len;

#if __clang__
#	pragma clang diagnostic pop
#endif
#pragma endregion 								ZPL INCLUDE


#if __clang__
#	pragma clang diagnostic ignored "-Wunused-const-variable"
#	pragma clang diagnostic ignored "-Wswitch"
#	pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wunknown-pragmas"
#	pragma clang diagnostic ignored "-Wvarargs"
#	pragma clang diagnostic ignored "-Wunused-function"
#endif


#include "Banned.define.hpp"


#if defined(__GNUC__) || defined(__clang__)
	// Supports 0-10 arguments
	#define macro_num_args_impl( _0,                      \
		_1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10,  \
		_11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
		N, ...                                            \
	) N
	// _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
	// _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
	// _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,

	// ## deletes preceding comma if _VA_ARGS__ is empty (GCC, Clang)
	#define macro_num_args(...)                 \
 	macro_num_args_impl(_, ## __VA_ARGS__,      \
		20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
		10,  9,  8,  7,  6,  5,  4,  3,  2,  1, \
		0                                       \
	)
  	// 50, 49, 48, 47, 46, 45, 44, 43, 42, 41,              \
  	// 40, 39, 38, 37, 36, 35, 34, 33, 32, 31,              \
  	// 30, 29, 28, 27, 26, 25, 24, 23, 22, 21,
#else
	// Supports 1-10 arguments
  	#define macro_num_args_impl(                          \
  		 _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
		_11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
 		 N, ...                                           \
	) N

	#define macro_num_args(...)                 \
	macro_num_args_impl( __VA_ARGS__,           \
		20, 19, 18, 17, 16, 15, 14, 13, 12, 11, \
		10,  9,  8,  7,  6,  5,  4,  3,  2,  1  \
	)
#endif

#define macro_expand( Expanded_ ) Expanded_

#define bit( Value_ )                             ( 1 << Value_ )
#define bitfield_is_equal( Type_, Field_, Mask_ ) ( (Type_(Mask_) & Type_(Field_)) == Type_(Mask_) )
#define forceinline                               ZPL_ALWAYS_INLINE
#define print_nl( _)                              zpl_printf("\n")
#define ccast( Type_, Value_ )                    * const_cast< Type_* >( & (Value_) )
#define scast( Type_, Value_ )			          static_cast< Type_ >( Value_ )
#define rcast( Type_, Value_ )			          reinterpret_cast< Type_ >( Value_ )
#define pcast( Type_, Value_ )                    ( * (Type_*)( & (Value_) ) )
#define txt_impl( ... )                           #__VA_ARGS__
#define txt( ... )                                txt_impl( __VA_ARGS__ )
#define txt_n_len( ... )		                  sizeof( txt_impl( __VA_ARGS__ ) ), txt_impl( __VA_ARGS__ )
#define do_once()      \
do                     \
{                      \
	static             \
	bool Done = false; \
	if ( Done )        \
		return;        \
	Done = true;       \
}                      \
while(0)

#define do_once_start  \
do                     \
{                      \
	static             \
	bool Done = false; \
	if ( Done )        \
		break;         \
	Done = true;

#define do_once_end    \
}                      \
while(0);

constexpr
char const* Msg_Invalid_Value = "INVALID VALUE PROVIDED";


#pragma region Memory
#pragma endregion Memory

#pragma region String
	// Constant string with length.
	struct StrC
	{
		sw          Len;
		char const* Ptr;

		static constexpr
		StrC from( char const* str )
		{
			return { str_len( str ), str };
		}

		operator char const* () const
		{
			return Ptr;
		}
	};

	// Dynamic String
	// This is directly based off the ZPL string api.
	// They used a header pattern
	// I kept it for simplicty of porting but its not necessary to keep it that way.
	struct String
	{
		struct Header
		{
			AllocatorInfo Allocator;
			sw            Length;
			sw            Capacity;
		};

		static String make( AllocatorInfo allocator, char const* str )
		{
			sw length = str ? str_len( str ) : 0;
			return make_length( allocator, str, length );
		}

		static String make( AllocatorInfo allocator, StrC str )
		{
			return make_length( allocator, str.Ptr, str.Len );
		}

		static String make_reserve( AllocatorInfo allocator, sw capacity )
		{
			constexpr sw header_size = sizeof( Header );

			s32   alloc_size = header_size + capacity + 1;
			void* allocation = alloc( allocator, alloc_size );

			if ( allocation == nullptr )
				return { nullptr };

			mem_set( allocation, 0, alloc_size );

			Header*
			header            = rcast(Header*, allocation);
			header->Allocator = allocator;
			header->Capacity  = capacity;
			header->Length    = 0;

			String result = { (char*)allocation + header_size };
			return result;
		}

		static String make_length( AllocatorInfo allocator, char const* str, sw length )
		{
			constexpr sw header_size = sizeof( Header );

			s32   alloc_size = header_size + length + 1;
			void* allocation = alloc( allocator, alloc_size );

			if ( allocation == nullptr )
				return { nullptr };

			if ( ! str )
				mem_set( allocation, 0, alloc_size );

			Header&
			header = * rcast(Header*, allocation);
			header = { allocator, length, length };

			String  result = { rcast( char*, allocation) + header_size };

			if ( length && str )
				mem_copy( result, str, length );

			result[ length ] = '\0';

			return result;
		}

		static String fmt( AllocatorInfo allocator, char* buf, sw buf_size, char const* fmt, ... )
		{
			va_list va;
			va_start( va, fmt );
			str_fmt_va( buf, buf_size, fmt, va );
			va_end( va );

			return make( allocator, buf );
		}

		static String fmt_buf( AllocatorInfo allocator, char const* fmt, ... )
		{
			local_persist thread_local
			char buf[ ZPL_PRINTF_MAXLEN ] = { 0 };

			va_list va;
			va_start( va, fmt );
			str_fmt_va( buf, ZPL_PRINTF_MAXLEN, fmt, va );
			va_end( va );

			return make( allocator, buf );
		}

		static String join( AllocatorInfo allocator, char const** parts, sw num_parts, char const* glue )
		{
			String result = make( allocator, "" );

			for ( sw idx = 0; idx < num_parts; ++idx )
			{
				result.append( parts[ idx ] );

				if ( idx < num_parts - 1 )
					result.append( glue );
			}

			return result;
		}

		static bool are_equal( String lhs, String rhs )
		{
			if ( lhs.length() != rhs.length() )
				return false;

			for ( sw idx = 0; idx < lhs.length(); ++idx )
				if ( lhs[ idx ] != rhs[ idx ] )
					return false;

			return true;
		}

		bool make_space_for( char const* str, sw add_len )
		{
			sw available = avail_space();

			// NOTE: Return if there is enough space left
			if ( available >= add_len )
			{
				return true;
			}
			else
			{
				sw new_len, old_size, new_size;

				void* ptr;
				void* new_ptr;

				AllocatorInfo allocator = get_header().Allocator;
				Header*       header	= nullptr;

				new_len  = length() + add_len;
				ptr      = & get_header();
				old_size = size_of( Header ) + length() + 1;
				new_size = size_of( Header ) + new_len + 1;

				new_ptr = resize( allocator, ptr, old_size, new_size );

				if ( new_ptr == nullptr )
					return false;

				header            = zpl_cast( Header* ) new_ptr;
				header->Allocator = allocator;
				header->Capacity  = new_len;

				Data = rcast( char*, header + 1 );

				return str;
			}
		}

		bool append( char const* str )
		{
			return append( str, str_len( str ) );
		}

		bool append( char const* str, sw length )
		{
			if ( sptr(str) > 0 )
			{
				sw curr_len = this->length();

				if ( ! make_space_for( str, length ) )
					return false;

				Header& header = get_header();

				mem_copy( Data + curr_len, str, length );

				Data[ curr_len + length ] = '\0';

				header.Length = curr_len + length;
			}

			return str;
		}

		bool append( StrC str)
		{
			return append( str.Ptr, str.Len );
		}

		bool append( const String other )
		{
			return append( other.Data, other.length() );
		}

		bool append_fmt( char const* fmt, ... )
		{
			sw   res;
			char buf[ ZPL_PRINTF_MAXLEN ] = { 0 };

			va_list va;
			va_start( va, fmt );
			res = str_fmt_va( buf, count_of( buf ) - 1, fmt, va ) - 1;
			va_end( va );

			return append( buf, res );
		}

		sw avail_space() const
		{
			Header const&
			header = * rcast( Header const*, Data - sizeof( Header ));

			return header.Capacity - header.Length;
		}

		sw capacity() const
		{
			Header const&
			header = * rcast( Header const*, Data - sizeof( Header ));

			return header.Capacity;
		}

		void clear()
		{
			get_header().Length = 0;
		}

		String duplicate( AllocatorInfo allocator )
		{
			return make_length( allocator, Data, length() );
		}

		void free()
		{
			if ( ! Data )
				return;

			Header& header = get_header();

			zpl::free( header.Allocator, & header );
		}

		Header& get_header()
		{
			return *(Header*)(Data - sizeof(Header));
		}

		sw length() const
		{
			Header const&
			header = * rcast( Header const*, Data - sizeof( Header ));

			return header.Length;
		}

		void trim( char const* cut_set )
		{
			sw len = 0;

			char* start_pos = Data;
			char* end_pos   = Data + length() - 1;

			while ( start_pos <= end_pos && char_first_occurence( cut_set, *start_pos ) )
				start_pos++;

			while ( end_pos > start_pos && char_first_occurence( cut_set, *end_pos ) )
				end_pos--;

			len = scast( sw, ( start_pos > end_pos ) ? 0 : ( ( end_pos - start_pos ) + 1 ) );

			if ( Data != start_pos )
				mem_move( Data, start_pos, len );

			Data[ len ] = '\0';

			get_header().Length = len;
		}

		void trim_space()
		{
			return trim( " \t\r\n\v\f" );
		}

		operator bool()
		{
			return Data;
		}

		operator char* ()
		{
			return Data;
		}

		operator char const* () const
		{
			return Data;
		}

		operator StrC() const
		{
			return
			{
				length(),
				Data
			};
		}

		String const& operator = ( String const& other ) const
		{
			if ( this == & other )
				return *this;

			String& this_ = ccast( String, *this );

			this_.Data = other.Data;

			return this_;
		}

		String& operator += ( String const& other )
		{
			append( other );
			return *this;
		}

		char& operator [] ( sw index )
		{
			return Data[ index ];
		}

		char const& operator [] ( sw index ) const
		{
			return Data[ index ];
		}


		char* Data = nullptr;
	};

	struct String_POD
	{
		char* Data;

		operator String()
		{
			return * rcast(String*, this);
		}
	};
	static_assert( sizeof( String_POD ) == sizeof( String ), "String is not a POD" );
#pragma endregion String


namespace Memory
{
	constexpr uw Initial_Reserve = megabytes(10);

	extern Arena Global_Arena;
	// #define g_allocator arena_allocator( & Memory::Global_Arena)

	// Heap allocator is being used for now to isolate errors from being memory related (tech debt till ready to address)
	#define g_allocator heap()

	void setup();
	void resize( uw new_size );
	void cleanup();
}

inline
sw log_fmt(char const* fmt, ...)
{
	sw res;
	va_list va;

	va_start(va, fmt);
	res = str_fmt_out_va(fmt, va);
	va_end(va);

	return res;
}

inline
sw fatal(char const* fmt, ...)
{
	local_persist thread_local
	char buf[ZPL_PRINTF_MAXLEN] = { 0 };

	va_list va;

#if Build_Debug
	va_start(va, fmt);
	str_fmt_va(buf, ZPL_PRINTF_MAXLEN, fmt, va);
	va_end(va);

	assert_crash(buf);
	return -1;
#else
	va_start(va, fmt);
	str_fmt_out_err_va( fmt, va);
	va_end(va);

	exit(1);
	return -1;
#endif
}
