#define GEN_FEATURE_PARSING
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_BENCHMARK
#include "gen.cpp"
#include "sanity.cpp"
#include "SOA.cpp"

#if gen_time
int gen_main()
{
	using namespace gen;
	log_fmt("\ngen_time:");

	check_sanity();

	check_SOA();

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
