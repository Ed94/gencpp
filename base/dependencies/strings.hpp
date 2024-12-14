#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "hashing.hpp"
#endif

#pragma region Strings

struct Str;

Str         to_str_from_c_str       (char const* bad_string);
bool        str_are_equal           (Str lhs, Str rhs);
char const* str_back                (Str str);
bool        str_contains            (Str str, Str substring);
Str         str_duplicate           (Str str, AllocatorInfo allocator);
b32         str_starts_with         (Str str, Str substring);
Str         str_visualize_whitespace(Str str, AllocatorInfo allocator);

// Constant string with length.
struct Str
{
	char const* Ptr;
	ssize       Len;

#if GEN_COMPILER_CPP
	forceinline operator char const* ()               const { return Ptr; }
	forceinline char const& operator[]( ssize index ) const { return Ptr[index]; }

#if ! GEN_C_LIKE_CPP
	forceinline bool        is_equal            (Str rhs)                 const { return str_are_equal(* this, rhs); }
	forceinline char const* back                ()                        const { return str_back(* this); }
	forceinline bool        contains            (Str substring)           const { return str_contains(* this, substring); }
	forceinline Str         duplicate           (AllocatorInfo allocator) const { return str_duplicate(* this, allocator); }
	forceinline b32         starts_with         (Str substring)           const { return str_starts_with(* this, substring); }
	forceinline Str         visualize_whitespace(AllocatorInfo allocator) const { return str_visualize_whitespace(* this, allocator); }
#endif
#endif
};

#define cast_to_str( str ) * rcast( Str*, (str) - sizeof(ssize) )

#ifndef txt
#	if GEN_COMPILER_CPP
#		define txt( text )          Str { ( text ), sizeof( text ) - 1 }
#	else
#		define txt( text )         (Str){ ( text ), sizeof( text ) - 1 }
#	endif
#endif

GEN_API_C_BEGIN
forceinline char const* str_begin(Str str)                   { return str.Ptr; }
forceinline char const* str_end  (Str str)                   { return str.Ptr + str.Len; }
forceinline char const* str_next (Str str, char const* iter) { return iter + 1; }
GEN_API_C_END

#if GEN_COMPILER_CPP
forceinline char const* begin(Str str)                   { return str.Ptr; }
forceinline char const* end  (Str str)                   { return str.Ptr + str.Len; }
forceinline char const* next (Str str, char const* iter) { return iter + 1; }
#endif

inline
bool str_are_equal(Str lhs, Str rhs)
{
	if (lhs.Len != rhs.Len)
		return false;

	for (ssize idx = 0; idx < lhs.Len; ++idx)
		if (lhs.Ptr[idx] != rhs.Ptr[idx])
			return false;

	return true;
}

inline
char const* str_back(Str str) {
	return & str.Ptr[str.Len - 1];
}

inline
bool str_contains(Str str, Str substring)
{
	if (substring.Len > str.Len)
		return false;

	ssize main_len = str.Len;
	ssize sub_len  = substring.Len;
	for (ssize idx = 0; idx <= main_len - sub_len; ++idx)
	{
		if (c_str_compare_len(str.Ptr + idx, substring.Ptr, sub_len) == 0)
			return true;
	}
	return false;
}

inline
b32 str_starts_with(Str str, Str substring) {
	if (substring.Len > str.Len)
		return false;

	b32 result = c_str_compare_len(str.Ptr, substring.Ptr, substring.Len) == 0;
		return result;
}

inline
Str to_str_from_c_str( char const* bad_str ) {
	Str result = { bad_str, c_str_len( bad_str ) };
	return result;
}

// Dynamic StrBuilder
// This is directly based off the ZPL string api.
// They used a header pattern
// I kept it for simplicty of porting but its not necessary to keep it that way.
#pragma region StrBuilder
struct StrBuilderHeader;

#if GEN_COMPILER_C
typedef char* StrBuilder;
#else
struct StrBuilder;
#endif

