#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEND
#include "gen.cpp"
#include "filesystem/gen.scanner.hpp"
#include "helpers/gen.helper.hpp"

using namespace gen;

bool namespace_by_default = true;

constexpr StrC nspace_default = txt_StrC(R"(
#if defined(GEN_DONT_USE_NAMESPACE) && ! defined(GEN_NS_BEGIN)
#	define GEN_NS_BEGIN
#	define GEN_NS_END
#elif ! defined(GEN_NS_BEGIN)
#  define GEN_NS_BEGIN namespace gen {
#  define GEN_NS_END   }
#endif
)");

constexpr StrC nspace_non_default = txt_StrC(R"(
#if ! defined(GEN_USE_NAMESPACE) && ! defined(GEN_NS_BEGIN)
#	define GEN_NS_BEGIN
#	define GEN_NS_END
#elif ! defined(GEN_NS_BEGIN)
#  define GEN_NS_BEGIN namespace gen {
#  define GEN_NS_END   }
#endif
)");

constexpr StrC implementation_guard_start = txt_StrC(R"(
#pragma region GENCPP IMPLEMENTATION GUARD
#if defined(GEN_IMPLEMENTATION) && ! defined(GEN_IMPLEMENTED)
#	define GEN_IMPLEMENTED
)");

constexpr StrC implementation_guard_end = txt_StrC(R"(
#endif
#pragma endregion GENCPP IMPLEMENTATION GUARD
)");

constexpr StrC roll_own_dependencies_guard_start = txt_StrC(R"(
//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
// Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES

)");

constexpr StrC roll_own_dependencies_guard_end = txt_StrC(R"(
// GEN_ROLL_OWN_DEPENDENCIES
#endif

)");

