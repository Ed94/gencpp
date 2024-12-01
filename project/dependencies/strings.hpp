#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "hashing.hpp"
#endif

#pragma region Strings

// Constant string with length.
struct StrC
{
	ssize       Len;
	char const* Ptr;

#if ! GEN_COMPILER_C
	operator char const* ()               const { return Ptr; }
	char const& operator[]( ssize index ) const { return Ptr[index]; }
#endif
};

#define cast_to_strc( str ) * rcast( StrC*, (str) - sizeof(ssize) )
#define txt( text )         StrC { sizeof( text ) - 1, ( text ) }

inline
StrC to_str( char const* str ) {
	return { str_len( str ), str };
}

// Dynamic String
// This is directly based off the ZPL string api.
// They used a header pattern
// I kept it for simplicty of porting but its not necessary to keep it that way.
#pragma region String
struct StringHeader;
struct String;

String        string_make(AllocatorInfo allocator, char const* str);
String        string_make(AllocatorInfo allocator, StrC str);
String        string_make_reserve(AllocatorInfo allocator, ssize capacity);
String        string_make_length(AllocatorInfo allocator, char const* str, ssize length);
String        string_fmt(AllocatorInfo allocator, char* buf, ssize buf_size, char const* fmt, ...);
String        string_fmt_buf(AllocatorInfo allocator, char const* fmt, ...);
String        string_join(AllocatorInfo allocator, char const** parts, ssize num_parts, char const* glue);
usize         string_grow_formula(usize value);
bool          are_equal(String lhs, String rhs);
bool          are_equal(String lhs, StrC rhs);
bool          make_space_for(String& str, char const* to_append, ssize add_len);
bool          append(String& str, char c);
bool          append(String& str, char const* str_to_append);
bool          append(String& str, char const* str_to_append, ssize length);
bool          append(String& str, StrC str_to_append);
bool          append(String& str, const String other);
bool          append_fmt(String& str, char const* fmt, ...);
ssize         avail_space(String const& str);
char&         back(String& str);
bool          contains(String const& str, StrC substring);
bool          contains(String const& str, String const& substring);
ssize         capacity(String const& str);
void          clear(String& str);
String        duplicate(String const& str, AllocatorInfo allocator);
void          free(String& str);
StringHeader& get_header(String& str);
ssize         length(String const& str);
b32           starts_with(String const& str, StrC substring);
b32           starts_with(String const& str, String substring);
void          skip_line(String& str);
void          strip_space(String& str);
void          trim(String& str, char const* cut_set);
void          trim_space(String& str);
String        visualize_whitespace(String const& str);

struct StringHeader {
	AllocatorInfo Allocator;
	ssize         Capacity;
	ssize         Length;
};

#if ! GEN_COMPILER_C
struct String
{
	char* Data;

	forceinline operator bool()              { return Data != nullptr; }
	forceinline operator char*()             { return Data; }
	forceinline operator char const*() const { return Data; }
	forceinline operator StrC() const        { return { GEN_NS length(* this), Data }; }

	String const& operator=(String const& other) const {
		if (this == &other)
			return *this;

		String* this_ = ccast(String*, this);
		this_->Data = other.Data;

		return *this;
	}

	forceinline char& operator[](ssize index)             { return Data[index]; }
	forceinline char const& operator[](ssize index) const { return Data[index]; }

	forceinline char* begin() const { return Data; }
	forceinline char* end() const   { return Data + GEN_NS length(* this); }

#if GEN_SUPPORT_CPP_MEMBER_FEATURES
#pragma region Member Mapping
	forceinline static String make(AllocatorInfo allocator, char const* str)                { return GEN_NS string_make(allocator, str); }
	forceinline static String make(AllocatorInfo allocator, StrC str)                       { return GEN_NS string_make(allocator, str); }
	forceinline static String make_reserve(AllocatorInfo allocator, ssize cap)              { return GEN_NS string_make_reserve(allocator, cap); }
	forceinline static String make_length(AllocatorInfo a, char const* s, ssize l)          { return GEN_NS string_make_length(a, s, l); }
	forceinline static String join(AllocatorInfo a, char const** p, ssize n, char const* g) { return GEN_NS string_join(a, p, n, g); }
	forceinline static usize  grow_formula(usize value)                                     { return GEN_NS string_grow_formula(value); }
	forceinline static bool   are_equal(String lhs, String rhs)                             { return GEN_NS are_equal(lhs, rhs); }
	forceinline static bool   are_equal(String lhs, StrC rhs)                               { return GEN_NS are_equal(lhs, rhs); }

