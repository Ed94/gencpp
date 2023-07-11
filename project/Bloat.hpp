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

using zpl::AllocType;
using zpl::Arena;
using zpl::AllocatorInfo;
using zpl::ArrayHeader;
using zpl::FileInfo;
using zpl::FileError;
using zpl::Pool;
// using zpl::String;

using zpl::EAllocation_ALLOC;
using zpl::EAllocation_FREE;
using zpl::EAllocation_FREE_ALL;
using zpl::EAllocation_RESIZE;
using zpl::EFileMode_WRITE;
using zpl::EFileError_NONE;

using zpl::ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO;

using zpl::align_forward;
using zpl::align_forward_i64;
using zpl::alloc;
using zpl::alloc_align;
using zpl::arena_allocator;
using zpl::arena_init_from_memory;
using zpl::arena_init_from_allocator;
using zpl::arena_free;
using zpl::assert_crash;
using zpl::char_first_occurence;
using zpl::char_is_alpha;
using zpl::char_is_alphanumeric;
using zpl::char_is_digit;
using zpl::char_is_hex_digit;
using zpl::char_is_space;
using zpl::crc32;
using zpl::free_all;
using zpl::is_power_of_two;
using zpl::mem_copy;
using zpl::mem_move;
using zpl::mem_set;
using zpl::pointer_add;
using zpl::pool_allocator;
using zpl::pool_init;
using zpl::pool_free;
using zpl::process_exit;
using zpl::str_compare;
using zpl::str_copy;
using zpl::str_fmt_buf;
using zpl::str_fmt_va;
using zpl::str_fmt_out_va;
using zpl::str_fmt_out_err_va;
using zpl::str_len;
using zpl::zero_size;

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
#define ccast( Type_, Value_ )                    * const_cast< Type_* >( & (Value_) )
#define scast( Type_, Value_ )			          static_cast< Type_ >( Value_ )
#define rcast( Type_, Value_ )			          reinterpret_cast< Type_ >( Value_ )
#define pcast( Type_, Value_ )                    ( * (Type_*)( & (Value_) ) )
#define GEN_STRINGIZE_VA( ... )                   #__VA_ARGS__
#define txt( ... )                                GEN_STRINGIZE_VA( __VA_ARGS__ )
#define txt_to_StrC( ... )		                  sizeof( GEN_STRINGIZE_VA( __VA_ARGS__ ) ), GEN_STRINGIZE_VA( __VA_ARGS__ )
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

// TODO : Use it.
struct gen_Arena
{
	static
	void* allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags );

	static
	gen_Arena init_from_memory( void* start, sw size )
	{
		return
		{
			{ nullptr, nullptr },
			start,
			size,
			0,
			0
		};
	}

	static
	gen_Arena init_from_allocator( AllocatorInfo backing, sw size )
	{
		gen_Arena result =
		{
			backing,
			alloc( backing, size),
			size,
			0,
			0
		};
		return result;
	}

	static
	gen_Arena init_sub( gen_Arena& parent, sw size )
	{
		return init_from_allocator( parent.Backing, size );
	}

	sw alignment_of( sw alignment )
	{
		sw alignment_offset, result_pointer, mask;
		ZPL_ASSERT( is_power_of_two( alignment ) );

		alignment_offset = 0;
		result_pointer   = (sw) PhysicalStart + TotalUsed;
		mask             = alignment - 1;

		if ( result_pointer & mask )
			alignment_offset = alignment - ( result_pointer & mask );

		return alignment_offset;
	}

	void check()
	{
		ZPL_ASSERT( TempCount == 0 );
	}

	void free()
	{
		if ( Backing.proc )
		{
			zpl::free( Backing, PhysicalStart );
			PhysicalStart = nullptr;
		}
	}

	sw size_remaining( sw alignment )
	{
		sw result = TotalSize - ( TotalUsed + alignment_of( alignment ) );
		return result;
	}

	AllocatorInfo Backing;
	void*         PhysicalStart;
	sw            TotalSize;
	sw            TotalUsed;
	sw            TempCount;
};

