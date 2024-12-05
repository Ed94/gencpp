#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "debug.hpp"
#endif

#pragma region Memory

#define kilobytes( x ) (          ( x ) * ( s64 )( 1024 ) )
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

GEN_API_C_BEGIN

//! Checks if value is power of 2.
b32 is_power_of_two( ssize x );

//! Aligns address to specified alignment.
void* align_forward( void* ptr, ssize alignment );

//! Aligns value to a specified alignment.
s64 align_forward_by_value( s64 value, ssize alignment );

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

enum AllocType_Def //enum_underlying(u8)
{
	EAllocation_ALLOC,
	EAllocation_FREE,
	EAllocation_FREE_ALL,
	EAllocation_RESIZE,
};
typedef enum AllocType_Def AllocType;

typedef void*(AllocatorProc)( void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags );

struct AllocatorInfo_Def
{
	AllocatorProc* Proc;
	void*          Data;
};
typedef struct AllocatorInfo_Def AllocatorInfo;

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
void allocator_free( AllocatorInfo a, void* ptr );

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
constexpr AllocatorInfo heap( void ) { AllocatorInfo allocator = { heap_allocator_proc, nullptr }; return allocator; }

//! Helper to allocate memory using heap allocator.
#define malloc( sz ) alloc( heap(), sz )

//! Helper to free memory allocated by heap allocator.
#define mfree( ptr ) free( heap(), ptr )

struct VirtualMemory_Def
{
	void*  data;
	ssize size;
};
typedef struct VirtualMemory_Def VirtualMemory;

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
b32 vm_purge( VirtualMemory vm );

//! Retrieve VM's page size and alignment.
ssize virtual_memory_page_size( ssize* alignment_out );

#pragma region Arena
struct Arena_Def;
typedef struct Arena_Def Arena;

AllocatorInfo arena_allocator_info( Arena* arena );

// Remove static keyword and rename allocator_proc
void* arena_allocator_proc(void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags);

// Add these declarations after the Arena struct
Arena arena_init_from_allocator(AllocatorInfo backing, ssize size);
Arena arena_init_from_memory   ( void* start, ssize size );

Arena arena_init_sub      (Arena* parent, ssize size);
ssize arena_alignment_of  (Arena* arena, ssize alignment);
void  arena_check         (Arena* arena);
void  arena_free          (Arena* arena);
ssize arena_size_remaining(Arena* arena, ssize alignment);

struct Arena_Def
{
	AllocatorInfo Backing;
	void*         PhysicalStart;
	ssize         TotalSize;
	ssize         TotalUsed;
	ssize         TempCount;

#if GEN_SUPPORT_CPP_MEMBER_FEATURES
#pragma region Member Mapping
	forceinline operator AllocatorInfo() { return GEN_NS arena_allocator_info(this); }

	forceinline static void* allocator_proc( void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags ) { return GEN_NS arena_allocator_proc( allocator_data, type, size, alignment, old_memory, old_size, flags ); }
	forceinline static Arena init_from_memory( void* start, ssize size )                                                                                      { return GEN_NS arena_init_from_memory( start, size ); }
	forceinline static Arena init_from_allocator( AllocatorInfo backing, ssize size )                                                                         { return GEN_NS arena_init_from_allocator( backing, size ); }
	forceinline static Arena init_sub( Arena& parent, ssize size )                                                                                            { return GEN_NS arena_init_from_allocator( parent.Backing, size ); }
	forceinline        ssize alignment_of( ssize alignment )                                                                                                  { return GEN_NS arena_alignment_of(this, alignment); }
	forceinline        void  free()                                                                                                                           { return GEN_NS arena_free(this);  }
	forceinline        ssize size_remaining( ssize alignment )                                                                                                { return GEN_NS arena_size_remaining(this, alignment); }

// This id is defined by Unreal for asserts
#pragma push_macro("check")
#undef check
	forceinline void check() { GEN_NS arena_check(this); }
#pragma pop_macro("check")

#pragma endregion Member Mapping
#endif
};

