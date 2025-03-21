/*!
 * mpi.h - multi-precision integers for mako
 * Copyright (c) 2020, Christopher Jeffrey (MIT License).
 * https://github.com/bcoin-org/mako
 *
 * A from-scratch reimplementation of GMP.
 */

#ifndef BTC_MPI_H
#define BTC_MPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "common.h"

/*
 * Symbol Aliases
 */

#define mp_bits_per_limb btc__mp_bits_per_limb
#define mpn_zero btc__mpn_zero
#define mpn_cleanse btc__mpn_cleanse
#define mpn_set_1 btc__mpn_set_1
#define mpn_copyi btc__mpn_copyi
#define mpn_copyd btc__mpn_copyd
#define mpn_zero_p btc__mpn_zero_p
#define mpn_cmp btc__mpn_cmp
#define mpn_add_1 btc__mpn_add_1
#define mpn_add_n btc__mpn_add_n
#define mpn_add btc__mpn_add
#define mpn_sec_add_1 btc__mpn_sec_add_1
#define mpn_sec_add btc__mpn_sec_add
#define mpn_sub_1 btc__mpn_sub_1
#define mpn_sub_n btc__mpn_sub_n
#define mpn_sub btc__mpn_sub
#define mpn_sec_sub_1 btc__mpn_sec_sub_1
#define mpn_sec_sub btc__mpn_sec_sub
#define mpn_mul_1 btc__mpn_mul_1
#define mpn_addmul_1 btc__mpn_addmul_1
#define mpn_submul_1 btc__mpn_submul_1
#define mpn_mul_n btc__mpn_mul_n
#define mpn_mul btc__mpn_mul
#define mpn_sqr btc__mpn_sqr
#define mpn_mulshift btc__mpn_mulshift
#define mpn_divmod_1 btc__mpn_divmod_1
#define mpn_div_1 btc__mpn_div_1
#define mpn_mod_1 btc__mpn_mod_1
#define mpn_divmod btc__mpn_divmod
#define mpn_div btc__mpn_div
#define mpn_mod btc__mpn_mod
#define mpn_divexact_1 btc__mpn_divexact_1
#define mpn_divexact btc__mpn_divexact
#define mpn_sqrtrem btc__mpn_sqrtrem
#define mpn_perfect_square_p btc__mpn_perfect_square_p
#define mpn_and_n btc__mpn_and_n
#define mpn_ior_n btc__mpn_ior_n
#define mpn_xor_n btc__mpn_xor_n
#define mpn_andn_n btc__mpn_andn_n
#define mpn_iorn_n btc__mpn_iorn_n
#define mpn_nand_n btc__mpn_nand_n
#define mpn_nior_n btc__mpn_nior_n
#define mpn_xnor_n btc__mpn_xnor_n
#define mpn_com btc__mpn_com
#define mpn_lshift btc__mpn_lshift
#define mpn_rshift btc__mpn_rshift
#define mpn_getbit btc__mpn_getbit
#define mpn_getbits btc__mpn_getbits
#define mpn_tstbit btc__mpn_tstbit
#define mpn_setbit btc__mpn_setbit
#define mpn_clrbit btc__mpn_clrbit
#define mpn_combit btc__mpn_combit
#define mpn_scan0 btc__mpn_scan0
#define mpn_scan1 btc__mpn_scan1
#define mpn_popcount btc__mpn_popcount
#define mpn_hamdist btc__mpn_hamdist
#define mpn_mask btc__mask
#define mpn_neg btc__mpn_neg
#define mpn_reduce_weak btc__mpn_reduce_weak
#define mpn_barrett btc__mpn_barrett
#define mpn_reduce btc__mpn_reduce
#define mpn_mont btc__mpn_mont
#define mpn_montmul btc__mpn_montmul
#define mpn_sec_montmul btc__mpn_sec_montmul
#define mpn_gcd btc__mpn_gcd
#define mpn_gcd_1 btc__mpn_gcd_1
#define mpn_gcdext btc__mpn_gcdext
#define mpn_invert btc__mpn_invert
#define mpn_invert_n btc__mpn_invert_n
#define mpn_sec_invert btc__mpn_sec_invert
#define mpn_sec_invert_n btc__mpn_sec_invert_n
#define mpn_jacobi btc__mpn_jacobi
#define mpn_jacobi_n btc__mpn_jacobi_n
#define mpn_powm btc__mpn_powm
#define mpn_sec_powm btc__mpn_sec_powm
#define mpn_ctz btc__mpn_ctz
#define mpn_bitlen btc__mpn_bitlen
#define mpn_bytelen btc__mpn_bytelen
#define mpn_sizeinbase btc__mpn_sizeinbase
#define mpn_cnd_zero btc__mpn_cnd_zero
#define mpn_cnd_select btc__mpn_cnd_select
#define mpn_cnd_swap btc__mpn_cnd_swap
#define mpn_cnd_add_n btc__mpn_cnd_add_n
#define mpn_cnd_sub_n btc__mpn_cnd_sub_n
#define mpn_cnd_neg btc__mpn_cnd_neg
#define mpn_sec_tabselect btc__mpn_sec_tabselect
#define mpn_sec_zero_p btc__mpn_sec_zero_p
#define mpn_sec_equal_p btc__mpn_sec_equal_p
#define mpn_sec_lt_p btc__mpn_sec_lt_p
#define mpn_sec_lte_p btc__mpn_sec_lte_p
#define mpn_sec_gt_p btc__mpn_sec_gt_p
#define mpn_sec_gte_p btc__mpn_sec_gte_p
#define mpn_sec_cmp btc__mpn_sec_cmp
#define mpn_import btc__mpn_import
#define mpn_export btc__mpn_export
#define mpn_set_str btc__mpn_set_str
#define mpn_get_str btc__mpn_get_str
#define mpn_print btc__mpn_print
#define mpn_random btc__mpn_random
#define mpn_randomm btc__mpn_randomm
#define mpz_init btc__mpz_init
#define mpz_init2 btc__mpz_init2
#define mpz_inits btc__mpz_inits
#define mpz_init_set btc__mpz_init_set
#define mpz_init_set_ui btc__mpz_init_set_ui
#define mpz_init_set_si btc__mpz_init_set_si
#define mpz_init_set_compact btc__mpz_init_set_compact
#define mpz_init_set_str btc__mpz_init_set_str
#define mpz_clear btc__mpz_clear
#define mpz_clears btc__mpz_clears
#define mpz_cleanse btc__mpz_cleanse
#define mpz_cleanses btc__mpz_cleanses
#define mpz_set btc__mpz_set
#define mpz_roset btc__mpz_roset
#define mpz_roinit_n btc__mpz_roinit_n
#define mpz_set_ui btc__mpz_set_ui
#define mpz_set_si btc__mpz_set_si
#define mpz_set_compact btc__mpz_set_compact
#define mpz_get_ui btc__mpz_get_ui
#define mpz_get_si btc__mpz_get_si
#define mpz_get_compact btc__mpz_get_compact
#define mpz_sgn btc__mpz_sgn
#define mpz_cmp btc__mpz_cmp
#define mpz_cmp_ui btc__mpz_cmp_ui
#define mpz_cmp_si btc__mpz_cmp_si
#define mpz_cmpabs btc__mpz_cmpabs
#define mpz_cmpabs_ui btc__mpz_cmpabs_ui
#define mpz_cmpabs_si btc__mpz_cmpabs_si
#define mpz_add btc__mpz_add
#define mpz_add_ui btc__mpz_add_ui
#define mpz_add_si btc__mpz_add_si
#define mpz_sub btc__mpz_sub
#define mpz_sub_ui btc__mpz_sub_ui
#define mpz_sub_si btc__mpz_sub_si
#define mpz_ui_sub btc__mpz_ui_sub
#define mpz_si_sub btc__mpz_si_sub
#define mpz_mul btc__mpz_mul
#define mpz_mul_ui btc__mpz_mul_ui
#define mpz_mul_si btc__mpz_mul_si
#define mpz_sqr btc__mpz_sqr
#define mpz_addmul btc__mpz_addmul
#define mpz_addmul_ui btc__mpz_addmul_ui
#define mpz_addmul_si btc__mpz_addmul_si
#define mpz_submul btc__mpz_submul
#define mpz_submul_ui btc__mpz_submul_ui
#define mpz_submul_si btc__mpz_submul_si
#define mpz_mulshift btc__mpz_mulshift
#define mpz_quorem btc__mpz_quorem
#define mpz_quo btc__mpz_quo
#define mpz_rem btc__mpz_rem
#define mpz_quo_ui btc__mpz_quo_ui
#define mpz_rem_ui btc__mpz_rem_ui
#define mpz_quo_si btc__mpz_quo_si
#define mpz_rem_si btc__mpz_rem_si
#define mpz_divmod btc__mpz_divmod
#define mpz_div btc__mpz_div
#define mpz_mod btc__mpz_mod
#define mpz_div_ui btc__mpz_div_ui
#define mpz_mod_ui btc__mpz_mod_ui
#define mpz_div_si btc__mpz_div_si
#define mpz_mod_si btc__mpz_mod_si
#define mpz_divexact btc__mpz_divexact
#define mpz_divexact_ui btc__mpz_divexact_ui
#define mpz_divexact_si btc__mpz_divexact_si
#define mpz_divround btc__mpz_divround
#define mpz_divround_ui btc__mpz_divround_ui
#define mpz_divround_si btc__mpz_divround_si
#define mpz_divisible_p btc__mpz_divisible_p
#define mpz_divisible_ui_p btc__mpz_divisible_ui_p
#define mpz_divisible_2exp_p btc__mpz_divisible_2exp_p
#define mpz_congruent_p btc__mpz_congruent_p
#define mpz_congruent_ui_p btc__mpz_congruent_ui_p
#define mpz_congruent_2exp_p btc__mpz_congruent_2exp_p
#define mpz_pow_ui btc__mpz_pow_ui
#define mpz_ui_pow_ui btc__mpz_ui_pow_ui
#define mpz_rootrem btc__mpz_rootrem
#define mpz_root btc__mpz_root
#define mpz_perfect_power_p btc__mpz_perfect_power_p
#define mpz_sqrtrem btc__mpz_sqrtrem
#define mpz_sqrt btc__mpz_sqrt
#define mpz_perfect_square_p btc__mpz_perfect_square_p
#define mpz_and btc__mpz_and
#define mpz_and_ui btc__mpz_and_ui
#define mpz_and_si btc__mpz_and_si
#define mpz_ior btc__mpz_ior
#define mpz_ior_ui btc__mpz_ior_ui
#define mpz_ior_si btc__mpz_ior_si
#define mpz_xor btc__mpz_xor
#define mpz_xor_ui btc__mpz_xor_ui
#define mpz_xor_si btc__mpz_xor_si
#define mpz_com btc__mpz_com
#define mpz_mul_2exp btc__mpz_mul_2exp
#define mpz_quo_2exp btc__mpz_quo_2exp
#define mpz_rem_2exp btc__mpz_rem_2exp
#define mpz_div_2exp btc__mpz_div_2exp
#define mpz_mod_2exp btc__mpz_mod_2exp
#define mpz_tstbit btc__mpz_tstbit
#define mpz_setbit btc__mpz_setbit
#define mpz_clrbit btc__mpz_clrbit
#define mpz_combit btc__mpz_combit
#define mpz_scan0 btc__mpz_scan0
#define mpz_scan1 btc__mpz_scan1
#define mpz_popcount btc__mpz_popcount
#define mpz_hamdist btc__mpz_hamdist
#define mpz_abs btc__mpz_abs
#define mpz_neg btc__mpz_neg
#define mpz_gcd btc__mpz_gcd
#define mpz_gcd_ui btc__mpz_gcd_ui
#define mpz_lcm btc__mpz_lcm
#define mpz_lcm_ui btc__mpz_lcm_ui
#define mpz_gcdext btc__mpz_gcdext
#define mpz_invert btc__mpz_invert
#define mpz_legendre btc__mpz_legendre
#define mpz_jacobi btc__mpz_jacobi
#define mpz_kronecker btc__mpz_kronecker
#define mpz_kronecker_ui btc__mpz_kronecker_ui
#define mpz_kronecker_si btc__mpz_kronecker_si
#define mpz_ui_kronecker btc__mpz_ui_kronecker
#define mpz_si_kronecker btc__mpz_si_kronecker
#define mpz_powm btc__mpz_powm
#define mpz_powm_ui btc__mpz_powm_ui
#define mpz_powm_sec btc__mpz_powm_sec
#define mpz_sqrtm btc__mpz_sqrtm
#define mpz_sqrtpq btc__mpz_sqrtpq
#define mpz_remove btc__mpz_remove
#define mpz_fac_ui btc__mpz_fac_ui
#define mpz_2fac_ui btc__mpz_2fac_ui
#define mpz_mfac_uiui btc__mpz_mfac_uiui
#define mpz_primorial_ui btc__mpz_primorial_ui
#define mpz_bin_ui btc__mpz_bin_ui
#define mpz_bin_uiui btc__mpz_bin_uiui
#define mpz_bin_siui btc__mpz_bin_siui
#define mpz_fib_ui btc__mpz_fib_ui
#define mpz_fib2_ui btc__mpz_fib2_ui
#define mpz_lucnum_ui btc__mpz_lucnum_ui
#define mpz_lucnum2_ui btc__mpz_lucnum2_ui
#define mpz_mr_prime_p btc__mpz_mr_prime_p
#define mpz_lucas_prime_p btc__mpz_lucas_prime_p
#define mpz_probab_prime_p btc__mpz_probab_prime_p
#define mpz_randprime btc__mpz_randprime
#define mpz_nextprime btc__mpz_nextprime
#define mpz_findprime btc__mpz_findprime
#define mpz_fits_ui_p btc__mpz_fits_ui_p
#define mpz_fits_si_p btc__mpz_fits_si_p
#define mpz_odd_p btc__mpz_odd_p
#define mpz_even_p btc__mpz_even_p
#define mpz_ctz btc__mpz_ctz
#define mpz_bitlen btc__mpz_bitlen
#define mpz_bytelen btc__mpz_bytelen
#define mpz_sizeinbase btc__mpz_sizeinbase
#define mpz_swap btc__mpz_swap
#define _mpz_realloc btc__mpz_realloc
#define mpz_realloc2 btc__mpz_realloc2
#define mpz_getlimbn btc__mpz_getlimbn
#define mpz_size btc__mpz_size
#define mpz_limbs_read btc__mpz_limbs_read
#define mpz_limbs_write btc__mpz_limbs_write
#define mpz_limbs_modify btc__mpz_limbs_modify
#define mpz_limbs_finish btc__mpz_limbs_finish
#define mpz_import btc__mpz_import
#define mpz_export btc__mpz_export
#define mpz_set_str btc__mpz_set_str
#define mpz_get_str btc__mpz_get_str
#define mpz_print btc__mpz_print
#define mpz_urandomb btc__mpz_urandomb
#define mpz_urandomm btc__mpz_urandomm

