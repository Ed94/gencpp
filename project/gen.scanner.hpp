#pragma once
#include "gen.hpp"

namespace gen {

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

// namespace gen
}
