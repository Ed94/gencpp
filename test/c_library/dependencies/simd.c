gen_internal void expect_simd_lanes()
{
#if defined(GEN_SYSTEM_WINDOWS) && (defined(_M_X64) || defined(__x86_64__))
	gen_u8 storage[32];
	gen_mem_set(storage, 0x11, 32);
	gen_u8 pattern[16] = {
		0x00, 0x80, 0xFF, 0x01,
		0x02, 0x03, 0x04, 0x05,
		0x06, 0x07, 0x08, 0x09,
		0x0A, 0x0B, 0x0C, 0x0D
	};
	gen_mem_copy(storage + 1, pattern, 16);
	gen_u8x16 loaded  = gen_u8x16_load(storage + 1);
	gen_u8x16 again   = gen_u8x16_load(storage + 1);
	gen_u16   same    = gen_u8x16_mask(gen_u8x16_equal(loaded, again));
	gen_u16 zero_mask = gen_u8x16_mask(gen_u8x16_equal(loaded, gen_u8x16_splat(0x00)));
	gen_u16 high_bit  = gen_u8x16_mask(gen_u8x16_equal(loaded, gen_u8x16_splat(0x80)));
	gen_u16 top       = gen_u8x16_mask(gen_u8x16_equal(loaded, gen_u8x16_splat(0xFF)));
	expect(same      == 0xFFFF, gen_txt("simd u8 equal load"));
	expect(zero_mask == 0x0001, gen_txt("simd byte 0"));
	expect(high_bit  == 0x0002, gen_txt("simd byte 0x80"));
	expect(top       == 0x0004, gen_txt("simd byte 0xFF"));

	gen_u8 high_lane[16];
	gen_mem_set(high_lane, 0x22, 16);
	high_lane[15] = 0xFF;
	gen_u8x16 high_loaded = gen_u8x16_load(high_lane);
	gen_u16   lane15      = gen_u8x16_mask(gen_u8x16_equal(high_loaded, gen_u8x16_splat(0xFF)));
	expect(lane15 == 0x8000, gen_txt("simd lane 15"));
	expect(gen_u32_mask_first_set(lane15) == 15, gen_txt("simd first set lane 15"));

	gen_u8 window[24];
	gen_mem_set(window, 0xEE, 24);
	window[1] = 0x00;
	window[2] = 0x80;
	window[3] = 0xFF;
	window[4] = 0x11;
	window[5] = 0x22;
	window[6] = 0x33;
	window[7] = 0x44;
	window[8] = 0x55;
	gen_u8x16 loaded8 = gen_u8x16_load8(window + 1);
	gen_u16   upper   = gen_u8x16_mask(gen_u8x16_equal(loaded8, gen_u8x16_splat(0x00)));
	gen_u16   low_ff  = gen_u8x16_mask(gen_u8x16_equal(loaded8, gen_u8x16_splat(0xFF)));
	gen_u16   low_80  = gen_u8x16_mask(gen_u8x16_equal(loaded8, gen_u8x16_splat(0x80)));
	gen_u16   byte7   = gen_u8x16_mask(gen_u8x16_equal(loaded8, gen_u8x16_splat(0x55)));
	expect((upper & 0xFF00) == 0xFF00, gen_txt("simd load8 upper zero"));
	expect((upper & 0x0001) == 0x0001, gen_txt("simd load8 low zero"));
	expect(low_ff           == 0x0004, gen_txt("simd load8 low 0xFF"));
	expect(low_80           == 0x0002, gen_txt("simd load8 low 0x80"));
	expect(byte7 == 0x0080, gen_txt("simd load8 byte 7"));
	gen_u16 sentinel = gen_u8x16_mask(gen_u8x16_equal(loaded8, gen_u8x16_splat(0xEE)));
	expect(sentinel == 0, gen_txt("simd load8 ignores tail"));
	gen_u8x16 loaded16 = gen_u8x16_load(window + 1);
	gen_u16   tail     = gen_u8x16_mask(gen_u8x16_equal(loaded16, gen_u8x16_splat(0xEE)));
	expect(tail == 0xFF00, gen_txt("simd load16 sees load8 tail"));

	gen_u32 hashes[4] = { 0x00000000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu };
	gen_u32x4 hash_loaded = gen_u32x4_load(hashes);
	gen_u8 lane0 = gen_u32x4_mask(gen_u32x4_equal(hash_loaded, gen_u32x4_splat(0x00000000u)));
	gen_u8 lane1 = gen_u32x4_mask(gen_u32x4_equal(hash_loaded, gen_u32x4_splat(0x7FFFFFFFu)));
	gen_u8 lane2 = gen_u32x4_mask(gen_u32x4_equal(hash_loaded, gen_u32x4_splat(0x80000000u)));
	gen_u8 lane3 = gen_u32x4_mask(gen_u32x4_equal(hash_loaded, gen_u32x4_splat(0xFFFFFFFFu)));
	expect(lane0 == 0x01, gen_txt("simd u32 lane 0"));
	expect(lane1 == 0x02, gen_txt("simd u32 lane 1"));
	expect(lane2 == 0x04, gen_txt("simd u32 bit31 lane"));
	expect(lane3 == 0x08, gen_txt("simd u32 lane 3"));
	expect((lane2 & 0xF0) == 0, gen_txt("simd u32 mask low bits"));

	gen_u32   all_values[4] = { 0x80000000u, 0x80000000u, 0x80000000u, 0x80000000u };
	gen_u32x4 all_loaded    = gen_u32x4_load(all_values);
	gen_u8    all_mask      = gen_u32x4_mask(gen_u32x4_equal(all_loaded, gen_u32x4_splat(0x80000000u)));
	expect(all_mask == 0x0F, gen_txt("simd u32 all bit31"));
	gen_u8 none_mask = gen_u32x4_mask(gen_u32x4_equal(all_loaded, gen_u32x4_splat(0x7FFFFFFFu)));
	expect(none_mask == 0x00, gen_txt("simd u32 none"));

	gen_u8 hash_storage[20];
	gen_mem_set (hash_storage, 0xA5, 20);
	gen_mem_copy(hash_storage + 1, hashes, 16);
	gen_u32x4 unaligned      = gen_u32x4_load(hash_storage + 1);
	gen_u8    unaligned_mask = gen_u32x4_mask(gen_u32x4_equal(unaligned, gen_u32x4_splat(0x80000000u)));
	expect(unaligned_mask == 0x04, gen_txt("simd u32 unaligned bit31"));

	expect(gen_u32_mask_first_set(0x1u)        == 0,  gen_txt("simd first set bit 0"));
	expect(gen_u32_mask_first_set(0x14u)       == 2,  gen_txt("simd first set lowest"));
	expect(gen_u32_mask_first_set(0x8000u)     == 15, gen_txt("simd first set bit 15"));
	expect(gen_u32_mask_first_set(0x80000000u) == 31, gen_txt("simd first set bit 31"));
	expect(gen_u32_mask_first_set(0xFFFFFFFFu) == 0,  gen_txt("simd first set all bits"));
#endif
}
