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

	Code ring = {0};
	{
		Code using_type = def_using( name(Type), t_type );

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
			Code body = def_execution( token_fmt( tmpl, 2
				, "type", (char const*)name
				, "data_type", (char const*)type
			));

			init = def_function( name(init), params, t_ring_type, body, spec_static_member );
		}

		Code append = def_function( name(append), def_param( t_type, name(value)), t_void
			, def_execution( code(
				Buffer[ Head ] = value;
				Head = ( Head + 1 ) % Capacity;

			if ( Head == Tail )
					Tail = ( Tail + 1 ) % Capacity;
			))
		);

		Code appendv;
		{
			Code params = def_params( 2
				, def_param( t_type_ptr, name(values))
				, def_param( t_sw,       name(num))
			);

			Code body = def_execution( code(
				for ( sw idx = 0; idx < num; idx++ )
					append( values[ idx ] );
			));

			appendv = def_function( name(append), params, t_void, body, spec_inline );
		}

		Code empty = def_function( name(empty), __, t_bool
			, def_execution( code(
				return Head == Tail;
			))
		);

		Code free = def_function( name(free), __, t_void
			, def_execution( code(
				Buffer.free();
			))
		);

		Code full = def_function( name(full), __, t_bool
			, def_execution( code(
				return (Head + 1) % Capacity == Tail;
			))
		);

		Code get = def_function( name(get), __, t_type_ref
			, def_execution( code(
				Type& data = Buffer[ Tail ];
				Tail = ( Tail + 1 ) % Capacity;

				return data;
			))
		);

		Code wipe = def_function( name(wipe), __, t_void
			, def_execution( code(
				Head = 0;
				Tail = 0;
				Buffer.wipe();
			))
		);

		ring = def_struct( name, def_struct_body( 14,
			using_type,

			init,

			append,
			appendv,
			empty,
			free,
			full,
			get,
			wipe,

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
		array_init( GenRingRequests, Memory::GlobalAllocator );
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

	// Ring definition depends on a array and buffer definition.
	gen__buffer_request( type, size, dep );

	GenRingRequest request = { dep, type, size};
	array_append( GenRingRequests, request );
}
#define gen_ring( type ) gen__ring_request( { txt_to_StrC(type) }, sizeof( type ))

u32 gen_ring_file()
{
	Builder
	gen_ring_file;
	gen_ring_file.open( "ring.NonParsed.gen.hpp" );

	gen_ring_file.print( def_include( StrC::from("Bloat.hpp")) );
	gen_ring_file.print( def_include( StrC::from("buffer.NonParsed.gen.hpp")) );
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
