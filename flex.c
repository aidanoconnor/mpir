/*
Wrapper functions.
Use standard calling convention (_stdcall) forcompatibility with Excel VBA.
Use HRESULT return type.
*/

/*

Copyright 1996, 1999, 2000, 2001 Free Software Foundation, Inc.

Copyright 2008 William Hart.

This file is part of the MPIR Library.

The MPIR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPIR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPIR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA. */

//#include <winnt.h> // For HRESULT
//#include <winerror.h> // For HRESULT values

#include <windows.h>

#include "mpir.h"
#include "gmp-impl.h"


TL_DSP_LIB_DEF tl_mpz_init(mpz_ptr x)
{
	mpz_init(x);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_init2(mpz_ptr x, mp_bitcnt_t bits)
{
	mpz_init2(x, bits);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_clear(mpz_ptr m)
{
	mpz_clear(m);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_realloc2(mpz_ptr m, mp_bitcnt_t bits)
{
	mpz_realloc2(m, bits);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_set(mpz_ptr w, mpz_srcptr u)
{
	mpz_set(w, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_set_ui(mpz_ptr dest, mpir_ui val)
{
	mpz_set_ui(dest, val);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_set_si(mpz_ptr dest, mpir_si val)
{
	mpz_set_si(dest, val);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_set_d(mpz_ptr r, double d)
{
	mpz_set_d(r, d);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_set_q(mpz_ptr w, mpq_srcptr u)
{
	mpz_set_q(w, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_set_f(mpz_ptr w, mpf_srcptr u)
{
	mpz_set_f(w, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_set_str(mpz_ptr x, const char *str, int base, int* ret)
{
	*ret = mpz_set_str(x, str, base);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_swap(mpz_ptr u, mpz_ptr v)
{
	mpz_swap(u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_init_set(mpz_ptr w, mpz_srcptr u)
{
	mpz_init_set(w, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_init_set_ui(mpz_ptr dest, mpir_ui val)
{
	mpz_init_set_ui(dest, val);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_init_set_si(mpz_ptr dest, mpir_si val)
{
	mpz_init_set_si(dest, val);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_init_set_d(mpz_ptr dest, double val)
{
	mpz_init_set_d(dest, val);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_init_set_str(mpz_ptr x, const char *str, int base, int* ret)
{
	*ret = mpz_init_set_str(x, str, base);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_get_ui(mpz_srcptr z, mpir_ui* ret)
{
	*ret = mpz_get_ui(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_get_si(mpz_srcptr z, mpir_si* ret)
{
	*ret = mpz_get_si(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_get_d(mpz_srcptr z, double* ret)
{
	*ret = mpz_get_d(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_get_d_2exp(signed long *exp2, mpz_srcptr src, double* ret)
{
	*ret = mpz_get_d_2exp(exp2, src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_get_str(int base, mpz_srcptr x, BSTR* ret) // Removed res_str parameter
{
	char* c_str = mpz_get_str(NULL, base, x); // Pass res_str = NULL to force internal allocation

	// Create a new BSTR from the C-string, then realloc the passed in BSTR and copy the string into it.
	BSTR vb_str = SysAllocStringByteLen(c_str, strlen(c_str));
	//SysReAllocString(&ret, vb_str);
	SysFreeString(*ret);
	*ret = vb_str;

	// Deallocate the memory allocated by mpz_get_str
	__gmp_default_free(c_str, 0);

	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_add(mpz_ptr w, mpz_srcptr u, mpz_srcptr v)
{
	mpz_add(w, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_add_ui(mpz_ptr w, mpz_srcptr u, mpir_ui vval)
{
	mpz_add_ui(w, u, vval);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_sub(mpz_ptr w, mpz_srcptr u, mpz_srcptr v)
{
	mpz_sub(w, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_sub_ui(mpz_ptr w, mpz_srcptr u, mpir_ui vval)
{
	mpz_sub_ui(w, u, vval);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_ui_sub(mpz_ptr w, mpir_ui uval, mpz_srcptr v)
{
	mpz_ui_sub(w, uval, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_mul(mpz_ptr w, mpz_srcptr u, mpz_srcptr v)
{
	mpz_mul(w, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_mul_si(mpz_ptr prod, mpz_srcptr mult, mpir_si small_mult)
{
	mpz_mul_si(prod, mult, small_mult);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_mul_ui(mpz_ptr prod, mpz_srcptr mult, mpir_ui small_mult)
{
	mpz_mul_ui(prod, mult, small_mult);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_addmul(mpz_ptr w, mpz_srcptr u, mpz_srcptr v)
{
	mpz_addmul(w, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_addmul_ui(mpz_ptr w, mpz_srcptr x, mpir_ui y)
{
	mpz_addmul_ui(w, x, y);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_submul(mpz_ptr w, mpz_srcptr u, mpz_srcptr v)
{
	mpz_submul(w, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_submul_ui(mpz_ptr w, mpz_srcptr x, mpir_ui y)
{
	mpz_submul_ui(w, x, y);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_mul_2exp(mpz_ptr w, mpz_srcptr u, mp_bitcnt_t cnt)
{
	mpz_mul_2exp(w, u, cnt);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_neg(mpz_ptr w, mpz_srcptr u)
{
	mpz_neg(w, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_abs(mpz_ptr w, mpz_srcptr u)
{
	mpz_abs(w, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cdiv_q(mpz_ptr quot, mpz_srcptr dividend, mpz_srcptr divisor)
{
	mpz_cdiv_q(quot, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cdiv_r(mpz_ptr rem, mpz_srcptr dividend, mpz_srcptr divisor)
{
	mpz_cdiv_r(rem, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cdiv_qr(mpz_ptr quot, mpz_ptr rem, mpz_srcptr dividend, mpz_srcptr divisor)
{
	mpz_cdiv_qr(quot, rem, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cdiv_q_ui(mpz_ptr quot, mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_cdiv_q_ui(quot, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cdiv_r_ui(mpz_ptr rem, mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_cdiv_r_ui(rem, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cdiv_qr_ui(mpz_ptr quot, mpz_ptr rem, mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_cdiv_qr_ui(quot, rem, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cdiv_ui(mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_cdiv_ui(dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cdiv_q_2exp(mpz_ptr w, mpz_srcptr u, mp_bitcnt_t cnt)
{
	mpz_cdiv_q_2exp(w, u, cnt);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cdiv_r_2exp(mpz_ptr w, mpz_srcptr u, mp_bitcnt_t cnt)
{
	mpz_cdiv_r_2exp(w, u, cnt);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fdiv_q(mpz_ptr quot, mpz_srcptr dividend, mpz_srcptr divisor)
{
	mpz_fdiv_q(quot, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fdiv_r(mpz_ptr rem, mpz_srcptr dividend, mpz_srcptr divisor)
{
	mpz_fdiv_r(rem, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fdiv_qr(mpz_ptr quot, mpz_ptr rem, mpz_srcptr dividend, mpz_srcptr divisor)
{
	mpz_fdiv_qr(quot, rem, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fdiv_q_ui(mpz_ptr quot, mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_fdiv_q_ui(quot, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fdiv_r_ui(mpz_ptr rem, mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_fdiv_r_ui(rem, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fdiv_qr_ui(mpz_ptr quot, mpz_ptr rem, mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_fdiv_qr_ui(quot, rem, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fdiv_ui(mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_fdiv_ui(dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fdiv_q_2exp(mpz_ptr w, mpz_srcptr u, mp_bitcnt_t cnt)
{
	mpz_fdiv_q_2exp(w, u, cnt);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fdiv_r_2exp(mpz_ptr w, mpz_srcptr u, mp_bitcnt_t cnt)
{
	mpz_fdiv_r_2exp(w, u, cnt);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_tdiv_q(mpz_ptr quot, mpz_srcptr num, mpz_srcptr den)
{
	mpz_tdiv_q(quot, num, den);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_tdiv_r(mpz_ptr rem, mpz_srcptr num, mpz_srcptr den)
{
	mpz_tdiv_r(rem, num, den);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_tdiv_qr(mpz_ptr quot, mpz_ptr rem, mpz_srcptr num, mpz_srcptr den)
{
	mpz_tdiv_qr(quot, rem, num, den);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_tdiv_q_ui(mpz_ptr quot, mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_tdiv_q_ui(quot, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_tdiv_r_ui(mpz_ptr rem, mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_tdiv_r_ui(rem, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_tdiv_qr_ui(mpz_ptr quot, mpz_ptr rem, mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_tdiv_qr_ui(quot, rem, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_tdiv_ui(mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_tdiv_ui(dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_tdiv_q_2exp(mpz_ptr w, mpz_srcptr u, mp_bitcnt_t cnt)
{
	mpz_tdiv_q_2exp(w, u, cnt);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_tdiv_r_2exp(mpz_ptr res, mpz_srcptr in, mp_bitcnt_t cnt)
{
	mpz_tdiv_r_2exp(res, in, cnt);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_mod(mpz_ptr rem, mpz_srcptr dividend, mpz_srcptr divisor)
{
	mpz_mod(rem, dividend, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_mod_ui(mpz_ptr rem, mpz_srcptr dividend, mpir_ui divisor, mpir_ui* ret)
{
	*ret = mpz_fdiv_r_ui(rem, dividend, divisor); /* same as fdiv_r because divisor unsigned */
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_divexact(mpz_ptr quot, mpz_srcptr num, mpz_srcptr den)
{
	mpz_divexact(quot, num, den);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_divexact_ui(mpz_ptr dst, mpz_srcptr src, mpir_ui divisor)
{
	mpz_divexact_ui(dst, src, divisor);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_divisible_p(mpz_srcptr a, mpz_srcptr d, int* ret)
{
	*ret = mpz_divisible_p(a, d);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_divisible_ui_p(mpz_srcptr a, mpir_ui d, int* ret)
{
	*ret = mpz_divisible_ui_p(a, d);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_divisible_2exp_p(mpz_srcptr a, mp_bitcnt_t d, int* ret)
{
	*ret = mpz_divisible_2exp_p(a, d);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_congruent_p(mpz_srcptr a, mpz_srcptr c, mpz_srcptr d, int* ret)
{
	*ret = mpz_congruent_p(a, c, d);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_congruent_ui_p(mpz_srcptr a, mpir_ui cu, mpir_ui du, int* ret)
{
	*ret = mpz_congruent_ui_p(a, cu, du);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_congruent_2exp_p(mpz_srcptr a, mpz_srcptr c, mp_bitcnt_t d, int* ret)
{
	*ret = mpz_congruent_2exp_p(a, c, d);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_powm(mpz_ptr r, mpz_srcptr b, mpz_srcptr e, mpz_srcptr m)
{
	mpz_powm(r, b, e, m);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_powm_ui(mpz_ptr r, mpz_srcptr b, mpir_ui el, mpz_srcptr m)
{
	mpz_powm_ui(r, b, el, m);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_pow_ui(mpz_ptr r, mpz_srcptr b, mpir_ui e)
{
	mpz_pow_ui(r, b, e);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_ui_pow_ui(mpz_ptr r, mpir_ui b, mpir_ui e)
{
	mpz_ui_pow_ui(r, b, e);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_root(mpz_ptr root, mpz_srcptr u, mpir_ui nth, int* ret)
{
	*ret = mpz_root(root, u, nth);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_nthroot(mpz_ptr root, mpz_srcptr u, mpir_ui nth)
{
	mpz_nthroot(root, u, nth);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_rootrem(mpz_ptr root, mpz_ptr rem, mpz_srcptr u, mpir_ui nth)
{
	mpz_rootrem(root, rem, u, nth);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_sqrt(mpz_ptr root, mpz_srcptr op)
{
	mpz_sqrt(root, op);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_sqrtrem(mpz_ptr root, mpz_ptr rem, mpz_srcptr op)
{
	mpz_sqrtrem(root, rem, op);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_perfect_power_p(mpz_srcptr u, int* ret)
{
	*ret = mpz_perfect_power_p(u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_perfect_square_p(mpz_srcptr a, int* ret)
{
	*ret = mpz_perfect_square_p(a);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_probable_prime_p(mpz_srcptr N, gmp_randstate_t STATE, int PROB, mpir_ui td, int* ret)
{
	*ret = mpz_probable_prime_p(N, STATE, PROB, td);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_likely_prime_p(mpz_srcptr N, gmp_randstate_t STATE, mpir_ui td, int* ret)
{
	*ret = mpz_likely_prime_p(N, STATE, td);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_next_prime_candidate(mpz_ptr p, mpz_srcptr n, gmp_randstate_t rnd)
{
	mpz_next_prime_candidate(p, n, rnd);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_gcd(mpz_ptr g, mpz_srcptr u, mpz_srcptr v)
{
	mpz_gcd(g, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_gcd_ui(mpz_ptr w, mpz_srcptr u, mpir_ui v, mpir_ui* ret)
{
	*ret = mpz_gcd_ui(w, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_gcdext(mpz_ptr g, mpz_ptr s, mpz_ptr t, mpz_srcptr a, mpz_srcptr b)
{
	mpz_gcdext(g, s, t, a, b);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_lcm(mpz_ptr r, mpz_srcptr u, mpz_srcptr v)
{
	mpz_lcm(r, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_lcm_ui(mpz_ptr r, mpz_srcptr u, mpir_ui v)
{
	mpz_lcm_ui(r, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_invert(mpz_ptr inverse, mpz_srcptr x, mpz_srcptr n, int* ret)
{
	*ret = mpz_invert(inverse, x, n);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_jacobi(mpz_srcptr a, mpz_srcptr b, int* ret)
{
	*ret = mpz_jacobi(a, b);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_legendre(mpz_srcptr a, mpz_srcptr p, int* ret)
{
	*ret = mpz_jacobi(a, p); /* alias */
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_kronecker(mpz_srcptr a, mpz_srcptr b, int* ret)
{
	*ret = mpz_jacobi(a, b); /* alias */
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_kronecker_si(mpz_srcptr a, mpir_si b, int* ret)
{
	*ret = mpz_kronecker_si(a, b);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_kronecker_ui(mpz_srcptr a, mpir_ui b, int* ret)
{
	*ret = mpz_kronecker_ui(a, b);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_si_kronecker(mpir_si a, mpz_srcptr b, int* ret)
{
	*ret = mpz_si_kronecker(a, b);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_ui_kronecker(mpir_ui a, mpz_srcptr b, int* ret)
{
	*ret = mpz_ui_kronecker(a, b);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_remove(mpz_ptr dest, mpz_srcptr src, mpz_srcptr f, mp_bitcnt_t* ret)
{
	*ret = mpz_remove(dest, src, f);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fac_ui(mpz_ptr x, mpir_ui n)
{
	mpz_fac_ui(x, n);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_2fac_ui(mpz_ptr x, mpir_ui n)
{
	mpz_2fac_ui(x, n);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_mfac_uiui(mpz_ptr x, mpir_ui n, mpir_ui m)
{
	mpz_mfac_uiui(x, n, m);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_primorial_ui(mpz_ptr x, mpir_ui n)
{
	mpz_primorial_ui(x, n);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_bin_ui(mpz_ptr r, mpz_srcptr n, mpir_ui k)
{
	mpz_bin_ui(r, n, k);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_bin_uiui(mpz_ptr r, mpir_ui n, mpir_ui k)
{
	mpz_bin_uiui(r, n, k);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fib_ui(mpz_ptr fn, mpir_ui n)
{
	mpz_fib_ui(fn, n);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fib2_ui(mpz_ptr fn, mpz_ptr fnsub1, mpir_ui n)
{
	mpz_fib2_ui(fn, fnsub1, n);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_lucnum_ui(mpz_ptr ln, mpir_ui n)
{
	mpz_lucnum_ui(ln, n);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_lucnum2_ui(mpz_ptr ln, mpz_ptr lnsub1, mpir_ui n)
{
	mpz_lucnum2_ui(ln, lnsub1, n);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cmp(mpz_srcptr u, mpz_srcptr v, int* ret)
{
	*ret = mpz_cmp(u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cmp_d(mpz_srcptr z, double d, int* ret)
{
	*ret = mpz_cmp_d(z, d);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cmp_si(mpz_srcptr u, mpir_si v_digit, int* ret)
{
	*ret = _mpz_cmp_si(u, v_digit);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cmp_ui(mpz_srcptr u, mpir_ui v_digit, int* ret)
{
	*ret = _mpz_cmp_ui(u, v_digit);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cmpabs(mpz_srcptr u, mpz_srcptr v, int* ret)
{
	*ret = mpz_cmpabs(u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cmpabs_d(mpz_srcptr z, double d, int* ret)
{
	*ret = mpz_cmpabs_d(z, d);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_cmpabs_ui(mpz_srcptr u, mpir_ui v_digit, int* ret)
{
	*ret = mpz_cmpabs_ui(u, v_digit);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_sgn(mpz_ptr z, int* ret)
{
	*ret = mpz_sgn(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_and(mpz_ptr res, mpz_srcptr op1, mpz_srcptr op2)
{
	mpz_and(res, op1, op2);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_ior(mpz_ptr res, mpz_srcptr op1, mpz_srcptr op2)
{
	mpz_ior(res, op1, op2);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_xor(mpz_ptr res, mpz_srcptr op1, mpz_srcptr op2)
{
	mpz_xor(res, op1, op2);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_com(mpz_ptr dst, mpz_srcptr src)
{
	mpz_com(dst, src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_popcount(mpz_srcptr u, mp_bitcnt_t* ret)
{
	*ret = mpz_popcount(u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_hamdist(mpz_srcptr u, mpz_srcptr v, mp_bitcnt_t* ret)
{
	*ret = mpz_hamdist(u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_scan0(mpz_srcptr u, mp_bitcnt_t starting_bit, mp_bitcnt_t* ret)
{
	*ret = mpz_scan0(u, starting_bit);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_scan1(mpz_srcptr u, mp_bitcnt_t starting_bit, mp_bitcnt_t* ret)
{
	*ret = mpz_scan1(u, starting_bit);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_setbit(mpz_ptr d, mp_bitcnt_t bit_index)
{
	mpz_setbit(d, bit_index);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_clrbit(mpz_ptr d, mp_bitcnt_t bit_index)
{
	mpz_clrbit(d, bit_index);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_combit(mpz_ptr d, mp_bitcnt_t bit_index)
{
	mpz_combit(d, bit_index);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_tstbit(mpz_srcptr u, mp_bitcnt_t bit_index, int* ret)
{
	*ret = mpz_tstbit(u, bit_index);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_urandomb(mpz_ptr rop, gmp_randstate_t rstate, mp_bitcnt_t nbits)
{
	mpz_urandomb(rop, rstate, nbits);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_urandomm(mpz_ptr rop, gmp_randstate_t rstate, mpz_srcptr n)
{
	mpz_urandomm(rop, rstate, n);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_rrandomb(mpz_ptr x, gmp_randstate_t rstate, mp_bitcnt_t nbits)
{
	mpz_rrandomb(x, rstate, nbits);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fits_ulong_p(mpz_srcptr z, int* ret)
{
	*ret = mpz_fits_ulong_p(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fits_slong_p(mpz_srcptr z, int* ret)
{
	*ret = mpz_fits_slong_p(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fits_uint_p(mpz_srcptr z, int* ret)
{
	*ret = mpz_fits_uint_p(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fits_sint_p(mpz_srcptr z, int* ret)
{
	*ret = mpz_fits_sint_p(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fits_ushort_p(mpz_srcptr z, int* ret)
{
	*ret = mpz_fits_ushort_p(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fits_sshort_p(mpz_srcptr z, int* ret)
{
	*ret = mpz_fits_sshort_p(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fits_si_p(mpz_srcptr z, int* ret) // not doc'ed
{
	*ret = mpz_fits_si_p(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_fits_ui_p(mpz_srcptr z, int* ret) // not doc'ed
{
	*ret = mpz_fits_ui_p(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_odd_p(mpz_ptr z, int* ret)
{
	*ret = mpz_odd_p(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_even_p(mpz_ptr z, int* ret)
{
	*ret = mpz_even_p(z);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpz_sizeinbase(mpz_srcptr x, int base, size_t* ret)
{
	*ret = mpz_sizeinbase(x, base);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_canonicalize(mpq_ptr op)
{
	mpq_canonicalize(op);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_init(mpq_ptr x)
{
	mpq_init(x);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_clear(mpq_ptr m)
{
	mpq_clear(m);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_set(mpq_ptr dest, mpq_srcptr src)
{
	mpq_set(dest, src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_set_z(mpq_ptr dest, mpz_srcptr src)
{
	mpq_set_z(dest, src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_set_ui(mpq_ptr dest, mpir_ui num, mpir_ui den)
{
	mpq_set_ui(dest, num, den);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_set_si(mpq_ptr dest, mpir_si num, mpir_ui den)
{
	mpq_set_si(dest, num, den);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_set_str(mpq_ptr q, const char *str, int base, int* ret) // int* ret
{
	*ret = mpq_set_str(q, str, base);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_swap(mpq_ptr u, mpq_ptr v)
{
	mpq_swap(u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_get_d(mpq_srcptr src, double* ret) // double* ret
{
	*ret = mpq_get_d(src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_set_d(mpq_ptr dest, double d)
{
	mpq_set_d(dest, d);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_set_f(mpq_ptr q, mpf_srcptr f)
{
	mpq_set_f(q, f);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_get_str(int base, mpq_srcptr q, BSTR* ret) // Removed str parameter
{
	char* c_str = mpq_get_str(NULL, base, q); // Pass str = NULL to force internal allocation

	// Create a new BSTR from the C-string, then realloc the passed in BSTR and copy the string into it.
	BSTR vb_str = SysAllocStringByteLen(c_str, strlen(c_str));
	//SysReAllocString(&ret, vb_str);
	SysFreeString(*ret);
	*ret = vb_str;

	// Deallocate the memory allocated by mpq_get_str
	__gmp_default_free(c_str, 0);

	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_add(mpq_ptr rop, mpq_srcptr op1, mpq_srcptr op2)
{
	mpq_add(rop, op1, op2);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_sub(mpq_ptr rop, mpq_srcptr op1, mpq_srcptr op2)
{
	mpq_sub(rop, op1, op2);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_mul(mpq_ptr prod, mpq_srcptr op1, mpq_srcptr op2)
{
	mpq_mul(prod, op1, op2);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_mul_2exp(mpq_ptr dst, mpq_srcptr src, mp_bitcnt_t n)
{
	mpq_mul_2exp(dst, src, n);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_div(mpq_ptr quot, mpq_srcptr op1, mpq_srcptr op2)
{
	mpq_div(quot, op1, op2);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_div_2exp(mpq_ptr dst, mpq_srcptr src, mp_bitcnt_t n)
{
	mpq_div_2exp(dst, src, n);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_neg(mpq_ptr dst, mpq_srcptr src)
{
	mpq_neg(dst, src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_abs(mpq_ptr dst, mpq_srcptr src)
{
	mpq_abs(dst, src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_inv(mpq_ptr dest, mpq_srcptr src)
{
	mpq_inv(dest, src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_cmp(mpq_srcptr op1, mpq_srcptr op2, int* ret) // int* ret
{
	*ret = mpq_cmp(op1, op2);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_cmp_z(mpq_srcptr op1, mpz_srcptr op2, int* ret) // int* ret
{
	*ret = mpq_cmp_z(op1, op2);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_cmp_ui(mpq_srcptr op1, mpir_ui num2, mpir_ui den2, int* ret) // int* ret
{
	*ret = _mpq_cmp_ui(op1, num2, den2);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_cmp_si(mpq_srcptr q, mpir_si n, mpir_ui d, int* ret) // int* ret
{
	*ret = _mpq_cmp_si(q, n, d);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_sgn(mpq_ptr q, int* ret) // macro
{
	*ret = mpq_sgn(q);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_equal(mpq_srcptr op1, mpq_srcptr op2, int* ret) // int* ret
{
	*ret = mpq_equal(op1, op2);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_numref(mpq_ptr src, mpz_ptr* num) // macro
{
	*num = mpq_numref(src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_denref(mpq_ptr src, mpz_ptr* den) // macro
{
	*den = mpq_denref(src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_get_num(mpz_ptr num, mpq_srcptr src)
{
	mpq_get_num(num, src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_get_den(mpz_ptr den, mpq_srcptr src)
{
	mpq_get_den(den, src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_set_den(mpq_ptr dest, mpz_srcptr den)
{
	mpq_set_den(dest, den);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpq_set_num(mpq_ptr dest, mpz_srcptr num)
{
	mpq_set_num(dest, num);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_set_default_prec(mp_bitcnt_t prec_in_bits)
{
	mpf_set_default_prec(prec_in_bits);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_get_default_prec(mp_bitcnt_t* ret) // mp_bitcnt_t* ret
{
	*ret = mpf_get_default_prec();
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_init(mpf_ptr r)
{
	mpf_init(r);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_init2(mpf_ptr r, mp_bitcnt_t prec_in_bits)
{
	mpf_init2(r, prec_in_bits);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_clear(mpf_ptr m)
{
	mpf_clear(m);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_get_prec(mpf_srcptr x, mp_bitcnt_t* ret) // mp_bitcnt_t* ret
{
	*ret = mpf_get_prec(x);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_set_prec(mpf_ptr x, mp_bitcnt_t new_prec_in_bits)
{
	mpf_set_prec(x, new_prec_in_bits);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_set_prec_raw(mpf_ptr x, mp_bitcnt_t prec_in_bits)
{
	mpf_set_prec_raw(x, prec_in_bits);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_set(mpf_ptr r, mpf_srcptr u)
{
	mpf_set(r, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_set_ui(mpf_ptr f, mpir_ui val)
{
	mpf_set_ui(f, val);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_set_si(mpf_ptr dest, mpir_si val)
{
	mpf_set_si(dest, val);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_set_d(mpf_ptr r, double d)
{
	mpf_set_d(r, d);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_set_z(mpf_ptr r, mpz_srcptr u)
{
	mpf_set_z(r, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_set_q(mpf_t r, mpq_srcptr q)
{
	mpf_set_q(r, q);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_set_str(mpf_ptr x, const char *str, int base, int* ret) // int* ret
{
	*ret = mpf_set_str(x, str, base);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_swap(mpf_ptr u, mpf_ptr v)
{
	mpf_swap(u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_init_set(mpf_ptr r, mpf_srcptr s)
{
	mpf_init_set(r, s);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_init_set_ui(mpf_ptr r, mpir_ui val)
{
	mpf_init_set_ui(r, val);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_init_set_si(mpf_ptr r, mpir_si val)
{
	mpf_init_set_si(r, val);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_init_set_d(mpf_ptr r, double val)
{
	mpf_init_set_d(r, val);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_init_set_str(mpf_ptr r, const char *s, int base, int* ret) // int* ret
{
	*ret = mpf_init_set_str(r, s, base);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_get_d(mpf_srcptr src, double* ret) // double* ret
{
	*ret = mpf_get_d(src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_get_d_2exp(signed long *exp2, mpf_srcptr src, double* ret) // double* ret
{
	*ret = mpf_get_d_2exp(exp2, src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_get_si(mpf_srcptr f, mpir_si* ret) // mpir_si* ret
{
	*ret = mpf_get_si(f);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_get_ui(mpf_srcptr f, mpir_ui* ret) // mpir_ui* ret
{
	*ret = mpf_get_ui(f);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_get_str(int base, size_t n_digits, mpf_srcptr u, BSTR* ret) // char* ret
{
	mp_exp_t exp;
	char* c_str = mpf_get_str(NULL, &exp, base, n_digits, u);

	// If value is zero, an empty string is returned and the exponent returned is 0.
	if (strlen(c_str) == 0 && exp == 0)
	{
		BSTR vb_str = SysAllocStringByteLen("0", strlen("0"));
		//SysReAllocString(&ret, vb_str);
		SysFreeString(*ret);
		*ret = vb_str;
	}
	else
	{
		// Number of significant digits in the returned string
		int sig_digits = strlen(c_str);
		if (c_str[0] == '-')
			sig_digits = sig_digits - 1;

		int dst_size;
		dst_size = 0;
		if (c_str[0] == '-')
			dst_size += 1;
		// Larger of the number of significant digits returned, and the exponent value returned
		// If exponent is larger than number of digits returned, will have to add zeros.
		if (exp > sig_digits)
			dst_size += exp;
		else
			dst_size += sig_digits;
		if (exp < sig_digits)
			dst_size += 1; // radix point
		if (exp == 0)
			dst_size += 1; // prepended zero
		dst_size += 1; // null-terminator

		// Buffer to build/format output string
		char* dst = (char*) calloc(dst_size, sizeof(char));
		if (dst == NULL)
			return E_OUTOFMEMORY;

		char* dst_p = dst; // pointer to current position in output buffer
		char* src_p = c_str; // pointer to current position in input string

		// Add minus sign to output buffer
		if (c_str[0] == '-')
		{
			*dst_p = '-';
			dst_p++;

			*src_p++;
		}

		// Prepend zero if radix point to left of all digits
		if (exp == 0)
		{
			*dst_p = '0';
			dst_p++;
		}

		// Copy digits left of the radix point to the outut
		int n = exp;
		if (n > sig_digits)
			n = sig_digits;
		if (n > 0)
		{
			strncpy(dst_p, src_p, n);
			dst_p += n;
			src_p += n;
		}
		// If there aren't enough significant digits retured, pad with zeros
		if (exp > sig_digits)
		{
			memset(dst_p, '0', exp - sig_digits);
			dst_p += exp - sig_digits;
		}

		// Add a radix point and the remaining digits
		if (sig_digits > exp)
		{
			*dst_p = '.';
			dst_p++;

			strncpy(dst_p, src_p, sig_digits - exp);
			dst_p += sig_digits - exp;
		}

		*dst_p = '\0';
			
		// Create a new BSTR from the C-string, then realloc the passed in BSTR and copy the string into it.
		BSTR vb_str = SysAllocStringByteLen(dst, strlen(dst));
		//SysReAllocString(&ret, vb_str);
		SysFreeString(*ret);
		*ret = vb_str;

		free(dst);
		
		// Deallocate the memory allocated by mpq_get_str
		__gmp_default_free(c_str, 0);
	}

	//ret = mpf_get_str(dbuf, exp, base, n_digits, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_add(mpf_ptr r, mpf_srcptr u, mpf_srcptr v)
{
	mpf_add(r, u,v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_add_ui(mpf_ptr sum, mpf_srcptr u, mpir_ui v)
{
	mpf_add_ui(sum, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_sub(mpf_ptr r, mpf_srcptr u, mpf_srcptr v)
{
	mpf_sub(r, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_ui_sub(mpf_ptr r, mpir_ui u, mpf_srcptr v)
{
	mpf_ui_sub(r, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_sub_ui(mpf_ptr sum, mpf_srcptr u, mpir_ui v)
{
	mpf_sub_ui(sum, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_mul(mpf_ptr r, mpf_srcptr u, mpf_srcptr v)
{
	mpf_mul(r, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_mul_ui(mpf_ptr r, mpf_srcptr u, mpir_ui v)
{
	mpf_mul_ui(r, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_div(mpf_ptr r, mpf_srcptr u, mpf_srcptr v)
{
	mpf_div(r, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_ui_div(mpf_ptr r, mpir_ui u, mpf_srcptr v)
{
	mpf_ui_div(r, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_div_ui(mpf_ptr r, mpf_srcptr u, mpir_ui v)
{
	mpf_div_ui(r, u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_sqrt(mpf_ptr r, mpf_srcptr u)
{
	mpf_sqrt(r, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_sqrt_ui(mpf_ptr r, mpir_ui u)
{
	mpf_sqrt_ui(r, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_pow_ui(mpf_ptr r, mpf_srcptr b, mpir_ui e)
{
	mpf_pow_ui(r, b, e);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_neg(mpf_ptr r, mpf_srcptr u)
{
	mpf_neg(r, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_abs(mpf_ptr r, mpf_srcptr u)
{
	mpf_abs(r, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_mul_2exp(mpf_ptr r, mpf_srcptr u, mp_bitcnt_t exp)
{
	mpf_mul_2exp(r, u, exp);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_div_2exp(mpf_ptr r, mpf_srcptr u, mp_bitcnt_t exp)
{
	mpf_div_2exp(r, u, exp);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_cmp(mpf_srcptr u, mpf_srcptr v, int* ret) // int* ret
{
	*ret = mpf_cmp(u, v);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_cmp_d(mpf_srcptr f, double d, int* ret) // int* ret
{
	*ret = mpf_cmp_d(f, d);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_cmp_ui(mpf_srcptr u, mpir_ui vval, int* ret) // int* ret
{
	*ret = mpf_cmp_ui(u, vval);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_cmp_si(mpf_srcptr u, mpir_si vval, int* ret) // int* ret
{
	*ret = mpf_cmp_si(u, vval);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_eq(mpf_srcptr u, mpf_srcptr v, mp_bitcnt_t n_bits, int* ret) // int* ret
{
	*ret = mpf_eq(u, v, n_bits);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_reldiff(mpf_ptr rdiff, mpf_srcptr x, mpf_srcptr y)
{
	mpf_reldiff(rdiff, x, y);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_sgn(mpf_ptr f, int* ret) // macro
{
	*ret = mpf_sgn(f);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_ceil(mpf_ptr r, mpf_srcptr u)
{
	mpf_ceil(r, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_floor(mpf_ptr r, mpf_srcptr u)
{
	mpf_floor(r, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_trunc(mpf_ptr r, mpf_srcptr u)
{
	mpf_trunc(r, u);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_integer_p(mpf_srcptr f, int* ret) // int* ret
{
	*ret= mpf_integer_p(f);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_fits_ulong_p(mpf_srcptr f, int* ret) // int* ret
{
	*ret = mpf_fits_ulong_p(f);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_fits_slong_p(mpf_srcptr f, int* ret) // int* ret
{
	*ret = mpf_fits_slong_p(f);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_fits_uint_p(mpf_srcptr f, int* ret) // int* ret
{
	*ret = mpf_fits_uint_p(f);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_fits_sint_p(mpf_srcptr f, int* ret) // int* ret
{
	*ret = mpf_fits_sint_p(f);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_fits_ushort_p(mpf_srcptr f, int* ret) // int* ret
{
	*ret = mpf_fits_ushort_p(f);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_fits_sshort_p(mpf_srcptr f, int* ret) // int* ret
{
	*ret = mpf_fits_sshort_p(f);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_urandomb(mpf_t rop, gmp_randstate_t rstate, mp_bitcnt_t nbits)
{
	mpf_urandomb(rop, rstate, nbits);
	return S_OK;
}
TL_DSP_LIB_DEF tl_mpf_rrandomb(mpf_ptr x, gmp_randstate_t rnd, mp_size_t xs, mp_exp_t exp)
{
	mpf_rrandomb(x, rnd, xs, exp);
	return S_OK;
}
TL_DSP_LIB_DEF tl_gmp_randinit_default(gmp_randstate_t rstate)
{
	gmp_randinit_default(rstate);
	return S_OK;
}
TL_DSP_LIB_DEF tl_gmp_randinit_mt(gmp_randstate_t rstate)
{
	gmp_randinit_mt(rstate);
	return S_OK;
}
TL_DSP_LIB_DEF tl_gmp_randinit_lc_2exp(gmp_randstate_t rstate, mpz_srcptr a, mpir_ui c, mp_bitcnt_t m2exp)
{
	gmp_randinit_lc_2exp(rstate, a, c, m2exp);
	return S_OK;
}
TL_DSP_LIB_DEF tl_gmp_randinit_lc_2exp_size(gmp_randstate_t rstate, mp_bitcnt_t size, int* ret)
{
	*ret = gmp_randinit_lc_2exp_size(rstate, size);
	return S_OK;
}
TL_DSP_LIB_DEF tl_gmp_randinit_set(gmp_randstate_ptr dst, gmp_randstate_srcptr src)
{
	gmp_randinit_set(dst, src);
	return S_OK;
}
TL_DSP_LIB_DEF tl_gmp_randclear(gmp_randstate_t rstate)
{
	gmp_randclear(rstate);
	return S_OK;
}
TL_DSP_LIB_DEF tl_gmp_randseed(gmp_randstate_t rstate, mpz_srcptr seed)
{
	gmp_randseed(rstate, seed);
	return S_OK;
}
TL_DSP_LIB_DEF tl_gmp_randseed_ui(gmp_randstate_t rstate, mpir_ui seed)
{
	gmp_randseed_ui(rstate, seed);
	return S_OK;
}
TL_DSP_LIB_DEF tl_gmp_urandomb_ui(gmp_randstate_ptr rstate, mpir_ui bits, mpir_ui* ret)
{
	*ret = gmp_urandomb_ui(rstate, bits);
	return S_OK;
}
TL_DSP_LIB_DEF tl_gmp_urandomm_ui(gmp_randstate_ptr rstate, mpir_ui n, mpir_ui* ret)
{
	*ret = gmp_urandomm_ui(rstate, n);
	return S_OK;
}
TL_DSP_LIB_DEF tl_gmp_version(int* version, int* version_minor, int* version_patch_level, BSTR* ret)
{
	if (version != NULL)
		*version = __GNU_MP_VERSION;
	if (version_minor != NULL)
		*version_minor = __GNU_MP_VERSION_MINOR;
	if (version_patch_level != NULL)
		*version_patch_level = __GNU_MP_VERSION_PATCHLEVEL;

	BSTR s = SysAllocStringByteLen(gmp_version, strlen(gmp_version));
	SysFreeString(*ret);
	*ret = s;

	return S_OK;
}
TL_DSP_LIB_DEF tl_mpir_version(int* version, int* version_minor, int* version_patch_level, BSTR* ret)
{
	if (version != NULL)
		*version = __MPIR_VERSION;
	if (version_minor != NULL)
		*version_minor = __MPIR_VERSION_MINOR;
	if (version_patch_level != NULL)
		*version_patch_level = __MPIR_VERSION_PATCHLEVEL;

	BSTR s = SysAllocStringByteLen(mpir_version, strlen(mpir_version));
	SysFreeString(*ret);
	*ret = s;
	//int len = SysStringLen(s);
	//int len = strlen(mpir_version);
	//SysReAllocStringLen(ret, s, len);
	return S_OK;

#if 0
	if (str_buffer == NULL || str_buffer_len == 0)
	{
		*str_buffer_len = strlen(mpir_version);
	}
	else
	{
		int len = strlen(mpir_version);
		if (*str_buffer_len < len)
			len = *str_buffer_len;

		strncpy((LPSTR)str_buffer, mpir_version, len);

		SysReAllocStringLen(&str_buffer, str_buffer, len);

		*str_buffer_len = len;
	}

	return S_OK;
#endif

#if 0
	// Create a new BSTR from the C-string, then free the BSTR referenced by ret and reset ret to point to the new BSTR.
	BSTR s = SysAllocStringByteLen(mpir_version, strlen(mpir_version));
	SysFreeString(ret);
	ret = s;
	//SysReAllocString(&ret, (BSTR)"1.2.3");

	return S_OK;
#endif

#if 0
	// This doesn't work - BSTR s has length set to 5. But when SysReAllocString() called, BSTR ret has length set to 6 and VB sees the null-terminator 

	// Create a new BSTR from the C-string, then realloc the passed in BSTR and copy the string into it.
	int len = strlen(mpir_version);
	BSTR s = SysAllocStringByteLen(mpir_version, len);
	SysReAllocString(ret, s);
	SysFreeString(s);
#endif

#if 0
	// This doesn't work - SysReallocString allocated len wchars, i.e. BSTR ret length set to 10 and VB sees extra chars

	// Create a new BSTR from the C-string, then realloc the passed in BSTR and copy the string into it.
	int len = strlen(mpir_version);
	BSTR s = SysAllocStringByteLen(mpir_version, len);
	SysReAllocString(ret, s);
	SysFreeString(s);
#endif

#if 0
	// Convert ANSI string to BSTR containing UNICODE.
	// First call to MultiByteToWideChar does not pass valid buffer, so it just returned required size;
	// The C-string length is passed so that null-terminator not included (if pass -1, null-terminator is included).
	// Then empty BSTR allocated of required size.
	// Then MultiByteToWideChar called again to convert ANSI string into BSTR with conversion.
	int ret1 = MultiByteToWideChar(CP_ACP, 0, mpir_version, strlen(mpir_version), 0, 0);
	if (ret1 == 0)
	{
		return E_INVALIDARG;
	}
	else
	{
		int len = ret1;
		BSTR s = SysAllocStringLen(0, len);
		int ret2 = MultiByteToWideChar(CP_ACP, 0, mpir_version, strlen(mpir_version), s, len);
		if (ret2 == 0)
		{
			return E_OUTOFMEMORY;
		}
		else
		{
			//SysReAllocString(ret, s);
			//SysFreeString(s);

			SysFreeString(*ret);
			*ret = s;
		}
	}

	return S_OK;
#endif
}

#if 0
const char* gmp_version_func(int* version, int* version_minor, int* version_patch_level)
{
	if (version != NULL)
		*version = __GNU_MP_VERSION;
	if (version_minor != NULL)
		*version_minor = __GNU_MP_VERSION_MINOR;
	if (version_patch_level != NULL)
		*version_patch_level = __GNU_MP_VERSION_PATCHLEVEL;

	return gmp_version;
}

const char* mpir_version_func(int* version, int* version_minor, int* version_patch_level)
{
	if (version != NULL)
		*version = __MPIR_VERSION;
	if (version_minor != NULL)
		*version_minor = __MPIR_VERSION_MINOR;
	if (version_patch_level != NULL)
		*version_patch_level = __MPIR_VERSION_PATCHLEVEL;

	return mpir_version;
}
#endif

