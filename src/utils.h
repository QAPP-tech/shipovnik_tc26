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

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Computes the bitwise XOR for given arrays
 * @param[in] x First input array.
 * @param[in] y Second input array.
 * @param[in] len Arrays size.
 * @param[out] result Output array.
 */
void bitwise_xor(const uint8_t *x, const uint8_t *y, uint32_t len,
                 uint8_t *result);

/**
 * @brief Reverses the order of elements in given array
 * @param[in] in_buf Input array.
 * @param[in] len input array length.
 * @param[out] out_buf Output array.
 */
void reverse(const uint8_t *in_buf, size_t len, uint8_t *out_buf);

/**
 * @brief Reverses the order of elements in given array in-place
 * @param[in,out] buf Input array.
 * @param[in] len input array length.
 */
void reverse_inplace(uint8_t *buf, size_t len);

/**
 * @brief Count number of bits set in a byte array src
 * @param[in] src - vector of bits
 * @param[in] len - size of vector
 * @param[out] result - result
 * @return[out] 0 if success otherwise 1
 */
int count_bits(const uint8_t *src, size_t len, size_t *result);

/**
 * @brief Securely erases the contests of given buffer
 * @param[in,out] buf Buffer to be erased.
 * @param[in] len Buffer size.
 */
void secure_erase(void *buf, size_t len);
#define SECURE_ERASE(type, name, len) secure_erase(name, (len) * sizeof(type));

#ifdef WIN32
#include <malloc.h>
// WARNING: Do not use it within loops! It can be used only within function
// scope.
#define ALLOC_ON_STACK(type, name, len)                                        \
  type *name = (type *)_alloca((len) * sizeof(type));
#else // WIN32
// WARNING: Do not use it within loops! It can be used only within function
// scope.
#define ALLOC_ON_STACK(type, name, len) type name[len];
#endif // WIN32
