#ifdef INTELLISENSE_DIRECTIVES
#	include "scanner.hpp"
#endif

#pragma region Scanner

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

	StrBuilder str = strbuilder_make_reserve( get_context()->Allocator_Temp, fsize );
		file_read( & file, str, fsize );
		strbuilder_get_header(str)->Length = fsize;

	// Skip INTELLISENSE_DIRECTIVES preprocessor blocks
	// Its designed so that the directive should be the first thing in the file.
	// Anything that comes before it will also be omitted.
	{
	#define current (*scanner)
	#define matched    0
	#define move_fwd() do { ++ scanner; -- left; } while (0)
		const Str directive_start = txt( "ifdef" );
		const Str directive_end   = txt( "endif" );
		const Str def_intellisense = txt("INTELLISENSE_DIRECTIVES" );

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
					if ( left && c_str_compare_len( scanner, directive_start.Ptr, directive_start.Len ) == matched )
					{
						scanner += directive_start.Len;
						left    -= directive_start.Len;

						while ( left && char_is_space( current ) )
							move_fwd();

						if ( left && c_str_compare_len( scanner, def_intellisense.Ptr, def_intellisense.Len ) == matched )
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

				if ( left && c_str_compare_len( scanner, directive_end.Ptr, directive_end.Len ) == matched )
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
						strbuilder_get_header(str)->Length = left;
						break;
					}

					mem_move( str, scanner, left );
					strbuilder_get_header(str)->Length = left;

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
	return untyped_str( strbuilder_to_str(str) );
}

CodeBody parse_file( const char* path ) {
	FileContents file    = file_read_contents( get_context()->Allocator_Temp, true, path );
	Str          content = { (char const*)file.data, file.size };
	CodeBody     code    = parse_global_body( content );
	log_fmt("\nParsed: %s\n", path);
	return code;
}

CSV_Column parse_csv_one_column(AllocatorInfo allocator, char const* path) {
	FileContents content   = file_read_contents( allocator, file_zero_terminate, path );
	Arena        csv_arena = arena_init_from_memory(content.data, content.size);

	CSV_Column result;
	csv_parse( & result.ADT, rcast(char*, content.data), allocator, false );
	result.Content = result.ADT.nodes[0].nodes;
	return result;
}

CSV_Columns2 parse_csv_two_columns(AllocatorInfo allocator, char const* path) {
	FileContents content   = file_read_contents( allocator, file_zero_terminate, path );
	Arena        csv_arena = arena_init_from_memory(content.data, content.size);

	CSV_Columns2 result;
	csv_parse( & result.ADT, rcast(char*, content.data), allocator, false );
	result.Col_1 = result.ADT.nodes[0].nodes;
	result.Col_2 = result.ADT.nodes[1].nodes;
	return result;
}

#pragma endregion Scanner
