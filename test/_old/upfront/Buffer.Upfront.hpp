#pragma once

#if GEN_TIME
#include "gen.hpp"

using namespace gen;

Code gen__buffer_base()
{
	CodeType t_allocator_info = def_type( name(AllocatorInfo) );

	Code header = def_struct( name(BufferHeader),
	def_struct_body( args(
		  def_variable( t_allocator_info, name(Backing) )
		, def_variable( t_uw,             name(Capacity) )
		, def_variable( t_uw,             name(Num) )
	)));

	return def_global_body( 1, header );
}

Code gen__buffer( StrC type, ssize type_size )
{
	static CodeType t_allocator_info = def_type( name(AllocatorInfo));

	static CodeUsing using_header = def_using( name(Header), def_type( name(BufferHeader) ) );

	StrC name;
	{
		char const* name_str = str_fmt_buf( "Buffer_%s\0", type.Ptr );
		s32         name_len = str_len( name_str );

		name = { name_len, name_str };
	};

	CodeType t_buffer_type = def_type( name );

	CodeType t_type        = def_type( type );
	CodeType t_type_ptr    = def_type( type, __, spec_ptr );
	CodeType t_type_ref    = def_type( type, __, spec_ref );

	CodeType t_header      = def_type( name(Header) );
	CodeType t_header_ptr  = def_type( name(Header), __, spec_ptr );
	CodeType t_header_ref  = def_type( name(Header), __, spec_ref );

	CodeStruct buffer = {0};
	{
		CodeUsing using_type = def_using( name(Type), t_type );
		CodeVar   data       = def_variable( t_type_ptr, name(Data) );

		CodeFn init;
		{
			CodeParam params = def_params( args(
				  def_param( t_allocator_info, name(allocator))
				, def_param( t_sw,             name(capacity))
			));

			Code body = def_execution( code(
				Header* header = rcast( Header*, alloc( allocator, sizeof(Header) + capacity * sizeof(Type) ) );

				if ( header == nullptr )
					return { nullptr };

				header->Backing  = allocator;
				header->Capacity = capacity;
				header->Num      = 0;

				return { rcast( Type*, header + 1) };
			));

			init = def_function( name(init), params, t_buffer_type, body, spec_static_member );
		}

		CodeFn init_copy;
		{
			CodeParam params = def_params( args(
				  def_param( t_allocator_info, name(allocator))
				, def_param( t_buffer_type,    name(other))
			));

			init_copy = def_function( name(init), params, t_buffer_type
				, def_execution( code(
					Header& other_header = other.get_header();
					Header* header = rcast( Header*, alloc( allocator, sizeof(Header) + other_header.Capacity * sizeof(Type) ) );

					if ( header == nullptr )
						return { nullptr };

					header->Backing  = allocator;
					header->Capacity = other_header.Capacity;
					header->Num      = other_header.Num;

					mem_copy( header + 1, other.Data, other_header.Num * sizeof(Type) );
					return { rcast( Type*, header + 1) };
				))
			);
		}

		CodeFn append = def_function( name(append), def_param( t_type, name(value)), t_void
			, def_execution( code(
				Header& header = get_header();
				Data[ header.Num ] = value;
				header.Num++;
			))
		);

		CodeFn appendv;
		{
			CodeParam params = def_params( args(
				  def_param( t_type_ptr, name( values))
				, def_param( t_sw, 	 name( num))
			));

			appendv = def_function( name(append), params, t_void
				, def_execution( code(
					Header& header = get_header();

					GEN_ASSERT( header.Num + num <= header.Capacity);

					mem_copy( Data + header.Num, values, num * sizeof( Type ) );

					header.Num += num;
				))
			);
		}

		CodeFn clear = def_function( name(clear), __, t_void
			, def_execution( code(
				Header& header = get_header();
				header.Num = 0;
			))
		);

		CodeFn end = def_function( name(end), __, t_type_ref
			, def_execution( code(
				Header& header = get_header();
				return Data[ header.Num - 1 ];
			))
		);

		CodeFn free = def_function( name(free), __, t_void
			, def_execution( code(
				Header& header = get_header();
				gen::free( header.Backing, & header );
			))
		);

		CodeFn get_header = def_function( name(get_header), __, t_header_ref
			, def_execution( code(
				return * ( rcast( Header*, Data ) - 1 );
			))
		);

		CodeFn num = def_function( name(num), __, t_sw
			, def_execution( code(
				return get_header().Num;
			))
		);

		CodeFn pop = def_function( name(pop), __, t_type
			, def_execution( code(
				Header& header = get_header();
				header.Num--;
				return Data[ header.Num ];
			))
		);

		CodeFn wipe = def_function( name(wipe), __, t_void
			, def_execution( code(
				Header& header = get_header();
				header.Num = 0;
				mem_set( Data, 0, header.Capacity * sizeof( Type ) );
			))
		);

		CodeOpCast op_type_ptr = def_operator_cast( t_type_ptr, def_execution( code(
			return Data;
		)));

		buffer = def_struct( name, def_struct_body( args(
			  using_header
			, using_type

			, init
			, init_copy
			, append
			, appendv
			, clear
			, end
			, free
			, get_header
			, num
			, pop
			, wipe

			, op_type_ptr

			, data
		)));
	}

	return buffer;
}

struct GenBufferRequest
{
	StrC Dependency;
	StrC Type;
	ssize   TypeSize;
};
Array<GenBufferRequest> GenBufferRequests;

void gen__buffer_request( StrC type, StrC dep = {} )
{
	do_once_start
		GenBufferRequests = Array<GenBufferRequest>::init( GlobalAllocator );
	do_once_end

	// Make sure we don't already have a request for the type.
	for ( ssize idx = 0; idx < GenBufferRequests.num(); ++idx )
	{
		StrC const reqest_type = GenBufferRequests[ idx ].Type;

		if ( reqest_type.Len != type.Len )
			continue;

		if ( str_compare( reqest_type.Ptr, type.Ptr, reqest_type.Len ) == 0 )
			return;
	}

	GenBufferRequest request = { dep, type};
	GenBufferRequests.append( request );
}
#define gen_buffer( type ) gen__buffer_request( code(type))

u32 gen_buffer_file()
{
	Builder
	gen_buffer_file;
	gen_buffer_file.open( "buffer.Upfront.gen.hpp" );

	gen_buffer_file.print( def_include( txt("gen.hpp")) );
	gen_buffer_file.print( def_using_namespace( name(gen)) );

	gen_buffer_file.print( gen__buffer_base() );

	GenBufferRequest* current = GenBufferRequests;
	s32 left = GenBufferRequests.num();
	while (left--)
	{
		GenBufferRequest const& request = * current;

		Code generated_buffer = gen__buffer( current->Type, current->TypeSize );

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

#endif // GEN_TIME
