#pragma once
#include "gen.hpp"

GEN_NS_BEGIN

Code scan_file( char const* path )
{
	FileInfo file;

	FileError error = file_open_mode( & file, EFileMode_READ, path );
	if ( error != EFileError_NONE )
	{
		fatal( "scan_file: Could not open genc.macro.h: %s", path );
	}

	sw fsize = file_size( & file );
	if ( fsize <= 0 )
	{
		fatal("scan_file: %s is empty", path );
	}

	String str = String::make_reserve( GlobalAllocator, fsize );
		file_read( & file, str, fsize );
		str.get_header().Length = fsize;

	file_close( & file );

	return untyped_str( str );
}

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

GEN_NS_END
