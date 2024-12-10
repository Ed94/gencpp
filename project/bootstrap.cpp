#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_C_LIKE_CPP 1
#include "../project/gen.cpp"

#include "helpers/push_ignores.inline.hpp"
#include "helpers/helper.hpp"

GEN_NS_BEGIN
#include "helpers/push_container_defines.inline.hpp"
#include "dependencies/parsing.cpp"
#include "helpers/pop_container_defines.inline.hpp"
GEN_NS_END

#include "auxillary/builder.hpp"
#include "auxillary/builder.cpp"
#include "auxillary/scanner.hpp"

using namespace gen;

constexpr char const* generation_notice =
"// This file was generated automatially by gencpp's bootstrap.cpp "
"(See: https://github.com/Ed94/gencpp)\n\n";

#include <cstdlib>   // for system()

void format_file( char const* path )
{
	String resolved_path = string_make_strc(GlobalAllocator, to_strc_from_c_str(path));

	String style_arg = string_make_strc(GlobalAllocator, txt("-style=file:"));
	string_append_strc( & style_arg, txt("../scripts/.clang-format "));

	// Need to execute clang format on the generated file to get it to match the original.
	#define clang_format      txt("clang-format ")
	#define cf_format_inplace txt("-i ")
	#define cf_verbose        txt("-verbose ")
	String command = string_make_strc( GlobalAllocator, clang_format );
	string_append_strc( & command, cf_format_inplace );
	string_append_strc( & command, cf_verbose );
	string_append_string( & command, style_arg );
	string_append_string( & command, resolved_path );
		log_fmt("\tRunning clang-format on file:\n");
		system( command );
		log_fmt("\tclang-format finished reformatting.\n");
	#undef cf_cmd
	#undef cf_format_inplace
	#undef cf_style
	#undef cf_verbse
}

Code dump_to_scratch_and_retireve( Code code )
{
	Builder ecode_file_temp = builder_open("gen/scratch.hpp");
	builder_print( & ecode_file_temp, code);
	builder_write(& ecode_file_temp);
	format_file("gen/scratch.hpp");
	Code result = scan_file( "gen/scratch.hpp" );
	remove("gen/scratch.hpp");
	return result;
}

