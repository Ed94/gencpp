
#ifdef GEN_INTELLISENSE_DIRECTIVES
#pragma once
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#include "../gen.cpp"

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
#endif

// Will format a file with the given style at the provided path.
// Assumes clang-format is defined in an user-exposed or system enviornment PATH.
void clang_format_file( char const* path, char const* style_path )
{
	GEN_ASSERT_NOT_NULL(path);
	String resolved_path = string_make_strc(GlobalAllocator, to_strc_from_c_str(path));

	String style_arg;
	if (style_path) {
		stle_arg = string_make_strc(GlobalAllocator, txt("-style=file:"));
		string_append_fmt( & style_arg, "%s ", style_path );
	}

	StrC clang_format      = txt("clang-format ")
	StrC cf_format_inplace = txt("-i ")
	StrC cf_verbose        = txt("-verbose ")

	String command = string_make_strc( GlobalAllocator, clang_format );
	string_append_strc( & command, cf_format_inplace );
	string_append_strc( & command, cf_verbose );
	string_append_string( & command, style_arg );
	string_append_string( & command, resolved_path );

	log_fmt("\tRunning clang-format:\n");
	system( command );
	log_fmt("\tclang-format finished formatting.\n");
}
// Will refactor a file with the given script at the provided path.
// Assumes refactor is defined in an user-exposed or system enviornment PATH.
// (See: ./gencpp/scripts/build.ci.ps1 for how)
void refactor_file( char const* path, char const* refactor_script )
{
	GEN_ASSERT_NOT_NULL(path, refactor_script);

	#define refactor

	String command = string_make_strc(GlobalAllocator, txt("refactor")));

	log_fmt("\tBeginning refactor:\n");
	system(command);
		log_fmt("\nRefactoring complete.\n");

	#undef refactor
}

Code code_refactor_and_format( Code code, char const* scratch_path, char const* refactor_script, char_const* clang_format_sytle_path )
{
	GEN_ASSERT_NOT_NULL(code);
	GEN_ASSERT_NOT_NULL(scratch_path);
	Builder scratch_file = builder_open("gen/scratch.hpp");
	builder_print( & scratch_file, code);
	builder_write(& scratch_file);

	if (refactor_script) {
		refactor_file(scratch_path, refactor_script)
	}
	if ( clang_format_sytle_path ) {
		clang_format_file(scratch_path, clang_format_sytle_path);
	}

	Code result = scan_file( scratch_path );
	remove("gen/scratch.hpp");
	return result;
}