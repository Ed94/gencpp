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

	log_fmt("generated AST!!!\n");

	s32 idx = 0;
	for ( Code entry : ast )
	{
		log_fmt("Entry %d: %s\n", idx, entry.to_string() );
		idx++;
	}

	Builder builder;
	builder.open( "singleheader_copy.gen.hpp" );
	log_fmt("\n\nserializng ast\n");
	builder.print( ast );
	builder.write();

	log_fmt("passed!!\n");
	gen::deinit();
}
