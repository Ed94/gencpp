#ifdef GEN_TIME
#define GEN_FEATURE_PARSING
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_BENCHMARK
#include "gen.cpp"
#include "Array.Upfront.hpp"
#include "Buffer.Upfront.hpp"
#include "HashTable.Upfront.hpp"
#include "Ring.Upfront.hpp"
#include "Sanity.Upfront.hpp"


using namespace gen;


int gen_main()
{
	gen::init();

	gen_sanity_upfront();

	gen_array( u8 );
	gen_array( ssize );

	gen_buffer( u8 );

	gen_hashtable( u32 );

	gen_ring( s16 );

	gen_array_file();
	gen_buffer_file();
	gen_hashtable_file();
	gen_ring_file();

	gen::deinit();
	return 0;
}
#endif


#ifdef runtime
int main()
{
	return 0;
}
#endif
