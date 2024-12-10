#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "printing.cpp"
#endif

#pragma region Memory

void* mem_copy( void* dest, void const* source, ssize n )
{
	if ( dest == nullptr )
	{
		return nullptr;
	}

	return memcpy( dest, source, n );
}

void const* mem_find( void const* data, u8 c, ssize n )
{
	u8 const* s = rcast( u8 const*, data);
	while ( ( rcast( uptr, s) & ( sizeof( usize ) - 1 ) ) && n && *s != c )
	{
		s++;
		n--;
	}
	if ( n && *s != c )
	{
		ssize const* w;
		ssize        k = GEN__ONES * c;
		w           = rcast( ssize const*, s);
		while ( n >= size_of( ssize ) && ! GEN__HAS_ZERO( *w ^ k ) )
		{
			w++;
			n -= size_of( ssize );
		}
		s = rcast( u8 const*, w);
		while ( n && *s != c )
		{
			s++;
			n--;
		}
	}

	return n ? rcast( void const*, s ) : NULL;
}

#define GEN_HEAP_STATS_MAGIC 0xDEADC0DE

typedef struct _heap_stats _heap_stats;
struct _heap_stats
{
	u32 magic;
	ssize  used_memory;
	ssize  alloc_count;
};

global _heap_stats _heap_stats_info;

void heap_stats_init( void )
{
	zero_item( &_heap_stats_info );
	_heap_stats_info.magic = GEN_HEAP_STATS_MAGIC;
}

ssize heap_stats_used_memory( void )
{
	GEN_ASSERT_MSG( _heap_stats_info.magic == GEN_HEAP_STATS_MAGIC, "heap_stats is not initialised yet, call heap_stats_init first!" );
	return _heap_stats_info.used_memory;
}

ssize heap_stats_alloc_count( void )
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

typedef struct _heap_alloc_info _heap_alloc_info;
struct _heap_alloc_info
{
	ssize    size;
	void* physical_start;
};

