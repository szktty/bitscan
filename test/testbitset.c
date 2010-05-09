#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"
#include "testgen.h"

struct testdata {
  size_t capa;
  size_t pos;
  size_t size;
  uint8_t byte;
};

static void **
datatestbitset()
{
  struct testdata **data;
  static size_t n = 10000, maxcapa = 1024;
  size_t i;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->capa = gencapa(maxcapa);
    data[i]->pos = gensize(data[i]->capa);
  }

  return (void **)data;
}

static void
freetestbitset(void *data)
{
  /* do nothing */
}

static void
testbitset(void *data)
{
  struct testdata *test;
  uint8_t *buf;

  test = data;
  buf = (uint8_t *)malloc(test->capa);
  memset(buf, 0, test->capa);
  testassert(!bitget(buf, test->pos), "buffer is not initialized");
  bitset(buf, test->pos, true);
  testassert(bitget(buf, test->pos), "bit is not set");

  free(buf);
}

static void **
datatestbitsets()
{
  struct testdata **data;
  static size_t n = 10000, maxcapa = 1024;
  size_t i;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->capa = gencapa(maxcapa);
    data[i]->size = gensize(data[i]->capa) / 8;
    data[i]->pos = genpos(data[i]->capa, data[i]->size * 8);
    data[i]->byte = (uint8_t)(abs(random()) % 256);
  }

  return (void **)data;
}

static void
freetestbitsets(void *data)
{
  /* do nothing */
}

static void
testbitsets(void *data)
{
  struct testdata *test;
  uint8_t *buf;
  size_t i;

  test = data;
  buf = (uint8_t *)malloc(test->capa);
  memset(buf, 0, test->capa);
  bitsets(buf, test->pos, test->byte, test->size); 

  for (i = test->pos; i < (test->pos + test->size * 8); i += 8) {
    if (!((bitget(buf, i) == ((test->byte >> 7) & 1)) &&
          (bitget(buf, i+1) == ((test->byte >> 6) & 1)) &&
          (bitget(buf, i+2) == ((test->byte >> 5) & 1)) &&
          (bitget(buf, i+3) == ((test->byte >> 4) & 1)) &&
          (bitget(buf, i+4) == ((test->byte >> 3) & 1)) &&
          (bitget(buf, i+5) == ((test->byte >> 2) & 1)) &&
          (bitget(buf, i+6) == ((test->byte >> 1) & 1)) &&
          (bitget(buf, i+7) == (test->byte & 1)))) {
      testfail("wrong bytes");
      break;
    }
  }

  free(buf);
}

void
inittestbitset()
{
  TESTADD(testbitset);
  TESTADD(testbitsets);
}

