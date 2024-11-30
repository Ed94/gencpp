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
struct ArrayHeader;
template<class Type> struct Array;

template<class Type> Array<Type>  array_init(AllocatorInfo allocator);
template<class Type> Array<Type>  array_init_reserve(AllocatorInfo allocator, ssize capacity);
template<class Type> usize        array_grow_formula(ssize value);
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

struct ArrayHeader
{
	AllocatorInfo Allocator;
	usize         Capacity;
	usize         Num;
};

template<class Type>
struct Array
{
    Type* Data;

#if 1
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
#endif
};

template<class Type> inline
Array<Type> array_init(AllocatorInfo allocator)
{
    return array_init_reserve<Type>(allocator, array_grow_formula<Type>(0));
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

template<class Type> inline
usize array_grow_formula(ssize value)
{
    return 2 * value + 8;
}

template<class Type> inline
bool append(Array<Type>& array, Array<Type> other)
{
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

    array.Data[header->Num] = value;
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

    Type* target = array.Data + idx;

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
Type& back(Array<Type>& array)
{
    ArrayHeader* header = get_header(array);
    return array.Data[header->Num - 1];
}

template<class Type> inline
void clear(Array<Type>& array)
{
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
        array.Data[idx] = value;
    }

    return true;
}

template<class Type> inline
void free(Array<Type>& array)
{
    ArrayHeader* header = get_header(array);
    gen::free(header->Allocator, header);
    array.Data = nullptr;
}

template<class Type> inline
ArrayHeader* get_header(Array<Type>& array)
{
    using NonConstType = TRemoveConst<Type>;
    return rcast(ArrayHeader*, const_cast<NonConstType*>(array.Data)) - 1;
}

template<class Type> inline
bool grow(Array<Type>& array, usize min_capacity)
{
    ArrayHeader* header = get_header(array);
    usize new_capacity = array_grow_formula<Type>(header->Capacity);

    if (new_capacity < min_capacity)
        new_capacity = min_capacity;

    return set_capacity(array, new_capacity);
}

template<class Type> inline
usize num(Array<Type>& array)
{
    return get_header(array)->Num;
}

template<class Type> inline
void pop(Array<Type>& array)
{
    ArrayHeader* header = get_header(array);
    GEN_ASSERT(header->Num > 0);
    header->Num--;
}

template<class Type> inline
void remove_at(Array<Type>& array, usize idx)
{
    ArrayHeader* header = get_header(array);
    GEN_ASSERT(idx < header->Num);

    mem_move(array.Data + idx, array.Data + idx + 1, sizeof(Type) * (header->Num - idx - 1));
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

    if (header->Capacity < num)
    {
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

    gen::free(header->Allocator, header);

    array.Data = rcast(Type*, new_header + 1);
    return true;
}
#pragma endregion Array

// TODO(Ed) : This thing needs ALOT of work.

template<typename Type>
struct HashTable
{
	struct FindResult
	{
		ssize HashIndex;
		ssize PrevIndex;
		ssize EntryIndex;
	};

	struct Entry
	{
		u64  Key;
		ssize   Next;
		Type Value;
	};

	static constexpr f32 CriticalLoadScale = 0.7f;

	static
	HashTable init( AllocatorInfo allocator )
	{
		HashTable<Type> result = init_reserve(allocator, 8);
		return result;
	}

	static
	HashTable init_reserve( AllocatorInfo allocator, usize num )
	{
		HashTable<Type> result = { { nullptr }, { nullptr } };

		result.Hashes  = Array<ssize>::init_reserve( allocator, num );
		result.Hashes.get_header()->Num = num;
		result.Hashes.resize( num );
		result.Hashes.fill( 0, num, -1);

		result.Entries = Array<Entry>::init_reserve( allocator, num );
		return result;
	}

	void clear( void )
	{
		Entries.clear();
		Hashes.fill( 0, Hashes.num(), -1);
	}

	void destroy( void )
	{
		if ( Hashes && Hashes.get_header()->Capacity )
		{
			Hashes.free();
			Entries.free();
		}
	}

	Type* get( u64 key )
	{
		ssize idx = find( key ).EntryIndex;
		if ( idx >= 0 )
			return & Entries[ idx ].Value;

		return nullptr;
	}

	using MapProc = void (*)( u64 key, Type  value );

	void map( MapProc map_proc )
	{
		GEN_ASSERT_NOT_NULL( map_proc );

		for ( ssize idx = 0; idx < ssize(Entries.num()); ++idx )
		{
			map_proc( Entries[ idx ].Key, Entries[ idx ].Value );
		}
	}

	using MapMutProc = void (*)( u64 key, Type* value );

	void map_mut( MapMutProc map_proc )
	{
		GEN_ASSERT_NOT_NULL( map_proc );

		for ( ssize idx = 0; idx < ssize(Entries.num()); ++idx )
		{
			map_proc( Entries[ idx ].Key, & Entries[ idx ].Value );
		}
	}

	void grow()
	{
		ssize new_num = Array<Entry>::grow_formula( Entries.num() );
		rehash( new_num );
	}

	void rehash( ssize new_num )
	{
		ssize last_added_index;

		HashTable<Type> new_ht = init_reserve( Hashes.get_header()->Allocator, new_num );
		for ( ssize idx = 0; idx < ssize(Entries.num()); ++idx )
		{
			FindResult find_result;

			Entry& entry     = Entries[ idx ];
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
		*this = new_ht;
	}

	void rehash_fast()
	{
		ssize idx;

		for ( idx = 0; idx < ssize(Entries.num()); idx++ )
			Entries[ idx ].Next = -1;

		for ( idx = 0; idx < ssize(Hashes.num()); idx++ )
			Hashes[ idx ] = -1;

		for ( idx = 0; idx < ssize(Entries.num()); idx++ )
		{
			Entry*     entry;
			FindResult find_result;

			entry       = & Entries[ idx ];
			find_result = find( entry->Key );

			if ( find_result.PrevIndex < 0 )
				Hashes[ find_result.HashIndex ] = idx;
			else
				Entries[ find_result.PrevIndex ].Next = idx;
		}
	}

	void remove( u64 key )
	{
		FindResult find_result = find( key);

		if ( find_result.EntryIndex >= 0 )
		{
			Entries.remove_at( find_result.EntryIndex );
			rehash_fast();
		}
	}

	void remove_entry( ssize idx )
	{
		Entries.remove_at( idx );
	}

	void set( u64 key, Type value )
	{
		ssize idx;
		FindResult find_result;

		if ( full() )
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

	ssize slot( u64 key )
	{
		for ( ssize idx = 0; idx < ssize(Hashes.num()); ++idx )
			if ( Hashes[ idx ] == key )
				return idx;

		return -1;
	}

	Array< ssize>    Hashes;
	Array< Entry> Entries;

protected:

	ssize add_entry( u64 key )
	{
		ssize idx;
		Entry entry = { key, -1 };

		idx = Entries.num();
		Entries.append( entry );
		return idx;
	}

	FindResult find( u64 key )
	{
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
	}

	b32 full()
	{
		usize critical_load = usize( CriticalLoadScale * f32(Hashes.num()) );
		b32 result = Entries.num() > critical_load;
		return result;
	}
};

#pragma endregion Containers
