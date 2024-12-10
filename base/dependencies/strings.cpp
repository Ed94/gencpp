#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "hashing.cpp"
#endif

#pragma region String

String string_make_length( AllocatorInfo allocator, char const* str, ssize length )
{
	ssize const header_size = sizeof( StringHeader );

	s32   alloc_size = header_size + length + 1;
	void* allocation = alloc( allocator, alloc_size );

	if ( allocation == nullptr ) {
		String null_string = {nullptr};
		return null_string;
	}

	StringHeader*
	header = rcast(StringHeader*, allocation);
	header->Allocator = allocator;
	header->Capacity  = length;
	header->Length    = length;

	String  result = { rcast( char*, allocation) + header_size };

	if ( length && str )
		mem_copy( result, str, length );
	else
		mem_set( result, 0, alloc_size - header_size );

	result[ length ] = '\0';

	return result;
}

String string_make_reserve( AllocatorInfo allocator, ssize capacity )
{
	ssize const header_size = sizeof( StringHeader );

	s32   alloc_size = header_size + capacity + 1;
	void* allocation = alloc( allocator, alloc_size );

	if ( allocation == nullptr ) {
		String null_string = {nullptr};
		return null_string;
	}
	mem_set( allocation, 0, alloc_size );

	StringHeader*
	header            = rcast(StringHeader*, allocation);
	header->Allocator = allocator;
	header->Capacity  = capacity;
	header->Length    = 0;

	String result = { rcast(char*, allocation) + header_size };
	return result;
}

#pragma endregion String
