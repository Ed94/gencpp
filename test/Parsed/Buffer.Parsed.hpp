#pragma once

#if gen_time
#include "gen.hpp"

using namespace gen;

Code gen__buffer_base()
{
	return parse_global_body( code(
		struct BufferHeader
		{
			AllocatorInfo Backing;
			uw            Capacity;
			uw            Num;
		};
	));
}

Code gen__buffer( StrC type )
{
	StrC name;
	{
		char const* name_str = str_fmt_buf( "Buffer_%s\0", type.Ptr );
		s32         name_len = str_len( name_str );

		name = { name_len, name_str };
	};

	Code buffer = parse_struct( token_fmt( 
		txt(
			struct <BufferName>
			{
				using Header = BufferHeader;
				using Type   = <type>;

				static <BufferName> init( AllocatorInfo allocator, sw capacity )
				{
					Header* header = rcast( Header*, alloc( allocator, sizeof( Header ) + capacity * sizeof( Type ) ) );

					if ( header == nullptr )
						return { nullptr };

					header->Backing  = allocator;
					header->Capacity = capacity;
					header->Num      = 0;

					return { rcast( Type*, header + 1 ) };
				}

				<BufferName> init( AllocatorInfo allocator, <BufferName> other )
				{
					Header& other_header = other.get_header();
					Header* header       = rcast( Header*, alloc( allocator, sizeof( Header ) + other_header.Capacity * sizeof( Type ) ) );

					if ( header == nullptr )
						return { nullptr };

					header->Backing  = allocator;
					header->Capacity = other_header.Capacity;
					header->Num      = other_header.Num;

					mem_copy( header + 1, other.Data, other_header.Num * sizeof( Type ) );

					return { rcast( Type*, header + 1 ) };
				}

				void append( Type value )
				{
					Header& header     = get_header();
					Data[ header.Num ] = value;
					header.Num++;
				}

				void append( Type* values, sw num )
				{
					Header& header = get_header();
					ZPL_ASSERT( header.Num + num <= header.Capacity);

					mem_copy( Data + header.Num, values, num * sizeof( Type ) );
					header.Num += num;
				}

				void clear( void )
				{
					Header& header = get_header();
					header.Num     = 0;
				}

				Type& end( void )
				{
					Header& header = get_header();
					return Data[ header.Num - 1 ];
				}

				void free( void )
				{
					Header& header = get_header();
					zpl::free( header.Backing, &header );
				}

				Header& get_header( void )
				{
					return *( rcast( Header*, Data ) - 1 );
				}

				sw num( void )
				{
					return get_header().Num;
				}

				void wipe( void )
				{
					Header& header = get_header();
					header.Num     = 0;
					mem_set( Data, 0, header.Capacity * sizeof( Type ) );
				}

				operator Type*()
				{
					return Data;
				}

				Type* Data;
			};
		),
		2
		, "BufferName", (char const*) name
		, "type", 	    (char const*) type
	));

	return buffer;
}

struct GenBufferRequest
{
	StrC Dependency;
	StrC Type;
};
Array<GenBufferRequest> GenBufferRequests;

void gen__buffer_request( StrC type, StrC dep = {} )
{
	do_once_start
		GenBufferRequests = Array<GenBufferRequest>::init( Memory::GlobalAllocator );
	do_once_end

	// Make sure we don't already have a request for the type.
	for ( sw idx = 0; idx < GenBufferRequests.num(); ++idx )
	{
		StrC const reqest_type = GenBufferRequests[ idx ].Type;

		if ( reqest_type.Len != type.Len )
			continue;

		if ( str_compare( reqest_type.Ptr, type.Ptr, reqest_type.Len ) == 0 )
			return;
	}

	GenBufferRequest request = { dep, type };
	GenBufferRequests.append( request );
}
#define gen_buffer( type ) gen__buffer_request( { txt_to_StrC(type) } )

u32 gen_buffer_file()
{
	Builder
	gen_buffer_file;
	gen_buffer_file.open( "buffer.Parsed.gen.hpp" );

	gen_buffer_file.print( def_include( StrC::from("Bloat.hpp")) );
	gen_buffer_file.print( gen__buffer_base() );

	GenBufferRequest* current = GenBufferRequests;
	s32 left = GenBufferRequests.num();
	while (left--)
	{
		GenBufferRequest const& request = * current;

		Code generated_buffer = gen__buffer( current->Type );

		if ( request.Dependency )
		{
			char const* cmt_str = str_fmt_buf( "// Dependency for %s type", request.Type );
			s32         cmt_len = str_len( cmt_str );

			Code cmt     = def_comment( { cmt_len, cmt_str } );
			Code include = def_include( request.Dependency );

			gen_buffer_file.print( cmt );
			gen_buffer_file.print( include );
		}

		gen_buffer_file.print( generated_buffer );
		current++;
	}

	gen_buffer_file.write();
	return 0;
}

#endif // gen_time
