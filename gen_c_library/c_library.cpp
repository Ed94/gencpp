#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
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

constexpr bool helper_use_c_definition = true;

int gen_main()
{
#define project_dir "../project/"
	gen::init();

	PreprocessorDefines.append(txt("GEN_API_C_BEGIN"));
	PreprocessorDefines.append(txt("GEN_API_C_END"));
	PreprocessorDefines.append(txt("HashTable("));
	PreprocessorDefines.append(txt("GEN_NS_PARSER"));
	PreprocessorDefines.append(txt("GEN_NS_PARSER_BEGIN"));
	PreprocessorDefines.append(txt("GEN_NS_PARSER_END"));
	PreprocessorDefines.append(txt("Using_Code("));
	PreprocessorDefines.append(txt("Using_CodeOps("));
	PreprocessorDefines.append(txt("GEN_OPTIMIZE_MAPPINGS_BEGIN"));
	PreprocessorDefines.append(txt("GEN_OPITMIZE_MAPPINGS_END"));
	//PreprocessorDefines.append(txt("GEN_EXECUTION_EXPRESSION_SUPPORT"));

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
		// Only has operator overload definitions that C doesn't need.
		// CodeBody ast_inlines = gen_ast_inlines();

		Code interface  = scan_file( project_dir "components/interface.hpp" );
		Code inlines 	= scan_file( project_dir "components/inlines.hpp" );
		Code header_end = scan_file( project_dir "components/header_end.hpp" );

		CodeBody ecode       = gen_ecode     ( project_dir "enums/ECodeTypes.csv", helper_use_c_definition );
		CodeBody eoperator   = gen_eoperator ( project_dir "enums/EOperator.csv",  helper_use_c_definition );
		CodeBody especifier  = gen_especifier( project_dir "enums/ESpecifier.csv", helper_use_c_definition );

		CodeBody parsed_types = parse_file( project_dir "components/types.hpp" );
		CodeBody types        = def_body(CT_Global_Body);
		for ( Code entry = parsed_types.begin(); entry != parsed_types.end(); ++ entry )
		{
			switch(entry->Type)
			{
				case CT_Using:
				{
					CodeUsing using_ver = cast(CodeUsing, entry);

					if (using_ver->UnderlyingType->ReturnType)
					{
						CodeTypename type       = using_ver->UnderlyingType;
						CodeTypedef typedef_ver = parse_typedef(token_fmt(
							"ReturnType", to_string(type->ReturnType).to_strc()
						,	"Name"      , using_ver->Name
						,	"Parameters", to_string(type->Params).to_strc()
						,	stringize(
								typedef <ReturnType>( * <Name>)(<Parameters>);
						)));
						types.append(typedef_ver);
						break;
					}
					CodeTypedef typedef_ver = def_typedef(using_ver->Name, using_ver->UnderlyingType);
					types.append(typedef_ver);
				}
				break;

				case CT_Enum:
				{
					log_fmt("Detected ENUM: %S", entry->Name);
					convert_cpp_enum_to_c(cast(CodeEnum, entry), types);
				}
				break;

				default:
					types.append(entry);
				break;
			}
		}
		CodeBody parsed_ast = parse_file( project_dir "components/ast.hpp" );
		CodeBody ast        = def_body(CT_Global_Body);
		for ( Code entry = parsed_ast.begin(); entry != parsed_ast.end(); ++ entry )
		switch (entry->Type)
		{
			case CT_Preprocess_If:
			{
				CodePreprocessCond cond = cast(CodePreprocessCond, entry);
				if (cond->Content.contains(txt("GEN_COMPILER_C")))
				{
					//++ entry;         //
					//ast.append(entry) // typedef
					//for ( ; entry != parsed_ast.end() && entry->Type != CT_Preprocess_EndIf; ++ entry) {}
					//++ entry;        // Consume endif
				}

				b32 found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP"), entry, parsed_ast, ast);
				if (found) break;

				ast.append(entry);
			}
			break;

			case CT_Struct_Fwd:
			{
				CodeStruct  fwd  = cast(CodeStruct, entry);
				CodeTypedef tdef = parse_typedef(token_fmt("name", fwd->Name, stringize( typedef struct <name> <name>; )));
				ast.append(fwd);
				ast.append(tdef);
			}
			break;

			case CT_Struct:
			{
				CodeStruct struct_def = cast(CodeStruct, entry);
				ast.append(struct_def);
				if ( ! entry->Name.is_equal(txt("AST")))
				{
					CodeTypedef tdef = parse_typedef(token_fmt("name", struct_def->Name, stringize( typedef struct <name> <name>; )));
					ast.append(tdef);
				}
			}
			break;

			case CT_Variable:
			{
				CodeVar var = cast(CodeVar, entry);

				s32 constexpr_found = var->Specs ? var->Specs.remove( Spec_Constexpr ) : - 1;
				if (constexpr_found > -1) {
					log_fmt("Found constexpr: %S\n", entry.to_string());
					if (var->Name.contains(txt("AST_ArrSpecs_Cap")))
					{
						Code def = untyped_str(txt(
R"(#define AST_ArrSpecs_Cap       \
(                                 \
        AST_POD_Size              \
		- sizeof(AST*) * 3        \
		- sizeof(Token*)          \
		- sizeof(AST*)            \
		- sizeof(StringCached)    \
		- sizeof(CodeType)        \
		- sizeof(ModuleFlag)      \
		- sizeof(int)             \
)                                 \
/ sizeof(int) - 1
)"
						));
						ast.append(def);
						break;
					}
					CodeDefine def = def_define(var->Name, var->Value.to_string());
					ast.append(def);
					break;
				}
				ast.append(var);
			}
			break;

			default:
				ast.append(entry);
			break;
		}
		CodeBody parsed_code_types = parse_file( project_dir "components/code_types.hpp" );
		CodeBody code_types        = def_body(CT_Global_Body);
		for ( Code entry = parsed_code_types.begin(); entry != parsed_code_types.end(); ++ entry ) switch( entry->Type )
		{
			case CT_Preprocess_If:
			case CT_Preprocess_IfDef:
			{
				b32 found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP"), entry, parsed_code_types, code_types );
				if (found) {
					++ entry;
					break;
				}
				found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_code_types, code_types );
				if (found) break;

				code_types.append(entry);
			}
			break;

			default:
				code_types.append(entry);
			break;
		}
		CodeBody parsed_ast_types = parse_file( project_dir "components/ast_types.hpp" );
		CodeBody ast_types        = def_body(CT_Global_Body);
		for ( Code entry = parsed_ast_types.begin(); entry != parsed_ast_types.end(); ++ entry ) switch( entry->Type )
		{
			case CT_Preprocess_If:
			case CT_Preprocess_IfDef:
			{
				b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_code_types, code_types );
				if (found) break;

				ast_types.append(entry);
			}
			break;

			case CT_Struct:
			{
				CodeStruct struct_def = cast(CodeStruct, entry);
				CodeTypedef tdef = parse_typedef(token_fmt("name", struct_def->Name, stringize( typedef struct <name> <name>; )));
				ast_types.append(struct_def);
				ast_types.append(tdef);
			}
			break;

			default:
				ast_types.append(entry);
			break;
		}
