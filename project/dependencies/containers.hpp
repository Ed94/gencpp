#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "printing.hpp"
#endif

#pragma region Containers

template<class TType>             struct RemoveConst                    { typedef TType Type;       };
template<class TType>             struct RemoveConst<const TType>       { typedef TType Type;       };
template<class TType>             struct RemoveConst<const TType[]>     { typedef TType Type[];     };
template<class TType, usize Size> struct RemoveConst<const TType[Size]> { typedef TType Type[Size]; };

template<class TType> using TRemoveConst = typename RemoveConst<TType>::Type;

template <class TType> struct RemovePtr         { typedef TType Type; };
template <class TType> struct RemovePtr<TType*> { typedef TType Type; };

template <class TType> using TRemovePtr = typename RemovePtr<TType>::Type;


#pragma region Array
#define Array(Type) Array<Type>

// #define array_init(Type, ...)         array_init        <Type>(__VA_ARGS__)
// #define array_init_reserve(Type, ...) array_init_reserve<Type>(__VA_ARGS__)

struct ArrayHeader;

#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	template<class Type> struct Array;
#	define get_array_underlying_type(array) typename TRemovePtr<typeof(array)>:: DataType
#else
	template<class Type> using Array = Type*;
#	define get_array_underlying_type(array) TRemovePtr<typeof(array)>
#endif

usize array_grow_formula(ssize value);

template<class Type> Array<Type>  array_init           (AllocatorInfo allocator);
template<class Type> Array<Type>  array_init_reserve   (AllocatorInfo allocator, ssize capacity);
template<class Type> bool         array_append_array   (Array<Type>* array, Array<Type> other);
template<class Type> bool         array_append         (Array<Type>* array, Type value);
template<class Type> bool         array_append_items   (Array<Type>* array, Type* items, usize item_num);
template<class Type> bool         array_append_at      (Array<Type>* array, Type item, usize idx);
template<class Type> bool         array_append_items_at(Array<Type>* array, Type* items, usize item_num, usize idx);
template<class Type> Type*        array_back           (Array<Type>  array);
template<class Type> void         array_clear          (Array<Type>  array);
template<class Type> bool         array_fill           (Array<Type>  array, usize begin, usize end, Type value);
template<class Type> void         array_free           (Array<Type>* array);
template<class Type> bool         arary_grow           (Array<Type>* array, usize min_capacity);
template<class Type> usize        array_num            (Array<Type>  array);
template<class Type> void         arary_pop            (Array<Type>  array);
template<class Type> void         arary_remove_at      (Array<Type>  array, usize idx);
template<class Type> bool         arary_reserve        (Array<Type>* array, usize new_capacity);
template<class Type> bool         arary_resize         (Array<Type>* array, usize num);
template<class Type> bool         arary_set_capacity   (Array<Type>* array, usize new_capacity);
template<class Type> ArrayHeader* arary_get_header     (Array<Type>  array);

struct ArrayHeader {
	AllocatorInfo Allocator;
	usize         Capacity;
	usize         Num;
};

#if GEN_SUPPORT_CPP_MEMBER_FEATURES
template<class Type>
struct Array
{
	Type* Data;

#pragma region Member Mapping
	forceinline static Array  init(AllocatorInfo allocator)                         { return GEN_NS array_init<Type>(allocator); }
	forceinline static Array  init_reserve(AllocatorInfo allocator, ssize capacity) { return GEN_NS array_init_reserve<Type>(allocator, capacity); }
	forceinline static usize  grow_formula(ssize value)                             { return GEN_NS array_grow_formula<Type>(value); }

