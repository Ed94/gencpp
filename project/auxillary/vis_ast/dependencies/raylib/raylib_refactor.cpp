#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_BENCHMARK
#define GEN_ENFORCE_STRONG_CODE_TYPES
// #define GEN_IMPLEMENTATION
#include "gen.cpp"
#include "gen.builder.cpp"


constexpr char const* path_config    = "config.h";
constexpr char const* path_raylib    = "raylib.h";
constexpr char const* path_raymath   = "raymath.h";
constexpr char const* path_rcore     = "rcore.h";
constexpr char const* path_rgestures = "rgestures.h";
constexpr char const* path_rgl       = "rgl.h";
constexpr char const* path_rtext     = "rtext.h";


using namespace gen;


void refactor_file( CodeBody code_file )
{
	String name_scratch = String::make_reserve( GlobalAllocator, kilobytes(1) );
	for ( Code elem : code_file )
	{
		if ( elem->Type == ECode::Preprocess_Define )
		{
			if ( str_compare( elem->Name, txt("RL_"), 2 ) == 0 )
				continue;

			name_scratch.append_fmt( "%RL_%S", elem->Name );
			elem->Name = get_cached_string( name_scratch );
			name_scratch.clear();
		}
	}
}

int gen_main()
{
	gen::init();

	FileContents config_file_content = file_read_contents( GlobalAllocator, true, path_config );
	CodeBody     config_code         = parse_global_body( { config_file_content.size, rcast(char const*, config_file_content.data) } );
	refactor_file( config_code );
	Builder config_builder = Builder::open( path_config );
	config_builder.print( config_code );
	config_builder.write();

	FileContents raylib_file_content = file_read_contents( GlobalAllocator, true, path_raylib );
	CodeBody     raylib_code         = parse_global_body( { raylib_file_content.size, rcast(char const*, raylib_file_content.data) } );
	refactor_file( raylib_code );
	Builder raylib_builder = Builder::open( path_raylib );
	config_builder.print( raylib_code );
	config_builder.write();

	return 0;
}
