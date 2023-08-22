#pragma once
#include "gen.hpp"

// This is a simple file reader that reads the entire file into memory.
// It has an extra option to skip the first few lines for undesired includes.
// This is done so that includes can be kept in dependency and component files so that intellisense works.
Code scan_file( char const* path, bool skip_initial_directives = true )
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

	if ( skip_initial_directives )
	{
	#define current (*scanner)
		StrC toks[] {
			txt( "pragma once" ),
			txt( "include" )
		};

		char* scanner = str;
		while ( current != '\r' && current != '\n' )
		{
			for ( StrC tok : toks )
			{
				if ( current == '#' )
				{
					++ scanner;
				}

				if ( strncmp( scanner, tok.Ptr, tok.Len ) == 0 )
				{
					scanner += tok.Len;
					while ( scanner < ( str.Data + str.length() ) && current != '\r' && current != '\n' )
					{
						++ scanner;
					}

					// Skip the line
					sptr skip_size = sptr( scanner - str.Data );
					if ( (scanner + 2) >= ( str.Data + str.length() ) )
					{
						sptr new_length = sptr( str.get_header().Length ) - skip_size;
						mem_move( str, scanner, new_length );
						str.get_header().Length = new_length;
						break;
					}

					if ( current == '\r' )
					{
						skip_size += 2;
						scanner   += 2;
					}
					else
					{
						skip_size += 1;
						scanner   += 1;
					}

					sptr new_length = sptr( str.get_header().Length ) - skip_size;
					mem_move( str, scanner, new_length );
					str.get_header().Length = new_length;

					scanner = str;
				}
			}

			++ scanner;
		}
	#undef current
	}

	file_close( & file );
	return untyped_str( str );
}

#if 0
struct Policy
{
	// Nothing for now.
};

struct SymbolInfo
{
	StringCached File;
	char const*  Marker;
	Code         Signature;
};

struct Scanner
{
	struct RequestEntry
	{
		SymbolInfo Info;
	};

	struct Receipt
	{
		StringCached File;
		Code         Defintion;
		bool         Result;
	};

	AllocatorInfo MemAlloc;

	static void set_allocator( AllocatorInfo allocator );

	Array<FileInfo>     Files;
	String              Buffer;
	Array<RequestEntry> Requests;

	void add_files( s32 num, char const** files );

	void add( SymbolInfo signature, Policy policy );

	bool process_requests( Array<Receipt> out_receipts );
};
#endif

