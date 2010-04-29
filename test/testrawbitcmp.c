#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "bitscan.h"
#include "test.h"

struct testdata {
  uint8_t *bytes1;
  size_t pos1;
  uint8_t *bytes2;
  size_t pos2;
  size_t size;
  size_t bytesize;
  int expected;
};

void **
datatestrawbitcmp()
{
  static struct testdata **data;
  static size_t n = 100, maxbytesize = 1024;
  size_t i, j, bytesize;
  bool b1, b2;

  if (data == NULL) {
    data = (struct testdata **)malloc(sizeof(struct testdata *) * n+1);
    data[n] = NULL;
    for (i = 0; i < n; i++) {
      bytesize = (size_t)(rand() % maxbytesize);
      data[i] = (struct testdata *)malloc(sizeof(struct testdata));
      data[i]->bytesize = bytesize;
      data[i]->bytes1 = (uint8_t *)malloc(bytesize);
      data[i]->bytes2 = (uint8_t *)malloc(bytesize);
      data[i]->size = (size_t)(rand() % (bytesize * 8));
      data[i]->pos1 = (size_t)(rand() % (bytesize * 8 - data[i]->size));
      data[i]->pos2 = (size_t)(rand() % (bytesize * 8 - data[i]->size));
      memset(data[i]->bytes1, 0, bytesize);
      memset(data[i]->bytes2, 0, bytesize);

      if (rand() % 2 == 0) {
        /* same bits */
        data[i]->expected = 0;
        rawbitrand(data[i]->bytes1, data[i]->pos1, data[i]->size);
        rawbitcpy(data[i]->bytes2, data[i]->pos2,
            data[i]->bytes1, data[i]->pos1, data[i]->size);
      } else {
        rawbitrand(data[i]->bytes1, data[i]->pos1, data[i]->size);
        rawbitrand(data[i]->bytes2, data[i]->pos2, data[i]->size);

        data[i]->expected = 0;
        for (j = 0; j < data[i]->size; j++) {
          b1 = rawbitget(data[i]->bytes1, data[i]->pos1 + j);
          b2 = rawbitget(data[i]->bytes2, data[i]->pos2 + j);
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
  }

  return (void **)data;
}

void
testrawbitcmp(void *data)
{
  struct testdata *test;

  test = (struct testdata *)data;
  testassert(rawbitcmp(test->bytes1, test->pos1,
        test->bytes2, test->pos2, test->size) == test->expected,
      "test failed");
}

void
testrawbiteq(void *data)
{
  struct testdata *test;

  test = (struct testdata *)data;
  testassert(rawbiteq(test->bytes1, test->pos1,
        test->bytes2, test->pos2, test->size) == (test->expected == 0),
      "test failed");
}

void
inittestrawbitcmp()
{
  TESTADD(testrawbitcmp);
  testadd("testrawbiteq", datatestrawbitcmp, testrawbiteq);
}

