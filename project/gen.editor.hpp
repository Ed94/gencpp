#pragma once

#include "gen.hpp"

namespace gen {

struct Policy
{
	// Nothing for now.
};

enum class SymbolType : u32
{
	Code,
	Line,
	Marker
};

struct Editor
{
	enum RequestType : u32
	{
		Add,
		Replace,
		Remove
	};

	struct SymbolData
	{
		Policy      Policy;
		SymbolInfo  Info;
	};

	struct RequestEntry
	{
		union {
			SymbolData Symbol;
			String     Specification;
		};
		RequestType    Type;
	};

	struct Receipt
	{
		StringCached File;
		Code         Found;
		Code         Written;
		bool         Result;
	};

	static AllocatorInfo Allocator;

	static void set_allocator( AllocatorInfo  allocator );

	Array(FileInfo)     Files;
	String              Buffer;
	Array(RequestEntry) Requests;

	void add_files( s32 num, char const** files );

	void add    ( SymbolInfo definition,  Policy policy, Code to_inject );
	void remove ( SymbolInfo definition,  Policy policy );
	void replace( SymbolInfo definition,  Policy policy, Code to_replace);

#	ifdef GEN_FEATURE_EDITOR_REFACTOR
	void refactor( char const* file_path, char const* specification_path );
#	endif

	bool process_requests( Array(Receipt) out_receipts );
};

// namespace gen
};