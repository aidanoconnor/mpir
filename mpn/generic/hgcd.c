/* hgcd.c.

   THE FUNCTIONS IN THIS FILE ARE INTERNAL WITH MUTABLE INTERFACES.  IT IS ONLY
   SAFE TO REACH THEM THROUGH DOCUMENTED INTERFACES.  IN FACT, IT IS ALMOST
   GUARANTEED THAT THEY'LL CHANGE OR DISAPPEAR IN A FUTURE GNU MP RELEASE.

Copyright 2003, 2004 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include "mpir.h"
#include "gmp-impl.h"
#include "longlong.h"

/* For NULL */
#include <stddef.h>

/* Comparison of _normalized_ numbers. */

#define MPN_EQUAL_P(ap, asize, bp, bsize)			\
((asize) == (bsize) && mpn_cmp ((ap), (bp), (asize)) == 0)

#define MPN_LEQ_P(ap, asize, bp, bsize)				\
((asize) < (bsize) || ((asize) == (bsize)			\
		       && mpn_cmp ((ap), (bp), (asize)) <= 0))

#define MPN_LESS_P(ap, asize, bp, bsize)			\
((asize) < (bsize) || ((asize) == (bsize)			\
		       && mpn_cmp ((ap), (bp), (asize)) < 0))

/* Extract one limb, shifting count bits left
    ________  ________
   |___xh___||___xl___|
	  |____r____|
   >count <

   The count includes any nail bits, so it should work fine if
   count is computed using count_leading_zeros.
*/

#define MPN_EXTRACT_LIMB(count, xh, xl)				\
  ((((xh) << ((count) - GMP_NAIL_BITS)) & GMP_NUMB_MASK) |	\
   ((xl) >> (GMP_LIMB_BITS - (count))))


/* Return -1 if a < x + y + z,
	   0 if a = x + y + z,
	   1 if a > x + y + z. */
static int
mpn_cmp_sum3 (mp_srcptr ap, mp_size_t an,
	      mp_srcptr xp, mp_size_t xn,
	      mp_srcptr yp, mp_size_t yn,
	      mp_srcptr zp, mp_size_t zn)
{
  mp_limb_t cy;

  /* Check that all limbs beyond an are zero. This should be slightly
     cheaper than fully normalizing all the input numbers. */

  while (xn > an)
    if (xp[--xn] > 0) return -1;
  while (yn > an)
    if (yp[--yn] > 0) return -1;
  while (zn > an)
    if (zp[--zn] > 0) return -1;

  /* Start by sorting so that xn >= yn >= zn. Six permutations, so we
     can't get away with less than three comparisons, at least not for
     the worst case. */

  if (xn < yn)
    MPN_SRCPTR_SWAP (xp, xn, yp, yn);
  if (yn < zn)
    MPN_SRCPTR_SWAP (yp, yn, zp, zn);
  if (xn < yn)
    MPN_SRCPTR_SWAP (xp, xn, yp, yn);

  ASSERT (an >= xn && xn >= yn && yn >= zn);

  /* Assume that a = x + y + z, and write the addition limb by limb.

       (c[1], a[0]) = x[0]   + y[0]   + z[0]   + c[0]
       (c[2], a[1]) = x[1]   + y[1]   + z[1]   + c[1]
     (c[k+1], a[k]) = x[k]   + y[k]   + z[k]   + c[2]
		   ...
     (c[n], a[n-1]) = x[n-1] + y[n-1] + z[n-1] + c[n-1]

     where the start and stop conditions are that c[0] = c[n] = 0.
     Then we can start at the high end, iterating

	c[k] = (c[k+1], a[k]) - x[k] - y[k] - z[k]

     If equality holds, then 0 <= c[k] <= 2 for all k (since for
     example 0xf + 0xf + 0xf + 2 = 0x2f). If we find c[k] < 0, then we
     know that a < x + y + z, and if we find c[k] > 2, then we know a
     > x + y + z. */

  cy = 0;

  while (an > xn)
    {
      /* c[k] = (c[k+1], a[k]) */
      if (cy > 0)
	return 1;

      cy = ap[--an];
    }

#if GMP_NAIL_BITS >= 2
  while (an > yn)
    {
      if (cy > 1)
	return 1;

      cy = (cy << GMP_NUMB_BITS) + ap[--an];
      if (cy < xp[an])
	return -1;
      cy -= xp[an];
    }
  while (an > zn)
    {
      mp_limb_t s;

      if (cy > 2)
	return 1;

      cy = (cy << GMP_NUMB_BITS ) + ap[--an];
      s = xp[an] + yp[an];
      if (cy < s)
	return -1;
      cy -= s;
    }
  while (an > 0)
    {
      mp_limb_t s;

      if (cy > 2)
	return 1;

      cy = (cy << GMP_NUMB_BITS ) + ap[--an];
      s = xp[an] + yp[an] + zp[an];
      if (cy < s)
	return -1;
      cy -= s;
    }
#else /* GMP_NAIL_BITS < 2 */
#if GMP_NAIL_BITS == 1
loselose
#endif
  while (an > yn)
    {
      /* c[k] = (c[k+1], a[k]) - x[k] */
      if (cy > 1)
	return 1;

      --an;

      if (cy == 1)
	{
	  if (ap[an] >= xp[an])
	    return 1;
	  cy = (ap[an] - xp[an]) & GMP_NUMB_MASK;
	}
      else
	{
	  /* cy == 0 */
	  if (ap[an] < xp[an])
	    return -1;
	  else
	    cy = ap[an] - xp[an];
	}
    }

  while (an > zn)
    {
      mp_limb_t sh, sl;

      /* c[k] = (c[k+1], a[k]) - x[k] - y[k] */
      if (cy > 2)
	return 1;

      --an;

      sl = xp[an] + yp[an];
      sh = (sl < xp[an]);

      if (cy < sh || (cy == sh && ap[an] < sl))
	return -1;

      sl = ap[an] - sl; /* Monkey business */
      sh = cy - sh - (sl > ap[an]);
      if (sh > 0)
	return 1;
      cy = sl;
    }
  while (an > 0)
    {
      mp_limb_t sh, sl;
      if (cy > 2)
	return 1;

      --an;

      sl = xp[an] + yp[an];
      sh = (sl < xp[an]);

      sl += zp[an];
      sh += sl < zp[an];

      if (cy < sh || (cy == sh && ap[an] < sl))
	return -1;
      sl = ap[an] - sl; /* Monkey business */
      sh = cy - sh - (sl > ap[an]);
      if (sh > 0)
	return 1;
      cy = sl;
    }
#endif /* GMP_NAIL_BITS < 2 */
  return cy > 0;
}

/* Only the first row has v = 0, a = 1 * a + 0 * b */
static inline int
hgcd_start_row_p (const struct hgcd_row *r, mp_size_t n)
{
  mp_size_t i;
  mp_srcptr vp = r->uvp[1];

  for (i = 0; i < n; i++)
    if (vp[i] != 0)
      return 0;

  return 1;
}

/* Called when r[0, 1, 2] >= W^M, r[3] < W^M. Returns the number of
   remainders that satisfy Jebelean's criterion, i.e. find the largest k
   such that

     r[k+1] >= max (-u[k+1], - v[k+1])

     r[k] - r[k-1] >= max (u[k+1] - u[k], v[k+1] - v[k])

   Return 0 on failure, i.e. if B or A mod B < W^M. Return 1 in case
   r0 and r1 are correct, but we still make no progress because r0 =
   A, r1 = B.

   Otherwise return 2, 3 or 4, the number of r:s that are correct.
*/
static int
hgcd_jebelean (const struct hgcd *hgcd, mp_size_t M)
{
  mp_size_t L;
  unsigned bit;

  ASSERT (hgcd->row[0].rsize > M);
  ASSERT (hgcd->row[1].rsize > M);
  ASSERT (hgcd->row[2].rsize > M);
  ASSERT (hgcd->row[3].rsize <= M);

  ASSERT (MPN_LESS_P (hgcd->row[1].rp, hgcd->row[1].rsize,
		      hgcd->row[0].rp, hgcd->row[0].rsize));
  ASSERT (MPN_LESS_P (hgcd->row[2].rp, hgcd->row[2].rsize,
		      hgcd->row[1].rp, hgcd->row[1].rsize));
  ASSERT (MPN_LESS_P (hgcd->row[3].rp, hgcd->row[3].rsize,
		      hgcd->row[2].rp, hgcd->row[2].rsize));

  ASSERT (mpn_cmp (hgcd->row[0].uvp[1], hgcd->row[1].uvp[1], hgcd->size) <= 0);
  ASSERT (mpn_cmp (hgcd->row[1].uvp[1], hgcd->row[2].uvp[1], hgcd->size) <= 0);
  ASSERT (mpn_cmp (hgcd->row[2].uvp[1], hgcd->row[3].uvp[1], hgcd->size) <= 0);

  /* The bound is really floor (N/2), which is <= M = ceil (N/2) */
  L = hgcd->size;
  ASSERT (L <= M);

  ASSERT (L > 0);
  ASSERT (hgcd->row[3].uvp[1][L - 1] != 0);

  bit = hgcd->sign < 0;

  /* Check r1 - r2 >= max (u2 - u1, v2 - v1) = {|u1| + |u2|, |v1| + |v2|}[bit] */

  if (mpn_cmp_sum3 (hgcd->row[1].rp, hgcd->row[1].rsize,
		    hgcd->row[2].rp, hgcd->row[2].rsize,
		    hgcd->row[1].uvp[bit], L,
		    hgcd->row[2].uvp[bit], L) < 0)
    return 2 - (hgcd_start_row_p (hgcd->row, hgcd->size));

  /* Ok, r2 is correct */

  /* Check r3 >= max (-u3, -v3) = (|u3|, |v3|)[bit] */
  if (hgcd->row[3].rsize > L)
    /* Condition satisfied */
    ;
  else
    {
      mp_size_t size;
      for (size = L; size > hgcd->row[3].rsize; size--)
	{
	  if (hgcd->row[3].uvp[bit][size-1] != 0)
	    return 3;
	}
      if (mpn_cmp (hgcd->row[3].rp, hgcd->row[3].uvp[bit], size) < 0)
	return 3;
    }

  /* Check r3 - r2 >= max(u3-u2, v3-v2) = {|u2| + |u3|, |v2| +|v3|}[1-bit] */

  if (mpn_cmp_sum3 (hgcd->row[2].rp, hgcd->row[2].rsize,
		    hgcd->row[3].rp, hgcd->row[3].rsize,
		    hgcd->row[2].uvp[bit ^ 1], L,
		    hgcd->row[3].uvp[bit ^ 1], L) < 0)
    return 3;

  /* Ok, r3 is correct */
  return 4;
}


