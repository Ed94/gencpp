#pragma once
#ifdef gen_time
#include "gen.hpp"

using namespace gen;

u32 gen_sanity()
{
	Builder
	gen_sanity_file;
	gen_sanity_file.open("./sanity.Parsed.gen.hpp");

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

	gen_sanity_file.print_fmt("\n");

	// Class
	{
		Code fwd = parse_class( code(
			class TestEmptyClass;
		));

		Code empty_body = parse_class( code(
			class TestEmptyClass
			{};
		));

		empty_body.body()->add_entry( def_comment( StrC::from("Empty class body") ) );

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(empty_body);
	}

	gen_sanity_file.print_fmt("\n");

	// Enum
	{
		Code fwd = parse_enum( code(
			enum ETestEnum : u8;
		));

		Code def = parse_enum( code(
			enum ETestEnum : u8
			{
				A,
				B,
				C
			};
		));

		Code fwd_enum_class = parse_enum( code(
			enum class ETestEnumClass : u8;
		));

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
		gen_sanity_file.print(fwd_enum_class);
	}

	gen_sanity_file.print_fmt("\n");

	// External Linkage
	{
		Code empty_comment = def_comment( StrC::from("Empty external linkage") );

		Code c_extern = parse_extern_link( code(
			extern "C"
			{
			};
		));

		c_extern.body()->add_entry( empty_comment );

		gen_sanity_file.print(c_extern);
	}

	gen_sanity_file.print_fmt("\n");

	// Friend
	if (0)
	{
		Code fwd = parse_class( code(
			class TestFriendClass;
		));

		Code def = parse_class( code(
			class TestFriend
			{
				friend class TestFriendClass;
			};
		));

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
	}

	gen_sanity_file.print_fmt("\n");

	// Function
	if (0)
	{
		Code fwd = parse_function( code(
			void test_function();
		));

		Code def = parse_function( code(
			void test_function()
			{
			}
		));

		def.body()->add_entry( def_comment( StrC::from("Empty function body") ) );

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
	}

	gen_sanity_file.print_fmt("\n");

	// Namespace
	if (0)
	{
		Code def = parse_namespace( code(
			namespace TestNamespace
			{
			}
		));

		def.body()->add_entry( def_comment( StrC::from("Empty namespace body") ) );

		gen_sanity_file.print(def);
	}

	gen_sanity_file.print_fmt("\n");

	// Operator

	if (0)
	{
		Code bitflagtest = parse_class( code(
			enum class EBitFlagTest : u8
			{
				A = 1 << 0,
				B = 1 << 1,
				C = 1 << 2
			};
		));

		Code op_fwd = parse_operator( code(
			EBitFlagTest operator | ( EBitFlagTest a, EBitFlagTest b );
		));

		Code op_or = parse_operator( code(
			EBitFlagTest operator | ( EBitFlagTest a, EBitFlagTest b )
			{
				return EBitFlagTest( (u8)a | (u8)b );
			}
		));

		gen_sanity_file.print(bitflagtest);
		gen_sanity_file.print(op_fwd);
		gen_sanity_file.print(op_or);
	}

	gen_sanity_file.print_fmt("\n");

	// Parameters
	if (0)
	{
		Code fwd = parse_function( code(
			void test_function( int a );
		));

		Code def = parse_function( code(
			void test_function( int a, int b )
			{
			}
		));

		def.body()->add_entry( def_comment( StrC::from("Empty function body") ) );

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
	}

	gen_sanity_file.print_fmt("\n");

	// Specifiers
	if (0)
	{
		Code fwd_fn = parse_function( code(
			inline
			void test_function_specifiers();
		));

		Code typedef_u8_ptr = parse_typedef( code(
			typedef u8* u8_ptr;
		));

		gen_sanity_file.print(fwd_fn);
		gen_sanity_file.print(typedef_u8_ptr);
	}

	gen_sanity_file.print_fmt("\n");

	// Struct
	if (0)
	{
		Code fwd = parse_struct( code(
			struct TestEmptyStruct;
		));

		Code empty_body = parse_struct( code(
			struct TestEmptyStruct
			{};
		));

		empty_body.body()->add_entry( def_comment( StrC::from("Empty struct body") ) );

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(empty_body);
	}

	gen_sanity_file.print_fmt("\n");

	// Union
	if (0)
	{
		Code empty = parse_union( code(
			union TestEmptyUnion
			{
			};
		));

		empty.body()->add_entry( def_comment( StrC::from("Empty union body") ) );

		Code def = parse_union( code(
			union TestUnion
			{
				u8 a;
				u16 b;
				u32 c;
			};
		));

		gen_sanity_file.print(empty);
		gen_sanity_file.print(def);
	}

	gen_sanity_file.print_fmt("\n");

	// Using
	if (0)
	{
		Code reg = parse_using( code(
			using TestUsing = u8;
		));

		Code nspace = parse_using( code(
			namespace TestNamespace
			{
			};

			using namespace TestUsing;
		));

		gen_sanity_file.print(reg);
		gen_sanity_file.print(nspace);
	}

	gen_sanity_file.print_fmt("\n");

	// Variable
	if (0)
	{
		Code bss = parse_variable( code(
			u8 test_variable;
		));

		Code data = parse_variable( code(
			u8 test_variable = 0x12;
		));
	}

	gen_sanity_file.print_fmt("\n");

	gen_sanity_file.print( def_comment( StrC::from(
		"End of base case tests\n"
	)));

	gen_sanity_file.write();
	return 0;
}
#endif
