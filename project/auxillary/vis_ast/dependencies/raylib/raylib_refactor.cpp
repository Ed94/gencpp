#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_BENCHMARK
#define GEN_ENFORCE_STRONG_CODE_TYPES
// #define GEN_IMPLEMENTATION
#include "gen.cpp"
#include "gen.builder.cpp"


constexpr char const* path_config_h    = "config.h";
constexpr char const* path_raylib_h    = "raylib.h";
constexpr char const* path_raymath_h   = "raymath.h";
constexpr char const* path_rcamera_h   = "rcamera.h";
constexpr char const* path_rcore_h     = "rcore.h";
constexpr char const* path_rgestures_h = "rgestures.h";
constexpr char const* path_rgl_h       = "rgl.h";
constexpr char const* path_rtext_h     = "rtext.h";

constexpr char const* path_rcore_desktop_c = "rcore_desktop.c";

constexpr char const* path_raudio_c    = "raudio.c";
constexpr char const* path_rcore_c     = "rcore.c";
constexpr char const* path_rglfw_c     = "rglfw.c";
constexpr char const* path_rmodels_c   = "rmodels.c";
constexpr char const* path_rtext_c     = "rtext.c";
constexpr char const* path_rtextures_c = "rtextures.c";
constexpr char const* path_rutils_c    = "rutils.c";

using namespace gen;

StringCached upper_snake_to_mixed_snake(StringCached str)
{
    local_persist String scratch = String::make_reserve(GlobalAllocator, kilobytes(1));
    scratch.clear();

    bool capitalizeNext = true;

    for (s32 index = 0; index < str.length(); ++index)
    {
        char c = str[index];

        if (c == '_')
        {
            scratch.append(c);
            capitalizeNext = true;
        }
        else if (capitalizeNext)
        {
            if (c >= 'a' && c <= 'z')
            {
                scratch.append(c - 32); // Convert to uppercase
            }
            else
            {
                scratch.append(c);
            }
            capitalizeNext = false;
        }
        else
        {
            if (c >= 'A' && c <= 'Z')
            {
                scratch.append(c + 32); // Convert to lowercase
            }
            else
            {
                scratch.append(c);
            }
        }
    }

    StringCached result = get_cached_string(scratch);
    return result;
}

StringCached pascal_to_lower_snake(StringCached str)
{
    local_persist String scratch = String::make_reserve(GlobalAllocator, kilobytes(1));
    scratch.clear();

    for (s32 index = 0; index < str.length(); ++index)
    {
        char c = str[index];
        char next = (index + 1 < str.length()) ? str[index + 1] : '\0'; // Ensure we don't go out of bounds

        // Whitelist check for "2D" and "3D"
        if ((c == '2' || c == '3' | c == '4') && (next == 'D' || next == 'd'))
        {
            if (index > 0) // If it's not the start of the string, append an underscore
            {
                char* prev = str.Data + index - 1;
                if (*prev != '_') // Avoid double underscores
                {
                    scratch.append('_');
                }
            }
            scratch.append(c);
            scratch.append('d'); // Convert to lowercase
            index++; // Skip the next character since we've already processed it
            continue;
        }

        if (c >= 'A' && c <= 'Z')
        {
            char* prev = (index > 0) ? str.Data + index - 1 : nullptr;

            if ((index > 0 && prev && *prev >= 'a' && *prev <= 'z') || 
                (prev && char_is_digit(*prev) && (next >= 'A' && next <= 'Z')))
            {
                scratch.append('_');
            }

            scratch.append(c + 32);
        }
        else if (char_is_digit(c) && (next >= 'A' && next <= 'Z')) // Check for a number followed by an uppercase letter
        {
            scratch.append(c);
            scratch.append('_');
        }
        else
        {
            scratch.append(c);
        }
    }

    StringCached result = get_cached_string(scratch);
    return result;
}

void refactor_define( CodeDefine& code )
{
	local_persist String name_scratch = String::make_reserve( GlobalAllocator, kilobytes(1) );

	if ( str_compare( elem->Name, txt("RL"), 2 ) == 0 || str_compare( elem->Name, txt("RAYLIB"), 6 ) == 0 )
		continue;

	name_scratch.append_fmt( "%RL_%S", elem->Name );
	elem->Name = get_cached_string( name_scratch );
	name_scratch.clear();
}

void refactor_enum( CodeEnum& code )
{
	for ( Code elem : code->Body )
	{
		if ( elem->Type == ECode::Untyped )
		{
			elem->Content = upper_snake_to_mixed_snake( elem->Content );
		}
	}
}

