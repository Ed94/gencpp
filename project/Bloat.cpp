#define BLOAT_IMPL
#include "Bloat.hpp"


void* gen_Arena::allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
{
	gen_Arena* arena = rcast(gen_Arena*, allocator_data);
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

				if ( flags & ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO )
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

void* gen_Pool::allocator_proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
{
	gen_Pool* pool = zpl_cast( gen_Pool* ) allocator_data;
	void* ptr  = NULL;

	// unused( old_size );

	switch ( type )
	{
		case EAllocation_ALLOC :
			{
				uptr next_free;

				ZPL_ASSERT( size == pool->BlockSize );
				ZPL_ASSERT( alignment == pool->BlockAlign );
				ZPL_ASSERT( pool->FreeList != NULL );

				next_free        = *zpl_cast( uptr* ) pool->FreeList;
				ptr              = pool->FreeList;
				pool->FreeList   = zpl_cast( void* ) next_free;
				pool->TotalSize += pool->BlockSize;

				if ( flags & ZPL_ALLOCATOR_FLAG_CLEAR_TO_ZERO )
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
			ZPL_PANIC( "You cannot resize something allocated by with a pool." );
			break;
	}

	return ptr;
}

gen_Pool gen_Pool::init_align( AllocatorInfo backing, sw num_blocks, sw block_size, sw block_align )
{
	gen_Pool pool = {};

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


namespace Memory
{
	using namespace zpl;

	global AllocatorInfo GlobalAllocator;

	global Array(Arena) Global_AllocatorBuckets;

	void* Global_Allocator_Proc( void* allocator_data, AllocType type, sw size, sw alignment, void* old_memory, sw old_size, u64 flags )
	{
		Arena* last = & array_back( Global_AllocatorBuckets );

		switch ( type )
		{
			case EAllocation_ALLOC:
			{
				if ( last->total_allocated + size > last->total_size )
				{
					Arena bucket;
					arena_init_from_allocator( & bucket, heap(), BucketSize );

					if ( bucket.physical_start == nullptr )
						fatal( "Failed to create bucket for Global_AllocatorBuckets");

					if ( ! array_append( Global_AllocatorBuckets, bucket ) )
						fatal( "Failed to append bucket to Global_AllocatorBuckets");

					last = & array_back( Global_AllocatorBuckets );
				}

				return alloc_align( arena_allocator( last), size, alignment );
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
				if ( last->total_allocated + size > last->total_size )
				{
					Arena bucket;
					arena_init_from_allocator( & bucket, heap(), BucketSize );

					if ( bucket.physical_start == nullptr )
						fatal( "Failed to create bucket for Global_AllocatorBuckets");

					if ( ! array_append( Global_AllocatorBuckets, bucket ) )
						fatal( "Failed to append bucket to Global_AllocatorBuckets");

					last = & array_back( Global_AllocatorBuckets );
				}

				void* result = alloc_align( arena_allocator( last), size, alignment );

				if ( result != nullptr && old_memory != nullptr )
				{
					mem_copy( result, old_memory, size );
				}

				return result;
			}
		}

		return nullptr;
	}

	void setup()
	{
		GlobalAllocator = AllocatorInfo { & Global_Allocator_Proc, nullptr };

		if ( ! array_init_reserve( Global_AllocatorBuckets, heap(), 128 ) )
			fatal( "Failed to reserve memory for Global_AllocatorBuckets");

		Arena bucket;
		arena_init_from_allocator( & bucket, heap(), BucketSize );

		if ( bucket.physical_start == nullptr )
			fatal( "Failed to create first bucket for Global_AllocatorBuckets");

		array_append( Global_AllocatorBuckets, bucket );
	}

	void cleanup()
	{
		s32 index = 0;
		s32 left  = array_count( Global_AllocatorBuckets );
		do
		{
			Arena* bucket = & Global_AllocatorBuckets[ index ];
			arena_free( bucket );
			index++;
		}
		while ( left--, left );

		array_free( Global_AllocatorBuckets );
	}
}
