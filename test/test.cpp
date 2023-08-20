#if GEN_TIME
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_BENCHMARK
#include "gen.cpp"
#include "gen.builder.cpp"
#include "sanity.cpp"
#include "SOA.cpp"
#include "test.singleheader_ast.cpp"

int gen_main()
{
	using namespace gen;
	log_fmt("\ngen_time:");

	// check_sanity();

	// check_SOA();

	check_singleheader_ast();

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