/* Compute au + bv. u and v are single limbs, a and b are n limbs each.
   Stores n+1 limbs in rp, and returns the (n+2)'nd limb. */
/* FIXME: With nails, we can instead return limb n+1, possibly including
   one non-zero nail bit. */
static mp_limb_t
mpn_addmul2_n_1 (mp_ptr rp, mp_size_t n,
		 mp_srcptr ap, mp_limb_t u,
		 mp_srcptr bp, mp_limb_t v)
{
  mp_limb_t h;
  mp_limb_t cy;

  h = mpn_mul_1 (rp, ap, n, u);
  cy = mpn_addmul_1 (rp, bp, n, v);
  h += cy;
#if GMP_NAIL_BITS == 0
  rp[n] = h;
  return (h < cy);
#else /* GMP_NAIL_BITS > 0 */
  rp[n] = h & GMP_NUMB_MASK;
  return h >> GMP_NUMB_BITS;
#endif /* GMP_NAIL_BITS > 0 */
}

/* hgcd2 operations */

/* Computes P = R * S. No overlap allowed. */
static mp_size_t
hgcd2_mul (struct hgcd_row *P, mp_size_t alloc,
	   const struct hgcd2_row *R,
	   const struct hgcd_row *S, mp_size_t n)
{
  int grow = 0;
  mp_limb_t h = 0;
  unsigned i;
  unsigned j;

  ASSERT (n < alloc);

  for (i = 0; i < 2; i++)
    for (j = 0; j < 2; j++)
      {
	/* Set P[i, j] = R[i, 0] S[0, j] + R[i,1] S[1, j]
		       = u_i s0j + v_i s1j */
	mp_limb_t cy;

	cy = mpn_addmul2_n_1 (P[i].uvp[j], n,
			      S[0].uvp[j], R[i].u,
			      S[1].uvp[j], R[i].v);
	if (cy)
	  {
	    ASSERT (n + 2 <= alloc);
	    P[i].uvp[j][n+1] = cy;
	    grow = 1;
	  }
	else
	  h |= P[i].uvp[j][n];
      }
  if (grow)
    return n + 2;
  else
    /* Don't add redundant zeroes */
    return n + (h != 0);
}

unsigned
mpn_hgcd_max_recursion (mp_size_t n)
{
  int count;

  count_leading_zeros (count, (mp_limb_t)
		       (1 + n / (HGCD_SCHOENHAGE_THRESHOLD  - 5)));

  return GMP_LIMB_BITS - count;
}

mp_size_t
mpn_hgcd_init_itch (mp_size_t size)
{
  /* r0 <= a, r1, r2, r3 <= b, but for simplicity, we allocate asize +
     1 for all of them. The size of the uv:s are limited to asize / 2,
     but we allocate one extra limb. The quotients aren't included
     here. */

  return 4 * (size + 1) + 8 * ((size / 2) + 1);
}

void
mpn_hgcd_init (struct hgcd *hgcd,
	       mp_size_t asize,
	       mp_limb_t *limbs)
{
  unsigned i;
  unsigned j;
  mp_size_t alloc = (asize / 2) + 1;

  hgcd->sign = 0;

  for (i = 0; i < 4; i++)
    {
      hgcd->row[i].rp = limbs;
      hgcd->row[i].rsize = asize + 1; limbs += asize + 1;
    }

  hgcd->alloc = alloc;
  hgcd->size = alloc;

  for (i = 0; i < 4; i++)
    for (j = 0; j < 2; j++)
      {
	hgcd->row[i].uvp[j] = limbs;
	limbs += alloc;
      }

  hgcd->qp[0] = hgcd->qp[1] = NULL;
}

/* FIXME: Could be a macro */
mp_size_t
mpn_hgcd_quotients_init_itch (mp_size_t asize)
{
  return 2*(asize + 1);
}

void
mpn_hgcd_quotients_init (struct hgcd *hgcd,
			 mp_size_t asize,
			 mp_limb_t *limbs)
{
  hgcd->qp[0] = limbs;
  hgcd->qp[1] = limbs + asize + 1;

  hgcd->qsize[0] = hgcd->qsize[1] = 0;
}

/* Helper functions for hgcd */
/* Sets (a, b, c, d)  <--  (b, c, d, a) */
#define HGCD_SWAP4_LEFT(row)				\
do {							\
  struct hgcd_row __hgcd_swap4_left_tmp;                \
  __hgcd_swap4_left_tmp = row[0];                       \
  row[0] = row[1];					\
  row[1] = row[2];					\
  row[2] = row[3];					\
  row[3] = __hgcd_swap4_left_tmp;			\
} while (0)

/* Sets (a, b, c, d)  <--  (d, a, b, c) */
#define HGCD_SWAP4_RIGHT(row)				\
do {							\
  struct hgcd_row __hgcd_swap4_right_tmp;               \
  __hgcd_swap4_right_tmp = row[3];                      \
  row[3] = row[2];					\
  row[2] = row[1];					\
  row[1] = row[0];					\
  row[0] = __hgcd_swap4_right_tmp;			\
} while (0)

/* Sets (a, b, c, d)  <--  (c, d, a, b) */
#define HGCD_SWAP4_2(row)				\
do {							\
  struct hgcd_row __hgcd_swap4_2_tmp;                   \
  __hgcd_swap4_2_tmp = row[0];                          \
  row[0] = row[2];					\
  row[2] = __hgcd_swap4_2_tmp;				\
  __hgcd_swap4_2_tmp = row[1];				\
  row[1] = row[3];					\
  row[3] = __hgcd_swap4_2_tmp;				\
} while (0)

/* Sets (a, b, c)  <--	(b, c, a) */
#define HGCD_SWAP3_LEFT(row)				\
do {							\
  struct hgcd_row __hgcd_swap4_left_tmp;                \
  __hgcd_swap4_left_tmp = row[0];                       \
  row[0] = row[1];					\
  row[1] = row[2];					\
  row[2] = __hgcd_swap4_left_tmp;			\
} while (0)

/* Computes P = R * S. No overlap allowed.

   Temporary space is needed for two numbers smaller than the
   resulting matrix elements, i.e. bounded by 2*L <= N. */
