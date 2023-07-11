#pragma once

#if gen_time
#include "gen.hpp"

using namespace gen;

Code gen__array_base()
{
	return parse_global_body( code(
		struct ArrayHeader
		{
			AllocatorInfo Allocator;
			uw            Capacity;
			uw            Num;
		};

		static inline uw array_grow_formula( uw value )
		{
			return 2 * value * 8;
		}
	));
}

Code gen__array( StrC type, sw type_size )
{
	StrC name;
	{
		char const* name_str = str_fmt_buf( "Array_%s\0", type.Ptr );
		s32         name_len = str_len( name_str );

		name = { name_len, name_str };
	};

	Code array = parse_struct( token_fmt(
		txt(
			struct <ArrayType>
			{
				using Header = ArrayHeader;
				using Type   = <type>;

				constexpr auto grow_formula = &array_glow_formula;

				static
				<ArrayType> init( AllocatorInfo allocator )
				{
					return init_reserve( allocator, grow_formula(0) );
				}

				static
				<ArrayType> init_reserve( AllocatorInfo allocator, sw capacity )
				{
					Header* header = rcast( Header*, alloc( allocator, sizeof(Header) + sizeof(Type) ));

					if ( header == nullptr )
						return { nullptr };

					header->Allocator = allocator;
					header->Capacity  = capacity;
					header->Num       = 0;

					return { rcast( Type*, header + 1) };
				}

				bool append( Type value )
				{
					Header& header = get_header();

					if ( header.Num == header.Capacity )
					{
						if ( ! grow( header.Capacity ))
							return false;
					}

					Data[ header.Num ] = value;
					header.Num++;

					return true;
				}

				Type& back( void )
				{
					Header& header = get_header();
					return Data[ header.Num - 1 ];
				}

				void clear( void )
				{
					Header& header = get_header();
					header.Num     = 0;
				}

				bool fill( uw begin, uw end, Type value )
				{
					Header& header = get_header();

					if ( begin < 0 || end >= header.Num )
						return false;

					for ( sw idx = begin; idx < end; idx++ )
					{
						Data[ idx ] = value;
					}

					return true;
				}

				void free( void )
				{
					Header& header = get_header();
					zpl::free( header.Allocator, &header );
				}

				Header& get_header( void )
				{
					return *( reinterpret_cast< Header* >( Data ) - 1 );
				}

				bool grow( uw min_capacity )
				{
					Header& header       = get_header();
					uw      new_capacity = grow_formula( header.Capacity );

					if ( new_capacity < min_capacity )
						new_capacity = 8;

					return set_capacity( new_capacity );
				}

				uw num( void )
				{
					return get_header().Num;
				}

				bool pop( void )
				{
					Header& header = get_header();

					ZPL_ASSERT( header.Num > 0 );
					header.Num--;
				}

				void remove_at( uw idx )
				{
					Header* header = &get_header();
					ZPL_ASSERT( idx < header->Num );

					mem_move( header + idx, header + idx + 1, sizeof( Type ) * ( header->Num - idx - 1 ) );
					header->Num--;
				}

				bool reserve( uw new_capacity )
				{
					Header& header = get_header();

					if ( header.Capacity < new_capacity )
						return set_capacity( new_capacity );

					return true;
				}

				bool resize( uw num )
				{
					Header& header = get_header();

					if ( num > header.Capacity )
					{
						if ( ! grow( header.Capacity ) )
							return false;
					}

					header.Num = num;
					return true;
				}

				bool set_capacity( uw new_capacity )
				{
					Header& header = get_header();

					if ( new_capacity == header.Capacity )
						return true;

					if ( new_capacity < header.Num )
						header.Num = new_capacity;

					sw      size       = sizeof( Header ) + sizeof( Type ) * new_capacity;
					Header* new_header = reinterpret_cast< Header* >( alloc( header.Allocator, size ) );

					if ( new_header == nullptr )
						return false;

					mem_move( new_header, &header, sizeof( Header ) + sizeof( Type ) * header.Num );

					new_header->Allocator = header.Allocator;
					new_header->Num       = header.Num;
					new_header->Capacity  = new_capacity;

					zpl::free( header.Allocator, &header );

					Data = ( Type* )new_header + 1;
					return true;
				}

				Type* Data;

				operator Type*()
				{
					return Data;
				}
			};
		),
		// Tokens
			2
			, "ArrayType", name
			, "type",      type
	));

	return array;
}


struct GenArrayRequest
{
	StrC Dependency;
	StrC Type;
	sw   Size;
};
Array(GenArrayRequest) GenArrayRequests;

void gen__array_request( StrC type, sw size, StrC dep = {} )
{
	do_once_start
		array_init( GenArrayRequests, Memory::GlobalAllocator );
	do_once_end

	// Make sure we don't already have a request for the type.
	for ( sw idx = 0; idx < array_count( GenArrayRequests ); ++idx )
	{
		StrC const reqest_type = GenArrayRequests[ idx ].Type;

		if ( reqest_type.Len != type.Len )
			continue;

		if ( str_compare( reqest_type.Ptr, type.Ptr, reqest_type.Len ) == 0 )
			return;
	}

	GenArrayRequest request = { dep, type, size };
	array_append( GenArrayRequests, request );
}
#define gen_array( type ) gen__array_request( { txt_to_StrC(type) }, sizeof(type) )

u32 gen_array_file()
{
	Builder
	gen_array_file;
	gen_array_file.open( "array.gen.hpp" );

	Code include_zpl = def_include( StrC::from("Bloat.hpp") );
	gen_array_file.print( include_zpl );

	Code array_base = gen__array_base();
	gen_array_file.print( array_base );

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
