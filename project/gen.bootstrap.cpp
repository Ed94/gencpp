#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#include "gen.cpp"
#include "filesystem/scanner.hpp"
#include "helpers/helper.hpp"

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

	Code push_ignores = scan_file( "helpers/push_ignores.inline.hpp" );
	Code pop_ignores  = scan_file( "helpers/pop_ignores.inline.hpp" );

	// gen_dep.hpp
	{
		Code header_start  = scan_file( "dependencies/header_start.hpp" );
		Code nspace_macro  = untyped_str( namespace_by_default ? nspace_default : nspace_non_default );
		Code macros 	   = scan_file( "dependencies/macros.hpp" );
		Code basic_types   = scan_file( "dependencies/basic_types.hpp" );
		Code debug         = scan_file( "dependencies/debug.hpp" );
		Code memory	       = scan_file( "dependencies/memory.hpp" );
		Code string_ops    = scan_file( "dependencies/string_ops.hpp" );
		Code printing      = scan_file( "dependencies/printing.hpp" );
		Code containers    = scan_file( "dependencies/containers.hpp" );
		Code hashing 	   = scan_file( "dependencies/hashing.hpp" );
		Code string        = scan_file( "dependencies/string.hpp" );
		Code parsing       = scan_file( "dependencies/parsing.hpp" );
		Code timing        = scan_file( "dependencies/timing.hpp" );

		// TOOD : Make this optional
		Code file_handling = scan_file( "dependencies/file_handling.hpp" );

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
		Code        impl_start = scan_file( "dependencies/impl_start.cpp" );
		Code 	    debug      = scan_file( "dependencies/debug.cpp" );
		Code 	    string_ops = scan_file( "dependencies/string_ops.cpp" );
		Code 	    printing   = scan_file( "dependencies/printing.cpp" );
		Code 	    memory     = scan_file( "dependencies/memory.cpp" );
		Code        parsing    = scan_file( "dependencies/parsing.cpp" );
		Code        hashing    = scan_file( "dependencies/hashing.cpp" );
		Code        string     = scan_file( "dependencies/string.cpp" );
		Code        timing     = scan_file( "dependencies/timing.cpp" );

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
			deps_impl.print( hashing );
			deps_impl.print( memory );
			deps_impl.print( parsing );
			deps_impl.print( string );
			deps_impl.print( timing );

			deps_impl.print_fmt( "GEN_NS_END\n\n");
		deps_impl.write();
	}

	// gen.hpp
	{
		Code header_start = scan_file( "components/header_start.hpp" );
		Code nspace_macro = untyped_str( namespace_by_default ? nspace_default : nspace_non_default );
		Code types        = scan_file( "components/types.hpp" );
		Code data_structs = scan_file( "components/data_structures.hpp" );
		Code interface    = scan_file( "components/interface.hpp" );
		Code header_end   = scan_file( "components/header_end.hpp" );

		CodeBody ecode      = gen_ecode( "enums/ECode.csv" );
		CodeBody eoperator  = gen_eoperator( "enums/EOperator.csv" );
		CodeBody especifier = gen_especifier( "enums/ESpecifier.csv" );

		// TODO : Make this optional to include
		Code builder = scan_file( "file_proecessors/builder.hpp" );

		Builder
		header;
		header.open( "gen/gen.hpp" );
			header.print_fmt("#pragma once\n\n");
			header.print( push_ignores );
			header.print( header_start );
			header.print( nspace_macro );
			header.print_fmt( "GEN_NS_BEGIN\n\n");

			header.print_fmt("#pragma region Types\n\n");
			header.print( types );
			header.print( ecode );
			header.print( eoperator );
			header.print( especifier );
			header.print_fmt("#pragma endregion Types\n\n");

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
		Code        data 	        = scan_file( "components/static_data.cpp" );
		Code        ast_case_macros = scan_file( "components/ast_case_macros.cpp" );
		Code        ast			    = scan_file( "components/ast.cpp" );
		Code        interface	    = scan_file( "components/interface.cpp" );
		Code        upfront 	    = scan_file( "components/interface.upfront.cpp" );
		Code 	    parsing 	    = scan_file( "components/interface.parsing.cpp" );
		Code        untyped 	    = scan_file( "components/untyped.cpp" );

		CodeBody etoktype = gen_etoktype( "enums/ETokType.csv", "enums/AttributeTokens.csv" );
		CodeNamespace parser_nspace = def_namespace( name(Parser), def_namespace_body( args(etoktype)) );

		// TODO : Make this optional to include
		Code builder = scan_file( "file_proecessors/builder.cpp" );

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
			impl.print( parser_nspace );
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
