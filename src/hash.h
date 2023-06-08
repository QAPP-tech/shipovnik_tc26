#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Calculates the Streebog-512F has of given message
 * @param[in] buf Message whose hash is to be calculated.
 * @param[in] len Message length.
 * @param[out] result Output buffer. Should have size at least
 * `GOST512_OUTPUT_BYTES`.
 */
void streebog_512_f(const uint8_t *buf, size_t len, uint8_t *result);
