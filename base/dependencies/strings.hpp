#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "hashing.hpp"
#endif

#pragma region Strings

struct StrC;

StrC        to_strc_from_c_str       (char const* bad_string);
bool        strc_are_equal           (StrC lhs, StrC rhs);
char const* strc_back                (StrC str);
bool        strc_contains            (StrC str, StrC substring);
StrC        strc_duplicate           (StrC str, AllocatorInfo allocator);
b32         strc_starts_with         (StrC str, StrC substring);
StrC        strc_visualize_whitespace(StrC str, AllocatorInfo allocator);

// Constant string with length.
struct StrC
{
	ssize       Len;
	char const* Ptr;

#if GEN_COMPILER_CPP
	forceinline operator char const* ()               const { return Ptr; }
	forceinline char const& operator[]( ssize index ) const { return Ptr[index]; }

#if ! GEN_C_LIKE_CPP
	forceinline bool        is_equal            (StrC rhs)                const { return strc_are_equal(* this, rhs); }
	forceinline char const* back                ()                        const { return strc_back(* this); }
	forceinline bool        contains            (StrC substring)          const { return strc_contains(* this, substring); }
	forceinline StrC        duplicate           (AllocatorInfo allocator) const { return strc_duplicate(* this, allocator); }
	forceinline b32         starts_with         (StrC substring)          const { return strc_starts_with(* this, substring); }
	forceinline StrC        visualize_whitespace(AllocatorInfo allocator) const { return strc_visualize_whitespace(* this, allocator); }
#endif
#endif
};

#define cast_to_strc( str ) * rcast( StrC*, (str) - sizeof(ssize) )

#ifndef txt
#	if GEN_COMPILER_CPP
#		define txt( text )          StrC { sizeof( text ) - 1, ( text ) }
#	else
#		define txt( text )         (StrC){ sizeof( text ) - 1, ( text ) }
#	endif
#endif

GEN_API_C_BEGIN
forceinline char const* strc_begin(StrC str)                   { return str.Ptr; }
forceinline char const* strc_end  (StrC str)                   { return str.Ptr + str.Len; }
forceinline char const* strc_next (StrC str, char const* iter) { return iter + 1; }
GEN_API_C_END

#if GEN_COMPILER_CPP
forceinline char const* begin(StrC str)                   { return str.Ptr; }
forceinline char const* end  (StrC str)                   { return str.Ptr + str.Len; }
forceinline char const* next (StrC str, char const* iter) { return iter + 1; }
#endif

inline
bool strc_are_equal(StrC lhs, StrC rhs)
{
	if (lhs.Len != rhs.Len)
		return false;

	for (ssize idx = 0; idx < lhs.Len; ++idx)
		if (lhs.Ptr[idx] != rhs.Ptr[idx])
			return false;

	return true;
}

inline
char const* strc_back(StrC str) {
	return & str.Ptr[str.Len - 1];
}

inline
bool strc_contains(StrC str, StrC substring)
{
	if (substring.Len > str.Len)
		return false;

	ssize main_len = str.Len;
	ssize sub_len  = substring.Len;
	for (ssize idx = 0; idx <= main_len - sub_len; ++idx)
	{
		if (str_compare_len(str.Ptr + idx, substring.Ptr, sub_len) == 0)
			return true;
	}
	return false;
}

inline
b32 strc_starts_with(StrC str, StrC substring) {
	if (substring.Len > str.Len)
		return false;

	b32 result = str_compare_len(str.Ptr, substring.Ptr, substring.Len) == 0;
		return result;
}

inline
StrC to_strc_from_c_str( char const* bad_str ) {
	StrC result = { str_len( bad_str ), bad_str };
	return result;
}

// Dynamic String
// This is directly based off the ZPL string api.
// They used a header pattern
// I kept it for simplicty of porting but its not necessary to keep it that way.
#pragma region String
struct StringHeader;

#if GEN_COMPILER_C
typedef char* String;
#else
struct String;
#endif

forceinline usize string_grow_formula(usize value);

