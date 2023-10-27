/*
   This product is distributed under 2-term BSD-license terms

   Copyright (c) 2023, QApp. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met: 

   1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
   2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