/*
 * Extern
 */

#define MP_EXTERN BTC_EXTERN

/*
 * Types
 */

#if defined(UINTPTR_MAX) && defined(UINT64_MAX)
/* Check size of uintptr_t if available. */
#  if UINTPTR_MAX >> 31 >> 31 >> 1 == 1
#    define MP_HAVE_64BIT
#  endif
#endif

#if defined(MP_HAVE_64BIT)
typedef uint64_t mp_limb_t;
typedef int64_t mp_long_t;
#  define MP_LIMB_BITS 64
#  define MP_LIMB_BYTES 8
#  define MP_LIMB_C UINT64_C
#  define MP_LIMB_MAX UINT64_MAX
#  define MP_LONG_C INT64_C
#  define MP_LONG_MIN INT64_MIN
#  define MP_LONG_MAX INT64_MAX
#else
typedef uint32_t mp_limb_t;
typedef int32_t mp_long_t;
#  define MP_LIMB_BITS 32
#  define MP_LIMB_BYTES 4
#  define MP_LIMB_C UINT32_C
#  define MP_LIMB_MAX UINT32_MAX
#  define MP_LONG_C INT32_C
#  define MP_LONG_MIN INT32_MIN
#  define MP_LONG_MAX INT32_MAX
#endif

typedef long mp_size_t;
typedef long mp_bits_t;
typedef mp_bits_t mp_bitcnt_t; /* compat */

