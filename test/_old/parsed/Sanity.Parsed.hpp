#pragma once
#ifdef GEN_TIME
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_BENCHMARK
#include "gen.hpp"

using namespace gen;

u32 gen_sanity()
{
	Builder
	gen_sanity_file;
	gen_sanity_file.open("./sanity.Parsed.gen.hpp");

	gen_sanity_file.print( def_comment( txt(
		"The following will show a series of base cases for the gen parsed api.\n"
	)));

	// Typedef
	{
		CodeTypedef u8_typedef = parse_typedef( code(
			typedef unsigned char u8;
		));

		gen_sanity_file.print(u8_typedef);
	}

	gen_sanity_file.print_fmt("\n");

	// Class
	{
		CodeClass fwd = parse_class( code(
			class TestEmptyClass;
		));

		CodeClass empty_body = parse_class( code(
			class TestEmptyClass
			{};
		));

		empty_body->Body.append( def_comment( txt("Empty class body") ) );

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(empty_body);
	}

	gen_sanity_file.print_fmt("\n");

	// Enum
	{
		CodeEnum fwd = parse_enum( code(
			enum ETestEnum : u8;
		));

		CodeEnum def = parse_enum( code(
			enum ETestEnum : u8
			{
				A,
				B,
				C
			};
		));

		CodeEnum fwd_enum_class = parse_enum( code(
			enum class ETestEnumClass : u8;
		));

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
		gen_sanity_file.print(fwd_enum_class);
	}

	gen_sanity_file.print_fmt("\n");

	// External Linkage
	{
		CodeComment empty_comment = def_comment( txt("Empty external linkage") );

		CodeExtern c_extern = parse_extern_link( code(
			extern "C"
			{
			};
		));

		c_extern->Body.append( empty_comment );

		gen_sanity_file.print(c_extern);
	}

	gen_sanity_file.print_fmt("\n");

	// Friend
	{
		CodeClass fwd = parse_class( code(
			class TestFriendClass;
		));

		CodeClass def = parse_class( code(
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
	{
		CodeFn fwd = parse_function( code(
			void test_function();
		));

		CodeFn def = parse_function( code(
			void test_function()
			{
			}
		));

		def->Body.append( def_comment( txt("Empty function body") ) );

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
	}

	gen_sanity_file.print_fmt("\n");

	// Namespace
	{
		CodeNS def = parse_namespace( code(
			namespace TestNamespace
			{
			}
		));

		def->Body.append( def_comment( txt("Empty namespace body") ) );

		gen_sanity_file.print(def);
	}

	gen_sanity_file.print_fmt("\n");

	// Operator
	{
		CodeEnum bitflagtest = parse_enum( code(
			enum class EBitFlagTest : u8
			{
				A = 1 << 0,
				B = 1 << 1,
				C = 1 << 2
			};
		));

		CodeOperator op_fwd = parse_operator( code(
			EBitFlagTest operator | ( EBitFlagTest a, EBitFlagTest b );
		));

		CodeOperator op_or = parse_operator( code(
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

	// Operator cast
	{
		CodeOpCast op_ptr = parse_operator_cast( code(
			operator u8* ();
		));

		CodeClass class_def = parse_class( code(
			class TestClass
			{
			};
		));

		class_def->Body.append( op_ptr );

		gen_sanity_file.print(class_def);
	}

	gen_sanity_file.print_fmt("\n");

	// Parameters
	{
		CodeFn fwd = parse_function( code(
			void test_function_param( int a );
		));

		CodeFn def = parse_function( code(
			void test_function_param2( int a, int b )
			{
			}
		));

		def->Body.append( def_comment( txt("Empty function body") ) );

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
	}

	gen_sanity_file.print_fmt("\n");

	// Specifiers
	{
		CodeFn fwd_fn = parse_function( code(
			inline
			void test_function_specifiers();
		));

		CodeTypedef typedef_u8_ptr = parse_typedef( code(
			typedef u8* u8_ptr;
		));

		gen_sanity_file.print(fwd_fn);
		gen_sanity_file.print(typedef_u8_ptr);
	}

	gen_sanity_file.print_fmt("\n");

	// Struct
	{
		CodeStruct fwd = parse_struct( code(
			struct TestEmptyStruct;
		));

		CodeStruct empty_body = parse_struct( code(
			struct TestEmptyStruct
			{};
		));

		empty_body->Body.append( def_comment( txt("Empty struct body") ) );

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(empty_body);
	}

	gen_sanity_file.print_fmt("\n");

	// Union
	{
		CodeUnion empty = parse_union( code(
			union TestEmptyUnion
			{
			};
		));

		empty->Body.append( def_comment( txt("Empty union body") ) );

		gen_sanity_file.print( parse_typedef( code( typedef unsigned short u16; )) );
		gen_sanity_file.print( parse_typedef( code( typedef unsigned long  u32; )) );

		CodeUnion def = parse_union( code(
			union TestUnion
			{
				u8  a;
				u16 b;
				u32 c;
			};
		));

		gen_sanity_file.print(empty);
		gen_sanity_file.print(def);
	}

	gen_sanity_file.print_fmt("\n");

	// Using
	{
		CodeUsing reg = (CodeUsing) parse_using( code(
			using TestUsing = u8;
		));

		CodeNS nspace = parse_namespace( code(
			namespace TestNamespace
			{
			};

		));

		CodeUsing npspace_using = parse_using( code(
			using namespace TestNamespace;
		));

		gen_sanity_file.print(reg);
		gen_sanity_file.print(nspace);
		gen_sanity_file.print(npspace_using);
	}

	gen_sanity_file.print_fmt("\n");

	// Variable
	{
		CodeVar bss = parse_variable( code(
			u8 test_variable;
		));

		CodeVar data = parse_variable( code(
			u8 test_variable = 0x12;
		));

		gen_sanity_file.print(bss);
		gen_sanity_file.print(data);
	}

	gen_sanity_file.print_fmt("\n");

	// template
	{
	#pragma push_macro("template")
	#undef template
		CodeTemplate tmpl = parse_template( code(
			template< typename Type >
			void test_template( Type a )
			{
			}
		));
	#pragma pop_macro("template")

		gen_sanity_file.print(tmpl);
	}

	gen_sanity_file.print_fmt("\n");

	gen_sanity_file.print( def_comment( txt(
		"End of base case tests\n"
	)));

	gen_sanity_file.write();
	return 0;
}
#endif
