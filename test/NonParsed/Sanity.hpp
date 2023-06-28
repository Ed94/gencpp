

#ifdef gen_time
#include "gen.hpp"

using namespace gen;

u32 gen_sanity()
{
	Builder
	gen_sanity_file;
	gen_sanity_file.open("./sanity.gen.hpp");

	// Comment
	{
		Code comment_test = def_comment( StrC::from("Sanity check: def_omment test") );

		gen_sanity_file.print(comment_test);
		gen_sanity_file.print_fmt("\n");
	}

	// Class
	{
		Code fwd = def_class( StrC::from("Test_EmptyClass") );
		Code empty_body;
		{
			Code cmt  = def_comment( StrC::from("Empty class") );
			Code body = def_class_body( 1, cmt );

			empty_body = def_class( StrC::from("Test_EmptyClass"), body );
		}

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(empty_body);
	}

	gen_sanity_file.print_fmt("\n");

	//
	{

	}

	gen_sanity_file.write();
	return 0;
}
#endif
