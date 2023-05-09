#include "Bloat.cpp"


#ifdef gen_time
#include "gen.cpp"

using namespace gen;





int gen_main()
{
	Memory::setup();

	gen::init();

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
