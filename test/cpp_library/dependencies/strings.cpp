internal gen::Str bounded_format(char* storage, gen::ssize storage_len, char const* bytes, gen::ssize len)
{
	gen::mem_set(storage, '!', storage_len);
	if (len > 0)
		gen::mem_copy(storage, bytes, len);
	storage[storage_len - 1] = '\0';
	gen::Str result = { storage, len };
	return result;
}

internal void expect_template_expansion(gen::Str label, gen::Str format, gen::StrTemplateSlot* slots, gen::ssize slot_count, gen::Str expansion)
{
	using namespace gen;
	StrBuilder output = strbuilder_make_reserve(heap(), 1);
	expect(strbuilder_append_str(& output, txt("KEEP")), label);
	Slice_StrTemplateSlot slice = { nullptr, 0 };
	if (slot_count != 0) {
		slice.Ptr = slots;
		slice.Len = slot_count;
	}
	bool appended = strbuilder_append_template(& output, slice, format);
	expect(appended, label);
	Str got = strbuilder_to_str(output);
	expect(got.Len == 4 + expansion.Len, label);
	expect(str_starts_with(got, txt("KEEP")), label);
	if (got.Len >= 4 + expansion.Len) {
		Str tail = { got.Ptr + 4, expansion.Len };
		expect(str_are_equal(tail, expansion), label);
	}
	StrBuilderHeader* header = strbuilder_get_header(output);
	expect(header->Allocator.Proc == heap_allocator_proc, label);
	strbuilder_free(& output);
}

internal void expect_template_text(gen::Str label, gen::Str format, gen::StrTemplateSlot* slots, gen::ssize slot_count, gen::Str expansion)
{
	expect_template_expansion(label, format, slots, slot_count, expansion);
}

internal void expect_literal_boundary(gen::ssize mark) {
	using namespace gen;
	char format_storage[64];
	char format_bytes[40];
	for (ssize idx = 0; idx < mark; ++ idx)
		format_bytes[idx] = 'L';
	Str format = bounded_format(format_storage, 64, format_bytes, mark);
	Str expansion = { format_bytes, mark };
	expect(format_storage[mark] == '!', txt("literal boundary"));
	expect_template_expansion(txt("literal boundary"), format, nullptr, 0, expansion);
}

internal void expect_open_boundary(gen::ssize mark)
{
	using namespace gen;
	char format_storage[64];
	char format_bytes[48];
	char expansion_bytes[48];
	ssize format_len = 0;
	for (ssize idx = 0; idx < mark; ++ idx)
		format_bytes[format_len ++] = 'L';
	format_bytes[format_len ++] = '<';
	format_bytes[format_len ++] = 'k';
	format_bytes[format_len ++] = '>';
	format_bytes[format_len ++] = 'Z';
	Str format = bounded_format(format_storage, 64, format_bytes, format_len);
	StrTemplateSlot slot = {};
	slot.Key   = txt("k");
	slot.Value = txt("V");
	ssize expansion_len = 0;
	for (ssize idx = 0; idx < mark; ++ idx)
		expansion_bytes[expansion_len ++] = 'L';
	expansion_bytes[expansion_len ++] = 'V';
	expansion_bytes[expansion_len ++] = 'Z';
	Str expansion = { expansion_bytes, expansion_len };
	expect(format.Ptr[mark] == '<', txt("open boundary"));
	expect(format_storage[format_len] == '!', txt("open boundary"));
	expect_template_expansion(txt("open boundary"), format, & slot, 1, expansion);
}

internal void expect_close_boundary(gen::ssize mark) {
	using namespace gen;
	char format_storage[64];
	char format_bytes[48];
	char key_storage[40];
	ssize key_len = mark - 1;
	mem_set(key_storage, '!', 40);
	for (ssize idx = 0; idx < key_len; ++ idx)
		key_storage[idx] = 'k';
	key_storage[39] = '\0';
	Str key = { key_storage, key_len };
	ssize format_len = 0;
	format_bytes[format_len ++] = '<';
	for (ssize idx = 0; idx < key_len; ++ idx)
		format_bytes[format_len ++] = 'k';
	format_bytes[format_len ++] = '>';
	format_bytes[format_len ++] = 'Z';
	Str format = bounded_format(format_storage, 64, format_bytes, format_len);
	StrTemplateSlot slot = {};
	slot.Key   = key;
	slot.Value = txt("V");
	expect(format.Ptr[mark] == '>', txt("close boundary"));
	expect(key.Ptr[key.Len] == '!', txt("close boundary"));
	expect(format_storage[format_len] == '!', txt("close boundary"));
	expect_template_expansion(txt("close boundary"), format, & slot, 1, txt("VZ"));
}

