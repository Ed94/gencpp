#pragma once

#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_BENCHMARK
#include "gen.hpp"
#include "file_processors/scanner.hpp"
using namespace gen;

void check_singleheader_ast()
{
	#define project_dir "../../"
	gen::init();
	log_fmt("\ncheck_singleheader_ast:\n");

	FileContents file = file_read_contents( GlobalAllocator, true, project_dir "singleheader/gen/gen.hpp" );

	CodeBody ast = parse_global_body( { file.size, (char const*)file.data } );

	log_fmt("generated AST!!!");

	Builder builder;
	builder.open( "singleheader_copy.hpp" );
	log_fmt("serializng ast");
	builder.print( ast );
	builder.write();

	log_fmt("passed!!\n");
	gen::deinit();
}
