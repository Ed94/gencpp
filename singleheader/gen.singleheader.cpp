#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#include "gen.cpp"
#include "file_processors/scanner.hpp"
#include "helpers/helper.hpp"

using namespace gen;

constexpr char const* generation_notice =
"// This file was generated automatially by gen.bootstrap.cpp "
"(See: https://github.com/Ed94/gencpp)\n\n";

constexpr StrC implementation_guard_start = txt_StrC(R"(
#pragma region GENCPP IMPLEMENTATION GUARD
#if defined(GEN_IMPLEMENTATION) && ! defined(GEN_IMPLEMENTED)
#	define GEN_IMPLEMENTED
)");

constexpr StrC implementation_guard_end = txt_StrC(R"(
#endif
#pragma endregion GENCPP IMPLEMENTATION GUARD
)");

constexpr StrC roll_own_dependencies_guard_start = txt_StrC(R"(
//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
// Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES

)");

constexpr StrC roll_own_dependencies_guard_end = txt_StrC(R"(
// GEN_ROLL_OWN_DEPENDENCIES
#endif
)");

global bool generate_gen_dep = true;
global bool generate_builder = true;
global bool generate_editor  = true;
global bool generate_scanner = true;

int gen_main()
{
	gen::init();

#define project_dir "../project/"

	Code push_ignores = scan_file( project_dir "helpers/push_ignores.inline.hpp" );
	Code pop_ignores  = scan_file( project_dir "helpers/pop_ignores.inline.hpp" );

	Code header_start = scan_file( "components/header_start.hpp" );

	Builder
	header = Builder::open( "gen/gen.hpp" );
	header.print( generation_notice );
	header.print( push_ignores );

	header.print_fmt("#pragma once\n\n");

	// Headers
	{
		header.print( header_start );

		if ( generate_gen_dep )
		{
			header.print_fmt( roll_own_dependencies_guard_start );

			Code header_start  = scan_file( project_dir "dependencies/header_start.hpp" );
			Code macros        = scan_file( project_dir "dependencies/macros.hpp" );
			Code basic_types   = scan_file( project_dir "dependencies/basic_types.hpp" );
			Code debug         = scan_file( project_dir "dependencies/debug.hpp" );
			Code memory	       = scan_file( project_dir "dependencies/memory.hpp" );
			Code string_ops    = scan_file( project_dir "dependencies/string_ops.hpp" );
			Code printing      = scan_file( project_dir "dependencies/printing.hpp" );
			Code containers    = scan_file( project_dir "dependencies/containers.hpp" );
			Code hashing 	   = scan_file( project_dir "dependencies/hashing.hpp" );
			Code string        = scan_file( project_dir "dependencies/string.hpp" );
			Code file_handling = scan_file( project_dir "dependencies/file_handling.hpp" );
			Code timing        = scan_file( project_dir "dependencies/timing.hpp" );

			header.print( header_start );
			header.print_fmt( "GEN_NS_BEGIN\n\n" );
			header.print( macros );
			header.print( basic_types );
			header.print( debug );
			header.print( memory );
			header.print( string_ops );
			header.print( printing );
			header.print( containers );
			header.print( hashing );
			header.print( string );
			header.print( file_handling );
			header.print( timing );
			header.print_fmt( "GEN_NS_END\n" );

			header.print_fmt( roll_own_dependencies_guard_end );
		}

		Code types        = scan_file( project_dir "components/types.hpp" );
		Code ast          = scan_file( project_dir "components/ast.hpp" );
		Code ast_types    = scan_file( project_dir "components/ast_types.hpp" );
		Code interface    = scan_file( project_dir "components/interface.hpp" );
		Code inlines 	  = scan_file( project_dir "components/inlines.hpp" );
		Code ast_inlines  = scan_file( project_dir "components/temp/ast_inlines.hpp" );
		Code header_end   = scan_file( project_dir "components/header_end.hpp" );

		CodeBody ecode      = gen_ecode     ( project_dir "enums/ECode.csv" );
		CodeBody eoperator  = gen_eoperator ( project_dir "enums/EOperator.csv" );
		CodeBody especifier = gen_especifier( project_dir "enums/ESpecifier.csv" );

		header.print_fmt( "GEN_NS_BEGIN\n\n" );

		header.print_fmt("#pragma region Types\n\n");
		header.print( types );
		header.print( ecode );
		header.print( eoperator );
		header.print( especifier );
		header.print_fmt("#pragma endregion Types\n\n");

		header.print_fmt("#pragma region AST\n\n");
		header.print( ast );
		header.print( ast_types );
		header.print_fmt("#pragma endregion AST\n\n");

		header.print( interface );

		header.print_fmt( inlines );
		header.print_fmt( ast_inlines );

		header.print( header_end );
		header.print_fmt( "GEN_NS_END\n" );
	}

	// Implementation
	{
		header.print_fmt( "%s\n", (char const*) implementation_guard_start );

		if ( generate_gen_dep )
		{
			Code impl_start    = scan_file( project_dir "dependencies/impl_start.cpp" );
			Code debug         = scan_file( project_dir "dependencies/debug.cpp" );
			Code string_ops    = scan_file( project_dir "dependencies/string_ops.cpp" );
			Code printing      = scan_file( project_dir "dependencies/printing.cpp" );
			Code memory        = scan_file( project_dir "dependencies/memory.cpp" );
			Code parsing       = scan_file( project_dir "dependencies/parsing.cpp" );
			Code hashing       = scan_file( project_dir "dependencies/hashing.cpp" );
			Code string        = scan_file( project_dir "dependencies/string.cpp" );
			Code file_handling = scan_file( project_dir "dependencies/file_handling.cpp" );
			Code timing        = scan_file( project_dir "dependencies/timing.cpp" );

			header.print_fmt( roll_own_dependencies_guard_start );
			header.print_fmt( "GEN_NS_BEGIN\n\n");

			header.print( impl_start );
			header.print( debug );
			header.print( string_ops );
			header.print( printing );
			header.print( memory );
			header.print( parsing );
			header.print( hashing );
			header.print( string );
			header.print( file_handling );
			header.print( timing );

			header.print_fmt( "GEN_NS_END\n");
			header.print_fmt( roll_own_dependencies_guard_end );
		}

		Code data 	         = scan_file( project_dir "components/static_data.cpp" );
		Code ast_case_macros = scan_file( project_dir "components/ast_case_macros.cpp" );
		Code ast             = scan_file( project_dir "components/ast.cpp" );
		Code interface       = scan_file( project_dir "components/interface.cpp" );
		Code upfront         = scan_file( project_dir "components/interface.upfront.cpp" );
		Code parsing         = scan_file( project_dir "components/interface.parsing.cpp" );
		Code untyped         = scan_file( project_dir "components/untyped.cpp" );

		CodeBody      etoktype      = gen_etoktype( project_dir "enums/ETokType.csv", project_dir "enums/AttributeTokens.csv" );
		CodeNamespace parser_nspace = def_namespace( name(Parser), def_namespace_body( args(etoktype)) );

		header.print_fmt( "GEN_NS_BEGIN\n\n");
		header.print( data );
		header.print( ast_case_macros );
		header.print( ast );
		header.print( interface );
		header.print( upfront );
		header.print( parser_nspace );
		header.print( parsing );
		header.print( untyped );
		header.print_fmt( "GEN_NS_END\n");

		header.print_fmt( "%s\n", (char const*) implementation_guard_end );
	}

	header.print( pop_ignores );
	header.write();

	gen::deinit();
	return 0;
}
