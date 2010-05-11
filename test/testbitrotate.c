#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitscan.h"
#include "test.h"
#include "testgen.h"

struct testdata {
  size_t capa;
  uint8_t *bytes;
  size_t pos;
  size_t size;
  size_t shift;
  uint8_t *expected;
  size_t expos;
};

static void **
datatestbitlrotate()
{
  struct testdata **data;
  static size_t n = 10000, maxcapa = 2;
  size_t i, j;
  bool f;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->capa = gencapa(maxcapa);
    data[i]->size = gensize(data[i]->capa);
    data[i]->shift = (size_t)(abs(rand()) % data[i]->size);
    data[i]->pos = genpos(data[i]->capa, data[i]->size);
    data[i]->expos = genpos(data[i]->capa, data[i]->size);
    data[i]->bytes = (uint8_t *)malloc(data[i]->capa);
    data[i]->expected = (uint8_t *)malloc(data[i]->capa);

    bitstdrand(data[i]->bytes, 0, data[i]->capa * 8);
    memcpy(data[i]->expected, data[i]->bytes, data[i]->capa);

    if (data[i]->shift > 0) {
      for (j = 0; j < data[i]->size; j++) {
        if (j + data[i]->shift < data[i]->size)
          f = bitget(data[i]->bytes, data[i]->pos + j + data[i]->shift);
        else
          f = bitget(data[i]->bytes, data[i]->pos + j
              - (data[i]->size - data[i]->shift));
        bitset(data[i]->expected, data[i]->expos + j, f);
      }
    } else {
      bitcpy(data[i]->expected, data[i]->expos,
          data[i]->bytes, data[i]->pos, data[i]->size);
    }
  }

  return (void **)data;
}

static void
freetestbitlrotate(void *data)
{
  struct testdata *test;

  test = data;
  free(test->bytes);
  free(test->expected);
}

static void
testbitlrotate(void *data)
{
  struct testdata *test;
  uint8_t *buf;

  test = data;
  buf = (uint8_t *)malloc(test->capa);

  /* write to a different buffer */
  memcpy(buf, test->bytes, test->capa);
  bitlrotate(buf, test->expos, test->bytes, test->pos,
      test->size, test->shift);
  testassert(biteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write bits to a different buffer");

  /* same buffer */
  memcpy(buf, test->bytes, test->capa);
  bitlrotate(buf, test->expos, buf, test->pos,
      test->size, test->shift);
  testassert(biteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write bits to a same buffer");

  free(buf);
}

static void **
datatestbitrrotate()
{
  struct testdata **data;
  static size_t n = 10000, maxcapa = 2;
  size_t i, j;
  bool f;

  data = (struct testdata **)malloc(sizeof(struct testdata *) * (n+1));
  data[n] = NULL;

  for (i = 0; i < n; i++) {
    data[i] = (struct testdata *)malloc(sizeof(struct testdata));
    data[i]->capa = gencapa(maxcapa);
    data[i]->size = gensize(data[i]->capa);
    data[i]->shift = (size_t)(abs(rand()) % data[i]->size);
    data[i]->pos = genpos(data[i]->capa, data[i]->size);
    data[i]->expos = genpos(data[i]->capa, data[i]->size);
    data[i]->bytes = (uint8_t *)malloc(data[i]->capa);
    data[i]->expected = (uint8_t *)malloc(data[i]->capa);

    bitstdrand(data[i]->bytes, 0, data[i]->capa * 8);
    memcpy(data[i]->expected, data[i]->bytes, data[i]->capa);

    if (data[i]->shift > 0) {
      for (j = 0; j < data[i]->size; j++) {
        if (j >= data[i]->shift) {
          f = bitget(data[i]->bytes, data[i]->pos + j - data[i]->shift);
        }
        else {
          f = bitget(data[i]->bytes, data[i]->pos + j
              + (data[i]->size - data[i]->shift));
        }
        bitset(data[i]->expected, data[i]->expos + j, f);
      }
    } else {
      bitcpy(data[i]->expected, data[i]->expos,
          data[i]->bytes, data[i]->pos, data[i]->size);
    }
  }

  return (void **)data;
}

static void
freetestbitrrotate(void *data)
{
  struct testdata *test;

  test = data;
  free(test->bytes);
  free(test->expected);
}

static void
testbitrrotate(void *data)
{
  struct testdata *test;
  uint8_t *buf;

  test = data;
  buf = (uint8_t *)malloc(test->capa);

  /* write to a different buffer */
  memcpy(buf, test->bytes, test->capa);
  bitrrotate(buf, test->expos, test->bytes, test->pos,
      test->size, test->shift);
  testassert(biteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write bits to a different buffer");

  /* same buffer */
  memcpy(buf, test->bytes, test->capa);
  bitrrotate(buf, test->expos, buf, test->pos,
      test->size, test->shift);
  testassert(biteq(buf, 0, test->expected, 0, test->capa * 8),
      "failed to write bits to a same buffer");

  free(buf);
}

void
inittestbitrotate()
{
  TESTADD(testbitlrotate);
  TESTADD(testbitrrotate);
}