int gen_main()
{
	gen::init();

	// PreprocessorDefines.append("GEN_NS");

	Code push_ignores = scan_file( "helpers/push_ignores.inline.hpp" );
	Code pop_ignores  = scan_file( "helpers/pop_ignores.inline.hpp" );

	// gen_dep.hpp
	{
		Code platform     = scan_file( "dependencies/platform.hpp" );
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

		Builder _header = builder_open("gen/gen.dep.hpp");
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

		builder_print_fmt( header, "\nGEN_NS_END\n" );
		builder_write(header);
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

		builder_print_fmt( src, "\nGEN_NS_END\n" );
		builder_write(src);
	}

	CodeBody gen_component_header = def_global_body( args(
		def_preprocess_cond( PreprocessCond_IfDef, txt("GEN_INTELLISENSE_DIRECTIVES") ),
		pragma_once,
		def_include(txt("components/types.hpp")),
		preprocess_endif,
		fmt_newline,
		untyped_str( to_strc_from_c_str(generation_notice) )
	));

	// gen.hpp
	{
		Code header_start = scan_file( "components/header_start.hpp" );
		Code types        = scan_file( "components/types.hpp" );
		Code ast          = scan_file( "components/ast.hpp" );
		Code ast_types    = scan_file( "components/ast_types.hpp" );
		Code code_types   = scan_file( "components/code_types.hpp" );
		Code interface    = scan_file( "components/interface.hpp" );
		Code inlines      = scan_file( "components/inlines.hpp" );
		Code header_end   = scan_file( "components/header_end.hpp" );

		CodeBody ecode       = gen_ecode     ( "enums/ECodeTypes.csv" );
		CodeBody eoperator   = gen_eoperator ( "enums/EOperator.csv" );
		CodeBody especifier  = gen_especifier( "enums/ESpecifier.csv" );
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
		builder_print( header, dump_to_scratch_and_retireve(ecode) );
		builder_print( header, fmt_newline);
		builder_print( header, dump_to_scratch_and_retireve(eoperator) );
		builder_print( header, fmt_newline);
		builder_print( header, dump_to_scratch_and_retireve(especifier) );
		builder_print( header, fmt_newline);
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
		builder_print( header, dump_to_scratch_and_retireve(ast_inlines) );
		builder_print( header, fmt_newline );
		builder_print_fmt( header, "#pragma endregion Inlines\n" );

		builder_print( header, header_end );
		builder_print_fmt( header, "GEN_NS_END\n\n" );
		builder_print( header, pop_ignores );
		builder_write(header);

		Builder header_ecode = builder_open( "components/gen/ecode.hpp" );
		builder_print( & header_ecode, gen_component_header );
		builder_print( & header_ecode, ecode );
		builder_write( & header_ecode);

		Builder header_eoperator = builder_open( "components/gen/eoperator.hpp" );
		builder_print( & header_eoperator, gen_component_header );
		builder_print( & header_eoperator, eoperator );
		builder_write( & header_eoperator );

		Builder header_especifier = builder_open( "components/gen/especifier.hpp" );
		builder_print( & header_especifier, gen_component_header );
		builder_print( & header_especifier, especifier );
		builder_write( & header_especifier);

		Builder header_ast_inlines = builder_open( "components/gen/ast_inlines.hpp" );
		builder_print( & header_ast_inlines, gen_component_header );
		builder_print( & header_ast_inlines, ast_inlines );
		builder_write( & header_ast_inlines);
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
		Code        lexer              = scan_file( "components/lexer.cpp" );
		Code        parser             = scan_file( "components/parser.cpp" );
		Code 	    parsing_interface  = scan_file( "components/interface.parsing.cpp" );
		Code        untyped 	       = scan_file( "components/interface.untyped.cpp" );

		CodeBody etoktype         = gen_etoktype( "enums/ETokType.csv", "enums/AttributeTokens.csv" );
		//CodeNS   nspaced_etoktype = def_namespace( name(parser), def_namespace_body( args(etoktype)) );
		CodeBody nspaced_etoktype = def_global_body( args(
			etoktype
		));

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
		builder_print( src, dump_to_scratch_and_retireve(nspaced_etoktype) );
		builder_print( src, lexer );
		builder_print( src, parser );
		builder_print( src, parsing_interface );
		builder_print( src, untyped );
		builder_print_fmt( src, "\n#pragma endregion Parsing\n\n" );
		builder_print_fmt( src, "#pragma endregion Interface\n\n" );

		builder_print_fmt( src, "GEN_NS_END\n\n");
		builder_print( src, pop_ignores );
		builder_write(src);

		Builder src_etoktype = builder_open( "components/gen/etoktype.cpp" );
		builder_print( & src_etoktype, gen_component_header );
		builder_print( & src_etoktype, nspaced_etoktype );
		builder_write( & src_etoktype);
	}

	// gen_builder.hpp
	{
		Code builder = scan_file( "auxillary/builder.hpp" );

		Builder header = builder_open( "gen/gen.builder.hpp" );
		builder_print_fmt( & header, generation_notice );
		builder_print_fmt( & header, "#pragma once\n\n" );
		builder_print( & header, def_include( txt("gen.hpp") ));
		builder_print_fmt( & header, "\nGEN_NS_BEGIN\n" );
		builder_print( & header, builder );
		builder_print_fmt( & header, "GEN_NS_END\n" );
		builder_write( & header);
	}

	// gen_builder.cpp

		Code builder = scan_file( "auxillary/builder.cpp" );

		Builder src = builder_open( "gen/gen.builder.cpp" );
		builder_print_fmt( & src, generation_notice );
		builder_print( & src, def_include( txt("gen.builder.hpp") ) );
		builder_print_fmt( & src, "\nGEN_NS_BEGIN\n" );
		builder_print( & src,  builder );
		builder_print_fmt( & src, "\nGEN_NS_END\n" );
		builder_write( & src);


	// gen_scanner.hpp
	{
		Code parsing = scan_file( "dependencies/parsing.hpp" );
		Code scanner = scan_file( "auxillary/scanner.hpp" );

		Builder header = builder_open( "gen/gen.scanner.hpp" );
		builder_print_fmt( & header, generation_notice );
		builder_print_fmt( & header, "#pragma once\n\n" );
		builder_print( & header, def_include( txt("gen.hpp") ) );
		builder_print_fmt( & header, "\nGEN_NS_BEGIN\n" );
		builder_print( & header, parsing );
		builder_print( & header, scanner );
		builder_print_fmt( & header, "\nGEN_NS_END\n" );
		builder_write(& header);
	}

	// gen_scanner.cpp
	{
		Code parsing = scan_file( "dependencies/parsing.cpp" );
		Code scanner = scan_file( "auxillary/scanner.cpp" );

		Builder src = builder_open( "gen/gen.scanner.cpp" );
		builder_print_fmt( & src, generation_notice );
		builder_print( & src, def_include( txt("gen.scanner.hpp") ) );
		builder_print_fmt( & src, "\nGEN_NS_BEGIN\n" );
		builder_print( & src, parsing );
		builder_print( & src, scanner );
		builder_print_fmt( & src, "GEN_NS_END\n" );
		builder_write( & src);
	}

	gen::deinit();
	return 0;
}
