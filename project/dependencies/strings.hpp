#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "hashing.hpp"
#endif

#pragma region Strings

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

#define cast_to_strc( str ) * rcast( StrC*, str - sizeof(sw) )
#define txt( text ) StrC { sizeof( text ) - 1, text }

StrC to_str( char const* str )
{
	return { str_len( str ), str };
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
		sw            Capacity;
		sw            Length;
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
	String make_reserve( AllocatorInfo allocator, sw capacity );

	static
	String make_length( AllocatorInfo allocator, char const* str, sw length );

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

	bool make_space_for( char const* str, sw add_len );

	bool append( char c )
	{
		return append( & c, 1 );
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
		return append( other.Data, other.length() );
	}

	bool append_fmt( char const* fmt, ... );

	sw avail_space() const
	{
		Header const&
		header = * rcast( Header const*, Data - sizeof( Header ));

		return header.Capacity - header.Length;
	}

	char& back()
	{
		return Data[ length() - 1 ];
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

	String duplicate( AllocatorInfo allocator ) const
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

	void skip_line()
	{
	#define current (*scanner)
		char* scanner = Data;
		while ( current != '\r' && current != '\n' )
		{
			++ scanner;
		}

		s32 new_length = scanner - Data;

		if ( current == '\r' )
		{
			new_length += 1;
		}

		mem_move( Data, scanner, new_length );

		Header* header = & get_header();
		header->Length = new_length;
	#undef current
	}

	void strip_space()
	{
		char* write_pos = Data;
		char* read_pos  = Data;

		while ( * read_pos)
		{
			if ( ! char_is_space( *read_pos ))
			{
				*write_pos = *read_pos;
				write_pos++;
			}
			read_pos++;
		}

		write_pos[0] = '\0';  // Null-terminate the modified string

		// Update the length if needed
		get_header().Length = write_pos - Data;
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

	// Debug function that provides a copy of the string with whitespace characters visualized.
	String visualize_whitespace() const
	{
		Header* header = (Header*)(Data - sizeof(Header));

		String result = make_reserve(header->Allocator, length() * 2); // Assume worst case for space requirements.

		for ( char c : *this )
		{
			switch ( c )
			{
				case ' ':
					result.append( txt("·") );
				break;
				case '\t':
					result.append( txt("→") );
				break;
				case '\n':
					result.append( txt("↵") );
				break;
				case '\r':
					result.append( txt("⏎") );
				break;
				case '\v':
					result.append( txt("⇕") );
				break;
				case '\f':
					result.append( txt("⌂") );
				break;
				default:
					result.append(c);
				break;
			}
		}

		return result;
	}

	// For-range support

	char* begin() const
	{
		return Data;
	}

	char* end() const
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
		return { length(), Data };
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

	char* Data;
};

struct String_POD
{
	char* Data;
};
static_assert( sizeof( String_POD ) == sizeof( String ), "String is not a POD" );

// Implements basic string interning. Data structure is based off the ZPL Hashtable.
using StringTable = HashTable<String const>;

// Represents strings cached with the string table.
// Should never be modified, if changed string is desired, cache_string( str ) another.
using StringCached = String const;

#pragma endregion Strings
