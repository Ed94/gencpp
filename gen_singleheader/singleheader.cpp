#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#include "gen.cpp"
#include "helpers/push_ignores.inline.hpp"

#include <stdlib.h>

GEN_NS_BEGIN
#include "helpers/base_codegen.hpp"
#include "helpers/misc.hpp"
GEN_NS_END

using namespace gen;

constexpr char const* generation_notice =
"// This file was generated automatially by gencpp's singleheader.cpp"
"(See: https://github.com/Ed94/gencpp)\n\n";

constexpr Str implementation_guard_start = txt(R"(
#pragma region GENCPP IMPLEMENTATION GUARD
#if defined(GEN_IMPLEMENTATION) && ! defined(GEN_IMPLEMENTED)
#	define GEN_IMPLEMENTED
)");

constexpr Str implementation_guard_end = txt(R"(
#endif
#pragma endregion GENCPP IMPLEMENTATION GUARD
)");

constexpr Str roll_own_dependencies_guard_start = txt(R"(
//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
// Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES
)");

constexpr Str roll_own_dependencies_guard_end = txt(R"(
// GEN_ROLL_OWN_DEPENDENCIES
#endif
)");

global bool generate_gen_dep = true;
global bool generate_builder = true;
global bool generate_editor  = true;
global bool generate_scanner = true;

#define path_format_style "../scripts/.clang-format "
#define scratch_file      "gen/scratch.hpp"
#define path_base         "../base/"

Code format( Code code ) {
	return code_refactor_and_format(code, scratch_file, nullptr, path_format_style );
}

