#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

struct testdata {
  size_t capa;
  uint8_t *bytes1;
  uint8_t *bytes2;
  size_t pos1;
  size_t pos2;
  size_t size;
  uint8_t *expected;
  size_t expos;
};

static void **
datatestrawbitand()
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
    data[i]->pos1 = genpos(data[i]->capa, data[i]->size);
    data[i]->pos2 = genpos(data[i]->capa, data[i]->size);
    data[i]->expos = genpos(data[i]->capa, data[i]->size);
    data[i]->bytes1 = (uint8_t *)malloc(data[i]->capa);
    data[i]->bytes2 = (uint8_t *)malloc(data[i]->capa);
    data[i]->expected = (uint8_t *)malloc(data[i]->capa);

    memset(data[i]->bytes1, 0, data[i]->capa);
    rawbitstdrand(data[i]->bytes2, 0, data[i]->capa * 8);
    memset(data[i]->expected, 0, data[i]->capa);

    for (j = 0; j < data[i]->size; j++) {
      rawbitset(data[i]->expected, data[i]->expos + j,
          rawbitget(data[i]->bytes1, data[i]->pos1 + j) &
          rawbitget(data[i]->bytes2, data[i]->pos2 + j));
    }
  }

  return (void **)data;
}

static void
freetestrawbitand(void *data)
{
  struct testdata *test;

  test = data;
  free(test->bytes1);
  free(test->bytes2);
  free(test->expected);
}

static void
testrawbitand(void *data)
{
  struct testdata *test;
  uint8_t *buf;
  size_t i;

  test = data;
  buf = (uint8_t *)malloc(test->capa);
  memset(buf, 0, test->capa);
  rawbitand(buf, test->expos, test->bytes1, test->pos1,
      test->bytes2, test->pos2, test->size);
  testassert(rawbiteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed");
  free(buf);
}

void
inittestrawbitop()
{
  TESTADD(testrawbitand);
}

