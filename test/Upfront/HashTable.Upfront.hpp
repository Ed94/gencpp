#pragma once

#if gen_time
#include "gen.hpp"
#include "Array.Upfront.hpp"

using namespace gen;

Code gen__hashtable_base()
{
	CodeVar hashIndex   = def_variable( t_sw, name(HashIndex) );
	CodeVar entry_prev  = def_variable( t_sw, name(PrevIndex) );
	CodeVar entry_index = def_variable( t_sw, name(EntryIndex) );

	CodeStruct find_result = def_struct( name(HashTable_FindResult), def_struct_body( 3
		, hashIndex
		, entry_prev
		, entry_index
	));

	return find_result;
}

Code gen__hashtable( StrC type )
{
	static CodeType t_allocator_info = def_type( name(AllocatorInfo) );

	CodeType t_find_result = def_type( name(HashTable_FindResult) );

	StringCached name;
	{
		char const* name_str = str_fmt_buf( "HashTable_%s", type.Ptr );
		s32         len      = str_len( name_str );

		name = get_cached_string({ len, name_str });
	}

	CodeType t_ht_type = def_type( name );

	CodeType t_type     = def_type( type );
	CodeType t_type_ptr = def_type( type, __, spec_ptr );
	CodeType t_type_ref = def_type( type, __, spec_ref );

	// Hash table depends on array container for its entry structure.
	CodeType t_ht_entry, t_array_ht_entry;
	CodeStruct ht_entry, array_ht_entry;
	{
		char const* name_str = str_fmt_buf( "HashTable_%s_Entry", type.Ptr );
		s32         len      = str_len( name_str );

		StringCached ht_entry_name = get_cached_string({ len, name_str });

		t_ht_entry = def_type( ht_entry_name );
		ht_entry   = def_struct( ht_entry_name, def_struct_body( args(
			  def_variable( t_u64,  name(Key))
			, def_variable( t_sw,   name(Next))
			, def_variable( t_type, name(Value))
		)));

		array_ht_entry   = gen__array( ht_entry_name );
		t_array_ht_entry = def_type( array_ht_entry->Name );
	}

	CodeStruct hashtable = {0};
	{
		CodeUsing using_entry       = def_using( name(Entry), t_ht_entry );
		CodeUsing using_array_entry = def_using( name(Array_Entry), t_array_ht_entry );
		CodeUsing using_find_result = def_using( name(FindResult), t_find_result );

		CodeType t_array_sw    = def_type( name(Array_sw) );
		CodeType t_array_entry = def_type( name(Array_Entry) );

		CodeVar hashes  = def_variable( t_array_sw, name(Hashes) );
		CodeVar entries = def_variable( t_array_entry, name(Entries));

		CodeFn init;
		{
			char const* tmpl = stringize(
				<type> result = { 0 };

				result.Hashes  = Array_sw   ::init( allocator );
				result.Entries = Array_Entry::init( allocator );

				return result;
			);
			Code body = def_execution( token_fmt( "type", (StrC)name, tmpl ) );

			init = def_function( name(init), def_param( t_allocator_info, name(allocator)), t_ht_type, body, spec_static_member );
		}


		CodeFn init_reserve;
		{
			char const* tmpl = stringize(
				<type> result = { { nullptr }, { nullptr } };

				result.Hashes  = Array_sw::init_reserve( allocator, num );
				result.Hashes.get_header()->Num = num;

				result.Entries = Array_Entry::init_reserve( allocator, num );

				return result;
			);
			Code body = def_execution( token_fmt( "type", (StrC)name, tmpl ) );

			CodeParam params = def_params( args( def_param( t_allocator_info, name(allocator)), def_param( t_sw, name(num))));

			init_reserve = def_function( name(init_reserve), params, t_ht_type, body, spec_static_member );
		}

		CodeFn clear = def_function( name(clear), __, t_void
			, def_execution( code(
				for ( s32 idx = 0; idx < Hashes.num(); idx++ )
					Hashes[ idx ] = -1;

				Entries.clear();
			))
		);

		CodeFn destroy = def_function( name(destroy), __, t_void
			, def_execution( code(
				if ( Hashes && Hashes.get_header()->Capacity )
					Hashes.free();
				if ( Entries && Hashes.get_header()->Capacity )
					Entries.free();
			))
		);

		CodeFn get = def_function( name(get), def_param( t_u64, name(key)), t_type_ptr
			, def_execution( code(
				sw idx = find( key ).EntryIndex;
				if ( idx >= 0 )
					return & Entries[ idx ].Value;

				return nullptr;
			))
		);

		CodeUsing using_map_proc;
		{
			char const* tmpl = stringize(
				void (*) ( u64 key, <type> value )
			);
			CodeType value = def_type( token_fmt( "type", (StrC)t_type.to_string(), tmpl ) );

			using_map_proc = def_using ( name(MapProc), value);
		}

		CodeFn map;
		{
			CodeType t_map_proc = def_type( name(MapProc) );

			Code body = def_execution( code(
				GEN_ASSERT_NOT_NULL( map_proc );

				for ( sw idx = 0; idx < Entries.num(); idx++ )
				{
					map_proc( Entries[ idx ].Key, Entries[ idx ].Value );
				}
			));

			map = def_function( name(map), def_param( t_map_proc, name(map_proc) ), t_void, body );
		}

		CodeUsing using_map_mut_proc;
		{
			char const* tmpl = stringize(
				void (*) ( u64 key, <type> value )
			);
			CodeType value = def_type( token_fmt( "type", (StrC)t_type_ptr.to_string(), tmpl ) );

			using_map_mut_proc = def_using ( name(MapMutProc), value);
		}

		CodeFn map_mut;
		{
			CodeType t_map_mut_proc = def_type( name(MapMutProc));

			Code body = def_execution( code(
				GEN_ASSERT_NOT_NULL( map_proc );

				for ( sw idx = 0; idx < Entries.num(); idx++ )
				{
					map_proc( Entries[ idx ].Key, & Entries[ idx ].Value );
				}
			));

			map_mut = def_function( name(map_mut), def_param( t_map_mut_proc, name(map_proc)), t_void, body );
		}

		CodeFn grow = def_function( name(grow), __, t_void
			, def_execution( code(
				sw new_num = array_grow_formula( Entries.num() );
				rehash( new_num );
			))
		);

		CodeFn rehash;
		{
			char const* tmpl = stringize(
				sw idx;
				sw last_added_index;

				<type> new_ht = init_reserve( Hashes.get_header()->Allocator, new_num );

				Array_sw::Header* hash_header = new_ht.Hashes.get_header();

				for ( idx = 0; idx < new_ht.Hashes.num(); ++idx )
					new_ht.Hashes[ idx ] = -1;

				for ( idx = 0; idx < Entries.num(); ++idx )
				{
					Entry& entry = Entries[ idx ];

					FindResult find_result;

					if ( new_ht.Hashes.num() == 0 )
						new_ht.grow();

					entry            = Entries[ idx ];
					find_result      = new_ht.find( entry.Key );
					last_added_index = new_ht.add_entry( entry.Key );

					if ( find_result.PrevIndex < 0 )
						new_ht.Hashes[ find_result.HashIndex ] = last_added_index;

					else
						new_ht.Entries[ find_result.PrevIndex ].Next = last_added_index;

					new_ht.Entries[ last_added_index ].Next  = find_result.EntryIndex;
					new_ht.Entries[ last_added_index ].Value = entry.Value;
				}

				destroy();
				*this = new_ht;
			);
			Code body = def_execution( token_fmt( "type", (StrC)name, tmpl ) );

			rehash = def_function( name(rehash), def_param( t_sw, name(new_num)), t_void, body );
		}

		CodeFn rehash_fast;
		{
			char const* tmpl = stringize(
				sw idx;

				for ( idx = 0; idx < Entries.num(); idx++ )
					Entries[ idx ].Next = -1;

				for ( idx = 0; idx < Hashes.num(); idx++ )
					Hashes[ idx ] = -1;

				for ( idx = 0; idx < Entries.num(); idx++ )
				{
					Entry* entry;

					FindResult find_result;
				}
			);
			Code body = def_execution( token_fmt( "type", name, tmpl ) );

			rehash_fast = def_function( name(rehash_fast), __, t_void, body );
		}

		CodeFn remove = def_function( name(remove), def_param( t_u64, name(key)), t_void
			, def_execution( code(
				FindResult find_result = find( key);

				if ( find_result.EntryIndex >= 0 )
				{
					Entries.remove_at( find_result.EntryIndex );
					rehash_fast();
				}
			))
		);

		CodeFn remove_entry = def_function( name(remove_entry), def_param( t_sw, name(idx)), t_void
			, def_execution( code(
				Entries.remove_at( idx );
			))
		);

		CodeFn set;
		{
			CodeParam params = def_params( args(
				  def_param( t_u64,  name(key))
				, def_param( t_type, name(value))
			));

			Code body = def_execution( code(
				sw idx;
				FindResult find_result;

				if ( Hashes.num() == 0 )
					grow();

				find_result = find( key );

				if ( find_result.EntryIndex >= 0 )
				{
					idx = find_result.EntryIndex;
				}
				else
				{
					idx = add_entry( key );

					if ( find_result.PrevIndex >= 0 )
					{
						Entries[ find_result.PrevIndex ].Next = idx;
					}
					else
					{
						Hashes[ find_result.HashIndex ] = idx;
					}
				}

				Entries[ idx ].Value = value;

				if ( full() )
					grow();
			));

			set = def_function( name(set), params, t_void, body );
		}

		CodeFn slot = def_function( name(slot), def_param( t_u64, name(key)), t_sw
			, def_execution( code(
				for ( sw idx = 0; idx < Hashes.num(); ++idx )
					if ( Hashes[ idx ] == key )
						return idx;

				return -1;
			))
		);

		CodeFn add_entry = def_function( name(add_entry), def_param( t_u64, name(key)), t_sw
			, def_execution( code(
				sw idx;
				Entry entry = { key, -1 };

				idx = Entries.num();
				Entries.append( entry );
				return idx;
			))
		);

		CodeFn find = def_function( name(find), def_param( t_u64, name(key)), t_find_result
			, def_execution( code(
				FindResult result = { -1, -1, -1 };

				if ( Hashes.num() > 0 )
				{
					result.HashIndex    = key % Hashes.num();
					result.EntryIndex  = Hashes[ result.HashIndex ];

					while ( result.EntryIndex >= 0 )
					{
						if ( Entries[ result.EntryIndex ].Key == key )
							break;

						result.PrevIndex  = result.EntryIndex;
						result.EntryIndex = Entries[ result.EntryIndex ].Next;
					}
				}

				return result;
			))
		);

		CodeFn full = def_function( name(full), __, t_b32
			, def_execution( code(
				return 0.75f * Hashes.num() < Entries.num();
			))
		);

		hashtable = def_struct( name, def_struct_body( args(
			  using_entry
			, using_array_entry
			, using_find_result
			, using_map_proc
			, using_map_mut_proc

			, init
			, init_reserve

			, clear
			, destroy
			, get
			, grow
			, map
			, map_mut
			, rehash
			, rehash_fast
			, remove
			, remove_entry
			, set
			, slot

			, hashes
			, entries

			, access_protected
			, add_entry
			, find
			, full
		)));
	}

	return def_global_body( args( ht_entry, array_ht_entry, hashtable ));
}

