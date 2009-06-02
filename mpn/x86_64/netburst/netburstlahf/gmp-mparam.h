/* Generated by tuneup.c, 2009-04-09, gcc 4.2 */

#define MUL_KARATSUBA_THRESHOLD          18
#define MUL_TOOM3_THRESHOLD             113
#define MUL_TOOM4_THRESHOLD             336
#define MUL_TOOM7_THRESHOLD             402

#define SQR_BASECASE_THRESHOLD            0  /* always (native) */
#define SQR_KARATSUBA_THRESHOLD          32
#define SQR_TOOM3_THRESHOLD             129

#define MULLOW_BASECASE_THRESHOLD         0  /* always */
#define MULLOW_DC_THRESHOLD              40
#define MULLOW_MUL_N_THRESHOLD          426

#define DIV_SB_PREINV_THRESHOLD           0  /* always */
#define DIV_DC_THRESHOLD                 40
#define POWM_THRESHOLD                   60

#define GCD_ACCEL_THRESHOLD              89
#define GCDEXT_THRESHOLD                 53
#define JACOBI_BASE_METHOD                1

#define DIVREM_1_NORM_THRESHOLD       MP_SIZE_T_MAX  /* never */
#define DIVREM_1_UNNORM_THRESHOLD     MP_SIZE_T_MAX  /* never */
#define MOD_1_NORM_THRESHOLD              0  /* always */
#define MOD_1_UNNORM_THRESHOLD            0  /* always */
#define USE_PREINV_DIVREM_1               0
#define USE_PREINV_MOD_1                  1
#define DIVREM_2_THRESHOLD                0  /* always */
#define DIVEXACT_1_THRESHOLD              0  /* always */
#define MODEXACT_1_ODD_THRESHOLD          0  /* always (native) */

#define GET_STR_DC_THRESHOLD             13
#define GET_STR_PRECOMPUTE_THRESHOLD     19
#define SET_STR_THRESHOLD              3296

#define MUL_FFT_TABLE  { 432, 928, 1856, 3840, 9216, 20480, 81920, 196608, 1310720, 5242880, 12582912, 0 }
#define MUL_FFT_MODF_THRESHOLD          448
#define MUL_FFT_THRESHOLD              5632

#define SQR_FFT_TABLE  { 432, 992, 1856, 3840, 9216, 28672, 81920, 196608, 786432, 3145728, 0 }
#define SQR_FFT_MODF_THRESHOLD          448
#define SQR_FFT_THRESHOLD              2688

/* Tuneup completed successfully, took 49 seconds */