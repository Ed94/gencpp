#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "strbuilder_ops.cpp"
#endif

#pragma region Printing

enum
{
	GEN_FMT_MINUS = bit( 0 ),
	GEN_FMT_PLUS  = bit( 1 ),
	GEN_FMT_ALT   = bit( 2 ),
	GEN_FMT_SPACE = bit( 3 ),
	GEN_FMT_ZERO  = bit( 4 ),

	GEN_FMT_CHAR   = bit( 5 ),
	GEN_FMT_SHORT  = bit( 6 ),
	GEN_FMT_INT    = bit( 7 ),
	GEN_FMT_LONG   = bit( 8 ),
	GEN_FMT_LLONG  = bit( 9 ),
	GEN_FMT_SIZE   = bit( 10 ),
	GEN_FMT_INTPTR = bit( 11 ),

	GEN_FMT_UNSIGNED = bit( 12 ),
	GEN_FMT_LOWER    = bit( 13 ),
	GEN_FMT_UPPER    = bit( 14 ),
	GEN_FMT_WIDTH    = bit( 15 ),

	GEN_FMT_DONE = bit( 30 ),

	GEN_FMT_INTS = GEN_FMT_CHAR | GEN_FMT_SHORT | GEN_FMT_INT | GEN_FMT_LONG | GEN_FMT_LLONG | GEN_FMT_SIZE | GEN_FMT_INTPTR
};

typedef struct _format_info _format_info;
struct _format_info
{
	s32 base;
	s32 flags;
	s32 width;
	s32 precision;
};

internal ssize _print_string( char* text, ssize max_len, _format_info* info, char const* str )
{
	ssize res = 0, len = 0;
	ssize remaining = max_len;
	char* begin     = text;

	if ( str == NULL && max_len >= 6 )
	{
		res += c_str_copy_nulpad( text, "(null)", 6 );
		return res;
	}

	if ( info && info->precision >= 0 )
		// Made the design decision for this library that precision is the length of the string.
		len = info->precision;
	else
		len = c_str_len( str );

	if ( info && ( info->width == 0 && info->flags & GEN_FMT_WIDTH ) )
	{
		return res;
	}

	if ( info && ( info->width == 0 || info->flags & GEN_FMT_MINUS ) )
	{
		if ( info->precision > 0 )
			len = info->precision < len ? info->precision : len;
		if ( res + len > max_len )
			return res;
		res  += c_str_copy_nulpad( text, str, len );
		text += res;

		if ( info->width > res )
		{
			ssize padding = info->width - len;

			char pad = ( info->flags & GEN_FMT_ZERO ) ? '0' : ' ';
			while ( padding-- > 0 && remaining-- > 0 )
				*text++ = pad, res++;
		}
	}
	else
	{
		if ( info && ( info->width > res ) )
		{
			ssize   padding = info->width - len;
			char pad     = ( info->flags & GEN_FMT_ZERO ) ? '0' : ' ';
			while ( padding-- > 0 && remaining-- > 0 )
				*text++ = pad, res++;
		}

		if ( res + len > max_len )
			return res;
		res += c_str_copy_nulpad( text, str, len );
	}

	if ( info )
	{
		if ( info->flags & GEN_FMT_UPPER )
			c_str_to_upper( begin );
		else if ( info->flags & GEN_FMT_LOWER )
			c_str_to_lower( begin );
	}

	return res;
}

internal ssize _print_char( char* text, ssize max_len, _format_info* info, char arg )
{
	char str[ 2 ] = "";
	str[ 0 ]      = arg;
	return _print_string( text, max_len, info, str );
}

internal ssize _print_repeated_char( char* text, ssize max_len, _format_info* info, char arg )
{
	ssize  res = 0;
	s32 rem = ( info ) ? ( info->width > 0 ) ? info->width : 1 : 1;
	res     = rem;
	while ( rem-- > 0 )
		*text++ = arg;

	return res;
}

