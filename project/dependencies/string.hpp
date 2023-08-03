#pragma region String

// Constant string with length.
struct StrC
{
	sw          Len;
	char const* Ptr;

	operator char const* () const
	{
		return Ptr;
	}
};

#define txt_StrC( text ) StrC { sizeof( text ) - 1, text }

StrC to_StrC( char const* str )
{
	return { str_len( str ), str };
}

sw StrC_len( char const* str )
{
	return (sw) ( str - 1 );
}

// Dynamic String
// This is directly based off the ZPL string api.
// They used a header pattern
// I kept it for simplicty of porting but its not necessary to keep it that way.
struct String
{
	struct Header
	{
		AllocatorInfo Allocator;
		sw            Length;
		sw            Capacity;
	};

	static
	uw grow_formula( uw value )
	{
		// Using a very aggressive growth formula to reduce time mem_copying with recursive calls to append in this library.
		return 4 * value + 8;
	}

	static
	String make( AllocatorInfo allocator, char const* str )
	{
		sw length = str ? str_len( str ) : 0;
		return make_length( allocator, str, length );
	}

	static
	String make( AllocatorInfo allocator, StrC str )
	{
		return make_length( allocator, str.Ptr, str.Len );
	}

	static
	String make_reserve( AllocatorInfo allocator, sw capacity )
	{
		constexpr sw header_size = sizeof( Header );

		s32   alloc_size = header_size + capacity + 1;
		void* allocation = alloc( allocator, alloc_size );

		if ( allocation == nullptr )
			return { nullptr };

		mem_set( allocation, 0, alloc_size );

		Header*
			header            = rcast(Header*, allocation);
		header->Allocator = allocator;
		header->Capacity  = capacity;
		header->Length    = 0;

		String result = { (char*)allocation + header_size };
		return result;
	}

	static
	String make_length( AllocatorInfo allocator, char const* str, sw length )
	{
		constexpr sw header_size = sizeof( Header );

		s32   alloc_size = header_size + length + 1;
		void* allocation = alloc( allocator, alloc_size );

		if ( allocation == nullptr )
			return { nullptr };

		Header&
			header = * rcast(Header*, allocation);
		header = { allocator, length, length };

		String  result = { rcast( char*, allocation) + header_size };

		if ( length && str )
			mem_copy( result, str, length );
		else
			mem_set( result, 0, alloc_size - header_size );

		result[ length ] = '\0';

		return result;
	}

	static
	String fmt( AllocatorInfo allocator, char* buf, sw buf_size, char const* fmt, ... );

	static
	String fmt_buf( AllocatorInfo allocator, char const* fmt, ... );

	static
	String join( AllocatorInfo allocator, char const** parts, sw num_parts, char const* glue )
	{
		String result = make( allocator, "" );

		for ( sw idx = 0; idx < num_parts; ++idx )
		{
			result.append( parts[ idx ] );

			if ( idx < num_parts - 1 )
				result.append( glue );
		}

		return result;
	}

	static
	bool are_equal( String lhs, String rhs )
	{
		if ( lhs.length() != rhs.length() )
			return false;

		for ( sw idx = 0; idx < lhs.length(); ++idx )
			if ( lhs[ idx ] != rhs[ idx ] )
				return false;

		return true;
	}

	bool make_space_for( char const* str, sw add_len )
	{
		sw available = avail_space();

		// NOTE: Return if there is enough space left
		if ( available >= add_len )
		{
			return true;
		}
		else
		{
			sw new_len, old_size, new_size;

			void* ptr;
			void* new_ptr;

			AllocatorInfo allocator = get_header().Allocator;
			Header*       header	= nullptr;

			new_len  = grow_formula( length() + add_len );
			ptr      = & get_header();
			old_size = size_of( Header ) + length() + 1;
			new_size = size_of( Header ) + new_len + 1;

			new_ptr = resize( allocator, ptr, old_size, new_size );

			if ( new_ptr == nullptr )
				return false;

			header            = zpl_cast( Header* ) new_ptr;
			header->Allocator = allocator;
			header->Capacity  = new_len;

			Data = rcast( char*, header + 1 );

			return str;
		}
	}

	bool append( char const* str )
	{
		return append( str, str_len( str ) );
	}

	bool append( char const* str, sw length )
	{
		if ( sptr(str) > 0 )
		{
			sw curr_len = this->length();

			if ( ! make_space_for( str, length ) )
				return false;

			Header& header = get_header();

			mem_copy( Data + curr_len, str, length );

			Data[ curr_len + length ] = '\0';

			header.Length = curr_len + length;
		}
		return str;
	}

	bool append( StrC str)
	{
		return append( str.Ptr, str.Len );
	}

	bool append( const String other )
	{
		return append( other.Data, other.length() );;
	}

	bool append_fmt( char const* fmt, ... );

	sw avail_space() const
	{
		Header const&
		header = * rcast( Header const*, Data - sizeof( Header ));

		return header.Capacity - header.Length;
	}

	sw capacity() const
	{
		Header const&
		header = * rcast( Header const*, Data - sizeof( Header ));

		return header.Capacity;
	}

	void clear()
	{
		get_header().Length = 0;
	}

	String duplicate( AllocatorInfo allocator )
	{
		return make_length( allocator, Data, length() );
	}

	void free()
	{
		if ( ! Data )
			return;

		Header& header = get_header();

		gen::free( header.Allocator, & header );
	}

	Header& get_header()
	{
		return *(Header*)(Data - sizeof(Header));
	}

	sw length() const
	{
		Header const&
		header = * rcast( Header const*, Data - sizeof( Header ));

		return header.Length;
	}

	void trim( char const* cut_set )
	{
		sw len = 0;

		char* start_pos = Data;
		char* end_pos   = Data + length() - 1;

		while ( start_pos <= end_pos && char_first_occurence( cut_set, *start_pos ) )
			start_pos++;

		while ( end_pos > start_pos && char_first_occurence( cut_set, *end_pos ) )
			end_pos--;

		len = scast( sw, ( start_pos > end_pos ) ? 0 : ( ( end_pos - start_pos ) + 1 ) );

		if ( Data != start_pos )
			mem_move( Data, start_pos, len );

		Data[ len ] = '\0';

		get_header().Length = len;
	}

	void trim_space()
	{
		return trim( " \t\r\n\v\f" );
	}

	// For-range support

	char* begin()
	{
		return Data;
	}

	char* end()
	{
		Header const&
		header = * rcast( Header const*, Data - sizeof( Header ));

		return Data + header.Length;
	}

	operator bool()
	{
		return Data;
	}

	operator char* ()
	{
		return Data;
	}

	operator char const* () const
	{
		return Data;
	}

	operator StrC() const
	{
		return
		{
			length(),
			Data
		};
	}

	// Used with cached strings
	// Essentially makes the string a string view.
	String const& operator = ( String const& other ) const
	{
		if ( this == & other )
			return *this;

		String& this_ = ccast( String, *this );

		this_.Data = other.Data;

		return this_;
	}

	char& operator [] ( sw index )
	{
		return Data[ index ];
	}

	char const& operator [] ( sw index ) const
	{
		return Data[ index ];
	}

	char* Data = nullptr;
};

struct String_POD
{
	char* Data;

	operator String()
	{
		return * rcast(String*, this);
	}
};
static_assert( sizeof( String_POD ) == sizeof( String ), "String is not a POD" );

#pragma endregion String

