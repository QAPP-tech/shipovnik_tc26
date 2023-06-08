#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Fills buffer with random data from defined source
 * @param[out] out Buffer to be filled.
 * @param[in] outlen Buffer length
 */
void randombytes(uint8_t *out, size_t outlen);
