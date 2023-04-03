#pragma once

#include "Bloat.hpp"
#include "gen.hpp"

#ifdef gen_time
	using namespace gen;

	char* sprintf_buf[ZPL_PRINTF_MAXLEN];

	/*
		What it should generate:
		
		inline
		type square( type value )
		{
			return value * value;
		}
	*/
	#define gen_square( Type_ ) gen__square( #Type_ )
	Code gen__square( char const* type )
	{
		Code integral_type = def_type( type );

	#ifndef GEN_DEFINE_DSL
		string name = string_sprintf( g_allocator, (char*)sprintf_buf, ZPL_PRINTF_MAXLEN, "square", type );

		Code square;
		{
			Code params     = def_parameters( 1, integral_type, "value" );
			Code specifiers = def_specifiers( 1, Specifier::Inline );
			Code ret_stmt   = untyped_fmt( "return value * value;" );

			square = def_function( name, specifiers, params, integral_type, ret_stmt );
		}

	#else 
		def( square )
		function( square, __, integral_type, params( integral_type, "value" ), untyped_str("return value * value") );
	#endif
	
		if ( ! square )
			fatal( "Failed to generate square function for: %s", type );

		return square;
	}

	u32 gen_math()
	{
		Code fadd_u8  = gen_square( u8 );
		Code fadd_u16 = gen_square( u16 );
		Code fadd_u32 = gen_square( u32 );
		Code fadd_u64 = gen_square( u64 );

		Builder 
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
#	include "math.gen.hpp"
#	undef square

#endif
