#include "multiword.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

multiword_number_t multiword_number_new(size_t capacity_words) {
  multiword_number_t number =
      (multiword_number_st *)malloc(sizeof(multiword_number_st));
  if (number == NULL) {
    return NULL;
  }

  number->capacity_words = capacity_words;
  number->size_words = 0;
  word_t *words = (word_t *)calloc(capacity_words, sizeof(word_t));
  if (words == NULL) {
    free(number);
    return NULL;
  }

  number->words = words;
  return number;
}

void multiword_number_free(multiword_number_t num) {
  if (num == NULL) {
    return;
  }
  free(num->words);
  free(num);
}

void multiword_number_set_word(multiword_number_t number, word_t value) {
  number->size_words = 1;
  number->words[0] = value;
}

uint8_t multiword_number_multiply_by_word(multiword_number_t num,
                                          word_t multiplier) {
  const size_t word_t_bits = sizeof(word_t) * 8;
  const dword_t mask_word = ((dword_t)1 << word_t_bits) - 1;

  const size_t size_words = num->size_words;
  dword_t carryover = 0;
  for (word_t *current = num->words; current < num->words + size_words;
       ++current) {
    const dword_t multiplication =
        (dword_t)(*current) * (dword_t)multiplier + carryover;
    carryover = multiplication >> word_t_bits;
    *current = multiplication & mask_word;
  }
  if (!carryover) {
    return 1;
  }

  if (size_words < num->capacity_words) {
    num->words[size_words] = carryover;
    num->size_words++;
    return 1;
  }

  // capacity exceeded
  return 0;
}

uint8_t multiword_number_div_3(const multiword_number_t divident,
                               multiword_number_t quotient, word_t *remainder) {
  const size_t word_t_bits = sizeof(word_t) * 8;

  // idea of fast algorithm of division is that instead of division
  // we can estimate quotient (upper bound) as:
  // (i) shift left N bits
  // (ii) divide by 3
  // (iii) shift right
  // The first to operations are equivalent to multiplication
  const word_t shift_left_div_3 = ((dword_t)1 << word_t_bits) / 3;
  const size_t size = divident->size_words;
  if (size == 0) {
    quotient->size_words = 0;
    *remainder = 0;
    return 1;
  }
  const word_t highest = divident->words[size - 1];
  if (highest >= 3) {
    quotient->size_words = size;
  } else {
    quotient->size_words = size - 1;
  }

  if (divident->capacity_words > quotient->capacity_words) {
    return 0;
  }

  word_t current_remainder = 0;
  for (size_t idx = size; idx-- > 0;) {
    const dword_t current_word = divident->words[idx];
    const dword_t current =
        ((dword_t)current_remainder << word_t_bits) | current_word;
    // estimation (!) for divident
    word_t current_quotient = (current * shift_left_div_3) >> word_t_bits;
    current_remainder = current - current_quotient * 3;

    // fix up estimation
    assert(current_remainder < 6);
    if (current_remainder >= 3) {
      current_remainder -= 3;
      current_quotient++;
    }
    quotient->words[idx] = current_quotient;
  }

  *remainder = current_remainder;
  return 1;
}

uint8_t multiword_number_copy(const multiword_number_t source,
                              multiword_number_t dest) {
  const size_t size = source->size_words;
  if (size > dest->capacity_words) {
    return 0;
  }
  dest->size_words = size;
  memcpy(dest->words, source->words, size * sizeof(word_t));

  return 1;
}