	static
	String fmt(AllocatorInfo allocator, char* buf, ssize buf_size, char const* fmt, ...) {
		va_list va;
		va_start(va, fmt);
		str_fmt_va(buf, buf_size, fmt, va);
		va_end(va);
		return GEN_NS string_make(allocator, buf);
	}

	static
	String fmt_buf(AllocatorInfo allocator, char const* fmt, ...) {
		local_persist thread_local
		char buf[GEN_PRINTF_MAXLEN] = { 0 };
		va_list va;
		va_start(va, fmt);
		str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va);
		va_end(va);
		return GEN_NS string_make(allocator, buf);
	}

	forceinline bool          make_space_for(char const* str, ssize add_len) { return GEN_NS make_space_for(*this, str, add_len); }
	forceinline bool          append(char c)                                 { return GEN_NS append(*this, c); }
	forceinline bool          append(char const* str)                        { return GEN_NS append(*this, str); }
	forceinline bool          append(char const* str, ssize length)          { return GEN_NS append(*this, str, length); }
	forceinline bool          append(StrC str)                               { return GEN_NS append(*this, str); }
	forceinline bool          append(const String other)                     { return GEN_NS append(*this, other); }
	forceinline ssize         avail_space() const                            { return GEN_NS avail_space(*this); }
	forceinline char&         back()                                         { return GEN_NS back(*this); }
	forceinline bool          contains(StrC substring) const                 { return GEN_NS contains(*this, substring); }
	forceinline bool          contains(String const& substring) const        { return GEN_NS contains(*this, substring); }
	forceinline ssize         capacity() const                               { return GEN_NS capacity(*this); }
	forceinline void          clear()                                        { GEN_NS clear(*this); }
	forceinline String        duplicate(AllocatorInfo allocator) const       { return GEN_NS duplicate(*this, allocator); }
	forceinline void          free()                                         { GEN_NS free(*this); }
	forceinline ssize         length() const                                 { return GEN_NS length(*this); }
	forceinline b32           starts_with(StrC substring) const              { return GEN_NS starts_with(*this, substring); }
	forceinline b32           starts_with(String substring) const            { return GEN_NS starts_with(*this, substring); }
	forceinline void          skip_line()                                    { GEN_NS skip_line(*this); }
	forceinline void          strip_space()                                  { GEN_NS strip_space(*this); }
	forceinline void          trim(char const* cut_set)                      { GEN_NS trim(*this, cut_set); }
	forceinline void          trim_space()                                   { GEN_NS trim_space(*this); }
	forceinline String        visualize_whitespace() const                   { return GEN_NS visualize_whitespace(*this); }
	forceinline StringHeader& get_header()                                   { return GEN_NS get_header(*this); }

	bool append_fmt(char const* fmt, ...) {
		ssize res;
		char buf[GEN_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		res = str_fmt_va(buf, count_of(buf) - 1, fmt, va) - 1;
		va_end(va);

		return GEN_NS append(*this, buf, res);
	}
#pragma endregion Member Mapping
#endif
};
#endif

inline char* begin(String& str) { return str; }
inline char* end(String& str)   { return scast(char*, str) + length(str); }
inline char* next(String& str)  { return scast(char*, str) + 1; }

inline
usize string_grow_formula(usize value) {
	// Using a very aggressive growth formula to reduce time mem_copying with recursive calls to append in this library.
	return 4 * value + 8;
}

inline
String string_make(AllocatorInfo allocator, char const* str) {
	ssize length = str ? str_len(str) : 0;
	return string_make_length(allocator, str, length);
}

inline
String string_make(AllocatorInfo allocator, StrC str) {
	return string_make_length(allocator, str.Ptr, str.Len);
}

inline
String string_fmt(AllocatorInfo allocator, char* buf, ssize buf_size, char const* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	str_fmt_va(buf, buf_size, fmt, va);
	va_end(va);

	return string_make(allocator, buf);
}

