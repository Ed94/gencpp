#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "debug.cpp"
#endif

#pragma region String Ops

internal
ssize _scan_zpl_i64( const char* text, s32 base, s64* value )
{
	const char* text_begin = text;
	s64         result     = 0;
	b32         negative   = false;

	if ( *text == '-' )
	{
		negative = true;
		text++;
	}

	if ( base == 16 && c_str_compare_len( text, "0x", 2 ) == 0 )
		text += 2;

	for ( ;; )
	{
		s64 v;
		if ( char_is_digit( *text ) )
			v = *text - '0';
		else if ( base == 16 && char_is_hex_digit( *text ) )
			v = hex_digit_to_int( *text );
		else
			break;

		result *= base;
		result += v;
		text++;
	}

	if ( value )
	{
		if ( negative )
			result = -result;
		*value = result;
	}

	return ( text - text_begin );
}

// TODO : Are these good enough for characters?
global const char _num_to_char_table[] =
	"0123456789"
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"@$";

s64 c_str_to_i64( const char* str, char** end_ptr, s32 base )
{
	ssize  len;
	s64 value;

	if ( ! base )
	{
		if ( ( c_str_len( str ) > 2 ) && ( c_str_compare_len( str, "0x", 2 ) == 0 ) )
			base = 16;
		else
			base = 10;
	}

	len = _scan_zpl_i64( str, base, &value );
	if ( end_ptr )
		*end_ptr = ( char* )str + len;
	return value;
}

void i64_to_str( s64 value, char* string, s32 base )
{
	char* buf      = string;
	b32   negative = false;
	u64   v;

	if ( value < 0 )
	{
		negative = true;
		value    = -value;
	}

	v = scast( u64, value);
	if ( v != 0 )
	{
		while ( v > 0 )
		{
			*buf++  = _num_to_char_table[ v % base ];
			v      /= base;
		}
	}
	else
	{
		*buf++ = '0';
	}
	if ( negative )
		*buf++ = '-';
	*buf = '\0';
	c_str_reverse( string );
}

void u64_to_str( u64 value, char* string, s32 base )
{
	char* buf = string;

	if ( value )
	{
		while ( value > 0 )
		{
			*buf++  = _num_to_char_table[ value % base ];
			value  /= base;
		}
	}
	else
	{
		*buf++ = '0';
	}
	*buf = '\0';

	c_str_reverse( string );
}

f64 c_str_to_f64( const char* str, char** end_ptr )
{
	f64 result, value, sign, scale;
	s32 frac;

	while ( char_is_space( *str ) )
	{
		str++;
	}

	sign = 1.0;
	if ( *str == '-' )
	{
		sign = -1.0;
		str++;
	}
	else if ( *str == '+' )
	{
		str++;
	}

	for ( value = 0.0; char_is_digit( *str ); str++ )
	{
		value = value * 10.0 + ( *str - '0' );
	}

	if ( *str == '.' )
	{
		f64 pow10 = 10.0;
		str++;
		while ( char_is_digit( *str ) )
		{
			value += ( *str - '0' ) / pow10;
			pow10 *= 10.0;
			str++;
		}
	}

	frac  = 0;
	scale = 1.0;
	if ( ( *str == 'e' ) || ( *str == 'E' ) )
	{
		u32 exp;

		str++;
		if ( *str == '-' )
		{
			frac = 1;
			str++;
		}
		else if ( *str == '+' )
		{
			str++;
		}

		for ( exp = 0; char_is_digit( *str ); str++ )
		{
			exp = exp * 10 + ( *str - '0' );
		}
		if ( exp > 308 )
			exp = 308;

		while ( exp >= 50 )
		{
			scale *= 1e50;
			exp   -= 50;
		}
		while ( exp >= 8 )
		{
			scale *= 1e8;
			exp   -= 8;
		}
		while ( exp > 0 )
		{
			scale *= 10.0;
			exp   -= 1;
		}
	}

	result = sign * ( frac ? ( value / scale ) : ( value * scale ) );

	if ( end_ptr )
		* end_ptr = rcast( char*, ccast(char*, str) );

	return result;
}

#pragma endregion String Ops