#pragma endregion Scan, Parse, and Generate Components

#pragma region Scan, Parse, and Generate Dependencies
		Code platform     = scan_file( project_dir "dependencies/platform.hpp" );
		Code macros       = scan_file( project_dir "dependencies/macros.hpp" );
		Code basic_types  = scan_file( project_dir "dependencies/basic_types.hpp" );
		Code debug        = scan_file( project_dir "dependencies/debug.hpp" );
		Code string_ops   = scan_file( project_dir "dependencies/string_ops.hpp" );
		Code hashing      = scan_file( project_dir "dependencies/hashing.hpp" );
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
					if (fn->Specs) {
						s32 constexpr_found = fn->Specs.remove( Spec_Constexpr );
						if (constexpr_found > -1) {
							log_fmt("Found constexpr: %S\n", entry.to_string());
							fn->Specs.append(Spec_Inline);
						}
					}
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
				case CT_Enum:
				{
					convert_cpp_enum_to_c(cast(CodeEnum, entry), memory);
				}
				break;
				case CT_Struct_Fwd:
				{
					CodeTypedef tdef = parse_typedef(token_fmt("name", entry->Name, stringize( typedef struct <name> <name>; )));
					memory.append(entry);
					memory.append(tdef);
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
							b32 found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP"), body_entry, body, new_body );
							if (found) break;
						}
						break;

						default:
							new_body.append(body_entry);
						break;
					}
					entry->Body = new_body;

					CodeTypedef tdef = parse_typedef(token_fmt("name", entry->Name, stringize( typedef struct <name> <name>; )));
					memory.append(entry);
					memory.append(tdef);
				}
				break;
				case CT_Preprocess_If:
				{
					b32 found = ignore_preprocess_cond_block(txt("! GEN_C_LIKE_CPP"), entry, parsed_memory, memory );
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
		for ( Code entry = parsed_strings.begin(); entry != parsed_strings.end(); ++ entry ) switch (entry->Type)
		{
			case CT_Preprocess_If:
			{
				CodePreprocessCond cond = cast(CodePreprocessCond, entry);
				if (cond->Content.is_equal(txt("GEN_COMPILER_C")))
				{
					++ entry;                                                 // Remove #if GEN_COMPILER_C
					for ( ; entry->Type != CT_Preprocess_Else
						&&  entry->Type != CT_Preprocess_EndIf; ++ entry ) {
						strings.append(entry);                                // Preserve content
					}
					for ( ; entry->Type != CT_Preprocess_EndIf; ++ entry ) {} // Discard C++
					// #endif discarded by for loop
					break;
				}

				bool found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP"), entry, parsed_strings, strings);
				if (found) break;

				found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP"), entry, parsed_strings, strings);
				if (found) break;
			}
			break;

			case CT_Preprocess_IfDef:
			{
				ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_strings, strings );
			}
			break;

			case CT_Preprocess_IfNotDef:
			{
				log_fmt("\nIFNDEF: %SC\n", entry->Content);
				strings.append(entry);
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
				else
				{
					CodeTypedef c_def = parse_typedef(token_fmt("name", entry->Name, stringize( typedef struct <name> <name>; )));
					strings.append(c_def);
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

		CodeBody parsed_filesystem = parse_file( project_dir "dependencies/filesystem.hpp" );
		CodeBody filesystem        = def_body(CT_Global_Body);
		for ( Code entry = parsed_filesystem.begin(); entry != parsed_filesystem.end(); ++ entry )
		{
			switch (entry->Type)
			{
				case CT_Preprocess_IfDef:
				{
					b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_filesystem, filesystem );
					if (found) break;

					filesystem.append(entry);
				}
				break;

				case CT_Enum:
				{
					if (entry->Name.is_equal(txt("FileOperations")))
						continue;
					convert_cpp_enum_to_c(cast(CodeEnum, entry), filesystem);
				}
				break;

				case CT_Enum_Fwd:
				case CT_Struct_Fwd:
				case CT_Struct:
				case CT_Union:
				case CT_Union_Fwd:
				{
					StrC type_str      = codetype_to_keyword_str(entry->Type);
					StrC formated_tmpl = token_fmt_impl( 3,
						"type", type_str
					,	"name", entry->Name,
					stringize(
						typedef <type> <name> <name>;
					));
					CodeTypedef tdef = parse_typedef(formated_tmpl);
					filesystem.append(entry);
					filesystem.append(tdef);
				}
				break;

				case CT_Variable:
				{
					CodeVar var = cast(CodeVar, entry);
					if (var->Specs.has(Spec_Constexpr) > -1)
					{
						CodeDefine define = def_define(entry->Name, entry->Value->Content);
						filesystem.append(define);
						continue;
					}
					//if ( strc_contains(entry->Name, txt("Msg_Invalid_Value")))
					//{
					//	CodeDefine define = def_define(entry->Name, entry->Value->Content);
					//	printing.append(define);
					//	continue;
					//}
					filesystem.append(entry);
				}
				break;
				default:
					filesystem.append(entry);
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
			containers.append( gen_hashtable_generic_selection_interface());

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
		header.print( dump_to_scratch_and_retireve(filesystem));
		header.print( timing );
		header.print_fmt( "\nGEN_NS_END\n" );
		header.print_fmt( roll_own_dependencies_guard_end );
#pragma endregion Print Dependencies

		header.print(fmt_newline);

#if 1
#pragma region region Print Components
		header.print_fmt( "GEN_NS_BEGIN\n" );
		header.print_fmt( "GEN_API_C_BEGIN\n\n" );

		header.print_fmt("#pragma region Types\n");
		header.print( dump_to_scratch_and_retireve(types) );
		header.print( fmt_newline );
		header.print( dump_to_scratch_and_retireve( ecode ));
		header.print( fmt_newline );
		header.print( dump_to_scratch_and_retireve( eoperator ));
		header.print( fmt_newline );
		header.print( dump_to_scratch_and_retireve( especifier ));
		header.print_fmt("#pragma endregion Types\n\n");

		header.print_fmt("#pragma region AST\n");
		header.print( dump_to_scratch_and_retireve(ast) );
		header.print( dump_to_scratch_and_retireve(code_types) );
		header.print( dump_to_scratch_and_retireve(ast_types) );
		header.print_fmt("\n#pragma endregion AST\n");

		header.print_fmt( "\nGEN_API_C_END\n" );

		header.print_fmt( "GEN_NS_END\n\n" );
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
