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
  static size_t n = 100, i, j, last, bytesize;
  struct datatestmake *test;
  uint8_t v, shift;

  if (data == NULL) {
    data = (struct datatestmake **)
      malloc(sizeof(struct datatestmake *) * (n+1));

    for (i = 0; i < n; i++) {
      test = (struct datatestmake *)malloc(sizeof(struct datatestmake));
      shift = (uint8_t)(random() % 8);
      test->pos = shift;
      test->size = (size_t)(random() % 2049);
      bytesize = (test->size / 8) + 2;
      test->bytes = malloc(bytesize);
      test->expected = malloc(bytesize);
      memset(test->bytes, 0, bytesize);
      memset(test->expected, 0, bytesize);
      last = (test->size / 8) + (test->size % 8 > 0);
      for (j = 0; j < last; j++) {
        v = (uint8_t)(random() % 256);
        test->bytes[j] |= v >> shift;
        test->bytes[j+1] = v << (8 - shift);
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
  testassert(0 == bits->_pos, "wrong pos");
  testassert(test->size == bits->_size, "wrong size");
  testassert(rawbiteq(test->expected, 0,
        bits->_bytes, 0, bits->_size),
      "bits are not matched");
  bitfree(bits);
}

static void
testmake_nocopy(void *data)
{
  struct datatestmake *test;
  bitarray *bits;
  size_t i;

  test = (struct datatestmake *)data;
  bits = bitmake(test->bytes, test->pos, test->size, false, NULL);
  testassert(test->pos == bits->_pos, "wrong pos");
  testassert(test->size == bits->_size, "wrong size");
  testassert(test->bytes == bits->_bytes, "wrong byte buffer");
  testassert(rawbiteq(test->bytes, test->pos,
        bits->_bytes, bits->_pos, bits->_size),
      "bits are modified");
  bitfree(bits);
}

void
inittestmake()
{
  TESTADD(testmake_copy);
  testadd("testmake_nocopy", datatestmake_copy, testmake_nocopy);
}