	forceinline bool         append(Array other)                               { return GEN_NS array_append_array<Type>(this, other); }
	forceinline bool         append(Type value)                                { return GEN_NS array_append<Type>(this, value); }
	forceinline bool         append(Type* items, usize item_num)               { return GEN_NS array_append_items<Type>(this, items, item_num); }
	forceinline bool         append_at(Type item, usize idx)                   { return GEN_NS array_append_at<Type>(this, item, idx); }
	forceinline bool         append_at(Type* items, usize item_num, usize idx) { return GEN_NS array_append_items_at<Type>(this, items, item_num, idx); }
	forceinline Type*        back()                                            { return GEN_NS array_back<Type>(* this); }
	forceinline void         clear()                                           {        GEN_NS array_clear<Type>(* this); }
	forceinline bool         fill(usize begin, usize end, Type value)          { return GEN_NS array_fill<Type>(* this, begin, end, value); }
	forceinline void         free()                                            {        GEN_NS array_free<Type>(this); }
	forceinline ArrayHeader* get_header()                                      { return GEN_NS array_get_header<Type>(* this); }
	forceinline bool         grow(usize min_capacity)                          { return GEN_NS array_grow<Type>(this, min_capacity); }
	forceinline usize        num()                                             { return GEN_NS array_num<Type>(*this); }
	forceinline void         pop()                                             {        GEN_NS array_pop<Type>(* this); }
	forceinline void         remove_at(usize idx)                              {        GEN_NS array_remove_at<Type>(* this, idx); }
	forceinline bool         reserve(usize new_capacity)                       { return GEN_NS array_reserve<Type>(this, new_capacity); }
	forceinline bool         resize(usize num)                                 { return GEN_NS array_resize<Type>(this, num); }
	forceinline bool         set_capacity(usize new_capacity)                  { return GEN_NS array_set_capacity<Type>(this, new_capacity); }
#pragma endregion Member Mapping

	forceinline operator Type*()             { return Data; }
	forceinline operator Type const*() const { return Data; }
	forceinline Type* begin()                { return Data; }
	forceinline Type* end()                  { return Data + get_header()->Num; }

	forceinline Type&       operator[](ssize index)       { return Data[index]; }
	forceinline Type const& operator[](ssize index) const { return Data[index]; }

	using DataType = Type;
};
#endif

#if GEN_SUPPORT_CPP_REFERENCES
template<class Type> bool         append(Array<Type>& array, Array<Type> other)                         { return GEN_NS append( & array, other ); }
template<class Type> bool         append(Array<Type>& array, Type value)                                { return GEN_NS append( & array, value ); }
template<class Type> bool         append(Array<Type>& array, Type* items, usize item_num)               { return GEN_NS append( & array, items, item_num ); }
template<class Type> bool         append_at(Array<Type>& array, Type item, usize idx)                   { return GEN_NS append_at( & array, item, idx ); }
template<class Type> bool         append_at(Array<Type>& array, Type* items, usize item_num, usize idx) { return GEN_NS append_at( & array, items, item_num, idx ); }
template<class Type> void         free(Array<Type>& array)                                              { return GEN_NS free( & array ); }
template<class Type> bool         grow(Array<Type>& array, usize min_capacity)                          { return GEN_NS grow( & array, min_capacity); }
template<class Type> bool         reserve(Array<Type>& array, usize new_capacity)                       { return GEN_NS reserve( & array, new_capacity); }
template<class Type> bool         resize(Array<Type>& array, usize num)                                 { return GEN_NS resize( & array, num); }
template<class Type> bool         set_capacity(Array<Type>& array, usize new_capacity)                  { return GEN_NS set_capacity( & array, new_capacity); }

template<class Type> forceinline Type* begin(Array<Type>& array)             { return array;      }
template<class Type> forceinline Type* end(Array<Type>& array)               { return array + array_get_header(array)->Num; }
template<class Type> forceinline Type* next(Array<Type>& array, Type* entry) { return entry + 1; }
#endif

template<class Type> forceinline Type* array_begin(Array<Type> array)             { return array;      }
template<class Type> forceinline Type* array_end(Array<Type> array)               { return array + array_get_header(array)->Num; }
template<class Type> forceinline Type* array_next(Array<Type> array, Type* entry) { return ++ entry; }

template<class Type> inline
Array<Type> array_init(AllocatorInfo allocator) {
	return array_init_reserve<Type>(allocator, array_grow_formula(0));
}