#define MP_SIZE_C(x) x ## L
#define MP_SIZE_MIN LONG_MIN
#define MP_SIZE_MAX LONG_MAX
#define MP_BITS_C(x) x ## L
#define MP_BITS_MIN LONG_MIN
#define MP_BITS_MAX LONG_MAX

#define MP_LIMB_HI (MP_LIMB_C(1) << (MP_LIMB_BITS - 1))
#define MP_MASK(bits) ((MP_LIMB_C(1) << (bits)) - 1)
#define MP_LOW_BITS (MP_LIMB_BITS / 2)
#define MP_LOW_MASK (MP_LIMB_MAX >> MP_LOW_BITS)

struct mpz_s {
  mp_limb_t *limbs;
  mp_size_t alloc;
  mp_size_t size;
};

typedef struct mpz_s mpz_t[1];

/* Note: these types aren't strictly documented,
 * but they are sometimes referenced in the docs[1],
 * and they are no doubt used by programmers as
 * they have been mentioned on the mailing list
 * a number of times[2][3].
 *
 * [1] https://gmplib.org/manual/Integer-Special-Functions
 * [2] https://gmplib.org/list-archives/gmp-discuss/2011-February/004493.html
 * [3] https://gmplib.org/list-archives/gmp-discuss/2009-May/003769.html
 */
typedef mp_limb_t *mp_ptr;
typedef const mp_limb_t *mp_srcptr;
typedef struct mpz_s *mpz_ptr;
typedef const struct mpz_s *mpz_srcptr;

typedef int mp_puts_f(const char *s);
typedef void mp_rng_f(void *out, size_t size, void *arg);

/*
 * Definitions
 */

#define MP_SLIDE_WIDTH 4
#define MP_SLIDE_SIZE (1 << (MP_SLIDE_WIDTH - 1))
#define MP_FIXED_WIDTH 4
#define MP_FIXED_SIZE (1 << MP_FIXED_WIDTH)

/*
 * Itches
 */

#define MPN_SQR_ITCH(n) (2 * (n))
#define MPN_MULSHIFT_ITCH(n) (2 * (n))
#define MPN_REDUCE_WEAK_ITCH(n) (n)
#define MPN_BARRETT_ITCH(shift) ((shift) + 1)
#define MPN_REDUCE_ITCH(n, shift) (1 + (shift) + ((shift) - (n) + 1))
#define MPN_MONT_ITCH(n) (2 * (n) + 1)
#define MPN_MONTMUL_ITCH(n) (2 * (n))
#define MPN_GCD_ITCH(xn, yn) ((xn) + (yn))
#define MPN_GCD_1_ITCH(xn) (xn)
#define MPN_INVERT_ITCH(n) (4 * ((n) + 1))
#define MPN_SEC_INVERT_ITCH(n) ((n) + MPN_SEC_POWM_ITCH(n))
#define MPN_JACOBI_ITCH(n) (2 * (n))
#define MPN_SLIDE_ITCH(yn, mn) ((yn) > 2 ? (MP_SLIDE_SIZE * (mn)) : 0)
#define MPN_POWM_ITCH(yn, mn) (6 * (mn) + MPN_SLIDE_ITCH(yn, mn))
#define MPN_SEC_POWM_ITCH(n) (5 * (n) + MP_FIXED_SIZE * (n) + 1)

/* Either Barrett or Montgomery precomputation. */
#define MPN_BARRETT_MONT_ITCH(shift) ((shift) + 2)

/*
 * Macros
 */

#define MPZ_ROINIT_N(xp, xs) {{(xp), 0, (xs)}}

/*
 * Globals
 */

/* https://gmplib.org/manual/Useful-Macros-and-Constants */
extern const int mp_bits_per_limb;

/*
 * MPN Interface
 */

/*
 * Initialization
 */

MP_EXTERN void
mpn_zero(mp_limb_t *zp, mp_size_t zn);

/*
 * Uninitialization
 */

MP_EXTERN void
mpn_cleanse(mp_limb_t *zp, mp_size_t zn);

/*
 * Assignment
 */

MP_EXTERN void
mpn_set_1(mp_limb_t *zp, mp_size_t zn, mp_limb_t x);