struct gen_Pool
{
	static
	void* allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags );

	static
	gen_Pool init( AllocatorInfo backing, sw num_blocks, sw block_size )
	{
		return init_align( backing, num_blocks, block_size, ZPL_DEFAULT_MEMORY_ALIGNMENT );
	}

	static
	gen_Pool init_align( AllocatorInfo backing, sw num_blocks, sw block_size, sw block_align );

	void free()
	{
		if ( Backing.proc )
		{
			zpl::free( Backing, PhysicalStart );
		}
	}

	AllocatorInfo Backing;
	void*         PhysicalStart;
	void*         FreeList;
	sw            BlockSize;
	sw            BlockAlign;
	sw            TotalSize;
	sw            NumBlocks;
};

#pragma endregion Memory

#pragma region Containers
#pragma push_macro("template")
#undef template

// TODO : Use it.
template<class Type>
struct TArray
{
	struct Header
	{
		AllocatorInfo Allocator;
		uw            Capacity;
		uw            Num;
	};

	static
	TArray init( AllocatorInfo allocator )
	{
		return init_reserve( allocator, grow_formula(0) );
	}

	static
	TArray init_reserve( AllocatorInfo allocator, sw capacity )
	{
		Header* header = rcast( Header*, alloc( allocator, sizeof(Header) + sizeof(Type) ));

		if ( header == nullptr )
			return { nullptr };

		header->Allocator = allocator;
		header->Capacity  = capacity;
		header->Num       = 0;

		return { rcast( Type*, header + 1) };
	}

	static
	uw grow_formula( uw value )
	{
		return 2 * value * 8;
	}

	bool append( Type value )
	{
		Header& header = get_header();

		if ( header.Num == header.Capacity )
		{
			if ( ! grow( header.Capacity ))
				return false;
		}

		Data[ header.Num ] = value;
		header.Num++;

		return true;
	}

	Type& back( void )
	{
		Header& header = get_header();
		return Data[ header.Num - 1 ];
	}

	void clear( void )
	{
		Header& header = get_header();
		header.Num     = 0;
	}

	bool fill( uw begin, uw end, Type value )
	{
		Header& header = get_header();

		if ( begin < 0 || end >= header.Num )
			return false;

		for ( sw idx = begin; idx < end; idx++ )
		{
			Data[ idx ] = value;
		}

		return true;
	}

	void free( void )
	{
		Header& header = get_header();
		zpl::free( header.Allocator, &header );
	}

	Header& get_header( void )
	{
		return *( reinterpret_cast< Header* >( Data ) - 1 );
	}

	bool grow( uw min_capacity )
	{
		Header& header       = get_header();
		uw      new_capacity = grow_formula( header.Capacity );

		if ( new_capacity < min_capacity )
			new_capacity = 8;

		return set_capacity( new_capacity );
	}

	uw num( void )
	{
		return get_header().Num;
	}

	bool pop( void )
	{
		Header& header = get_header();

		ZPL_ASSERT( header.Num > 0 );
		header.Num--;
	}

	void remove_at( uw idx )
	{
		Header* header = &get_header();
		ZPL_ASSERT( idx < header->Num );

		mem_move( header + idx, header + idx + 1, sizeof( Type ) * ( header->Num - idx - 1 ) );
		header->Num--;
	}

	bool reserve( uw new_capacity )
	{
		Header& header = get_header();

		if ( header.Capacity < new_capacity )
			return set_capacity( new_capacity );

		return true;
	}

	bool resize( uw num )
	{
		Header& header = get_header();

		if ( num > header.Capacity )
		{
			if ( ! grow( header.Capacity ) )
				return false;
		}

		header.Num = num;
		return true;
	}

	bool set_capacity( uw new_capacity )
	{
		Header& header = get_header();

		if ( new_capacity == header.Capacity )
			return true;

		if ( new_capacity < header.Num )
			header.Num = new_capacity;

		sw      size       = sizeof( Header ) + sizeof( Type ) * new_capacity;
		Header* new_header = reinterpret_cast< Header* >( alloc( header.Allocator, size ) );

		if ( new_header == nullptr )
			return false;

		mem_move( new_header, &header, sizeof( Header ) + sizeof( Type ) * header.Num );

		new_header->Allocator = header.Allocator;
		new_header->Num       = header.Num;
		new_header->Capacity  = new_capacity;

		zpl::free( header.Allocator, &header );

		Data = ( Type* )new_header + 1;
		return true;
	}

