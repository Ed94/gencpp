#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_C_LIKE_CPP 1
#include "gen.cpp"
#include "helpers/push_ignores.inline.hpp"

#include <stdlib.h>

GEN_NS_BEGIN
#include "helpers/base_codegen.hpp"
#include "helpers/misc.hpp"
GEN_NS_END

using namespace gen;

constexpr char const* path_format_style = "../scripts/.clang-format";
constexpr char const* scratch_file      = "build/scratch.hpp";

Code format( Code code ) {
	return code_refactor_and_format(code, scratch_file, nullptr, path_format_style );
}

constexpr char const* generation_notice =
"// This file was generated automatially by gencpp's bootstrap.cpp "
"(See: https://github.com/Ed94/gencpp)\n\n";

int gen_main()
{
	gen::Context ctx {};
	gen::init( & ctx);

	CodeBody gen_component_header = def_global_body( args(
		def_preprocess_cond( PreprocessCond_IfDef, txt("GEN_INTELLISENSE_DIRECTIVES") ),
		pragma_once,
		def_include(txt("components/types.hpp")),
		preprocess_endif,
		fmt_newline,
		untyped_str( to_str_from_c_str(generation_notice) )
	));

	CodeBody ecode       = gen_ecode     ( "enums/ECodeTypes.csv" );
	CodeBody eoperator   = gen_eoperator ( "enums/EOperator.csv" );
	CodeBody especifier  = gen_especifier( "enums/ESpecifier.csv" );
	CodeBody etoktype    = gen_etoktype  ( "enums/ETokType.csv", "enums/AttributeTokens.csv" );
	CodeBody ast_inlines = gen_ast_inlines();

	Builder header_ecode = builder_open( "components/gen/ecodetypes.hpp" );
	builder_print( & header_ecode, gen_component_header );
	builder_print( & header_ecode, format(ecode) );
	builder_write( & header_ecode);

	Builder header_eoperator = builder_open( "components/gen/eoperator.hpp" );
	builder_print( & header_eoperator, gen_component_header );
	builder_print( & header_eoperator, format(eoperator) );
	builder_write( & header_eoperator );

	Builder header_especifier = builder_open( "components/gen/especifier.hpp" );
	builder_print( & header_especifier, gen_component_header );
	builder_print( & header_especifier, format(especifier) );
	builder_write( & header_especifier);
	
	Builder header_etoktype = builder_open( "components/gen/etoktype.hpp" );
	builder_print( & header_etoktype, gen_component_header );
	builder_print( & header_etoktype, format(etoktype) );
	builder_write( & header_etoktype);

	Builder header_ast_inlines = builder_open( "components/gen/ast_inlines.hpp" );
	builder_print( & header_ast_inlines, gen_component_header );
	builder_print( & header_ast_inlines, format(ast_inlines) );
	builder_write( & header_ast_inlines);

	gen::deinit(& ctx);
	return 0;
}
