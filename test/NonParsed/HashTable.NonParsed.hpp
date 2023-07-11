#pragma once

#if gen_time
#include "gen.hpp"
#include "Array.NonParsed.hpp"

using namespace gen;

Code gen__hashtable_base()
{
	Code hashIndex   = def_variable( t_sw, name(HashIndex) );
	Code entry_prev  = def_variable( t_sw, name(PrevIndex) );
	Code entry_index = def_variable( t_sw, name(EntryIndex) );

	Code find_result = def_struct( name(HashTable_FindResult), def_struct_body( 3
		, hashIndex
		, entry_prev
		, entry_index
	));

	return find_result;
}

Code gen__hashtable( StrC type, sw type_size )
{
	static Code t_allocator_info = def_type( name(AllocatorInfo) );

	Code t_find_result = def_type( name(HashTable_FindResult) );

	StringCached name;
	{
		char const* name_str = str_fmt_buf( "HashTable_%s", type.Ptr );
		s32         len      = str_len( name_str );

		name = get_cached_string({ len, name_str });
	}

	Code t_ht_type = def_type( name );

	Code t_type     = def_type( type );
	Code t_type_ptr = def_type( type, __, spec_ptr );
	Code t_type_ref = def_type( type, __, spec_ref );

	// Hash table depends on array container for its entry structure.
	Code t_ht_entry, ht_entry, array_ht_entry, t_array_ht_entry;
	{
		char const* name_str = str_fmt_buf( "HashTable_%s_Entry", type.Ptr );
		s32         len      = str_len( name_str );

		StringCached ht_entry_name = get_cached_string({ len, name_str });
		sw const     entry_size    = sizeof( u64 ) + sizeof( sw ) + type_size;

		t_ht_entry = def_type( ht_entry_name );
		ht_entry   = def_struct( ht_entry_name, def_struct_body( 3
			, def_variable( t_u64,  name(Key))
			, def_variable( t_sw,   name(Next))
			, def_variable( t_type, name(Value))
		));

		array_ht_entry   = gen__array( ht_entry_name, entry_size );
		t_array_ht_entry = def_type( array_ht_entry->Name );
	}

	Code hashtable = {0};
	{
		Code using_entry       = def_using( name(Entry), t_ht_entry );
		Code using_array_entry = def_using( name(Array_Entry), t_array_ht_entry );
		Code using_find_result = def_using( name(FindResult), t_find_result );

		Code t_array_sw    = def_type( name(Array_sw) );
		Code t_array_entry = def_type( name(Array_Entry) );

		Code hashes  = def_variable( t_array_sw, name(Hashes) );
		Code entries = def_variable( t_array_entry, name(Entries));

		Code init;
		{
			char const* tmpl = txt(
				<type> result = { 0 };

				result.Hashes  = Array_sw   ::init( allocator );
				result.Entries = Array_Entry::init( allocator );

				return result;
			);
			Code body = def_execution( token_fmt( tmpl, 1, "type", name ) );

			init = def_function( name(init), def_param( t_allocator_info, name(allocator)), t_ht_type, body, spec_static_member );
		}

		Code clear = def_function( name(clear), __, t_void
			, def_execution( code(
				for ( s32 idx = 0; idx < Hashes.num(), idx++ )
					Hashes[ idx ] = -1;

				Entries.clear();
			))
		);

		Code destroy = def_function( name(destroy), __, t_void
			, def_execution( code(
				if ( Hashes )
					Hashes.free();
				if ( Entries )
					Entries.free();
			))
		);

		Code get = def_function( name(get), def_param( t_u64, name(key)), t_type_ptr
			, def_execution( code(
				sw idx = find( key ).EntryIndex;
				if ( idx > 0 )
					return & Entries[ idx ].Value;

				return nullptr;
			))
		);

		Code using_map_proc;
		{
			char const* tmpl = txt(
				void (*) ( u64 key, <type> value )
			);
			Code value = untyped_str( token_fmt( tmpl, 1, "type", t_type.to_string() ) );

			using_map_proc = def_using ( name(MapProc), value);
		}

		Code map;
		{
			Code t_map_proc = def_type( name(MapProc) );

			Code body = def_execution( code(
				ZPL_ASSERT_NOT_NULL( map_proc );

				for ( sw idx = 0; idx < Entries.num(); idx++ )
				{
					map_proc( Entries[ idx ].Key, Entries[ idx ].Value );
				}
			));

			map = def_function( name(map), def_param( t_map_proc, name(map_proc) ), t_void, body );
		}

		Code using_map_mut_proc;
		{
			char const* tmpl = txt(
				void (*) ( u64 key, <type> value )
			);
			Code value = untyped_str( token_fmt( tmpl, 1, "type", t_type_ptr.to_string() ) );

			using_map_mut_proc = def_using ( name(MapMutProc), value);
		}

		Code map_mut;
		{
			Code t_map_mut_proc = def_type( name(MapMutProc));

			Code body = def_execution( code(
				ZPL_ASSERT_NOT_NULL( map_proc );

				for ( sw idx = 0; idx < Entries.num(); idx++ )
				{
					map_proc( Entries[ idx ].Key, & Entries[ idx ].Value );
				}
			));

			map_mut = def_function( name(map_mut), def_param( t_map_mut_proc, name(map_proc)), t_void, body );
		}

		Code grow = def_function( name(grow), __, t_void
			, def_execution( code(
				sw new_num = array_grow_formula( Entries.num() );
				rehash( new_num );
			))
		);

		Code rehash;
		{
			char const* tmpl = txt(
				sw idx;
				sw last_added_index;

				<type> new_ht = <type>::init( Hashes.get_header().Allocator );

				new_ht.Hashes.resize( new_num );
				new_ht.Entries.reserve( new_ht.Hashes.num() );

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

				// *this = new_ht;

				// old_ht.destroy();

				destroy();
				Hashes  = new_ht.Hashes;
				Entries = new_ht.Entries;
			);
			Code body = def_execution( token_fmt( tmpl, 1, "type", name ) );

			rehash = def_function( name(rehash), def_param( t_sw, name(new_num)), t_void, body, spec_inline);
		}

		Code rehash_fast;
		{
			char const* tmpl = txt(
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
			Code body = def_execution( token_fmt( tmpl, 1, "type", name ) );

			rehash_fast = def_function( name(rehash_fast), __, t_void, body );
		}

		Code remove = def_function( name(remove), def_param( t_u64, name(key)), t_void
			, def_execution( code(
				FindResult find_result = find( key);

				if ( find_result.EntryIndex >= 0 )
				{
					Entries.remove_at( find_result.EntryIndex );
					rehash_fast();
				}
			))
		);

		Code remove_entry = def_function( name(remove_entry), def_param( t_sw, name(idx)), t_void
			, def_execution( code(
				Entries.remove_at( idx );
			))
		);

		Code set;
		{
			Code params = def_params( 2
				, def_param( t_u64,  name(key))
				, def_param( t_type, name(value))
			);

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

		Code slot = def_function( name(slot), def_param( t_u64, name(key)), t_sw
			, def_execution( code(
				for ( sw idx = 0; idx < Hashes.num(); ++idx )
					if ( Hashes[ idx ] == key )
						return idx;

				return -1;
			))
		);

		Code add_entry = def_function( name(add_entry), def_param( t_u64, name(key)), t_sw
			, def_execution( code(
				sw idx;
				Entry entry = { key, -1 };

				idx = Entries.num();
				Entries.append( entry );
				return idx;
			))
		);

		Code find = def_function( name(find), def_param( t_u64, name(key)), t_find_result
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

		Code full = def_function( name(full), __, t_b32
			, def_execution( code(
				return 0.75f * Hashes.num() < Entries.num();
			))
		);

		hashtable = def_struct( name, def_struct_body( 24
			, using_entry
			, using_array_entry
			, using_find_result
			, using_map_proc
			, using_map_mut_proc

			, init

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
		));
	}

	return def_global_body( 3, ht_entry, array_ht_entry, hashtable );
}

struct GenHashTableRequest
{
	StrC Dependency;
	StrC Type;
	sw   TypeSize;
};
Array(GenHashTableRequest) GenHashTableRequests;

void gen__hashtable_request( StrC type, sw size, StrC dep = {} )
{
	do_once_start
		array_init( GenHashTableRequests, Memory::GlobalAllocator );

		gen_array( sw );
	do_once_end

	// Make sure we don't already have a request for the type.
	for ( sw idx = 0; idx < array_count( GenHashTableRequests ); ++idx )
	{
		StrC const reqest_type = GenHashTableRequests[ idx ].Type;

		if ( reqest_type.Len != type.Len )
			continue;

		if ( str_compare( reqest_type.Ptr, type.Ptr, reqest_type.Len ) == 0 )
			return;
	}

	GenHashTableRequest request = { dep, type, size};
	array_append( GenHashTableRequests, request );
}
#define gen_hashtable( type ) gen__hashtable_request( { txt_to_StrC(type) }, sizeof( type ))

u32 gen_hashtable_file()
{
	Builder
	gen_buffer_file;
	gen_buffer_file.open( "hashtable.gen.hpp" );

	gen_buffer_file.print( def_include( StrC::from("Bloat.hpp")) );
	gen_buffer_file.print( def_include( StrC::from("Array.NonParsed.hpp")) );
	gen_buffer_file.print( def_include( StrC::from("array.gen.hpp")) );

	gen_buffer_file.print( gen__hashtable_base());

	GenHashTableRequest* current = GenHashTableRequests;
	s32 left = array_count( GenHashTableRequests );
	while (left--)
	{
		GenHashTableRequest const& request = * current;

		Code generated_buffer = gen__hashtable( current->Type, current->TypeSize );

		if ( request.Dependency )
		{
			char const* cmt_str = str_fmt_buf( "// Dependency for %s type", request.Type );
			s32         cmt_len = str_len( cmt_str );

			Code cmt     = def_comment( { cmt_len, cmt_str } );
			Code include = def_include( request.Dependency );

			gen_buffer_file.print( cmt );
			gen_buffer_file.print( include );
		}

		gen_buffer_file.print( generated_buffer );
		current++;
	}

	gen_buffer_file.write();
	return 0;
}

#endif // gen_time
