gen_internal gen_Str bounded_format(char* storage, gen_ssize storage_len, char const* bytes, gen_ssize len) {
	gen_mem_set(storage, '!', storage_len);
	if (len > 0)
		gen_mem_copy(storage, bytes, len);
	storage[storage_len - 1] = '\0';
	gen_Str result = { storage, len };
	return result;
}

gen_internal void expect_template_expansion(gen_Str label, gen_Str format, gen_StrTemplateSlot* slots, gen_ssize slot_count, gen_Str expansion) {
	gen_StrBuilder output = gen_strbuilder_make_reserve(gen_heap(), 1);
	expect(gen_strbuilder_append_str(& output, gen_txt("KEEP")), label);
	gen_Slice_StrTemplateSlot slice = { 0, 0 };
	if (slot_count != 0) {
		slice.Ptr = slots;
		slice.Len = slot_count;
	}
	bool appended = gen_strbuilder_append_template(& output, slice, format); 
	expect(appended, label);
	gen_Str got = gen_strbuilder_to_str(output);
	expect(got.Len == 4 + expansion.Len, label);
	expect(gen_str_starts_with(got, gen_txt("KEEP")), label);
	if (got.Len >= 4 + expansion.Len) {
		gen_Str tail = { got.Ptr + 4, expansion.Len };
		expect(gen_str_are_equal(tail, expansion), label);
	}
	gen_StrBuilderHeader* header = gen_strbuilder_get_header(output);
	expect(header->Allocator.Proc == gen_heap_allocator_proc, label);
	gen_strbuilder_free(& output);
}

gen_internal void expect_template_text(gen_Str label, gen_Str format, gen_StrTemplateSlot* slots, gen_ssize slot_count, gen_Str expansion) {
	expect_template_expansion(label, format, slots, slot_count, expansion);
}

gen_internal void expect_literal_boundary(gen_ssize mark) {
	char format_storage[64];
	char format_bytes  [40];
	for (gen_ssize idx = 0; idx < mark; ++ idx) format_bytes[idx] = 'L';
	gen_Str format    = bounded_format(format_storage, 64, format_bytes, mark);
	gen_Str expansion = { format_bytes, mark };
	expect(format_storage[mark] == '!', gen_txt("literal boundary"));
	expect_template_expansion(gen_txt("literal boundary"), format, 0, 0, expansion);
}

gen_internal void expect_open_boundary(gen_ssize mark) {
	char format_storage [64];
	char format_bytes   [48];
	char expansion_bytes[48];
	gen_ssize format_len = 0;
	for (gen_ssize idx = 0; idx < mark; ++ idx)
		format_bytes[format_len ++] = 'L';
	format_bytes[format_len ++] = '<';
	format_bytes[format_len ++] = 'k';
	format_bytes[format_len ++] = '>';
	format_bytes[format_len ++] = 'Z';
	gen_Str           format = bounded_format(format_storage, 64, format_bytes, format_len);
	gen_StrTemplateSlot slot   = {0};
	slot.Key   = gen_txt("k");
	slot.Value = gen_txt("V");
	gen_ssize expansion_len = 0;
	for (gen_ssize idx = 0; idx < mark; ++ idx)
		expansion_bytes[expansion_len ++] = 'L';
	expansion_bytes[expansion_len ++] = 'V';
	expansion_bytes[expansion_len ++] = 'Z';
	gen_Str expansion = { expansion_bytes, expansion_len };
	expect(format.Ptr[mark]           == '<', gen_txt("open boundary"));
	expect(format_storage[format_len] == '!', gen_txt("open boundary"));
	expect_template_expansion(gen_txt("open boundary"), format, & slot, 1, expansion);
}

gen_internal void expect_close_boundary(gen_ssize mark) {
	char format_storage[64];
	char format_bytes  [48];
	char key_storage   [40];
	gen_ssize key_len = mark - 1;
	gen_mem_set(key_storage, '!', 40);
	for (gen_ssize idx = 0; idx < key_len; ++ idx)
		key_storage[idx] = 'k';
	key_storage[39]      = '\0';
	gen_Str key          = { key_storage, key_len };
	gen_ssize format_len = 0;
	format_bytes[format_len ++] = '<';
	for (gen_ssize idx = 0; idx < key_len; ++ idx)
		format_bytes[format_len ++] = 'k';
	format_bytes[format_len ++] = '>';
	format_bytes[format_len ++] = 'Z';
	gen_Str           format = bounded_format(format_storage, 64, format_bytes, format_len);
	gen_StrTemplateSlot slot   = {0};
	slot.Key   = key;
	slot.Value = gen_txt("V");
	expect(format.Ptr    [mark]       == '>', gen_txt("close boundary"));
	expect(key.Ptr       [key.Len]    == '!', gen_txt("close boundary"));
	expect(format_storage[format_len] == '!', gen_txt("close boundary"));
	expect_template_expansion(gen_txt("close boundary"), format, & slot, 1, gen_txt("VZ"));
}