MP_EXTERN void
mpn_copyi(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn);

MP_EXTERN void
mpn_copyd(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn);

/*
 * Comparison
 */

MP_EXTERN int
mpn_zero_p(const mp_limb_t *xp, mp_size_t xn);

MP_EXTERN int
mpn_cmp(const mp_limb_t *xp, const mp_limb_t *yp, mp_size_t n);

/*
 * Addition
 */

MP_EXTERN mp_limb_t
mpn_add_1(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_limb_t y);

MP_EXTERN mp_limb_t
mpn_add_n(mp_limb_t *zp, const mp_limb_t *xp,
                         const mp_limb_t *yp,
                         mp_size_t n);

MP_EXTERN mp_limb_t
mpn_add(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn,
                       const mp_limb_t *yp, mp_size_t yn);

/*
 * Secure Addition
 */

MP_EXTERN mp_limb_t
mpn_sec_add_1(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_limb_t y);

MP_EXTERN mp_limb_t
mpn_sec_add(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn,
                           const mp_limb_t *yp, mp_size_t yn);

/*
 * Subtraction
 */

MP_EXTERN mp_limb_t
mpn_sub_1(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_limb_t y);

MP_EXTERN mp_limb_t
mpn_sub_n(mp_limb_t *zp, const mp_limb_t *xp,
                         const mp_limb_t *yp,
                         mp_size_t n);

MP_EXTERN mp_limb_t
mpn_sub(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn,
                       const mp_limb_t *yp, mp_size_t yn);

/*
 * Secure Subtraction
 */

MP_EXTERN mp_limb_t
mpn_sec_sub_1(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_limb_t y);

MP_EXTERN mp_limb_t
mpn_sec_sub(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn,
                           const mp_limb_t *yp, mp_size_t yn);

/*
 * Multiplication
 */

MP_EXTERN mp_limb_t
mpn_mul_1(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_limb_t y);

MP_EXTERN mp_limb_t
mpn_addmul_1(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_limb_t y);

MP_EXTERN mp_limb_t
mpn_submul_1(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_limb_t y);

MP_EXTERN void
mpn_mul_n(mp_limb_t *zp, const mp_limb_t *xp,
                         const mp_limb_t *yp,
                         mp_size_t n);

MP_EXTERN void
mpn_mul(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn,
                       const mp_limb_t *yp, mp_size_t yn);

MP_EXTERN void
mpn_sqr(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_limb_t *scratch);

/*
 * Multiply + Shift
 */

MP_EXTERN mp_limb_t
mpn_mulshift(mp_limb_t *zp, const mp_limb_t *xp,
                            const mp_limb_t *yp,
                            mp_size_t n,
                            mp_bits_t bits,
                            mp_limb_t *scratch);

/*
 * Division
 */

MP_EXTERN mp_limb_t
mpn_divmod_1(mp_limb_t *qp, const mp_limb_t *np, mp_size_t nn, mp_limb_t d);

MP_EXTERN void
mpn_div_1(mp_limb_t *qp, const mp_limb_t *np, mp_size_t nn, mp_limb_t d);

MP_EXTERN mp_limb_t
mpn_mod_1(const mp_limb_t *np, mp_size_t nn, mp_limb_t d);

MP_EXTERN void
mpn_divmod(mp_limb_t *qp, mp_limb_t *rp,
           const mp_limb_t *np, mp_size_t nn,
           const mp_limb_t *dp, mp_size_t dn);

MP_EXTERN void
mpn_div(mp_limb_t *qp, const mp_limb_t *np, mp_size_t nn,
                       const mp_limb_t *dp, mp_size_t dn);

MP_EXTERN void
mpn_mod(mp_limb_t *rp, const mp_limb_t *np, mp_size_t nn,
                       const mp_limb_t *dp, mp_size_t dn);

/*
 * Exact Division
 */

MP_EXTERN void
mpn_divexact_1(mp_limb_t *qp, const mp_limb_t *np, mp_size_t nn, mp_limb_t d);

MP_EXTERN void
mpn_divexact(mp_limb_t *qp, const mp_limb_t *np, mp_size_t nn,
                            const mp_limb_t *dp, mp_size_t dn);

/*
 * Roots
 */

MP_EXTERN mp_size_t
mpn_sqrtrem(mp_limb_t *zp, mp_limb_t *rp, const mp_limb_t *xp, mp_size_t xn);

MP_EXTERN int
mpn_perfect_square_p(const mp_limb_t *xp, mp_size_t xn);

/*
 * AND
 */

MP_EXTERN void
mpn_and_n(mp_limb_t *zp, const mp_limb_t *xp,
                         const mp_limb_t *yp,
                         mp_size_t n);

/*
 * OR
 */

MP_EXTERN void
mpn_ior_n(mp_limb_t *zp, const mp_limb_t *xp,
                         const mp_limb_t *yp,
                         mp_size_t n);

/*
 * XOR
 */

MP_EXTERN void
mpn_xor_n(mp_limb_t *zp, const mp_limb_t *xp,
                         const mp_limb_t *yp,
                         mp_size_t n);

/*
 * AND+NOT
 */

MP_EXTERN void
mpn_andn_n(mp_limb_t *zp, const mp_limb_t *xp,
                          const mp_limb_t *yp,
                          mp_size_t n);

/*
 * OR+NOT
 */

MP_EXTERN void
mpn_iorn_n(mp_limb_t *zp, const mp_limb_t *xp,
                          const mp_limb_t *yp,
                          mp_size_t n);

/*
 * NOT+AND
 */

MP_EXTERN void
mpn_nand_n(mp_limb_t *zp, const mp_limb_t *xp,
                          const mp_limb_t *yp,
                          mp_size_t n);

/*
 * NOT+OR
 */

MP_EXTERN void
mpn_nior_n(mp_limb_t *zp, const mp_limb_t *xp,
                          const mp_limb_t *yp,
                          mp_size_t n);

/*
 * NOT+XOR
 */

MP_EXTERN void
mpn_xnor_n(mp_limb_t *zp, const mp_limb_t *xp,
                          const mp_limb_t *yp,
                          mp_size_t n);

/*
 * NOT
 */

MP_EXTERN void
mpn_com(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn);

/*
 * Left Shift
 */

MP_EXTERN mp_limb_t
mpn_lshift(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_bits_t bits);

/*
 * Right Shift
 */

MP_EXTERN mp_limb_t
mpn_rshift(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_bits_t bits);

/*
 * Bit Manipulation
 */

MP_EXTERN mp_limb_t
mpn_getbit(const mp_limb_t *xp, mp_size_t xn, mp_bits_t pos);

MP_EXTERN mp_limb_t
mpn_getbits(const mp_limb_t *xp, mp_size_t xn, mp_bits_t pos, mp_bits_t width);

MP_EXTERN int
mpn_tstbit(const mp_limb_t *xp, mp_bits_t pos);

