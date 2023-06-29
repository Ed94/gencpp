

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

	// Typedef
	{
		Code t_unsigned_char = def_type( name(unsigned char) );

		Code u8_typedef = def_typedef( name(u8), t_unsigned_char );

		gen_sanity_file.print(u8_typedef);
	}

	gen_sanity_file.print_fmt("\n");

	// Enum
	{
		Code fwd = def_enum( StrC::from("Test_Enum"), NoCode, t_u8 );
		Code def;
		{

			Code body = untyped_str( StrC::from(
			#define enum_entry( id ) "\t" #id ",\n"
				enum_entry( A )
				enum_entry( B )
				enum_entry( C )
			#undef enum_entry
			));

			def = def_enum( StrC::from("Test_Enum"), body, t_u8 );
		}

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
	}

	gen_sanity_file.write();
	return 0;
}
#endif
