/* Copyright (c) 2017, 2019 Evan Nemerson <evan@nemerson.com>
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

#include "../x86-internal.h"
#include "../../../simde/x86/sse2.h"

#include <stdio.h>
#include <math.h>

#define NAN_AS_U32 (((union { simde_float32 f32; uint32_t u32; }) { .f32 = NAN }).u32)

#define assert_m128_ps(a, cmp, b)					\
  do {									\
    munit_assert_float(((simde_float32*) (&a))[0], cmp, ((simde_float32*) (&b))[0]);	\
    munit_assert_float(((simde_float32*) (&a))[1], cmp, ((simde_float32*) (&b))[1]);	\
    munit_assert_float(((simde_float32*) (&a))[2], cmp, ((simde_float32*) (&b))[2]);	\
    munit_assert_float(((simde_float32*) (&a))[3], cmp, ((simde_float32*) (&b))[3]);	\
  } while (0)

static inline simde__m128
random_m128_f32(void) {
  simde__m128 r;
  random_f32v(sizeof(r.f32) / sizeof(r.f32[0]), (simde_float32*) &r);
  return r;
}

static inline simde__m128
simde_m128_set_u32(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
  return simde_mm_castsi128_ps(simde_x_mm_set_epu32(a, b, c, d));
}

static MunitResult
test_simde_mm_set_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  simde_float32 d[4];
  random_f32v(sizeof(d) / sizeof(d[0]), d);

  simde__m128 x = simde_mm_set_ps(d[0], d[1], d[2], d[3]);

  simde_float32* f = (simde_float32*) &x;
  munit_assert_float(f[0], ==, d[3]);
  munit_assert_float(f[1], ==, d[2]);
  munit_assert_float(f[2], ==, d[1]);
  munit_assert_float(f[3], ==, d[0]);

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_set_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  simde_float32 d;
  random_f32v(1, &d);

  simde__m128 x = simde_mm_set_ss(d);

  simde_float32* f = (simde_float32*) &x;
  munit_assert_float(f[0], ==, d);
  munit_assert_float(f[1], ==, 0.0f);
  munit_assert_float(f[2], ==, 0.0f);
  munit_assert_float(f[3], ==, 0.0f);

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_set1_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  simde_float32 d;
  random_f32v(1, &d);

  simde__m128 x = simde_mm_set1_ps(d);

  simde_float32* f = (simde_float32*) &x;
  munit_assert_float(f[0], ==, d);
  munit_assert_float(f[1], ==, d);
  munit_assert_float(f[2], ==, d);
  munit_assert_float(f[3], ==, d);

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_setr_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  simde_float32 d[4];
  random_f32v(sizeof(d) / sizeof(d[0]), d);

  simde__m128 x = simde_mm_setr_ps(d[3], d[2], d[1], d[0]);

  simde_float32* f = (simde_float32*) &x;
  munit_assert_float(f[0], ==, d[3]);
  munit_assert_float(f[1], ==, d[2]);
  munit_assert_float(f[2], ==, d[1]);
  munit_assert_float(f[3], ==, d[0]);

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_setzero_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  simde__m128 r = simde_mm_setzero_ps();

  simde_float32* f = (simde_float32*) &r;
  munit_assert_float(f[0], ==, 0.0f);
  munit_assert_float(f[1], ==, 0.0f);
  munit_assert_float(f[2], ==, 0.0f);
  munit_assert_float(f[3], ==, 0.0f);

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_add_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps(SIMDE_FLOAT32_C(  782.92), SIMDE_FLOAT32_C( -222.67), SIMDE_FLOAT32_C(  471.90), SIMDE_FLOAT32_C( -984.28)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -729.71), SIMDE_FLOAT32_C( -713.63), SIMDE_FLOAT32_C(  101.93), SIMDE_FLOAT32_C(  419.08)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(   53.21), SIMDE_FLOAT32_C( -936.30), SIMDE_FLOAT32_C(  573.83), SIMDE_FLOAT32_C( -565.20)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C( -310.27), SIMDE_FLOAT32_C( -266.98), SIMDE_FLOAT32_C(  473.62), SIMDE_FLOAT32_C( -317.54)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  896.47), SIMDE_FLOAT32_C(   62.00), SIMDE_FLOAT32_C( -359.88), SIMDE_FLOAT32_C(  461.78)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  586.20), SIMDE_FLOAT32_C( -204.98), SIMDE_FLOAT32_C(  113.74), SIMDE_FLOAT32_C(  144.24)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C( -277.35), SIMDE_FLOAT32_C( -650.32), SIMDE_FLOAT32_C( -363.29), SIMDE_FLOAT32_C( -263.97)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -142.70), SIMDE_FLOAT32_C( -245.27), SIMDE_FLOAT32_C(  255.99), SIMDE_FLOAT32_C( -801.08)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -420.05), SIMDE_FLOAT32_C( -895.59), SIMDE_FLOAT32_C( -107.30), SIMDE_FLOAT32_C(-1065.05)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C(  662.19), SIMDE_FLOAT32_C( -244.69), SIMDE_FLOAT32_C(  858.56), SIMDE_FLOAT32_C( -832.94)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  -70.79), SIMDE_FLOAT32_C(  352.43), SIMDE_FLOAT32_C(  233.94), SIMDE_FLOAT32_C( -606.56)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  591.40), SIMDE_FLOAT32_C(  107.74), SIMDE_FLOAT32_C( 1092.50), SIMDE_FLOAT32_C(-1439.50)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C(  455.61), SIMDE_FLOAT32_C( -560.65), SIMDE_FLOAT32_C(  490.54), SIMDE_FLOAT32_C(  601.32)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -556.80), SIMDE_FLOAT32_C(  720.62), SIMDE_FLOAT32_C(  233.64), SIMDE_FLOAT32_C(  655.91)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -101.19), SIMDE_FLOAT32_C(  159.97), SIMDE_FLOAT32_C(  724.18), SIMDE_FLOAT32_C( 1257.23)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C( -750.70), SIMDE_FLOAT32_C(  176.07), SIMDE_FLOAT32_C( -164.60), SIMDE_FLOAT32_C(  316.39)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -817.56), SIMDE_FLOAT32_C( -794.34), SIMDE_FLOAT32_C(  436.75), SIMDE_FLOAT32_C( -459.44)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(-1568.26), SIMDE_FLOAT32_C( -618.27), SIMDE_FLOAT32_C(  272.15), SIMDE_FLOAT32_C( -143.05)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C(  250.41), SIMDE_FLOAT32_C( -913.31), SIMDE_FLOAT32_C(  734.13), SIMDE_FLOAT32_C(  357.61)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -834.50), SIMDE_FLOAT32_C( -102.94), SIMDE_FLOAT32_C( -972.78), SIMDE_FLOAT32_C( -849.09)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -584.09), SIMDE_FLOAT32_C(-1016.25), SIMDE_FLOAT32_C( -238.65), SIMDE_FLOAT32_C( -491.48)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C( -147.44), SIMDE_FLOAT32_C( -361.51), SIMDE_FLOAT32_C(   21.86), SIMDE_FLOAT32_C( -447.24)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -940.88), SIMDE_FLOAT32_C(  299.17), SIMDE_FLOAT32_C(  981.06), SIMDE_FLOAT32_C( -140.81)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(-1088.32), SIMDE_FLOAT32_C(  -62.34), SIMDE_FLOAT32_C( 1002.92), SIMDE_FLOAT32_C( -588.05)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128 r = simde_mm_add_ps(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_close(r, test_vec[i].r, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_add_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps(SIMDE_FLOAT32_C(  807.64), SIMDE_FLOAT32_C(  618.52), SIMDE_FLOAT32_C( -396.84), SIMDE_FLOAT32_C( -297.31)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -471.66), SIMDE_FLOAT32_C(  713.91), SIMDE_FLOAT32_C(  549.43), SIMDE_FLOAT32_C( -169.45)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  807.64), SIMDE_FLOAT32_C(  618.52), SIMDE_FLOAT32_C( -396.84), SIMDE_FLOAT32_C( -466.76)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C(  544.68), SIMDE_FLOAT32_C(   42.73), SIMDE_FLOAT32_C( -460.22), SIMDE_FLOAT32_C( -292.61)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  187.56), SIMDE_FLOAT32_C( -798.97), SIMDE_FLOAT32_C(  272.51), SIMDE_FLOAT32_C(   51.52)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  544.68), SIMDE_FLOAT32_C(   42.73), SIMDE_FLOAT32_C( -460.22), SIMDE_FLOAT32_C( -241.09)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C( -110.27), SIMDE_FLOAT32_C(  680.50), SIMDE_FLOAT32_C(  761.66), SIMDE_FLOAT32_C(  277.48)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  -67.06), SIMDE_FLOAT32_C(  934.00), SIMDE_FLOAT32_C(  502.27), SIMDE_FLOAT32_C(  358.64)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -110.27), SIMDE_FLOAT32_C(  680.50), SIMDE_FLOAT32_C(  761.66), SIMDE_FLOAT32_C(  636.12)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C(  434.09), SIMDE_FLOAT32_C(  948.98), SIMDE_FLOAT32_C( -323.81), SIMDE_FLOAT32_C( -338.37)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -221.66), SIMDE_FLOAT32_C( -861.25), SIMDE_FLOAT32_C(   78.84), SIMDE_FLOAT32_C( -873.32)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  434.09), SIMDE_FLOAT32_C(  948.98), SIMDE_FLOAT32_C( -323.81), SIMDE_FLOAT32_C(-1211.69)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C( -878.90), SIMDE_FLOAT32_C( -860.71), SIMDE_FLOAT32_C(  840.94), SIMDE_FLOAT32_C(   25.49)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  571.90), SIMDE_FLOAT32_C( -703.66), SIMDE_FLOAT32_C(  -53.73), SIMDE_FLOAT32_C( -560.77)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -878.90), SIMDE_FLOAT32_C( -860.71), SIMDE_FLOAT32_C(  840.94), SIMDE_FLOAT32_C( -535.28)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C(  496.95), SIMDE_FLOAT32_C( -413.78), SIMDE_FLOAT32_C(  -24.10), SIMDE_FLOAT32_C(  918.19)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(   -8.88), SIMDE_FLOAT32_C(  751.51), SIMDE_FLOAT32_C(  315.02), SIMDE_FLOAT32_C(  734.82)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  496.95), SIMDE_FLOAT32_C( -413.78), SIMDE_FLOAT32_C(  -24.10), SIMDE_FLOAT32_C( 1653.01)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C(  101.05), SIMDE_FLOAT32_C( -424.85), SIMDE_FLOAT32_C( -158.08), SIMDE_FLOAT32_C(  354.83)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  568.62), SIMDE_FLOAT32_C( -525.68), SIMDE_FLOAT32_C(   73.90), SIMDE_FLOAT32_C( -463.92)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  101.05), SIMDE_FLOAT32_C( -424.85), SIMDE_FLOAT32_C( -158.08), SIMDE_FLOAT32_C( -109.09)) },
    { simde_mm_set_ps(SIMDE_FLOAT32_C( -477.51), SIMDE_FLOAT32_C(  986.65), SIMDE_FLOAT32_C(  -44.77), SIMDE_FLOAT32_C(  993.26)),
      simde_mm_set_ps(SIMDE_FLOAT32_C(  591.07), SIMDE_FLOAT32_C(  806.35), SIMDE_FLOAT32_C( -137.78), SIMDE_FLOAT32_C( -899.14)),
      simde_mm_set_ps(SIMDE_FLOAT32_C( -477.51), SIMDE_FLOAT32_C(  986.65), SIMDE_FLOAT32_C(  -44.77), SIMDE_FLOAT32_C(   94.12)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128 r = simde_mm_add_ss(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_close(r, test_vec[i].r, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_and_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C(  230308132), INT32_C( -227837326), INT32_C( 2068647778), INT32_C(  339280616)),
      simde_mm_set_epi32(INT32_C(-2005788472), INT32_C(  635603096), INT32_C( 1918425762), INT32_C( -674479342)),
      simde_mm_set_epi32(INT32_C(  137500672), INT32_C(  543295504), INT32_C( 1917323810), INT32_C(  336069120)) },
    { simde_mm_set_epi32(INT32_C( -291660154), INT32_C( 1180299090), INT32_C( 1302245871), INT32_C(-2053540867)),
      simde_mm_set_epi32(INT32_C(  -87778126), INT32_C( -288172017), INT32_C( 1641035711), INT32_C( -987206598)),
      simde_mm_set_epi32(INT32_C( -360407934), INT32_C( 1179699202), INT32_C( 1099968943), INT32_C(-2063048648)) },
    { simde_mm_set_epi32(INT32_C( 1329542878), INT32_C( 1207699188), INT32_C(   24647547), INT32_C( -417192016)),
      simde_mm_set_epi32(INT32_C(-1903170010), INT32_C( 1636014785), INT32_C(-1789298238), INT32_C(  431363012)),
      simde_mm_set_epi32(INT32_C(  235873798), INT32_C( 1098908352), INT32_C(   22548802), INT32_C(   19006336)) },
    { simde_mm_set_epi32(INT32_C( 1291921239), INT32_C(-2088318277), INT32_C(  293687175), INT32_C(  -70855120)),
      simde_mm_set_epi32(INT32_C( -181527647), INT32_C( 1885539289), INT32_C( 1200729803), INT32_C(-1998296563)),
      simde_mm_set_epi32(INT32_C( 1157628673), INT32_C(     133785), INT32_C(   25235075), INT32_C(-2000403968)) },
    { simde_mm_set_epi32(INT32_C( -352637975), INT32_C( 1914022535), INT32_C( -883801519), INT32_C(-2099443995)),
      simde_mm_set_epi32(INT32_C(   82894436), INT32_C(  345641451), INT32_C(-1766434082), INT32_C(  545403119)),
      simde_mm_set_epi32(INT32_C(   15731296), INT32_C(  269484163), INT32_C(-2112732592), INT32_C(    8388837)) },
    { simde_mm_set_epi32(INT32_C(-1731787377), INT32_C( -251709819), INT32_C(  707964452), INT32_C( 1634791391)),
      simde_mm_set_epi32(INT32_C( -198581371), INT32_C( 1425724652), INT32_C(  -85534714), INT32_C(  593903546)),
      simde_mm_set_epi32(INT32_C(-1878982267), INT32_C( 1358566532), INT32_C(  706906116), INT32_C(  559951770)) },
    { simde_mm_set_epi32(INT32_C( 1475548270), INT32_C(  196183104), INT32_C( 1788414168), INT32_C( -339387422)),
      simde_mm_set_epi32(INT32_C(  372209195), INT32_C( 2023049541), INT32_C(  537336467), INT32_C(-1542274771)),
      simde_mm_set_epi32(INT32_C(  371397674), INT32_C(  143720512), INT32_C(  536938640), INT32_C(-1610596064)) },
    { simde_mm_set_epi32(INT32_C( 1365641873), INT32_C(-1596735940), INT32_C( 1148782984), INT32_C( -513375328)),
      simde_mm_set_epi32(INT32_C(-1663652637), INT32_C(-1297596491), INT32_C(-1521550159), INT32_C(-2063966090)),
      simde_mm_set_epi32(INT32_C(  273025153), INT32_C(-1602208204), INT32_C(   71829632), INT32_C(-2141061088)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(simde_mm_and_ps(simde_mm_castsi128_ps(test_vec[i].a), simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_equal(r, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_andnot_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C( 1177189245), INT32_C( 1897888048), INT32_C(-1445410189), INT32_C(-2027773136)),
      simde_mm_set_epi32(INT32_C(-1799619754), INT32_C( 1146608002), INT32_C( -118138295), INT32_C( -396850997)),
      simde_mm_set_epi32(INT32_C(-1869512702), INT32_C(   71336066), INT32_C( 1344606216), INT32_C( 1750599883)) },
    { simde_mm_set_epi32(INT32_C(  207339929), INT32_C(-1653197317), INT32_C( 1742328793), INT32_C(-1943741545)),
      simde_mm_set_epi32(INT32_C( 1475927203), INT32_C(-1393438167), INT32_C( -244601726), INT32_C(-1116772072)),
      simde_mm_set_epi32(INT32_C( 1403001890), INT32_C(  545374720), INT32_C(-1876808702), INT32_C(  827006984)) },
    { simde_mm_set_epi32(INT32_C(  280787508), INT32_C( -476758383), INT32_C( -690500493), INT32_C( 1730225777)),
      simde_mm_set_epi32(INT32_C(  455229920), INT32_C(  254808493), INT32_C( -318322364), INT32_C( -744606195)),
      simde_mm_set_epi32(INT32_C(  184680896), INT32_C(  203424044), INT32_C(  687866116), INT32_C(-1868689396)) },
    { simde_mm_set_epi32(INT32_C( 1314224819), INT32_C( -883715485), INT32_C( 1830060352), INT32_C(  484123993)),
      simde_mm_set_epi32(INT32_C(-1600415403), INT32_C( 1247040590), INT32_C( -903138997), INT32_C( 1339483582)),
      simde_mm_set_epi32(INT32_C(-1601535676), INT32_C(     279564), INT32_C(-2111098869), INT32_C( 1124384934)) },
    { simde_mm_set_epi32(INT32_C(  491653802), INT32_C( 1332021673), INT32_C(-1558543881), INT32_C( 1668194718)),
      simde_mm_set_epi32(INT32_C( 1904424558), INT32_C(-1115627084), INT32_C(-1533509640), INT32_C( 1120379446)),
      simde_mm_set_epi32(INT32_C( 1619080260), INT32_C(-1333731308), INT32_C(   75529736), INT32_C(    8454688)) },
    { simde_mm_set_epi32(INT32_C( 1894428518), INT32_C( 1907474957), INT32_C( -645595730), INT32_C( -336066790)),
      simde_mm_set_epi32(INT32_C( -923650038), INT32_C(-1468609389), INT32_C(-1710432847), INT32_C(-1709785329)),
      simde_mm_set_epi32(INT32_C(-2012213240), INT32_C(-2008675182), INT32_C(   34078737), INT32_C(  268875781)) },
    { simde_mm_set_epi32(INT32_C(  194114005), INT32_C(  123455954), INT32_C(-1857684581), INT32_C( -281966329)),
      simde_mm_set_epi32(INT32_C( -794388211), INT32_C(-1374144398), INT32_C( 1808738256), INT32_C(   -8964047)),
      simde_mm_set_epi32(INT32_C( -802813944), INT32_C(-1476382688), INT32_C( 1787429952), INT32_C(  273035312)) },
    { simde_mm_set_epi32(INT32_C(-1545262354), INT32_C(  905989253), INT32_C( -414577725), INT32_C( 1431039599)),
      simde_mm_set_epi32(INT32_C( 1566256833), INT32_C(-1518668169), INT32_C(-1754223695), INT32_C( 1280260013)),
      simde_mm_set_epi32(INT32_C( 1545208833), INT32_C(-2122665358), INT32_C(  271622192), INT32_C(  134484352)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_andnot_ps(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_avg_pu16(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m64 a;
    simde__m64 b;
    simde__m64 r;
  } test_vec[] = {
    { simde_mm_set_pi16(INT16_C(   6128), INT16_C(   7442), INT16_C( -31532), INT16_C(  -5209)),
      simde_mm_set_pi16(INT16_C(  31554), INT16_C(  22472), INT16_C(  18285), INT16_C(  25512)),
      simde_mm_set_pi16(INT16_C(  18841), INT16_C(  14957), INT16_C(  26145), INT16_C( -22616)) },
    { simde_mm_set_pi16(INT16_C( -19966), INT16_C(   3864), INT16_C(  13970), INT16_C( -16975)),
      simde_mm_set_pi16(INT16_C(  28577), INT16_C( -12299), INT16_C(   -233), INT16_C( -30382)),
      simde_mm_set_pi16(INT16_C( -28462), INT16_C(  28551), INT16_C( -25899), INT16_C( -23678)) },
    { simde_mm_set_pi16(INT16_C( -12513), INT16_C(  13864), INT16_C( -31803), INT16_C(  -7440)),
      simde_mm_set_pi16(INT16_C(  13007), INT16_C(   9436), INT16_C(  -8761), INT16_C( -23247)),
      simde_mm_set_pi16(INT16_C( -32521), INT16_C(  11650), INT16_C( -20282), INT16_C( -15343)) },
    { simde_mm_set_pi16(INT16_C( -14820), INT16_C(  -5122), INT16_C(  21491), INT16_C(  27384)),
      simde_mm_set_pi16(INT16_C( -19778), INT16_C(  15228), INT16_C( -10682), INT16_C(  -2597)),
      simde_mm_set_pi16(INT16_C( -17299), INT16_C( -27715), INT16_C( -27363), INT16_C( -20374)) },
    { simde_mm_set_pi16(INT16_C(  11637), INT16_C( -31082), INT16_C( -21358), INT16_C(  19428)),
      simde_mm_set_pi16(INT16_C( -22928), INT16_C(   8401), INT16_C(  21568), INT16_C(  22582)),
      simde_mm_set_pi16(INT16_C(  27123), INT16_C(  21428), INT16_C( -32663), INT16_C(  21005)) },
    { simde_mm_set_pi16(INT16_C( -16023), INT16_C( -15046), INT16_C(   1378), INT16_C(  32708)),
      simde_mm_set_pi16(INT16_C(  11319), INT16_C(  -5281), INT16_C(  28806), INT16_C( -15277)),
      simde_mm_set_pi16(INT16_C(  30416), INT16_C( -10163), INT16_C(  15092), INT16_C( -24052)) },
    { simde_mm_set_pi16(INT16_C(  11542), INT16_C( -24085), INT16_C(  22205), INT16_C( -30811)),
      simde_mm_set_pi16(INT16_C(  17618), INT16_C( -10296), INT16_C(    694), INT16_C( -11808)),
      simde_mm_set_pi16(INT16_C(  14580), INT16_C( -17190), INT16_C(  11450), INT16_C( -21309)) },
    { simde_mm_set_pi16(INT16_C( -28274), INT16_C(   7246), INT16_C(  -5872), INT16_C(   6166)),
      simde_mm_set_pi16(INT16_C( -31184), INT16_C(   7719), INT16_C(  -6055), INT16_C(  -8342)),
      simde_mm_set_pi16(INT16_C( -29729), INT16_C(   7483), INT16_C(  -5963), INT16_C(  31680)) },
    { simde_mm_set_pi16(INT16_C( -16646), INT16_C( -13563), INT16_C(  14064), INT16_C( -16202)),
      simde_mm_set_pi16(INT16_C(  19063), INT16_C(   -668), INT16_C(  -1275), INT16_C(  21772)),
      simde_mm_set_pi16(INT16_C( -31559), INT16_C(  -7115), INT16_C( -26373), INT16_C( -29983)) },
    { simde_mm_set_pi16(INT16_C(  -9215), INT16_C( -23163), INT16_C( -24572), INT16_C(  -7019)),
      simde_mm_set_pi16(INT16_C(   5290), INT16_C( -15577), INT16_C( -28729), INT16_C(  11637)),
      simde_mm_set_pi16(INT16_C(  30806), INT16_C( -19370), INT16_C( -26650), INT16_C( -30459)) },
    { simde_mm_set_pi16(INT16_C( -12267), INT16_C(  24488), INT16_C(  17862), INT16_C(  10347)),
      simde_mm_set_pi16(INT16_C( -31762), INT16_C(  30984), INT16_C( -28618), INT16_C(  29735)),
      simde_mm_set_pi16(INT16_C( -22014), INT16_C(  27736), INT16_C(  27390), INT16_C(  20041)) },
    { simde_mm_set_pi16(INT16_C(   3850), INT16_C( -23709), INT16_C(   3464), INT16_C(  15024)),
      simde_mm_set_pi16(INT16_C(  -8288), INT16_C(  24652), INT16_C(   3944), INT16_C(   6895)),
      simde_mm_set_pi16(INT16_C(  30549), INT16_C( -32296), INT16_C(   3704), INT16_C(  10960)) },
    { simde_mm_set_pi16(INT16_C(  26638), INT16_C(  13283), INT16_C( -10177), INT16_C(  30168)),
      simde_mm_set_pi16(INT16_C(  21695), INT16_C(   8102), INT16_C(  20649), INT16_C( -16569)),
      simde_mm_set_pi16(INT16_C(  24167), INT16_C(  10693), INT16_C( -27532), INT16_C( -25968)) },
    { simde_mm_set_pi16(INT16_C(  -7286), INT16_C(  16651), INT16_C( -26954), INT16_C(  -7877)),
      simde_mm_set_pi16(INT16_C(  -2486), INT16_C(  17552), INT16_C(  17765), INT16_C(  29740)),
      simde_mm_set_pi16(INT16_C(  -4886), INT16_C(  17102), INT16_C(  28174), INT16_C( -21836)) },
    { simde_mm_set_pi16(INT16_C( -28013), INT16_C( -10086), INT16_C(  14371), INT16_C(  23831)),
      simde_mm_set_pi16(INT16_C(  29786), INT16_C( -11449), INT16_C(  -6751), INT16_C(  10534)),
      simde_mm_set_pi16(INT16_C( -31881), INT16_C( -10767), INT16_C( -28958), INT16_C(  17183)) },
    { simde_mm_set_pi16(INT16_C( -20820), INT16_C(  29304), INT16_C(   9027), INT16_C( -13018)),
      simde_mm_set_pi16(INT16_C( -16380), INT16_C(  26027), INT16_C(  11591), INT16_C(  29000)),
      simde_mm_set_pi16(INT16_C( -18600), INT16_C(  27666), INT16_C(  10309), INT16_C( -24777)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m64 r = simde_mm_avg_pu16(test_vec[i].a, test_vec[i].b);
    simde_mm_empty();
    simde_assert_m64_i16(r, ==, test_vec[i].r);
  }

  simde_mm_empty();
  return MUNIT_OK;
}

static MunitResult
test_simde_mm_avg_pu8(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m64 a;
    simde__m64 b;
    simde__m64 r;
  } test_vec[16] = {
    { simde_mm_set_pi8(0x91, 0xe1, 0x7e, 0x06, 0x12, 0x2f, 0xf1, 0x7f),
      simde_mm_set_pi8(0xd1, 0xfc, 0x30, 0x76, 0x81, 0xc7, 0x76, 0x03),
      simde_mm_set_pi8(0xb1, 0xef, 0x57, 0x3e, 0x4a, 0x7b, 0xb4, 0x41) },
    { simde_mm_set_pi8(0x28, 0xe2, 0xc6, 0x08, 0xc6, 0xf1, 0x56, 0x1f),
      simde_mm_set_pi8(0xe8, 0x95, 0x6e, 0x04, 0x09, 0x51, 0xd2, 0xb3),
      simde_mm_set_pi8(0x88, 0xbc, 0x9a, 0x06, 0x68, 0xa1, 0x94, 0x69) },
    { simde_mm_set_pi8(0xc0, 0xc2, 0x13, 0x4c, 0x22, 0x94, 0x45, 0x19),
      simde_mm_set_pi8(0x10, 0x0f, 0x38, 0xff, 0xa0, 0x9f, 0x10, 0xe4),
      simde_mm_set_pi8(0x68, 0x69, 0x26, 0xa6, 0x61, 0x9a, 0x2b, 0x7f) },
    { simde_mm_set_pi8(0x1e, 0x80, 0x8f, 0x2a, 0x75, 0x0c, 0xbe, 0xe8),
      simde_mm_set_pi8(0x8d, 0x95, 0x48, 0x12, 0x5d, 0xed, 0x02, 0xff),
      simde_mm_set_pi8(0x56, 0x8b, 0x6c, 0x1e, 0x69, 0x7d, 0x60, 0xf4) },
    { simde_mm_set_pi8(0xbd, 0x28, 0x85, 0x34, 0xe4, 0x5d, 0x36, 0xff),
      simde_mm_set_pi8(0x7f, 0xfe, 0x2a, 0xf8, 0xc0, 0x72, 0x81, 0xcf),
      simde_mm_set_pi8(0x9e, 0x93, 0x58, 0x96, 0xd2, 0x68, 0x5c, 0xe7) },
    { simde_mm_set_pi8(0x33, 0x41, 0x89, 0x25, 0xcd, 0x5d, 0xba, 0x64),
      simde_mm_set_pi8(0x25, 0xee, 0xc1, 0xdf, 0x79, 0x43, 0x6a, 0x26),
      simde_mm_set_pi8(0x2c, 0x98, 0xa5, 0x82, 0xa3, 0x50, 0x92, 0x45) },
    { simde_mm_set_pi8(0xf0, 0xd0, 0x63, 0x95, 0xd8, 0x9e, 0xdf, 0xc9),
      simde_mm_set_pi8(0x83, 0x4f, 0x74, 0x6b, 0x67, 0x46, 0x8d, 0x5f),
      simde_mm_set_pi8(0xba, 0x90, 0x6c, 0x80, 0xa0, 0x72, 0xb6, 0x94) },
    { simde_mm_set_pi8(0x88, 0xbf, 0x20, 0xd2, 0x99, 0x57, 0x5b, 0x7d),
      simde_mm_set_pi8(0x85, 0xce, 0x9f, 0xfc, 0xd5, 0xa1, 0xaf, 0xba),
      simde_mm_set_pi8(0x87, 0xc7, 0x60, 0xe7, 0xb7, 0x7c, 0x85, 0x9c) },
    { simde_mm_set_pi8(0x1d, 0x88, 0xc2, 0xc4, 0xcf, 0x95, 0x3c, 0x27),
      simde_mm_set_pi8(0x84, 0xd6, 0x51, 0xf3, 0x4d, 0x28, 0x78, 0x9e),
      simde_mm_set_pi8(0x51, 0xaf, 0x8a, 0xdc, 0x8e, 0x5f, 0x5a, 0x63) },
    { simde_mm_set_pi8(0x7f, 0x7e, 0x22, 0xe5, 0x13, 0x92, 0x9a, 0xfa),
      simde_mm_set_pi8(0x5b, 0xd1, 0xd0, 0x82, 0x0e, 0x8b, 0xcf, 0x12),
      simde_mm_set_pi8(0x6d, 0xa8, 0x79, 0xb4, 0x11, 0x8f, 0xb5, 0x86) },
    { simde_mm_set_pi8(0x98, 0x86, 0xf1, 0xd3, 0xbd, 0x99, 0x22, 0x5c),
      simde_mm_set_pi8(0x65, 0xb2, 0x34, 0x36, 0x68, 0x16, 0xbe, 0xf9),
      simde_mm_set_pi8(0x7f, 0x9c, 0x93, 0x85, 0x93, 0x58, 0x70, 0xab) },
    { simde_mm_set_pi8(0xb2, 0x3b, 0x6f, 0xf0, 0x4a, 0xd1, 0x08, 0xd4),
      simde_mm_set_pi8(0x53, 0xe9, 0x83, 0xf8, 0xeb, 0x24, 0xf5, 0xd7),
      simde_mm_set_pi8(0x83, 0x92, 0x79, 0xf4, 0x9b, 0x7b, 0x7f, 0xd6) },
    { simde_mm_set_pi8(0x83, 0x6a, 0x3b, 0x8e, 0x5a, 0xf6, 0x9c, 0xc5),
      simde_mm_set_pi8(0x1c, 0xa3, 0xe7, 0x5b, 0x87, 0xa5, 0x06, 0x06),
      simde_mm_set_pi8(0x50, 0x87, 0x91, 0x75, 0x71, 0xce, 0x51, 0x66) },
    { simde_mm_set_pi8(0x5c, 0x3a, 0xa5, 0x0f, 0x3f, 0x23, 0x6a, 0x18),
      simde_mm_set_pi8(0x1a, 0xe4, 0xee, 0xf4, 0xec, 0xbc, 0xc9, 0x4e),
      simde_mm_set_pi8(0x3b, 0x8f, 0xca, 0x82, 0x96, 0x70, 0x9a, 0x33) },
    { simde_mm_set_pi8(0xa3, 0x77, 0x5c, 0x7e, 0x51, 0x77, 0xf1, 0xf4),
      simde_mm_set_pi8(0x2d, 0x3c, 0xd3, 0xbc, 0x5e, 0x32, 0xa6, 0x64),
      simde_mm_set_pi8(0x68, 0x5a, 0x98, 0x9d, 0x58, 0x55, 0xcc, 0xac) },
    { simde_mm_set_pi8(0x92, 0xb8, 0xc8, 0xb8, 0x4d, 0x82, 0xc4, 0xff),
      simde_mm_set_pi8(0xf7, 0x65, 0xea, 0xb9, 0x1d, 0x6f, 0xf1, 0x53),
      simde_mm_set_pi8(0xc5, 0x8f, 0xd9, 0xb9, 0x35, 0x79, 0xdb, 0xa9) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m64 r = simde_mm_avg_pu8(test_vec[i].a, test_vec[i].b);
    simde_mm_empty();
    simde_assert_m64_u8(r, ==, test_vec[i].r);
  }

  simde_mm_empty();
  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpeq_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C( 1552099433), INT32_C(-1589398258), INT32_C(  -34553673), INT32_C(-1178885563)),
      simde_mm_set_epi32(INT32_C( 1552099433), INT32_C( 2025844073), INT32_C(  -34553673), INT32_C(-1178885563)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1766041281), INT32_C(-1662168132), INT32_C(  997624458), INT32_C(  980393671)),
      simde_mm_set_epi32(INT32_C( 1122334686), INT32_C(-1662168132), INT32_C( 1502327165), INT32_C( 1228800964)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1053856039), INT32_C(  189518021), INT32_C(-2093780846), INT32_C(  717465502)),
      simde_mm_set_epi32(INT32_C( 1053856039), INT32_C(  189518021), INT32_C( -730260217), INT32_C( 1570531267)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1211703701), INT32_C( 1792645956), INT32_C(  451470260), INT32_C(   72883505)),
      simde_mm_set_epi32(INT32_C(-1211703701), INT32_C( -873756537), INT32_C( 1700529530), INT32_C(   72883505)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 1123941451), INT32_C(-1499396317), INT32_C( -496690637), INT32_C(  857633319)),
      simde_mm_set_epi32(INT32_C(-1095910111), INT32_C(-1499396317), INT32_C(  -59751101), INT32_C(  -47922840)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1786351786), INT32_C(  618588335), INT32_C(-1648947504), INT32_C(-1160443929)),
      simde_mm_set_epi32(INT32_C( 1923711258), INT32_C(  775175059), INT32_C(-1633817987), INT32_C(  778628411)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 2132068337), INT32_C(  303020289), INT32_C(-1544950892), INT32_C(-2063582009)),
      simde_mm_set_epi32(INT32_C( 2132068337), INT32_C(  628767021), INT32_C(-1152607496), INT32_C(-1526726995)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 2000861670), INT32_C(-1296714344), INT32_C(  682422961), INT32_C( -767128199)),
      simde_mm_set_epi32(INT32_C( 2000861670), INT32_C(-1296714344), INT32_C(  682422961), INT32_C(-1784377677)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(         -1), INT32_C(          0)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpeq_ps(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpeq_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C(  975888251), INT32_C( 1037275787), INT32_C( -206927716), INT32_C( 1719930130)),
      simde_mm_set_epi32(INT32_C(-1683768293), INT32_C( 1037275787), INT32_C( -206927716), INT32_C( 1719930130)),
      simde_mm_set_epi32(INT32_C(  975888251), INT32_C( 1037275787), INT32_C( -206927716), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1524488806), INT32_C( -573553543), INT32_C(   42112282), INT32_C(  112731860)),
      simde_mm_set_epi32(INT32_C( 1557664843), INT32_C(  -39848297), INT32_C(   42112282), INT32_C(  112731860)),
      simde_mm_set_epi32(INT32_C(-1524488806), INT32_C( -573553543), INT32_C(   42112282), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 1378184574), INT32_C(-1915421318), INT32_C( -131047892), INT32_C( -798352758)),
      simde_mm_set_epi32(INT32_C( 1378184574), INT32_C(-1967398858), INT32_C( -131047892), INT32_C(-1869734720)),
      simde_mm_set_epi32(INT32_C( 1378184574), INT32_C(-1915421318), INT32_C( -131047892), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1981544552), INT32_C(  329373642), INT32_C( -104014353), INT32_C(-2092043281)),
      simde_mm_set_epi32(INT32_C( 2125102991), INT32_C(  978475086), INT32_C( 1426268882), INT32_C( -436875296)),
      simde_mm_set_epi32(INT32_C( 1981544552), INT32_C(  329373642), INT32_C( -104014353), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  600612499), INT32_C(-1490414099), INT32_C(-2094604589), INT32_C(  126748780)),
      simde_mm_set_epi32(INT32_C(  600612499), INT32_C(-1490414099), INT32_C(-2094604589), INT32_C(  126748780)),
      simde_mm_set_epi32(INT32_C(  600612499), INT32_C(-1490414099), INT32_C(-2094604589), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(   60156341), INT32_C( 1267841603), INT32_C(-1347475320), INT32_C(-1548408923)),
      simde_mm_set_epi32(INT32_C(   60156341), INT32_C( -387226227), INT32_C(  -76120938), INT32_C(-1548408923)),
      simde_mm_set_epi32(INT32_C(   60156341), INT32_C( 1267841603), INT32_C(-1347475320), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-2114878036), INT32_C(-1987807687), INT32_C( 1314812087), INT32_C( -402159797)),
      simde_mm_set_epi32(INT32_C(-2114878036), INT32_C(-1987807687), INT32_C( -580165908), INT32_C( -311736955)),
      simde_mm_set_epi32(INT32_C(-2114878036), INT32_C(-1987807687), INT32_C( 1314812087), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1498340262), INT32_C(  467089732), INT32_C( -352096361), INT32_C( -125007519)),
      simde_mm_set_epi32(INT32_C(-1919145018), INT32_C(  854922987), INT32_C(  767022324), INT32_C( -125007519)),
      simde_mm_set_epi32(INT32_C( 1498340262), INT32_C(  467089732), INT32_C( -352096361), INT32_C(         -1)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpeq_ss(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpge_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C(-2117681573), INT32_C( 1744824135), INT32_C(-1062100051), INT32_C( -758867207)),
      simde_mm_set_epi32(INT32_C(  447802409), INT32_C(-1805122446), INT32_C(-1062100051), INT32_C(  180201744)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(         -1), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  501801973), INT32_C( -437596087), INT32_C(   96926344), INT32_C( 1479688678)),
      simde_mm_set_epi32(INT32_C( 2057298249), INT32_C( -899804986), INT32_C( -711059659), INT32_C(  886835596)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  927826628), INT32_C(-1620708712), INT32_C(-2141396655), INT32_C(-1921942403)),
      simde_mm_set_epi32(INT32_C( -440885672), INT32_C(-1620708712), INT32_C( 2101230656), INT32_C(-1921942403)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1545828562), INT32_C( -731809091), INT32_C(-1137290929), INT32_C(  -77584541)),
      simde_mm_set_epi32(INT32_C( 1117766142), INT32_C(-2055588954), INT32_C(-1137290929), INT32_C(-1359346144)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(         -1), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1420198491), INT32_C( 1600056611), INT32_C( -347234499), INT32_C( 1584151154)),
      simde_mm_set_epi32(INT32_C(-1420198491), INT32_C( 1600056611), INT32_C( 1047775179), INT32_C(  673458453)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( -594070283), INT32_C( -270758299), INT32_C(  185236064), INT32_C(  607788733)),
      simde_mm_set_epi32(INT32_C( -310503835), INT32_C( 1875198957), INT32_C(  185236064), INT32_C(  290211410)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  275701977), INT32_C(-1310829701), INT32_C(-1932286124), INT32_C(  124706827)),
      simde_mm_set_epi32(INT32_C(  275701977), INT32_C( 2044408096), INT32_C(  371148478), INT32_C( 1787635053)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  335124996), INT32_C(-2113521475), INT32_C(  960990723), INT32_C( -969567969)),
      simde_mm_set_epi32(INT32_C(-1587045050), INT32_C(-2113521475), INT32_C( -168625313), INT32_C( 1040258918)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(         -1), INT32_C(          0)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpge_ps(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpge_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C(-1012979392), INT32_C(-1453212125), INT32_C(  815083508), INT32_C( 1351772430)),
      simde_mm_set_epi32(INT32_C( 1169877184), INT32_C(-1037467546), INT32_C( 1421795564), INT32_C(  905862977)),
      simde_mm_set_epi32(INT32_C(-1012979392), INT32_C(-1453212125), INT32_C(  815083508), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( -324333154), INT32_C( 1314427040), INT32_C( -483618994), INT32_C( 1502460142)),
      simde_mm_set_epi32(INT32_C( -202797776), INT32_C(   94174629), INT32_C(  926181510), INT32_C( 1502460142)),
      simde_mm_set_epi32(INT32_C( -324333154), INT32_C( 1314427040), INT32_C( -483618994), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  467112718), INT32_C(-1304161813), INT32_C(-1064952269), INT32_C(  631164600)),
      simde_mm_set_epi32(INT32_C(  467112718), INT32_C(-1304161813), INT32_C(  200476027), INT32_C( 1663275417)),
      simde_mm_set_epi32(INT32_C(  467112718), INT32_C(-1304161813), INT32_C(-1064952269), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -347696391), INT32_C(  148936949), INT32_C( 1398613274), INT32_C( 1250010386)),
      simde_mm_set_epi32(INT32_C( -347696391), INT32_C(  148936949), INT32_C( 1202050063), INT32_C( 1250010386)),
      simde_mm_set_epi32(INT32_C( -347696391), INT32_C(  148936949), INT32_C( 1398613274), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1684279233), INT32_C(-2118983098), INT32_C(  160153353), INT32_C( -113728462)),
      simde_mm_set_epi32(INT32_C(-1684279233), INT32_C(-2118983098), INT32_C(  160153353), INT32_C( 1908868579)),
      simde_mm_set_epi32(INT32_C(-1684279233), INT32_C(-2118983098), INT32_C(  160153353), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1844989631), INT32_C( 1725503201), INT32_C(  822658308), INT32_C( -958604095)),
      simde_mm_set_epi32(INT32_C( 1098943561), INT32_C( 1725503201), INT32_C( -755708322), INT32_C( -958604095)),
      simde_mm_set_epi32(INT32_C(-1844989631), INT32_C( 1725503201), INT32_C(  822658308), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  375717031), INT32_C( -882753921), INT32_C( 1130912475), INT32_C( 2045186948)),
      simde_mm_set_epi32(INT32_C(  375717031), INT32_C( -882753921), INT32_C( -800937725), INT32_C( 2045186948)),
      simde_mm_set_epi32(INT32_C(  375717031), INT32_C( -882753921), INT32_C( 1130912475), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  602871888), INT32_C(  470458702), INT32_C(-1169608009), INT32_C( -214741679)),
      simde_mm_set_epi32(INT32_C(  602871888), INT32_C( -493719927), INT32_C(-1169608009), INT32_C( 2044474163)),
      simde_mm_set_epi32(INT32_C(  602871888), INT32_C(  470458702), INT32_C(-1169608009), INT32_C(          0)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpge_ss(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpgt_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C( 1960669177), INT32_C(-1800221672), INT32_C( -201138064), INT32_C( 1882570539)),
      simde_mm_set_epi32(INT32_C( 1960669177), INT32_C( 1636503467), INT32_C(  379370104), INT32_C( 1549588428)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 1497868169), INT32_C( 1587819932), INT32_C(   11597982), INT32_C(  367113544)),
      simde_mm_set_epi32(INT32_C( 1497868169), INT32_C( 1587819932), INT32_C(   11597982), INT32_C(-2087685948)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( -420177326), INT32_C( 1458568188), INT32_C( 1173908260), INT32_C(-1290366123)),
      simde_mm_set_epi32(INT32_C(-1452540627), INT32_C(-1072533390), INT32_C( 1463315378), INT32_C(  944710330)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1987295390), INT32_C( -783471208), INT32_C( 1614612869), INT32_C(  941455851)),
      simde_mm_set_epi32(INT32_C(  776864087), INT32_C( -275776600), INT32_C( 1614612869), INT32_C(  941455851)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -802004143), INT32_C(-2136990592), INT32_C( 1617943513), INT32_C( 1268400008)),
      simde_mm_set_epi32(INT32_C( -802004143), INT32_C(-2136990592), INT32_C( 1458980297), INT32_C( 1268400008)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(         -1), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1974256896), INT32_C( 1350058685), INT32_C(-1649890899), INT32_C(-2052839662)),
      simde_mm_set_epi32(INT32_C(-1974256896), INT32_C( 1388826354), INT32_C(-2033282795), INT32_C(-2052839662)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(   42472091), INT32_C( 2006715626), INT32_C(   18778978), INT32_C( 1523053306)),
      simde_mm_set_epi32(INT32_C(   42472091), INT32_C(-1636528069), INT32_C(   18778978), INT32_C(  737106093)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 1244568026), INT32_C(-1381724018), INT32_C( -492895509), INT32_C( -783840348)),
      simde_mm_set_epi32(INT32_C(  567191695), INT32_C( -729265936), INT32_C( -492895509), INT32_C( -783840348)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpgt_ps(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpgt_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C( 1869056804), INT32_C(-1753986411), INT32_C( -585204320), INT32_C(-1910961055)),
      simde_mm_set_epi32(INT32_C( 1869056804), INT32_C(-1102496477), INT32_C(  -20892909), INT32_C(  -63499356)),
      simde_mm_set_epi32(INT32_C( 1869056804), INT32_C(-1753986411), INT32_C( -585204320), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  528987825), INT32_C( -236154839), INT32_C( 1847822202), INT32_C( 1073580784)),
      simde_mm_set_epi32(INT32_C(  301802145), INT32_C( -236154839), INT32_C( 1847822202), INT32_C( 1073580784)),
      simde_mm_set_epi32(INT32_C(  528987825), INT32_C( -236154839), INT32_C( 1847822202), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -453663293), INT32_C(  374222760), INT32_C(  660253560), INT32_C(-1094975358)),
      simde_mm_set_epi32(INT32_C(  922033710), INT32_C(-1385400175), INT32_C(  281683050), INT32_C(-1094975358)),
      simde_mm_set_epi32(INT32_C( -453663293), INT32_C(  374222760), INT32_C(  660253560), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(   56611761), INT32_C( -294357766), INT32_C(  221827642), INT32_C(-1086961889)),
      simde_mm_set_epi32(INT32_C(-2059704292), INT32_C( -294357766), INT32_C(-1642431615), INT32_C(-1453505032)),
      simde_mm_set_epi32(INT32_C(   56611761), INT32_C( -294357766), INT32_C(  221827642), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -677567670), INT32_C(-1065179289), INT32_C( 1230662601), INT32_C( 1022052917)),
      simde_mm_set_epi32(INT32_C( -677567670), INT32_C(  692939036), INT32_C( 1108906800), INT32_C(-2115601689)),
      simde_mm_set_epi32(INT32_C( -677567670), INT32_C(-1065179289), INT32_C( 1230662601), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  658893489), INT32_C( -592874075), INT32_C(   90769968), INT32_C( 1780967347)),
      simde_mm_set_epi32(INT32_C( -745618317), INT32_C( -592874075), INT32_C(   90769968), INT32_C( 1901683344)),
      simde_mm_set_epi32(INT32_C(  658893489), INT32_C( -592874075), INT32_C(   90769968), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  596908542), INT32_C( -725878217), INT32_C(   43862031), INT32_C(  120397305)),
      simde_mm_set_epi32(INT32_C( 1105546030), INT32_C( -725878217), INT32_C(   43862031), INT32_C(-1679076026)),
      simde_mm_set_epi32(INT32_C(  596908542), INT32_C( -725878217), INT32_C(   43862031), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-2095835916), INT32_C( 2093817071), INT32_C(-1700253832), INT32_C( 1213385208)),
      simde_mm_set_epi32(INT32_C(-1122300334), INT32_C( 1314746582), INT32_C(-1700253832), INT32_C( 1213385208)),
      simde_mm_set_epi32(INT32_C(-2095835916), INT32_C( 2093817071), INT32_C(-1700253832), INT32_C(          0)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpgt_ss(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmple_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C(-1214095902), INT32_C(  -66909402), INT32_C( -553169346), INT32_C( 1655881585)),
      simde_mm_set_epi32(INT32_C( 1411879436), INT32_C( 1800787365), INT32_C( -553169346), INT32_C( -934991244)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(         -1), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  935354519), INT32_C( -241132058), INT32_C( 1879436355), INT32_C( -995448835)),
      simde_mm_set_epi32(INT32_C(  935354519), INT32_C( -241132058), INT32_C(  778854493), INT32_C( 1939053113)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-2000937674), INT32_C(  926691548), INT32_C( 1566199240), INT32_C(-2071387770)),
      simde_mm_set_epi32(INT32_C(  -33682990), INT32_C(  926691548), INT32_C( -862341564), INT32_C(-2071387770)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 1400009830), INT32_C( 2074443710), INT32_C(-1988154474), INT32_C(-1285973673)),
      simde_mm_set_epi32(INT32_C( 1431717555), INT32_C( 2074443710), INT32_C( -236063022), INT32_C(-1285973673)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( -969028248), INT32_C( 1961285965), INT32_C( 2034451315), INT32_C(  982408470)),
      simde_mm_set_epi32(INT32_C( -969028248), INT32_C(  737591133), INT32_C( 2034451315), INT32_C( 1972820242)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  689515799), INT32_C( 1226329239), INT32_C( -601248060), INT32_C( -246025007)),
      simde_mm_set_epi32(INT32_C(  689515799), INT32_C( 1567083199), INT32_C( -315367605), INT32_C(  895186883)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 1299136382), INT32_C(   66630204), INT32_C(    5361190), INT32_C( 1189577124)),
      simde_mm_set_epi32(INT32_C(-1645066809), INT32_C(   66630204), INT32_C( -985693313), INT32_C( 1189577124)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( -861186400), INT32_C(-1832454806), INT32_C(  -43975819), INT32_C(   67490279)),
      simde_mm_set_epi32(INT32_C( -259498670), INT32_C(-1523721259), INT32_C(  -43975819), INT32_C(  292054476)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(         -1), INT32_C(         -1)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmple_ps(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmple_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C(  577834131), INT32_C(  125783617), INT32_C(-1878005293), INT32_C(-2039280607)),
      simde_mm_set_epi32(INT32_C(  577834131), INT32_C(  125783617), INT32_C(-1878005293), INT32_C( 2032820947)),
      simde_mm_set_epi32(INT32_C(  577834131), INT32_C(  125783617), INT32_C(-1878005293), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1229544726), INT32_C(-1899015779), INT32_C( 1583449198), INT32_C(-1177374746)),
      simde_mm_set_epi32(INT32_C(  -10568623), INT32_C( 2105302200), INT32_C( 1583449198), INT32_C(  113127759)),
      simde_mm_set_epi32(INT32_C(-1229544726), INT32_C(-1899015779), INT32_C( 1583449198), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1403164448), INT32_C(-1420499873), INT32_C(-1053362140), INT32_C(-1673889705)),
      simde_mm_set_epi32(INT32_C(-1403164448), INT32_C(-1420499873), INT32_C(-1053362140), INT32_C(-1409095555)),
      simde_mm_set_epi32(INT32_C(-1403164448), INT32_C(-1420499873), INT32_C(-1053362140), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1464154482), INT32_C( -512313078), INT32_C(-1671698744), INT32_C(-1338032665)),
      simde_mm_set_epi32(INT32_C(-1464154482), INT32_C(  350929661), INT32_C(-1095578259), INT32_C( -137690799)),
      simde_mm_set_epi32(INT32_C(-1464154482), INT32_C( -512313078), INT32_C(-1671698744), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -876240227), INT32_C(  638830515), INT32_C( 1194125228), INT32_C( 1201799459)),
      simde_mm_set_epi32(INT32_C(  370717124), INT32_C(  575915100), INT32_C(  323333140), INT32_C(-1701764718)),
      simde_mm_set_epi32(INT32_C( -876240227), INT32_C(  638830515), INT32_C( 1194125228), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1870075868), INT32_C( -538093027), INT32_C( 1796667049), INT32_C(  944394189)),
      simde_mm_set_epi32(INT32_C( 1870075868), INT32_C(-1718307451), INT32_C( 1796667049), INT32_C(  944394189)),
      simde_mm_set_epi32(INT32_C( 1870075868), INT32_C( -538093027), INT32_C( 1796667049), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 1976935464), INT32_C(-1126118718), INT32_C(  992307422), INT32_C( 1933654831)),
      simde_mm_set_epi32(INT32_C(  980618308), INT32_C(-1126118718), INT32_C(  992307422), INT32_C( 1933654831)),
      simde_mm_set_epi32(INT32_C( 1976935464), INT32_C(-1126118718), INT32_C(  992307422), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  957949500), INT32_C(  241498261), INT32_C(-2057282883), INT32_C(  160259126)),
      simde_mm_set_epi32(INT32_C(  945346495), INT32_C(  241498261), INT32_C(-2057282883), INT32_C(  160259126)),
      simde_mm_set_epi32(INT32_C(  957949500), INT32_C(  241498261), INT32_C(-2057282883), INT32_C(         -1)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmple_ss(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmplt_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C( -864121404), INT32_C(  820258235), INT32_C(-1696337178), INT32_C(-1482432629)),
      simde_mm_set_epi32(INT32_C( -864121404), INT32_C(  820258235), INT32_C(-1696337178), INT32_C(-1482432629)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1757897801), INT32_C(-1139623844), INT32_C(  396231785), INT32_C( 1433495183)),
      simde_mm_set_epi32(INT32_C(-1757897801), INT32_C( 1640940421), INT32_C(  923198702), INT32_C( 1433495183)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(         -1), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1329593143), INT32_C( 1809390238), INT32_C( 1760230809), INT32_C( -549294944)),
      simde_mm_set_epi32(INT32_C( 1011105980), INT32_C( 1809390238), INT32_C( 1760230809), INT32_C( -549294944)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1918857994), INT32_C(-1864521589), INT32_C(  155537477), INT32_C( 1666430711)),
      simde_mm_set_epi32(INT32_C(-1918857994), INT32_C(-1864521589), INT32_C(  155537477), INT32_C(  735305870)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  197471455), INT32_C(   -1555852), INT32_C(-1752379132), INT32_C(  240472065)),
      simde_mm_set_epi32(INT32_C(  579332444), INT32_C(   -1555852), INT32_C(-1752379132), INT32_C(  240472065)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1086999122), INT32_C( 1309092278), INT32_C(  365604292), INT32_C(-1138767432)),
      simde_mm_set_epi32(INT32_C( 1086999122), INT32_C( -886110099), INT32_C(  219975772), INT32_C(-1138767432)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1954260003), INT32_C( 1696592095), INT32_C( 2078494375), INT32_C(  650856359)),
      simde_mm_set_epi32(INT32_C( 1954260003), INT32_C( 2098551061), INT32_C( 2078494375), INT32_C(  650856359)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1672271308), INT32_C(-1503351515), INT32_C(-1874414686), INT32_C(  435117874)),
      simde_mm_set_epi32(INT32_C(-1672271308), INT32_C(-1630328519), INT32_C(-1874414686), INT32_C(  435117874)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmplt_ps(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmplt_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C( 1231873664), INT32_C( 1036602093), INT32_C( -933713727), INT32_C( 2093671800)),
      simde_mm_set_epi32(INT32_C( 1231873664), INT32_C( 1036602093), INT32_C(  357806524), INT32_C( 2093671800)),
      simde_mm_set_epi32(INT32_C( 1231873664), INT32_C( 1036602093), INT32_C( -933713727), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  793534664), INT32_C(  376969487), INT32_C(  721355231), INT32_C( 1023311191)),
      simde_mm_set_epi32(INT32_C(-1925750607), INT32_C(-1255391862), INT32_C(  721355231), INT32_C( 2118013683)),
      simde_mm_set_epi32(INT32_C(  793534664), INT32_C(  376969487), INT32_C(  721355231), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1783953523), INT32_C( 1823399536), INT32_C( 1402581392), INT32_C(-1166205233)),
      simde_mm_set_epi32(INT32_C( 1218188972), INT32_C(-1327532162), INT32_C(-1033869830), INT32_C(-1166205233)),
      simde_mm_set_epi32(INT32_C(-1783953523), INT32_C( 1823399536), INT32_C( 1402581392), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  223867160), INT32_C(-1451057003), INT32_C( 2044754038), INT32_C(  605919704)),
      simde_mm_set_epi32(INT32_C(  223867160), INT32_C(-1334167478), INT32_C( 2044754038), INT32_C(  605919704)),
      simde_mm_set_epi32(INT32_C(  223867160), INT32_C(-1451057003), INT32_C( 2044754038), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  951875437), INT32_C( 1770306939), INT32_C(-1002738540), INT32_C( 1475715403)),
      simde_mm_set_epi32(INT32_C(  935489339), INT32_C( 1770306939), INT32_C(-1002738540), INT32_C(  867597907)),
      simde_mm_set_epi32(INT32_C(  951875437), INT32_C( 1770306939), INT32_C(-1002738540), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -400845231), INT32_C(-1931717289), INT32_C(  187179269), INT32_C( 1693770206)),
      simde_mm_set_epi32(INT32_C( -400845231), INT32_C( 1409986791), INT32_C(  867295140), INT32_C( 1693770206)),
      simde_mm_set_epi32(INT32_C( -400845231), INT32_C(-1931717289), INT32_C(  187179269), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1979013327), INT32_C( 1144749620), INT32_C( -915616748), INT32_C(-1688776910)),
      simde_mm_set_epi32(INT32_C( 1979013327), INT32_C( -522411693), INT32_C( -915616748), INT32_C(-1688776910)),
      simde_mm_set_epi32(INT32_C( 1979013327), INT32_C( 1144749620), INT32_C( -915616748), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1335736202), INT32_C(  434505386), INT32_C( 1706493603), INT32_C( 1636681283)),
      simde_mm_set_epi32(INT32_C(-1335736202), INT32_C(  434505386), INT32_C( 1706493603), INT32_C(  754767105)),
      simde_mm_set_epi32(INT32_C(-1335736202), INT32_C(  434505386), INT32_C( 1706493603), INT32_C(          0)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmplt_ss(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpneq_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C(  532215048), INT32_C(  130142512), INT32_C(-2127364592), INT32_C( 1091895543)),
      simde_mm_set_epi32(INT32_C(  532215048), INT32_C(  130142512), INT32_C(-2127364592), INT32_C( 1091895543)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1046985290), INT32_C(-1731603099), INT32_C(  610751781), INT32_C( 1112478841)),
      simde_mm_set_epi32(INT32_C(-1046985290), INT32_C(  622608062), INT32_C(-1239630421), INT32_C(-1197092706)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 1281269663), INT32_C(   10288009), INT32_C(  351928123), INT32_C(   68696372)),
      simde_mm_set_epi32(INT32_C( 1281269663), INT32_C( 1321240790), INT32_C( 1472147540), INT32_C(  221255688)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 2082220393), INT32_C(  853275580), INT32_C( 1554259301), INT32_C( 1874019211)),
      simde_mm_set_epi32(INT32_C( 2082220393), INT32_C(  853275580), INT32_C( -464704033), INT32_C( 1874019211)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(         -1), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1607341285), INT32_C(  801224090), INT32_C(  790655246), INT32_C( 1500708888)),
      simde_mm_set_epi32(INT32_C( 1686421224), INT32_C( -848290084), INT32_C( 1521902022), INT32_C(  974050035)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  209990814), INT32_C( 2072268417), INT32_C( -198384983), INT32_C( -634248146)),
      simde_mm_set_epi32(INT32_C(-1549476809), INT32_C( -306787617), INT32_C( -198384983), INT32_C(  461233263)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 2046647956), INT32_C(  403639600), INT32_C( -500490027), INT32_C(   21369987)),
      simde_mm_set_epi32(INT32_C( 2046647956), INT32_C( 1922885083), INT32_C( -500490027), INT32_C(  437329742)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1569313979), INT32_C(-1644454001), INT32_C(-1155267042), INT32_C( -840725054)),
      simde_mm_set_epi32(INT32_C(-1569313979), INT32_C(-1403674957), INT32_C(-1155267042), INT32_C( -840725054)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpneq_ps(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpneq_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C( -360810495), INT32_C(-1592808236), INT32_C(  688144434), INT32_C( 1729628761)),
      simde_mm_set_epi32(INT32_C(-1174522920), INT32_C(  592092453), INT32_C(  688144434), INT32_C(-1493258549)),
      simde_mm_set_epi32(INT32_C( -360810495), INT32_C(-1592808236), INT32_C(  688144434), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 1897806214), INT32_C( -836251000), INT32_C( 1879840962), INT32_C( 1741986879)),
      simde_mm_set_epi32(INT32_C( 1897806214), INT32_C( 1708676885), INT32_C( -829246870), INT32_C( 1436108855)),
      simde_mm_set_epi32(INT32_C( 1897806214), INT32_C( -836251000), INT32_C( 1879840962), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1752241902), INT32_C(-2071998685), INT32_C( -472663454), INT32_C( -626361712)),
      simde_mm_set_epi32(INT32_C( 2013622797), INT32_C(-2071998685), INT32_C(-1052835841), INT32_C( -626361712)),
      simde_mm_set_epi32(INT32_C(-1752241902), INT32_C(-2071998685), INT32_C( -472663454), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1426139517), INT32_C(  408854398), INT32_C( 1288359889), INT32_C(-1580268734)),
      simde_mm_set_epi32(INT32_C(-1426139517), INT32_C(  408854398), INT32_C( 1288359889), INT32_C(-1580268734)),
      simde_mm_set_epi32(INT32_C(-1426139517), INT32_C(  408854398), INT32_C( 1288359889), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1447450397), INT32_C(  498817306), INT32_C(   48851994), INT32_C(-1751968553)),
      simde_mm_set_epi32(INT32_C( -142346191), INT32_C(-1158316806), INT32_C( 1062598271), INT32_C( -267767173)),
      simde_mm_set_epi32(INT32_C( 1447450397), INT32_C(  498817306), INT32_C(   48851994), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  777179606), INT32_C( 1922869133), INT32_C(-2009725956), INT32_C( 2047200466)),
      simde_mm_set_epi32(INT32_C(  778691217), INT32_C(  807594000), INT32_C(-2009725956), INT32_C( 2047200466)),
      simde_mm_set_epi32(INT32_C(  777179606), INT32_C( 1922869133), INT32_C(-2009725956), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1844022768), INT32_C(  257130431), INT32_C( 1749772003), INT32_C(-1329350641)),
      simde_mm_set_epi32(INT32_C( 1038626596), INT32_C(  257130431), INT32_C( 1749772003), INT32_C( 1329214996)),
      simde_mm_set_epi32(INT32_C(-1844022768), INT32_C(  257130431), INT32_C( 1749772003), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  381813049), INT32_C( -254051472), INT32_C(-1103075616), INT32_C(-1510860170)),
      simde_mm_set_epi32(INT32_C(-1489319643), INT32_C( -254051472), INT32_C(-1103075616), INT32_C( -696173092)),
      simde_mm_set_epi32(INT32_C(  381813049), INT32_C( -254051472), INT32_C(-1103075616), INT32_C(         -1)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpneq_ss(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpnge_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C( 1888650448), INT32_C( 1952380142), INT32_C( -418024326), INT32_C(  954406307)),
      simde_mm_set_epi32(INT32_C( 1888650448), INT32_C( 1952380142), INT32_C( 1530340429), INT32_C(  315056976)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(         -1), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -675143251), INT32_C(   71969785), INT32_C( -921847024), INT32_C(   22067797)),
      simde_mm_set_epi32(INT32_C(   72561527), INT32_C( 1005464844), INT32_C( -921847024), INT32_C(   22067797)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-2139620548), INT32_C(-1593174748), INT32_C( -604878160), INT32_C(-1601463094)),
      simde_mm_set_epi32(INT32_C(-2139620548), INT32_C(  492282745), INT32_C( -604878160), INT32_C( 1512309058)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1508734054), INT32_C(  370976534), INT32_C( 2066803930), INT32_C( -701634011)),
      simde_mm_set_epi32(INT32_C( 1927420129), INT32_C(  370976534), INT32_C( 2066803930), INT32_C( -701634011)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  297952699), INT32_C( 2014428800), INT32_C( -102481167), INT32_C( 2086329997)),
      simde_mm_set_epi32(INT32_C( 1611455963), INT32_C( 2014428800), INT32_C( -102481167), INT32_C(-2102996894)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -649625825), INT32_C( -985868832), INT32_C( 1191775411), INT32_C(  442065450)),
      simde_mm_set_epi32(INT32_C( -649625825), INT32_C( 1060858907), INT32_C( -423355707), INT32_C(-2066634583)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1343578941), INT32_C(  107928701), INT32_C(-2043979132), INT32_C( 1583468645)),
      simde_mm_set_epi32(INT32_C( -744928720), INT32_C(-1908186697), INT32_C(-2043979132), INT32_C(  -98145208)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1297942286), INT32_C( 1312694015), INT32_C(-1476884375), INT32_C( 1085937493)),
      simde_mm_set_epi32(INT32_C( 1297942286), INT32_C( 1312694015), INT32_C(-1476884375), INT32_C(  811271927)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(          0)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpnge_ps(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpnge_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C(  106734352), INT32_C(-1177695143), INT32_C( 1251867427), INT32_C(-1113436842)),
      simde_mm_set_epi32(INT32_C(-2016761807), INT32_C(-1602250813), INT32_C(-1502543805), INT32_C(-1113436842)),
      simde_mm_set_epi32(INT32_C(  106734352), INT32_C(-1177695143), INT32_C( 1251867427), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -992010454), INT32_C(  875520944), INT32_C( -773626008), INT32_C(  297000538)),
      simde_mm_set_epi32(INT32_C( -992010454), INT32_C(  875520944), INT32_C(-1960267017), INT32_C(  267267504)),
      simde_mm_set_epi32(INT32_C( -992010454), INT32_C(  875520944), INT32_C( -773626008), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1423404927), INT32_C( -273294615), INT32_C( 1115052821), INT32_C( 1762207327)),
      simde_mm_set_epi32(INT32_C(-1423404927), INT32_C(-2004658753), INT32_C( 1115052821), INT32_C(-1955515955)),
      simde_mm_set_epi32(INT32_C(-1423404927), INT32_C( -273294615), INT32_C( 1115052821), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  630570300), INT32_C( 1480720620), INT32_C(-2111634368), INT32_C(  381139912)),
      simde_mm_set_epi32(INT32_C(   91445288), INT32_C( 1480720620), INT32_C(-2111634368), INT32_C(  381139912)),
      simde_mm_set_epi32(INT32_C(  630570300), INT32_C( 1480720620), INT32_C(-2111634368), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-2140660333), INT32_C( -873371869), INT32_C(-1549425333), INT32_C( 1861722511)),
      simde_mm_set_epi32(INT32_C(-1410997069), INT32_C( -873371869), INT32_C(-1549425333), INT32_C( -149520118)),
      simde_mm_set_epi32(INT32_C(-2140660333), INT32_C( -873371869), INT32_C(-1549425333), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1896750546), INT32_C(  577432699), INT32_C( -255384715), INT32_C(  717895922)),
      simde_mm_set_epi32(INT32_C( 1233088565), INT32_C(-1957258082), INT32_C( 1913240068), INT32_C(-1565227266)),
      simde_mm_set_epi32(INT32_C(-1896750546), INT32_C(  577432699), INT32_C( -255384715), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1577990746), INT32_C( 1577580126), INT32_C( -298703690), INT32_C(  397262282)),
      simde_mm_set_epi32(INT32_C(-2105366944), INT32_C( 1950034916), INT32_C( -298703690), INT32_C( 1632078127)),
      simde_mm_set_epi32(INT32_C( 1577990746), INT32_C( 1577580126), INT32_C( -298703690), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1398991860), INT32_C( -868057052), INT32_C(  897239402), INT32_C(  478239630)),
      simde_mm_set_epi32(INT32_C(-1398991860), INT32_C(-1311190075), INT32_C(-1018578616), INT32_C(-1096405398)),
      simde_mm_set_epi32(INT32_C(-1398991860), INT32_C( -868057052), INT32_C(  897239402), INT32_C(          0)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpnge_ss(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpngt_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C(-1123406769), INT32_C( -826046923), INT32_C( 1391137025), INT32_C(  225408485)),
      simde_mm_set_epi32(INT32_C(-1123406769), INT32_C( 1170001915), INT32_C( -443741078), INT32_C(  225408485)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  919359233), INT32_C(  121568041), INT32_C( 1196900513), INT32_C(-2068491895)),
      simde_mm_set_epi32(INT32_C( -229431781), INT32_C(  121568041), INT32_C( -495278912), INT32_C(-2068491895)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1546227894), INT32_C(  -80396000), INT32_C( 1691185200), INT32_C(-1185796610)),
      simde_mm_set_epi32(INT32_C(  930129185), INT32_C(  460197193), INT32_C( 1461329142), INT32_C(  -46416365)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  801509529), INT32_C(  166196414), INT32_C( 1533458041), INT32_C(-2125985376)),
      simde_mm_set_epi32(INT32_C(  801509529), INT32_C(  166196414), INT32_C(  170298010), INT32_C( -614823370)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -383558878), INT32_C( 1739426797), INT32_C(-1413157826), INT32_C(-1057748324)),
      simde_mm_set_epi32(INT32_C( -383558878), INT32_C(  561929576), INT32_C(-1413157826), INT32_C(-1057748324)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  303550578), INT32_C(  722713191), INT32_C(-1131128469), INT32_C( -492258520)),
      simde_mm_set_epi32(INT32_C( -260529955), INT32_C(  722713191), INT32_C(-1131128469), INT32_C( -492258520)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( -760270686), INT32_C(-1716923162), INT32_C(  225019073), INT32_C(-2108522398)),
      simde_mm_set_epi32(INT32_C( -103603972), INT32_C(  226706263), INT32_C(  225019073), INT32_C( -506819004)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(         -1), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(   44043689), INT32_C( 1891018514), INT32_C(  705898855), INT32_C( -899283598)),
      simde_mm_set_epi32(INT32_C(   44043689), INT32_C( 1891018514), INT32_C( -639205394), INT32_C( -899283598)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpngt_ps(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpngt_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C(  612778091), INT32_C( 1832771785), INT32_C( -622594968), INT32_C(  910095126)),
      simde_mm_set_epi32(INT32_C(  458628115), INT32_C(  689947875), INT32_C( -348460368), INT32_C(  227312121)),
      simde_mm_set_epi32(INT32_C(  612778091), INT32_C( 1832771785), INT32_C( -622594968), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1078825247), INT32_C(-1403039755), INT32_C( 1610931740), INT32_C(  175799384)),
      simde_mm_set_epi32(INT32_C( -601333689), INT32_C(  307533582), INT32_C( 1610931740), INT32_C(  175799384)),
      simde_mm_set_epi32(INT32_C(-1078825247), INT32_C(-1403039755), INT32_C( 1610931740), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( -473381137), INT32_C(-1853677516), INT32_C(    4434085), INT32_C(  177860717)),
      simde_mm_set_epi32(INT32_C( -473381137), INT32_C(-1429612088), INT32_C( 1023613916), INT32_C(-1567281359)),
      simde_mm_set_epi32(INT32_C( -473381137), INT32_C(-1853677516), INT32_C(    4434085), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1461690128), INT32_C( -186641995), INT32_C(-1876360628), INT32_C(  720924768)),
      simde_mm_set_epi32(INT32_C( 1256087859), INT32_C( -186641995), INT32_C(-1876360628), INT32_C(  720924768)),
      simde_mm_set_epi32(INT32_C(-1461690128), INT32_C( -186641995), INT32_C(-1876360628), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 2056917783), INT32_C( -907049588), INT32_C( 1171345475), INT32_C(-1386896922)),
      simde_mm_set_epi32(INT32_C( 2056917783), INT32_C( -907049588), INT32_C( 2132166674), INT32_C(-1386896922)),
      simde_mm_set_epi32(INT32_C( 2056917783), INT32_C( -907049588), INT32_C( 1171345475), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  333966351), INT32_C( 1310053758), INT32_C( 1466732857), INT32_C(-1121891710)),
      simde_mm_set_epi32(INT32_C(  333966351), INT32_C( 1310053758), INT32_C( 1572556388), INT32_C(-1121891710)),
      simde_mm_set_epi32(INT32_C(  333966351), INT32_C( 1310053758), INT32_C( 1466732857), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  860690730), INT32_C(-1667003350), INT32_C(-1337189707), INT32_C( 1913729946)),
      simde_mm_set_epi32(INT32_C(  860690730), INT32_C(-1667003350), INT32_C( -470676019), INT32_C( -895735920)),
      simde_mm_set_epi32(INT32_C(  860690730), INT32_C(-1667003350), INT32_C(-1337189707), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1440878372), INT32_C( 1637362808), INT32_C( -984574857), INT32_C(  760105912)),
      simde_mm_set_epi32(INT32_C(  511504303), INT32_C( 1637362808), INT32_C( -984574857), INT32_C(  760105912)),
      simde_mm_set_epi32(INT32_C( 1440878372), INT32_C( 1637362808), INT32_C( -984574857), INT32_C(         -1)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpngt_ss(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpnle_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C( 2084445991), INT32_C(-1270940367), INT32_C( -137999497), INT32_C( 1274474466)),
      simde_mm_set_epi32(INT32_C( 2084445991), INT32_C( 1003404174), INT32_C( -137999497), INT32_C( 1274474466)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1073857581), INT32_C(  297220883), INT32_C(-1118323043), INT32_C(-2009485771)),
      simde_mm_set_epi32(INT32_C(-1073857581), INT32_C(-1478175756), INT32_C(-1118323043), INT32_C(-2009485771)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1368751028), INT32_C( 1620987038), INT32_C( 1536890594), INT32_C( -889040581)),
      simde_mm_set_epi32(INT32_C( 2024237287), INT32_C( 1620987038), INT32_C(  809803938), INT32_C( -761648004)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  965653593), INT32_C( 1368656732), INT32_C(-2063999421), INT32_C( -170225342)),
      simde_mm_set_epi32(INT32_C(  965653593), INT32_C( 1368656732), INT32_C(-1629280761), INT32_C( -170225342)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(         -1), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1924982985), INT32_C(  388770010), INT32_C( 1672412411), INT32_C(-1198604482)),
      simde_mm_set_epi32(INT32_C( -502648824), INT32_C( 1358695998), INT32_C(-1024782381), INT32_C(-1198604482)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(         -1), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1453860753), INT32_C(-1007750264), INT32_C( 2035232495), INT32_C(-1933616133)),
      simde_mm_set_epi32(INT32_C(-1419270643), INT32_C(-2082779635), INT32_C(-1193599699), INT32_C( 1986918969)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(         -1), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  731183446), INT32_C( -811315535), INT32_C( -739483356), INT32_C(  137475709)),
      simde_mm_set_epi32(INT32_C(-1602128714), INT32_C( -811315535), INT32_C( -739483356), INT32_C(  137475709)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -278168455), INT32_C(-1440405844), INT32_C( -351927257), INT32_C( -925114922)),
      simde_mm_set_epi32(INT32_C(   97237822), INT32_C(-1440405844), INT32_C( 1100655169), INT32_C(  181456962)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(          0), INT32_C(          0), INT32_C(          0)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpnle_ps(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpnle_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C(  586573716), INT32_C( 2092571439), INT32_C(-1569791359), INT32_C( -771884114)),
      simde_mm_set_epi32(INT32_C(-1772197058), INT32_C( 2000867488), INT32_C( 1628528221), INT32_C( -771884114)),
      simde_mm_set_epi32(INT32_C(  586573716), INT32_C( 2092571439), INT32_C(-1569791359), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1418270756), INT32_C(-1023444334), INT32_C( 1630284961), INT32_C( 1200999231)),
      simde_mm_set_epi32(INT32_C(-1887216511), INT32_C(   -3136960), INT32_C( 1630284961), INT32_C( 1952383628)),
      simde_mm_set_epi32(INT32_C( 1418270756), INT32_C(-1023444334), INT32_C( 1630284961), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-1506287819), INT32_C(  667997188), INT32_C( -179453871), INT32_C( 1445563364)),
      simde_mm_set_epi32(INT32_C(-1506287819), INT32_C(  832794192), INT32_C( -179453871), INT32_C( 1445563364)),
      simde_mm_set_epi32(INT32_C(-1506287819), INT32_C(  667997188), INT32_C( -179453871), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -921490700), INT32_C(  767377840), INT32_C( 1198841751), INT32_C(  335888297)),
      simde_mm_set_epi32(INT32_C( -729465888), INT32_C(  767377840), INT32_C(-1211611524), INT32_C(  962373371)),
      simde_mm_set_epi32(INT32_C( -921490700), INT32_C(  767377840), INT32_C( 1198841751), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  279427637), INT32_C(-1356366148), INT32_C( -778146350), INT32_C( -407135484)),
      simde_mm_set_epi32(INT32_C( -396894639), INT32_C(-1356366148), INT32_C( -778146350), INT32_C( 1071745245)),
      simde_mm_set_epi32(INT32_C(  279427637), INT32_C(-1356366148), INT32_C( -778146350), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(  646913785), INT32_C(-1754178229), INT32_C(  419585954), INT32_C(  -90816511)),
      simde_mm_set_epi32(INT32_C(  733359934), INT32_C(-1754178229), INT32_C( 1438707211), INT32_C(  -90816511)),
      simde_mm_set_epi32(INT32_C(  646913785), INT32_C(-1754178229), INT32_C(  419585954), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -929496881), INT32_C( -605733719), INT32_C(  626875427), INT32_C(  862216293)),
      simde_mm_set_epi32(INT32_C(  769282939), INT32_C( -605733719), INT32_C(  626875427), INT32_C(  862216293)),
      simde_mm_set_epi32(INT32_C( -929496881), INT32_C( -605733719), INT32_C(  626875427), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( 1307976927), INT32_C( 1620554754), INT32_C( 1085448799), INT32_C( 1857805228)),
      simde_mm_set_epi32(INT32_C(-1751946539), INT32_C( 1620554754), INT32_C( -209862470), INT32_C(-1671218193)),
      simde_mm_set_epi32(INT32_C( 1307976927), INT32_C( 1620554754), INT32_C( 1085448799), INT32_C(         -1)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpnle_ss(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpnlt_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C( -696394921), INT32_C( -747986358), INT32_C( -647555009), INT32_C( 1026098152)),
      simde_mm_set_epi32(INT32_C( -633562196), INT32_C( -747986358), INT32_C( -647555009), INT32_C( 1026098152)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( -452783834), INT32_C( -762534148), INT32_C(  527526057), INT32_C(  875267484)),
      simde_mm_set_epi32(INT32_C( 2135403130), INT32_C( -762534148), INT32_C(  527526057), INT32_C( -792882965)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1833897906), INT32_C(-1370978496), INT32_C(-1396909811), INT32_C(  612871018)),
      simde_mm_set_epi32(INT32_C(-1833897906), INT32_C(-1370978496), INT32_C( 1459624563), INT32_C(  612871018)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 1770608184), INT32_C(-1440913128), INT32_C( -375433996), INT32_C(-1167135078)),
      simde_mm_set_epi32(INT32_C( 1685329021), INT32_C( 1594341486), INT32_C( -749207466), INT32_C(-1647221832)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(          0), INT32_C(          0), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C(-2031323732), INT32_C( -384315017), INT32_C(-2075546059), INT32_C(-1034483494)),
      simde_mm_set_epi32(INT32_C(  261868471), INT32_C( -384315017), INT32_C(-2075546059), INT32_C(-1034483494)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  749959102), INT32_C( 1174344579), INT32_C( 1538886158), INT32_C( 1204052778)),
      simde_mm_set_epi32(INT32_C(  915491351), INT32_C( 1174344579), INT32_C( 1538886158), INT32_C( 1204052778)),
      simde_mm_set_epi32(INT32_C(          0), INT32_C(         -1), INT32_C(         -1), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  702745450), INT32_C(-1148209866), INT32_C(  305771684), INT32_C(   10161619)),
      simde_mm_set_epi32(INT32_C( -437200102), INT32_C(-1148209866), INT32_C(  703243165), INT32_C(   10161619)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(          0), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(-1984623370), INT32_C( 1198216493), INT32_C( 1096311405), INT32_C( -697370337)),
      simde_mm_set_epi32(INT32_C(-1984623370), INT32_C( 1198216493), INT32_C( 1096311405), INT32_C( -697370337)),
      simde_mm_set_epi32(INT32_C(         -1), INT32_C(         -1), INT32_C(         -1), INT32_C(         -1)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpnlt_ps(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpnlt_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128i a;
    simde__m128i b;
    simde__m128i r;
  } test_vec[8] = {
    { simde_mm_set_epi32(INT32_C( 1412010535), INT32_C(-1239400933), INT32_C(  594063422), INT32_C( 1188961914)),
      simde_mm_set_epi32(INT32_C( 1663850825), INT32_C(-1239400933), INT32_C(  594063422), INT32_C( 1188961914)),
      simde_mm_set_epi32(INT32_C( 1412010535), INT32_C(-1239400933), INT32_C(  594063422), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( -316965064), INT32_C(-1377591008), INT32_C(-1622896924), INT32_C(-1267585285)),
      simde_mm_set_epi32(INT32_C( -662180320), INT32_C(-1377591008), INT32_C( -505498051), INT32_C( -824079960)),
      simde_mm_set_epi32(INT32_C( -316965064), INT32_C(-1377591008), INT32_C(-1622896924), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  952041483), INT32_C( -759750393), INT32_C(  571126339), INT32_C(-1097881178)),
      simde_mm_set_epi32(INT32_C(  952041483), INT32_C( -759750393), INT32_C(  571126339), INT32_C(  206622744)),
      simde_mm_set_epi32(INT32_C(  952041483), INT32_C( -759750393), INT32_C(  571126339), INT32_C(          0)) },
    { simde_mm_set_epi32(INT32_C( -778057296), INT32_C(-1150074331), INT32_C(  485854915), INT32_C( -514788129)),
      simde_mm_set_epi32(INT32_C(  235219858), INT32_C(-1150074331), INT32_C(  485854915), INT32_C( -514788129)),
      simde_mm_set_epi32(INT32_C( -778057296), INT32_C(-1150074331), INT32_C(  485854915), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( 1021281951), INT32_C(  386387638), INT32_C(  435001339), INT32_C(  317446933)),
      simde_mm_set_epi32(INT32_C(-1890218411), INT32_C(-1750380903), INT32_C(  435001339), INT32_C(  317446933)),
      simde_mm_set_epi32(INT32_C( 1021281951), INT32_C(  386387638), INT32_C(  435001339), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( -137019013), INT32_C( 1227598678), INT32_C( 1179331321), INT32_C(-2004841764)),
      simde_mm_set_epi32(INT32_C( 1473692950), INT32_C( 1227598678), INT32_C( 1179331321), INT32_C(-2004841764)),
      simde_mm_set_epi32(INT32_C( -137019013), INT32_C( 1227598678), INT32_C( 1179331321), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C(  196197184), INT32_C(-1956925508), INT32_C( 1585557819), INT32_C(  132702049)),
      simde_mm_set_epi32(INT32_C(-1510035164), INT32_C(-1956925508), INT32_C(-2054438272), INT32_C(  132702049)),
      simde_mm_set_epi32(INT32_C(  196197184), INT32_C(-1956925508), INT32_C( 1585557819), INT32_C(         -1)) },
    { simde_mm_set_epi32(INT32_C( -985194720), INT32_C( 1720892194), INT32_C( 1096426703), INT32_C(-1009397670)),
      simde_mm_set_epi32(INT32_C( -874151390), INT32_C( -332830918), INT32_C( 1096426703), INT32_C( 1294469295)),
      simde_mm_set_epi32(INT32_C( -985194720), INT32_C( 1720892194), INT32_C( 1096426703), INT32_C(          0)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m128i r = simde_mm_castps_si128(
        simde_mm_cmpnlt_ss(
          simde_mm_castsi128_ps(test_vec[i].a),
          simde_mm_castsi128_ps(test_vec[i].b)));
    simde_assert_m128i_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpord_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[] = {
    { simde_mm_set_ps(1.0f,  NAN,  NAN, 2.0f),
      simde_mm_set_ps( NAN, 3.0f,  NAN, 4.0f),
      simde_m128_set_u32(0x0, 0x0, 0x0, ~0x0) },
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_cmpord_ps(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpord_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[] = {
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0xffffffff)) },
    { simde_mm_set_ps( NAN,  NAN,  NAN,  NAN),
      simde_mm_set_ps( NAN,  NAN,  NAN,  NAN),
      simde_m128_set_u32(NAN_AS_U32, NAN_AS_U32, NAN_AS_U32, UINT32_C(0x00000000)) },
    { simde_mm_set_ps(NAN,  2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(NAN_AS_U32, UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0xffffffff)) },
    { simde_mm_set_ps(1.0f,  NAN, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), NAN_AS_U32, UINT32_C(0x40400000), UINT32_C(0xffffffff)) },
    { simde_mm_set_ps(1.0f, 2.0f,  NAN, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), NAN_AS_U32, UINT32_C(0xffffffff)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, NAN),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps( NAN, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0xffffffff)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f,  NAN, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0xffffffff)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f,  NAN, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0xffffffff)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f,  NAN),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0x00000000)) },
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_cmpord_ss(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_u32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpunord_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[] = {
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x00000000), UINT32_C(0x00000000), UINT32_C(0x00000000), UINT32_C(0x00000000)) },
    { simde_mm_set_ps( NAN,  NAN,  NAN,  NAN),
      simde_mm_set_ps( NAN,  NAN,  NAN,  NAN),
      simde_m128_set_u32(UINT32_C(0xffffffff), UINT32_C(0xffffffff), UINT32_C(0xffffffff), UINT32_C(0xffffffff)) },
    { simde_mm_set_ps(NAN,  2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0xffffffff), UINT32_C(0x00000000), UINT32_C(0x00000000), UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f,  NAN, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x00000000), UINT32_C(0xffffffff), UINT32_C(0x00000000), UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f, 2.0f,  NAN, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x00000000), UINT32_C(0x00000000), UINT32_C(0xffffffff), UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, NAN),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x00000000), UINT32_C(0x00000000), UINT32_C(0x00000000), UINT32_C(0xffffffff)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps( NAN, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0xffffffff), UINT32_C(0x00000000), UINT32_C(0x00000000), UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f,  NAN, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x00000000), UINT32_C(0xffffffff), UINT32_C(0x00000000), UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f,  NAN, 4.0f),
      simde_m128_set_u32(UINT32_C(0x00000000), UINT32_C(0x00000000), UINT32_C(0xffffffff), UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f,  NAN),
      simde_m128_set_u32(UINT32_C(0x00000000), UINT32_C(0x00000000), UINT32_C(0x00000000), UINT32_C(0xffffffff)) },
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_cmpunord_ps(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_comieq_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    int r;
  } test_vec[8] = {
    { simde_mm_set_ps(0.4193f, 0.8439f, 0.8100f, 0.2669f),
      simde_mm_set_ps(0.4193f, 0.8439f, 0.8100f, 0.2669f),
      1 },
    { simde_mm_set_ps(0.9426f, 0.1679f, 0.2845f, 0.1698f),
      simde_mm_set_ps(0.9426f, 0.6931f, 0.2845f, 0.1698f),
      1 },
    { simde_mm_set_ps(0.0547f, 0.2368f, 0.3365f, 0.9146f),
      simde_mm_set_ps(0.0547f, 0.5935f, 0.3365f, 0.5014f),
      0 },
    { simde_mm_set_ps(0.7179f, 0.8607f, 0.4372f, 0.6140f),
      simde_mm_set_ps(0.7179f, 0.7239f, 0.4372f, 0.6140f),
      1 },
    { simde_mm_set_ps(0.4474f, 0.6848f, 0.4305f, 0.8738f),
      simde_mm_set_ps(0.6059f, 0.8463f, 0.4305f, 0.1517f),
      0 },
    { simde_mm_set_ps(0.4537f, 0.7254f, 0.9987f, 0.9115f),
      simde_mm_set_ps(0.1771f, 0.2982f, 0.9987f, 0.9003f),
      0 },
    { simde_mm_set_ps(0.2162f, 0.6303f, 0.0602f, 0.9986f),
      simde_mm_set_ps(0.2162f, 0.5872f, 0.0602f, 0.2491f),
      0 },
    { simde_mm_set_ps(0.4836f, 0.9929f, 0.8942f, 0.2367f),
      simde_mm_set_ps(0.4836f, 0.9929f, 0.4202f, 0.2367f),
      1 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_comieq_ss(test_vec[i].a, test_vec[i].b);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_comige_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    int r;
  } test_vec[8] = {
    { simde_mm_set_ps(0.1174f, 0.9995f, 0.7362f, 0.9966f),
      simde_mm_set_ps(0.1174f, 0.5850f, 0.0225f, 0.0035f),
      1 },
    { simde_mm_set_ps(0.6820f, 0.9892f, 0.6235f, 0.1819f),
      simde_mm_set_ps(0.0898f, 0.9892f, 0.6235f, 0.3305f),
      0 },
    { simde_mm_set_ps(0.7152f, 0.9286f, 0.9635f, 0.8823f),
      simde_mm_set_ps(0.3403f, 0.4177f, 0.9635f, 0.8737f),
      1 },
    { simde_mm_set_ps(0.5619f, 0.6892f, 0.2137f, 0.5336f),
      simde_mm_set_ps(0.1340f, 0.0152f, 0.9280f, 0.5336f),
      1 },
    { simde_mm_set_ps(0.5476f, 0.8606f, 0.2177f, 0.5284f),
      simde_mm_set_ps(0.5476f, 0.6253f, 0.1285f, 0.7135f),
      0 },
    { simde_mm_set_ps(0.6649f, 0.2053f, 0.5053f, 0.0378f),
      simde_mm_set_ps(0.0308f, 0.2053f, 0.5053f, 0.8789f),
      0 },
    { simde_mm_set_ps(0.3714f, 0.8736f, 0.8711f, 0.9491f),
      simde_mm_set_ps(0.8296f, 0.2212f, 0.5986f, 0.9491f),
      1 },
    { simde_mm_set_ps(0.8791f, 0.5862f, 0.4977f, 0.0888f),
      simde_mm_set_ps(0.8669f, 0.7545f, 0.4977f, 0.0888f),
      1 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_comige_ss(test_vec[i].a, test_vec[i].b);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_comigt_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    int r;
  } test_vec[8] = {
    { simde_mm_set_ps(0.1174f, 0.9995f, 0.7362f, 0.9966f),
      simde_mm_set_ps(0.1174f, 0.5850f, 0.0225f, 0.0035f),
      1 },
    { simde_mm_set_ps(0.6820f, 0.9892f, 0.6235f, 0.1819f),
      simde_mm_set_ps(0.0898f, 0.9892f, 0.6235f, 0.3305f),
      0 },
    { simde_mm_set_ps(0.7152f, 0.9286f, 0.9635f, 0.8823f),
      simde_mm_set_ps(0.3403f, 0.4177f, 0.9635f, 0.8737f),
      1 },
    { simde_mm_set_ps(0.5619f, 0.6892f, 0.2137f, 0.5336f),
      simde_mm_set_ps(0.1340f, 0.0152f, 0.9280f, 0.5336f),
      0 },
    { simde_mm_set_ps(0.5476f, 0.8606f, 0.2177f, 0.5284f),
      simde_mm_set_ps(0.5476f, 0.6253f, 0.1285f, 0.7135f),
      0 },
    { simde_mm_set_ps(0.6649f, 0.2053f, 0.5053f, 0.0378f),
      simde_mm_set_ps(0.0308f, 0.2053f, 0.5053f, 0.8789f),
      0 },
    { simde_mm_set_ps(0.3714f, 0.8736f, 0.8711f, 0.9491f),
      simde_mm_set_ps(0.8296f, 0.2212f, 0.5986f, 0.9491f),
      0 },
    { simde_mm_set_ps(0.8791f, 0.5862f, 0.4977f, 0.0888f),
      simde_mm_set_ps(0.8669f, 0.7545f, 0.4977f, 0.0888f),
      0 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_comigt_ss(test_vec[i].a, test_vec[i].b);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_comile_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    int r;
  } test_vec[8] = {
    { simde_mm_set_ps(0.1174f, 0.9995f, 0.7362f, 0.9966f),
      simde_mm_set_ps(0.1174f, 0.5850f, 0.0225f, 0.0035f),
      0 },
    { simde_mm_set_ps(0.6820f, 0.9892f, 0.6235f, 0.1819f),
      simde_mm_set_ps(0.0898f, 0.9892f, 0.6235f, 0.3305f),
      1 },
    { simde_mm_set_ps(0.7152f, 0.9286f, 0.9635f, 0.8823f),
      simde_mm_set_ps(0.3403f, 0.4177f, 0.9635f, 0.8737f),
      0 },
    { simde_mm_set_ps(0.5619f, 0.6892f, 0.2137f, 0.5336f),
      simde_mm_set_ps(0.1340f, 0.0152f, 0.9280f, 0.5336f),
      1 },
    { simde_mm_set_ps(0.5476f, 0.8606f, 0.2177f, 0.5284f),
      simde_mm_set_ps(0.5476f, 0.6253f, 0.1285f, 0.7135f),
      1 },
    { simde_mm_set_ps(0.6649f, 0.2053f, 0.5053f, 0.0378f),
      simde_mm_set_ps(0.0308f, 0.2053f, 0.5053f, 0.8789f),
      1 },
    { simde_mm_set_ps(0.3714f, 0.8736f, 0.8711f, 0.9491f),
      simde_mm_set_ps(0.8296f, 0.2212f, 0.5986f, 0.9491f),
      1 },
    { simde_mm_set_ps(0.8791f, 0.5862f, 0.4977f, 0.0888f),
      simde_mm_set_ps(0.8669f, 0.7545f, 0.4977f, 0.0888f),
      1 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_comile_ss(test_vec[i].a, test_vec[i].b);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_comilt_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    int r;
  } test_vec[8] = {
    { simde_mm_set_ps(0.1174f, 0.9995f, 0.7362f, 0.9966f),
      simde_mm_set_ps(0.1174f, 0.5850f, 0.0225f, 0.0035f),
      0 },
    { simde_mm_set_ps(0.6820f, 0.9892f, 0.6235f, 0.1819f),
      simde_mm_set_ps(0.0898f, 0.9892f, 0.6235f, 0.3305f),
      1 },
    { simde_mm_set_ps(0.7152f, 0.9286f, 0.9635f, 0.8823f),
      simde_mm_set_ps(0.3403f, 0.4177f, 0.9635f, 0.8737f),
      0 },
    { simde_mm_set_ps(0.5619f, 0.6892f, 0.2137f, 0.5336f),
      simde_mm_set_ps(0.1340f, 0.0152f, 0.9280f, 0.5336f),
      0 },
    { simde_mm_set_ps(0.5476f, 0.8606f, 0.2177f, 0.5284f),
      simde_mm_set_ps(0.5476f, 0.6253f, 0.1285f, 0.7135f),
      1 },
    { simde_mm_set_ps(0.6649f, 0.2053f, 0.5053f, 0.0378f),
      simde_mm_set_ps(0.0308f, 0.2053f, 0.5053f, 0.8789f),
      1 },
    { simde_mm_set_ps(0.3714f, 0.8736f, 0.8711f, 0.9491f),
      simde_mm_set_ps(0.8296f, 0.2212f, 0.5986f, 0.9491f),
      0 },
    { simde_mm_set_ps(0.8791f, 0.5862f, 0.4977f, 0.0888f),
      simde_mm_set_ps(0.8669f, 0.7545f, 0.4977f, 0.0888f),
      0 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_comilt_ss(test_vec[i].a, test_vec[i].b);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_comineq_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    int r;
  } test_vec[8] = {
    { simde_mm_set_ps(0.1174f, 0.9995f, 0.7362f, 0.9966f),
      simde_mm_set_ps(0.1174f, 0.5850f, 0.0225f, 0.0035f),
      1 },
    { simde_mm_set_ps(0.6820f, 0.9892f, 0.6235f, 0.1819f),
      simde_mm_set_ps(0.0898f, 0.9892f, 0.6235f, 0.3305f),
      1 },
    { simde_mm_set_ps(0.7152f, 0.9286f, 0.9635f, 0.8823f),
      simde_mm_set_ps(0.3403f, 0.4177f, 0.9635f, 0.8737f),
      1 },
    { simde_mm_set_ps(0.5619f, 0.6892f, 0.2137f, 0.5336f),
      simde_mm_set_ps(0.1340f, 0.0152f, 0.9280f, 0.5336f),
      0 },
    { simde_mm_set_ps(0.5476f, 0.8606f, 0.2177f, 0.5284f),
      simde_mm_set_ps(0.5476f, 0.6253f, 0.1285f, 0.7135f),
      1 },
    { simde_mm_set_ps(0.6649f, 0.2053f, 0.5053f, 0.0378f),
      simde_mm_set_ps(0.0308f, 0.2053f, 0.5053f, 0.8789f),
      1 },
    { simde_mm_set_ps(0.3714f, 0.8736f, 0.8711f, 0.9491f),
      simde_mm_set_ps(0.8296f, 0.2212f, 0.5986f, 0.9491f),
      0 },
    { simde_mm_set_ps(0.8791f, 0.5862f, 0.4977f, 0.0888f),
      simde_mm_set_ps(0.8669f, 0.7545f, 0.4977f, 0.0888f),
      0 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_comineq_ss(test_vec[i].a, test_vec[i].b);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cmpunord_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[] = {
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0x00000000)) },
    { simde_mm_set_ps( NAN,  NAN,  NAN,  NAN),
      simde_mm_set_ps( NAN,  NAN,  NAN,  NAN),
      simde_m128_set_u32(NAN_AS_U32, NAN_AS_U32, NAN_AS_U32, UINT32_C(0xffffffff)) },
    { simde_mm_set_ps(NAN,  2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(NAN_AS_U32, UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f,  NAN, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), NAN_AS_U32, UINT32_C(0x40400000), UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f, 2.0f,  NAN, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), NAN_AS_U32, UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, NAN),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0xffffffff)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps( NAN, 2.0f, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f,  NAN, 3.0f, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f,  NAN, 4.0f),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0x00000000)) },
    { simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f),
      simde_mm_set_ps(1.0f, 2.0f, 3.0f,  NAN),
      simde_m128_set_u32(UINT32_C(0x3f800000), UINT32_C(0x40000000), UINT32_C(0x40400000), UINT32_C(0xffffffff)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_cmpunord_ss(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_i32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvt_pi2ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a, r;
    simde__m64 b;

    munit_rand_memory(sizeof(a), (uint8_t*) &a);
    munit_rand_memory(sizeof(b), (uint8_t*) &b);

    r = simde_mm_cvt_pi2ps(a, b);

    munit_assert_float(r.f32[0], ==, (simde_float32) b.i32[0]);
    munit_assert_float(r.f32[1], ==, (simde_float32) b.i32[1]);
    munit_assert_int32(r.i32[2], ==, a.i32[2]);
    munit_assert_int32(r.i32[3], ==, a.i32[3]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvt_ps2pi(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a;
    simde__m64 r;

    a = random_m128_f32();

    r = simde_mm_cvt_ps2pi(a);

    for (size_t j = 0 ; j < 2 ; j++) {
      int32_t n = (int32_t) a.f32[j];
      if (n != r.i32[j] && (n + 1) != r.i32[j] && (n - 1) != r.i32[j])
	munit_errorf("expected %d, got %d (from %g)", n, r.i32[j], a.f32[j]);
    }
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvt_si2ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a, r;
    int32_t b;

    munit_rand_memory(sizeof(a), (uint8_t*) &a);
    munit_rand_memory(sizeof(b), (uint8_t*) &b);

    r = simde_mm_cvt_si2ss(a, b);

    munit_assert_float(r.f32[0], ==, (simde_float32) b);
    /* Use int32 since comparing NaNs is always false. */
    munit_assert_int32(r.i32[1], ==, a.i32[1]);
    munit_assert_int32(r.i32[2], ==, a.i32[2]);
    munit_assert_int32(r.i32[3], ==, a.i32[3]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvt_ss2si(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a;
    int32_t r;

    munit_rand_memory(sizeof(a), (uint8_t*) &a);

    r = simde_mm_cvt_ss2si(a);

    simde_assert_int32_close(r, (int32_t) a.f32[0]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtpi16_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m64 a;
    simde__m128 r;

    munit_rand_memory(sizeof(a), (uint8_t*) &a);

    r = simde_mm_cvtpi16_ps(a);

    for (size_t j = 0 ; j < 4 ; j++) {
      simde_assert_int32_close(a.i16[j], (int16_t) r.f32[j]);
    }
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtpi32_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a, r;
    simde__m64 b;

    munit_rand_memory(sizeof(a), (uint8_t*) &a);
    for (size_t j = 0 ; j < 2 ; j++) {
      a.i32[j] = (int32_t) munit_rand_int_range(-65536, 65535);
      b.i32[j] = (int32_t) munit_rand_int_range(-65536, 65535);
    }

    r = simde_mm_cvtpi32_ps(a, b);

    simde_assert_int32_close(b.i32[0], (int32_t) r.f32[0]);
    simde_assert_int32_close(b.i32[1], (int32_t) r.f32[1]);
    munit_assert_int32(a.i32[2], ==, r.i32[2]);
    munit_assert_int32(a.i32[3], ==, r.i32[3]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtpi32x2_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m64 a, b;
    simde__m128 r;

    for (size_t j = 0 ; j < 2 ; j++) {
      a.i32[j] = munit_rand_int_range(-65536, 65535);
      b.i32[j] = munit_rand_int_range(-65536, 65535);
    }

    r = simde_mm_cvtpi32x2_ps(a, b);

    simde_assert_int32_close(a.i32[0], (int32_t) r.f32[0]);
    simde_assert_int32_close(a.i32[1], (int32_t) r.f32[1]);
    simde_assert_int32_close(b.i32[0], (int32_t) r.f32[2]);
    simde_assert_int32_close(b.i32[1], (int32_t) r.f32[3]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtpi8_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m64 a;
    simde__m128 r;

    munit_rand_memory(sizeof(a), (uint8_t*) &a);

    r = simde_mm_cvtpi8_ps(a);

    simde_assert_int32_close(a.i8[0], (int32_t) r.f32[0]);
    simde_assert_int32_close(a.i8[1], (int32_t) r.f32[1]);
    simde_assert_int32_close(a.i8[2], (int32_t) r.f32[2]);
    simde_assert_int32_close(a.i8[3], (int32_t) r.f32[3]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtps_pi16(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a;
    simde__m64 r;

    for (size_t j = 0 ; j < (sizeof(a.f32) / sizeof(a.f32[0])) ; j++) {
      a.f32[j] = (simde_float32) random_f64_range((simde_float64) INT16_MIN, (simde_float64) INT16_MAX);
    }

    r = simde_mm_cvtps_pi16(a);

    simde_assert_int32_close(r.i16[0], (int16_t) a.f32[0]);
    simde_assert_int32_close(r.i16[1], (int16_t) a.f32[1]);
    simde_assert_int32_close(r.i16[2], (int16_t) a.f32[2]);
    simde_assert_int32_close(r.i16[3], (int16_t) a.f32[3]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtps_pi32(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a;
    simde__m64 r;

    for (size_t j = 0 ; j < (sizeof(a.f32) / sizeof(a.f32[0])) ; j++) {
      a.f32[j] = (simde_float32) random_f64_range((simde_float64) INT32_MIN, (simde_float64) INT32_MAX);
    }

    r = simde_mm_cvtps_pi32(a);

    simde_assert_int32_close(r.i32[0], (int32_t) a.f32[0]);
    simde_assert_int32_close(r.i32[1], (int32_t) a.f32[1]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtps_pi8(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a;
    simde__m64 r;

    for (size_t j = 0 ; j < (sizeof(a.f32) / sizeof(a.f32[0])) ; j++) {
      a.f32[j] = (simde_float32) random_f64_range((simde_float64) INT8_MIN, (simde_float64) INT8_MAX);
    }

    r = simde_mm_cvtps_pi8(a);

    simde_assert_int32_close(r.i8[0], (int32_t) a.f32[0]);
    simde_assert_int32_close(r.i8[1], (int32_t) a.f32[1]);
    simde_assert_int32_close(r.i8[2], (int32_t) a.f32[2]);
    simde_assert_int32_close(r.i8[3], (int32_t) a.f32[3]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtpu16_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m64 a;
    simde__m128 r;

    munit_rand_memory(sizeof(a), (uint8_t*) &a);

    r = simde_mm_cvtpu16_ps(a);

    simde_assert_int32_close((uint16_t) r.f32[0], a.u16[0]);
    simde_assert_int32_close((uint16_t) r.f32[1], a.u16[1]);
    simde_assert_int32_close((uint16_t) r.f32[2], a.u16[2]);
    simde_assert_int32_close((uint16_t) r.f32[3], a.u16[3]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtpu8_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m64 a;
    simde__m128 r;

    munit_rand_memory(sizeof(a), (uint8_t*) &a);

    r = simde_mm_cvtpu8_ps(a);

    simde_assert_int32_close((uint16_t) r.f32[0], a.u8[0]);
    simde_assert_int32_close((uint16_t) r.f32[1], a.u8[1]);
    simde_assert_int32_close((uint16_t) r.f32[2], a.u8[2]);
    simde_assert_int32_close((uint16_t) r.f32[3], a.u8[3]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtsi32_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a, r;
    int32_t b;

    munit_rand_memory(sizeof(a), (uint8_t*) &a);
    b = (int32_t) munit_rand_int_range(INT16_MIN, INT16_MAX);

    r = simde_mm_cvtsi32_ss(a, b);

    simde_assert_int32_close((int) r.f32[0], b);
    munit_assert_int32(r.i32[1], ==, a.i32[1]);
    munit_assert_int32(r.i32[2], ==, a.i32[2]);
    munit_assert_int32(r.i32[3], ==, a.i32[3]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtsi64_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a, r;
    int64_t b;

    munit_rand_memory(sizeof(a), (uint8_t*) &a);
    b = (int64_t) munit_rand_int_range(INT16_MIN, INT16_MAX);

    r = simde_mm_cvtsi64_ss(a, b);

    simde_assert_int32_close((int) r.f32[0], (int) b);
    munit_assert_int32(r.i32[1], ==, a.i32[1]);
    munit_assert_int32(r.i32[2], ==, a.i32[2]);
    munit_assert_int32(r.i32[3], ==, a.i32[3]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtss_f32(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a;
    simde_float32 r;

    random_f32v(sizeof(a.f32) / sizeof(a.f32[0]), (simde_float32*) &a);

    r = simde_mm_cvtss_f32(a);

    munit_assert_float(r, ==, a.f32[0]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtss_si32(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a;
    int32_t r;

    random_f32v(sizeof(a.f32) / sizeof(a.f32[0]), (simde_float32*) &a);

    r = simde_mm_cvtss_si32(a);

    simde_assert_int32_close(r, (int) a.f32[0]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtss_si64(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a;
    int64_t r;

    random_f32v(sizeof(a.f32) / sizeof(a.f32[0]), (simde_float32*) &a);

    r = simde_mm_cvtss_si64(a);

    simde_assert_int64_close((int64_t) r, (int64_t) a.f32[0]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtt_ps2pi(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a;
    simde__m64 r;

    for (size_t j = 0 ; j < (sizeof(a.f32) / sizeof(a.f32[0])) ; j++) {
      a.f32[j] = (simde_float32) random_f64_range((simde_float64) INT16_MIN, (simde_float64) INT16_MAX);
    }

    r = simde_mm_cvtt_ps2pi(a);

    munit_assert_int32(r.i32[0], ==, SIMDE_CONVERT_FTOI(int32_t, truncf(a.f32[0])));
    munit_assert_int32(r.i32[1], ==, SIMDE_CONVERT_FTOI(int32_t, truncf(a.f32[1])));
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvtt_ss2si(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a;
    int32_t r;

    for (size_t j = 0 ; j < (sizeof(a.f32) / sizeof(a.f32[0])) ; j++) {
      a.f32[j] = (simde_float32) random_f64_range((simde_float64) INT16_MIN, (simde_float64) INT16_MAX);
    }

    r = simde_mm_cvtt_ss2si(a);

    munit_assert_int32(r, ==, SIMDE_CONVERT_FTOI(int32_t, truncf(a.f32[0])));
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_cvttss_si64(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a;
    int64_t r;

    for (size_t j = 0 ; j < (sizeof(a.f32) / sizeof(a.f32[0])) ; j++) {
      a.f32[j] = (simde_float32) random_f64_range((simde_float64) INT16_MIN, (simde_float64) INT16_MAX);
    }

    r = simde_mm_cvttss_si64(a);

    munit_assert_int64(r, ==, SIMDE_CONVERT_FTOI(int64_t, truncf(a.f32[0])));
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_div_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a, b, r, x;

    for (size_t j = 0 ; j < (sizeof(a.f32) / sizeof(a.f32[0])) ; j++) {
      a.f32[j] = (simde_float32) random_f64_range((simde_float64) INT32_MIN, (simde_float64) INT32_MAX);
      b.f32[j] = (simde_float32) random_f64_range((simde_float64) INT32_MIN, (simde_float64) INT32_MAX);
      x.f32[j] = a.f32[j] / b.f32[j];
    }

    r = simde_mm_div_ps(a, b);

    simde_assert_m128_close(r, x, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_div_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m128 a, b, r, x;

    for (size_t j = 0 ; j < (sizeof(a.f32) / sizeof(a.f32[0])) ; j++) {
      a.f32[j] = (simde_float32) random_f64_range((simde_float64) INT32_MIN, (simde_float64) INT32_MAX);
      b.f32[j] = (simde_float32) random_f64_range((simde_float64) INT32_MIN, (simde_float64) INT32_MAX);
      x.f32[j] = (j == 0) ? (a.f32[j] / b.f32[j]) : a.f32[j];
    }

    r = simde_mm_div_ss(a, b);

    simde_assert_m128_close(r, x, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_extract_pi16(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t i = 0 ; i < TEST_PREFERRED_ITERATIONS ; i++) {
    simde__m64 a;
    int_fast16_t r;

    munit_rand_memory(sizeof(a), (uint8_t*) &a);

    r = simde_mm_extract_pi16(a, 2);

    munit_assert_int16((int16_t) (r & 0xffff), ==, a.i16[2]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_insert_pi16(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t j = 0 ; j < TEST_PREFERRED_ITERATIONS ; j++) {
    simde__m64 a, r, x;
    int16_t i;

    munit_rand_memory(sizeof(a), (uint8_t*) &a);
    i = (int16_t) munit_rand_int_range(INT16_MIN, INT16_MAX);

    r = simde_mm_insert_pi16(a, i, 2);
    x.i64[0] = a.i64[0];
    x.i16[2] = i;

    simde_assert_m64_i16(r, ==, x);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_load_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t j = 0 ; j < TEST_PREFERRED_ITERATIONS ; j++) {
    simde__m128 a, r;

    a = random_m128_f32();

    r = simde_mm_load_ps((simde_float32*) &a);

    simde_assert_m128_f32(r, ==, a);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_load_ps1(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t j = 0 ; j < TEST_PREFERRED_ITERATIONS ; j++) {
    simde__m128 r;
    simde_float32 a;

    random_f32v(1, &a);

    r = simde_mm_load_ps1(&a);

    for (size_t i = 0 ; i < sizeof(r.f32) / sizeof(r.f32[0]) ; i++) {
      munit_assert_float(r.f32[i], ==, a);
    }
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_load_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t j = 0 ; j < TEST_PREFERRED_ITERATIONS ; j++) {
    simde__m128 r;
    simde_float32 a;

    r.i32[0] = 0;
    r.i32[1] = 0;
    r.i32[2] = 0;
    r.i32[3] = 0;

    random_f32v(1, &a);

    r = simde_mm_load_ss(&a);

    munit_assert_float(r.f32[0], ==, a);
    munit_assert_int32(r.i32[1], ==, 0);
    munit_assert_int32(r.i32[2], ==, 0);
    munit_assert_int32(r.i32[3], ==, 0);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_loadh_pi(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t j = 0 ; j < TEST_PREFERRED_ITERATIONS ; j++) {
    simde__m128 a, r;
    simde__m64 b;

    a = random_m128_f32();
    random_f32v(sizeof(b) / sizeof(simde_float32), (simde_float32*) &b);

    r = simde_mm_loadh_pi(a, &b);

    munit_assert_double_equal(r.f32[0], a.f32[0], 4);
    munit_assert_double_equal(r.f32[1], a.f32[1], 4);
    munit_assert_double_equal(r.f32[2], b.f32[0], 4);
    munit_assert_double_equal(r.f32[3], b.f32[1], 4);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_loadl_pi(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t j = 0 ; j < TEST_PREFERRED_ITERATIONS ; j++) {
    simde__m128 a, r;
    simde__m64 b;

    a = random_m128_f32();
    random_f32v(sizeof(b) / sizeof(simde_float32), (simde_float32*) &b);

    r = simde_mm_loadl_pi(a, &b);

    munit_assert_double_equal(r.f32[0], b.f32[0], 4);
    munit_assert_double_equal(r.f32[1], b.f32[1], 4);
    munit_assert_double_equal(r.f32[2], a.f32[2], 4);
    munit_assert_double_equal(r.f32[3], a.f32[3], 4);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_loadr_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t j = 0 ; j < TEST_PREFERRED_ITERATIONS ; j++) {
    simde__m128 r;
    SIMDE_ALIGN(16) simde_float32 a[4];

    random_f32v(sizeof(a) / sizeof(a[0]), a);

    r = simde_mm_loadr_ps(a);

    munit_assert_double_equal(r.f32[0], a[3], 4);
    munit_assert_double_equal(r.f32[1], a[2], 4);
    munit_assert_double_equal(r.f32[2], a[1], 4);
    munit_assert_double_equal(r.f32[3], a[0], 4);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_loadu_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  for (size_t j = 0 ; j < TEST_PREFERRED_ITERATIONS ; j++) {
    simde__m128 r;
    simde_float32 a[4];

    random_f32v(sizeof(a) / sizeof(a[0]), a);

    r = simde_mm_loadu_ps(a);

    munit_assert_double_equal(r.f32[0], a[0], 4);
    munit_assert_double_equal(r.f32[1], a[1], 4);
    munit_assert_double_equal(r.f32[2], a[2], 4);
    munit_assert_double_equal(r.f32[3], a[3], 4);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_maskmove_si64(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m64 a;
    char b[8];
    simde__m64 mask;
    char r[8];
  } test_vec[8] = {
    { simde_mm_set_pi8(INT8_C(  -5), INT8_C( 112), INT8_C( 109), INT8_C(  -9), INT8_C( -10), INT8_C( -30), INT8_C(-103), INT8_C( -13)),
      { INT8_C( -67), INT8_C( -21), INT8_C( 107), INT8_C(  96), INT8_C( -47), INT8_C(  95), INT8_C(  34), INT8_C(-108) },
      simde_mm_set_pi8(-112,   64,  -92,   19,  -70,  -49,  -42,   75),
      { INT8_C( -67), INT8_C(-103), INT8_C( -30), INT8_C( -10), INT8_C( -47), INT8_C( 109), INT8_C(  34), INT8_C(  -5) } },
    { simde_mm_set_pi8(INT8_C(-111), INT8_C(  -8), INT8_C( -84), INT8_C(  80), INT8_C(   8), INT8_C( -61), INT8_C( -80), INT8_C( -85)),
      { INT8_C(  66), INT8_C(  19), INT8_C(  96), INT8_C(   6), INT8_C( -30), INT8_C(  85), INT8_C(  97), INT8_C( -44) },
      simde_mm_set_pi8( -90,  -11, -113,   11,  -96, -125,  121, -113),
      { INT8_C( -85), INT8_C(  19), INT8_C( -61), INT8_C(   8), INT8_C( -30), INT8_C( -84), INT8_C(  -8), INT8_C(-111) } },
    { simde_mm_set_pi8(INT8_C( 120), INT8_C(-105), INT8_C(   2), INT8_C(  21), INT8_C(  93), INT8_C(-124), INT8_C(  -2), INT8_C(  79)),
      { INT8_C(  87), INT8_C(  98), INT8_C( -52), INT8_C(  28), INT8_C(  37), INT8_C(-120), INT8_C( 109), INT8_C(  79) },
      simde_mm_set_pi8(  34,  106,  -74,   83, -114,  -10,   67,  111),
      { INT8_C(  87), INT8_C(  98), INT8_C(-124), INT8_C(  93), INT8_C(  37), INT8_C(   2), INT8_C( 109), INT8_C(  79) } },
    { simde_mm_set_pi8(INT8_C( -26), INT8_C(  96), INT8_C(-115), INT8_C(  78), INT8_C(  35), INT8_C(  49), INT8_C(  36), INT8_C( -25)),
      { INT8_C(  29), INT8_C(  28), INT8_C(  30), INT8_C( -80), INT8_C( -12), INT8_C(  81), INT8_C( -81), INT8_C( 120) },
      simde_mm_set_pi8( -59,  -55, -111, -119,   86,   -5,   74,    2),
      { INT8_C(  29), INT8_C(  28), INT8_C(  49), INT8_C( -80), INT8_C(  78), INT8_C(-115), INT8_C(  96), INT8_C( -26) } },
    { simde_mm_set_pi8(INT8_C( -37), INT8_C(  41), INT8_C( 123), INT8_C(-107), INT8_C(-123), INT8_C(  32), INT8_C(  33), INT8_C(   5)),
      { INT8_C(-126), INT8_C(  94), INT8_C( -30), INT8_C( -13), INT8_C(  99), INT8_C( 126), INT8_C(  16), INT8_C(   5) },
      simde_mm_set_pi8( 113,    1,   66,    5,  -18,   77,  -41,   -4),
      { INT8_C(   5), INT8_C(  33), INT8_C( -30), INT8_C(-123), INT8_C(  99), INT8_C( 126), INT8_C(  16), INT8_C(   5) } },
    { simde_mm_set_pi8(INT8_C( -90), INT8_C(-113), INT8_C(  97), INT8_C(  73), INT8_C(  33), INT8_C(  -3), INT8_C( 109), INT8_C(  63)),
      { INT8_C( -41), INT8_C( -87), INT8_C(-115), INT8_C( -26), INT8_C(  68), INT8_C( -33), INT8_C(   7), INT8_C( -69) },
      simde_mm_set_pi8(  28,  106,  -56,  -23, -126, -119,   22,  -23),
      { INT8_C(  63), INT8_C( -87), INT8_C(  -3), INT8_C(  33), INT8_C(  73), INT8_C(  97), INT8_C(   7), INT8_C( -69) } },
    { simde_mm_set_pi8(INT8_C(  12), INT8_C(   9), INT8_C( -66), INT8_C(-114), INT8_C(  19), INT8_C(  94), INT8_C(-103), INT8_C(  38)),
      { INT8_C( 102), INT8_C( -71), INT8_C(  55), INT8_C(  40), INT8_C(-122), INT8_C(  94), INT8_C(  71), INT8_C( -99) },
      simde_mm_set_pi8(  78,   82, -126,   33,  118,   93, -125,  113),
      { INT8_C( 102), INT8_C(-103), INT8_C(  55), INT8_C(  40), INT8_C(-122), INT8_C( -66), INT8_C(  71), INT8_C( -99) } },
    { simde_mm_set_pi8(INT8_C(  67), INT8_C( 108), INT8_C(  55), INT8_C( -68), INT8_C(  -5), INT8_C( -18), INT8_C( 115), INT8_C( 126)),
      { INT8_C(  92), INT8_C( -82), INT8_C( 101), INT8_C( -48), INT8_C(  45), INT8_C( 101), INT8_C(  95), INT8_C(  51) },
      simde_mm_set_pi8( -92,  -27,   65,  -21,   36,  126,   80,   50),
      { INT8_C(  92), INT8_C( -82), INT8_C( 101), INT8_C( -48), INT8_C( -68), INT8_C( 101), INT8_C( 108), INT8_C(  67) } }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    char r[8];
    memcpy(r, test_vec[i].b, sizeof(r));

    simde_mm_maskmove_si64(test_vec[i].a, test_vec[i].mask, r);
    simde_assert_int8vx(8, (int8_t*) r, ==, (int8_t*) test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_max_pi16(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m64 a;
    simde__m64 b;
    simde__m64 r;
  } test_vec[8] = {
    { simde_mm_set_pi8(  79, -120,    6,   13,  -78,  -13,  -16,   54),
      simde_mm_set_pi8(   5,  -78,   46,   46,  -22,   54,  126,  -85),
      simde_mm_set_pi8(  79, -120,   46,   46,  -22,   54,  126,  -85) },
    { simde_mm_set_pi8( 127,   51,   47, -108,   82,  -85,  -83,   11),
      simde_mm_set_pi8(-104,  -98,  -35,   -9,  -38,   92,    0,   10),
      simde_mm_set_pi8( 127,   51,   47, -108,   82,  -85,    0,   10) },
    { simde_mm_set_pi8( 110,   85,  -80,   87, -127,  -32, -121, -101),
      simde_mm_set_pi8( 105,  -98,   69, -109,  -78, -124,  -76,  -55),
      simde_mm_set_pi8( 110,   85,   69, -109,  -78, -124,  -76,  -55) },
    { simde_mm_set_pi8( -52,   59,   86, -104,  113,   82, -123,  119),
      simde_mm_set_pi8(  47,   29,  110,  -75,   83,  -56,   -6,  -11),
      simde_mm_set_pi8(  47,   29,  110,  -75,  113,   82,   -6,  -11) },
    { simde_mm_set_pi8(  99,  -23,    8,   -8,  -76,   -7, -106,   28),
      simde_mm_set_pi8( -85,   79,  -49,  -76,   40,  -66,   26,   72),
      simde_mm_set_pi8(  99,  -23,    8,   -8,   40,  -66,   26,   72) },
    { simde_mm_set_pi8( -57,  104, -109,  -39,   -2,  -10, -113,   72),
      simde_mm_set_pi8(  44,    7,  108,   98,   31,   78,  -79,    8),
      simde_mm_set_pi8(  44,    7,  108,   98,   31,   78,  -79,    8) },
    { simde_mm_set_pi8( -91,  -97,  -40,   31,  -83,  -78,   87,  -84),
      simde_mm_set_pi8(  43,   17,   57,   -3, -119,   78,  -66,   74),
      simde_mm_set_pi8(  43,   17,   57,   -3,  -83,  -78,   87,  -84) },
    { simde_mm_set_pi8(  52,  -99,  -84,   61,   17,   -5, -105,   72),
      simde_mm_set_pi8(  33,   -2,    0,  -88,  -17,  -91,   -4, -127),
      simde_mm_set_pi8(  52,  -99,    0,  -88,   17,   -5,   -4, -127) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m64 r = simde_mm_max_pi16(test_vec[i].a, test_vec[i].b);
    simde_assert_m64_i16(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_max_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps( 243.39f,  726.06f, -707.75f,  539.11f),
      simde_mm_set_ps( 452.06f, -743.40f, -959.79f, -625.85f),
      simde_mm_set_ps( 452.06f,  726.06f, -707.75f,  539.11f) },
    { simde_mm_set_ps( 927.87f,  825.19f, -742.41f, -106.29f),
      simde_mm_set_ps(-988.96f, -513.77f,  271.05f, -506.77f),
      simde_mm_set_ps( 927.87f,  825.19f,  271.05f, -106.29f) },
    { simde_mm_set_ps( 501.07f, -751.90f,  333.57f,  862.44f),
      simde_mm_set_ps(-129.71f,  542.65f,  -99.64f, -609.96f),
      simde_mm_set_ps( 501.07f,  542.65f,  333.57f,  862.44f) },
    { simde_mm_set_ps( 237.27f, -100.72f,  806.63f,  318.52f),
      simde_mm_set_ps(  75.91f,  129.34f, -169.38f, -401.83f),
      simde_mm_set_ps( 237.27f,  129.34f,  806.63f,  318.52f) },
    { simde_mm_set_ps(-664.90f,  204.37f, -623.14f,   77.06f),
      simde_mm_set_ps(-909.66f,  849.83f, -206.26f, -162.33f),
      simde_mm_set_ps(-664.90f,  849.83f, -206.26f,   77.06f) },
    { simde_mm_set_ps(-141.59f, -323.72f, -113.68f, -564.18f),
      simde_mm_set_ps( 293.03f, -880.63f,  701.27f, -113.42f),
      simde_mm_set_ps( 293.03f, -323.72f,  701.27f, -113.42f) },
    { simde_mm_set_ps(-158.10f, -618.57f,  197.04f,  507.93f),
      simde_mm_set_ps(-813.81f, -476.03f, -357.06f,  262.52f),
      simde_mm_set_ps(-158.10f, -476.03f,  197.04f,  507.93f) },
    { simde_mm_set_ps( 336.66f, -150.62f, -774.81f,  -52.00f),
      simde_mm_set_ps(-283.99f, -939.93f, -115.92f,  471.61f),
      simde_mm_set_ps( 336.66f, -150.62f, -115.92f,  471.61f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_max_ps(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_f32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_max_pu8(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m64 a;
    simde__m64 b;
    simde__m64 r;
  } test_vec[8] = {
    { simde_x_mm_set_pu8( 95,  35, 185, 136, 198,  90, 109, 138),
      simde_x_mm_set_pu8(165,  90, 139, 114, 195, 147,  62, 160),
      simde_x_mm_set_pu8(165,  90, 185, 136, 198, 147, 109, 160) },
    { simde_x_mm_set_pu8(192, 104, 198, 106, 121, 208, 222,  80),
      simde_x_mm_set_pu8( 14,  66, 107, 237,  19, 244, 121, 240),
      simde_x_mm_set_pu8(192, 104, 198, 237, 121, 244, 222, 240) },
    { simde_x_mm_set_pu8( 95,  71, 177, 202,  66,  32,  85, 206),
      simde_x_mm_set_pu8(206, 134,  86, 165, 246, 242, 112, 247),
      simde_x_mm_set_pu8(206, 134, 177, 202, 246, 242, 112, 247) },
    { simde_x_mm_set_pu8(247, 220,   6,  72, 190, 176, 185, 129),
      simde_x_mm_set_pu8(102, 233,  69,  70,  65,  36, 164, 122),
      simde_x_mm_set_pu8(247, 233,  69,  72, 190, 176, 185, 129) },
    { simde_x_mm_set_pu8(121,  66, 178,  31,  46,  35, 117,  91),
      simde_x_mm_set_pu8(162, 127, 145,  79, 214,  91, 102,  58),
      simde_x_mm_set_pu8(162, 127, 178,  79, 214,  91, 117,  91) },
    { simde_x_mm_set_pu8( 75,  55, 102,  27, 144, 219,  63,  26),
      simde_x_mm_set_pu8(178, 228,  83,  88,  34,  43, 215,  34),
      simde_x_mm_set_pu8(178, 228, 102,  88, 144, 219, 215,  34) },
    { simde_x_mm_set_pu8( 71, 199, 130, 210,  23, 163, 117, 223),
      simde_x_mm_set_pu8( 47, 138,  43,  60, 152,  77, 246,   8),
      simde_x_mm_set_pu8( 71, 199, 130, 210, 152, 163, 246, 223) },
    { simde_x_mm_set_pu8( 65, 226,  26,  83, 148,  71,   8, 192),
      simde_x_mm_set_pu8( 48,  22, 250, 180,  93,  65,  44,  38),
      simde_x_mm_set_pu8( 65, 226, 250, 180, 148,  71,  44, 192) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m64 r = simde_mm_max_pu8(test_vec[i].a, test_vec[i].b);
    simde_assert_m64_u8(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_max_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps( 275.98f,  244.68f,  294.03f,  414.26f),
      simde_mm_set_ps( 915.80f,   -0.19f,   23.06f,   81.33f),
      simde_mm_set_ps( 275.98f,  244.68f,  294.03f,  414.26f) },
    { simde_mm_set_ps(-813.38f,  435.35f,  419.41f, -441.22f),
      simde_mm_set_ps( 389.17f, -536.41f, -137.18f, -787.72f),
      simde_mm_set_ps(-813.38f,  435.35f,  419.41f, -441.22f) },
    { simde_mm_set_ps(-619.96f, -614.05f, -479.78f, -823.70f),
      simde_mm_set_ps(-814.29f,  295.27f, -132.00f,  -70.04f),
      simde_mm_set_ps(-619.96f, -614.05f, -479.78f,  -70.04f) },
    { simde_mm_set_ps(-480.26f, -233.90f,  242.17f, -129.02f),
      simde_mm_set_ps(-777.79f, -728.41f,  -33.93f, -163.52f),
      simde_mm_set_ps(-480.26f, -233.90f,  242.17f, -129.02f) },
    { simde_mm_set_ps(-442.14f,  410.97f,  665.05f, -946.79f),
      simde_mm_set_ps(-545.42f,   47.51f,  -78.24f, -648.70f),
      simde_mm_set_ps(-442.14f,  410.97f,  665.05f, -648.70f) },
    { simde_mm_set_ps(-136.30f, -558.74f,  355.69f,   48.70f),
      simde_mm_set_ps(-820.06f, -448.36f,  -48.18f, -396.98f),
      simde_mm_set_ps(-136.30f, -558.74f,  355.69f,   48.70f) },
    { simde_mm_set_ps( 955.43f, -448.98f, -165.93f,   79.87f),
      simde_mm_set_ps(-380.33f,  295.42f,  -77.30f,  721.77f),
      simde_mm_set_ps( 955.43f, -448.98f, -165.93f,  721.77f) },
    { simde_mm_set_ps( -40.78f,  393.73f,  -60.99f, -143.02f),
      simde_mm_set_ps(-232.14f,   77.20f, -606.64f, -624.14f),
      simde_mm_set_ps( -40.78f,  393.73f,  -60.99f, -143.02f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_max_ss(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_f32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_min_pi16(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m64 a;
    simde__m64 b;
    simde__m64 r;
  } test_vec[8] = {
    { simde_mm_set_pi16(-17712,  12551, -23434, -13133),
      simde_mm_set_pi16( 26266, -20490,   7912,  29803),
      simde_mm_set_pi16(-17712, -20490, -23434, -13133) },
    { simde_mm_set_pi16(-24457,  29877, -26735, -29539),
      simde_mm_set_pi16( 11486,  18014,  20714, -14577),
      simde_mm_set_pi16(-24457,  18014, -26735, -29539) },
    { simde_mm_set_pi16(-20528,   7690,  11233,  -7462),
      simde_mm_set_pi16(-32519,   4248, -31417, -32471),
      simde_mm_set_pi16(-32519,   4248, -31417, -32471) },
    { simde_mm_set_pi16(-31520,   3559, -26842,  21046),
      simde_mm_set_pi16( 13846,  -3714,  16375,  18158),
      simde_mm_set_pi16(-31520,  -3714, -26842,  18158) },
    { simde_mm_set_pi16( 21922,   9874,  13654,  24031),
      simde_mm_set_pi16( 23732,  13322,   8641,  -2491),
      simde_mm_set_pi16( 21922,   9874,   8641,  -2491) },
    { simde_mm_set_pi16( 14557,   3319,  16372,  28742),
      simde_mm_set_pi16(-29436,  20833, -11479, -29779),
      simde_mm_set_pi16(-29436,   3319, -11479, -29779) },
    { simde_mm_set_pi16( 14514,  25528,  18329,  19467),
      simde_mm_set_pi16(  7807, -10832,  -5002, -30632),
      simde_mm_set_pi16(  7807, -10832,  -5002, -30632) },
    { simde_mm_set_pi16( 10007,  31428,  28911, -29602),
      simde_mm_set_pi16( 29865, -25102,  11884,   9524),
      simde_mm_set_pi16( 10007, -25102,  11884, -29602) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m64 r = simde_mm_min_pi16(test_vec[i].a, test_vec[i].b);
    simde_assert_m64_i16(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_min_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps(-208.76f, -538.62f,  274.66f, -108.23f),
      simde_mm_set_ps(  64.30f,  627.51f, -431.61f, -737.40f),
      simde_mm_set_ps(-208.76f, -538.62f, -431.61f, -737.40f) },
    { simde_mm_set_ps( -73.05f,  464.03f,  573.95f, -332.31f),
      simde_mm_set_ps( 981.43f,  170.70f,  925.44f,   10.20f),
      simde_mm_set_ps( -73.05f,  170.70f,  573.95f, -332.31f) },
    { simde_mm_set_ps(-607.82f, -692.09f,  269.02f, -871.09f),
      simde_mm_set_ps( -20.67f,  813.79f,  396.23f, -759.77f),
      simde_mm_set_ps(-607.82f, -692.09f,  269.02f, -871.09f) },
    { simde_mm_set_ps(-134.36f,  503.32f,  958.70f, -249.82f),
      simde_mm_set_ps(  85.42f,  559.57f, -188.45f, -400.64f),
      simde_mm_set_ps(-134.36f,  503.32f, -188.45f, -400.64f) },
    { simde_mm_set_ps(-181.45f,  972.62f, -574.93f,  785.60f),
      simde_mm_set_ps( 831.31f,  832.49f,  748.83f,  657.11f),
      simde_mm_set_ps(-181.45f,  832.49f, -574.93f,  657.11f) },
    { simde_mm_set_ps( 510.02f, -639.06f, -929.32f, -561.38f),
      simde_mm_set_ps( 470.99f,  798.01f, -752.21f, -260.75f),
      simde_mm_set_ps( 470.99f, -639.06f, -929.32f, -561.38f) },
    { simde_mm_set_ps( 948.37f,  798.25f,  885.60f, -998.75f),
      simde_mm_set_ps(  -1.83f, -742.81f,   99.17f,   81.46f),
      simde_mm_set_ps(  -1.83f, -742.81f,   99.17f, -998.75f) },
    { simde_mm_set_ps(-202.80f, -308.12f,    5.40f, -806.83f),
      simde_mm_set_ps( 249.52f, -139.94f,  736.53f, -851.39f),
      simde_mm_set_ps(-202.80f, -308.12f,    5.40f, -851.39f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_min_ps(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_f32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_min_pu8(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m64 a;
    simde__m64 b;
    simde__m64 r;
  } test_vec[8] = {
    { simde_x_mm_set_pu8(  3,  32, 201, 226, 126, 175,  31,  50),
      simde_x_mm_set_pu8(220, 148, 109, 216,  32, 219, 221, 116),
      simde_x_mm_set_pu8(  3,  32, 109, 216,  32, 175,  31,  50) },
    { simde_x_mm_set_pu8(123, 186, 147,  62,  85, 163, 217, 248),
      simde_x_mm_set_pu8( 50, 187, 220, 240, 243, 231, 241, 209),
      simde_x_mm_set_pu8( 50, 186, 147,  62,  85, 163, 217, 209) },
    { simde_x_mm_set_pu8(100,  27,  19,  67, 100, 214, 111, 154),
      simde_x_mm_set_pu8( 31,  16,  59, 138, 178,  43,  63, 213),
      simde_x_mm_set_pu8( 31,  16,  19,  67, 100,  43,  63, 154) },
    { simde_x_mm_set_pu8(205, 136,  79, 245, 178, 167,   7,   9),
      simde_x_mm_set_pu8(103, 214, 180, 123,  12, 141,  59, 104),
      simde_x_mm_set_pu8(103, 136,  79, 123,  12, 141,   7,   9) },
    { simde_x_mm_set_pu8(143, 111, 158,  95, 192,  18,  83,  18),
      simde_x_mm_set_pu8( 81, 138, 112,  76,  64, 169,  64,  35),
      simde_x_mm_set_pu8( 81, 111, 112,  76,  64,  18,  64,  18) },
    { simde_x_mm_set_pu8(246, 106, 240, 187, 202, 248,   5, 105),
      simde_x_mm_set_pu8(184, 221, 161, 239, 162, 163,  17, 109),
      simde_x_mm_set_pu8(184, 106, 161, 187, 162, 163,   5, 105) },
    { simde_x_mm_set_pu8(172,  59,  82,   1, 130,  31, 233,  87),
      simde_x_mm_set_pu8( 51, 153, 219,  33, 100, 204, 105, 228),
      simde_x_mm_set_pu8( 51,  59,  82,   1, 100,  31, 105,  87) },
    { simde_x_mm_set_pu8(228, 182, 179, 248,  70,  35,  65,  84),
      simde_x_mm_set_pu8( 84, 159, 106,   2, 156, 107, 120,  67),
      simde_x_mm_set_pu8( 84, 159, 106,   2,  70,  35,  65,  67) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m64 r = simde_mm_min_pu8(test_vec[i].a, test_vec[i].b);
    simde_assert_m64_u8(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_min_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps( 905.52f, -330.94f, -743.29f,  -10.28f),
      simde_mm_set_ps(-603.64f,  -33.37f,  723.28f, -975.56f),
      simde_mm_set_ps( 905.52f, -330.94f, -743.29f, -975.56f) },
    { simde_mm_set_ps(-901.94f,  395.72f,  391.94f, -212.21f),
      simde_mm_set_ps(-188.76f,  605.72f, -757.32f, -217.92f),
      simde_mm_set_ps(-901.94f,  395.72f,  391.94f, -217.92f) },
    { simde_mm_set_ps( 270.60f,  585.69f, -494.83f,  500.56f),
      simde_mm_set_ps( 444.26f,  925.14f, -362.96f,  120.59f),
      simde_mm_set_ps( 270.60f,  585.69f, -494.83f,  120.59f) },
    { simde_mm_set_ps( 222.03f, -452.05f, -212.51f,   16.60f),
      simde_mm_set_ps(-338.89f,  786.83f, -596.87f,  345.56f),
      simde_mm_set_ps( 222.03f, -452.05f, -212.51f,   16.60f) },
    { simde_mm_set_ps( 130.17f, -389.05f, -693.23f, -558.79f),
      simde_mm_set_ps( 351.18f,    1.64f,  661.55f,  667.31f),
      simde_mm_set_ps( 130.17f, -389.05f, -693.23f, -558.79f) },
    { simde_mm_set_ps(-492.87f,  857.67f,   99.45f, -129.44f),
      simde_mm_set_ps( 424.94f,  552.64f,   68.12f,  195.21f),
      simde_mm_set_ps(-492.87f,  857.67f,   99.45f, -129.44f) },
    { simde_mm_set_ps( 213.75f,  969.89f, -341.00f,  -19.41f),
      simde_mm_set_ps( 773.33f, -228.51f,   68.57f, -153.07f),
      simde_mm_set_ps( 213.75f,  969.89f, -341.00f, -153.07f) },
    { simde_mm_set_ps(-482.05f, -169.03f, -647.88f, -151.80f),
      simde_mm_set_ps( 604.32f, -221.45f,  450.87f, -490.43f),
      simde_mm_set_ps(-482.05f, -169.03f, -647.88f, -490.43f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_min_ss(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_f32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_move_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps(-358.23f,  557.58f,  318.93f, -758.38f),
      simde_mm_set_ps(   1.51f,  975.39f, -136.78f, -157.41f),
      simde_mm_set_ps(-358.23f,  557.58f,  318.93f, -157.41f) },
    { simde_mm_set_ps(-944.08f, -768.00f,  457.67f,  835.58f),
      simde_mm_set_ps(-700.69f,  960.42f,  670.21f, -766.17f),
      simde_mm_set_ps(-944.08f, -768.00f,  457.67f, -766.17f) },
    { simde_mm_set_ps(-640.75f,   79.11f, -809.30f, -582.60f),
      simde_mm_set_ps( 451.92f,  260.70f, -368.72f, -418.36f),
      simde_mm_set_ps(-640.75f,   79.11f, -809.30f, -418.36f) },
    { simde_mm_set_ps(-265.37f, -906.15f,  463.48f,  857.51f),
      simde_mm_set_ps(  52.86f, -189.27f,  -89.79f,  636.22f),
      simde_mm_set_ps(-265.37f, -906.15f,  463.48f,  636.22f) },
    { simde_mm_set_ps( 627.30f, -419.51f,  242.55f, -669.89f),
      simde_mm_set_ps( 891.75f,  884.03f,  808.69f,   48.90f),
      simde_mm_set_ps( 627.30f, -419.51f,  242.55f,   48.90f) },
    { simde_mm_set_ps( 162.10f, -144.97f,  -36.34f,  747.42f),
      simde_mm_set_ps( 962.83f,  377.89f, -519.04f, -497.15f),
      simde_mm_set_ps( 162.10f, -144.97f,  -36.34f, -497.15f) },
    { simde_mm_set_ps(-230.32f,  536.55f, -396.11f,  274.97f),
      simde_mm_set_ps(-442.89f,  237.99f, -587.16f,  603.90f),
      simde_mm_set_ps(-230.32f,  536.55f, -396.11f,  603.90f) },
    { simde_mm_set_ps(-213.83f,  999.36f,  795.03f,  885.52f),
      simde_mm_set_ps(-878.99f, -162.39f,   89.22f, -749.67f),
      simde_mm_set_ps(-213.83f,  999.36f,  795.03f, -749.67f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_move_ss(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_f32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_movehl_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps( 448.42f,  812.28f, -569.58f, -612.98f),
      simde_mm_set_ps(-260.91f,  851.49f, -543.31f, -452.37f),
      simde_mm_set_ps( 448.42f,  812.28f, -260.91f,  851.49f) },
    { simde_mm_set_ps( 164.79f, -510.29f, -875.53f,  338.43f),
      simde_mm_set_ps( 195.22f, -222.31f, -247.28f, -544.38f),
      simde_mm_set_ps( 164.79f, -510.29f,  195.22f, -222.31f) },
    { simde_mm_set_ps( 632.26f, -381.04f, -465.25f, -913.48f),
      simde_mm_set_ps(-790.33f, -504.12f, -321.51f, -760.29f),
      simde_mm_set_ps( 632.26f, -381.04f, -790.33f, -504.12f) },
    { simde_mm_set_ps( 203.91f,  884.83f, -352.58f, -259.85f),
      simde_mm_set_ps(  92.13f,  448.80f,  494.59f,  -13.35f),
      simde_mm_set_ps( 203.91f,  884.83f,   92.13f,  448.80f) },
    { simde_mm_set_ps( 223.91f, -533.66f,  185.50f, -579.52f),
      simde_mm_set_ps(-316.81f, -862.60f, -895.66f,  129.46f),
      simde_mm_set_ps( 223.91f, -533.66f, -316.81f, -862.60f) },
    { simde_mm_set_ps(-731.11f,  221.64f,  388.77f,   -5.75f),
      simde_mm_set_ps(-220.42f,  -24.67f, -629.56f, -668.90f),
      simde_mm_set_ps(-731.11f,  221.64f, -220.42f,  -24.67f) },
    { simde_mm_set_ps(-367.65f,  429.37f,  435.99f,  954.93f),
      simde_mm_set_ps( 382.29f, -511.24f,  874.66f,  450.20f),
      simde_mm_set_ps(-367.65f,  429.37f,  382.29f, -511.24f) },
    { simde_mm_set_ps( 917.13f, -437.03f, -611.86f, -766.78f),
      simde_mm_set_ps( -43.20f, -568.30f,  -68.56f, -878.32f),
      simde_mm_set_ps( 917.13f, -437.03f,  -43.20f, -568.30f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_movehl_ps(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_f32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_movelh_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps(-126.61f, -366.61f,  342.01f, -290.15f),
      simde_mm_set_ps( 691.90f, -944.15f,   99.58f,  438.89f),
      simde_mm_set_ps(  99.58f,  438.89f,  342.01f, -290.15f) },
    { simde_mm_set_ps(-879.79f,  930.29f,  951.33f,  492.78f),
      simde_mm_set_ps( 600.74f, -198.87f,  936.84f,  313.69f),
      simde_mm_set_ps( 936.84f,  313.69f,  951.33f,  492.78f) },
    { simde_mm_set_ps(-467.81f, -802.41f,  821.13f, -596.55f),
      simde_mm_set_ps( 695.24f,  173.94f, -480.51f,  -41.27f),
      simde_mm_set_ps(-480.51f,  -41.27f,  821.13f, -596.55f) },
    { simde_mm_set_ps( 645.54f,  -99.35f, -669.53f, -518.75f),
      simde_mm_set_ps( 261.98f,   -2.77f,  -35.85f, -725.12f),
      simde_mm_set_ps( -35.85f, -725.12f, -669.53f, -518.75f) },
    { simde_mm_set_ps( 851.93f,  376.50f, -125.53f,  315.67f),
      simde_mm_set_ps( 722.06f, -287.10f,  806.63f, -831.38f),
      simde_mm_set_ps( 806.63f, -831.38f, -125.53f,  315.67f) },
    { simde_mm_set_ps(-180.42f, -861.51f,  293.97f,  929.27f),
      simde_mm_set_ps( -61.47f, -964.08f, -555.27f,  147.09f),
      simde_mm_set_ps(-555.27f,  147.09f,  293.97f,  929.27f) },
    { simde_mm_set_ps( 294.20f,   18.46f,  779.53f, -177.14f),
      simde_mm_set_ps( 664.57f,  349.92f,  797.65f,  206.26f),
      simde_mm_set_ps( 797.65f,  206.26f,  779.53f, -177.14f) },
    { simde_mm_set_ps(-737.06f, -946.48f, -251.45f, -808.64f),
      simde_mm_set_ps(-245.46f,  616.13f, -342.03f,  914.50f),
      simde_mm_set_ps(-342.03f,  914.50f, -251.45f, -808.64f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_movelh_ps(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_f32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_movemask_pi8(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m64 a;
    int r;
  } test_vec[8] = {
    { simde_mm_set_pi8(   7,  -33,    4,  -58,  -87,   16,   83,  -97),  89 },
    { simde_mm_set_pi8(  28,  -40,  -15, -114,  -71,  -97,  -12,  -53), 127 },
    { simde_mm_set_pi8( -16,  -80,   47,   37,   16, -111,  120,  -12), 197 },
    { simde_mm_set_pi8(   4,  -17,  -44,   -3,  -35,   81,  -87,   97), 122 },
    { simde_mm_set_pi8( -84,   23,   93,   30,   87,  114,   66,   94), 128 },
    { simde_mm_set_pi8(  -1,  -24,   -4,  -87,   33,   91,   32,   43), 240 },
    { simde_mm_set_pi8( -11,   96,  -68,   84,   53, -120,  124,   -4), 165 },
    { simde_mm_set_pi8(-122,   91,   -3,  -17,  -54,   62,  119,  -40), 185 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_movemask_pi8(test_vec[i].a);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_movemask_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    int r;
  } test_vec[8] = {
    { simde_m128_set_u32(INT32_C(UINT32_C(0xa67d815d)), INT32_C(UINT32_C(0x313ba9ba)), INT32_C(UINT32_C(0x21c24eef)), INT32_C(UINT32_C(0x423f8c9e))),   8 },
    { simde_m128_set_u32(INT32_C(UINT32_C(0x489edd7d)), INT32_C(UINT32_C(0x67cd5a03)), INT32_C(UINT32_C(0x615ae189)), INT32_C(UINT32_C(0x97259ce3))),   1 },
    { simde_m128_set_u32(INT32_C(UINT32_C(0xe28d2b70)), INT32_C(UINT32_C(0xe91a3281)), INT32_C(UINT32_C(0x73d2c004)), INT32_C(UINT32_C(0x7cc3587e))),  12 },
    { simde_m128_set_u32(INT32_C(UINT32_C(0xb38e9200)), INT32_C(UINT32_C(0xd86e4d45)), INT32_C(UINT32_C(0xd67c3858)), INT32_C(UINT32_C(0x6dd9c655))),  14 },
    { simde_m128_set_u32(INT32_C(UINT32_C(0x3324bf52)), INT32_C(UINT32_C(0x86f260cf)), INT32_C(UINT32_C(0x1c6c8682)), INT32_C(UINT32_C(0x53be68fe))),   4 },
    { simde_m128_set_u32(INT32_C(UINT32_C(0xf2018c61)), INT32_C(UINT32_C(0x250c57a7)), INT32_C(UINT32_C(0x0654d448)), INT32_C(UINT32_C(0x8a06fe60))),   9 },
    { simde_m128_set_u32(INT32_C(UINT32_C(0x0430e063)), INT32_C(UINT32_C(0x7ffc7ad3)), INT32_C(UINT32_C(0x9306516d)), INT32_C(UINT32_C(0x5896591c))),   2 },
    { simde_m128_set_u32(INT32_C(UINT32_C(0xfa68023e)), INT32_C(UINT32_C(0x2e799bce)), INT32_C(UINT32_C(0x88c4c4ea)), INT32_C(UINT32_C(0x31bc8ed8))),  10 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_movemask_ps(test_vec[i].a);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_mul_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const simde__m128 expected = simde_mm_set_ps(5.0f, 12.0f, 21.0f, 32.0f);

  simde__m128 a = simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f);
  simde__m128 b = simde_mm_set_ps(5.0f, 6.0f, 7.0f, 8.0f);
  simde__m128 r = simde_mm_mul_ps(a, b);

  assert_m128_ps(r, ==, expected);

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_mul_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const simde__m128 expected = simde_mm_set_ps(1.0f, 2.0f, 3.0f, 32.0f);

  simde__m128 a = simde_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f);
  simde__m128 b = simde_mm_set_ps(5.0f, 6.0f, 7.0f, 8.0f);
  simde__m128 r = simde_mm_mul_ss(a, b);

  assert_m128_ps(r, ==, expected);

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_mulhi_pu16(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m64 a;
    simde__m64 b;
    simde__m64 r;
  } test_vec[8] = {
    { simde_mm_set_pi16(INT16_C(   3302), INT16_C( -18418), INT16_C(  29552), INT16_C( -12249)),
      simde_mm_set_pi16(INT16_C(  20635), INT16_C(    976), INT16_C(  20801), INT16_C(  27768)),
      simde_mm_set_pi16(INT16_C(   1039), INT16_C(    701), INT16_C(   9379), INT16_C(  22578)) },
    { simde_mm_set_pi16(INT16_C( -16934), INT16_C( -18474), INT16_C( -28035), INT16_C(  13184)),
      simde_mm_set_pi16(INT16_C(  25135), INT16_C( -17972), INT16_C( -31326), INT16_C(  10493)),
      simde_mm_set_pi16(INT16_C(  18640), INT16_C( -31380), INT16_C(  19575), INT16_C(   2110)) },
    { simde_mm_set_pi16(INT16_C(  -5196), INT16_C( -21059), INT16_C(   6141), INT16_C(  10454)),
      simde_mm_set_pi16(INT16_C( -14498), INT16_C(   9860), INT16_C(   8552), INT16_C(    960)),
      simde_mm_set_pi16(INT16_C( -18545), INT16_C(   6691), INT16_C(    801), INT16_C(    153)) },
    { simde_mm_set_pi16(INT16_C( -17849), INT16_C(  -9969), INT16_C( -15056), INT16_C( -28520)),
      simde_mm_set_pi16(INT16_C( -24808), INT16_C( -26456), INT16_C( -26790), INT16_C(  -3426)),
      simde_mm_set_pi16(INT16_C(  29635), INT16_C( -32401), INT16_C(  29844), INT16_C( -30456)) },
    { simde_mm_set_pi16(INT16_C( -24762), INT16_C(   -824), INT16_C(  -2723), INT16_C( -32389)),
      simde_mm_set_pi16(INT16_C( -29024), INT16_C(  -1816), INT16_C( -22701), INT16_C(   6596)),
      simde_mm_set_pi16(INT16_C(  22716), INT16_C(  -2618), INT16_C( -24481), INT16_C(   3336)) },
    { simde_mm_set_pi16(INT16_C(  -1811), INT16_C( -27744), INT16_C( -22096), INT16_C( -20125)),
      simde_mm_set_pi16(INT16_C( -15991), INT16_C(   9594), INT16_C( -27730), INT16_C(  -1898)),
      simde_mm_set_pi16(INT16_C( -17361), INT16_C(   5532), INT16_C(  25059), INT16_C( -21441)) },
    { simde_mm_set_pi16(INT16_C(  -5432), INT16_C(  -5589), INT16_C(  25546), INT16_C(  28936)),
      simde_mm_set_pi16(INT16_C( -17981), INT16_C(   9819), INT16_C( -24686), INT16_C( -13285)),
      simde_mm_set_pi16(INT16_C( -21923), INT16_C(   8981), INT16_C(  15923), INT16_C(  23070)) },
    { simde_mm_set_pi16(INT16_C( -31421), INT16_C( -22219), INT16_C( -25658), INT16_C(   5034)),
      simde_mm_set_pi16(INT16_C(  11089), INT16_C(   1039), INT16_C( -18910), INT16_C(  31781)),
      simde_mm_set_pi16(INT16_C(   5772), INT16_C(    686), INT16_C(  28371), INT16_C(   2441)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m64 r = simde_mm_mulhi_pu16(test_vec[i].a, test_vec[i].b);
    simde_assert_m64_u16(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_or_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_m128_set_u32(UINT32_C(0x3f15cce0), UINT32_C(0xc9e102f7), UINT32_C(0x4ab44838), UINT32_C(0x5bcd6039)),
      simde_m128_set_u32(UINT32_C(0x15e48867), UINT32_C(0xd6298bba), UINT32_C(0x8405d277), UINT32_C(0x897f4e3e)),
      simde_m128_set_u32(UINT32_C(0x3ff5cce7), UINT32_C(0xdfe98bff), UINT32_C(0xceb5da7f), UINT32_C(0xdbff6e3f)) },
    { simde_m128_set_u32(UINT32_C(0xa184791e), UINT32_C(0x09fa0504), UINT32_C(0x3d22f3d7), UINT32_C(0xcac1099b)),
      simde_m128_set_u32(UINT32_C(0xd346da14), UINT32_C(0xa334ef18), UINT32_C(0x9edd6028), UINT32_C(0xfada120d)),
      simde_m128_set_u32(UINT32_C(0xf3c6fb1e), UINT32_C(0xabfeef1c), UINT32_C(0xbffff3ff), UINT32_C(0xfadb1b9f)) },
    { simde_m128_set_u32(UINT32_C(0x32a3f44e), UINT32_C(0xa6658097), UINT32_C(0xb5df14a2), UINT32_C(0x26f0f39a)),
      simde_m128_set_u32(UINT32_C(0x7cd48cb0), UINT32_C(0xcd3e8015), UINT32_C(0x9b14a112), UINT32_C(0xd25bc865)),
      simde_m128_set_u32(UINT32_C(0x7ef7fcfe), UINT32_C(0xef7f8097), UINT32_C(0xbfdfb5b2), UINT32_C(0xf6fbfbff)) },
    { simde_m128_set_u32(UINT32_C(0x31f7aa8c), UINT32_C(0x66e88f0b), UINT32_C(0xc0127212), UINT32_C(0x8a751789)),
      simde_m128_set_u32(UINT32_C(0xaf254e63), UINT32_C(0xe25dc364), UINT32_C(0x1af40823), UINT32_C(0xba4c2120)),
      simde_m128_set_u32(UINT32_C(0xbff7eeef), UINT32_C(0xe6fdcf6f), UINT32_C(0xdaf67a33), UINT32_C(0xba7d37a9)) },
    { simde_m128_set_u32(UINT32_C(0x5e7cc730), UINT32_C(0x88243815), UINT32_C(0x35f22425), UINT32_C(0xb46c9b6d)),
      simde_m128_set_u32(UINT32_C(0x30058d37), UINT32_C(0xea0ed70a), UINT32_C(0x7e38690b), UINT32_C(0x872e2bec)),
      simde_m128_set_u32(UINT32_C(0x7e7dcf37), UINT32_C(0xea2eff1f), UINT32_C(0x7ffa6d2f), UINT32_C(0xb76ebbed)) },
    { simde_m128_set_u32(UINT32_C(0xb3361fea), UINT32_C(0xd7e3ebea), UINT32_C(0x049a4d7b), UINT32_C(0xa4ddb93c)),
      simde_m128_set_u32(UINT32_C(0xeb98f3e1), UINT32_C(0x4d469573), UINT32_C(0x0edab5c9), UINT32_C(0x56e22997)),
      simde_m128_set_u32(UINT32_C(0xfbbeffeb), UINT32_C(0xdfe7fffb), UINT32_C(0x0edafdfb), UINT32_C(0xf6ffb9bf)) },
    { simde_m128_set_u32(UINT32_C(0x836329d3), UINT32_C(0x7c88cc2e), UINT32_C(0x22049925), UINT32_C(0x67726596)),
      simde_m128_set_u32(UINT32_C(0xdf253de0), UINT32_C(0x0fd75660), UINT32_C(0x0f23b348), UINT32_C(0x351290d1)),
      simde_m128_set_u32(UINT32_C(0xdf673df3), UINT32_C(0x7fdfde6e), UINT32_C(0x2f27bb6d), UINT32_C(0x7772f5d7)) },
    { simde_m128_set_u32(UINT32_C(0x7f123165), UINT32_C(0x98931e26), UINT32_C(0x7fc611a1), UINT32_C(0xf9b4e373)),
      simde_m128_set_u32(UINT32_C(0xb4dd8bb4), UINT32_C(0x3a9ec4d4), UINT32_C(0x4aa2e147), UINT32_C(0x9b090515)),
      simde_m128_set_u32(UINT32_C(0xffdfbbf5), UINT32_C(0xba9fdef6), UINT32_C(0x7fe6f1e7), UINT32_C(0xfbbde777)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_or_ps(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_u32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_rcp_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps(-8.23f, -5.18f,  3.66f, -3.55f),
      simde_mm_set_ps(-0.12f, -0.19f,  0.27f, -0.28f) },
    { simde_mm_set_ps( 5.27f,  1.94f, -1.37f,  3.59f),
      simde_mm_set_ps( 0.19f,  0.52f, -0.73f,  0.28f) },
    { simde_mm_set_ps( 1.70f, -6.14f,  4.86f,  1.14f),
      simde_mm_set_ps( 0.59f, -0.16f,  0.21f,  0.88f) },
    { simde_mm_set_ps(-6.46f,  0.31f,  0.95f,  1.02f),
      simde_mm_set_ps(-0.15f,  3.25f,  1.05f,  0.98f) },
    { simde_mm_set_ps( 0.27f,  3.55f, -8.33f, -7.65f),
      simde_mm_set_ps( 3.70f,  0.28f, -0.12f, -0.13f) },
    { simde_mm_set_ps( 3.04f, -3.61f,  1.60f, -3.92f),
      simde_mm_set_ps( 0.33f, -0.28f,  0.62f, -0.25f) },
    { simde_mm_set_ps( 4.81f,  3.50f,  6.45f,  9.52f),
      simde_mm_set_ps( 0.21f,  0.29f,  0.15f,  0.11f) },
    { simde_mm_set_ps( 6.39f,  6.57f, -0.50f,  6.01f),
      simde_mm_set_ps( 0.16f,  0.15f, -2.00f,  0.17f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_rcp_ps(test_vec[i].a);
    simde_assert_m128_close(r, test_vec[i].r, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_rcp_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps(-0.17f, -5.82f,  9.03f,  0.51f),
      simde_mm_set_ps(-0.17f, -5.82f,  9.03f,  1.97f) },
    { simde_mm_set_ps( 3.71f,  8.82f,  3.74f, -1.45f),
      simde_mm_set_ps( 3.71f,  8.82f,  3.74f, -0.69f) },
    { simde_mm_set_ps( 6.34f, -2.54f, -3.13f, -5.87f),
      simde_mm_set_ps( 6.34f, -2.54f, -3.13f, -0.17f) },
    { simde_mm_set_ps(-3.12f,  8.32f,  6.67f,  6.69f),
      simde_mm_set_ps(-3.12f,  8.32f,  6.67f,  0.15f) },
    { simde_mm_set_ps( 8.74f,  8.53f,  5.33f,  6.71f),
      simde_mm_set_ps( 8.74f,  8.53f,  5.33f,  0.15f) },
    { simde_mm_set_ps( 8.56f, -4.33f,  4.16f, -1.33f),
      simde_mm_set_ps( 8.56f, -4.33f,  4.16f, -0.75f) },
    { simde_mm_set_ps( 0.83f, -2.25f, -0.87f,  8.44f),
      simde_mm_set_ps( 0.83f, -2.25f, -0.87f,  0.12f) },
    { simde_mm_set_ps( 0.99f,  5.65f, -2.23f,  1.17f),
      simde_mm_set_ps( 0.99f,  5.65f, -2.23f,  0.85f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_rcp_ss(test_vec[i].a);
    simde_assert_m128_close(r, test_vec[i].r, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_rsqrt_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps( 19.76f,  43.05f,  24.93f,  44.79f),
      simde_mm_set_ps(  0.22f,   0.15f,   0.20f,   0.15f) },
    { simde_mm_set_ps( 53.16f,  37.63f,  14.64f,  91.38f),
      simde_mm_set_ps(  0.14f,   0.16f,   0.26f,   0.10f) },
    { simde_mm_set_ps( 51.45f,  20.86f,  31.69f,  22.93f),
      simde_mm_set_ps(  0.14f,   0.22f,   0.18f,   0.21f) },
    { simde_mm_set_ps( 70.34f,  27.96f,  47.70f,  68.63f),
      simde_mm_set_ps(  0.12f,   0.19f,   0.14f,   0.12f) },
    { simde_mm_set_ps( 15.37f,  83.67f,  71.19f,  29.53f),
      simde_mm_set_ps(  0.26f,   0.11f,   0.12f,   0.18f) },
    { simde_mm_set_ps( 54.38f,   5.48f,  29.73f,  69.45f),
      simde_mm_set_ps(  0.14f,   0.43f,   0.18f,   0.12f) },
    { simde_mm_set_ps( 84.04f,  25.31f,  28.88f,  94.95f),
      simde_mm_set_ps(  0.11f,   0.20f,   0.19f,   0.10f) },
    { simde_mm_set_ps( 62.50f,   1.25f,  73.97f,  57.92f),
      simde_mm_set_ps(  0.13f,   0.90f,   0.12f,   0.13f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_rsqrt_ps(test_vec[i].a);
    simde_assert_m128_close(r, test_vec[i].r, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_rsqrt_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps( 19.76f,  43.05f,  24.93f,  44.79f),
      simde_mm_set_ps( 19.76f,  43.05f,  24.93f,   0.15f) },
    { simde_mm_set_ps( 53.16f,  37.63f,  14.64f,  91.38f),
      simde_mm_set_ps( 53.16f,  37.63f,  14.64f,   0.10f) },
    { simde_mm_set_ps( 51.45f,  20.86f,  31.69f,  22.93f),
      simde_mm_set_ps( 51.45f,  20.86f,  31.69f,   0.21f) },
    { simde_mm_set_ps( 70.34f,  27.96f,  47.70f,  68.63f),
      simde_mm_set_ps( 70.34f,  27.96f,  47.70f,   0.12f) },
    { simde_mm_set_ps( 15.37f,  83.67f,  71.19f,  29.53f),
      simde_mm_set_ps( 15.37f,  83.67f,  71.19f,   0.18f) },
    { simde_mm_set_ps( 54.38f,   5.48f,  29.73f,  69.45f),
      simde_mm_set_ps( 54.38f,   5.48f,  29.73f,   0.12f) },
    { simde_mm_set_ps( 84.04f,  25.31f,  28.88f,  94.95f),
      simde_mm_set_ps( 84.04f,  25.31f,  28.88f,   0.10f) },
    { simde_mm_set_ps( 62.50f,   1.25f,  73.97f,  57.92f),
      simde_mm_set_ps( 62.50f,   1.25f,  73.97f,   0.13f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_rsqrt_ss(test_vec[i].a);
    simde_assert_m128_close(r, test_vec[i].r, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_sad_pu8(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m64 a;
    simde__m64 b;
    simde__m64 r;
  } test_vec[8] = {
    { simde_x_mm_set_pu8(158,  38, 204, 230, 242, 108, 135, 100),
      simde_x_mm_set_pu8(130, 168, 102, 233, 237, 176,  22, 158),
      simde_x_mm_set_pu16(0, 0, 0, 507) },
    { simde_x_mm_set_pu8( 15, 252, 176, 193, 115,  44,   0,  83),
      simde_x_mm_set_pu8( 99, 169,  76, 203, 218, 181, 138, 226),
      simde_x_mm_set_pu16(0, 0, 0, 798) },
    { simde_x_mm_set_pu8(230,  50, 152, 234, 252,  79, 170, 145),
      simde_x_mm_set_pu8(225, 219, 116, 170, 250, 129, 102, 178),
      simde_x_mm_set_pu16(0, 0, 0, 427) },
    { simde_x_mm_set_pu8( 77, 112,  20, 247, 206, 117, 128, 107),
      simde_x_mm_set_pu8(189, 223, 203, 181,  71, 239,  64, 186),
      simde_x_mm_set_pu16(0, 0, 0, 872) },
    { simde_x_mm_set_pu8(128, 104,  93, 138, 250, 105, 219, 255),
      simde_x_mm_set_pu8(113, 248, 217,  59,  72,   4, 165,  83),
      simde_x_mm_set_pu16(0, 0, 0, 867) },
    { simde_x_mm_set_pu8(143,  12,  71,  81, 251, 175,  44, 206),
      simde_x_mm_set_pu8( 80, 100, 129,  82,  59,  63,  26,  22),
      simde_x_mm_set_pu16(0, 0, 0, 716) },
    { simde_x_mm_set_pu8(  7, 202, 222,  71, 138,  18, 223,  92),
      simde_x_mm_set_pu8(208, 174,  15, 221,  13,  93, 209, 116),
      simde_x_mm_set_pu16(0, 0, 0, 824) },
    { simde_x_mm_set_pu8( 92, 133, 132,   0,  24, 132, 201, 186),
      simde_x_mm_set_pu8(194,  29, 160,  58,  50,  10,  65, 234),
      simde_x_mm_set_pu16(0, 0, 0, 624) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / sizeof(test_vec[0])); i++) {
    simde__m64 r = simde_mm_sad_pu8(test_vec[i].a, test_vec[i].b);
    simde_assert_m64_u16(r, ==, test_vec[i].r);
  }

  simde_mm_empty();
  return MUNIT_OK;
}

static MunitResult
test_simde_mm_shuffle_pi16(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m64 a;
    simde__m64 r;
  } test_vec[8] = {
    { simde_mm_set_pi16( 20374,  -8020,   9831, -21724),
      simde_mm_set_pi16(-21724, -21724,   9831,   9831) },
    { simde_mm_set_pi16( 26825,   6867,  -1457,  28819),
      simde_mm_set_pi16( 28819,  28819,  -1457,  -1457) },
    { simde_mm_set_pi16( -4347, -12641,  -8333, -18450),
      simde_mm_set_pi16(-18450, -18450,  -8333,  -8333) },
    { simde_mm_set_pi16( 22439,  23179, -32421,  -3266),
      simde_mm_set_pi16( -3266,  -3266, -32421, -32421) },
    { simde_mm_set_pi16(  9337,  -3310,  22225, -14472),
      simde_mm_set_pi16(-14472, -14472,  22225,  22225) },
    { simde_mm_set_pi16(-17114, -15656,  26827,  -1486),
      simde_mm_set_pi16( -1486,  -1486,  26827,  26827) },
    { simde_mm_set_pi16(  8123,   8758,  31545,  -8216),
      simde_mm_set_pi16( -8216,  -8216,  31545,  31545) },
    { simde_mm_set_pi16(-32324,  31163,  -3386,  23646),
      simde_mm_set_pi16( 23646,  23646,  -3386,  -3386) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m64 r = simde_mm_shuffle_pi16(test_vec[i].a, 5);
    simde_assert_m64_i16(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_shuffle_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps( 387.45f, -469.79f,  719.43f,  371.94f),
      simde_mm_set_ps( 641.56f,  341.35f,  292.84f,  441.22f),
      simde_mm_set_ps( 441.22f,  441.22f, -469.79f,  387.45f) },
    { simde_mm_set_ps( 648.82f,  641.81f, -789.10f,  982.80f),
      simde_mm_set_ps( 472.27f, -304.33f,  524.09f, -589.31f),
      simde_mm_set_ps(-589.31f, -589.31f,  641.81f,  648.82f) },
    { simde_mm_set_ps(-163.67f, -311.30f, -600.60f,  597.71f),
      simde_mm_set_ps(-247.76f,  246.42f, -742.25f,  -20.93f),
      simde_mm_set_ps( -20.93f,  -20.93f, -311.30f, -163.67f) },
    { simde_mm_set_ps( 968.74f,  810.41f, -699.53f,  224.20f),
      simde_mm_set_ps(-966.41f,  917.94f, -300.26f,   64.06f),
      simde_mm_set_ps(  64.06f,   64.06f,  810.41f,  968.74f) },
    { simde_mm_set_ps(  99.15f,  957.94f,  380.12f, -611.50f),
      simde_mm_set_ps( -77.49f, -255.84f,  787.35f, -671.91f),
      simde_mm_set_ps(-671.91f, -671.91f,  957.94f,   99.15f) },
    { simde_mm_set_ps(-280.55f, -182.50f,  340.17f,  473.64f),
      simde_mm_set_ps(  -3.29f, -413.78f, -406.24f,  521.82f),
      simde_mm_set_ps( 521.82f,  521.82f, -182.50f, -280.55f) },
    { simde_mm_set_ps(-677.92f,  481.01f,  494.26f,  565.24f),
      simde_mm_set_ps( 205.66f,  769.40f, -900.58f, -847.82f),
      simde_mm_set_ps(-847.82f, -847.82f,  481.01f, -677.92f) },
    { simde_mm_set_ps( 703.71f,  397.64f,  773.55f, -739.53f),
      simde_mm_set_ps(  99.18f, -932.61f, -902.04f,  169.61f),
      simde_mm_set_ps( 169.61f,  169.61f,  397.64f,  703.71f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_shuffle_ps(test_vec[i].a, test_vec[i].b, 11);
    simde_assert_m128_f32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_sqrt_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps( 407.59f,  875.44f,  929.81f,  658.11f),
      simde_mm_set_ps(  20.19f,   29.59f,   30.49f,   25.65f) },
    { simde_mm_set_ps( 511.62f,  750.84f,  820.58f,  601.07f),
      simde_mm_set_ps(  22.62f,   27.40f,   28.65f,   24.52f) },
    { simde_mm_set_ps(  67.49f,  303.60f,  282.76f,  795.53f),
      simde_mm_set_ps(   8.22f,   17.42f,   16.82f,   28.21f) },
    { simde_mm_set_ps( 777.54f,   62.15f,  901.98f,  524.87f),
      simde_mm_set_ps(  27.88f,    7.88f,   30.03f,   22.91f) },
    { simde_mm_set_ps( 397.99f,  291.75f,  558.37f,  417.44f),
      simde_mm_set_ps(  19.95f,   17.08f,   23.63f,   20.43f) },
    { simde_mm_set_ps( 855.95f,  696.25f,  826.76f,  664.95f),
      simde_mm_set_ps(  29.26f,   26.39f,   28.75f,   25.79f) },
    { simde_mm_set_ps( 435.38f,  218.65f,  679.39f,  587.73f),
      simde_mm_set_ps(  20.87f,   14.79f,   26.07f,   24.24f) },
    { simde_mm_set_ps( 384.56f,  314.94f,   34.58f,   10.06f),
      simde_mm_set_ps(  19.61f,   17.75f,    5.88f,    3.17f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_sqrt_ps(test_vec[i].a);
    simde_assert_m128_f32_close(r, test_vec[i].r, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_sqrt_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps( 619.69f,  412.13f,  497.92f,  538.00f),
      simde_mm_set_ps( 619.69f,  412.13f,  497.92f,   23.19f) },
    { simde_mm_set_ps( 620.13f,  731.84f,  667.03f,  801.31f),
      simde_mm_set_ps( 620.13f,  731.84f,  667.03f,   28.31f) },
    { simde_mm_set_ps( 720.02f,  314.91f,  596.04f,  727.65f),
      simde_mm_set_ps( 720.02f,  314.91f,  596.04f,   26.97f) },
    { simde_mm_set_ps( 888.04f,  213.48f,  907.68f,  515.93f),
      simde_mm_set_ps( 888.04f,  213.48f,  907.68f,   22.71f) },
    { simde_mm_set_ps( 196.63f,  888.92f,  163.58f,  421.90f),
      simde_mm_set_ps( 196.63f,  888.92f,  163.58f,   20.54f) },
    { simde_mm_set_ps( 966.24f,  260.28f,  707.39f,  467.81f),
      simde_mm_set_ps( 966.24f,  260.28f,  707.39f,   21.63f) },
    { simde_mm_set_ps( 762.80f,  805.99f,  106.70f,  460.44f),
      simde_mm_set_ps( 762.80f,  805.99f,  106.70f,   21.46f) },
    { simde_mm_set_ps( 398.96f,  926.80f,  642.37f,  392.46f),
      simde_mm_set_ps( 398.96f,  926.80f,  642.37f,   19.81f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_sqrt_ss(test_vec[i].a);
    simde_assert_m128_close(r, test_vec[i].r, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_store_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    SIMDE_ALIGN(16) simde_float32 r[4];
  } test_vec[8] = {
    { simde_mm_set_ps( 797.84f,  342.63f,  173.26f,  427.65f),
      {  427.65f,  173.26f,  342.63f,  797.84f } },
    { simde_mm_set_ps( 911.58f,  127.96f,   79.17f,    4.20f),
      {    4.20f,   79.17f,  127.96f,  911.58f } },
    { simde_mm_set_ps( 206.93f,  418.54f,  921.68f,  840.36f),
      {  840.36f,  921.68f,  418.54f,  206.93f } },
    { simde_mm_set_ps( 692.59f,   90.67f,  787.67f,  591.30f),
      {  591.30f,  787.67f,   90.67f,  692.59f } },
    { simde_mm_set_ps(  57.64f,   25.24f,  486.10f,  797.60f),
      {  797.60f,  486.10f,   25.24f,   57.64f } },
    { simde_mm_set_ps( 987.53f,  693.84f,  633.62f,  186.39f),
      {  186.39f,  633.62f,  693.84f,  987.53f } },
    { simde_mm_set_ps( 772.05f,  227.52f,  175.91f,  879.34f),
      {  879.34f,  175.91f,  227.52f,  772.05f } },
    { simde_mm_set_ps( 640.49f,  596.15f,  891.32f,   37.15f),
      {   37.15f,  891.32f,  596.15f,  640.49f } }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    SIMDE_ALIGN(16) simde_float32 r[4];
    simde_mm_store_ps(r, test_vec[i].a);
    for (size_t j = 0 ; j < 4 ; j++) {
      munit_assert_float(r[j], ==, test_vec[i].r[j]);
    }
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_store_ps1(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    SIMDE_ALIGN(16) simde_float32 r[4];
  } test_vec[8] = {
    { simde_mm_set_ps( 854.57f,  299.85f,   53.35f,  467.01f),
      {  467.01f,  467.01f,  467.01f,  467.01f } },
    { simde_mm_set_ps( 477.02f,  854.94f,  677.55f,  339.79f),
      {  339.79f,  339.79f,  339.79f,  339.79f } },
    { simde_mm_set_ps( 736.99f,  148.01f,  536.77f,  621.38f),
      {  621.38f,  621.38f,  621.38f,  621.38f } },
    { simde_mm_set_ps( 947.26f,  901.13f,  502.63f,  117.54f),
      {  117.54f,  117.54f,  117.54f,  117.54f } },
    { simde_mm_set_ps( 193.39f,  545.92f,  514.75f,  687.45f),
      {  687.45f,  687.45f,  687.45f,  687.45f } },
    { simde_mm_set_ps( 844.08f,  252.82f,  783.61f,  372.00f),
      {  372.00f,  372.00f,  372.00f,  372.00f } },
    { simde_mm_set_ps( 988.23f,  778.23f,  199.92f,  424.51f),
      {  424.51f,  424.51f,  424.51f,  424.51f } },
    { simde_mm_set_ps( 515.70f,  944.74f,  777.26f,  470.44f),
      {  470.44f,  470.44f,  470.44f,  470.44f } }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    SIMDE_ALIGN(16) simde_float32 r[4];
    simde_mm_store_ps1(r, test_vec[i].a);
    for (size_t j = 0 ; j < 4 ; j++) {
      munit_assert_float(r[j], ==, test_vec[i].r[j]);
    }
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_store_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde_float32 r[4];
  } test_vec[8] = {
    { simde_mm_set_ps( 854.57f,  299.85f,   53.35f,  467.01f),
      {  467.01f,    0.00f,    0.00f,    0.00f } },
    { simde_mm_set_ps( 477.02f,  854.94f,  677.55f,  339.79f),
      {  339.79f,    0.00f,    0.00f,    0.00f } },
    { simde_mm_set_ps( 736.99f,  148.01f,  536.77f,  621.38f),
      {  621.38f,    0.00f,    0.00f,    0.00f } },
    { simde_mm_set_ps( 947.26f,  901.13f,  502.63f,  117.54f),
      {  117.54f,    0.00f,    0.00f,    0.00f } },
    { simde_mm_set_ps( 193.39f,  545.92f,  514.75f,  687.45f),
      {  687.45f,    0.00f,    0.00f,    0.00f } },
    { simde_mm_set_ps( 844.08f,  252.82f,  783.61f,  372.00f),
      {  372.00f,    0.00f,    0.00f,    0.00f } },
    { simde_mm_set_ps( 988.23f,  778.23f,  199.92f,  424.51f),
      {  424.51f,    0.00f,    0.00f,    0.00f } },
    { simde_mm_set_ps( 515.70f,  944.74f,  777.26f,  470.44f),
      {  470.44f,    0.00f,    0.00f,    0.00f } }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde_float32 r[4] = { 0, };
    simde_mm_store_ss(r, test_vec[i].a);
    for (size_t j = 0 ; j < 4 ; j++) {
      munit_assert_float(r[j], ==, test_vec[i].r[j]);
    }
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_store1_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    SIMDE_ALIGN(16) simde_float32 r[4];
  } test_vec[8] = {
    { simde_mm_set_ps( 274.23f,   89.27f,  784.72f,  646.53f),
      {  646.53f,  646.53f,  646.53f,  646.53f } },
    { simde_mm_set_ps( 322.23f,  913.84f,  883.41f,  484.76f),
      {  484.76f,  484.76f,  484.76f,  484.76f } },
    { simde_mm_set_ps( 272.81f,  739.51f,  776.03f,   57.85f),
      {   57.85f,   57.85f,   57.85f,   57.85f } },
    { simde_mm_set_ps( 414.64f,  733.36f,  680.70f,  973.86f),
      {  973.86f,  973.86f,  973.86f,  973.86f } },
    { simde_mm_set_ps( 541.43f,  860.27f,  162.49f,  726.46f),
      {  726.46f,  726.46f,  726.46f,  726.46f } },
    { simde_mm_set_ps( 735.30f,  924.35f,   18.46f,  634.38f),
      {  634.38f,  634.38f,  634.38f,  634.38f } },
    { simde_mm_set_ps( 676.23f,   61.60f,   24.97f,  803.79f),
      {  803.79f,  803.79f,  803.79f,  803.79f } },
    { simde_mm_set_ps( 373.81f,  509.18f,  233.96f,  910.43f),
      {  910.43f,  910.43f,  910.43f,  910.43f } }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    SIMDE_ALIGN(16) simde_float32 r[4] = { 0, };
    simde_mm_store1_ps(r, test_vec[i].a);
    for (size_t j = 0 ; j < 4 ; j++) {
      munit_assert_float(r[j], ==, test_vec[i].r[j]);
    }
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_storeh_pi(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m64 r;
  } test_vec[8] = {
    { simde_mm_set_ps( 575.54f,  220.03f,  206.01f,  870.81f),
      { .f32 = {  220.03f,  575.54f } } },
    { simde_mm_set_ps( 289.58f,  629.51f,  767.25f,  704.01f),
      { .f32 = {  629.51f,  289.58f } } },
    { simde_mm_set_ps( 627.14f,  949.08f,  581.33f,  434.65f),
      { .f32 = {  949.08f,  627.14f } } },
    { simde_mm_set_ps( 369.75f,  459.24f,  702.99f,   90.66f),
      { .f32 = {  459.24f,  369.75f } } },
    { simde_mm_set_ps(  57.99f,  910.00f,  605.74f,   76.21f),
      { .f32 = {  910.00f,   57.99f } } },
    { simde_mm_set_ps( 918.98f,  456.74f,  224.68f,  627.20f),
      { .f32 = {  456.74f,  918.98f } } },
    { simde_mm_set_ps( 963.09f,  356.12f,   33.23f,  995.23f),
      { .f32 = {  356.12f,  963.09f } } },
    { simde_mm_set_ps( 967.89f,  231.25f,  230.84f,  538.31f),
      { .f32 = {  231.25f,  967.89f } } }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m64 r;
    simde_mm_storeh_pi(&r, test_vec[i].a);
    for (size_t j = 0 ; j < 2 ; j++) {
      munit_assert_float(r.f32[j], ==, test_vec[i].r.f32[j]);
    }
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_storel_pi(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m64 r;
  } test_vec[8] = {
    { simde_mm_set_ps( 258.22f,  461.17f,  500.19f,  792.00f),
      { .f32 = {  792.00f,  500.19f } } },
    { simde_mm_set_ps( 839.05f,  239.15f,  886.65f,  576.97f),
      { .f32 = {  576.97f,  886.65f } } },
    { simde_mm_set_ps( 905.98f,  580.29f,   37.85f,  782.47f),
      { .f32 = {  782.47f,   37.85f } } },
    { simde_mm_set_ps( 367.06f,  905.12f,  385.52f,  288.25f),
      { .f32 = {  288.25f,  385.52f } } },
    { simde_mm_set_ps( 345.47f,   91.39f,  757.41f,  177.58f),
      { .f32 = {  177.58f,  757.41f } } },
    { simde_mm_set_ps(  41.60f,  601.66f,  873.51f,  499.62f),
      { .f32 = {  499.62f,  873.51f } } },
    { simde_mm_set_ps( 185.64f,  301.30f,   85.92f,  684.99f),
      { .f32 = {  684.99f,   85.92f } } },
    { simde_mm_set_ps( 556.83f,  958.68f,  557.15f,  788.14f),
      { .f32 = {  788.14f,  557.15f } } }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m64 r;
    simde_mm_storel_pi(&r, test_vec[i].a);
    for (size_t j = 0 ; j < 2 ; j++) {
      munit_assert_float(r.f32[j], ==, test_vec[i].r.f32[j]);
    }
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_storer_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    SIMDE_ALIGN(16) simde_float32 r[4];
  } test_vec[8] = {
    { simde_mm_set_ps( 709.97f,  746.23f,  453.60f,  303.28f),
      {  709.97f,  746.23f,  453.60f,  303.28f } },
    { simde_mm_set_ps( 764.16f,  229.13f,  431.64f,  384.08f),
      {  764.16f,  229.13f,  431.64f,  384.08f } },
    { simde_mm_set_ps( 237.88f,  156.25f,  582.89f,   91.67f),
      {  237.88f,  156.25f,  582.89f,   91.67f } },
    { simde_mm_set_ps( 244.82f,   86.89f,  868.91f,  106.59f),
      {  244.82f,   86.89f,  868.91f,  106.59f } },
    { simde_mm_set_ps( 570.53f,  397.46f,  850.83f,  961.86f),
      {  570.53f,  397.46f,  850.83f,  961.86f } },
    { simde_mm_set_ps( 946.89f,  801.12f,  881.81f,  752.94f),
      {  946.89f,  801.12f,  881.81f,  752.94f } },
    { simde_mm_set_ps( 779.85f,   31.75f,  218.17f,  299.71f),
      {  779.85f,   31.75f,  218.17f,  299.71f } },
    { simde_mm_set_ps( 409.61f,  712.65f,  619.44f,  952.97f),
      {  409.61f,  712.65f,  619.44f,  952.97f } }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    SIMDE_ALIGN(16) simde_float32 r[4] = { 0, };
    simde_mm_storer_ps(r, test_vec[i].a);
    for (size_t j = 0 ; j < 4 ; j++) {
      munit_assert_float(r[j], ==, test_vec[i].r[j]);
    }
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_storeu_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde_float32 r[4];
  } test_vec[8] = {
    { simde_mm_set_ps( 304.73f,  535.11f,   34.80f,  759.84f),
      {  759.84f,   34.80f,  535.11f,  304.73f } },
    { simde_mm_set_ps( 360.59f,  120.47f,  501.36f,  116.79f),
      {  116.79f,  501.36f,  120.47f,  360.59f } },
    { simde_mm_set_ps( 709.31f,  269.06f,  586.46f,  622.65f),
      {  622.65f,  586.46f,  269.06f,  709.31f } },
    { simde_mm_set_ps( 653.72f,  295.37f,   94.24f,  886.62f),
      {  886.62f,   94.24f,  295.37f,  653.72f } },
    { simde_mm_set_ps(   4.93f,  676.49f,  303.89f,  920.73f),
      {  920.73f,  303.89f,  676.49f,    4.93f } },
    { simde_mm_set_ps( 930.44f,  841.16f,  891.12f,  349.34f),
      {  349.34f,  891.12f,  841.16f,  930.44f } },
    { simde_mm_set_ps( 899.13f,  242.56f,  161.77f,   99.90f),
      {   99.90f,  161.77f,  242.56f,  899.13f } },
    { simde_mm_set_ps( 350.94f,  738.74f,  750.24f,  329.42f),
      {  329.42f,  750.24f,  738.74f,  350.94f } }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde_float32 r[4] = { 0, };
    simde_mm_storeu_ps(r, test_vec[i].a);
    for (size_t j = 0 ; j < 4 ; j++) {
      munit_assert_float(r[j], ==, test_vec[i].r[j]);
    }
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_sub_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps(-670.13f, -257.04f,  449.06f,  965.86f),
      simde_mm_set_ps(-637.34f,  741.89f,  -14.02f,  918.72f),
      simde_mm_set_ps( -32.80f, -998.93f,  463.07f,   47.13f) },
    { simde_mm_set_ps(-744.89f,  -52.84f,  460.90f,  134.66f),
      simde_mm_set_ps( 177.89f, -904.94f,  443.55f, -742.83f),
      simde_mm_set_ps(-922.78f,  852.10f,   17.35f,  877.49f) },
    { simde_mm_set_ps( 807.56f,  945.11f,  259.44f,  557.41f),
      simde_mm_set_ps( 404.99f,  399.93f,  -68.79f,  957.61f),
      simde_mm_set_ps( 402.57f,  545.19f,  328.23f, -400.20f) },
    { simde_mm_set_ps(  74.77f,  473.53f, -483.74f,  415.65f),
      simde_mm_set_ps(-608.38f,  553.08f, -146.88f,   64.98f),
      simde_mm_set_ps( 683.15f,  -79.55f, -336.86f,  350.67f) },
    { simde_mm_set_ps( 879.68f, -763.80f,  -53.30f,   95.40f),
      simde_mm_set_ps( 164.85f,  427.62f,  174.59f,  925.02f),
      simde_mm_set_ps( 714.82f, -1191.42f, -227.89f, -829.62f) },
    { simde_mm_set_ps( 571.32f,  207.75f,  439.31f, -973.48f),
      simde_mm_set_ps(-719.88f, -346.38f, -249.12f, -239.62f),
      simde_mm_set_ps(1291.20f,  554.13f,  688.43f, -733.86f) },
    { simde_mm_set_ps( 995.34f, -761.80f, -600.36f,  665.89f),
      simde_mm_set_ps( 578.64f, -853.98f,  329.02f, -360.51f),
      simde_mm_set_ps( 416.70f,   92.18f, -929.38f, 1026.41f) },
    { simde_mm_set_ps(-961.63f,  818.44f, -364.30f, -740.41f),
      simde_mm_set_ps(  86.59f,  540.64f,  243.58f,  458.04f),
      simde_mm_set_ps(-1048.22f,  277.80f, -607.89f, -1198.46f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_sub_ps(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_close(r, test_vec[i].r, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_sub_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps(-670.13f, -257.04f,  449.06f,  965.86f),
      simde_mm_set_ps(-637.34f,  741.89f,  -14.02f,  918.72f),
      simde_mm_set_ps(-670.13f, -257.04f,  449.06f,   47.13f) },
    { simde_mm_set_ps(-744.89f,  -52.84f,  460.90f,  134.66f),
      simde_mm_set_ps( 177.89f, -904.94f,  443.55f, -742.83f),
      simde_mm_set_ps(-744.89f,  -52.84f,  460.90f,  877.49f) },
    { simde_mm_set_ps( 807.56f,  945.11f,  259.44f,  557.41f),
      simde_mm_set_ps( 404.99f,  399.93f,  -68.79f,  957.61f),
      simde_mm_set_ps( 807.56f,  945.11f,  259.44f, -400.20f) },
    { simde_mm_set_ps(  74.77f,  473.53f, -483.74f,  415.65f),
      simde_mm_set_ps(-608.38f,  553.08f, -146.88f,   64.98f),
      simde_mm_set_ps(  74.77f,  473.53f, -483.74f,  350.67f) },
    { simde_mm_set_ps( 879.68f, -763.80f,  -53.30f,   95.40f),
      simde_mm_set_ps( 164.85f,  427.62f,  174.59f,  925.02f),
      simde_mm_set_ps( 879.68f, -763.80f,  -53.30f, -829.62f) },
    { simde_mm_set_ps( 571.32f,  207.75f,  439.31f, -973.48f),
      simde_mm_set_ps(-719.88f, -346.38f, -249.12f, -239.62f),
      simde_mm_set_ps( 571.32f,  207.75f,  439.31f, -733.86f) },
    { simde_mm_set_ps( 995.34f, -761.80f, -600.36f,  665.89f),
      simde_mm_set_ps( 578.64f, -853.98f,  329.02f, -360.51f),
      simde_mm_set_ps( 995.34f, -761.80f, -600.36f, 1026.41f) },
    { simde_mm_set_ps(-961.63f,  818.44f, -364.30f, -740.41f),
      simde_mm_set_ps(  86.59f,  540.64f,  243.58f,  458.04f),
      simde_mm_set_ps(-961.63f,  818.44f, -364.30f, -1198.46f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_sub_ss(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_close(r, test_vec[i].r, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_ucomieq_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    int r;
  } test_vec[8] = {
    { simde_mm_set_ps(0.4193f, 0.8439f, 0.8100f, 0.2669f),
      simde_mm_set_ps(0.4193f, 0.8439f, 0.8100f, 0.2669f),
      1 },
    { simde_mm_set_ps(0.9426f, 0.1679f, 0.2845f, 0.1698f),
      simde_mm_set_ps(0.9426f, 0.6931f, 0.2845f, 0.1698f),
      1 },
    { simde_mm_set_ps(0.0547f, 0.2368f, 0.3365f, 0.9146f),
      simde_mm_set_ps(0.0547f, 0.5935f, 0.3365f, 0.5014f),
      0 },
    { simde_mm_set_ps(0.7179f, 0.8607f, 0.4372f, 0.6140f),
      simde_mm_set_ps(0.7179f, 0.7239f, 0.4372f, 0.6140f),
      1 },
    { simde_mm_set_ps(0.4474f, 0.6848f, 0.4305f, 0.8738f),
      simde_mm_set_ps(0.6059f, 0.8463f, 0.4305f, 0.1517f),
      0 },
    { simde_mm_set_ps(0.4537f, 0.7254f, 0.9987f, 0.9115f),
      simde_mm_set_ps(0.1771f, 0.2982f, 0.9987f, 0.9003f),
      0 },
    { simde_mm_set_ps(0.2162f, 0.6303f, 0.0602f, 0.9986f),
      simde_mm_set_ps(0.2162f, 0.5872f, 0.0602f, 0.2491f),
      0 },
    { simde_mm_set_ps(0.4836f, 0.9929f, 0.8942f, 0.2367f),
      simde_mm_set_ps(0.4836f, 0.9929f, 0.4202f, 0.2367f),
      1 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_ucomieq_ss(test_vec[i].a, test_vec[i].b);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_ucomige_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    int r;
  } test_vec[8] = {
    { simde_mm_set_ps(0.1174f, 0.9995f, 0.7362f, 0.9966f),
      simde_mm_set_ps(0.1174f, 0.5850f, 0.0225f, 0.0035f),
      1 },
    { simde_mm_set_ps(0.6820f, 0.9892f, 0.6235f, 0.1819f),
      simde_mm_set_ps(0.0898f, 0.9892f, 0.6235f, 0.3305f),
      0 },
    { simde_mm_set_ps(0.7152f, 0.9286f, 0.9635f, 0.8823f),
      simde_mm_set_ps(0.3403f, 0.4177f, 0.9635f, 0.8737f),
      1 },
    { simde_mm_set_ps(0.5619f, 0.6892f, 0.2137f, 0.5336f),
      simde_mm_set_ps(0.1340f, 0.0152f, 0.9280f, 0.5336f),
      1 },
    { simde_mm_set_ps(0.5476f, 0.8606f, 0.2177f, 0.5284f),
      simde_mm_set_ps(0.5476f, 0.6253f, 0.1285f, 0.7135f),
      0 },
    { simde_mm_set_ps(0.6649f, 0.2053f, 0.5053f, 0.0378f),
      simde_mm_set_ps(0.0308f, 0.2053f, 0.5053f, 0.8789f),
      0 },
    { simde_mm_set_ps(0.3714f, 0.8736f, 0.8711f, 0.9491f),
      simde_mm_set_ps(0.8296f, 0.2212f, 0.5986f, 0.9491f),
      1 },
    { simde_mm_set_ps(0.8791f, 0.5862f, 0.4977f, 0.0888f),
      simde_mm_set_ps(0.8669f, 0.7545f, 0.4977f, 0.0888f),
      1 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_ucomige_ss(test_vec[i].a, test_vec[i].b);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_ucomigt_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    int r;
  } test_vec[8] = {
    { simde_mm_set_ps(0.1174f, 0.9995f, 0.7362f, 0.9966f),
      simde_mm_set_ps(0.1174f, 0.5850f, 0.0225f, 0.0035f),
      1 },
    { simde_mm_set_ps(0.6820f, 0.9892f, 0.6235f, 0.1819f),
      simde_mm_set_ps(0.0898f, 0.9892f, 0.6235f, 0.3305f),
      0 },
    { simde_mm_set_ps(0.7152f, 0.9286f, 0.9635f, 0.8823f),
      simde_mm_set_ps(0.3403f, 0.4177f, 0.9635f, 0.8737f),
      1 },
    { simde_mm_set_ps(0.5619f, 0.6892f, 0.2137f, 0.5336f),
      simde_mm_set_ps(0.1340f, 0.0152f, 0.9280f, 0.5336f),
      0 },
    { simde_mm_set_ps(0.5476f, 0.8606f, 0.2177f, 0.5284f),
      simde_mm_set_ps(0.5476f, 0.6253f, 0.1285f, 0.7135f),
      0 },
    { simde_mm_set_ps(0.6649f, 0.2053f, 0.5053f, 0.0378f),
      simde_mm_set_ps(0.0308f, 0.2053f, 0.5053f, 0.8789f),
      0 },
    { simde_mm_set_ps(0.3714f, 0.8736f, 0.8711f, 0.9491f),
      simde_mm_set_ps(0.8296f, 0.2212f, 0.5986f, 0.9491f),
      0 },
    { simde_mm_set_ps(0.8791f, 0.5862f, 0.4977f, 0.0888f),
      simde_mm_set_ps(0.8669f, 0.7545f, 0.4977f, 0.0888f),
      0 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_ucomigt_ss(test_vec[i].a, test_vec[i].b);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_ucomile_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    int r;
  } test_vec[8] = {
    { simde_mm_set_ps(0.1174f, 0.9995f, 0.7362f, 0.9966f),
      simde_mm_set_ps(0.1174f, 0.5850f, 0.0225f, 0.0035f),
      0 },
    { simde_mm_set_ps(0.6820f, 0.9892f, 0.6235f, 0.1819f),
      simde_mm_set_ps(0.0898f, 0.9892f, 0.6235f, 0.3305f),
      1 },
    { simde_mm_set_ps(0.7152f, 0.9286f, 0.9635f, 0.8823f),
      simde_mm_set_ps(0.3403f, 0.4177f, 0.9635f, 0.8737f),
      0 },
    { simde_mm_set_ps(0.5619f, 0.6892f, 0.2137f, 0.5336f),
      simde_mm_set_ps(0.1340f, 0.0152f, 0.9280f, 0.5336f),
      1 },
    { simde_mm_set_ps(0.5476f, 0.8606f, 0.2177f, 0.5284f),
      simde_mm_set_ps(0.5476f, 0.6253f, 0.1285f, 0.7135f),
      1 },
    { simde_mm_set_ps(0.6649f, 0.2053f, 0.5053f, 0.0378f),
      simde_mm_set_ps(0.0308f, 0.2053f, 0.5053f, 0.8789f),
      1 },
    { simde_mm_set_ps(0.3714f, 0.8736f, 0.8711f, 0.9491f),
      simde_mm_set_ps(0.8296f, 0.2212f, 0.5986f, 0.9491f),
      1 },
    { simde_mm_set_ps(0.8791f, 0.5862f, 0.4977f, 0.0888f),
      simde_mm_set_ps(0.8669f, 0.7545f, 0.4977f, 0.0888f),
      1 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_ucomile_ss(test_vec[i].a, test_vec[i].b);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_ucomilt_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    int r;
  } test_vec[8] = {
    { simde_mm_set_ps(0.1174f, 0.9995f, 0.7362f, 0.9966f),
      simde_mm_set_ps(0.1174f, 0.5850f, 0.0225f, 0.0035f),
      0 },
    { simde_mm_set_ps(0.6820f, 0.9892f, 0.6235f, 0.1819f),
      simde_mm_set_ps(0.0898f, 0.9892f, 0.6235f, 0.3305f),
      1 },
    { simde_mm_set_ps(0.7152f, 0.9286f, 0.9635f, 0.8823f),
      simde_mm_set_ps(0.3403f, 0.4177f, 0.9635f, 0.8737f),
      0 },
    { simde_mm_set_ps(0.5619f, 0.6892f, 0.2137f, 0.5336f),
      simde_mm_set_ps(0.1340f, 0.0152f, 0.9280f, 0.5336f),
      0 },
    { simde_mm_set_ps(0.5476f, 0.8606f, 0.2177f, 0.5284f),
      simde_mm_set_ps(0.5476f, 0.6253f, 0.1285f, 0.7135f),
      1 },
    { simde_mm_set_ps(0.6649f, 0.2053f, 0.5053f, 0.0378f),
      simde_mm_set_ps(0.0308f, 0.2053f, 0.5053f, 0.8789f),
      1 },
    { simde_mm_set_ps(0.3714f, 0.8736f, 0.8711f, 0.9491f),
      simde_mm_set_ps(0.8296f, 0.2212f, 0.5986f, 0.9491f),
      0 },
    { simde_mm_set_ps(0.8791f, 0.5862f, 0.4977f, 0.0888f),
      simde_mm_set_ps(0.8669f, 0.7545f, 0.4977f, 0.0888f),
      0 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_ucomilt_ss(test_vec[i].a, test_vec[i].b);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_ucomineq_ss(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    int r;
  } test_vec[8] = {
    { simde_mm_set_ps(0.1174f, 0.9995f, 0.7362f, 0.9966f),
      simde_mm_set_ps(0.1174f, 0.5850f, 0.0225f, 0.0035f),
      1 },
    { simde_mm_set_ps(0.6820f, 0.9892f, 0.6235f, 0.1819f),
      simde_mm_set_ps(0.0898f, 0.9892f, 0.6235f, 0.3305f),
      1 },
    { simde_mm_set_ps(0.7152f, 0.9286f, 0.9635f, 0.8823f),
      simde_mm_set_ps(0.3403f, 0.4177f, 0.9635f, 0.8737f),
      1 },
    { simde_mm_set_ps(0.5619f, 0.6892f, 0.2137f, 0.5336f),
      simde_mm_set_ps(0.1340f, 0.0152f, 0.9280f, 0.5336f),
      0 },
    { simde_mm_set_ps(0.5476f, 0.8606f, 0.2177f, 0.5284f),
      simde_mm_set_ps(0.5476f, 0.6253f, 0.1285f, 0.7135f),
      1 },
    { simde_mm_set_ps(0.6649f, 0.2053f, 0.5053f, 0.0378f),
      simde_mm_set_ps(0.0308f, 0.2053f, 0.5053f, 0.8789f),
      1 },
    { simde_mm_set_ps(0.3714f, 0.8736f, 0.8711f, 0.9491f),
      simde_mm_set_ps(0.8296f, 0.2212f, 0.5986f, 0.9491f),
      0 },
    { simde_mm_set_ps(0.8791f, 0.5862f, 0.4977f, 0.0888f),
      simde_mm_set_ps(0.8669f, 0.7545f, 0.4977f, 0.0888f),
      0 }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    int r = simde_mm_ucomineq_ss(test_vec[i].a, test_vec[i].b);
    munit_assert_int(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_unpackhi_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps(-756.38f, -450.47f,  901.78f,  859.60f),
      simde_mm_set_ps( -17.95f, -383.47f,  601.72f,  328.61f),
      simde_mm_set_ps( -17.95f, -756.38f, -383.47f, -450.47f) },
    { simde_mm_set_ps(-696.67f, -318.67f, -746.84f,  486.97f),
      simde_mm_set_ps(  76.54f, -173.13f,  775.95f,  744.80f),
      simde_mm_set_ps(  76.54f, -696.67f, -173.13f, -318.67f) },
    { simde_mm_set_ps(-181.38f,  -64.52f, -525.50f,  383.47f),
      simde_mm_set_ps(-830.80f,  462.90f,  208.76f, -840.43f),
      simde_mm_set_ps(-830.80f, -181.38f,  462.90f,  -64.52f) },
    { simde_mm_set_ps(-437.50f, -751.51f, -255.04f,  713.97f),
      simde_mm_set_ps(-835.16f,  838.19f, -968.67f, -800.39f),
      simde_mm_set_ps(-835.16f, -437.50f,  838.19f, -751.51f) },
    { simde_mm_set_ps( 252.51f,  219.43f,  234.16f,  718.42f),
      simde_mm_set_ps( 737.55f, -360.52f,  129.47f,  866.75f),
      simde_mm_set_ps( 737.55f,  252.51f, -360.52f,  219.43f) },
    { simde_mm_set_ps(-543.89f,  -12.39f,  996.39f, -521.27f),
      simde_mm_set_ps(-330.73f, -334.66f,  798.87f, -360.08f),
      simde_mm_set_ps(-330.73f, -543.89f, -334.66f,  -12.39f) },
    { simde_mm_set_ps( 633.16f,  239.27f,  625.35f,  330.43f),
      simde_mm_set_ps(-890.86f,  495.17f, -524.21f,  275.19f),
      simde_mm_set_ps(-890.86f,  633.16f,  495.17f,  239.27f) },
    { simde_mm_set_ps(-443.30f, -851.22f,  842.49f,  697.77f),
      simde_mm_set_ps(   8.81f, -953.12f,   -1.45f, -983.63f),
      simde_mm_set_ps(   8.81f, -443.30f, -953.12f, -851.22f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_unpackhi_ps(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_close(r, test_vec[i].r, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_unpacklo_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_mm_set_ps(-756.38f, -450.47f,  901.78f,  859.60f),
      simde_mm_set_ps( -17.95f, -383.47f,  601.72f,  328.61f),
      simde_mm_set_ps( 601.72f,  901.78f,  328.61f,  859.60f) },
    { simde_mm_set_ps(-696.67f, -318.67f, -746.84f,  486.97f),
      simde_mm_set_ps(  76.54f, -173.13f,  775.95f,  744.80f),
      simde_mm_set_ps( 775.95f, -746.84f,  744.80f,  486.97f) },
    { simde_mm_set_ps(-181.38f,  -64.52f, -525.50f,  383.47f),
      simde_mm_set_ps(-830.80f,  462.90f,  208.76f, -840.43f),
      simde_mm_set_ps( 208.76f, -525.50f, -840.43f,  383.47f) },
    { simde_mm_set_ps(-437.50f, -751.51f, -255.04f,  713.97f),
      simde_mm_set_ps(-835.16f,  838.19f, -968.67f, -800.39f),
      simde_mm_set_ps(-968.67f, -255.04f, -800.39f,  713.97f) },
    { simde_mm_set_ps( 252.51f,  219.43f,  234.16f,  718.42f),
      simde_mm_set_ps( 737.55f, -360.52f,  129.47f,  866.75f),
      simde_mm_set_ps( 129.47f,  234.16f,  866.75f,  718.42f) },
    { simde_mm_set_ps(-543.89f,  -12.39f,  996.39f, -521.27f),
      simde_mm_set_ps(-330.73f, -334.66f,  798.87f, -360.08f),
      simde_mm_set_ps( 798.87f,  996.39f, -360.08f, -521.27f) },
    { simde_mm_set_ps( 633.16f,  239.27f,  625.35f,  330.43f),
      simde_mm_set_ps(-890.86f,  495.17f, -524.21f,  275.19f),
      simde_mm_set_ps(-524.21f,  625.35f,  275.19f,  330.43f) },
    { simde_mm_set_ps(-443.30f, -851.22f,  842.49f,  697.77f),
      simde_mm_set_ps(   8.81f, -953.12f,   -1.45f, -983.63f),
      simde_mm_set_ps(  -1.45f,  842.49f, -983.63f,  697.77f) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_unpacklo_ps(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_close(r, test_vec[i].r, 1);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_undefined_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  simde__m128 z = simde_mm_setzero_ps();
  simde__m128 v = simde_mm_undefined_ps();
  v = simde_mm_xor_ps(v, v);

  simde_assert_m128_u32(v, ==, z);

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_xor_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    simde__m128 b;
    simde__m128 r;
  } test_vec[8] = {
    { simde_m128_set_u32(UINT32_C(0x024f4584), UINT32_C(0xa17da0be), UINT32_C(0x58a7e615), UINT32_C(0x4a3b7e6a)),
      simde_m128_set_u32(UINT32_C(0x48d3d04c), UINT32_C(0xdc574d32), UINT32_C(0x6c3bc006), UINT32_C(0xc8120e07)),
      simde_m128_set_u32(UINT32_C(0x4a9c95c8), UINT32_C(0x7d2aed8c), UINT32_C(0x349c2613), UINT32_C(0x8229706d)) },
    { simde_m128_set_u32(UINT32_C(0x430ea8b4), UINT32_C(0x2c6e1c1f), UINT32_C(0x4634b744), UINT32_C(0xc1a535c3)),
      simde_m128_set_u32(UINT32_C(0x2ebc830c), UINT32_C(0x09ff7c77), UINT32_C(0xb03e6975), UINT32_C(0x3b6351c2)),
      simde_m128_set_u32(UINT32_C(0x6db22bb8), UINT32_C(0x25916068), UINT32_C(0xf60ade31), UINT32_C(0xfac66401)) },
    { simde_m128_set_u32(UINT32_C(0xc2025c69), UINT32_C(0xb9a74036), UINT32_C(0x7a940029), UINT32_C(0x0ba4d901)),
      simde_m128_set_u32(UINT32_C(0xc3fc959c), UINT32_C(0x59083db4), UINT32_C(0x915055e4), UINT32_C(0x5dbf7703)),
      simde_m128_set_u32(UINT32_C(0x01fec9f5), UINT32_C(0xe0af7d82), UINT32_C(0xebc455cd), UINT32_C(0x561bae02)) },
    { simde_m128_set_u32(UINT32_C(0xf802d7d5), UINT32_C(0xed34ed1c), UINT32_C(0x9d828497), UINT32_C(0xc6637f1f)),
      simde_m128_set_u32(UINT32_C(0xdba38586), UINT32_C(0x7071b51f), UINT32_C(0xcac3fcc0), UINT32_C(0x8f7eb2d1)),
      simde_m128_set_u32(UINT32_C(0x23a15253), UINT32_C(0x9d455803), UINT32_C(0x57417857), UINT32_C(0x491dcdce)) },
    { simde_m128_set_u32(UINT32_C(0x79606c79), UINT32_C(0xf71d7967), UINT32_C(0xada55028), UINT32_C(0xb78eeb2e)),
      simde_m128_set_u32(UINT32_C(0x69d0518e), UINT32_C(0x2752e0c6), UINT32_C(0x3f450894), UINT32_C(0x77f477b4)),
      simde_m128_set_u32(UINT32_C(0x10b03df7), UINT32_C(0xd04f99a1), UINT32_C(0x92e058bc), UINT32_C(0xc07a9c9a)) },
    { simde_m128_set_u32(UINT32_C(0x441b98db), UINT32_C(0xe94eb7e6), UINT32_C(0xb63975b0), UINT32_C(0x3d68f9d5)),
      simde_m128_set_u32(UINT32_C(0x26f600f1), UINT32_C(0x5d69ebb6), UINT32_C(0x58f8ec40), UINT32_C(0xae6e3695)),
      simde_m128_set_u32(UINT32_C(0x62ed982a), UINT32_C(0xb4275c50), UINT32_C(0xeec199f0), UINT32_C(0x9306cf40)) },
    { simde_m128_set_u32(UINT32_C(0xdd0bf52e), UINT32_C(0x1aac98e2), UINT32_C(0x9f63c82f), UINT32_C(0x9e5b5bfb)),
      simde_m128_set_u32(UINT32_C(0xb98fae38), UINT32_C(0xf9bff81d), UINT32_C(0x54613b0c), UINT32_C(0x2d9c5cba)),
      simde_m128_set_u32(UINT32_C(0x64845b16), UINT32_C(0xe31360ff), UINT32_C(0xcb02f323), UINT32_C(0xb3c70741)) },
    { simde_m128_set_u32(UINT32_C(0xba2156c4), UINT32_C(0xdb2aa6aa), UINT32_C(0xd67bed5a), UINT32_C(0xdab4319d)),
      simde_m128_set_u32(UINT32_C(0xc2c3ccf5), UINT32_C(0xabb0afdd), UINT32_C(0x96fd5dd6), UINT32_C(0xa2b25a76)),
      simde_m128_set_u32(UINT32_C(0x78e29a31), UINT32_C(0x709a0977), UINT32_C(0x4086b08c), UINT32_C(0x78066beb)) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m128 r = simde_mm_xor_ps(test_vec[i].a, test_vec[i].b);
    simde_assert_m128_u32(r, ==, test_vec[i].r);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_stream_pi(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m64 a;
    simde__m64 r;
  } test_vec[8] = {
    { simde_mm_set_pi16(  6761, -22445, -23476, -30705),
      simde_mm_set_pi16(  6761, -22445, -23476, -30705) },
    { simde_mm_set_pi16( 19012, -30608,  30482, -20411),
      simde_mm_set_pi16( 19012, -30608,  30482, -20411) },
    { simde_mm_set_pi16( 23487, -28638,  -4660,  -3430),
      simde_mm_set_pi16( 23487, -28638,  -4660,  -3430) },
    { simde_mm_set_pi16( -9884,  -9588,  -4377, -27077),
      simde_mm_set_pi16( -9884,  -9588,  -4377, -27077) },
    { simde_mm_set_pi16( 15488, -17321,   2333,   8745),
      simde_mm_set_pi16( 15488, -17321,   2333,   8745) },
    { simde_mm_set_pi16( 30278,   5796, -17311,  13106),
      simde_mm_set_pi16( 30278,   5796, -17311,  13106) },
    { simde_mm_set_pi16(-18652,  26507,  -8174,  31557),
      simde_mm_set_pi16(-18652,  26507,  -8174,  31557) },
    { simde_mm_set_pi16(-16725,  22668,   9074,   1013),
      simde_mm_set_pi16(-16725,  22668,   9074,   1013) }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    simde__m64 r;
    simde_mm_stream_pi(&r, test_vec[i].a);
    munit_assert_int64(r.i64[0], ==, test_vec[i].r.i64[0]);
  }

  return MUNIT_OK;
}

static MunitResult
test_simde_mm_stream_ps(const MunitParameter params[], void* data) {
  (void) params;
  (void) data;

  const struct {
    simde__m128 a;
    SIMDE_ALIGN(16) simde_float32 r[4];
  } test_vec[8] = {
    { simde_mm_set_ps(-386.97f,  492.19f,  318.83f,  345.85f),
      {  345.85f,  318.83f,  492.19f, -386.97f } },
    { simde_mm_set_ps( 908.43f,  787.88f, -776.77f, -773.68f),
      { -773.68f, -776.77f,  787.88f,  908.43f } },
    { simde_mm_set_ps( 241.81f,  684.64f, -474.83f,  614.26f),
      {  614.26f, -474.83f,  684.64f,  241.81f } },
    { simde_mm_set_ps(-327.50f, -550.14f, -266.51f, -677.19f),
      { -677.19f, -266.51f, -550.14f, -327.50f } },
    { simde_mm_set_ps( 706.39f, -425.59f,  678.55f, -877.83f),
      { -877.83f,  678.55f, -425.59f,  706.39f } },
    { simde_mm_set_ps( 902.50f,  144.03f,  -93.04f,  995.74f),
      {  995.74f,  -93.04f,  144.03f,  902.50f } },
    { simde_mm_set_ps( 898.99f, -437.71f, -170.25f,  875.61f),
      {  875.61f, -170.25f, -437.71f,  898.99f } },
    { simde_mm_set_ps( 347.85f, -128.18f,  904.62f,  936.88f),
      {  936.88f,  904.62f, -128.18f,  347.85f } }
  };

  for (size_t i = 0 ; i < (sizeof(test_vec) / (sizeof(test_vec[0]))) ; i++) {
    SIMDE_ALIGN(16) simde_float32 r[4];
    simde_mm_stream_ps(r, test_vec[i].a);
    for (size_t j = 0 ; j < sizeof(simde__m128) / sizeof(simde_float32) ; j++) {
      munit_assert_float(r[j], ==, test_vec[i].r[j]);
    }
  }

  return MUNIT_OK;
}

static MunitTest test_suite_tests[] = {
  TEST_FUNC(mm_set_ps),
  TEST_FUNC(mm_set1_ps),
  TEST_FUNC(mm_set_ss),
  TEST_FUNC(mm_setr_ps),
  TEST_FUNC(mm_setzero_ps),
  TEST_FUNC(mm_add_ps),
  TEST_FUNC(mm_add_ss),
  TEST_FUNC(mm_and_ps),
  TEST_FUNC(mm_andnot_ps),
  TEST_FUNC(mm_avg_pu16),
  TEST_FUNC(mm_avg_pu8),
  TEST_FUNC(mm_cmpeq_ps),
  TEST_FUNC(mm_cmpeq_ss),
  TEST_FUNC(mm_cmpge_ps),
  TEST_FUNC(mm_cmpge_ss),
  TEST_FUNC(mm_cmpgt_ps),
  TEST_FUNC(mm_cmpgt_ss),
  TEST_FUNC(mm_cmple_ps),
  TEST_FUNC(mm_cmple_ss),
  TEST_FUNC(mm_cmplt_ps),
  TEST_FUNC(mm_cmplt_ss),
  TEST_FUNC(mm_cmpneq_ps),
  TEST_FUNC(mm_cmpneq_ss),
  TEST_FUNC(mm_cmpnge_ps),
  TEST_FUNC(mm_cmpnge_ss),
  TEST_FUNC(mm_cmpngt_ps),
  TEST_FUNC(mm_cmpngt_ss),
  TEST_FUNC(mm_cmpnle_ps),
  TEST_FUNC(mm_cmpnle_ss),
  TEST_FUNC(mm_cmpnlt_ps),
  TEST_FUNC(mm_cmpnlt_ss),
  TEST_FUNC(mm_cmpord_ps),
  TEST_FUNC(mm_cmpord_ss),
  TEST_FUNC(mm_cmpunord_ps),
  TEST_FUNC(mm_cmpunord_ss),
  TEST_FUNC(mm_comieq_ss),
  TEST_FUNC(mm_comige_ss),
  TEST_FUNC(mm_comigt_ss),
  TEST_FUNC(mm_comile_ss),
  TEST_FUNC(mm_comilt_ss),
  TEST_FUNC(mm_comineq_ss),
  TEST_FUNC(mm_cvt_pi2ps),
  TEST_FUNC(mm_cvt_ps2pi),
  TEST_FUNC(mm_cvt_si2ss),
  TEST_FUNC(mm_cvt_ss2si),
  TEST_FUNC(mm_cvtpi16_ps),
  TEST_FUNC(mm_cvtpi32_ps),
  TEST_FUNC(mm_cvtpi32x2_ps),
  TEST_FUNC(mm_cvtpi8_ps),
  TEST_FUNC(mm_cvtps_pi16),
  TEST_FUNC(mm_cvtps_pi32),
  TEST_FUNC(mm_cvtps_pi8),
  TEST_FUNC(mm_cvtpu16_ps),
  TEST_FUNC(mm_cvtpu8_ps),
  TEST_FUNC(mm_cvtsi32_ss),
  TEST_FUNC(mm_cvtsi64_ss),
  TEST_FUNC(mm_cvtss_f32),
  TEST_FUNC(mm_cvtss_si32),
  TEST_FUNC(mm_cvtss_si64),
  TEST_FUNC(mm_cvtt_ps2pi),
  TEST_FUNC(mm_cvtt_ss2si),
  TEST_FUNC(mm_cvttss_si64),
  TEST_FUNC(mm_div_ps),
  TEST_FUNC(mm_div_ss),
  TEST_FUNC(mm_extract_pi16),
  TEST_FUNC(mm_insert_pi16),
  TEST_FUNC(mm_load_ps),
  TEST_FUNC(mm_load_ps1),
  TEST_FUNC(mm_load_ss),
  TEST_FUNC(mm_loadh_pi),
  TEST_FUNC(mm_loadl_pi),
  TEST_FUNC(mm_loadr_ps),
  TEST_FUNC(mm_loadu_ps),
  TEST_FUNC(mm_maskmove_si64),
  TEST_FUNC(mm_max_pi16),
  TEST_FUNC(mm_max_pu8),
  TEST_FUNC(mm_max_ps),
  TEST_FUNC(mm_max_ss),
  TEST_FUNC(mm_min_pi16),
  TEST_FUNC(mm_min_pu8),
  TEST_FUNC(mm_min_ps),
  TEST_FUNC(mm_min_ss),
  TEST_FUNC(mm_move_ss),
  TEST_FUNC(mm_movehl_ps),
  TEST_FUNC(mm_movelh_ps),
  TEST_FUNC(mm_movemask_pi8),
  TEST_FUNC(mm_movemask_ps),
  TEST_FUNC(mm_mul_ps),
  TEST_FUNC(mm_mul_ss),
  TEST_FUNC(mm_mulhi_pu16),
  TEST_FUNC(mm_or_ps),
  TEST_FUNC(mm_rcp_ps),
  TEST_FUNC(mm_rcp_ss),
  TEST_FUNC(mm_rsqrt_ps),
  TEST_FUNC(mm_rsqrt_ss),
  TEST_FUNC(mm_sad_pu8),
  TEST_FUNC(mm_shuffle_pi16),
  TEST_FUNC(mm_shuffle_ps),
  TEST_FUNC(mm_sqrt_ps),
  TEST_FUNC(mm_sqrt_ss),
  TEST_FUNC(mm_store_ps),
  TEST_FUNC(mm_store_ps1),
  TEST_FUNC(mm_store_ss),
  TEST_FUNC(mm_store1_ps),
  TEST_FUNC(mm_storeh_pi),
  TEST_FUNC(mm_storel_pi),
  TEST_FUNC(mm_storer_ps),
  TEST_FUNC(mm_storeu_ps),
  TEST_FUNC(mm_sub_ps),
  TEST_FUNC(mm_sub_ss),
  TEST_FUNC(mm_ucomieq_ss),
  TEST_FUNC(mm_ucomige_ss),
  TEST_FUNC(mm_ucomigt_ss),
  TEST_FUNC(mm_ucomile_ss),
  TEST_FUNC(mm_ucomilt_ss),
  TEST_FUNC(mm_ucomineq_ss),
  TEST_FUNC(mm_unpackhi_ps),
  TEST_FUNC(mm_unpacklo_ps),
  TEST_FUNC(mm_undefined_ps),
  TEST_FUNC(mm_xor_ps),
  TEST_FUNC(mm_stream_pi),
  TEST_FUNC(mm_stream_ps),

  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#if defined(SIMDE_NO_NATIVE)
#  define simde_sse_test_suite simde_sse_emul_test_suite
#endif

MunitSuite simde_sse_test_suite = {
  (char*) "/sse",
  test_suite_tests,
  NULL,
  1,
  MUNIT_SUITE_OPTION_NONE
};
