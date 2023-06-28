#include "Bloat.cpp"
#include "NonParsed\Array.NonParsed.hpp"
#include "NonParsed\Sanity.hpp"


#ifdef gen_time
#include "gen.cpp"

using namespace gen;




int gen_main()
{
	Memory::setup();
	gen::init();

	gen_sanity();
	// gen_array_file();

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
