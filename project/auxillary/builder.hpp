#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "gen.hpp"
#endif

struct Builder;
typedef struct Builder Builder;

Builder builder_open        ( char const* path );
void    builder_pad_lines   ( Builder* builder, s32 num );
void    builder_print       ( Builder* builder, Code code );
void    builder_print_fmt_va( Builder* builder, char const* fmt, va_list va );
void    builder_print_fmt   ( Builder* builder, char const* fmt, ... ) {
	va_list va;
	va_start( va, fmt );
	builder_print_fmt_va( builder, fmt, va );
	va_end( va );
}
void builder_write( Builder* builder );

struct Builder
{
	FileInfo File;
	String   Buffer;

#if GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP
	forceinline static Builder open( char const* path ) { return builder_open(path); }

	forceinline void pad_lines( s32 num ) { return builder_pad_lines(this, num); }

	forceinline void print( Code code )                { return builder_print(this, code); }
	forceinline void print_fmt( char const* fmt, ... ) {
		va_list va;
		va_start( va, fmt );
		builder_print_fmt_va( this, fmt, va );
		va_end( va );
	}

	forceinline void write() { return builder_write(this); }
#endif
};

#if GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP
void    builder_pad_lines( Builder& builder, s32 num )    { return builder_pad_lines(& builder, num); }
void    builder_print    ( Builder& builder, Code code )  { return builder_print(& builder, code); }
void    builder_write    ( Builder& builder )             { return builder_write(& builder ); }
void    builder_print_fmt( Builder& builder, char const* fmt, ...) {
	va_list va;
	va_start( va, fmt );
	builder_print_fmt_va( & builder, fmt, va );
	va_end( va );
}
#endif
