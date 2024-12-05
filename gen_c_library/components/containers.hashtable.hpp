#pragma once

#include "../project/gen.hpp"
#include "containers.array.hpp"

using namespace gen;

CodeBody gen_hashtable_base()
{
	CodeBody struct_def = parse_global_body( code(
		typedef struct HT_FindResult HT_FindResult;
		struct HT_FindResult
		{
			ssize HashIndex;
			ssize PrevIndex;
			ssize EntryIndex;
		};
	));

	Code define_type = untyped_str(txt(
R"(#define HashTable(_type) struct _type
)"
	));

	return def_global_body(args(struct_def, define_type));
}

CodeBody gen_hashtable( StrC type, StrC hashtable_name )
{

	String tbl_type = {(char*) hashtable_name.duplicate(GlobalAllocator).Ptr};
	String fn       = tbl_type.duplicate(GlobalAllocator);
	// str_to_lower(fn.Data);

	String name_lower = String::make( GlobalAllocator, hashtable_name );
	// str_to_lower( name_lower.Data );

	String hashtable_entry   = String::fmt_buf( GlobalAllocator, "HTE_%.*s",     hashtable_name.Len, hashtable_name.Ptr );
	String entry_array_name  = String::fmt_buf( GlobalAllocator, "Arr_HTE_%.*s", hashtable_name.Len, hashtable_name.Ptr );
	String entry_array_fn_ns = String::fmt_buf( GlobalAllocator, "arr_hte_%.*s", name_lower.length(), name_lower.Data );

	CodeBody hashtable_types = parse_global_body( token_fmt(
		"type",        (StrC) type,
		"tbl_name",    (StrC) hashtable_name,
		"tbl_type",    (StrC) tbl_type,
	stringize(
		typedef struct HashTable_<type> <tbl_type>;
		typedef struct HTE_<tbl_name> HTE_<tbl_name>;
		struct HTE_<tbl_name>
		{
			u64    Key;
			ssize  Next;
			<type> Value;
		};

		typedef void (* <tbl_type>_MapProc)    ( <tbl_type> self, u64 key, <type> value );
		typedef void (* <tbl_type>_MapMutProc) ( <tbl_type> self, u64 key, <type>* value );
	)));

	CodeBody entry_array = gen_array( hashtable_entry, entry_array_name );

#pragma push_macro( "GEN_ASSERT" )
#pragma push_macro( "GEN_ASSERT_NOT_NULL" )
#undef GEN_ASSERT
#undef GEN_ASSERT_NOT_NULL
	CodeBody hashtable_def = parse_global_body( token_fmt(
		"type",           (StrC) type,
		"tbl_name",       (StrC) hashtable_name,
		"tbl_type",       (StrC) tbl_type,
		"fn",             (StrC) fn,
		"entry_type",     (StrC) hashtable_entry,
		"array_entry",    (StrC) entry_array_name,
		"fn_array",       (StrC) entry_array_fn_ns,
	stringize(
		struct HashTable_<type>
		{
			Array_ssize   Hashes;
			<array_entry> Entries;
		};

		<tbl_type> <fn>_make        ( AllocatorInfo allocator );
		<tbl_type> <fn>_make_reserve( AllocatorInfo allocator, ssize num );
		void       <fn>_clear       ( <tbl_type> self );
		void       <fn>_destroy     ( <tbl_type> self );
		<type>*    <fn>_get         ( <tbl_type> self, u64 key );
		void       <fn>_map         ( <tbl_type> self, <tbl_type>_MapProc map_proc );
		void       <fn>_map_mut     ( <tbl_type> self, <tbl_type>_MapMutProc map_proc );
		void       <fn>_grow        ( <tbl_type>* self );
		void       <fn>_rehash      ( <tbl_type>* self, ssize new_num );
		void       <fn>_rehash_fast ( <tbl_type> self );
		void       <fn>_remove      ( <tbl_type> self, u64 key );
		void       <fn>_remove_entry( <tbl_type> self, ssize idx );
		void       <fn>_set         ( <tbl_type>* self, u64 key, <type> value );
		ssize      <fn>_slot        ( <tbl_type> self, u64 key );

		ssize            <fn>__add_entry( <tbl_type> self, u64 key );
		HT_FindResult <fn>__find     ( <tbl_type> self, u64 key );
		b32           <fn>__full     ( <tbl_type> self );

		<tbl_type> <fn>_make( AllocatorInfo allocator )
		{
			<tbl_type>
			result        = { NULL, NULL };
			array_init(result.Hashes, allocator );
			array_init(result.Entries, allocator );

			return result;
		}

		<tbl_type> <fn>_make_reserve( AllocatorInfo allocator, ssize num )
		{
			<tbl_type>
			result         = { NULL, NULL };
			array_init_reserve(result.Hashes, allocator, num );
			array_init_reserve(result.Entries, allocator, num );

			return result;
		}

		void <fn>_clear( <tbl_type> self )
		{
			for ( ssize idx = 0; idx < array_get_header( self.Hashes )->Num; idx++ )
				self.Hashes[idx] = -1;

			array_clear( self.Hashes );
			array_clear( self.Entries );
		}

		void <fn>_destroy( <tbl_type> self )
		{
			if ( self.Hashes && self.Entries )
			{
				array_free( self.Hashes );
				array_free( self.Entries );
			}
		}

		<type>* <fn>_get( <tbl_type> self, u64 key )
		{
			ssize idx = <fn>__find( self, key ).EntryIndex;
			if ( idx > 0 )
				return & self.Entries[idx].Value;

			return NULL;
		}

		void <fn>_map( <tbl_type> self, <tbl_type>_MapProc map_proc )
		{
			GEN_ASSERT_NOT_NULL( map_proc );

			for ( ssize idx = 0; idx < array_get_header( self.Entries )->Num; idx++ )
			{
				map_proc( self, self.Entries[idx].Key, self.Entries[idx].Value );
			}
		}

		void <fn>_map_mut( <tbl_type> self, <tbl_type>_MapMutProc map_proc )
		{
			GEN_ASSERT_NOT_NULL( map_proc );

			for ( ssize idx = 0; idx < array_get_header( self.Entries )->Num; idx++ )
			{
				map_proc( self, self.Entries[idx].Key, & self.Entries[idx].Value );
			}
		}

		void <fn>_grow( <tbl_type>* self )
		{
			ssize new_num = array_grow_formula( array_get_header( self->Entries )->Num );
			<fn>_rehash( self, new_num );
		}

		void <fn>_rehash( <tbl_type>* self, ssize new_num )
		{
			ssize idx;
			ssize last_added_index;

			ArrayHeader* old_hash_header    = array_get_header( self->Hashes );
			ArrayHeader* old_entries_header = array_get_header( self->Entries );

			<tbl_type> new_tbl = <fn>_make_reserve( old_hash_header->Allocator, old_hash_header->Num );

			ArrayHeader* new_hash_header = array_get_header( new_tbl.Hashes );

			for ( idx = 0; idx < new_hash_header->Num; idx++ )
				new_tbl.Hashes[idx] = -1;

			for ( idx = 0; idx < old_entries_header->Num; idx++ )
			{
				<entry_type>*  entry;
				HT_FindResult  find_result;

				if ( new_hash_header->Num == 0 )
					<fn>_grow( & new_tbl );

				entry            = & self->Entries[ idx ];
				find_result      = <fn>__find( new_tbl, entry->Key );
				last_added_index = <fn>__add_entry( new_tbl, entry->Key );

				if ( find_result.PrevIndex < 0 )
					new_tbl.Hashes[ find_result.HashIndex ] = last_added_index;
				else
					new_tbl.Entries[ find_result.PrevIndex ].Next = last_added_index;

				new_tbl.Entries[ last_added_index ].Next  = find_result.EntryIndex;
				new_tbl.Entries[ last_added_index ].Value = entry->Value;
			}

			<fn>_destroy( *self );
			* self = new_tbl;
		}

		void <fn>_rehash_fast( <tbl_type> self )
		{
			ssize idx;

			for ( idx = 0; idx < array_get_header( self.Entries )->Num; idx++ )
				self.Entries[ idx ].Next = -1;

			for ( idx = 0; idx < array_get_header( self.Hashes )->Num; idx++ )
				self.Hashes[ idx ] = -1;

			for ( idx = 0; idx < array_get_header( self.Entries )->Num; idx++ )
			{
				<entry_type>*     entry;
				HT_FindResult find_result;

				entry       = & self.Entries[ idx ];
				find_result = <fn>__find( self, entry->Key );

				if ( find_result.PrevIndex < 0 )
					self.Hashes[ find_result.HashIndex ] = idx;
				else
					self.Entries[ find_result.PrevIndex ].Next = idx;
			}
		}

		void <fn>_remove( <tbl_type> self, u64 key )
		{
			HT_FindResult find_result = <fn>__find( self, key );

			if ( find_result.EntryIndex >= 0 )
			{
				array_remove_at( self.Entries, find_result.EntryIndex );
				<fn>_rehash_fast( self );
			}
		}

		void <fn>_remove_entry( <tbl_type> self, ssize idx )
		{
			array_remove_at( self.Entries, idx );
		}

		void <fn>_set( <tbl_type>* self, u64 key, <type> value )
		{
			ssize         idx;
			HT_FindResult find_result;

			if ( array_get_header( self->Hashes )->Num == 0 )
				<fn>_grow( self );

			find_result = <fn>__find( * self, key );

			if ( find_result.EntryIndex >= 0 )
			{
				idx = find_result.EntryIndex;
			}
			else
			{
				idx = <fn>__add_entry( * self, key );

				if ( find_result.PrevIndex >= 0 )
				{
					self->Entries[ find_result.PrevIndex ].Next = idx;
				}
				else
				{
					self->Hashes[ find_result.HashIndex ] = idx;
				}
			}

			self->Entries[ idx ].Value = value;

			if ( <fn>__full( * self ) )
				<fn>_grow( self );
		}

		ssize <fn>_slot( <tbl_type> self, u64 key )
		{
			for ( ssize idx = 0; idx < array_get_header( self.Hashes )->Num; ++idx )
				if ( self.Hashes[ idx ] == key )
					return idx;

			return -1;
		}

		ssize <fn>__add_entry( <tbl_type> self, u64 key )
		{
			ssize idx;
			<entry_type> entry = { key, -1 };

			idx = array_get_header( self.Entries )->Num;
			array_append( self.Entries, entry );
			return idx;
		}

		HT_FindResult <fn>__find( <tbl_type> self, u64 key )
		{
			HT_FindResult result = { -1, -1, -1 };

			ArrayHeader* hash_header = array_get_header( self.Hashes );

			if ( hash_header->Num > 0 )
			{
				result.HashIndex  = key % hash_header->Num;
				result.EntryIndex = self.Hashes[ result.HashIndex ];

				while ( result.EntryIndex >= 0 )
				{
					if ( self.Entries[ result.EntryIndex ].Key == key )
						break;

					result.PrevIndex  = result.EntryIndex;
					result.EntryIndex = self.Entries[ result.EntryIndex ].Next;
				}
			}

			return result;
		}

		b32 <fn>__full( <tbl_type> self )
		{
			ArrayHeader* hash_header    = array_get_header( self.Hashes );
			ArrayHeader* entries_header = array_get_header( self.Entries );

			return 0.75f * hash_header->Num < entries_header->Num;
		}
	)));
#pragma pop_macro( "GEN_ASSERT" )
#pragma pop_macro( "GEN_ASSERT_NOT_NULL" )

	char const* cmt_str = str_fmt_buf( "Name: %.*s Type: %.*s"
		, tbl_type.length(), tbl_type.Data
		, type.Len, type.Ptr );

	return def_global_body(args(
		def_pragma( string_to_strc( string_fmt_buf( GlobalAllocator, "region %S", tbl_type ))),
		fmt_newline,
		hashtable_types,
		fmt_newline,
		entry_array,
		hashtable_def,
		fmt_newline,
		def_pragma( string_to_strc( string_fmt_buf( GlobalAllocator, "endregion %S", tbl_type ))),
		fmt_newline
	));
}