String        string_make_c_str          (AllocatorInfo allocator, char const*  str);
String        string_make_strc           (AllocatorInfo allocator, StrC         str);
String        string_make_reserve        (AllocatorInfo allocator, ssize        capacity);
String        string_make_length         (AllocatorInfo allocator, char const*  str,   ssize length);
String        string_fmt                 (AllocatorInfo allocator, char*        buf,   ssize buf_size,  char const* fmt, ...);
String        string_fmt_buf             (AllocatorInfo allocator, char const*  fmt, ...);
String        string_join                (AllocatorInfo allocator, char const** parts, ssize num_parts, char const* glue);
bool          string_are_equal           (String const lhs, String const rhs);
bool          string_are_equal_strc      (String const lhs, StrC rhs);
bool          string_make_space_for      (String*      str, char const*  to_append, ssize add_len);
bool          string_append_char         (String*      str, char         c);
bool          string_append_c_str        (String*      str, char const*  str_to_append);
bool          string_append_c_str_len    (String*      str, char const*  str_to_append, ssize length);
bool          string_append_strc         (String*      str, StrC         str_to_append);
bool          string_append_string       (String*      str, String const other);
bool          string_append_fmt          (String*      str, char const*  fmt, ...);
ssize         string_avail_space         (String const str);
char*         string_back                (String       str);
bool          string_contains_strc       (String const str, StrC         substring);
bool          string_contains_string     (String const str, String const substring);
ssize         string_capacity            (String const str);
void          string_clear               (String       str);
String        string_duplicate           (String const str, AllocatorInfo allocator);
void          string_free                (String*      str);
StringHeader* string_get_header          (String       str);
ssize         string_length              (String const str);
b32           string_starts_with_strc    (String const str, StrC   substring);
b32           string_starts_with_string  (String const str, String substring);
void          string_skip_line           (String       str);
void          string_strip_space         (String       str);
StrC          string_to_strc             (String       str);
void          string_trim                (String       str, char const* cut_set);
void          string_trim_space          (String       str);
String        string_visualize_whitespace(String const str);

struct StringHeader {
	AllocatorInfo Allocator;
	ssize         Capacity;
	ssize         Length;
};

#if GEN_COMPILER_CPP
struct String
{
	char* Data;

	forceinline operator char*()             { return Data; }
	forceinline operator char const*() const { return Data; }
	forceinline operator StrC()        const { return { string_length(* this), Data }; }

	String const& operator=(String const& other) const {
		if (this == &other)
			return *this;

		String* this_ = ccast(String*, this);
		this_->Data = other.Data;

		return *this;
	}

	forceinline char&       operator[](ssize index)       { return Data[index]; }
	forceinline char const& operator[](ssize index) const { return Data[index]; }

	       forceinline bool operator==(std::nullptr_t) const             { return     Data == nullptr; }
	       forceinline bool operator!=(std::nullptr_t) const             { return     Data != nullptr; }
	friend forceinline bool operator==(std::nullptr_t, const String str) { return str.Data == nullptr; }
	friend forceinline bool operator!=(std::nullptr_t, const String str) { return str.Data != nullptr; }

#if ! GEN_C_LIKE_CPP
	forceinline char* begin() const { return Data; }
	forceinline char* end()   const { return Data + string_length(* this); }

#pragma region Member Mapping
	forceinline static String make(AllocatorInfo allocator, char const* str)                { return string_make_c_str(allocator, str); }
	forceinline static String make(AllocatorInfo allocator, StrC str)                       { return string_make_strc(allocator, str); }
	forceinline static String make_reserve(AllocatorInfo allocator, ssize cap)              { return string_make_reserve(allocator, cap); }
	forceinline static String make_length(AllocatorInfo a, char const* s, ssize l)          { return string_make_length(a, s, l); }
	forceinline static String join(AllocatorInfo a, char const** p, ssize n, char const* g) { return string_join(a, p, n, g); }
	forceinline static usize  grow_formula(usize value)                                     { return string_grow_formula(value); }

	static
	String fmt(AllocatorInfo allocator, char* buf, ssize buf_size, char const* fmt, ...) {
		va_list va;
		va_start(va, fmt);
		ssize res = str_fmt_va(buf, buf_size, fmt, va) - 1;
		va_end(va);
		return string_make_length(allocator, buf, res);
	}