forceinline usize strbuilder_grow_formula(usize value);

StrBuilder        strbuilder_make_c_str          (AllocatorInfo allocator, char const*  str);
StrBuilder        strbuilder_make_str            (AllocatorInfo allocator, Str         str);
StrBuilder        strbuilder_make_reserve        (AllocatorInfo allocator, ssize        capacity);
StrBuilder        strbuilder_make_length         (AllocatorInfo allocator, char const*  str,   ssize length);
StrBuilder        strbuilder_fmt                 (AllocatorInfo allocator, char*        buf,   ssize buf_size,  char const* fmt, ...);
StrBuilder        strbuilder_fmt_buf             (AllocatorInfo allocator, char const*  fmt, ...);
StrBuilder        strbuilder_join                (AllocatorInfo allocator, char const** parts, ssize num_parts, char const* glue);
bool              strbuilder_are_equal           (StrBuilder const lhs, StrBuilder const rhs);
bool              strbuilder_are_equal_str       (StrBuilder const lhs, Str rhs);
bool              strbuilder_make_space_for      (StrBuilder*      str, char const*  to_append, ssize add_len);
bool              strbuilder_append_char         (StrBuilder*      str, char         c);
bool              strbuilder_append_c_str        (StrBuilder*      str, char const*  c_str_to_append);
bool              strbuilder_append_c_str_len    (StrBuilder*      str, char const*  c_str_to_append, ssize length);
bool              strbuilder_append_str          (StrBuilder*      str, Str         c_str_to_append);
bool              strbuilder_append_string       (StrBuilder*      str, StrBuilder const other);
bool              strbuilder_append_fmt          (StrBuilder*      str, char const*  fmt, ...);
ssize             strbuilder_avail_space         (StrBuilder const str);
char*             strbuilder_back                (StrBuilder       str);
bool              strbuilder_contains_str        (StrBuilder const str, Str         substring);
bool              strbuilder_contains_string     (StrBuilder const str, StrBuilder const substring);
ssize             strbuilder_capacity            (StrBuilder const str);
void              strbuilder_clear               (StrBuilder       str);
StrBuilder        strbuilder_duplicate           (StrBuilder const str, AllocatorInfo allocator);
void              strbuilder_free                (StrBuilder*      str);
StrBuilderHeader* strbuilder_get_header          (StrBuilder       str);
ssize             strbuilder_length              (StrBuilder const str);
b32               strbuilder_starts_with_str     (StrBuilder const str, Str   substring);
b32               strbuilder_starts_with_string  (StrBuilder const str, StrBuilder substring);
void              strbuilder_skip_line           (StrBuilder       str);
void              strbuilder_strip_space         (StrBuilder       str);
Str               strbuilder_to_str              (StrBuilder       str);
void              strbuilder_trim                (StrBuilder       str, char const* cut_set);
void              strbuilder_trim_space          (StrBuilder       str);
StrBuilder        strbuilder_visualize_whitespace(StrBuilder const str);

struct StrBuilderHeader {
	AllocatorInfo Allocator;
	ssize         Capacity;
	ssize         Length;
};

#if GEN_COMPILER_CPP
struct StrBuilder
{
	char* Data;

	forceinline operator char*()             { return Data; }
	forceinline operator char const*() const { return Data; }
	forceinline operator Str()         const { return { Data, strbuilder_length(* this) }; }

	StrBuilder const& operator=(StrBuilder const& other) const {
		if (this == &other)
			return *this;

		StrBuilder* this_ = ccast(StrBuilder*, this);
		this_->Data = other.Data;

		return *this;
	}

	forceinline char&       operator[](ssize index)       { return Data[index]; }
	forceinline char const& operator[](ssize index) const { return Data[index]; }

