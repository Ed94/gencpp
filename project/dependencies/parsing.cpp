#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#endif

#pragma region ADT
GEN_API_C_BEGIN

#define _adt_fprintf( s_, fmt_, ... )                      \
	do                                                     \
	{                                                      \
		if ( str_fmt_file( s_, fmt_, ##__VA_ARGS__ ) < 0 ) \
			return EADT_ERROR_OUT_OF_MEMORY;               \
	} while ( 0 )

u8 adt_make_branch( ADT_Node* node, AllocatorInfo backing, char const* name, b32 is_array )
{
	ADT_Type type = EADT_TYPE_OBJECT;
	if ( is_array )
		type = EADT_TYPE_ARRAY;

	ADT_Node* parent = node->parent;
	zero_item( node );

	node->type   = type;
	node->name   = name;
	node->parent = parent;
	node->nodes  = array_init<ADT_Node>( backing );

	if ( ! node->nodes )
		return EADT_ERROR_OUT_OF_MEMORY;

	return 0;
}

u8 adt_destroy_branch( ADT_Node* node )
{
	GEN_ASSERT_NOT_NULL( node );
	if ( ( node->type == EADT_TYPE_OBJECT || node->type == EADT_TYPE_ARRAY ) && node->nodes )
	{
		for ( ssize i = 0; i < scast(ssize, array_num(node->nodes)); ++i )
		{
			adt_destroy_branch( node->nodes + i );
		}

		array_free(& node->nodes);
	}
	return 0;
}

u8 adt_make_leaf( ADT_Node* node, char const* name, ADT_Type type )
{
	GEN_ASSERT( type != EADT_TYPE_OBJECT && type != EADT_TYPE_ARRAY );

	ADT_Node* parent = node->parent;
	zero_item( node );

	node->type   = type;
	node->name   = name;
	node->parent = parent;
	return 0;
}

ADT_Node* adt_find( ADT_Node* node, char const* name, b32 deep_search )
{
	if ( node->type != EADT_TYPE_OBJECT )
	{
		return NULL;
	}

	for ( ssize i = 0; i < scast(ssize, array_num(node->nodes)); i++ )
	{
		if ( ! str_compare( node->nodes[ i ].name, name ) )
		{
			return ( node->nodes + i );
		}
	}

	if ( deep_search )
	{
		for ( ssize i = 0; i < scast(ssize, array_num(node->nodes)); i++ )
		{
			ADT_Node* res = adt_find( node->nodes + i, name, deep_search );

			if ( res != NULL )
				return res;
		}
	}

	return NULL;
}

internal ADT_Node* _adt_get_value( ADT_Node* node, char const* value )
{
	switch ( node->type )
	{
		case EADT_TYPE_MULTISTRING :
		case EADT_TYPE_STRING :
			{
				if ( node->string && ! str_compare( node->string, value ) )
				{
					return node;
				}
			}
			break;
		case EADT_TYPE_INTEGER :
		case EADT_TYPE_REAL :
			{
				char     back[ 4096 ] = { 0 };
				FileInfo tmp;

				/* allocate a file descriptor for a memory-mapped number to string conversion, input source buffer is not cloned, however. */
				file_stream_open( &tmp, heap(), ( u8* )back, size_of( back ), EFileStream_WRITABLE );
				adt_print_number( &tmp, node );

				ssize  fsize = 0;
				u8* buf   = file_stream_buf( &tmp, &fsize );

				if ( ! str_compare( ( char const* )buf, value ) )
				{
					file_close( &tmp );
					return node;
				}

				file_close( &tmp );
			}
			break;
		default :
			break; /* node doesn't support value based lookup */
	}

	return NULL;
}

internal ADT_Node* _adt_get_field( ADT_Node* node, char* name, char* value )
{
	for ( ssize i = 0; i < scast(ssize, array_num(node->nodes)); i++ )
	{
		if ( ! str_compare( node->nodes[ i ].name, name ) )
		{
			ADT_Node* child = &node->nodes[ i ];
			if ( _adt_get_value( child, value ) )
			{
				return node; /* this object does contain a field of a specified value! */
			}
		}
	}

	return NULL;
}

ADT_Node* adt_query( ADT_Node* node, char const* uri )
{
	GEN_ASSERT_NOT_NULL( uri );

	if ( *uri == '/' )
	{
		uri++;
	}

	if ( *uri == 0 )
	{
		return node;
	}

	if ( ! node || ( node->type != EADT_TYPE_OBJECT && node->type != EADT_TYPE_ARRAY ) )
	{
		return NULL;
	}

#if defined EADT_URI_DEBUG || 0
	str_fmt_out( "uri: %s\n", uri );
#endif

	char *    p = ( char* )uri, *b = p, *e = p;
	ADT_Node* found_node = NULL;

	b = p;
	p = e     = ( char* )str_skip( p, '/' );
	char* buf = str_fmt_buf( "%.*s", ( int )( e - b ), b );

	/* handle field value lookup */
	if ( *b == '[' )
	{
		char *l_p = buf + 1, *l_b = l_p, *l_e = l_p, *l_b2 = l_p, *l_e2 = l_p;
		l_e  = ( char* )str_skip( l_p, '=' );
		l_e2 = ( char* )str_skip( l_p, ']' );

		if ( ( ! *l_e && node->type != EADT_TYPE_ARRAY ) || ! *l_e2 )
		{
			GEN_ASSERT_MSG( 0, "Invalid field value lookup" );
			return NULL;
		}

		*l_e2 = 0;

		/* [field=value] */
		if ( *l_e )
		{
			*l_e = 0;
			l_b2 = l_e + 1;

			/* run a value comparison against our own fields */
			if ( node->type == EADT_TYPE_OBJECT )
			{
				found_node = _adt_get_field( node, l_b, l_b2 );
			}

			/* run a value comparison against any child that is an object node */
			else if ( node->type == EADT_TYPE_ARRAY )
			{
				for ( ssize i = 0; i < scast(ssize, array_num(node->nodes)); i++ )
				{
					ADT_Node* child = &node->nodes[ i ];
					if ( child->type != EADT_TYPE_OBJECT )
					{
						continue;
					}

					found_node = _adt_get_field( child, l_b, l_b2 );

					if ( found_node )
						break;
				}
			}
		}
		/* [value] */
		else
		{
			for ( ssize i = 0; i < scast(ssize, array_num(node->nodes)); i++ )
			{
				ADT_Node* child = &node->nodes[ i ];
				if ( _adt_get_value( child, l_b2 ) )
				{
					found_node = child;
					break; /* we found a matching value in array, ignore the rest of it */
				}
			}
		}

		/* go deeper if uri continues */
		if ( *e )
		{
			return adt_query( found_node, e + 1 );
		}
	}
	/* handle field name lookup */
	else if ( node->type == EADT_TYPE_OBJECT )
	{
		found_node = adt_find( node, buf, false );

		/* go deeper if uri continues */
		if ( *e )
		{
			return adt_query( found_node, e + 1 );
		}
	}
	/* handle array index lookup */
	else
	{
		ssize idx = ( ssize )str_to_i64( buf, NULL, 10 );
		if ( idx >= 0 && idx < scast(ssize, array_num(node->nodes)) )
		{
			found_node = &node->nodes[ idx ];

			/* go deeper if uri continues */
			if ( *e )
			{
				return adt_query( found_node, e + 1 );
			}
		}
	}

	return found_node;
}

ADT_Node* adt_alloc_at( ADT_Node* parent, ssize index )
{
	if ( ! parent || ( parent->type != EADT_TYPE_OBJECT && parent->type != EADT_TYPE_ARRAY ) )
	{
		return NULL;
	}

	if ( ! parent->nodes )
		return NULL;

	if ( index < 0 || index > scast(ssize, array_num(parent->nodes)) )
		return NULL;

	ADT_Node o = { 0 };
	o.parent   = parent;
	if ( ! array_append_at( & parent->nodes, o, index ) )
		return NULL;

	ADT_Node* node = & parent->nodes[index];
	return node;
}

ADT_Node* adt_alloc( ADT_Node* parent )
{
	if ( ! parent || ( parent->type != EADT_TYPE_OBJECT && parent->type != EADT_TYPE_ARRAY ) )
	{
		return NULL;
	}

	if ( ! parent->nodes )
		return NULL;

	return adt_alloc_at( parent, array_num(parent->nodes) );
}

b8 adt_set_obj( ADT_Node* obj, char const* name, AllocatorInfo backing )
{
	return adt_make_branch( obj, backing, name, 0 );
}

b8 adt_set_arr( ADT_Node* obj, char const* name, AllocatorInfo backing )
{
	return adt_make_branch( obj, backing, name, 1 );
}

b8 adt_set_str( ADT_Node* obj, char const* name, char const* value )
{
	adt_make_leaf( obj, name, EADT_TYPE_STRING );
	obj->string = value;
	return true;
}

b8 adt_set_flt( ADT_Node* obj, char const* name, f64 value )
{
	adt_make_leaf( obj, name, EADT_TYPE_REAL );
	obj->real = value;
	return true;
}

b8 adt_set_int( ADT_Node* obj, char const* name, s64 value )
{
	adt_make_leaf( obj, name, EADT_TYPE_INTEGER );
	obj->integer = value;
	return true;
}

ADT_Node* adt_move_node_at( ADT_Node* node, ADT_Node* new_parent, ssize index )
{
	GEN_ASSERT_NOT_NULL( node );
	GEN_ASSERT_NOT_NULL( new_parent );
	ADT_Node* old_parent = node->parent;
	ADT_Node* new_node   = adt_alloc_at( new_parent, index );
	*new_node            = *node;
	new_node->parent     = new_parent;
	if ( old_parent )
	{
		adt_remove_node( node );
	}
	return new_node;
}

ADT_Node* adt_move_node( ADT_Node* node, ADT_Node* new_parent )
{
	GEN_ASSERT_NOT_NULL( node );
	GEN_ASSERT_NOT_NULL( new_parent );
	GEN_ASSERT( new_parent->type == EADT_TYPE_ARRAY || new_parent->type == EADT_TYPE_OBJECT );
	return adt_move_node_at( node, new_parent, array_num(new_parent->nodes) );
}

void adt_swap_nodes( ADT_Node* node, ADT_Node* other_node )
{
	GEN_ASSERT_NOT_NULL( node );
	GEN_ASSERT_NOT_NULL( other_node );
	ADT_Node* parent                     = node->parent;
	ADT_Node* other_parent               = other_node->parent;
	ssize        index                      = ( pointer_diff( parent->nodes, node ) / size_of( ADT_Node ) );
	ssize        index2                     = ( pointer_diff( other_parent->nodes, other_node ) / size_of( ADT_Node ) );
	ADT_Node  temp                       = parent->nodes[ index ];
	temp.parent                          = other_parent;
	other_parent->nodes[ index2 ].parent = parent;
	parent->nodes[ index ]               = other_parent->nodes[ index2 ];
	other_parent->nodes[ index2 ]        = temp;
}

void adt_remove_node( ADT_Node* node )
{
	GEN_ASSERT_NOT_NULL( node );
	GEN_ASSERT_NOT_NULL( node->parent );
	ADT_Node* parent = node->parent;
	ssize        index  = ( pointer_diff( parent->nodes, node ) / size_of( ADT_Node ) );
	array_remove_at( parent->nodes, index );
}

ADT_Node* adt_append_obj( ADT_Node* parent, char const* name )
{
	ADT_Node* o = adt_alloc( parent );
	if ( ! o )
		return NULL;
	if ( adt_set_obj( o, name, array_get_header(parent->nodes)->Allocator ) )
	{
		adt_remove_node( o );
		return NULL;
	}
	return o;
}

ADT_Node* adt_append_arr( ADT_Node* parent, char const* name )
{
	ADT_Node* o = adt_alloc( parent );
	if ( ! o )
		return NULL;

	ArrayHeader* node_header = array_get_header(parent->nodes);
	if ( adt_set_arr( o, name, node_header->Allocator ) )
	{
		adt_remove_node( o );
		return NULL;
	}
	return o;
}

ADT_Node* adt_append_str( ADT_Node* parent, char const* name, char const* value )
{
	ADT_Node* o = adt_alloc( parent );
	if ( ! o )
		return NULL;
	adt_set_str( o, name, value );
	return o;
}

ADT_Node* adt_append_flt( ADT_Node* parent, char const* name, f64 value )
{
	ADT_Node* o = adt_alloc( parent );
	if ( ! o )
		return NULL;
	adt_set_flt( o, name, value );
	return o;
}

ADT_Node* adt_append_int( ADT_Node* parent, char const* name, s64 value )
{
	ADT_Node* o = adt_alloc( parent );
	if ( ! o )
		return NULL;
	adt_set_int( o, name, value );
	return o;
}

/* parser helpers */
char* adt_parse_number_strict( ADT_Node* node, char* base_str )
{
	GEN_ASSERT_NOT_NULL( node );
	GEN_ASSERT_NOT_NULL( base_str );
	char *p = base_str, *e = p;

	while ( *e )
		++e;

	while ( *p && ( char_first_occurence( "eE.+-", *p ) || char_is_hex_digit( *p ) ) )
	{
		++p;
	}

	if ( p >= e )
	{
		return adt_parse_number( node, base_str );
	}

	return base_str;
}

char* adt_parse_number( ADT_Node* node, char* base_str )
{
	GEN_ASSERT_NOT_NULL( node );
	GEN_ASSERT_NOT_NULL( base_str );
	char *p = base_str, *e = p;

	s32       base         = 0;
	s32       base2        = 0;
	u8        base2_offset = 0;
	s8        exp = 0, orig_exp = 0;
	u8        neg_zero   = 0;
	u8        lead_digit = 0;
	ADT_Type  node_type  = EADT_TYPE_UNINITIALISED;
	u8        node_props = 0;

	/* skip false positives and special cases */
	if ( ! ! char_first_occurence( "eE", *p ) || ( ! ! char_first_occurence( ".+-", *p ) && ! char_is_hex_digit( *( p + 1 ) ) && *( p + 1 ) != '.' ) )
	{
		return ++base_str;
	}

	node_type = EADT_TYPE_INTEGER;
	neg_zero  = false;

	ssize   ib        = 0;
	char buf[ 48 ] = { 0 };

	if ( *e == '+' )
		++e;
	else if ( *e == '-' )
	{
		buf[ ib++ ] = *e++;
	}

	if ( *e == '.' )
	{
		node_type   = EADT_TYPE_REAL;
		node_props  = EADT_PROPS_IS_PARSED_REAL;
		lead_digit  = false;
		buf[ ib++ ] = '0';
		do
		{
			buf[ ib++ ] = *e;
		} while ( char_is_digit( *++e ) );
	}
	else
	{
		if ( ! str_compare_len( e, "0x", 2 ) || ! str_compare_len( e, "0X", 2 ) )
		{
			node_props = EADT_PROPS_IS_HEX;
		}

		/* bail if ZPL_ADT_PROPS_IS_HEX is unset but we get 'x' on input */
		if ( char_to_lower( *e ) == 'x' && ( node_props != EADT_PROPS_IS_HEX ) )
		{
			return ++base_str;
		}

		while ( char_is_hex_digit( *e ) || char_to_lower( *e ) == 'x' )
		{
			buf[ ib++ ] = *e++;
		}

		if ( *e == '.' )
		{
			node_type  = EADT_TYPE_REAL;
			lead_digit = true;
			u32 step   = 0;

			do
			{
				buf[ ib++ ] = *e;
				++step;
			} while ( char_is_digit( *++e ) );

			if ( step < 2 )
			{
				buf[ ib++ ] = '0';
			}
		}
	}

	/* check if we have a dot here, this is a false positive (IP address, ...) */
	if ( *e == '.' )
	{
		return ++base_str;
	}

	f32  eb          = 10;
	char expbuf[ 6 ] = { 0 };
	ssize   expi        = 0;

	if ( *e && ! ! char_first_occurence( "eE", *e ) )
	{
		++e;
		if ( *e == '+' || *e == '-' || char_is_digit( *e ) )
		{
			if ( *e == '-' )
			{
				eb = 0.1f;
			}
			if ( ! char_is_digit( *e ) )
			{
				++e;
			}
			while ( char_is_digit( *e ) )
			{
				expbuf[ expi++ ] = *e++;
			}
		}

		orig_exp = exp = ( u8 )str_to_i64( expbuf, NULL, 10 );
	}

	if ( node_type == EADT_TYPE_INTEGER )
	{
		node->integer = str_to_i64( buf, 0, 0 );
#ifndef GEN_PARSER_DISABLE_ANALYSIS
		/* special case: negative zero */
		if ( node->integer == 0 && buf[ 0 ] == '-' )
		{
			neg_zero = true;
		}
#endif
		while ( orig_exp-- > 0 )
		{
			node->integer *= ( s64 )eb;
		}
	}
	else
	{
		node->real = str_to_f64( buf, 0 );

#ifndef GEN_PARSER_DISABLE_ANALYSIS
		char *q = buf, *base_string = q, *base_string2 = q;
		base_string           = ccast( char*, str_skip( base_string, '.' ));
		*base_string          = '\0';
		base_string2          = base_string + 1;
		char* base_string_off = base_string2;
		while ( *base_string_off++ == '0' )
			base2_offset++;

		base  = ( s32 )str_to_i64( q, 0, 0 );
		base2 = ( s32 )str_to_i64( base_string2, 0, 0 );
		if ( exp )
		{
			exp        = exp * ( ! ( eb == 10.0f ) ? -1 : 1 );
			node_props = EADT_PROPS_IS_EXP;
		}

		/* special case: negative zero */
		if ( base == 0 && buf[ 0 ] == '-' )
		{
			neg_zero = true;
		}
#endif
		while ( orig_exp-- > 0 )
		{
			node->real *= eb;
		}
	}

	node->type  = node_type;
	node->props = node_props;

#ifndef GEN_PARSER_DISABLE_ANALYSIS
	node->base         = base;
	node->base2        = base2;
	node->base2_offset = base2_offset;
	node->exp          = exp;
	node->neg_zero     = neg_zero;
	node->lead_digit   = lead_digit;
#else
	unused( base );
	unused( base2 );
	unused( base2_offset );
	unused( exp );
	unused( neg_zero );
	unused( lead_digit );
#endif
	return e;
}

ADT_Error adt_print_number( FileInfo* file, ADT_Node* node )
{
	GEN_ASSERT_NOT_NULL( file );
	GEN_ASSERT_NOT_NULL( node );
	if ( node->type != EADT_TYPE_INTEGER && node->type != EADT_TYPE_REAL )
	{
		return EADT_ERROR_INVALID_TYPE;
	}

#ifndef GEN_PARSER_DISABLE_ANALYSIS
	if ( node->neg_zero )
	{
		_adt_fprintf( file, "-" );
	}
#endif

	switch ( node->type )
	{
		case EADT_TYPE_INTEGER :
			{
				if ( node->props == EADT_PROPS_IS_HEX )
				{
					_adt_fprintf( file, "0x%llx", ( long long )node->integer );
				}
				else
				{
					_adt_fprintf( file, "%lld", ( long long )node->integer );
				}
			}
			break;

		case EADT_TYPE_REAL :
			{
				if ( node->props == EADT_PROPS_NAN )
				{
					_adt_fprintf( file, "NaN" );
				}
				else if ( node->props == EADT_PROPS_NAN_NEG )
				{
					_adt_fprintf( file, "-NaN" );
				}
				else if ( node->props == EADT_PROPS_INFINITY )
				{
					_adt_fprintf( file, "Infinity" );
				}
				else if ( node->props == EADT_PROPS_INFINITY_NEG )
				{
					_adt_fprintf( file, "-Infinity" );
				}
				else if ( node->props == EADT_PROPS_TRUE )
				{
					_adt_fprintf( file, "true" );
				}
				else if ( node->props == EADT_PROPS_FALSE )
				{
					_adt_fprintf( file, "false" );
				}
				else if ( node->props == EADT_PROPS_NULL )
				{
					_adt_fprintf( file, "null" );
#ifndef GEN_PARSER_DISABLE_ANALYSIS
				}
				else if ( node->props == EADT_PROPS_IS_EXP )
				{
					_adt_fprintf( file, "%lld.%0*d%llde%lld", ( long long )node->base, node->base2_offset, 0, ( long long )node->base2, ( long long )node->exp );
				}
				else if ( node->props == EADT_PROPS_IS_PARSED_REAL )
				{
					if ( ! node->lead_digit )
						_adt_fprintf( file, ".%0*d%lld", node->base2_offset, 0, ( long long )node->base2 );
					else
						_adt_fprintf( file, "%lld.%0*d%lld", ( long long int )node->base2_offset, 0, ( int )node->base, ( long long )node->base2 );
#endif
				}
				else
				{
					_adt_fprintf( file, "%f", node->real );
				}
			}
			break;
	}

	return EADT_ERROR_NONE;
}

ADT_Error adt_print_string( FileInfo* file, ADT_Node* node, char const* escaped_chars, char const* escape_symbol )
{
	GEN_ASSERT_NOT_NULL( file );
	GEN_ASSERT_NOT_NULL( node );
	GEN_ASSERT_NOT_NULL( escaped_chars );
	if ( node->type != EADT_TYPE_STRING && node->type != EADT_TYPE_MULTISTRING )
	{
		return EADT_ERROR_INVALID_TYPE;
	}

	/* escape string */
	char const *p = node->string, *b = p;

	if ( ! p )
		return EADT_ERROR_NONE;

	do
	{
		p = str_skip_any( p, escaped_chars );
		_adt_fprintf( file, "%.*s", pointer_diff( b, p ), b );
		if ( *p && ! ! char_first_occurence( escaped_chars, *p ) )
		{
			_adt_fprintf( file, "%s%c", escape_symbol, *p );
			p++;
		}
		b = p;
	} while ( *p );

	return EADT_ERROR_NONE;
}

ADT_Error adt_str_to_number( ADT_Node* node )
{
	GEN_ASSERT( node );

	if ( node->type == EADT_TYPE_REAL || node->type == EADT_TYPE_INTEGER )
		return EADT_ERROR_ALREADY_CONVERTED; /* this is already converted/parsed */
	if ( node->type != EADT_TYPE_STRING && node->type != EADT_TYPE_MULTISTRING )
	{
		return EADT_ERROR_INVALID_TYPE;
	}

	adt_parse_number( node, ( char* )node->string );

	return EADT_ERROR_NONE;
}

ADT_Error adt_str_to_number_strict( ADT_Node* node )
{
	GEN_ASSERT( node );

	if ( node->type == EADT_TYPE_REAL || node->type == EADT_TYPE_INTEGER )
		return EADT_ERROR_ALREADY_CONVERTED; /* this is already converted/parsed */
	if ( node->type != EADT_TYPE_STRING && node->type != EADT_TYPE_MULTISTRING )
	{
		return EADT_ERROR_INVALID_TYPE;
	}

	adt_parse_number_strict( node, ( char* )node->string );

	return EADT_ERROR_NONE;
}

#undef _adt_fprintf

#pragma endregion ADT

#pragma region CSV

#ifdef GEN_CSV_DEBUG
#	define GEN_CSV_ASSERT( msg ) GEN_PANIC( msg )
#else
#	define GEN_CSV_ASSERT( msg )
#endif

u8 csv_parse_delimiter( CSV_Object* root, char* text, AllocatorInfo allocator, b32 has_header, char delim )
{
	CSV_Error error = ECSV_Error__NONE;
	GEN_ASSERT_NOT_NULL( root );
	GEN_ASSERT_NOT_NULL( text );
	zero_item( root );

	adt_make_branch( root, allocator, NULL, has_header ? false : true );

	char* currentChar = text;
	char* beginChar;
	char* endChar;

	ssize columnIndex       = 0;
	ssize totalColumnIndex = 0;

	do
	{
		char delimiter = 0;
		currentChar = ccast( char*, str_trim( currentChar, false ));

		if ( *currentChar == 0 )
			break;

		ADT_Node rowItem = { 0 };
		rowItem.type     = EADT_TYPE_STRING;

	#ifndef GEN_PARSER_DISABLE_ANALYSIS
		rowItem.name_style = EADT_NAME_STYLE_NO_QUOTES;
	#endif

		/* handle string literals */
		if ( *currentChar == '"' )
		{
			currentChar   += 1;
			beginChar      = currentChar;
			endChar        = currentChar;
			rowItem.string = beginChar;
		#ifndef GEN_PARSER_DISABLE_ANALYSIS
			rowItem.name_style = EADT_NAME_STYLE_DOUBLE_QUOTE;
		#endif
			do
			{
				endChar = ccast( char*, str_skip( endChar, '"' ));

				if ( *endChar && *( endChar + 1 ) == '"' )
				{
					endChar += 2;
				}
				else
					break;
			}
			while ( *endChar );

			if ( *endChar == 0 )
			{
				GEN_CSV_ASSERT( "unmatched quoted string" );
				error = ECSV_Error__UNEXPECTED_END_OF_INPUT;
				return error;
			}

			*endChar    = 0;
			currentChar = ccast( char*, str_trim( endChar + 1, true ));
			delimiter   = * currentChar;

			/* unescape escaped quotes (so that unescaped text escapes :) */
			{
				char* escapedChar = beginChar;
				do
				{
					if ( *escapedChar == '"' && *( escapedChar + 1 ) == '"' )
					{
						mem_move( escapedChar, escapedChar + 1, str_len( escapedChar ) );
					}
					escapedChar++;
				}
				while ( *escapedChar );
			}
		}
		else if ( *currentChar == delim )
		{
			delimiter      = * currentChar;
			rowItem.string = "";
		}
		else if ( *currentChar )
		{
			/* regular data */
			beginChar      = currentChar;
			endChar        = currentChar;
			rowItem.string = beginChar;

			do
			{
				endChar++;
			}
			while ( * endChar && * endChar != delim && * endChar != '\n' );

			if ( * endChar )
			{
				currentChar = ccast( char*, str_trim( endChar, true ));

				while ( char_is_space( *( endChar - 1 ) ) )
				{
					endChar--;
				}

				delimiter = * currentChar;
				* endChar = 0;
			}
			else
			{
				delimiter   = 0;
				currentChar = endChar;
			}

			/* check if number and process if so */
			b32   skip_number = false;
			char* num_p       = beginChar;

			// We only consider hexadecimal values if they start with 0x
			if ( str_len(num_p) > 2 && num_p[0] == '0' && (num_p[1] == 'x' || num_p[1] == 'X') )
			{
				num_p += 2; // skip '0x' prefix
				do
				{
					if (!char_is_hex_digit(*num_p))
					{
						skip_number = true;
						break;
					}
				} while (*num_p++);
			}
			else
			{
				skip_number = true;
			}

			if (!skip_number)
			{
				adt_str_to_number(&rowItem);
			}
		}

		if ( columnIndex >= scast(ssize, array_num(root->nodes)) )
		{
			adt_append_arr( root, NULL );
		}

		array_append( root->nodes[ columnIndex ].nodes, rowItem );

		if ( delimiter == delim )
		{
			columnIndex++;
			currentChar++;
		}
		else if ( delimiter == '\n' || delimiter == 0 )
		{
			/* check if number of rows is not mismatched */
			if ( totalColumnIndex < columnIndex )
				totalColumnIndex = columnIndex;

			else if ( totalColumnIndex != columnIndex )
			{
				GEN_CSV_ASSERT( "mismatched rows" );
				error = ECSV_Error__MISMATCHED_ROWS;
				return error;
			}

			columnIndex = 0;

			if ( delimiter != 0 )
				currentChar++;
		}
	}
	while ( *currentChar );

	if (array_num( root->nodes) == 0 )
	{
		GEN_CSV_ASSERT( "unexpected end of input. stream is empty." );
		error = ECSV_Error__UNEXPECTED_END_OF_INPUT;
		return error;
	}

	/* consider first row as a header. */
	if ( has_header )
	{
		for ( ssize i = 0; i < scast(ssize, array_num(root->nodes)); i++ )
		{
			CSV_Object* col = root->nodes + i;
			CSV_Object* hdr = col->nodes;
			col->name       = hdr->string;
			array_remove_at(col->nodes, 0 );
		}
	}

	return error;
}

void csv_free( CSV_Object* obj )
{
	adt_destroy_branch( obj );
}

void _csv_write_record( FileInfo* file, CSV_Object* node )
{
	switch ( node->type )
	{
		case EADT_TYPE_STRING :
			{
#ifndef GEN_PARSER_DISABLE_ANALYSIS
				switch ( node->name_style )
				{
					case EADT_NAME_STYLE_DOUBLE_QUOTE :
						{
							str_fmt_file( file, "\"" );
							adt_print_string( file, node, "\"", "\"" );
							str_fmt_file( file, "\"" );
						}
						break;

					case EADT_NAME_STYLE_NO_QUOTES :
						{
#endif
							str_fmt_file( file, "%s", node->string );
#ifndef GEN_PARSER_DISABLE_ANALYSIS
						}
						break;
				}
#endif
			}
			break;

		case EADT_TYPE_REAL :
		case EADT_TYPE_INTEGER :
			{
				adt_print_number( file, node );
			}
			break;
	}
}

void _csv_write_header( FileInfo* file, CSV_Object* header )
{
	CSV_Object temp = *header;
	temp.string     = temp.name;
	temp.type       = EADT_TYPE_STRING;
	_csv_write_record( file, &temp );
}

void csv_write_delimiter( FileInfo* file, CSV_Object* obj, char delimiter )
{
	GEN_ASSERT_NOT_NULL( file );
	GEN_ASSERT_NOT_NULL( obj );
	GEN_ASSERT( obj->nodes );
	ssize cols = array_num(obj->nodes);
	if ( cols == 0 )
		return;

	ssize rows = array_num(obj->nodes[ 0 ].nodes);
	if ( rows == 0 )
		return;

	b32 has_headers = obj->nodes[ 0 ].name != NULL;

	if ( has_headers )
	{
		for ( ssize i = 0; i < cols; i++ )
		{
			_csv_write_header( file, &obj->nodes[ i ] );
			if ( i + 1 != cols )
			{
				str_fmt_file( file, "%c", delimiter );
			}
		}
		str_fmt_file( file, "\n" );
	}

	for ( ssize r = 0; r < rows; r++ )
	{
		for ( ssize i = 0; i < cols; i++ )
		{
			_csv_write_record( file, &obj->nodes[ i ].nodes[ r ] );
			if ( i + 1 != cols )
			{
				str_fmt_file( file, "%c", delimiter );
			}
		}
		str_fmt_file( file, "\n" );
	}
}

String csv_write_string_delimiter( AllocatorInfo a, CSV_Object* obj, char delimiter )
{
	FileInfo tmp;
	file_stream_new( &tmp, a );
	csv_write_delimiter( &tmp, obj, delimiter );

	ssize  fsize;
	u8*    buf    = file_stream_buf( &tmp, &fsize );
	String output = string_make_length( a, ( char* )buf, fsize );
	file_close( &tmp );
	return output;
}

GEN_API_C_END
#pragma endregion CSV
