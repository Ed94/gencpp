#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "hashing.hpp"
#endif

#pragma region Strings

struct StrC;

bool        are_equal           (StrC lhs, StrC rhs);
char const* back                (StrC str);
bool        contains            (StrC str, StrC substring);
StrC        duplicate           (StrC str, AllocatorInfo allocator);
b32         starts_with         (StrC str, StrC substring);
StrC        to_str              (char const* bad_string);
StrC        visualize_whitespace(StrC str, AllocatorInfo allocator);

// Constant string with length.
struct StrC
{
	ssize       Len;
	char const* Ptr;

#if ! GEN_COMPILER_C
	operator char const* ()               const { return Ptr; }
	char const& operator[]( ssize index ) const { return Ptr[index]; }

#if GEN_SUPPORT_CPP_MEMBER_FEATURES
	bool        is_equal            (StrC rhs)                const { return GEN_NS are_equal(* this, rhs); }
	char const* back                ()                        const { return GEN_NS back(* this); }
	bool        contains            (StrC substring)          const { return GEN_NS contains(* this, substring); }
	StrC        duplicate           (AllocatorInfo allocator) const { return GEN_NS duplicate(* this, allocator); }
	b32         starts_with         (StrC substring)          const { return GEN_NS starts_with(* this, substring); }
	StrC        visualize_whitespace(AllocatorInfo allocator) const { return GEN_NS visualize_whitespace(* this, allocator); }
#endif
#endif
};

#define cast_to_strc( str ) * rcast( StrC*, (str) - sizeof(ssize) )
#define txt( text )         StrC { sizeof( text ) - 1, ( text ) }

inline char const* begin(StrC str)                   { return str.Ptr; }
inline char const* end  (StrC str)                   { return str.Ptr + str.Len; }
inline char const* next (StrC str, char const* iter) { return iter + 1; }

inline
bool are_equal(StrC lhs, StrC rhs)
{
	if (lhs.Len != rhs.Len)
		return false;

	for (ssize idx = 0; idx < lhs.Len; ++idx)
		if (lhs[idx] != rhs[idx])
			return false;

	return true;
}

inline
char const* back(StrC str) {
	return & str.Ptr[str.Len - 1];
}

inline
bool contains(StrC str, StrC substring)
{
	if (substring.Len > str.Len)
		return false;

	ssize main_len = str.Len;
	ssize sub_len  = substring.Len;
	for (ssize idx = 0; idx <= main_len - sub_len; ++idx)
	{
		if (str_compare(str.Ptr + idx, substring.Ptr, sub_len) == 0)
			return true;
	}
	return false;
}

inline
b32 starts_with(StrC str, StrC substring) {
	if (substring.Len > str.Len)
		return false;

	b32 result = str_compare(str.Ptr, substring.Ptr, substring.Len) == 0;
		return result;
}

inline
StrC to_str( char const* bad_str ) {
	return { str_len( bad_str ), bad_str };
}

// Dynamic String
// This is directly based off the ZPL string api.
// They used a header pattern
// I kept it for simplicty of porting but its not necessary to keep it that way.
#pragma region String
struct StringHeader;

#if GEN_COMPILER_C || ! GEN_SUPPORT_CPP_MEMBER_FEATURES
typedef char* String;
#else
struct String;
#endif

usize string_grow_formula(usize value);

String        string_make         (AllocatorInfo allocator, char const*  str);
String        string_make         (AllocatorInfo allocator, StrC         str);
String        string_make_reserve (AllocatorInfo allocator, ssize        capacity);
String        string_make_length  (AllocatorInfo allocator, char const*  str,   ssize length);
String        string_fmt          (AllocatorInfo allocator, char*        buf,   ssize buf_size,  char const* fmt, ...);
String        string_fmt_buf      (AllocatorInfo allocator, char const*  fmt, ...);
String        string_join         (AllocatorInfo allocator, char const** parts, ssize num_parts, char const* glue);
bool          are_equal           (String const lhs, String const rhs);
bool          are_equal           (String const lhs, StrC rhs);
bool          make_space_for      (String*      str, char const*  to_append, ssize add_len);
bool          append              (String*      str, char         c);
bool          append              (String*      str, char const*  str_to_append);
bool          append              (String*      str, char const*  str_to_append, ssize length);
bool          append              (String*      str, StrC         str_to_append);
bool          append              (String*      str, String const other);
bool          append_fmt          (String*      str, char const*  fmt, ...);
ssize         avail_space         (String const str);
char*         back                (String       str);
bool          contains            (String const str, StrC         substring);
bool          contains            (String const str, String const substring);
ssize         capacity            (String const str);
void          clear               (String       str);
String        duplicate           (String const str, AllocatorInfo allocator);
void          free                (String*      str);
StringHeader* get_header          (String       str);
ssize         length              (String const str);
b32           starts_with         (String const str, StrC   substring);
b32           starts_with         (String const str, String substring);
void          skip_line           (String       str);
void          strip_space         (String       str);
StrC          to_strc             (String       str);
void          trim                (String       str, char const* cut_set);
void          trim_space          (String       str);
String        visualize_whitespace(String const str);

