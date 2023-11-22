
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_BENCHMARK
#include "gen.hpp"
#include "gen.builder.hpp"
#include "gen.scanner.hpp"
using namespace gen;

#ifdef GEN_SYSTEM_WINDOWS
	#include <process.h>
#endif

#define path_root         "../"
#define path_project      path_root       "project/"
#define path_scripts      path_root       "scripts/"
#define path_components   path_project    "components/"
#define path_generated    path_components "gen/"
#define path_dependencies path_project    "dependencies/"
#define path_helpers      path_project    "helpers/"

void validate_file_ast( char const* path, char const* path_gen )
{
	log_fmt( "\nValidating: %s", path );

	FileContents file = file_read_contents( GlobalAllocator, true, path );
	u64          time_start = time_rel_ms();
	CodeBody     ast        = parse_global_body( { file.size, (char const*)file.data } );
	log_fmt("\n\tAst generated. Time taken: %llu ms", time_rel_ms() - time_start);

	log_fmt("\n\tSerializng ast:\n");
	time_start = time_rel_ms();
	Builder
	builder = Builder::open( path_gen );
	builder.print( ast );
	builder.write();
	log_fmt("\tSerialized. Time taken: %llu ms", time_rel_ms() - time_start);

	// Need to execute clang format on the generated file to get it to match the original.
	#define clang_format      "clang-format "
	#define cf_format_inplace "-i "
	#define cf_style          "-style=file:" "C:/projects/gencpp/scripts/.clang-format "
	#define cf_verbose        "-verbose "
	String command = String::make( GlobalAllocator, clang_format );
	command.append( cf_format_inplace );
	command.append( cf_style );
	command.append( cf_verbose );
	command.append( path_gen );
		log_fmt("\n\tRunning clang-format on generated file:\n");
		system( command );
		log_fmt("\tclang-format finished reformatting.");
	#undef cf_cmd
	#undef cf_format_inplace
	#undef cf_style
	#undef cf_verbse

	FileContents file_gen  = file_read_contents( GlobalAllocator, true, path_gen );
	log_fmt("\n\tReconstructing from generated file:");
	         time_start = time_rel_ms();
	CodeBody ast_gen    = parse_global_body( { file_gen.size, (char const*)file_gen.data } );
	log_fmt("\n\tAst generated. Time taken: %llu ms", time_rel_ms() - time_start);

	time_start = time_rel_ms();

	if ( ast.is_equal( ast_gen ) )
		log_fmt( "\n\tPassed!: AST passed validation! " );
	else
		log_fmt( "\nFailed: AST did not pass validation " );

	log_fmt( "Time taken: %llu ms\n", time_rel_ms() - time_start );
}

void validate_original_files_ast()
{
	gen::init();
	log_fmt("\nvalidate_original_files_ast:\n");

	PreprocessorDefines.append( get_cached_string( txt("GEN_DEF_INLINE")  ));
	PreprocessorDefines.append( get_cached_string( txt("GEN_IMPL_INLINE") ));

	// Helpers
	{
		#define validate( path ) validate_file_ast( path_helpers path, "gen/original/helpers/" path );
		validate( "push_ignores.inline.hpp" );
		validate( "pop_ignores.inline.hpp" );
		#undef validate
	}

	// Dependencies
	{
		#define validate( path ) validate_file_ast( path_dependencies path, "gen/original/dependencies/" path )
		validate( "header_start.hpp" );
		validate( "macros.hpp"       );
		validate( "basic_types.hpp"  );
		validate( "debug.hpp"        );
		validate( "memory.hpp"       );
		validate( "string_ops.hpp"   );
		validate( "printing.hpp"     );
		validate( "containers.hpp"   );
		validate( "hashing.hpp"      );
		validate( "strings.hpp"      );
		validate( "filesystem.hpp"   );
		validate( "timing.hpp"       );

		validate( "src_start.cpp"    );
		validate( "debug.cpp"        );
		validate( "string_ops.cpp"   );
		validate( "printing.cpp"     );
		validate( "memory.cpp"       );
		validate( "hashing.cpp"      );
		validate( "strings.cpp"      );
		validate( "filesystem.cpp"   );
		validate( "timing.cpp"       );

		validate( "parsing.cpp"      );
		validate( "parisng.hpp"      );
		#undef validate
	}

	// Components
	{
		#define validate( path ) validate_file_ast( path_components path, "gen/original/components/" path )
		validate( "header_start.hpp" );
		validate( "types.hpp" );
		validate( "gen/ecode.hpp" );
		validate( "gen/eoperator.hpp" );
		validate( "gen/especifier.hpp" );
		validate( "ast.hpp" );
		validate( "code_types.hpp" );
		validate( "ast_types.hpp" );
		validate( "interface.hpp" );
		validate( "inlines.hpp" );
		validate( "gen/ast_inlines.hpp" );
		validate( "header_end.hpp" );

		validate( "static_data.cpp" );
		validate( "ast_case_macros.cpp" );
		validate( "ast.cpp" );
		validate( "code_serialization.cpp" );
		validate( "interface.cpp" );
		validate( "interface.upfront.cpp" );
		validate( "gen/etoktype.cpp" );
		validate( "lexer.cpp" );
		validate( "parser.cpp" );
		validate( "interface.parsing.cpp" );
		validate( "interface.untyped.cpp" );
		#undef validate
	}

	gen::deinit();
}

#undef path_root
#undef path_project
#undef path_scripts
#undef path_components
#undef path_generated
#undef path_dependencies
