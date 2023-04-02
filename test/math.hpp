#pragma once

#ifdef gen_time
	#include "Bloat.hpp"
	#include "gen.hpp"

	using namespace gen;

	/*
		What it should generate:
		
		inline
		type square_#type( type value )
		{
			return value * value;
		}
	*/
	#define gen_square( Type_ ) gen__squre( #Type_ )
	Code gen__squre( char const* type )
	{
		Code integral_type = make_type( type );

		string name       = string_sprintf_buf( g_allocator, "square_%s", type );
		Code   specifiers = make_specifiers( 1, Specifier::Inline );
		Code   params     = make_parameters( 1, "value", integral_type );
		Code   ret_stmt   = make_fmt( "return value * value" );

		Code result = make_function( name, 
			specifiers,
			params,
			integral_type, 
			ret_stmt 
		);

		if ( ! result )
			fatal( "Failed to generate square function for: %s", type );

		return result;
	}

	u32 gen_math()
	{
		Code fadd_u8  = gen_square( u8 );
		Code fadd_u16 = gen_square( u16 );
		Code fadd_u32 = gen_square( u32 );
		Code fadd_u64 = gen_square( u64 );

		File 
		mathgen;
		mathgen.open( "math.gen.hpp" );
		mathgen.print( fadd_u8 );
		mathgen.print( fadd_u16 );
		mathgen.print( fadd_u32 );
		mathgen.print( fadd_u64 );
		mathgen.write();
		return 0;
	}
#endif

#ifndef gen_time
	#include "math.gen.hpp"
	#undef square

	#define sym_square( Type_, Value_ ) square_#Type_( Value_ )

	template<class type>
	type square( type value )
	[
		sym_square( type, value );
	]
#endif
