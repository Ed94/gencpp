#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_SUPPORT_CPP_MEMBER_FEATURES 1
#include "../project/gen.cpp"

#include "helpers/push_ignores.inline.hpp"
#include "helpers/helper.hpp"

GEN_NS_BEGIN
#include "dependencies/parsing.cpp"
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
"// This file was generated automatially by gencpp's c_library.cpp"
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
	String resolved_path = String::make(GlobalAllocator, to_str(path));

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
	return code;
}

int gen_main()
{
#define project_dir "../project/"
	gen::init();

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

		Code platform     = scan_file( project_dir "dependencies/platform.hpp" );
		Code macros       = scan_file( project_dir "dependencies/macros.hpp" );
		Code basic_types  = scan_file( project_dir "dependencies/basic_types.hpp" );
		Code debug        = scan_file( project_dir "dependencies/debug.hpp" );

		header.print_fmt( roll_own_dependencies_guard_start );
		header.print( platform );
		header.print_fmt( "\nGEN_NS_BEGIN\n" );

		header.print( macros );
		header.print( basic_types );
		header.print( debug );

		CodeBody parsed_memory = parse_file( project_dir "dependencies/memory.hpp" );
		CodeBody memory        = def_body(ECode::Struct_Body);
		for ( Code entry = parsed_memory.begin(); entry != parsed_memory.end(); ++ entry )
		{
			switch (entry->Type)
			{
				case ECode::Using:
				{
					log_fmt("REPLACE THIS MANUALLY: %S\n", entry->Name);
					CodeUsing   using_ver   = entry.cast<CodeUsing>();
					CodeTypedef typedef_ver = def_typedef(using_ver->Name, using_ver->UnderlyingType);

					memory.append(typedef_ver);
				}
				break;
				case ECode::Function:
				{
					CodeFn fn = entry.cast<CodeFn>();
					s32 constexpr_found = fn->Specs.remove( ESpecifier::Constexpr );
					if (constexpr_found > -1) {
						log_fmt("Found constexpr proc\n");
						fn->Specs.append(ESpecifier::Inline);
					}
					memory.append(entry);
				}
				break;
				case ECode::Class:
				case ECode::Struct:
				{
					CodeBody body     = entry->Body->operator CodeBody();
					CodeBody new_body = def_body( entry->Body->Type );
					for ( Code body_entry = body.begin(); body_entry != body.end(); ++ body_entry ) switch
					(body_entry->Type) {
						case ECode::Preprocess_If:
						{
							ignore_preprocess_cond_block(txt("GEN_SUPPORT_CPP_MEMBER_FEATURES"), body_entry, body );
						}
						break;

						default:
							new_body.append(body_entry);
						break;
					}

					entry->Body = rcast(AST*, new_body.ast);
					memory.append(entry);
				}
				break;
				case ECode::Preprocess_If:
				{
					ignore_preprocess_cond_block(txt("GEN_SUPPORT_CPP_MEMBER_FEATURES"), entry, parsed_memory );
				}
				break;
				case ECode::Preprocess_IfDef:
				{
					ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, parsed_memory );
				}
				break;
				case ECode::Preprocess_Pragma:
				{
					swap_pragma_region_implementation( txt("FixedArena"), gen_fixed_arenas, entry, memory);
				}
				break;
				default: {
					memory.append(entry);
				}
				break;
			}
		}

		header.print( memory );

		Code string_ops = scan_file( project_dir "dependencies/string_ops.hpp" );
		header.print( string_ops );

		CodeBody printing_parsed = parse_file( project_dir "dependencies/printing.hpp" );
		CodeBody printing        = def_body(ECode::Struct_Body);
		for ( Code entry = printing_parsed.begin(); entry != printing_parsed.end(); ++ entry )
		{
			switch (entry->Type)
			{
				case ECode::Preprocess_IfDef:
				{
					ignore_preprocess_cond_block(txt("GEN_INTELLISENSE_DIRECTIVES"), entry, printing_parsed );
				}
			}

			if (entry->Type == ECode::Variable &&
				contains(entry->Name, txt("Msg_Invalid_Value")))
			{
				CodeDefine define = def_define(entry->Name, entry->Value->Content);
				printing.append(define);
				continue;
			}
			printing.append(entry);
		}

		header.print(printing);

		CodeBody parsed_containers = parse_file( project_dir "dependencies/containers.hpp" );
		CodeBody containers        = def_body(ECode::Struct_Body);
		for ( Code entry = parsed_containers.begin(); entry != parsed_containers.end(); ++ entry )
		{
			switch ( entry->Type )
			{
				case ECode::Preprocess_Pragma:
				{
					bool found = false;

					found = swap_pragma_region_implementation( txt("Array"), gen_array_base, entry, containers);
					if (found) {
						break;
					}

					found = swap_pragma_region_implementation( txt("HashTable"), gen_hashtable_base, entry, containers);
					if (found) {
						break;
					}

					containers.append(entry);
				}
				break;
			}
		}

		header.print(containers);
	}

	header.print( pop_ignores );
	header.write();

	format_file( "gen/gen.h" );

	gen::deinit();
	return 0;
#undef project_dir
}
