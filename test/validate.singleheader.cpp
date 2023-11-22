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

void validate_singleheader_ast()
{
	#define root_dir "../"
	gen::init();
	log_fmt("\validate_singleheader_ast:\n");

	FileContents file       = file_read_contents( GlobalAllocator, true, root_dir "singleheader/gen/gen.hpp" );
	u64          time_start = time_rel_ms();
	CodeBody     ast        = parse_global_body( { file.size, (char const*)file.data } );
	log_fmt("\nAst generated. Time taken: %llu ms\n", time_rel_ms() - time_start);

	log_fmt("\nSerializng ast:\n");
	time_start = time_rel_ms();
	Builder
	builder = Builder::open( "gen/singleheader_copy.gen.hpp" );
	builder.print( ast );
	builder.write();
	log_fmt("Serialized. Time taken: %llu ms\n", time_rel_ms() - time_start);

	// Need to execute clang format on the generated file to get it to match the original.
	#define script_path          root_dir "scripts/"
	#define clang_format         "clang-format "
	#define cf_format_inplace    "-i "
	#define cf_style             "-style=file:" "C:/projects/gencpp/scripts/.clang-format "
	#define cf_verbose           "-verbose "

	log_fmt("\nRunning clang-format on generated file:\n");
	system( clang_format cf_format_inplace cf_style cf_verbose "gen/singleheader_copy.gen.hpp" );
	log_fmt("clang-format finished reformatting.\n");
	#undef script_path
	#undef cf_cmd
	#undef cf_format_inplace
	#undef cf_style
	#undef cf_verbse

	FileContents file_gen  = file_read_contents( GlobalAllocator, true, "gen/singleheader_copy.gen.hpp" );
	log_fmt("\nReconstructing from generated file:\n");
	         time_start = time_rel_ms();
	CodeBody ast_gen    = parse_global_body( { file_gen.size, (char const*)file_gen.data } );
	log_fmt("\nAst generated. Time taken: %llu ms\n\n", time_rel_ms() - time_start);

	time_start = time_rel_ms();

	if ( ast.is_equal( ast_gen ) )
		log_fmt( "\nPassed!: AST passed validation!\n" );
	else
		log_fmt( "\nFailed: AST did not pass validation\n" );

	log_fmt( "Time taken: %llu ms\n", time_rel_ms() - time_start );

	gen::deinit();
}
