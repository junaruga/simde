/* Copyright (c) 2018-2019 Evan Nemerson <evan@nemerson.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if !defined(SIMDE__INSIDE_NEON_H)
#  error Do not include simde/arm/neon/uint8x16.h directly; use simde/arm/neon.h.
#endif

#if !defined(SIMDE__NEON_UINT8X16_H)
#define SIMDE__NEON_UINT8X16_H

typedef union {
#if defined(SIMDE_VECTOR_SUBSCRIPT)
  uint8_t          u8 SIMDE_VECTOR(16) SIMDE_MAY_ALIAS;
#else
  uint8_t          u8[16];
#endif

#if defined(SIMDE_NEON_NATIVE)
  uint8x16_t        n;
#endif

#if defined(SIMDE_NEON_MMX)
  __m64           mmx[2];
#endif
#if defined(SIMDE_NEON_SSE2)
  __m128i         sse;
#endif
} simde_uint8x16_t;

#if defined(SIMDE_NEON_NATIVE)
HEDLEY_STATIC_ASSERT(sizeof(uint8x16_t) == sizeof(simde_uint8x16_t), "uint8x16_t size doesn't match simde_uint8x16_t size");
#endif
HEDLEY_STATIC_ASSERT(16 == sizeof(simde_uint8x16_t), "simde_uint8x16_t size incorrect");

SIMDE__FUNCTION_ATTRIBUTES
simde_uint8x16_t
simde_vaddq_u8(simde_uint8x16_t a, simde_uint8x16_t b) {
  simde_uint8x16_t r;
#if defined(SIMDE_NEON_NATIVE)
  r.n = vaddq_u8(a.n, b.n);
#else
  SIMDE__VECTORIZE
  for (size_t i = 0 ; i < (sizeof(r.u8) / sizeof(r.u8[0])) ; i++) {
    r.u8[i] = a.u8[i] + b.u8[i];
  }
#endif
  return r;
}

SIMDE__FUNCTION_ATTRIBUTES
simde_uint8x16_t
simde_vld1q_u8 (uint8_t const ptr[8]) {
  simde_uint8x16_t r;
#if defined(SIMDE_NEON_NATIVE)
  r.n = vld1q_u8(ptr);
#else
  SIMDE__VECTORIZE
  for (size_t i = 0 ; i < (sizeof(r.u8) / sizeof(r.u8[0])) ; i++) {
    r.u8[i] = ptr[i];
  }
#endif
  return r;
}

SIMDE__FUNCTION_ATTRIBUTES
simde_uint8x16_t
simde_x_vloadq_u8 (uint8_t  l0, uint8_t  l1, uint8_t  l2, uint8_t  l3,
		   uint8_t  l4, uint8_t  l5, uint8_t  l6, uint8_t  l7,
		   uint8_t  l8, uint8_t  l9, uint8_t l10, uint8_t l11,
		   uint8_t l12, uint8_t l13, uint8_t l14, uint8_t l15) {
  uint8_t v[] = { l0, l1,  l2,  l3,  l4,  l5,  l6,  l7,
		  l8, l9, l10, l11, l12, l13, l14, l15};
  return simde_vld1q_u8(v);
}

SIMDE__FUNCTION_ATTRIBUTES
simde_uint8x16_t
simde_vdupq_n_u8 (uint8_t value) {
  simde_uint8x16_t r;
#if defined(SIMDE_NEON_NATIVE)
  r.n = vdupq_n_u8(value);
#else
  SIMDE__VECTORIZE
  for (size_t i = 0 ; i < (sizeof(r.u8) / sizeof(r.u8[0])) ; i++) {
    r.u8[i] = value;
  }
#endif
  return r;
}

SIMDE__FUNCTION_ATTRIBUTES
simde_uint8x16_t
simde_vmulq_u8(simde_uint8x16_t a, simde_uint8x16_t b) {
  simde_uint8x16_t r;
#if defined(SIMDE_NEON_NATIVE)
  r.n = vmulq_u8(a.n, b.n);
#else
  SIMDE__VECTORIZE
  for (size_t i = 0 ; i < (sizeof(r.u8) / sizeof(r.u8[0])) ; i++) {
    r.u8[i] = a.u8[i] * b.u8[i];
  }
#endif
  return r;
}

SIMDE__FUNCTION_ATTRIBUTES
simde_uint8x16_t
simde_vsubq_u8(simde_uint8x16_t a, simde_uint8x16_t b) {
  simde_uint8x16_t r;
#if defined(SIMDE_NEON_NATIVE)
  r.n = vsubq_u8(a.n, b.n);
#else
  SIMDE__VECTORIZE
  for (size_t i = 0 ; i < (sizeof(r.u8) / sizeof(r.u8[0])) ; i++) {
    r.u8[i] = a.u8[i] - b.u8[i];
  }
#endif
  return r;
}

#endif