template<class Type> inline
Array<Type> array_init_reserve(AllocatorInfo allocator, ssize capacity)
{
	GEN_ASSERT(capacity > 0);
	ArrayHeader* header = rcast(ArrayHeader*, alloc(allocator, sizeof(ArrayHeader) + sizeof(Type) * capacity));

	if (header == nullptr)
 		return {nullptr};

	header->Allocator = allocator;
	header->Capacity  = capacity;
	header->Num       = 0;

	return {rcast(Type*, header + 1)};
}

usize array_grow_formula(ssize value) {
	return 2 * value + 8;
}

template<class Type> inline
bool array_append_array(Array<Type>* array, Array<Type> other) {
	return array_append_items(array, (Type*)other, num(other));
}

template<class Type> inline
bool array_append(Array<Type>* array, Type value)
{
	GEN_ASSERT(  array != nullptr);
	GEN_ASSERT(* array != nullptr);
	ArrayHeader* header = array_get_header(* array);

	if (header->Num == header->Capacity)
	{
		if ( ! array_grow(array, header->Capacity))
			return false;
		header = array_get_header(* array);
	}

	(*array)[ header->Num] = value;
	header->Num++;

	return true;
}

template<class Type> inline
bool array_append_items(Array<Type>* array, Type* items, usize item_num)
{
	GEN_ASSERT(  array != nullptr);
	GEN_ASSERT(* array != nullptr);
	GEN_ASSERT(items != nullptr);
	GEN_ASSERT(item_num > 0);
	ArrayHeader* header = array_get_header(array);

	if (header->Num + item_num > header->Capacity)
	{
		if ( ! grow(array, header->Capacity + item_num))
			return false;
		header = array_get_header(array);
	}

	mem_copy((Type*)array + header->Num, items, item_num * sizeof(Type));
	header->Num += item_num;

	return true;
}

template<class Type> inline
bool array_append_at(Array<Type>* array, Type item, usize idx)
{
	GEN_ASSERT(  array != nullptr);
	GEN_ASSERT(* array != nullptr);
	ArrayHeader* header = array_get_header(* array);

	ssize slot = idx;
	if (slot >= header->Num)
	 	slot = header->Num - 1;

	if (slot < 0)
 		slot = 0;

	if (header->Capacity < header->Num + 1)
	{
		if ( ! array_grow(array, header->Capacity + 1))
			return false;

		header = array_get_header(* array);
	}

	Type* target = &(*array)[slot];

	mem_move(target + 1, target, (header->Num - slot) * sizeof(Type));
	header->Num++;

	return true;
}

template<class Type> inline
bool array_append_items_at(Array<Type>* array, Type* items, usize item_num, usize idx)
{
	GEN_ASSERT(  array != nullptr);
	GEN_ASSERT(* array != nullptr);
	ArrayHeader* header = get_header(array);

	if (idx >= header->Num)
	{
		return array_append_items(array, items, item_num);
 	}

	if (item_num > header->Capacity)
	{
		if (! grow(array, header->Capacity + item_num))
			return false;

		header = get_header(array);
	}

	Type* target = array.Data + idx + item_num;
	Type* src    = array.Data + idx;

	mem_move(target, src, (header->Num - idx) * sizeof(Type));
	mem_copy(src, items, item_num * sizeof(Type));
	header->Num += item_num;

	return true;
}

template<class Type> inline
Type* array_back(Array<Type> array)
{
	GEN_ASSERT(array != nullptr);

	ArrayHeader* header = array_get_header(array);
	if (header->Num <= 0)
		return nullptr;

	return & (array)[header->Num - 1];
}

template<class Type> inline
void array_clear(Array<Type> array) {
	GEN_ASSERT(array != nullptr);
	ArrayHeader* header = array_get_header(array);
	header->Num = 0;
}

template<class Type> inline
bool array_fill(Array<Type> array, usize begin, usize end, Type value)
{
	GEN_ASSERT(array != nullptr);
	GEN_ASSERT(begin <= end);
	ArrayHeader* header = array_get_header(array);

	if (begin < 0 || end > header->Num)
	return false;

	for (ssize idx = ssize(begin); idx < ssize(end); idx++)
	{
		array[idx] = value;
	}

	return true;
}

