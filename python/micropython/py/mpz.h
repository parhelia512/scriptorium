/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef __MICROPY_INCLUDED_PY_MPZ_H__
#define __MICROPY_INCLUDED_PY_MPZ_H__

#include <stdint.h>

#include "py/mpconfig.h"
#include "py/misc.h"

// This mpz module implements arbitrary precision integers.
//
// The storage for each digit is defined by mpz_dig_t.  The actual number of
// bits in mpz_dig_t that are used is defined by MPZ_DIG_SIZE.  The machine must
// also provide a type that is twice as wide as mpz_dig_t, in both signed and
// unsigned versions.
//
// MPZ_DIG_SIZE can be between 4 and 8*sizeof(mpz_dig_t), but it makes most
// sense to have it as large as possible.  If MPZ_DIG_SIZE is not already
// defined then it is auto-detected below, depending on the machine.  The types
// are then set based on the value of MPZ_DIG_SIZE (although they can be freely
// changed so long as the constraints mentioned above are met).

#ifndef MPZ_DIG_SIZE
  #if defined(__x86_64__) || defined(_WIN64)
    // 64-bit machine, using 32-bit storage for digits
    #define MPZ_DIG_SIZE (32)
  #else
    // default: 32-bit machine, using 16-bit storage for digits
    #define MPZ_DIG_SIZE (16)
  #endif
#endif

#if MPZ_DIG_SIZE > 16
typedef uint32_t mpz_dig_t;
typedef uint64_t mpz_dbl_dig_t;
typedef int64_t mpz_dbl_dig_signed_t;
#elif MPZ_DIG_SIZE > 8
typedef uint16_t mpz_dig_t;
typedef uint32_t mpz_dbl_dig_t;
typedef int32_t mpz_dbl_dig_signed_t;
#elif MPZ_DIG_SIZE > 4
typedef uint8_t mpz_dig_t;
typedef uint16_t mpz_dbl_dig_t;
typedef int16_t mpz_dbl_dig_signed_t;
#else
typedef uint8_t mpz_dig_t;
typedef uint8_t mpz_dbl_dig_t;
typedef int8_t mpz_dbl_dig_signed_t;
#endif

#ifdef _WIN64
  #define MPZ_LONG_1 1i64
#else
  #define MPZ_LONG_1 1L
#endif

#define MPZ_NUM_DIG_FOR_INT (sizeof(mp_int_t) * 8 / MPZ_DIG_SIZE + 1)
#define MPZ_NUM_DIG_FOR_LL (sizeof(long long) * 8 / MPZ_DIG_SIZE + 1)

typedef struct _mpz_t {
    mp_uint_t neg : 1;
    mp_uint_t fixed_dig : 1;
    mp_uint_t alloc : BITS_PER_WORD - 2;
    mp_uint_t len;
    mpz_dig_t *dig;
} mpz_t;

// convenience macro to declare an mpz with a digit array from the stack, initialised by an integer
#define MPZ_CONST_INT(z, val) mpz_t z; mpz_dig_t z ## _digits[MPZ_NUM_DIG_FOR_INT]; mpz_init_fixed_from_int(&z, z_digits, MPZ_NUM_DIG_FOR_INT, val);

void mpz_init_zero(mpz_t *z);
void mpz_init_from_int(mpz_t *z, mp_int_t val);
void mpz_init_fixed_from_int(mpz_t *z, mpz_dig_t *dig, mp_uint_t dig_alloc, mp_int_t val);
void mpz_deinit(mpz_t *z);

void mpz_set(mpz_t *dest, const mpz_t *src);
void mpz_set_from_int(mpz_t *z, mp_int_t src);
void mpz_set_from_ll(mpz_t *z, long long i, bool is_signed);
#if MICROPY_PY_BUILTINS_FLOAT
void mpz_set_from_float(mpz_t *z, mp_float_t src);
#endif
mp_uint_t mpz_set_from_str(mpz_t *z, const char *str, mp_uint_t len, bool neg, mp_uint_t base);

bool mpz_is_zero(const mpz_t *z);
int mpz_cmp(const mpz_t *lhs, const mpz_t *rhs);

void mpz_abs_inpl(mpz_t *dest, const mpz_t *z);
void mpz_neg_inpl(mpz_t *dest, const mpz_t *z);
void mpz_not_inpl(mpz_t *dest, const mpz_t *z);
void mpz_shl_inpl(mpz_t *dest, const mpz_t *lhs, mp_int_t rhs);
void mpz_shr_inpl(mpz_t *dest, const mpz_t *lhs, mp_int_t rhs);
void mpz_add_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_sub_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_mul_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_pow_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_and_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_or_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_xor_inpl(mpz_t *dest, const mpz_t *lhs, const mpz_t *rhs);
void mpz_divmod_inpl(mpz_t *dest_quo, mpz_t *dest_rem, const mpz_t *lhs, const mpz_t *rhs);

mp_int_t mpz_hash(const mpz_t *z);
bool mpz_as_int_checked(const mpz_t *z, mp_int_t *value);
bool mpz_as_uint_checked(const mpz_t *z, mp_uint_t *value);
void mpz_as_bytes(const mpz_t *z, bool big_endian, mp_uint_t len, byte *buf);
#if MICROPY_PY_BUILTINS_FLOAT
mp_float_t mpz_as_float(const mpz_t *z);
#endif
mp_uint_t mpz_as_str_size(const mpz_t *i, mp_uint_t base, const char *prefix, char comma);
mp_uint_t mpz_as_str_inpl(const mpz_t *z, mp_uint_t base, const char *prefix, char base_char, char comma, char *str);

#endif // __MICROPY_INCLUDED_PY_MPZ_H__
