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

	// gen_dep.hpp
	{
		Code header_start  = scan_file( "dependencies/gen.header_start.hpp" );
		Code nspace_macro  = untyped_str( namespace_by_default ? nspace_default : nspace_non_default );
		Code macros 	   = scan_file( "dependencies/gen.macros.hpp" );
		Code basic_types   = scan_file( "dependencies/gen.basic_types.hpp" );
		Code debug         = scan_file( "dependencies/gen.debug.hpp" );
		Code memory	       = scan_file( "dependencies/gen.memory.hpp" );
		Code string_ops    = scan_file( "dependencies/gen.string_ops.hpp" );
		Code printing      = scan_file( "dependencies/gen.printing.hpp" );
		Code containers    = scan_file( "dependencies/gen.containers.hpp" );
		Core hashing 	   = scan_file( "dependencies/gen.hashing.hpp" );
		Code string        = scan_file( "dependencies/gen.string.hpp" );
		Code file_handling = scan_file( "dependencies/gen.file_handling.hpp" );
		Code parsing       = scan_file( "dependencies/gen.parsing.hpp" );
		Code timing        = scan_file( "dependencies/gen.timing.hpp" );

		Builder
		deps_header;
		deps_header.open("gen/gen_dep.hpp");
			deps_header.print_fmt("// This file is intended to be included within gen.hpp (There is no pragma diagnostic ignores)\n\n");
			deps_header.print_fmt("#pragma once\n\n");
			deps_header.print( header_start );
			deps_header.print( nspace_macro );
			deps_header.print_fmt( "GEN_NS_BEGIN\n\n");
				deps_header.print( macros );
				deps_header.print( basic_types );
				deps_header.print( debug );
				deps_header.print( memory );
				deps_header.print( string_ops );
				deps_header.print( printing );
				deps_header.print( containers );
				deps_header.print( hashing );
				deps_header.print( string );
				deps_header.print( file_handling );
				deps_header.print( parsing );
				deps_header.print( timing );
			deps_header.print_fmt( "GEN_NS_END\n\n");
		deps_header.write();
	}

	// gen_dep.cpp
	{
		CodeInclude header     = def_include( txt_StrC("gen_dep.hpp") );
		Code        impl_start = scan_file( "dependencies/gen.impl_start.cpp" );
		Code 	    debug      = scan_file( "dependencies/gen.debug.cpp" );
		Code 	    string_ops = scan_file( "dependencies/gen.string_ops.cpp" );
		Code 	    printing   = scan_file( "dependencies/gen.printing.cpp" );
		Code 	    memory     = scan_file( "dependencies/gen.memory.cpp" );
		Code        parsing    = scan_file( "dependencies/gen.parsing.cpp" );
		Code        hashing    = scan_file( "dependencies/gen.hashing.cpp" );
		Code        string     = scan_file( "dependencies/gen.string.cpp" );
		Code        timing     = scan_file( "dependencies/gen.timing.cpp" );

		Builder
		deps_impl;
		deps_impl.open("gen/gen_dep.cpp");
			deps_impl.print_fmt("// This file is intended to be included within gen.cpp (There is no pragma diagnostic ignores)\n\n");
			deps_impl.print( impl_start );
			deps_impl.print( header );
			deps_impl.print_fmt( "\nGEN_NS_BEGIN\n");
				deps_impl.print( debug );
				deps_impl.print( string_ops );
				deps_impl.print( printing );
				deps_impl.print( memory );
				deps_impl.print( parsing );
				deps_impl.print( string );
				deps_impl.print( timing );
			deps_impl.print_fmt( "GEN_NS_END\n\n");
		deps_impl.write();
	}

	// gen.hpp
	{
		Code header_start = scan_file( "components/gen.header_start.hpp" );
		Code nspace_macro = untyped_str( namespace_by_default ? nspace_default : nspace_non_default );
		Code types        = scan_file( "components/gen.types.hpp" );
		Code data_structs = scan_file( "components/gen.data_structures.hpp" );
		Code interface    = scan_file( "components/gen.interface.hpp" );
		Code header_end   = scan_file( "components/gen.header_end.hpp" );

		Code builder = scan_file( "filesystem/gen.builder.hpp" );

		Builder
		header;
		header.open( "gen/gen.hpp" );
			header.print_fmt("#pragma once\n\n");
			header.print( push_ignores );
			header.print( header_start );
			header.print( nspace_macro );
			header.print_fmt( "GEN_NS_BEGIN\n\n");
				header.print( types );
				header.print( data_structs );
				header.print( interface );
				header.print( header_end );
					header.print( builder );
			header.print_fmt( "GEN_NS_END\n\n");
			header.print( pop_ignores );
		header.write();
	}

	// gen.cpp
	{
		Code        impl_start      = scan_file( "components/gen.impl_start.cpp" );
		CodeInclude header          = def_include( txt_StrC("gen.hpp") );
		Code        data 	        = scan_file( "components/gen.data.cpp" );
		Code        ast_case_macros = scan_file( "components/gen.ast_case_macros.cpp" );
		Code        ast			    = scan_file( "components/gen.ast.cpp" );
		Code        interface	    = scan_file( "components/gen.interface.cpp" );
		Code        upfront 	    = scan_file( "components/gen.interface.upfront.cpp" );
		Code 	    parsing 	    = scan_file( "components/gen.interface.parsing.cpp" );
		Code        untyped 	    = scan_file( "components/gen.untyped.cpp" );

		Code builder = scan_file( "filesystem/gen.builder.cpp" );

		Builder
		impl;
		impl.open( "gen/gen.cpp" );
			impl.print( push_ignores );
			impl.print( impl_start );
			impl.print( header );
			impl.print_fmt( "\nGEN_NS_BEGIN\n\n");
				impl.print( data );
				impl.print( ast_case_macros );
				impl.print( ast );
				impl.print( interface );
				impl.print( upfront );
				impl.print( parsing );
				impl.print( untyped );
					impl.print( builder );
			impl.print_fmt( "GEN_NS_END\n\n");
			impl.print( pop_ignores );
		impl.write();
	}

	gen::deinit();
	return 0;
}
