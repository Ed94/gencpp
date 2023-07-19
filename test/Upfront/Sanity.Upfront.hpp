#ifdef GEN_TIME
#include "gen.hpp"

using namespace gen;

u32 gen_sanity_upfront()
{
	Builder
	gen_sanity_file;
	gen_sanity_file.open("./sanity.Upfront.gen.hpp");

	// Comment
	{
		CodeComment comment_test = def_comment( txt_StrC("Sanity check: def_comment test") );

		gen_sanity_file.print(comment_test);
	}

	gen_sanity_file.print_fmt("\n");
	gen_sanity_file.print( def_comment( txt_StrC(
		"The following will show a series of base cases for the gen api.\n"
	)));

	// Class
	{
		CodeClass fwd = def_class( name(TestEmptyClass) );
		CodeClass empty_body;
		{
			CodeComment cmt  = def_comment( txt_StrC("Empty class body") );
			CodeBody    body = def_class_body( args( cmt ) );

			empty_body = def_class( name(TestEmptyClass), body );
		}

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(empty_body);
	}

	gen_sanity_file.print_fmt("\n");

	// Typedef
	{
		CodeType    t_unsigned_char = def_type( name(unsigned char) );
		CodeTypedef u8_typedef      = def_typedef( name(u8), t_unsigned_char );

		gen_sanity_file.print(u8_typedef);
	}

	gen_sanity_file.print_fmt("\n");

	// Enum
	{
		CodeEnum fwd = def_enum( name(ETestEnum), NoCode, t_u8 );
		CodeEnum def;
		{
			Code body = untyped_str( code(
				A,
				B,
				C
			));

			def = def_enum( name(ETestEnum), body, t_u8 );
		}

		CodeEnum fwd_enum_class = def_enum( name(ETestEnumClass), NoCode, t_u8, EnumClass );

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
		gen_sanity_file.print(fwd_enum_class);
	}

	gen_sanity_file.print_fmt("\n");

	// External Linkage
	{
		CodeBody body = def_extern_link_body( 1
			, def_comment( txt_StrC("Empty extern body") )
		);

		CodeExtern c_extern = def_extern_link( name(C), body );

		gen_sanity_file.print(c_extern);
	}

	gen_sanity_file.print_fmt("\n");

	// Friend
	{
		CodeClass fwd  = def_class( name(TestFriendFwd));
		CodeBody  body = def_class_body( args( def_friend( fwd ) ) );

		gen_sanity_file.print( def_class( name(TestFriend), body ) );
	}

	gen_sanity_file.print_fmt("\n");

	// Function
	{
		CodeFn fwd = def_function( name(test_function) );
		CodeFn def;
		{
			CodeBody body = def_function_body( 1
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
		CodeInclude include = def_include( txt_StrC("../DummyInclude.hpp") );

		gen_sanity_file.print(include);
	}

	gen_sanity_file.print_fmt("\n");

	// Module
	if (0)
	{
		CodeModule module_export = def_module( name(TestModule), ModuleFlag::Export );
		CodeModule module_import = def_module( name(TestModule), ModuleFlag::Import );
		CodeModule module_both   = def_module( name(TestModule), ModuleFlag::Export | ModuleFlag::Import );

		gen_sanity_file.print(module_global_fragment);
		gen_sanity_file.print(module_private_fragment);
		gen_sanity_file.print(module_export);
		gen_sanity_file.print(module_import);
		gen_sanity_file.print(module_both);
	}

	gen_sanity_file.print_fmt("\n");

	// Namespace
	{
		CodeNamespace namespace_def;
		{
			CodeBody body = def_namespace_body( 1
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

		CodeEnum bitflagtest;
		{
			CodeBody body = def_enum_body( 1, untyped_str( code(
				A = 1 << 0,
				B = 1 << 1,
				C = 1 << 2
			)));
			bitflagtest = def_enum( name(EBitFlagtest), body, t_u8,  EnumClass );
		}
		CodeType t_bitflag = def_type( name(EBitFlagtest) );

		CodeOperator op_fwd, op_or;
		{
			CodeParam params = def_params( args(
				def_param( t_bitflag, name(a) ),
				def_param( t_bitflag, name(b) )
			));

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
		CodeType t_u8_ptr = def_type( name(u8), __, spec_ptr );

		CodeOpCast op_ptr = def_operator_cast( t_u8_ptr, __ );

		CodeClass op_class = def_class( name(TestOperatorCast), def_class_body( args( op_ptr) ) );

		gen_sanity_file.print(op_class);
	}

	gen_sanity_file.print_fmt("\n");

	// Parameters
	{
		CodeFn fwd;
		{
			CodeParam params = def_param( t_u8, name(a) );

			fwd = def_function( name(test_function_wparam), params );
		}

		CodeFn def, def2;
		{
			CodeBody body = def_function_body( 1
				, def_comment( txt_StrC("Empty function body") )
			);

			CodeParam params = def_params( args(
				  def_param( t_u8, name(a) )
				, def_param( t_u8, name(b) )
			));

			def = def_function( name(test_function_wparams), params, __, body );

			CodeParam param_a = def_param( t_u8, name(a));
			CodeParam param_b = def_param( t_u8, name(b));
			CodeParam params_arr[2] = { param_a, param_b };

			CodeParam params2 = def_params( 2, params_arr );

			def2 = def_function( name(test_function_wparams2), params2, __, body );
		}

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(def);
		gen_sanity_file.print(def2);
	}

	gen_sanity_file.print_fmt("\n");

	// Specifiers
	{
		CodeFn fwd_fn = def_function( name(test_function_specifiers), __, __, __, spec_inline );

		// TODO : Need an op overload here

		CodeType    u8_ptr         = def_type( name(u8), __, spec_ptr );
		CodeTypedef typedef_u8_ptr = def_typedef( name(ConstExprTest), u8_ptr );

		gen_sanity_file.print(fwd_fn);
		gen_sanity_file.print(typedef_u8_ptr);
	}

	gen_sanity_file.print_fmt("\n");

	// Struct
	{
		CodeClass fwd = def_class( name(TestEmptyStruct) );
		CodeClass empty_body;
		{
			CodeComment cmt  = def_comment( txt_StrC("Empty struct body") );
			CodeBody    body = def_class_body( args( cmt ) );

			empty_body = def_class( name(TestEmptyStruct), body );
		}

		gen_sanity_file.print(fwd);
		gen_sanity_file.print(empty_body);
	}

	gen_sanity_file.print_fmt("\n");

	// Union
	{
		CodeBody body = def_union_body( 1
			, def_comment( txt_StrC("Empty union body") )
		);

		CodeUnion def = def_union( name(TestEmptyUnion), body );

		gen_sanity_file.print(def);
	}

	gen_sanity_file.print_fmt("\n");

	// Using
	{
		CodeUsing          reg    = def_using( name(TestUsing), t_u8 );
		CodeUsingNamespace nspace = def_using_namespace( name(TestNamespace) );

		gen_sanity_file.print(reg);
		gen_sanity_file.print(nspace);
	}

	gen_sanity_file.print_fmt("\n");

	// Variable
	{
		CodeVar bss  = def_variable( t_u8, name(test_variable) );
		CodeVar data = def_variable( t_u8, name(test_variable2), untyped_str( code( 0x12 )) );

		gen_sanity_file.print(bss);
		gen_sanity_file.print(data);
	}

	gen_sanity_file.print_fmt("\n");

	// Template
	{
		CodeType t_Type = def_type( name(Type) );

		CodeTemplate tmpl = def_template(  def_param( t_class, name(Type) )
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