#if GEN_SUPPORT_CPP_REFERENCES
GEN_API_C_END
forceinline AllocatorInfo allocator_info(Arena& arena )                 { return arena_allocator_info(& arena); }
forceinline Arena         init_sub      (Arena& parent, ssize size)     { return arena_init_sub( & parent, size); }
forceinline ssize         alignment_of  (Arena& arena, ssize alignment) { return arena_alignment_of( & arena, alignment); }
forceinline void          free          (Arena& arena)                  { return arena_free(& arena); }
forceinline ssize         size_remaining(Arena& arena, ssize alignment) { return arena_size_remaining(& arena, alignment); }

// This id is defined by Unreal for asserts
#pragma push_macro("check")
#undef check
forceinline void check(Arena& arena) { return arena_check(& arena); };
#pragma pop_macro("check")
GEN_API_C_BEGIN
#endif


inline
AllocatorInfo arena_allocator_info( Arena* arena ) {
	GEN_ASSERT(arena != nullptr);
	AllocatorInfo info = { arena_allocator_proc, arena };
	return info;
}

inline
Arena arena_init_from_memory( void* start, ssize size )
{
	Arena arena = {
		{ nullptr, nullptr },
		start,
		size,
		0,
		0
	};
	return arena;
}

inline
Arena arena_init_from_allocator(AllocatorInfo backing, ssize size) {
	Arena result = {
		backing,
		alloc(backing, size),
		size,
		0,
		0
	};
	return result;
}

inline
Arena arena_init_sub(Arena* parent, ssize size) {
	GEN_ASSERT(parent != nullptr);
	return arena_init_from_allocator(parent->Backing, size);
}

inline
ssize arena_alignment_of(Arena* arena, ssize alignment)
{
	GEN_ASSERT(arena != nullptr);
	ssize alignment_offset, result_pointer, mask;
	GEN_ASSERT(is_power_of_two(alignment));

	alignment_offset = 0;
	result_pointer  = (ssize)arena->PhysicalStart + arena->TotalUsed;
	mask            = alignment - 1;

	if (result_pointer & mask)
	alignment_offset = alignment - (result_pointer & mask);

	return alignment_offset;
}

inline
void arena_check(Arena* arena)
{
    GEN_ASSERT(arena != nullptr );
    GEN_ASSERT(arena->TempCount == 0);
}

inline
void arena_free(Arena* arena)
{
	GEN_ASSERT(arena != nullptr);
	if (arena->Backing.Proc)
	{
		allocator_free(arena->Backing, arena->PhysicalStart);
		arena->PhysicalStart = nullptr;
	}
}

inline
ssize arena_size_remaining(Arena* arena, ssize alignment)
{
	GEN_ASSERT(arena != nullptr);
	ssize result = arena->TotalSize - (arena->TotalUsed + arena_alignment_of(arena, alignment));
	return result;
}
#pragma endregion Arena

GEN_API_C_END

#pragma region FixedArena
template<s32 Size>
struct FixedArena;

template<s32 Size> FixedArena<Size> fixed_arena_init();
template<s32 Size> AllocatorInfo    fixed_arena_allocator_info(FixedArena<Size>* fixed_arena );
template<s32 Size> ssize            fixed_arena_size_remaining(FixedArena<Size>* fixed_arena, ssize alignment);

#if GEN_SUPPORT_CPP_REFERENCES
template<s32 Size> AllocatorInfo    allocator_info( FixedArena<Size>& fixed_arena )                { return allocator_info(& fixed_arena); }
template<s32 Size> ssize            size_remaining(FixedArena<Size>& fixed_arena, ssize alignment) { return size_remaining( & fixed_arena, alignment); }
#endif

// Just a wrapper around using an arena with memory associated with its scope instead of from an allocator.
// Used for static segment or stack allocations.
template< s32 Size >
struct FixedArena
{
	char  memory[Size];
	Arena arena;

#if GEN_SUPPORT_CPP_MEMBER_FEATURES
#pragma region Member Mapping
	forceinline operator AllocatorInfo() { return GEN_NS allocator_info(this); }

	forceinline static FixedArena init()                          { FixedArena result; GEN_NS fixed_arena_init<Size>(result); return result; }
	forceinline ssize             size_remaining(ssize alignment) { GEN_NS size_remaining(this, alignment); }
#pragma endregion Member Mapping
#endif
};

