#include <cstdlib>   // for system()

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

#define path_refactor_script "./c_library.refactor"
#define path_format_style    "../scripts/.clang-format "
#define scratch_file         "gen/scratch.hpp"
#define path_base            "../base/"

Code refactor( Code code ) {
	return code_refactor_and_format(code, scratch_file, path_refactor_script, nullptr );
}
Code refactor_and_format( Code code ) {
	return code_refactor_and_format(code, scratch_file, path_refactor_script, path_format_style );
}

constexpr bool helper_use_c_definition = true;

int gen_main()
{
	gen::init();

	PreprocessorDefines.append(txt("GEN_API_C_BEGIN"));
	PreprocessorDefines.append(txt("GEN_API_C_END"));
	PreprocessorDefines.append(txt("Array("));
	PreprocessorDefines.append(txt("HashTable("));
	PreprocessorDefines.append(txt("GEN_NS_PARSER"));
	PreprocessorDefines.append(txt("GEN_NS_PARSER_BEGIN"));
	PreprocessorDefines.append(txt("GEN_NS_PARSER_END"));
	PreprocessorDefines.append(txt("Using_Code("));
	PreprocessorDefines.append(txt("Using_CodeOps("));
	PreprocessorDefines.append(txt("GEN_OPTIMIZE_MAPPINGS_BEGIN"));
	PreprocessorDefines.append(txt("GEN_OPITMIZE_MAPPINGS_END"));
	PreprocessorDefines.append(txt("GEN_PARAM_DEFAULT"));
	//PreprocessorDefines.append(txt("GEN_EXECUTION_EXPRESSION_SUPPORT"));

	Code push_ignores           = scan_file( path_base "helpers/push_ignores.inline.hpp" );
	Code pop_ignores            = scan_file( path_base "helpers/pop_ignores.inline.hpp" );
	Code c_library_header_start = scan_file( "components/header_start.hpp" );

// Header Content: Reflection and Generation

#pragma region Resolve Dependencies
	Code header_platform       = scan_file( path_base "dependencies/platform.hpp" );
	Code header_macros         = scan_file( path_base "dependencies/macros.hpp" );
	Code header_generic_macros = scan_file(           "components/generic_macros.hpp" );
	Code header_basic_types    = scan_file( path_base "dependencies/basic_types.hpp" );
	Code header_debug          = scan_file( path_base "dependencies/debug.hpp" );
	Code header_string_ops     = scan_file( path_base "dependencies/string_ops.hpp" );
	Code header_hashing        = scan_file( path_base "dependencies/hashing.hpp" );
	Code header_timing         = scan_file( path_base "dependencies/timing.hpp" );

	CodeBody parsed_header_memory = parse_file( path_base "dependencies/memory.hpp" );
	CodeBody header_memory        = def_body(CT_Global_Body);
	for ( Code entry = parsed_header_memory.begin(); entry != parsed_header_memory.end(); ++ entry ) switch (entry->Type)
	{
		case CT_Using:
		{
			log_fmt("REPLACE THIS MANUALLY: %SC\n", entry->Name);
			CodeUsing   using_ver   = cast(CodeUsing, entry);
			CodeTypedef typedef_ver = def_typedef(using_ver->Name, using_ver->UnderlyingType);

			header_memory.append(typedef_ver);
		}
		break;
		case CT_Function_Fwd:
		{
			CodeFn fn = cast(CodeFn, entry);
			header_memory.append(fn);
		}
		break;
		case CT_Function:
		{
			CodeFn fn = cast(CodeFn, entry);
			if (fn->Name.is_equal(txt("heap")))
			{
				Code heap_def = untyped_str(txt("#define heap() (AllocatorInfo){ heap_allocator_proc, nullptr }"));
				header_memory.append(heap_def);
				continue;
			}

			if (fn->Specs) {
				s32 constexpr_found = fn->Specs.remove( Spec_Constexpr );
				if (constexpr_found > -1) {
					//log_fmt("Found constexpr: %S\n", entry.to_string());
					fn->Specs.append(Spec_Inline);
				}
			}
			header_memory.append(fn);
		}
		break;
		case CT_Template:
		{
			CodeTemplate tmpl = cast(CodeTemplate, entry);
			if ( tmpl->Declaration->Name.contains(txt("swap")))
			{
				log_fmt("SWAPPED");
				CodeBody macro_swap = parse_global_body( txt(R"(
#define swap( a, b )        \
do                          \
{                           \
	typeof( a ) temp = ( a ); \
	( a )            = ( b ); \
	( b )            = temp;  \
	} while ( 0 )
)"
				));
				header_memory.append(macro_swap);
			}
		}
		break;
		case CT_Enum:
		{
			convert_cpp_enum_to_c(cast(CodeEnum, entry), header_memory);
		}
		break;
		case CT_Struct_Fwd:
		{
			CodeTypedef tdef = parse_typedef(token_fmt("name", entry->Name, stringize( typedef struct <name> <name>; )));
			header_memory.append(entry);
			header_memory.append(tdef);
		}
		break;
		case CT_Class:
		case CT_Struct:
		{
			CodeBody body     = cast(CodeBody, entry->Body);
			CodeBody new_body = def_body( entry->Body->Type );
			for ( Code body_entry = body.begin(); body_entry != body.end(); ++ body_entry ) switch (body_entry->Type)
			{
				case CT_Preprocess_If:
				{
					b32 found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP"), body_entry, body, new_body );
					if (found) break;

					new_body.append(body_entry);
				}
				break;

				default:
					new_body.append(body_entry);
				break;
			}
			entry->Body = new_body;

			CodeTypedef tdef = parse_typedef(token_fmt("name", entry->Name, stringize( typedef struct <name> <name>; )));
			header_memory.append(entry);
			header_memory.append(tdef);
		}
		break;
		case CT_Preprocess_If:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP"), entry, parsed_header_memory, header_memory );
			if (found) break;

			header_memory.append(entry);
		}
		break;
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_header_memory, header_memory );
			if (found) break;

			header_memory.append(entry);
		}
		break;
		case CT_Preprocess_Pragma:
		{
			CodePragma pragma = cast(CodePragma, entry);

			b32 found = swap_pragma_region_implementation( txt("FixedArena"), gen_fixed_arenas, entry, header_memory);
			if (found) break;

			header_memory.append(entry);
		}
		break;
		default: {
			header_memory.append(entry);
		}
		break;
	}

	CodeBody parsed_header_printing = parse_file( path_base "dependencies/printing.hpp" );
	CodeBody header_printing        = def_body(CT_Global_Body);
	for ( Code entry = parsed_header_printing.begin(); entry != parsed_header_printing.end(); ++ entry ) switch (entry->Type)
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_header_printing, header_printing );
			if (found) break;

			header_printing.append(entry);
		}
		break;
		case CT_Variable:
		{
			if ( strc_contains(entry->Name, txt("Msg_Invalid_Value")))
			{
				CodeDefine define = def_define(entry->Name, entry->Value->Content);
				header_printing.append(define);
				continue;
			}
			header_printing.append(entry);
		}
		break;
		default:
			header_printing.append(entry);
		break;
	}

	Code array_ssize         = gen_array(txt("gen_ssize"), txt("Array_gen_ssize"));
	Code array_string_cached = gen_array(txt("gen_StringCached"), txt("Array_gen_StringCached"));

	CodeBody parsed_header_strings = parse_file( path_base "dependencies/strings.hpp" );
	CodeBody header_strings        = def_body(CT_Global_Body);
	for ( Code entry = parsed_header_strings.begin(); entry != parsed_header_strings.end(); ++ entry ) switch (entry->Type)
	{
		case CT_Preprocess_If:
		{
			CodePreprocessCond cond = cast(CodePreprocessCond, entry);
			if (cond->Content.is_equal(txt("GEN_COMPILER_C")))
			{
				++ entry;                                                 // Remove #if GEN_COMPILER_C
				for ( ; entry->Type != CT_Preprocess_Else
					&&  entry->Type != CT_Preprocess_EndIf; ++ entry ) {
					header_strings.append(entry);                         // Preserve content
				}
				for ( ; entry->Type != CT_Preprocess_EndIf; ++ entry ) {} // Discard C++
				// #endif discarded by for loop
				break;
			}

			bool found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP"), entry, parsed_header_strings, header_strings);
			if (found) break;

			found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP"), entry, parsed_header_strings, header_strings);
			if (found) break;

			header_strings.append(entry);
		}
		break;

		case CT_Preprocess_IfDef:
		{
			ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_header_strings, header_strings );
		}
		break;

		case CT_Preprocess_IfNotDef:
		{
			//log_fmt("\nIFNDEF: %SC\n", entry->Content);
			header_strings.append(entry);
		}
		break;

		case CT_Struct_Fwd:
		{
			if ( entry->Name.is_equal(txt("String")) )
			{
				CodeTypedef c_def = parse_typedef(code( typedef char* String; ));
				header_strings.append(c_def);
				header_strings.append(fmt_newline);
				++ entry;
				continue;
			}
			else
			{
				CodeTypedef c_def = parse_typedef(token_fmt("name", entry->Name, stringize( typedef struct <name> <name>; )));
				header_strings.append(c_def);
			}
			header_strings.append(entry);
		}
		break;

		case CT_Struct:
		{
			CodeBody body     = cast(CodeBody, entry->Body);
			CodeBody new_body = def_body( entry->Body->Type );
			for ( Code body_entry = body.begin(); body_entry != body.end(); ++ body_entry ) switch (body_entry->Type)
			{
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
			header_strings.append(entry);
		}
		break;

		case CT_Typedef:
		{
			StrC name_string_table = txt("StringTable");

			CodeTypedef td = cast(CodeTypedef, entry);
			if (td->Name.contains(name_string_table))
			{
				CodeBody ht = gen_hashtable(txt("gen_StrC"), name_string_table);
				header_strings.append(ht);
				break;
			}
			header_strings.append(td);
		}
		break;

		default:
			header_strings.append(entry);
		break;
	}

	CodeBody array_u8 = gen_array(txt("gen_u8"), txt("Array_gen_u8"));

	CodeBody parsed_header_filesystem = parse_file( path_base "dependencies/filesystem.hpp" );
	CodeBody header_filesystem        = def_body(CT_Global_Body);
	for ( Code entry = parsed_header_filesystem.begin(); entry != parsed_header_filesystem.end(); ++ entry ) switch (entry->Type)
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_header_filesystem, header_filesystem );
			if (found) break;

			header_filesystem.append(entry);
		}
		break;

		case CT_Enum:
		{
			if (entry->Name.is_equal(txt("FileOperations")))
				continue;
			convert_cpp_enum_to_c(cast(CodeEnum, entry), header_filesystem);
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
			header_filesystem.append(entry);
			header_filesystem.append(tdef);
		}
		break;

		case CT_Variable:
		{
			CodeVar var = cast(CodeVar, entry);
			if (var->Specs.has(Spec_Constexpr) > -1)
			{
				CodeDefine define = def_define(entry->Name, entry->Value->Content);
				header_filesystem.append(define);
				continue;
			}
			header_filesystem.append(entry);
		}
		break;
		default:
			header_filesystem.append(entry);
		break;
	}

	CodeBody array_adt_node = gen_array(txt("gen_ADT_Node"), txt("Array_gen_ADT_Node"));

	CodeBody parsed_header_parsing = parse_file( path_base "dependencies/parsing.hpp" );
	CodeBody header_parsing        = def_body(CT_Global_Body);
	for ( Code entry = parsed_header_parsing.begin(); entry != parsed_header_parsing.end(); ++ entry ) switch (entry->Type)
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_header_parsing, header_parsing );
			if (found) break;

			header_parsing.append(entry);
		}
		break;

		case CT_Preprocess_Pragma:
		{
			if ( entry->Content.contains(txt("ADT")) )
			{
				header_parsing.append(entry);
				header_parsing.append(fmt_newline);

				// Add ADT_Node forward and typedef early.
				CodeStruct  adt_node_fwd     = parse_struct(code( struct gen_ADT_Node; ));
				CodeTypedef adt_node_typedef = parse_typedef(code( typedef struct gen_ADT_Node gen_ADT_Node; ));
				header_parsing.append(adt_node_fwd);
				header_parsing.append(adt_node_typedef);

				// Skip typedef since we added it
				b32 continue_for = true;
				for (Code array_entry = array_adt_node.begin(); continue_for && array_entry != array_adt_node.end(); ++ array_entry) switch (array_entry->Type)
				{
					case CT_Typedef:
					{
						// pop the array entry
						array_adt_node->NumEntries -= 1;
						Code next                   = array_entry->Next;
						Code prev                   = array_entry->Prev;
						next->Prev                  = array_entry->Prev;
						prev->Next                  = next;
						if ( array_adt_node->Front == array_entry )
							array_adt_node->Front = next;

						header_parsing.append(array_entry);
						continue_for = false;
					}
					break;
				}

			}
		}
		break;

		case CT_Enum:
		{
			convert_cpp_enum_to_c(cast(CodeEnum, entry), header_parsing);
		}
		break;

		case CT_Struct:
		{
			CodeStruct struct_def = cast(CodeStruct, entry);
			if ( struct_def->Name.is_equal(txt("ADT_Node") ) )
			{
				header_parsing.append(entry);

				// We need to define the array for ADT_Node right here.
				header_parsing.append(array_adt_node);
				header_parsing.append(fmt_newline);
				continue;
			}

			StrC type_str      = codetype_to_keyword_str(entry->Type);
			StrC formated_tmpl = token_fmt_impl( 3,
				"type", type_str
			,	"name", entry->Name,
			stringize(
				typedef <type> <name> <name>;
			));
			CodeTypedef tdef = parse_typedef(formated_tmpl);
			header_parsing.append(entry);
			header_parsing.append(tdef);
		}
		break;

		default:
		{
			header_parsing.append(entry);
		}
		break;
	}