gen_internal void expect_template_shapes()
{
	char literal_storage[8];
	char literal_bytes  [6] = { 'a', 'b', 'c', 'D', 'E', 'F' };
	gen_Str literal = bounded_format(literal_storage, 8, literal_bytes, 3);
	expect(literal_storage[3] == '!', gen_txt("literal sentinel"));
	expect_template_expansion(gen_txt("literal bounded"), literal, 0, 0, gen_txt("abc"));

	char empty_storage[4];
	gen_Str empty_format = bounded_format(empty_storage, 4, empty_storage, 0);
	expect(empty_storage[0] == '!', gen_txt("empty sentinel"));
	expect_template_expansion(gen_txt("empty format"), empty_format, 0, 0, gen_txt(""));

	gen_StrTemplateSlot name_slot = {0};
	name_slot.Key   = gen_txt("name");
	name_slot.Value = gen_txt("world");
	expect_template_text(gen_txt("one slot"), gen_txt("hello <name>!"), & name_slot, 1, gen_txt("hello world!"));

	gen_StrTemplateSlot repeat_slot = {0};
	repeat_slot.Key   = gen_txt("w");
	repeat_slot.Value = gen_txt("v");
	expect_template_text(gen_txt("repeated slot"), gen_txt("<w>-<w>-<w>"), & repeat_slot, 1, gen_txt("v-v-v"));

	gen_StrTemplateSlot adjacent[2] = {0};
	adjacent[0].Key   = gen_txt("a");
	adjacent[0].Value = gen_txt("X");
	adjacent[1].Key   = gen_txt("b");
	adjacent[1].Value = gen_txt("Y");
	expect_template_text(gen_txt("adjacent slots"), gen_txt("<a><b><a>"), adjacent, 2, gen_txt("XYX"));

	gen_StrTemplateSlot raw_slots[3] = {0};
	raw_slots[0].Key   = gen_txt("raw");
	raw_slots[0].Value = gen_txt("a<b>c");
	raw_slots[1].Key   = gen_txt("angles");
	raw_slots[1].Value = gen_txt("<>");
	raw_slots[2].Key   = gen_txt("b");
	raw_slots[2].Value = gen_txt("SHOULD_NOT");
	expect_template_text(gen_txt("value angles literal"), gen_txt("<raw><angles>"), raw_slots, 3, gen_txt("a<b>c<>"));

	char empty_value_storage[2] = { '!', '!' };
	gen_StrTemplateSlot empty_slot = {0};
	empty_slot.Key   = gen_txt("e");
	empty_slot.Value = (gen_Str){ empty_value_storage, 0 };
	expect_template_text(gen_txt("empty value"), gen_txt("A<e>B"), & empty_slot, 1, gen_txt("AB"));

	gen_StrTemplateSlot empty_key = {0};
	empty_key.Key   = (gen_Str){ 0, 0 };
	empty_key.Value = gen_txt("Z");
	expect_template_text(gen_txt("empty key"), gen_txt("<>"), & empty_key, 1, gen_txt("Z"));

	gen_StrTemplateSlot null_value = {0};
	null_value.Key   = gen_txt("e");
	null_value.Value = (gen_Str){ 0, 0 };
	expect(null_value.Value.Ptr == 0, gen_txt("null empty value pointer"));
	expect(null_value.Value.Len == 0, gen_txt("null empty value length"));
	expect_template_text(gen_txt("null empty value"), gen_txt("A<e>B"), & null_value, 1, gen_txt("AB"));

	char nul_value_bytes[4] = { 'a', '\0', 'b', '!' };
	char nul_expected   [5] = { '[', 'a', '\0', 'b', ']' };
	gen_StrTemplateSlot nul_slot = {0};
	nul_slot.Key   = gen_txt("n");
	nul_slot.Value = (gen_Str){ nul_value_bytes, 3 };
	gen_Str nul_expansion = { nul_expected, 5 };
	expect_template_text(gen_txt("embedded nul"), gen_txt("[<n>]"), & nul_slot, 1, nul_expansion);

	gen_StrTemplateSlot duffle[2] = {0};
	duffle[0].Key   = gen_txt("operands[0]");
	duffle[0].Value = gen_txt("eax");
	duffle[1].Key   = gen_txt("mod_rm.bits");
	duffle[1].Value = gen_txt("11");
	expect_template_text(gen_txt("duffle keys"), gen_txt("mov <operands[0]>, <mod_rm.bits>;"), duffle, 2, gen_txt("mov eax, 11;"));
}