template<s32 Size> inline
AllocatorInfo fixed_arena_allocator_info( FixedArena<Size>* fixed_arena ) {
	GEN_ASSERT(fixed_arena);
	return { arena_allocator_proc, & fixed_arena->arena };
}

template<s32 Size> inline
void fixed_arena_init(FixedArena<Size>* result) {
    zero_size(& result->memory[0], Size);
    result->arena = arena_init_from_memory(& result->memory[0], Size);
}

template<s32 Size> inline
void fixed_arena_free(FixedArena<Size>* fixed_arena) {
	arena_free( & fixed_arena->arena);
}

template<s32 Size> inline
ssize fixed_arena_size_remaining(FixedArena<Size>* fixed_arena, ssize alignment) {
    return size_remaining(fixed_arena->arena, alignment);
}

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
#pragma endregion FixedArena

GEN_API_C_BEGIN

#pragma region Pool
struct Pool_Def;
typedef struct Pool_Def Pool;

void* pool_allocator_proc(void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags);

Pool          pool_init(AllocatorInfo backing, ssize num_blocks, ssize block_size);
Pool          pool_init_align(AllocatorInfo backing, ssize num_blocks, ssize block_size, ssize block_align);
AllocatorInfo pool_allocator_info(Pool* pool);
void          pool_clear(Pool* pool);
void          pool_free(Pool* pool);

#if GEN_SUPPORT_CPP_REFERENCES
GEN_API_C_END
AllocatorInfo allocator_info(Pool& pool) { return pool_allocator_info(& pool); }
void          clear(Pool& pool)          { return pool_clear(& pool); }
void          free(Pool& pool)           { return pool_free(& pool); }
GEN_API_C_BEGIN
#endif

struct Pool_Def
{
	AllocatorInfo Backing;
	void*         PhysicalStart;
	void*         FreeList;
	ssize         BlockSize;
	ssize         BlockAlign;
	ssize         TotalSize;
	ssize         NumBlocks;

#if GEN_SUPPORT_CPP_MEMBER_FEATURES
#pragma region Member Mapping
    forceinline operator AllocatorInfo() { return GEN_NS pool_allocator_info(this); }

    forceinline static void* allocator_proc(void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags) { return GEN_NS pool_allocator_proc(allocator_data, type, size, alignment, old_memory, old_size, flags); }
    forceinline static Pool  init(AllocatorInfo backing, ssize num_blocks, ssize block_size)                                                                { return GEN_NS pool_init(backing, num_blocks, block_size); }
    forceinline static Pool  init_align(AllocatorInfo backing, ssize num_blocks, ssize block_size, ssize block_align)                                       { return GEN_NS pool_init_align(backing, num_blocks, block_size, block_align); }
    forceinline        void  clear() { GEN_NS pool_clear( this); }
    forceinline        void  free()  { GEN_NS pool_free( this); }
#pragma endregion
#endif
};

inline
AllocatorInfo pool_allocator_info(Pool* pool) {
	AllocatorInfo info = { pool_allocator_proc, pool };
	return info;
}

inline
Pool pool_init(AllocatorInfo backing, ssize num_blocks, ssize block_size) {
   return pool_init_align(backing, num_blocks, block_size, GEN_DEFAULT_MEMORY_ALIGNMENT);
}

inline
void pool_free(Pool* pool) {
   if(pool->Backing.Proc) {
       allocator_free(pool->Backing, pool->PhysicalStart);
   }
}
#pragma endregion Pool

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

inline s64 align_forward_s64( s64 value, ssize alignment ) { return value + ( alignment - value % alignment ) % alignment; }

inline void*       pointer_add      ( void*       ptr, ssize bytes ) { return rcast(void*,         rcast( u8*,        ptr) + bytes ); }
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
void allocator_free( AllocatorInfo a, void* ptr ) {
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
		allocator_free( a, old_memory );
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
		allocator_free( a, old_memory );
		return new_memory;
	}
}

inline
void zero_size( void* ptr, ssize size ) {
	mem_set( ptr, 0, size );
}

GEN_API_C_END

#pragma endregion Memory
