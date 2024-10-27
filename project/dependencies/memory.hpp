#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "debug.hpp"
#endif

#pragma region Memory

#define kilobytes( x ) ( ( x ) * ( s64 )( 1024 ) )
#define megabytes( x ) ( kilobytes( x ) * ( s64 )( 1024 ) )
#define gigabytes( x ) ( megabytes( x ) * ( s64 )( 1024 ) )
#define terabytes( x ) ( gigabytes( x ) * ( s64 )( 1024 ) )

#define GEN__ONES          ( scast( GEN_NS usize, - 1) / GEN_U8_MAX )
#define GEN__HIGHS         ( GEN__ONES * ( GEN_U8_MAX / 2 + 1 ) )
#define GEN__HAS_ZERO( x ) ( ( ( x ) - GEN__ONES ) & ~( x ) & GEN__HIGHS )

template< class Type >
void swap( Type& a, Type& b )
{
	Type tmp = a;
	a = b;
	b = tmp;
}

//! Checks if value is power of 2.
b32 is_power_of_two( ssize x );

//! Aligns address to specified alignment.
void* align_forward( void* ptr, ssize alignment );

//! Aligns value to a specified alignment.
s64 align_forward_i64( s64 value, ssize alignment );

//! Moves pointer forward by bytes.
void* pointer_add( void* ptr, ssize bytes );

//! Moves pointer forward by bytes.
void const* pointer_add_const( void const* ptr, ssize bytes );

//! Calculates difference between two addresses.
ssize pointer_diff( void const* begin, void const* end );

//! Copy non-overlapping memory from source to destination.
void* mem_copy( void* dest, void const* source, ssize size );

//! Search for a constant value within the size limit at memory location.
void const* mem_find( void const* data, u8 byte_value, ssize size );

//! Copy memory from source to destination.
void* mem_move( void* dest, void const* source, ssize size );

//! Set constant value at memory location with specified size.
void* mem_set( void* data, u8 byte_value, ssize size );

//! @param ptr Memory location to clear up.
//! @param size The size to clear up with.
void zero_size( void* ptr, ssize size );

//! Clears up an item.
#define zero_item( t ) zero_size( ( t ), size_of( *( t ) ) )    // NOTE: Pass pointer of struct

//! Clears up an array.
#define zero_array( a, count ) zero_size( ( a ), size_of( *( a ) ) * count )

enum AllocType : u8
{
	EAllocation_ALLOC,
	EAllocation_FREE,
	EAllocation_FREE_ALL,
	EAllocation_RESIZE,
};

using AllocatorProc = void* ( void* allocator_data, AllocType type
	, ssize size, ssize alignment
	, void* old_memory, ssize old_size
	, u64 flags );

struct AllocatorInfo
{
	AllocatorProc* Proc;
	void*          Data;
};

enum AllocFlag
{
	ALLOCATOR_FLAG_CLEAR_TO_ZERO = bit( 0 ),
};

#ifndef GEN_DEFAULT_MEMORY_ALIGNMENT
#	define GEN_DEFAULT_MEMORY_ALIGNMENT ( 2 * size_of( void* ) )
#endif

#ifndef GEN_DEFAULT_ALLOCATOR_FLAGS
#	define GEN_DEFAULT_ALLOCATOR_FLAGS ( ALLOCATOR_FLAG_CLEAR_TO_ZERO )
#endif

//! Allocate memory with default alignment.
void* alloc( AllocatorInfo a, ssize size );

//! Allocate memory with specified alignment.
void* alloc_align( AllocatorInfo a, ssize size, ssize alignment );

//! Free allocated memory.
void free( AllocatorInfo a, void* ptr );

//! Free all memory allocated by an allocator.
void free_all( AllocatorInfo a );

//! Resize an allocated memory.
void* resize( AllocatorInfo a, void* ptr, ssize old_size, ssize new_size );

//! Resize an allocated memory with specified alignment.
void* resize_align( AllocatorInfo a, void* ptr, ssize old_size, ssize new_size, ssize alignment );

//! Allocate memory for an item.
#define alloc_item( allocator_, Type ) ( Type* )alloc( allocator_, size_of( Type ) )

//! Allocate memory for an array of items.
#define alloc_array( allocator_, Type, count ) ( Type* )alloc( allocator_, size_of( Type ) * ( count ) )

/* heap memory analysis tools */
/* define GEN_HEAP_ANALYSIS to enable this feature */
/* call zpl_heap_stats_init at the beginning of the entry point */
/* you can call zpl_heap_stats_check near the end of the execution to validate any possible leaks */
void  heap_stats_init( void );
ssize heap_stats_used_memory( void );
ssize heap_stats_alloc_count( void );
void  heap_stats_check( void );

