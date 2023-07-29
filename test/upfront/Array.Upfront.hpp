#pragma once



#if GEN_TIME
#include "gen.hpp"

using namespace gen;

Code gen__array_base()
{
	CodeType t_allocator_info = def_type( name(AllocatorInfo) );

	CodeStruct header = def_struct( name(ArrayHeader),
	def_struct_body( args(
		  def_variable( t_allocator_info, name(Allocator) )
		, def_variable( t_uw,             name(Capacity) )
		, def_variable( t_uw,             name(Num) )
	)));

	CodeFn grow_formula = def_function( name(array_grow_formula), def_param( t_uw, name(value)), t_uw
		, def_execution( code( return 2 * value * 8; ) )
		, def_specifiers( args( ESpecifier::Static, ESpecifier::Inline ) )
	);

	return def_global_body( args( header, grow_formula ) );
}

Code gen__array( StrC type )
{
	static CodeType t_allocator_info = def_type( name(AllocatorInfo) );
	static Code     v_nullptr        = code_str(nullptr);

	static CodeSpecifiers spec_ct_member     = def_specifiers( 2, ESpecifier::Constexpr, ESpecifier::Static );
	static CodeSpecifiers spec_static_inline = def_specifiers( 2, ESpecifier::Static, ESpecifier::Inline );
	static CodeSpecifiers spec_static        = def_specifier( ESpecifier::Static );

	static CodeUsing using_header    = def_using( name(Header), def_type( name(ArrayHeader) ) );
	static CodeVar ct_grow_formula = def_variable( t_auto, name(grow_formula), untyped_str( code( & array_grow_formula )), spec_ct_member );

	StrC name;
	{
		char const* name_str = str_fmt_buf( "Array_%s\0", type.Ptr );
		s32         name_len = str_len( name_str );

		name = { name_len, name_str };
	};

	CodeType t_array_type = def_type( name );

	CodeType t_type     = def_type( type );
	CodeType t_type_ptr = def_type( type, __, spec_ptr );
	CodeType t_type_ref = def_type( type, __, spec_ref );

	CodeType t_alias     = def_type( name(Type) );
	CodeType t_alias_ptr = def_type( name(Type), __, spec_ptr );
	CodeType t_alias_ref = def_type( name(Type), __, spec_ref );

	CodeType t_header     = def_type( name(Header) );
	CodeType t_header_ptr = def_type( name(Header), __, spec_ptr );
	CodeType t_header_ref = def_type( name(Header), __, spec_ref );

	CodeStruct array = {0};
	{
		CodeUsing using_type = def_using( name(Type), t_type );
		CodeVar   data       = def_variable( t_alias_ptr, name(Data) );

		CodeFn init = def_function( name(init), def_param( t_allocator_info, name(allocator) ), t_array_type
			, def_execution( code(
				return init_reserve( allocator, grow_formula(0) );
			))
			, spec_static
		);

		CodeFn init_reserve;
		{
			CodeParam params = def_params( args(
				  def_param( t_allocator_info, name(allocator) )
				, def_param( t_sw, name(capacity) )
			));

			Code body = def_execution( code(
				Header* header = rcast( Header*, alloc( allocator, sizeof(Header) + sizeof(Type) ));

				if ( header == nullptr )
					return { nullptr };

				header->Allocator = allocator;
				header->Capacity  = capacity;
				header->Num       = 0;

				return { rcast( Type*, header + 1) };
			));

			init_reserve = def_function( name(init_reserve), params, t_array_type, body, spec_static );
		}

		CodeFn append = def_function( name(append), def_param(t_alias, name(value)), t_bool
			, def_execution( code(
				Header* header = get_header();

				if ( header->Num == header->Capacity )
				{
					if ( ! grow( header->Capacity ))
						return false;

					header = get_header();
				}

				Data[ header->Num ] = value;
				header->Num++;

				return true;
			))
		);

		CodeFn back = def_function( name(back), __, t_alias_ref
			, def_execution( code(
				Header& header = * get_header();
				return Data[ header.Num - 1 ];
			))
		);

		CodeFn clear = def_function( name(clear), __, t_void
			, def_execution( code(
				Header& header = * get_header();
				header.Num = 0;
			))
		);

		CodeFn fill;
		{
			CodeParam params = def_params( args(
				  def_param( t_uw,    name(begin) )
				, def_param( t_uw,    name(end) )
				, def_param( t_alias, name(value) )
			));

			Code body = untyped_str( code(
				Header& header = * get_header();

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

		CodeFn free = def_function( name(free), __, t_void
			, def_execution( code(
				Header* header = get_header();
				gen::free( header->Allocator, header );
			))
		);

		CodeFn get_header = def_function( name(get_header), __, t_header_ptr
			, def_execution( code(
				return rcast( Header*, Data ) - 1;
			))
		);

		CodeFn grow = def_function( name(grow), def_param( t_uw, name(min_capacity)), t_bool
			, def_execution( code(
				Header& header = * get_header();

				uw new_capacity = grow_formula( header.Capacity );

				if ( new_capacity < min_capacity )
					new_capacity = 8;

				return set_capacity( new_capacity );
			))
		);

		CodeFn num = def_function( name(num), __, t_uw
			, def_execution( code(
				return get_header()->Num;
			))
		);

		CodeFn pop = def_function( name(pop), __, t_bool
			, def_execution( code(
				Header& header = * get_header();

				GEN_ASSERT( header.Num > 0 );
				header.Num--;
			))
		);

		CodeFn remove_at = def_function( name(remove_at), def_param( t_uw, name(idx)), t_void
			, def_execution( code(
				Header* header = get_header();
				GEN_ASSERT( idx < header->Num );

				mem_move( header + idx, header + idx + 1, sizeof( Type ) * ( header->Num - idx - 1 ) );
				header->Num--;
			))
		);

		CodeFn reserve = def_function( name(reserve), def_param( t_uw, name(new_capacity)), t_bool
			, def_execution( code(
				Header& header = * get_header();

				if ( header.Capacity < new_capacity )
					return set_capacity( new_capacity );

				return true;
			))
		);

		CodeFn resize = def_function( name(resize), def_param( t_uw, name(num)), t_bool
			, def_execution( code(
				Header* header = get_header();

				if ( num > header->Capacity )
				{
					if ( ! grow( header->Capacity ))
						return false;

					header = get_header();
				}

				header->Num = num;
				return true;
			))
		);

		CodeFn set_capacity;
		{
			Code body = def_execution( code(
				Header& header = * get_header();

				if ( new_capacity == header.Capacity )
					return true;

				if ( new_capacity < header.Num )
					header.Num = new_capacity;

				sw      size       = sizeof(Header) + sizeof(Type) * new_capacity;
				Header* new_header = rcast( Header*, alloc( header.Allocator, size ));

				if ( new_header == nullptr )
					return false;

				mem_move( new_header, & header, sizeof( Header ) + sizeof(Type) * header.Num );

				new_header->Capacity = new_capacity;

				gen::free( header.Allocator, & header );

				Data = rcast( Type*, new_header + 1);

				return true;
			));

			set_capacity = def_function( name(set_capacity), def_param( t_uw, name(new_capacity)), t_bool, body );
		}

		CodeOpCast op_ptr = def_operator_cast( t_type_ptr, def_execution( code(
			return Data;
		)));

		CodeBody body = def_struct_body( args(
			  using_header
			, using_type
			, ct_grow_formula

			, init
			, init_reserve

			, append
			, back
			, clear
			, fill
			, free
			, get_header
			, grow
			, num
			, pop
			, remove_at
			, reserve
			, resize
			, set_capacity

			, op_ptr

			, data
		));
		array = def_struct( name, body );
	}

	return array;
}


struct GenArrayRequest
{
	StrC Dependency;
	StrC Type;
};
Array<GenArrayRequest> GenArrayRequests;

void gen__array_request( StrC type, StrC dep = {} )
{
	do_once_start
		GenArrayRequests = Array<GenArrayRequest>::init( GlobalAllocator );
	do_once_end

	// Make sure we don't already have a request for the type.
	for ( sw idx = 0; idx < GenArrayRequests.num(); ++idx )
	{
		StrC const reqest_type = GenArrayRequests[ idx ].Type;

		if ( reqest_type.Len != type.Len )
			continue;

		if ( str_compare( reqest_type.Ptr, type.Ptr, reqest_type.Len ) == 0 )
			return;
	}

	GenArrayRequest request = { dep, type };
	GenArrayRequests.append( request );
}
#define gen_array( type ) gen__array_request( code(type) )

u32 gen_array_file()
{
	Builder
	gen_array_file;
	gen_array_file.open( "array.Upfront.gen.hpp" );

	CodeInclude include_gen = def_include( txt_StrC("gen.hpp") );
	gen_array_file.print( include_gen );

	gen_array_file.print( def_using_namespace( name(gen)));

	Code array_base = gen__array_base();
	gen_array_file.print( array_base );

	GenArrayRequest* current = GenArrayRequests;
	s32 left = GenArrayRequests.num();
	while (left--)
	{
		GenArrayRequest const& request = * current;

		Code generated_array = gen__array( request.Type );

		if ( request.Dependency )
		{
			char const* cmt_str = str_fmt_buf( "// Dependency for %s type", request.Type );
			s32         cmt_len = str_len( cmt_str );

			CodeComment cmt     = def_comment( { cmt_len, cmt_str } );
			CodeInclude include = def_include( request.Dependency );

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