internal void expect_template_shapes()
{
	using namespace gen;
	char literal_storage[8];
	char literal_bytes[6] = { 'a', 'b', 'c', 'D', 'E', 'F' };
	Str literal = bounded_format(literal_storage, 8, literal_bytes, 3);
	expect(literal_storage[3] == '!', txt("literal sentinel"));
	expect_template_expansion(txt("literal bounded"), literal, nullptr, 0, txt("abc"));

	char empty_storage[4];
	Str empty_format = bounded_format(empty_storage, 4, empty_storage, 0);
	expect(empty_storage[0] == '!', txt("empty sentinel"));
	expect_template_expansion(txt("empty format"), empty_format, nullptr, 0, txt(""));

	StrTemplateSlot name_slot = {};
	name_slot.Key   = txt("name");
	name_slot.Value = txt("world");
	expect_template_text(txt("one slot"), txt("hello <name>!"), & name_slot, 1, txt("hello world!"));

	StrTemplateSlot repeat_slot = {};
	repeat_slot.Key   = txt("w");
	repeat_slot.Value = txt("v");
	expect_template_text(txt("repeated slot"), txt("<w>-<w>-<w>"), & repeat_slot, 1, txt("v-v-v"));

	StrTemplateSlot adjacent[2] = {};
	adjacent[0].Key   = txt("a");
	adjacent[0].Value = txt("X");
	adjacent[1].Key   = txt("b");
	adjacent[1].Value = txt("Y");
	expect_template_text(txt("adjacent slots"), txt("<a><b><a>"), adjacent, 2, txt("XYX"));

	StrTemplateSlot raw_slots[3] = {};
	raw_slots[0].Key   = txt("raw");
	raw_slots[0].Value = txt("a<b>c");
	raw_slots[1].Key   = txt("angles");
	raw_slots[1].Value = txt("<>");
	raw_slots[2].Key   = txt("b");
	raw_slots[2].Value = txt("SHOULD_NOT");
	expect_template_text(txt("value angles literal"), txt("<raw><angles>"), raw_slots, 3, txt("a<b>c<>"));

	char empty_value_storage[2] = { '!', '!' };
	StrTemplateSlot empty_slot = {};
	empty_slot.Key   = txt("e");
	empty_slot.Value = { empty_value_storage, 0 };
	expect_template_text(txt("empty value"), txt("A<e>B"), & empty_slot, 1, txt("AB"));

	StrTemplateSlot empty_key = {};
	empty_key.Key   = { nullptr, 0 };
	empty_key.Value = txt("Z");
	expect_template_text(txt("empty key"), txt("<>"), & empty_key, 1, txt("Z"));

	StrTemplateSlot null_value = {};
	null_value.Key   = txt("e");
	null_value.Value = { nullptr, 0 };
	expect(null_value.Value.Ptr == nullptr, txt("null empty value pointer"));
	expect(null_value.Value.Len == 0, txt("null empty value length"));
	expect_template_text(txt("null empty value"), txt("A<e>B"), & null_value, 1, txt("AB"));

	char nul_value_bytes[4] = { 'a', '\0', 'b', '!' };
	char nul_expected[5]    = { '[', 'a', '\0', 'b', ']' };
	StrTemplateSlot nul_slot = {};
	nul_slot.Key   = txt("n");
	nul_slot.Value = { nul_value_bytes, 3 };
	gen::Str nul_expansion = { nul_expected, 5 };
	expect_template_text(txt("embedded nul"), txt("[<n>]"), & nul_slot, 1, nul_expansion);

	StrTemplateSlot duffle[2] = {};
	duffle[0].Key   = txt("operands[0]");
	duffle[0].Value = txt("eax");
	duffle[1].Key   = txt("mod_rm.bits");
	duffle[1].Value = txt("11");
	expect_template_text(txt("duffle keys"), txt("mov <operands[0]>, <mod_rm.bits>;"), duffle, 2, txt("mov eax, 11;"));
}

