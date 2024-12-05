#if GEN_SUPPORT_CPP_MEMBER_FEATURES
// These are intended for use in the base library of gencpp and the C-variant of the library
// It provides a interoperability between the C++ and C implementation of arrays. (not letting these do any crazy substiution though)
// They are undefined in gen.hpp and gen.cpp at the end of the files.
// We cpp library expects the user to use the regular calls as they can resolve the type fine.

#define array_init(type, allocator)                        array_init        <typeof(array)>(type, allocator )
#define array_init_reserve(type, allocator, cap)           array_init_reserve<typeof(array)>(type, allocator, cap)
#define array_append_array(type, array, other)             array_append      <typeof(array)>(type, & array, other )
#define array_append_value(type, array, value)             array_append      <typeof(array)>(type, & array, value )
#define array_append_items(type, array, items, item_num)   array_append      <typeof(array)>(type, & array, items, item_num )
#define array_append_at(type, array, item, idx )           array_append_at   <typeof(array)>(type, & array, item, idx )
#define array_append_at(type, array, items, item_num, idx) array_append_at   <typeof(array)>(type, & items, item_num, idx )
#define array_back(type, array )                           array_back        <typeof(array)>(type, array )
#define array_clear(type, array )                          array_clear       <typeof(array)>(type, array )
#define array_fill(type, array, begin, end, value)         array_fill        <typeof(array)>(type, array, begin, end, value )
#define array_free(type, array)                            array_free        <typeof(array)>(type, & array )
#define arary_grow(type, array, min_capacity)              arary_grow        <typeof(array)>(type, & array, min_capacity)
#define array_num(type, array)                             array_num         <typeof(array)>(type, array )
#define arary_pop(type, array)                             arary_pop         <typeof(array)>(type, array )
#define arary_remove_at(type, array, idx)                  arary_remove_at   <typeof(array)>(type, idx)
#define arary_reserve(type, array, new_capacity)           arary_reserve     <typeof(array)>(type, & array, new_capacity )
#define arary_resize(type, array, num)                     arary_resize      <typeof(array)>(type, & array, num)
#define arary_set_capacity(array, new_capacity)            arary_set_capacity<typeof(array)>(type, & array, new_capacity )
#define arary_get_header(array)                            arary_get_header  <typeof(array)>(type, array )
#endif