//! Allocate/Resize memory using default options.

//! Use this if you don't need a "fancy" resize allocation
void* default_resize_align( AllocatorInfo a, void* ptr, ssize old_size, ssize new_size, ssize alignment );

void* heap_allocator_proc( void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags );

//! The heap allocator backed by operating system's memory manager.
constexpr AllocatorInfo heap( void ) { return { heap_allocator_proc, nullptr }; }

//! Helper to allocate memory using heap allocator.
#define malloc( sz ) alloc( heap(), sz )

//! Helper to free memory allocated by heap allocator.
#define mfree( ptr ) free( heap(), ptr )

struct VirtualMemory
{
	void*  data;
	ssize size;
};

//! Initialize virtual memory from existing data.
VirtualMemory vm_from_memory( void* data, ssize size );

//! Allocate virtual memory at address with size.

//! @param addr The starting address of the region to reserve. If NULL, it lets operating system to decide where to allocate it.
//! @param size The size to serve.
VirtualMemory vm_alloc( void* addr, ssize size );

//! Release the virtual memory.
b32 vm_free( VirtualMemory vm );

//! Trim virtual memory.
VirtualMemory vm_trim( VirtualMemory vm, ssize lead_size, ssize size );

//! Purge virtual memory.
b32 gen_vm_purge( VirtualMemory vm );

//! Retrieve VM's page size and alignment.
ssize gen_virtual_memory_page_size( ssize* alignment_out );

struct Arena
{
	static
	void* allocator_proc( void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags );

	static
	Arena init_from_memory( void* start, ssize size )
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
	Arena init_from_allocator( AllocatorInfo backing, ssize size )
	{
		Arena result =
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
	Arena init_sub( Arena& parent, ssize size )
	{
		return init_from_allocator( parent.Backing, size );
	}

	ssize alignment_of( ssize alignment )
	{
		ssize alignment_offset, result_pointer, mask;
		GEN_ASSERT( is_power_of_two( alignment ) );

		alignment_offset = 0;
		result_pointer   = (ssize) PhysicalStart + TotalUsed;
		mask             = alignment - 1;

		if ( result_pointer & mask )
			alignment_offset = alignment - ( result_pointer & mask );

		return alignment_offset;
	}

// This id is defined by Unreal for asserts
#pragma push_macro("check")
#undef check
	void check()
	{
		GEN_ASSERT( TempCount == 0 );
	}
#pragma pop_macro("check")

	void free()
	{
		if ( Backing.Proc )
		{
			gen::free( Backing, PhysicalStart );
			PhysicalStart = nullptr;
		}
	}

	ssize size_remaining( ssize alignment )
	{
		ssize result = TotalSize - ( TotalUsed + alignment_of( alignment ) );
		return result;
	}

	AllocatorInfo Backing;
	void*         PhysicalStart;
	ssize            TotalSize;
	ssize            TotalUsed;
	ssize            TempCount;

	operator AllocatorInfo()
	{
		return { allocator_proc, this };
	}
};

// Just a wrapper around using an arena with memory associated with its scope instead of from an allocator.
// Used for static segment or stack allocations.
template< s32 Size >
struct FixedArena
{
	static
	FixedArena init()
	{
		FixedArena result = { Arena::init_from_memory( result.memory, Size ), {0} };
		return result;
	}

	ssize size_remaining( ssize alignment )
	{
		return arena.size_remaining( alignment );
	}

	operator AllocatorInfo()
	{
		return { Arena::allocator_proc, &arena };
	}

	Arena arena;
	char  memory[ Size ];
};

using Arena_1KB   = FixedArena< kilobytes( 1 ) >;
using Arena_4KB   = FixedArena< kilobytes( 4 ) >;
using Arena_8KB   = FixedArena< kilobytes( 8 ) >;
using Arena_16KB  = FixedArena< kilobytes( 16 ) >;
using Arena_32KB  = FixedArena< kilobytes( 32 ) >;
using Arena_64KB  = FixedArena< kilobytes( 64 ) >;
using Arena_128KB = FixedArena< kilobytes( 128 ) >;
using Arena_256KB = FixedArena< kilobytes( 256 ) >;
using Arena_512KB = FixedArena< kilobytes( 512 ) >;
using Arena_1MB   = FixedArena< megabytes( 1 ) >;
using Arena_2MB   = FixedArena< megabytes( 2 ) >;
using Arena_4MB   = FixedArena< megabytes( 4 ) >;

struct Pool
{
	static
	void* allocator_proc( void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags );

