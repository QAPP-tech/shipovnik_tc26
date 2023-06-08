#pragma once

#include "stdint.h"

/**
 * @brief Compute pubkey from matrix H' and secret key.
 * @param[in] H_prime The H' matrix.
 * @param[in] sk Secret key of size `SHIPOVNIK_SECRETKEYBYTES`.
 * @param[out] pk Public key of size `SHIPOVNIK_PUBLICKEYBYTES`.
 */
void syndrome(const uint8_t *H_prime, const uint8_t *sk, uint8_t *pk);
