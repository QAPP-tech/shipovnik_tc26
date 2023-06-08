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