gen_internal void fill_lookup_slots(gen_StrTemplateSlot* slots, gen_ssize slot_count) {
	gen_Str keys[9] = {
		gen_txt("k0"), gen_txt("k1"), gen_txt("k2"), gen_txt("k3"), gen_txt("k4"),
		gen_txt("k5"), gen_txt("k6"), gen_txt("k7"), gen_txt("k8")
	};
	gen_Str values[9] = {
		gen_txt("V0"), gen_txt("V1"), gen_txt("V2"), gen_txt("V3"), gen_txt("V4"),
		gen_txt("V5"), gen_txt("V6"), gen_txt("V7"), gen_txt("V8")
	};
	for (gen_ssize idx = 0; idx < slot_count; ++ idx) {
		slots[idx].Key   = keys[idx];
		slots[idx].Value = values[idx];
	}
}

gen_internal void expect_template_lookup()
{
	gen_StrTemplateSlot slots[9] = {0};
	expect_template_text(gen_txt("lookup 0"), gen_txt("none"), 0, 0, gen_txt("none"));

	fill_lookup_slots(slots, 1);
	expect_template_text(gen_txt("lookup 1"), gen_txt("<k0>"), slots, 1, gen_txt("V0"));

	fill_lookup_slots(slots, 2);
	expect_template_text(gen_txt("lookup 2"), gen_txt("<k1><k0>"), slots, 2, gen_txt("V1V0"));

	fill_lookup_slots(slots, 3);
	expect_template_text(gen_txt("lookup 3"), gen_txt("<k2>-<k0>-<k1>"), slots, 3, gen_txt("V2-V0-V1"));

	fill_lookup_slots(slots, 4);
	expect_template_text(gen_txt("lookup 4"), gen_txt("<k3><k0><k2><k1>"), slots, 4, gen_txt("V3V0V2V1"));

	fill_lookup_slots(slots, 5);
	expect_template_text(gen_txt("lookup 5 tail"), gen_txt("<k4><k0><k3>"), slots, 5, gen_txt("V4V0V3"));

	fill_lookup_slots(slots, 8);
	expect_template_text(gen_txt("lookup 8 lanes"), gen_txt("<k7><k4><k3><k0>"), slots, 8, gen_txt("V7V4V3V0"));

	fill_lookup_slots(slots, 9);
	expect_template_text(gen_txt("lookup 9 tail"), gen_txt("<k8><k5><k6><k4>"), slots, 9, gen_txt("V8V5V6V4"));

	gen_StrTemplateSlot dup_small[3] = {0};
	dup_small[0].Key = gen_txt("dup");   dup_small[0].Value = gen_txt("FIRST");
	dup_small[1].Key = gen_txt("other"); dup_small[1].Value = gen_txt("X");
	dup_small[2].Key = gen_txt("dup");   dup_small[2].Value = gen_txt("SECOND");
	expect_template_text(gen_txt("duplicate first wins"), gen_txt("<dup><other>"), dup_small, 3, gen_txt("FIRSTX"));

	gen_StrTemplateSlot dup_lanes[4] = {0};
	dup_lanes[0].Key = gen_txt("same");  dup_lanes[0].Value = gen_txt("A");
	dup_lanes[1].Key = gen_txt("same");  dup_lanes[1].Value = gen_txt("B");
	dup_lanes[2].Key = gen_txt("other"); dup_lanes[2].Value = gen_txt("C");
	dup_lanes[3].Key = gen_txt("same");  dup_lanes[3].Value = gen_txt("D");
	expect_template_text(gen_txt("duplicate lane first wins"), gen_txt("<same><other>"), dup_lanes, 4, gen_txt("AC"));

	gen_StrTemplateSlot dup_tail[5] = {0};
	dup_tail[0].Key = gen_txt("dup"); dup_tail[0].Value = gen_txt("FIRST");
	dup_tail[1].Key = gen_txt("k1");  dup_tail[1].Value = gen_txt("V1");
	dup_tail[2].Key = gen_txt("k2");  dup_tail[2].Value = gen_txt("V2");
	dup_tail[3].Key = gen_txt("k3");  dup_tail[3].Value = gen_txt("V3");
	dup_tail[4].Key = gen_txt("dup"); dup_tail[4].Value = gen_txt("TAIL");
	expect_template_text(gen_txt("duplicate tail first wins"), gen_txt("<dup>"), dup_tail, 5, gen_txt("FIRST"));
}

gen_internal void expect_template_boundaries() {
	gen_ssize marks[12] = { 7, 8, 9, 15, 16, 17, 23, 24, 25, 31, 32, 33 };
	for (gen_ssize idx = 0; idx < gen_count_of(marks); ++ idx) {
		expect_literal_boundary(marks[idx]);
		expect_open_boundary(marks[idx]);
		expect_close_boundary(marks[idx]);
	}
}