internal ssize _print_i64( char* text, ssize max_len, _format_info* info, s64 value )
{
	char num[ 130 ];
	i64_to_str( value, num, info ? info->base : 10 );
	return _print_string( text, max_len, info, num );
}

internal ssize _print_u64( char* text, ssize max_len, _format_info* info, u64 value )
{
	char num[ 130 ];
	u64_to_str( value, num, info ? info->base : 10 );
	return _print_string( text, max_len, info, num );
}

internal ssize _print_f64( char* text, ssize max_len, _format_info* info, b32 is_hexadecimal, f64 arg )
{
	// TODO: Handle exponent notation
	ssize    width, len, remaining = max_len;
	char* text_begin = text;

	if ( arg )
	{
		u64 value;
		if ( arg < 0 )
		{
			if ( remaining > 1 )
				*text = '-', remaining--;
			text++;
			arg = -arg;
		}
		else if ( info->flags & GEN_FMT_MINUS )
		{
			if ( remaining > 1 )
				*text = '+', remaining--;
			text++;
		}

		value  = scast( u64, arg);
		len    = _print_u64( text, remaining, NULL, value );
		text  += len;

		if ( len >= remaining )
			remaining = min( remaining, 1 );
		else
			remaining -= len;
		arg -= value;

		if ( info->precision < 0 )
			info->precision = 6;

		if ( ( info->flags & GEN_FMT_ALT ) || info->precision > 0 )
		{
			s64 mult = 10;
			if ( remaining > 1 )
				*text = '.', remaining--;
			text++;
			while ( info->precision-- > 0 )
			{
				value  = scast( u64, arg * mult );
				len    = _print_u64( text, remaining, NULL, value );
				text  += len;
				if ( len >= remaining )
					remaining = min( remaining, 1 );
				else
					remaining -= len;
				arg  -= scast( f64, value / mult);
				mult *= 10;
			}
		}
	}
	else
	{
		if ( remaining > 1 )
			*text = '0', remaining--;
		text++;
		if ( info->flags & GEN_FMT_ALT )
		{
			if ( remaining > 1 )
				*text = '.', remaining--;
			text++;
		}
	}

	width = info->width - ( text - text_begin );
	if ( width > 0 )
	{
		char  fill = ( info->flags & GEN_FMT_ZERO ) ? '0' : ' ';
		char* end  = text + remaining - 1;
		len        = ( text - text_begin );

		for ( len = ( text - text_begin ); len--; )
		{
			if ( ( text_begin + len + width ) < end )
				*( text_begin + len + width ) = *( text_begin + len );
		}

		len   = width;
		text += len;
		if ( len >= remaining )
			remaining = min( remaining, 1 );
		else
			remaining -= len;

		while ( len-- )
		{
			if ( text_begin + len < end )
				text_begin[ len ] = fill;
		}
	}

	return ( text - text_begin );
}

