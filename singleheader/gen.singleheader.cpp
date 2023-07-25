#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#include "gen.cpp"
#include "filesystem/gen.scanner.hpp"

using namespace gen;

bool namespace_by_default = true;

constexpr StrC nspace_default = txt_StrC(R"(
#if defined(GEN_DONT_USE_NAMESPACE) && ! defined(GEN_NS_BEGIN)
#	define GEN_NS_BEGIN
#	define GEN_NS_END
#elif ! defined(GEN_NS_BEGIN)
#  define GEN_NS_BEGIN namespace gen {
#  define GEN_NS_END   }
#endif
)");

constexpr StrC nspace_non_default = txt_StrC(R"(
#if ! defined(GEN_USE_NAMESPACE) && ! defined(GEN_NS_BEGIN)
#	define GEN_NS_BEGIN
#	define GEN_NS_END
#elif ! defined(GEN_NS_BEGIN)
#  define GEN_NS_BEGIN namespace gen {
#  define GEN_NS_END   }
#endif
)");

constexpr StrC gen_implementation_guard = txt_StrC(R"(
#pragma region GENCPP IMPLEMENTATION GUARD
#if ! defined(GEN_IMPLEMENTATION)
#	define GEN_IMPLEMENTATION
)");

constexpr StrC gen_implementation_end = txt_StrC(R"(
#endif
#pragma endregion GENCPP IMPLEMENTATION GUARD
)");

int gen_main()
{
	gen::init();

	Code push_ignores = scan_file( "../project/helpers/gen.push_ignores.inline.hpp" );
	Code pop_ignores  = scan_file( "../project/helpers/gen.pop_ignores.inline.hpp" );

	Code header_start = scan_file( "components/gen.header_start.hpp" );
	Code nspace_macro = untyped_str( namespace_by_default ? nspace_default : nspace_non_default );

	Builder gen_header;
	gen_header.open( "gen/gen.hpp" );
	gen_header.print( push_ignores );

	// Headers
		gen_header.print_fmt("#pragma once\n\n");
		gen_header.print( header_start );
		gen_header.print( nspace_macro );
		gen_header.print_fmt( "GEN_NS_BEGIN\n");

		gen_header.print_fmt( "\nGEN_NS_END\n");

	// Implementation

		gen_header.print_fmt( "%s\n\n", (char const*) gen_implementation_guard );
		gen_header.print_fmt( "GEN_NS_BEGIN\n");

		gen_header.print_fmt( "\nGEN_NS_END\n");
		gen_header.print_fmt( "%s\n", (char const*) gen_implementation_end );

	gen_header.print( pop_ignores );
	gen_header.write();

	gen::deinit();
	return 0;
}
