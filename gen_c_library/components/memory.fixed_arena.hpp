#pragma once
#include "../project/gen.hpp"

using namespace gen;

CodeBody gen_fixed_arenas()
{
	CodeBody result = def_body(ECode::Global_Body);
	result.append(def_pragma(txt("region FixedArena")));

	char const* template_struct = stringize(
		struct FixedArena_<Name>
		{
			char  memory[<Size>];
			Arena arena;
		};
	);

	char const* template_interface = stringize(
		inline
		void fixed_arena_init_<Name>(FixedArena_<Name>* result) {
			zero_size(& result->memory[0], <Size>);
			result->arena = arena_init_from_memory(& result->memory[0], <Size>);
		}

		inline
		ssize fixed_arena_size_remaining_<Name>(FixedArena_<Name>* fixed_arena, ssize alignment) {
			return size_remaining(fixed_arena->arena, alignment);
		}
	);

	CodeStruct arena_struct_1kb   = parse_struct( token_fmt_impl( 3, "Name", txt("1KB"),   "Size", txt("kilobytes(1)"),   template_struct ));
	CodeStruct arena_struct_4kb   = parse_struct( token_fmt_impl( 3, "Name", txt("4KB"),   "Size", txt("kilobytes(4)"),   template_struct ));
	CodeStruct arena_struct_8kb   = parse_struct( token_fmt_impl( 3, "Name", txt("8KB"),   "Size", txt("kilobytes(8)"),   template_struct ));
	CodeStruct arena_struct_16kb  = parse_struct( token_fmt_impl( 3, "Name", txt("16KB"),  "Size", txt("kilobytes(16)"),  template_struct ));
	CodeStruct arena_struct_32kb  = parse_struct( token_fmt_impl( 3, "Name", txt("32KB"),  "Size", txt("kilobytes(32)"),  template_struct ));
	CodeStruct arena_struct_64kb  = parse_struct( token_fmt_impl( 3, "Name", txt("64KB"),  "Size", txt("kilobytes(64)"),  template_struct ));
	CodeStruct arena_struct_128kb = parse_struct( token_fmt_impl( 3, "Name", txt("128KB"), "Size", txt("kilobytes(128)"), template_struct ));
	CodeStruct arena_struct_256kb = parse_struct( token_fmt_impl( 3, "Name", txt("256KB"), "Size", txt("kilobytes(256)"), template_struct ));
	CodeStruct arena_struct_512kb = parse_struct( token_fmt_impl( 3, "Name", txt("512KB"), "Size", txt("kilobytes(512)"), template_struct ));
	CodeStruct arena_struct_1mb   = parse_struct( token_fmt_impl( 3, "Name", txt("1MB"),   "Size", txt("megabytes(1)"),   template_struct ));
	CodeStruct arena_struct_2mb   = parse_struct( token_fmt_impl( 3, "Name", txt("2MB"),   "Size", txt("megabytes(2)"),   template_struct ));
	CodeStruct arena_struct_4mb   = parse_struct( token_fmt_impl( 3, "Name", txt("4MB"),   "Size", txt("megabytes(4)"),   template_struct ));


	CodeBody arena_interface_1kb   = parse_global_body( token_fmt_impl( 3, "Name", txt("1KB"),   "Size", txt("kilobytes(1)"),    template_interface ));
	CodeBody arena_interface_4kb   = parse_global_body( token_fmt_impl( 3, "Name", txt("4KB"),   "Size", txt("kilobytes(4)"),    template_interface ));
	CodeBody arena_interface_8kb   = parse_global_body( token_fmt_impl( 3, "Name", txt("8KB"),   "Size", txt("kilobytes(8)"),    template_interface ));
	CodeBody arena_interface_16kb  = parse_global_body( token_fmt_impl( 3, "Name", txt("16KB"),  "Size", txt("kilobytes(16)"),   template_interface ));
	CodeBody arena_interface_32kb  = parse_global_body( token_fmt_impl( 3, "Name", txt("32KB"),  "Size", txt("kilobytes(32)"),   template_interface ));
	CodeBody arena_interface_64kb  = parse_global_body( token_fmt_impl( 3, "Name", txt("64KB"),  "Size", txt("kilobytes(64)"),   template_interface ));
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
	result.append(arena_interface_128kb);
	result.append(arena_interface_256kb);
	result.append(arena_interface_512kb);
	result.append(arena_interface_1mb);
	result.append(arena_interface_2mb);
	result.append(arena_interface_4mb);

	CodeDefine def = def_define(txt("fixed_arena_allocator_info(fixed_arena)"), code({ arena_allocator_proc, & fixed_arena.arena }) );
	result.append(def);

	result.append(parse_global_body(txt(R"(
#define fixed_arena_init(expr) _Generic((expr), \
    FixedArena_1KB*   : fixed_arena_init_1KB,   \
    FixedArena_4KB*   : fixed_arena_init_4KB,   \
    FixedArena_8KB*   : fixed_arena_init_8KB,   \
    FixedArena_16KB*  : fixed_arena_init_16KB,  \
    FixedArena_32KB*  : fixed_arena_init_32KB,  \
    FixedArena_64KB*  : fixed_arena_init_64KB,  \
    FixedArena_128KB* : fixed_arena_init_128KB, \
    FixedArena_256KB* : fixed_arena_init_256KB, \
    FixedArena_512KB* : fixed_arena_init_512KB, \
    FixedArena_1MB*   : fixed_arena_init_1MB,   \
    FixedArena_2MB*   : fixed_arena_init_2MB,   \
    FixedArena_4MB*   : fixed_arena_init_4MB    \
)(expr)

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
    FixedArena_4MB*   : fixed_arena_size_remaining_4MB               \
)(expr, alignment)
)"
	)));

	result.append(def_pragma(txt("endregion FixedArena")));

	return result;
}
