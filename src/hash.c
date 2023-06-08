#include "hash.h"
#include "utils.h"

#include "gost3411-2012-core.h"

#include <stdlib.h>

static GOST34112012Context *
CTX(unsigned char data[sizeof(GOST34112012Context) + 16]) {
  uintptr_t ptr = (uintptr_t) & (data[0]);
  if (ptr % 0x10 == 0x00) {
    return (GOST34112012Context *)ptr;
  }
  ptr += 0x10 - (ptr % 0x10);
  return (GOST34112012Context *)ptr;
}

static void streebog_digest_f(const uint8_t *buf, size_t len, uint8_t *result,
                              unsigned int digest_size) {
  unsigned char *data = malloc(sizeof(GOST34112012Context) + 16);
  GOST34112012Context *ctx = CTX(data);

  GOST34112012Init(ctx, digest_size);

  if (digest_size == 256) {
    ALLOC_ON_STACK(uint8_t, reversed_buf, len);

    reverse(buf, len, reversed_buf);
    GOST34112012Update(ctx, reversed_buf, len);

    // secure sensitive data
    SECURE_ERASE(uint8_t, reversed_buf, len);

    GOST34112012Final(ctx, result);
    GOST34112012Cleanup(ctx);
    reverse_inplace(result, digest_size / 8);
  } else { // 512
    GOST34112012Update(ctx, buf, len);

    GOST34112012Final(ctx, result);
    GOST34112012Cleanup(ctx);
  }

  free(data);
}

void streebog_512_f(const uint8_t *buf, size_t len, uint8_t *result) {
  streebog_digest_f(buf, len, result, 512);
}