template<class Type> forceinline
void array_free(Array<Type>* array) {
	GEN_ASSERT(  array != nullptr);
	GEN_ASSERT(* array != nullptr);
	ArrayHeader* header = array_get_header(* array);
	allocator_free(header->Allocator, header);
	Type** Data = (Type**)array;
	*Data = nullptr;
}

template<class Type> forceinline
ArrayHeader* array_get_header(Array<Type> array) {
	GEN_ASSERT(array != nullptr);
    Type* Data = array;

	using NonConstType = TRemoveConst<Type>;
    return rcast(ArrayHeader*, const_cast<NonConstType*>(Data)) - 1;
}
template<class Type> forceinline
bool array_grow(Array<Type>* array, usize min_capacity)
{
	GEN_ASSERT(  array != nullptr);
	GEN_ASSERT(* array != nullptr);
	GEN_ASSERT( min_capacity > 0 );
	ArrayHeader* header       = array_get_header(* array);
	usize        new_capacity = array_grow_formula(header->Capacity);

	if (new_capacity < min_capacity)
		new_capacity = min_capacity;

	return array_set_capacity(array, new_capacity);
}

template<class Type> forceinline
usize array_num(Array<Type> array) {
	GEN_ASSERT(array != nullptr);
	return array_get_header(array)->Num;
}

template<class Type> forceinline
void array_pop(Array<Type> array) {
	GEN_ASSERT(array != nullptr);
	ArrayHeader* header = array_get_header(array);
	GEN_ASSERT(header->Num > 0);
	header->Num--;
}

template<class Type> inline
void array_remove_at(Array<Type> array, usize idx)
{
	GEN_ASSERT(array != nullptr);
	ArrayHeader* header = array_get_header(array);
	GEN_ASSERT(idx < header->Num);

	mem_move(array + idx, array + idx + 1, sizeof(Type) * (header->Num - idx - 1));
	header->Num--;
}

template<class Type> inline
bool array_reserve(Array<Type>* array, usize new_capacity)
{
	GEN_ASSERT(  array != nullptr);
	GEN_ASSERT(* array != nullptr);
	GEN_ASSERT(num > 0)
	ArrayHeader* header = array_get_header(array);

	if (header->Capacity < new_capacity)
 		return set_capacity(array, new_capacity);

	return true;
}

template<class Type> inline
bool array_resize(Array<Type>* array, usize num)
{
	GEN_ASSERT(  array != nullptr);
	GEN_ASSERT(* array != nullptr);
	ArrayHeader* header = array_get_header(* array);

	if (header->Capacity < num) {
		if (! array_grow( array, num))
			return false;
		header = array_get_header(* array);
	}

	header->Num = num;
 	return true;
}

template<class Type> inline
bool array_set_capacity(Array<Type>* array, usize new_capacity)
{
	GEN_ASSERT(  array != nullptr);
	GEN_ASSERT(* array != nullptr);
	ArrayHeader* header = array_get_header(* array);

	if (new_capacity == header->Capacity)
		return true;

	if (new_capacity < header->Num)
	{
		header->Num = new_capacity;
		return true;
	}

	ssize        size       = sizeof(ArrayHeader) + sizeof(Type) * new_capacity;
	ArrayHeader* new_header = rcast(ArrayHeader*, alloc(header->Allocator, size));

	if (new_header == nullptr)
		return false;

	mem_move(new_header, header, sizeof(ArrayHeader) + sizeof(Type) * header->Num);

	new_header->Capacity = new_capacity;

	allocator_free(header->Allocator, header);

	Type** Data = (Type**)array;
	* Data = rcast(Type*, new_header + 1);
	return true;
}

// These are intended for use in the base library of gencpp and the C-variant of the library
// It provides a interoperability between the C++ and C implementation of arrays. (not letting these do any crazy substiution though)
// They are undefined in gen.hpp and gen.cpp at the end of the files.
// We cpp library expects the user to use the regular calls as they can resolve the type fine.

