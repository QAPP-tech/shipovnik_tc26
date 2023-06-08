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
  shipovnik_sign(sk, msg, sizeof(msg), sig, &sig_len);

  puts("SIGNATURE");
  dump_bstr(sig, sig_len);

  int ret = shipovnik_verify(pk, sig, msg, sizeof(msg));
  if (ret == 0) {
    puts("VERIFY: SUCCESS");
  } else {
    puts("VERIFY: FAILURE");
  }
}
