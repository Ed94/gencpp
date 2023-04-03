/*
	This is based of the array container implementation in the zpl.h library.
*/

#pragma once

#include "Bloat.hpp"
#include "gen.hpp"

#ifdef gen_time
	using namespace gen;

	Code gen__array_base()
	{
		// Make these global consts to be accessed anywhere...
			Code t_sw        = def_type( txt(sw) );
			Code t_uw        = def_type( txt(uw) );
			Code t_allocator = def_type( txt(allocator) );

	#ifndef GEN_DEFINE_DSL
		Code header;
		{
			Code num           = def_variable( "Num",       t_uw );
			Code capacity      = def_variable( "Capacity",  t_uw );
			Code allocator_var = def_variable( "Allocator", t_allocator );
			Code header_body   = def_struct_body( 3, num, capacity, allocator_var );

			header = def_struct( "ArrayHeader", header_body );
		}

		Code grow_formula;
		{
			Code spec   = def_specifiers(1, Specifier::Inline);
			Code params = def_parameters(1, t_uw, "value" );
			Code body   = untyped_str( "return 2 * value * 8;" );

			grow_formula = def_function( "grow_formula", spec, params, t_sw, body );
		}

		Code body       = def_struct_body(2, header, grow_formula);
		Code ArrayBase = def_struct( "ArrayBase", body );
		
	#else
		def( ArrayBase )
			def ( Header ) 
			{
				var( Num,       t_uw, __, __ );
				var( Capacity,  t_uw, __, __);
				var( Allocator, t_allocator, __, __);

				Code body = struct_body( Num, Capacity, Allocator );

				struct( Header, __, __, body );
			}

			def( grow_formula )
			{
				function( grow_formula, spec_inline, t_uw, params( t_uw, "value" ), untyped_str("return 2 * value * 8") );
			}

			Code body = struct_body( Header, grow_formula );
		struct( ArrayBase, __, __, body );
	#endif

		return ArrayBase;
	}

	#define gen_array( Type_ ) gen__array( #Type_, sizeof(Type_), a_base )
	Code gen__array( char const* type_str, s32 type_size, Code parent )
	{
	#ifndef GEN_DEFINE_DSL
		// Make these global consts to be accessed anywhere...
			Code t_uw        = def_type( txt(uw) );
			Code t_sw        = def_type( txt(sw) );
			Code t_bool      = def_type( txt(bool) );
			Code t_allocator = def_type( txt(allocator) );
			Code t_void      = def_type( txt(void) );

			Code v_nullptr = untyped_str( "nullptr" );

			Code spec_ct     = def_specifiers(1, Specifier::Constexpr );
			Code spec_inline = def_specifiers(1, Specifier::Inline );

		Code type     = def_type( type_str );
		Code ptr_type = def_type( bprintf( "%s*", type_str ) );
		Code ref_type = def_type( bprintf( "%s&", type_str ) );

		// From ArrayBase
			Code t_header   = def_type( "Header" );
			Code ptr_header = def_type( "Header*" );
			Code ref_header = def_type( "Header&" );

		Code array_def;
		{
			Code using_type = def_using( "Type", type );
			Code data       = def_variable( "Data", ptr_type );

			Code init;
			{
				Code params = def_parameters( 1, t_allocator, "mem_handler" );
				Code body   = untyped_str( "return init_reserve( mem_handler, grow_formula(0) );" );

				init = def_function( "init", UnusedCode, params, t_bool, body );
			}

			Code init_reserve;
			{
				Code params = def_parameters( 2, t_allocator, "mem_handler", t_sw, "capacity" );
				Code body;
				{
					Code header_value = untyped_str( 
						"rcast( Header*, alloc( mem_handler, sizeof( Header ) + sizeof(Type) + capacity ))"
					);
					Code header = def_variable( "header", ptr_header, header_value );

					Code null_check = untyped_str(
							"if (header == nullptr)"
						"\n"	"return false;"
					);

					Code header_init = untyped_str(
							"header->Num       = 0;"
						"\n""header->Capacity  = capacity;"
						"\n""header->Allocator = mem_handler;"
					);

					Code assign_data = untyped_str(
						"Data = rcast( %s, header + 1 );", ptr_type
					);

					Code ret_true = untyped_str( "\t""return true" );

					body = def_function_body( 5
					,	header
					,	null_check
					,	header_init
					,	assign_data
					,	ret_true
					);
				}

				init_reserve = def_function( "init_reserve", UnusedCode, params, t_bool, body );
			}

			Code free;
			{
				Code body = untyped_str( 
						"Header& header = get_header();"
					"\n""::free( header.Allocator, & get_header() );"
				);

				free = def_function( "free", UnusedCode, UnusedCode, t_void, body );
			}

			Code append;
			{
				Code params = def_parameters( 1, type, "value" );
				Code body;
				{
					Code header = def_variable( "header", ref_header, untyped_str( "get_header()") );

					Code check_cap = untyped_str(
							"if ( header.Capacity < header.Num + 1 )"
						"\n"	"if ( ! grow(0) )"
						"\n"		"return false;"
					);

					Code assign = untyped_str(
							 "Data[ header.Num ] = value;"
						"\n" "header.Num++;"
						"\n"
						"\n" "return true;"
					);

					body = def_function_body( 3, header, check_cap, assign );
				}

				append = def_function( "append", UnusedCode, params, t_void, body );
			}

			Code back;
			{
				Code body = untyped_str(
					     "Header& header = get_header();"
					"\n" "return data[ header.Num - 1 ];"
				);

				back = def_function( "back", UnusedCode, UnusedCode, type, body );
			}

			Code clear;
			{
				Code body = untyped_str( "get_header().Num = 0;" );

				clear = def_function( "clear", UnusedCode, UnusedCode, t_void, body );
			}

			Code fill;
			{
				Code params = def_parameters( 3, t_uw, "begin", t_uw, "end", type, "value" );
				Code body;
				{
					Code header = def_variable( "header", ref_header, untyped_str( "get_header()") );

					Code check = untyped_str(
							"if ( begin < 0 || end >= header.Num )"
						"\n"	"fatal( \"Range out of bounds\" );"
					);

					Code iter = untyped_str(
							"for ( sw index = begin; index < end; index++ )"
						"\n"	"Data[index] = vallue;"
					);

					body = def_function_body( 3, header, check, iter );
				}

				fill = def_function( "fill", UnusedCode, params, t_void, body );
			}

			Code get_header;
			{
				Code body = untyped_str( "return pcast( Header, Data - 1 );" );

				get_header = def_function( "get_header", spec_inline, UnusedCode, ref_header, body );
			}

			Code grow;
			{
				Code param = def_parameters( 1, t_uw, "min_capacity" );
				Code body;
				{
					Code header       = def_variable( "header",      ref_header, untyped_str("get_header()") );
					Code new_capacity = def_variable( "new_capacity", t_uw,      untyped_str("grow_formula( header.Capacity )") );

					Code check_n_set = untyped_str(
							"if ( new_capacity < min_capacity )"
						"\n"	"new_capacity = min_capacity;"
					);

					Code ret = untyped_str( "return set_capacity( new_capacity );" );

					body = def_function_body( 4, header, new_capacity, check_n_set, ret );
				}

				grow = def_function( "grow", UnusedCode, param, t_bool, body );
			}

			Code pop;
			{
				Code body;
				{
					Code header = def_variable( "header", ref_header, untyped_str("get_header()") );

					Code assertion = untyped_str( "assert( header.Num > 0 );" );
					Code decrement = untyped_str( "header.Num--; " );

					body = def_function_body( 3, header, assertion, decrement );
				}

				pop = def_function( "pop", UnusedCode, UnusedCode, t_void, body );
			}

			Code reserve;
			{
				Code params = def_parameters( 1, t_uw, "new_capacity" );
				Code body;
				{
					Code header = def_variable( "header", ref_header, untyped_str("get_header()") );

					Code check_n_set = untyped_str(
							"if ( header.Capacity < new_capacity )"
						"\n"	"return set_capacity( new_capacity );"
					);

					Code ret = untyped_str( "\t" "return true" );

					body = def_function_body( 3, header, check_n_set, ret );
				}

				reserve = def_function( "reserve", UnusedCode, params, t_bool, body );
			}

			Code resize;
			{
				Code param = def_parameters( 1, t_uw, "new_num" );

				Code body;
				{
					Code header = def_variable( "header", ref_header, untyped_str("get_header()") );

					Code check_n_grow = untyped_str(
							"if ( header.Capacity < new_num )"
						"\n"	"if ( ! grow( new_num) )"
						"\n"		"return false;"
					);

					Code set_n_ret = untyped_str(
							"header.Count = new_num;"
						"\n""return true;"
					);

					body = def_function_body( 3, header, check_n_grow, set_n_ret );
				}

				resize = def_function( "resize", UnusedCode, param, t_bool, body );
			}

			Code set_capacity;
			{
				Code param = def_parameters( 1, t_uw, "capacity" );

				Code body;
				{
					Code header = def_variable( "header", ref_header, untyped_str("get_header()") );

					Code checks = untyped_str(
						"if ( capacity == header.Capacity )"
					"\n"	"return true;"
					
						"if ( capacity < header.Num )"
					"\n"	"header.Num = capacity;"
					);

					Code size       = def_variable( "size", t_uw, untyped_str("sizeof(Header) + sizeof(Type) * capacity"));
					Code new_header = def_variable( "new_header", ptr_header, untyped_str("rcast( Header*, alloc( header.Allocator, size ));"));

					Code check_n_move = untyped_str(
							"if ( new_header == nullptr )"
					"\n"		"return false;"
					"\n"
					"\n"	"memmove( new_header, & header, sizeof(Header) + sizeof(Type) * header.Num );"
					);

					Code set_free_ret = untyped_str(
							"new_header->Allocator = header.Allocator;"
					"\n"	"new_header->Num       = header.Num;"
					"\n"	"new_header->Capacity  = header.Capacity;"
					"\n"
					"\n"	"zpl_free( header );"
					"\n"
					"\n"	"*Data = new_header + 1;"
					"\n"
					"\n"	"return true;"
					);				

					body = def_function_body( 6, header, checks, size, new_header, check_n_move, set_free_ret );
				}

				set_capacity = def_function( "set_capacity", UnusedCode, param, t_bool, body );
			}

			string name = bprintf( "Array_%s", type_str );

			Code body = def_struct_body( 15
				, using_type
				, data

				, init
				, init_reserve
				, append
				, back
				, clear
				, fill
				, free
				, get_header
				, grow
				, pop
				, reserve
				, resize
				, set_capacity
			);

			array_def = def_struct( name, body, parent );
		}
	#else
		type( t_uw, uw );
		type( t_sw, sw );
		type( t_bool, bool );
		type( t_allocator, allocator );
		type( t_void, void );
		
		value( v_nullptr, nullptr );

		specifiers( spec_ct, Specifier::Constexpr );
		specifiers( spec_inline, Specifier::Inline );

		type_fmt( type, type_str, __);
		type_fmt( ptr_type, "%s*", type_str );
		type_fmt( ref_type, "%&", type_str );


		// From ArrayBase
			type( t_header, Header );
			type( ptr_header, Header* );
			type( ref_header, Header& );

		def( array_def )
		{
			using( Type, type );
			var( Data, ptr_type, __, __ );

			def( init )
			{
				Code body = function_body( untyped_str("return init_reserve( mem_handler, grow_formula(0) );" ));

				function( init, __, t_bool, params( t_allocator, "mem_handler" ), body );
			}

			def( init_reserve )
			{
				def( body )
				{
					var(header, ptr_header, untyped_str("rcast( Header*, alloc( mem_handler, sizeof(Header) + sizeof(Type) + capacity))" ), __);

					Code null_check = untyped_str(
							"if (header == nullptr)"
						"\n"	"return false;"
					);

					Code header_init = untyped_str(
							"header->Num       = 0;"
						"\n""header->Capacity  = capacity;"
						"\n""header->Allocator = mem_handler;"
					);

					Code assign_data = untyped_str( "Data = rcast( Type*, header + 1 );" );
					Code ret_true    = untyped_str( "return true" );

					Code body = function_body( header, null_check, header_init, assign_data, ret_true );
				}

				function( init_reserve, __, t_bool, params( ref_type, "mem_handler" ) , body);
			}

			def( free )
			{
				Code body = untyped_str( 
						"Header& header = get_header();"
					"\n""free( header.Allocator, & get_header() );"
				);

				function( free, __, t_void, __, body );
			}

			def( append )
			{
				def( body )
				{
					var( header, ref_header, untyped_str("get_header()"), __ );

					Code check_cap = untyped_str(
							"if ( header.Capacity < header.Num + 1 )"
						"\n"	"if ( ! grow(0) )"
						"\n"		"return false;"
					);

					Code assign = untyped_str(
							 "Data[ header.Num ] = value;"
						"\n" "header.Num++;"
						"\n"
						"\n" "return true;"
					);

					body = function_body( header, check_cap, assign );
				}

				function( append, __, t_void, params( type, "value" ), body );
			}

			def( back );
			{
				Code body = untyped_str(
					     "Header& header = get_header();"
					"\n" "return data[ header.Num - 1 ];"
				);

				function( back, __, type, __, body );
			}

			def( clear )
			function( clear, __, t_void, __, untyped_str("get_header().Num = 0;") );

			def( fill );
			{
				def( body )
				{
					var( header, ref_header, untyped_str("get_header()"), __ );

					Code check = untyped_str(
							"if ( begin < 0 || end >= header.Num )"
						"\n"	"fatal( \"Range out of bounds\" );"
					);

					Code iter = untyped_str(
							"for ( sw index = begin; index < end; index++ )"
						"\n"	"Data[index] = vallue;"
					);

					function_body( header, check, iter );
				}

				function( fill, __, t_void, params( t_uw, "begin", t_uw, "end", type, "value" ), body );
			}

			def( get_header )
			function( get_header, spec_inline, ref_header, __, untyped_str("return pcast( Header, Data - 1);") );

			def( grow )
			{
				def( body )
				{
					var( header, ref_header, untyped_str("get_header()"), __ );
					var( new_capacity, t_uw, untyped_str("grow_formula( header.Capacity)"), __);

					Code check_n_set = untyped_str(
						"if ( new_capacity < min_capacity )"
							"new_capacity = min_capacity;"
					);

					Code ret = untyped_str( "return set_capacity( new_capacity );" );

					body = function_body( header, new_capacity, check_n_set, ret );
				}

				function( grow, __, t_bool, params( t_uw, "min_capacity" ), body );
			}

			def( pop )
			{
				def( body )
				{
					var( header, ref_header, get_header(), UnusedCode );

					Code assertion = untyped_str( "assert( header.Num > 0 );" );
					Code decrement = untyped_str( "header.Num--; " );

					body = function_body( header, assertion, decrement );
				}

				function( pop, __, t_void, __, body );
			}

			def( reserve )
			{
				def( body )
				{
					var( header, ref_header, untyped_str("get_header()"), __ );

					Code check_n_set = untyped_str(
						"if ( header.Capacity < new_capacity )"
							"return set_capacity( new_capacity );"
					);

					Code ret = untyped_str("return true");

					body = function_body( header, check_n_set, ret );
				}

				function( reserve, __, t_bool, params( t_uw, "new_capacity" ), body );
			}

			def( resize )
			{
				def( body )
				{
					var( header, ref_header, untyped_str("get_header()"), __ );

					Code check_n_grow = untyped_str(
						"if ( header.Capacity < new_num )"
							"if ( ! grow( new_num) )"
								"return false;"
					);

					Code set_n_ret = untyped_str(
						"header.Count = new_num;"
						"return true;"
					);

					body = function_body( header, check_n_grow, set_n_ret );
				}

				function( resize, __, t_bool, params( t_uw, "new_num" ), body );
			}

			def( set_capacity )
			{
				def( body )
				{
					var( header, ref_header, untyped_str("get_header()"), __ );

					Code checks = untyped_str(
						"if ( capacity == header.Capacity )"
							"return true;"
					"\n\n"
						"if ( capacity < header.Num )"
							"header.Num = capacity;"
					);

					var( size,       t_uw,       untyped_str("sizeof(Header) + sizeof(Type) * capacity"), __ );
					var( new_header, ptr_header, untyped_str("rcast( Header*, alloc( header.Allocator, size ))"), __ );

					Code check_n_move = untyped_str(
						"if ( new_header == nullptr )"
							"return false;"
					"\n\n"
						"memmove( new_header, & header, sizeof(Header) + sizeof(Type) * header.Num );"
					);

					Code set_free_ret = untyped_str(
						"new_header->Allocator = header.Allocator;"
						"new_header->Num       = header.Num;"
						"new_header->Capacity  = header.Capacity;"
					"\n\n"
						"zpl_free( header );"
					"\n\n"
						"*Data = new_header + 1;"
					"\n\n"
						"return true;"
					);				

					body = function_body( header, checks, size, new_header, check_n_move, set_free_ret );
				}

				function( set_capacity, __, t_bool, params( t_uw, "capacity" ), body );
			}

			char const* name = bprintf( "Array_%s", type_str );

			Code body = struct_body(
				  Type 
				, Data

				, init
				, init_reserve
				, append
				, back
				, clear
				, fill
				, free
				, get_header
				, grow
				, pop
				, reserve
				, resize
				, set_capacity
			);

			array_def = def_struct( name, body, parent );
		}
	#endif

		return array_def;
	}

	u32 gen_array_file()
	{
		Code a_base = gen__array_base();

		Code a_u32  = gen_array( u32 );
		Code a_cstr = gen_array( char const* );

		Builder
		arraygen;
		arraygen.open( "Array.gen.hpp" );
		arraygen.print( a_u32 );
		arraygen.print( a_cstr );
		arraygen.write();
		return 0;
	}
#endif

#ifndef gen_time
#	include "Array.gen.hpp"

#	define array( Type_ ) array_##Type_
#endif