internal void fill_lookup_slots(gen::StrTemplateSlot* slots, gen::ssize slot_count) {
	gen::Str keys[9] = {
		txt("k0"), txt("k1"), txt("k2"), txt("k3"), txt("k4"),
		txt("k5"), txt("k6"), txt("k7"), txt("k8")
	};
	gen::Str values[9] = {
		txt("V0"), txt("V1"), txt("V2"), txt("V3"), txt("V4"),
		txt("V5"), txt("V6"), txt("V7"), txt("V8")
	};
	for (gen::ssize idx = 0; idx < slot_count; ++ idx) {
		slots[idx].Key   = keys[idx];
		slots[idx].Value = values[idx];
	}
}

internal void expect_template_lookup()
{
	using namespace gen;
	StrTemplateSlot slots[9] = {};
	expect_template_text(txt("lookup 0"), txt("none"), nullptr, 0, txt("none"));

	fill_lookup_slots(slots, 1);
	expect_template_text(txt("lookup 1"), txt("<k0>"), slots, 1, txt("V0"));

	fill_lookup_slots(slots, 2);
	expect_template_text(txt("lookup 2"), txt("<k1><k0>"), slots, 2, txt("V1V0"));

	fill_lookup_slots(slots, 3);
	expect_template_text(txt("lookup 3"), txt("<k2>-<k0>-<k1>"), slots, 3, txt("V2-V0-V1"));

	fill_lookup_slots(slots, 4);
	expect_template_text(txt("lookup 4"), txt("<k3><k0><k2><k1>"), slots, 4, txt("V3V0V2V1"));

	fill_lookup_slots(slots, 5);
	expect_template_text(txt("lookup 5 tail"), txt("<k4><k0><k3>"), slots, 5, txt("V4V0V3"));

	fill_lookup_slots(slots, 8);
	expect_template_text(txt("lookup 8 lanes"), txt("<k7><k4><k3><k0>"), slots, 8, txt("V7V4V3V0"));

	fill_lookup_slots(slots, 9);
	expect_template_text(txt("lookup 9 tail"), txt("<k8><k5><k6><k4>"), slots, 9, txt("V8V5V6V4"));

	StrTemplateSlot dup_small[3] = {};
	dup_small[0].Key = txt("dup");   dup_small[0].Value = txt("FIRST");
	dup_small[1].Key = txt("other"); dup_small[1].Value = txt("X");
	dup_small[2].Key = txt("dup");   dup_small[2].Value = txt("SECOND");
	expect_template_text(txt("duplicate first wins"), txt("<dup><other>"), dup_small, 3, txt("FIRSTX"));

	StrTemplateSlot dup_lanes[4] = {};
	dup_lanes[0].Key = txt("same");  dup_lanes[0].Value = txt("A");
	dup_lanes[1].Key = txt("same");  dup_lanes[1].Value = txt("B");
	dup_lanes[2].Key = txt("other"); dup_lanes[2].Value = txt("C");
	dup_lanes[3].Key = txt("same");  dup_lanes[3].Value = txt("D");
	expect_template_text(txt("duplicate lane first wins"), txt("<same><other>"), dup_lanes, 4, txt("AC"));

	StrTemplateSlot dup_tail[5] = {};
	dup_tail[0].Key = txt("dup"); dup_tail[0].Value = txt("FIRST");
	dup_tail[1].Key = txt("k1");  dup_tail[1].Value = txt("V1");
	dup_tail[2].Key = txt("k2");  dup_tail[2].Value = txt("V2");
	dup_tail[3].Key = txt("k3");  dup_tail[3].Value = txt("V3");
	dup_tail[4].Key = txt("dup"); dup_tail[4].Value = txt("TAIL");
	expect_template_text(txt("duplicate tail first wins"), txt("<dup>"), dup_tail, 5, txt("FIRST"));
}

namespace gen {
internal void expect_template_boundaries() {
	ssize marks[12] = { 7, 8, 9, 15, 16, 17, 23, 24, 25, 31, 32, 33 };
	for (ssize idx = 0; idx < count_of(marks); ++ idx) {
		expect_literal_boundary(marks[idx]);
		expect_open_boundary(marks[idx]);
		expect_close_boundary(marks[idx]);
	}
}
}

