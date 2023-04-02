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
			Code params = def_parameters(1, "value", t_uw );
			Code body   = untyped_fmt( "\t""return 2 * value * 8;" );

			grow_formula = def_function( "grow_formula", spec, params, t_sw, body );
		}

		Code base_body = def_struct_body(2, header, grow_formula);
		Code base      = def_struct( "ArrayBase", base_body );
		return base;
	}

	#define gen_array( Type_ ) gen__array( #Type_, sizeof(Type_), a_base )
	Code gen__array( char const* type_str, s32 type_size, Code parent )
	{
		// Make these global consts to be accessed anywhere...
			Code t_uw        = def_type( txt(uw) );
			Code t_sw        = def_type( txt(sw) );
			Code t_bool      = def_type( txt(bool) );
			Code t_allocator = def_type( txt(allocator) );
			Code t_void      = def_type( txt(void) );

			Code v_nullptr = untyped_fmt( "nullptr" );

			Code spec_ct     = def_specifiers(1, Specifier::Constexpr );
			Code spec_inline = def_specifiers(1, Specifier::Inline );

		Code type     = def_type( type_str );
		Code ptr_type = def_type( string_sprintf_buf( g_allocator, "%s*", type_str ) );
		Code ref_type = def_type( string_sprintf_buf( g_allocator, "%s&", type_str ) );

		string name = string_sprintf_buf( g_allocator, "Array_%s", type_str );

		// From ArrayBase
			Code t_header   = def_type( "Header" );
			Code ptr_header = def_type( "Header*" );
			Code ref_header = def_type( "Header&" );

		Code array_def;
		{
			Code using_type = def_using( "type", type );
			Code data       = def_variable( "Data", ptr_type );

			Code init;
			{
				Code params = def_parameters( 1, "mem_hanlder", t_allocator );
				Code body   = untyped_fmt( "\t""return init_reserve( mem_handler, grow_formula(0) );" );

				init = def_function( "init", UnusedCode, params, t_bool, body );
			}

			Code init_reserve;
			{
				Code params = def_parameters( 2, "mem_handler", ref_type, "capacity", t_sw );
				Code body;
				{
					Code header_value = untyped_fmt( 
						"rcast( Header*, alloc( mem_handler, sizeof( Header ) + sizeof(type) + capacity ))"
					);
					Code header = def_variable( "", ptr_header, header_value );

					Code null_check = untyped_fmt(
						"\t"	"if (header == nullptr)"
						"\n\t\t"	"return false;"
					);

					Code header_init = untyped_fmt(
						"\n\t" "header->Num       = 0;"
						"\n\t" "header->Capacity  = capacity;"
						"\n\t" "header->Allocator = mem_handler;"
					);

					Code assign_data = untyped_fmt(
						"\t" "Data = rcast( %s, header + 1 );", ptr_type
					);

					Code ret_true = untyped_fmt( "\t""return true" );

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
				Code body = untyped_fmt( 
					"\t"	"Header& header = get_header();"
					"\n\t"	"::free( header.Allocator, & get_header() );"
				);

				free = def_function( "free", UnusedCode, UnusedCode, t_void, body );
			}

			Code append;
			{
				Code params = def_parameters( 1, "value", type );
				Code body;
				{
					Code header = def_variable( "", ref_header, untyped_fmt( "get_header()") );

					Code check_cap = untyped_fmt(
						"\t"		"if ( header.Capacity < header.Num + 1 )"
						"\n\t\t"		"if ( ! grow(0) )"
						"\n\t\t\t"			"return false;"
					);

					Code assign = untyped_fmt(
						"\t"   "Data[ header.Num ] = value;"
						"\t\n" "header.Num++;"
						"\n"
						"\n\t" "return true;"
					);

					body = def_function_body( 3, header, check_cap, assign );
				}

				append = def_function( "append", UnusedCode, params, t_void, body );
			}

			Code back;
			{
				Code body = untyped_fmt(
					"\t"	"Header& header = get_header();"
					"\n\t"	"return data[ header.Num - 1 ];"
				);

				back = def_function( "back", UnusedCode, UnusedCode, type, body );
			}

			Code clear;
			{
				Code body = untyped_fmt( "\t""get_header().Num = 0;" );

				clear = def_function( "clear", UnusedCode, UnusedCode, t_void, body );
			}

			Code fill;
			{
				Code params = def_parameters( 3, "begin", t_uw, "end", t_uw, "value", type );
				Code body;
				{
					Code header = def_variable( "", ref_header, untyped_fmt( "get_header()") );

					Code check = untyped_fmt(
						"\t"	"if ( begin < 0 || end >= header.Num )"
						"\n\t\t"	"fatal( \"Range out of bounds\" );"
					);

					Code iter = untyped_fmt(
						"\t"	"for ( sw index = begin; index < end; index++ )"
						"\n\t\t"	"Data[index] = vallue;"
					);

					body = def_function_body( 3, header, check, iter );
				}

				fill = def_function( "fill", UnusedCode, params, t_void, body );
			}

			Code get_header;
			{
				Code body = untyped_fmt( "\t""return pcast( Header, Data - 1 );" );

				get_header = def_function( "get_header", spec_inline, UnusedCode, ref_header, body );
			}

			Code grow;
			{
				Code param = def_parameters( 1, "min_capacity", t_uw );
				Code body;
				{
					Code header       = def_variable( "header", ref_header, untyped_fmt("get_header") );
					Code new_capacity = def_variable( "new_capacity", t_uw, untyped_fmt("grow_formula( header.Capacity )") );

					Code check_n_set = untyped_fmt(
						"\t"	"if ( new_capacity < min_capacity )"
						"\n\t\t"	"new_capacity = min_capacity;"
					);

					Code ret = untyped_fmt( "\t" "return set_capacity( new_capacity );" );

					body = def_function_body( 4, header, new_capacity, check_n_set, ret );
				}

				grow = def_function( "grow", UnusedCode, param, t_bool, body );
			}

			Code pop;
			{
				Code body;
				{
					Code header = def_variable( "header", ref_header, untyped_fmt("get_header()") );

					Code assertion = untyped_fmt( "\t" "assert( header.Num > 0 );" );
					Code decrement = untyped_fmt( "\t" "header.Num--; " );

					body = def_function_body( 3, header, assertion, decrement );
				}

				pop = def_function( "pop", UnusedCode, UnusedCode, t_void, body );
			}

			Code reserve;
			{
				Code params = def_parameters( 1, "new_capacity", t_uw );
				Code body;
				{
					Code header = def_variable( "header", ref_header, untyped_fmt("get_header()") );

					Code check_n_set = untyped_fmt(
						"\t" 	"if ( header.Capacity < new_capacity )"
						"\n\t\t"	"return set_capacity( new_capacity );"
					);

					Code ret = untyped_fmt( "\t" "return true" );

					body = def_function_body( 3, header, check_n_set, ret );
				}

				reserve = def_function( "reserve", UnusedCode, params, t_bool, body );
			}

			Code resize;
			{
				Code param = def_parameters( 1, "new_num", t_uw );

				Code body;
				{
					Code header = def_variable( "header", ref_header, untyped_fmt("get_header()") );

					Code check_n_grow = untyped_fmt(
						"\t"	"if ( header.Capacity < new_num )"
						"\n\t\t"	"if ( ! grow( new_num) )"
						"\n\t\t\t"		"return false;"
					);

					Code set_n_ret = untyped_fmt(
						"\t"	"header.Count = new_num;"
						"\n\t"	"return true;"
					);

					body = def_function_body( 3, header, check_n_grow, set_n_ret );
				}

				resize = def_function( "resize", UnusedCode, param, t_bool, body );
			}

			Code set_capacity;
			{
				Code param = def_parameters( 1, "capacity", t_uw );

				Code body;
				{
					Code header = def_variable( "header", ref_header, untyped_fmt("get_header()") );

					Code checks = untyped_fmt(
						"\t"	"if ( capacity == header.Capacity )"
						"\n\t\t" "return true;"
						"\n"
						"\n\t" "if ( capacity < header.Num )"
						"\n\t\t" "header.Num = capacity;"
					);

					Code size       = def_variable( "size", t_uw, untyped_fmt("sizeof(Header) + sizeof(type) * capacity"));
					Code new_header = def_variable( "new_header", ptr_header, untyped_fmt("rcast( Header*, alloc( header.Allocator, size ));"));

					Code check_n_move = untyped_fmt(
						"\t""if ( new_header == nullptr )"
						"\n\t\t""return false;"
						"\n"
						"\n\t""memmove( new_header, & header, sizeof(Header) + sizeof(type) * header.Num );"
					);

					Code set_free_ret = untyped_fmt(
						"\t"	"new_header->Allocator = header.Allocator;"
						"\n\t"	"new_header->Num       = header.Num;"
						"\n\t"	"new_header->Capacity  = header.Capacity;"
						"\n"
						"\n\t"	"zpl_free( header );"
						"\n"
						"\n\t"	"*Data = new_header + 1;"
						"\n"
						"\n\t"	"return true;"
					);				

					body = def_function_body( 6, header, checks, size, new_header, check_n_move, set_free_ret );
				}

				set_capacity = def_function( "set_capacity", UnusedCode, param, t_bool, body );
			}

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