gen_internal void expect_template_growth(gen_ssize printf_maxlen)
{
	gen_ssize huge_len = printf_maxlen + 64;
	char*  huge = gen_rcast(char*, gen_alloc(gen_heap(), huge_len + 1));
	expect(huge != 0, gen_txt("growth alloc"));
	if (huge == 0) return;
	gen_mem_set(huge, 'Q', huge_len);
	huge[huge_len] = '!';

	gen_Str             value = { huge, huge_len };
	gen_StrTemplateSlot slot  = {0};
	slot.Key   = gen_txt("blob");
	slot.Value = value;

	char format_storage[16];
	gen_Str        format = gen_txt("<blob>");
	gen_StrBuilder output = gen_strbuilder_make_reserve(gen_heap(), 1);
	expect(gen_strbuilder_capacity(output) == 1,                 gen_txt("growth initial capacity"));
	expect(gen_strbuilder_append_str(& output, gen_txt("KEEP")), gen_txt("growth prefix"));

	gen_Slice_StrTemplateSlot slice = { & slot, 1 };
	bool   appended = gen_strbuilder_append_template(& output, slice, format);
	expect(appended, gen_txt("growth append"));

	gen_Str got = gen_strbuilder_to_str(output);
	expect(got.Len == 4 + huge_len,                   gen_txt("growth length"));
	expect(got.Len > printf_maxlen,                   gen_txt("growth beyond printf max"));
	expect(gen_str_starts_with(got, gen_txt("KEEP")), gen_txt("growth keeps prefix"));

	gen_ssize capacity = gen_strbuilder_capacity(output);
	expect(capacity >= got.Len, gen_txt("growth capacity"));
	if (got.Len == 4 + huge_len && capacity >= got.Len) {
		gen_b32 payload_matches = true;
		for (gen_ssize idx = 0; idx < got.Len - 4; ++ idx) {
			if (got.Ptr[4 + idx] != 'Q') payload_matches = false;
		}
		expect(payload_matches, gen_txt("growth bytes"));
	}

	gen_StrBuilderHeader* header = gen_strbuilder_get_header(output);
	expect(header->Allocator.Proc == gen_heap_allocator_proc, gen_txt("growth allocator"));
	expect(huge[huge_len] == '!', gen_txt("growth sentinel"));
	gen_strbuilder_free(& output);
	gen_allocator_free(gen_heap(), huge);
}

gen_internal void expect_template_crc_collision() {
	gen_Str plumless  = gen_txt("plumless");
	gen_Str buckeroo  = gen_txt("buckeroo");
	gen_u32 plum_hash = gen_crc32(plumless.Ptr, plumless.Len);
	gen_u32 buck_hash = gen_crc32(buckeroo.Ptr, buckeroo.Len);
	expect(plum_hash == buck_hash, gen_txt("crc collision equal"));
	expect(plum_hash == 0x4ddb0c25, gen_txt("crc collision value"));
	expect(gen_str_are_equal(plumless, buckeroo) == false, gen_txt("crc collision keys differ"));

	gen_StrTemplateSlot scalar[2] = {0};
	scalar[0].Key = plumless; scalar[0].Value = gen_txt("WRONG");
	scalar[1].Key = buckeroo; scalar[1].Value = gen_txt("RIGHT");
	expect_template_text(gen_txt("crc scalar 2"), gen_txt("<buckeroo>"), scalar, 2, gen_txt("RIGHT"));

	gen_StrTemplateSlot lanes[4] = {0};
	lanes[0].Key = plumless; lanes[0].Value = gen_txt("WRONG");
	lanes[1].Key = gen_txt("k1"); lanes[1].Value = gen_txt("V1");
	lanes[2].Key = buckeroo; lanes[2].Value = gen_txt("RIGHT");
	lanes[3].Key = gen_txt("k3"); lanes[3].Value = gen_txt("V3");
	expect_template_text(gen_txt("crc lane reject"), gen_txt("<buckeroo>"), lanes, 4, gen_txt("RIGHT"));

	gen_StrTemplateSlot tail[5] = {0};
	tail[0].Key = plumless; tail[0].Value = gen_txt("WRONG");
	tail[1].Key = gen_txt("k1"); tail[1].Value = gen_txt("V1");
	tail[2].Key = gen_txt("k2"); tail[2].Value = gen_txt("V2");
	tail[3].Key = gen_txt("k3"); tail[3].Value = gen_txt("V3");
	tail[4].Key = buckeroo; tail[4].Value = gen_txt("TAIL");
	expect_template_text(gen_txt("crc tail after false hit"), gen_txt("<buckeroo>"), tail, 5, gen_txt("TAIL"));
}

gen_internal void* resize_fail_allocator_proc(void* allocator_data, gen_AllocType type, gen_ssize size, gen_ssize alignment
	, void* old_memory, gen_ssize old_size
	, gen_u64 flags
){
	if (type == EAllocation_RESIZE) {
		gen_s32* attempts = gen_rcast(gen_s32*, allocator_data);
		attempts[0] = attempts[0] + 1;
		return 0;
	}
	return gen_heap_allocator_proc(0, type, size, alignment, old_memory, old_size, flags);
}