static mp_size_t
hgcd_mul (struct hgcd_row *P, mp_size_t alloc,
	  const struct hgcd_row *R, mp_size_t rsize,
	  const struct hgcd_row *S, mp_size_t ssize,
	  mp_ptr tp, mp_size_t talloc)
{
  unsigned i;
  unsigned j;

  mp_size_t psize;
  mp_limb_t h = 0;
  int grow = 0;

  MPN_NORMALIZE (R[1].uvp[1], rsize);
  ASSERT (S[1].uvp[1][ssize - 1] != 0);

  psize = rsize + ssize;
  ASSERT (psize <= talloc);

  if (rsize >= ssize)
    {
      for (i = 0; i < 2; i++)
	for (j = 0; j < 2; j++)
	  {
	    /* Set P[i, j] = R[i, 0] S[0, j] + R[i,1] S[1, j] */
	    mp_limb_t cy;

	    mpn_mul (P[i].uvp[j], R[i].uvp[0], rsize, S[0].uvp[j], ssize);
	    mpn_mul (tp, R[i].uvp[1], rsize, S[1].uvp[j], ssize);

	    cy = mpn_add_n (P[i].uvp[j], P[i].uvp[j], tp, psize);

	    if (cy)
	      {
		ASSERT (psize + 1 < alloc);
		P[i].uvp[j][psize] = cy;
		grow = 1;
	      }
	    else
	      h |= P[i].uvp[j][psize - 1];
	  }
    }
  else
    {
      for (i = 0; i < 2; i++)
	for (j = 0; j < 2; j++)
	  {
	    /* Set P[i, j] = R[i, 0] S[0, j] + R[i,1] S[1, j] */
	    mp_limb_t cy;

	    mpn_mul (P[i].uvp[j], S[0].uvp[j], ssize, R[i].uvp[0], rsize);
	    mpn_mul (tp, S[1].uvp[j], ssize, R[i].uvp[1], rsize);

	    cy = mpn_add_n (P[i].uvp[j], P[i].uvp[j], tp, psize);

	    if (cy)
	      {
		ASSERT (psize + 1 < alloc);
		P[i].uvp[j][psize] = cy;
		grow = 1;
	      }
	    else
	      h |= P[i].uvp[j][psize - 1];
	  }
    }

  if (grow)
    return psize + 1;
  else
    return psize - (h == 0);
}

/* Computes R = W^k s->r + s->u A' - s->v B', which must be
   non-negative. W denotes 2^(GMP_NUMB_BITS). Temporary space needed
   is k + uvsize <= M + L = N.

   Must have v > 0, v >= u. */

mp_size_t
mpn_hgcd_fix (mp_size_t k,
	      mp_ptr rp, mp_size_t ralloc,
	      int sign, mp_size_t uvsize,
	      const struct hgcd_row *s,
	      mp_srcptr ap,
	      mp_srcptr bp,
	      mp_ptr tp, mp_size_t talloc)
{
  mp_size_t tsize;
  mp_limb_t cy;
  mp_size_t rsize;
  mp_srcptr up;
  mp_srcptr vp;

  up = s->uvp[0]; vp = s->uvp[1];
  MPN_NORMALIZE (vp, uvsize);
  ASSERT (uvsize > 0);

  if (sign < 0)
    {
      MP_SRCPTR_SWAP (up, vp);
      MP_SRCPTR_SWAP (ap, bp);
    }

  tsize = k + uvsize;

  ASSERT (k + s->rsize <= ralloc);
  ASSERT (tsize <= talloc);
  ASSERT (tsize <= ralloc);

  ASSERT (rp != s->rp);

  /* r = W^k s + u a */
  if (uvsize <= k)
    mpn_mul (rp, ap, k, up, uvsize);
  else
    mpn_mul (rp, up, uvsize, ap, k);

  if (uvsize <= s->rsize)
    {
      cy = mpn_add (rp + k, s->rp, s->rsize, rp + k, uvsize);
      rsize = k + s->rsize;
    }
  else
    {
      cy = mpn_add (rp + k, rp + k, uvsize, s->rp, s->rsize);
      rsize = k + uvsize;
    }

  if (cy)
    {
      ASSERT (rsize < ralloc);
      rp[rsize++] = cy;
    }

  /* r -= v b */

  if (uvsize <= k)
    mpn_mul (tp, bp, k, vp, uvsize);
  else
    mpn_mul (tp, vp, uvsize, bp, k);

  ASSERT_NOCARRY (mpn_sub (rp, rp, rsize, tp, tsize));
  MPN_NORMALIZE (rp, rsize);

  return rsize;
}

/* Compute r2 = r0 - q r1, when q is a single limb */
static void
hgcd_update_r_1 (struct hgcd_row *r, mp_limb_t q)
{
  mp_size_t size;

  if (q == 1)
    {
      ASSERT_NOCARRY (mpn_sub (r[2].rp, r[0].rp, r[0].rsize, r[1].rp, r[1].rsize));
    }
  else
    {
      mp_size_t s = r[1].rsize;
      mp_limb_t cy = mpn_mul_1 (r[2].rp, r[1].rp, s, q);

      if (cy)
	{
	  ASSERT (s < r[0].rsize);
	  r[2].rp[s++] = cy;
	}

      ASSERT_NOCARRY (mpn_sub (r[2].rp, r[0].rp, r[0].rsize, r[2].rp, s));

    }
  /* NOTE: In the usual case, we could take size = r[1].rsize, since
     r[2] is expected to be smaller than r[1], but occasionally, we
     call this with an incorrect quotient resulting in a larger
     r[2]. */
  size = r[0].rsize;
  MPN_NORMALIZE (r[2].rp, size);
  r[2].rsize = size;
}

/* Compute r2 = r0 - q r1 */
static void
hgcd_update_r (struct hgcd_row *r, mp_srcptr qp, mp_size_t qsize)
{
  ASSERT (MPN_LESS_P (r[1].rp, r[1].rsize, r[0].rp, r[0].rsize));
  ASSERT (qsize > 0);

  if (qsize == 1)
    {
      hgcd_update_r_1 (r, qp[0]);
    }
  else
    {
      mp_srcptr r0p = r[0].rp;
      mp_srcptr r1p = r[1].rp;
      mp_ptr r2p = r[2].rp;
      mp_size_t r0size = r[0].rsize;
      mp_size_t r1size = r[1].rsize;

      mp_size_t size = r1size + qsize;
      ASSERT (size <= r0size + 1);

      if (qsize <= r1size)
	mpn_mul (r2p, r1p, r1size, qp, qsize);
      else
	mpn_mul (r2p, qp, qsize, r1p, r1size);

      if (size > r0size)
	{
	  ASSERT (size == r0size + 1);
	  size--;
	  ASSERT (r2p[size] == 0);
	}

      ASSERT_NOCARRY (mpn_sub (r2p, r0p, r0size, r2p, size));

      MPN_NORMALIZE (r[2].rp, r0size);
      r[2].rsize = r0size;

      ASSERT (MPN_LESS_P (r2p, r0size, r1p, r1size));
    }
}

/* Compute (u2, v2) = (u0, v0) + q (u1, v1)
   Return the size of the largest u,v element.
   Caller must ensure that usize + qsize <= available storage */
static mp_size_t
hgcd_update_uv_1 (struct hgcd_row *r, mp_size_t usize,
		  mp_limb_t q)
{
  mp_limb_t hu, hv;
  if (q == 1)
    {
      hu = mpn_add_n (r[2].uvp[0], r[0].uvp[0], r[1].uvp[0], usize);
      hv = mpn_add_n (r[2].uvp[1], r[0].uvp[1], r[1].uvp[1], usize);
    }
  else
    {
      /* Too bad we don't have an addmul_1 with distinct source and
	 destination */
      hu = mpn_mul_1 (r[2].uvp[0], r[1].uvp[0], usize, q);
      hu += mpn_add_n (r[2].uvp[0], r[2].uvp[0], r[0].uvp[0], usize);
      hv = mpn_mul_1 (r[2].uvp[1], r[1].uvp[1], usize, q);
      hv += mpn_add_n (r[2].uvp[1], r[2].uvp[1], r[0].uvp[1], usize);
    }
  if (hu || hv)
    {
      r[2].uvp[0][usize] = hu;
      r[2].uvp[1][usize] = hv;
      usize++;
    }
  return usize;
}

/* Compute (u2, v2) = (u0, v0) + q (u1, v1)
   Return the size of the largest u,v element.
   Caller must ensure that usize + qsize <= available storage */
/* FIXME: Doesn't need the qp, qsize arguments */
static mp_size_t
hgcd_update_uv (struct hgcd_row *r, mp_size_t usize,
		mp_srcptr qp, mp_size_t qsize)
{
  unsigned i;
  mp_size_t grow;

  ASSERT (r[1].uvp[1][usize - 1] != 0);

  /* Compute u2	 = u0 + q u1 */

  if (qsize == 1)
    return hgcd_update_uv_1 (r, usize, qp[0]);
  else
    {
      for (i = 0; i < 2; i++)
	{
	  mp_srcptr u0p = r[0].uvp[i];
	  mp_srcptr u1p = r[1].uvp[i];
	  mp_ptr u2p = r[2].uvp[i];

	  if (qsize <= usize)
	    mpn_mul (u2p, u1p, usize, qp, qsize);
	  else
	    mpn_mul (u2p, qp, qsize, u1p, usize);

	  ASSERT_NOCARRY (mpn_add (u2p, u2p, usize + qsize, u0p, usize));
	  grow = qsize - ((u2p[usize + qsize - 1]) == 0);
	}
    }

  usize += grow;

  /* The values should be allocated with one limb margin */
  ASSERT (mpn_cmp (r[1].uvp[0], r[2].uvp[0], usize) <= 0);
  ASSERT (mpn_cmp (r[1].uvp[1], r[2].uvp[1], usize) <= 0);
  ASSERT (r[2].uvp[1][usize - 1] != 0);

  return usize;
}

