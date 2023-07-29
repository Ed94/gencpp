#pragma region Containers

template<class Type>
struct Array
{
	struct Header
	{
		AllocatorInfo Allocator;
		uw            Capacity;
		uw            Num;
	};

	static
	Array init( AllocatorInfo allocator )
	{
		return init_reserve( allocator, grow_formula(0) );
	}

	static
	Array init_reserve( AllocatorInfo allocator, sw capacity )
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
	uw grow_formula( uw value )
	{
		return 2 * value + 8;
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

	bool append( Type* items, uw item_num )
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

	bool append_at( Type item, uw idx )
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

	bool append_at( Type* items, uw item_num, uw idx )
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

	bool fill( uw begin, uw end, Type value )
	{
		Header& header = * get_header();

		if ( begin < 0 || end >= header.Num )
			return false;

		for ( sw idx = begin; idx < end; idx++ )
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
		return rcast( Header*, Data ) - 1 ;
	}

	bool grow( uw min_capacity )
	{
		Header& header       = * get_header();
		uw      new_capacity = grow_formula( header.Capacity );

		if ( new_capacity < min_capacity )
			new_capacity = min_capacity;

		return set_capacity( new_capacity );
	}

	uw num( void )
	{
		return get_header()->Num;
	}

	bool pop( void )
	{
		Header& header = * get_header();

		GEN_ASSERT( header.Num > 0 );
		header.Num--;
	}

	void remove_at( uw idx )
	{
		Header* header = get_header();
		GEN_ASSERT( idx < header->Num );

		mem_move( header + idx, header + idx + 1, sizeof( Type ) * ( header->Num - idx - 1 ) );
		header->Num--;
	}

	bool reserve( uw new_capacity )
	{
		Header& header = * get_header();

		if ( header.Capacity < new_capacity )
			return set_capacity( new_capacity );

		return true;
	}

	bool resize( uw num )
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

	bool set_capacity( uw new_capacity )
	{
		Header& header = * get_header();

		if ( new_capacity == header.Capacity )
			return true;

		if ( new_capacity < header.Num )
			header.Num = new_capacity;

		sw      size       = sizeof( Header ) + sizeof( Type ) * new_capacity;
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

template<typename Type>
struct HashTable
{
	struct FindResult
	{
		sw HashIndex;
		sw PrevIndex;
		sw EntryIndex;
	};

	struct Entry
	{
		u64  Key;
		sw   Next;
		Type Value;
	};

	static
	HashTable init( AllocatorInfo allocator )
	{
		HashTable<Type> result = { { nullptr }, { nullptr } };

		result.Hashes  = Array<sw>::init( allocator );
		result.Entries = Array<Entry>::init( allocator );

		return result;
	}

	static
	HashTable init_reserve( AllocatorInfo allocator, uw num )
	{
		HashTable<Type> result = { { nullptr }, { nullptr } };

		result.Hashes  = Array<sw>::init_reserve( allocator, num );
		result.Hashes.get_header()->Num = num;

		result.Entries = Array<Entry>::init_reserve( allocator, num );

		return result;
	}

	void clear( void )
	{
		for ( sw idx = 0; idx < Hashes.num(); idx++ )
			Hashes[ idx ] = -1;

		Hashes.clear();
		Entries.clear();
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
		sw idx = find( key ).EntryIndex;
		if ( idx >= 0 )
			return & Entries[ idx ].Value;

		return nullptr;
	}

	using MapProc = void (*)( u64 key, Type  value );

	void map( MapProc map_proc )
	{
		GEN_ASSERT_NOT_NULL( map_proc );

		for ( sw idx = 0; idx < Entries.num(); idx++ )
		{
			map_proc( Entries[ idx ].Key, Entries[ idx ].Value );
		}
	}

	using MapMutProc = void (*)( u64 key, Type* value );

	void map_mut( MapMutProc map_proc )
	{
		GEN_ASSERT_NOT_NULL( map_proc );

		for ( sw idx = 0; idx < Entries.num(); idx++ )
		{
			map_proc( Entries[ idx ].Key, & Entries[ idx ].Value );
		}
	}

	void grow()
	{
		sw new_num = Array<Entry>::grow_formula( Entries.num() );
		rehash( new_num );
	}

	void rehash( sw new_num )
	{
		sw idx;
		sw last_added_index;

		HashTable<Type> new_ht = init_reserve( Hashes.get_header()->Allocator, new_num );

		Array<sw>::Header* hash_header = new_ht.Hashes.get_header();

		for ( idx = 0; idx < new_ht.Hashes.num(); ++idx )
			new_ht.Hashes[ idx ] = -1;

		for ( idx = 0; idx < Entries.num(); ++idx )
		{
			Entry& entry = Entries[ idx ];

			FindResult find_result;

			if ( new_ht.Hashes.num() == 0 )
				new_ht.grow();

			entry            = Entries[ idx ];
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
		sw idx;

		for ( idx = 0; idx < Entries.num(); idx++ )
			Entries[ idx ].Next = -1;

		for ( idx = 0; idx < Hashes.num(); idx++ )
			Hashes[ idx ] = -1;

		for ( idx = 0; idx < Entries.num(); idx++ )
		{
			Entry* entry;

			FindResult find_result;
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

	void remove_entry( sw idx )
	{
		Entries.remove_at( idx );
	}

	void set( u64 key, Type value )
	{
		sw idx;
		FindResult find_result;

		if ( Hashes.num() == 0 )
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

	sw slot( u64 key )
	{
		for ( sw idx = 0; idx < Hashes.num(); ++idx )
			if ( Hashes[ idx ] == key )
				return idx;

		return -1;
	}

	Array< sw>    Hashes;
	Array< Entry> Entries;

protected:

	sw add_entry( u64 key )
	{
		sw idx;
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
		return 0.75f * Hashes.num() < Entries.num();
	}
};

#pragma endregion Containers
