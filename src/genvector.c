/*
   This product is distributed under 2-term BSD-license terms

   Copyright (c) 2023, QApp. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met: 

   1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
   2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "genvector.h"
#include "params.h"
#include "randombytes.h"

#include <stddef.h>
#include <string.h>

static inline void uint64_cmp(uint64_t *a, uint64_t *b) {
  uint64_t a_tmp = *a;
  uint64_t b_tmp = *b;
  uint64_t c_tmp = b_tmp - a_tmp;
  c_tmp >>= 63;

  c_tmp = -c_tmp;
  c_tmp &= a_tmp ^ b_tmp;
  *a ^= c_tmp;
  *b ^= c_tmp;
}

static void uint64_sort(uint64_t *arr, uint16_t arr_size) {
  size_t top = 1;
  while (top < arr_size) {
    top *= 2;
  }
  top /= 2;

  for (size_t p = top; p > 0; p >>= 1) {
    for (size_t i = 0; i < arr_size - p; ++i) {
      if (!(i & p)) {
        uint64_cmp(&arr[i], &arr[i + p]);
      }
    }
    size_t i = 0;
    for (size_t q = top; q > p; q >>= 1) {
      for (; i < arr_size - q; ++i) {
        if (!(i & p)) {
          uint64_t a = arr[i + p];
          for (size_t r = q; r > p; r >>= 1) {
            uint64_cmp(&a, &arr[i + r]);
          }
          arr[i + p] = a;
        }
      }
    }
  }
}

void shuffle(const uint32_t *p, uint16_t *pi, uint64_t *buf, size_t len) {
  const uint64_t mask = 0xFFFF;
  for (size_t i = 0; i < len; ++i) {
    buf[i] = p[i];
    buf[i] <<= 16;
    buf[i] |= pi[i];
  }

  uint64_sort(buf, len);

  for (size_t i = 0; i < len; i++) {
    pi[i] = buf[i] & mask;
  }
}

void gen_vector(uint16_t *s) {

  memset(s + W, 0, sizeof(uint16_t) * (N - W));
  for (size_t i = 0; i < W; ++i) {
    s[i] = 1;
  }

  uint32_t entropy[N];
  randombytes((uint8_t *)entropy, N * 4);

  uint64_t buf[N];
  shuffle(entropy, s, buf, N);
}
