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
inline
Code scan_file( char const* path )
{
	FileInfo file;

	FileError error = file_open_mode( & file, EFileMode_READ, path );
	if ( error != EFileError_NONE )
	{
		GEN_FATAL( "scan_file: Could not open: %s", path );
	}

	ssize fsize = file_size( & file );
	if ( fsize <= 0 )
	{
		GEN_FATAL("scan_file: %s is empty", path );
	}

	String str = string_make_reserve( GlobalAllocator, fsize );
		file_read( & file, str, fsize );
		string_get_header(str)->Length = fsize;

	// Skip GEN_INTELLISENSE_DIRECTIVES preprocessor blocks
	// Its designed so that the directive should be the first thing in the file.
	// Anything that comes before it will also be omitted.
	{
	#define current (*scanner)
	#define matched    0
	#define move_fwd() do { ++ scanner; -- left; } while (0)
		const StrC directive_start = txt( "ifdef" );
		const StrC directive_end   = txt( "endif" );
		const StrC def_intellisense = txt("GEN_INTELLISENSE_DIRECTIVES" );

		bool        found_directive = false;
		char const* scanner         = (char const*)str;
		s32         left            = fsize;
		while ( left )
		{
			// Processing directive.
			if ( current == '#' )
			{
				move_fwd();
				while ( left && char_is_space( current ) )
					move_fwd();

				if ( ! found_directive )
				{
					if ( left && str_compare_len( scanner, directive_start.Ptr, directive_start.Len ) == matched )
					{
						scanner += directive_start.Len;
						left    -= directive_start.Len;

						while ( left && char_is_space( current ) )
							move_fwd();

						if ( left && str_compare_len( scanner, def_intellisense.Ptr, def_intellisense.Len ) == matched )
						{
							scanner += def_intellisense.Len;
							left    -= def_intellisense.Len;

							found_directive = true;
						}
					}

					// Skip to end of line
					while ( left && current != '\r' && current != '\n' )
						move_fwd();
					move_fwd();

					if ( left && current == '\n' )
						move_fwd();

					continue;
				}

				if ( left && str_compare_len( scanner, directive_end.Ptr, directive_end.Len ) == matched )
				{
					scanner += directive_end.Len;
					left    -= directive_end.Len;

					// Skip to end of line
					while ( left && current != '\r' && current != '\n' )
						move_fwd();
					move_fwd();

					if ( left && current == '\n' )
						move_fwd();

					// sptr skip_size = fsize - left;
					if ( (scanner + 2) >= ( (char const*) str + fsize ) )
					{
						mem_move( str, scanner, left );
						string_get_header(str)->Length = left;
						break;
					}

					mem_move( str, scanner, left );
					string_get_header(str)->Length = left;

					break;
				}
			}

			move_fwd();
		}
	#undef move_fwd
	#undef matched
	#undef current
	}

	file_close( & file );
	return untyped_str( string_to_strc(str) );
}

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
	CSV_Object Owner;
	Array<ADT_Node> Content;
};

typedef struct CSV_Columns2 CSV_Columns2;
struct CSV_Columns2 {
	CSV_Object Owner;
	Array<ADT_Node> Col_1;
	Array<ADT_Node> Col_2;
};

CSV_Column parse_csv_one_column(AllocatorInfo allocator, char const* path) {
	char scratch_mem[kilobytes(32)];
	Arena scratch = arena_init_from_memory( scratch_mem, sizeof(scratch_mem) );

	file_read_contents( arena_allocator_info( & scratch), file_zero_terminate, path );

	CSV_Column result;
	csv_parse( & result.owner, scratch_mem, allocator, false );
	result.Content = csv_nodes.nodes[0].nodes;
	return result;
}

CSV_Columns2 parse_csv_two_columns(AllocatorInfo allocator, char const* path) {
	char scratch_mem[kilobytes(32)];
	Arena scratch = arena_init_from_memory( scratch_mem, sizeof(scratch_mem) );

	file_read_contents( arena_allocator_info( & scratch), file_zero_terminate, path );

	CSV_Columns2 result;
	csv_parse( & result.owner, scratch_mem, allocator, false );
	result.Col_1 = csv_nodes.nodes[0].nodes;
	result.Col_2 = csv_nodes.nodes[1].nodes;
	return result;
}

#pragma endregion Scanner
