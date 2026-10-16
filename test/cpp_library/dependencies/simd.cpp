internal void expect_simd_lanes()
{
#if defined(GEN_SYSTEM_WINDOWS) && (defined(_M_X64) || defined(__x86_64__))
	using namespace gen;
	u8 storage[32];
	mem_set(storage, 0x11, 32);
	u8 pattern[16] = {
		0x00, 0x80, 0xFF, 0x01,
		0x02, 0x03, 0x04, 0x05,
		0x06, 0x07, 0x08, 0x09,
		0x0A, 0x0B, 0x0C, 0x0D
	};
	mem_copy(storage + 1, pattern, 16);
	u8x16 loaded = u8x16_load(storage + 1);
	u8x16 again = u8x16_load(storage + 1);
	u16 same = u8x16_mask(u8x16_equal(loaded, again));
	expect(same == 0xFFFF, txt("simd u8 equal load"));
	u16 zero_mask = u8x16_mask(u8x16_equal(loaded, u8x16_splat(0x00)));
	expect(zero_mask == 0x0001, txt("simd byte 0"));
	u16 high_bit = u8x16_mask(u8x16_equal(loaded, u8x16_splat(0x80)));
	expect(high_bit == 0x0002, txt("simd byte 0x80"));
	u16 top = u8x16_mask(u8x16_equal(loaded, u8x16_splat(0xFF)));
	expect(top == 0x0004, txt("simd byte 0xFF"));

	u8 high_lane[16];
	mem_set(high_lane, 0x22, 16);
	high_lane[15] = 0xFF;
	u8x16 high_loaded = u8x16_load(high_lane);
	u16 lane15 = u8x16_mask(u8x16_equal(high_loaded, u8x16_splat(0xFF)));
	expect(lane15 == 0x8000, txt("simd lane 15"));
	expect(u32_mask_first_set(lane15) == 15, txt("simd first set lane 15"));

	u8 window[24];
	mem_set(window, 0xEE, 24);
	window[1] = 0x00;
	window[2] = 0x80;
	window[3] = 0xFF;
	window[4] = 0x11;
	window[5] = 0x22;
	window[6] = 0x33;
	window[7] = 0x44;
	window[8] = 0x55;
	u8x16 loaded8 = u8x16_load8(window + 1);
	u16 upper = u8x16_mask(u8x16_equal(loaded8, u8x16_splat(0x00)));
	expect((upper & 0xFF00) == 0xFF00, txt("simd load8 upper zero"));
	expect((upper & 0x0001) == 0x0001, txt("simd load8 low zero"));
	u16 low_ff = u8x16_mask(u8x16_equal(loaded8, u8x16_splat(0xFF)));
	expect(low_ff == 0x0004, txt("simd load8 low 0xFF"));
	u16 low_80 = u8x16_mask(u8x16_equal(loaded8, u8x16_splat(0x80)));
	expect(low_80 == 0x0002, txt("simd load8 low 0x80"));
	u16 byte7 = u8x16_mask(u8x16_equal(loaded8, u8x16_splat(0x55)));
	expect(byte7 == 0x0080, txt("simd load8 byte 7"));
	u16 sentinel = u8x16_mask(u8x16_equal(loaded8, u8x16_splat(0xEE)));
	expect(sentinel == 0, txt("simd load8 ignores tail"));
	u8x16 loaded16 = u8x16_load(window + 1);
	u16 tail = u8x16_mask(u8x16_equal(loaded16, u8x16_splat(0xEE)));
	expect(tail == 0xFF00, txt("simd load16 sees load8 tail"));

	u32 hashes[4] = { 0x00000000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu };
	u32x4 hash_loaded = u32x4_load(hashes);
	u8 lane0 = u32x4_mask(u32x4_equal(hash_loaded, u32x4_splat(0x00000000u)));
	u8 lane1 = u32x4_mask(u32x4_equal(hash_loaded, u32x4_splat(0x7FFFFFFFu)));
	u8 lane2 = u32x4_mask(u32x4_equal(hash_loaded, u32x4_splat(0x80000000u)));
	u8 lane3 = u32x4_mask(u32x4_equal(hash_loaded, u32x4_splat(0xFFFFFFFFu)));
	expect(lane0 == 0x01, txt("simd u32 lane 0"));
	expect(lane1 == 0x02, txt("simd u32 lane 1"));
	expect(lane2 == 0x04, txt("simd u32 bit31 lane"));
	expect(lane3 == 0x08, txt("simd u32 lane 3"));
	expect((lane2 & 0xF0) == 0, txt("simd u32 mask low bits"));

	u32 all_values[4] = { 0x80000000u, 0x80000000u, 0x80000000u, 0x80000000u };
	u32x4 all_loaded = u32x4_load(all_values);
	u8 all_mask = u32x4_mask(u32x4_equal(all_loaded, u32x4_splat(0x80000000u)));
	expect(all_mask == 0x0F, txt("simd u32 all bit31"));
	u8 none_mask = u32x4_mask(u32x4_equal(all_loaded, u32x4_splat(0x7FFFFFFFu)));
	expect(none_mask == 0x00, txt("simd u32 none"));

	u8 hash_storage[20];
	mem_set(hash_storage, 0xA5, 20);
	mem_copy(hash_storage + 1, hashes, 16);
	u32x4 unaligned = u32x4_load(hash_storage + 1);
	u8 unaligned_mask = u32x4_mask(u32x4_equal(unaligned, u32x4_splat(0x80000000u)));
	expect(unaligned_mask == 0x04, txt("simd u32 unaligned bit31"));

	expect(u32_mask_first_set(0x1u) == 0, txt("simd first set bit 0"));
	expect(u32_mask_first_set(0x14u) == 2, txt("simd first set lowest"));
	expect(u32_mask_first_set(0x8000u) == 15, txt("simd first set bit 15"));
	expect(u32_mask_first_set(0x80000000u) == 31, txt("simd first set bit 31"));
	expect(u32_mask_first_set(0xFFFFFFFFu) == 0, txt("simd first set all bits"));
#endif
}