	static
	String fmt_buf(AllocatorInfo allocator, char const* fmt, ...) {
		local_persist thread_local
		char buf[GEN_PRINTF_MAXLEN] = { 0 };
		va_list va;
		va_start(va, fmt);
		ssize res = str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va) - 1;
		va_end(va);
		return string_make_length(allocator, buf, res);
	}

	forceinline bool          make_space_for(char const* str, ssize add_len) { return string_make_space_for(this, str, add_len); }
	forceinline bool          append(char c)                                 { return string_append_char(this, c); }
	forceinline bool          append(char const* str)                        { return string_append_c_str(this, str); }
	forceinline bool          append(char const* str, ssize length)          { return string_append_c_str_len(this, str, length); }
	forceinline bool          append(StrC str)                               { return string_append_strc(this, str); }
	forceinline bool          append(const String other)                     { return string_append_string(this, other); }
	forceinline ssize         avail_space() const                            { return string_avail_space(* this); }
	forceinline char*         back()                                         { return string_back(* this); }
	forceinline bool          contains(StrC substring) const                 { return string_contains_strc(* this, substring); }
	forceinline bool          contains(String const& substring) const        { return string_contains_string(* this, substring); }
	forceinline ssize         capacity() const                               { return string_capacity(* this); }
	forceinline void          clear()                                        {        string_clear(* this); }
	forceinline String        duplicate(AllocatorInfo allocator) const       { return string_duplicate(* this, allocator); }
	forceinline void          free()                                         {        string_free(this); }
	forceinline bool          is_equal(String const& other) const            { return string_are_equal(* this, other); }
	forceinline bool          is_equal(StrC other) const                     { return string_are_equal_strc(* this, other); }
	forceinline ssize         length() const                                 { return string_length(* this); }
	forceinline b32           starts_with(StrC substring) const              { return string_starts_with_strc(* this, substring); }
	forceinline b32           starts_with(String substring) const            { return string_starts_with_string(* this, substring); }
	forceinline void          skip_line()                                    {        string_skip_line(* this); }
	forceinline void          strip_space()                                  {        string_strip_space(* this); }
	forceinline StrC          to_strc()                                      { return { string_length(*this), Data}; }
	forceinline void          trim(char const* cut_set)                      {        string_trim(* this, cut_set); }
	forceinline void          trim_space()                                   {        string_trim_space(* this); }
	forceinline String        visualize_whitespace() const                   { return string_visualize_whitespace(* this); }
	forceinline StringHeader& get_header()                                   { return * string_get_header(* this); }

	bool append_fmt(char const* fmt, ...) {
		ssize res;
		char buf[GEN_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		res = str_fmt_va(buf, count_of(buf) - 1, fmt, va) - 1;
		va_end(va);

		return string_append_c_str_len(this, buf, res);
	}
#pragma endregion Member Mapping
#endif
};
#endif

forceinline char* string_begin(String str)                   { return ((char*) str); }
forceinline char* string_end  (String str)                   { return ((char*) str + string_length(str)); }
forceinline char* string_next (String str, char const* iter) { return ((char*) iter + 1); }

#if GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP
forceinline char* begin(String str)             { return ((char*) str); }
forceinline char* end  (String str)             { return ((char*) str + string_length(str)); }
forceinline char* next (String str, char* iter) { return ((char*) iter + 1); }
#endif

#if GEN_COMPILER_CPP && ! GEN_C_LIKE_CPP
forceinline bool  make_space_for(String& str, char const* to_append, ssize add_len);
forceinline bool  append(String& str, char c);
forceinline bool  append(String& str, char const* str_to_append);
forceinline bool  append(String& str, char const* str_to_append, ssize length);
forceinline bool  append(String& str, StrC str_to_append);
forceinline bool  append(String& str, const String other);
forceinline bool  append_fmt(String& str, char const* fmt, ...);
forceinline char& back(String& str);
forceinline void  clear(String& str);
forceinline void  free(String& str);
#endif

forceinline
usize string_grow_formula(usize value) {
	// Using a very aggressive growth formula to reduce time mem_copying with recursive calls to append in this library.
	return 4 * value + 8;
}

forceinline
String string_make_c_str(AllocatorInfo allocator, char const* str) {
	ssize length = str ? str_len(str) : 0;
	return string_make_length(allocator, str, length);
}

forceinline
String string_make_strc(AllocatorInfo allocator, StrC str) {
	return string_make_length(allocator, str.Ptr, str.Len);
}

inline
String string_fmt(AllocatorInfo allocator, char* buf, ssize buf_size, char const* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	ssize res = str_fmt_va(buf, buf_size, fmt, va) - 1;
	va_end(va);

	return string_make_length(allocator, buf, res);
}

inline
String string_fmt_buf(AllocatorInfo allocator, char const* fmt, ...)
{
	local_persist thread_local
	PrintF_Buffer buf = struct_init(PrintF_Buffer, {0});

	va_list va;
	va_start(va, fmt);
	ssize res = str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va) -1;
	va_end(va);

	return string_make_length(allocator, buf, res);
}

inline
String string_join(AllocatorInfo allocator, char const** parts, ssize num_parts, char const* glue)
{
	String result = string_make_c_str(allocator, "");

	for (ssize idx = 0; idx < num_parts; ++idx)
	{
		string_append_c_str(& result, parts[idx]);

		if (idx < num_parts - 1)
			string_append_c_str(& result, glue);
	}

	return result;
}

