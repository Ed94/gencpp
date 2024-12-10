#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#include "gen.cpp"

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

#include <cstdlib>   // for system()

using namespace gen;

constexpr char const* generation_notice =
"// This file was generated automatially by gencpp's unreal.cpp "
"(See: https://github.com/Ed94/gencpp)\n\n";

constexpr StrC implementation_guard_start = txt(R"(
#pragma region GENCPP IMPLEMENTATION GUARD
#if defined(GEN_IMPLEMENTATION) && ! defined(GEN_IMPLEMENTED)
#	define GEN_IMPLEMENTED
)");

constexpr StrC implementation_guard_end = txt(R"(
#endif
#pragma endregion GENCPP IMPLEMENTATION GUARD
)");

constexpr StrC roll_own_dependencies_guard_start = txt(R"(
//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
// Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES

)");

constexpr StrC roll_own_dependencies_guard_end = txt(R"(
// GEN_ROLL_OWN_DEPENDENCIES
#endif
)");

global bool generate_gen_dep = true;
global bool generate_builder = true;
global bool generate_editor  = true;
global bool generate_scanner = true;

void format_file( char const* path )
{
	String resolved_path = String::make(GlobalAllocator, to_strc_from_c_str(path));

	String style_arg = String::make(GlobalAllocator, txt("-style=file:"));
	style_arg.append("../scripts/.clang-format ");

	// Need to execute clang format on the generated file to get it to match the original.
	#define clang_format      "clang-format "
	#define cf_format_inplace "-i "
	#define cf_verbose        "-verbose "
	String command = String::make( GlobalAllocator, clang_format );
	command.append( cf_format_inplace );
	command.append( cf_verbose );
	command.append( style_arg );
	command.append( resolved_path );
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
	Builder ecode_file_temp = Builder::open("gen/scratch.hpp");
	ecode_file_temp.print(code);
	ecode_file_temp.write();
	format_file("gen/scratch.hpp");
	Code result = scan_file( "gen/scratch.hpp" );
	remove("gen/scratch.hpp");
	return result;
}

