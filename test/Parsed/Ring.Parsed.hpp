#pragma once

#if gen_time
#include "gen.hpp"
#include "Buffer.Parsed.hpp"

using namespace gen;

Code gen__ring( StrC type )
{
	static Code t_allocator_info = def_type( name(AllocatorInfo) );

	String name;
	{
		char const* name_str = str_fmt_buf( "Ring_%s\0", type.Ptr );
		s32         name_len = str_len( name_str );

		name = get_cached_string({ name_len, name_str });
	};

	Code ring = parse_struct( token_fmt(
		txt(
			struct <RingName>
			{
				using Type = <type>;

				static <RingName> init( AllocatorInfo allocator, uw max_size )
				{
					<RingName> result = { 0 };

					result.Backing  = allocator;
					result.Buffer   = <BufferName>::init( allocator, max_size + 1 );

					if ( result.Buffer == nullptr )
						return { nullptr };

					result.Capacity = max_size + 1;
					return result;
				}

				void append( s16 value )
				{
					Buffer[ Head ] = value;
					Head           = ( Head + 1 ) % Capacity;
					if ( Head == Tail )
						Tail = ( Tail + 1 ) % Capacity;
				}

				inline void append( Type* values, sw num )
				{
					for ( sw idx = 0; idx < num; idx++ )
						append( values[ idx ] );
				}

				bool empty( void )
				{
					return Head == Tail;
				}

				void free( void )
				{
					Buffer.free();
				}

				bool full( void )
				{
					return ( Head + 1 ) % Capacity == Tail;
				}

				Type& get( void )
				{
					Type& data = Buffer[ Tail ];
					Tail       = ( Tail + 1 ) % Capacity;
					return data;
				}

				void wipe( void )
				{
					Head = 0;
					Tail = 0;
					Buffer.wipe();
				}

				AllocatorInfo Backing;
				uw            Capacity;
				uw            Head;
				uw            Tail;
				<BufferName>  Buffer;
			};
		),
		3
		, "RingName",   (char const*) name
		, "type",       (char const*) type
		, "BufferName", str_fmt_buf( "Buffer_%s", type.Ptr )
	));

	return ring;
}

struct GenRingRequest
{
	StrC Dependency;
	StrC Type;
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
	gen__buffer_request( type, dep );

	GenRingRequest request = { dep, type };
	array_append( GenRingRequests, request );
}
#define gen_ring( type ) gen__ring_request( { txt_to_StrC(type) }, sizeof( type ))

u32 gen_ring_file()
{
	Builder
	gen_ring_file;
	gen_ring_file.open( "ring.Parsed.gen.hpp" );

	gen_ring_file.print( def_include( StrC::from("Bloat.hpp")) );
	gen_ring_file.print( def_include( StrC::from("buffer.Parsed.gen.hpp")) );
	// gen_ring_file.print( gen__ring_base() );

	GenRingRequest* current = GenRingRequests;
	s32 left = array_count( GenRingRequests );
	while (left--)
	{
		GenRingRequest const& request = * current;

		Code generated_ring = gen__ring( current->Type );

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
