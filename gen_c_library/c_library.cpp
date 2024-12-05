#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_SUPPORT_CPP_MEMBER_FEATURES 1
#define GEN_SUPPORT_CPP_REFERENCES      1
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

#include <cstdlib>   // for system()

#include "components/memory.fixed_arena.hpp"
#include "components/misc.hpp"
#include "components/containers.array.hpp"
#include "components/containers.hashtable.hpp"

using namespace gen;

constexpr char const* generation_notice =
"// This file was generated automatially by gencpp's c_library.cpp  "
"(See: https://github.com/Ed94/gencpp)\n\n";

constexpr StrC roll_own_dependencies_guard_start = txt(R"(
//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
// Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES
)");

constexpr StrC roll_own_dependencies_guard_end = txt(R"(
// GEN_ROLL_OWN_DEPENDENCIES
#endif
)");

constexpr StrC implementation_guard_start = txt(R"(
#pragma region GENCPP IMPLEMENTATION GUARD
#if defined(GEN_IMPLEMENTATION) && ! defined(GEN_IMPLEMENTED)
#	define GEN_IMPLEMENTED
)");

constexpr StrC implementation_guard_end = txt(R"(
#endif
#pragma endregion GENCPP IMPLEMENTATION GUARD
)");

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

CodeBody parse_file( const char* path )
{
	FileContents file = file_read_contents( GlobalAllocator, true, path );
	CodeBody     code = parse_global_body( { file.size, (char const*)file.data } );
	log_fmt("\nParsed: %s\n", path);
	return code;
}