/* Compute r0 = r2 + q r1, and the corresponding uv. Uses a
   single-limb q. */

static void
hgcd_backup_1 (struct hgcd_row *r, mp_size_t usize, mp_limb_t q)
{
  /* r0 = r2 + q r1
     u0 = u2 - q u1
     v0 = v2 - q v1 */

  /* Just like for mpn_addmul_1, the r0 is the same size as r1, or one
     limb larger. */

  mp_limb_t cy;
  mp_size_t size;
  if (q == 1)
    {
      ASSERT_NOCARRY (mpn_sub_n (r[0].uvp[0], r[2].uvp[0], r[1].uvp[0], usize));
      ASSERT_NOCARRY (mpn_sub_n (r[0].uvp[1], r[2].uvp[1], r[1].uvp[1], usize));

      cy = mpn_add (r[0].rp, r[1].rp, r[1].rsize, r[2].rp, r[2].rsize);
    }
  else
    {
      ASSERT_NOCARRY (mpn_mul_1 (r[0].uvp[0], r[1].uvp[0], usize, q));
      ASSERT_NOCARRY (mpn_sub_n (r[0].uvp[0], r[2].uvp[0], r[0].uvp[0], usize));

      ASSERT_NOCARRY (mpn_mul_1 (r[0].uvp[1], r[1].uvp[1], usize, q));
      ASSERT_NOCARRY (mpn_sub_n (r[0].uvp[1], r[2].uvp[1], r[0].uvp[1], usize));

      cy = mpn_mul_1 (r[0].rp, r[1].rp, r[1].rsize, q);
      cy += mpn_add (r[0].rp, r[0].rp, r[1].rsize, r[2].rp, r[2].rsize);
    }
  size = r[1].rsize;
  if (cy)
    r[0].rp[size++] = cy;

  r[0].rsize = size;
}

static void
hgcd_backup_r (struct hgcd_row *r,
	       mp_srcptr qp, mp_size_t qsize)
{
  mp_ptr r0p = r[0].rp;
  mp_srcptr r1p = r[1].rp;
  mp_srcptr r2p = r[2].rp;
  mp_size_t r0size;
  mp_size_t r1size = r[1].rsize;
  mp_size_t r2size = r[2].rsize;

  /* r0 = r2 + q r1

     Result must be of size r1size + q1size - 1, or one limb
     larger. */

  r0size = r1size + qsize;
  if (r1size >= qsize)
    mpn_mul (r0p, r1p, r1size, qp, qsize);
  else
    mpn_mul (r0p, qp, qsize, r1p, r1size);

  ASSERT_NOCARRY (mpn_add (r0p, r0p, r0size, r2p, r2size));

  r0size -= (r0p[r0size-1] == 0);
  r[0].rsize = r0size;
}

static void
hgcd_backup_uv (struct hgcd_row *r, mp_size_t usize,
		mp_srcptr qp, mp_size_t qsize)
{
  /* (u0, v0) = (u2, v2) - q (u1, v1) */

  mp_ptr u0p = r[0].uvp[0];
  mp_ptr v0p = r[0].uvp[1];
  mp_srcptr u1p = r[1].uvp[0];
  mp_srcptr v1p = r[1].uvp[1];
  mp_srcptr u2p = r[2].uvp[0];
  mp_srcptr v2p = r[2].uvp[1];
  mp_size_t size;

  /* FIXME: Special case qsize = 1 and q = 1 */

  /* (u0,v0) = (u2,v2) - q (u1, v1) */

  /* We must have

       usize >= #(q u1) >= qsize + #u1 - 1

     which means that u1 must have at least

       usize - #u1 >= qsize - 1

     zero limbs at the high end, and similarly for v1. */

  ASSERT (qsize <= usize);
  size = usize - qsize + 1;

  /* NOTE: Needs an extra limb for the u,v values */

  if (qsize <= size)
    {
      mpn_mul (u0p, u1p, size, qp, qsize);
      mpn_mul (v0p, v1p, size, qp, qsize);
    }
  else
    {
      mpn_mul (u0p, qp, qsize, u1p, size);
      mpn_mul (v0p, qp, qsize, v1p, size);
    }

  /* qsize + size = usize + 1 */
  ASSERT (u0p[usize] == 0);
  ASSERT (v0p[usize] == 0);

  ASSERT_NOCARRY (mpn_sub_n (u0p, u2p, u0p, usize));
  ASSERT_NOCARRY (mpn_sub_n (v0p, v2p, v0p, usize));
}

static void
hgcd_backup (struct hgcd_row *r, mp_size_t usize,
	     mp_srcptr qp, mp_size_t qsize)
{
  if (qsize == 1)
    hgcd_backup_1 (r, usize, qp[0]);

  hgcd_backup_r (r, qp, qsize);
  hgcd_backup_uv (r, usize, qp, qsize);
}

/* Compute r0 = r2 + q r1, and the corresponding uv.
 * q is recovered from the uv matrix, as follows.
 *
 * We have (u2, v2) = (u0, v0) + q (u1, v1).
 *
 * If v1 = 0, we fail, since (u1, v1; u2, v2) must be the identity
 * matrix, and we return 0 for failure.
 *
 * If u1 = 0, then we must have (u1, v1; u2, v2) = (0,1; 1, q),
 * so that q = v2 and (u0, v0) = (1, 0).
 *
 * Else, q = floor(v2 / v1), and (u0, v0) = (u2, v2) - q (u1, v1).
 *
 * In other words, we compute
 *
 *   (q, v0) = divmod(v2 / v1)
 *   u0 = u2 - q u1
 *
 * There's one exception though: If v2 mod v1 = 0, which can happen
 * only when u1 = v1 = 1. Then u0 = 0, v0 = 1 and q = v2 - 1.
 *
 * On success, returns qsize > 0.
 *
 */

static mp_size_t
hgcd_backup_recover_q (struct hgcd_row *r, mp_size_t usize,
		       mp_ptr qp)
{
  mp_size_t qsize;

  mp_ptr u0p = r[0].uvp[0];
  mp_ptr v0p = r[0].uvp[1];
  mp_srcptr u1p = r[1].uvp[0];
  mp_srcptr v1p = r[1].uvp[1];
  mp_srcptr u2p = r[2].uvp[0];
  mp_srcptr v2p = r[2].uvp[1];

  mp_size_t u1size;
  mp_size_t v1size;
  mp_size_t v2size;

  ASSERT (MPN_LEQ_P (r[2].rp, r[2].rsize, r[1].rp, r[1].rsize));

  /* Derive q */
  u1size = v1size = v2size = usize;

  MPN_NORMALIZE (v1p, v1size);
  if (!v1size)
    return 0;

  MPN_NORMALIZE (v2p, v2size);
  MPN_NORMALIZE (u1p, u1size);

  if (!u1size)
    {
      MPN_COPY (qp, v2p, v2size);
      qsize = v2size;

      MPN_ZERO (u0p, usize);
      MPN_ZERO (v0p, usize);
      u0p[0] = 1;
    }
  else if (u1size == 1 && v1size == 1 && u1p[0] == 1 && v1p[0] == 1)
    {
      ASSERT_NOCARRY (mpn_sub_1 (qp, v2p, v2size, 1));
      qsize = v2size;
      qsize -= (qp[qsize - 1] == 0);

      MPN_ZERO (u0p, usize);
      MPN_ZERO (v0p, usize);
      v0p[0] = 1;
    }
  else
    {
      mpn_tdiv_qr (qp, v0p, 0, v2p, v2size, v1p, v1size);
      qsize = v2size - v1size;
      qsize += (qp[qsize] != 0);

      if (v1size < usize)
	MPN_ZERO (v0p + v1size, usize - v1size);

      if (qsize == 1)
	{
	  mp_limb_t q = qp[0];
	  if (q == 1)
	    {
	      ASSERT_NOCARRY (mpn_sub_n (u0p, u2p, u1p, usize));
	    }
	  else
	    {
	      ASSERT_NOCARRY (mpn_mul_1 (u0p, u1p, usize, q));
	      ASSERT_NOCARRY (mpn_sub_n (u0p, u2p, u0p, usize));
	    }
	}
      else
	{
	  /* NOTE: Needs an extra limb for the u0 value */
	  mp_size_t u0size = qsize + u1size;

	  ASSERT (u0size <= usize + 1);
	  if (qsize <= u1size)
	    mpn_mul (u0p, u1p, u1size, qp, qsize);
	  else
	    mpn_mul (u0p, qp, qsize, u1p, u1size);
	  u0size -= (u0p[u0size-1] == 0);

	  ASSERT (u0size <= usize);
	  ASSERT_NOCARRY (mpn_sub (u0p, u2p, usize, u0p, u0size));
	}
    }

  if (qsize == 1)
    {
      /* NOTE: Some duplication with hgcd_backup_1 */
      mp_size_t size;
      mp_limb_t cy;
      mp_limb_t q = qp[0];
      
      size = r[1].rsize;
      if (q == 1)
	cy = mpn_add (r[0].rp, r[1].rp, size, r[2].rp, r[2].rsize);
      else
	{
	  cy = mpn_mul_1 (r[0].rp, r[1].rp, size, q);
	  cy += mpn_add (r[0].rp, r[0].rp, size, r[2].rp, r[2].rsize);
	}
	
      if (cy)
	r[0].rp[size++] = cy;
      r[0].rsize = size;
    }
  else
    hgcd_backup_r (r, qp, qsize);

  return qsize;
}

