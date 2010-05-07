#include "bitscan.h"
#include "test.h"
#include "testgen.h"
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
  struct datatestmake **data = NULL;
  static size_t n = 10000, i, j, last, maxcapa = 1024;
  struct datatestmake *test;
  uint8_t v, shift;
  size_t capa;

  data = (struct datatestmake **)
    malloc(sizeof(struct datatestmake *) * (n+1));

  for (i = 0; i < n; i++) {
    test = (struct datatestmake *)malloc(sizeof(struct datatestmake));
    shift = (uint8_t)(abs(random()) % 8);
    capa = gencapa(maxcapa);
    test->pos = shift;
    test->size = gensize(capa);
    test->alloc = genbool();
    capa = (test->size / 8) + 2;
    test->bytes = malloc(capa);
    test->expected = malloc(capa);
    memset(test->bytes, 0, capa);
    memset(test->expected, 0, capa);
    last = (test->size / 8) + (test->size % 8 > 0);
    for (j = 0; j < last; j++) {
      v = (uint8_t)(abs(random()) % 256);
      test->bytes[j] |= v >> shift;
      test->bytes[j+1] = v << (8 - shift);
      test->expected[j] = v;
    }
    data[i] = test;
  }
  data[n] = NULL;

  return (void **)data;
}

static void
freetestmake_copy(void *data)
{
  struct datatestmake *test;

  test = (struct datatestmake *)data;
  free(test->bytes);
  free(test->expected);
}

static void
testmake_copy(void *data)
{
  struct datatestmake *test;
  bitarray *bits;
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
  testadd("testmake_nocopy", datatestmake_copy, testmake_nocopy,
      freetestmake_copy);
}

