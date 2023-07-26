#pragma once

#include "gen.hpp"

using namespace gen;

CodeBody gen_ecode( char const* path )
{
	char scratch_mem[kilobytes(1)];
	Arena scratch = Arena::init_from_memory( scratch_mem, sizeof(scratch_mem) );

	file_read_contents( scratch, zero_terminate, path );

	CSV_Object csv_nodes;
	csv_parse( &csv_nodes, scratch_mem, GlobalAllocator, false );

	Array<ADT_Node> enum_strs = csv_nodes.nodes[0].nodes;

	String enum_entries   = String::make_reserve( GlobalAllocator, kilobytes(1) );
	String to_str_entries = String::make_reserve( GlobalAllocator, kilobytes(1) );

	for ( ADT_Node node : enum_strs )
	{
		char const* code = node.string;
		enum_entries.append_fmt( "%s,\n", code );
		to_str_entries.append_fmt( "{ sizeof(\"%s\"), \"%s\" },\n", code, code );
	}

	CodeEnum enum_code = parse_enum( token_fmt( "entries", (StrC)enum_entries, stringize(
		enum Type : u32
		{
			<entries>
			NumTypes
		};
	)));

#pragma push_macro( "local_persist" )
#undef local_persist
	CodeFn to_str = parse_function( token_fmt( "entries", (StrC)to_str_entries, stringize(
		StrC to_str( Type type )
		{
			local_persist
			StrC lookup[] {
				<entries>
			};

			return lookup[ type ];
		}
	)));
#pragma pop_macro( "local_persist" )

	CodeNamespace nspace = def_namespace( name(ECode), def_namespace_body( args( enum_code, to_str ) ) );

	CodeUsing code_t = def_using( name(CodeT), def_type( name(ECode::Type) ) );

	return def_global_body( args( nspace, code_t ) );
}

CodeBody gen_eoperator( char const* path )
{
	char scratch_mem[kilobytes(1)];
	Arena scratch = Arena::init_from_memory( scratch_mem, sizeof(scratch_mem) );

	file_read_contents( scratch, zero_terminate, path );

	CSV_Object csv_nodes;
	csv_parse( &csv_nodes, scratch_mem, GlobalAllocator, false );

	Array<ADT_Node> enum_strs = csv_nodes.nodes[0].nodes;
	Array<ADT_Node> str_strs  = csv_nodes.nodes[1].nodes;

	String enum_entries   = String::make_reserve( GlobalAllocator, kilobytes(1) );
	String to_str_entries = String::make_reserve( GlobalAllocator, kilobytes(1) );

	for (uw idx = 0; idx < enum_strs.num(); idx++)
	{
		char const* enum_str     = enum_strs[idx].string;
		char const* entry_to_str = str_strs [idx].string;

		enum_entries.append_fmt( "%s,\n", enum_str );
		to_str_entries.append_fmt( "{ sizeof(\"%s\"), \"%s\" },\n", entry_to_str, entry_to_str);
	}

	CodeEnum  enum_code = parse_enum(token_fmt("entries", (StrC)enum_entries, stringize(
		enum Type : u32
		{
			<entries>
			NumOps
		};
	)));

#pragma push_macro( "local_persist" )
#undef local_persist
	CodeFn to_str = parse_function(token_fmt("entries", (StrC)to_str_entries, stringize(
		StrC to_str( Type op )
		{
			local_persist
			StrC lookup[] {
				<entries>
			};

			return lookup[ op ];
		}
	)));
#pragma pop_macro( "local_persist" )

	CodeNamespace nspace = def_namespace( name(EOperator), def_namespace_body( args( enum_code, to_str ) ) );

	CodeUsing operator_t = def_using( name(OperatorT), def_type( name(EOperator::Type) ) );

	return def_global_body( args( nspace, operator_t ) );
}

CodeBody gen_especifier( char const* path )
{
	char scratch_mem[kilobytes(1)];
	Arena scratch = Arena::init_from_memory( scratch_mem, sizeof(scratch_mem) );

	file_read_contents( scratch, zero_terminate, path );

	CSV_Object csv_nodes;
	csv_parse( &csv_nodes, scratch_mem, GlobalAllocator, false );

	Array<ADT_Node> enum_strs = csv_nodes.nodes[0].nodes;
	Array<ADT_Node> str_strs  = csv_nodes.nodes[1].nodes;

	String enum_entries   = String::make_reserve( GlobalAllocator, kilobytes(1) );
	String to_str_entries = String::make_reserve( GlobalAllocator, kilobytes(1) );

	for (uw idx = 0; idx < enum_strs.num(); idx++)
	{
		char const* enum_str     = enum_strs[idx].string;
		char const* entry_to_str = str_strs [idx].string;

		enum_entries.append_fmt( "%s,\n", enum_str );
		to_str_entries.append_fmt( "{ sizeof(\"%s\"), \"%s\" },\n", entry_to_str, entry_to_str);
	}

	CodeEnum  enum_code = parse_enum(token_fmt("entries", (StrC)enum_entries, stringize(
		enum Type : u32
		{
			<entries>
			NumSpecifiers
		};
	)));

	CodeFn is_trailing = parse_function(token_fmt("specifier", (StrC)to_str_entries, stringize(
		bool is_trailing( Type specifier )
		{
			return specifier > Virtual;
		}
	)));

#pragma push_macro( "local_persist" )
#pragma push_macro( "do_once_start" )
#pragma push_macro( "do_once_end" )
#undef local_persist
#undef do_once_start
#undef do_once_end

	CodeFn to_str = parse_function(token_fmt("entries", (StrC)to_str_entries, stringize(
		StrC to_str( Type type )
		{
			local_persist
			StrC lookup[] {
				<entries>
			};

			return lookup[ type ];
		}
	)));

	CodeFn to_type = parse_function( token_fmt( "entries", (StrC)to_str_entries, stringize(
		Type to_type( StrC str )
		{
			local_persist
			u32 keymap[ NumSpecifiers ];
			do_once_start
				for ( u32 index = 0; index < NumSpecifiers; index++ )
				{
					StrC enum_str = to_str( (Type)index );

					// We subtract 1 to remove the null terminator
					// This is because the tokens lexed are not null terminated.
					keymap[index] = crc32( enum_str.Ptr, enum_str.Len - 1);
				}
			do_once_end

			u32 hash = crc32( str.Ptr, str.Len );

			for ( u32 index = 0; index < NumSpecifiers; index++ )
			{
				if ( keymap[index] == hash )
					return (Type)index;
			}

			return Invalid;
		}
	)));

#pragma pop_macro( "local_persist" )
#pragma pop_macro( "do_once_start" )
#pragma pop_macro( "do_once_end" )

	CodeNamespace nspace = def_namespace( name(ESpecifier), def_namespace_body( args( enum_code, is_trailing, to_str, to_type ) ) );

	CodeUsing specifier_t = def_using( name(SpecifierT), def_type( name(ESpecifier::Type) ) );

	return def_global_body( args( nspace, specifier_t ) );
}

CodeBody gen_etoktype()
{
	return CodeInvalid;
}
