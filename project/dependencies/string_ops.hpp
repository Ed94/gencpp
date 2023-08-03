#pragma region String Ops

GEN_DEF_INLINE const char* char_first_occurence( const char* str, char c );
constexpr auto str_find = &char_first_occurence;

GEN_DEF_INLINE b32   char_is_alpha( char c );
GEN_DEF_INLINE b32   char_is_alphanumeric( char c );
GEN_DEF_INLINE b32   char_is_digit( char c );
GEN_DEF_INLINE b32   char_is_hex_digit( char c );
GEN_DEF_INLINE b32   char_is_space( char c );
GEN_DEF_INLINE char  char_to_lower( char c );
GEN_DEF_INLINE char  char_to_upper( char c );

GEN_DEF_INLINE s32  digit_to_int( char c );
GEN_DEF_INLINE s32  hex_digit_to_int( char c );

GEN_DEF_INLINE s32         str_compare( const char* s1, const char* s2 );
GEN_DEF_INLINE s32         str_compare( const char* s1, const char* s2, sw len );
GEN_DEF_INLINE char*       str_copy( char* dest, const char* source, sw len );
GEN_DEF_INLINE sw          str_copy_nulpad( char* dest, const char* source, sw len );
GEN_DEF_INLINE sw          str_len( const char* str );
GEN_DEF_INLINE sw          str_len( const char* str, sw max_len );
GEN_DEF_INLINE char*       str_reverse( char* str );    // NOTE: ASCII only
GEN_DEF_INLINE char const* str_skip( char const* str, char c );
GEN_DEF_INLINE char const* str_skip_any( char const* str, char const* char_list );
GEN_DEF_INLINE char const* str_trim( char const* str, b32 catch_newline );

// NOTE: ASCII only
GEN_DEF_INLINE void str_to_lower( char* str );
GEN_DEF_INLINE void str_to_upper( char* str );

s64  str_to_i64( const char* str, char** end_ptr, s32 base );
void i64_to_str( s64 value, char* string, s32 base );
void u64_to_str( u64 value, char* string, s32 base );
f64  str_to_f64( const char* str, char** end_ptr );

GEN_IMPL_INLINE const char* char_first_occurence( const char* s, char c )
{
	char ch = c;
	for ( ; *s != ch; s++ )
	{
		if ( *s == '\0' )
			return NULL;
	}
	return s;
}

GEN_IMPL_INLINE b32 char_is_alpha( char c )
{
	if ( ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) )
		return true;
	return false;
}

GEN_IMPL_INLINE b32 char_is_alphanumeric( char c )
{
	return char_is_alpha( c ) || char_is_digit( c );
}

GEN_IMPL_INLINE b32 char_is_digit( char c )
{
	if ( c >= '0' && c <= '9' )
		return true;
	return false;
}

GEN_IMPL_INLINE b32 char_is_hex_digit( char c )
{
	if ( char_is_digit( c ) || ( c >= 'a' && c <= 'f' ) || ( c >= 'A' && c <= 'F' ) )
		return true;
	return false;
}

GEN_IMPL_INLINE b32 char_is_space( char c )
{
	if ( c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v' )
		return true;
	return false;
}

GEN_IMPL_INLINE char char_to_lower( char c )
{
	if ( c >= 'A' && c <= 'Z' )
		return 'a' + ( c - 'A' );
	return c;
}

GEN_IMPL_INLINE char char_to_upper( char c )
{
	if ( c >= 'a' && c <= 'z' )
		return 'A' + ( c - 'a' );
	return c;
}

GEN_IMPL_INLINE s32 digit_to_int( char c )
{
	return char_is_digit( c ) ? c - '0' : c - 'W';
}

GEN_IMPL_INLINE s32 hex_digit_to_int( char c )
{
	if ( char_is_digit( c ) )
		return digit_to_int( c );
	else if ( is_between( c, 'a', 'f' ) )
		return c - 'a' + 10;
	else if ( is_between( c, 'A', 'F' ) )
		return c - 'A' + 10;
	return -1;
}

GEN_IMPL_INLINE s32 str_compare( const char* s1, const char* s2 )
{
	while ( *s1 && ( *s1 == *s2 ) )
	{
		s1++, s2++;
	}
	return *( u8* )s1 - *( u8* )s2;
}

GEN_IMPL_INLINE s32 str_compare( const char* s1, const char* s2, sw len )
{
	for ( ; len > 0; s1++, s2++, len-- )
	{
		if ( *s1 != *s2 )
			return ( ( s1 < s2 ) ? -1 : +1 );
		else if ( *s1 == '\0' )
			return 0;
	}
	return 0;
}

GEN_IMPL_INLINE char* str_copy( char* dest, const char* source, sw len )
{
	GEN_ASSERT_NOT_NULL( dest );
	if ( source )
	{
		char* str = dest;
		while ( len > 0 && *source )
		{
			*str++ = *source++;
			len--;
		}
		while ( len > 0 )
		{
			*str++ = '\0';
			len--;
		}
	}
	return dest;
}

GEN_IMPL_INLINE sw str_copy_nulpad( char* dest, const char* source, sw len )
{
	sw result = 0;
	GEN_ASSERT_NOT_NULL( dest );
	if ( source )
	{
		const char* source_start = source;
		char*       str          = dest;
		while ( len > 0 && *source )
		{
			*str++ = *source++;
			len--;
		}
		while ( len > 0 )
		{
			*str++ = '\0';
			len--;
		}

		result = source - source_start;
	}
	return result;
}

GEN_IMPL_INLINE sw str_len( const char* str )
{
	if ( str == NULL )
	{
		return 0;
	}
	const char* p = str;
	while ( *str )
		str++;
	return str - p;
}

GEN_IMPL_INLINE sw str_len( const char* str, sw max_len )
{
	const char* end = zpl_cast( const char* ) mem_find( str, 0, max_len );
	if ( end )
		return end - str;
	return max_len;
}

GEN_IMPL_INLINE char* str_reverse( char* str )
{
	sw    len  = str_len( str );
	char* a    = str + 0;
	char* b    = str + len - 1;
	len       /= 2;
	while ( len-- )
	{
		swap( *a, *b );
		a++, b--;
	}
	return str;
}

GEN_IMPL_INLINE char const* str_skip( char const* str, char c )
{
	while ( *str && *str != c )
	{
		++str;
	}
	return str;
}

GEN_IMPL_INLINE char const* str_skip_any( char const* str, char const* char_list )
{
	char const* closest_ptr     = zpl_cast( char const* ) pointer_add( ( void* )str, str_len( str ) );
	sw          char_list_count = str_len( char_list );
	for ( sw i = 0; i < char_list_count; i++ )
	{
		char const* p = str_skip( str, char_list[ i ] );
		closest_ptr   = min( closest_ptr, p );
	}
	return closest_ptr;
}

GEN_IMPL_INLINE char const* str_trim( char const* str, b32 catch_newline )
{
	while ( *str && char_is_space( *str ) && ( ! catch_newline || ( catch_newline && *str != '\n' ) ) )
	{
		++str;
	}
	return str;
}

GEN_IMPL_INLINE void str_to_lower( char* str )
{
	if ( ! str )
		return;
	while ( *str )
	{
		*str = char_to_lower( *str );
		str++;
	}
}

GEN_IMPL_INLINE void str_to_upper( char* str )
{
	if ( ! str )
		return;
	while ( *str )
	{
		*str = char_to_upper( *str );
		str++;
	}
}

#pragma endregion String Ops

