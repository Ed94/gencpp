#ifdef GEN_TIME
#define GEN_FEATURE_PARSING
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_BENCHMARK
#include "gen.hpp"

void check_upfront()
{
	using namespace gen;
	log_fmt("\nupfront: ");
	gen::init();

	// TODO

	gen::deinit();
	log_fmt("Passed!\n");
}

#endif