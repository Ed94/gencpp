#pragma once

#include "../project/gen.hpp"

using namespace gen;

// Used to know what slot the array will be for generic selection
global s32 ArrayDefinitionCounter = 0;

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
#undef GEN_ASSERT
#undef rcast
#undef cast
	CodeBody result = parse_global_body( token_fmt( "array_type", (StrC)array_type, "fn", (StrC)fn, "type", (StrC)type
	, stringize(
		typedef <type>* <array_type>;

		void         <fn>_init           ( <array_type>* self, AllocatorInfo allocator );
		void         <fn>_init_reserve   ( <array_type>* self, AllocatorInfo allocator, usize capacity );
		bool         <fn>_append_array   ( <array_type>* self, <array_type> other );
		bool         <fn>_append         ( <array_type>* self, <type> value );
		bool         <fn>_append_items   ( <array_type>* self, <type>* items, usize item_num );
		bool         <fn>_append_at      ( <array_type>* self, <type> item, usize idx );
		bool         <fn>_append_items_at( <array_type>* self, <type>* items, usize item_num, usize idx );
		<type>*      <fn>_back           ( <array_type>  self );
		void         <fn>_clear          ( <array_type>  self );
		bool         <fn>_fill		     ( <array_type>  self, usize begin, usize end, <type> value );
		void         <fn>_free           ( <array_type>  self );
		bool         <fn>_grow           ( <array_type>* self, usize min_capacity );
		usize        <fn>_num            ( <array_type>  self );
		<type>       <fn>_pop 	         ( <array_type>  self );
		bool         <fn>_reserve        ( <array_type>* self, usize new_capacity );
		bool         <fn>_resize         ( <array_type>* self, usize num );
		bool         <fn>_set_capacity   ( <array_type>* self, usize new_capacity );

		void <fn>_init( <array_type>* self, AllocatorInfo allocator )
		{
			size_t initial_size = array_grow_formula(0);
			array_init_reserve( * self, allocator, initial_size );
		}

		void <fn>_init_reserve( <array_type>* self, AllocatorInfo allocator, usize capacity )
		{
			ArrayHeader* header = rcast(ArrayHeader*, alloc(allocator, sizeof(ArrayHeader) + sizeof(<type>) * capacity));

			if (header == nullptr)
				self = nullptr;

			header->Allocator = allocator;
			header->Capacity  = capacity;
			header->Num       = 0;

			self = rcast(<array_type>*, header + 1);
		}

		bool <fn>_append_array( <array_type>* self, <array_type> other )
		{
			return array_append_items( * self, (<array_type>)other, <fn>_num(other));
		}

		bool <fn>_append( <array_type>* self, <type> value )
		{
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

		bool <fn>_append_items( <array_type>* self, <type>* items, usize item_num )
		{
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

		bool <fn>_append_at( <array_type>* self, <type> item, usize idx )
		{
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

		bool <fn>_append_items_at( <array_type>* self, <type>* items, usize item_num, usize idx )
		{
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

		<type>* <fn>_back( <array_type> self )
		{
			ArrayHeader* header = array_get_header( self );

			if ( header->Num == 0 )
				return NULL;

			return self + header->Num - 1;
		}

		void <fn>_clear( <array_type> self )
		{
			ArrayHeader* header = array_get_header( self );
			header->Num = 0;
		}

		bool <fn>_fill( <array_type> self, usize begin, usize end, <type> value )
		{
			ArrayHeader* header = array_get_header( self );

			if ( begin < 0 || end >= header->Num )
				return false;

			for ( ssize idx = begin; idx < end; idx ++ )
				self[ idx ] = value;

			return true;
		}

		void <fn>_free( <array_type> self )
		{
			ArrayHeader* header = array_get_header( self );
			allocator_free( header->Allocator, header );
			self = NULL;
		}

		bool <fn>_grow( <array_type>* self, usize min_capacity )
		{
			ArrayHeader* header      = array_get_header( *self );
			usize       new_capacity = array_grow_formula( header->Capacity );

			if ( new_capacity < min_capacity )
				new_capacity = min_capacity;

			return array_set_capacity( * self, new_capacity );
		}

		usize <fn>_num( <array_type> self )
		{
			return array_get_header(self)->Num;
		}

		<type> <fn>_pop( <array_type> self )
		{
			ArrayHeader* header = array_get_header( self );
			GEN_ASSERT( header->Num > 0 );

			<type> result = self[ header->Num - 1 ];
			header->Num--;
			return result;
		}

		void <fn>_remove_at( <array_type> self, usize idx )
		{
			ArrayHeader* header = array_get_header( self );
			GEN_ASSERT( idx < header->Num );

			mem_move( self + idx, self + idx + 1, sizeof( <type> ) * ( header->Num - idx - 1 ) );
			header->Num--;
		}

		bool <fn>_reserve( <array_type>* self, usize new_capacity )
		{
			ArrayHeader* header = array_get_header( * self );

			if ( header->Capacity < new_capacity )
				return array_set_capacity( * self, new_capacity );

			return true;
		}

		bool <fn>_resize( <array_type>* self, usize num )
		{
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

		bool <fn>_set_capacity( <array_type>* self, usize new_capacity )
		{
			ArrayHeader* header = array_get_header( * self );

			if ( new_capacity == header->Capacity )
				return true;

			if ( new_capacity < header->Num )
				header->Num = new_capacity;

			usize       size        = sizeof( ArrayHeader ) + sizeof( <type> ) * new_capacity;
			ArrayHeader* new_header = cast( ArrayHeader*, alloc( header->Allocator, size ));

			if ( new_header == NULL )
				return false;

			mem_move( new_header, header, sizeof( ArrayHeader ) + sizeof( <type> ) * header->Num );
			allocator_free( header->Allocator, & header );

			new_header->Capacity = new_capacity;
			* self = cast( <type>*, new_header + 1 );
			return true;
		}
	)));
#pragma pop_macro( "GEN_ASSERT" )
#pragma pop_macro( "rcast" )
#pragma pop_macro( "cast" )

	++ ArrayDefinitionCounter;
	StrC slot_str = String::fmt_buf(GlobalAllocator, "%d", ArrayDefinitionCounter).to_strc();

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
#define GENERIC_SLOT_<slot>__array_grow            <type_delimiter>*, <type_delimiter>_grow
#define GENERIC_SLOT_<slot>__array_num             <type_delimiter>,  <type_delimiter>_num
#define GENERIC_SLOT_<slot>__array_pop             <type_delimiter>,  <type_delimiter>_pop
#define GENERIC_SLOT_<slot>__array_reserve         <type_delimiter>,  <type_delimiter>_reserve
#define GENERIC_SLOT_<slot>__array_resize          <type_delimiter>,  <type_delimiter>_resize
#define GENERIC_SLOT_<slot>__array_set_capacity    <type_delimiter>,  <type_delimiter>_set_capacity
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

constexpr bool array_by_ref = true;
Code gen_array_generic_selection_function_macro( StrC macro_name, bool by_ref = false )
{
	local_persist
	String define_builder = String::make_reserve(GlobalAllocator, kilobytes(64));
	define_builder.clear();

	StrC macro_begin = token_fmt( "macro_name", (StrC)macro_name,
R"(#define <macro_name>(selector_arg, ...) _Generic( \
	(selector_arg), /* Select Via Expression*/       \
		/* Extendibility slots: */                   \
)"
	);
	define_builder.append(macro_begin);

	for ( s32 slot = 1; slot <= ArrayDefinitionCounter; ++ slot )
	{
		StrC slot_str = String::fmt_buf(GlobalAllocator, "%d", slot).to_strc();
		if (slot == ArrayDefinitionCounter)
		{
			define_builder.append( token_fmt( "macro_name", macro_name, "slot", slot_str,
R"(		GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT_LAST(  GENERIC_SLOT_<slot>__<macro_name> ) \
)"
			));
			continue;
		}

		define_builder.append( token_fmt( "macro_name", macro_name, "slot", slot_str,
R"(		GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GENERIC_SLOT_<slot>__<macro_name> ) \
)"
		));
	}

	if (by_ref)
		define_builder.append(txt(")\tGEN_RESOLVED_FUNCTION_CALL( & selector_arg, __VA_ARGS__ )"));
	else
		define_builder.append(txt(")\tGEN_RESOLVED_FUNCTION_CALL( selector_arg, __VA_ARGS__ )"));

	// Add gap for next definition
	define_builder.append(txt("\n\n"));

	Code macro = untyped_str(define_builder.to_strc());
	return macro;
}

CodeBody gen_array_generic_selection_interface()
{
	CodeBody interface_defines = def_body(CT_Global_Body);
	interface_defines.append( gen_array_generic_selection_function_macro(txt("array_init"), array_by_ref) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("array_init_reserve"), array_by_ref) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("array_append"), array_by_ref) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("array_append_items"), array_by_ref) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("array_back")) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("array_clear")) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("array_fill")) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("array_free")) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("array_grow")) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("array_num")) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("arary_pop")) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("arary_remove_at")) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("arary_reserve"), array_by_ref) );
	interface_defines.append( gen_array_generic_selection_function_macro(txt("array_set_capacity"), array_by_ref) );
	return interface_defines;
}