inline
String string_fmt_buf(AllocatorInfo allocator, char const* fmt, ...)
{
	local_persist thread_local
	char buf[GEN_PRINTF_MAXLEN] = { 0 };

	va_list va;
	va_start(va, fmt);
	str_fmt_va(buf, GEN_PRINTF_MAXLEN, fmt, va);
	va_end(va);

	return string_make(allocator, buf);
}

inline
String string_join(AllocatorInfo allocator, char const** parts, ssize num_parts, char const* glue)
{
	String result = string_make(allocator, "");

	for (ssize idx = 0; idx < num_parts; ++idx)
	{
		append(result, parts[idx]);

		if (idx < num_parts - 1)
			append(result, glue);
	}

	return result;
}

inline
bool append(String& str, char c) {
	return append(str, &c, 1);
}

inline
bool append(String& str, char const* str_to_append) {
	return append(str, str_to_append, str_len(str_to_append));
}

inline
bool append(String& str, char const* str_to_append, ssize append_length)
{
	if (sptr(str_to_append) > 0)
	{
		ssize curr_len = length(str);

		if (!make_space_for(str, str_to_append, append_length))
			return false;

		StringHeader& header = get_header(str);

		mem_copy( scast(char*, str) + curr_len, str_to_append, append_length);

		str[curr_len + append_length] = '\0';

		header.Length = curr_len + append_length;
	}
	return str_to_append != nullptr;
}

inline
bool append(String& str, StrC str_to_append) {
	return append(str, str_to_append.Ptr, str_to_append.Len);
}

inline
bool append(String& str, const String other) {
	return append(str, other, length(other));
}

bool append_fmt(String& str, char const* fmt, ...) {
	ssize res;
	char buf[GEN_PRINTF_MAXLEN] = { 0 };

	va_list va;
	va_start(va, fmt);
	res = str_fmt_va(buf, count_of(buf) - 1, fmt, va) - 1;
	va_end(va);

	return append(str, buf, res);
}

inline
bool are_equal(String lhs, String rhs)
{
	if (length(lhs) != length(rhs))
		return false;

	for (ssize idx = 0; idx < length(lhs); ++idx)
		if (lhs[idx] != rhs[idx])
			return false;

	return true;
}

inline
bool are_equal(String lhs, StrC rhs)
{
	if (length(lhs) != (rhs.Len))
		return false;

	for (ssize idx = 0; idx < length(lhs); ++idx)
		if (lhs[idx] != rhs[idx])
			return false;

	return true;
}

inline
ssize avail_space(String const& str) {
	StringHeader const& header = *rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));
	return header.Capacity - header.Length;
}

inline
char& back(String& str) {
	return str.Data[length(str) - 1];
}

inline
bool contains(String const& str, StrC substring)
{
	StringHeader const& header = *rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));

	if (substring.Len > header.Length)
		return false;

	ssize main_len = header.Length;
	ssize sub_len = substring.Len;

	for (ssize idx = 0; idx <= main_len - sub_len; ++idx)
	{
		if (str_compare(str.Data + idx, substring.Ptr, sub_len) == 0)
			return true;
	}

	return false;
}

inline
bool contains(String const& str, String const& substring)
{
	StringHeader const& header = *rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));

	if (length(substring) > header.Length)
		return false;

	ssize main_len = header.Length;
	ssize sub_len = length(substring);

	for (ssize idx = 0; idx <= main_len - sub_len; ++idx)
	{
		if (str_compare(str.Data + idx, substring.Data, sub_len) == 0)
			return true;
	}

	return false;
}

inline
ssize capacity(String const& str) {
   StringHeader const& header = *rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));
   return header.Capacity;
}

inline
void clear(String& str) {
   get_header(str).Length = 0;
}

inline
String duplicate(String const& str, AllocatorInfo allocator) {
   return string_make_length(allocator, str, length(str));
}

inline
void free(String& str) {
   if (!str.Data)
   	return;

   StringHeader& header = get_header(str);
   GEN_NS free(header.Allocator, &header);
}

inline
StringHeader& get_header(String& str) {
   return *(StringHeader*)(str.Data - sizeof(StringHeader));
}