int gen_main()
{
#define project_dir "../project/"
	gen::init();

	PreprocessorDefines.append(txt("GEN_API_C_BEGIN"));
	PreprocessorDefines.append(txt("GEN_API_C_END"));
	PreprocessorDefines.append(txt("HashTable("));

	Code push_ignores           = scan_file( project_dir "helpers/push_ignores.inline.hpp" );
	Code pop_ignores            = scan_file( project_dir "helpers/pop_ignores.inline.hpp" );
	Code c_library_header_start = scan_file( "components/header_start.hpp" );

	Builder
	header = Builder::open( "gen/gen.h" );
	header.print_fmt( generation_notice );
	header.print_fmt("#pragma once\n\n");
	header.print( push_ignores );

	// Headers
	{
		header.print( c_library_header_start );

#pragma region Scan, Parse, and Generate Components
		Code types      = scan_file( project_dir "components/types.hpp" );
		Code ast        = scan_file( project_dir "components/ast.hpp" );
		Code ast_types  = scan_file( project_dir "components/ast_types.hpp" );
		Code code_types = scan_file( project_dir "components/code_types.hpp" );
		Code interface  = scan_file( project_dir "components/interface.hpp" );
		Code inlines 	= scan_file( project_dir "components/inlines.hpp" );
		Code header_end = scan_file( project_dir "components/header_end.hpp" );

		CodeBody ecode       = gen_ecode     ( project_dir "enums/ECode.csv" );
		CodeBody eoperator   = gen_eoperator ( project_dir "enums/EOperator.csv" );
		CodeBody especifier  = gen_especifier( project_dir "enums/ESpecifier.csv" );
		CodeBody ast_inlines = gen_ast_inlines();
#pragma endregion Scan, Parse, and Generate Components

#pragma region Scan, Parse, and Generate Dependencies
		Code platform     = scan_file( project_dir "dependencies/platform.hpp" );
		Code macros       = scan_file( project_dir "dependencies/macros.hpp" );
		Code basic_types  = scan_file( project_dir "dependencies/basic_types.hpp" );
		Code debug        = scan_file( project_dir "dependencies/debug.hpp" );
		Code string_ops   = scan_file( project_dir "dependencies/string_ops.hpp" );
		Code hashing      = scan_file( project_dir "dependencies/hashing.hpp" );
		Code filesystem   = scan_file( project_dir "dependencies/filesystem.hpp" );
		Code timing       = scan_file( project_dir "dependencies/timing.hpp" );

		CodeBody parsed_memory = parse_file( project_dir "dependencies/memory.hpp" );
		CodeBody memory        = def_body(CT_Global_Body);
		for ( Code entry = parsed_memory.begin(); entry != parsed_memory.end(); ++ entry )
		{
			switch (entry->Type)
			{
				case CT_Using:
				{
					log_fmt("REPLACE THIS MANUALLY: %SC\n", entry->Name);
					CodeUsing   using_ver   = cast(CodeUsing, entry);
					CodeTypedef typedef_ver = def_typedef(using_ver->Name, using_ver->UnderlyingType);

					memory.append(typedef_ver);
				}
				break;
				case CT_Function_Fwd:
				{
					CodeFn fn = cast(CodeFn, entry);
					// for ( StrC id : to_rename ) if (fn->Name.is_equal(id)) {
					// 	rename_function_to_unique_symbol(fn);
					// }
					memory.append(fn);
				}
				break;
				case CT_Function:
				{
					CodeFn fn = cast(CodeFn, entry);
					s32 constexpr_found = fn->Specs.remove( Spec_Constexpr );
					if (constexpr_found > -1) {
						log_fmt("Found constexpr: %S\n", entry.to_string());
						fn->Specs.append(Spec_Inline);
					}
					// for ( StrC id : to_rename ) if (fn->Name.is_equal(id)) {
					// 	Array(CodeFn) list = * needs_selectors.get(id);
					// 	list.append(rename_function_to_unique_symbol(fn));
					// }
					memory.append(fn);
				}
				break;
				case CT_Template:
				{
					CodeTemplate tmpl = cast(CodeTemplate, entry);
					if ( tmpl->Declaration->Name.contains(txt("swap")))
					{
						CodeBody macro_swap = parse_global_body( txt(R"(
#define swap( a, b )              \
	do                            \
	{                             \
		typeof( a ) temp = ( a ); \
		( a )            = ( b ); \
		( b )            = temp;  \
	} while ( 0 )
)"
						));
						memory.append(macro_swap);
					}
				}
				break;
				case CT_Class:
				case CT_Struct:
				{
					CodeBody body     = cast(CodeBody, entry->Body);
					CodeBody new_body = def_body( entry->Body->Type );
					for ( Code body_entry = body.begin(); body_entry != body.end(); ++ body_entry ) switch
					(body_entry->Type) {
						case CT_Preprocess_If:
						{
							ignore_preprocess_cond_block(txt("GEN_SUPPORT_CPP_MEMBER_FEATURES"), body_entry, body, new_body );
						}
						break;

						default:
							new_body.append(body_entry);
						break;
					}

					entry->Body = new_body;
					memory.append(entry);
				}
				break;
				case CT_Preprocess_If:
				{
					b32 found = ignore_preprocess_cond_block(txt("GEN_SUPPORT_CPP_MEMBER_FEATURES"), entry, parsed_memory, memory );
					if (found) break;

					found = ignore_preprocess_cond_block(txt("GEN_SUPPORT_CPP_REFERENCES"), entry, parsed_memory, memory );
					if (found) break;

					memory.append(entry);
				}
				break;
				case CT_Preprocess_IfDef:
				{
					b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_memory, memory );
					if (found) break;

					memory.append(entry);
				}
				break;
				case CT_Preprocess_Pragma:
				{
					CodePragma pragma = cast(CodePragma, entry);
					// if (pragma->Content.starts_with(txt("region Memory"))) {
					// 	memory.append(generic_test);
					// 	break;
					// }

					b32 found = swap_pragma_region_implementation( txt("FixedArena"), gen_fixed_arenas, entry, memory);
					if (found) break;

					memory.append(entry);
				}
				break;
				default: {
					memory.append(entry);
				}
				break;
			}
		}

		CodeBody printing_parsed = parse_file( project_dir "dependencies/printing.hpp" );
		CodeBody printing        = def_body(CT_Global_Body);
		for ( Code entry = printing_parsed.begin(); entry != printing_parsed.end(); ++ entry )
		{
			switch (entry->Type)
			{
				case CT_Preprocess_IfDef:
				{
					b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, printing_parsed, printing );
					if (found) break;

					printing.append(entry);
				}
				break;
				case CT_Variable:
				{
					if ( strc_contains(entry->Name, txt("Msg_Invalid_Value")))
					{
						CodeDefine define = def_define(entry->Name, entry->Value->Content);
						printing.append(define);
						continue;
					}
					printing.append(entry);
				}
				break;
				default:
					printing.append(entry);
				break;
			}
		}

		CodeBody parsed_strings = parse_file( project_dir "dependencies/strings.hpp" );
		CodeBody strings        = def_body(CT_Global_Body);
		for ( Code entry = parsed_strings.begin(); entry != parsed_strings.end(); ++ entry )
		{
			switch (entry->Type)
			{
				case CT_Preprocess_If:
				{
					CodePreprocessCond cond = cast(CodePreprocessCond, entry);
					if (cond->Content.starts_with(txt("GEN_COMPILER_C || ! GEN_SUPPORT_CPP_MEMBER_FEATURES")))
					{
						for (; entry != end(parsed_strings) && entry->Type != CT_Typedef; ++ entry) {}
						strings.append(entry);
						strings.append(fmt_newline);

						for (; entry != end(parsed_strings) && entry->Type != CT_Preprocess_EndIf; ++ entry) {}
						++ entry;
						break;
					}

					bool found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP"), entry, parsed_strings, strings);
					if (found) break;

					found = ignore_preprocess_cond_block(txt("GEN_SUPPORT_CPP_REFERENCES"), entry, parsed_strings, strings );
				}
				break;

				case CT_Preprocess_IfDef:
				{
					ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_strings, strings );
				}
				break;

				case CT_Struct_Fwd:
				{
					if ( entry->Name.is_equal(txt("String")) )
					{
						CodeTypedef c_def = parse_typedef(code( typedef char* String; ));
						strings.append(c_def);
						strings.append(fmt_newline);
						++ entry;
						continue;
					}
					strings.append(entry);
				}
				break;

				case CT_Struct:
				{
					CodeBody body     = cast(CodeBody, entry->Body);
					CodeBody new_body = def_body( entry->Body->Type );
					for ( Code body_entry = body.begin(); body_entry != body.end(); ++ body_entry ) switch
					(body_entry->Type) {
						case CT_Preprocess_If:
						{
							b32 found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP"), body_entry, body, new_body );
							if (found) break;

							new_body.append(body_entry);
						}
						break;
						default:
							new_body.append(body_entry);
						break;
					}
					entry->Body = new_body;
					strings.append(entry);
				}
				break;

				case CT_Typedef:
				{
					StrC name_string_table = txt("StringTable");

					CodeTypedef td = cast(CodeTypedef, entry);
					if (td->Name.contains(name_string_table))
					{
						CodeBody ht = gen_hashtable(txt("StrC"), name_string_table);
						strings.append(ht);
						break;
					}
					strings.append(td);
				}
				break;

				default:
					strings.append(entry);
				break;
			}
		}

		CodeBody containers = def_body(CT_Global_Body);
		{
			CodeBody array_ssize = gen_array(txt("ssize"), txt("Array_ssize"));

			containers.append( def_pragma(code(region Containers)));

			// At this point all arrays required should have been defined so its safe to generate the generic selectors.
			containers.append( gen_array_base() );
			containers.append( gen_array_generic_selection_interface());
			containers.append( gen_hashtable_base() );

			containers.append(fmt_newline);
			containers.append(array_ssize);

			containers.append( def_pragma(code(endregion Containers)));
			containers.append(fmt_newline);
		}