struct GenHashTableRequest
{
	StrC Dependency;
	StrC Type;
};
Array<GenHashTableRequest> GenHashTableRequests;

void gen__hashtable_request( StrC type, StrC dep = {} )
{
	do_once_start
		GenHashTableRequests = Array<GenHashTableRequest>::init( Memory::GlobalAllocator );

		gen_array( sw );
	do_once_end

	// Make sure we don't already have a request for the type.
	for ( sw idx = 0; idx < GenHashTableRequests.num(); ++idx )
	{
		StrC const reqest_type = GenHashTableRequests[ idx ].Type;

		if ( reqest_type.Len != type.Len )
			continue;

		if ( str_compare( reqest_type.Ptr, type.Ptr, reqest_type.Len ) == 0 )
			return;
	}

	GenHashTableRequest request = { dep, type };
	GenHashTableRequests.append( request );
}
#define gen_hashtable( type ) gen__hashtable_request( code(type))

u32 gen_hashtable_file()
{
	Builder
	gen_hashtable_file;
	gen_hashtable_file.open( "hashtable.Upfront.gen.hpp" );

	gen_hashtable_file.print( def_include( txt_StrC("gen.hpp")) );
	gen_hashtable_file.print( def_include( txt_StrC("Array.Upfront.hpp")) );
	gen_hashtable_file.print( def_include( txt_StrC("array.Upfront.gen.hpp")) );

	gen_hashtable_file.print( def_using_namespace( name(gen)));

	gen_hashtable_file.print( gen__hashtable_base());

	GenHashTableRequest* current = GenHashTableRequests;
	s32 left = GenHashTableRequests.num();
	while (left--)
	{
		GenHashTableRequest const& request = * current;

		Code generated_buffer = gen__hashtable( current->Type  );

		if ( request.Dependency )
		{
			char const* cmt_str = str_fmt_buf( "// Dependency for %s type", request.Type );
			s32         cmt_len = str_len( cmt_str );

			Code cmt     = def_comment( { cmt_len, cmt_str } );
			Code include = def_include( request.Dependency );

			gen_hashtable_file.print( cmt );
			gen_hashtable_file.print( include );
		}

		gen_hashtable_file.print( generated_buffer );
		current++;
	}

	gen_hashtable_file.write();
	return 0;
}

#endif // gen_time
