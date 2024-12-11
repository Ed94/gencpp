#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "helpers/push_ignores.inline.hpp"
#	include "components/header_start.hpp"
#	include "components/types.hpp"
#	include "components/gen/ecode.hpp"
#	include "components/gen/eoperator.hpp"
#	include "components/gen/especifier.hpp"
#	include "components/ast.hpp"
#	include "components/code_types.hpp"
#	include "components/ast_types.hpp"
#	include "components/interface.hpp"
#	include "components/inlines.hpp"
#	include "components/gen/ast_inlines.hpp"
#	include "components/header_end.hpp"
#endif

#pragma region Scanner

// This is a simple file reader that reads the entire file into memory.
// It has an extra option to skip the first few lines for undesired includes.
// This is done so that includes can be kept in dependency and component files so that intellisense works.
Code scan_file( char const* path );

inline
CodeBody parse_file( const char* path )
{
	FileContents file = file_read_contents( GlobalAllocator, true, path );
	CodeBody     code = parse_global_body( { file.size, (char const*)file.data } );
	log_fmt("\nParsed: %s\n", path);
	return code;
}

// The follow is basic support for light csv parsing (use it as an example)
// Make something robust if its more serious.

typedef struct CSV_Column CSV_Column;
struct CSV_Column {
	CSV_Object      ADT;
	Array<ADT_Node> Content;
};

typedef struct CSV_Columns2 CSV_Columns2;
struct CSV_Columns2 {
	CSV_Object      ADT;
	Array<ADT_Node> Col_1;
	Array<ADT_Node> Col_2;
};

inline
CSV_Column parse_csv_one_column(AllocatorInfo allocator, char const* path) {
	char scratch_mem[kilobytes(32)];
	Arena scratch = arena_init_from_memory( scratch_mem, sizeof(scratch_mem) );

	file_read_contents( arena_allocator_info( & scratch), file_zero_terminate, path );

	CSV_Column result;
	csv_parse( & result.ADT, scratch_mem, allocator, false );
	result.Content = result.ADT.nodes[0].nodes;
	return result;
}

inline
CSV_Columns2 parse_csv_two_columns(AllocatorInfo allocator, char const* path) {
	char scratch_mem[kilobytes(32)];
	Arena scratch = arena_init_from_memory( scratch_mem, sizeof(scratch_mem) );

	file_read_contents( arena_allocator_info( & scratch), file_zero_terminate, path );

	CSV_Columns2 result;
	csv_parse( & result.ADT, scratch_mem, allocator, false );
	result.Col_1 = result.ADT.nodes[0].nodes;
	result.Col_2 = result.ADT.nodes[1].nodes;
	return result;
}

#pragma endregion Scanner
