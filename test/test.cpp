#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_BENCHMARK
#include "gen.cpp"
#include "sanity.cpp"
#include "SOA.cpp"

#if GEN_TIME
int gen_main()
{
	using namespace gen;
	log_fmt("\ngen_time:");

	// check_sanity();

	check_SOA();

	return 0;
}
#endif


// This only has to be done if symbol conflicts occur.
#ifndef GEN_TIME
int main()
{


	return 0;
}
#endif