internal void expect_template_growth(gen::ssize printf_maxlen)
{
	using namespace gen;
	ssize huge_len = printf_maxlen + 64;
	char* huge = rcast(char*, alloc(heap(), huge_len + 1));
	expect(huge != nullptr, txt("growth alloc"));
	if (huge == nullptr) return;
	mem_set(huge, 'Q', huge_len);
	huge[huge_len] = '!';

	Str             value = { huge, huge_len };
	StrTemplateSlot slot  = {};
	slot.Key   = txt("blob");
	slot.Value = value;
	Str format = txt("<blob>");
	StrBuilder output = strbuilder_make_reserve(heap(), 1);
	expect(strbuilder_capacity(output) == 1,             txt("growth initial capacity"));
	expect(strbuilder_append_str(& output, txt("KEEP")), txt("growth prefix"));
	
	Slice_StrTemplateSlot slice = { & slot, 1 };
	bool appended = strbuilder_append_template(& output, slice, format);
	expect(appended, txt("growth append"));
	
	Str got = strbuilder_to_str(output);
	expect(got.Len == 4 + huge_len, txt("growth length"));
	expect(got.Len > printf_maxlen, txt("growth beyond printf max"));
	expect(str_starts_with(got, txt("KEEP")), txt("growth keeps prefix"));
	
	ssize capacity = strbuilder_capacity(output);
	expect(capacity >= got.Len, txt("growth capacity"));

	if (got.Len == 4 + huge_len && capacity >= got.Len) {
		b32 payload_matches = true;
		for (ssize idx = 0; idx < got.Len - 4; ++ idx) {
			if (got.Ptr[4 + idx] != 'Q') payload_matches = false;
		}
		expect(payload_matches, txt("growth bytes"));
	}

	StrBuilderHeader* header = strbuilder_get_header(output);
	expect(header->Allocator.Proc == heap_allocator_proc, txt("growth allocator"));
	expect(huge[huge_len]         == '!',                 txt("growth sentinel"));
	strbuilder_free(& output);
	allocator_free(heap(), huge);
}

internal void expect_template_crc_collision()
{
	using namespace gen;
	Str plumless  = txt("plumless");
	Str buckeroo  = txt("buckeroo");
	u32 plum_hash = crc32(plumless.Ptr, plumless.Len);
	u32 buck_hash = crc32(buckeroo.Ptr, buckeroo.Len);
	expect(plum_hash == buck_hash,  txt("crc collision equal"));
	expect(plum_hash == 0x4ddb0c25, txt("crc collision value"));
	expect(str_are_equal(plumless, buckeroo) == false, txt("crc collision keys differ"));

	StrTemplateSlot scalar[2] = {};
	scalar[0].Key = plumless; scalar[0].Value = txt("WRONG");
	scalar[1].Key = buckeroo; scalar[1].Value = txt("RIGHT");
	expect_template_text(txt("crc scalar 2"), txt("<buckeroo>"), scalar, 2, txt("RIGHT"));

	StrTemplateSlot lanes[4] = {};
	lanes[0].Key = plumless;  lanes[0].Value = txt("WRONG");
	lanes[1].Key = txt("k1"); lanes[1].Value = txt("V1");
	lanes[2].Key = buckeroo;  lanes[2].Value = txt("RIGHT");
	lanes[3].Key = txt("k3"); lanes[3].Value = txt("V3");
	expect_template_text(txt("crc lane reject"), txt("<buckeroo>"), lanes, 4, txt("RIGHT"));

	StrTemplateSlot tail[5] = {};
	tail[0].Key = plumless;  tail[0].Value = txt("WRONG");
	tail[1].Key = txt("k1"); tail[1].Value = txt("V1");
	tail[2].Key = txt("k2"); tail[2].Value = txt("V2");
	tail[3].Key = txt("k3"); tail[3].Value = txt("V3");
	tail[4].Key = buckeroo;  tail[4].Value = txt("TAIL");
	expect_template_text(txt("crc tail after false hit"), txt("<buckeroo>"), tail, 5, txt("TAIL"));
}

