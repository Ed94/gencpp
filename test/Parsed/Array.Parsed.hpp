#pragma once

#if gen_time
#include "gen.hpp"

using namespace gen;

Code gen__array_base()
{
	Code array_base = parse_struct( code(
		struct ArrayBase
		{
			struct Header
			{
				AllocatorInfo Allocator;
				uw            Capacity;
				uw            Num;
			};

			static inline
			sw grow_formula( sw value )
			{
				return 2 * value * 8;
			}
		};
	));

	return array_base;
}

Code gen__array( s32 length, char const* type_str, sw type_size )
{
	StrC tmpl = code(
		struct Array_{type} : ArrayBase
		{
			using Type = {type};

			Type* Data;

			static Array_{type} init( AllocatorInfo allocator )
			{
				return init_reserve( allocator, grow_formula(0) );
			}

			static Array_{type} init_reserve( AllocatorInfo allocator, uw capacity )
			{
				Header* header = rcast( Header*, alloc( allocator, sizeof(Header) + sizeof(Type) ));

				if ( header == nullptr )
					return false;

				header->Allocator = allocator;
				header->Capacity  = capacity;
				header->Num       = 0;

				Array_{type} array;
				array.Data = rcast( Type*, header + 1 );

				return array;
			}

			bool append( Type const& value )
			{
				Header& header = get_header();

				if ( header.Num == header.Capacity )
				{
					if ( ! grow( header.Allocator ))
						return false;
				}

				data[ header.Num ] = value;
				header.Num++;

				return true;
			}

			Type& back()
			{
				Header& header = get_header();
				return data[ header.Num - 1 ];
			}

			void clear()
			{
				Header& header = get_header();
				header.Num = 0;
			}

			bool fill( uw begin, uw end, Type const& value )
			{
				Header& header = get_header();

				if ( begin < 0 || end >= header.Num )
					return false;

				for ( uw idx = begin; idx < end; idx++ )
					data[ idx ] = value;

				return true;
			}

			void free()
			{
				Header& header = get_header();
				::free( header.Allocator, & header );
			}

			Header& get_header()
			{
				return pcast( Header, Data - 1 );
			}

			bool grow( uw min_capacity )
			{
				Header& header = get_header();

				uw new_capacity = grow_formula( header.Capacity );

				if ( new_capacity < min_capacity )
					new_capacity = min_capacity;

				return set_capacity( new_capacity );
			}

			void pop()
			{
				Header& header = get_header();

				assert_crash( header.Num > 0 );
				header.Num--;
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

				if ( header.Capacity < num )
					if ( ! grow( num ))
						return false;

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

				sw      size       = sizeof(Header) + sizeof(Type) * new_capacity;
				Header* new_header = rcast( Header*, alloc( header.Allocator, size ));

				if ( new_header == nullptr )
					return false;

				mem_move( new_header, & header, sizeof( Header ) + sizeof(Type) * header.Num );

				new_header->Allocator = header.Allocator;
				new_header->Num       = header.Num;
				new_header->Capacity  = new_capacity;

				::free( header );

				*Data = new_header + 1;

				return true;
			}
		}
	);

	char const* gen_from_tmpl     = token_fmt( tmpl.Ptr, 1, "type", type_str );
	s32         gen_from_tmpl_len = str_len( gen_from_tmpl );

	Code array = parse_struct( { gen_from_tmpl_len, gen_from_tmpl } );
}


struct GenArrayRequest
{
	s32         TypeLength;
	char const* Type;
	sw          Size;
	s32         DependencyLength;
	char const* Dependency;
};
Array(GenArrayRequest) GenArrayRequests;

void gen__array_request( s32 type_len, char const* type_str, sw type_size, s32 dep_len, char const* dep )
{
	GenArrayRequest request = { type_len, type_str, type_size, dep_len, dep };
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

		Code generated_array = gen__array( request.TypeLength, request.Type, request.Size );

		if ( request.Dependency )
		{
			char const* cmt_str = str_fmt_buf( "// Dependency for %s type", request.Type );
			s32         cmt_len = str_len( cmt_str );

			Code cmt     = def_comment( { cmt_len, cmt_str } );
			Code include = def_include( { request.DependencyLength, request.Dependency } );

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
