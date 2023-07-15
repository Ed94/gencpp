#ifdef gen_time
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_FEATURE_PARSING
#include "gen.cpp"
#include "Upfront\Array.Upfront.hpp"
#include "Upfront\Buffer.Upfront.hpp"
#include "Upfront\HashTable.Upfront.hpp"
#include "Upfront\Ring.Upfront.hpp"
#include "Upfront\Sanity.Upfront.hpp"



using namespace gen;


int gen_main()
{
	Memory::setup();
	gen::init();

	gen_sanity_upfront();

	gen_array( u8 );
	gen_array( sw );

	gen_buffer( u8 );

	gen_hashtable( u32 );

	gen_ring( s16 );

	gen_array_file();
	gen_buffer_file();
	gen_hashtable_file();
	gen_ring_file();

	gen::deinit();
	Memory::cleanup();
	return 0;
}
#endif


#ifdef runtime
int main()
{
	return 0;
}
#endif
