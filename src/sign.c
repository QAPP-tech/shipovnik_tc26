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

#include "sign.h"
#include "multiword.h"
#include "params.h"
#include <string.h>

int pack_sigma(const uint16_t *in, size_t in_len, uint8_t *out) {
  if (in_len % 2) {
    return 1;
  }

  for (size_t i = 0; i < in_len; ++i) {
    if (i % 2 == 0) {
      *out++ = in[i] >> 4;
      *out++ = (in[i] << 4) | (in[i + 1] >> 8);
    } else {
      *out++ = in[i];
    }
  }

  return 0;
}

int unpack_sigma(const uint8_t *in, size_t in_len, uint16_t *out) {
  if (in_len % 3) {
    return 1;
  }

  for (size_t i = 0; i < in_len; i += 3) {
    *out++ = (in[i] << 4) | (in[i + 1] >> 4);
    *out++ = ((in[i + 1] & 0xF) << 8) | in[i + 2];
  }

  return 0;
}

void apply_permutation(const uint16_t *p, const uint8_t *a, uint8_t *buf,
                       size_t len) {
  // iter bits from left to right
  for (size_t i = 0; i < len; ++i) {
    uint16_t j = p[i];
    uint8_t a_pos = 7 - j % 8;                        // j-th bit from left
    uint8_t bit = (a[j / 8] & (1 << a_pos)) >> a_pos; // take bit at j

    uint8_t *pbuf = buf + i / 8;
    uint8_t buf_pos = 7 - (i % 8); // i-th bit from left

    *pbuf &= ~(1 << buf_pos); // clear i-th bit
    *pbuf |= bit << buf_pos;  // set j-th bit of `a` to i-th bit of `buf`
  }
}

// h' = (h * pow(3, delta)) >> 512
multiword_number_t h_3_delta_shift(const uint8_t *h, size_t h_size) {

  // log(3**219, 2)/32 = 219*log(3, 2)/32
  const size_t cap3 = DELTA * 2 / 32;
  const size_t capacity = h_size / sizeof(word_t) + cap3;
  const multiword_number_t h1 = multiword_number_new(capacity);
  if (NULL == h1) {
    return NULL;
  }
  h1->size_words = h_size / sizeof(word_t);

  // initialize h_prime
  for (size_t i = 0; i < h_size; i += sizeof(uint32_t)) {
    // read uint32_t as big endian
    word_t w = (h[i] << 24) | (h[i + 1] << 16) | (h[i + 2] << 8) | h[i + 3];
    size_t j = h1->size_words - 1 - i / sizeof(uint32_t);
    h1->words[j] = w;
  }

  // max power of 3 that is less than UINT32_MAX: pow(3, 20)
  const uint32_t max_pow_3 = 3486784401;

  // h_prime * pow(3, 200)
  for (size_t i = 0; i < DELTA / 20; ++i) {
    if (!multiword_number_multiply_by_word(h1, max_pow_3)) {
      multiword_number_free(h1);
      return NULL;
    }
  }
  // pow(3, 19)
  size_t leftover = 1;
  for (size_t i = 0; i < DELTA % 20; ++i) {
    leftover *= 3;
  }
  // h_prime * pow(3, 19)
  if (!multiword_number_multiply_by_word(h1, leftover)) {
    multiword_number_free(h1);
    return NULL;
  }

#define MIN(a, b) ((a) < (b) ? (a) : (b))

  // h' >> 512
  const size_t shift = 512 / (8 * sizeof(word_t));
  for (size_t i = 0; shift + i < MIN(2 * shift, h1->size_words); ++i) {
    h1->words[i] = h1->words[shift + i];
  }

  if (h1->size_words < shift) {
    h1->size_words = 0;
  } else {
    h1->size_words -= shift;
  }

  return h1;
}

int h_to_ternary_vec(multiword_number_t mwh, uint8_t *b, size_t b_size) {

  memset(b, 0, b_size);

  // continuously divide h_prime by 3 and save remainders
  const multiword_number_t h_mate = multiword_number_new(mwh->capacity_words);
  if (NULL == h_mate) {
    return 1;
  }
  multiword_number_t divident = mwh;
  multiword_number_t quotient = h_mate;
  size_t i = b_size - 1;
  while (divident->size_words > 1 ||
         (divident->size_words != 0 && divident->words[0] > 0)) {
    uint32_t rem = 0;
    if (!multiword_number_div_3(divident, quotient, &rem)) {
      multiword_number_free(h_mate);
      return 1;
    }
    // swap quotient and divident
    multiword_number_t temp = divident;
    divident = quotient;
    quotient = temp;
    // save remainder
    b[i--] = rem;
  };

  multiword_number_free(h_mate);

  return 0;
}
