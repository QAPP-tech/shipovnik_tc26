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

#include "shipovnik.h"
#include "genvector.h"
#include "hash.h"
#include "params.h"
#include "randombytes.h"
#include "sign.h"
#include "syndrome.h"
#include "utils.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void shipovnik_generate_keys(uint8_t *sk, uint8_t *pk) {
  ALLOC_ON_STACK(uint16_t, s, N);
  gen_vector(s);
  for (size_t i = 0; i < N; ++i) {
    size_t j = i / 8;
    sk[j] <<= 1;
    sk[j] |= s[i] & 1;
  }
  syndrome(H_PRIME, sk, pk);
}

#define SIGMA_Y_SIZE (SIGMA_PACKED_BYTES + SHIPOVNIK_PUBLICKEYBYTES)

void shipovnik_sign(const uint8_t *sk, const uint8_t *msg, size_t msg_len,
                    uint8_t *sig, size_t *sig_len) {

  // temporary buffers
  ALLOC_ON_STACK(uint64_t, shuf64_, N);
  ALLOC_ON_STACK(uint32_t, entropy32_, N);
  ALLOC_ON_STACK(uint8_t, sigma_y_, SIGMA_Y_SIZE);
  ALLOC_ON_STACK(uint8_t, u1_, SHIPOVNIK_SECRETKEYBYTES);
  ALLOC_ON_STACK(uint8_t, u2_, SHIPOVNIK_SECRETKEYBYTES);

  ALLOC_ON_STACK(uint8_t, h, GOST512_OUTPUT_BYTES);
  ALLOC_ON_STACK(uint8_t, b, DELTA);

  // array of random bit vectors (u)
  uint8_t *const us = malloc(DELTA * SHIPOVNIK_SECRETKEYBYTES);
  // array of permutation indices (sigma)
  uint16_t *const sigmas = malloc(DELTA * SIGMA_BYTES);

  for (size_t i = 0; i < DELTA; i++) {
    uint8_t *u = us + i * SHIPOVNIK_SECRETKEYBYTES;
    uint16_t *sigma = sigmas + i * N;
    for (uint16_t j = 0; j < N; ++j) {
      sigma[j] = j; // init indices
    }

    /* Step 2 */
    uint8_t *ebytes = (uint8_t *)entropy32_;
    randombytes(u, SHIPOVNIK_SECRETKEYBYTES);
    randombytes(ebytes, N * sizeof(uint32_t));
    // random shuffle permutation indices
    shuffle(entropy32_, sigma, shuf64_, N);

    /* Step 3 */
    uint8_t *ci = sig + i * 3 * GOST512_OUTPUT_BYTES;

    // sigma_k_ = sigma || H*u
    pack_sigma(sigma, N, sigma_y_);
    uint8_t *y = sigma_y_ + SIGMA_PACKED_BYTES;
    syndrome(H_PRIME, u, y);
    streebog_512_f(sigma_y_, SIGMA_Y_SIZE, ci); // ci0

    apply_permutation(sigma, u, u1_, N); // u1_ = sigma(u)
    streebog_512_f(u1_, SHIPOVNIK_SECRETKEYBYTES,
                   ci + GOST512_OUTPUT_BYTES); // ci1

    bitwise_xor(u, sk, SHIPOVNIK_SECRETKEYBYTES, u1_); // u1_ = u xor sk
    apply_permutation(sigma, u1_, u2_, N);             // u2_ = sigma(u1_)
    streebog_512_f(u2_, SHIPOVNIK_SECRETKEYBYTES,
                   ci + 2 * GOST512_OUTPUT_BYTES); // ci2
  }

  /* Step 5 */
  uint8_t *const msg_cs = malloc(msg_len + CS_BYTES);
  memcpy(msg_cs, msg, msg_len);
  memcpy(msg_cs + msg_len, sig, CS_BYTES);
  streebog_512_f(msg_cs, msg_len + CS_BYTES, h);
  free(msg_cs);

  /* Step 6 */
  const multiword_number_t mwh = h_3_delta_shift(h, GOST512_OUTPUT_BYTES);
  if (NULL == mwh) {
    goto cleanup;
  }

  /* Step 7 */
  if (h_to_ternary_vec(mwh, b, DELTA)) {
    goto cleanup;
  }

  /* Step 8 */
  uint8_t *rs = sig + CS_BYTES;
  *sig_len = CS_BYTES;
  for (size_t i = 0; i < DELTA; i++) {
    uint8_t *u = us + i * SHIPOVNIK_SECRETKEYBYTES;
    uint16_t *sigma = sigmas + i * N;
    switch (b[i]) {
    case 0: // sigma_i || u_i
      pack_sigma(sigma, N, rs);
      rs += SIGMA_PACKED_BYTES;
      memcpy(rs, u, SHIPOVNIK_SECRETKEYBYTES);
      rs += SHIPOVNIK_SECRETKEYBYTES;
      *sig_len += SIGMA_PACKED_BYTES + SHIPOVNIK_SECRETKEYBYTES;
      break;
    case 1: // sigma_i || (u_i xor s)
      pack_sigma(sigma, N, rs);
      rs += SIGMA_PACKED_BYTES;
      // u xor sk
      bitwise_xor(u, sk, SHIPOVNIK_SECRETKEYBYTES, rs);
      rs += SHIPOVNIK_SECRETKEYBYTES;
      *sig_len += SIGMA_PACKED_BYTES + SHIPOVNIK_SECRETKEYBYTES;
      break;
    case 2: // sigma_i(u_i) || sigma_i(s)
      apply_permutation(sigma, u, rs, N);
      rs += SHIPOVNIK_SECRETKEYBYTES;
      apply_permutation(sigma, sk, rs, N);
      rs += SHIPOVNIK_SECRETKEYBYTES;
      *sig_len += 2 * SHIPOVNIK_SECRETKEYBYTES;
      break;
    default:
      goto cleanup;
    }
  }

cleanup:
  free(us);
  free(sigmas);
  multiword_number_free(mwh);
}

