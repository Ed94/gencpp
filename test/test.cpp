#include "gen.cpp"
#include "sanity.cpp"

#if gen_time
int gen_main()
{
	using namespace gen;
	log_fmt("\ngen_time:");

	check_sanity();

	return 0;
}
#endif


// This only has to be done if symbol conflicts occur.
#ifndef gen_time
int main()
{


	return 0;
}
#endif