void refactor_typename( CodeType& type )
{
	local_persist CodeType t_unsigned_char      = parse_type( code(unsigned char) );
	local_persist CodeType t_unsigned_char_ptr  = parse_type( code(unsigned char*) );
	local_persist CodeType t_unsigned_short_ptr	= parse_type( code(unsigned short*) );
	local_persist CodeType t_int                = parse_type( code(int) );
	local_persist CodeType t_int_ptr 		    = parse_type( code(int*) );
	local_persist CodeType t_unsigned_int       = parse_type( code(unsigned int) );
	local_persist CodeType t_float 	            = parse_type( code(float) );
	local_persist CodeType t_float_ptr          = parse_type( code(float*) );

	local_persist CodeType t_f32_ptr = parse_type( code(f32*) );
	local_persist CodeType t_u8_ptr  = parse_type( code(u8*) );
	local_persist CodeType t_s32_ptr = parse_type( code(s32*) );

	String type_str = type.to_string();

	if ( str_compare( type_str, t_unsigned_char.to_string() ) == 0 )
	{
		type.ast = t_u8.ast;
	}
	if ( str_compare( type_str, t_unsigned_char_ptr.to_string() ) == 0 )
	{
		type.ast = t_u8_ptr.ast;
	}
	if ( str_compare( type_str, t_unsigned_short_ptr.to_string() ) == 0 )
	{
		type.ast = t_u8_ptr.ast;
	}
	if ( str_compare( type_str, t_int.to_string() ) == 0 )
	{
		type.ast = t_s32.ast;
	}
	if ( str_compare( type_str, t_int_ptr.to_string() ) == 0 )
	{
		type.ast = t_s32_ptr.ast;
	}
	if ( str_compare( type_str, t_unsigned_int.to_string() ) == 0 )
	{
		type.ast = t_u32.ast;
	}
	if ( str_compare( type_str, t_float.to_string() ) == 0 )
	{
		type.ast = t_f32.ast;
	}
	if ( str_compare( type_str, t_float_ptr.to_string() ) == 0 )
	{
		type.ast = t_f32_ptr.ast;
	}
}

void refactor_fn( CodeFn& fn )
{
	StringCached original_name = fn->Name;
	fn->Name = pascal_to_lower_snake( fn->Name );

	log_fmt( "%S", "Proc ID: %S -> %S", original_name, fn->Name );

	for ( CodeParam param : fn->Params )
	{
		refactor_typename( param->ValueType );
	}
}

void refactor_struct( CodeStruct& code )
{
	for ( Code field : code->Body )
	{
		if ( field->Type == ECode::Variable )
		{
			CodeVar var = field.cast<CodeVar>();
			refactor_typename( var->ValueType );
		}
	}
}

void refactor_file( char const* path )
{
	FileContents contents = file_read_contents( GlobalAllocator, true, path );
	CodeBody     code     = parse_global_body( { contents.size, rcast(char const*, contents.data) } );

	local_perist String name_scratch = String::make_reserve( GlobalAllocator, kilobytes(1) );

	// CodeBody includes
	// CodeBody nspace_body = def_body( ECode::Namespace );
	CodeBody new_code = def_body( ECode::Global_Body );

	for ( Code elem : code )
	{
		if ( elem->Type == ECode::Preprocess_Define )
		{
			refactor_define( elem.cast<CodeDefine>() );
		}

		if ( elem->Type == ECode::Enum )
		{
			refactor_enum( elem.cast<CodeEnum>() );
		}

		if ( elem->Type == ECode::Typedef )
		{
			CodeTypedef td = elem.cast<CodeTypedef>();
			if ( td->UnderlyingType->Type == ECode::Enum )
			{
				CodeEnum code = td->UnderlyingType.cast<CodeEnum>();
				refactor_enum( code );
			}
			if ( td->UnderlyingType->Type == ECode::Struct )
			{
				CodeStruct code = td->UnderlyingType.cast<CodeStruct>();
				refactor_struct( code );
			}
		}

		if ( elem->Type == ECode::Struct )
		{
			refactor_struct( elem.cast<CodeStruct>() );
		}

		if ( elem->Type == ECode::Function || elem->Type == ECode::Function_Fwd )
		{
			refactor_fn( elem.cast<CodeFn>() );
		}

		if ( elem->Type == ECode::Extern_Linkage )
		{
			CodeBody body = elem.cast<CodeExtern>()->Body;
			for ( Code elem : body )
			{
				if ( elem->Type == ECode::Function || elem->Type == ECode::Function_Fwd )
				{
					refactor_fn( elem.cast<CodeFn>() );
				}
			}

			Code nspace = def_namespace( txt("raylib"), def_namespace_body( args(elem) ) );
			elem = nspace;
		}

		new_code.append( elem );
	}

	Builder builder = Builder::open( path );
	builder.print( new_code );
	builder.write();

	name_scratch.clear();
}

int gen_main()
{
	gen::init();

	refactor_file( path_config_h );
	refactor_file( path_raylib_h );
	refactor_file( path_rcamera_h );
	refactor_file( path_raymath_h );
	refactor_file( path_rcore_h );
	refactor_file( path_rgl_h );
	refactor_file( path_rtext_h );

	refactor_file( path_rcore_desktop_c );
	refactor_file( path_raudio_c );
	refactor_file( path_rcore_c );
	refactor_file( path_rglfw_c );
	refactor_file( path_rmodels_c );
	refactor_file( path_rtext_c );
	refactor_file( path_rutils_c );

	return 0;
}