internal
void* resize_fail_allocator_proc(void* allocator_data, gen::AllocType type, gen::ssize size, gen::ssize alignment
	, void* old_memory, gen::ssize old_size
	, gen::u64 flags
){
	if (type == gen::EAllocation_RESIZE) {
		gen::s32* attempts = rcast(gen::s32*, allocator_data);
		attempts[0] = attempts[0] + 1;
		return nullptr;
	}
	return gen::heap_allocator_proc(nullptr, type, size, alignment, old_memory, old_size, flags);
}

internal void expect_template_resize_fail(gen::ssize printf_maxlen)
{
	using namespace gen;
	ssize huge_len = printf_maxlen + 64;
	char* huge     = rcast(char*, alloc(heap(), huge_len + 1));
	expect(huge != nullptr, txt("resize fail alloc"));
	if (huge == nullptr) return;
	mem_set(huge, 'L', huge_len);
	huge[huge_len] = '!';
	Str huge_text = { huge, huge_len };

	s32 literal_attempts = 0;
	AllocatorInfo literal_alloc = {};
	literal_alloc.Proc = resize_fail_allocator_proc;
	literal_alloc.Data = & literal_attempts;
	StrBuilder literal_output = strbuilder_make_reserve(literal_alloc, 4);
	expect(strbuilder_append_str(& literal_output, txt("KEEP")), txt("resize fail literal prefix"));

	Slice_StrTemplateSlot literal_slots = { nullptr, 0 };
	bool literal_appended = strbuilder_append_template(& literal_output, literal_slots, huge_text);
	expect(literal_appended == false, txt("resize fail literal"));

	Str literal_got = strbuilder_to_str(literal_output);
	expect(literal_got.Len == 4, txt("resize fail literal length"));
	expect(str_are_equal(literal_got, txt("KEEP")), txt("resize fail literal keep"));
	expect(literal_attempts > 0, txt("resize fail literal attempts"));
	strbuilder_free(& literal_output);

	s32 value_attempts = 0;
	AllocatorInfo value_alloc = {};
	value_alloc.Proc = resize_fail_allocator_proc;
	value_alloc.Data = & value_attempts;

	StrTemplateSlot slot = {};
	slot.Key   = txt("blob");
	slot.Value = huge_text;
	Str format = txt("<blob>");
	StrBuilder value_output = strbuilder_make_reserve(value_alloc, 4);
	expect(strbuilder_append_str(& value_output, txt("KEEP")), txt("resize fail value prefix"));
	
	Slice_StrTemplateSlot value_slots = { & slot, 1 };
	bool   value_appended = strbuilder_append_template(& value_output, value_slots, format);
	expect(value_appended == false, txt("resize fail value"));

	Str value_got = strbuilder_to_str(value_output);
	expect(value_got.Len == 4, txt("resize fail value length"));
	expect(str_are_equal(value_got, txt("KEEP")), txt("resize fail value keep"));
	expect(value_attempts > 0, txt("resize fail value attempts"));
	strbuilder_free(& value_output);
	allocator_free(heap(), huge);
}

internal gen::Str         counted_template_value (gen::s32* reads)                          { reads[0] = reads[0] + 1; return txt("ONCE"); }
internal gen::Str         counted_template_format(gen::s32* reads)                          { reads[0] = reads[0] + 1; return txt("<k>:<k>"); }
internal gen::StrBuilder* counted_template_output(gen::StrBuilder* output, gen::s32* reads) { reads[0] = reads[0] + 1; return output; }
internal bool*            counted_template_result(bool* result, gen::s32* reads)            { reads[0] = reads[0] + 1; return result; }