struct StringHeader {
	AllocatorInfo Allocator;
	ssize         Capacity;
	ssize         Length;
};

#if ! GEN_COMPILER_C && GEN_SUPPORT_CPP_MEMBER_FEATURES
struct String
{
	char* Data;

	forceinline operator char*()             { return Data; }
	forceinline operator char const*() const { return Data; }
	forceinline operator StrC()        const { return { GEN_NS length(* this), Data }; }

	String const& operator=(String const& other) const {
		if (this == &other)
			return *this;

		String* this_ = ccast(String*, this);
		this_->Data = other.Data;

		return *this;
	}

	forceinline char&       operator[](ssize index)       { return Data[index]; }
	forceinline char const& operator[](ssize index) const { return Data[index]; }

	       bool operator==(std::nullptr_t) const             { return     Data == nullptr; }
	       bool operator!=(std::nullptr_t) const             { return     Data != nullptr; }
	friend bool operator==(std::nullptr_t, const String str) { return str.Data == nullptr; }
	friend bool operator!=(std::nullptr_t, const String str) { return str.Data != nullptr; }

	forceinline char* begin() const { return Data; }
	forceinline char* end()   const { return Data + GEN_NS length(* this); }

#pragma region Member Mapping
	forceinline static String make(AllocatorInfo allocator, char const* str)                { return GEN_NS string_make(allocator, str); }
	forceinline static String make(AllocatorInfo allocator, StrC str)                       { return GEN_NS string_make(allocator, str); }
	forceinline static String make_reserve(AllocatorInfo allocator, ssize cap)              { return GEN_NS string_make_reserve(allocator, cap); }
	forceinline static String make_length(AllocatorInfo a, char const* s, ssize l)          { return GEN_NS string_make_length(a, s, l); }
	forceinline static String join(AllocatorInfo a, char const** p, ssize n, char const* g) { return GEN_NS string_join(a, p, n, g); }
	forceinline static usize  grow_formula(usize value)                                     { return GEN_NS string_grow_formula(value); }

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

	forceinline bool          make_space_for(char const* str, ssize add_len) { return GEN_NS make_space_for(this, str, add_len); }
	forceinline bool          append(char c)                                 { return GEN_NS append(this, c); }
	forceinline bool          append(char const* str)                        { return GEN_NS append(this, str); }
	forceinline bool          append(char const* str, ssize length)          { return GEN_NS append(this, str, length); }
	forceinline bool          append(StrC str)                               { return GEN_NS append(this, str); }
	forceinline bool          append(const String other)                     { return GEN_NS append(this, other); }
	forceinline ssize         avail_space() const                            { return GEN_NS avail_space(* this); }
	forceinline char*         back()                                         { return GEN_NS back(* this); }
	forceinline bool          contains(StrC substring) const                 { return GEN_NS contains(* this, substring); }
	forceinline bool          contains(String const& substring) const        { return GEN_NS contains(* this, substring); }
	forceinline ssize         capacity() const                               { return GEN_NS capacity(* this); }
	forceinline void          clear()                                        { GEN_NS clear(* this); }
	forceinline String        duplicate(AllocatorInfo allocator) const       { return GEN_NS duplicate(* this, allocator); }
	forceinline void          free()                                         { GEN_NS free(this); }
	forceinline bool          is_equal(String const& other) const            { return GEN_NS are_equal(* this, other); }
	forceinline bool          is_equal(StrC other) const                     { return GEN_NS are_equal(* this, other); }
	forceinline ssize         length() const                                 { return GEN_NS length(* this); }
	forceinline b32           starts_with(StrC substring) const              { return GEN_NS starts_with(* this, substring); }
	forceinline b32           starts_with(String substring) const            { return GEN_NS starts_with(* this, substring); }
	forceinline void          skip_line()                                    { GEN_NS skip_line(* this); }
	forceinline void          strip_space()                                  { GEN_NS strip_space(* this); }
	forceinline StrC          to_strc()                                      { return { length(), Data}; }
	forceinline void          trim(char const* cut_set)                      { GEN_NS trim(* this, cut_set); }
	forceinline void          trim_space()                                   { GEN_NS trim_space(* this); }
	forceinline String        visualize_whitespace() const                   { return GEN_NS visualize_whitespace(* this); }
	forceinline StringHeader& get_header()                                   { return * GEN_NS get_header(* this); }

