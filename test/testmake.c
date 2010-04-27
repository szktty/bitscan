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
  bool alloc;
  uint8_t *expected;
};

static bool alloc_call;
static bool realloc_call;
static bool free_call;

static void
initalloc()
{
  alloc_call = false;
  realloc_call = false;
  free_call = false;
}

static void *
testalloc(size_t size)
{
  alloc_call = true;
  return malloc(size);
}

static void *
testrealloc(void *p, size_t size)
{
  realloc_call = true;
  return realloc(p, size);
}

static void
testfree(void *p)
{
  free_call = true;
  free(p);
}

static bitalloc dummyalloc = {
  testalloc,
  testrealloc,
  testfree
};

static void **
datatestmake_copy()
{
  static struct datatestmake **data = NULL;
  static size_t n = 500, i, j, last, bytesize;
  struct datatestmake *test;
  uint8_t v, shift;

  if (data == NULL) {
    data = (struct datatestmake **)
      malloc(sizeof(struct datatestmake *) * (n+1));

    for (i = 0; i < n; i++) {
      test = (struct datatestmake *)malloc(sizeof(struct datatestmake));
      shift = (uint8_t)(random() % 8);
      test->pos = shift;
      test->size = (size_t)(random() % 4097);
      test->alloc = (bool)(random() % 2);
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
  bitalloc *alloc = NULL;

  initalloc();
  test = (struct datatestmake *)data;
  if (test->alloc)
    alloc = &dummyalloc;
  bits = bitmake(test->bytes, test->pos, test->size, true, alloc);
  if (test->alloc)
    testassert(alloc_call, "user alloc is not used");
  testassert(0 == bits->_pos, "wrong pos");
  testassert(test->size == bits->_size, "wrong size");
  testassert(rawbiteq(test->expected, 0,
        bits->_bytes, 0, bits->_size),
      "bits are not matched");
  bitfree(bits);
  if (test->alloc)
    testassert(free_call, "user alloc is not used");
}

static void
testmake_nocopy(void *data)
{
  struct datatestmake *test;
  bitarray *bits;
  size_t i;
  bitalloc *alloc = NULL;

  initalloc();
  test = (struct datatestmake *)data;
  if (test->alloc)
    alloc = &dummyalloc;
  bits = bitmake(test->bytes, test->pos, test->size, false, alloc);
  if (test->alloc)
    testassert(alloc_call, "user alloc is not used");
  testassert(test->pos == bits->_pos, "wrong pos");
  testassert(test->size == bits->_size, "wrong size");
  testassert(test->bytes == bits->_bytes, "wrong byte buffer");
  testassert(rawbiteq(test->bytes, test->pos,
        bits->_bytes, bits->_pos, bits->_size),
      "bits are modified");
  bitfree(bits);
  if (test->alloc)
    testassert(free_call, "user free is not used");
}

void
inittestmake()
{
  TESTADD(testmake_copy);
  testadd("testmake_nocopy", datatestmake_copy, testmake_nocopy);
}