	static
	Pool init( AllocatorInfo backing, ssize num_blocks, ssize block_size )
	{
		return init_align( backing, num_blocks, block_size, GEN_DEFAULT_MEMORY_ALIGNMENT );
	}

	static
	Pool init_align( AllocatorInfo backing, ssize num_blocks, ssize block_size, ssize block_align );

	void clear();

	void free()
	{
		if ( Backing.Proc )
		{
			gen::free( Backing, PhysicalStart );
		}
	}

	AllocatorInfo Backing;
	void*         PhysicalStart;
	void*         FreeList;
	ssize         BlockSize;
	ssize         BlockAlign;
	ssize         TotalSize;
	ssize         NumBlocks;

	operator AllocatorInfo()
	{
		return { allocator_proc, this };
	}
};


inline
b32 is_power_of_two( ssize x ) {
	if ( x <= 0 )
		return false;
	return ! ( x & ( x - 1 ) );
}

inline
mem_ptr align_forward( void* ptr, ssize alignment )
{
	GEN_ASSERT( is_power_of_two( alignment ) );
	uptr p       = to_uptr(ptr);
	uptr forward = (p + ( alignment - 1 ) ) & ~( alignment - 1 );

	return to_mem_ptr(forward);
}

inline s64 align_forward_i64( s64 value, ssize alignment ) { return value + ( alignment - value % alignment ) % alignment; }

inline void*       pointer_add      ( void*       ptr, ssize bytes ) { return rcast(void*,       rcast( u8*,        ptr) + bytes ); }
inline void const* pointer_add_const( void const* ptr, ssize bytes ) { return rcast(void const*, rcast( u8 const*,  ptr) + bytes ); }

inline sptr pointer_diff( mem_ptr_const begin, mem_ptr_const end ) {
	return scast( ssize, rcast( u8 const*, end) - rcast(u8 const*, begin) );
}

inline
void* mem_move( void* destination, void const* source, ssize byte_count )
{
	if ( destination == NULL )
	{
		return NULL;
	}

	u8*       dest_ptr = rcast( u8*, destination);
	u8 const* src_ptr  = rcast( u8 const*, source);

	if ( dest_ptr == src_ptr )
		return dest_ptr;

	if ( src_ptr + byte_count <= dest_ptr || dest_ptr + byte_count <= src_ptr )    // NOTE: Non-overlapping
		return mem_copy( dest_ptr, src_ptr, byte_count );

	if ( dest_ptr < src_ptr )
	{
		if ( to_uptr(src_ptr) % size_of( ssize ) == to_uptr(dest_ptr) % size_of( ssize ) )
		{
			while ( pcast( uptr, dest_ptr) % size_of( ssize ) )
			{
				if ( ! byte_count-- )
					return destination;

				*dest_ptr++ = *src_ptr++;
			}
			while ( byte_count >= size_of( ssize ) )
			{
				* rcast(ssize*, dest_ptr)  = * rcast(ssize const*, src_ptr);
				byte_count -= size_of( ssize );
				dest_ptr   += size_of( ssize );
				src_ptr    += size_of( ssize );
			}
		}
		for ( ; byte_count; byte_count-- )
			*dest_ptr++ = *src_ptr++;
	}
	else
	{
		if ( ( to_uptr(src_ptr) % size_of( ssize ) ) == ( to_uptr(dest_ptr) % size_of( ssize ) ) )
		{
			while ( to_uptr( dest_ptr + byte_count ) % size_of( ssize ) )
			{
				if ( ! byte_count-- )
					return destination;

				dest_ptr[ byte_count ] = src_ptr[ byte_count ];
			}
			while ( byte_count >= size_of( ssize ) )
			{
				byte_count                              -= size_of( ssize );
				* rcast(ssize*, dest_ptr + byte_count )  = * rcast( ssize const*, src_ptr + byte_count );
			}
		}
		while ( byte_count )
			byte_count--, dest_ptr[ byte_count ] = src_ptr[ byte_count ];
	}

	return destination;
}

inline
void* mem_set( void* destination, u8 fill_byte, ssize byte_count )
{
	if ( destination == NULL )
	{
		return NULL;
	}

	ssize align_offset;
	u8*   dest_ptr  = rcast( u8*, destination);
	u32   fill_word = ( ( u32 )-1 ) / 255 * fill_byte;

	if ( byte_count == 0 )
		return destination;

	dest_ptr[ 0 ] = dest_ptr[ byte_count - 1 ] = fill_byte;
	if ( byte_count < 3 )
		return destination;

	dest_ptr[ 1 ] = dest_ptr[ byte_count - 2 ] = fill_byte;
	dest_ptr[ 2 ] = dest_ptr[ byte_count - 3 ] = fill_byte;
	if ( byte_count < 7 )
		return destination;

	dest_ptr[ 3 ] = dest_ptr[ byte_count - 4 ] = fill_byte;
	if ( byte_count < 9 )
		return destination;

	align_offset  = -to_sptr( dest_ptr ) & 3;
	dest_ptr     += align_offset;
	byte_count   -= align_offset;
	byte_count   &= -4;

	* rcast( u32*, ( dest_ptr + 0              ) ) = fill_word;
	* rcast( u32*, ( dest_ptr + byte_count - 4 ) ) = fill_word;
	if ( byte_count < 9 )
		return destination;

	* rcast( u32*, dest_ptr + 4 )               = fill_word;
	* rcast( u32*, dest_ptr + 8 )               = fill_word;
	* rcast( u32*, dest_ptr + byte_count - 12 ) = fill_word;
	* rcast( u32*, dest_ptr + byte_count - 8 )  = fill_word;
	if ( byte_count < 25 )
		return destination;

	* rcast( u32*, dest_ptr + 12 )              = fill_word;
	* rcast( u32*, dest_ptr + 16 )              = fill_word;
	* rcast( u32*, dest_ptr + 20 )              = fill_word;
	* rcast( u32*, dest_ptr + 24 )              = fill_word;
	* rcast( u32*, dest_ptr + byte_count - 28 ) = fill_word;
	* rcast( u32*, dest_ptr + byte_count - 24 ) = fill_word;
	* rcast( u32*, dest_ptr + byte_count - 20 ) = fill_word;
	* rcast( u32*, dest_ptr + byte_count - 16 ) = fill_word;

	align_offset  = 24 + to_uptr( dest_ptr ) & 4;
	dest_ptr     += align_offset;
	byte_count   -= align_offset;

	{
		u64 fill_doubleword = ( scast( u64, fill_word) << 32 ) | fill_word;
		while ( byte_count > 31 )
		{
			* rcast( u64*, dest_ptr + 0 )  = fill_doubleword;
			* rcast( u64*, dest_ptr + 8 )  = fill_doubleword;
			* rcast( u64*, dest_ptr + 16 ) = fill_doubleword;
			* rcast( u64*, dest_ptr + 24 ) = fill_doubleword;

			byte_count -= 32;
			dest_ptr += 32;
		}
	}

	return destination;
}

inline
void* alloc_align( AllocatorInfo a, ssize size, ssize alignment ) {
	return a.Proc( a.Data, EAllocation_ALLOC, size, alignment, nullptr, 0, GEN_DEFAULT_ALLOCATOR_FLAGS );
}

inline
void* alloc( AllocatorInfo a, ssize size ) {
	return alloc_align( a, size, GEN_DEFAULT_MEMORY_ALIGNMENT );
}

inline
void free( AllocatorInfo a, void* ptr ) {
	if ( ptr != nullptr )
		a.Proc( a.Data, EAllocation_FREE, 0, 0, ptr, 0, GEN_DEFAULT_ALLOCATOR_FLAGS );
}

inline
void free_all( AllocatorInfo a ) {
	a.Proc( a.Data, EAllocation_FREE_ALL, 0, 0, nullptr, 0, GEN_DEFAULT_ALLOCATOR_FLAGS );
}

inline
void* resize( AllocatorInfo a, void* ptr, ssize old_size, ssize new_size ) {
	return resize_align( a, ptr, old_size, new_size, GEN_DEFAULT_MEMORY_ALIGNMENT );
}

inline
void* resize_align( AllocatorInfo a, void* ptr, ssize old_size, ssize new_size, ssize alignment ) {
	return a.Proc( a.Data, EAllocation_RESIZE, new_size, alignment, ptr, old_size, GEN_DEFAULT_ALLOCATOR_FLAGS );
}

inline
void* default_resize_align( AllocatorInfo a, void* old_memory, ssize old_size, ssize new_size, ssize alignment )
{
	if ( ! old_memory )
		return alloc_align( a, new_size, alignment );

	if ( new_size == 0 )
	{
		free( a, old_memory );
		return nullptr;
	}

	if ( new_size < old_size )
		new_size = old_size;

	if ( old_size == new_size )
	{
		return old_memory;
	}
	else
	{
		void*  new_memory = alloc_align( a, new_size, alignment );
		if ( ! new_memory )
			return nullptr;

		mem_move( new_memory, old_memory, min( new_size, old_size ) );
		free( a, old_memory );
		return new_memory;
	}
}

inline
void zero_size( void* ptr, ssize size ) {
	mem_set( ptr, 0, size );
}

#pragma endregion Memory
