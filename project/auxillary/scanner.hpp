#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "gen.hpp"
#endif

// This is a simple file reader that reads the entire file into memory.
// It has an extra option to skip the first few lines for undesired includes.
// This is done so that includes can be kept in dependency and component files so that intellisense works.
Code scan_file( char const* path )
{
	FileInfo file;

	FileError error = file_open_mode( & file, EFileMode_READ, path );
	if ( error != EFileError_NONE )
	{
		GEN_FATAL( "scan_file: Could not open: %s", path );
	}

	sw fsize = file_size( & file );
	if ( fsize <= 0 )
	{
		GEN_FATAL("scan_file: %s is empty", path );
	}

	String str = String::make_reserve( GlobalAllocator, fsize );
		file_read( & file, str, fsize );
		str.get_header().Length = fsize;

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
		char const* scanner         = str.Data;
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
					if ( left && str_compare( scanner, directive_start.Ptr, directive_start.Len ) == matched )
					{
						scanner += directive_start.Len;
						left    -= directive_start.Len;

						while ( left && char_is_space( current ) )
							move_fwd();

						if ( left && str_compare( scanner, def_intellisense.Ptr, def_intellisense.Len ) == matched )
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

				if ( left && str_compare( scanner, directive_end.Ptr, directive_end.Len ) == matched )
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
					if ( (scanner + 2) >= ( str.Data + fsize ) )
					{
						mem_move( str, scanner, left );
						str.get_header().Length = left;
						break;
					}

					mem_move( str, scanner, left );
					str.get_header().Length = left;

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
	return untyped_str( str );
}

#if 0
struct CodeFile
{
	using namespace Parser;

	String              FilePath;
	TokArray            Tokens;
	Array<ParseFailure> ParseFailures;
	Code                CodeRoot;
};

namespace Parser
{
	struct ParseFailure
	{
		String Reason;
		Code   Node;
	};
}

CodeFile scan_file( char const* path )
{
	using namespace Parser;

	CodeFile
	result = {};
	result.FilePath = String::make( GlobalAllocator, path );

	Code code = scan_file( path );
	result.CodeRoot = code;

	ParseContext context = parser_get_last_context();
	result.Tokens        = context.Tokens;
	result.ParseFailures = context.Failures;

	return result;
}
#endif
