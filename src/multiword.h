#pragma once

#include <stddef.h>
#include <stdint.h>

typedef uint32_t word_t;
typedef uint64_t dword_t;

/**
 * @brief represents big unsigned integer
 */
typedef struct multiword_number_st {
  size_t capacity_words;
  size_t size_words;
  word_t *words;
} multiword_number_st;

typedef multiword_number_st *multiword_number_t;

/**
 * @brief allocate new big number
 *
 * @param[in] capacity_words specifies how many 32-bit words has to be allocated
 */
multiword_number_t multiword_number_new(size_t capacity_words);

/**
 * @brief free memory allocated for a given big number
 */
void multiword_number_free(multiword_number_t number);

/**
 * @brief assigns a 32bit value to a given big number
 */
void multiword_number_set_word(multiword_number_t number, word_t value);

/**
 * @brief multiplies a given big number by a 32bit integer
 */
uint8_t multiword_number_multiply_by_word(multiword_number_t number,
                                          word_t multiplier);

/**
 * @brief divide a given big number by 3
 * @param[in] divident number to divide
 * @param[out] quotient result of division
 * @param[out] remainder remainder of division
 */
uint8_t multiword_number_div_3(const multiword_number_t divident,
                               multiword_number_t quotient, word_t *remainder);

/**
 * @brief assigns value of a source number to destination number
 *
 * @param[in] source number with a value to assign
 * @param[out] dest number to assign the value to
 */
uint8_t multiword_number_copy(const multiword_number_t source,
                              multiword_number_t dest);
