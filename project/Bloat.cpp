#define BLOAT_IMPL
#include "Bloat.hpp"


namespace Memory
{
	using namespace zpl;

	Arena Global_Arena {};

	void setup()
	{
		arena_init_from_allocator( & Global_Arena, heap(), Initial_Reserve );

		if ( Global_Arena.total_size == 0 )
		{
			assert_crash( "Failed to reserve memory for Tests:: Global_Arena" );
		}
	}

	void resize( uw new_size )
	{
		void* new_memory = resize( heap(), Global_Arena.physical_start, Global_Arena.total_size, new_size );

		if ( new_memory == nullptr )
		{
			fatal("Failed to resize global arena!");
		}

		Global_Arena.physical_start = new_memory;
		Global_Arena.total_size     = new_size;
	}

	void cleanup()
	{
		arena_free( & Global_Arena);
	}
}
