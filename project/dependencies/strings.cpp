#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "hashing.cpp"
#endif

#pragma region String

String String::fmt( AllocatorInfo allocator, char* buf, ssize buf_size, char const* fmt, ... )
{
	va_list va;
	va_start( va, fmt );
	str_fmt_va( buf, buf_size, fmt, va );
	va_end( va );

	return make( allocator, buf );
}

String String::make_length( AllocatorInfo allocator, char const* str, ssize length )
{
	constexpr ssize header_size = sizeof( Header );

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

String String::make_reserve( AllocatorInfo allocator, ssize capacity )
{
	constexpr ssize header_size = sizeof( Header );

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

	String result = { rcast(char*, allocation) + header_size };
	return result;
}

String String::fmt_buf( AllocatorInfo allocator, char const* fmt, ... )
{
	local_persist thread_local
	char buf[ GEN_PRINTF_MAXLEN ] = { 0 };

	va_list va;
	va_start( va, fmt );
	str_fmt_va( buf, GEN_PRINTF_MAXLEN, fmt, va );
	va_end( va );

	return make( allocator, buf );
}

bool String::append_fmt( char const* fmt, ... )
{
	ssize   res;
	char buf[ GEN_PRINTF_MAXLEN ] = { 0 };

	va_list va;
	va_start( va, fmt );
	res = str_fmt_va( buf, count_of( buf ) - 1, fmt, va ) - 1;
	va_end( va );

	return append( buf, res );
}

bool String::make_space_for( char const* str, ssize add_len )
{
	ssize available = avail_space();

	// NOTE: Return if there is enough space left
	if ( available >= add_len )
	{
		return true;
	}
	else
	{
		ssize new_len, old_size, new_size;

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

		header            = rcast( Header*, new_ptr);
		header->Allocator = allocator;
		header->Capacity  = new_len;

		Data = rcast( char*, header + 1 );

		return true;
	}
}
#pragma endregion String
