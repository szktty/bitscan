#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "bitscan.h"
#include "test.h"
#include "testgen.h"

struct testdata {
  uint8_t *bytes1;
  size_t pos1;
  uint8_t *bytes2;
  size_t pos2;
  size_t size;
  size_t capa;
  int expected;
};

void **
datatestbitcmp()
{
  struct testdata **data;
  static size_t n = 10000, maxcapa = 1024;
  size_t i, j;
  bool b1, b2;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;
  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->capa = gencapa(maxcapa);
    data[i]->bytes1 = (uint8_t *)malloc(data[i]->capa);
    data[i]->bytes2 = (uint8_t *)malloc(data[i]->capa);
    data[i]->size = gensize(data[i]->capa);
    data[i]->pos1 = genpos(data[i]->capa, data[i]->size);
    data[i]->pos2 = genpos(data[i]->capa, data[i]->size);
    memset(data[i]->bytes1, 0, data[i]->capa);
    memset(data[i]->bytes2, 0, data[i]->capa);

    if (rand() % 2 == 0) {
      /* same bits */
      data[i]->expected = 0;
      bitstdrand(data[i]->bytes1, data[i]->pos1, data[i]->size);
      bitcpy(data[i]->bytes2, data[i]->pos2,
          data[i]->bytes1, data[i]->pos1, data[i]->size);
    } else {
      bitstdrand(data[i]->bytes1, data[i]->pos1, data[i]->size);
      bitstdrand(data[i]->bytes2, data[i]->pos2, data[i]->size);

      data[i]->expected = 0;
      for (j = 0; j < data[i]->size; j++) {
        b1 = bitget(data[i]->bytes1, data[i]->pos1 + j);
        b2 = bitget(data[i]->bytes2, data[i]->pos2 + j);
        if (b1 > b2) {
          data[i]->expected = 1;
          break;
        } else if (b1 < b2) {
          data[i]->expected = -1;
          break;
        }
      }
    }
  }

  return (void **)data;
}

static void
freetestbitcmp(void *data)
{
  struct testdata *test;

  test = (struct testdata *)data;
  free(test->bytes1);
  free(test->bytes2);
}

void
testbitcmp(void *data)
{
  struct testdata *test;

  test = (struct testdata *)data;
  testassert(bitcmp(test->bytes1, test->pos1,
        test->bytes2, test->pos2, test->size) == test->expected,
      "test failed");
}

void
testbiteq(void *data)
{
  struct testdata *test;

  test = (struct testdata *)data;
  testassert(biteq(test->bytes1, test->pos1,
        test->bytes2, test->pos2, test->size) == (test->expected == 0),
      "test failed");
}

void
inittestbitcmp()
{
  TESTADD(testbitcmp);
  testadd("testbiteq", datatestbitcmp, testbiteq, freetestbitcmp);
}