MP_EXTERN void
mpn_setbit(mp_limb_t *zp, mp_bits_t pos);

MP_EXTERN void
mpn_clrbit(mp_limb_t *zp, mp_bits_t pos);

MP_EXTERN void
mpn_combit(mp_limb_t *zp, mp_bits_t pos);

MP_EXTERN mp_bits_t
mpn_scan0(const mp_limb_t *xp, mp_size_t xn, mp_bits_t pos);

MP_EXTERN mp_bits_t
mpn_scan1(const mp_limb_t *xp, mp_size_t xn, mp_bits_t pos);

MP_EXTERN mp_bits_t
mpn_popcount(const mp_limb_t *xp, mp_size_t xn);

MP_EXTERN mp_bits_t
mpn_hamdist(const mp_limb_t *xp, const mp_limb_t *yp, mp_size_t n);

MP_EXTERN void
mpn_mask(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_bits_t bits);

/*
 * Negation
 */

MP_EXTERN mp_limb_t
mpn_neg(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn);

/*
 * Weak Reduction
 */

MP_EXTERN int
mpn_reduce_weak(mp_limb_t *zp, const mp_limb_t *xp,
                               const mp_limb_t *np,
                               mp_size_t n,
                               mp_limb_t hi,
                               mp_limb_t *scratch);

/*
 * Barrett Reduction
 */

MP_EXTERN void
mpn_barrett(mp_limb_t *mp, const mp_limb_t *np,
                           mp_size_t n,
                           mp_size_t shift,
                           mp_limb_t *scratch);

MP_EXTERN void
mpn_reduce(mp_limb_t *zp, const mp_limb_t *xp,
                          const mp_limb_t *mp,
                          const mp_limb_t *np,
                          mp_size_t n,
                          mp_size_t shift,
                          mp_limb_t *scratch);

/*
 * Montgomery Multiplication
 */

MP_EXTERN void
mpn_mont(mp_limb_t *kp,
         mp_limb_t *rp,
         const mp_limb_t *mp,
         mp_size_t n,
         mp_limb_t *scratch);

MP_EXTERN void
mpn_montmul(mp_limb_t *zp, const mp_limb_t *xp,
                           const mp_limb_t *yp,
                           const mp_limb_t *mp,
                           mp_size_t n,
                           mp_limb_t k,
                           mp_limb_t *scratch);

MP_EXTERN void
mpn_sec_montmul(mp_limb_t *zp, const mp_limb_t *xp,
                               const mp_limb_t *yp,
                               const mp_limb_t *mp,
                               mp_size_t n,
                               mp_limb_t k,
                               mp_limb_t *scratch);

/*
 * Number Theoretic Functions
 */

MP_EXTERN mp_size_t
mpn_gcd(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn,
                       const mp_limb_t *yp, mp_size_t yn,
                       mp_limb_t *scratch);

MP_EXTERN mp_limb_t
mpn_gcd_1(const mp_limb_t *xp, mp_size_t xn, mp_limb_t y, mp_limb_t *scratch);

MP_EXTERN mp_size_t
mpn_gcdext(mp_limb_t *gp,
           mp_limb_t *sp, mp_size_t *sn,
           const mp_limb_t *xp, mp_size_t xn,
           const mp_limb_t *yp, mp_size_t yn);

MP_EXTERN int
mpn_invert(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn,
                          const mp_limb_t *yp, mp_size_t yn,
                          mp_limb_t *scratch);

MP_EXTERN int
mpn_invert_n(mp_limb_t *zp, const mp_limb_t *xp,
                            const mp_limb_t *yp,
                            mp_size_t n,
                            mp_limb_t *scratch);

MP_EXTERN int
mpn_sec_invert(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn,
                              const mp_limb_t *mp, mp_size_t mn,
                              mp_limb_t *scratch);

MP_EXTERN int
mpn_sec_invert_n(mp_limb_t *zp, const mp_limb_t *xp,
                                const mp_limb_t *yp,
                                mp_size_t n,
                                mp_limb_t *scratch);

MP_EXTERN int
mpn_jacobi(const mp_limb_t *xp, mp_size_t xn,
           const mp_limb_t *yp, mp_size_t yn,
           mp_limb_t *scratch);

MP_EXTERN int
mpn_jacobi_n(const mp_limb_t *xp,
             const mp_limb_t *yp,
             mp_size_t n,
             mp_limb_t *scratch);

MP_EXTERN void
mpn_powm(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn,
                        const mp_limb_t *yp, mp_size_t yn,
                        const mp_limb_t *mp, mp_size_t mn,
                        mp_limb_t *scratch);

MP_EXTERN void
mpn_sec_powm(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn,
                            const mp_limb_t *yp, mp_size_t yn,
                            const mp_limb_t *mp, mp_size_t mn,
                            mp_limb_t *scratch);

/*
 * Helpers
 */

MP_EXTERN mp_bits_t
mpn_ctz(const mp_limb_t *xp, mp_size_t xn);

MP_EXTERN mp_bits_t
mpn_bitlen(const mp_limb_t *xp, mp_size_t xn);

MP_EXTERN size_t
mpn_bytelen(const mp_limb_t *xp, mp_size_t xn);

MP_EXTERN size_t
mpn_sizeinbase(const mp_limb_t *xp, mp_size_t xn, int base);

/*
 * Constant Time
 */

MP_EXTERN void
mpn_cnd_zero(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_limb_t cnd);

MP_EXTERN void
mpn_cnd_select(mp_limb_t *zp, const mp_limb_t *xp,
                              const mp_limb_t *yp,
                              mp_size_t n,
                              mp_limb_t cnd);

MP_EXTERN void
mpn_cnd_swap(mp_limb_t *xp, mp_limb_t *yp, mp_size_t n, mp_limb_t cnd);

MP_EXTERN mp_limb_t
mpn_cnd_add_n(mp_limb_t *zp, const mp_limb_t *xp,
                             const mp_limb_t *yp,
                             mp_size_t n,
                             mp_limb_t cnd);

MP_EXTERN mp_limb_t
mpn_cnd_sub_n(mp_limb_t *zp, const mp_limb_t *xp,
                             const mp_limb_t *yp,
                             mp_size_t n,
                             mp_limb_t cnd);

MP_EXTERN mp_limb_t
mpn_cnd_neg(mp_limb_t *zp, const mp_limb_t *xp, mp_size_t xn, mp_limb_t cnd);

MP_EXTERN void
mpn_sec_tabselect(mp_limb_t *zp,
                  const mp_limb_t *tp,
                  mp_size_t n,
                  mp_size_t nents,
                  mp_size_t which);

MP_EXTERN int
mpn_sec_zero_p(const mp_limb_t *xp, mp_size_t xn);

MP_EXTERN int
mpn_sec_equal_p(const mp_limb_t *xp, const mp_limb_t *yp, mp_size_t n);