	       forceinline bool operator==(std::nullptr_t) const                 { return     Data == nullptr; }
	       forceinline bool operator!=(std::nullptr_t) const                 { return     Data != nullptr; }
	friend forceinline bool operator==(std::nullptr_t, const StrBuilder str) { return str.Data == nullptr; }
	friend forceinline bool operator!=(std::nullptr_t, const StrBuilder str) { return str.Data != nullptr; }

#if ! GEN_C_LIKE_CPP
	forceinline char* begin() const { return Data; }
	forceinline char* end()   const { return Data + strbuilder_length(* this); }

#pragma region Member Mapping
	forceinline static StrBuilder make(AllocatorInfo allocator, char const* str)                { return strbuilder_make_c_str(allocator, str); }
	forceinline static StrBuilder make(AllocatorInfo allocator, Str str)                        { return strbuilder_make_str(allocator, str); }
	forceinline static StrBuilder make_reserve(AllocatorInfo allocator, ssize cap)              { return strbuilder_make_reserve(allocator, cap); }
	forceinline static StrBuilder make_length(AllocatorInfo a, char const* s, ssize l)          { return strbuilder_make_length(a, s, l); }
	forceinline static StrBuilder join(AllocatorInfo a, char const** p, ssize n, char const* g) { return strbuilder_join(a, p, n, g); }
	forceinline static usize      grow_formula(usize value)                                     { return strbuilder_grow_formula(value); }

	static
	StrBuilder fmt(AllocatorInfo allocator, char* buf, ssize buf_size, char const* fmt, ...) {
		va_list va;
		va_start(va, fmt);
		ssize res = c_str_fmt_va(buf, buf_size, fmt, va) - 1;
		va_end(va);
		return strbuilder_make_length(allocator, buf, res);
	}

	static
	StrBuilder fmt_buf(AllocatorInfo allocator, char const* fmt, ...) {
		local_persist thread_local
		char buf[GEN_PRINTF_MAXLEN] = { 0 };
		va_list va;
		va_start(va, fmt);
		ssize res = c_str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va) - 1;
		va_end(va);
		return strbuilder_make_length(allocator, buf, res);
	}

	forceinline bool              make_space_for(char const* str, ssize add_len) { return strbuilder_make_space_for(this, str, add_len); }
	forceinline bool              append(char c)                                 { return strbuilder_append_char(this, c); }
	forceinline bool              append(char const* str)                        { return strbuilder_append_c_str(this, str); }
	forceinline bool              append(char const* str, ssize length)          { return strbuilder_append_c_str_len(this, str, length); }
	forceinline bool              append(Str str)                                { return strbuilder_append_str(this, str); }
	forceinline bool              append(const StrBuilder other)                 { return strbuilder_append_string(this, other); }
	forceinline ssize             avail_space() const                            { return strbuilder_avail_space(* this); }
	forceinline char*             back()                                         { return strbuilder_back(* this); }
	forceinline bool              contains(Str substring) const                  { return strbuilder_contains_str(* this, substring); }
	forceinline bool              contains(StrBuilder const& substring) const    { return strbuilder_contains_string(* this, substring); }
	forceinline ssize             capacity() const                               { return strbuilder_capacity(* this); }
	forceinline void              clear()                                        {        strbuilder_clear(* this); }
	forceinline StrBuilder        duplicate(AllocatorInfo allocator) const       { return strbuilder_duplicate(* this, allocator); }
	forceinline void              free()                                         {        strbuilder_free(this); }
	forceinline bool              is_equal(StrBuilder const& other) const        { return strbuilder_are_equal(* this, other); }
	forceinline bool              is_equal(Str other) const                      { return strbuilder_are_equal_str(* this, other); }
	forceinline ssize             length() const                                 { return strbuilder_length(* this); }
	forceinline b32               starts_with(Str substring) const               { return strbuilder_starts_with_str(* this, substring); }
	forceinline b32               starts_with(StrBuilder substring) const        { return strbuilder_starts_with_string(* this, substring); }
	forceinline void              skip_line()                                    {        strbuilder_skip_line(* this); }
	forceinline void              strip_space()                                  {        strbuilder_strip_space(* this); }
	forceinline Str               to_str()                                       { return { Data, strbuilder_length(*this) }; }
	forceinline void              trim(char const* cut_set)                      {        strbuilder_trim(* this, cut_set); }
	forceinline void              trim_space()                                   {        strbuilder_trim_space(* this); }
	forceinline StrBuilder        visualize_whitespace() const                   { return strbuilder_visualize_whitespace(* this); }
	forceinline StrBuilderHeader& get_header()                                   { return * strbuilder_get_header(* this); }

	bool append_fmt(char const* fmt, ...) {
		ssize res;
		char buf[GEN_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		res = c_str_fmt_va(buf, count_of(buf) - 1, fmt, va) - 1;
		va_end(va);

		return strbuilder_append_c_str_len(this, buf, res);
	}
#pragma endregion Member Mapping
#endif
};
#endif

