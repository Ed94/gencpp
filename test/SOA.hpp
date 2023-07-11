#pragma once

#ifdef gen_time
#include "gen.hpp"
using namespace gen;

Code gen_SOA( Code struct_def, bool use_dynamic = false )
{
	StrC name;
	name.Ptr = str_fmt_buf( "SOA_%s", (char const*) struct_def->Name );
	name.Len = str_len( name );

	Code
	soa_entry = { struct_def->duplicate() };
	soa_entry->Name = get_cached_string( name(Entry) );

	Array<Code> vars = Array<Code>::init( Memory::GlobalAllocator );;

	Code soa = def_struct( name, def_struct_body( 1, soa_entry ) );
	{
		Code body = struct_def.body();
		for ( s32 idx = 0; idx < body->num_entries(); idx++ )
		{
			Code struct_mem = { body->entry( idx ) };

			if ( struct_mem->Type == ECode::Variable )
			{
				Code var_type  = { struct_mem->entry(0) };

				Code entry_arr = { nullptr };
				if ( use_dynamic)
				{
					entry_arr = parse_variable( token_fmt( "Array<<type>> <name>;", 2
						, "type", (char const*)var_type->Name
						, "name", (char const*)struct_mem->Name )
					);
				}
				else
				{
					entry_arr = parse_variable( token_fmt( "<type> <name>[100];", 2
						, "type", (char const*)var_type->Name
						, "name", (char const*)struct_mem->Name )
					);
				}

				vars.append( entry_arr );
				soa.body()->add_entry( entry_arr );
			}
		}
	}

	Code make;
	{
		make = parse_function( token_fmt(
		txt(
			static
			<SOA_Type> make( AllocatorInfo allocator )
			{
				<SOA_Type> soa = {};
			}
		),
			1, "SOA_Type", (char const*)name
		));

		if ( use_dynamic )
		{
			for ( s32 idx = 0; idx < vars.num(); idx++ )
			{
				Code member = vars[idx];

				Code arr_init = def_execution( token_fmt( "soa.<var_name> = <var_type>::init( allocator );", 2
					, "var_name", (char const*)member->Name
					, "var_type", (char const*)member->entry(0)->Name
				));

				make.body()->add_entry( arr_init );
			}
		}

		make.body()->add_entry( def_execution( code( return soa; ) ));
	}

	Code get;
	{
		get = parse_function( code(
			Entry get( s32 idx )
			{
			}
		));

		String content = String::make( Memory::GlobalAllocator, "return\n{\n" );

		for ( s32 idx = 0; idx < vars.num(); idx ++ )
		{
			Code member = vars[idx];

			content.append_fmt( token_fmt( "<var_name>[idx],", 1
				, "var_name", (char const*)member->Name
			));
		}

		content.append( "};" );

		Code ret = def_execution( content );

		get.body()->add_entry( ret );
	}

	soa.body()->add_entry( make );
	soa.body()->add_entry( get );
	return soa;
}
#endif