#pragma endregion Resolve Dependencies

#pragma region Resolve Components
	// Only has operator overload definitions that C doesn't need.
	// CodeBody ast_inlines = gen_ast_inlines();

	CodeBody ecode       = gen_ecode     ( path_base "enums/ECodeTypes.csv", helper_use_c_definition );
	CodeBody eoperator   = gen_eoperator ( path_base "enums/EOperator.csv",  helper_use_c_definition );
	CodeBody especifier  = gen_especifier( path_base "enums/ESpecifier.csv", helper_use_c_definition );

	CodeBody parsed_types = parse_file( path_base "components/types.hpp" );
	CodeBody types        = def_body(CT_Global_Body);
	for ( Code entry = parsed_types.begin(); entry != parsed_types.end(); ++ entry ) switch(entry->Type)
	{
		case CT_Preprocess_If:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP"), entry, parsed_types, types );
			if (found) break;

			types.append(entry);
		}
		break;

		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_types, types );
			if (found) break;

			types.append(entry);
		}
		break;

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
			if (entry->Name.is_equal(txt("ETypenameTag")))
			{
#pragma push_macro("enum_underlying")
#undef enum_underlying
				entry->UnderlyingTypeMacro = untyped_str(token_fmt("type", entry->UnderlyingType->Name, stringize(enum_underlying(<type>))));
				entry->UnderlyingType      = CodeTypename{nullptr};
				types.append(entry);
#pragma pop_macro("enum_underlying")

				CodeTypedef entry_td = parse_typedef(code( typedef u16 ETypenameTag; ));
				types.append(entry_td);
				continue;
			}
			//log_fmt("Detected ENUM: %S", entry->Name);
			convert_cpp_enum_to_c(cast(CodeEnum, entry), types);
		}
		break;

		default:
			types.append(entry);
		break;
	}

	// Used to track which functions need generic selectors.
	Array(CodeFn) code_c_interface = array_init_reserve<CodeFn>(GlobalAllocator, 16);

	CodeBody parsed_ast = parse_file( path_base "components/ast.hpp" );
	CodeBody ast        = def_body(CT_Global_Body);
	for ( Code entry = parsed_ast.begin(); entry != parsed_ast.end(); ++ entry ) switch (entry->Type)
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_ast, ast );
			if (found) break;

			ast.append(entry);
		}
		break;

		case CT_Preprocess_If:
		{
			CodePreprocessCond cond = cast(CodePreprocessCond, entry);
			if (cond->Content.is_equal(txt("GEN_COMPILER_C")))
			{
				++ entry; // #if
				for ( ; entry != parsed_ast.end() && entry->Type != CT_Preprocess_Else; ++ entry) {
					ast.append(entry);
				}
				for ( ; entry != parsed_ast.end() && entry->Type != CT_Preprocess_EndIf; ++ entry) {}
				++ entry;        // Consume endif
				continue;
			}

			b32 found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP"), entry, parsed_ast, ast);
			if (found) break;

			ast.append(entry);
		}
		break;

		case CT_Preprocess_Pragma:
		{
			if ( ! entry->Content.contains(txt("region Code C-Interface"))) {
				continue;
			}
			// Reached the #pragma region Code C-Interface
			for (b32 continue_for = true; continue_for; ++ entry) switch(entry->Type)
			{
				default:
					// Pass through everything but function forwards or the end region pragma
					ast.append(entry);
				break;

				case CT_Function_Fwd:
				{
					// Were going to wrap usage of these procedures into generic selectors in code_types.hpp section,
					// so we're changing the namespace to code__<name>
					CodeFn fn = cast(CodeFn, entry);
					if (fn->Name.starts_with(txt("code_")))
					{
						StrC   old_prefix  = txt("code_");
						StrC   actual_name = { fn->Name.Len  - old_prefix.Len, fn->Name.Ptr + old_prefix.Len };
						String new_name    = String::fmt_buf(GlobalAllocator, "code__%SC", actual_name );

						fn->Name = get_cached_string(new_name);
						code_c_interface.append(fn);
					}
					ast.append(entry);
				}
				break;

				case CT_Preprocess_Pragma:
					// Reached the end of the interface, go back to regular ast.hpp iteration.
					ast.append(entry);
					if ( entry->Content.contains(txt("endregion Code C-Interface"))) {
						continue_for = false;
					}
				break;
			}
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
				//log_fmt("Found constexpr: %S\n", entry.to_string());
				if (var->Name.contains(txt("AST_ArrSpecs_Cap")))
				{
					Code def = untyped_str(txt(
R"(#define AST_ArrSpecs_Cap \
(                           \
	AST_POD_Size            \
	- sizeof(Code)          \
	- sizeof(StringCached)  \
	- sizeof(Code) * 2      \
	- sizeof(Token*)        \
	- sizeof(Code)          \
	- sizeof(CodeType)      \
	- sizeof(ModuleFlag)    \
	- sizeof(u32)           \
)                           \
/ sizeof(Specifier) - 1
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

	StrC code_typenames[] = {
		txt("Code"),
		txt("CodeBody"),
		txt("CodeAttributes"),
		txt("CodeComment"),
		txt("CodeClass"),
		txt("CodeConstructor"),
		txt("CodeDefine"),
		txt("CodeDestructor"),
		txt("CodeEnum"),
		txt("CodeExec"),
		txt("CodeExtern"),
		txt("CodeInclude"),
		txt("CodeFriend"),
		txt("CodeFn"),
		txt("CodeModule"),
		txt("CodeNS"),
		txt("CodeOperator"),
		txt("CodeOpCast"),
		txt("CodePragma"),
		txt("CodeParams"),
		txt("CodePreprocessCond"),
		txt("CodeSpecifiers"),
		txt("CodeStruct"),
		txt("CodeTemplate"),
		txt("CodeTypename"),
		txt("CodeTypedef"),
		txt("CodeUnion"),
		txt("CodeUsing"),
		txt("CodeVar"),
	};

	CodeBody parsed_code_types = parse_file( path_base "components/code_types.hpp" );
	CodeBody code_types        = def_body(CT_Global_Body);
	for ( Code entry = parsed_code_types.begin(); entry != parsed_code_types.end(); ++ entry ) switch( entry->Type )
	{
		case CT_Preprocess_If:
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP"), entry, parsed_code_types, code_types );
			if (found) {
				++ entry; // Skip a newline...
				break;
			}
			found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_code_types, code_types );
			if (found) break;

			code_types.append(entry);
		}
		break;

		case CT_Preprocess_Pragma: if ( entry->Content.is_equal(txt("region Code Type C-Interface")) )
		{
			code_types.append(entry);
			code_types.append(fmt_newline);
			/*
			This thing makes a:
			#define code_<interface_name>( code, ... ) _Generic( (code),                    \
				<slots> of defintions that look like: <typeof(code)>: code__<interface_name>, \
				default: gen_generic_selection (Fail case)                                    \
			) GEN_RESOLVED_FUNCTION_CALL( code, ... )                                       \
			*/
			String generic_selector = String::make_reserve(GlobalAllocator, kilobytes(2));
			for ( CodeFn fn : code_c_interface )
			{
				generic_selector.clear();
				StrC   private_prefix  = txt("code__");
				StrC   actual_name     = { fn->Name.Len - private_prefix.Len, fn->Name.Ptr + private_prefix.Len };
				String interface_name  = String::fmt_buf(GlobalAllocator, "code_%SC", actual_name );

				// Resolve generic's arguments
				b32    has_args   = fn->Params->NumEntries > 1;
				String params_str = String::make_reserve(GlobalAllocator, 32);
				for (CodeParams param = fn->Params->Next; param != fn->Params.end(); ++ param) {
					// We skip the first parameter as its always going to be the code for selection
					if (param->Next == nullptr) {
						params_str.append_fmt( "%SC", param->Name );
						continue;
					}
					params_str.append_fmt( "%SC, ", param->Name );
				}

				char const* tmpl_def_start = nullptr;
				if (has_args) {
					tmpl_def_start =
R"(#define <interface_name>( code, <params> ) _Generic( (code),  \
)";
				}
				else {
					tmpl_def_start =
R"(#define <interface_name>( code ) _Generic( (code), \
)";
				}
				// Definition start
				generic_selector.append( token_fmt(
							"interface_name", interface_name.to_strc()
					,		"params",         params_str.to_strc() // Only used if has_args
					, tmpl_def_start
				));

				// Append slots
				for(StrC type : code_typenames ) {
					generic_selector.append_fmt("%SC : %SC,\\\n", type, fn->Name );
				}
				generic_selector.append(txt("default: gen_generic_selection_fail \\\n"));

				char const* tmpl_def_end = nullptr;
				if (has_args) {
					tmpl_def_end = txt("\t)\tGEN_RESOLVED_FUNCTION_CALL( (<type>)code, <params> )");
				}
				else {
					tmpl_def_end = txt("\t)\tGEN_RESOLVED_FUNCTION_CALL( (<type>)code )");
				}
				// Definition end
				generic_selector.append( token_fmt(
						"params", params_str.to_strc()
				, 	"type",   fn->Params->ValueType->Name
				,		tmpl_def_end ) );

				code_types.append( untyped_str(generic_selector) );
				code_types.append( fmt_newline);
				code_types.append( fmt_newline);
			}
		}
		else {
			code_types.append(entry); // Ignore the pragma otherwise
		}
		break;

		default:
			code_types.append(entry);
		break;
	}

	CodeBody parsed_ast_types = parse_file( path_base "components/ast_types.hpp" );
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
			CodeBody body = cast(CodeBody, entry->Body);
			for ( Code body_entry = body.begin(); body_entry != body.end(); ++ body_entry ) switch (body_entry->Type)
			{
				case CT_Union:
				{
					Code union_entry = body_entry->Body->Front;
					if ( body_entry && union_entry->Name.is_equal(txt("_PAD_")) )
					{
						char conversion_buf[32] = {};
						u64_to_str(size_of(AST_Body::_PAD_), conversion_buf, 10);

						StrC arr_exp  = union_entry->ValueType->ArrExpr->Content;
						StrC cpp_size = to_strc_from_c_str(conversion_buf);
						union_entry->ValueType->ArrExpr = untyped_str( cpp_size );
						union_entry->InlineCmt          = untyped_str(token_fmt("arr_exp", arr_exp,
							"// Had to hardcode _PAD_ because (<arr_exp>) was 67 bytes in C\n"
						));
					}
				}
			}
			CodeTypedef tdef = parse_typedef(token_fmt("name", entry->Name, stringize( typedef struct <name> <name>; )));
			ast_types.append(entry);
			ast_types.append(tdef);
		}
		break;

		default:
			ast_types.append(entry);
		break;
	}

	CodeBody parsed_interface = parse_file( path_base "components/interface.hpp" );
	CodeBody interface        = def_body(CT_Global_Body);
	for ( Code entry = parsed_interface.begin(); entry != parsed_interface.end(); ++ entry ) switch( entry->Type )
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_interface, interface );
			if (found) break;

			interface.append(entry);
		}
		break;

		case CT_Function_Fwd:
		case CT_Function:
		{
			CodeFn fn = cast(CodeFn, entry);
			Code prev = entry->Prev;

			if (prev && prev->Name.is_equal(entry->Name)) {
				// rename second definition so there isn't a symbol conflict
				String postfix_arr = String::fmt_buf(GlobalAllocator, "%SC_arr", entry->Name);
				entry->Name = get_cached_string(postfix_arr.to_strc());
				postfix_arr.free();
			}

			b32 handled= false;
			for ( CodeParams opt_param : fn->Params ) if (opt_param->ValueType->Name.starts_with(txt("Opts_")))
			{
				// Convert the definition to use a default struct: https://vxtwitter.com/vkrajacic/status/1749816169736073295
				StrC prefix      = txt("def_");
				StrC actual_name = { fn->Name.Len  - prefix.Len, fn->Name.Ptr + prefix.Len };
				StrC new_name    = String::fmt_buf(GlobalAllocator, "def__%SC", actual_name ).to_strc();

				// Resolve define's arguments
				b32    has_args   = fn->Params->NumEntries > 1;
				String params_str = String::make_reserve(GlobalAllocator, 32);
				for (CodeParams other_param = fn->Params; other_param != opt_param; ++ other_param) {
					if ( other_param == opt_param ) {
						params_str.append_fmt( "%SC", other_param->Name );
						break;
					}
					// If there are arguments before the optional, prepare them here.
					params_str.append_fmt( "%SC, ", other_param->Name );
				}
				char const* tmpl_fn_macro = nullptr;
				if (params_str.length() > 0 ) {
					tmpl_fn_macro= "#define <def_name>( <params> ... ) <def__name>( <params> (<opts_type>) { __VA_ARGS__ } )\n";
				}
				else {
					tmpl_fn_macro= "#define <def_name>( ... ) <def__name>( (<opts_type>) { __VA_ARGS__ } )\n";
				}
				Code fn_macro = untyped_str(token_fmt(
						"def_name",  fn->Name
				,		"def__name", new_name
				,		"params",    params_str.to_strc()
				,		"opts_type", opt_param->ValueType->Name
				,	tmpl_fn_macro
				));

				fn->Name = get_cached_string(new_name);
				interface.append(fn);
				interface.append(fn_macro);
				if (entry->Next && entry->Next->Type != CT_NewLine) {
					interface.append(fmt_newline);
				}
				handled = true;
				break;
			}

			if (! handled)
				interface.append(fn);
		}
		break;

		case CT_Struct:
		{
			CodeTypedef tdef = parse_typedef(token_fmt("name", entry->Name, stringize( typedef struct <name> <name>; )));
			interface.append(entry);
			interface.append(tdef);
			interface.append(fmt_newline);
		}
		break;

		default:
			interface.append(entry);
		break;
	}

	CodeBody parsed_inlines = parse_file( path_base "components/inlines.hpp" );
	CodeBody inlines        = def_body(CT_Global_Body);
	for ( Code entry = parsed_inlines.begin(); entry != parsed_inlines.end(); ++ entry ) switch( entry->Type )
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_inlines, inlines );
			if (found) break;

			inlines.append(entry);
		}
		break;

		case CT_Function:
		{
			// Were going to wrap usage of these procedures into generic selectors in code_types.hpp section,
			// so we're changing the namespace to code__<name>
			CodeFn fn = cast(CodeFn, entry);
			if (fn->Name.starts_with(txt("code_")))
			{
				StrC   old_prefix  = txt("code_");
				StrC   actual_name = { fn->Name.Len  - old_prefix.Len, fn->Name.Ptr + old_prefix.Len };
				String new_name    = String::fmt_buf(GlobalAllocator, "code__%SC", actual_name );

				fn->Name = get_cached_string(new_name);
			}
			inlines.append(entry);
		}
		break;

		default:
			inlines.append(entry);
		break;
	}

	CodeBody parsed_header_builder = parse_file( path_base "auxillary/builder.hpp" );
	CodeBody header_builder        = def_body(CT_Global_Body);
	for ( Code entry = parsed_header_builder.begin(); entry != parsed_header_builder.end(); ++ entry ) switch( entry->Type )
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_header_builder, header_builder );
			if (found) break;

			header_builder.append(entry);
		}
		break;

		case CT_Preprocess_If:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP"), entry, parsed_header_builder, header_builder );
			if (found) break;

			found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP"), entry, parsed_header_builder, header_builder );
			if (found) break;

			header_builder.append(entry);
		}
		break;

		case CT_Struct:
		{
			CodeBody body     = cast(CodeBody, entry->Body);
			CodeBody new_body = def_body(CT_Struct_Body);
			for ( Code body_entry = body.begin(); body_entry != body.end(); ++ body_entry ) switch(body_entry->Type)
			{
				case CT_Preprocess_If:
				{
					b32 found = ignore_preprocess_cond_block(txt("GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP"), body_entry, body, new_body );
					if (found) break;

					new_body.append(body_entry);
				}
				break;

				default:
					new_body.append(body_entry);
				break;
			}
			if ( new_body->NumEntries > 0 ) {
				entry->Body = new_body;
			}

			header_builder.append(entry);
		}
		break;

		default:
			header_builder.append(entry);
		break;
	}

	s32 idx = 0;
	CodeBody parsed_header_end = parse_file( path_base "components/header_end.hpp" );
	CodeBody header_end        = def_body(CT_Global_Body);
	for ( Code entry = parsed_header_end.begin(); entry != parsed_header_end.end(); ++ entry, ++ idx ) switch( entry->Type )
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_header_end, header_end );
			if (found) break;

			header_end.append(entry);
		}
		break;

		case CT_Variable:
		{
			CodeVar var = cast(CodeVar, entry);
			if (var->Specs)
			{
				s32 constexpr_found = var->Specs.remove( Spec_Constexpr );
				if (constexpr_found > -1) {
					CodeDefine define = def_define(entry->Name, entry->Value->Content);
					header_end.append(define);
					continue;
				}
			}
			header_end.append(entry);
		}
		break;

		default:
			header_end.append(entry);
		break;
	}