int gen_main()
{
#define project_dir "../project/"
	gen::init();

	Code push_ignores        = scan_file( project_dir "helpers/push_ignores.inline.hpp" );
	Code pop_ignores         = scan_file( project_dir "helpers/pop_ignores.inline.hpp" );

	Code ue_forceinline = code_str(FORCEINLINE);
	// Code

	// gen_dep.hpp
	{
		CodeBody macros = def_body( CT_Global_Body );
		{
			FileContents content    = file_read_contents( GlobalAllocator, true, project_dir "dependencies/macros.hpp" );
			CodeBody     ori_macros = parse_global_body( StrC { content.size, (char const*)content.data });

			for (Code	code =  ori_macros.begin();
						code != ori_macros.end();
						++ code )
			{
				switch (code->Type)
				{
					case CT_Preprocess_Define:
					{
						CodeDefine define = cast(CodeDefine, code);
						if ( define->Name.starts_with(txt("global")) )
						{
							macros.append(parse_global_body(txt("#define global // Global variables")));
							continue;
						}

						macros.append(define);
					}
					break;

					default:
						macros.append(code);
					break;
				}
			}
		}

		Code platform     = scan_file( project_dir "dependencies/platform.hpp" );
		Code basic_types  = scan_file( project_dir "dependencies/basic_types.hpp" );
		Code debug        = scan_file( project_dir "dependencies/debug.hpp" );
		Code memory	      = scan_file( project_dir "dependencies/memory.hpp" );
		Code string_ops   = scan_file( project_dir "dependencies/string_ops.hpp" );
		Code printing     = scan_file( project_dir "dependencies/printing.hpp" );
		Code containers   = scan_file( project_dir "dependencies/containers.hpp" );
		Code hashing 	  = scan_file( project_dir "dependencies/hashing.hpp" );
		Code strings      = scan_file( project_dir "dependencies/strings.hpp" );
		Code filesystem   = scan_file( project_dir "dependencies/filesystem.hpp" );
		Code timing       = scan_file( project_dir "dependencies/timing.hpp" );

		Builder
		header = Builder::open("gen/gen.dep.hpp");
		header.print_fmt( generation_notice );
		header.print( pragma_once );
		header.print( push_ignores );
		header.print( platform );
		header.print_fmt( "\nGEN_NS_BEGIN\n" );

		header.print( fmt_newline);
		header.print( dump_to_scratch_and_retireve(macros) );
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
		header.print( fmt_newline );
		header.print( pop_ignores );
		header.write();
	}

	// gen_dep.cpp
	{
		Code src_start  = scan_file( project_dir "dependencies/src_start.cpp" );
		Code debug      = scan_file( project_dir "dependencies/debug.cpp" );
		Code string_ops = scan_file( project_dir "dependencies/string_ops.cpp" );
		Code printing   = scan_file( project_dir "dependencies/printing.cpp" );
		Code memory     = scan_file( project_dir "dependencies/memory.cpp" );
		Code hashing    = scan_file( project_dir "dependencies/hashing.cpp" );
		Code strings    = scan_file( project_dir "dependencies/strings.cpp" );
		Code filesystem = scan_file( project_dir "dependencies/filesystem.cpp" );
		Code timing     = scan_file( project_dir "dependencies/timing.cpp" );

		Builder
		src = Builder::open( "gen/gen.dep.cpp" );
		src.print_fmt( generation_notice );
		src.print( def_include(txt("gen.dep.hpp")));
		src.print( fmt_newline );
		src.print( push_ignores );
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
		src.print( fmt_newline );
		src.print( pop_ignores );
		src.write();
	}

	// gen.hpp
	{
		Code header_start = scan_file(             "components/header_start.hpp" );
		Code types        = scan_file( project_dir "components/types.hpp" );
		Code ast          = scan_file( project_dir "components/ast.hpp" );
		Code ast_types    = scan_file( project_dir "components/ast_types.hpp" );
		Code code_types   = scan_file( project_dir "components/code_types.hpp" );
		Code interface    = scan_file( project_dir "components/interface.hpp" );
		Code inlines      = scan_file( project_dir "components/inlines.hpp" );
		Code header_end   = scan_file( project_dir "components/header_end.hpp" );

		CodeBody ecode       = gen_ecode     ( project_dir "enums/ECodeTypes.csv" );
		CodeBody eoperator   = gen_eoperator ( project_dir "enums/EOperator.csv" );
		CodeBody especifier  = gen_especifier( project_dir "enums/ESpecifier.csv" );
		CodeBody ast_inlines = gen_ast_inlines();

		Builder
		header = Builder::open( "gen/gen.hpp" );
		header.print_fmt( generation_notice );
		header.print_fmt( "#pragma once\n\n" );
		header.print( push_ignores );
		header.print( fmt_newline );
		header.print( header_start );
		header.print_fmt( "\nGEN_NS_BEGIN\n\n" );

		header.print_fmt( "#pragma region Types\n" );
		header.print( types );
		header.print( fmt_newline );
		header.print( dump_to_scratch_and_retireve(ecode) );
		header.print( fmt_newline );
		header.print( dump_to_scratch_and_retireve(eoperator) );
		header.print( fmt_newline );
		header.print( dump_to_scratch_and_retireve(especifier) );
		header.print( fmt_newline );
		header.print_fmt( "#pragma endregion Types\n\n" );

		header.print_fmt( "#pragma region AST\n" );
		header.print( ast );
		header.print( code_types );
		header.print( ast_types );
		header.print_fmt( "\n#pragma endregion AST\n" );

		header.print( interface );

		header.print_fmt( "\n#pragma region Inlines\n" );
		header.print( inlines );
		header.print( fmt_newline );
		header.print( dump_to_scratch_and_retireve(ast_inlines) );
		header.print( fmt_newline );
		header.print_fmt( "#pragma endregion Inlines\n" );

		header.print( header_end );
		header.print_fmt( "GEN_NS_END\n\n" );
		header.print( pop_ignores );
		header.write();
	}

	// gen.cpp
	{
		Code        src_start          = scan_file(             "components/src_start.cpp" );
		Code        static_data 	   = scan_file( project_dir "components/static_data.cpp" );
		Code        ast_case_macros    = scan_file( project_dir "components/ast_case_macros.cpp" );
		Code        ast			       = scan_file( project_dir "components/ast.cpp" );
		Code        code_serialization = scan_file( project_dir "components/code_serialization.cpp" );
		Code        interface	       = scan_file( project_dir "components/interface.cpp" );
		Code        upfront 	       = scan_file( project_dir "components/interface.upfront.cpp" );
		Code        lexer              = scan_file( project_dir "components/lexer.cpp" );
		Code        parser             = scan_file( project_dir "components/parser.cpp" );
		Code 	    parsing_interface  = scan_file( project_dir "components/interface.parsing.cpp" );
		Code        untyped 	       = scan_file( project_dir "components/interface.untyped.cpp" );

		// Note(Ed): The Attribute tokens need to be expanded and regenerated on a per-project/installation of this library for a specific codebase of Unreal.
		// We can support an arbitrary set of modules or plugin apis for parsing
		// but its up to the user to define them all (This will just provide whats I've used up till now).
		CodeBody etoktype         = gen_etoktype( project_dir "enums/ETokType.csv", "enums/AttributeTokens.csv" );
		CodeNS   nspaced_etoktype = def_namespace( name(parser), def_namespace_body( args(etoktype)) );

		Builder
		src = Builder::open( "gen/gen.cpp" );
		src.print_fmt( generation_notice );
		src.print( push_ignores );
		src.print( fmt_newline );
		src.print( src_start );
		src.print( fmt_newline );
		src.print_fmt( "GEN_NS_BEGIN\n");

		src.print( static_data );

		src.print_fmt( "\n#pragma region AST\n\n" );
		src.print( ast_case_macros );
		src.print( ast );
		src.print( code_serialization );
		src.print_fmt( "\n#pragma endregion AST\n" );

		src.print_fmt( "\n#pragma region Interface\n" );
		src.print( interface );
		src.print( upfront );
		src.print_fmt( "\n#pragma region Parsing\n\n" );
		src.print( dump_to_scratch_and_retireve(nspaced_etoktype) );
		src.print( lexer );
		src.print( parser );
		src.print( parsing_interface );
		src.print( untyped );
		src.print_fmt( "\n#pragma endregion Parsing\n\n" );
		src.print_fmt( "#pragma endregion Interface\n\n" );

		src.print_fmt( "GEN_NS_END\n\n");
		src.print( pop_ignores );
		src.write();
	}

	// gen_builder.hpp
	{
		Code builder = scan_file( project_dir "auxillary/builder.hpp" );

		Builder
		header = Builder::open( "gen/gen.builder.hpp" );
		header.print_fmt( generation_notice );
		header.print( push_ignores );
		header.print( fmt_newline );
		header.print_fmt( "#pragma once\n\n" );
		header.print( def_include( txt("gen.hpp") ));
		header.print_fmt( "\nGEN_NS_BEGIN\n" );
		header.print( builder );
		header.print_fmt( "GEN_NS_END\n" );
		header.print( fmt_newline );
		header.print( pop_ignores );
		header.write();
	}

	// gen_builder.cpp
	{
		Code builder = scan_file( project_dir "auxillary/builder.cpp" );

		Builder
		src = Builder::open( "gen/gen.builder.cpp" );
		src.print_fmt( generation_notice );
		src.print( push_ignores );
		src.print( fmt_newline );
		src.print( def_include( txt("gen.builder.hpp") ) );
		src.print_fmt( "\nGEN_NS_BEGIN\n" );
		src.print( builder );
		src.print_fmt( "\nGEN_NS_END\n" );
		src.print( fmt_newline );
		src.print( pop_ignores );
		src.write();
	}

	// gen_scanner.hpp
	{
		Code parsing = scan_file( project_dir "dependencies/parsing.hpp" );
		Code scanner = scan_file( project_dir "auxillary/scanner.hpp" );

		Builder
		header = Builder::open( "gen/gen.scanner.hpp" );
		header.print_fmt( generation_notice );
		header.print_fmt( "#pragma once\n\n" );
		header.print( push_ignores );
		header.print( fmt_newline );
		header.print( def_include( txt("gen.hpp") ) );
		header.print_fmt( "\nGEN_NS_BEGIN\n" );
		header.print( parsing );
		header.print( scanner );
		header.print_fmt( "GEN_NS_END\n" );
		header.print( fmt_newline );
		header.print( pop_ignores );
		header.write();
	}

	// gen.scanner.cpp
	{
		Code parsing = scan_file( project_dir "dependencies/parsing.cpp" );
		Code scanner = scan_file( project_dir "auxillary/scanner.cpp" );

		Builder
		src = Builder::open( "gen/gen.scanner.cpp" );
		src.print_fmt( generation_notice );
		src.print( push_ignores );
		src.print( fmt_newline );
		src.print( def_include( txt("gen.scanner.hpp") ) );
		src.print_fmt( "\nGEN_NS_BEGIN\n" );
		src.print( parsing );
		// src.print( scanner );
		src.print_fmt( "GEN_NS_END\n" );
		src.print( fmt_newline );
		src.print( pop_ignores );
		src.write();
	}
}
