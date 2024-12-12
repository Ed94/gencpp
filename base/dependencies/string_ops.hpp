#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "memory.hpp"
#endif

#pragma region StrBuilder Ops

const char* char_first_occurence( const char* str, char c );

b32   char_is_alpha( char c );
b32   char_is_alphanumeric( char c );
b32   char_is_digit( char c );
b32   char_is_hex_digit( char c );
b32   char_is_space( char c );
char  char_to_lower( char c );
char  char_to_upper( char c );

s32  digit_to_int( char c );
s32  hex_digit_to_int( char c );

s32         c_str_compare( const char* s1, const char* s2 );
s32         c_str_compare_len( const char* s1, const char* s2, ssize len );
char*       c_str_copy( char* dest, const char* source, ssize len );
ssize       c_str_copy_nulpad( char* dest, const char* source, ssize len );
ssize       c_str_len( const char* str );
ssize       c_str_len_capped( const char* str, ssize max_len );
char*       c_str_reverse( char* str );    // NOTE: ASCII only
char const* c_str_skip( char const* str, char c );
char const* c_str_skip_any( char const* str, char const* char_list );
char const* c_str_trim( char const* str, b32 catch_newline );

// NOTE: ASCII only
void c_str_to_lower( char* str );
void c_str_to_upper( char* str );

s64  c_str_to_i64( const char* str, char** end_ptr, s32 base );
void i64_to_str( s64 value, char* string, s32 base );
void u64_to_str( u64 value, char* string, s32 base );
f64  c_str_to_f64( const char* str, char** end_ptr );

inline
const char* char_first_occurence( const char* s, char c )
{
	char ch = c;
	for ( ; *s != ch; s++ )
	{
		if ( *s == '\0' )
			return NULL;
	}
	return s;
}

inline
b32 char_is_alpha( char c )
{
	if ( ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' ) )
		return true;
	return false;
}

inline
b32 char_is_alphanumeric( char c )
{
	return char_is_alpha( c ) || char_is_digit( c );
}

inline
b32 char_is_digit( char c )
{
	if ( c >= '0' && c <= '9' )
		return true;
	return false;
}

inline
b32 char_is_hex_digit( char c )
{
	if ( char_is_digit( c ) || ( c >= 'a' && c <= 'f' ) || ( c >= 'A' && c <= 'F' ) )
		return true;
	return false;
}

inline
b32 char_is_space( char c )
{
	if ( c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v' )
		return true;
	return false;
}

inline
char char_to_lower( char c )
{
	if ( c >= 'A' && c <= 'Z' )
		return 'a' + ( c - 'A' );
	return c;
}

inline char char_to_upper( char c )
{
	if ( c >= 'a' && c <= 'z' )
		return 'A' + ( c - 'a' );
	return c;
}

inline
s32 digit_to_int( char c )
{
	return char_is_digit( c ) ? c - '0' : c - 'W';
}

inline
s32 hex_digit_to_int( char c )
{
	if ( char_is_digit( c ) )
		return digit_to_int( c );
	else if ( is_between( c, 'a', 'f' ) )
		return c - 'a' + 10;
	else if ( is_between( c, 'A', 'F' ) )
		return c - 'A' + 10;
	return -1;
}

inline
s32 c_str_compare( const char* s1, const char* s2 )
{
	while ( *s1 && ( *s1 == *s2 ) )
	{
		s1++, s2++;
	}
	return *( u8* )s1 - *( u8* )s2;
}

inline
s32 c_str_compare_len( const char* s1, const char* s2, ssize len )
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

inline
char* c_str_copy( char* dest, const char* source, ssize len )
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

inline
ssize c_str_copy_nulpad( char* dest, const char* source, ssize len )
{
	ssize result = 0;
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

inline
ssize c_str_len( const char* str )
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

inline
ssize c_str_len_capped( const char* str, ssize max_len )
{
	const char* end = rcast(const char*, mem_find( str, 0, max_len ));
	if ( end )
		return end - str;
	return max_len;
}

inline
char* c_str_reverse( char* str )
{
	ssize    len  = c_str_len( str );
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

inline
char const* c_str_skip( char const* str, char c )
{
	while ( *str && *str != c )
	{
		++str;
	}
	return str;
}

inline
char const* c_str_skip_any( char const* str, char const* char_list )
{
	char const* closest_ptr     = rcast( char const*, pointer_add_const( rcast(mem_ptr_const, str), c_str_len( str ) ));
	ssize       char_list_count = c_str_len( char_list );
	for ( ssize i = 0; i < char_list_count; i++ )
	{
		char const* p = c_str_skip( str, char_list[ i ] );
		closest_ptr   = min( closest_ptr, p );
	}
	return closest_ptr;
}

inline
char const* c_str_trim( char const* str, b32 catch_newline )
{
	while ( *str && char_is_space( *str ) && ( ! catch_newline || ( catch_newline && *str != '\n' ) ) )
	{
		++str;
	}
	return str;
}

inline
void c_str_to_lower( char* str )
{
	if ( ! str )
		return;
	while ( *str )
	{
		*str = char_to_lower( *str );
		str++;
	}
}

inline
void c_str_to_upper( char* str )
{
	if ( ! str )
		return;
	while ( *str )
	{
		*str = char_to_upper( *str );
		str++;
	}
}

#pragma endregion StrBuilder Ops