#pragma endregion Resolve Components

// Source Content : Reflection and Generation

#pragma region Resolve Dependencies
	Code src_dep_start  = scan_file( path_base "dependencies/src_start.cpp" );
	Code src_debug      = scan_file( path_base "dependencies/debug.cpp" );
	Code src_string_ops = scan_file( path_base "dependencies/string_ops.cpp" );
	Code src_printing   = scan_file( path_base "dependencies/printing.cpp" );
	Code src_memory     = scan_file( path_base "dependencies/memory.cpp" );
	Code src_hashing    = scan_file( path_base "dependencies/hashing.cpp" );
	Code src_strings    = scan_file( path_base "dependencies/strings.cpp" );
	Code src_filesystem = scan_file( path_base "dependencies/filesystem.cpp" );
	Code src_timing     = scan_file( path_base "dependencies/timing.cpp" );
#pragma endregion Resolve Dependencies

#pragma region Resolve Components
	CodeBody array_arena = gen_array(txt("gen_Arena"), txt("Array_gen_Arena"));
	CodeBody array_pool  = gen_array(txt("gen_Pool"),  txt("Array_gen_Pool"));
	CodeBody array_token = gen_array(txt("gen_Token"), txt("Array_gen_Token"));

	Code src_start              = scan_file(           "components/src_start.c" );
	Code src_static_data 	      = scan_file( path_base "components/static_data.cpp" );
	Code src_ast_case_macros    = scan_file( path_base "components/ast_case_macros.cpp" );
	Code src_code_serialization = scan_file( path_base "components/code_serialization.cpp" );
	Code src_interface          = scan_file( path_base "components/interface.cpp" );
	Code src_parsing_interface  = scan_file( path_base "components/interface.parsing.cpp" );
	Code src_untyped            = scan_file( path_base "components/interface.untyped.cpp" );

	CodeBody parsed_src_ast = parse_file( path_base "components/ast.cpp" );
	CodeBody src_ast        = def_body(CT_Global_Body);
	for ( Code entry = parsed_src_ast.begin(); entry != parsed_src_ast.end(); ++ entry ) switch( entry ->Type )
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_src_ast, src_ast );
			if (found) break;

			src_ast.append(entry);
		}
		break;

		case CT_Function:
		{
			// Were going to wrap usage of these procedures into generic selectors in code_types.hpp section,
			// so we're changing the namespace to code__<name>
			CodeFn fn = cast(CodeFn, entry);
			if (fn->Name.starts_with(txt("code_")))
			{
				StrC   old_prefix  = txt("code_");
				StrC   actual_name = { fn->Name.Len  - old_prefix.Len, fn->Name.Ptr + old_prefix.Len };
				String new_name    = String::fmt_buf(GlobalAllocator, "code__%SC", actual_name );

				fn->Name = get_cached_string(new_name);
			}
			src_ast.append(entry);
		}
		break;

		default:
			src_ast.append(entry);
		break;
	}

	CodeBody parsed_src_upfront = parse_file( path_base "components/interface.upfront.cpp" );
	CodeBody src_upfront        = def_body(CT_Global_Body);
	for ( Code entry = parsed_src_upfront.begin(); entry != parsed_src_upfront.end(); ++ entry ) switch( entry ->Type )
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_src_upfront, src_upfront );
			if (found) break;

			src_upfront.append(entry);
		}
		break;

		case CT_Enum: {
			convert_cpp_enum_to_c(cast(CodeEnum, entry), src_upfront);
		}
		break;

		case CT_Function:
		{
			CodeFn fn = cast(CodeFn, entry);
			Code prev = entry->Prev;

			for ( CodeParams arr_param : fn->Params )
				if (	fn->Name.starts_with(txt("def_"))
					&&	(		(arr_param->ValueType->Name.starts_with(txt("Specifier")) && fn->Params->NumEntries > 1)
							||	arr_param->ValueType->Name.starts_with(txt("Code")) )
				)
				{
					// rename second definition so there isn't a symbol conflict
					String postfix_arr = String::fmt_buf(GlobalAllocator, "%SC_arr", fn->Name);
					fn->Name = get_cached_string(postfix_arr.to_strc());
					postfix_arr.free();
				}

			for ( CodeParams opt_param : fn->Params ) if (opt_param->ValueType->Name.starts_with(txt("Opts_")))
			{
				StrC prefix      = txt("def_");
				StrC actual_name = { fn->Name.Len  - prefix.Len, fn->Name.Ptr + prefix.Len };
				StrC new_name    = String::fmt_buf(GlobalAllocator, "def__%SC", actual_name ).to_strc();

				fn->Name = get_cached_string(new_name);
			}

			src_upfront.append(fn);
		}
		break;

		default:
			src_upfront.append(entry);
		break;
	}

	CodeBody parsed_src_lexer = parse_file( path_base "components/lexer.cpp" );
	CodeBody src_lexer        = def_body(CT_Global_Body);
	for ( Code entry = parsed_src_lexer.begin(); entry != parsed_src_lexer.end(); ++ entry ) switch( entry ->Type )
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_src_lexer, src_lexer );
			if (found) break;

			src_lexer.append(entry);
		}
		break;

		case CT_Enum:
		{
			if (entry->Name.Len)
			{
				convert_cpp_enum_to_c(cast(CodeEnum, entry), src_lexer);
				break;
			}

			src_lexer.append(entry);
		}
		break;

		case CT_Struct:
		{
			if ( entry->Name.is_equal(txt("Token")))
			{
				// Add struct Token forward and typedef early.
				CodeStruct  token_fwd     = parse_struct(code( struct Token; ));
				CodeTypedef token_typedef = parse_typedef(code( typedef struct Token Token; ));
				src_lexer.append(token_fwd);
				src_lexer.append(token_typedef);

				// Skip typedef since we added it
				b32 continue_for = true;
				for (Code array_entry = array_token.begin(); continue_for && array_entry != array_token.end(); ++ array_entry) switch (array_entry->Type)
				{
					case CT_Typedef:
					{
						// pop the array entry
						array_token->NumEntries -= 1;
						Code next                   = array_entry->Next;
						Code prev                   = array_entry->Prev;
						next->Prev                  = array_entry->Prev;
						prev->Next                  = next;
						if ( array_token->Front == array_entry )
							array_token->Front = next;

						src_lexer.append(array_entry);
						continue_for = false;
					}
					break;
				}

				// Append the struct
				src_lexer.append(entry);

				// Append the token array
				src_lexer.append(array_token);
				continue;
			}

			CodeTypedef struct_tdef = parse_typedef(token_fmt("name", entry->Name, stringize( typedef struct <name> <name>; )));
			src_lexer.append(entry);
			src_lexer.append(struct_tdef);
		}
		break;

		case CT_Variable:
		{
			CodeVar var = cast(CodeVar, entry);
			if (var->Specs && var->Specs.has(Spec_Constexpr) > -1) {
				Code define_ver = untyped_str(token_fmt(
						"name",  var->Name
					,	"value", var->Value->Content
					,	"type",  var->ValueType.to_string().to_strc()
					,	"#define <name> (<type>) <value>\n"
				));
				src_lexer.append(define_ver);
				continue;
			}
			src_lexer.append(entry);
		}
		break;

		default:
			src_lexer.append(entry);
		break;
	}

	CodeBody array_code_typename = gen_array(txt("gen_CodeTypename"), txt("Array_gen_CodeTypename"));

	CodeBody parsed_src_parser = parse_file( path_base "components/parser.cpp" );
	CodeBody src_parser        = def_body(CT_Global_Body);
	for ( Code entry = parsed_src_parser.begin(); entry != parsed_src_parser.end(); ++ entry ) switch( entry ->Type )
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_src_parser, src_parser );
			if (found) break;

			src_parser.append(entry);
		}
		break;

		case CT_Struct:
		{
			CodeTypedef tdef = parse_typedef(token_fmt("name", entry->Name, stringize( typedef struct <name> <name>; )));
			src_parser.append(tdef);
			src_parser.append(entry);
		}
		break;

		case CT_Variable:
		{
			CodeVar var = cast(CodeVar, entry);
			if (var->Specs && var->Specs.has(Spec_Constexpr) > -1) {
				Code define_ver = untyped_str(token_fmt(
						"name",  var->Name
					,	"value", var->Value->Content
					,	"type",  var->ValueType.to_string().to_strc()
					,	"#define <name> (<type>) <value>\n"
				));
				src_parser.append(define_ver);
				continue;
			}
			src_parser.append(entry);
		}
		break;

		default:
			src_parser.append(entry);
		break;
	}
