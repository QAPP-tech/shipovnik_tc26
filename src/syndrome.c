#include "syndrome.h"
#include "params.h"

#include <string.h>

#define N_BYTES ((N + 7) / 8)
#define K_BYTES ((K + 7) / 8)
#define PRIME_ROW_BYTES (N_BYTES - K_BYTES)

static inline uint8_t dot_product(const uint8_t *l, const uint8_t *r,
                                  size_t len) {
  // wordwise dot product
  const uint64_t *ll = (const uint64_t *)l;
  const uint64_t *lr = (const uint64_t *)r;
  uint64_t w = 0;
  const size_t nwords = len / sizeof(uint64_t);
  for (size_t i = 0; i < nwords; ++i) {
    w ^= ll[i] & lr[i];
  }

  // bytewise dot product
  const size_t start = nwords * sizeof(uint64_t);
  uint8_t b = 0;
  for (size_t i = 0; i < len % sizeof(uint64_t); ++i) {
    b ^= l[start + i] & r[start + i];
  }

  const uint8_t *p = (const uint8_t *)&w;
  b ^= p[0] ^ p[1] ^ p[2] ^ p[3] ^ p[4] ^ p[5] ^ p[6] ^ p[7];

  // finally, xor byte bits, e.g. xor(11011110) = 0, xor(11011111) = 1
  b ^= b >> 4;
  b ^= b >> 2;
  b ^= b >> 1;
  b &= 1;

  return b;
}

void syndrome(const uint8_t *H_prime, const uint8_t *sk, uint8_t *pk) {

  uint8_t row[N_BYTES];
  memset(row + PRIME_ROW_BYTES, 0, K_BYTES);

  memset(pk, 0, K_BYTES);

  for (uint32_t i = 0; i < K; ++i) {
    // get next row of H'
    memcpy(row, H_prime, PRIME_ROW_BYTES);
    // set bit in identity row
    row[PRIME_ROW_BYTES + i / 8] = 1 << (7 - i % 8);

    uint8_t b = dot_product(row, sk, N_BYTES);

    // unset identity bit
    row[PRIME_ROW_BYTES + i / 8] = 0;
    // set i-th bit in pubkey starting from left
    pk[i / 8] |= b << (7 - i % 8);
    // move to next row
    H_prime += PRIME_ROW_BYTES;
  }
}
