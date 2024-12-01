#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "printing.hpp"
#endif

#pragma region Containers

template<class TType>             struct RemoveConst                    { typedef TType Type;       };
template<class TType>             struct RemoveConst<const TType>       { typedef TType Type;       };
template<class TType>             struct RemoveConst<const TType[]>     { typedef TType Type[];     };
template<class TType, usize Size> struct RemoveConst<const TType[Size]> { typedef TType Type[Size]; };

template<class TType>
using TRemoveConst = typename RemoveConst<TType>::Type;

#pragma region Array
#if ! GEN_COMPILER_C
#define Array(Type) Array<Type>

// #define array_init(Type, ...)         array_init        <Type>(__VA_ARGS__)
// #define array_init_reserve(Type, ...) array_init_reserve<Type>(__VA_ARGS__)
#endif

struct ArrayHeader;

#if GEN_SUPPORT_CPP_MEMBER_FEATURES
template<class Type> struct Array;
#else
template<class Type>
using Array = Type*;
#endif

usize array_grow_formula(ssize value);

template<class Type> Array<Type>  array_init(AllocatorInfo allocator);
template<class Type> Array<Type>  array_init_reserve(AllocatorInfo allocator, ssize capacity);
template<class Type> bool         append(Array<Type>& array, Array<Type> other);
template<class Type> bool         append(Array<Type>& array, Type value);
template<class Type> bool         append(Array<Type>& array, Type* items, usize item_num);
template<class Type> bool         append_at(Array<Type>& array, Type item, usize idx);
template<class Type> bool         append_at(Array<Type>& array, Type* items, usize item_num, usize idx);
template<class Type> Type&        back(Array<Type>& array);
template<class Type> void         clear(Array<Type>& array);
template<class Type> bool         fill(Array<Type>& array, usize begin, usize end, Type value);
template<class Type> void         free(Array<Type>& array);
template<class Type> bool         grow(Array<Type>& array, usize min_capacity);
template<class Type> usize        num(Array<Type>& array);
template<class Type> void         pop(Array<Type>& array);
template<class Type> void         remove_at(Array<Type>& array, usize idx);
template<class Type> bool         reserve(Array<Type>& array, usize new_capacity);
template<class Type> bool         resize(Array<Type>& array, usize num);
template<class Type> bool         set_capacity(Array<Type>& array, usize new_capacity);
template<class Type> ArrayHeader* get_header(Array<Type>& array);

template<class Type> forceinline Type* begin(Array<Type>& array)             { return array;      }
template<class Type> forceinline Type* end(Array<Type>& array)               { return array + get_header(array)->Num; }
template<class Type> forceinline Type* next(Array<Type>& array, Type* entry) { return entry + 1; }

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

	forceinline bool         append(Array other)                               { return GEN_NS append<Type>(*this, other); }
	forceinline bool         append(Type value)                                { return GEN_NS append<Type>(*this, value); }
	forceinline bool         append(Type* items, usize item_num)               { return GEN_NS append<Type>(*this, items, item_num); }
	forceinline bool         append_at(Type item, usize idx)                   { return GEN_NS append_at<Type>(*this, item, idx); }
	forceinline bool         append_at(Type* items, usize item_num, usize idx) { return GEN_NS append_at<Type>(*this, items, item_num, idx); }
	forceinline Type&        back()                                            { return GEN_NS back<Type>(*this); }
	forceinline void         clear()                                           { GEN_NS clear<Type>(*this); }
	forceinline bool         fill(usize begin, usize end, Type value)          { return GEN_NS fill<Type>(*this, begin, end, value); }
	forceinline void         free()                                            { GEN_NS free<Type>(*this); }
	forceinline ArrayHeader* get_header()                                      { return GEN_NS get_header<Type>(*this); }
	forceinline bool         grow(usize min_capacity)                          { return GEN_NS grow<Type>(*this, min_capacity); }
	forceinline usize        num()                                             { return GEN_NS num<Type>(*this); }
	forceinline void         pop()                                             { GEN_NS pop<Type>(*this); }
	forceinline void         remove_at(usize idx)                              { GEN_NS remove_at<Type>(*this, idx); }
	forceinline bool         reserve(usize new_capacity)                       { return GEN_NS reserve<Type>(*this, new_capacity); }
	forceinline bool         resize(usize num)                                 { return GEN_NS resize<Type>(*this, num); }
	forceinline bool         set_capacity(usize new_capacity)                  { return GEN_NS set_capacity<Type>(*this, new_capacity); }

	forceinline operator Type*()             { return Data; }
	forceinline operator Type const*() const { return Data; }
	forceinline Type* begin()                { return Data; }
	forceinline Type* end()                  { return Data + get_header()->Num; }
