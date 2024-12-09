#include <cstdlib>   // for system()

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

Code format_code_to_untyped( Code code )
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
	PreprocessorDefines.append(txt("GEN_PARAM_DEFAULT"));

	Code push_ignores           = scan_file( project_dir "helpers/push_ignores.inline.hpp" );
	Code pop_ignores            = scan_file( project_dir "helpers/pop_ignores.inline.hpp" );
	Code c_library_header_start = scan_file( "components/header_start.hpp" );

// Header Content: Reflection and Generation

#pragma region Resolve Dependencies
	Code header_platform     = scan_file( project_dir "dependencies/platform.hpp" );
	Code header_macros       = scan_file( project_dir "dependencies/macros.hpp" );
	Code header_basic_types  = scan_file( project_dir "dependencies/basic_types.hpp" );
	Code header_debug        = scan_file( project_dir "dependencies/debug.hpp" );
	Code header_string_ops   = scan_file( project_dir "dependencies/string_ops.hpp" );
	Code header_hashing      = scan_file( project_dir "dependencies/hashing.hpp" );
	Code header_timing       = scan_file( project_dir "dependencies/timing.hpp" );

	CodeBody parsed_header_memory = parse_file( project_dir "dependencies/memory.hpp" );
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
			b32 found = ignore_preprocess_cond_block(txt("! GEN_C_LIKE_CPP"), entry, parsed_header_memory, header_memory );
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

	CodeBody parsed_header_printing = parse_file( project_dir "dependencies/printing.hpp" );
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

	Code array_ssize         = gen_array(txt("ssize"), txt("Array_ssize"));
	Code array_string_cached = gen_array(txt("StringCached"), txt("Array_StringCached"));

	CodeBody parsed_header_strings = parse_file( project_dir "dependencies/strings.hpp" );
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
				CodeBody ht = gen_hashtable(txt("StrC"), name_string_table);
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

	CodeBody array_u8 = gen_array(txt("u8"), txt("Array_u8"));

	CodeBody parsed_header_filesystem = parse_file( project_dir "dependencies/filesystem.hpp" );
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

	CodeBody array_adt_node = gen_array(txt("ADT_Node"), txt("Array_ADT_Node"));

	CodeBody parsed_header_parsing = parse_file( project_dir "dependencies/parsing.hpp" );
	CodeBody header_parsing        = def_body(CT_Global_Body);
	for ( Code entry = parsed_header_parsing.begin(); entry != parsed_header_parsing.end(); ++ entry ) switch (entry->Type)
	{
		case CT_Preprocess_Pragma:
		{
			if ( entry->Content.contains(txt("ADT")) )
			{
				header_parsing.append(entry);
				header_parsing.append(fmt_newline);

				// Add ADT_Node forward and typedef early.
				CodeStruct  adt_node_fwd     = parse_struct(code( struct ADT_Node; ));
				CodeTypedef adt_node_typedef = parse_typedef(code( typedef struct ADT_Node ADT_Node; ));
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

	Code inlines 	= scan_file( project_dir "components/inlines.hpp" );

	CodeBody ecode       = gen_ecode     ( project_dir "enums/ECodeTypes.csv", helper_use_c_definition );
	CodeBody eoperator   = gen_eoperator ( project_dir "enums/EOperator.csv",  helper_use_c_definition );
	CodeBody especifier  = gen_especifier( project_dir "enums/ESpecifier.csv", helper_use_c_definition );

	CodeBody parsed_types = parse_file( project_dir "components/types.hpp" );
	CodeBody types        = def_body(CT_Global_Body);
	for ( Code entry = parsed_types.begin(); entry != parsed_types.end(); ++ entry ) switch(entry->Type)
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

	CodeBody parsed_ast = parse_file( project_dir "components/ast.hpp" );
	CodeBody ast        = def_body(CT_Global_Body);
	for ( Code entry = parsed_ast.begin(); entry != parsed_ast.end(); ++ entry ) switch (entry->Type)
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
							"// Had to hardcode _PAD_ because (<arr_exp>) was 67 bytes in C (Injected C++ size_of(AST_<Type>::_PAD_) from C++ side)\n"
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

	CodeBody parsed_interface = parse_file( project_dir "components/interface.hpp" );
	CodeBody interface        = def_body(CT_Global_Body);
	for ( Code entry = parsed_interface.begin(); entry != parsed_interface.end(); ++ entry ) switch( entry->Type )
	{
		case CT_Preprocess_IfDef:
		{
			b32 found = ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_code_types, code_types );
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
				String postfix_arr = String::fmt_buf(GlobalAllocator, "%SC_arr", entry->Name);
				entry->Name = get_cached_string(postfix_arr.to_strc());
				postfix_arr.free();
			}
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

	s32 idx = 0;
	CodeBody parsed_header_end = parse_file( project_dir "components/header_end.hpp" );
	CodeBody header_end        = def_body(CT_Global_Body);
	for ( Code entry = parsed_header_end.begin(); entry != parsed_header_end.end(); ++ entry, ++ idx ) switch( entry->Type )
	{
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
	Code src_impl_start = scan_file( project_dir "dependencies/src_start.cpp" );
	Code src_debug      = scan_file( project_dir "dependencies/debug.cpp" );
	Code src_string_ops = scan_file( project_dir "dependencies/string_ops.cpp" );
	Code src_printing   = scan_file( project_dir "dependencies/printing.cpp" );
	Code src_memory     = scan_file( project_dir "dependencies/memory.cpp" );
	Code src_hashing    = scan_file( project_dir "dependencies/hashing.cpp" );
	Code src_strings    = scan_file( project_dir "dependencies/strings.cpp" );
	Code src_filesystem = scan_file( project_dir "dependencies/filesystem.cpp" );
	Code src_timing     = scan_file( project_dir "dependencies/timing.cpp" );
#pragma endregion Resolve Dependencies

#pragma region Resolve Components
	CodeBody array_arena = gen_array(txt("Arena"), txt("Array_Arena"));
	CodeBody array_pool  = gen_array(txt("Pool"), txt("Array_Pool"));

	Code src_static_data 	   = scan_file( project_dir "components/static_data.cpp" );
	Code src_ast_case_macros   = scan_file( project_dir "components/ast_case_macros.cpp" );
	Code src_ast               = scan_file( project_dir "components/ast.cpp" );
	Code src_code              = scan_file( project_dir "components/code_serialization.cpp" );
	Code src_interface         = scan_file( project_dir "components/interface.cpp" );
	Code src_upfront           = scan_file( project_dir "components/interface.upfront.cpp" );
	Code src_lexer             = scan_file( project_dir "components/lexer.cpp" );
	Code src_parser            = scan_file( project_dir "components/parser.cpp" );
	Code src_parsing_interface = scan_file( project_dir "components/interface.parsing.cpp" );
	Code src_untyped           = scan_file( project_dir "components/interface.untyped.cpp" );
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

// Printing : Everything below is jsut serialization & formatting ot a single-file.

	Builder
	header = Builder::open( "gen/gen.h" );
	header.print_fmt( generation_notice );
	header.print_fmt("#pragma once\n\n");
	header.print( push_ignores );

	header.print( c_library_header_start );

	// Header files
	{
	#pragma region Print Dependencies
		header.print_fmt( roll_own_dependencies_guard_start );
		header.print( header_platform );
		header.print_fmt( "\nGEN_NS_BEGIN\n" );

		header.print( header_macros );
		header.print( header_basic_types );
		header.print( header_debug );
		header.print( format_code_to_untyped(header_memory) );
		header.print( format_code_to_untyped(header_printing));
		header.print( header_string_ops );
		header.print( fmt_newline);
		header.print( format_code_to_untyped(containers));
		header.print( header_hashing );
		header.print( format_code_to_untyped(header_strings));
		header.print( format_code_to_untyped(header_filesystem));
		header.print( header_timing );

		header.print_fmt( "\n#pragma region Parsing\n" );
		header.print( format_code_to_untyped(header_parsing) );
		header.print_fmt( "#pragma endregion Parsing\n\n" );

		header.print_fmt( "\nGEN_NS_END\n" );
		header.print_fmt( roll_own_dependencies_guard_end );
	#pragma endregion Print Dependencies

		header.print(fmt_newline);

	#pragma region region Print Components
		header.print_fmt( "GEN_NS_BEGIN\n" );
		header.print_fmt( "GEN_API_C_BEGIN\n\n" );

		header.print_fmt("#pragma region Types\n");
		header.print( format_code_to_untyped(types) );
		header.print( fmt_newline );
		header.print( format_code_to_untyped( ecode ));
		header.print( fmt_newline );
		header.print( format_code_to_untyped( eoperator ));
		header.print( fmt_newline );
		header.print( format_code_to_untyped( especifier ));
		header.print_fmt("#pragma endregion Types\n\n");

		header.print_fmt("#pragma region AST\n");
		header.print( format_code_to_untyped(ast) );
		header.print( format_code_to_untyped(code_types) );
		header.print( format_code_to_untyped(ast_types) );
		header.print_fmt("\n#pragma endregion AST\n");

		header.print( format_code_to_untyped(interface) );

		header.print_fmt("#pragma region Inlines\n");
		header.print( inlines );
		header.print_fmt("#pragma endregion Inlines\n");

		header.print(fmt_newline);
		header.print( format_code_to_untyped(array_string_cached));
		header.print(fmt_newline);

		header.print( format_code_to_untyped(header_end) );

		header.print_fmt( "\nGEN_API_C_END\n" );

		header.print_fmt( "GEN_NS_END\n\n" );
	#pragma endregion Print Compoennts
	}

	// Source files
	{
		header.print_fmt( implementation_guard_start );

	#pragma region Print Dependencies
		header.print_fmt( roll_own_dependencies_guard_start );
		header.print_fmt( "GEN_NS_BEGIN\n\n");

		header.print( src_impl_start );
		header.print( src_debug );
		header.print( src_string_ops );
		header.print( src_printing );
		header.print( src_memory );
		header.print( src_hashing );
		header.print( src_strings );
		header.print( src_filesystem );
		header.print( src_timing );

		header.print_fmt( "\n#pragma region Parsing\n" );
		header.print( scan_file( project_dir "dependencies/parsing.cpp" ) );
		header.print_fmt( "#pragma endregion Parsing\n\n" );

		header.print_fmt( "GEN_NS_END\n");
		header.print_fmt( roll_own_dependencies_guard_end );
	#pragma endregion Print Dependencies

	#pragma region Print Components
		CodeBody etoktype = gen_etoktype( project_dir "enums/ETokType.csv", project_dir "enums/AttributeTokens.csv" );

		header.print_fmt( "\nGEN_NS_BEGIN\n");

		header.print( format_code_to_untyped(array_arena));
		header.print( fmt_newline);
		header.print( format_code_to_untyped(array_pool));

		header.print( src_static_data );
		header.print( fmt_newline);

		header.print_fmt( "#pragma region AST\n\n" );
		header.print( src_ast_case_macros );
		 header.print( src_ast );
		// header.print( src_code );
		header.print_fmt( "#pragma endregion AST\n\n" );

		// header.print_fmt( "#pragma region Interface\n" );
		// header.print( src_interface );
		// header.print( src_upfront );
		// header.print_fmt( "\n#pragma region Parsing\n\n" );
		// header.print( format_code_to_untyped(parser_nspace) );
		// header.print( lexer );
		// header.print( parser );
		// header.print( parsing_interface );
		// header.print_fmt( "\n#pragma endregion Parsing\n" );
		// header.print( untyped );
		// header.print_fmt( "\n#pragma endregion Interface\n\n");

		// header.print_fmt( "#pragma region Builder\n" );
		// header.print( scan_file( project_dir "auxillary/builder.cpp"  ) );
		// header.print_fmt( "\n#pragma endregion Builder\n\n" );

		// header.print_fmt( "\n#pragma region Scanner\n" );
		// header.print( scan_file( project_dir "auxillary/scanner.hpp" ) );
		// header.print_fmt( "#pragma endregion Scanner\n\n" );
	#pragma endregion Print Components

		header.print_fmt( implementation_guard_end );
	}
	header.print( pop_ignores );
	header.write();

	gen::deinit();
	return 0;
#undef project_dir
}
