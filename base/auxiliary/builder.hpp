#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "helpers/push_ignores.inline.hpp"
#	include "components/header_start.hpp"
#	include "components/types.hpp"
#	include "components/gen/ecodetypes.hpp"
#	include "components/gen/eoperator.hpp"
#	include "components/gen/especifier.hpp"
#	include "components/ast.hpp"
#	include "components/code_types.hpp"
#	include "components/ast_types.hpp"
#	include "components/interface.hpp"
#	include "components/inlines.hpp"
#	include "components/gen/ast_inlines.hpp"
#	include "components/header_end.hpp"
using namespace gen;
#endif

#pragma region Builder

struct Builder;
typedef struct Builder Builder;

GEN_API Builder builder_open        ( char const* path );
GEN_API void    builder_pad_lines   ( Builder* builder, s32 num );
GEN_API void    builder_print       ( Builder* builder, Code code );
GEN_API void    builder_print_fmt_va( Builder* builder, char const* fmt, va_list va );
GEN_API void    builder_write       ( Builder* builder );

forceinline void builder_print_fmt   ( Builder* builder, char const* fmt, ... ) {
	va_list va;
	va_start( va, fmt );
	builder_print_fmt_va( builder, fmt, va );
	va_end( va );
}

struct Builder
{
	FileInfo File;
	StrBuilder   Buffer;

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
forceinline void    builder_pad_lines( Builder& builder, s32 num )    { return builder_pad_lines(& builder, num); }
forceinline void    builder_print    ( Builder& builder, Code code )  { return builder_print(& builder, code); }
forceinline void    builder_write    ( Builder& builder )             { return builder_write(& builder ); }
forceinline void    builder_print_fmt( Builder& builder, char const* fmt, ...) {
	va_list va;
	va_start( va, fmt );
	builder_print_fmt_va( & builder, fmt, va );
	va_end( va );
}
#endif

#pragma endregion Builder