/* Called after HGCD_SWAP4_RIGHT, to adjust the size field. Large
   numbers in row 0 don't count, and are overwritten. */
static void
hgcd_normalize (struct hgcd *hgcd)
{
  mp_size_t size = hgcd->size;

  /* v3 should always be the largest element */
  while (size > 0 && hgcd->row[3].uvp[1][size - 1] == 0)
    {
      size--;
      /* Row 0 is about to be overwritten. We must zero out unused limbs */
      hgcd->row[0].uvp[0][size] = 0;
      hgcd->row[0].uvp[1][size] = 0;

      ASSERT (hgcd->row[1].uvp[0][size] == 0);
      ASSERT (hgcd->row[1].uvp[1][size] == 0);
      ASSERT (hgcd->row[2].uvp[0][size] == 0);
      ASSERT (hgcd->row[2].uvp[1][size] == 0);
      ASSERT (hgcd->row[3].uvp[0][size] == 0);
    }

  hgcd->size = size;
}

int
mpn_hgcd2_lehmer_step (struct hgcd2 *hgcd,
		       mp_srcptr ap, mp_size_t asize,
		       mp_srcptr bp, mp_size_t bsize)
{
  mp_limb_t ah;
  mp_limb_t al;
  mp_limb_t bh;
  mp_limb_t bl;

  ASSERT (asize >= bsize);
  ASSERT (MPN_LEQ_P (bp, bsize, ap, asize));

  if (bsize < 2)
    return 0;

  /* The case asize == 2 is needed to take care of values that are
     between one and two *full* limbs in size. */
  if (asize == 2 || (ap[asize-1] & GMP_NUMB_HIGHBIT))
    {
      if (bsize < asize)
	return 0;

      al = ap[asize - 2];
      ah = ap[asize - 1];

      ASSERT (asize == bsize);
      bl = bp[asize - 2];
      bh = bp[asize - 1];
    }
  else
    {
      unsigned shift;
      if (bsize + 1 < asize)
	return 0;

      /* We want two *full* limbs */
      ASSERT (asize > 2);

      count_leading_zeros (shift, ap[asize-1]);
      if (bsize == asize)
	bh = MPN_EXTRACT_LIMB (shift, bp[asize - 1], bp[asize - 2]);
      else
	{
	  ASSERT (asize == bsize + 1);
	  bh = bp[asize - 2] >> (GMP_LIMB_BITS - shift);
	}

      bl = MPN_EXTRACT_LIMB (shift, bp[asize - 2], bp[asize - 3]);

      al = MPN_EXTRACT_LIMB (shift, ap[asize - 2], ap[asize - 3]);
      ah = MPN_EXTRACT_LIMB (shift, ap[asize - 1], ap[asize - 2]);
    }

  return mpn_hgcd2 (hgcd, ah, al, bh, bl);
}

static void
hgcd_start (struct hgcd *hgcd,
	    mp_srcptr ap, mp_size_t asize,
	    mp_srcptr bp, mp_size_t bsize)
{
  MPN_COPY (hgcd->row[0].rp, ap, asize);
  hgcd->row[0].rsize = asize;

  MPN_COPY (hgcd->row[1].rp, bp, bsize);
  hgcd->row[1].rsize = bsize;

  hgcd->sign = 0;
  if (hgcd->size != 0)
    {
      /* We must zero out the uv array */
      unsigned i;
      unsigned j;

      for (i = 0; i < 4; i++)
	for (j = 0; j < 2; j++)
	  MPN_ZERO (hgcd->row[i].uvp[j], hgcd->size);
    }

  hgcd->size = 1;
  hgcd->row[0].uvp[0][0] = 1;
  hgcd->row[1].uvp[1][0] = 1;
}

/* Performs one euclid step on r0, r1. Returns >= 0 if hgcd should be
   terminated, -1 if we should go on */
static int
euclid_step (struct hgcd *hgcd, mp_size_t M)
{
  mp_size_t asize;

  mp_size_t qsize;
  mp_ptr qp;
  mp_size_t rsize;
  mp_ptr rp;

  asize = hgcd->row[0].rsize;
  rsize = hgcd->row[1].rsize;
  qsize = asize - rsize + 1;

  rp = hgcd->row[2].rp;
  /* If we need to return q, it's always as q1 */
  qp = hgcd->qp[1];

  mpn_tdiv_qr (qp, rp, 0, hgcd->row[0].rp, asize, hgcd->row[1].rp, rsize);
  MPN_NORMALIZE (rp, rsize);
  hgcd->row[2].rsize = rsize;

  qsize -= (qp[qsize - 1] == 0);

  /* Update u and v */
  ASSERT (hgcd->size + qsize <= hgcd->alloc);
  hgcd->size = hgcd_update_uv (hgcd->row, hgcd->size, qp, qsize);
  ASSERT (hgcd->size < hgcd->alloc);

  if (hgcd->row[2].rsize <= M)
    {
      HGCD_SWAP4_RIGHT (hgcd->row);
      hgcd_normalize (hgcd);

      hgcd->qsize[1] = qsize;
      hgcd->qsize[0] = hgcd_backup_recover_q (hgcd->row, hgcd->size,
					      hgcd->qp[0]);
      if (!hgcd->qsize[0])
	return 0;

      hgcd->sign = ~hgcd->sign;

      return hgcd_jebelean (hgcd, M);
    }
  else
    {
      /* Keep this remainder */
      hgcd->sign = ~hgcd->sign;

      HGCD_SWAP4_LEFT (hgcd->row);
      return -1;
    }
}

/* Called when values have been computed in r[0] and r[1], and the
   latter value is too large, and we know that it's not much too
   large. Returns the updated size for the uv matrix. */
static mp_size_t
hgcd_adjust (struct hgcd_row *r, mp_size_t size, mp_limb_t *dp)
{
  mp_limb_t c0;
  mp_limb_t c1;
  mp_limb_t d;

  /* Compute the correct r1. We have r1' = r1 - d r0, and we always
     have d = 1 or 2. */

  ASSERT_NOCARRY (mpn_sub (r[1].rp, r[1].rp, r[1].rsize, r[0].rp, r[0].rsize));

  MPN_NORMALIZE (r[1].rp, r[1].rsize);

  if (MPN_LESS_P (r[1].rp, r[1].rsize, r[0].rp, r[0].rsize))
    {
      c0 = mpn_add_n (r[1].uvp[0], r[1].uvp[0], r[0].uvp[0], size);
      c1 = mpn_add_n (r[1].uvp[1], r[1].uvp[1], r[0].uvp[1], size);
      d = 1;
    }
  else
    {
      ASSERT_NOCARRY (mpn_sub (r[1].rp, r[1].rp, r[1].rsize, r[0].rp, r[0].rsize));
      MPN_NORMALIZE (r[1].rp, r[1].rsize);
      ASSERT (MPN_LESS_P (r[1].rp, r[1].rsize, r[0].rp, r[0].rsize));

      c0 = mpn_addmul_1 (r[1].uvp[0], r[0].uvp[0], size, 2);
      c1 = mpn_addmul_1 (r[1].uvp[1], r[0].uvp[1], size, 2);
      d = 2;
    }

  /* FIXME: Can avoid branches */
  if (c1 != 0)
    {
      r[1].uvp[0][size] = c0;
      r[1].uvp[1][size] = c1;
      size++;
    }
  else
    {
      ASSERT (c0 == 0);
    }

  if (dp)
    *dp = d;

  return size;
}

/* Reduce using Lehmer steps. Called by mpn_hgcd when r1 has been
   reduced to approximately the right size. Also used by
   mpn_hgcd_lehmer. */