internal void expect_template_slice() {
	using namespace gen;
	StrTemplateSlot slots[2] = {
		strtmpl_slot("a", txt("X")),
		strtmpl_slot("b", txt("Y"))
	};
	Slice_StrTemplateSlot slice  = { slots, 2 };
	StrBuilder            output = strbuilder_make_reserve(heap(), 1);
	expect(strbuilder_append_str(& output, txt("KEEP")), txt("slice prefix"));

	bool   appended = strbuilder_append_template(& output, slice, txt("<b><a><b>"));
	expect(appended, txt("slice append"));
	expect(str_are_equal(strbuilder_to_str(output), txt("KEEPYXY")), txt("slice text"));
	expect(str_are_equal(slots[0].Key,              txt("a")),       txt("slice key"));
	expect(str_are_equal(slots[0].Value,            txt("X")),       txt("slice value"));
	strbuilder_free(& output);

	slice.Ptr[0].Value = txt("Z");
	StrBuilder mutated_output   = strbuilder_make_reserve(heap(), 1);
	bool       mutated_appended = strbuilder_append_template(& mutated_output, slice, txt("<b><a><b>"));
	expect(mutated_appended, txt("slice mutate append"));
	expect(str_are_equal(strbuilder_to_str(mutated_output), txt("YZY")), txt("slice mutate text"));
	strbuilder_free(& mutated_output);

	Slice_StrTemplateSlot empty_slice  = { nullptr, 0 };
	StrBuilder            empty_output = strbuilder_make_reserve(heap(), 1);
	expect(strbuilder_append_str(& empty_output, txt("KEEP")), txt("zero slice prefix"));
	bool   empty_appended = strbuilder_append_template(& empty_output, empty_slice, txt("none"));
	expect(empty_appended, txt("zero slice append"));
	expect(str_are_equal(strbuilder_to_str(empty_output), txt("KEEPnone")), txt("zero slice text"));
	expect(empty_slice.Ptr == nullptr, txt("zero slice pointer"));
	expect(empty_slice.Len == 0, txt("zero slice length"));
	strbuilder_free(& empty_output);
}

internal void expect_template_wrapper() {
	using namespace gen;
	StrBuilder output = strbuilder_make_reserve(heap(), 1);
	expect(strbuilder_append_str(& output, txt("KEEP")), txt("wrapper prefix"));

	bool appended = false;
	strtmpl_append(& output, appended,
		strtmpl_slots(
			strtmpl_slot("name", txt("world")),
			strtmpl_slot("tail", txt("!"))
		),
		txt("hello <name>\n<name><tail>")
	);
	expect(appended, txt("wrapper multiline result"));

	Str got = strbuilder_to_str(output);
	expect(str_starts_with(got, txt("KEEP")),                    txt("wrapper prefix kept"));
	expect(str_are_equal  (got, txt("KEEPhello world\nworld!")), txt("wrapper multiline text"));
	strbuilder_free(& output);
}

internal void expect_template_wrapper_scope()
{
	using namespace gen;
	StrBuilder then_output = strbuilder_make_reserve(heap(), 1);
	StrBuilder else_output = strbuilder_make_reserve(heap(), 1);
	expect(strbuilder_append_str(& then_output, txt("KEEP")), txt("wrapper then prefix"));
	expect(strbuilder_append_str(& else_output, txt("KEEP")), txt("wrapper else prefix"));
	bool then_result  = false;
	bool else_result  = false;
	s32  selector     = 1;
	s32  after_branch = 0;
	if (selector == 1) strtmpl_append(& then_output, then_result, strtmpl_slots(strtmpl_slot("arm", txt("THEN"))), txt("<arm>"));
	else               strtmpl_append(& else_output, else_result, strtmpl_slots(strtmpl_slot("arm", txt("ELSE"))), txt("<arm>"));

	after_branch = 1;
	expect(after_branch == 1,    txt("wrapper then continues"));
	expect(then_result,          txt("wrapper then assigned"));
	expect(else_result == false, txt("wrapper else skipped"));
	expect(str_are_equal(strbuilder_to_str(then_output), txt("KEEPTHEN")), txt("wrapper then text"));
	expect(str_are_equal(strbuilder_to_str(else_output), txt("KEEP")),     txt("wrapper else untouched"));
	strbuilder_free(& then_output);
	strbuilder_free(& else_output);

	then_output = strbuilder_make_reserve(heap(), 1);
	else_output = strbuilder_make_reserve(heap(), 1);
	expect(strbuilder_append_str(& then_output, txt("KEEP")), txt("wrapper skipped then prefix"));
	expect(strbuilder_append_str(& else_output, txt("KEEP")), txt("wrapper taken else prefix"));

	then_result  = false;
	else_result  = false;
	selector     = 0;
	after_branch = 0;
	if (selector == 1) strtmpl_append(& then_output, then_result, strtmpl_slots(strtmpl_slot("arm", txt("THEN"))), txt("<arm>"));
	else               strtmpl_append(& else_output, else_result, strtmpl_slots(strtmpl_slot("arm", txt("ELSE"))), txt("<arm>"));

	after_branch = 1;
	expect(after_branch == 1,     txt("wrapper else continues"));
	expect(then_result  == false, txt("wrapper then skipped"));
	expect(else_result,           txt("wrapper else assigned"));
	expect(str_are_equal(strbuilder_to_str(then_output), txt("KEEP")),     txt("wrapper then untouched"));
	expect(str_are_equal(strbuilder_to_str(else_output), txt("KEEPELSE")), txt("wrapper else text"));
	strbuilder_free(& then_output);
	strbuilder_free(& else_output);
}

