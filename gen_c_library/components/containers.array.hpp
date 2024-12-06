#pragma once

#include "../project/gen.hpp"

using namespace gen;

// Used to know what slot the array will be for generic selection
global s32 Array_DefinitionCounter = 0;

CodeBody gen_array_base()
{
	CodeTypedef td_header = parse_typedef( code( typedef struct ArrayHeader ArrayHeader; ));
	CodeStruct  header    = parse_struct( code(
		struct ArrayHeader
		{
			AllocatorInfo Allocator;
			usize         Capacity;
			usize         Num;
		};
	));

	Code grow_formula = untyped_str( txt( "#define array_grow_formula( value ) ( 2 * value + 8 )\n" ));
	Code get_header   = untyped_str( txt( "#define array_get_header( self ) ( (ArrayHeader*)( self ) - 1)\n" ));

	return def_global_body( args( fmt_newline, td_header, header, grow_formula, get_header, fmt_newline ) );
};

CodeBody gen_array( StrC type, StrC array_name )
{
	String array_type = String::fmt_buf( GlobalAllocator, "%.*s", array_name.Len, array_name.Ptr );
	String fn         = String::fmt_buf( GlobalAllocator, "%.*s", array_name.Len, array_name.Ptr );
	// str_to_lower(fn.Data);

#pragma push_macro( "GEN_ASSERT" )
#pragma push_macro( "rcast" )
#pragma push_macro( "cast" )
#pragma push_macro( "typeof" )
#pragma push_macro( "forceinline" )
#undef GEN_ASSERT
#undef rcast
#undef cast
#undef typeof
#undef forceinline
	CodeBody result = parse_global_body( token_fmt( "array_type", (StrC)array_type, "fn", (StrC)fn, "type", (StrC)type
	, stringize(
		typedef <type>* <array_type>;

		<array_type> <fn>_init           ( AllocatorInfo allocator );
		<array_type> <fn>_init_reserve   ( AllocatorInfo allocator, usize capacity );
		bool         <fn>_append_array   ( <array_type>*  self, <array_type> other );
		bool         <fn>_append         ( <array_type>*  self, <type> value );
		bool         <fn>_append_items   ( <array_type>*  self, <type>* items, usize item_num );
		bool         <fn>_append_at      ( <array_type>*  self, <type> item, usize idx );
		bool         <fn>_append_items_at( <array_type>*  self, <type>* items, usize item_num, usize idx );
		<type>*      <fn>_back           ( <array_type>   self );
		void         <fn>_clear          ( <array_type>   self );
		bool         <fn>_fill		     ( <array_type>   self, usize begin, usize end, <type> value );
		void         <fn>_free           ( <array_type>*  self );
		bool         <fn>_grow           ( <array_type>*  self, usize min_capacity );
		usize        <fn>_num            ( <array_type>   self );
		<type>       <fn>_pop 	         ( <array_type>   self );
		void         <fn>_remove_at      ( <array_type>   self, usize idx );
		bool         <fn>_reserve        ( <array_type>*  self, usize new_capacity );
		bool         <fn>_resize         ( <array_type>*  self, usize num );
		bool         <fn>_set_capacity   ( <array_type>*  self, usize new_capacity );

		forceinline
		<array_type> <fn>_init( AllocatorInfo allocator )
		{
			size_t initial_size = array_grow_formula(0);
			return array_init_reserve( <array_type>, allocator, initial_size );
		}

		inline
		<array_type> <fn>_init_reserve( AllocatorInfo allocator, usize capacity )
		{
			GEN_ASSERT(capacity > 0);
			ArrayHeader* header = rcast(ArrayHeader*, alloc(allocator, sizeof(ArrayHeader) + sizeof(<type>) * capacity));

			if (header == nullptr)
				return nullptr;

			header->Allocator = allocator;
			header->Capacity  = capacity;
			header->Num       = 0;

			return rcast(<type>*, header + 1);
		}

		forceinline
		bool <fn>_append_array( <array_type>* self, <array_type> other )
		{
			return array_append_items( * self, (<array_type>)other, <fn>_num(other));
		}

		inline
		bool <fn>_append( <array_type>* self, <type> value )
		{
			GEN_ASSERT(  self != nullptr);
			GEN_ASSERT(* self != nullptr);
			ArrayHeader* header = array_get_header( * self );

			if ( header->Num == header->Capacity )
			{
				if ( ! array_grow( self, header->Capacity))
					return false;

				header = array_get_header( * self );
			}

			(* self)[ header->Num ] = value;
			header->Num++;

			return true;
		}

		inline
		bool <fn>_append_items( <array_type>* self, <type>* items, usize item_num )
		{
			GEN_ASSERT(  self != nullptr);
			GEN_ASSERT(* self != nullptr);
			GEN_ASSERT(items != nullptr);
			GEN_ASSERT(item_num > 0);
			ArrayHeader* header = array_get_header( * self );

			if ( header->Num + item_num > header->Capacity )
			{
				if ( ! array_grow( self, header->Capacity + item_num ))
					return false;

				header = array_get_header( * self );
			}

			mem_copy( (* self) + header->Num, items, sizeof(<type>) * item_num );
			header->Num += item_num;

			return true;
		}

		inline
		bool <fn>_append_at( <array_type>* self, <type> item, usize idx )
		{
			GEN_ASSERT(  self != nullptr);
			GEN_ASSERT(* self != nullptr);
			ArrayHeader* header = array_get_header( * self );

			if ( idx >= header->Num )
				idx = header->Num - 1;

			if ( idx < 0 )
				idx = 0;

			if ( header->Capacity < header->Num + 1 )
			{
				if ( ! array_grow( self, header->Capacity + 1 ) )
					return false;

				header = array_get_header( * self );
			}

			<array_type> target = (* self) + idx;

			mem_move( target + 1, target, (header->Num - idx) * sizeof(<type>) );
			header->Num++;

			return true;
		}

		inline
		bool <fn>_append_items_at( <array_type>* self, <type>* items, usize item_num, usize idx )
		{
			GEN_ASSERT(  self != nullptr);
			GEN_ASSERT(* self != nullptr);
			ArrayHeader* header = array_get_header( * self );

			if ( idx >= header->Num )
			{
				return array_append_items( * self, items, item_num );
			}

			if ( item_num > header->Capacity )
			{
				if ( ! array_grow( self, item_num + header->Capacity ) )
					return false;

				header = array_get_header( * self );
			}

			<type>* target = (* self) + idx + item_num;
			<type>* src    = (* self) + idx;

			mem_move( target, src, (header->Num - idx) * sizeof(<type>) );
			mem_copy( src, items, item_num * sizeof(<type>) );
			header->Num += item_num;

			return true;
		}

		inline
		<type>* <fn>_back( <array_type> self )
		{
			GEN_ASSERT(self != nullptr);
			ArrayHeader* header = array_get_header( self );

			if ( header->Num == 0 )
				return NULL;

			return self + header->Num - 1;
		}

		inline
		void <fn>_clear( <array_type> self )
		{
			GEN_ASSERT(self != nullptr);
			ArrayHeader* header = array_get_header( self );
			header->Num = 0;
		}

		inline
		bool <fn>_fill( <array_type> self, usize begin, usize end, <type> value )
		{
			GEN_ASSERT(self != nullptr);
			GEN_ASSERT(begin <= end);
			ArrayHeader* header = array_get_header( self );

			if ( begin < 0 || end >= header->Num )
				return false;

			for ( ssize idx = begin; idx < end; idx ++ )
				self[ idx ] = value;

			return true;
		}

		inline
		void <fn>_free( <array_type>* self )
		{
			GEN_ASSERT(  self != nullptr);
			GEN_ASSERT(* self != nullptr);
			ArrayHeader* header = array_get_header( * self );
			allocator_free( header->Allocator, header );
			self = NULL;
		}

		inline
		bool <fn>_grow( <array_type>* self, usize min_capacity )
		{
			GEN_ASSERT(  self != nullptr);
			GEN_ASSERT(* self != nullptr);
			GEN_ASSERT( min_capacity > 0 );
			ArrayHeader* header       = array_get_header( *self );
			usize        new_capacity = array_grow_formula( header->Capacity );

			if ( new_capacity < min_capacity )
				new_capacity = min_capacity;

			return array_set_capacity( self, new_capacity );
		}

		forceinline
		usize <fn>_num( <array_type> self )
		{
			GEN_ASSERT(  self != nullptr);
			return array_get_header(self)->Num;
		}

		inline
		<type> <fn>_pop( <array_type> self )
		{
			GEN_ASSERT(  self != nullptr);
			ArrayHeader* header = array_get_header( self );
			GEN_ASSERT( header->Num > 0 );

			<type> result = self[ header->Num - 1 ];
			header->Num--;
			return result;
		}

		forceinline
		void <fn>_remove_at( <array_type> self, usize idx )
		{
			GEN_ASSERT(  self != nullptr);
			ArrayHeader* header = array_get_header( self );
			GEN_ASSERT( idx < header->Num );

			mem_move( self + idx, self + idx + 1, sizeof( <type> ) * ( header->Num - idx - 1 ) );
			header->Num--;
		}

		inline
		bool <fn>_reserve( <array_type>* self, usize new_capacity )
		{
			GEN_ASSERT(  self != nullptr);
			GEN_ASSERT(* self != nullptr);
			GEN_ASSERT(new_capacity > 0);
			ArrayHeader* header = array_get_header( * self );

			if ( header->Capacity < new_capacity )
				return array_set_capacity( self, new_capacity );

			return true;
		}

		inline
		bool <fn>_resize( <array_type>* self, usize num )
		{
			GEN_ASSERT(  self != nullptr);
			GEN_ASSERT(* self != nullptr);
			GEN_ASSERT(num > 0);
			ArrayHeader* header = array_get_header( * self );

			if ( header->Capacity < num )
			{
				if ( ! array_grow( self, num ) )
					return false;

				header = array_get_header( * self );
			}

			header->Num = num;
			return true;
		}

		inline
		bool <fn>_set_capacity( <array_type>* self, usize new_capacity )
		{
			GEN_ASSERT(  self != nullptr);
			GEN_ASSERT(* self != nullptr);
			GEN_ASSERT( new_capacity > 0 );
			ArrayHeader* header = array_get_header( * self );

			if ( new_capacity == header->Capacity )
				return true;

			if ( new_capacity < header->Num )
			{
				header->Num = new_capacity;
				return true;
			}

			usize        size       = sizeof( ArrayHeader ) + sizeof( <type> ) * new_capacity;
			ArrayHeader* new_header = cast( ArrayHeader*, alloc( header->Allocator, size ));

			if ( new_header == NULL )
				return false;

			mem_move( new_header, header, sizeof( ArrayHeader ) + sizeof( <type> ) * header->Num );
			new_header->Capacity = new_capacity;

			allocator_free( header->Allocator, & header );

			* self = cast( <type>*, new_header + 1 );
			return true;
		}
	)));
#pragma pop_macro( "GEN_ASSERT" )
#pragma pop_macro( "rcast" )
#pragma pop_macro( "cast" )
#pragma pop_macro( "typeof" )
#pragma pop_macro( "forceinline" )

	++ Array_DefinitionCounter;
	StrC slot_str = String::fmt_buf(GlobalAllocator, "%d", Array_DefinitionCounter).to_strc();

	Code generic_interface_slot = untyped_str(token_fmt( "type_delimiter", (StrC)array_type, "slot", (StrC)slot_str,
R"(#define GENERIC_SLOT_<slot>__array_init         <type_delimiter>,  <type_delimiter>_init
#define GENERIC_SLOT_<slot>__array_init_reserve    <type_delimiter>,  <type_delimiter>_init_reserve
#define GENERIC_SLOT_<slot>__array_append          <type_delimiter>,  <type_delimiter>_append
#define GENERIC_SLOT_<slot>__array_append_items    <type_delimiter>,  <type_delimiter>_append_items
#define GENERIC_SLOT_<slot>__array_append_at       <type_delimiter>,  <type_delimiter>_append_at
#define GENERIC_SLOT_<slot>__array_append_items_at <type_delimiter>,  <type_delimiter>_append_items_at
#define GENERIC_SLOT_<slot>__array_back            <type_delimiter>,  <type_delimiter>_back
#define GENERIC_SLOT_<slot>__array_clear           <type_delimiter>,  <type_delimiter>_clear
#define GENERIC_SLOT_<slot>__array_fill            <type_delimiter>,  <type_delimiter>_fill
#define GENERIC_SLOT_<slot>__array_free            <type_delimiter>,  <type_delimiter>_free
#define GENERIC_SLOT_<slot>__array_grow            <type_delimiter>*, <type_delimiter>_grow
#define GENERIC_SLOT_<slot>__array_num             <type_delimiter>,  <type_delimiter>_num
#define GENERIC_SLOT_<slot>__array_pop             <type_delimiter>,  <type_delimiter>_pop
#define GENERIC_SLOT_<slot>__array_remove_at       <type_delimiter>,  <type_delimiter>_remove_at
#define GENERIC_SLOT_<slot>__array_reserve         <type_delimiter>,  <type_delimiter>_reserve
#define GENERIC_SLOT_<slot>__array_resize          <type_delimiter>,  <type_delimiter>_resize
#define GENERIC_SLOT_<slot>__array_set_capacity    <type_delimiter>*, <type_delimiter>_set_capacity
)"
	));

	return def_global_body( args(
		def_pragma( string_to_strc( string_fmt_buf( GlobalAllocator, "region %S", array_type ))),
		fmt_newline,
		generic_interface_slot,
		fmt_newline,
		result,
		fmt_newline,
		def_pragma( string_to_strc(string_fmt_buf( GlobalAllocator, "endregion %S", array_type ))),
		fmt_newline
	));
};

CodeBody gen_array_generic_selection_interface()
{
	CodeBody interface_defines = def_body(CT_Global_Body);
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_init"), GenericSel_Direct_Type ));
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_init_reserve"), GenericSel_Direct_Type ));
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_append"), GenericSel_By_Ref ));
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_append_items"), GenericSel_By_Ref ));
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_back"), GenericSel_Default, GenericSel_One_Arg ));
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_clear"), GenericSel_Default, GenericSel_One_Arg ));
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_fill")) );
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_free"), GenericSel_By_Ref, GenericSel_One_Arg ) );
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_grow")) );
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_num"), GenericSel_Default, GenericSel_One_Arg ));
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_pop"), GenericSel_Default, GenericSel_One_Arg ));
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_remove_at")) );
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_reserve"), GenericSel_By_Ref) );
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_resize"), GenericSel_By_Ref) );
	interface_defines.append( gen_generic_selection_function_macro( Array_DefinitionCounter, txt("array_set_capacity")) );
	return interface_defines;
}