forceinline
bool string_append_char(String* str, char c) {
	GEN_ASSERT(str != nullptr);
	return string_append_c_str_len( str, (char const*)& c, (ssize)1);
}

forceinline
bool string_append_c_str(String* str, char const* str_to_append) {
	GEN_ASSERT(str != nullptr);
	return string_append_c_str_len(str, str_to_append, str_len(str_to_append));
}

inline
bool string_append_c_str_len(String* str, char const* str_to_append, ssize append_length)
{
	GEN_ASSERT(str != nullptr);
	if ( rcast(sptr, str_to_append) > 0)
	{
		ssize curr_len = string_length(* str);

		if ( ! string_make_space_for(str, str_to_append, append_length))
			return false;

		StringHeader* header = string_get_header(* str);

		char* Data = * str;
		mem_copy( Data + curr_len, str_to_append, append_length);

		Data[curr_len + append_length] = '\0';

		header->Length = curr_len + append_length;
	}
	return str_to_append != nullptr;
}

forceinline
bool string_append_strc(String* str, StrC str_to_append) {
	GEN_ASSERT(str != nullptr);
	return string_append_c_str_len(str, str_to_append.Ptr, str_to_append.Len);
}

forceinline
bool string_append_string(String* str, String const other) {
	GEN_ASSERT(str != nullptr);
	return string_append_c_str_len(str, (char const*)other, string_length(other));
}

bool string_append_fmt(String* str, char const* fmt, ...) {
	GEN_ASSERT(str != nullptr);
	ssize res;
	char buf[GEN_PRINTF_MAXLEN] = { 0 };

	va_list va;
	va_start(va, fmt);
	res = str_fmt_va(buf, count_of(buf) - 1, fmt, va) - 1;
	va_end(va);

	return string_append_c_str_len(str, (char const*)buf, res);
}

inline
bool string_are_equal_string(String const lhs, String const rhs)
{
	if (string_length(lhs) != string_length(rhs))
		return false;

	for (ssize idx = 0; idx < string_length(lhs); ++idx)
		if (lhs[idx] != rhs[idx])
			return false;

	return true;
}

inline
bool string_are_equal_strc(String const lhs, StrC rhs)
{
	if (string_length(lhs) != (rhs.Len))
		return false;

	for (ssize idx = 0; idx < string_length(lhs); ++idx)
		if (lhs[idx] != rhs.Ptr[idx])
			return false;

	return true;
}

forceinline
ssize string_avail_space(String const str) {
	StringHeader const* header = rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));
	return header->Capacity - header->Length;
}

forceinline
char* string_back(String str) {
	return & (str)[string_length(str) - 1];
}

inline
bool string_contains_StrC(String const str, StrC substring)
{
	StringHeader const* header = rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));

	if (substring.Len > header->Length)
		return false;

	ssize main_len = header->Length;
	ssize sub_len  = substring.Len;

	for (ssize idx = 0; idx <= main_len - sub_len; ++idx)
	{
		if (str_compare_len(str + idx, substring.Ptr, sub_len) == 0)
			return true;
	}

	return false;
}

inline
bool string_contains_string(String const str, String const substring)
{
	StringHeader const* header = rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));

	if (string_length(substring) > header->Length)
		return false;

	ssize main_len = header->Length;
	ssize sub_len  = string_length(substring);

	for (ssize idx = 0; idx <= main_len - sub_len; ++idx)
	{
		if (str_compare_len(str + idx, substring, sub_len) == 0)
			return true;
	}

	return false;
}

forceinline
ssize string_capacity(String const str) {
	StringHeader const* header = rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));
	return header->Capacity;
}

forceinline
void string_clear(String str) {
	string_get_header(str)->Length = 0;
}

forceinline
String string_duplicate(String const str, AllocatorInfo allocator) {
	return string_make_length(allocator, str, string_length(str));
}

forceinline
void string_free(String* str) {
	GEN_ASSERT(str != nullptr);
	if (! (* str))
		return;

	StringHeader* header = string_get_header(* str);
	allocator_free(header->Allocator, header);
}

forceinline
StringHeader* string_get_header(String str) {
	return (StringHeader*)(scast(char*, str) - sizeof(StringHeader));
}

forceinline
ssize string_length(String const str)
{
	StringHeader const* header = rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));
	return header->Length;
}