#define array_init(type, allocator)                        array_init           <type>                               (allocator )
#define array_init_reserve(type, allocator, cap)           array_init_reserve   <type>                               (allocator, cap)
#define array_append_array(array, other)                   array_append_array   < get_array_underlying_type(array) > (& array, other )
#define array_append(array, value)                         array_append         < get_array_underlying_type(array) > (& array, value )
#define array_append_items(array, items, item_num)         array_append_items   < get_array_underlying_type(array) > (& array, items, item_num )
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

#pragma endregion Array

// TODO(Ed) : This thing needs ALOT of work.

#pragma region HashTable
#define HashTable(Type) HashTable<Type>

template<class Type> struct HashTable;

#ifndef get_hashtable_underlying_type
#define get_hashtable_underlying_type(table) typename TRemovePtr<typeof(table)>:: DataType
#endif

struct HashTableFindResult {
	ssize HashIndex;
	ssize PrevIndex;
	ssize EntryIndex;
};

template<class Type>
struct HashTableEntry {
	u64   Key;
	ssize Next;
	Type  Value;
};

#define HashTableEntry(Type) HashTableEntry<Type>

template<class Type> HashTable<Type>       hashtable_init        (AllocatorInfo allocator);
template<class Type> HashTable<Type>       hashtable_init_reserve(AllocatorInfo allocator, usize num);
template<class Type> void                  hashtable_clear       (HashTable<Type>  table);
template<class Type> void                  hashtable_destroy     (HashTable<Type>* table);
template<class Type> Type*                 hashtable_get         (HashTable<Type>  table, u64 key);
template<class Type> void                  hashtable_grow        (HashTable<Type>* table);
template<class Type> void                  hashtable_rehash      (HashTable<Type>* table, ssize new_num);
template<class Type> void                  hashtable_rehash_fast (HashTable<Type>  table);
template<class Type> void                  hashtable_remove      (HashTable<Type>  table, u64 key);
template<class Type> void                  hashtable_remove_entry(HashTable<Type>  table, ssize idx);
template<class Type> void                  hashtable_set         (HashTable<Type>* table, u64 key, Type value);
template<class Type> ssize                 hashtable_slot        (HashTable<Type>  table, u64 key);
template<class Type> void                  hashtable_map         (HashTable<Type>  table, void (*map_proc)(u64 key, Type value));
template<class Type> void                  hashtable_map_mut     (HashTable<Type>  table, void (*map_proc)(u64 key, Type* value));

template<class Type> ssize                 hashtable__add_entry  (HashTable<Type>* table, u64 key);
template<class Type> HashTableFindResult   hashtable__find       (HashTable<Type>  table, u64 key);
template<class Type> bool                  hashtable__full       (HashTable<Type>  table);

static constexpr f32 HashTable_CriticalLoadScale = 0.7f;

template<typename Type>
struct HashTable
{
	Array<ssize>                Hashes;
	Array<HashTableEntry<Type>> Entries;

#if GEN_SUPPORT_CPP_MEMBER_FEATURES
#pragma region Member Mapping
	forceinline static HashTable init(AllocatorInfo allocator)                    { return GEN_NS hashtable_init<Type>(allocator); }
	forceinline static HashTable init_reserve(AllocatorInfo allocator, usize num) { return GEN_NS hashtable_init_reserve<Type>(allocator, num); }

	forceinline void  clear()                           { GEN_NS clear<Type>(*this); }
	forceinline void  destroy()                         { GEN_NS destroy<Type>(*this); }
	forceinline Type* get(u64 key)                      { return GEN_NS get<Type>(*this, key); }
	forceinline void  grow()                            { GEN_NS grow<Type>(*this); }
	forceinline void  rehash(ssize new_num)             { GEN_NS rehash<Type>(*this, new_num); }
	forceinline void  rehash_fast()                     { GEN_NS rehash_fast<Type>(*this); }
	forceinline void  remove(u64 key)                   { GEN_NS remove<Type>(*this, key); }
	forceinline void  remove_entry(ssize idx)           { GEN_NS remove_entry<Type>(*this, idx); }
	forceinline void  set(u64 key, Type value)          { GEN_NS set<Type>(*this, key, value); }
	forceinline ssize slot(u64 key)                     { return GEN_NS slot<Type>(*this, key); }
	forceinline void  map(void (*proc)(u64, Type))      { GEN_NS map<Type>(*this, proc); }
	forceinline void  map_mut(void (*proc)(u64, Type*)) { GEN_NS map_mut<Type>(*this, proc); }
#pragma endregion Member Mapping
#endif

