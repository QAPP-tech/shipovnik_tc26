#include "randombytes.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

void randombytes(uint8_t *out, size_t outlen) {
  static int fd = -1;
  ssize_t ret;

  while (fd == -1) {
    fd = open(ENTROPY_SOURCE, O_RDONLY);
    if (fd == -1 && errno == EINTR)
      continue;
    else if (fd == -1)
      abort();
  }

  while (outlen > 0) {
    ret = read(fd, out, outlen);
    if (ret == -1 && errno == EINTR)
      continue;
    else if (ret == -1)
      abort();

    out += ret;
    outlen -= ret;
  }
}
