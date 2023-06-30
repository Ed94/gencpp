#include "Bloat.cpp"
#include "NonParsed\Array.NonParsed.hpp"
#include "NonParsed\Buffer.NonParsed.hpp"
#include "NonParsed\Sanity.hpp"


#ifdef gen_time
#include "gen.cpp"

using namespace gen;


int gen_main()
{
	Memory::setup();
	gen::init();

	// gen_sanity();

	gen_array( u8 );
	// gen_array( sw );
	gen_array_file();

	gen_buffer( u8 );
	gen_buffer_file();

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