inline
bool string_make_space_for(String* str, char const* to_append, ssize add_len)
{
	ssize available = string_avail_space(* str);

	if (available >= add_len) {
		return true;
	}
	else
	{
		ssize new_len, old_size, new_size;
		void* ptr;
		void* new_ptr;

		AllocatorInfo allocator = string_get_header(* str)->Allocator;
		StringHeader* header    = nullptr;

		new_len  = string_grow_formula(string_length(* str) + add_len);
		ptr      = string_get_header(* str);
		old_size = size_of(StringHeader) + string_length(* str) + 1;
		new_size = size_of(StringHeader) + new_len + 1;

		new_ptr = resize(allocator, ptr, old_size, new_size);

		if (new_ptr == nullptr)
			return false;

		header = rcast(StringHeader*, new_ptr);
		header->Allocator = allocator;
		header->Capacity  = new_len;

		char** Data = rcast(char**, str);
		* Data = rcast(char*, header + 1);

		return true;
	}
}

forceinline
b32 string_starts_with_strc(String const str, StrC substring) {
	if (substring.Len > string_length(str))
	return false;

	b32 result = str_compare_len(str, substring.Ptr, substring.Len) == 0;
	return result;
}

forceinline
b32 string_starts_with_string(String const str, String substring) {
	if (string_length(substring) > string_length(str))
		return false;

	b32 result = str_compare_len(str, substring, string_length(substring) - 1) == 0;
	return result;
}

inline
void string_skip_line(String str)
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

	StringHeader* header = string_get_header(str);
	header->Length = new_length;
#undef current
}

inline
void strip_space(String str)
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
	string_get_header(str)->Length = write_pos - str;
}

forceinline
StrC string_to_strc(String str) {
	StrC result = { string_length(str), (char const*)str };
	return result;
}

inline
void string_trim(String str, char const* cut_set)
{
	ssize len = 0;

	char* start_pos = str;
	char* end_pos   = scast(char*, str) + string_length(str) - 1;

	while (start_pos <= end_pos && char_first_occurence(cut_set, *start_pos))
	start_pos++;

	while (end_pos > start_pos && char_first_occurence(cut_set, *end_pos))
	end_pos--;

	len = scast(ssize, (start_pos > end_pos) ? 0 : ((end_pos - start_pos) + 1));

	if (str != start_pos)
		mem_move(str, start_pos, len);

	str[len] = '\0';

	string_get_header(str)->Length = len;
}

forceinline
void string_trim_space(String str) {
	string_trim(str, " \t\r\n\v\f");
}

inline
String string_visualize_whitespace(String const str)
{
	StringHeader* header = (StringHeader*)(scast(char const*, str) - sizeof(StringHeader));
	String        result = string_make_reserve(header->Allocator, string_length(str) * 2); // Assume worst case for space requirements.

	for (char const* c = string_begin(str); c != string_end(str); c = string_next(str, c))
	switch ( * c )
	{
		case ' ':
			string_append_strc(& result, txt("·"));
		break;
		case '\t':
			string_append_strc(& result, txt("→"));
		break;
		case '\n':
			string_append_strc(& result, txt("↵"));
		break;
		case '\r':
			string_append_strc(& result, txt("⏎"));
		break;
		case '\v':
			string_append_strc(& result, txt("⇕"));
		break;
		case '\f':
			string_append_strc(& result, txt("⌂"));
		break;
		default:
			string_append_char(& result, * c);
		break;
	}

	return result;
}
#pragma endregion String

#if GEN_COMPILER_CPP
struct String_POD {
	char* Data;
};
static_assert( sizeof( String_POD ) == sizeof( String ), "String is not a POD" );
#endif

forceinline
StrC strc_duplicate(StrC str, AllocatorInfo allocator) {
	StrC result = string_to_strc( string_make_length(allocator, str.Ptr, str.Len));
	return result;
}

inline
StrC strc_visualize_whitespace(StrC str, AllocatorInfo allocator)
{
	String result = string_make_reserve(allocator, str.Len * 2); // Assume worst case for space requirements.
	for (char const* c = strc_begin(str); c != strc_end(str); c = strc_next(str, c))
	switch ( * c )
	{
		case ' ':
			string_append_strc(& result, txt("·"));
		break;
		case '\t':
			string_append_strc(& result, txt("→"));
		break;
		case '\n':
			string_append_strc(& result, txt("↵"));
		break;
		case '\r':
			string_append_strc(& result, txt("⏎"));
		break;
		case '\v':
			string_append_strc(& result, txt("⇕"));
		break;
		case '\f':
			string_append_strc(& result, txt("⌂"));
		break;
		default:
			string_append_char(& result, * c);
		break;
}
	return string_to_strc(result);
}

// Represents strings cached with the string table.
// Should never be modified, if changed string is desired, cache_string( str ) another.
typedef StrC StringCached;

// Implements basic string interning. Data structure is based off the ZPL Hashtable.
typedef HashTable(StringCached) StringTable;
#pragma endregion Strings
