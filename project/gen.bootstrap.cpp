#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#include "gen.cpp"

#include "helpers/helper.hpp"

GEN_NS_BEGIN
#include "dependencies/parsing.cpp"
GEN_NS_END

#include "file_processors/builder.hpp"
#include "file_processors/builder.cpp"
#include "file_processors/scanner.hpp"

using namespace gen;

constexpr char const* generation_notice =
"// This file was generated automatially by gen.bootstrap.cpp "
"(See: https://github.com/Ed94/gencpp)\n\n";

int gen_main()
{
	gen::init();

	Code push_ignores = scan_file( "helpers/push_ignores.inline.hpp" );
	Code pop_ignores  = scan_file( "helpers/pop_ignores.inline.hpp" );

	// gen_dep.hpp
	{
		Code header_start  = scan_file( "dependencies/header_start.hpp" );
		Code macros 	   = scan_file( "dependencies/macros.hpp" );
		Code basic_types   = scan_file( "dependencies/basic_types.hpp" );
		Code debug         = scan_file( "dependencies/debug.hpp" );
		Code memory	       = scan_file( "dependencies/memory.hpp" );
		Code string_ops    = scan_file( "dependencies/string_ops.hpp" );
		Code printing      = scan_file( "dependencies/printing.hpp" );
		Code containers    = scan_file( "dependencies/containers.hpp" );
		Code hashing 	   = scan_file( "dependencies/hashing.hpp" );
		Code string        = scan_file( "dependencies/string.hpp" );
		Code file_handling = scan_file( "dependencies/file_handling.hpp" );
		Code timing        = scan_file( "dependencies/timing.hpp" );

		Builder
		deps_header = Builder::open("gen/gen.dep.hpp");
		deps_header.print_fmt( generation_notice );
		deps_header.print_fmt( "// This file is intended to be included within gen.hpp (There is no pragma diagnostic ignores)\n\n" );
		deps_header.print( header_start );
		deps_header.print_fmt( "GEN_NS_BEGIN\n\n" );

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
		deps_header.print( timing );

		deps_header.print_fmt( "GEN_NS_END\n\n" );
		deps_header.write();
	}

	// gen_dep.cpp
	{
		Code src_start     = scan_file( "dependencies/src_start.cpp" );
		Code debug         = scan_file( "dependencies/debug.cpp" );
		Code string_ops    = scan_file( "dependencies/string_ops.cpp" );
		Code printing      = scan_file( "dependencies/printing.cpp" );
		Code memory        = scan_file( "dependencies/memory.cpp" );
		Code hashing       = scan_file( "dependencies/hashing.cpp" );
		Code string        = scan_file( "dependencies/string.cpp" );
		Code file_handling = scan_file( "dependencies/file_handling.cpp" );
		Code timing        = scan_file( "dependencies/timing.cpp" );

		Builder
		deps_impl = Builder::open( "gen/gen.dep.cpp" );
		deps_impl.print_fmt( generation_notice );
		deps_impl.print_fmt( "// This file is intended to be included within gen.cpp (There is no pragma diagnostic ignores)\n\n" );
		deps_impl.print( src_start );
		deps_impl.print_fmt( "GEN_NS_BEGIN\n\n" );

		deps_impl.print( debug );
		deps_impl.print( string_ops );
		deps_impl.print( printing );
		deps_impl.print( hashing );
		deps_impl.print( memory );
		deps_impl.print( string );
		deps_impl.print( file_handling );
		deps_impl.print( timing );

		deps_impl.print_fmt( "GEN_NS_END\n\n" );
		deps_impl.write();
	}

	// gen.hpp
	{
		Code header_start = scan_file( "components/header_start.hpp" );
		Code types        = scan_file( "components/types.hpp" );
		Code ast          = scan_file( "components/ast.hpp" );
		Code ast_types    = scan_file( "components/ast_types.hpp" );
		Code interface    = scan_file( "components/interface.hpp" );
		Code inlines      = scan_file( "components/inlines.hpp" );
		Code ast_inlines  = scan_file( "components/temp/ast_inlines.hpp" );
		Code header_end   = scan_file( "components/header_end.hpp" );

		CodeBody ecode      = gen_ecode     ( "enums/ECode.csv" );
		CodeBody eoperator  = gen_eoperator ( "enums/EOperator.csv" );
		CodeBody especifier = gen_especifier( "enums/ESpecifier.csv" );

		Builder
		header = Builder::open( "gen/gen.hpp" );
		header.print_fmt( generation_notice );
		header.print_fmt( "#pragma once\n\n" );
		header.print( push_ignores );
		header.print( header_start );
		header.print_fmt( "GEN_NS_BEGIN\n\n" );

		header.print_fmt( "#pragma region Types\n\n" );
		header.print( types );
		header.print( ecode );
		header.print( eoperator );
		header.print( especifier );
		header.print_fmt( "#pragma endregion Types\n\n" );

		header.print_fmt( "#pragma region AST\n\n" );
		header.print( ast );
		header.print( ast_types );
		header.print_fmt( "#pragma endregion AST\n\n" );

		header.print( interface );

		header.print_fmt( "#pragma region Inlines\n\n" );
		header.print( inlines );
		header.print( ast_inlines );
		header.print_fmt( "#pragma endregion Inlines\n\n" );

		header.print( header_end );
		header.print_fmt( "GEN_NS_END\n\n" );
		header.print( pop_ignores );
		header.write();
	}

	// gen.cpp
	{
		Code        src_start       = scan_file( "components/src_start.cpp" );
		Code        static_data 	= scan_file( "components/static_data.cpp" );
		Code        ast_case_macros = scan_file( "components/ast_case_macros.cpp" );
		Code        ast			    = scan_file( "components/ast.cpp" );
		Code        interface	    = scan_file( "components/interface.cpp" );
		Code        upfront 	    = scan_file( "components/interface.upfront.cpp" );
		Code 	    parsing 	    = scan_file( "components/interface.parsing.cpp" );
		Code        untyped 	    = scan_file( "components/untyped.cpp" );

		CodeBody      etoktype      = gen_etoktype( "enums/ETokType.csv", "enums/AttributeTokens.csv" );
		CodeNamespace parser_nspace = def_namespace( name(Parser), def_namespace_body( args(etoktype)) );

		Builder
		src = Builder::open( "gen/gen.cpp" );
		src.print_fmt( generation_notice );
		src.print( push_ignores );
		src.print( src_start );
		src.print_fmt( "\nGEN_NS_BEGIN\n\n");

		src.print( static_data );

		src.print_fmt( "#pragma region AST\n\n" );
		src.print( ast_case_macros );
		src.print( ast );
		src.print_fmt( "#pragma endregion AST\n\n" );

		src.print_fmt( "#pragma region Interface\n\n" );
		src.print( interface );
		src.print( upfront );
		src.print_fmt( "#pragma region Parsing\n\n" );
		src.print( parser_nspace );
		src.print( parsing );
		src.print( untyped );
		src.print_fmt( "#pragma endregion Parsing\n\n" );
		src.print_fmt( "#pragma endregion Interface\n\n" );

		src.print_fmt( "GEN_NS_END\n\n");
		src.print( pop_ignores );
		src.write();
	}

	// gen_builder.hpp
	{
		Code builder = scan_file( "file_processors/builder.hpp" );

		Builder
		header = Builder::open( "gen/gen.builder.hpp" );
		header.print_fmt( generation_notice );
		header.print_fmt( "#pragma once\n\n" );
		header.print( def_include( txt_StrC("gen.hpp") ));
		header.print_fmt( "\nGEN_NS_BEGIN\n\n" );
		header.print( builder );
		header.print_fmt( "\nGEN_NS_END\n\n" );
		header.write();
	}

	// gen_builder.cpp
	{
		Code builder = scan_file( "file_processors/builder.cpp" );

		Builder
		src = Builder::open( "gen/gen.builder.cpp" );
		src.print_fmt( generation_notice );
		src.print( def_include( txt_StrC("gen.builder.hpp") ) );
		src.print_fmt( "\nGEN_NS_BEGIN\n\n" );
		src.print( builder );
		src.print_fmt( "\nGEN_NS_END\n\n" );
		src.write();
	}

	// gen_scanner.hpp
	{
		Code parsing = scan_file( "dependencies/parsing.hpp" );
		Code scanner = scan_file( "file_processors/scanner.hpp" );

		Builder
		header = Builder::open( "gen/gen.scanner.hpp" );
		header.print_fmt( generation_notice );
		header.print_fmt( "#pragma once\n\n" );
		header.print( def_include( txt_StrC("gen.hpp") ) );
		header.print_fmt( "\nGEN_NS_BEGIN\n\n" );
		header.print( parsing );
		header.print( scanner );
		header.print_fmt( "\nGEN_NS_END\n\n" );
		header.write();
	}

	// gen_scanner.cpp
	{
		Code parsing = scan_file( "dependencies/parsing.cpp" );
		// Code scanner = scan_file( "file_processors/scanner.cpp" );

		Builder
		src = Builder::open( "gen/gen.scanner.cpp" );
		src.print_fmt( generation_notice );
		src.print( def_include( txt_StrC("gen.scanner.hpp") ) );
		src.print_fmt( "\nGEN_NS_BEGIN\n\n" );
		src.print( parsing );
		// src.print( scanner );
		src.print_fmt( "\nGEN_NS_END\n\n" );
		src.write();
	}

	gen::deinit();
	return 0;
}
