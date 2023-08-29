#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "gen.hpp"
#endif

struct Builder
{
	FileInfo File;
	String   Buffer;

	static Builder open( char const* path );

	void pad_lines( s32 num );

	void print( Code );
	void print_fmt( char const* fmt, ... );

	void write();
};