	using DataType = Type;
};

#if GEN_SUPPORT_CPP_REFERENCES
template<class Type> void  destroy  (HashTable<Type>& table)                      { destroy(& table); }
template<class Type> void  grow     (HashTable<Type>& table)                      { grow(& table); }
template<class Type> void  rehash   (HashTable<Type>& table, ssize new_num)       { rehash(& table, new_num); }
template<class Type> void  set      (HashTable<Type>& table, u64 key, Type value) { set(& table, key, value); }
template<class Type> ssize add_entry(HashTable<Type>& table, u64 key)             { add_entry(& table, key); }
#endif

template<typename Type> inline
HashTable<Type> hashtable_init(AllocatorInfo allocator) {
	HashTable<Type> result = hashtable_init_reserve<Type>(allocator, 8);
	return result;
}

template<typename Type> inline
HashTable<Type> hashtable_init_reserve(AllocatorInfo allocator, usize num)
{
	HashTable<Type> result = { { nullptr }, { nullptr } };

	result.Hashes = array_init_reserve<ssize>(allocator, num);
	array_get_header(result.Hashes)->Num = num;
	array_resize(& result.Hashes, num);
	array_fill(result.Hashes, 0, num, (ssize)-1);

	result.Entries = array_init_reserve<HashTableEntry<Type>>(allocator, num);
	return result;
}

template<typename Type> forceinline
void hashtable_clear(HashTable<Type> table) {
	GEN_ASSERT_NOT_NULL(table.Hashes);
	GEN_ASSERT_NOT_NULL(table.Entries);
	array_clear(table.Entries);
	array_fill(table.Hashes, 0, array_num(table.Hashes), (ssize)-1);
}

template<typename Type> forceinline
void hashtable_destroy(HashTable<Type>* table) {
	GEN_ASSERT_NOT_NULL(table->Hashes);
	GEN_ASSERT_NOT_NULL(table->Entries);
	if (table->Hashes && array_get_header(table->Hashes)->Capacity) {
		array_free(table->Hashes);
		array_free(table->Entries);
	}
}

template<typename Type> forceinline
Type* hashtable_get(HashTable<Type> table, u64 key) {
	GEN_ASSERT_NOT_NULL(table.Hashes);
	GEN_ASSERT_NOT_NULL(table.Entries);
	ssize idx = hashtable__find(table, key).EntryIndex;
	if (idx >= 0)
		return & table.Entries[idx].Value;

	return nullptr;
}

template<typename Type> forceinline
void hashtable_map(HashTable<Type> table, void (*map_proc)(u64 key, Type value)) {
	GEN_ASSERT_NOT_NULL(table.Hashes);
	GEN_ASSERT_NOT_NULL(table.Entries);
	GEN_ASSERT_NOT_NULL(map_proc);

	for (ssize idx = 0; idx < ssize(num(table.Entries)); ++idx) {
		map_proc(table.Entries[idx].Key, table.Entries[idx].Value);
	}
}

template<typename Type> forceinline
void hashtable_map_mut(HashTable<Type> table, void (*map_proc)(u64 key, Type* value)) {
	GEN_ASSERT_NOT_NULL(table.Hashes);
	GEN_ASSERT_NOT_NULL(table.Entries);
	GEN_ASSERT_NOT_NULL(map_proc);

	for (ssize idx = 0; idx < ssize(num(table.Entries)); ++idx) {
		map_proc(table.Entries[idx].Key, & table.Entries[idx].Value);
	}
}

