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

#pragma once

#include "multiword.h"

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Pack permutation indices into bytes in big endian order, bit packing
 * width is 12. [0x0C1A, 0x02F9] -> [0xC1, 0xA2, 0xF9]
 * @param[in] in permutation indices
 * @param[in] in_len permutation indices length
 * @param[out] out output byte array
 * @return 0 if Ok, 1 if `in_len` is not even
 */
int pack_sigma(const uint16_t *in, size_t in_len, uint8_t *out);

/**
 * @brief Unpack permutation indices from bytes in big endian order, bit packing
 * width is 12. [0xC1, 0xA2, 0xF9] -> [0x0C1A, 0x02F9]
 * @param[in] in packed byte array
 * @param[in] in_len number of bytes in `in`
 * @param[out] out permutation indices
 * @return 0 if Ok, 1 if `in_len` is not divisible by 3
 */
int unpack_sigma(const uint8_t *in, size_t in_len, uint16_t *out);

/**
 * @brief Permutate bits according to indices.
 * @param[in] p permutation indices
 * @param[in] a bits to permute
 * @param[out] buf save permutation to `buf`
 * @param[in] len length of `p`
 */
void apply_permutation(const uint16_t *p, const uint8_t *a, uint8_t *buf,
                       size_t len);

/**
 * @brief Computes step 6 of Shipovnik sign algorithm
 * @param[in] h hash bytes, that are interpreted as a multiword number
 * @param[in] h_size size of h in bytes
 * @return (h * pow(3, delta)) >> 512
 */
multiword_number_t h_3_delta_shift(const uint8_t *h, size_t h_size);

/**
 * @brief Compute ternary representation of a multiword number. Computes step 7
 * of Shipovnik sign algorithm
 * @param[in,out] mwh hash as a multiword number
 * @param[out] b ternary representation of `h_prime`, e.g. [2,0,1,2,0,0,1...]
 * @param[in] b_size number of bytes in `b`
 * @return 0 if Ok, 1 if division error occurred
 */
int h_to_ternary_vec(multiword_number_t mwh, uint8_t *b, size_t b_size);