forceinline char* strbuilder_begin(StrBuilder str)                   { return ((char*) str); }
forceinline char* strbuilder_end  (StrBuilder str)                   { return ((char*) str + strbuilder_length(str)); }
forceinline char* strbuilder_next (StrBuilder str, char const* iter) { return ((char*) iter + 1); }

#if GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP
forceinline char* begin(StrBuilder str)             { return ((char*) str); }
forceinline char* end  (StrBuilder str)             { return ((char*) str + strbuilder_length(str)); }
forceinline char* next (StrBuilder str, char* iter) { return ((char*) iter + 1); }
#endif

#if GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP
forceinline bool  make_space_for(StrBuilder& str, char const* to_append, ssize add_len);
forceinline bool  append(StrBuilder& str, char c);
forceinline bool  append(StrBuilder& str, char const* c_str_to_append);
forceinline bool  append(StrBuilder& str, char const* c_str_to_append, ssize length);
forceinline bool  append(StrBuilder& str, Str c_str_to_append);
forceinline bool  append(StrBuilder& str, const StrBuilder other);
forceinline bool  append_fmt(StrBuilder& str, char const* fmt, ...);
forceinline char& back(StrBuilder& str);
forceinline void  clear(StrBuilder& str);
forceinline void  free(StrBuilder& str);
#endif

forceinline
usize strbuilder_grow_formula(usize value) {
	// Using a very aggressive growth formula to reduce time mem_copying with recursive calls to append in this library.
	return 4 * value + 8;
}

forceinline
StrBuilder strbuilder_make_c_str(AllocatorInfo allocator, char const* str) {
	ssize length = str ? c_str_len(str) : 0;
	return strbuilder_make_length(allocator, str, length);
}

forceinline
StrBuilder strbuilder_make_str(AllocatorInfo allocator, Str str) {
	return strbuilder_make_length(allocator, str.Ptr, str.Len);
}

inline
StrBuilder strbuilder_fmt(AllocatorInfo allocator, char* buf, ssize buf_size, char const* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	ssize res = c_str_fmt_va(buf, buf_size, fmt, va) - 1;
	va_end(va);

	return strbuilder_make_length(allocator, buf, res);
}

inline
StrBuilder strbuilder_fmt_buf(AllocatorInfo allocator, char const* fmt, ...)
{
	local_persist thread_local
	PrintF_Buffer buf = struct_init(PrintF_Buffer, {0});

	va_list va;
	va_start(va, fmt);
	ssize res = c_str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va) -1;
	va_end(va);

	return strbuilder_make_length(allocator, buf, res);
}

inline
StrBuilder strbuilder_join(AllocatorInfo allocator, char const** parts, ssize num_parts, char const* glue)
{
	StrBuilder result = strbuilder_make_c_str(allocator, "");

	for (ssize idx = 0; idx < num_parts; ++idx)
	{
		strbuilder_append_c_str(& result, parts[idx]);

		if (idx < num_parts - 1)
			strbuilder_append_c_str(& result, glue);
	}

	return result;
}

