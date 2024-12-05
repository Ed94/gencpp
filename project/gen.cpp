#include "helpers/push_ignores.inline.hpp"

// ReSharper disable CppClangTidyClangDiagnosticSwitchEnum

#if ! defined(GEN_DONT_ENFORCE_GEN_TIME_GUARD) && ! defined(GEN_TIME)
#	error Gen.hpp : GEN_TIME not defined
#endif

#include "gen.hpp"

// These are intended for use in the base library of gencpp and the C-variant of the library
// It provides a interoperability between the C++ and C interfacing for containers. (not letting these do any crazy substiution though)
// They are undefined in gen.hpp and gen.cpp at the end of the files.
// We cpp library expects the user to use the regular calls as they can resolve the type fine.

#define array_init(type, allocator)                        array_init           <type>                      (allocator )
#define array_init_reserve(type, allocator, cap)           array_init_reserve   <type>                      (allocator, cap)
#define array_append_array(array, other)                   array_append         < get_array_underlying_type(array) > (& array, other )
#define array_append_value(array, value)                   array_append         < get_array_underlying_type(array) > (& array, value )
#define array_append_items(array, items, item_num)         array_append         < get_array_underlying_type(array) > (& array, items, item_num )
#define array_append_at(array, item, idx )                 array_append_at      < get_array_underlying_type(array) > (& array, item, idx )
#define array_append_at_items(array, items, item_num, idx) array_append_at_items< get_array_underlying_type(array) > (& items, item_num, idx )
#define array_back(array)                                  array_back           < get_array_underlying_type(array) > (array )
#define array_clear(array)                                 array_clear          < get_array_underlying_type(array) > (array )
#define array_fill(array, begin, end, value)               array_fill           < get_array_underlying_type(array) > (array, begin, end, value )
#define array_free(array)                                  array_free           < get_array_underlying_type(array) > (& array )
#define arary_grow(array, min_capacity)                    arary_grow           < get_array_underlying_type(array) > (& array, min_capacity)
#define array_num(array)                                   array_num            < get_array_underlying_type(array) > (array )
#define arary_pop(array)                                   arary_pop            < get_array_underlying_type(array) > (array )
#define arary_remove_at(array, idx)                        arary_remove_at      < get_array_underlying_type(array) > (idx)
#define arary_reserve(array, new_capacity)                 arary_reserve        < get_array_underlying_type(array) > (& array, new_capacity )
#define arary_resize(array, num)                           arary_resize         < get_array_underlying_type(array) > (& array, num)
#define arary_set_capacity(new_capacity)                   arary_set_capacity   < get_array_underlying_type(array) > (& array, new_capacity )
#define arary_get_header(array)                            arary_get_header     < get_array_underlying_type(array) > (array )

#define hashtable_init(type, allocator)              hashtable_init        <type>(allocator)
#define hashtable_init_reserve(type, allocator, num) hashtable_init_reserve<type>(allocator, num)
#define hashtable_clear(table)                       hashtable_clear       (table)
#define hashtable_destroy(table)                     hashtable_destroy     (& table)
#define hashtable_get(table, key)                    hashtable_get         (table, key)
#define hashtable_grow(table)                        hashtable_grow        (& table)
#define hashtable_rehash(table, new_num)             hashtable_rehash      (& table, new_num)
#define hashtable_rehash_fast(table)                 hashtable_rehash_fast (table)
#define hashtable_remove(table, key)                 hashtable_remove      (table, key)
#define hashtable_remove_entry(table, idx)           hashtable_remove_entry(table, idx)
#define hashtable_set(table, key, value)             hashtable_set         (& table, key, value)
#define hashtable_slot(table, key)                   hashtable_slot        (table, key)
#define hashtable_add_entry(table, key)              hashtable_add_entry   (& table, key)
#define hashtable_find(table, key)                   hashtable_find        (table, key)
#define hashtable_full(table)                        hashtable_full        (table)
#define hashtable_map(table, map_proc)               hashtable_map         (table, map_proc)
#define hashtable_map_mut(table, map_proc)           hashtable_map_mut     (table, map_proc)

//! If its desired to roll your own dependencies, define GEN_ROLL_OWN_DEPENDENCIES before including this file.
//! Dependencies are derived from the c-zpl library: https://github.com/zpl-c/zpl
#ifndef GEN_ROLL_OWN_DEPENDENCIES
#	include "gen.dep.cpp"
#endif

GEN_NS_BEGIN

#include "components/static_data.cpp"

#include "components/ast_case_macros.cpp"
#include "components/ast.cpp"
#include "components/code_serialization.cpp"

#include "components/interface.cpp"
#include "components/interface.upfront.cpp"
#include "components/gen/etoktype.cpp"
#include "components/lexer.cpp"
#include "components/parser.cpp"
#include "components/interface.parsing.cpp"
#include "components/interface.untyped.cpp"

GEN_NS_END

#include "helpers/pop_ignores.inline.hpp"

#undef array_init
#undef array_init_reserve
#undef array_append_array
#undef array_append_value
#undef array_append_items
#undef array_append_at
#undef array_append_at
#undef array_back
#undef array_clear
#undef array_fill
#undef array_free
#undef arary_grow
#undef array_num
#undef arary_pop
#undef arary_remove_at
#undef arary_reserve
#undef arary_resize
#undef arary_set_capacity
#undef arary_get_header

#undef hashtable_init
#undef hashtable_init_reserve
#undef hashtable_clear
#undef hashtable_destroy
#undef hashtable_get
#undef hashtable_grow
#undef hashtable_rehash
#undef hashtable_rehash_fast
#undef hashtable_remove
#undef hashtable_remove_entry
#undef hashtable_set
#undef hashtable_slot
#undef hashtable_add_entry
#undef hashtable_find
#undef hashtable_full
#undef hashtable_map
#undef hashtable_map_mut
