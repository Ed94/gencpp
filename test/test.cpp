#include "Bloat.cpp"
#include "math.hpp"


#ifdef gen_time
#include "gen.cpp"

int gen_main()
{
	Memory::setup();

	zpl_printf("\nPress any key after attaching to process\n");
	getchar();

	gen::init()

	int result = gen_math();

	Memory::cleanup();
	return result;
}
#endif


#ifndef gen_time
#include "math.hpp"

int main()
{
	u32 result = square( 5U );

	zpl_printf("TEST RESULT: %d", result);
}
#endif
