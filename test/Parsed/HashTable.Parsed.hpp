#pragma once

#if gen_time
#include "gen.hpp"
#include "Array.Parsed.hpp"

using namespace gen;

Code gen__hashtable_base()
{
	return parse_global_body( code(
		struct HashTable_FindResult
		{
			sw HashIndex;
			sw PrevIndex;
			sw EntryIndex;
		};
	));
}

Code gen__hashtable( StrC type, sw type_size )
{
	StringCached name;
	{
		char const* name_str = str_fmt_buf( "HashTable_%s", type.Ptr );
		s32         len      = str_len( name_str );

		name = get_cached_string({ len, name_str });
	}

	Code ht_entry = parse_struct( token_fmt(
		txt(
			struct <HashTableName>_Entry
			{
				u64 Key;
				sw  Next;
				<type> Value;
			};
		),
		2
		, "HashTableName", (char const*) name
		, "type",          (char const*) type
	));

	StringCached ht_entry_name = get_cached_string( token_fmt( "<HashTableName>_Entry", 1, "HashTableName", name ) );

	Code array_ht_entry = gen__array( ht_entry_name );

	Code hashtable = parse_struct( token_fmt(
		txt(
			struct <HashTableName>
			{
				using Type        = <type>;
				using Entry       = <HashTableName>_Entry;
				using Array_Entry = Array_<HashTableName>_Entry;
				using FindResult  = HashTable_FindResult;
				using MapProc     = void ( * )( u64 key, Type value );
				using MapMutProc  = void ( * )( u64 key, Type* value );

				static
				<HashTableName> init( AllocatorInfo allocator )
				{
					<HashTableName>
					result         = { 0 };
					result.Hashes  = Array_sw ::init( allocator );
					result.Entries = Array_Entry::init( allocator );
					return result;
				}

				void clear( void )
				{
					for ( s32 idx = 0; idx < Hashes.num(); idx++ )
						Hashes[ idx ] = -1;

					Entries.clear();
				}

				void destroy( void )
				{
					if ( Hashes )
						Hashes.free();
					if ( Entries )
						Entries.free();
				}

				Type* get( u64 key )
				{
					sw idx = find( key ).EntryIndex;

					if ( idx > 0 )
						return &Entries[ idx ].Value;

					return nullptr;
				}

				void grow( void )
				{
					sw new_num = array_grow_formula( Entries.num() );

					rehash( new_num );
				}

				void map( MapProc map_proc )
				{
					ZPL_ASSERT_NOT_NULL( map_proc );

					for ( sw idx = 0; idx < Entries.num(); idx++ )
					{
						map_proc( Entries[ idx ].Key, Entries[ idx ].Value );
					}
				}

				void map_mut( MapMutProc map_proc )
				{
					ZPL_ASSERT_NOT_NULL( map_proc );

					for ( sw idx = 0; idx < Entries.num(); idx++ )
					{
						map_proc( Entries[ idx ].Key, &Entries[ idx ].Value );
					}
				}

				void rehash( sw new_num )
				{
					sw            idx;
					sw            last_added_index;
					HashTable_u32 new_ht = HashTable_u32::init( Hashes.get_header().Allocator );

					new_ht.Hashes.resize( new_num );
					new_ht.Entries.reserve( new_ht.Hashes.num() );

					for ( idx = 0; idx < new_ht.Hashes.num(); ++idx )
						new_ht.Hashes[ idx ] = -1;

					for ( idx = 0; idx < Entries.num(); ++idx )
					{
						Entry&     entry = Entries[ idx ];
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

					Hashes  = new_ht.Hashes;
					Entries = new_ht.Entries;
				}

				void rehash_fast( void )
				{
					sw idx;

					for ( idx = 0; idx < Entries.num(); idx++ )
						Entries[ idx ].Next = -1;

					for ( idx = 0; idx < Hashes.num(); idx++ )
						Hashes[ idx ] = -1;

					for ( idx = 0; idx < Entries.num(); idx++ )
					{
						Entry*     entry;
						FindResult find_result;
					}
				}

				void remove( u64 key )
				{
					FindResult find_result = find( key );

					if ( find_result.EntryIndex >= 0 )
					{
						Entries.remove_at( find_result.EntryIndex );
						rehash_fast();
					}
				}

				void remove_entry( sw idx )
				{
					Entries.remove_at( idx );
				}

				void set( u64 key, Type value )
				{
					sw         idx;
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
				}

				sw slot( u64 key )
				{
					for ( sw idx = 0; idx < Hashes.num(); ++idx )
						if ( Hashes[ idx ] == key )
							return idx;

					return -1;
				}

				Array_sw    Hashes;
				Array_Entry Entries;

			protected:

				sw add_entry( u64 key )
				{
					sw    idx;
					Entry entry = { key, -1 };
					idx         = Entries.num();
					Entries.append( entry );
					return idx;
				}

				HashTable_FindResult find( u64 key )
				{
					FindResult result = { -1, -1, -1 };
					if ( Hashes.num() > 0 )
					{
						result.HashIndex  = key % Hashes.num();
						result.EntryIndex = Hashes[ result.HashIndex ];

						while ( result.EntryIndex >= 0 )
						{
							if ( Entries[ result.EntryIndex ].Key == key )
								break;

							result.PrevIndex  = result.EntryIndex;
							result.EntryIndex = Entries[ result.EntryIndex ].Next;
						}
					}
					return result;
				}

				b32 full( void )
				{
					return 0.75f * Hashes.num() < Entries.num();
				}
			};
		),
		2
		, "HashTableName", (char const*) name
		, "type",          (char const*) type
	));

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
	gen_buffer_file.open( "hashtable.Parsed.gen.hpp" );

	gen_buffer_file.print( def_include( StrC::from("Bloat.hpp")) );
	gen_buffer_file.print( def_include( StrC::from("Array.Parsed.hpp")) );
	gen_buffer_file.print( def_include( StrC::from("array.Parsed.gen.hpp")) );
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
