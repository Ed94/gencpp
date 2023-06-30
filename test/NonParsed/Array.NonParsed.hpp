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

		Code spec = def_specifiers( 2, ESpecifier::Static_Member, ESpecifier::Inline );

		grow_formula = def_function( name(grow_formula), params, t_uw, body, spec );
	}

	Code body       = def_struct_body( 2, header, grow_formula );
	Code array_base = def_struct( name(ArrayBase), body );

	return array_base;
}

Code gen__array( StrC type, sw type_size )
{
	static Code t_allocator_info = def_type( name(AllocatorInfo) );
	static Code v_nullptr        = untyped_str( code(nullptr));

	// static Code array_base = def_type( name(ArrayBase) );
	static Code header;
	do_once_start
		Code allocator = def_variable( t_allocator_info, name(Allocator) );
		Code capacity  = def_variable( t_uw,             name(Capacity) );
		Code num       = def_variable( t_uw,             name(Num) );

		Code body   = def_struct_body( 3, allocator, capacity, num );
		     header = def_struct( name(Header), body );
	do_once_end

	static Code grow_formula;
	do_once_start
		Code params = def_param( t_uw, name(value));
		Code body   = untyped_str( code( return 2 * value * 8; ));

		Code spec = def_specifiers( 2, ESpecifier::Static_Member, ESpecifier::Inline );

		grow_formula = def_function( name(grow_formula), params, t_uw, body, spec );
	do_once_end

	StrC name;
	{
		char const* name_str = str_fmt_buf( "Array_%s\0", type.Ptr );
		s32         name_len = str_len( name_str );

		name = { name_len, name_str };
	};

	Code t_array_type = def_type( name );

	Code t_type     = def_type( type );
	Code t_type_ptr = def_type( type, __, spec_ptr );
	Code t_type_ref = def_type( type, __, spec_ref );

	Code t_header     = def_type( name(Header) );
	Code t_header_ptr = def_type( name(Header), __, spec_ptr );
	Code t_header_ref = def_type( name(Header), __, spec_ref );

	static Code spec_static_inline = def_specifiers( 2, ESpecifier::Static_Member, ESpecifier::Inline );
	static Code spec_static        = def_specifier( ESpecifier::Static_Member );

	Code array;
	{
		Code using_type = def_using( name(Type), t_type );
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
			Code params = def_params( 2, (Code[2]){
				def_param( t_allocator_info, name(allocator) ),
				def_param( t_sw, name(capacity) )
			});

			char const* tmpl = txt(
				Header* header = rcast( Header*, alloc( allocator, sizeof(Header) + sizeof(Type) ));

				if ( header == nullptr )
					return (<type>){ nullptr };

				header->Allocator = allocator;
				header->Capacity  = capacity;
				header->Num       = 0;

				return (<type>){ rcast( Type*, header + 1) };
			);

			Code body = untyped_str( token_fmt( tmpl, 1, "type", (char const*)t_array_type->Name) );

			init_reserve = def_function( name(init_reserve), params, t_array_type, body, spec_static );
		}

		Code append;
		{
			Code params = def_param( t_type, name(value));

			Code body = untyped_str( code(
				Header& header = get_header();

				if ( header.Num == header.Capacity )
				{
					if ( ! grow( header.Capacity ))
						return false;
				}

				Data[ header.Num ] = value;
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

			back = def_function( name(back), __, t_type_ref, body, spec_inline );
		}

		Code clear = def_function( name(clear), __, t_void, untyped_str( code(
			Header& header = get_header();
			header.Num = 0;
		)), spec_inline );

		Code fill;
		{
			Code params = def_params( 3, (Code[3]){
				def_param( t_uw, name(begin) ),
				def_param( t_uw, name(end) ),
				def_param( t_type, name(value) )
			});

			Code body = untyped_str( code(
				Header& header = get_header();

				if ( begin < 0 || end >= header.Num )
					return false;

				for ( sw idx = begin; idx < end; idx++ )
				{
					Data[ idx ] = value;
				}

				return true;
			));

			fill = def_function( name(fill), params, t_bool, body );
		}

		Code free;
		{
			Code body = untyped_str( code(
				Header& header = get_header();
				::free( header.Allocator, & header );
			));

			free = def_function( name(free), __, t_void, body, spec_inline );
		}

		Code get_header = def_function( name(get_header), __, t_header_ref, untyped_str( code(
			return * ( rcast( Header*, Data ) - 1 );
		)));

		Code grow;
		{
			Code param = def_param( t_uw, name(min_capacity) );

			Code body = untyped_str( code(
				Header& header = get_header();

				uw new_capacity = grow_formula( header.Capacity );

				if ( new_capacity < min_capacity )
					new_capacity = 8;

				return set_capacity( new_capacity );
			));

			grow = def_function( name(grow), param, t_bool, body );
		}

		Code pop;
		{
			Code body = untyped_str( code(
				Header& header = get_header();

				ZPL_ASSERT( header.Num > 0 );
				header.Num--;
			));

			pop = def_function( name(pop), __, t_bool, body, spec_inline );
		}

		Code reserve;
		{
			Code param = def_param( t_uw, name(new_capacity) );

			Code body = untyped_str( code(
				Header& header = get_header();

				if ( header.Capacity < new_capacity )
					return set_capacity( new_capacity );

				return true;
			));

			reserve = def_function( name(reserve), param, t_bool, body );
		}

		Code resize;
		{
			Code param = def_param( t_uw, name( num ));

			Code body = untyped_str( code(
				Header& header = get_header();

				if ( num > header.Capacity )
				{
					if ( ! grow( header.Capacity ))
						return false;
				}

				header.Num = num;
				return true;
			));

			resize = def_function( name(resize), param, t_bool, body );
		}

		Code set_capacity;
		{
			Code param = def_param( t_uw, name(new_capacity) );

			Code body = untyped_str( code(
				Header& header = get_header();

				if ( new_capacity == header.Capacity )
					return true;

				if ( new_capacity < header.Num )
					header.Num = new_capacity;

				sw      size       = sizeof(Header) + sizeof(Type) * new_capacity;
				Header* new_header = rcast( Header*, alloc( header.Allocator, size ));

				if ( new_header == nullptr )
					return false;

				mem_move( new_header, & header, sizeof( Header ) + sizeof(Type) * header.Num );

				new_header->Allocator = header.Allocator;
				new_header->Num       = header.Num;
				new_header->Capacity  = new_capacity;

				::free( header.Allocator, & header );

				Data = (u8*) new_header + 1;

				return true;
			));

			set_capacity = def_function( name(set_capacity), param, t_bool, body );
		}

		Code body = def_struct_body( 17
			, header
			, grow_formula
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

void gen__array_request( StrC type, sw size, StrC dep = {} )
{
	do_once_start
		array_init( GenArrayRequests, g_allocator );
	do_once_end

	GenArrayRequest request = { type, dep, size };
	array_append( GenArrayRequests, request );
}
#define gen_array( type ) gen__array_request( { txt_n_len(type) }, sizeof(type) )

u32 gen_array_file()
{
	Builder
	gen_array_file;
	gen_array_file.open( "array.gen.hpp" );

	Code include_zpl = def_include( StrC::from("../../thirdparty/zpl.h") );
	gen_array_file.print( include_zpl );

	// Code array_base = gen__array_base();
	// gen_array_file.print( array_base );

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