gen_internal void expect_template_resize_fail(gen_ssize printf_maxlen)
{
	gen_ssize huge_len = printf_maxlen + 64;
	char* huge = gen_rcast(char*, gen_alloc(gen_heap(), huge_len + 1));
	expect(huge != 0, gen_txt("resize fail alloc"));
	if (huge == 0) return;
	gen_mem_set(huge, 'L', huge_len);
	huge[huge_len]    = '!';
	gen_Str huge_text = { huge, huge_len };

	gen_s32           literal_attempts = 0;
	gen_AllocatorInfo literal_alloc    = {0};
	literal_alloc.Proc = resize_fail_allocator_proc;
	literal_alloc.Data = & literal_attempts;
	gen_StrBuilder literal_output = gen_strbuilder_make_reserve(literal_alloc, 4);
	expect(gen_strbuilder_append_str(& literal_output, gen_txt("KEEP")), gen_txt("resize fail literal prefix"));
	
	gen_Slice_StrTemplateSlot literal_slots    = { 0, 0 };
	bool                      literal_appended = gen_strbuilder_append_template(& literal_output, literal_slots, huge_text);
	expect(literal_appended == false, gen_txt("resize fail literal"));

	gen_Str literal_got = gen_strbuilder_to_str(literal_output);
	expect(literal_got.Len == 4,                            gen_txt("resize fail literal length"));
	expect(gen_str_are_equal(literal_got, gen_txt("KEEP")), gen_txt("resize fail literal keep"));
	expect(literal_attempts > 0,                            gen_txt("resize fail literal attempts"));
	gen_strbuilder_free(& literal_output);

	gen_s32           value_attempts = 0;
	gen_AllocatorInfo value_alloc    = {0};
	value_alloc.Proc = resize_fail_allocator_proc;
	value_alloc.Data = & value_attempts;
	
	gen_StrTemplateSlot slot = {0};
	slot.Key   = gen_txt("blob");
	slot.Value = huge_text;
	char format_storage[16];
	
	gen_Str        format       = gen_txt("<blob>");
	gen_StrBuilder value_output = gen_strbuilder_make_reserve(value_alloc, 4);
	expect(gen_strbuilder_append_str(& value_output, gen_txt("KEEP")), gen_txt("resize fail value prefix"));

	gen_Slice_StrTemplateSlot value_slots = { & slot, 1 };
	bool value_appended = gen_strbuilder_append_template(& value_output, value_slots, format);
	expect(value_appended == false, gen_txt("resize fail value"));

	gen_Str value_got = gen_strbuilder_to_str(value_output);
	expect(value_got.Len == 4,                            gen_txt("resize fail value length"));
	expect(gen_str_are_equal(value_got, gen_txt("KEEP")), gen_txt("resize fail value keep"));
	expect(value_attempts > 0,                            gen_txt("resize fail value attempts"));

	gen_strbuilder_free(& value_output);
	gen_allocator_free(gen_heap(), huge);
}

gen_internal gen_Str         counted_template_value (gen_s32* reads)                         { reads[0] = reads[0] + 1; return gen_txt("ONCE"); }
gen_internal gen_Str         counted_template_format(gen_s32* reads)                         { reads[0] = reads[0] + 1; return gen_txt("<k>:<k>"); }
gen_internal gen_StrBuilder* counted_template_output(gen_StrBuilder* output, gen_s32* reads) { reads[0] = reads[0] + 1; return output; }
gen_internal bool*           counted_template_result(bool* result, gen_s32* reads)           { reads[0] = reads[0] + 1; return result; }