MP_EXTERN int
mpn_sec_lt_p(const mp_limb_t *xp, const mp_limb_t *yp, mp_size_t n);

MP_EXTERN int
mpn_sec_lte_p(const mp_limb_t *xp, const mp_limb_t *yp, mp_size_t n);

MP_EXTERN int
mpn_sec_gt_p(const mp_limb_t *xp, const mp_limb_t *yp, mp_size_t n);

MP_EXTERN int
mpn_sec_gte_p(const mp_limb_t *xp, const mp_limb_t *yp, mp_size_t n);

MP_EXTERN int
mpn_sec_cmp(const mp_limb_t *xp, const mp_limb_t *yp, mp_size_t n);

/*
 * Import
 */

MP_EXTERN void
mpn_import(mp_limb_t *zp, mp_size_t zn,
           const unsigned char *xp, size_t xn,
           int endian);

/*
 * Export
 */

MP_EXTERN void
mpn_export(unsigned char *zp, size_t zn,
           const mp_limb_t *xp, mp_size_t xn,
           int endian);

/*
 * String Import
 */

MP_EXTERN int
mpn_set_str(mp_limb_t *zp, mp_size_t zn, const char *str, int base);

/*
 * String Export
 */

MP_EXTERN size_t
mpn_get_str(char *str, const mp_limb_t *xp, mp_size_t xn, int base);

/*
 * STDIO
 */

MP_EXTERN void
mpn_print(const mp_limb_t *xp, mp_size_t xn, int base, mp_puts_f *mp_puts);

/*
 * RNG
 */

MP_EXTERN void
mpn_random(mp_limb_t *zp, mp_size_t zn, mp_rng_f *rng, void *arg);

MP_EXTERN void
mpn_randomm(mp_limb_t *zp,
            const mp_limb_t *xp, mp_size_t xn,
            mp_rng_f *rng, void *arg);

/*
 * MPZ Interface
 */

/*
 * Initialization
 */

MP_EXTERN void
mpz_init(mpz_ptr z);

MP_EXTERN void
mpz_init2(mpz_ptr z, mp_bits_t bits);

MP_EXTERN void
mpz_inits(mpz_ptr z, ...);

MP_EXTERN void
mpz_init_set(mpz_ptr z, mpz_srcptr x);

MP_EXTERN void
mpz_init_set_ui(mpz_ptr z, mp_limb_t x);

MP_EXTERN void
mpz_init_set_si(mpz_ptr z, mp_long_t x);

MP_EXTERN void
mpz_init_set_compact(mpz_ptr z, mp_limb_t x);

MP_EXTERN int
mpz_init_set_str(mpz_ptr z, const char *str, int base);

/*
 * Uninitialization
 */

MP_EXTERN void
mpz_clear(mpz_ptr z);

MP_EXTERN void
mpz_clears(mpz_ptr z, ...);

MP_EXTERN void
mpz_cleanse(mpz_ptr z);

MP_EXTERN void
mpz_cleanses(mpz_ptr z, ...);

/*
 * Assignment
 */

MP_EXTERN void
mpz_set(mpz_ptr z, mpz_srcptr x);

MP_EXTERN void
mpz_roset(mpz_ptr z, mpz_srcptr x);

MP_EXTERN mpz_srcptr
mpz_roinit_n(mpz_ptr z, const mp_limb_t *xp, mp_size_t xs);

MP_EXTERN void
mpz_set_ui(mpz_ptr z, mp_limb_t x);

MP_EXTERN void
mpz_set_si(mpz_ptr z, mp_long_t x);

MP_EXTERN void
mpz_set_compact(mpz_ptr z, mp_limb_t x);

/*
 * Conversion
 */

MP_EXTERN mp_limb_t
mpz_get_ui(mpz_srcptr x);

MP_EXTERN mp_long_t
mpz_get_si(mpz_srcptr x);

MP_EXTERN mp_limb_t
mpz_get_compact(mpz_srcptr x);

/*
 * Comparison
 */

MP_EXTERN int
mpz_sgn(mpz_srcptr x);

MP_EXTERN int
mpz_cmp(mpz_srcptr x, mpz_srcptr y);

MP_EXTERN int
mpz_cmp_ui(mpz_srcptr x, mp_limb_t y);

MP_EXTERN int
mpz_cmp_si(mpz_srcptr x, mp_long_t y);

/*
 * Unsigned Comparison
 */

MP_EXTERN int
mpz_cmpabs(mpz_srcptr x, mpz_srcptr y);

MP_EXTERN int
mpz_cmpabs_ui(mpz_srcptr x, mp_limb_t y);

MP_EXTERN int
mpz_cmpabs_si(mpz_srcptr x, mp_long_t y);

/*
 * Addition
 */

