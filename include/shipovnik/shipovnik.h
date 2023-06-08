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
