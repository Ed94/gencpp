#define GEN_IMPLEMENTATION
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#include "gen.hpp"

int main()
{
    gen::log_fmt("FAIL forced assertion\n");
    return 1;
}