forceinline
bool strbuilder_append_char(StrBuilder* str, char c) {
	GEN_ASSERT(str != nullptr);
	return strbuilder_append_c_str_len( str, (char const*)& c, (ssize)1);
}

forceinline
bool strbuilder_append_c_str(StrBuilder* str, char const* c_str_to_append) {
	GEN_ASSERT(str != nullptr);
	return strbuilder_append_c_str_len(str, c_str_to_append, c_str_len(c_str_to_append));
}

inline
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

forceinline
bool strbuilder_append_str(StrBuilder* str, Str c_str_to_append) {
	GEN_ASSERT(str != nullptr);
	return strbuilder_append_c_str_len(str, c_str_to_append.Ptr, c_str_to_append.Len);
}

forceinline
bool strbuilder_append_string(StrBuilder* str, StrBuilder const other) {
	GEN_ASSERT(str != nullptr);
	return strbuilder_append_c_str_len(str, (char const*)other, strbuilder_length(other));
}

bool strbuilder_append_fmt(StrBuilder* str, char const* fmt, ...) {
	GEN_ASSERT(str != nullptr);
	ssize res;
	char buf[GEN_PRINTF_MAXLEN] = { 0 };

	va_list va;
	va_start(va, fmt);
	res = c_str_fmt_va(buf, count_of(buf) - 1, fmt, va) - 1;
	va_end(va);

	return strbuilder_append_c_str_len(str, (char const*)buf, res);
}

inline
bool strbuilder_are_equal_string(StrBuilder const lhs, StrBuilder const rhs)
{
	if (strbuilder_length(lhs) != strbuilder_length(rhs))
		return false;

	for (ssize idx = 0; idx < strbuilder_length(lhs); ++idx)
		if (lhs[idx] != rhs[idx])
			return false;

	return true;
}

inline
bool strbuilder_are_equal_str(StrBuilder const lhs, Str rhs)
{
	if (strbuilder_length(lhs) != (rhs.Len))
		return false;

	for (ssize idx = 0; idx < strbuilder_length(lhs); ++idx)
		if (lhs[idx] != rhs.Ptr[idx])
			return false;

	return true;
}

forceinline
ssize strbuilder_avail_space(StrBuilder const str) {
	StrBuilderHeader const* header = rcast(StrBuilderHeader const*, scast(char const*, str) - sizeof(StrBuilderHeader));
	return header->Capacity - header->Length;
}

forceinline
char* strbuilder_back(StrBuilder str) {
	return & (str)[strbuilder_length(str) - 1];
}

inline
bool strbuilder_contains_StrC(StrBuilder const str, Str substring)
{
	StrBuilderHeader const* header = rcast(StrBuilderHeader const*, scast(char const*, str) - sizeof(StrBuilderHeader));

	if (substring.Len > header->Length)
		return false;

	ssize main_len = header->Length;
	ssize sub_len  = substring.Len;

	for (ssize idx = 0; idx <= main_len - sub_len; ++idx)
	{
		if (c_str_compare_len(str + idx, substring.Ptr, sub_len) == 0)
			return true;
	}

	return false;
}

inline
bool strbuilder_contains_string(StrBuilder const str, StrBuilder const substring)
{
	StrBuilderHeader const* header = rcast(StrBuilderHeader const*, scast(char const*, str) - sizeof(StrBuilderHeader));

	if (strbuilder_length(substring) > header->Length)
		return false;

	ssize main_len = header->Length;
	ssize sub_len  = strbuilder_length(substring);

	for (ssize idx = 0; idx <= main_len - sub_len; ++idx)
	{
		if (c_str_compare_len(str + idx, substring, sub_len) == 0)
			return true;
	}

	return false;
}

forceinline
ssize strbuilder_capacity(StrBuilder const str) {
	StrBuilderHeader const* header = rcast(StrBuilderHeader const*, scast(char const*, str) - sizeof(StrBuilderHeader));
	return header->Capacity;
}

