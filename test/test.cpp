#include "math.hpp"


#ifdef gen_time
u32 gen_main()
{
	return gen_math();
}

#include "gen.cpp"
#endif


#ifndef gen_time
int main()
{
	u32 result = square(5);

	zpl_printf("TEST RESULT: %d", result);
}
#endif
