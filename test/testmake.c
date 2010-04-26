#include "bitscan.h"
#include "test.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct datatestmake {
  uint8_t *bytes;
  size_t pos;
  size_t size;
  uint8_t *expected;
};

static void **
datatestmake_copy()
{
  static struct datatestmake **data = NULL;
  static size_t n = 100, i, j;
  struct datatestmake *test;
  uint8_t v;

  if (data == NULL) {
    data = (struct datatestmake **)
      malloc(sizeof(struct datatestmake *) * (n+1));

    for (i = 0; i < n; i++) {
      test = (struct datatestmake *)malloc(sizeof(struct datatestmake));
      test->pos = 0;
      test->size = (size_t)(random() % 2049);
      test->bytes = malloc((test->size / 8) + 1);
      test->expected = malloc((test->size / 8) + 1);
      for (j = 0; j < test->size / 8; j++) {
        v = (size_t)(random() % 256);
        test->bytes[j] = v;
        test->expected[j] = v;
      }
      data[i] = test;
    }
    data[n] = NULL;
  }
  return (void **)data;
}

static void
testmake_copy(void *data)
{
  struct datatestmake *test;
  bitarray *bits;
  size_t i;

  test = (struct datatestmake *)data;
  bits = bitmake(test->bytes, test->pos, test->size, true, NULL);
  testassert(test->pos == bits->_pos, "wrong pos");
  testassert(test->size == bits->_size, "wrong size");
  testassert(rawbiteq(test->expected, test->pos,
        bits->_bytes, bits->_pos, bits->_size),
      "bits are not matched");
  bitfree(bits);
}

void
inittestmake()
{
  TESTADD(testmake_copy);
}

