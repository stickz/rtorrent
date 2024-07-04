// rak - Rakshasa's toolbox
// Copyright (C) 2005-2007, Jari Sundell
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// In addition, as a special exception, the copyright holders give
// permission to link the code of portions of this program with the
// OpenSSL library under certain conditions as described in each
// individual source file, and distribute linked combinations
// including the two.
//
// You must obey the GNU General Public License in all respects for
// all of the code used other than OpenSSL.  If you modify file(s)
// with this exception, you may extend this exception to your version
// of the file(s), but you are not obligated to do so.  If you do not
// wish to do so, delete this exception statement from your version.
// If you delete this exception statement from all source files in the
// program, then also delete it here.
//
// Contact:  Jari Sundell <jaris@ifi.uio.no>
//
//           Skomakerveien 33
//           3185 Skoppum, NORWAY

#ifndef RAK_ALGORITHM_H
#define RAK_ALGORITHM_H

#include <algorithm>
#include <functional>
#include <limits>

#if USE_AVX2_POPCOUNT
#include <immintrin.h>
#include <x86intrin.h>
#endif

namespace rak {

#if USE_AVX2_POPCOUNT
std::uint64_t popcnt_AVX2_lookup(const uint8_t* data, const size_t n) {

    size_t i = 0;

    const __m256i lookup = _mm256_setr_epi8(
        /* 0 */ 0, /* 1 */ 1, /* 2 */ 1, /* 3 */ 2,
        /* 4 */ 1, /* 5 */ 2, /* 6 */ 2, /* 7 */ 3,
        /* 8 */ 1, /* 9 */ 2, /* a */ 2, /* b */ 3,
        /* c */ 2, /* d */ 3, /* e */ 3, /* f */ 4,

        /* 0 */ 0, /* 1 */ 1, /* 2 */ 1, /* 3 */ 2,
        /* 4 */ 1, /* 5 */ 2, /* 6 */ 2, /* 7 */ 3,
        /* 8 */ 1, /* 9 */ 2, /* a */ 2, /* b */ 3,
        /* c */ 2, /* d */ 3, /* e */ 3, /* f */ 4
    );

    const __m256i low_mask = _mm256_set1_epi8(0x0f);

    __m256i acc = _mm256_setzero_si256();

#define ITER { \
        const __m256i vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i)); \
        const __m256i lo  = _mm256_and_si256(vec, low_mask); \
        const __m256i hi  = _mm256_and_si256(_mm256_srli_epi16(vec, 4), low_mask); \
        const __m256i popcnt1 = _mm256_shuffle_epi8(lookup, lo); \
        const __m256i popcnt2 = _mm256_shuffle_epi8(lookup, hi); \
        local = _mm256_add_epi8(local, popcnt1); \
        local = _mm256_add_epi8(local, popcnt2); \
        i += 32; \
    }

    while (i + 8*32 <= n) {
        __m256i local = _mm256_setzero_si256();
        ITER ITER ITER ITER
        ITER ITER ITER ITER
        acc = _mm256_add_epi64(acc, _mm256_sad_epu8(local, _mm256_setzero_si256()));
    }

    __m256i local = _mm256_setzero_si256();

    while (i + 32 <= n) {
        ITER;
    }

    acc = _mm256_add_epi64(acc, _mm256_sad_epu8(local, _mm256_setzero_si256()));

#undef ITER

    uint64_t result = 0;

    result += static_cast<uint64_t>(_mm256_extract_epi64(acc, 0));
    result += static_cast<uint64_t>(_mm256_extract_epi64(acc, 1));
    result += static_cast<uint64_t>(_mm256_extract_epi64(acc, 2));
    result += static_cast<uint64_t>(_mm256_extract_epi64(acc, 3));

    for (/**/; i < n; i++) {
        result += lookup8bit[data[i]];
    }

    return result;
}
#endif

template <typename _InputIter, typename _Function>
_Function
for_each_pre(_InputIter __first, _InputIter __last, _Function __f) {
  _InputIter __tmp;

  while (__first != __last) {
    __tmp = __first++;
    
    __f(*__tmp);
  }

  return __f;
}

// Return a range with a distance of no more than __distance and
// between __first and __last, centered on __middle1.
template <typename _InputIter, typename _Distance>
std::pair<_InputIter, _InputIter>
advance_bidirectional(_InputIter __first, _InputIter __middle1, _InputIter __last, _Distance __distance) {
  _InputIter __middle2 = __middle1;

  do {
    if (!__distance)
      break;

    if (__middle2 != __last) {
      ++__middle2;
      --__distance;

    } else if (__middle1 == __first) {
      break;
    }

    if (!__distance)
      break;

    if (__middle1 != __first) {
      --__middle1;
      --__distance;

    } else if (__middle2 == __last) {
      break;
    }

  } while (true);

  return std::make_pair(__middle1, __middle2);
}

template <typename _InputIter, typename _Distance>
_InputIter
advance_forward(_InputIter __first, _InputIter __last, _Distance __distance) {
  while (__first != __last && __distance != 0) {
    __first++;
    __distance--;
  }

  return __first;
}

template <typename _InputIter, typename _Distance>
_InputIter
advance_backward(_InputIter __first, _InputIter __last, _Distance __distance) {
  while (__first != __last && __distance != 0) {
    __first--;
    __distance--;
  }

  return __first;
}

template <typename _Value>
struct compare_base : public std::binary_function<_Value, _Value, bool> {
  bool operator () (const _Value& complete, const _Value& base) const {
    return !complete.compare(0, base.size(), base);
  }
};

// Count the number of elements from the start of the containers to
// the first inequal element.
template <typename _InputIter1, typename _InputIter2>
typename std::iterator_traits<_InputIter1>::difference_type
count_base(_InputIter1 __first1, _InputIter1 __last1,
	   _InputIter2 __first2, _InputIter2 __last2) {

  typename std::iterator_traits<_InputIter1>::difference_type __n = 0;

  for ( ;__first1 != __last1 && __first2 != __last2; ++__first1, ++__first2, ++__n)
    if (*__first1 != *__first2)
      return __n;

  return __n;
}

template <typename _Return, typename _InputIter, typename _Ftor>
_Return
make_base(_InputIter __first, _InputIter __last, _Ftor __ftor) {
  if (__first == __last)
    return "";

  _Return __base = __ftor(*__first++);

  for ( ;__first != __last; ++__first) {
    typename std::iterator_traits<_InputIter>::difference_type __pos = count_base(__base.begin(), __base.end(),
										  __ftor(*__first).begin(), __ftor(*__first).end());

    if (__pos < (typename std::iterator_traits<_InputIter>::difference_type)__base.size())
      __base.resize(__pos);
  }

  return __base;
}

template<typename T>
inline int popcount_wrapper(T t) {
#if USE_AVX2_POPCOUNT
  return popcnt_AVX2_lookup(t, std::numeric_limits<T>::max);
#else	
#if USE_BUILTIN_POPCOUNT
  if (std::numeric_limits<T>::digits <= std::numeric_limits<unsigned int>::digits)
    return __builtin_popcount(t);
  else
    return __builtin_popcountll(t);
#else
#error __builtin_popcount not found.
  unsigned int count = 0;
  
  while (t) {
    count += t & 0x1;
    t >> 1;
  }

  return count;
#endif
#endif
}

}

#endif
