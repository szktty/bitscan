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
};

static void **
datatestrawbitclear()
{
  struct testdata **data;
  static size_t n = 10000, maxcapa = 512;
  size_t i;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->capa = gencapa(maxcapa);
    data[i]->size = gensize(data[i]->capa);
    data[i]->pos = genpos(data[i]->capa, data[i]->size);
  }

  return (void **)data;
}

static void
freetestrawbitclear(void *data)
{
  /* do nothing */
}

static void
testrawbitclear(void *data)
{
  struct testdata *test;
  uint8_t *buf;
  size_t i;

  test = data;
  buf = (uint8_t *)malloc(test->capa);
  memset(buf, 0xff, test->capa);
  rawbitclear(buf, test->pos, test->size);

  for (i = 0; i < test->pos; i++) {
    if (!rawbitget(buf, i)) {
      testassert(false, "bits between 0 and pos are cleared");
      goto error;
    }
  }

  for (i = test->pos; i < test->pos + test->size; i++) {
    if (rawbitget(buf, i)) {
      testassert(false, "bits are not cleared");
      goto error;
    }
  }

  for (i = test->pos + test->size; i < test->capa * 8; i++) {
    if (!rawbitget(buf, i)) {
      testassert(false, "rest bits are cleared");
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
inittestrawbitclear()
{
  TESTADD(testrawbitclear);
}