	Type* Data;

	operator Type*()
	{
		return Data;
	}

	operator Type const*() const
	{
		return Data;
	}
};

// TODO : Use it.
template<typename Type>
struct THashTable
{
	struct FindResult
	{
		sw HashIndex;
		sw PrevIndex;
		sw EntryIndex;
	};

	struct Entry
	{
		u64  Key;
		sw   Next;
		Type Value;
	};

	static
	THashTable init( AllocatorInfo allocator )
	{
		THashTable<Type> result = {0};

		result.Hashes.init( allocator );
		result.Entries.init( allocator );

		return result;
	}

	void clear( void )
	{
		for ( sw idx = 0; idx < Hashes.num(); idx++ )
			Hashes[ idx ] = -1;

		Hashes.clear();
		Entries.clear();
	}

	void destroy( void )
	{
		if ( Hashes )
			Hashes.free();
		if ( Entries )
			Entries.free();
	}

	Type* get( u64 key )
	{
		sw idx = find( key ).EntryIndex;
		if ( idx > 0 )
			return & Entries[ idx ].Value;

		return nullptr;
	}

	using MapProc = void (*)( u64 key, Type  value );

	void map( MapProc map_proc )
	{
		ZPL_ASSERT_NOT_NULL( map_proc );

		for ( sw idx = 0; idx < Entries.num(); idx++ )
		{
			map_proc( Entries[ idx ].Key, Entries[ idx ].Value );
		}
	}

	using MapMutProc = void (*)( u64 key, Type* value );

	void map_mut( MapMutProc map_proc )
	{
		ZPL_ASSERT_NOT_NULL( map_proc );

		for ( sw idx = 0; idx < Entries.num(); idx++ )
		{
			map_proc( Entries[ idx ].Key, & Entries[ idx ].Value );
		}
	}

	void grow()
	{
		sw new_num = TArray<Entry>::grow_formula( Entries.num() )
		rehash( new_num );
	}

	void rehash( sw new_num )
	{
		sw idx;
		sw last_added_index;

		THashTable<Type> new_ht = init( Hashes.get_header().Allocator );

		new_ht.Hashes.resize( new_num );
		new_ht.Entries.reserve( new_ht.Hashes.num() );

		for ( idx = 0; idx < new_ht.Hashes.num(); ++idx )
			new_ht.Hashes[ idx ] = -1;

		for ( idx = 0; idx < Entries.num(); ++idx )
		{
			Entry& entry = Entries[ idx ];

			FindResult find_result;

			if ( new_ht.Hashes.num() == 0 )
				new_ht.grow();

			entry            = Entries[ idx ];
			find_result      = new_ht.find( entry.Key );
			last_added_index = new_ht.add_entry( entry.Key );

			if ( find_result.PrevIndex < 0 )
				new_ht.Hashes[ find_result.HashIndex ] = last_added_index;

			else
				new_ht.Entries[ find_result.PrevIndex ].Next = last_added_index;

			new_ht.Entries[ last_added_index ].Next  = find_result.EntryIndex;
			new_ht.Entries[ last_added_index ].Value = entry.Value;
		}

		// *this = new_ht;

		// old_ht.destroy();

		destroy();
		Hashes  = new_ht.Hashes;
		Entries = new_ht.Entries;
	}

	void rehash_fast()
	{
		sw idx;

		for ( idx = 0; idx < Entries.num(); idx++ )
			Entries[ idx ].Next = -1;

		for ( idx = 0; idx < Hashes.num(); idx++ )
			Hashes[ idx ] = -1;

		for ( idx = 0; idx < Entries.num(); idx++ )
		{
			Entry* entry;

			FindResult find_result;
		}
	}

