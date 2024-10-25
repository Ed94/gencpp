#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#define GEN_EXPOSE_BACKEDN
#include "gen.cpp"

#include "helpers/push_ignores.inline.hpp"
#include "helpers/helper.hpp"

GEN_NS_BEGIN
#include "dependencies/parsing.cpp"
GEN_NS_END

#include "auxillary/builder.hpp"
#include "auxillary/builder.cpp"
#include "auxillary/scanner.hpp"

using namespace gen;

constexpr char const* generation_notice =
"// This file was generated automatially by gencpp's unreal.cpp"
"(See: https://github.com/Ed94/gencpp)\n\n";

constexpr StrC implementation_guard_start = txt(R"(
#pragma region GENCPP IMPLEMENTATION GUARD
#if defined(GEN_IMPLEMENTATION) && ! defined(GEN_IMPLEMENTED)
#	define GEN_IMPLEMENTED
)");

constexpr StrC implementation_guard_end = txt(R"(
#endif
#pragma endregion GENCPP IMPLEMENTATION GUARD
)");

constexpr StrC roll_own_dependencies_guard_start = txt(R"(
//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
// Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES

)");

constexpr StrC roll_own_dependencies_guard_end = txt(R"(
// GEN_ROLL_OWN_DEPENDENCIES
#endif
)");

global bool generate_gen_dep = true;
global bool generate_builder = true;
global bool generate_editor  = true;
global bool generate_scanner = true;

int gen_main()
{
#define project_dir "../project/"
	gen::init();

	Code push_ignores        = scan_file( project_dir "helpers/push_ignores.inline.hpp" );
	Code pop_ignores         = scan_file( project_dir "helpers/pop_ignores.inline.hpp" );

	Code ue_forceinline = code_str(FORCEINLINE);
	// Code

	// gen_dep.hpp
	{
		CodeBody header_start = def_body( CodeT::Global_Body );
		{
			FileContents content          = file_read_contents( GlobalAllocator, true, project_dir "dependencies/header_start.hpp" );
			CodeBody     ori_header_start = parse_global_body( StrC { content.size, (char const*)content.data });

			for (Code	code =  ori_header_start.begin();
						code != ori_header_start.end();
						++ code )
			{
				header_start.append(code);
				if (code->Type == CodeT::Preprocess_Pragma && code->Content.starts_with(txt("once")))
				{
					header_start.append( fmt_newline );
					header_start.append( push_ignores );
				}
			}
		}

		CodeBody macros = def_body( CodeT::Global_Body );
		{
			FileContents content    = file_read_contents( GlobalAllocator, true, project_dir "dependencies/macros.hpp" );
			CodeBody     ori_macros = parse_global_body( StrC { content.size, (char const*)content.data });

			for (Code	code =  ori_macros.begin();
						code != ori_macros.end();
						++ code )
			{
				switch (code->Type)
				{
					using namespace ECode;
					case Preprocess_Define:
					{
						CodeDefine define = code.cast<CodeDefine>();
						if ( define->Name.starts_with(txt("global")) )
						{
							macros.append(parse_global_body(txt("#define global // Global variables")));
							continue;
						}

						macros.append(define);
					}
					break;

					case Preprocess_Pragma:
					{
						macros.append(code);
						continue;

						local_persist bool found = false;
						if (found)
						{
							macros.append(code);
							continue;
						}

						if (code->Content.starts_with(txt("region ForceInline Definition")))
						{
							macros.append(code);
							++ code;

							CodeBody replacement = parse_global_body(StrC(txt(
R"(#ifdef GEN_COMPILER_MSVC
	#define FORCEINLINE __forceinline
	#define neverinline __declspec( noinline )
#elif defined( GEN_COMPILER_GCC )
	#define FORCEINLINE inline __attribute__( ( __always_inline__ ) )
	#define neverinline __attribute__( ( __noinline__ ) )
#elif defined( GEN_COMPILER_CLANG )
	#if __has_attribute( __always_inline__ )
		#define FORCEINLINE inline __attribute__( ( __always_inline__ ) )
		#define neverinline __attribute__( ( __noinline__ ) )
	#else
		#define FORCEINLINE
		#define neverinline
	#endif
#else
	#define FORCEINLINE
	#define neverinline
#endif)")));
							macros.append(replacement);

							while (code->Type != ECode::Preprocess_Pragma
								|| ! code->Content.starts_with(txt("endregion ForceInline Definition")))
								++ code;

							macros.append( code );
							found = true;
						}
					}
					break;

					default:
						macros.append(code);
					break;
				}
			}
		}

		Code basic_types  = scan_file( project_dir "dependencies/basic_types.hpp" );
		Code debug        = scan_file( project_dir "dependencies/debug.hpp" );
		Code memory	      = scan_file( project_dir "dependencies/memory.hpp" );
		Code string_ops   = scan_file( project_dir "dependencies/string_ops.hpp" );
		Code printing     = scan_file( project_dir "dependencies/printing.hpp" );
		Code containers   = scan_file( project_dir "dependencies/containers.hpp" );
		Code hashing 	  = scan_file( project_dir "dependencies/hashing.hpp" );
		Code strings      = scan_file( project_dir "dependencies/strings.hpp" );
		Code filesystem   = scan_file( project_dir "dependencies/filesystem.hpp" );
		Code timing       = scan_file( project_dir "dependencies/timing.hpp" );

		Builder
		header = Builder::open("gen/gen.dep.hpp");
		header.print_fmt( generation_notice );
		header.print( header_start );
		header.print_fmt( "\nGEN_NS_BEGIN\n" );

		header.print( macros );
		header.print( basic_types );
		header.print( debug );
		header.print( memory );
		header.print( string_ops );
		header.print( printing );
		header.print( containers );
		header.print( hashing );
		header.print( strings );
		header.print( filesystem );
		header.print( timing );

		header.print_fmt( "\nGEN_NS_END\n" );
		header.print( fmt_newline );
		header.print( pop_ignores );
		header.write();
	}
}
