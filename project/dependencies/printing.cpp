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

struct _format_info
{
	s32 base;
	s32 flags;
	s32 width;
	s32 precision;
};

internal sw _print_string( char* text, sw max_len, _format_info* info, char const* str )
{
	sw    res = 0, len = 0;
	sw    remaining = max_len;
	char* begin     = text;

	if ( str == NULL && max_len >= 6 )
	{
		res += str_copy_nulpad( text, "(null)", 6 );
		return res;
	}

	if ( info && info->precision >= 0 )
		len = str_len( str, info->precision );
	else
		len = str_len( str );

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
		res  += str_copy_nulpad( text, str, len );
		text += res;

		if ( info->width > res )
		{
			sw padding = info->width - len;

			char pad = ( info->flags & GEN_FMT_ZERO ) ? '0' : ' ';
			while ( padding-- > 0 && remaining-- > 0 )
				*text++ = pad, res++;
		}
	}
	else
	{
		if ( info && ( info->width > res ) )
		{
			sw   padding = info->width - len;
			char pad     = ( info->flags & GEN_FMT_ZERO ) ? '0' : ' ';
			while ( padding-- > 0 && remaining-- > 0 )
				*text++ = pad, res++;
		}

		if ( res + len > max_len )
			return res;
		res += str_copy_nulpad( text, str, len );
	}

	if ( info )
	{
		if ( info->flags & GEN_FMT_UPPER )
			str_to_upper( begin );
		else if ( info->flags & GEN_FMT_LOWER )
			str_to_lower( begin );
	}

	return res;
}

internal sw _print_char( char* text, sw max_len, _format_info* info, char arg )
{
	char str[ 2 ] = "";
	str[ 0 ]      = arg;
	return _print_string( text, max_len, info, str );
}

internal sw _print_repeated_char( char* text, sw max_len, _format_info* info, char arg )
{
	sw  res = 0;
	s32 rem = ( info ) ? ( info->width > 0 ) ? info->width : 1 : 1;
	res     = rem;
	while ( rem-- > 0 )
		*text++ = arg;

	return res;
}

internal sw _print_i64( char* text, sw max_len, _format_info* info, s64 value )
{
	char num[ 130 ];
	i64_to_str( value, num, info ? info->base : 10 );
	return _print_string( text, max_len, info, num );
}

internal sw _print_u64( char* text, sw max_len, _format_info* info, u64 value )
{
	char num[ 130 ];
	u64_to_str( value, num, info ? info->base : 10 );
	return _print_string( text, max_len, info, num );
}

internal sw _print_f64( char* text, sw max_len, _format_info* info, b32 is_hexadecimal, f64 arg )
{
	// TODO: Handle exponent notation
	sw    width, len, remaining = max_len;
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

		value  = zpl_cast( u64 ) arg;
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
				value  = zpl_cast( u64 )( arg * mult );
				len    = _print_u64( text, remaining, NULL, value );
				text  += len;
				if ( len >= remaining )
					remaining = min( remaining, 1 );
				else
					remaining -= len;
				arg  -= zpl_cast( f64 ) value / mult;
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

neverinline sw str_fmt_va( char* text, sw max_len, char const* fmt, va_list va )
{
	char const* text_begin = text;
	sw          remaining  = max_len, res;

	while ( *fmt )
	{
		_format_info info = { 0 };
		sw           len  = 0;
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
			info.width = zpl_cast( s32 ) str_to_i64( fmt, zpl_cast( char** ) & fmt, 10 );
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
				info.precision = zpl_cast( s32 ) str_to_i64( fmt, zpl_cast( char** ) & fmt, 10 );
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
				len = _print_char( text, remaining, &info, zpl_cast( char ) va_arg( va, int ) );
				break;

			case 's' :
				len = _print_string( text, remaining, &info, va_arg( va, char* ) );
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
						value = zpl_cast( u64 ) zpl_cast( u8 ) va_arg( va, int );
						break;
					case GEN_FMT_SHORT :
						value = zpl_cast( u64 ) zpl_cast( u16 ) va_arg( va, int );
						break;
					case GEN_FMT_LONG :
						value = zpl_cast( u64 ) va_arg( va, unsigned long );
						break;
					case GEN_FMT_LLONG :
						value = zpl_cast( u64 ) va_arg( va, unsigned long long );
						break;
					case GEN_FMT_SIZE :
						value = zpl_cast( u64 ) va_arg( va, uw );
						break;
					case GEN_FMT_INTPTR :
						value = zpl_cast( u64 ) va_arg( va, uptr );
						break;
					default :
						value = zpl_cast( u64 ) va_arg( va, unsigned int );
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
						value = zpl_cast( s64 ) zpl_cast( s8 ) va_arg( va, int );
						break;
					case GEN_FMT_SHORT :
						value = zpl_cast( s64 ) zpl_cast( s16 ) va_arg( va, int );
						break;
					case GEN_FMT_LONG :
						value = zpl_cast( s64 ) va_arg( va, long );
						break;
					case GEN_FMT_LLONG :
						value = zpl_cast( s64 ) va_arg( va, long long );
						break;
					case GEN_FMT_SIZE :
						value = zpl_cast( s64 ) va_arg( va, uw );
						break;
					case GEN_FMT_INTPTR :
						value = zpl_cast( s64 ) va_arg( va, uptr );
						break;
					default :
						value = zpl_cast( s64 ) va_arg( va, int );
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

char* str_fmt_buf_va( char const* fmt, va_list va )
{
	local_persist thread_local char buffer[ GEN_PRINTF_MAXLEN ];
	str_fmt_va( buffer, size_of( buffer ), fmt, va );
	return buffer;
}

char* str_fmt_buf( char const* fmt, ... )
{
	va_list va;
	char*   str;
	va_start( va, fmt );
	str = str_fmt_buf_va( fmt, va );
	va_end( va );
	return str;
}

sw str_fmt_file_va( struct FileInfo* f, char const* fmt, va_list va )
{
	local_persist thread_local char buf[ GEN_PRINTF_MAXLEN ];
	sw                              len = str_fmt_va( buf, size_of( buf ), fmt, va );
	b32                             res = file_write( f, buf, len - 1 );    // NOTE: prevent extra whitespace
	return res ? len : -1;
}

sw str_fmt_file( struct FileInfo* f, char const* fmt, ... )
{
	sw      res;
	va_list va;
	va_start( va, fmt );
	res = str_fmt_file_va( f, fmt, va );
	va_end( va );
	return res;
}

sw str_fmt_out_va( char const* fmt, va_list va )
{
	return str_fmt_file_va( file_get_standard( EFileStandard_OUTPUT ), fmt, va );
}

sw str_fmt_out_err_va( char const* fmt, va_list va )
{
	return str_fmt_file_va( file_get_standard( EFileStandard_ERROR ), fmt, va );
}

sw str_fmt_out_err( char const* fmt, ... )
{
	sw      res;
	va_list va;
	va_start( va, fmt );
	res = str_fmt_out_err_va( fmt, va );
	va_end( va );
	return res;
}

#pragma endregion Printing

