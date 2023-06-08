#pragma once

#include <stdint.h>

// main params

#define N 2896
#define K 1448
#define W 318
#define DELTA 219

#define GOST512_OUTPUT_BYTES 64

#define H_PRIME_SIZE 262088
// H' bit matrix [k x (n-k)] (rowwise)
extern const uint8_t H_PRIME[H_PRIME_SIZE];

// derived params

#define SHIPOVNIK_PUBLICKEYBYTES ((N - K) / 8)
#define SHIPOVNIK_SECRETKEYBYTES (N / 8)

#define CS_BYTES (DELTA * 3 * GOST512_OUTPUT_BYTES)
#define SIGMA_BIT_WIDTH 12
#define SIGMA_BYTES (N * sizeof(uint16_t))
#define SIGMA_PACKED_BYTES (SIGMA_BIT_WIDTH * N / 8)

#define SHIPOVNIK_SIGBYTES                                                           \
  (CS_BYTES + DELTA * (SIGMA_PACKED_BYTES + SHIPOVNIK_SECRETKEYBYTES))