#pragma endregion Member Mapping
};
#endif

template<class Type> inline
Array<Type> array_init(AllocatorInfo allocator) {
	return array_init_reserve<Type>(allocator, array_grow_formula(0));
}

template<class Type> inline
Array<Type> array_init_reserve(AllocatorInfo allocator, ssize capacity)
{
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
bool append(Array<Type>& array, Array<Type> other) {
	return append(array, other, num(other));
}

template<class Type> inline
bool append(Array<Type>& array, Type value)
{
	ArrayHeader* header = get_header(array);

	if (header->Num == header->Capacity)
	{
		if (!grow(array, header->Capacity))
			return false;
		header = get_header(array);
	}

	array[header->Num] = value;
	header->Num++;

	return true;
}

template<class Type> inline
bool append(Array<Type>& array, Type* items, usize item_num)
{
	ArrayHeader* header = get_header(array);

	if (header->Num + item_num > header->Capacity)
	{
		if (!grow(array, header->Capacity + item_num))
			return false;
		header = get_header(array);
	}

	mem_copy(array.Data + header->Num, items, item_num * sizeof(Type));
	header->Num += item_num;

	return true;
}

template<class Type> inline
bool append_at(Array<Type>& array, Type item, usize idx)
{
	ArrayHeader* header = get_header(array);

	if (idx >= header->Num)
	 	idx = header->Num - 1;

	if (idx < 0)
 		idx = 0;

	if (header->Capacity < header->Num + 1)
	{
		if (!grow(array, header->Capacity + 1))
			return false;

		header = get_header(array);
	}

	Type* target = array + idx;

	mem_move(target + 1, target, (header->Num - idx) * sizeof(Type));
	header->Num++;

	return true;
}

template<class Type> inline
bool append_at(Array<Type>& array, Type* items, usize item_num, usize idx)
{
	ArrayHeader* header = get_header(array);

	if (idx >= header->Num)
	{
		return append(array, items, item_num);
 	}

	if (item_num > header->Capacity)
	{
		if (!grow(array, header->Capacity + item_num))
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
Type& back(Array<Type>& array) {
	ArrayHeader* header = get_header(array);
	return array[header->Num - 1];
}

template<class Type> inline
void clear(Array<Type>& array) {
	ArrayHeader* header = get_header(array);
	header->Num = 0;
}

template<class Type> inline
bool fill(Array<Type>& array, usize begin, usize end, Type value)
{
	ArrayHeader* header = get_header(array);

	if (begin < 0 || end > header->Num)
	return false;

	for (ssize idx = ssize(begin); idx < ssize(end); idx++)
	{
		array[idx] = value;
	}

	return true;
}

template<class Type> inline
void free(Array<Type>& array) {
	ArrayHeader* header = get_header(array);
	GEN_NS free(header->Allocator, header);
	Type*& Data = rcast(Type*&, array);
	Data = nullptr;
}

template<class Type> inline
ArrayHeader* get_header(Array<Type>& array) {
	using NonConstType = TRemoveConst<Type>;
	Type* Data = array; // This should do nothing in C but in C++ gets member Data struct.
	return rcast(ArrayHeader*, const_cast<NonConstType*>(Data)) - 1;
}

template<class Type> inline
bool grow(Array<Type>& array, usize min_capacity)
{
	ArrayHeader* header = get_header(array);
	usize new_capacity = array_grow_formula(header->Capacity);

	if (new_capacity < min_capacity)
	new_capacity = min_capacity;

	return set_capacity(array, new_capacity);
}

template<class Type> inline
usize num(Array<Type>& array) {
	return get_header(array)->Num;
}

template<class Type> inline
void pop(Array<Type>& array) {
	ArrayHeader* header = get_header(array);
	GEN_ASSERT(header->Num > 0);
	header->Num--;
}

template<class Type> inline
void remove_at(Array<Type>& array, usize idx)
{
	ArrayHeader* header = get_header(array);
	GEN_ASSERT(idx < header->Num);

	mem_move(array + idx, array + idx + 1, sizeof(Type) * (header->Num - idx - 1));
	header->Num--;
}

template<class Type> inline
bool reserve(Array<Type>& array, usize new_capacity)
{
	ArrayHeader* header = get_header(array);

	if (header->Capacity < new_capacity)
 		return set_capacity(array, new_capacity);

	return true;
}

template<class Type> inline
bool resize(Array<Type>& array, usize num)
{
	ArrayHeader* header = get_header(array);

	if (header->Capacity < num) {
		if (!grow(array, num))
			return false;
		header = get_header(array);
	}

	header->Num = num;
 	return true;
}

template<class Type> inline
bool set_capacity(Array<Type>& array, usize new_capacity)
{
	ArrayHeader* header = get_header(array);

	if (new_capacity == header->Capacity)
	return true;

	if (new_capacity < header->Num)
	{
		header->Num = new_capacity;
		return true;
	}

	ssize size = sizeof(ArrayHeader) + sizeof(Type) * new_capacity;
	ArrayHeader* new_header = rcast(ArrayHeader*, alloc(header->Allocator, size));

	if (new_header == nullptr)
		return false;

	mem_move(new_header, header, sizeof(ArrayHeader) + sizeof(Type) * header->Num);

	new_header->Capacity = new_capacity;

	GEN_NS free(header->Allocator, header);

	Type*& Data = rcast(Type*&, array);
	Data = rcast(Type*, new_header + 1);
	return true;
}
#pragma endregion Array

// TODO(Ed) : This thing needs ALOT of work.

#pragma region HashTable
template<class Type> struct HashTable;

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

// Forward declarations for all lifted functions
template<class Type> HashTable<Type>       hashtable_init(AllocatorInfo allocator);
template<class Type> HashTable<Type>       hashtable_init_reserve(AllocatorInfo allocator, usize num);
template<class Type> void                  clear(HashTable<Type>& table);
template<class Type> void                  destroy(HashTable<Type>& table);
template<class Type> Type*                 get(HashTable<Type>& table, u64 key);
template<class Type> void                  grow(HashTable<Type>& table);
template<class Type> void                  rehash(HashTable<Type>& table, ssize new_num);
template<class Type> void                  rehash_fast(HashTable<Type>& table);
template<class Type> void                  remove(HashTable<Type>& table, u64 key);
template<class Type> void                  remove_entry(HashTable<Type>& table, ssize idx);
template<class Type> void                  set(HashTable<Type>& table, u64 key, Type value);
template<class Type> ssize                 slot(HashTable<Type>& table, u64 key);
template<class Type> ssize                 add_entry(HashTable<Type>& table, u64 key);
template<class Type> HashTableFindResult   find(HashTable<Type>& table, u64 key);
template<class Type> bool                  full(HashTable<Type>& table);
template<class Type> void                  map(HashTable<Type>& table, void (*map_proc)(u64 key, Type value));
template<class Type> void                  map_mut(HashTable<Type>& table, void (*map_proc)(u64 key, Type* value));

static constexpr f32 HashTable_CriticalLoadScale = 0.7f;

template<typename Type>
struct HashTable
{
	Array<ssize>                Hashes;
	Array<HashTableEntry<Type>> Entries;

#if 1
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
};

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
	get_header(result.Hashes)->Num = num;
	resize(result.Hashes, num);
	fill<ssize>(result.Hashes, 0, num, -1);

	result.Entries = array_init_reserve<HashTableEntry<Type>>(allocator, num);
	return result;
}

template<typename Type> inline
void clear(HashTable<Type>& table) {
	clear(table.Entries);
	fill<ssize>(table.Hashes, 0, num(table.Hashes), -1);
}

template<typename Type> inline
void destroy(HashTable<Type>& table) {
	if (table.Hashes && get_header(table.Hashes)->Capacity) {
		free(table.Hashes);
		free(table.Entries);
	}
}

template<typename Type> inline
Type* get(HashTable<Type>& table, u64 key) {
	ssize idx = find(table, key).EntryIndex;
	if (idx >= 0)
		return &table.Entries[idx].Value;

	return nullptr;
}

template<typename Type> inline
void map(HashTable<Type>& table, void (*map_proc)(u64 key, Type value)) {
	GEN_ASSERT_NOT_NULL(map_proc);

	for (ssize idx = 0; idx < ssize(table.Entries.num()); ++idx) {
		map_proc(table.Entries[idx].Key, table.Entries[idx].Value);
	}
}

template<typename Type> inline
void map_mut(HashTable<Type>& table, void (*map_proc)(u64 key, Type* value)) {
	GEN_ASSERT_NOT_NULL(map_proc);

	for (ssize idx = 0; idx < ssize(table.Entries.num()); ++idx) {
		map_proc(table.Entries[idx].Key, &table.Entries[idx].Value);
	}
}

template<typename Type> inline
void grow(HashTable<Type>& table) {
	ssize new_num = array_grow_formula(num(table.Entries));
	rehash(table, new_num);
}

template<typename Type> inline
void rehash(HashTable<Type>& table, ssize new_num)
{
	ssize last_added_index;
	HashTable<Type> new_ht = hashtable_init_reserve<Type>(get_header(table.Hashes)->Allocator, new_num);

	for (ssize idx = 0; idx < ssize(num(table.Entries)); ++idx)
	{
		HashTableFindResult find_result;
		HashTableEntry<Type>& entry = table.Entries[idx];

		find_result = find(new_ht, entry.Key);
		last_added_index = add_entry(new_ht, entry.Key);

		if (find_result.PrevIndex < 0)
			new_ht.Hashes[find_result.HashIndex] = last_added_index;
		else
			new_ht.Entries[find_result.PrevIndex].Next = last_added_index;

		new_ht.Entries[last_added_index].Next = find_result.EntryIndex;
		new_ht.Entries[last_added_index].Value = entry.Value;
	}

	destroy(table);
	table = new_ht;
}

template<typename Type> inline
void rehash_fast(HashTable<Type>& table)
{
	ssize idx;

	for (idx = 0; idx < ssize(table.Entries.num()); idx++)
		table.Entries[idx].Next = -1;

	for (idx = 0; idx < ssize(table.Hashes.num()); idx++)
		table.Hashes[idx] = -1;

	for (idx = 0; idx < ssize(table.Entries.num()); idx++)
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

template<typename Type> inline
void remove(HashTable<Type>& table, u64 key) {
	HashTableFindResult find_result = find(table, key);

	if (find_result.EntryIndex >= 0) {
		table.Entries.remove_at(find_result.EntryIndex);
		rehash_fast(table);
	}
}

template<typename Type> inline
void remove_entry(HashTable<Type>& table, ssize idx) {
	table.Entries.remove_at(idx);
}

template<typename Type> inline
void set(HashTable<Type>& table, u64 key, Type value)
{
	ssize idx;
	HashTableFindResult find_result;

	if (full(table))
		grow(table);

	find_result = find(table, key);
	if (find_result.EntryIndex >= 0) {
		idx = find_result.EntryIndex;
	}
	else
	{
		idx = add_entry(table, key);

		if (find_result.PrevIndex >= 0) {
			table.Entries[find_result.PrevIndex].Next = idx;
		}
		else {
			table.Hashes[find_result.HashIndex] = idx;
		}
	}

	table.Entries[idx].Value = value;

	if (full(table))
		grow(table);
}

template<typename Type> inline
ssize slot(HashTable<Type>& table, u64 key) {
	for (ssize idx = 0; idx < ssize(table.Hashes.num()); ++idx)
		if (table.Hashes[idx] == key)
			return idx;

	return -1;
}

template<typename Type> inline
ssize add_entry(HashTable<Type>& table, u64 key) {
	ssize idx;
	HashTableEntry<Type> entry = { key, -1 };

	idx = num(table.Entries);
	append(table.Entries, entry);
	return idx;
}

template<typename Type> inline
HashTableFindResult find(HashTable<Type>& table, u64 key)
{
	HashTableFindResult result = { -1, -1, -1 };

	if (num(table.Hashes) > 0)
	{
		result.HashIndex = key % num(table.Hashes);
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

template<typename Type> inline
bool full(HashTable<Type>& table) {
	usize critical_load = usize(HashTable_CriticalLoadScale * f32(num(table.Hashes)));
	b32 result = num(table.Entries) > critical_load;
	return result;
}
#pragma endregion HashTable

#pragma endregion Containers