template<typename Type> forceinline
void hashtable_grow(HashTable<Type>* table) {
	GEN_ASSERT_NOT_NULL(table);
	GEN_ASSERT_NOT_NULL(table->Hashes);
	GEN_ASSERT_NOT_NULL(table->Entries);
	ssize new_num = array_grow_formula( array_num(table->Entries));
	hashtable_rehash(table, new_num);
}

template<typename Type> inline
void hashtable_rehash(HashTable<Type>* table, ssize new_num)
{
	GEN_ASSERT_NOT_NULL(table);
	GEN_ASSERT_NOT_NULL(table->Hashes);
	GEN_ASSERT_NOT_NULL(table->Entries);
	ssize last_added_index;
	HashTable<Type> new_ht = hashtable_init_reserve<Type>( array_get_header(table->Hashes)->Allocator, new_num);

	for (ssize idx = 0; idx < ssize( array_num(table->Entries)); ++idx)
	{
		HashTableFindResult find_result;
		HashTableEntry<Type>& entry = table->Entries[idx];

		find_result = hashtable__find(new_ht, entry.Key);
		last_added_index = hashtable__add_entry(& new_ht, entry.Key);

		if (find_result.PrevIndex < 0)
			new_ht.Hashes[find_result.HashIndex] = last_added_index;
		else
			new_ht.Entries[find_result.PrevIndex].Next = last_added_index;

		new_ht.Entries[last_added_index].Next = find_result.EntryIndex;
		new_ht.Entries[last_added_index].Value = entry.Value;
	}

	hashtable_destroy(table);
	* table = new_ht;
}

template<typename Type> inline
void hashtable_rehash_fast(HashTable<Type> table)
{
	GEN_ASSERT_NOT_NULL(table.Hashes);
	GEN_ASSERT_NOT_NULL(table.Entries);
	ssize idx;

	for (idx = 0; idx < ssize(num(table.Entries)); idx++)
		table.Entries[idx].Next = -1;

	for (idx = 0; idx < ssize(num(table.Hashes)); idx++)
		table.Hashes[idx] = -1;

	for (idx = 0; idx < ssize(num(table.Entries)); idx++)
	{
		HashTableEntry<Type>* entry;
		HashTableFindResult find_result;

		entry = &table.Entries[idx];
		find_result = find(table, entry->Key);

		if (find_result.PrevIndex < 0)
			table.Hashes[find_result.HashIndex] = idx;
		else
			table.Entries[find_result.PrevIndex].Next = idx;
	}
}

template<typename Type> forceinline
void hashtable_remove(HashTable<Type> table, u64 key) {
	GEN_ASSERT_NOT_NULL(table.Hashes);
	GEN_ASSERT_NOT_NULL(table.Entries);
	HashTableFindResult find_result = find(table, key);

	if (find_result.EntryIndex >= 0) {
		remove_at(table.Entries, find_result.EntryIndex);
		rehash_fast(table);
	}
}

template<typename Type> forceinline
void hashtable_remove_entry(HashTable<Type> table, ssize idx) {
	GEN_ASSERT_NOT_NULL(table.Hashes);
	GEN_ASSERT_NOT_NULL(table.Entries);
	remove_at(table.Entries, idx);
}

template<typename Type> inline
void hashtable_set(HashTable<Type>* table, u64 key, Type value)
{
	GEN_ASSERT_NOT_NULL(table);
	GEN_ASSERT_NOT_NULL(table->Hashes);
	GEN_ASSERT_NOT_NULL(table->Entries);
	ssize idx;
	HashTableFindResult find_result;

	if (hashtable_full(* table))
		hashtable_grow(table);

	find_result = hashtable__find(* table, key);
	if (find_result.EntryIndex >= 0) {
		idx = find_result.EntryIndex;
	}
	else
	{
		idx = hashtable__add_entry(table, key);

		if (find_result.PrevIndex >= 0) {
			table->Entries[find_result.PrevIndex].Next = idx;
		}
		else {
			table->Hashes[find_result.HashIndex] = idx;
		}
	}

	table->Entries[idx].Value = value;

	if (hashtable_full(* table))
		hashtable_grow(table);
}

