#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "printing.cpp"
#endif

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

#pragma region VirtualMemory
VirtualMemory vm_from_memory( void* data, sw size )
{
	VirtualMemory vm;
	vm.data = data;
	vm.size = size;
	return vm;
}

#if defined( GEN_SYSTEM_WINDOWS )
VirtualMemory vm_alloc( void* addr, sw size )
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
		if ( info.BaseAddress != vm.data || info.AllocationBase != vm.data || info.State != MEM_COMMIT || info.RegionSize > zpl_cast( uw ) vm.size )
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

VirtualMemory vm_trim( VirtualMemory vm, sw lead_size, sw size )
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

sw virtual_memory_page_size( sw* alignment_out )
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
VirtualMemory vm_alloc( void* addr, sw size )
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

VirtualMemory vm_trim( VirtualMemory vm, sw lead_size, sw size )
{
	void*  ptr;
	sw trail_size;
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

sw virtual_memory_page_size( sw* alignment_out )
{
	// TODO: Is this always true?
	sw result = zpl_cast( sw ) sysconf( _SC_PAGE_SIZE );
	if ( alignment_out )
		*alignment_out = result;
	return result;
}
#endif

#pragma endregion VirtualMemory

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
