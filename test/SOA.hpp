#pragma once

#ifdef gen_time
#define GEN_FEATURE_PARSING
#define GEN_DEFINE_LIBRARY_CODE_CONSTANTS
#define GEN_ENFORCE_STRONG_CODE_TYPES
#include "gen.hpp"
using namespace gen;

Code gen_SOA( CodeStruct struct_def, s32 num_entries = 0 )
{
	StringCached name = get_cached_string( token_fmt( "name", (StrC)struct_def->Name,
		stringize( SOA_<name> )
	));

	Code
	soa_entry = { struct_def.duplicate() };
	soa_entry->Name = get_cached_string( name(Entry) );

	constexpr s32 Num_Vars_Cap = 128;

	local_persist Code var_memory[Num_Vars_Cap];
	local_persist Arena var_arena;
	do_once_start
		var_arena = Arena::init_from_memory( var_memory, kilobytes(Num_Vars_Cap) );
	do_once_end

	Array<CodeVar> vars = Array<CodeVar>::init( var_arena );;

	CodeStruct soa = def_struct( name, def_struct_body( args( soa_entry ) ));
	{
		for ( Code struct_mem : struct_def->Body )
		{
			if ( struct_mem->Type == ECode::Variable )
			{
				CodeType var_type        = struct_mem.cast<CodeVar>()->ValueType;
				StrC     num_entries_str = to_StrC( str_fmt_buf( "%d", num_entries ) );

				CodeVar entry_arr = { nullptr };
				if ( ! num_entries)
				{
					entry_arr = parse_variable( token_fmt( "type", (StrC)var_type->Name, "name", (StrC)struct_mem->Name,
						stringize( Array<<type>> <name>; )
					));
				}
				else
				{
					entry_arr = parse_variable( token_fmt( "type", (StrC)var_type->Name, "name", (StrC)struct_mem->Name, "num", num_entries_str,
						stringize( <type> <name>[<num>]; )
					));
				}

				vars.append( entry_arr );
				soa->Body.append( entry_arr );
			}
		}
	}

	CodeFn make;
	{
		make = parse_function( token_fmt("SOA_Type", (StrC)name,
			stringize(
				static
				<SOA_Type> make( AllocatorInfo allocator )
				{
					<SOA_Type> soa = {};
				}
			)
		));

		if ( ! num_entries )
		{
			for ( CodeVar member : vars )
			{
				Code arr_init = def_execution( token_fmt( "var_name", (StrC)member->Name, "var_type", (StrC)member->ValueType->Name,
					stringize( soa.<var_name> = <var_type>::init( allocator ); )
				));

				make->Body.append( arr_init );
			}
		}

		make->Body.append( def_execution( code( return soa; ) ));
	}

	CodeFn get;
	{
		get = parse_function( code(
			Entry get( s32 idx )
			{
			}
		));

		String content = String::make( Memory::GlobalAllocator, "return\n{\n" );

		for ( CodeVar member : vars )
		{
			content.append_fmt( token_fmt( "var_name", (StrC)member->Name,
				"<var_name>[idx],"
			));
		}

		content.append( "};" );

		CodeExec ret = def_execution( content );

		get->Body.append( ret );
	}

	soa->Body.append( make );
	soa->Body.append( get );
	soa->Body.raw()->validate_body();
	vars.free();

	return soa;
}
#endif
