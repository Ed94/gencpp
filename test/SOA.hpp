#pragma once

#ifdef gen_time
#include "gen.hpp"
using namespace gen;

Code gen_SOA( CodeStruct struct_def, s32 num_entries = 0 )
{
	StringCached name = get_cached_string( token_fmt( "name", (StrC)struct_def->Name,
		stringize( SOA_<name> )
	));

	Code
	soa_entry = { struct_def->duplicate() };
	soa_entry->Name = get_cached_string( name(Entry) );

	constexpr s32 Num_Vars_Cap = 128;

	local_persist Code var_memory[Num_Vars_Cap];
	local_persist Arena var_arena;
	do_once_start
		var_arena = Arena::init_from_memory( var_memory, kilobytes(Num_Vars_Cap) );
	do_once_end

	Array<CodeVar> vars = Array<CodeVar>::init( var_arena );;

	CodeStruct soa = def_struct( name, def_body( ECode::Struct_Body ));
	{
		for ( Code struct_mem : struct_def.body() )
		{
			if ( struct_mem->Type == ECode::Variable )
			{
				CodeType var_type        = CodeVar(struct_mem).type();
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
				soa.body()->add_entry( entry_arr );
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
				Code arr_init = def_execution( token_fmt( "var_name", (StrC)member->Name, "var_type", (StrC)member->entry(0)->Name,
					stringize( soa.<var_name> = <var_type>::init( allocator ); )
				));

				make.body()->add_entry( arr_init );
			}
		}

		make.body()->add_entry( def_execution( code( return soa; ) ));
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

		get.body()->add_entry( ret );
	}

	soa.body()->add_entry( make );
	soa.body()->add_entry( get );
	soa.body()->validate_body();
	vars.free();

	return soa;
}
#endif