neverinline ssize c_str_fmt_va( char* text, ssize max_len, char const* fmt, va_list va )
{
	char const* text_begin = text;
	ssize          remaining  = max_len, res;

	while ( *fmt )
	{
		_format_info info = { 0 };
		ssize        len  = 0;
		info.precision    = -1;

		while ( *fmt && *fmt != '%' && remaining )
			*text++ = *fmt++;

		if ( *fmt == '%' )
		{
			do
			{
				switch ( *++fmt )
				{
					case '-' :
						{
							info.flags |= GEN_FMT_MINUS;
							break;
						}
					case '+' :
						{
							info.flags |= GEN_FMT_PLUS;
							break;
						}
					case '#' :
						{
							info.flags |= GEN_FMT_ALT;
							break;
						}
					case ' ' :
						{
							info.flags |= GEN_FMT_SPACE;
							break;
						}
					case '0' :
						{
							info.flags |= ( GEN_FMT_ZERO | GEN_FMT_WIDTH );
							break;
						}
					default :
						{
							info.flags |= GEN_FMT_DONE;
							break;
						}
				}
			} while ( ! ( info.flags & GEN_FMT_DONE ) );
		}

		// NOTE: Optional Width
		if ( *fmt == '*' )
		{
			int width = va_arg( va, int );
			if ( width < 0 )
			{
				info.flags |= GEN_FMT_MINUS;
				info.width  = -width;
			}
			else
			{
				info.width = width;
			}
			info.flags |= GEN_FMT_WIDTH;
			fmt++;
		}
		else
		{
			info.width = scast( s32, c_str_to_i64( fmt, ccast( char**, & fmt), 10 ));
			if ( info.width != 0 )
			{
				info.flags |= GEN_FMT_WIDTH;
			}
		}

		// NOTE: Optional Precision
		if ( *fmt == '.' )
		{
			fmt++;
			if ( *fmt == '*' )
			{
				info.precision = va_arg( va, int );
				fmt++;
			}
			else
			{
				info.precision = scast( s32, c_str_to_i64( fmt, ccast( char**, & fmt), 10 ));
			}
			info.flags &= ~GEN_FMT_ZERO;
		}

		switch ( *fmt++ )
		{
			case 'h' :
				if ( *fmt == 'h' )
				{    // hh => char
					info.flags |= GEN_FMT_CHAR;
					fmt++;
				}
				else
				{    // h => short
					info.flags |= GEN_FMT_SHORT;
				}
				break;

			case 'l' :
				if ( *fmt == 'l' )
				{    // ll => long long
					info.flags |= GEN_FMT_LLONG;
					fmt++;
				}
				else
				{    // l => long
					info.flags |= GEN_FMT_LONG;
				}
				break;

				break;

			case 'z' :    // NOTE: zpl_usize
				info.flags |= GEN_FMT_UNSIGNED;
				// fallthrough
			case 't' :    // NOTE: zpl_isize
				info.flags |= GEN_FMT_SIZE;
				break;

			default :
				fmt--;
				break;
		}

		switch ( *fmt )
		{
			case 'u' :
				info.flags |= GEN_FMT_UNSIGNED;
				// fallthrough
			case 'd' :
			case 'i' :
				info.base = 10;
				break;

			case 'o' :
				info.base = 8;
				break;

			case 'x' :
				info.base   = 16;
				info.flags |= ( GEN_FMT_UNSIGNED | GEN_FMT_LOWER );
				break;

			case 'X' :
				info.base   = 16;
				info.flags |= ( GEN_FMT_UNSIGNED | GEN_FMT_UPPER );
				break;

			case 'f' :
			case 'F' :
			case 'g' :
			case 'G' :
				len = _print_f64( text, remaining, &info, 0, va_arg( va, f64 ) );
				break;

			case 'a' :
			case 'A' :
				len = _print_f64( text, remaining, &info, 1, va_arg( va, f64 ) );
				break;

			case 'c' :
				len = _print_char( text, remaining, &info, scast( char, va_arg( va, int ) ));
				break;

			case 's' :
				len = _print_string( text, remaining, &info, va_arg( va, char* ) );
				break;

			case 'S':
			{
				if ( *(fmt + 1) == 'B' )
				{

					++ fmt;
					StrBuilder gen_str = { va_arg( va, char*) };

					info.precision = strbuilder_length(gen_str);
					len            = _print_string( text, remaining, &info, gen_str );
					break;
				}

				Str gen_str    = va_arg( va, Str);
				info.precision = gen_str.Len;
				len            = _print_string( text, remaining, &info, gen_str.Ptr );
			}
			break;

			case 'r' :
				len = _print_repeated_char( text, remaining, &info, va_arg( va, int ) );
				break;

			case 'p' :
				info.base   = 16;
				info.flags |= ( GEN_FMT_LOWER | GEN_FMT_UNSIGNED | GEN_FMT_ALT | GEN_FMT_INTPTR );
				break;

			case '%' :
				len = _print_char( text, remaining, &info, '%' );
				break;

			default :
				fmt--;
				break;
		}

		fmt++;

		if ( info.base != 0 )
		{
			if ( info.flags & GEN_FMT_UNSIGNED )
			{
				u64 value = 0;
				switch ( info.flags & GEN_FMT_INTS )
				{
					case GEN_FMT_CHAR :
						value = scast( u64, scast( u8, va_arg( va, int )));
						break;
					case GEN_FMT_SHORT :
						value = scast( u64, scast( u16, va_arg( va, int )));
						break;
					case GEN_FMT_LONG:
						value = scast( u64, va_arg( va, unsigned long ));
						break;
					case GEN_FMT_LLONG :
						value = scast( u64, va_arg( va, unsigned long long ));
						break;
					case GEN_FMT_SIZE :
						value = scast( u64, va_arg( va, usize ));
						break;
					case GEN_FMT_INTPTR :
						value = scast( u64, va_arg( va, uptr ));
						break;
					default :
						value = scast( u64, va_arg( va, unsigned int ));
						break;
				}

				len = _print_u64( text, remaining, &info, value );
			}
			else
			{
				s64 value = 0;
				switch ( info.flags & GEN_FMT_INTS )
				{
					case GEN_FMT_CHAR :
						value = scast( s64, scast( s8, va_arg( va, int )));
						break;
					case GEN_FMT_SHORT :
						value = scast( s64, scast( s16, va_arg( va, int )));
						break;
					case GEN_FMT_LONG :
						value = scast( s64, va_arg( va, long ));
						break;
					case GEN_FMT_LLONG :
						value = scast( s64, va_arg( va, long long ));
						break;
					case GEN_FMT_SIZE :
						value = scast( s64, va_arg( va, usize ));
						break;
					case GEN_FMT_INTPTR :
						value = scast( s64, va_arg( va, uptr ));
						break;
					default :
						value = scast( s64, va_arg( va, int ));
						break;
				}

				len = _print_i64( text, remaining, &info, value );
			}
		}

		text += len;
		if ( len >= remaining )
			remaining = min( remaining, 1 );
		else
			remaining -= len;
	}

	*text++ = '\0';
	res     = ( text - text_begin );
	return ( res >= max_len || res < 0 ) ? -1 : res;
}

