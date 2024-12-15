#pragma once
#include "gen.hpp"
#include "containers.array.hpp"

using namespace gen;

global s32 HashTable_DefinitionCounter = 0;

CodeBody gen_hashtable_base()
{
	CodeBody struct_def = parse_global_body( code(
		typedef struct HT_FindResult_Def HT_FindResult;
		struct HT_FindResult_Def
		{
			ssize HashIndex;
			ssize PrevIndex;
			ssize EntryIndex;
		};
	));

	Code define_type = untyped_str(txt(
R"(#define HashTable(_type) struct gen_HashTable_##_type
)"
	));

	Code define_critical_load_scale = untyped_str(txt("#define HashTable_CriticalLoadScale 0.7f\n"));
	return def_global_body(args(struct_def, define_type, define_critical_load_scale));
}

CodeBody gen_hashtable( Str type, Str hashtable_name )
{

	StrBuilder tbl_type = {(char*) hashtable_name.duplicate(_ctx->Allocator_Temp).Ptr};
	StrBuilder fn       = tbl_type.duplicate(_ctx->Allocator_Temp);
	// c_str_to_lower(fn.Data);

	StrBuilder name_lower = StrBuilder::make( _ctx->Allocator_Temp, hashtable_name );
	// c_str_to_lower( name_lower.Data );

	StrBuilder hashtable_entry   = StrBuilder::fmt_buf( _ctx->Allocator_Temp, "HTE_%.*s",     hashtable_name.Len, hashtable_name.Ptr );
	StrBuilder entry_array_name  = StrBuilder::fmt_buf( _ctx->Allocator_Temp, "Arr_HTE_%.*s", hashtable_name.Len, hashtable_name.Ptr );
	StrBuilder entry_array_fn_ns = StrBuilder::fmt_buf( _ctx->Allocator_Temp, "arr_hte_%.*s", name_lower.length(), name_lower.Data );

	CodeBody hashtable_types = parse_global_body( token_fmt(
		"type",        (Str) type,
		"tbl_name",    (Str) hashtable_name,
		"tbl_type",    (Str) tbl_type,
	stringize(
		typedef struct HashTable_<type> <tbl_type>;
		typedef struct HTE_<tbl_name> HTE_<tbl_name>;
		struct HTE_<tbl_name> {
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
#pragma push_macro( "rcast" )
#pragma push_macro( "cast" )
#pragma push_macro( "typeof" )
#pragma push_macro( "forceinline" )
#undef GEN_ASSERT
#undef GEN_ASSERT_NOT_NULL
#undef GEN_ASSERT
#undef rcast
#undef cast
#undef typeof
#undef forceinline
	CodeBody hashtable_def = parse_global_body( token_fmt(
		"type",           (Str) type,
		"tbl_name",       (Str) hashtable_name,
		"tbl_type",       (Str) tbl_type,
		"fn",             (Str) fn,
		"entry_type",     (Str) hashtable_entry,
		"array_entry",    (Str) entry_array_name,
		"fn_array",       (Str) entry_array_fn_ns,
	stringize(
		struct HashTable_<type> {
			Array_ssize   Hashes;
			<array_entry> Entries;
		};

		<tbl_type> <fn>_init        ( AllocatorInfo allocator );
		<tbl_type> <fn>_init_reserve( AllocatorInfo allocator, ssize num );
		void       <fn>_clear       ( <tbl_type>  self );
		void       <fn>_destroy     ( <tbl_type>* self );
		<type>*    <fn>_get         ( <tbl_type>  self, u64 key );
		void       <fn>_map         ( <tbl_type>  self, <tbl_type>_MapProc map_proc );
		void       <fn>_map_mut     ( <tbl_type>  self, <tbl_type>_MapMutProc map_proc );
		void       <fn>_grow        ( <tbl_type>* self );
		void       <fn>_rehash      ( <tbl_type>* self, ssize new_num );
		void       <fn>_rehash_fast ( <tbl_type>  self );
		void       <fn>_remove      ( <tbl_type>  self, u64 key );
		void       <fn>_remove_entry( <tbl_type>  self, ssize idx );
		void       <fn>_set         ( <tbl_type>* self, u64 key, <type> value );
		ssize      <fn>_slot        ( <tbl_type>  self, u64 key );

		ssize         <fn>__add_entry( <tbl_type>* self, u64 key );
		HT_FindResult <fn>__find     ( <tbl_type>  self, u64 key );
		b32           <fn>__full     ( <tbl_type>  self );

		<tbl_type> <fn>_init( AllocatorInfo allocator )
		{
			<tbl_type> result = hashtable_init_reserve(<type>, allocator, 8);
			return result;
		}

		<tbl_type> <fn>_init_reserve( AllocatorInfo allocator, ssize num )
		{
			<tbl_type> result = { NULL, NULL };
			result.Hashes  = array_init_reserve(ssize, allocator, num );
			array_get_header(result.Hashes)->Num = num;
			array_resize(result.Hashes, num);
			array_fill(result.Hashes, 0, num, -1);

			result.Entries = array_init_reserve(<entry_type>, allocator, num );
			return result;
		}

		void <fn>_clear( <tbl_type> self )
		{
			GEN_ASSERT_NOT_NULL(self.Hashes);
			GEN_ASSERT_NOT_NULL(self.Entries);
			array_clear( self.Entries );
			s32 what = array_num(self.Hashes);
			array_fill( self.Hashes, 0, what, (ssize)-1 );
		}

		void <fn>_destroy( <tbl_type>* self )
		{
			GEN_ASSERT_NOT_NULL(self);
			GEN_ASSERT_NOT_NULL(self->Hashes);
			GEN_ASSERT_NOT_NULL(self->Entries);
			if ( self->Hashes && array_get_header(self->Hashes)->Capacity) {
				array_free( self->Hashes );
				array_free( self->Entries );
			}
		}

		<type>* <fn>_get( <tbl_type> self, u64 key )
		{
			GEN_ASSERT_NOT_NULL(self.Hashes);
			GEN_ASSERT_NOT_NULL(self.Entries);
			ssize idx = <fn>__find( self, key ).EntryIndex;
			if ( idx > 0 )
				return & self.Entries[idx].Value;

			return nullptr;
		}

		void <fn>_map( <tbl_type> self, <tbl_type>_MapProc map_proc )
		{
			GEN_ASSERT_NOT_NULL(self.Hashes);
			GEN_ASSERT_NOT_NULL(self.Entries);
			GEN_ASSERT_NOT_NULL( map_proc );

			for ( ssize idx = 0; idx < array_get_header( self.Entries )->Num; idx++ ) {
				map_proc( self, self.Entries[idx].Key, self.Entries[idx].Value );
			}
		}

		void <fn>_map_mut( <tbl_type> self, <tbl_type>_MapMutProc map_proc )
		{
			GEN_ASSERT_NOT_NULL(self.Hashes);
			GEN_ASSERT_NOT_NULL(self.Entries);
			GEN_ASSERT_NOT_NULL( map_proc );

			for ( ssize idx = 0; idx < array_get_header( self.Entries )->Num; idx++ ) {
				map_proc( self, self.Entries[idx].Key, & self.Entries[idx].Value );
			}
		}

		void <fn>_grow( <tbl_type>* self )
		{
			GEN_ASSERT_NOT_NULL(self);
			GEN_ASSERT_NOT_NULL(self->Hashes);
			GEN_ASSERT_NOT_NULL(self->Entries);
			ssize new_num = array_grow_formula( array_get_header( self->Entries )->Num );
			hashtable_rehash( self, new_num );
		}

		void <fn>_rehash( <tbl_type>* self, ssize new_num )
		{
			GEN_ASSERT_NOT_NULL(self);
			GEN_ASSERT_NOT_NULL(self->Hashes);
			GEN_ASSERT_NOT_NULL(self->Entries);
			GEN_ASSERT( new_num > 0 );
			ssize idx;
			ssize last_added_index;

			ArrayHeader* old_hash_header    = array_get_header( self->Hashes );
			ArrayHeader* old_entries_header = array_get_header( self->Entries );

			<tbl_type> new_tbl = hashtable_init_reserve( <type>, old_hash_header->Allocator, old_hash_header->Num );

			ArrayHeader* new_hash_header = array_get_header( new_tbl.Hashes );

			for (ssize idx = 0; idx < cast(ssize, old_hash_header->Num); ++idx)
			{
				<entry_type>* entry = & self->Entries[idx];
				HT_FindResult find_result;

				find_result      = <fn>__find( new_tbl, entry->Key);
				last_added_index = <fn>__add_entry( & new_tbl, entry->Key);

				if (find_result.PrevIndex < 0)
					new_tbl.Hashes[find_result.HashIndex] = last_added_index;
				else
					new_tbl.Entries[find_result.PrevIndex].Next = last_added_index;

				new_tbl.Entries[last_added_index].Next = find_result.EntryIndex;
				new_tbl.Entries[last_added_index].Value = entry->Value;
			}

			<fn>_destroy( self );
			* self = new_tbl;
		}

		void <fn>_rehash_fast( <tbl_type> self )
		{
			GEN_ASSERT_NOT_NULL(self.Hashes);
			GEN_ASSERT_NOT_NULL(self.Entries);
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
			GEN_ASSERT_NOT_NULL(self.Hashes);
			GEN_ASSERT_NOT_NULL(self.Entries);
			HT_FindResult find_result = <fn>__find( self, key );

			if ( find_result.EntryIndex >= 0 ) {
				array_remove_at( self.Entries, find_result.EntryIndex );
				hashtable_rehash_fast( self );
			}
		}

		void <fn>_remove_entry( <tbl_type> self, ssize idx )
		{
			GEN_ASSERT_NOT_NULL(self.Hashes);
			GEN_ASSERT_NOT_NULL(self.Entries);
			array_remove_at( self.Entries, idx );
		}

		void <fn>_set( <tbl_type>* self, u64 key, <type> value )
		{
			GEN_ASSERT_NOT_NULL(self);
			GEN_ASSERT_NOT_NULL(self->Hashes);
			GEN_ASSERT_NOT_NULL(self->Entries);
			ssize         idx;
			HT_FindResult find_result;

			if ( array_get_header( self->Hashes )->Num == 0 )
				hashtable_grow( self );

			find_result = <fn>__find( * self, key );

			if ( find_result.EntryIndex >= 0 ) {
				idx = find_result.EntryIndex;
			}
			else
			{
				idx = <fn>__add_entry( self, key );

				if ( find_result.PrevIndex >= 0 ) {
					self->Entries[ find_result.PrevIndex ].Next = idx;
				}
				else {
					self->Hashes[ find_result.HashIndex ] = idx;
				}
			}

			self->Entries[ idx ].Value = value;

			if ( <fn>__full( * self ) )
				hashtable_grow( self );
		}

		ssize <fn>_slot( <tbl_type> self, u64 key )
		{
			GEN_ASSERT_NOT_NULL(self.Hashes);
			GEN_ASSERT_NOT_NULL(self.Entries);
			for ( ssize idx = 0; idx < array_get_header( self.Hashes )->Num; ++idx )
				if ( self.Hashes[ idx ] == key )
					return idx;

			return -1;
		}

		ssize <fn>__add_entry( <tbl_type>* self, u64 key )
		{
			GEN_ASSERT_NOT_NULL(self);
			GEN_ASSERT_NOT_NULL(self->Hashes);
			GEN_ASSERT_NOT_NULL(self->Entries);
			ssize idx;
			<entry_type> entry = { key, -1 };

			idx = array_get_header( self->Entries )->Num;
			array_append( self->Entries, entry );
			return idx;
		}

		HT_FindResult <fn>__find( <tbl_type> self, u64 key )
		{
			GEN_ASSERT_NOT_NULL(self.Hashes);
			GEN_ASSERT_NOT_NULL(self.Entries);
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
			GEN_ASSERT_NOT_NULL(self.Hashes);
			GEN_ASSERT_NOT_NULL(self.Entries);
			ArrayHeader* hash_header    = array_get_header( self.Hashes );
			ArrayHeader* entries_header = array_get_header( self.Entries );

			usize critical_load = cast(usize, HashTable_CriticalLoadScale * cast(f32, hash_header->Num));
			b32 result = entries_header->Num > critical_load;
			return result;
		}
	)));
#pragma pop_macro( "GEN_ASSERT" )
#pragma pop_macro( "GEN_ASSERT_NOT_NULL" )
#pragma pop_macro( "rcast" )
#pragma pop_macro( "cast" )
#pragma pop_macro( "typeof" )
#pragma pop_macro( "forceinline" )

	++ HashTable_DefinitionCounter;
	Str slot_str = StrBuilder::fmt_buf(_ctx->Allocator_Temp, "%d", HashTable_DefinitionCounter).to_str();

	Code generic_interface_slot = untyped_str(token_fmt( "type", type, "tbl_type", (Str)tbl_type, "slot", (Str)slot_str,
R"(#define GENERIC_SLOT_<slot>__hashtable_init          <type>,      <tbl_type>_init
#define GENERIC_SLOT_<slot>__hashtable_init_reserve     <type>,      <tbl_type>_init_reserve
#define GENERIC_SLOT_<slot>__hashtable_clear            <tbl_type>,  <tbl_type>_clear
#define GENERIC_SLOT_<slot>__hashtable_destroy          <tbl_type>,  <tbl_type>_destroy
#define GENERIC_SLOT_<slot>__hashtable_get              <tbl_type>,  <tbl_type>_get
#define GENERIC_SLOT_<slot>__hashtable_map              <tbl_type>,  <tbl_type>_map
#define GENERIC_SLOT_<slot>__hashtable_map_mut          <tbl_type>,  <tbl_type>_map_mut
#define GENERIC_SLOT_<slot>__hashtable_grow             <tbl_type>*, <tbl_type>_grow
#define GENERIC_SLOT_<slot>__hashtable_rehash           <tbl_type>*, <tbl_type>_rehash
#define GENERIC_SLOT_<slot>__hashtable_rehash_fast      <tbl_type>,  <tbl_type>_rehash_fast
#define GENERIC_SLOT_<slot>__hashtable_remove_entry     <tbl_type>,  <tbl_type>_remove_entry
#define GENERIC_SLOT_<slot>__hashtable_set              <tbl_type>,  <tbl_type>_set
#define GENERIC_SLOT_<slot>__hashtable_slot             <tbl_type>,  <tbl_type>_slot

#define GENERIC_SLOT_<slot>__hashtable__add_entry       <tbl_type>*, <tbl_type>__add_entry
#define GENERIC_SLOT_<slot>__hashtable__find            <tbl_type>,  <tbl_type>__find
#define GENERIC_SLOT_<slot>__hashtable__full            <tbl_type>,  <tbl_type>__full
)"
	));

	char const* cmt_str = c_str_fmt_buf( "Name: %.*s Type: %.*s"
		, tbl_type.length(), tbl_type.Data
		, type.Len, type.Ptr );

	return def_global_body(args(
		def_pragma( strbuilder_to_str( strbuilder_fmt_buf( _ctx->Allocator_Temp, "region %SB", tbl_type ))),
		fmt_newline,
		generic_interface_slot,
		fmt_newline,
		hashtable_types,
		fmt_newline,
		entry_array,
		hashtable_def,
		fmt_newline,
		def_pragma( strbuilder_to_str( strbuilder_fmt_buf( _ctx->Allocator_Temp, "endregion %SB", tbl_type ))),
		fmt_newline
	));
}

CodeBody gen_hashtable_generic_selection_interface()
{
	CodeBody interface_defines = def_body(CT_Global_Body);
	interface_defines.append( gen_generic_selection_function_macro( HashTable_DefinitionCounter, txt("hashtable_init"), GenericSel_Direct_Type ));
	interface_defines.append( gen_generic_selection_function_macro( HashTable_DefinitionCounter, txt("hashtable_init_reserve"), GenericSel_Direct_Type ));
	interface_defines.append( gen_generic_selection_function_macro( HashTable_DefinitionCounter, txt("hashtable_clear"), GenericSel_Default, GenericSel_One_Arg ));
	interface_defines.append( gen_generic_selection_function_macro( HashTable_DefinitionCounter, txt("hashtable_destroy"), GenericSel_By_Ref, GenericSel_One_Arg ) );
	interface_defines.append( gen_generic_selection_function_macro( HashTable_DefinitionCounter, txt("hashtable_get") ));
	interface_defines.append( gen_generic_selection_function_macro( HashTable_DefinitionCounter, txt("hashtable_grow"), GenericSel_Default, GenericSel_One_Arg ));
	interface_defines.append( gen_generic_selection_function_macro( HashTable_DefinitionCounter, txt("hashtable_rehash") ));
	interface_defines.append( gen_generic_selection_function_macro( HashTable_DefinitionCounter, txt("hashtable_rehash_fast"), GenericSel_Default, GenericSel_One_Arg ));
	interface_defines.append( gen_generic_selection_function_macro( HashTable_DefinitionCounter, txt("hashtable_remove") ));
	interface_defines.append( gen_generic_selection_function_macro( HashTable_DefinitionCounter, txt("hashtable_remove_entry") ));
	interface_defines.append( gen_generic_selection_function_macro( HashTable_DefinitionCounter, txt("hashtable_set"), GenericSel_By_Ref ));
	interface_defines.append( gen_generic_selection_function_macro( HashTable_DefinitionCounter, txt("hashtable_slot") ));
	return interface_defines;
}
