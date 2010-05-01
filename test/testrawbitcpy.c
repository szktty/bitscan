#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

struct testdata {
  uint8_t *src;
  size_t capa;
  size_t srcpos;
  size_t destpos;
  size_t size;
};

static void **
datatestrawbitcpy()
{
  struct testdata **data;
  static size_t n = 10000, maxcapa = 1024;
  size_t i;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->capa = gencapa(maxcapa);
    data[i]->size = gensize(data[i]->capa);
    data[i]->srcpos = genpos(data[i]->capa, data[i]->size);
    data[i]->destpos = genpos(data[i]->capa, data[i]->size);
    data[i]->src = (uint8_t *)malloc(data[i]->capa);
    rawbitstdrand(data[i]->src, data[i]->srcpos, data[i]->size);
  }

  return (void **)data;
}

static void
freetestrawbitcpy(void *data)
{
  struct testdata *test;

  test = data;
  free(test->src);
}

static void
testrawbitcpy(void *data)
{
  struct testdata *test;
  uint8_t *buf;
  size_t i;

  test = data;
  buf = (uint8_t *)malloc(test->capa);
  memset(buf, 0, test->capa);
  rawbitcpy(buf, test->destpos, test->src, test->srcpos, test->size);

  for (i = 0; i < test->destpos; i++) {
    if (rawbitget(buf, i)) {
      testfail("bits between 0 and pos are modified");
      goto error;
    }
  }

  testassert(rawbiteq(test->src, test->srcpos,
        buf, test->destpos, test->size),
      "copied bits are wrong");

  for (i = test->destpos + test->size; i < test->capa * 8; i++) {
    if (rawbitget(buf, i)) {
      testfail("rest bits are copied");
      goto error;
    }
  }

  free(buf);
  testassert(true, NULL);
  return;

error:
  free(buf);
}

void
inittestrawbitcpy()
{
  TESTADD(testrawbitcpy);
}

