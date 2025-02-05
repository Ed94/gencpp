#ifdef INTELLISENSE_DIRECTIVES
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

bool strbuilder_append_c_str_len(StrBuilder* str, char const* c_str_to_append, ssize append_length)
{
	GEN_ASSERT(str != nullptr);
	if ( rcast(sptr, c_str_to_append) > 0)
	{
		ssize curr_len = strbuilder_length(* str);

		if ( ! strbuilder_make_space_for(str, c_str_to_append, append_length))
			return false;

		StrBuilderHeader* header = strbuilder_get_header(* str);

		char* Data = * str;
		mem_copy( Data + curr_len, c_str_to_append, append_length);

		Data[curr_len + append_length] = '\0';

		header->Length = curr_len + append_length;
	}
	return c_str_to_append != nullptr;
}

void strbuilder_trim(StrBuilder str, char const* cut_set)
{
	ssize len = 0;

	char* start_pos = str;
	char* end_pos   = scast(char*, str) + strbuilder_length(str) - 1;

	while (start_pos <= end_pos && char_first_occurence(cut_set, *start_pos))
	start_pos++;

	while (end_pos > start_pos && char_first_occurence(cut_set, *end_pos))
	end_pos--;

	len = scast(ssize, (start_pos > end_pos) ? 0 : ((end_pos - start_pos) + 1));

	if (str != start_pos)
		mem_move(str, start_pos, len);

	str[len] = '\0';

	strbuilder_get_header(str)->Length = len;
}

StrBuilder strbuilder_visualize_whitespace(StrBuilder const str)
{
	StrBuilderHeader* header = (StrBuilderHeader*)(scast(char const*, str) - sizeof(StrBuilderHeader));
	StrBuilder        result = strbuilder_make_reserve(header->Allocator, strbuilder_length(str) * 2); // Assume worst case for space requirements.

	for (char const* c = strbuilder_begin(str); c != strbuilder_end(str); c = strbuilder_next(str, c))
	switch ( * c )
	{
		case ' ':
			strbuilder_append_str(& result, txt("·"));
		break;
		case '\t':
			strbuilder_append_str(& result, txt("→"));
		break;
		case '\n':
			strbuilder_append_str(& result, txt("↵"));
		break;
		case '\r':
			strbuilder_append_str(& result, txt("⏎"));
		break;
		case '\v':
			strbuilder_append_str(& result, txt("⇕"));
		break;
		case '\f':
			strbuilder_append_str(& result, txt("⌂"));
		break;
		default:
			strbuilder_append_char(& result, * c);
		break;
	}

	return result;
}

#pragma endregion StrBuilder