char* c_str_fmt_buf_va( char const* fmt, va_list va )
{
	local_persist thread_local char buffer[ GEN_PRINTF_MAXLEN ];
	c_str_fmt_va( buffer, size_of( buffer ), fmt, va );
	return buffer;
}

char* c_str_fmt_buf( char const* fmt, ... )
{
	va_list va;
	char*   str;
	va_start( va, fmt );
	str = c_str_fmt_buf_va( fmt, va );
	va_end( va );
	return str;
}

ssize c_str_fmt_file_va( FileInfo* f, char const* fmt, va_list va )
{
	local_persist thread_local char buf[ GEN_PRINTF_MAXLEN ];
	ssize                              len = c_str_fmt_va( buf, size_of( buf ), fmt, va );
	b32                             res = file_write( f, buf, len - 1 );    // NOTE: prevent extra whitespace
	return res ? len : -1;
}

ssize c_str_fmt_file( FileInfo* f, char const* fmt, ... )
{
	ssize      res;
	va_list va;
	va_start( va, fmt );
	res = c_str_fmt_file_va( f, fmt, va );
	va_end( va );
	return res;
}

ssize c_str_fmt( char* str, ssize n, char const* fmt, ... )
{
	ssize      res;
	va_list va;
	va_start( va, fmt );
	res = c_str_fmt_va( str, n, fmt, va );
	va_end( va );
	return res;
}

ssize c_str_fmt_out_va( char const* fmt, va_list va )
{
	return c_str_fmt_file_va( file_get_standard( EFileStandard_OUTPUT ), fmt, va );
}

ssize c_str_fmt_out_err_va( char const* fmt, va_list va )
{
	return c_str_fmt_file_va( file_get_standard( EFileStandard_ERROR ), fmt, va );
}

ssize c_str_fmt_out_err( char const* fmt, ... )
{
	ssize      res;
	va_list va;
	va_start( va, fmt );
	res = c_str_fmt_out_err_va( fmt, va );
	va_end( va );
	return res;
}

#pragma endregion Printing
