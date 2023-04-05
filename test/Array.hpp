/*
	This is based of the array container implementation in the zpl.h library.

	This specific header has two implementations of the array generator;
	One showing use of the gen api directly, the other using it's DSL.
*/

#pragma once

#include "Bloat.hpp"
#include "gen.hpp"

#ifdef gen_time
	Code gen__array_base()
	{
	#ifndef GEN_DEFINE_DSL
		using namespace gen;

		Code t_allocator = def_type( txt(allocator)  );

		Code header;
		{
			Code num           = def_variable( t_uw, "Num" );
			Code capacity      = def_variable( t_uw, "Capacity" );
			Code allocator_var = def_variable( t_allocator, "Allocator" );
			Code header_body   = def_struct_body( 3, num, capacity, allocator_var );

			header = def_struct( "ArrayHeader", header_body );
		}

		Code grow_formula;
		{
			Code spec   = def_specifiers(1, ESpecifier::Inline);
			Code params = def_params(1, t_uw, "value" );
			Code body   = untyped_str( txt( return 2 * value * 8; ) );

			grow_formula = def_proc( "grow_formula", spec, params, t_sw, body );
		}

		Code body      = def_struct_body(2, header, grow_formula);
		Code ArrayBase = def_struct( "ArrayBase", body );
		
	#else
		typename( allocator, allocator );

		Code Header;
		{
			variable( uw,        Num,      );
			variable( uw,        Capacity  );
			variable( allocator, Allocator );

			Code body = struct_body( Num, Capacity, Allocator );

			struct( Header, __, __, body );
		}

		Code proc( grow_formula, spec_inline, t_uw, params( t_uw, "value" ), 
			untyped( return 2 * value * 8 ) 
		);

		Code struct( ArrayBase, __, __, struct_body( Header, grow_formula ) );
	#endif

		return ArrayBase;
	}

	Code gen__array( char const* type_str, s32 type_size, Code parent )
	{
	#ifndef GEN_DEFINE_DSL
		// Make these global consts to be accessed anywhere...
			Code t_allocator = def_type( txt(allocator) );

			Code v_nullptr = untyped_str( "nullptr" );

			Code spec_ct = def_specifiers(1, ESpecifier::Constexpr );

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
			Code data       = def_variable( ptr_type, "Data" );

			// This getter is used often in all the member procedures
			Code header = def_variable( ref_header, "header", untyped_str( txt( get_header() )));

			Code init;
			{
				Code params = def_params( 1, t_allocator, "mem_handler" );
				Code body   = untyped_str( txt( return init_reserve( mem_handler, grow_formula(0) ); ) );

				init = def_proc( "init", UnusedCode, params, t_bool, body );
			}

			Code init_reserve;
			{
				Code params = def_params( 2, t_allocator, "mem_handler", t_sw, "capacity" );
				Code body;
				{
					Code header_value = untyped_str( txt( 
						rcast( Header*, alloc( mem_handler, sizeof( Header ) + sizeof(Type) + capacity )) 
					));

					Code header = def_variable( ptr_header, "header", header_value );

					Code null_check = untyped_str( txt(
						if (header == nullptr)
							return false;
					));

					Code header_init = untyped_str( txt(
						header->Num       = 0;
						header->Capacity  = capacity;
						header->Allocator = mem_handler;
					));

					Code assign_data = untyped_fmt(
						"Data = rcast( %s, header + 1 );", ptr_type
					);

					Code ret_true = untyped_str( txt( 
						return true 
					));

					body = def_proc_body( 5
					,	header
					,	null_check
					,	header_init
					,	assign_data
					,	ret_true
					);
				}

				init_reserve = def_proc( "init_reserve", UnusedCode, params, t_bool, body );
			}

			Code free;
			{
				Code body = untyped_str( txt(
					Header& header = get_header();
					::free( header.Allocator, & get_header() );
				));

				free = def_proc( "free", UnusedCode, UnusedCode, t_void, body );
			}

			Code append = make_proc( "append" );
			{
				append->add( def_params( 1, type, "value") );
				append->add( t_bool );

				Code 
				body = append.body();
				body->add( 
					untyped_str( txt(
						if ( header.Capacity < header.Num + 1 )
							if ( ! grow(0) )
								return false;
					))
				);

				body->add( untyped_str( txt(
					Data[ header.Num ] = value;
					header.Num++;
					
					return true;
				)));

				append->check();
			}

			Code back;
			{
				Code body = untyped_str( txt(
					Header& header = get_header();

					return data[ header.Num - 1 ];
				));

				back = def_proc( "back", UnusedCode, UnusedCode, type, body );
			}

			Code clear;
			{
				Code body = untyped_str( txt( get_header().Num = 0; ));

				clear = def_proc( "clear", UnusedCode, UnusedCode, t_void, body );
			}

			Code fill;
			{
				Code params = def_params( 3, t_uw, "begin", t_uw, "end", type, "value" );
				Code body;
				{
					Code check = untyped_str( txt(
						if ( begin < 0 || end >= header.Num )
							fatal( "Range out of bounds" );
					));

					Code iter = untyped_str( txt(
						for ( sw index = begin; index < end; index++ )
							Data[index] = vallue;
					));

					body = def_proc_body( 3, header, check, iter );
				}

				fill = def_proc( "fill", UnusedCode, params, t_void, body );
			}

			Code get_header;
			{
				Code body = untyped_str( txt( return pcast( Header, Data - 1 ); ));

				get_header = def_proc( "get_header", spec_inline, UnusedCode, ref_header, body );
			}

			Code grow;
			{
				Code param = def_params( 1, t_uw, "min_capacity" );
				Code body;
				{
					Code new_capacity = def_variable( t_uw, "new_capacity", untyped_str("grow_formula( header.Capacity )") );

					Code check_n_set = untyped_str( txt(
						if ( new_capacity < min_capacity )
							new_capacity = min_capacity;
					));

					Code ret = untyped_str( "return set_capacity( new_capacity );" );

					body = def_proc_body( 4, header, new_capacity, check_n_set, ret );
				}

				grow = def_proc( "grow", UnusedCode, param, t_bool, body );
			}

			Code pop;
			{
				Code body;
				{
					Code assertion = untyped_str( "assert( header.Num > 0 );" );
					Code decrement = untyped_str( "header.Num--; " );

					body = def_proc_body( 3, header, assertion, decrement );
				}

				pop = def_proc( "pop", UnusedCode, UnusedCode, t_void, body );
			}

			Code reserve;
			{
				Code params = def_params( 1, t_uw, "new_capacity" );
				Code body;
				{
					Code check_n_set = untyped_str(
							"if ( header.Capacity < new_capacity )"
						"\n"	"return set_capacity( new_capacity );"
					);

					Code ret = untyped_str( "\t" "return true" );

					body = def_proc_body( 3, header, check_n_set, ret );
				}

				reserve = def_proc( "reserve", UnusedCode, params, t_bool, body );
			}

			Code resize;
			{
				Code param = def_params( 1, t_uw, "new_num" );

				Code body;
				{
					Code check_n_grow = untyped_str( txt(
						if ( header.Capacity < new_num )
							if ( ! grow( new_num) )
								return false;
					));

					Code set_n_ret = untyped_str(
							"header.Count = new_num;"
						"\n""return true;"
					);

					body = def_proc_body( 3, header, check_n_grow, set_n_ret );
				}

				resize = def_proc( "resize", UnusedCode, param, t_bool, body );
			}

			Code set_capacity = parse_proc( txt_with_length(
				bool set_capacity( sw new_capacity )
				{
					Header& header = get_header();

					if ( capacity == header.Capacity )
						return true;

					if ( capacity < header.Num )
						header.Num = capacity;

					sw      size       = sizeof(Header) + sizeof(Type) * capacity;
					Header* new_header = rcast( Header* alloc( header.Allocator, size ));

					if ( new_header == nullptr )
						return false;

					memmove( new_header, & header, sizeof( Header ) + sizeof(Type) * header.Num );

					new_header->Allocator = header.Allocator;
					new_header->Num       = header.Num;
					new_header->Capacity  = header.Capacity;

					free( header );

					*Data = new_header + 1;

					return true;
				}
			));

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
		typename( allocator, allocator );
		
		untyped( v_nullptr, nullptr );

		typename( elem_type, type_str );
		typename_fmt( ptr_type, "%s*", type_str );
		typename_fmt( ref_type, "%&", type_str );


		// From ArrayBase
			typename( header, Header );
			typename( ptr_header, Header* );
			typename( ref_header, Header& );

		Code array_def;
		{
			using_type( Type, elem_type );

			variable( ptr_type, Data );
			variable( ref_header, header, untyped_str("get_header()") );

			Code init;
			{
				Code body = proc_body( untyped( 
					return init_reserve( mem_handler, grow_formula(0) ); 
				));

				proc( init, __, t_bool, params( t_allocator, "mem_handler" ), body );
			}

			make( proc, init_reserve, __, params( t_ref_type, "mem_handler" ), t_bool);
			{
				Code 
				body = init_reserve.body();
				body->add_var( ptr_header, header, untyped(
					value_str( rcast( Header*, alloc( mem_handler, sizeof(Header) + sizeof(Type) + capacity)) )
				) );

				body->add_untyped(
					if (header == nullptr)
						return false;
				);

				body->add_untyped(
					header->Num       = 0;
					header->Capacity  = capacity;
					header->Allocator = mem_handler;
				);

				body->add_untyped(
					Data = rcast( Type*, header + 1 );
					return true;
				);
			}

			Code free;
			{
				proc( free, __, t_void, __, untyped( 
					Header& header = get_header();
					
					free( header.Allocator, & get_header() );
				));
			}

			make( proc, append )
			{
				append->add_params( t_elem_value, "value" );
				append->add_ret_type( void );

				Code 
				body = append.body();
				body->add_untyped(
					if ( header.Capacity < header.Num + 1 )
						if ( ! grow(0) )
							return false;
				);

				body->add_untyped( assign,
					Data[ header.Num ] = value;
					header.Num++;
					
					return true;
				);
			}

			Code back;
			{
				Code body = untyped(
					Header& header = get_header();

					return data[ header.Num - 1 ];
				);

				proc( back, __, t_elem_type, __, body );
			}

			Code clear;
			proc( clear, __, t_void, __, untyped_str("get_header().Num = 0;") );

			Code fill;
			{
				Code check = untyped(
					if ( begin < 0 || end >= header.Num )
						fatal( "Range out of bounds" );
				);

				Code iter = untyped(
					for ( sw index = begin; index < end; index++ )
						Data[index] = vallue;
				);

				Code body = proc_body( header, check, iter );

				proc( fill, __, t_void, params( t_uw, "begin", t_uw, "end", t_elem_type, "value" ), body );
			}

			Code get_header;
			proc( get_header, spec_inline, t_ref_header, __, untyped_str("return pcast( Header, Data - 1);") );

			Code grow;
			{
				Code body;
				{
					variable( uw, new_capacity, untyped( grow_formula( header.Capacity) ));

					Code check_n_set = untyped(
						if ( new_capacity < min_capacity )
							new_capacity = min_capacity;
					);

					Code ret = untyped( return set_capacity( new_capacity ); );

					body = proc_body( header, new_capacity, check_n_set, ret );
				}

				proc( grow, __, t_bool, params( t_uw, "min_capacity" ), body );
			}

			Code pop;
			{
				untyped_code( assertion,  assert( header.Num > 0 ); );
				untyped_code( decrement,  header.Num--;  );

				Code body = proc_body( header, assertion, decrement );

				proc( pop, __, t_void, __, body );
			}

			Code reserve;
			{
				untyped_code( check_n_set,
					if ( header.Capacity < new_capacity )
						return set_capacity( new_capacity );
				);

				Code ret = untyped_str("return true");

				Code body = proc_body( header, check_n_set, ret );

				proc( reserve, __, t_bool, params( t_uw, "new_capacity" ), body );
			}

			Code resize;
			{
				Code body;
				{
					untyped_code( check_n_grow,
						if ( header.Capacity < new_num )
							if ( ! grow( new_num) )
								return false;
					);

					untyped_code( set_n_ret,
						header.Count = new_num;
						return true;
					);

					body = proc_body( header, check_n_grow, set_n_ret );
				}

				proc( resize, __, t_bool, params( t_uw, "new_num" ), body );
			}

			Code set_capacity = proc_code(
				bool set_capacity( new_capacity )
				{
					Header& header = get_header();

					if ( capacity == header.Capacity )
						return true;

					if ( capacity < header.Num )
						header.Num = capacity;

					uw      size       = sizeof(Header) + sizeof(Type) * capacity;
					Header* new_header = rcast( Header* alloc( header.Allocator, size ));

					if ( new_header == nullptr )
						return false;

					memmove( new_header, & header, sizeof( Header ) + sizeof(Type) * header.Num );

					new_header->Allocator = header.Allocator;
					new_header->Num       = header.Num;
					new_header->Capacity  = header.Capacity;

					free( header );

					*Data = new_header + 1;

					return true;
				}
			);

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

	struct ArrayRequest
	{
		char const* Name;
		sw          Size;
	};

	array(ArrayRequest) UserArrayGenQueue;

	#define gen_array( Type_ ) add_gen_array_request( #Type_, sizeof(Type_) )

	void add_gen_array_request( const char* type_str, sw type_size )
	{
		ArrayRequest request = { type_str, type_size };

		array_append( UserArrayGenQueue, request );
	}

	u32 gen_array_file()
	{
		Code a_base = gen__array_base();

		add_gen_array_request( "u32", sizeof(u32) );
		gen_array( char const* );

		array(Code) array_asts;
		array_init( array_asts, g_allocator );

		sw left  = array_count( UserArrayGenQueue );
		sw index = 0;
		while( left -- )
		{
			ArrayRequest request = UserArrayGenQueue[index];

			Code result = gen__array( request.Name, request.Size, a_base );

			array_append( array_asts, result );
		}

		Builder
		arraygen;
		arraygen.open( "Array.gen.hpp" );

		left  = array_count( array_asts );
		index = 0;

		while( left-- )
		{
			Code code = array_asts[index];

			arraygen.print( code );
		}

		arraygen.write();
		return 0;
	}
#endif

#ifndef gen_time
#	include "Array.gen.hpp"

#	define array( Type_ ) array_##Type_
#endif
