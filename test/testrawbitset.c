#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

struct testdata {
  size_t capa;
  size_t index;
};

static void **
datatestrawbitset()
{
  struct testdata **data;
  static size_t n = 10000, maxcapa = 1024;
  size_t i;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->capa = gencapa(maxcapa);
    data[i]->index = gensize(data[i]->capa);
  }

  return (void **)data;
}

static void
freetestrawbitset(void *data)
{
  /* do nothing */
}

static void
testrawbitset(void *data)
{
  struct testdata *test;
  uint8_t *buf;

  test = data;
  buf = (uint8_t *)malloc(test->capa);
  memset(buf, 0, test->capa);
  testassert(!rawbitget(buf, test->index), "buffer is not initialized");
  rawbitset(buf, test->index, true);
  testassert(rawbitget(buf, test->index), "bit is not set");

  free(buf);
}

void
inittestrawbitset()
{
  TESTADD(testrawbitset);
}