forceinline
void strbuilder_clear(StrBuilder str) {
	strbuilder_get_header(str)->Length = 0;
}

forceinline
StrBuilder strbuilder_duplicate(StrBuilder const str, AllocatorInfo allocator) {
	return strbuilder_make_length(allocator, str, strbuilder_length(str));
}

forceinline
void strbuilder_free(StrBuilder* str) {
	GEN_ASSERT(str != nullptr);
	if (! (* str))
		return;

	StrBuilderHeader* header = strbuilder_get_header(* str);
	allocator_free(header->Allocator, header);
}

forceinline
StrBuilderHeader* strbuilder_get_header(StrBuilder str) {
	return (StrBuilderHeader*)(scast(char*, str) - sizeof(StrBuilderHeader));
}

forceinline
ssize strbuilder_length(StrBuilder const str)
{
	StrBuilderHeader const* header = rcast(StrBuilderHeader const*, scast(char const*, str) - sizeof(StrBuilderHeader));
	return header->Length;
}

inline
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

forceinline
b32 strbuilder_starts_with_str(StrBuilder const str, Str substring) {
	if (substring.Len > strbuilder_length(str))
	return false;

	b32 result = c_str_compare_len(str, substring.Ptr, substring.Len) == 0;
	return result;
}

forceinline
b32 strbuilder_starts_with_string(StrBuilder const str, StrBuilder substring) {
	if (strbuilder_length(substring) > strbuilder_length(str))
		return false;

	b32 result = c_str_compare_len(str, substring, strbuilder_length(substring) - 1) == 0;
	return result;
}

inline
void strbuilder_skip_line(StrBuilder str)
{
#define current (*scanner)
	char* scanner = str;
	while (current != '\r' && current != '\n') {
		++scanner;
	}

	s32 new_length = scanner - str;

	if (current == '\r') {
		new_length += 1;
	}

	mem_move((char*)str, scanner, new_length);

	StrBuilderHeader* header = strbuilder_get_header(str);
	header->Length = new_length;
#undef current
}

inline
void strip_space(StrBuilder str)
{
	char* write_pos = str;
	char* read_pos  = str;

	while (* read_pos)
	{
		if (! char_is_space(* read_pos))
		{
   			* write_pos = * read_pos;
			write_pos++;
		}
		read_pos++;
	}
   write_pos[0] = '\0';  // Null-terminate the modified string

	// Update the length if needed
	strbuilder_get_header(str)->Length = write_pos - str;
}

forceinline
Str strbuilder_to_str(StrBuilder str) {
	Str result = { (char const*)str, strbuilder_length(str) };
	return result;
}

inline
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

forceinline
void strbuilder_trim_space(StrBuilder str) {
	strbuilder_trim(str, " \t\r\n\v\f");
}

inline
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

#if GEN_COMPILER_CPP
struct StrBuilder_POD {
	char* Data;
};
static_assert( sizeof( StrBuilder_POD ) == sizeof( StrBuilder ), "StrBuilder is not a POD" );
#endif

forceinline
Str str_duplicate(Str str, AllocatorInfo allocator) {
	Str result = strbuilder_to_str( strbuilder_make_length(allocator, str.Ptr, str.Len));
	return result;
}

inline
Str str_visualize_whitespace(Str str, AllocatorInfo allocator)
{
	StrBuilder result = strbuilder_make_reserve(allocator, str.Len * 2); // Assume worst case for space requirements.
	for (char const* c = str_begin(str); c != str_end(str); c = str_next(str, c))
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
	return strbuilder_to_str(result);
}

// Represents strings cached with the string table.
// Should never be modified, if changed string is desired, cache_string( str ) another.
typedef Str StrCached;

// Implements basic string interning. Data structure is based off the ZPL Hashtable.
typedef HashTable(StrCached) StringTable;
#pragma endregion Strings
