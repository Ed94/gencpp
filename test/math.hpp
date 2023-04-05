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

		#if 0
		Code square;
		{
			Code params     = def_params( 1, integral_type, "value" );
			Code specifiers = def_specifiers( 1, SpecifierT::Inline );
			Code ret_stmt   = untyped_str( txt( return value * value; ));

			square = def_proc( name, specifiers, params, integral_type, ret_stmt );
		}

		#else
		// Test of token template str.
		char const* tmpl = txt(
			{type} square( {type} value )
			{
				return value * value;
			}
		);
		char const* gen_code = token_fmt( tmpl, 1, "type", type );

		Code square = parse_proc(gen_code, strlen(gen_code));
		#endif

	#else 

		Code proc( square, __, integral_type, params( integral_type, "value" ), 
			untyped(return value * value) 
		);
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
