#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_BENCHMARK
#include "gen.hpp"
#include "gen.builder.hpp"
#include "gen.scanner.hpp"
using namespace gen;

void check_singleheader_ast()
{
	#define project_dir "../"
	gen::init();
	log_fmt("\ncheck_singleheader_ast:\n");

	FileContents file       = file_read_contents( GlobalAllocator, true, project_dir "singleheader/gen/gen.hpp" );
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

	FileContents file_gen  = file_read_contents( GlobalAllocator, true, "gen/singleheader_copy.gen.hpp" );

	log_fmt("Reconstructing from generated file:\n");
	        time_start = time_rel_ms();
	CodeBody ast_gen   = parse_global_body( { file_gen.size, (char const*)file_gen.data } );

	log_fmt("\nAst generated. Time taken: %llu ms\n", time_rel_ms() - time_start);

	time_start = time_rel_ms();

	if ( ast.is_equal( ast_gen ) )
		log_fmt( "Passed!: AST passed validation!\n" );
	else
		log_fmt( "Failed: AST did not pass validation\n" );

	log_fmt( "Time taken: %llu ms\n", time_rel_ms() - time_start );

	gen::deinit();
}