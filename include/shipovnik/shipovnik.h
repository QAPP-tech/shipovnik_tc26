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

#include "params.h"

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Generates random secret key and public key.
 * @param[out] sk Contiguous array to receive secret key, of size
 *   `SHIPOVNIK_SECRETKEYBYTES`.
 * @param[out] pk Contiguous array to receive public key, of size
 *   `SHIPOVNIK_PUBLICKEYBYTES`.
 */
void shipovnik_generate_keys(uint8_t *sk, uint8_t *pk);

/**
 * @brief Generates signature for given message according to secret key.
 *
 * @param[in] sk Secret key, the contiguous array of size
 *   `SHIPOVNIK_SECRETKEYBYTES`.
 * @param[in] msg Message to generate signature of, the contiguous array.
 * @param[in] msg_len The length of a message in bytes.
 * @param[out] sig Contiguous array to receive signature, of size
 *   `SHIPOVNIK_SIGBYTES`.
 * @param[out] sig_len The result signature size.
 */
void shipovnik_sign(const uint8_t *sk, const uint8_t *msg, size_t msg_len,
                    uint8_t *sig, size_t *sig_len);

/**
 * @brief Verifies that given signature is the signature of given message.
 *
 * @param[in] pk Public key, the contiguous array of size
 *   `SHIPOVNIK_PUBLICKEYBYTES`.
 * @param[in] sig Signature, the contiguous array of size
 *   `SHIPOVNIK_SIGBYTES'.
 * @param[in] msg Message to verify signature of, the contiguous array.
 * @param[in] msg_len The length of a message in bytes.
 * @return `0` if given signature is the signature of given message, otherwise
 *   non-zero value.
 */
int shipovnik_verify(const uint8_t *pk, const uint8_t *sig, const uint8_t *msg,
                     size_t msg_len);
