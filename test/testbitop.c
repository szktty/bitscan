#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitscan.h"
#include "test.h"
#include "testgen.h"

struct testdata {
  size_t capa;
  uint8_t *bytes1;
  uint8_t *bytes2;
  size_t pos1;
  size_t pos2;
  size_t size;
  uint8_t *expected;
  size_t expos;
};

enum TESTOP {
  ANDOP,
  OROP,
  XOROP
};

static void **
datatestbitop(int op)
{
  struct testdata **data;
  static size_t n = 10000, maxcapa = 1024;
  size_t i, j;
  bool f1, f2, fop;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->capa = gencapa(maxcapa);
    data[i]->size = gensize(data[i]->capa);
    data[i]->pos1 = genpos(data[i]->capa, data[i]->size);
    data[i]->pos2 = genpos(data[i]->capa, data[i]->size);
    data[i]->expos = genpos(data[i]->capa, data[i]->size);
    data[i]->bytes1 = (uint8_t *)malloc(data[i]->capa);
    data[i]->bytes2 = (uint8_t *)malloc(data[i]->capa);
    data[i]->expected = (uint8_t *)malloc(data[i]->capa);

    bitstdrand(data[i]->bytes1, 0, data[i]->capa * 8);
    bitstdrand(data[i]->bytes2, 0, data[i]->capa * 8);
    memcpy(data[i]->expected, data[i]->bytes1, data[i]->capa);

    for (j = 0; j < data[i]->size; j++) {
      f1 = bitget(data[i]->bytes1, data[i]->pos1 + j);
      f2 = bitget(data[i]->bytes2, data[i]->pos2 + j);
      if (op == ANDOP)
        fop = f1 & f2;
      else if (op == OROP)
        fop = f1 | f2;
      else if (op == XOROP)
        fop = f1 ^ f2;
      else {
        printf("testbitop: unknown op\n");
        exit(1);
      }
      bitset(data[i]->expected, data[i]->expos + j, fop);
    }
  }

  return (void **)data;
}

static void
freetestbitop(void *data)
{
  struct testdata *test;

  test = data;
  free(test->bytes1);
  free(test->bytes2);
  free(test->expected);
}

static void
testbitop(int op, void *data)
{
  struct testdata *test;
  uint8_t *buf;
  void (*tester)(void *dest, size_t destpos,
      const void *bits1, size_t pos1,
      const void *bits2, size_t pos2, size_t size) = NULL;

  if (op == ANDOP)
    tester = bitand;
  else if (op == OROP)
    tester = bitor;
  else if (op == XOROP)
    tester = bitxor;

  test = data;
  buf = (uint8_t *)malloc(test->capa);

  /* write to a different buffer */
  memcpy(buf, test->bytes1, test->capa);
  tester(buf, test->expos, test->bytes1, test->pos1,
      test->bytes2, test->pos2, test->size);
  testassert(biteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write bits to a different buffer");

  /* same buffer */
  memcpy(buf, test->bytes1, test->capa);
  tester(buf, test->expos, buf, test->pos1,
      test->bytes2, test->pos2, test->size);
  testassert(biteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write bits to a same buffer");

  free(buf);
}

static void **
datatestbitand()
{
  return datatestbitop(ANDOP);
}

static void
testbitand(void *data)
{
  testbitop(ANDOP, data);
}

static void **
datatestbitor()
{
  return datatestbitop(OROP);
}

static void
testbitor(void *data)
{
  testbitop(OROP, data);
}

static void **
datatestbitxor()
{
  return datatestbitop(XOROP);
}

static void
testbitxor(void *data)
{
  testbitop(XOROP, data);
}

static void **
datatestbitnot()
{
  struct testdata **data;
  static size_t n = 10000, maxcapa = 1024;
  size_t i, j;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->capa = gencapa(maxcapa);
    data[i]->size = gensize(data[i]->capa);
    data[i]->pos1 = genpos(data[i]->capa, data[i]->size);
    data[i]->expos = genpos(data[i]->capa, data[i]->size);
    data[i]->bytes1 = (uint8_t *)malloc(data[i]->capa);
    data[i]->expected = (uint8_t *)malloc(data[i]->capa);

    bitstdrand(data[i]->bytes1, 0, data[i]->capa * 8);
    memcpy(data[i]->expected, data[i]->bytes1, data[i]->capa);

    for (j = 0; j < data[i]->size; j++) {
      bitset(data[i]->expected, data[i]->expos + j,
          !bitget(data[i]->bytes1, data[i]->pos1 + j));
    }
  }

  return (void **)data;
}

static void
freetestbitnot(void *data)
{
  struct testdata *test;

  test = data;
  free(test->bytes1);
  free(test->expected);
}

static void
testbitnot(void *data)
{
  struct testdata *test;
  uint8_t *buf;

  test = data;
  buf = (uint8_t *)malloc(test->capa);

  /* different pointers */
  memcpy(buf, test->bytes1, test->capa);
  bitnot(buf, test->expos, test->bytes1, test->pos1, test->size);
  testassert(biteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write notd bits to a different pointer");

  /* same pointer */
  memcpy(buf, test->bytes1, test->capa);
  bitnot(buf, test->expos, buf, test->pos1, test->size);
  testassert(biteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write notd bits to the pointer");

  free(buf);
}

static void **
datatestbitreverse()
{
  struct testdata **data;
  static size_t n = 10000, maxcapa = 1024;
  size_t i, j;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->capa = gencapa(maxcapa);
    data[i]->size = gensize(data[i]->capa);
    data[i]->pos1 = genpos(data[i]->capa, data[i]->size);
    data[i]->expos = genpos(data[i]->capa, data[i]->size);
    data[i]->bytes1 = (uint8_t *)malloc(data[i]->capa);
    data[i]->expected = (uint8_t *)malloc(data[i]->capa);

    bitstdrand(data[i]->bytes1, 0, data[i]->capa * 8);
    memcpy(data[i]->expected, data[i]->bytes1, data[i]->capa);

    for (j = 0; j < data[i]->size; j++) {
      bitset(data[i]->expected, data[i]->expos + j,
          bitget(data[i]->bytes1,
            data[i]->pos1 + (data[i]->size - j - 1)));
    }
  }

  return (void **)data;
}

static void
freetestbitreverse(void *data)
{
  struct testdata *test;

  test = data;
  free(test->bytes1);
  free(test->expected);
}

static void
testbitreverse(void *data)
{
  struct testdata *test;
  uint8_t *buf;

  test = data;
  buf = (uint8_t *)malloc(test->capa);

  /* different pointers */
  memcpy(buf, test->bytes1, test->capa);
  bitreverse(buf, test->expos, test->bytes1, test->pos1, test->size);
  testassert(biteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write reversed bits to a different pointer");

  /* same pointer */
  memcpy(buf, test->bytes1, test->capa);
  bitreverse(buf, test->expos, buf, test->pos1, test->size);
  testassert(biteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write reversed bits to the pointer");

  free(buf);
}

void
inittestbitop()
{
  testadd("testbitand", datatestbitand, testbitand, freetestbitop);
  testadd("testbitor", datatestbitor, testbitor, freetestbitop);
  testadd("testbitxor", datatestbitxor, testbitxor, freetestbitop);
  TESTADD(testbitnot);
  TESTADD(testbitreverse);
}