	bool append_fmt(char const* fmt, ...) {
		ssize res;
		char buf[GEN_PRINTF_MAXLEN] = { 0 };

		va_list va;
		va_start(va, fmt);
		res = str_fmt_va(buf, count_of(buf) - 1, fmt, va) - 1;
		va_end(va);

		return GEN_NS append(this, buf, res);
	}
#pragma endregion Member Mapping
};
#endif

inline char* begin(String str)             { return ((char*) str); }
inline char* end  (String str)             { return ((char*) str + length(str)); }
inline char* next (String str, char* iter) { return ((char*) iter + 1); }

#if GEN_SUPPORT_CPP_REFERENCES
inline bool          make_space_for(String& str, char const* to_append, ssize add_len);
inline bool          append(String& str, char c);
inline bool          append(String& str, char const* str_to_append);
inline bool          append(String& str, char const* str_to_append, ssize length);
inline bool          append(String& str, StrC str_to_append);
inline bool          append(String& str, const String other);
inline bool          append_fmt(String& str, char const* fmt, ...);
inline char&         back(String& str);
inline void          clear(String& str);
inline void          free(String& str);
#endif

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
		append(& result, parts[idx]);

		if (idx < num_parts - 1)
			append(& result, glue);
	}

	return result;
}

inline
bool append(String* str, char c) {
	GEN_ASSERT(str != nullptr);
	return append( str, (char const*)& c, (ssize)1);
}

inline
bool append(String* str, char const* str_to_append) {
	GEN_ASSERT(str != nullptr);
	return append(str, str_to_append, str_len(str_to_append));
}

inline
bool append(String* str, char const* str_to_append, ssize append_length)
{
	GEN_ASSERT(str != nullptr);
	if (sptr(str_to_append) > 0)
	{
		ssize curr_len = length(* str);

		if ( ! make_space_for(str, str_to_append, append_length))
			return false;

		StringHeader* header = get_header(* str);

		char* Data = * str;
		mem_copy( Data + curr_len, str_to_append, append_length);

		Data[curr_len + append_length] = '\0';

		header->Length = curr_len + append_length;
	}
	return str_to_append != nullptr;
}

inline
bool append(String* str, StrC str_to_append) {
	GEN_ASSERT(str != nullptr);
	return append(str, str_to_append.Ptr, str_to_append.Len);
}

inline
bool append(String* str, String const other) {
	GEN_ASSERT(str != nullptr);
	return append(str, (char const*)other, length(other));
}

bool append_fmt(String* str, char const* fmt, ...) {
	GEN_ASSERT(str != nullptr);
	ssize res;
	char buf[GEN_PRINTF_MAXLEN] = { 0 };

	va_list va;
	va_start(va, fmt);
	res = str_fmt_va(buf, count_of(buf) - 1, fmt, va) - 1;
	va_end(va);

	return append(str, (char const*)buf, res);
}

inline
bool are_equal(String const lhs, String const rhs)
{
	if (length(lhs) != length(rhs))
		return false;

	for (ssize idx = 0; idx < length(lhs); ++idx)
		if (lhs[idx] != rhs[idx])
			return false;

	return true;
}

inline
bool are_equal(String const lhs, StrC rhs)
{
	if (length(lhs) != (rhs.Len))
		return false;

	for (ssize idx = 0; idx < length(lhs); ++idx)
		if (lhs[idx] != rhs.Ptr[idx])
			return false;

	return true;
}

inline
ssize avail_space(String const str) {
	StringHeader const* header = rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));
	return header->Capacity - header->Length;
}

inline
char* back(String* str) {
	return & (*str)[length(* str) - 1];
}

inline
bool contains(String const str, StrC substring)
{
	StringHeader const* header = rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));

	if (substring.Len > header->Length)
		return false;

	ssize main_len = header->Length;
	ssize sub_len  = substring.Len;

	for (ssize idx = 0; idx <= main_len - sub_len; ++idx)
	{
		if (str_compare(str + idx, substring.Ptr, sub_len) == 0)
			return true;
	}

	return false;
}

inline
bool contains(String const str, String const substring)
{
	StringHeader const* header = rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));

	if (length(substring) > header->Length)
		return false;

	ssize main_len = header->Length;
	ssize sub_len  = length(substring);

	for (ssize idx = 0; idx <= main_len - sub_len; ++idx)
	{
		if (str_compare(str + idx, substring, sub_len) == 0)
			return true;
	}

	return false;
}

inline
ssize capacity(String const str) {
   StringHeader const* header = rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));
   return header->Capacity;
}

