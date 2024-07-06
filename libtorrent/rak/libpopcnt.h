/*
 * libpopcnt.h - C/C++ library for counting the number of 1 bits (bit
 * population count) in an array as quickly as possible using
 * specialized CPU instructions i.e. POPCNT, AVX2, AVX512, NEON.
 *
 * Copyright (c) 2016 - 2024, Kim Walisch
 * Copyright (c) 2016 - 2018, Wojciech Muła
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBPOPCNT_H
#define LIBPOPCNT_H

#include <stdint.h>

#ifndef __has_builtin
  #define __has_builtin(x) 0
#endif

#ifndef __has_attribute
  #define __has_attribute(x) 0
#endif

#ifndef __has_include
  #define __has_include(x) 0
#endif

#ifdef __GNUC__
  #define LIBPOPCNT_GNUC_PREREQ(x, y) \
      (__GNUC__ > x || (__GNUC__ == x && __GNUC_MINOR__ >= y))
#else
  #define LIBPOPCNT_GNUC_PREREQ(x, y) 0
#endif

#ifdef __clang__
  #define LIBPOPCNT_CLANG_PREREQ(x, y) \
      (__clang_major__ > x || (__clang_major__ == x && __clang_minor__ >= y))
#else
  #define LIBPOPCNT_CLANG_PREREQ(x, y) 0
#endif

#if (_MSC_VER < 1900) && \
    !defined(__cplusplus)
  #define inline __inline
#endif

#if (defined(__i386__) || \
     defined(__x86_64__) || \
     defined(_M_IX86) || \
     defined(_M_X64))
  #define LIBPOPCNT_X86_OR_X64
#endif

#if LIBPOPCNT_GNUC_PREREQ(4, 2) || \
    __has_builtin(__builtin_popcount)
  #define LIBPOPCNT_HAVE_BUILTIN_POPCOUNT
#endif

#if LIBPOPCNT_GNUC_PREREQ(4, 2) || \
    LIBPOPCNT_CLANG_PREREQ(3, 0)
  #define LIBPOPCNT_HAVE_ASM_POPCNT
#endif

#if defined(LIBPOPCNT_X86_OR_X64) && \
   (defined(LIBPOPCNT_HAVE_ASM_POPCNT) || \
    defined(_MSC_VER))
  #define LIBPOPCNT_HAVE_POPCNT
#endif

/* GCC compiler */
#if defined(LIBPOPCNT_X86_OR_X64) && \
    LIBPOPCNT_GNUC_PREREQ(5, 0)
  #define LIBPOPCNT_HAVE_AVX2
#endif

/* GCC compiler */
#if defined(LIBPOPCNT_X86_OR_X64) && \
    LIBPOPCNT_GNUC_PREREQ(11, 0)
  #define LIBPOPCNT_HAVE_AVX512
#endif

/* Clang (Unix-like OSes) */
#if defined(LIBPOPCNT_X86_OR_X64) && !defined(_MSC_VER)
  #if LIBPOPCNT_CLANG_PREREQ(3, 8) && \
      __has_attribute(target) && \
      (!defined(__apple_build_version__) || __apple_build_version__ >= 8000000)
    #define LIBPOPCNT_HAVE_AVX2
  #endif
  #if LIBPOPCNT_CLANG_PREREQ(9, 0) && \
      __has_attribute(target) && \
      (!defined(__apple_build_version__) || __apple_build_version__ >= 8000000)
    #define LIBPOPCNT_HAVE_AVX512
  #endif
#endif

/*
 * This uses fewer arithmetic operations than any other known
 * implementation on machines with fast multiplication.
 * It uses 12 arithmetic operations, one of which is a multiply.
 * http://en.wikipedia.org/wiki/Hamming_weight#Efficient_implementation
 */
static inline uint64_t popcnt64_bitwise(uint64_t x)
{
  uint64_t m1 = 0x5555555555555555ull;
  uint64_t m2 = 0x3333333333333333ull;
  uint64_t m4 = 0x0F0F0F0F0F0F0F0Full;
  uint64_t h01 = 0x0101010101010101ull;

  x -= (x >> 1) & m1;
  x = (x & m2) + ((x >> 2) & m2);
  x = (x + (x >> 4)) & m4;

  return (x * h01) >> 56;
}

#if defined(LIBPOPCNT_HAVE_ASM_POPCNT) && \
    defined(__x86_64__)

static inline uint64_t popcnt64(uint64_t x)
{
  __asm__ ("popcnt %1, %0" : "=r" (x) : "0" (x));
  return x;
}

/* no hardware POPCNT,
 * use pure integer algorithm */
#else

static inline uint64_t popcnt64(uint64_t x)
{
  return popcnt64_bitwise(x);
}

#endif
#endif