int shipovnik_verify(const uint8_t *pk, const uint8_t *sig, const uint8_t *msg,
                     size_t msg_len) {

  ALLOC_ON_STACK(uint8_t, h, GOST512_OUTPUT_BYTES); // hash_f(buff_MC)
  ALLOC_ON_STACK(uint8_t, b, DELTA);                // b
  ALLOC_ON_STACK(uint16_t, sigma, N);

  ALLOC_ON_STACK(uint8_t, sigma_y_, SIGMA_Y_SIZE)         // uint16_t buffer
  ALLOC_ON_STACK(uint8_t, u_1, SHIPOVNIK_SECRETKEYBYTES); // buffer
  ALLOC_ON_STACK(uint8_t, cij_, GOST512_OUTPUT_BYTES);    // сi0_, ci1_, ci2_

  // buf of M||C...
  const size_t c_border = CS_BYTES; // 3 * delta * GOST512_OUTPUT_BYTES
  uint8_t *buff_mc = malloc(sizeof(uint8_t) * (msg_len + c_border));

  memcpy(buff_mc, msg, msg_len);            // copy M
  memcpy(buff_mc + msg_len, sig, c_border); // copy C

  // step 1
  streebog_512_f(buff_mc, msg_len + c_border, h);

  // step 2
  int ret = 0;
  multiword_number_t mwh = h_3_delta_shift(h, GOST512_OUTPUT_BYTES);
  if (NULL == mwh) {
    ret = 1;
    goto cleanup;
  }

  // step 3
  if (h_to_ternary_vec(mwh, b, DELTA)) {
    ret = 1;
    goto cleanup;
  }

  const uint8_t *ci0_true = NULL, *ci1_true = NULL, *ci2_true = NULL;
  const uint8_t *ri0 = NULL, *ri1 = NULL;
  const uint8_t *ci = sig;
  const uint8_t *ri = sig + c_border;
  size_t weight = 0;                   // weight of vector
  for (size_t i = 0; i < DELTA; i++) { // step 4
    // step 5
    ci0_true = ci;
    ci1_true = ci + GOST512_OUTPUT_BYTES;
    ci2_true = ci + 2 * GOST512_OUTPUT_BYTES;
    ci += 3 * GOST512_OUTPUT_BYTES;
    switch (b[i]) { // step 5.1
    case 0: {
      ri0 = ri;
      ri1 = ri + SIGMA_PACKED_BYTES;
      ri += SIGMA_PACKED_BYTES + SHIPOVNIK_SECRETKEYBYTES;

      // calculate ci0_
      memcpy(sigma_y_, ri0, SIGMA_PACKED_BYTES);
      syndrome(H_PRIME, ri1, sigma_y_ + SIGMA_PACKED_BYTES);
      streebog_512_f(sigma_y_, SIGMA_Y_SIZE, cij_);
      if (memcmp(ci0_true, cij_, GOST512_OUTPUT_BYTES)) {
        ret = 1;
        goto cleanup;
      }

      // calculate ci1_
      if (unpack_sigma(ri0, SIGMA_PACKED_BYTES, sigma) != 0) {
        ret = 1;
        goto cleanup;
      }
      apply_permutation(sigma, ri1, u_1, N);
      streebog_512_f(u_1, SHIPOVNIK_SECRETKEYBYTES, cij_);

      if (memcmp(ci1_true, cij_, GOST512_OUTPUT_BYTES)) {
        ret = 1;
        goto cleanup;
      }

      break;
    }
    case 1: { // step 5.2
      ri0 = ri;
      ri1 = ri + SIGMA_PACKED_BYTES;
      ri += SIGMA_PACKED_BYTES + SHIPOVNIK_SECRETKEYBYTES;

      // calculate ci0_
      memcpy(sigma_y_, ri0, SIGMA_PACKED_BYTES);
      uint8_t *y = sigma_y_ + SIGMA_PACKED_BYTES;
      syndrome(H_PRIME, ri1, y);
      bitwise_xor(y, pk, SHIPOVNIK_PUBLICKEYBYTES, y);
      streebog_512_f(sigma_y_, SIGMA_Y_SIZE, cij_);
      if (memcmp(ci0_true, cij_, GOST512_OUTPUT_BYTES)) {
        ret = 1;
        goto cleanup;
      }

      // calculate ci2_
      if (unpack_sigma(ri0, SIGMA_PACKED_BYTES, sigma) != 0) {
        ret = 1;
        goto cleanup;
      }
      apply_permutation(sigma, ri1, u_1, N);
      streebog_512_f(u_1, SHIPOVNIK_SECRETKEYBYTES, cij_);

      if (memcmp(ci2_true, cij_, GOST512_OUTPUT_BYTES)) {
        ret = 1;
        goto cleanup;
      }

      break;
    }
    case 2: { // step 5.3
      ri0 = ri;
      ri1 = ri + SHIPOVNIK_SECRETKEYBYTES;
      ri += 2 * SHIPOVNIK_SECRETKEYBYTES;

      // calculate ci1_
      memcpy(u_1, ri0, SHIPOVNIK_SECRETKEYBYTES);
      streebog_512_f(u_1, SHIPOVNIK_SECRETKEYBYTES, cij_);
      if (memcmp(ci1_true, cij_, GOST512_OUTPUT_BYTES)) {
        ret = 1;
        goto cleanup;
      }

      // calculate ci2_
      bitwise_xor(ri0, ri1, SHIPOVNIK_SECRETKEYBYTES, u_1);
      streebog_512_f(u_1, SHIPOVNIK_SECRETKEYBYTES, cij_);
      if (memcmp(ci2_true, cij_, GOST512_OUTPUT_BYTES)) {
        ret = 1;
        goto cleanup;
      }

      count_bits(ri1, SHIPOVNIK_SECRETKEYBYTES, &weight);
      if (W != weight) {
        ret = 1;
        goto cleanup;
      }

      break;
    }
    default: {
      ret = 1;
      goto cleanup;
    }
    }
  }

cleanup:
  free(buff_mc);
  multiword_number_free(mwh);
  return ret;
}
