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

template<class Type>
struct Array
{
	struct Header
	{
		AllocatorInfo Allocator;
		usize            Capacity;
		usize            Num;
	};

	static
	Array init( AllocatorInfo allocator )
	{
		return init_reserve( allocator, grow_formula(0) );
	}

	static
	Array init_reserve( AllocatorInfo allocator, ssize capacity )
	{
		Header* header = rcast( Header*, alloc( allocator, sizeof(Header) + sizeof(Type) * capacity ));

		if ( header == nullptr )
			return { nullptr };

		header->Allocator = allocator;
		header->Capacity  = capacity;
		header->Num       = 0;

		return { rcast( Type*, header + 1) };
	}

	static
	usize grow_formula( usize value )
	{
		return 2 * value + 8;
	}

	bool append( Array other )
	{
		return append( other, other.num() );
	}

	bool append( Type value )
	{
		Header* header = get_header();

		if ( header->Num == header->Capacity )
		{
			if ( ! grow( header->Capacity ))
				return false;

			header = get_header();
		}

		Data[ header->Num ] = value;
		header->Num++;

		return true;
	}

	bool append( Type* items, usize item_num )
	{
		Header* header = get_header();

		if ( header->Num + item_num > header->Capacity )
		{
			if ( ! grow( header->Capacity + item_num ))
				return false;

			header = get_header();
		}

		mem_copy( Data + header->Num, items, item_num * sizeof(Type) );
		header->Num += item_num;

		return true;
	}

	bool append_at( Type item, usize idx )
	{
		Header* header = get_header();

		if ( idx >= header->Num )
			idx = header->Num - 1;

		if ( idx < 0 )
			idx = 0;

		if ( header->Capacity < header->Num + 1 )
		{
			if ( ! grow( header->Capacity + 1 ))
				return false;

			header = get_header();
		}

		Type* target = Data + idx;

		mem_move( target + 1, target, (header->Num - idx) * sizeof(Type) );
		header->Num++;

		return true;
	}

	bool append_at( Type* items, usize item_num, usize idx )
	{
		Header* header = get_header();

		if ( idx >= header->Num )
		{
			return append( items, item_num );
		}

		if ( item_num > header->Capacity )
		{
			if ( ! grow( header->Capacity + item_num ) )
				return false;

			header = get_header();
		}

		Type* target = Data + idx + item_num;
		Type* src    = Data + idx;

		mem_move( target, src, (header->Num - idx) * sizeof(Type) );
		mem_copy( src, items, item_num * sizeof(Type) );
		header->Num += item_num;

		return true;
	}

	Type& back( void )
	{
		Header& header = * get_header();
		return Data[ header.Num - 1 ];
	}

	void clear( void )
	{
		Header& header = * get_header();
		header.Num     = 0;
	}

	bool fill( usize begin, usize end, Type value )
	{
		Header& header = * get_header();

		if ( begin < 0 || end > header.Num )
			return false;

		for ( ssize idx = ssize(begin); idx < ssize(end); idx++ )
		{
			Data[ idx ] = value;
		}

		return true;
	}

	void free( void )
	{
		Header& header = * get_header();
		gen::free( header.Allocator, &header );
		Data = nullptr;
	}

	Header* get_header( void )
	{
		using NonConstType = TRemoveConst< Type >;
		return rcast( Header*, const_cast<NonConstType*>(Data) ) - 1 ;
	}

	bool grow( usize min_capacity )
	{
		Header& header       = * get_header();
		usize      new_capacity = grow_formula( header.Capacity );

		if ( new_capacity < min_capacity )
			new_capacity = min_capacity;

		return set_capacity( new_capacity );
	}

	usize num( void )
	{
		return get_header()->Num;
	}

	void pop( void )
	{
		Header& header = * get_header();

		GEN_ASSERT( header.Num > 0 );
		header.Num--;
	}

	void remove_at( usize idx )
	{
		Header* header = get_header();
		GEN_ASSERT( idx < header->Num );

		mem_move( header + idx, header + idx + 1, sizeof( Type ) * ( header->Num - idx - 1 ) );
		header->Num--;
	}

	bool reserve( usize new_capacity )
	{
		Header& header = * get_header();

		if ( header.Capacity < new_capacity )
			return set_capacity( new_capacity );

		return true;
	}

	bool resize( usize num )
	{
		Header* header = get_header();

		if ( header->Capacity < num )
		{
			if ( ! grow( num ) )
				return false;

			header = get_header();
		}

		header->Num = num;
		return true;
	}

	bool set_capacity( usize new_capacity )
	{
		Header& header = * get_header();

		if ( new_capacity == header.Capacity )
			return true;

		if ( new_capacity < header.Num )
		{
			// Already have the memory, mine as well keep it.
			header.Num = new_capacity;
			return true;
		}

		ssize      size       = sizeof( Header ) + sizeof( Type ) * new_capacity;
		Header* new_header = rcast( Header*, alloc( header.Allocator, size ) );

		if ( new_header == nullptr )
			return false;

		mem_move( new_header, &header, sizeof( Header ) + sizeof( Type ) * header.Num );

		new_header->Capacity = new_capacity;

		gen::free( header.Allocator, &header );

		Data = rcast( Type*, new_header + 1);
		return true;
	}

	Type* Data;

	operator Type*()
	{
		return Data;
	}

	operator Type const*() const
	{
		return Data;
	}

	// For-range based support

	Type* begin()
	{
		return Data;
	}

	Type* end()
	{
		return Data + get_header()->Num;
	}
};

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
