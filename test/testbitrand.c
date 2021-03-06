#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"
#include "testgen.h"

struct testdata {
  uint8_t *bytes;
  size_t capa;
  size_t pos;
  size_t size;
  bool flag;
};

static void **
datatestbitstdrand()
{
  struct testdata **data;
  static size_t n = 10000, bytesize = 512;
  size_t i;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->bytes = (uint8_t *)malloc(bytesize);
    data[i]->capa = bytesize;
    data[i]->size = (size_t)(abs(rand() % (bytesize * 8)));
    if (data[i]->size == 0)
      data[i]->size = 1;

    if (data[i]->size == bytesize * 8)
      data[i]->pos = 0;
    else
      data[i]->pos =
        (size_t)(abs(rand() % (bytesize * 8 - data[i]->size)));
    data[i]->flag = (bool)(abs(random() % 2));
  }

  return (void **)data;
}

static void
freetestbitstdrand(void *data)
{
  struct testdata *test;

  test = data;
  free(test->bytes);
}

static void
testbitstdrand(void *data)
{
  struct testdata *test;
  size_t i;
  bool modified = false;
  char *errmsg;

  test = data;
  memset(test->bytes, test->flag ? 0xff : 0, test->capa);
  bitstdrand(test->bytes, test->pos, test->size);
  for (i = 0; i < test->pos; i++) {
    if (bitget(test->bytes, i) != test->flag) {
      testassert(false, "bits between 0 and pos are modified");
      return;
    }
  }

  for (i = test->pos; i < test->pos + test->size; i++) {
    if (bitget(test->bytes, i) != test->flag) {
      modified = true;
      break;
    }
  }
  errmsg = (char *)malloc(200);
  sprintf(errmsg, "random bits are not generated "
      "(or generated bits equals to the original bits -- size %lu)",
      test->size);
  testassert(modified || test->size <= 16, errmsg);
  free(errmsg);

  for (i = test->pos + test->size; i < test->capa * 8; i++) {
    if (bitget(test->bytes, i) != test->flag) {
      testassert(false, "rest bits are modified");
      return;
    }
  }
  testassert(true, NULL);
}

void
inittestbitrand()
{
  TESTADD(testbitstdrand);
}

