#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"
#include "testgen.h"

struct testdata {
  uint8_t *src;
  size_t capa;
  size_t srcpos;
  size_t destpos;
  size_t size;
  uint8_t *expected;
};

static void **
datatestbitcpy()
{
  struct testdata **data;
  static size_t n = 10000, maxcapa = 1024;
  size_t i, j;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->capa = gencapa(maxcapa);
    data[i]->size = gensize(data[i]->capa);
    data[i]->srcpos = genpos(data[i]->capa, data[i]->size);
    data[i]->destpos = genpos(data[i]->capa, data[i]->size);
    data[i]->src = (uint8_t *)malloc(data[i]->capa);
    data[i]->expected = (uint8_t *)malloc(data[i]->capa);
    bitstdrand(data[i]->src, 0, data[i]->capa * 8);
    memcpy(data[i]->expected, data[i]->src, data[i]->capa);

    for (j = 0; j < data[i]->size; j++) {
      bitset(data[i]->expected, data[i]->destpos + j,
          bitget(data[i]->src, data[i]->srcpos + j));
    }
  }

  return (void **)data;
}

static void
freetestbitcpy(void *data)
{
  struct testdata *test;

  test = data;
  free(test->src);
}

static void
testbitcpy(void *data)
{
  struct testdata *test;
  uint8_t *buf;

  test = data;
  buf = (uint8_t *)malloc(test->capa);

  memcpy(buf, test->src, test->capa);
  bitcpy(buf, test->destpos, test->src, test->srcpos, test->size);
  testassert(biteq(buf, 0, test->expected, 0, test->capa * 8),
      "fail different buffer");

  memcpy(buf, test->src, test->capa);
  bitcpy(buf, test->destpos, buf, test->srcpos, test->size);
  testassert(biteq(buf, 0, test->expected, 0, test->capa * 8),
      "fail same buffer");

  free(buf);
  return;
}

void
inittestbitcpy()
{
  TESTADD(testbitcpy);
}

