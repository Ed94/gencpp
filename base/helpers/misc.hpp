#pragma once

#ifdef GEN_INTELLISENSE_DIRECTIVES
#	define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#	define GEN_ENFORCE_STRONG_CODE_TYPES
#	define GEN_EXPOSE_BACKEND
#	include "gen.hpp"
#	include "helpers/push_ignores.inline.hpp"
#	include "helpers/helper.hpp"
#	include "auxiliary/builder.hpp"
#	include "auxiliary/builder.cpp"
#	include "auxiliary/scanner.hpp"

#include <stdlib.h>

using namespace gen;
#endif

// Will format a file with the given style at the provided path.
// Assumes clang-format is defined in an user-exposed or system enviornment PATH.
void clang_format_file( char const* path, char const* style_path )
{
	GEN_ASSERT_NOT_NULL(path);
	StrBuilder resolved_path = strbuilder_make_str(_ctx->Allocator_Temp, to_str_from_c_str(path));
	StrBuilder style_arg;
	if (style_path) {
		style_arg = strbuilder_make_str(_ctx->Allocator_Temp, txt("-style=file:"));
		strbuilder_append_fmt( & style_arg, "%s ", style_path );
	}

	Str clang_format      = txt("clang-format ");
	Str cf_format_inplace = txt("-i ");
	Str cf_verbose        = txt("-verbose ");

	StrBuilder command = strbuilder_make_str( _ctx->Allocator_Temp, clang_format );
	strbuilder_append_str( & command, cf_format_inplace );
	strbuilder_append_str( & command, cf_verbose );
	strbuilder_append_string( & command, style_arg );
	strbuilder_append_string( & command, resolved_path );
	system( command );
}

// Will refactor a file with the given script at the provided path.
// Assumes refactor is defined in an user-exposed or system enviornment PATH.
// (See: ./gencpp/scripts/build.ci.ps1 for how)
void refactor_file( char const* path, char const* refactor_script )
{
	GEN_ASSERT_NOT_NULL(path);
	GEN_ASSERT_NOT_NULL(refactor_script);

	StrBuilder command = strbuilder_make_str(_ctx->Allocator_Temp, txt("refactor "));
	// strbuilder_append_str( & command, txt("-debug ") );
	strbuilder_append_str( & command, txt("-num=1 ") );
	strbuilder_append_fmt( & command, "-src=%s ", path );
	strbuilder_append_fmt( & command,"-spec=%s ", refactor_script );
	system(command);
	log_fmt("\n");
}

// Does either of the above or both to the provided code.
// Code returned will be untyped content (its be serialized)
Code code_refactor_and_format( Code code, char const* scratch_path, char const* refactor_script, char const* clang_format_sytle_path )
{
	GEN_ASSERT(code);
	GEN_ASSERT_NOT_NULL(scratch_path);
	Builder scratch_file = builder_open( scratch_path );
	builder_print( & scratch_file, code);
	builder_write(& scratch_file);

	if (refactor_script) {
		refactor_file(scratch_path, refactor_script);
	}
	if ( clang_format_sytle_path ) {
		clang_format_file(scratch_path, clang_format_sytle_path);
	}

	Code result = scan_file( scratch_path );
	::remove(scratch_path);
	return result;
}