MP_EXTERN void
mpz_add(mpz_ptr z, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN void
mpz_add_ui(mpz_ptr z, mpz_srcptr x, mp_limb_t y);

MP_EXTERN void
mpz_add_si(mpz_ptr z, mpz_srcptr x, mp_long_t y);

/*
 * Subtraction
 */

MP_EXTERN void
mpz_sub(mpz_ptr z, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN void
mpz_sub_ui(mpz_ptr z, mpz_srcptr x, mp_limb_t y);

MP_EXTERN void
mpz_sub_si(mpz_ptr z, mpz_srcptr x, mp_long_t y);

MP_EXTERN void
mpz_ui_sub(mpz_ptr z, mp_limb_t x, mpz_srcptr y);

MP_EXTERN void
mpz_si_sub(mpz_ptr z, mp_long_t x, mpz_srcptr y);

/*
 * Multiplication
 */

MP_EXTERN void
mpz_mul(mpz_ptr z, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN void
mpz_mul_ui(mpz_ptr z, mpz_srcptr x, mp_limb_t y);

MP_EXTERN void
mpz_mul_si(mpz_ptr z, mpz_srcptr x, mp_long_t y);

MP_EXTERN void
mpz_sqr(mpz_ptr z, mpz_srcptr x);

MP_EXTERN void
mpz_addmul(mpz_ptr z, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN void
mpz_addmul_ui(mpz_ptr z, mpz_srcptr x, mp_limb_t y);

MP_EXTERN void
mpz_addmul_si(mpz_ptr z, mpz_srcptr x, mp_long_t y);

MP_EXTERN void
mpz_submul(mpz_ptr z, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN void
mpz_submul_ui(mpz_ptr z, mpz_srcptr x, mp_limb_t y);

MP_EXTERN void
mpz_submul_si(mpz_ptr z, mpz_srcptr x, mp_long_t y);

/*
 * Multiply + Shift
 */

MP_EXTERN void
mpz_mulshift(mpz_ptr z, mpz_srcptr x, mpz_srcptr y, mp_bits_t bits);

/*
 * Truncation Division
 */

MP_EXTERN void
mpz_quorem(mpz_ptr q, mpz_ptr r, mpz_srcptr n, mpz_srcptr d);

MP_EXTERN void
mpz_quo(mpz_ptr q, mpz_srcptr n, mpz_srcptr d);

MP_EXTERN void
mpz_rem(mpz_ptr r, mpz_srcptr n, mpz_srcptr d);

MP_EXTERN mp_limb_t
mpz_quo_ui(mpz_ptr q, mpz_srcptr n, mp_limb_t d);

MP_EXTERN mp_limb_t
mpz_rem_ui(mpz_srcptr n, mp_limb_t d);

MP_EXTERN mp_long_t
mpz_quo_si(mpz_ptr q, mpz_srcptr n, mp_long_t d);

MP_EXTERN mp_long_t
mpz_rem_si(mpz_srcptr n, mp_long_t d);

/*
 * Euclidean Division
 */

MP_EXTERN void
mpz_divmod(mpz_ptr q, mpz_ptr r, mpz_srcptr n, mpz_srcptr d);

MP_EXTERN void
mpz_div(mpz_ptr q, mpz_srcptr n, mpz_srcptr d);

MP_EXTERN void
mpz_mod(mpz_ptr r, mpz_srcptr n, mpz_srcptr d);

MP_EXTERN mp_limb_t
mpz_div_ui(mpz_ptr q, mpz_srcptr n, mp_limb_t d);

MP_EXTERN mp_limb_t
mpz_mod_ui(mpz_srcptr n, mp_limb_t d);

MP_EXTERN mp_long_t
mpz_div_si(mpz_ptr q, mpz_srcptr n, mp_long_t d);

MP_EXTERN mp_long_t
mpz_mod_si(mpz_srcptr n, mp_long_t d);

/*
 * Exact Division
 */

MP_EXTERN void
mpz_divexact(mpz_ptr q, mpz_srcptr n, mpz_srcptr d);

MP_EXTERN void
mpz_divexact_ui(mpz_ptr q, mpz_srcptr n, mp_limb_t d);

MP_EXTERN void
mpz_divexact_si(mpz_ptr q, mpz_srcptr n, mp_long_t d);

/*
 * Round Division
 */

MP_EXTERN void
mpz_divround(mpz_ptr q, mpz_srcptr n, mpz_srcptr d);

MP_EXTERN void
mpz_divround_ui(mpz_ptr q, mpz_srcptr n, mp_limb_t d);

MP_EXTERN void
mpz_divround_si(mpz_ptr q, mpz_srcptr n, mp_long_t d);

/*
 * Divisibility
 */

MP_EXTERN int
mpz_divisible_p(mpz_srcptr n, mpz_srcptr d);

MP_EXTERN int
mpz_divisible_ui_p(mpz_srcptr n, mp_limb_t d);

MP_EXTERN int
mpz_divisible_2exp_p(mpz_srcptr n, mp_bits_t bits);

/*
 * Congruence
 */

MP_EXTERN int
mpz_congruent_p(mpz_srcptr x, mpz_srcptr y, mpz_srcptr d);

MP_EXTERN int
mpz_congruent_ui_p(mpz_srcptr x, mpz_srcptr y, mp_limb_t d);

MP_EXTERN int
mpz_congruent_2exp_p(mpz_srcptr x, mpz_srcptr y, mp_bits_t bits);

/*
 * Exponentiation
 */

MP_EXTERN void
mpz_pow_ui(mpz_ptr z, mpz_srcptr x, mp_limb_t y);

MP_EXTERN void
mpz_ui_pow_ui(mpz_ptr z, mp_limb_t x, mp_limb_t y);

/*
 * Roots
 */

MP_EXTERN void
mpz_rootrem(mpz_ptr z, mpz_ptr r, mpz_srcptr x, mp_limb_t k);

MP_EXTERN int
mpz_root(mpz_ptr z, mpz_srcptr x, mp_limb_t k);

MP_EXTERN int
mpz_perfect_power_p(mpz_srcptr x);

MP_EXTERN void
mpz_sqrtrem(mpz_ptr z, mpz_ptr r, mpz_srcptr x);

MP_EXTERN void
mpz_sqrt(mpz_ptr z, mpz_srcptr x);

MP_EXTERN int
mpz_perfect_square_p(mpz_srcptr x);

/*
 * AND
 */

MP_EXTERN void
mpz_and(mpz_ptr z, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN mp_limb_t
mpz_and_ui(mpz_srcptr x, mp_limb_t y);

MP_EXTERN void
mpz_and_si(mpz_ptr z, mpz_srcptr x, mp_long_t y);

/*
 * OR
 */

MP_EXTERN void
mpz_ior(mpz_ptr z, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN void
mpz_ior_ui(mpz_ptr z, mpz_srcptr x, mp_limb_t y);

MP_EXTERN void
mpz_ior_si(mpz_ptr z, mpz_srcptr x, mp_long_t y);

/*
 * XOR
 */

MP_EXTERN void
mpz_xor(mpz_ptr z, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN void
mpz_xor_ui(mpz_ptr z, mpz_srcptr x, mp_limb_t y);

MP_EXTERN void
mpz_xor_si(mpz_ptr z, mpz_srcptr x, mp_long_t y);

/*
 * NOT
 */

MP_EXTERN void
mpz_com(mpz_ptr z, mpz_srcptr x);

/*
 * Left Shift
 */

MP_EXTERN void
mpz_mul_2exp(mpz_ptr z, mpz_srcptr x, mp_bits_t bits);

/*
 * Unsigned Right Shift
 */

MP_EXTERN void
mpz_quo_2exp(mpz_ptr z, mpz_srcptr x, mp_bits_t bits);

MP_EXTERN void
mpz_rem_2exp(mpz_ptr z, mpz_srcptr x, mp_bits_t bits);

/*
 * Right Shift
 */

MP_EXTERN void
mpz_div_2exp(mpz_ptr z, mpz_srcptr x, mp_bits_t bits);

MP_EXTERN void
mpz_mod_2exp(mpz_ptr z, mpz_srcptr x, mp_bits_t bits);

/*
 * Bit Manipulation
 */

MP_EXTERN int
mpz_tstbit(mpz_srcptr x, mp_bits_t pos);

MP_EXTERN void
mpz_setbit(mpz_ptr z, mp_bits_t pos);

MP_EXTERN void
mpz_clrbit(mpz_ptr z, mp_bits_t pos);

MP_EXTERN void
mpz_combit(mpz_ptr z, mp_bits_t pos);

MP_EXTERN mp_bits_t
mpz_scan0(mpz_srcptr x, mp_bits_t pos);

MP_EXTERN mp_bits_t
mpz_scan1(mpz_srcptr x, mp_bits_t pos);

MP_EXTERN mp_bits_t
mpz_popcount(mpz_srcptr x);

MP_EXTERN mp_bits_t
mpz_hamdist(mpz_srcptr x, mpz_srcptr y);

/*
 * Negation
 */

MP_EXTERN void
mpz_abs(mpz_ptr z, mpz_srcptr x);

MP_EXTERN void
mpz_neg(mpz_ptr z, mpz_srcptr x);

/*
 * Number Theoretic Functions
 */

MP_EXTERN void
mpz_gcd(mpz_ptr z, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN mp_limb_t
mpz_gcd_ui(mpz_ptr z, mpz_srcptr x, mp_limb_t y);

MP_EXTERN void
mpz_lcm(mpz_ptr z, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN void
mpz_lcm_ui(mpz_ptr z, mpz_srcptr x, mp_limb_t y);

MP_EXTERN void
mpz_gcdext(mpz_ptr g, mpz_ptr s, mpz_ptr t, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN int
mpz_invert(mpz_ptr z, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN int
mpz_legendre(mpz_srcptr x, mpz_srcptr p);

MP_EXTERN int
mpz_jacobi(mpz_srcptr x, mpz_srcptr y);

MP_EXTERN int
mpz_kronecker(mpz_srcptr x, mpz_srcptr y);

MP_EXTERN int
mpz_kronecker_ui(mpz_srcptr x, mp_limb_t y);

MP_EXTERN int
mpz_kronecker_si(mpz_srcptr x, mp_long_t y);

MP_EXTERN int
mpz_ui_kronecker(mp_limb_t x, mpz_srcptr y);

MP_EXTERN int
mpz_si_kronecker(mp_long_t x, mpz_srcptr y);

MP_EXTERN void
mpz_powm(mpz_ptr z, mpz_srcptr x, mpz_srcptr y, mpz_srcptr m);

MP_EXTERN void
mpz_powm_ui(mpz_ptr z, mpz_srcptr x, mp_limb_t y, mpz_srcptr m);

MP_EXTERN void
mpz_powm_sec(mpz_ptr z, mpz_srcptr x, mpz_srcptr y, mpz_srcptr m);

MP_EXTERN int
mpz_sqrtm(mpz_ptr z, mpz_srcptr x, mpz_srcptr p);

MP_EXTERN int
mpz_sqrtpq(mpz_ptr z, mpz_srcptr x, mpz_srcptr p, mpz_srcptr q);

MP_EXTERN mp_bits_t
mpz_remove(mpz_ptr z, mpz_srcptr x, mpz_srcptr y);

MP_EXTERN void
mpz_fac_ui(mpz_ptr z, mp_limb_t n);

MP_EXTERN void
mpz_2fac_ui(mpz_ptr z, mp_limb_t n);

MP_EXTERN void
mpz_mfac_uiui(mpz_ptr z, mp_limb_t n, mp_limb_t m);

MP_EXTERN void
mpz_primorial_ui(mpz_ptr z, mp_limb_t n);

MP_EXTERN void
mpz_bin_ui(mpz_ptr z, mpz_srcptr n, mp_limb_t k);

MP_EXTERN void
mpz_bin_uiui(mpz_ptr z, mp_limb_t n, mp_limb_t k);

MP_EXTERN void
mpz_bin_siui(mpz_ptr z, mp_long_t n, mp_limb_t k);

MP_EXTERN void
mpz_fib_ui(mpz_ptr fn, mp_limb_t n);

MP_EXTERN void
mpz_fib2_ui(mpz_ptr fn, mpz_ptr fn1, mp_limb_t n);

MP_EXTERN void
mpz_lucnum_ui(mpz_ptr ln, mp_limb_t n);

MP_EXTERN void
mpz_lucnum2_ui(mpz_ptr ln, mpz_ptr ln1, mp_limb_t n);

/*
 * Primality Testing
 */

MP_EXTERN int
mpz_mr_prime_p(mpz_srcptr n, int reps, int force2, mp_rng_f *rng, void *arg);

MP_EXTERN int
mpz_lucas_prime_p(mpz_srcptr n, mp_limb_t limit);

MP_EXTERN int
mpz_probab_prime_p(mpz_srcptr x, int rounds, mp_rng_f *rng, void *arg);

MP_EXTERN void
mpz_randprime(mpz_ptr z, mp_bits_t bits, mp_rng_f *rng, void *arg);

MP_EXTERN void
mpz_nextprime(mpz_ptr z, mpz_srcptr x, mp_rng_f *rng, void *arg);

MP_EXTERN int
mpz_findprime(mpz_ptr z, mpz_srcptr x, mp_limb_t m, mp_rng_f *rng, void *arg);

/*
 * Helpers
 */

MP_EXTERN int
mpz_fits_ui_p(mpz_srcptr x);

MP_EXTERN int
mpz_fits_si_p(mpz_srcptr x);

MP_EXTERN int
mpz_odd_p(mpz_srcptr x);

MP_EXTERN int
mpz_even_p(mpz_srcptr x);

MP_EXTERN mp_bits_t
mpz_ctz(mpz_srcptr x);

MP_EXTERN mp_bits_t
mpz_bitlen(mpz_srcptr x);

MP_EXTERN size_t
mpz_bytelen(mpz_srcptr x);

MP_EXTERN size_t
mpz_sizeinbase(mpz_srcptr x, int base);

MP_EXTERN void
mpz_swap(mpz_ptr x, mpz_ptr y);

MP_EXTERN void *
_mpz_realloc(mpz_ptr z, mp_size_t n);

MP_EXTERN void
mpz_realloc2(mpz_ptr z, mp_bits_t bits);

/*
 * Limb Helpers
 */

MP_EXTERN mp_limb_t
mpz_getlimbn(mpz_srcptr x, mp_size_t n);

MP_EXTERN size_t
mpz_size(mpz_srcptr x);

MP_EXTERN const mp_limb_t *
mpz_limbs_read(mpz_srcptr x);

MP_EXTERN mp_limb_t *
mpz_limbs_write(mpz_ptr z, mp_size_t n);

MP_EXTERN mp_limb_t *
mpz_limbs_modify(mpz_ptr z, mp_size_t n);

MP_EXTERN void
mpz_limbs_finish(mpz_ptr z, mp_size_t n);

/*
 * Import
 */

MP_EXTERN void
mpz_import(mpz_ptr z, const unsigned char *raw, size_t size, int endian);

/*
 * Export
 */

MP_EXTERN void
mpz_export(unsigned char *raw, mpz_srcptr x, size_t size, int endian);

/*
 * String Import
 */

MP_EXTERN int
mpz_set_str(mpz_ptr z, const char *str, int base);

/*
 * String Export
 */

MP_EXTERN char *
mpz_get_str(mpz_srcptr x, int base);

/*
 * STDIO
 */

MP_EXTERN void
mpz_print(mpz_srcptr x, int base, mp_puts_f *mp_puts);

/*
 * RNG
 */

MP_EXTERN void
mpz_urandomb(mpz_ptr z, mp_bits_t bits, mp_rng_f *rng, void *arg);

MP_EXTERN void
mpz_urandomm(mpz_ptr z, mpz_srcptr x, mp_rng_f *rng, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* BTC_MPI_H */