gen_internal void expect_template_slice()
{
	gen_StrTemplateSlot slots[2] = {
		gen_strtmpl_slot("a", gen_txt("X")),
		gen_strtmpl_slot("b", gen_txt("Y"))
	};
	gen_Slice_StrTemplateSlot slice  = { slots, 2 };
	gen_StrBuilder          output = gen_strbuilder_make_reserve(gen_heap(), 1);
	expect(gen_strbuilder_append_str(& output, gen_txt("KEEP")), gen_txt("slice prefix"));

	bool   appended = gen_strbuilder_append_template(& output, slice, gen_txt("<b><a><b>"));
	expect(appended, gen_txt("slice append"));
	expect(gen_str_are_equal(gen_strbuilder_to_str(output), gen_txt("KEEPYXY")), gen_txt("slice text"));
	expect(gen_str_are_equal(slots[0].Key,                  gen_txt("a")),       gen_txt("slice key"));
	expect(gen_str_are_equal(slots[0].Value,                gen_txt("X")),       gen_txt("slice value"));
	gen_strbuilder_free(& output);

	slice.Ptr[0].Value = gen_txt("Z");
	gen_StrBuilder mutated_output   = gen_strbuilder_make_reserve(gen_heap(), 1);
	bool           mutated_appended = gen_strbuilder_append_template(& mutated_output, slice, gen_txt("<b><a><b>"));
	expect(mutated_appended, gen_txt("slice mutate append"));
	expect(gen_str_are_equal(gen_strbuilder_to_str(mutated_output), gen_txt("YZY")), gen_txt("slice mutate text"));
	gen_strbuilder_free(& mutated_output);

	gen_Slice_StrTemplateSlot empty_slice  = { 0, 0 };
	gen_StrBuilder          empty_output = gen_strbuilder_make_reserve(gen_heap(), 1);
	expect(gen_strbuilder_append_str(& empty_output, gen_txt("KEEP")), gen_txt("zero slice prefix"));

	bool   empty_appended = gen_strbuilder_append_template(& empty_output, empty_slice, gen_txt("none"));
	expect(empty_appended, gen_txt("zero slice append"));
	expect(gen_str_are_equal(gen_strbuilder_to_str(empty_output), gen_txt("KEEPnone")), gen_txt("zero slice text"));
	expect(empty_slice.Ptr == 0,                                                        gen_txt("zero slice pointer"));
	expect(empty_slice.Len == 0,                                                        gen_txt("zero slice length"));
	gen_strbuilder_free(& empty_output);
}

gen_internal void expect_template_wrapper() {
	gen_StrBuilder output = gen_strbuilder_make_reserve(gen_heap(), 1);
	expect(gen_strbuilder_append_str(& output, gen_txt("KEEP")), gen_txt("wrapper prefix"));
	bool appended = false;
	gen_strtmpl_append(& output, appended, gen_strtmpl_slots(
			gen_strtmpl_slot("name", gen_txt("world")),
			gen_strtmpl_slot("tail", gen_txt("!"))
		),
		gen_txt(
			"hello <name>\n"
			"<name><tail>"
		)
	);
	expect(appended, gen_txt("wrapper multiline result"));
	gen_Str got = gen_strbuilder_to_str(output);
	expect(gen_str_starts_with(got, gen_txt("KEEP")),                    gen_txt("wrapper prefix kept"));
	expect(gen_str_are_equal  (got, gen_txt("KEEPhello world\nworld!")), gen_txt("wrapper multiline text"));
	gen_strbuilder_free(& output);
}

gen_internal void expect_template_wrapper_scope()
{
	gen_StrBuilder then_output = gen_strbuilder_make_reserve(gen_heap(), 1);
	gen_StrBuilder else_output = gen_strbuilder_make_reserve(gen_heap(), 1);
	expect(gen_strbuilder_append_str(& then_output, gen_txt("KEEP")), gen_txt("wrapper then prefix"));
	expect(gen_strbuilder_append_str(& else_output, gen_txt("KEEP")), gen_txt("wrapper else prefix"));

	bool then_result     = false;
	bool else_result     = false;
	gen_s32 selector     = 1;
	gen_s32 after_branch = 0;
	if (selector == 1)
		gen_strtmpl_append(& then_output, then_result, gen_strtmpl_slots(gen_strtmpl_slot("arm", gen_txt("THEN"))),
			gen_txt("<arm>")
		);
	else
		gen_strtmpl_append(& else_output, else_result, gen_strtmpl_slots(gen_strtmpl_slot("arm", gen_txt("ELSE"))),
			gen_txt("<arm>")
		);
	after_branch = 1;
	expect(after_branch == 1,    gen_txt("wrapper then continues"));
	expect(then_result,          gen_txt("wrapper then assigned"));
	expect(else_result == false, gen_txt("wrapper else skipped"));
	expect(gen_str_are_equal(gen_strbuilder_to_str(then_output), gen_txt("KEEPTHEN")), gen_txt("wrapper then text"));
	expect(gen_str_are_equal(gen_strbuilder_to_str(else_output), gen_txt("KEEP")),     gen_txt("wrapper else untouched"));
	gen_strbuilder_free(& then_output);
	gen_strbuilder_free(& else_output);

	then_output = gen_strbuilder_make_reserve(gen_heap(), 1);
	else_output = gen_strbuilder_make_reserve(gen_heap(), 1);
	expect(gen_strbuilder_append_str(& then_output, gen_txt("KEEP")), gen_txt("wrapper skipped then prefix"));
	expect(gen_strbuilder_append_str(& else_output, gen_txt("KEEP")), gen_txt("wrapper taken else prefix"));

	then_result  = false;
	else_result  = false;
	selector     = 0;
	after_branch = 0;
	if (selector == 1) gen_strtmpl_append(& then_output, then_result, gen_strtmpl_slots(gen_strtmpl_slot("arm", gen_txt("THEN"))), gen_txt("<arm>"));
	else               gen_strtmpl_append(& else_output, else_result, gen_strtmpl_slots(gen_strtmpl_slot("arm", gen_txt("ELSE"))), gen_txt("<arm>"));
	after_branch = 1;
	expect(after_branch == 1,     gen_txt("wrapper else continues"));
	expect(then_result  == false, gen_txt("wrapper then skipped"));
	expect(else_result, gen_txt("wrapper else assigned"));
	expect(gen_str_are_equal(gen_strbuilder_to_str(then_output), gen_txt("KEEP")),     gen_txt("wrapper then untouched"));
	expect(gen_str_are_equal(gen_strbuilder_to_str(else_output), gen_txt("KEEPELSE")), gen_txt("wrapper else text"));
	gen_strbuilder_free(& then_output);
	gen_strbuilder_free(& else_output);
}

