#if GEN_TIME
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_BENCHMARK
#include "gen.cpp"
#include "gen.builder.cpp"
#include "gen.scanner.cpp"
#include "sanity.cpp"
#include "SOA.cpp"

#ifdef GEN_SYSTEM_WINDOWS
	#include <process.h>
#endif

using namespace gen;

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

#include "validate.original.cpp"
#include "validate.singleheader.cpp"

int gen_main()
{
	log_fmt("\ngen_time:");

	// check_sanity();

	// check_SOA();

	// validate_original_files_ast();
	validate_singleheader_ast();

	return 0;
}
#endif


// This only has to be done if symbol conflicts occur.
#ifndef GEN_TIME
int main()
{


	return 0;
}
#endif