#pragma endregion Scan, Parse, and Generate Dependencies

#pragma region Print Dependencies
		header.print_fmt( roll_own_dependencies_guard_start );
		header.print( platform );
		header.print_fmt( "\nGEN_NS_BEGIN\n" );

		header.print( macros );
		header.print( basic_types );
		header.print( debug );
		header.print( dump_to_scratch_and_retireve(memory) );
		header.print( dump_to_scratch_and_retireve(printing));
		header.print( string_ops );
		header.print( dump_to_scratch_and_retireve(containers));
		header.print( hashing );
		header.print( dump_to_scratch_and_retireve(strings));
		// header.print( filesystem );
		// header.print( timing );
		header.print_fmt( "\nGEN_NS_END\n" );
		header.print_fmt( roll_own_dependencies_guard_end );
#pragma endregion Print Dependencies


#if 0
#region region Print Components
		header.print_fmt("#pragma region Types\n");
		header.print( types );
		header.print( fmt_newline );
		header.print( dump_to_scratch_and_retireve( ecode ));
		header.print( fmt_newline );
		header.print( dump_to_scratch_and_retireve( eoperator ));
		header.print( fmt_newline );
		header.print( dump_to_scratch_and_retireve( especifier ));
		header.print( fmt_newline );
		header.print_fmt("#pragma endregion Types\n\n");
#pragma endregion Print Compoennts
#endif
	}

	header.print( pop_ignores );
	header.write();

	// format_file( "gen/gen.h" );

	gen::deinit();
	return 0;
#undef project_dir
}