gen_internal void expect_template_wrapper_once() {
	gen_s32 value_reads  = 0;
	gen_s32 format_reads = 0;
	gen_s32 output_reads = 0;
	gen_s32 result_reads = 0;
	gen_StrBuilder output = gen_strbuilder_make_reserve(gen_heap(), 1);
	expect(gen_strbuilder_append_str(& output, gen_txt("KEEP")), gen_txt("wrapper once prefix"));
	bool appended = false;
	gen_strtmpl_append(
		counted_template_output(& output,   & output_reads),
		counted_template_result(& appended, & result_reads)[0],
		gen_strtmpl_slots(
			gen_strtmpl_slot("k", counted_template_value(& value_reads)),
			gen_strtmpl_slot("tail", gen_txt("Z"))
		),
		counted_template_format(& format_reads)
	);
	expect(appended,          gen_txt("wrapper once result"));
	expect(value_reads  == 1, gen_txt("wrapper value once"));
	expect(format_reads == 1, gen_txt("wrapper format once"));
	expect(output_reads == 1, gen_txt("wrapper output once"));
	expect(result_reads == 1, gen_txt("wrapper result once"));
	expect(gen_str_are_equal(gen_strbuilder_to_str(output), gen_txt("KEEPONCE:ONCE")), gen_txt("wrapper once text"));
	gen_strbuilder_free(& output);
}

gen_internal void expect_template_wrapper_resize_fail() {
	gen_s32           attempts = 0;
	gen_AllocatorInfo alloc    = {0};
	alloc.Proc = resize_fail_allocator_proc;
	alloc.Data = & attempts;
	gen_StrBuilder output = gen_strbuilder_make_reserve(alloc, 4);
	expect(gen_strbuilder_capacity(output) == 4, gen_txt("wrapper resize capacity"));
	expect(gen_strbuilder_append_str(& output, gen_txt("KEEP")), gen_txt("wrapper resize prefix"));

	bool appended = true;
	gen_strtmpl_append(& output, appended,
		gen_strtmpl_slots(gen_strtmpl_slot("blob", gen_txt("X"))),
		gen_txt("<blob>")
	);
	expect(appended == false, gen_txt("wrapper resize assigned false"));
	expect(gen_str_are_equal(gen_strbuilder_to_str(output), gen_txt("KEEP")), gen_txt("wrapper resize keep"));
	expect(attempts > 0, gen_txt("wrapper resize attempts"));
	gen_strbuilder_free(& output);
}