#pragma endregion Resolve Components

	// THERE SHOULD BE NO NEW GENERIC CONTAINER DEFINTIONS PAST THIS POINT (It will not have slots for the generic selection generated macros)
	CodeBody containers = def_body(CT_Global_Body);
	{
		containers.append( def_pragma(code(region Containers)));

		containers.append( gen_array_base() );

		containers.append( gen_array_generic_selection_interface());
		containers.append( gen_hashtable_base() );
		containers.append( fmt_newline);
		containers.append( gen_hashtable_generic_selection_interface());

		containers.append( array_ssize);
		containers.append( array_u8);

		containers.append( def_pragma(code(endregion Containers)));
		containers.append( fmt_newline);
	}

// Printing : Everything below is jsut serialization & formatting to a singleheader file & segmented set of files

#pragma region Refactored / Formatted
 	Code r_header_platform     = refactor(header_platform);
	Code r_header_macros       = refactor(header_macros);
	Code r_header_basic_types  = refactor(header_basic_types);
	Code r_header_debug        = refactor(header_debug);
	Code rf_header_memory      = refactor_and_format(header_memory);
	Code rf_header_printing    = refactor_and_format(header_printing);
	Code r_header_string_ops   = refactor(header_string_ops);
	Code rf_containers         = refactor_and_format(containers);
	Code r_header_hashing      = refactor(header_hashing);
	Code rf_header_strings     = refactor_and_format(header_strings);
	Code rf_header_filesystem  = refactor_and_format(header_filesystem);
	Code r_header_timing       = refactor(header_timing);
	Code rf_header_parsing     = refactor_and_format(header_parsing);

	Code rf_types      = refactor_and_format(types);
	Code rf_ecode      = refactor_and_format(ecode);
	Code rf_eoperator  = refactor_and_format(eoperator);
	Code rf_especifier = refactor_and_format(especifier);
	Code rf_ast        = refactor_and_format(ast);
	Code rf_code_types = refactor_and_format(code_types);
	Code rf_ast_types  = refactor_and_format(ast_types);

	Code rf_interface = refactor_and_format(interface);
	Code rf_inlines   = refactor_and_format(inlines);

	Code rf_array_string_cached = refactor_and_format(array_string_cached);
	Code rf_header_end          = refactor_and_format(header_end);
	Code rf_header_builder      = refactor_and_format(header_builder);
	Code rf_header_scanner      = refactor_and_format( scan_file( path_base "auxillary/scanner.hpp" ));

	Code r_src_dep_start  = refactor(src_dep_start);
	Code r_src_debug      = refactor(src_debug);
	Code r_src_string_ops = refactor(src_string_ops);
	Code r_src_printing   = refactor(src_printing);
	Code r_src_memory     = refactor(src_memory);
	Code r_src_hashing    = refactor(src_hashing);
	Code r_src_strings    = refactor(src_strings);
	Code r_src_filesystem = refactor(src_filesystem);
	Code r_src_timing     = refactor(src_timing);

	Code rf_src_parsing = refactor_and_format( scan_file( path_base "dependencies/parsing.cpp" ));

	Code rf_array_arena           = refactor_and_format(array_arena);
	Code rf_array_pool            = refactor_and_format(array_pool);
	Code r_src_static_data        = refactor(src_static_data);
	Code r_src_ast_case_macros    = refactor(src_ast_case_macros);
	Code r_src_ast                = refactor(src_ast);
	Code r_src_code_serialization = refactor(src_code_serialization);

	Code r_src_interface        = refactor(src_interface);
	Code r_src_upfront          = refactor_and_format(src_upfront);
	Code r_src_lexer            = refactor_and_format(src_lexer);
	Code rf_array_code_typename = refactor_and_format(array_code_typename);
 	Code rf_src_parser          = refactor_and_format(src_parser);
	Code r_src_parsing          = refactor(src_parsing_interface);
	Code r_src_untyped          = refactor(src_untyped);
	
	CodeBody etoktype    = gen_etoktype( path_base "enums/ETokType.csv", path_base "enums/AttributeTokens.csv", helper_use_c_definition );
	Code     rf_etoktype = refactor_and_format(etoktype);

	Code rf_src_builder = refactor_and_format( scan_file( path_base "auxillary/builder.cpp" ));
	Code rf_src_scanner = refactor_and_format( scan_file( path_base "auxillary/scanner.cpp" ));
