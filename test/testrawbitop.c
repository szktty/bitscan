#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitscan.h"
#include "test.h"

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
datatestrawbitop(int op)
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

    rawbitstdrand(data[i]->bytes1, 0, data[i]->capa * 8);
    rawbitstdrand(data[i]->bytes2, 0, data[i]->capa * 8);
    memcpy(data[i]->expected, data[i]->bytes1, data[i]->capa);

    for (j = 0; j < data[i]->size; j++) {
      f1 = rawbitget(data[i]->bytes1, data[i]->pos1 + j);
      f2 = rawbitget(data[i]->bytes2, data[i]->pos2 + j);
      if (op == ANDOP)
        fop = f1 & f2;
      else if (op == OROP)
        fop = f1 | f2;
      else if (op == XOROP)
        fop = f1 ^ f2;
      else {
        printf("testrawbitop: unknown op\n");
        exit(1);
      }
      rawbitset(data[i]->expected, data[i]->expos + j, fop);
    }
  }

  return (void **)data;
}

static void
freetestrawbitop(void *data)
{
  struct testdata *test;

  test = data;
  free(test->bytes1);
  free(test->bytes2);
  free(test->expected);
}

static void
testrawbitop(int op, void *data)
{
  struct testdata *test;
  uint8_t *buf;
  size_t i;
  void (*tester)(void *dest, size_t destpos,
      const void *bits1, size_t pos1,
      const void *bits2, size_t pos2, size_t size);

  if (op == ANDOP)
    tester = rawbitand;
  else if (op == OROP)
    tester = rawbitor;
  else if (op == XOROP)
    tester = rawbitxor;

  test = data;
  buf = (uint8_t *)malloc(test->capa);

  /* write to a different buffer */
  memcpy(buf, test->bytes1, test->capa);
  tester(buf, test->expos, test->bytes1, test->pos1,
      test->bytes2, test->pos2, test->size);
  testassert(rawbiteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write bits to a different buffer");

  /* same buffer */
  memcpy(buf, test->bytes1, test->capa);
  tester(buf, test->expos, buf, test->pos1,
      test->bytes2, test->pos2, test->size);
  testassert(rawbiteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write bits to a same buffer");

  free(buf);
}

static void **
datatestrawbitand()
{
  return datatestrawbitop(ANDOP);
}

static void
testrawbitand(void *data)
{
  testrawbitop(ANDOP, data);
}

static void **
datatestrawbitor()
{
  return datatestrawbitop(OROP);
}

static void
testrawbitor(void *data)
{
  testrawbitop(OROP, data);
}

static void **
datatestrawbitxor()
{
  return datatestrawbitop(XOROP);
}

static void
testrawbitxor(void *data)
{
  testrawbitop(XOROP, data);
}

static void **
datatestrawbitreverse()
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

    rawbitstdrand(data[i]->bytes1, 0, data[i]->capa * 8);
    memcpy(data[i]->expected, data[i]->bytes1, data[i]->capa);

    for (j = 0; j < data[i]->size; j++) {
      rawbitset(data[i]->expected, data[i]->expos + j,
          !rawbitget(data[i]->bytes1, data[i]->pos1 + j));
    }
  }

  return (void **)data;
}

static void
freetestrawbitreverse(void *data)
{
  struct testdata *test;

  test = data;
  free(test->bytes1);
  free(test->expected);
}

static void
testrawbitreverse(void *data)
{
  struct testdata *test;
  uint8_t *buf;
  size_t i;

  test = data;
  buf = (uint8_t *)malloc(test->capa);

  /* different pointers */
  memcpy(buf, test->bytes1, test->capa);
  rawbitreverse(buf, test->expos, test->bytes1, test->pos1, test->size);
  testassert(rawbiteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write reversed bits to a different pointer");

  /* same pointer */
  memcpy(buf, test->bytes1, test->capa);
  rawbitreverse(buf, test->expos, buf, test->pos1, test->size);
  testassert(rawbiteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write reversed bits to the pointer");

  free(buf);
}

void
inittestrawbitop()
{
  testadd("testrawbitand", datatestrawbitand, testrawbitand, freetestrawbitop);
  testadd("testrawbitor", datatestrawbitor, testrawbitor, freetestrawbitop);
  testadd("testrawbitxor", datatestrawbitxor, testrawbitxor, freetestrawbitop);
  TESTADD(testrawbitreverse);
}