gen_internal void expect_template_slot_literal()
{
	gen_Str             value = gen_txt("V");
	gen_StrTemplateSlot slot  = gen_strtmpl_slot("k", value);
	expect(gen_str_are_equal(slot.Key, gen_txt("k")), gen_txt("slot literal key slice"));
	expect(slot.Key.Ptr[slot.Key.Len] == '\0',        gen_txt("slot literal key terminator"));
	expect(slot.Value.Ptr == value.Ptr,               gen_txt("slot literal value pointer"));
	expect(slot.Value.Len == value.Len,               gen_txt("slot literal value length"));

	gen_Str assigned = gen_txt("W");
	slot = gen_strtmpl_slot("key", assigned);
	expect(gen_str_are_equal(slot.Key, gen_txt("key")), gen_txt("slot assign key slice"));
	expect(slot.Value.Ptr == assigned.Ptr,    gen_txt("slot assign value pointer"));
	expect(slot.Value.Len == assigned.Len,    gen_txt("slot assign value length"));
	expect_template_text(gen_txt("slot assign expansion"), gen_txt("<key>"), & slot, 1, gen_txt("W"));

	gen_StrTemplateSlot row[2] = {
		gen_strtmpl_slot("a", gen_txt("X")),
		gen_strtmpl_slot("b", gen_txt("Y"))
	};
	expect(gen_str_are_equal(row[0].Value, gen_txt("X")), gen_txt("slot array value slice"));
	expect(gen_str_are_equal(row[1].Key, gen_txt("b")), gen_txt("slot array key slice"));
	expect_template_text(gen_txt("slot array entry"), gen_txt("<b><a>"), row, 2, gen_txt("YX"));

	gen_StrBuilder output = gen_strbuilder_make_reserve(gen_heap(), 1);
	expect(gen_strbuilder_append_str(& output, gen_txt("KEEP")), gen_txt("slot wrapper prefix"));
	bool appended = false;
	gen_strtmpl_append(& output, appended, gen_strtmpl_slots(
			gen_strtmpl_slot("name", gen_txt("world")),
			gen_strtmpl_slot("tail", gen_txt("!"))
		),
		gen_txt("<name><tail>")
	);
	expect(appended, gen_txt("slot wrapper result"));
	expect(gen_str_are_equal(gen_strbuilder_to_str(output), gen_txt("KEEPworld!")), gen_txt("slot wrapper text"));
	gen_strbuilder_free(& output);

	gen_StrTemplateSlot long_key = gen_strtmpl_slot("operands[0]", gen_txt("eax"));
	expect(gen_str_are_equal(long_key.Key, gen_txt("operands[0]")), gen_txt("slot literal key slice"));
	expect(long_key.Key.Ptr[long_key.Key.Len] == '\0',              gen_txt("slot literal key excludes nul"));
	expect(gen_str_are_equal(long_key.Value, gen_txt("eax")),       gen_txt("slot literal value slice"));

	gen_s32 value_reads = 0;
	gen_StrTemplateSlot once = gen_strtmpl_slot("k", counted_template_value(& value_reads));
	expect(value_reads == 1, gen_txt("slot value once"));
	expect(gen_str_are_equal(once.Value, gen_txt("ONCE")), gen_txt("slot value once text"));
	expect(once.Key.Len == 1, gen_txt("slot value once key"));

	char nul_bytes[4] = { 'a', '\0', 'b', '!' };
	gen_Str             nul_value = { nul_bytes, 3 };
	gen_StrTemplateSlot nul_slot  = gen_strtmpl_slot("n", nul_value);
	expect(nul_slot.Value.Ptr    == nul_bytes, gen_txt("slot nul pointer"));
	expect(nul_slot.Value.Len    == 3,         gen_txt("slot nul length"));
	expect(nul_slot.Value.Ptr[1] == '\0',      gen_txt("slot nul byte"));
	char    nul_expected[5] = { '[', 'a', '\0', 'b', ']' };
	gen_Str nul_expansion   = { nul_expected, 5 };
	expect_template_text(gen_txt("slot nul expansion"), gen_txt("[<n>]"), & nul_slot, 1, nul_expansion);

	gen_Str             angle_value = gen_txt("a<b>c");
	gen_StrTemplateSlot angle_slot  = gen_strtmpl_slot("raw", angle_value);
	expect(angle_slot.Value.Ptr == angle_value.Ptr, gen_txt("slot angle pointer"));
	expect(angle_slot.Value.Len == angle_value.Len, gen_txt("slot angle length"));
	expect_template_text(gen_txt("slot angle expansion"), gen_txt("<raw>"), & angle_slot, 1, gen_txt("a<b>c"));

	gen_Str             empty_value = { 0, 0 };
	gen_StrTemplateSlot empty_slot  = gen_strtmpl_slot("e", empty_value);
	expect(empty_slot.Value.Ptr == 0, gen_txt("slot null value pointer"));
	expect(empty_slot.Value.Len == 0, gen_txt("slot null value length"));
	expect_template_text(gen_txt("slot null value expansion"), gen_txt("A<e>B"), & empty_slot, 1, gen_txt("AB"));

	gen_Str             percent_value = gen_txt("%d:%S");
	gen_StrTemplateSlot percent_slot  = gen_strtmpl_slot("fmt", percent_value);
	expect(percent_slot.Value.Ptr == percent_value.Ptr,             gen_txt("slot percent value pointer"));
	expect(gen_str_are_equal(percent_slot.Value, gen_txt("%d:%S")), gen_txt("slot percent value text"));
	expect_template_text(gen_txt("slot percent literal"), gen_txt("pre %d:%S <fmt>"), & percent_slot, 1, gen_txt("pre %d:%S %d:%S"));
}

gen_internal void expect_text_template(gen_ssize printf_maxlen) {
	expect_template_shapes();
	expect_template_lookup();
	expect_template_crc_collision();
	expect_template_boundaries();
	expect_template_growth(printf_maxlen);
	expect_template_resize_fail(printf_maxlen);
	expect_template_slice();
	expect_template_wrapper();
	expect_template_wrapper_scope();
	expect_template_wrapper_once();
	expect_template_wrapper_resize_fail();
	expect_template_slot_literal();
}