#pragma endregion Refactored / Formatted

#pragma region Singleheader
	Builder
	header = Builder::open( "gen/gen_singleheader.h" );
	header.print_fmt( generation_notice );
	header.print_fmt("#pragma once\n\n");
	header.print( push_ignores );

	header.print( c_library_header_start );

	// Header files
	{
	#pragma region Print Dependencies
		header.print_fmt( roll_own_dependencies_guard_start );
		header.print( r_header_platform );
		header.print_fmt( "\nGEN_NS_BEGIN\n" );

		header.print( r_header_macros );
		header.print( header_generic_macros );
		header.print( r_header_basic_types );
		header.print( r_header_debug );
		header.print( rf_header_memory );
		header.print( rf_header_printing);
		header.print( r_header_string_ops );
		header.print( fmt_newline);
		header.print( rf_containers);
		header.print( r_header_hashing );
		header.print( rf_header_strings);
		header.print( rf_header_filesystem);
		header.print( r_header_timing );
		header.print(rf_header_parsing );

		header.print_fmt( "\nGEN_NS_END\n" );
		header.print_fmt( roll_own_dependencies_guard_end );
	#pragma endregion Print Dependencies

		header.print(fmt_newline);

	#pragma region region Print Components
		header.print_fmt( "GEN_NS_BEGIN\n" );
		header.print_fmt( "GEN_API_C_BEGIN\n\n" );

		header.print_fmt("#pragma region Types\n");
		header.print( rf_types );
		header.print( fmt_newline );
		header.print( rf_ecode );
		header.print( fmt_newline );
		header.print( rf_eoperator );
		header.print( fmt_newline );
		header.print( rf_especifier );
		header.print_fmt("#pragma endregion Types\n\n");

		header.print_fmt("#pragma region AST\n");
		header.print( rf_ast );
		header.print( rf_code_types );
		header.print( rf_ast_types );
		header.print_fmt("\n#pragma endregion AST\n");

		header.print( rf_interface );
		header.print(fmt_newline);

		header.print_fmt("#pragma region Inlines\n");
		header.print( rf_inlines );
		header.print_fmt("#pragma endregion Inlines\n");

		header.print(fmt_newline);
		header.print( rf_array_string_cached );

		header.print( rf_header_end );
		header.print( rf_header_builder );
		header.print( rf_header_scanner );

		header.print_fmt( "\nGEN_API_C_END\n" );
		header.print_fmt( "GEN_NS_END\n\n" );
	#pragma endregion Print Compoennts
	}

	// Source files
	{
		header.print_fmt( implementation_guard_start );

	#pragma region Print Dependencies
		header.print_fmt( roll_own_dependencies_guard_start );
		header.print_fmt( "GEN_NS_BEGIN\n");
		header.print_fmt( "GEN_API_C_BEGIN\n" );

		header.print( r_src_dep_start );
		header.print( r_src_debug );
		header.print( r_src_string_ops );
		header.print( r_src_printing );
		header.print( r_src_memory );
		header.print( r_src_hashing );
		header.print( r_src_strings );
		header.print( r_src_filesystem );
		header.print( r_src_timing );
		header.print( rf_src_parsing );

		header.print_fmt( "GEN_NS_END\n");
		header.print_fmt( roll_own_dependencies_guard_end );
	#pragma endregion Print Dependencies

	#pragma region Print Components
		header.print_fmt( "\nGEN_NS_BEGIN\n");

		header.print( fmt_newline);
		header.print( rf_array_arena );
		header.print( fmt_newline);
		header.print( rf_array_pool);

		header.print( r_src_static_data );
		header.print( fmt_newline);

		header.print_fmt( "#pragma region AST\n\n" );
		header.print( r_src_ast_case_macros );
		header.print( r_src_ast );
		header.print( r_src_code_serialization );
		header.print_fmt( "#pragma endregion AST\n\n" );

		header.print_fmt( "#pragma region Interface\n" );
		header.print( r_src_interface );
		header.print( r_src_upfront );
		header.print_fmt( "\n#pragma region Parsing\n\n" );
		header.print( rf_etoktype );
		header.print( r_src_lexer );
		header.print( fmt_newline);
		header.print( rf_array_code_typename );
		header.print( fmt_newline);
		header.print( rf_src_parser );
		header.print( r_src_parsing );
		header.print_fmt( "\n#pragma endregion Parsing\n" );
		header.print( r_src_untyped );
		header.print_fmt( "\n#pragma endregion Interface\n\n");

		header.print( rf_src_builder );
		header.print( rf_src_scanner );

		header.print_fmt( "GEN_API_C_END\n" );
	#pragma endregion Print Components

		header.print_fmt( implementation_guard_end );
	}
	header.print( pop_ignores );
	header.write();