internal void expect_template_wrapper_once() {
	using namespace gen;
	s32 value_reads  = 0;
	s32 format_reads = 0;
	s32 output_reads = 0;
	s32 result_reads = 0;
	StrBuilder output = strbuilder_make_reserve(heap(), 1);
	expect(strbuilder_append_str(& output, txt("KEEP")), txt("wrapper once prefix"));

	bool appended = false;
	strtmpl_append(
		counted_template_output(& output,   & output_reads),
		counted_template_result(& appended, & result_reads)[0],
		strtmpl_slots(
			strtmpl_slot("k", counted_template_value(& value_reads)),
			strtmpl_slot("tail", txt("Z"))
		),
		counted_template_format(& format_reads)
	);
	expect(appended,          txt("wrapper once result"));
	expect(value_reads == 1,  txt("wrapper value once"));
	expect(format_reads == 1, txt("wrapper format once"));
	expect(output_reads == 1, txt("wrapper output once"));
	expect(result_reads == 1, txt("wrapper result once"));
	expect(str_are_equal(strbuilder_to_str(output), txt("KEEPONCE:ONCE")), txt("wrapper once text"));
	strbuilder_free(& output);
}

internal void expect_template_wrapper_resize_fail() {
	using namespace gen;
	s32 attempts = 0;
	AllocatorInfo alloc = {};
	alloc.Proc = resize_fail_allocator_proc;
	alloc.Data = & attempts;

	StrBuilder output = strbuilder_make_reserve(alloc, 4);
	expect(strbuilder_capacity(output) == 4, txt("wrapper resize capacity"));
	expect(strbuilder_append_str(& output, txt("KEEP")), txt("wrapper resize prefix"));
	bool appended = true;
	strtmpl_append(
		& output,
		appended,
		strtmpl_slots(strtmpl_slot("blob", txt("X"))),
		txt("<blob>")
	);
	expect(appended == false,                                     txt("wrapper resize assigned false"));
	expect(str_are_equal(strbuilder_to_str(output), txt("KEEP")), txt("wrapper resize keep"));
	expect(attempts > 0,                                          txt("wrapper resize attempts"));
	strbuilder_free(& output);
}