inline
void clear(String str) {
   get_header(str)->Length = 0;
}

inline
String duplicate(String const str, AllocatorInfo allocator) {
   return string_make_length(allocator, str, length(str));
}

inline
void free(String* str) {
	GEN_ASSERT(str != nullptr);
	if (! (* str))
		return;

	StringHeader* header = get_header(* str);
	GEN_NS free(header->Allocator, header);
}

inline
StringHeader* get_header(String str) {
   return (StringHeader*)(scast(char*, str) - sizeof(StringHeader));
}

inline
ssize length(String const str)
{
   StringHeader const& header = *rcast(StringHeader const*, scast(char const*, str) - sizeof(StringHeader));
   return header.Length;
}

inline
bool make_space_for(String* str, char const* to_append, ssize add_len)
{
	ssize available = avail_space(* str);

	if (available >= add_len) {
		return true;
	}
	else
	{
		ssize new_len, old_size, new_size;
		void* ptr;
		void* new_ptr;

		AllocatorInfo allocator = get_header(* str)->Allocator;
		StringHeader* header    = nullptr;

		new_len  = string_grow_formula(length(* str) + add_len);
		ptr      = get_header(* str);
		old_size = size_of(StringHeader) + length(* str) + 1;
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

inline
b32 starts_with(String const str, StrC substring) {
	if (substring.Len > length(str))
	return false;

	b32 result = str_compare(str, substring.Ptr, substring.Len) == 0;
	return result;
}

inline
b32 starts_with(String const str, String substring) {
	if (length(substring) > length(str))
		return false;

	b32 result = str_compare(str, substring, length(substring) - 1) == 0;
	return result;
}

inline
void skip_line(String str)
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

	StringHeader* header = get_header(str);
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
   get_header(str)->Length = write_pos - str;
}

inline
StrC to_strc(String str) {
	return { length(str), (char const*)str };
}

inline
void trim(String str, char const* cut_set)
{
	ssize len = 0;

	char* start_pos = str;
	char* end_pos   = scast(char*, str) + length(str) - 1;

	while (start_pos <= end_pos && char_first_occurence(cut_set, *start_pos))
	start_pos++;

	while (end_pos > start_pos && char_first_occurence(cut_set, *end_pos))
	end_pos--;

	len = scast(ssize, (start_pos > end_pos) ? 0 : ((end_pos - start_pos) + 1));

	if (str != start_pos)
		mem_move(str, start_pos, len);

	str[len] = '\0';

   get_header(str)->Length = len;
}

inline
void trim_space(String str) {
   trim(str, " \t\r\n\v\f");
}

inline
String visualize_whitespace(String const str)
{
	StringHeader* header = (StringHeader*)(scast(char const*, str) - sizeof(StringHeader));
	String        result = string_make_reserve(header->Allocator, length(str) * 2); // Assume worst case for space requirements.

	foreach (char*, c, str) switch ( * c )
	{
		case ' ':
			append(& result, txt("·"));
		break;
		case '\t':
			append(& result, txt("→"));
		break;
		case '\n':
			append(& result, txt("↵"));
		break;
		case '\r':
			append(& result, txt("⏎"));
		break;
		case '\v':
			append(& result, txt("⇕"));
		break;
		case '\f':
			append(& result, txt("⌂"));
		break;
		default:
			append(& result, c);
		break;
	}

	return result;
}
#pragma endregion String

struct String_POD {
	char* Data;
};
static_assert( sizeof( String_POD ) == sizeof( String ), "String is not a POD" );

inline
StrC duplicate(StrC str, AllocatorInfo allocator) {
	String result = string_make_length(allocator, str.Ptr, str.Len);
	return {  get_header(result)->Length, result };
}

inline
StrC visualize_whitespace(StrC str, AllocatorInfo allocator)
{
	String result = string_make_reserve(allocator, str.Len * 2); // Assume worst case for space requirements.
	foreach (char const*, c, str) switch ( * c )
	{
		case ' ':
			append(& result, txt("·"));
		break;
		case '\t':
			append(& result, txt("→"));
		break;
		case '\n':
			append(& result, txt("↵"));
		break;
		case '\r':
			append(& result, txt("⏎"));
		break;
		case '\v':
			append(& result, txt("⇕"));
		break;
		case '\f':
			append(& result, txt("⌂"));
		break;
		default:
			append(& result, c);
		break;
	}
	return to_strc(result);
}

// Represents strings cached with the string table.
// Should never be modified, if changed string is desired, cache_string( str ) another.
typedef StrC StringCached;

// Implements basic string interning. Data structure is based off the ZPL Hashtable.
typedef HashTable<StringCached> StringTable;
#pragma endregion Strings
