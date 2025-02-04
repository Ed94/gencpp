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

bool strbuilder_make_space_for(StrBuilder* str, char const* to_append, ssize add_len)
{
	ssize available = strbuilder_avail_space(* str);

	if (available >= add_len) {
		return true;
	}
	else
	{
		ssize new_len, old_size, new_size;
		void* ptr;
		void* new_ptr;

		AllocatorInfo allocator = strbuilder_get_header(* str)->Allocator;
		StrBuilderHeader* header    = nullptr;

		new_len  = strbuilder_grow_formula(strbuilder_length(* str) + add_len);
		ptr      = strbuilder_get_header(* str);
		old_size = size_of(StrBuilderHeader) + strbuilder_length(* str) + 1;
		new_size = size_of(StrBuilderHeader) + new_len + 1;

		new_ptr = resize(allocator, ptr, old_size, new_size);

		if (new_ptr == nullptr)
			return false;

		header = rcast(StrBuilderHeader*, new_ptr);
		header->Allocator = allocator;
		header->Capacity  = new_len;

		char** Data = rcast(char**, str);
		* Data = rcast(char*, header + 1);

		return true;
	}
}

#pragma endregion StrBuilder
