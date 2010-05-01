#ifndef __TESTGEN_H__
#define __TESTGEN_H__

#include <stdint.h>
#include <stdlib.h>

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

#ifdef __cplusplus
}
#endif

#endif /* __TESTGEN_H__ */