void* heap_allocator_proc( void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags )
{
	void* ptr = NULL;
	// unused( allocator_data );
	// unused( old_size );
	if ( ! alignment )
		alignment = GEN_DEFAULT_MEMORY_ALIGNMENT;

#ifdef GEN_HEAP_ANALYSIS
	ssize alloc_info_size      = size_of( _heap_alloc_info );
	ssize alloc_info_remainder = ( alloc_info_size % alignment );
	ssize track_size           = max( alloc_info_size, alignment ) + alloc_info_remainder;
	switch ( type )
	{
		case EAllocation_FREE :
			{
				if ( ! old_memory )
					break;
				_heap_alloc_info* alloc_info  = rcast( _heap_alloc_info*, old_memory) - 1;
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
		_heap_alloc_info* alloc_info = rcast( _heap_alloc_info*, rcast( char*, ptr) + alloc_info_remainder );
		zero_item( alloc_info );
		alloc_info->size              = size - track_size;
		alloc_info->physical_start    = ptr;
		ptr                           = rcast( void*, alloc_info + 1 );
		_heap_stats_info.used_memory += alloc_info->size;
		_heap_stats_info.alloc_count++;
	}
#endif

	return ptr;
}

#pragma region VirtualMemory
VirtualMemory vm_from_memory( void* data, ssize size )
{
	VirtualMemory vm;
	vm.data = data;
	vm.size = size;
	return vm;
}

#if defined( GEN_SYSTEM_WINDOWS )
VirtualMemory vm_alloc( void* addr, ssize size )
{
	VirtualMemory vm;
	GEN_ASSERT( size > 0 );
	vm.data = VirtualAlloc( addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	vm.size = size;
	return vm;
}

b32 vm_free( VirtualMemory vm )
{
	MEMORY_BASIC_INFORMATION info;
	while ( vm.size > 0 )
	{
		if ( VirtualQuery( vm.data, &info, size_of( info ) ) == 0 )
			return false;
		if ( info.BaseAddress != vm.data || info.AllocationBase != vm.data || info.State != MEM_COMMIT || info.RegionSize > scast( usize, vm.size) )
		{
			return false;
		}
		if ( VirtualFree( vm.data, 0, MEM_RELEASE ) == 0 )
			return false;
		vm.data  = pointer_add( vm.data, info.RegionSize );
		vm.size -= info.RegionSize;
	}
	return true;
}

VirtualMemory vm_trim( VirtualMemory vm, ssize lead_size, ssize size )
{
	VirtualMemory new_vm = { 0 };
	void*             ptr;
	GEN_ASSERT( vm.size >= lead_size + size );

	ptr = pointer_add( vm.data, lead_size );

	vm_free( vm );
	new_vm = vm_alloc( ptr, size );
	if ( new_vm.data == ptr )
		return new_vm;
	if ( new_vm.data )
		vm_free( new_vm );
	return new_vm;
}

b32 vm_purge( VirtualMemory vm )
{
	VirtualAlloc( vm.data, vm.size, MEM_RESET, PAGE_READWRITE );
	// NOTE: Can this really fail?
	return true;
}

ssize virtual_memory_page_size( ssize* alignment_out )
{
	SYSTEM_INFO info;
	GetSystemInfo( &info );
	if ( alignment_out )
		*alignment_out = info.dwAllocationGranularity;
	return info.dwPageSize;
}

#else
#	include <sys/mman.h>

#	ifndef MAP_ANONYMOUS
#		define MAP_ANONYMOUS MAP_ANON
#	endif
VirtualMemory vm_alloc( void* addr, ssize size )
{
	VirtualMemory vm;
	GEN_ASSERT( size > 0 );
	vm.data = mmap( addr, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0 );
	vm.size = size;
	return vm;
}

b32 vm_free( VirtualMemory vm )
{
	munmap( vm.data, vm.size );
	return true;
}

VirtualMemory vm_trim( VirtualMemory vm, ssize lead_size, ssize size )
{
	void*  ptr;
	ssize trail_size;
	GEN_ASSERT( vm.size >= lead_size + size );

	ptr        = pointer_add( vm.data, lead_size );
	trail_size = vm.size - lead_size - size;

	if ( lead_size != 0 )
		vm_free( vm_from_memory(( vm.data, lead_size ) );
	if ( trail_size != 0 )
		vm_free( vm_from_memory( ptr, trail_size ) );
	return vm_from_memory( ptr, size );
}

b32 vm_purge( VirtualMemory vm )
{
	int err = madvise( vm.data, vm.size, MADV_DONTNEED );
	return err != 0;
}

ssize virtual_memory_page_size( ssize* alignment_out )
{
	// TODO: Is this always true?
	ssize result = scast( ssize, sysconf( _SC_PAGE_SIZE ));
	if ( alignment_out )
		*alignment_out = result;
	return result;
}
#endif

#pragma endregion VirtualMemory

void* arena_allocator_proc( void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags )
{
	Arena* arena = rcast(Arena*, allocator_data);
	void*      ptr   = NULL;

	// unused( old_size );

	switch ( type )
	{
		case EAllocation_ALLOC :
			{
				void* end        = pointer_add( arena->PhysicalStart, arena->TotalUsed );
				ssize total_size = align_forward_s64( size, alignment );

				// NOTE: Out of memory
				if ( arena->TotalUsed + total_size > (ssize) arena->TotalSize )
				{
					// zpl__printf_err("%s", "Arena out of memory\n");
					GEN_FATAL("Arena out of memory! (Possibly could not fit for the largest size Arena!!)");
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

void* pool_allocator_proc( void* allocator_data, AllocType type, ssize size, ssize alignment, void* old_memory, ssize old_size, u64 flags )
{
	Pool* pool = rcast( Pool*, allocator_data);
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

				next_free        = * rcast( uptr*, pool->FreeList);
				ptr              = pool->FreeList;
				pool->FreeList   = rcast( void*, next_free);
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

				next             = rcast( uptr*, old_memory);
				*next            = rcast( uptr, pool->FreeList);
				pool->FreeList   = old_memory;
				pool->TotalSize -= pool->BlockSize;
			}
			break;

		case EAllocation_FREE_ALL :
			{
				ssize    actual_block_size, block_index;
				void* curr;
				uptr* end;

				actual_block_size = pool->BlockSize + pool->BlockAlign;
				pool->TotalSize   = 0;

				// NOTE: Init intrusive freelist
				curr = pool->PhysicalStart;
				for ( block_index = 0; block_index < pool->NumBlocks - 1; block_index++ )
				{
					uptr* next = rcast( uptr*, curr);
					* next     = rcast( uptr, curr) + actual_block_size;
					curr       = pointer_add( curr, actual_block_size );
				}

				end            = rcast( uptr*, curr);
				* end          = scast( uptr, NULL);
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

Pool pool_init_align( AllocatorInfo backing, ssize num_blocks, ssize block_size, ssize block_align )
{
	Pool pool = {};

	ssize    actual_block_size, pool_size, block_index;
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

void pool_clear(Pool* pool)
{
	ssize actual_block_size, block_index;
	void* curr;
	uptr* end;

	actual_block_size = pool->BlockSize + pool->BlockAlign;

	curr = pool->PhysicalStart;
	for ( block_index = 0; block_index < pool->NumBlocks - 1; block_index++ )
	{
		uptr* next = ( uptr* ) curr;
		*next      = ( uptr  ) curr + actual_block_size;
		curr       = pointer_add( curr, actual_block_size );
	}

	end  =  ( uptr* ) curr;
	*end =  ( uptr )  NULL;

	pool->FreeList = pool->PhysicalStart;
}

#pragma endregion Memory
