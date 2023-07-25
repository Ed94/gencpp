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

int gen_main()
{
	gen::init();

	Code push_ignores = scan_file( "helpers/gen.push_ignores.inline.hpp" );
	Code pop_ignores  = scan_file( "helpers/gen.pop_ignores.inline.hpp" );

	Builder gen_deps_header;
	{

	}

	Builder gen_deps_impl;
	{

	}

	Builder gen_header;
	{
		Code header_start = scan_file( "components/gen.header_start.hpp" );
		Code nspace_macro = untyped_str( namespace_by_default ? nspace_default : nspace_non_default );

		gen_header.open( "gen/gen.hpp" );
			gen_header.print_fmt("#pragma once\n\n");
			gen_header.print( push_ignores );
			gen_header.print( header_start );
			gen_header.print( nspace_macro );
			gen_header.print_fmt( "GEN_NS_BEGIN\n");

			gen_header.print_fmt( "\nGEN_NS_END\n\n");
			gen_header.print( pop_ignores );
		gen_header.write();
	}

	Builder gen_impl;
	{

	}

	gen::deinit();
	return 0;
}
