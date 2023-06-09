#ifdef gen_time
#include "gen.hpp"

using namespace gen;

u32 gen_sanity()
{
	Builder
	gen_sanity_file;
	gen_sanity_file.open("./sanity.Upfront.gen.hpp");

	// Comment
	{
		Code comment_test = def_comment( txt_StrC("Sanity check: def_comment test") );

		gen_sanity_file.print(comment_test);
	}

	gen_sanity_file.print_fmt("\n");
	gen_sanity_file.print( def_comment( txt_StrC(
		"The following will show a series of base cases for the gen api.\n"
	)));

	// Class
	{
		Code fwd = def_class( name(TestEmptyClass) );
		Code empty_body;
		{
			Code cmt  = def_comment( txt_StrC("Empty class body") );
			Code body = def_class_body( 1, cmt );

			empty_body = def_class( name(TestEmptyClass), body );
		}

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(empty_body);
	}

	gen_sanity_file.print_fmt("\n");

	// Typedef
	{
		Code t_unsigned_char = def_type( name(unsigned char) );
		Code u8_typedef      = def_typedef( name(u8), t_unsigned_char );

		gen_sanity_file.print(u8_typedef);
	}

	gen_sanity_file.print_fmt("\n");

	// Enum
	{
		Code fwd = def_enum( name(ETestEnum), NoCode, t_u8 );
		Code def;
		{
			Code body = untyped_str( code(
				A,
				B,
				C
			));

			def = def_enum( name(ETestEnum), body, t_u8 );
		}

		Code fwd_enum_class = def_enum( name(ETestEnumClass), NoCode, t_u8, EnumClass );

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
		gen_sanity_file.print(fwd_enum_class);
	}

	gen_sanity_file.print_fmt("\n");

	// External Linkage
	{
		Code body = def_extern_link_body( 1
			, def_comment( txt_StrC("Empty extern body") )
		);

		Code c_extern = def_extern_link( name(C), body );

		gen_sanity_file.print(c_extern);
	}

	gen_sanity_file.print_fmt("\n");

	// Friend
	{
		Code fwd  = def_class( name(TestFriendFwd));
		Code body = def_class_body( 1
			, def_friend( fwd )
		);

		gen_sanity_file.print( def_class( name(TestFriend), body ) );
	}

	gen_sanity_file.print_fmt("\n");

	// Function
	{
		Code fwd = def_function( name(test_function) );
		Code def;
		{
			Code body = def_function_body( 1
				, def_comment( txt_StrC("Empty function body") )
			);

			def = def_function( name(test_function), __, __, body );
		}

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
	}

	gen_sanity_file.print_fmt("\n");

	// Include
	{
		Code include = def_include( txt_StrC("../DummyInclude.hpp") );

		gen_sanity_file.print(include);
	}

	gen_sanity_file.print_fmt("\n");

	// Module
	if (0)
	{
		Code module_export = def_module( name(TestModule), ModuleFlag::Export );
		Code module_import = def_module( name(TestModule), ModuleFlag::Import );
		Code module_both   = def_module( name(TestModule), ModuleFlag::Export | ModuleFlag::Import );

		gen_sanity_file.print(module_global_fragment);
		gen_sanity_file.print(module_private_fragment);
		gen_sanity_file.print(module_export);
		gen_sanity_file.print(module_import);
		gen_sanity_file.print(module_both);
	}

	gen_sanity_file.print_fmt("\n");

	// Namespace
	{
		Code namespace_def;
		{
			Code body = def_namespace_body( 1
				, def_comment( txt_StrC("Empty namespace body") )
			);

			namespace_def = def_namespace( name(TestNamespace), body );
		}

		gen_sanity_file.print(namespace_def);
	}

	gen_sanity_file.print_fmt("\n");

	// Operator
	{
		// Going to make a bit flag set of overloads for this.

		Code bitflagtest;
		{
			Code body = def_enum_body( 1, untyped_str( code(
				A = 1 << 0,
				B = 1 << 1,
				C = 1 << 2
			)));
			bitflagtest = def_enum( name(EBitFlagtest), body, t_u8,  EnumClass );
		}
		Code t_bitflag = def_type( name(EBitFlagtest) );

		Code op_fwd, op_or;
		{
			Code params = def_params( 2,
				def_param( t_bitflag, name(a) ),
				def_param( t_bitflag, name(b) )
			);

			op_fwd = def_operator( EOperator::BOr, params, t_bitflag );
			op_or  = def_operator( EOperator::BOr, params, t_bitflag, untyped_str( code(
				return EBitFlagtest( (u8)a | (u8)b );
			)));
		}

		gen_sanity_file.print(bitflagtest);
		gen_sanity_file.print(op_fwd);
		gen_sanity_file.print(op_or);
	}

	gen_sanity_file.print_fmt("\n");

	// Operator cast
	{
		Code t_u8_ptr = def_type( name(u8), __, spec_ptr );

		Code op_ptr = def_operator_cast( t_u8_ptr, __ );

		Code op_class = def_class( name(TestOperatorCast), def_class_body( 1, op_ptr ) );

		gen_sanity_file.print(op_class);
	}

	gen_sanity_file.print_fmt("\n");

	// Parameters
	{
		Code fwd;
		{
			Code params = def_param( t_u8, name(a) );

			fwd = def_function( name(test_function_wparam), params );
		}

		Code def, def2;
		{
			Code body = def_function_body( 1
				, def_comment( txt_StrC("Empty function body") )
			);

			Code params = def_params( args(
				  def_param( t_u8, name(a) )
				, def_param( t_u8, name(b) )
			));

			def = def_function( name(test_function_wparams), params, __, body );

			Code param_a = def_param( t_u8, name(a));
			Code param_b = def_param( t_u8, name(b));
			Code params_arr[2] = { param_a, param_b };

			Code params2 = def_params( 2, params_arr );

			def2 = def_function( name(test_function_wparams2), params2, __, body );
		}

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
		gen_sanity_file.print(def2);
	}

	gen_sanity_file.print_fmt("\n");

	// Specifiers
	{
		Code fwd_fn         = def_function( name(test_function_specifiers), __, __, __, spec_inline );

		// TODO: Need an op overload here

		Code u8_ptr         = def_type( name(u8), __, spec_ptr );
		Code typedef_u8_ptr = def_typedef( name(ConstExprTest), u8_ptr );

		gen_sanity_file.print(fwd_fn);
		gen_sanity_file.print(typedef_u8_ptr);
	}

	gen_sanity_file.print_fmt("\n");

	// Struct
	{
		Code fwd = def_class( name(TestEmptyStruct) );
		Code empty_body;
		{
			Code cmt  = def_comment( txt_StrC("Empty struct body") );
			Code body = def_class_body( 1, cmt );

			empty_body = def_class( name(TestEmptyStruct), body );
		}

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(empty_body);
	}

	gen_sanity_file.print_fmt("\n");

	// Union
	{
		Code body = def_union_body( 1
			, def_comment( txt_StrC("Empty union body") )
		);

		Code def = def_union( name(TestEmptyUnion), body );

		gen_sanity_file.print(def);
	}

	gen_sanity_file.print_fmt("\n");

	// Using
	{
		Code reg    = def_using( name(TestUsing), t_u8 );
		Code nspace = def_using_namespace( name(TestNamespace) );

		gen_sanity_file.print(reg);
		gen_sanity_file.print(nspace);
	}

	gen_sanity_file.print_fmt("\n");

	// Variable
	{
		Code bss  = def_variable( t_u8, name(test_variable) );
		Code data = def_variable( t_u8, name(test_variable2), untyped_str( code( 0x12 )) );

		gen_sanity_file.print(bss);
		gen_sanity_file.print(data);
	}

	gen_sanity_file.print_fmt("\n");

	// Template
	{
		Code t_Type = def_type( name(Type) );

		Code tmpl = def_template(  def_param( t_class, name(Type) )
			, def_function( name(test_template), def_param( t_Type, name(a) ), __
				, def_function_body(1, def_comment( txt_StrC("Empty template function body")))
			)
		);

		gen_sanity_file.print(tmpl);
	}

	gen_sanity_file.print_fmt("\n");

	gen_sanity_file.print( def_comment( txt_StrC(
		"End of base case tests.\n"
	)));

	gen_sanity_file.write();
	return 0;
}
#endif
