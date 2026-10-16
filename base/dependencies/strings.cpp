#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "simd.hpp"
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

#if defined(GEN_SYSTEM_WINDOWS) && (defined(_M_X64) || defined(__x86_64__))
internal
ssize strbuilder_find_template_byte(char const* text, ssize length, char needle)
{
	ssize idx = 0;
	for (; (length - idx) >= 16; )
	{
		char const* at = text + idx;
		u8x16 chunk = u8x16_load(at);
		u8x16 splat = u8x16_splat(scast(u8, needle));
		u16   mask  = u8x16_mask(u8x16_equal(chunk, splat));
		if (mask != 0) {
			s32 bit_index = u32_mask_first_set(mask);
			return idx + scast(ssize, bit_index);
		}
		idx += 16;
	}
	for (; (length - idx) >= 8; )
	{
		char const* at = text + idx;
		u8x16 chunk = u8x16_load8(at);
		u8x16 splat = u8x16_splat(scast(u8, needle));
		u16   mask  = u8x16_mask(u8x16_equal(chunk, splat)) & 0xFF;
		if (mask != 0) {
			s32 bit_index = u32_mask_first_set(mask);
			return idx + scast(ssize, bit_index);
		}
		idx += 8;
	}
	for (; idx < length; ++ idx) {
		if (text[idx] == needle) return idx;
	}
	return length;
}
#else
internal
ssize strbuilder_find_template_byte(char const* text, ssize length, char needle) {
	for (ssize idx = 0; idx < length; ++ idx)
	{
		if (text[idx] == needle)
			return idx;
	}
	return length;
}
#endif

#if defined(GEN_SYSTEM_WINDOWS) && (defined(_M_X64) || defined(__x86_64__))
internal
StrTemplateSlot* strbuilder_find_template_slot(Slice_StrTemplateSlot slots, Str key)
{
	u32 key_hash = crc32(key.Ptr, key.Len);
	ssize idx = 0;
	for (; (slots.Len - idx) >= 4; idx += 4)
	{
		u32 packed[4];
		packed[0] = crc32(slots.Ptr[idx + 0].Key.Ptr, slots.Ptr[idx + 0].Key.Len);
		packed[1] = crc32(slots.Ptr[idx + 1].Key.Ptr, slots.Ptr[idx + 1].Key.Len);
		packed[2] = crc32(slots.Ptr[idx + 2].Key.Ptr, slots.Ptr[idx + 2].Key.Len);
		packed[3] = crc32(slots.Ptr[idx + 3].Key.Ptr, slots.Ptr[idx + 3].Key.Len);

		u32x4 packed_vec = u32x4_load(packed);
		u32x4 want_vec   = u32x4_splat(key_hash);
		u8    mask       = u32x4_mask(u32x4_equal(packed_vec, want_vec));

		for (s32 lane = 0; lane < 4; ++ lane)
		{
			if ((mask & (1u << lane)) != 0) {
				StrTemplateSlot* slot = & slots.Ptr[idx + lane];
				if (str_are_equal(slot[0].Key, key)) return slot;
			}
		}
	}
	for (; idx < slots.Len; ++ idx)
	{
		StrTemplateSlot* slot = & slots.Ptr[idx];
		u32 slot_hash = crc32(slot[0].Key.Ptr, slot[0].Key.Len);
		if (slot_hash == key_hash) {
			if (str_are_equal(slot[0].Key, key)) return slot;
		}
	}
	return nullptr;
}
#else
internal
StrTemplateSlot* strbuilder_find_template_slot(Slice_StrTemplateSlot slots, Str key) {
	u32 key_hash = crc32(key.Ptr, key.Len);
	for (ssize idx = 0; idx < slots.Len; ++ idx)
	{
		StrTemplateSlot* slot = & slots.Ptr[idx];
		u32 slot_hash = crc32(slot[0].Key.Ptr, slot[0].Key.Len);
		if (slot_hash == key_hash && str_are_equal(slot[0].Key, key))
			return slot;
	}
	return nullptr;
}
#endif

bool strbuilder_append_template(StrBuilder* output, Slice_StrTemplateSlot slots, Str format)
{
	GEN_ASSERT(output != nullptr && output[0] != nullptr);
	StrBuilderHeader* header = strbuilder_get_header(output[0]);
	GEN_ASSERT((header[0].Length >= 0) & (header[0].Capacity >= header[0].Length));
	GEN_ASSERT_SLICE(format);
	GEN_ASSERT_SLICE(slots);
	for (ssize slot_idx = 0; slot_idx < slots.Len; ++ slot_idx) {
		GEN_ASSERT_SLICE(slots.Ptr[slot_idx].Key);
		GEN_ASSERT_SLICE(slots.Ptr[slot_idx].Value);
	}

	bool result = false;
	if (format.Len == 0) {
		result = true;
		goto finish;
	}

	for (ssize cursor = 0; cursor < format.Len;)
	{
		if (format.Ptr[cursor] == '<')
		{
			ssize key_at  = cursor + 1;
			ssize key_max = format.Len - key_at;
			ssize key_len = strbuilder_find_template_byte(format.Ptr + key_at, key_max, '>');
			if (key_len == key_max) goto finish;

			Str              key  = { format.Ptr + key_at, key_len };
			StrTemplateSlot* slot = strbuilder_find_template_slot(slots, key);
			if (slot == nullptr) goto finish;

			if (slot[0].Value.Len > 0) {
				bool appended = strbuilder_append_c_str_len(output, slot[0].Value.Ptr, slot[0].Value.Len);
				if (appended == false) goto finish;
			}
			cursor = key_at + key_len + 1;
			continue;
		}

		ssize literal_max = format.Len - cursor;
		ssize literal_len = strbuilder_find_template_byte(format.Ptr + cursor, literal_max, '<');
		if (literal_len > 0) {
			bool appended = strbuilder_append_c_str_len(output, format.Ptr + cursor, literal_len);
			if (appended == false) goto finish;
		}
		cursor += literal_len;
	}
	result = true;
finish:
	return result;
}

#pragma endregion StrBuilder
