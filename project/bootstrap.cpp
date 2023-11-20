#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#include "gen.cpp"

#include "helpers/push_ignores.inline.hpp"
#include "helpers/helper.hpp"

GEN_NS_BEGIN
#include "dependencies/parsing.cpp"
GEN_NS_END

#include "auxillary/builder.hpp"
#include "auxillary/builder.cpp"
#include "auxillary/scanner.hpp"

using namespace gen;

constexpr char const* generation_notice =
"// This file was generated automatially by gencpp's bootstrap.cpp "
"(See: https://github.com/Ed94/gencpp)\n\n";

int gen_main()
{
	gen::init();

	Code push_ignores = scan_file( "helpers/push_ignores.inline.hpp" );
	Code pop_ignores  = scan_file( "helpers/pop_ignores.inline.hpp" );

	// gen_dep.hpp
	{
		Code header_start = scan_file( "dependencies/header_start.hpp" );
		Code macros 	  = scan_file( "dependencies/macros.hpp" );
		Code basic_types  = scan_file( "dependencies/basic_types.hpp" );
		Code debug        = scan_file( "dependencies/debug.hpp" );
		Code memory	      = scan_file( "dependencies/memory.hpp" );
		Code string_ops   = scan_file( "dependencies/string_ops.hpp" );
		Code printing     = scan_file( "dependencies/printing.hpp" );
		Code containers   = scan_file( "dependencies/containers.hpp" );
		Code hashing 	  = scan_file( "dependencies/hashing.hpp" );
		Code strings      = scan_file( "dependencies/strings.hpp" );
		Code filesystem   = scan_file( "dependencies/filesystem.hpp" );
		Code timing       = scan_file( "dependencies/timing.hpp" );

		Builder
		header = Builder::open("gen/gen.dep.hpp");
		header.print_fmt( generation_notice );
		header.print_fmt( "// This file is intended to be included within gen.hpp (There is no pragma diagnostic ignores)\n\n" );
		header.print( header_start );
		header.print_fmt( "\nGEN_NS_BEGIN\n" );

		header.print( macros );
		header.print( basic_types );
		header.print( debug );
		header.print( memory );
		header.print( string_ops );
		header.print( printing );
		header.print( containers );
		header.print( hashing );
		header.print( strings );
		header.print( filesystem );
		header.print( timing );

		header.print_fmt( "\nGEN_NS_END\n" );
		header.write();
	}

	// gen_dep.cpp
	{
		Code src_start  = scan_file( "dependencies/src_start.cpp" );
		Code debug      = scan_file( "dependencies/debug.cpp" );
		Code string_ops = scan_file( "dependencies/string_ops.cpp" );
		Code printing   = scan_file( "dependencies/printing.cpp" );
		Code memory     = scan_file( "dependencies/memory.cpp" );
		Code hashing    = scan_file( "dependencies/hashing.cpp" );
		Code strings    = scan_file( "dependencies/strings.cpp" );
		Code filesystem = scan_file( "dependencies/filesystem.cpp" );
		Code timing     = scan_file( "dependencies/timing.cpp" );

		Builder
		src = Builder::open( "gen/gen.dep.cpp" );
		src.print_fmt( generation_notice );
		src.print_fmt( "// This file is intended to be included within gen.cpp (There is no pragma diagnostic ignores)\n\n" );
		src.print( src_start );
		src.print_fmt( "\nGEN_NS_BEGIN\n" );

		src.print( debug );
		src.print( string_ops );
		src.print( printing );
		src.print( hashing );
		src.print( memory );
		src.print( strings );
		src.print( filesystem );
		src.print( timing );

		src.print_fmt( "\nGEN_NS_END\n" );
		src.write();
	}

	CodeBody gen_component_header = def_global_body( args(
		def_preprocess_cond( PreprocessCond_IfDef, txt("GEN_INTELLISENSE_DIRECTIVES") ),
		pragma_once,
		def_include(txt("components/types.hpp")),
		preprocess_endif,
		fmt_newline,
		untyped_str( to_str(generation_notice) )
	));

	// gen.hpp
	{
		Code header_start = scan_file( "components/header_start.hpp" );
		Code types        = scan_file( "components/types.hpp" );
		Code ast          = scan_file( "components/ast.hpp" );
		Code ast_types    = scan_file( "components/ast_types.hpp" );
		Code interface    = scan_file( "components/interface.hpp" );
		Code inlines      = scan_file( "components/inlines.hpp" );
		Code header_end   = scan_file( "components/header_end.hpp" );

		CodeBody ecode       = gen_ecode     ( "enums/ECode.csv" );
		CodeBody eoperator   = gen_eoperator ( "enums/EOperator.csv" );
		CodeBody especifier  = gen_especifier( "enums/ESpecifier.csv" );
		CodeBody ast_inlines = gen_ast_inlines();

		Builder
		header = Builder::open( "gen/gen.hpp" );
		header.print_fmt( generation_notice );
		header.print_fmt( "#pragma once\n\n" );
		header.print( push_ignores );
		header.print( header_start );
		header.print_fmt( "\nGEN_NS_BEGIN\n\n" );

		header.print_fmt( "#pragma region Types\n" );
		header.print( types );
		header.print( ecode );
		header.print( eoperator );
		header.print( especifier );
		header.print_fmt( "#pragma endregion Types\n\n" );

		header.print_fmt( "#pragma region AST\n" );
		header.print( ast );
		header.print( ast_types );
		header.print_fmt( "\n#pragma endregion AST\n" );

		header.print( interface );

		header.print_fmt( "\n#pragma region Inlines\n" );
		header.print( inlines );
		header.print( fmt_newline );
		header.print( ast_inlines );
		header.print_fmt( "#pragma endregion Inlines\n" );

		header.print( header_end );
		header.print_fmt( "GEN_NS_END\n\n" );
		header.print( pop_ignores );
		header.write();

		Builder
		header_ecode = Builder::open( "components/gen/ecode.hpp" );
		header_ecode.print( gen_component_header );
		header_ecode.print( ecode );
		header_ecode.write();

		Builder
		header_eoperator = Builder::open( "components/gen/eoperator.hpp" );
		header_eoperator.print( gen_component_header );
		header_eoperator.print( eoperator );
		header_eoperator.write();

		Builder
		header_especifier = Builder::open( "components/gen/especifier.hpp" );
		header_especifier.print( gen_component_header );
		header_especifier.print( especifier );
		header_especifier.write();

		Builder
		header_ast_inlines = Builder::open( "components/gen/ast_inlines.hpp" );
		header_ast_inlines.print( gen_component_header );
		header_ast_inlines.print( ast_inlines );
		header_ast_inlines.write();
	}

	// gen.cpp
	{
		Code        src_start          = scan_file( "components/src_start.cpp" );
		Code        static_data 	   = scan_file( "components/static_data.cpp" );
		Code        ast_case_macros    = scan_file( "components/ast_case_macros.cpp" );
		Code        ast			       = scan_file( "components/ast.cpp" );
		Code        code_serialization = scan_file( "components/code_serialization.cpp" );
		Code        interface	       = scan_file( "components/interface.cpp" );
		Code        upfront 	       = scan_file( "components/interface.upfront.cpp" );
		Code 	    parsing 	       = scan_file( "components/interface.parsing.cpp" );
		Code        untyped 	       = scan_file( "components/interface.untyped.cpp" );

		CodeBody etoktype         = gen_etoktype( "enums/ETokType.csv", "enums/AttributeTokens.csv" );
		CodeNS   nspaced_etoktype = def_namespace( name(parser), def_namespace_body( args(etoktype)) );

		Builder
		src = Builder::open( "gen/gen.cpp" );
		src.print_fmt( generation_notice );
		src.print( push_ignores );
		src.print( src_start );
		src.print_fmt( "GEN_NS_BEGIN\n");

		src.print( static_data );

		src.print_fmt( "\n#pragma region AST\n\n" );
		src.print( ast_case_macros );
		src.print( ast );
		src.print( code );
		src.print_fmt( "\n#pragma endregion AST\n" );

		src.print_fmt( "\n#pragma region Interface\n" );
		src.print( interface );
		src.print( upfront );
		src.print_fmt( "\n#pragma region Parsing\n\n" );
		src.print( nspaced_etoktype );
		src.print( parsing );
		src.print( untyped );
		src.print_fmt( "\n#pragma endregion Parsing\n\n" );
		src.print_fmt( "#pragma endregion Interface\n\n" );

		src.print_fmt( "GEN_NS_END\n\n");
		src.print( pop_ignores );
		src.write();

		Builder
		src_etoktype = Builder::open( "components/gen/etoktype.cpp" );
		src_etoktype.print( gen_component_header );
		src_etoktype.print( nspaced_etoktype );
		src_etoktype.write();
	}

	// gen_builder.hpp
	{
		Code builder = scan_file( "auxillary/builder.hpp" );

		Builder
		header = Builder::open( "gen/gen.builder.hpp" );
		header.print_fmt( generation_notice );
		header.print_fmt( "#pragma once\n\n" );
		header.print( def_include( txt("gen.hpp") ));
		header.print_fmt( "\nGEN_NS_BEGIN\n" );
		header.print( builder );
		header.print_fmt( "GEN_NS_END\n" );
		header.write();
	}

	// gen_builder.cpp
	{
		Code builder = scan_file( "auxillary/builder.cpp" );

		Builder
		src = Builder::open( "gen/gen.builder.cpp" );
		src.print_fmt( generation_notice );
		src.print( def_include( txt("gen.builder.hpp") ) );
		src.print_fmt( "\nGEN_NS_BEGIN\n" );
		src.print( builder );
		src.print_fmt( "\nGEN_NS_END\n" );
		src.write();
	}

	// gen_scanner.hpp
	{
		Code parsing = scan_file( "dependencies/parsing.hpp" );
		Code scanner = scan_file( "auxillary/scanner.hpp" );

		Builder
		header = Builder::open( "gen/gen.scanner.hpp" );
		header.print_fmt( generation_notice );
		header.print_fmt( "#pragma once\n\n" );
		header.print( def_include( txt("gen.hpp") ) );
		header.print_fmt( "\nGEN_NS_BEGIN\n" );
		header.print( parsing );
		header.print( scanner );
		header.print_fmt( "GEN_NS_END\n" );
		header.write();
	}

	// gen_scanner.cpp
	{
		Code parsing = scan_file( "dependencies/parsing.cpp" );
		Code scanner = scan_file( "auxillary/scanner.cpp" );

		Builder
		src = Builder::open( "gen/gen.scanner.cpp" );
		src.print_fmt( generation_notice );
		src.print( def_include( txt("gen.scanner.hpp") ) );
		src.print_fmt( "\nGEN_NS_BEGIN\n" );
		src.print( parsing );
		src.print( scanner );
		src.print_fmt( "GEN_NS_END\n" );
		src.write();
	}

	gen::deinit();
	return 0;
}