int gen_main()
{
	gen::init();

#define project_dir "../project/"

	Code push_ignores = scan_file( project_dir "helpers/gen.push_ignores.inline.hpp" );
	Code pop_ignores  = scan_file( project_dir "helpers/gen.pop_ignores.inline.hpp" );

	Code header_start = scan_file( "components/gen.header_start.hpp" );
	Code nspace_macro = untyped_str( namespace_by_default ? nspace_default : nspace_non_default );

	Builder
	header;
	header.open( "gen/gen.hpp" );
	header.print( push_ignores );

	header.print_fmt("#pragma once\n\n");

	// Headers
	{
		header.print( header_start );
		header.print( nspace_macro );

		{
			header.print_fmt( roll_own_dependencies_guard_start );

			Code header_start  = scan_file( project_dir "dependencies/gen.header_start.hpp" );
			Code macros        = scan_file( project_dir "dependencies/gen.macros.hpp" );
			Code basic_types   = scan_file( project_dir "dependencies/gen.basic_types.hpp" );
			Code debug         = scan_file( project_dir "dependencies/gen.debug.hpp" );
			Code memory	       = scan_file( project_dir "dependencies/gen.memory.hpp" );
			Code string_ops    = scan_file( project_dir "dependencies/gen.string_ops.hpp" );
			Code printing      = scan_file( project_dir "dependencies/gen.printing.hpp" );
			Code containers    = scan_file( project_dir "dependencies/gen.containers.hpp" );
			Code hashing 	   = scan_file( project_dir "dependencies/gen.hashing.hpp" );
			Code string        = scan_file( project_dir "dependencies/gen.string.hpp" );
			Code file_handling = scan_file( project_dir "dependencies/gen.file_handling.hpp" );
			Code parsing       = scan_file( project_dir "dependencies/gen.parsing.hpp" );
			Code timing        = scan_file( project_dir "dependencies/gen.timing.hpp" );

			header.print( header_start );
			header.print_fmt( "GEN_NS_BEGIN\n\n" );
			header.print( macros );
			header.print( basic_types );
			header.print( debug );
			header.print( memory );
			header.print( string_ops );
			header.print( printing );
			header.print( containers );
			header.print( hashing );
			header.print( string );
			header.print( file_handling );
			header.print( parsing );
			header.print( timing );
			header.print_fmt( "GEN_NS_END\n" );

			header.print_fmt( roll_own_dependencies_guard_end );
		}

		Code types        = scan_file( project_dir "components/gen.types.hpp" );
		Code data_structs = scan_file( project_dir "components/gen.data_structures.hpp" );
		Code interface    = scan_file( project_dir "components/gen.interface.hpp" );
		Code header_end   = scan_file( project_dir "components/gen.header_end.hpp" );

		CodeBody ecode      = gen_ecode( project_dir "components/ECode.csv" );
		CodeBody eoperator  = gen_eoperator( project_dir "components/EOperator.csv" );
		CodeBody especifier = gen_especifier( project_dir "components/ESpecifier.csv" );

		Code builder = scan_file( project_dir "filesystem/gen.builder.hpp" );

		header.print_fmt( "GEN_NS_BEGIN\n\n" );

		header.print_fmt("#pragma region Types");
		header.print( types );
		header.print( ecode );
		header.print( eoperator );
		header.print( especifier );
		header.print_fmt("#pragma endregion Types");

		header.print( data_structs );
		header.print( interface );
		header.print( header_end );
		header.print( builder );
		header.print_fmt( "GEN_NS_END\n" );
	}

	// Implementation
	{
		header.print_fmt( "%s\n", (char const*) implementation_guard_start );
		{
			header.print_fmt( roll_own_dependencies_guard_start );

			Code impl_start = scan_file( project_dir "dependencies/gen.impl_start.cpp" );
			Code debug      = scan_file( project_dir "dependencies/gen.debug.cpp" );
			Code string_ops = scan_file( project_dir "dependencies/gen.string_ops.cpp" );
			Code printing   = scan_file( project_dir "dependencies/gen.printing.cpp" );
			Code memory     = scan_file( project_dir "dependencies/gen.memory.cpp" );
			Code parsing    = scan_file( project_dir "dependencies/gen.parsing.cpp" );
			Code hashing    = scan_file( project_dir "dependencies/gen.hashing.cpp" );
			Code string     = scan_file( project_dir "dependencies/gen.string.cpp" );
			Code timing     = scan_file( project_dir "dependencies/gen.timing.cpp" );

			Code file_handling = scan_file( project_dir "dependencies/gen.file_handling.cpp" );

			header.print_fmt( "GEN_NS_BEGIN\n\n");
			header.print( impl_start );
			header.print( debug );
			header.print( string_ops );
			header.print( printing );
			header.print( memory );
			header.print( parsing );
			header.print( hashing );
			header.print( string );
			header.print( timing );

			header.print( file_handling );

			header.print_fmt( "GEN_NS_END\n");

			header.print_fmt( roll_own_dependencies_guard_end );
		}

		Code data 	         = scan_file( project_dir "components/gen.data.cpp" );
		Code ast_case_macros = scan_file( project_dir "components/gen.ast_case_macros.cpp" );
		Code ast             = scan_file( project_dir "components/gen.ast.cpp" );
		Code interface       = scan_file( project_dir "components/gen.interface.cpp" );
		Code upfront         = scan_file( project_dir "components/gen.interface.upfront.cpp" );
		Code parsing         = scan_file( project_dir "components/gen.interface.parsing.cpp" );
		Code untyped         = scan_file( project_dir "components/gen.untyped.cpp" );

		Code builder = scan_file( project_dir "filesystem/gen.builder.cpp" );

		header.print_fmt( "GEN_NS_BEGIN\n\n");
		header.print( data );
		header.print( ast_case_macros );
		header.print( ast );
		header.print( interface );
		header.print( upfront );
		header.print( parsing );
		header.print( untyped );
			header.print( builder );
		header.print_fmt( "GEN_NS_END\n");

		header.print_fmt( "%s\n", (char const*) implementation_guard_end );
	}

	header.print( pop_ignores );
	header.write();

	gen::deinit();
	return 0;
}
