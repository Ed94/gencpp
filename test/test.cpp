#include "Bloat.cpp"


#ifdef gentime
#include "gen.cpp"

int gen_main()
{
	Memory::setup();

	zpl_printf("\nPress any key after attaching to process\n");
	getchar();

	gen::init();

	Memory::cleanup();
	return result;
}
#endif


#ifdef runtime
int main()
{
	return 0;
}
#endif