template<typename Type> forceinline
ssize hashtable_slot(HashTable<Type> table, u64 key) {
	GEN_ASSERT_NOT_NULL(table.Hashes);
	GEN_ASSERT_NOT_NULL(table.Entries);
	for (ssize idx = 0; idx < ssize(num(table.Hashes)); ++idx)
		if (table.Hashes[idx] == key)
			return idx;

	return -1;
}

template<typename Type> forceinline
ssize hashtable__add_entry(HashTable<Type>* table, u64 key) {
	GEN_ASSERT_NOT_NULL(table);
	GEN_ASSERT_NOT_NULL(table->Hashes);
	GEN_ASSERT_NOT_NULL(table->Entries);
	ssize idx;
	HashTableEntry<Type> entry = { key, -1 };

	idx = array_num(table->Entries);
	array_append( table->Entries, entry);
	return idx;
}

template<typename Type> inline
HashTableFindResult hashtable__find(HashTable<Type> table, u64 key)
{
	GEN_ASSERT_NOT_NULL(table.Hashes);
	GEN_ASSERT_NOT_NULL(table.Entries);
	HashTableFindResult result = { -1, -1, -1 };

	if (array_num(table.Hashes) > 0)
	{
		result.HashIndex = key % array_num(table.Hashes);
		result.EntryIndex = table.Hashes[result.HashIndex];

		while (result.EntryIndex >= 0)
		{
			if (table.Entries[result.EntryIndex].Key == key)
				break;

			result.PrevIndex = result.EntryIndex;
			result.EntryIndex = table.Entries[result.EntryIndex].Next;
		}
	}

	return result;
}

template<typename Type> forceinline
bool hashtable_full(HashTable<Type> table) {
	GEN_ASSERT_NOT_NULL(table.Hashes);
	GEN_ASSERT_NOT_NULL(table.Entries);
	usize critical_load = usize(HashTable_CriticalLoadScale * f32(array_num(table.Hashes)));
	b32 result = array_num(table.Entries) > critical_load;
	return result;
}

#define hashtable_init(type, allocator)              hashtable_init        <type              >(allocator)
#define hashtable_init_reserve(type, allocator, num) hashtable_init_reserve<type              >(allocator, num)
#define hashtable_clear(table)                       hashtable_clear       < get_hashtable_underlying_type(table) >(table)
#define hashtable_destroy(table)                     hashtable_destroy     < get_hashtable_underlying_type(table) >(& table)
#define hashtable_get(table, key)                    hashtable_get         < get_hashtable_underlying_type(table) >(table, key)
#define hashtable_grow(table)                        hashtable_grow        < get_hashtable_underlying_type(table) >(& table)
#define hashtable_rehash(table, new_num)             hashtable_rehash      < get_hashtable_underlying_type(table) >(& table, new_num)
#define hashtable_rehash_fast(table)                 hashtable_rehash_fast < get_hashtable_underlying_type(table) >(table)
#define hashtable_remove(table, key)                 hashtable_remove      < get_hashtable_underlying_type(table) >(table, key)
#define hashtable_remove_entry(table, idx)           hashtable_remove_entry< get_hashtable_underlying_type(table) >(table, idx)
#define hashtable_set(table, key, value)             hashtable_set         < get_hashtable_underlying_type(table) >(& table, key, value)
#define hashtable_slot(table, key)                   hashtable_slot        < get_hashtable_underlying_type(table) >(table, key)
#define hashtable_map(table, map_proc)               hashtable_map         < get_hashtable_underlying_type(table) >(table, map_proc)
#define hashtable_map_mut(table, map_proc)           hashtable_map_mut     < get_hashtable_underlying_type(table) >(table, map_proc)

//#define hashtable_add_entry(table, key)              hashtable_add_entry   < get_hashtable_underlying_type(table) >(& table, key)
//#define hashtable_find(table, key)                   hashtable_find        < get_hashtable_underlying_type(table) >(table, key)
//#define hashtable_full(table)                        hashtable_full        < get_hashtable_underlying_type(table) >(table)

#pragma endregion HashTable

#pragma endregion Containers