	void remove( u64 key )
	{
		FindResult find_result = find( key);

		if ( find_result.EntryIndex >= 0 )
		{
			Entries.remove_at( find_result.EntryIndex );
			rehash_fast();
		}
	}

	void remove_entry( sw idx )
	{
		Entries.remove_at( idx );
	}

	void set( u64 key, Type value )
	{
		sw idx;
		FindResult find_result;

		if ( Hashes.num() == 0 )
			grow();

		find_result = find( key );

		if ( find_result.EntryIndex >= 0 )
		{
			idx = find_result.EntryIndex;
		}
		else
		{
			idx = add_entry( key );

			if ( find_result.PrevIndex >= 0 )
			{
				Entries[ find_result.PrevIndex ].Next = idx;
			}
			else
			{
				Hashes[ find_result.HashIndex ] = idx;
			}
		}

		Entries[ idx ].Value = value;

		if ( full() )
			grow();
	}

	sw slot( u64 key )
	{
		for ( sw idx = 0; idx < Hashes.num(); ++idx )
			if ( Hashes[ idx ] == key )
				return idx;

		return -1;
	}

	TArray< sw>    Hashes;
	TArray< Entry> Entries;

protected:

	sw add_entry( u64 key )
	{
		sw idx;
		Entry entry = { key, -1 };

		idx = Entries.num();
		Entries.append( entry );
		return idx;
	}

	FindResult find( u64 key )
	{
		FindResult result = { -1, -1, -1 };

		if ( Hashes.num() > 0 )
		{
			result.HashIndex    = key % Hashes.num();
			result.EntryIndex  = Hashes[ result.HashIndex ];

			while ( result.EntryIndex >= 0 )
			{
				if ( Entries[ result.EntryIndex ].Key == key )
					break;

				result.PrevIndex  = result.EntryIndex;
				result.EntryIndex = Entries[ result.EntryIndex ].Next;
			}
		}

		return result;
	}

	b32 full()
	{
		return 0.75f * Hashes.num() < Entries.num();
	}
};

#pragma pop_macro("template")
#pragma endregion Containers

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

		static
		String make( AllocatorInfo allocator, char const* str )
		{
			sw length = str ? str_len( str ) : 0;
			return make_length( allocator, str, length );
		}

		static
		String make( AllocatorInfo allocator, StrC str )
		{
			return make_length( allocator, str.Ptr, str.Len );
		}

		static
		String make_reserve( AllocatorInfo allocator, sw capacity )
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

		static
		String make_length( AllocatorInfo allocator, char const* str, sw length )
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

		static
		String fmt( AllocatorInfo allocator, char* buf, sw buf_size, char const* fmt, ... )
		{
			va_list va;
			va_start( va, fmt );
			str_fmt_va( buf, buf_size, fmt, va );
			va_end( va );

			return make( allocator, buf );
		}

		static
		String fmt_buf( AllocatorInfo allocator, char const* fmt, ... )
		{
			local_persist thread_local
			char buf[ ZPL_PRINTF_MAXLEN ] = { 0 };

			va_list va;
			va_start( va, fmt );
			str_fmt_va( buf, ZPL_PRINTF_MAXLEN, fmt, va );
			va_end( va );

			return make( allocator, buf );
		}

		static
		String join( AllocatorInfo allocator, char const** parts, sw num_parts, char const* glue )
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

		static
		bool are_equal( String lhs, String rhs )
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

		// Used with cached strings
		// Essentially makes the string a string view.
		String const& operator = ( String const& other ) const
		{
			if ( this == & other )
				return *this;

			String& this_ = ccast( String, *this );

			this_.Data = other.Data;

			return this_;
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
	// NOTE: This limits the size of the string that can be read from a file or generated to 10 megs.
	// If you are generating a string larger than this, increase the size of the bucket here.
	constexpr uw BucketSize = megabytes(10);

	// Global allocator used for data with process lifetime.
	extern AllocatorInfo GlobalAllocator;

	// Heap allocator is being used for now to isolate errors from being memory related (tech debt till ready to address)
	// #define g_allocator heap()

	void setup();
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
