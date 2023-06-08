#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Shuffles `pi` using entropy from `p`
 * @param[in] p random array of size `len`
 * @param[in,out] pi elements to be shuffled of size `len`
 * @param[in,out] buf temporary buffer of size `len`
 */
void shuffle(const uint32_t *p, uint16_t *pi, uint64_t *buf, size_t len);

/**
 * @brief Generate random binary vector.
 * @param[out] s buffer to be filled. Should have size at least 'N'.
 */
void gen_vector(uint16_t *s);
