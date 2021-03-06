#ifndef __TESTGEN_H__
#define __TESTGEN_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "bitscan.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bitsrc bitsrc;

struct bitsrc {
  uint8_t *bytes;
  size_t pos;
  size_t size;
};

extern size_t gencapa(size_t max);
extern size_t gensize(size_t capa);
extern size_t genpos(size_t capa, size_t size);
extern bool genbool();

#ifdef __cplusplus
}
#endif

#endif /* __TESTGEN_H__ */

