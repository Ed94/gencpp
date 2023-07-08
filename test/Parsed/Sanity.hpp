#pragma once
#ifdef gen_time
#include "gen.hpp"

using namespace gen;

void gen_sanity()
{
	Builder
	gen_sanity_file;
	gen_sanity_file.open("./sanity.gen.hpp");

	gen_sanity_file.print( def_comment( StrC::from(
		"The following will show a series of base cases for the gen parsed api.\n"
	)));

	// Typedef
	{
		Code u8_typedef = parse_typedef( code(
			typedef unsigned char u8;
		));

		gen_sanity_file.print(u8_typedef);
	}

	gen_sanity_file.write();
}
#endif

