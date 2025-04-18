#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#include "gen.cpp"
#include "helpers/push_ignores.inline.hpp"

#include <stdlib.h>

GEN_NS_BEGIN
#include "helpers/base_codegen.hpp"
#include "helpers/misc.hpp"
GEN_NS_END

using namespace gen;

constexpr char const* generation_notice =
"// This file was generated automatially by gencpp's unreal.cpp "
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
	Context ctx {};
	gen::init( & ctx);

	Code push_ignores        = scan_file( path_base "helpers/push_ignores.inline.hpp" );
	Code pop_ignores         = scan_file( path_base "helpers/pop_ignores.inline.hpp" );

	Code ue_forceinline = code_str(FORCEINLINE);
	// Code

	register_macros( args(
		(Macro { txt("bit"),                          MT_Expression, MF_Functional }),
		(Macro { txt("bitfield_is_set"),              MT_Expression, MF_Functional }),
		(Macro { txt("GEN_C_LIKE_CPP"),               MT_Expression, MF_Null       }),
		(Macro { txt("cast"),                         MT_Expression, MF_Functional }),
		(Macro { txt("ccast"),                        MT_Expression, MF_Functional }),
		(Macro { txt("rcast"),                        MT_Expression, MF_Functional }),
		(Macro { txt("pcast"),                        MT_Expression, MF_Functional }),
		(Macro { txt("scast"),                        MT_Expression, MF_Functional }),
		(Macro { txt("stringize_va"),                 MT_Expression, MF_Functional }),
		(Macro { txt("stringize"),                    MT_Expression, MF_Functional }),
		(Macro { txt("do_once"),                      MT_Expression, MF_Functional }), 
		(Macro { txt("do_once_defer"),                MT_Expression, MF_Functional }),
		(Macro { txt("do_once_start"),                MT_Statement,  MF_Null       }), 
		(Macro { txt("do_once_end"),                  MT_Statement,  MF_Null       }), 
		(Macro { txt("labeled_scope_start"),          MT_Statement,  MF_Null       }), 
		(Macro { txt("labeled_scope_end"),            MT_Statement,  MF_Null       }), 
		(Macro { txt("compiler_decorated_func_name"), MT_Expression, MF_Null       }), 
		(Macro { txt("num_args_impl"),                MT_Expression, MF_Functional }),
		(Macro { txt("num_args"),                     MT_Expression, MF_Functional }),
		(Macro { txt("count_of"),                     MT_Expression, MF_Functional }),
		(Macro { txt("clamp"),                        MT_Expression, MF_Functional }),
		(Macro { txt("is_between"),                   MT_Expression, MF_Functional }),
		(Macro { txt("size_of"),                      MT_Expression, MF_Functional }),
		(Macro { txt("min"),                          MT_Expression, MF_Functional }),
		(Macro { txt("max"),                          MT_Expression, MF_Functional }),
		(Macro { txt("offset_of"),                    MT_Expression, MF_Functional }),
		(Macro { txt("static_assert"),                MT_Statement,  MF_Functional }),
		(Macro { txt("typeof"),                       MT_Expression, MF_Null       }),
		(Macro { txt("GEN_API_C_BEGIN"),              MT_Statement,  MF_Null       }),
		(Macro { txt("GEN_API_C_END"),                MT_Statement,  MF_Null       }),
		(Macro { txt("nullptr"),                      MT_Expression, MF_Null       }),
		(Macro { txt("GEN_REMOVE_PTR"),               MT_Expression, MF_Functional }),
		(Macro { txt("GEN_PARAM_DEFAULT"),            MT_Expression, MF_Null       }),
		(Macro { txt("struct_init"),                  MT_Expression, MF_Functional }),
		(Macro { txt("GEN_OPTIMIZE_MAPPINGS_BEGIN"),  MT_Statement,  MF_Null       }),
		(Macro { txt("GEN_OPITMIZE_MAPPINGS_END"),    MT_Statement,  MF_Null       }),
		(Macro { txt("src_line_str"),                 MT_Expression, MF_Null       })
	));

	// gen_dep.hpp
	{
		CodeBody macros = def_body( CT_Global_Body );
		{
			FileContents content    = file_read_contents( ctx.Allocator_Temp, file_zero_terminate, path_base "dependencies/macros.hpp" );
			CodeBody     ori_macros = parse_global_body( Str { (char const*)content.data, content.size });

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

		Code platform     = scan_file( path_base "dependencies/platform.hpp" );
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

		Builder
		header = Builder::open("gen/gen.dep.hpp");
		header.print_fmt( generation_notice );
		header.print( pragma_once );
		header.print( push_ignores );
		header.print( platform );
		header.print_fmt( "\nGEN_NS_BEGIN\n" );

		header.print( fmt_newline);
		header.print( format(macros) );
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
		header.print(parsing);

		header.print_fmt( "\nGEN_NS_END\n" );
		header.print( fmt_newline );
		header.print( pop_ignores );
		header.write();
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
		src.print( parsing );

		src.print_fmt( "\nGEN_NS_END\n" );
		src.print( fmt_newline );
		src.print( pop_ignores );
		src.write();
	}

	// gen.hpp
	{
		Code header_start = scan_file(           "components/header_start.hpp" );
		Code types        = scan_file( path_base "components/types.hpp" );
		Code parser_types = scan_file( path_base "components/parser_types.hpp");
		Code ast          = scan_file( path_base "components/ast.hpp" );
		Code ast_types    = scan_file( path_base "components/ast_types.hpp" );
		Code code_types   = scan_file( path_base "components/code_types.hpp" );
		Code interface    = scan_file( path_base "components/interface.hpp" );
		Code constants    = scan_file( path_base "components/constants.hpp" );
		Code inlines      = scan_file( path_base "components/inlines.hpp" );

		CodeBody ecode       = gen_ecode     ( path_base "enums/ECodeTypes.csv" );
		CodeBody eoperator   = gen_eoperator ( path_base "enums/EOperator.csv" );
		CodeBody especifier  = gen_especifier(           "enums/ESpecifier.csv" );
		CodeBody ast_inlines = gen_ast_inlines();

		// Note(Ed): The Attribute tokens need to be expanded and regenerated on a per-project/installation of this library for a specific codebase of Unreal.
		// We can support an arbitrary set of modules or plugin apis for parsing
		// but its up to the user to define them all (This will just provide whats I've used up till now).
		CodeBody etoktype = gen_etoktype( "enums/ETokType.csv", "enums/AttributeTokens.csv" );

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
		header.print( format(ecode) );
		header.print( fmt_newline );
		header.print( format(eoperator) );
		header.print( fmt_newline );
		header.print( format(especifier) );
		header.print( fmt_newline );
		header.print( format(etoktype) );
		header.print( parser_types );
		header.print_fmt( "#pragma endregion Types\n\n" );

		header.print_fmt( "#pragma region AST\n" );
		header.print( ast );
		header.print( code_types );
		header.print( ast_types );
		header.print_fmt( "\n#pragma endregion AST\n" );

		header.print( interface );
		header.print( constants );

		header.print_fmt( "\n#pragma region Inlines\n" );
		header.print( inlines );
		header.print( fmt_newline );
		header.print( format(ast_inlines) );
		header.print( fmt_newline );
		header.print_fmt( "#pragma endregion Inlines\n" );

		header.print_fmt( "\nGEN_NS_END\n\n" );
		header.print( pop_ignores );
		header.write();
	}

	// gen.cpp
	{
		Code        src_start          = scan_file(           "components/src_start.cpp" );
		Code        static_data 	   = scan_file( path_base "components/static_data.cpp" );
		Code        ast_case_macros    = scan_file( path_base "components/ast_case_macros.cpp" );
		Code        ast			       = scan_file( path_base "components/ast.cpp" );
		Code        code_serialization = scan_file( path_base "components/code_serialization.cpp" );
		Code        interface	       = scan_file( path_base "components/interface.cpp" );
		Code        upfront 	       = scan_file( path_base "components/interface.upfront.cpp" );
		Code        lexer              = scan_file( path_base "components/lexer.cpp" );
		Code        parser_case_macros = scan_file(           "components/parser_case_macros.cpp" );
		Code        parser             = scan_file( path_base "components/parser.cpp" );
		Code 	    parsing_interface  = scan_file( path_base "components/interface.parsing.cpp" );
		Code        untyped 	       = scan_file( path_base "components/interface.untyped.cpp" );

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
		src.print( lexer );
		src.print( parser_case_macros );
		src.print( parser );
		src.print( parsing_interface );
		src.print_fmt( "\n#pragma endregion Parsing\n\n" );

		src.print_fmt( "\n#pragma region Untyped\n\n" );
		src.print( untyped );
		src.print_fmt( "#pragma endregion \n\n" );

		src.print_fmt( "#pragma endregion Interface\n\n" );

		src.print_fmt( "GEN_NS_END\n\n");
		src.print( pop_ignores );
		src.write();
	}

	// gen_builder.hpp
	{
		Code builder = scan_file( path_base "auxiliary/builder.hpp" );

		Builder
		header = Builder::open( "gen/gen.builder.hpp" );
		header.print_fmt( generation_notice );
		header.print( push_ignores );
		header.print( fmt_newline );
		header.print_fmt( "#pragma once\n\n" );
		header.print( def_include( txt("gen.hpp") ));
		header.print_fmt( "\nGEN_NS_BEGIN\n" );
		header.print( builder );
		header.print_fmt( "\nGEN_NS_END\n" );
		header.print( fmt_newline );
		header.print( pop_ignores );
		header.write();
	}

	// gen_builder.cpp
	{
		Code builder = scan_file( path_base "auxiliary/builder.cpp" );

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
		Code scanner = scan_file( path_base "auxiliary/scanner.hpp" );

		Builder
		header = Builder::open( "gen/gen.scanner.hpp" );
		header.print_fmt( generation_notice );
		header.print_fmt( "#pragma once\n\n" );
		header.print( push_ignores );
		header.print( fmt_newline );
		header.print( def_include( txt("gen.hpp") ) );
		header.print_fmt( "\nGEN_NS_BEGIN\n" );
		header.print( scanner );
		header.print_fmt( "\nGEN_NS_END\n" );
		header.print( fmt_newline );
		header.print( pop_ignores );
		header.write();
	}

	// gen.scanner.cpp
	{
		Code scanner = scan_file( path_base "auxiliary/scanner.cpp" );

		Builder
		src = Builder::open( "gen/gen.scanner.cpp" );
		src.print_fmt( generation_notice );
		src.print( push_ignores );
		src.print( fmt_newline );
		src.print( def_include( txt("gen.scanner.hpp") ) );
		src.print_fmt( "\nGEN_NS_BEGIN\n" );
		src.print( scanner );
		src.print_fmt( "\nGEN_NS_END\n" );
		src.print( fmt_newline );
		src.print( pop_ignores );
		src.write();
	}
}
