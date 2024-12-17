// Includes are exposed to base directory

#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_C_LIKE_CPP 1
#include "gen.cpp"
#include "helpers/push_ignores.inline.hpp"

#include <stdlib.h>

GEN_NS_BEGIN
#include "helpers/base_codegen.hpp"
#include "helpers/misc.hpp"
GEN_NS_END

using namespace gen;

constexpr char const* generation_notice =
"// This file was generated automatially by gencpp's bootstrap.cpp "
"(See: https://github.com/Ed94/gencpp)\n\n";

#include <cstdlib>   // for system()

#define path_format_style "../scripts/.clang-format "
#define scratch_file      "gen/scratch.hpp"
#define path_base         "../base/"

Code format( Code code ) {
	return code_refactor_and_format(code, scratch_file, nullptr, path_format_style );
}

int gen_main()
{
	Context ctx {
		
	};
	gen::init(& ctx);

	Code push_ignores = scan_file( (path_base "helpers/push_ignores.inline.hpp") );
	Code pop_ignores  = scan_file( (path_base "helpers/pop_ignores.inline.hpp") );

	// gen_dep.hpp
	{
		Code platform     = scan_file( path_base "dependencies/platform.hpp" );
		Code macros 	  = scan_file( path_base "dependencies/macros.hpp" );
		Code basic_types  = scan_file( path_base "dependencies/basic_types.hpp" );
		Code debug        = scan_file( path_base "dependencies/debug.hpp" );
		Code memory	      = scan_file( path_base "dependencies/memory.hpp" );
		Code string_ops   = scan_file( path_base "dependencies/string_ops.hpp" );
		Code printing     = scan_file( path_base "dependencies/printing.hpp" );
		Code containers   = scan_file( path_base "dependencies/containers.hpp" );
		Code hashing 	  = scan_file( path_base "dependencies/hashing.hpp" );
		Code strings      = scan_file( path_base "dependencies/strings.hpp" );
		Code filesystem   = scan_file( path_base "dependencies/filesystem.hpp" );
		Code timing       = scan_file( path_base "dependencies/timing.hpp" );
		Code parsing      = scan_file( path_base "dependencies/parsing.hpp" );

		Builder _header = builder_open( "gen/gen.dep.hpp");
		Builder* header = & _header;
		builder_print_fmt( header, generation_notice );
		builder_print_fmt( header, "// This file is intended to be included within gen.hpp (There is no pragma diagnostic ignores)\n" );
		builder_print( header, platform );
		builder_print_fmt( header, "\nGEN_NS_BEGIN\n" );

		builder_print( header, macros );
		builder_print( header, basic_types );
		builder_print( header, debug );
		builder_print( header, memory );
		builder_print( header, string_ops );
		builder_print( header, printing );
		builder_print( header, containers );
		builder_print( header, hashing );
		builder_print( header, strings );
		builder_print( header, filesystem );
		builder_print( header, timing );
		builder_print( header, parsing );

		builder_print_fmt( header, "\nGEN_NS_END\n" );
		builder_write(header);
	}

	// gen_dep.cpp
	{
		Code src_start  = scan_file( path_base "dependencies/src_start.cpp" );
		Code debug      = scan_file( path_base "dependencies/debug.cpp" );
		Code string_ops = scan_file( path_base "dependencies/string_ops.cpp" );
		Code printing   = scan_file( path_base "dependencies/printing.cpp" );
		Code memory     = scan_file( path_base "dependencies/memory.cpp" );
		Code hashing    = scan_file( path_base "dependencies/hashing.cpp" );
		Code strings    = scan_file( path_base "dependencies/strings.cpp" );
		Code filesystem = scan_file( path_base "dependencies/filesystem.cpp" );
		Code timing     = scan_file( path_base "dependencies/timing.cpp" );
		Code parsing    = scan_file( path_base "dependencies/parsing.cpp" );

		Builder _src = builder_open( "gen/gen.dep.cpp" );
		Builder* src = & _src;
		builder_print_fmt(src, generation_notice );
		builder_print_fmt( src, "// This file is intended to be included within gen.cpp (There is no pragma diagnostic ignores)\n" );
		builder_print( src, src_start );
		builder_print_fmt( src, "\nGEN_NS_BEGIN\n" );

		builder_print( src, debug );
		builder_print( src, string_ops );
		builder_print( src, printing );
		builder_print( src, hashing );
		builder_print( src, memory );
		builder_print( src, strings );
		builder_print( src, filesystem );
		builder_print( src, timing );
		builder_print( src, parsing );

		builder_print_fmt( src, "\nGEN_NS_END\n" );
		builder_write(src);
	}

	CodeBody gen_component_header = def_global_body( args(
		def_preprocess_cond( PreprocessCond_IfDef, txt("GEN_INTELLISENSE_DIRECTIVES") ),
		pragma_once,
		def_include(txt("components/types.hpp")),
		preprocess_endif,
		fmt_newline,
		untyped_str( to_str_from_c_str(generation_notice) )
	));

	// gen.hpp
	{
		Code header_start = scan_file( path_base "components/header_start.hpp" );
		Code types        = scan_file( path_base "components/types.hpp" );
		Code parser_types = scan_file( path_base "components/parser_types.hpp" );
		Code ast          = scan_file( path_base "components/ast.hpp" );
		Code ast_types    = scan_file( path_base "components/ast_types.hpp" );
		Code code_types   = scan_file( path_base "components/code_types.hpp" );
		Code interface    = scan_file( path_base "components/interface.hpp" );
		Code inlines      = scan_file( path_base "components/inlines.hpp" );
		Code header_end   = scan_file( path_base "components/header_end.hpp" );

		CodeBody ecode       = gen_ecode     ( path_base "enums/ECodeTypes.csv" );
		CodeBody eoperator   = gen_eoperator ( path_base "enums/EOperator.csv" );
		CodeBody especifier  = gen_especifier( path_base "enums/ESpecifier.csv" );
		CodeBody etoktype    = gen_etoktype  ( path_base "enums/ETokType.csv", path_base "enums/AttributeTokens.csv" );
		CodeBody ast_inlines = gen_ast_inlines();

		Builder _header = builder_open( "gen/gen.hpp" );
		Builder* header = & _header;
		builder_print_fmt( header, generation_notice );
		builder_print_fmt( header, "#pragma once\n\n" );
		builder_print( header, push_ignores );
		builder_print( header, header_start );
		builder_print_fmt( header, "\nGEN_NS_BEGIN\n\n" );

		builder_print_fmt(header, "#pragma region Types\n" );
		builder_print( header, types );
		builder_print( header, fmt_newline);
		builder_print( header, format(ecode) );
		builder_print( header, fmt_newline);
		builder_print( header, format(eoperator) );
		builder_print( header, fmt_newline);
		builder_print( header, format(especifier) );
		builder_print( header, fmt_newline);
		builder_print( header, format(etoktype));
		builder_print( header, parser_types);
		builder_print_fmt( header, "#pragma endregion Types\n\n" );

		builder_print_fmt( header, "#pragma region AST\n" );
		builder_print( header, ast );
		builder_print( header, code_types );
		builder_print( header, ast_types );
		builder_print_fmt( header, "\n#pragma endregion AST\n" );

		builder_print( header, interface );

		builder_print_fmt( header, "\n#pragma region Inlines\n" );
		builder_print( header, inlines );
		builder_print( header, fmt_newline );
		builder_print( header, format(ast_inlines) );
		builder_print( header, fmt_newline );
		builder_print_fmt( header, "#pragma endregion Inlines\n" );

		builder_print( header, header_end );
		builder_print_fmt( header, "\nGEN_NS_END\n\n" );
		builder_print( header, pop_ignores );
		builder_write(header);
	}

	// gen.cpp
	{
		Code        src_start          = scan_file( path_base "components/src_start.cpp" );
		Code        static_data 	   = scan_file( path_base "components/static_data.cpp" );
		Code        ast_case_macros    = scan_file( path_base "components/ast_case_macros.cpp" );
		Code        ast			       = scan_file( path_base "components/ast.cpp" );
		Code        code_serialization = scan_file( path_base "components/code_serialization.cpp" );
		Code        interface	       = scan_file( path_base "components/interface.cpp" );
		Code        upfront 	       = scan_file( path_base "components/interface.upfront.cpp" );
		Code        lexer              = scan_file( path_base "components/lexer.cpp" );
		Code        parser_case_macros = scan_file( path_base "components/parser_case_macros.cpp" );
		Code        parser             = scan_file( path_base "components/parser.cpp" );
		Code 	    parsing_interface  = scan_file( path_base "components/interface.parsing.cpp" );
		Code        untyped 	       = scan_file( path_base "components/interface.untyped.cpp" );

		Builder _src = builder_open( "gen/gen.cpp" );
		Builder* src = & _src;
		builder_print_fmt( src, generation_notice );
		builder_print( src, push_ignores );
		builder_print( src, src_start );
		builder_print_fmt( src, "\nGEN_NS_BEGIN\n");

		builder_print( src, static_data );

		builder_print_fmt( src, "\n#pragma region AST\n\n" );
		builder_print( src, ast_case_macros );
		builder_print( src, ast );
		builder_print( src, code_serialization );
		builder_print_fmt( src, "\n#pragma endregion AST\n" );

		builder_print_fmt( src, "\n#pragma region Interface\n" );
		builder_print( src, interface );
		builder_print( src, upfront );
		builder_print_fmt( src, "\n#pragma region Parsing\n\n" );
		builder_print( src, lexer );
		builder_print( src, parser_case_macros );
		builder_print( src, parser );
		builder_print( src, parsing_interface );
		builder_print_fmt( src, "\n#pragma endregion Parsing\n\n" );
		builder_print( src, untyped );
		builder_print_fmt( src, "#pragma endregion Interface\n\n" );

		builder_print_fmt( src, "GEN_NS_END\n\n");
		builder_print( src, pop_ignores );
		builder_write(src);
	}

	// gen_builder.hpp
	{
		Code builder = scan_file( path_base "auxiliary/builder.hpp" );

		Builder header = builder_open( "gen/gen.builder.hpp" );
		builder_print_fmt( & header, generation_notice );
		builder_print_fmt( & header, "#pragma once\n\n" );
		builder_print( & header, def_include( txt("gen.hpp") ));
		builder_print_fmt( & header, "\nGEN_NS_BEGIN\n" );
		builder_print( & header, builder );
		builder_print_fmt( & header, "\nGEN_NS_END\n" );
		builder_write( & header);
	}

	// gen_builder.cpp
	{
		Code builder = scan_file( path_base "auxiliary/builder.cpp" );

		Builder src = builder_open( "gen/gen.builder.cpp" );
		builder_print_fmt( & src, generation_notice );
		builder_print( & src, def_include( txt("gen.builder.hpp") ) );
		builder_print_fmt( & src, "\nGEN_NS_BEGIN\n" );
		builder_print( & src,  builder );
		builder_print_fmt( & src, "\nGEN_NS_END\n" );
		builder_write( & src);
	}

	// gen_scanner.hpp
	{
		Code scanner = scan_file( path_base "auxiliary/scanner.hpp" );

		Builder header = builder_open( "gen/gen.scanner.hpp" );
		builder_print_fmt( & header, generation_notice );
		builder_print_fmt( & header, "#pragma once\n\n" );
		builder_print( & header, def_include( txt("gen.hpp") ) );
		builder_print_fmt( & header, "\nGEN_NS_BEGIN\n" );
		builder_print( & header, scanner );
		builder_print_fmt( & header, "\nGEN_NS_END\n" );
		builder_write(& header);
	}

	// gen_scanner.cpp
	{
		Code scanner = scan_file( path_base "auxiliary/scanner.cpp" );

		Builder src = builder_open( "gen/gen.scanner.cpp" );
		builder_print_fmt( & src, generation_notice );
		builder_print( & src, def_include( txt("gen.scanner.hpp") ) );
		builder_print_fmt( & src, "\nGEN_NS_BEGIN\n" );
		builder_print( & src, scanner );
		builder_print_fmt( & src, "\nGEN_NS_END\n" );
		builder_write( & src);
	}

	gen::deinit( & ctx);
	return 0;
}
