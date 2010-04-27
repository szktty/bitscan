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

#ifdef __cplusplus
}
#endif

#endif /* __TESTGEN_H__ */

