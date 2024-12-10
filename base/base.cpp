#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#define GEN_C_LIKE_CPP 1
#include "../project/gen.cpp"

#include "helpers/push_ignores.inline.hpp"
#include "helpers/helper.hpp"

GEN_NS_BEGIN
#include "helpers/push_container_defines.inline.hpp"
#include "dependencies/parsing.cpp"
#include "helpers/pop_container_defines.inline.hpp"
GEN_NS_END

#include "auxillary/builder.hpp"
#include "auxillary/builder.cpp"
#include "auxillary/scanner.hpp"
#include "auxillary/misc.hpp"

using namespace gen;

constexpr char const* path_format_style = "../scripts/.clang-format";
constexpr char const* scratch_file      = "gen/scratch.hpp";

Code format( Code code ) {
	return code_refactor_and_format(code, scratch_file, nullptr, path_format_style );
}

int gen_main()
{
	CodeBody ecode       = gen_ecode     ( "enums/ECodeTypes.csv" );
	CodeBody eoperator   = gen_eoperator ( "enums/EOperator.csv" );
	CodeBody especifier  = gen_especifier( "enums/ESpecifier.csv" );
	CodeBody ast_inlines = gen_ast_inlines();

	Builder header_ecode = builder_open( "components/gen/ecode.hpp" );
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

	Builder header_ast_inlines = builder_open( "components/gen/ast_inlines.hpp" );
	builder_print( & header_ast_inlines, gen_component_header );
	builder_print( & header_ast_inlines, format(ast_inlines) );
	builder_write( & header_ast_inlines);
}