static int
hgcd_final (struct hgcd *hgcd, mp_size_t M)
{
  ASSERT (hgcd->row[0].rsize > M);
  ASSERT (hgcd->row[1].rsize > M);

  /* Can be equal when called by hgcd_lehmer. */
  ASSERT (MPN_LEQ_P (hgcd->row[1].rp, hgcd->row[1].rsize,
		     hgcd->row[0].rp, hgcd->row[0].rsize));

  for (;;)
    {
      mp_size_t L = hgcd->row[0].rsize;

      struct hgcd2 R;
      int res;

      if (L <= M + 2
	  && (L < M + 2 || (hgcd->row[0].rp[M+1] & GMP_NUMB_HIGHBIT) == 0))
	break;

      res = mpn_hgcd2_lehmer_step (&R,
				   hgcd->row[0].rp, hgcd->row[0].rsize,
				   hgcd->row[1].rp, hgcd->row[1].rsize);

      if (res == 0)
	{
	  /* We must divide to make progress */
	  res = euclid_step (hgcd, M);

	  if (res >= 0)
	    return res;
	}
      else if (res == 1)
	{
	  mp_limb_t q;

	  /* The quotient that has been computed for r2 is at most 2
	     off. So adjust that, and avoid a full division. */

	  q = R.q[0];

	  /* Top two rows of R must be the identity matrix, followed
	     by a row (1, q). */
	  ASSERT (R.row[0].u == 1 && R.row[0].v == 0);
	  ASSERT (R.row[1].u == 0 && R.row[1].v == 1);
	  ASSERT (R.row[2].u == 1 && R.row[2].v == q);

	  hgcd_update_r_1 (hgcd->row, q);
	  hgcd->size = hgcd_update_uv_1 (hgcd->row, hgcd->size, q);
	  ASSERT (hgcd->size < hgcd->alloc);

	  if (MPN_LEQ_P (hgcd->row[1].rp, hgcd->row[1].rsize,
			 hgcd->row[2].rp, hgcd->row[2].rsize))
	    hgcd->size = hgcd_adjust (hgcd->row + 1, hgcd->size, NULL);

	  ASSERT (hgcd->size < hgcd->alloc);

	  hgcd->sign = ~hgcd->sign;
	  HGCD_SWAP4_LEFT (hgcd->row);
	}
      else
	{
	  const struct hgcd2_row *s = R.row + (res - 2);
	  int sign = R.sign;
	  /* Max size after reduction, plus one */
	  mp_size_t ralloc = hgcd->row[1].rsize + 1;

	  if (res == 3)
	    sign = ~sign;

	  /* s[0] and s[1] correct. */
	  hgcd->row[2].rsize
	    = mpn_hgcd2_fix (hgcd->row[2].rp, ralloc,
			     sign,
			     s[0].u, hgcd->row[0].rp, hgcd->row[0].rsize,
			     s[0].v, hgcd->row[1].rp, hgcd->row[1].rsize);

	  hgcd->row[3].rsize
	    = mpn_hgcd2_fix (hgcd->row[3].rp, ralloc,
			     ~sign,
			     s[1].u, hgcd->row[0].rp, hgcd->row[0].rsize,
			     s[1].v, hgcd->row[1].rp, hgcd->row[1].rsize);

	  hgcd->size = hgcd2_mul (hgcd->row + 2, hgcd->alloc,
				  s, hgcd->row, hgcd->size);
	  hgcd->sign ^= sign;

	  ASSERT (hgcd->row[2].rsize > M);

	  if (hgcd->row[3].rsize <= M)
	    {
	      /* Can happen only in the res == 4 case */
	      ASSERT (res == 4);

	      /* Backup two steps */
	      hgcd_backup_1 (hgcd->row + 1, hgcd->size, R.q[1]);
	      hgcd_backup_1 (hgcd->row, hgcd->size, R.q[0]);

	      hgcd->qsize[0] = hgcd->qsize[1] = 1;
	      hgcd->qp[0][0] = R.q[0];
	      hgcd->qp[1][0] = R.q[1];

	      return hgcd_jebelean (hgcd, M);
	    }

	  HGCD_SWAP4_2 (hgcd->row);
	}
    }

  ASSERT (hgcd->row[1].rsize > M);

  for (;;)
    {
      mp_size_t L = hgcd->row[0].rsize;
      mp_size_t ralloc;

      struct hgcd2 R;
      int res;

      /* We don't want hgcd2 to pickup any bits below r0p[M-1], so
	 don't tell mpn_hgcd2_lehmer_step about them. */
      res = mpn_hgcd2_lehmer_step (&R,
				   hgcd->row[0].rp+M-1, hgcd->row[0].rsize-M+1,
				   hgcd->row[1].rp+M-1, hgcd->row[1].rsize-M+1);
      if (res == 0)
	{
	  /* We must divide to make progress */
	  res = euclid_step (hgcd, M);

	  if (res >= 0)
	    return res;

	  continue;
	}

      if (res == 1)
	{
	  mp_limb_t q;

	  q = R.q[0];

	  /* The quotient that has been computed for r2 is at most 2
	     off. So adjust that, and avoid a full division. */

	  /* Top two rows of R must be the identity matrix, followed
	     by a row (1, q). */
	  ASSERT (R.row[0].u == 1 && R.row[0].v == 0);
	  ASSERT (R.row[1].u == 0 && R.row[1].v == 1);
	  ASSERT (R.row[2].u == 1 && R.row[2].v == q);

	  hgcd_update_r_1 (hgcd->row, q);
	  hgcd->size = hgcd_update_uv_1 (hgcd->row, hgcd->size, q);
	  ASSERT (hgcd->size < hgcd->alloc);

	  if (MPN_LEQ_P (hgcd->row[1].rp, hgcd->row[1].rsize,
			 hgcd->row[2].rp, hgcd->row[2].rsize))
	    hgcd->size = hgcd_adjust (hgcd->row + 1, hgcd->size, NULL);

	  ASSERT (hgcd->size < hgcd->alloc);

	  hgcd->sign = ~hgcd->sign;
	  HGCD_SWAP4_LEFT (hgcd->row);

	  continue;
	}

      /* Now r0 and r1 are always correct. */
      /* Store new values in rows 2 and 3, to avoid overlap */

      /* Max size after reduction, plus one */
      ralloc = hgcd->row[1].rsize + 1;

      hgcd->row[2].rsize
	= mpn_hgcd2_fix (hgcd->row[2].rp, ralloc,
			 R.sign,
			 R.row[0].u, hgcd->row[0].rp, hgcd->row[0].rsize,
			 R.row[0].v, hgcd->row[1].rp, hgcd->row[1].rsize);

      hgcd->row[3].rsize
	= mpn_hgcd2_fix (hgcd->row[3].rp, ralloc,
			 ~R.sign,
			 R.row[1].u, hgcd->row[0].rp, hgcd->row[0].rsize,
			 R.row[1].v, hgcd->row[1].rp, hgcd->row[1].rsize);

      ASSERT (hgcd->row[2].rsize >= L - 1);
      ASSERT (hgcd->row[3].rsize >= L - 2);

      ASSERT (hgcd->row[2].rsize > M);
      ASSERT (hgcd->row[3].rsize > M-1);

      hgcd->size = hgcd2_mul (hgcd->row + 2, hgcd->alloc,
			      R.row, hgcd->row, hgcd->size);
      hgcd->sign ^= R.sign;

      if (hgcd->row[3].rsize <= M)
	{
	  /* Backup two steps */
	  hgcd->qsize[1] = hgcd_backup_recover_q (hgcd->row + 1, hgcd->size,
						  hgcd->qp[1]);
	  ASSERT (hgcd->qsize[1] > 0);

	  hgcd->qsize[0] = hgcd_backup_recover_q (hgcd->row, hgcd->size,
						  hgcd->qp[0]);
	  ASSERT (hgcd->qsize[0] > 0);

	  return hgcd_jebelean (hgcd, M);
	}

      HGCD_SWAP4_2 (hgcd->row);

      if (res == 2)
	continue;

      /* We already know the correct q for computing r2 */

      hgcd->qp[0][0] = R.q[0];
      hgcd->qsize[0] = 1;

      ASSERT (hgcd->size < hgcd->alloc);
      hgcd_update_r_1 (hgcd->row, R.q[0]);
      hgcd->size = hgcd_update_uv_1 (hgcd->row, hgcd->size, R.q[0]);
      ASSERT (hgcd->size < hgcd->alloc);

      ASSERT (hgcd->row[2].rsize >= M - 2);

      if (hgcd->row[2].rsize <= M)
	{
	  /* Discard r3 */

	  HGCD_SWAP4_RIGHT (hgcd->row);
	  hgcd_normalize (hgcd);

	  hgcd->qp[1][0] = R.q[0];
	  hgcd->qsize[1] = 1;

	  hgcd->qsize[0] = hgcd_backup_recover_q (hgcd->row, hgcd->size,
						  hgcd->qp[0]);
	  if (!hgcd->qsize[0])
	    /* Can this happen */
	    return 0;

	  hgcd->sign = ~hgcd->sign;

	  return hgcd_jebelean (hgcd, M);
	}
      if (res == 3)
	{
	  /* Drop quotient for r3 */
	  hgcd->sign = ~hgcd->sign;
	  HGCD_SWAP4_LEFT (hgcd->row);

	  continue;
	}

      ASSERT (res == 4);
      ASSERT (hgcd->row[2].rsize > M);

      /* We already know the correct q for computing r3 */
      ASSERT (hgcd->size < hgcd->alloc);
      hgcd_update_r_1 (hgcd->row + 1, R.q[1]);
      hgcd->size = hgcd_update_uv_1 (hgcd->row + 1, hgcd->size, R.q[1]);
      ASSERT (hgcd->size < hgcd->alloc);

      ASSERT (hgcd->row[3].rsize <= M + 1);
      /* Appearantly not true. Probably because we have leading zeros
	 when we call hgcd2. */
      /* ASSERT (hgcd->row[3].rsize <= M || hgcd->row[3].rp[M] == 1); */

      if (hgcd->row[3].rsize <= M)
	{
	  hgcd->qp[0][0] = R.q[0]; hgcd->qsize[0] = 1;
	  hgcd->qp[1][0] = R.q[1]; hgcd->qsize[1] = 1;

	  return hgcd_jebelean (hgcd, M);
	}

      HGCD_SWAP4_2 (hgcd->row);
    }
}