internal void expect_template_slot_literal()
{
	using namespace gen;
	Str value = txt("V");
	StrTemplateSlot slot = strtmpl_slot("k", value);
	expect(str_are_equal(slot.Key, txt("k")),       txt("slot literal key text"));
	expect(slot.Key.Ptr[slot.Key.Len] == '\0',      txt("slot literal key terminator"));
	expect(slot.Value.Ptr             == value.Ptr, txt("slot literal value pointer"));
	expect(slot.Value.Len             == value.Len, txt("slot literal value length"));

	Str assigned = txt("W");
	slot = strtmpl_slot("key", assigned);
	expect(str_are_equal(slot.Key, txt("key")), txt("slot assign key"));
	expect(slot.Value.Ptr == assigned.Ptr,      txt("slot assign value pointer"));
	expect(slot.Value.Len == assigned.Len,      txt("slot assign value length"));
	expect_template_text(txt("slot assign expansion"), txt("<key>"), & slot, 1, txt("W"));

	StrTemplateSlot row[2] = {
		strtmpl_slot("a", txt("X")),
		strtmpl_slot("b", txt("Y"))
	};
	expect(str_are_equal(row[0].Value, txt("X")), txt("slot array value"));
	expect(str_are_equal(row[1].Key,   txt("b")), txt("slot array key"));
	expect_template_text(txt("slot array entry"), txt("<b><a>"), row, 2, txt("YX"));

	StrBuilder output = strbuilder_make_reserve(heap(), 1);
	expect(strbuilder_append_str(& output, txt("KEEP")), txt("slot wrapper prefix"));
	bool appended = false;
	strtmpl_append(
		& output,
		appended,
		strtmpl_slots(
			strtmpl_slot("name", txt("world")),
			strtmpl_slot("tail", txt("!"))
		),
		txt("<name><tail>")
	);
	expect(appended, txt("slot wrapper result"));
	expect(str_are_equal(strbuilder_to_str(output), txt("KEEPworld!")), txt("slot wrapper text"));
	strbuilder_free(& output);

	StrTemplateSlot long_key = strtmpl_slot("operands[0]", txt("eax"));
	expect(str_are_equal(long_key.Key, txt("operands[0]")), txt("slot literal key equal"));
	expect(long_key.Key.Ptr[long_key.Key.Len] == '\0',      txt("slot literal key excludes nul"));
	expect(str_are_equal(long_key.Value, txt("eax")),       txt("slot literal value"));

	s32 value_reads = 0;
	StrTemplateSlot once = strtmpl_slot("k", counted_template_value(& value_reads));
	expect(value_reads == 1,                       txt("slot value once"));
	expect(str_are_equal(once.Value, txt("ONCE")), txt("slot value once text"));
	expect(once.Key.Len == 1,                      txt("slot value once key"));

	char nul_bytes[4] = { 'a', '\0', 'b', '!' };
	Str             nul_value = { nul_bytes, 3 };
	StrTemplateSlot nul_slot  = strtmpl_slot("n", nul_value);
	expect(nul_slot.Value.Ptr    == nul_bytes, txt("slot nul pointer"));
	expect(nul_slot.Value.Len    == 3,         txt("slot nul length"));
	expect(nul_slot.Value.Ptr[1] == '\0',      txt("slot nul byte"));

	char nul_expected[5] = { '[', 'a', '\0', 'b', ']' };
	Str  nul_expansion   = { nul_expected, 5 };
	expect_template_text(txt("slot nul expansion"), txt("[<n>]"), & nul_slot, 1, nul_expansion);

	Str             angle_value = txt("a<b>c");
	StrTemplateSlot angle_slot  = strtmpl_slot("raw", angle_value);
	expect(angle_slot.Value.Ptr == angle_value.Ptr, txt("slot angle pointer"));
	expect(angle_slot.Value.Len == angle_value.Len, txt("slot angle length"));
	expect_template_text(txt("slot angle expansion"), txt("<raw>"), & angle_slot, 1, txt("a<b>c"));

	Str             empty_value = { nullptr, 0 };
	StrTemplateSlot empty_slot  = strtmpl_slot("e", empty_value);
	expect(empty_slot.Value.Ptr == nullptr, txt("slot null value pointer"));
	expect(empty_slot.Value.Len == 0,       txt("slot null value length"));
	expect_template_text(txt("slot null value expansion"), txt("A<e>B"), & empty_slot, 1, txt("AB"));

	Str             percent_value = txt("%d:%S");
	StrTemplateSlot percent_slot  = strtmpl_slot("fmt", percent_value);
	expect(percent_slot.Value.Ptr == percent_value.Ptr,     txt("slot percent value pointer"));
	expect(str_are_equal(percent_slot.Value, txt("%d:%S")), txt("slot percent value text"));
	expect_template_text(txt("slot percent literal"), txt("pre %d:%S <fmt>"), & percent_slot, 1, txt("pre %d:%S %d:%S"));
}

internal void expect_text_template(gen::ssize printf_maxlen) {
	expect_template_shapes();
	expect_template_lookup();
	expect_template_crc_collision();
	gen::expect_template_boundaries();
	expect_template_growth(printf_maxlen);
	expect_template_resize_fail(printf_maxlen);
	expect_template_slice();
	expect_template_wrapper();
	expect_template_wrapper_scope();
	expect_template_wrapper_once();
	expect_template_wrapper_resize_fail();
	expect_template_slot_literal();
}
