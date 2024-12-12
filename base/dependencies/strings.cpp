#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "hashing.cpp"
#endif

#pragma region StrBuilder

StrBuilder strbuilder_make_length( AllocatorInfo allocator, char const* str, ssize length )
{
	ssize const header_size = sizeof( StrBuilderHeader );

	s32   alloc_size = header_size + length + 1;
	void* allocation = alloc( allocator, alloc_size );

	if ( allocation == nullptr ) {
		StrBuilder null_string = {nullptr};
		return null_string;
	}

	StrBuilderHeader*
	header = rcast(StrBuilderHeader*, allocation);
	header->Allocator = allocator;
	header->Capacity  = length;
	header->Length    = length;

	StrBuilder  result = { rcast( char*, allocation) + header_size };

	if ( length && str )
		mem_copy( result, str, length );
	else
		mem_set( result, 0, alloc_size - header_size );

	result[ length ] = '\0';

	return result;
}

StrBuilder strbuilder_make_reserve( AllocatorInfo allocator, ssize capacity )
{
	ssize const header_size = sizeof( StrBuilderHeader );

	s32   alloc_size = header_size + capacity + 1;
	void* allocation = alloc( allocator, alloc_size );

	if ( allocation == nullptr ) {
		StrBuilder null_string = {nullptr};
		return null_string;
	}
	mem_set( allocation, 0, alloc_size );

	StrBuilderHeader*
	header            = rcast(StrBuilderHeader*, allocation);
	header->Allocator = allocator;
	header->Capacity  = capacity;
	header->Length    = 0;

	StrBuilder result = { rcast(char*, allocation) + header_size };
	return result;
}

#pragma endregion StrBuilder