mp_size_t
mpn_hgcd_itch (mp_size_t asize)
{
  /* Scratch space is needed for calling hgcd. We need space for the
     results of all recursive calls. In addition, we need space for
     calling hgcd_fix and hgcd_mul, for which N = asize limbs should
     be enough. */

  /* Limit on the recursion depth */
  unsigned k = mpn_hgcd_max_recursion (asize);

  return asize + mpn_hgcd_init_itch (asize + 6 * k) + 12 * k;
}

/* Repeatedly divides A by B, until the remainder fits in M =
   ceil(asize / 2) limbs. Stores cofactors in HGCD, and pushes the
   quotients on STACK. On success, HGCD->row[0, 1, 2] correspond to
   remainders that are larger than M limbs, while HGCD->row[3]
   correspond to a remainder that fit in M limbs.

   Return 0 on failure (if B or A mod B fits in M limbs), otherwise
   return one of 1 - 4 as specified for hgcd_jebelean. */
int
mpn_hgcd (struct hgcd *hgcd,
	  mp_srcptr ap, mp_size_t asize,
	  mp_srcptr bp, mp_size_t bsize,
	  mp_ptr tp, mp_size_t talloc)
{
  mp_size_t N = asize;
  mp_size_t M = (N + 1)/2;
  mp_size_t n;
  mp_size_t m;

  struct hgcd R;
  mp_size_t itch;

  ASSERT (M);

  if (bsize <= M)
    return 0;

  ASSERT (asize >= 2);

  /* Initialize, we keep r0 and r1 as the reduced numbers (so far). */
  hgcd_start (hgcd, ap, asize, bp, bsize);

  if (BELOW_THRESHOLD (N, HGCD_SCHOENHAGE_THRESHOLD))
    return hgcd_final (hgcd, M);

  /* Reduce the size to M + m + 1. Usually, only one hgcd call is
     needed, but we may need multiple calls. When finished, the values
     are stored in r0 (potentially large) and r1 (smaller size) */

  n = N - M;
  m = (n + 1)/2;

  /* The second recursive call can use numbers of size up to n+3 */
  itch = mpn_hgcd_init_itch (n+3);

  ASSERT (itch <= talloc);
  mpn_hgcd_init (&R, n+3, tp);
  tp += itch; talloc -= itch;

  /* Use same storage for quotients */
  R.qp[0] = hgcd->qp[0];
  R.qp[1] = hgcd->qp[1];

  while (hgcd->row[1].rsize > M + m + 1)
    {
      /* Max size after reduction, plus one */
      mp_size_t ralloc = hgcd->row[1].rsize + 1;

      int res = mpn_hgcd (&R,
			  hgcd->row[0].rp + M, hgcd->row[0].rsize - M,
			  hgcd->row[1].rp + M, hgcd->row[1].rsize - M,
			  tp, talloc);

      if (res == 0)
	{
	  /* We must divide to make progress */
	  res = euclid_step (hgcd, M);

	  if (res >= 0)
	    return res;
	}
      else if (res <= 2)
	{
	  /* The reason we use hgcd_adjust also when res == 2 is that
	     either r2 is correct, and we get it for free.

	     Or r2 is too large. Then can correct it by a few bignum
	     subtractions, and we are *guaranteed* that the result is
	     small enough that we don't need another run through this
	     loop. */

	  /* FIXME: For res == 1, the newly computed row[2] will be
	     the same as the old row[1], so we do some unnecessary
	     computations. */

	  /* Store new values in rows 2 and 3, to avoid overlap */
	  hgcd->row[2].rsize
	    = mpn_hgcd_fix (M, hgcd->row[2].rp, ralloc,
			    ~R.sign, R.size, &R.row[1],
			    hgcd->row[0].rp, hgcd->row[1].rp,
			    tp, talloc);

	  hgcd->row[3].rsize
	    = mpn_hgcd_fix (M, hgcd->row[3].rp, ralloc,
			    R.sign, R.size, &R.row[2],
			    hgcd->row[0].rp, hgcd->row[1].rp,
			    tp, talloc);

	  ASSERT (hgcd->row[2].rsize > M);
	  ASSERT (hgcd->row[3].rsize > M);

	  /* Computes the uv matrix for the (possibly incorrect)
	     values r1, r2. The elements must be smaller than the
	     correct ones, since they correspond to a too small q. */

	  hgcd->size = hgcd_mul (hgcd->row + 2, hgcd->alloc,
				 R.row + 1, R.size,
				 hgcd->row, hgcd->size,
				 tp, talloc);
	  hgcd->sign ^= ~R.sign;

	  if (MPN_LESS_P (hgcd->row[3].rp, hgcd->row[3].rsize,
			  hgcd->row[2].rp, hgcd->row[2].rsize))
	    {
	      HGCD_SWAP4_2 (hgcd->row);
	    }
	  else
	    {
	      /* r2 was too large, i.e. q0 too small. In this case we
		 must have r2 % r1 <= r2 - r1 smaller than M + m + 1. */
	      mp_limb_t d;

	      hgcd->size = hgcd_adjust (hgcd->row + 2, hgcd->size, &d);
	      ASSERT (hgcd->row[3].rsize <= M + m + 1);

	      if (hgcd->row[3].rsize <= M)
		{
		  /* Backup two steps. We already know one of the quotients. */
		  mp_limb_t cy;
		  hgcd->qp[1] = R.qp[0]; hgcd->qsize[1] = R.qsize[0];

		  /* Add in adjustment */
		  cy = mpn_add_1 (hgcd->qp[1], hgcd->qp[1], hgcd->qsize[1], d);
		  if (cy)
		    hgcd->qp[1][hgcd->qsize[1]++] = cy;

		  hgcd_backup (hgcd->row + 1, hgcd->size, hgcd->qp[1], hgcd->qsize[1]);

		  hgcd->qp[0] = R.qp[1];
		  hgcd->qsize[0] = hgcd_backup_recover_q (hgcd->row, hgcd->size,
							  hgcd->qp[0]);
		  if (!hgcd->qsize[0])
		    /* Can this happen? */
		    return 0;

		  return hgcd_jebelean (hgcd, M);
		}

	      HGCD_SWAP4_2 (hgcd->row);

	      /* Loop always terminates here. */
	      break;
	    }
	}
      else if (res == 3)
	{
	  /* Store new values in rows 2 and 3, to avoid overlap */
	  hgcd->row[2].rsize
	    = mpn_hgcd_fix (M, hgcd->row[2].rp, ralloc,
			    ~R.sign, R.size, &R.row[1],
			    hgcd->row[0].rp, hgcd->row[1].rp,
			    tp, talloc);

	  hgcd->row[3].rsize
	    = mpn_hgcd_fix (M, hgcd->row[3].rp, ralloc,
			    R.sign, R.size, &R.row[2],
			    hgcd->row[0].rp, hgcd->row[1].rp,
			    tp, talloc);

	  ASSERT (hgcd->row[2].rsize > M);
	  ASSERT (hgcd->row[3].rsize > M);

	  hgcd->size = hgcd_mul (hgcd->row + 2, hgcd->alloc,
				 R.row + 1, R.size,
				 hgcd->row, hgcd->size,
				 tp, talloc);
	  hgcd->sign ^= ~R.sign;

	  HGCD_SWAP4_2 (hgcd->row);
	}
      else
	{
	  ASSERT (res == 4);

	  /* All of r0, r1, r3 and r3 are correct.
	     Compute r2 and r3 */

	  /* Store new values in rows 2 and 3, to avoid overlap */
	  hgcd->row[2].rsize
	    = mpn_hgcd_fix (M, hgcd->row[2].rp, ralloc,
			    R.sign, R.size, &R.row[2],
			    hgcd->row[0].rp, hgcd->row[1].rp,
			    tp, talloc);

	  hgcd->row[3].rsize
	    = mpn_hgcd_fix (M, hgcd->row[3].rp, ralloc,
			    ~R.sign, R.size, &R.row[3],
			    hgcd->row[0].rp, hgcd->row[1].rp,
			    tp, talloc);

	  ASSERT (hgcd->row[2].rsize > M);
	  ASSERT (hgcd->row[3].rsize <= M + m + 1);

	  hgcd->size = hgcd_mul (hgcd->row+2, hgcd->alloc,
				 R.row+2, R.size,
				 hgcd->row, hgcd->size,
				 tp, talloc);
	  hgcd->sign ^= R.sign;

	  if (hgcd->row[3].rsize <= M)
	    {
	      /* Backup two steps */

	      /* Backup two steps */
	      hgcd_backup (hgcd->row + 1, hgcd->size, R.qp[1], R.qsize[1]);
	      hgcd_backup (hgcd->row, hgcd->size, R.qp[0], R.qsize[0]);

	      hgcd->qp[0] = R.qp[0]; hgcd->qsize[0] = R.qsize[0];
	      hgcd->qp[1] = R.qp[1]; hgcd->qsize[1] = R.qsize[1];

	      return hgcd_jebelean (hgcd, M);
	    }
	  HGCD_SWAP4_2 (hgcd->row);

	  /* Always exit the loop. */
	  break;
	}
    }

  ASSERT (hgcd->row[0].rsize >= hgcd->row[1].rsize);
  ASSERT (hgcd->row[1].rsize > M);
  ASSERT (hgcd->row[1].rsize <= M + m + 1);

  if (hgcd->row[0].rsize > M + m + 1)
    {
      /* One euclid step to reduce size. */
      int res = euclid_step (hgcd, M);

      if (res >= 0)
	return res;
    }

  ASSERT (hgcd->row[0].rsize >= hgcd->row[1].rsize);
  ASSERT (hgcd->row[0].rsize <= M + m + 1);
  ASSERT (hgcd->row[1].rsize > M);

  /* Second phase, reduce size until we have one number of size > M
     and one of size <= M+1 */
  while (hgcd->row[1].rsize > M + 1)
    {
      mp_size_t k = 2*M - hgcd->row[0].rsize;
      mp_size_t n1 = hgcd->row[0].rsize - k;
      int res;

      ASSERT (k + (n1 + 1)/2 == M);
      ASSERT (n1 >= 2);

      ASSERT (n1 <= 2*(m + 1));
      ASSERT (n1 <= n + 3);

      res = mpn_hgcd (&R,
		      hgcd->row[0].rp + k, hgcd->row[0].rsize - k,
		      hgcd->row[1].rp + k, hgcd->row[1].rsize - k,
		      tp, talloc);

      if (res == 0)
	{
	  /* The first remainder was small. Then there's a good chance
	     that the remainder A % B is also small. */
	  res = euclid_step (hgcd, M);

	  if (res >= 0)
	    return res;

	  continue;
	}

      if (res == 1)
	{
	  /* Compute possibly incorrect r2 and corresponding u2, v2.
	     Incorrect matrix elements must be smaller than the
	     correct ones, since they correspond to a too small q. */

	  mp_limb_t d;

	  /* Row 2 must be (1, q) */
	  ASSERT (R.qsize[0] + hgcd->size <= hgcd->alloc);
	  hgcd_update_r (hgcd->row, R.qp[0], R.qsize[0]);
	  hgcd->size = hgcd_update_uv (hgcd->row, hgcd->size,
				       R.qp[0], R.qsize[0]);
	  ASSERT (hgcd->size < hgcd->alloc);

	  if (!MPN_LESS_P (hgcd->row[3].rp, hgcd->row[3].rsize,
			   hgcd->row[2].rp, hgcd->row[2].rsize))
	    hgcd->size = hgcd_adjust (hgcd->row + 1, hgcd->size, &d);

	  if (hgcd->row[2].rsize <= M)
	    {
	      /* Discard r3, and backup one steps */
	      mp_limb_t cy;

	      HGCD_SWAP4_RIGHT (hgcd->row);
	      hgcd_normalize (hgcd);

	      hgcd->qp[1] = R.qp[0];
	      hgcd->qsize[1] = R.qsize[0];

	      /* Add in adjustment */
	      cy = mpn_add_1 (hgcd->qp[1], hgcd->qp[1], hgcd->qsize[1], d);
	      if (cy)
		hgcd->qp[1][hgcd->qsize[1]++] = cy;

	      hgcd->qp[0] = R.qp[1];
	      hgcd->qsize[0] = hgcd_backup_recover_q (hgcd->row, hgcd->size,
						      hgcd->qp[0]);
	      if (!hgcd->qsize[0])
		/* Can this happen */
		return 0;

	      hgcd->sign = ~hgcd->sign;

	      return hgcd_jebelean (hgcd, M);
	    }

	  HGCD_SWAP4_LEFT (hgcd->row);
	  hgcd->sign = ~hgcd->sign;
	  continue;
	}

      /* Now r0 and r1 are always correct. */

      /* It's possible that first two "new" r:s are the same as the
	 old ones. In that case skip recomputing them. */

      if (!hgcd_start_row_p (&R.row[0], R.size))
	{
	  /* Store new values in rows 2 and 3, to avoid overlap */
	  hgcd->row[2].rsize
	    = mpn_hgcd_fix (k, hgcd->row[2].rp, hgcd->row[0].rsize + 1,
			    R.sign, R.size, &R.row[0],
			    hgcd->row[0].rp, hgcd->row[1].rp,
			    tp, talloc);

	  hgcd->row[3].rsize
	    = mpn_hgcd_fix (k, hgcd->row[3].rp, hgcd->row[1].rsize + 1,
			    ~R.sign, R.size, &R.row[1],
			    hgcd->row[0].rp, hgcd->row[1].rp,
			    tp, talloc);

	  ASSERT (hgcd->row[2].rsize > M);
	  ASSERT (hgcd->row[3].rsize > k);

	  hgcd->size = hgcd_mul (hgcd->row+2, hgcd->alloc,
				 R.row, R.size, hgcd->row, hgcd->size,
				 tp, talloc);
	  hgcd->sign ^= R.sign;

	  if (hgcd->row[3].rsize <= M)
	    {
	      /* Backup two steps */

	      hgcd->qsize[1] = hgcd_backup_recover_q (hgcd->row + 1, hgcd->size,
						      hgcd->qp[1]);
	      ASSERT (hgcd->qsize[1] > 0);

	      hgcd->qsize[0] = hgcd_backup_recover_q (hgcd->row, hgcd->size,
						      hgcd->qp[0]);
	      ASSERT (hgcd->qsize[0] > 0);

	      return hgcd_jebelean (hgcd, M);
	    }

	  HGCD_SWAP4_2 (hgcd->row);

	  if (res == 2)
	    continue;
	}

      ASSERT (res >= 3);

      /* We already know the correct q */

      ASSERT (R.qsize[0] + hgcd->size <= hgcd->alloc);
      hgcd_update_r (hgcd->row, R.qp[0], R.qsize[0]);
      hgcd->size = hgcd_update_uv (hgcd->row, hgcd->size,
				   R.qp[0], R.qsize[0]);
      ASSERT (hgcd->size < hgcd->alloc);

      ASSERT (hgcd->row[2].rsize > k);
      if (hgcd->row[2].rsize <= M)
	{
	  /* Discard r3 */
	  HGCD_SWAP4_RIGHT (hgcd->row);
	  hgcd_normalize (hgcd);

	  hgcd->qp[1] = R.qp[0];
	  hgcd->qsize[1] = R.qsize[0];

	  hgcd->qp[0] = R.qp[1];
	  hgcd->qsize[0] = hgcd_backup_recover_q (hgcd->row, hgcd->size,
						  hgcd->qp[0]);
	  if (!hgcd->qsize[0])
	    /* Can this happen */
	    return 0;

	  hgcd->sign = ~hgcd->sign;

	  return hgcd_jebelean (hgcd, M);
	}
      if (res == 3)
	{
	  /* Drop quotient for r3 */
	  hgcd->sign = ~hgcd->sign;
	  HGCD_SWAP4_LEFT (hgcd->row);

	  continue;
	}

      ASSERT (hgcd->row[2].rsize > M);
      ASSERT (res == 4);

      /* We already know the correct q */

      ASSERT (R.qsize[1] + hgcd->size <= hgcd->alloc);
      hgcd_update_r (hgcd->row + 1, R.qp[1], R.qsize[1]);
      hgcd->size = hgcd_update_uv (hgcd->row + 1, hgcd->size,
				   R.qp[1], R.qsize[1]);
      ASSERT (hgcd->size < hgcd->alloc);
      ASSERT (hgcd->row[3].rsize <= M + 1);

      if (hgcd->row[3].rsize <= M)
	{
	  hgcd->qp[0] = R.qp[0]; hgcd->qsize[0] = R.qsize[0];
	  hgcd->qp[1] = R.qp[1]; hgcd->qsize[1] = R.qsize[1];
	  return hgcd_jebelean (hgcd, M);
	}
      HGCD_SWAP4_2 (hgcd->row);
    }

  return hgcd_final (hgcd, M);
}