#pragma once

#if gen_time
#include "gen.hpp"

using namespace gen;

Code gen__array_base()
{
	Code t_allocator_info = def_type( name(AllocatorInfo) );
	Code header;
	{
		Code allocator = def_variable( t_allocator_info, name(Allocator) );
		Code capacity  = def_variable( t_uw,             name(Capacity) );
		Code num       = def_variable( t_uw,             name(Num) );

		Code body   = def_struct_body( 3, allocator, capacity, num );
		     header = def_struct( name(Header), body );
	}

	Code grow_formula;
	{
		Code params = def_param( t_uw, name(value));
		Code body   = untyped_str( code( return 2 * value * 8; ));

		Code spec = def_specifiers( ESpecifier::Static_Member, ESpecifier::Inline );

		grow_formula = def_function( name(grow_formula), params, t_uw, body, spec );
	}

	Code body       = def_struct_body( 2, header, grow_formula );
	Code array_base = def_struct( name(ArrayBase), body );

	return array_base;
}

Code gen__array( StrC type, sw type_size )
{
	Code t_allocator_info = def_type( name(AllocatorInfo) );
	Code v_nullptr        = untyped_str( code(nullptr));

	static Code ArrayBase = gen__array_base();

	StrC name;
	{
		char const* name_str = str_fmt_buf( "Array_%s", type.Ptr );
		s32         name_len = str_len( name );

		name = { name_len, name_str };
	};

	Code t_array_type = def_type( name );

	Code t_type     = def_type( type );
	Code t_type_ptr = def_type( type, __, spec_ptr );
	Code t_type_ref = def_type( type, __, spec_ref );

	Code t_header     = def_type( name(Header) );
	Code t_header_ptr = def_type( name(Header), __, spec_ptr );
	Code t_header_ref = def_type( name(Header), __, spec_ref );

	Code spec_static_inline = def_specifiers( ESpecifier::Static_Member, ESpecifier::Inline );
	Code spec_static        = def_specifiers( ESpecifier::Static_Member );

	Code array;
	{
		Code using_type = def_using( name(Type), UsingRegular, t_type );
		Code data       = def_variable( t_type_ptr, name(Data) );

		Code init;
		{
			Code params = def_param( t_allocator_info, name(allocator) );
			Code body   = untyped_str( code(
				return init_reserve( allocator, grow_formula(0) );
			));

			init = def_function( name(init), params, t_array_type, body, spec_static );
		}

		Code init_reserve;
		{
			Code params = def_params( 2, t_allocator_info, txt_n_len(allocator), t_sw, txt_n_len(capacity) );
			Code body = untyped_str( code(
				Header* header = rcast( Header*, alloc( allocator, sizeof(Header) + sizeof(Type) ));

				if ( header == nullptr )
					return false;

				header->Allocator = allocator;
				header->Capacity  = capacity;
				header->Num       = 0;

				Data = rcast( Type*, header + 1 );

				return true;
			));

			init_reserve = def_function( name(init_reserve), params, t_array_type, body, spec_static );
		}

		Code append;
		{
			Code params = def_param( t_type, name(value));

			Code body = untyped_str( code(
				Header& header = get_header();

				if ( header.Num == header.Capacity )
				{
					if ( ! grow( header.Allocator ))
						return false;
				}

				data[ header.Num ] = value;
				header.Num++;

				return true;
			));

			append = def_function( name(append), params, t_bool, body );
		}

		Code back;
		{
			Code body = untyped_str( code(
				Header& header = get_header();
				return Data[ header.Num - 1 ];
			));

			back = def_function( name(back), __, t_type_ref, body );
		}

		Code clear;
		Code fill;

		Code free;
		{
			Code body = untyped_str( code(
				Header& header = get_header();
				::free( header.Allocator, & header );
			));

			free = def_function( name(free), __, t_void, body, spec_inline );
		}

		Code get_header;
		Code grow;
		Code pop;
		Code reserve;
		Code resize;
		Code set_capacity;

		Code body = def_struct_body( 17
			, using_type

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

			, data
		);

		array = def_struct( name, body );
	}

	return array;
}


struct GenArrayRequest
{
	StrC Type;
	StrC Dependency;
	sw   Size;
};
Array(GenArrayRequest) GenArrayRequests;

void gen__array_request( StrC type, StrC dep, sw size )
{
	GenArrayRequest request = { type, dep, size };
	array_append( GenArrayRequests, request );
}
#define Gen_Array( type ) gen__array_request( txt_n_len( type ), sizeof(type) )

u32 gen_array_file()
{
	Builder
	gen_array_file;
	gen_array_file.open( "array.gen.hpp" );


	GenArrayRequest* current = GenArrayRequests;
	s32 left = array_count( GenArrayRequests );
	while (left--)
	{
		GenArrayRequest const& request = * current;

		Code generated_array = gen__array( request.Type, request.Size );

		if ( request.Dependency )
		{
			char const* cmt_str = str_fmt_buf( "// Dependency for %s type", request.Type );
			s32         cmt_len = str_len( cmt_str );

			Code cmt     = def_comment( { cmt_len, cmt_str } );
			Code include = def_include( request.Dependency );

			gen_array_file.print( cmt );
			gen_array_file.print( include );
		}

		gen_array_file.print( generated_array );
		current++;
	}

	gen_array_file.write();
	return 0;
}

#endif
