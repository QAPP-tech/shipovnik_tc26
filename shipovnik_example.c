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

#include "shipovnik.h"

#include <stdio.h>

void dump_bstr(uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    printf("%02X", data[i]);
  }
  puts("");
}

int main(int argc, char *argv[]) {
  uint8_t sk[SHIPOVNIK_SECRETKEYBYTES];
  uint8_t pk[SHIPOVNIK_PUBLICKEYBYTES];

  shipovnik_generate_keys(sk, pk);
  puts("SECRET KEY");
  dump_bstr(sk, SHIPOVNIK_SECRETKEYBYTES);

  puts("PUBLIC KEY");
  dump_bstr(pk, SHIPOVNIK_PUBLICKEYBYTES);

  uint8_t msg[] = "test test test";
  puts("MESSAGE");
  dump_bstr(msg, sizeof(msg));

  uint8_t sig[SHIPOVNIK_SIGBYTES];
  size_t sig_len;
  shipovnik_sign(sk, msg, sizeof(msg) - 1, sig, &sig_len);

  puts("SIGNATURE");
  dump_bstr(sig, sig_len);

  int ret = shipovnik_verify(pk, sig, msg, sizeof(msg) - 1);

  if (ret == 0) {
    puts("VERIFY: SUCCESS");
  } else {
    puts("VERIFY: FAILURE");
  }
}
