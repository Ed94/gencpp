#pragma once
#include "gen.hpp"

using namespace gen;

CodeBody gen_fixed_arenas()
{
	CodeBody result = def_body(CT_Global_Body);
	result.append(def_pragma(txt("region FixedArena")));

	char const* template_struct = stringize(
		struct FixedArena_<Name>
		{
			char  memory[<Size>];
			Arena arena;
		};
		typedef struct FixedArena_<Name> FixedArena_<Name>;
	);

	char const* template_interface = stringize(
		inline
		void fixed_arena_init_<Name>(FixedArena_<Name>* result)
		{
			result->arena = arena_init_from_memory(& result->memory[0], <Size>);
		}

		inline
		ssize fixed_arena_size_remaining_<Name>(FixedArena_<Name>* fixed_arena, ssize alignment)
		{
			return arena_size_remaining( & fixed_arena->arena, alignment);
		}

		inline
		void fixed_arena_free_<Name>(FixedArena_<Name>* fixed_arena) {
			arena_free( & fixed_arena->arena);
		}
	);

	CodeBody arena_struct_1kb   = parse_global_body( token_fmt_impl( 3, "Name", txt("1KB"),   "Size", txt("kilobytes(1)"),   template_struct ));
	CodeBody arena_struct_4kb   = parse_global_body( token_fmt_impl( 3, "Name", txt("4KB"),   "Size", txt("kilobytes(4)"),   template_struct ));
	CodeBody arena_struct_8kb   = parse_global_body( token_fmt_impl( 3, "Name", txt("8KB"),   "Size", txt("kilobytes(8)"),   template_struct ));
	CodeBody arena_struct_16kb  = parse_global_body( token_fmt_impl( 3, "Name", txt("16KB"),  "Size", txt("kilobytes(16)"),  template_struct ));
	CodeBody arena_struct_32kb  = parse_global_body( token_fmt_impl( 3, "Name", txt("32KB"),  "Size", txt("kilobytes(32)"),  template_struct ));
	CodeBody arena_struct_64kb  = parse_global_body( token_fmt_impl( 3, "Name", txt("64KB"),  "Size", txt("kilobytes(64)"),  template_struct ));
	CodeBody arena_struct_128kb = parse_global_body( token_fmt_impl( 3, "Name", txt("128KB"), "Size", txt("kilobytes(128)"), template_struct ));
	CodeBody arena_struct_256kb = parse_global_body( token_fmt_impl( 3, "Name", txt("256KB"), "Size", txt("kilobytes(256)"), template_struct ));
	CodeBody arena_struct_512kb = parse_global_body( token_fmt_impl( 3, "Name", txt("512KB"), "Size", txt("kilobytes(512)"), template_struct ));
	CodeBody arena_struct_1mb   = parse_global_body( token_fmt_impl( 3, "Name", txt("1MB"),   "Size", txt("megabytes(1)"),   template_struct ));
	CodeBody arena_struct_2mb   = parse_global_body( token_fmt_impl( 3, "Name", txt("2MB"),   "Size", txt("megabytes(2)"),   template_struct ));
	CodeBody arena_struct_4mb   = parse_global_body( token_fmt_impl( 3, "Name", txt("4MB"),   "Size", txt("megabytes(4)"),   template_struct ));


	CodeBody arena_interface_1kb   = parse_global_body( token_fmt_impl( 3, "Name", txt("1KB"),   "Size", txt("kilobytes(1)"),   template_interface ));
	CodeBody arena_interface_4kb   = parse_global_body( token_fmt_impl( 3, "Name", txt("4KB"),   "Size", txt("kilobytes(4)"),   template_interface ));
	CodeBody arena_interface_8kb   = parse_global_body( token_fmt_impl( 3, "Name", txt("8KB"),   "Size", txt("kilobytes(8)"),   template_interface ));
	CodeBody arena_interface_16kb  = parse_global_body( token_fmt_impl( 3, "Name", txt("16KB"),  "Size", txt("kilobytes(16)"),  template_interface ));
	CodeBody arena_interface_32kb  = parse_global_body( token_fmt_impl( 3, "Name", txt("32KB"),  "Size", txt("kilobytes(32)"),  template_interface ));
	CodeBody arena_interface_64kb  = parse_global_body( token_fmt_impl( 3, "Name", txt("64KB"),  "Size", txt("kilobytes(64)"),  template_interface ));
	CodeBody arena_interface_128kb = parse_global_body( token_fmt_impl( 3, "Name", txt("128KB"), "Size", txt("kilobytes(128)"), template_interface ));
	CodeBody arena_interface_256kb = parse_global_body( token_fmt_impl( 3, "Name", txt("256KB"), "Size", txt("kilobytes(256)"), template_interface ));
	CodeBody arena_interface_512kb = parse_global_body( token_fmt_impl( 3, "Name", txt("512KB"), "Size", txt("kilobytes(512)"), template_interface ));
	CodeBody arena_interface_1mb   = parse_global_body( token_fmt_impl( 3, "Name", txt("1MB"),   "Size", txt("megabytes(1)"),   template_interface ));
	CodeBody arena_interface_2mb   = parse_global_body( token_fmt_impl( 3, "Name", txt("2MB"),   "Size", txt("megabytes(2)"),   template_interface ));
	CodeBody arena_interface_4mb   = parse_global_body( token_fmt_impl( 3, "Name", txt("4MB"),   "Size", txt("megabytes(4)"),   template_interface ));

	result.append(arena_struct_1kb);
	result.append(arena_struct_4kb);
	result.append(arena_struct_8kb);
	result.append(arena_struct_16kb);
	result.append(arena_struct_32kb);
	result.append(arena_struct_64kb);
	result.append(arena_struct_128kb);
	result.append(arena_struct_256kb);
	result.append(arena_struct_512kb);
	result.append(arena_struct_1mb);
	result.append(arena_struct_2mb);
	result.append(arena_struct_4mb);

	result.append(arena_interface_1kb);
	result.append(arena_interface_4kb);
	result.append(arena_interface_8kb);
	result.append(arena_interface_16kb);
	result.append(arena_interface_32kb);
	result.append(arena_interface_64kb);
	result.append(arena_interface_128kb);
	result.append(arena_interface_256kb);
	result.append(arena_interface_512kb);
	result.append(arena_interface_1mb);
	result.append(arena_interface_2mb);
	result.append(arena_interface_4mb);

	register_macros( args(
		( Macro { txt("fixed_arena_allocator_info"), MT_Expression, MF_Functional }),
		( Macro { txt("fixed_arena_init"),           MT_Expression, MF_Functional }),
		( Macro { txt("fixed_arena_free"),           MT_Expression, MF_Functional }),
		( Macro { txt("fixed_arena_size_remaining"), MT_Expression, MF_Functional })
	));

	CodeDefine def = parse_define(txt("#define fixed_arena_allocator_info(fixed_arena) ( (AllocatorInfo) { arena_allocator_proc, & (fixed_arena)->arena } )\n"));
	result.append(def);
	result.append(fmt_newline);

	result.append(parse_global_body(txt(R"(
#define fixed_arena_init(expr) _Generic((expr),    \
    FixedArena_1KB*   : fixed_arena_init_1KB,      \
    FixedArena_4KB*   : fixed_arena_init_4KB,      \
    FixedArena_8KB*   : fixed_arena_init_8KB,      \
    FixedArena_16KB*  : fixed_arena_init_16KB,     \
    FixedArena_32KB*  : fixed_arena_init_32KB,     \
    FixedArena_64KB*  : fixed_arena_init_64KB,     \
    FixedArena_128KB* : fixed_arena_init_128KB,    \
    FixedArena_256KB* : fixed_arena_init_256KB,    \
    FixedArena_512KB* : fixed_arena_init_512KB,    \
    FixedArena_1MB*   : fixed_arena_init_1MB,      \
    FixedArena_2MB*   : fixed_arena_init_2MB,      \
    FixedArena_4MB*   : fixed_arena_init_4MB,      \
	default           : gen_generic_selection_fail \
) GEN_RESOLVED_FUNCTION_CALL(expr)

#define fixed_arena_free(expr) _Generic((expr),    \
    FixedArena_1KB*   : fixed_arena_free_1KB,      \
    FixedArena_4KB*   : fixed_arena_free_4KB,      \
    FixedArena_8KB*   : fixed_arena_free_8KB,      \
    FixedArena_16KB*  : fixed_arena_free_16KB,     \
    FixedArena_32KB*  : fixed_arena_free_32KB,     \
    FixedArena_64KB*  : fixed_arena_free_64KB,     \
    FixedArena_128KB* : fixed_arena_free_128KB,    \
    FixedArena_256KB* : fixed_arena_free_256KB,    \
    FixedArena_512KB* : fixed_arena_free_512KB,    \
    FixedArena_1MB*   : fixed_arena_free_1MB,      \
    FixedArena_2MB*   : fixed_arena_free_2MB,      \
    FixedArena_4MB*   : fixed_arena_free_4MB,      \
	default           : gen_generic_selection_fail \
)	GEN_RESOLVED_FUNCTION_CALL(expr)

#define fixed_arena_size_remaining(expr, alignment) _Generic((expr), \
    FixedArena_1KB*   : fixed_arena_size_remaining_1KB,              \
    FixedArena_4KB*   : fixed_arena_size_remaining_4KB,              \
    FixedArena_8KB*   : fixed_arena_size_remaining_8KB,              \
    FixedArena_16KB*  : fixed_arena_size_remaining_16KB,             \
    FixedArena_32KB*  : fixed_arena_size_remaining_32KB,             \
    FixedArena_64KB*  : fixed_arena_size_remaining_64KB,             \
    FixedArena_128KB* : fixed_arena_size_remaining_128KB,            \
    FixedArena_256KB* : fixed_arena_size_remaining_256KB,            \
    FixedArena_512KB* : fixed_arena_size_remaining_512KB,            \
    FixedArena_1MB*   : fixed_arena_size_remaining_1MB,              \
    FixedArena_2MB*   : fixed_arena_size_remaining_2MB,              \
    FixedArena_4MB*   : fixed_arena_size_remaining_4MB,              \
	default           : gen_generic_selection_fail                   \
)	GEN_RESOLVED_FUNCTION_CALL(expr, alignment)
)"
	)));

	result.append(fmt_newline);
	result.append(def_pragma(txt("endregion FixedArena")));

	return result;
}
