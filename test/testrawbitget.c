#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

struct testdata {
  uint8_t *bytes;
  size_t pos;
  size_t size;
  uint8_t *expected;
};

static void **
datatestrawbitget()
{
  struct testdata **data;
  static size_t n = 100, bytesize = 512;
  size_t i, j;
  bool b;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->bytes = (uint8_t *)malloc(bytesize);
    data[i]->size = (size_t)(rand() % (bytesize * 8));
    data[i]->pos = (size_t)(rand() % (bytesize * 8 - data[i]->size));
    data[i]->expected = (uint8_t *)malloc(bytesize * 8);
    for (j = 0; j < data[i]->size; j++) {
      b = (bool)(rand() % 2);
      rawbitset(data[i]->bytes, data[i]->pos + j, b);
      data[i]->expected[j] = b;
    }
  }

  return (void **)data;
}

static void
freetestrawbitget(void *data)
{
  struct testdata *test;

  test = data;
  free(test->bytes);
  free(test->expected);
}

static void
testrawbitget(void *data)
{
  struct testdata *test;
  size_t i;

  test = data;
  for (i = 0; i < test->size; i++) {
    if (rawbitget(test->bytes, test->pos + i) != test->expected[i]) {
      testassert(false, "failed");
      return;
    }
  }
  testassert(true, "this message should not be printed");
}

void
inittestrawbitget()
{
  TESTADD(testrawbitget);
}