inline
ssize length(String const& str)
{
   StringHeader const& header = *rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));
   return header.Length;
}

inline
bool make_space_for(String& str, char const* to_append, ssize add_len)
{
	ssize available = avail_space(str);

	if (available >= add_len) {
		return true;
	}
	else
	{
		ssize new_len, old_size, new_size;
		void* ptr;
		void* new_ptr;

		AllocatorInfo allocator = get_header(str).Allocator;
		StringHeader* header = nullptr;

		new_len = string_grow_formula(length(str) + add_len);
		ptr = &get_header(str);
		old_size = size_of(StringHeader) + length(str) + 1;
		new_size = size_of(StringHeader) + new_len + 1;

		new_ptr = resize(allocator, ptr, old_size, new_size);

		if (new_ptr == nullptr)
			return false;

		header = rcast(StringHeader*, new_ptr);
		header->Allocator = allocator;
		header->Capacity = new_len;

		str.Data = rcast(char*, header + 1);

		return true;
	}
}

inline
b32 starts_with(String const& str, StrC substring) {
	if (substring.Len > length(str))
	return false;

	b32 result = str_compare(str.Data, substring.Ptr, substring.Len) == 0;
	return result;
}

inline
b32 starts_with(String const& str, String substring) {
	if (length(substring) > length(str))
		return false;

	b32 result = str_compare(str.Data, substring.Data, length(substring) - 1) == 0;
	return result;
}

inline
void skip_line(String& str)
{
#define current (*scanner)
	char* scanner = str.Data;
	while (current != '\r' && current != '\n') {
 		++scanner;
	}

	s32 new_length = scanner - str.Data;

	if (current == '\r') {
		new_length += 1;
	}

	mem_move(str.Data, scanner, new_length);

	StringHeader* header = &get_header(str);
	header->Length = new_length;
#undef current
}

inline
void strip_space(String& str)
{
   char* write_pos = str.Data;
   char* read_pos = str.Data;

   while (*read_pos)
   {
   	if (!char_is_space(*read_pos))
   	{
   		*write_pos = *read_pos;
   		write_pos++;
   	}
   	read_pos++;
   }

   write_pos[0] = '\0';  // Null-terminate the modified string

   // Update the length if needed
   get_header(str).Length = write_pos - str.Data;
}

inline
void trim(String& str, char const* cut_set)
{
   ssize len = 0;

   char* start_pos = str.Data;
   char* end_pos = str.Data + length(str) - 1;

   while (start_pos <= end_pos && char_first_occurence(cut_set, *start_pos))
   	start_pos++;

   while (end_pos > start_pos && char_first_occurence(cut_set, *end_pos))
   	end_pos--;

   len = scast(ssize, (start_pos > end_pos) ? 0 : ((end_pos - start_pos) + 1));

   if (str.Data != start_pos)
   	mem_move(str.Data, start_pos, len);

   str.Data[len] = '\0';

   get_header(str).Length = len;
}

inline
void trim_space(String& str) {
   trim(str, " \t\r\n\v\f");
}

inline
String visualize_whitespace(String const& str)
{
	StringHeader* header = (StringHeader*)(scast(char const*, str) - sizeof(StringHeader));
	String        result = string_make_reserve(header->Allocator, length(str) * 2); // Assume worst case for space requirements.

	for (auto c : str) switch (c)
	{
		case ' ':
			append(result, txt("·"));
		break;
		case '\t':
			append(result, txt("→"));
		break;
		case '\n':
			append(result, txt("↵"));
		break;
		case '\r':
			append(result, txt("⏎"));
		break;
		case '\v':
			append(result, txt("⇕"));
		break;
		case '\f':
			append(result, txt("⌂"));
		break;
		default:
			append(result, c);
		break;
	}

	return result;
}
#pragma endregion String

struct String_POD {
	char* Data;
};
static_assert( sizeof( String_POD ) == sizeof( String ), "String is not a POD" );

// Implements basic string interning. Data structure is based off the ZPL Hashtable.
typedef HashTable<String const>   StringTable;

// Represents strings cached with the string table.
// Should never be modified, if changed string is desired, cache_string( str ) another.
typedef String const   StringCached;

#pragma endregion Strings