int gen_main()
{
	gen::init();

	Code push_ignores        = scan_file( path_base "helpers/push_ignores.inline.hpp" );
	Code pop_ignores         = scan_file( path_base "helpers/pop_ignores.inline.hpp" );
	Code single_header_start = scan_file( "components/header_start.hpp" );

	Builder
	header = Builder::open( "gen/gen.hpp" );
	header.print_fmt( generation_notice );
	header.print_fmt("#pragma once\n\n");
	header.print( push_ignores );

	// Headers
	{
		header.print( single_header_start );

		if ( generate_gen_dep )
		{
			Code platform     = scan_file( path_base "dependencies/platform.hpp" );
			Code macros       = scan_file( path_base "dependencies/macros.hpp" );
			Code basic_types  = scan_file( path_base "dependencies/basic_types.hpp" );
			Code debug        = scan_file( path_base "dependencies/debug.hpp" );
			Code memory	      = scan_file( path_base "dependencies/memory.hpp" );
			Code stirng_ops   = scan_file( path_base "dependencies/string_ops.hpp" );
			Code printing     = scan_file( path_base "dependencies/printing.hpp" );
			Code containers   = scan_file( path_base "dependencies/containers.hpp" );
			Code hashing 	  = scan_file( path_base "dependencies/hashing.hpp" );
			Code strings      = scan_file( path_base "dependencies/strings.hpp" );
			Code filesystem   = scan_file( path_base "dependencies/filesystem.hpp" );
			Code timing       = scan_file( path_base "dependencies/timing.hpp" );

			header.print_fmt( roll_own_dependencies_guard_start );
			header.print( platform );
			header.print_fmt( "\nGEN_NS_BEGIN\n" );

			header.print( macros );
			header.print( basic_types );
			header.print( debug );
			header.print( memory );
			header.print( stirng_ops );
			header.print( printing );
			header.print( containers );
			header.print( hashing );
			header.print( strings );
			header.print( filesystem );
			header.print( timing );

			if ( generate_scanner ) {
				header.print( scan_file( path_base "dependencies/parsing.hpp" ) );
			}

			header.print_fmt( "GEN_NS_END\n" );
			header.print_fmt( roll_own_dependencies_guard_end );
			header.print( fmt_newline );
		}

		Code types      = scan_file( path_base "components/types.hpp" );
		Code ast        = scan_file( path_base "components/ast.hpp" );
		Code ast_types  = scan_file( path_base "components/ast_types.hpp" );
		Code code_types = scan_file( path_base "components/code_types.hpp" );
		Code interface  = scan_file( path_base "components/interface.hpp" );
		Code inlines 	= scan_file( path_base "components/inlines.hpp" );
		Code header_end = scan_file( path_base "components/header_end.hpp" );

		CodeBody ecode       = gen_ecode     ( path_base "enums/ECodeTypes.csv" );
		CodeBody eoperator   = gen_eoperator ( path_base "enums/EOperator.csv" );
		CodeBody especifier  = gen_especifier( path_base "enums/ESpecifier.csv" );
		CodeBody ast_inlines = gen_ast_inlines();

		header.print_fmt( "GEN_NS_BEGIN\n\n" );

		header.print_fmt("#pragma region Types\n");
		header.print( types );
		header.print( fmt_newline );
		header.print( format( ecode ));
		header.print( fmt_newline );
		header.print( format( eoperator ));
		header.print( fmt_newline );
		header.print( format( especifier ));
		header.print( fmt_newline );
		header.print_fmt("#pragma endregion Types\n\n");

		header.print_fmt("#pragma region AST\n");
		header.print( ast );
		header.print( code_types );
		header.print( ast_types );
		header.print_fmt("\n#pragma endregion AST\n");

		header.print( interface );

		header.print_fmt( "\n#pragma region Inlines\n" );
		header.print( inlines );
		header.print( format( ast_inlines ));
		header.print( fmt_newline );
		header.print_fmt( "#pragma endregion Inlines\n" );

		header.print( header_end );

		if ( generate_builder ) {
			header.print( scan_file( path_base "auxillary/builder.hpp" ) );
		}

		header.print_fmt( "GEN_NS_END\n" );
	}

	// Implementation
	{
		header.print_fmt( "%s\n", (char const*) implementation_guard_start );

		if ( generate_gen_dep )
		{
			Code impl_start = scan_file( path_base "dependencies/src_start.cpp" );
			Code debug      = scan_file( path_base "dependencies/debug.cpp" );
			Code string_ops = scan_file( path_base "dependencies/string_ops.cpp" );
			Code printing   = scan_file( path_base "dependencies/printing.cpp" );
			Code memory     = scan_file( path_base "dependencies/memory.cpp" );
			Code hashing    = scan_file( path_base "dependencies/hashing.cpp" );
			Code strings    = scan_file( path_base "dependencies/strings.cpp" );
			Code filesystem = scan_file( path_base "dependencies/filesystem.cpp" );
			Code timing     = scan_file( path_base "dependencies/timing.cpp" );

			header.print_fmt( roll_own_dependencies_guard_start );
			header.print( impl_start );
			header.print_fmt( "GEN_NS_BEGIN\n\n");

			header.print( debug );
			header.print( string_ops );
			header.print( printing );
			header.print( memory );
			header.print( hashing );
			header.print( strings );
			header.print( filesystem );
			header.print( timing );

			if ( generate_scanner ) {
				header.print_fmt( "\n#pragma region Parsing\n" );
				header.print( scan_file( path_base "dependencies/parsing.cpp" ) );
				header.print_fmt( "#pragma endregion Parsing\n\n" );
			}

			header.print_fmt( "GEN_NS_END\n");
			header.print_fmt( roll_own_dependencies_guard_end );
		}

		Code static_data 	   = scan_file( path_base "components/static_data.cpp" );
		Code ast_case_macros   = scan_file( path_base "components/ast_case_macros.cpp" );
		Code ast               = scan_file( path_base "components/ast.cpp" );
		Code code              = scan_file( path_base "components/code_serialization.cpp" );
		Code interface         = scan_file( path_base "components/interface.cpp" );
		Code upfront           = scan_file( path_base "components/interface.upfront.cpp" );
		Code lexer             = scan_file( path_base "components/lexer.cpp" );
		Code parser            = scan_file( path_base "components/parser.cpp" );
		Code parsing_interface = scan_file( path_base "components/interface.parsing.cpp" );
		Code untyped           = scan_file( path_base "components/interface.untyped.cpp" );

		CodeBody etoktype = gen_etoktype( path_base "enums/ETokType.csv", path_base "enums/AttributeTokens.csv" );

		header.print_fmt( "\nGEN_NS_BEGIN\n");
		header.print( static_data );

		header.print_fmt( "#pragma region AST\n\n" );
		header.print( ast_case_macros );
		header.print( ast );
		header.print( code );
		header.print_fmt( "#pragma endregion AST\n\n" );

		header.print_fmt( "#pragma region Interface\n" );
		header.print( interface );
		header.print( upfront );
		header.print_fmt( "\n#pragma region Parsing\n\n" );
		header.print( format(etoktype) );
		header.print( lexer );
		header.print( parser );
		header.print( parsing_interface );
		header.print_fmt( "\n#pragma endregion Parsing\n" );
		header.print( untyped );
		header.print_fmt( "\n#pragma endregion Interface\n\n");

		if ( generate_builder ) {
			header.print( scan_file( path_base "auxillary/builder.cpp"  ) );
		}

		// Scanner header depends on implementation
		if ( generate_scanner ) {
			header.print( scan_file( path_base "auxillary/scanner.hpp" ) );
		}

		if ( generate_scanner ) {
			header.print( scan_file( path_base "auxillary/scanner.cpp" ) );
		}

		header.print_fmt( "GEN_NS_END\n");

		header.print_fmt( "%s\n", (char const*) implementation_guard_end );
	}

	header.print( pop_ignores );
	header.write();

	gen::deinit();
	return 0;
}