#pragma endregion Singleheader

#pragma region Segmented
	// gen_dep.h
	{
		Builder header = Builder::open( "gen/gen.dep.h");
		builder_print_fmt( header, generation_notice );
		builder_print_fmt( header, "// This file is intended to be included within gen.hpp (There is no pragma diagnostic ignores)\n" );
		header.print( r_header_platform );
		header.print_fmt( "\nGEN_NS_BEGIN\n" );

		header.print( r_header_macros );
		header.print( header_generic_macros );
		header.print( r_header_basic_types );
		header.print( r_header_debug );
		header.print( rf_header_memory );
		header.print( rf_header_printing);
		header.print( r_header_string_ops );
		header.print( fmt_newline);
		header.print( rf_containers);
		header.print( r_header_hashing );
		header.print( rf_header_strings);
		header.print( rf_header_filesystem);
		header.print( r_header_timing );
		header.print(rf_header_parsing );

		header.print_fmt( "\nGEN_NS_END\n" );
		header.write();
	}
	// gen_dep.c
	{
		Builder src = Builder::open( "gen/gen.dep.c" );
		src.print_fmt( "GEN_NS_BEGIN\n");
		src.print_fmt( "GEN_API_C_BEGIN\n" );

		builder_print_fmt(src, generation_notice );
		builder_print_fmt( src, "// This file is intended to be included within gen.cpp (There is no pragma diagnostic ignores)\n" );
		src.print( r_src_dep_start );
		src.print( r_src_debug );
		src.print( r_src_string_ops );
		src.print( r_src_printing );
		src.print( r_src_memory );
		src.print( r_src_hashing );
		src.print( r_src_strings );
		src.print( r_src_filesystem );
		src.print( r_src_timing );
		src.print( rf_src_parsing );

		src.print_fmt( "GEN_NS_END\n");
		src.write();
	}
	// gen.h
	{
		Builder header = builder_open( "gen/gen.h" );
		builder_print_fmt( header, generation_notice );
		builder_print_fmt( header, "#pragma once\n\n" );
		builder_print( header, push_ignores );
		header.print( c_library_header_start );
		header.print( scan_file( "components/header_seg_includes.h" ));
		header.print( fmt_newline );
		header.print_fmt( "GEN_NS_BEGIN\n" );
		header.print_fmt( "GEN_API_C_BEGIN\n\n" );

		header.print_fmt("#pragma region Types\n");
		header.print( rf_types );
		header.print( fmt_newline );
		header.print( rf_ecode );
		header.print( fmt_newline );
		header.print( rf_eoperator );
		header.print( fmt_newline );
		header.print( rf_especifier );
		header.print_fmt("#pragma endregion Types\n\n");

		header.print_fmt("#pragma region AST\n");
		header.print( rf_ast );
		header.print( rf_code_types );
		header.print( rf_ast_types );
		header.print_fmt("\n#pragma endregion AST\n");

		header.print( rf_interface );
		header.print(fmt_newline);

		header.print_fmt("#pragma region Inlines\n");
		header.print( rf_inlines );
		header.print_fmt("#pragma endregion Inlines\n");

		header.print(fmt_newline);
		header.print( rf_array_string_cached );

		header.print( rf_header_end );
		header.print( rf_header_builder );
		header.print( rf_header_scanner );

		header.print_fmt( "\nGEN_API_C_END\n" );
		header.print_fmt( "GEN_NS_END\n\n" );
		builder_print( header, pop_ignores );
		builder_write(header);
	}
	// gen.c
	{
		Builder src = Builder::open( "gen/gen.c" );
		builder_print_fmt( src, generation_notice );
		builder_print( src, push_ignores );
		builder_print( src, src_start );
		src.print_fmt( "\nGEN_NS_BEGIN\n");

		src.print( fmt_newline);
		src.print( rf_array_arena );
		src.print( fmt_newline);
		src.print( rf_array_pool);

		src.print( r_src_static_data );
		src.print( fmt_newline);

		src.print_fmt( "#pragma region AST\n\n" );
		src.print( r_src_ast_case_macros );
		src.print( r_src_ast );
		src.print( r_src_code_serialization );
		src.print_fmt( "#pragma endregion AST\n\n" );

		src.print_fmt( "#pragma region Interface\n" );
		src.print( r_src_interface );
		src.print( r_src_upfront );
		src.print_fmt( "\n#pragma region Parsing\n\n" );
		src.print( rf_etoktype );
		src.print( r_src_lexer );
		src.print( fmt_newline);
		src.print( rf_array_code_typename );
		src.print( fmt_newline);
		src.print( rf_src_parser );
		src.print( r_src_parsing );
		src.print_fmt( "\n#pragma endregion Parsing\n" );
		src.print( r_src_untyped );
		src.print_fmt( "\n#pragma endregion Interface\n\n");

		src.print( rf_src_builder );
		src.print( rf_src_scanner );

		src.print_fmt( "GEN_API_C_END\n" );
		src.write();
	}
#pragma endregion Segmented

	gen::deinit();
	return 0;
}
