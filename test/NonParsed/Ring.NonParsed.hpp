#pragma once

#if gen_time
#include "gen.hpp"
#include "Buffer.NonParsed.hpp"

using namespace gen;

Code gen__ring( StrC type, sw type_size )
{
	static Code t_allocator_info = def_type( name(AllocatorInfo) );

	String name;
	{
		char const* name_str = str_fmt_buf( "Ring_%s\0", type.Ptr );
		s32         name_len = str_len( name_str );

		name = get_cached_string({ name_len, name_str });
	};

	Code t_ring_type     = def_type( name );
	Code t_ring_type_ptr = def_type( name, __, spec_ptr );

	Code t_type       = def_type( type );
	Code t_type_ptr   = def_type( type, __, spec_ptr );
	Code t_type_ref   = def_type( type, __, spec_ref );

	Code t_buffer_type;
	{
		char const* name_str = str_fmt_buf( "Buffer_%s\0", type.Ptr );
		s32         len      = str_len( name_str );

		t_buffer_type = def_type( { len, name_str } );
	}

	Code ring;
	{
		Code backing  = def_variable( t_allocator_info, name(Backing) );
		Code capacity = def_variable( t_uw, name(Capacity) );
		Code head     = def_variable( t_uw, name(Head) );
		Code tail     = def_variable( t_uw, name(Tail) );
		Code buffer   = def_variable( t_buffer_type, name(Buffer) );

		Code init;
		{
			Code params = def_params( 2
				, def_param( t_allocator_info, name(allocator) )
				, def_param( t_uw,             name(max_size) )
			);

			char const* tmpl = txt(
				<type> result = { 0 };

				result.Backing = allocator;

				result.Buffer = Buffer_<data_type>::init( allocator, max_size + 1 );

				if ( result.Buffer == nullptr )
					return { nullptr };

				result.Capacity = max_size + 1;

				return result;
			);
			Code body = untyped_str( token_fmt( tmpl, 2
				, "type", (char const*)name
				, "data_type", (char const*)type
			));

			init = def_function( name(init), params, t_ring_type, body, spec_static_member );
		}

		ring = def_struct( name, def_struct_body( 6,
			init,

			backing,
			capacity,
			head,
			tail,
			buffer
		));
	}

	return ring;
}

struct GenRingRequest
{
	StrC Dependency;
	StrC Type;
	sw   TypeSize;
};
Array(GenRingRequest) GenRingRequests;

void gen__ring_request( StrC type, sw size, StrC dep = {} )
{
	do_once_start
		array_init( GenRingRequests, g_allocator );
	do_once_end

	// Make sure we don't already have a request for the type.
	for ( sw idx = 0; idx < array_count( GenRingRequests ); ++idx )
	{
		StrC const reqest_type = GenRingRequests[ idx ].Type;

		if ( reqest_type.Len != type.Len )
			continue;

		if ( str_compare( reqest_type.Ptr, type.Ptr, reqest_type.Len ) == 0 )
			return;
	}

	// Ring definition depends on a buffer definition.
	gen__buffer_request( type, size, dep );

	GenRingRequest request = { dep, type, size};
	array_append( GenRingRequests, request );
}
#define gen_ring( type ) gen__ring_request( { txt_n_len(type) }, sizeof( type ))

u32 gen_ring_file()
{
	Builder
	gen_ring_file;
	gen_ring_file.open( "ring.gen.hpp" );

	gen_ring_file.print( def_include( StrC::from("Bloat.hpp")) );
	gen_ring_file.print( def_include( StrC::from("buffer.gen.hpp")) );
	// gen_ring_file.print( gen__ring_base() );

	GenRingRequest* current = GenRingRequests;
	s32 left = array_count( GenRingRequests );
	while (left--)
	{
		GenRingRequest const& request = * current;

		Code generated_ring = gen__ring( current->Type, current->TypeSize );

		if ( request.Dependency )
		{
			char const* cmt_str = str_fmt_buf( "// Dependency for %s type", request.Type );
			s32         cmt_len = str_len( cmt_str );

			Code cmt     = def_comment( { cmt_len, cmt_str } );
			Code include = def_include( request.Dependency );

			gen_ring_file.print( cmt );
			gen_ring_file.print( include );
		}

		gen_ring_file.print( generated_ring );
		current++;
	}

	gen_ring_file.write();
	return 0;
}

#endif // gen_time
