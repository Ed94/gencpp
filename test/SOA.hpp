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
					entry_arr = parse_variable( token_fmt( "type", (StrC)var_type->Name, "name", (StrC)struct_mem->Name,
						stringize( Array<<type>> <name>; )
					));
				}
				else
				{
					entry_arr = parse_variable( token_fmt( "type", (StrC)var_type->Name, "name", (StrC)struct_mem->Name,
						stringize( <type> <name>[100]; )
					));
				}

				vars.append( entry_arr );
				soa.body()->add_entry( entry_arr );
			}
		}
	}

	Code make;
	{
		make = parse_function( token_fmt("SOA_Type", name,
			stringize(
				static
				<SOA_Type> make( AllocatorInfo allocator )
				{
					<SOA_Type> soa = {};
				}
			)
		));

		if ( use_dynamic )
		{
			for ( s32 idx = 0; idx < vars.num(); idx++ )
			{
				Code member = vars[idx];

				Code arr_init = def_execution( token_fmt( "var_name", (StrC)member->Name, "var_type", (StrC)member->entry(0)->Name,
					stringize( soa.<var_name> = <var_type>::init( allocator ); )
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

			content.append_fmt( token_fmt( "var_name", (StrC)member->Name,
				"<var_name>[idx],"
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
