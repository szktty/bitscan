#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "test.h"

struct testdata {
  uint8_t *bytes;
  size_t capa;
  size_t pos;
  size_t size;
  bool flag;
};

static void **
datatestrawbitrand()
{
  static struct testdata **data;
  static size_t n = 100, bytesize = 512;
  size_t i, j;
  bool b;

  if (data == NULL) {
    data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
    data[n] = NULL;

    for (i = 0; i < n; i++) {
      data[i] = (struct testdata *)malloc(sizeof(struct testdata));
      data[i]->bytes = (uint8_t *)malloc(bytesize);
      data[i]->capa = bytesize;
      data[i]->size = (size_t)(rand() % (bytesize * 8));
      data[i]->pos = (size_t)(rand() % (bytesize * 8 - data[i]->size));
      data[i]->flag = (bool)(random() % 2);
    }
  }

  return (void **)data;
}

static void
testrawbitrand(void *data)
{
  struct testdata *test;
  size_t i;
  bool modified = false;

  test = data;
  memset(test->bytes, test->flag ? 0xff : 0, test->capa);
  rawbitrand(test->bytes, test->pos, test->size);
  for (i = 0; i < test->pos; i++) {
    if (rawbitget(test->bytes, i) != test->flag) {
      testassert(false, "bits between 0 and pos are modified");
      return;
    }
  }

  for (i = test->pos; i < test->pos + test->size; i++) {
    if (rawbitget(test->bytes, i) != test->flag) {
      modified = true;
      break;
    }
  }
  testassert(modified, "random bits are not generated");

  for (i = test->pos + test->size; i < test->capa * 8; i++) {
    if (rawbitget(test->bytes, i) != test->flag) {
      testassert(false, "rest bits are modified");
      return;
    }
  }
  testassert(true, NULL);
}

void
inittestrawbitrand()
{
  TESTADD(testrawbitrand);
}

