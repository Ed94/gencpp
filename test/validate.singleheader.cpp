#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_BENCHMARK
#include "gen.hpp"
#include "gen.builder.hpp"
#include "gen.scanner.hpp"
using namespace gen;

void validate_singleheader_ast()
{
	#define root_dir "../"
	gen::init();
	log_fmt("\nvalidate_singleheader_ast:\n");

	FileContents file = file_read_contents( GlobalAllocator, true, root_dir "singleheader/gen/gen.hpp" );

	// Duplicate and format
	{
		// Sleep(100);
		FileInfo  scratch;
		FileError error = file_open_mode( & scratch, EFileMode_WRITE, "gen/scratch.cpp" );
		if ( error != EFileError_NONE ) {
			log_failure( "gen::File::open - Could not open file: %s", "gen/scratch.cpp");
			return;
		}
		// Sleep(100);
		b32 result = file_write( & scratch, file.data, file.size );
		if ( result == false ) {
			log_failure("gen::File::write - Failed to write to file: %s\n", file_name( & scratch ) );
			file_close( & scratch );
			return;
		}
		file_close( & scratch );
		// Sleep(100);
		format_file( "gen/scratch.cpp" );
		// Sleep(100);

		file = file_read_contents( GlobalAllocator, true, "gen/scratch.cpp" );
	}

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
