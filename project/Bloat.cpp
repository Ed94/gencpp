#define BLOAT_IMPL
#include "Bloat.hpp"


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
			case EAllocationALLOC:
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
			case EAllocationFREE:
			{
				// Doesn't recycle.
			}
			case EAllocationFREE_ALL:
			{
				// Memory::cleanup instead.
			}
			case EAllocationRESIZE:
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
