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
#include "utils.h"

void bitwise_xor(const uint8_t *x, const uint8_t *y, uint32_t len,
                 uint8_t *result) {
  if (len == 0) {
    return;
  }

  for (size_t i = 0; i < len; i++) {
    result[i] = x[i] ^ y[i];
  }
}

void reverse_inplace(uint8_t *buf, size_t len) {
  uint8_t tmp;
  size_t i, j;

  for (i = 0, j = len - 1; i < j; i++, j--) {
    tmp = buf[i];
    buf[i] = buf[j];
    buf[j] = tmp;
  }
}

void reverse(const uint8_t *in_buf, size_t len, uint8_t *out_buf) {
  // copy
  for (size_t i = 0; i < len; i++) {
    out_buf[len - i - 1] = in_buf[i];
  }
}

static inline uint8_t count_ones(uint8_t byte) {
  static const uint8_t nibble_lookup[16] = {0, 1, 1, 2, 1, 2, 2, 3,
                                            1, 2, 2, 3, 2, 3, 3, 4};
  return nibble_lookup[byte & 0x0F] + nibble_lookup[byte >> 4];
}

int count_bits(const uint8_t *src, size_t len, size_t *result) {
  if (src == NULL || len == 0) {
    return 1;
  }

  *result = 0;
  for (int i = 0; i < len; ++i) {
    *result += count_ones(src[i]);
  }

  return 0;
}

void secure_erase(void* buf, size_t len)
{
#if (__STDC_VERSION__ >= 201112L) && __STDC_LIB_EXT1__
    memset_s(buf, len, 0, len);
#else
    volatile uint8_t* p = buf;
    while (len--) {
        *p++ = 0;
    }
#endif  // (__STDC_VERSION__ >= 201112L) && __STDC_LIB_EXT1__
}
