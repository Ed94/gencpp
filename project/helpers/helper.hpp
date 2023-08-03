#pragma once

#include "gen.hpp"

GEN_NS_BEGIN
#include "dependencies/parsing.hpp"
GEN_NS_END

using namespace gen;

CodeBody gen_ecode( char const* path )
{
	char  scratch_mem[kilobytes(1)];
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
	CodeUsing    code_t  = def_using( name(CodeT), def_type( name(ECode::Type) ) );

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

CodeBody gen_etoktype( char const* etok_path, char const* attr_path )
{
	char  scratch_mem[kilobytes(16)];
	Arena scratch = Arena::init_from_memory( scratch_mem, sizeof(scratch_mem) );

	FileContents enum_content = file_read_contents( scratch, zero_terminate, etok_path );

	CSV_Object csv_enum_nodes;
	csv_parse( &csv_enum_nodes, rcast(char*, enum_content.data), GlobalAllocator, false );

	FileContents attrib_content = file_read_contents( scratch, zero_terminate, attr_path );

	CSV_Object csv_attr_nodes;
	csv_parse( &csv_attr_nodes, rcast(char*, attrib_content.data), GlobalAllocator, false );

	Array<ADT_Node> enum_strs          = csv_enum_nodes.nodes[0].nodes;
	Array<ADT_Node> enum_str_strs      = csv_enum_nodes.nodes[1].nodes;
	Array<ADT_Node> attribute_strs     = csv_attr_nodes.nodes[0].nodes;
	Array<ADT_Node> attribute_str_strs = csv_attr_nodes.nodes[1].nodes;

	String enum_entries             = String::make_reserve( GlobalAllocator, kilobytes(2) );
	String to_str_entries           = String::make_reserve( GlobalAllocator, kilobytes(4) );
	String attribute_entries        = String::make_reserve( GlobalAllocator, kilobytes(2) );
	String to_str_attributes        = String::make_reserve( GlobalAllocator, kilobytes(4) );
	String attribute_define_entries = String::make_reserve( GlobalAllocator, kilobytes(4) );

	for (uw idx = 0; idx < enum_strs.num(); idx++)
	{
		char const* enum_str     = enum_strs[idx].string;
		char const* entry_to_str = enum_str_strs [idx].string;

		enum_entries.append_fmt( "%s,\n", enum_str );
		to_str_entries.append_fmt( "{ sizeof(\"%s\"), \"%s\" },\n", entry_to_str, entry_to_str);
	}

	for ( uw idx = 0; idx < attribute_strs.num(); idx++ )
	{
		char const* attribute_str = attribute_strs[idx].string;
		char const* entry_to_str  = attribute_str_strs [idx].string;

		attribute_entries.append_fmt( "%s,\n", attribute_str );
		to_str_attributes.append_fmt( "{ sizeof(\"%s\"), \"%s\" },\n", entry_to_str, entry_to_str);
		attribute_define_entries.append_fmt( "Entry( %s, %s )", attribute_str, entry_to_str );

		if ( idx < attribute_strs.num() - 1 )
			attribute_define_entries.append( " \\\n");
	}

#pragma push_macro( "GEN_DEFINE_ATTRIBUTE_TOKENS" )
#undef GEN_DEFINE_ATTRIBUTE_TOKENS
	CodeDefine attribute_entires_def = def_define( name(GEN_DEFINE_ATTRIBUTE_TOKENS), attribute_define_entries  );
#pragma pop_macro( "GEN_DEFINE_ATTRIBUTE_TOKENS" )

	CodeEnum enum_code = parse_enum(token_fmt("entries", (StrC)enum_entries, "attribute_toks", (StrC)attribute_entries, stringize(
		enum Type : u32
		{
			<entries>
			<attribute_toks>
			NumTokens
		};
	)));

#pragma push_macro( "local_persist" )
#pragma push_macro( "do_once_start" )
#pragma push_macro( "do_once_end" )
#undef local_persist
#undef do_once_start
#undef do_once_end
	CodeFn to_str = parse_function(token_fmt("entries", (StrC)to_str_entries, "attribute_toks", (StrC)to_str_attributes, stringize(
		StrC to_str( Type type )
		{
			local_persist
			StrC lookup[] {
				<entries>
				<attribute_toks>
			};

			return lookup[ type ];
		}
	)));

	CodeFn to_type = parse_function( token_fmt( "entries", (StrC)to_str_entries, stringize(
		Type to_type( StrC str )
		{
			local_persist
			u32 keymap[ NumTokens ];
			do_once_start
				for ( u32 index = 0; index < NumTokens; index++ )
				{
					StrC enum_str = to_str( (Type)index );

					// We subtract 1 to remove the null terminator
					// This is because the tokens lexed are not null terminated.
					keymap[index] = crc32( enum_str.Ptr, enum_str.Len - 1);
				}
			do_once_end

			u32 hash = crc32( str.Ptr, str.Len );

			for ( u32 index = 0; index < NumTokens; index++ )
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

	CodeNamespace nspace     = def_namespace( name(ETokType), def_namespace_body( args( attribute_entires_def, enum_code, to_str, to_type ) ) );
	CodeUsing     td_toktype = def_using( name(TokType), def_type( name(ETokType::Type) ) );

	return def_global_body( args( nspace, td_toktype ) );
}

CodeBody gen_data_structures( char const* data_path, char const* ast_path )
{
	return CodeInvalid;
}
