#ifdef INTELLISENSE_DIRECTIVES
#pragma once
#include "debug.hpp"
#endif

#pragma region SIMD

// Lane names use bit widths: sixteen unsigned 8-bit lanes or four unsigned 32-bit lanes.
// Vector operations are available on Windows x64.
#if defined(GEN_SYSTEM_WINDOWS) && (defined(_M_X64) || defined(__x86_64__))
typedef __m128i u8x16;
typedef __m128i u32x4;

// Unaligned loads require 16 or 8 readable bytes respectively. load8 zeros lanes 8 through 15.
internal forceinline u8x16 u8x16_load (void const* address) { return _mm_loadu_si128(rcast(__m128i const*, address)); }
internal forceinline u8x16 u8x16_load8(void const* address) { return _mm_loadl_epi64(rcast(__m128i const*, address)); }

internal forceinline u8x16 u8x16_splat(u8 value) { return _mm_set1_epi8(scast(char, value)); }

internal forceinline u8x16 u8x16_equal(u8x16 lhs, u8x16 rhs) { return _mm_cmpeq_epi8(lhs, rhs); }

// Extract each lane's high bit. Bit 0 represents lane 0 at the lowest address.
internal forceinline u16 u8x16_mask(u8x16 value) { return scast(u16, _mm_movemask_epi8(value)); }

// Reads 16 bytes without requiring alignment.
internal forceinline u32x4 u32x4_load(void const* address) { return _mm_loadu_si128(rcast(__m128i const*, address)); }

internal forceinline u32x4 u32x4_splat(u32 value) { return _mm_set1_epi32(scast(s32, value)); }

internal forceinline u32x4 u32x4_equal(u32x4 lhs, u32x4 rhs) { return _mm_cmpeq_epi32(lhs, rhs); }

// Extract each lane's high bit into bits 0 through 3. Higher result bits are zero.
internal forceinline u8 u32x4_mask(u32x4 value) { return scast(u8, _mm_movemask_ps(_mm_castsi128_ps(value))); }
#endif

// Requires a nonzero mask. Returns the lowest set-bit index, from 0 through 31.
s32 u32_mask_first_set(u32 mask);

#if defined(GEN_SYSTEM_WINDOWS)
internal forceinline
s32 u32_mask_first_set(u32 mask) {
	GEN_ASSERT(mask != 0);
	unsigned long bit_index = 0;
	_BitScanForward(& bit_index, scast(unsigned long, mask));
	return scast(s32, bit_index);
}
#endif

#if ! defined(GEN_SYSTEM_WINDOWS)
internal forceinline
s32 u32_mask_first_set(u32 mask) {
	GEN_ASSERT(mask != 0);
	s32 bit  = 0;
	u32 bits = mask;
	while ((bits & 1u) == 0u) {
		bits >>= 1;
		bit  += 1;
	}
	return bit;
}
#endif

#pragma endregion SIMD
