/*
 * Copyright (c) 2010 SUZUKI Tetsuya <suzuki@spice-of-life.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "bitscan.h"

typedef enum BITOP {
  ANDOP,
  OROP,
  XOROP
} BITOP;

#define BYTE(x)             (x/8)
#define ISBYTEALIGN(x)      (x%8==0)

#define AT(bytes,idx)       ((uint8_t *)(bytes))[(idx)/8]
#define SHIFTS(idx)         (7-(idx)%8)
#define GET(bytes,idx)      \
  ((AT(bytes,idx) & (1<<SHIFTS(idx))) >> SHIFTS(idx))
#define SET(bytes,idx,v)    \
  do {                                                      \
    AT(bytes,idx) = (AT(bytes,idx) & ~(1<<SHIFTS(idx))) |   \
      ((v)&1?1<<SHIFTS(idx):0);                             \
  } while (0)

static void bitshift(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size, size_t shift, bool left);
static void bitop(BITOP op, void *dest, size_t destpos,
    const void *bits1, size_t pos1,
    const void *bits2, size_t pos2, size_t size);

int
bitcmp(const void *bits1, size_t pos1,
    const void *bits2, size_t pos2, size_t size)
{
  size_t i;

  for (i = 0; i < size; i++) {
    if (GET(bits1, pos1 + i) == GET(bits2, pos2 + i))
      continue;
    else if (GET(bits1, pos1 + i) > GET(bits2, pos2 + i))
      return 1;
    else
      return -1;
  }
  return 0;
}

bool
biteq(const void *bits1, size_t pos1,
    const void *bits2, size_t pos2, size_t size)
{
  return (bool)(bitcmp(bits1, pos1, bits2, pos2, size) == 0);
}

bool
bitget(const void *bits, size_t pos)
{
  return (bool)GET(bits, pos);
}

void
bitset(void *bits, size_t pos, bool value)
{
  SET(bits, pos, value);
}

void
bitsets(void *bits, size_t pos, uint8_t byte, size_t size)
{
  size_t i;

  for (i = 0; i < size; i++, pos += 8) {
    SET(bits, pos, byte >> 7);
    SET(bits, pos + 1, byte >> 6);
    SET(bits, pos + 2, byte >> 5);
    SET(bits, pos + 3, byte >> 4);
    SET(bits, pos + 4, byte >> 3);
    SET(bits, pos + 5, byte >> 2);
    SET(bits, pos + 6, byte >> 1);
    SET(bits, pos + 7, byte);
  }
}

void
bitcpy(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size)
{
  size_t i;
  uint8_t *temp;

  if ((size_t)src + srcpos + size < (size_t)dest ||
      (size_t)dest + destpos + size < (size_t)src) {
    for (i = 0; i < size; i++) {
      SET(dest, destpos + i, GET(src, srcpos + i));
    }
  } else {
    temp = (uint8_t *)malloc(size / 8 + 1);
    for (i = 0; i < size; i++) {
      SET(temp, i, GET(src, srcpos + i));
    }
    for (i = 0; i < size; i++) {
      SET(dest, destpos + i, GET(temp, i));
    }
    free(temp);
  }
}

void
bitclear(void *bits, size_t pos, size_t size)
{
  size_t i = 0;

  for (; i < size; i++) {
    SET(bits, pos + i, false);
  }
}

void
bitrand(void *bits, size_t pos, size_t size,
    size_t randsize, void (*rand)(void *buf))
{
  size_t i, j, capa;
  uint8_t *buf;

  capa = randsize / 8 * 8 + 1;
  buf = malloc(capa);

  for (i = 0; i < size;) {
    memset(buf, 0, capa);
    rand(buf);
    for (j = 0; j < randsize && i < size; i++, j++) {
      SET(bits, pos+i, GET(buf, j));
    }
  }
}

static void
stdrand(void *buf)
{
  memset(buf, random(), sizeof(int));
}

void
bitstdrand(void *bits, size_t pos, size_t size)
{
  bitrand(bits, pos, size, sizeof(int) * 8, stdrand);
}

static void
bitshift(void *dest, size_t destpos, const void *src, size_t srcpos,
    size_t size, size_t shift, bool left)
{
  size_t i;
  void *temp;

  if (shift == 0) {
    bitcpy(dest, destpos, src, srcpos, size);
    return;
  }

  if ((size_t)src + srcpos + size < (size_t)dest ||
      (size_t)dest + destpos + size < (size_t)src) {
    if (left) {
      for (i = 0; i < size; i++) {
        if (i + shift < size) {
          SET(dest, destpos + i, GET(src, srcpos + i + shift));
        } else {
          SET(dest, destpos + i, false);
        }
      }
    } else {
      for (i = 0; i < size; i++) {
        if (i > shift) {
          SET(dest, destpos + i, GET(src, srcpos + i - shift));
        } else {
          SET(dest, destpos + i, false);
        }
      }
    }
  } else {
    temp = malloc(size / 8 + 1);
    bitcpy(temp, 0, src, srcpos, size);
    if (left) {
      for (i = 0; i < size; i++) {
        if (i + shift < size) {
          SET(dest, destpos + i, GET(temp, i + shift));
        } else {
          SET(dest, destpos + i, false);
        }
      }
    } else {
      for (i = 0; i < size; i++) {
        if (i > shift) {
          SET(dest, destpos + i, GET(temp, i - shift));
        } else {
          SET(dest, destpos + i, false);
        }
      }
    }
    free(temp);
  }
}

void
bitlshift(void *dest, size_t destpos, const void *src, size_t srcpos,
    size_t size, size_t shift)
{
  bitshift(dest, destpos, src, srcpos, size, shift, true);
}

void
bitrshift(void *dest, size_t destpos, const void *src, size_t srcpos,
    size_t size, size_t shift)
{
  bitshift(dest, destpos, src, srcpos, size, shift, false);
}

static void
bitop(BITOP op, void *dest, size_t destpos,
    const void *bits1, size_t pos1,
    const void *bits2, size_t pos2, size_t size)
{
  size_t i, capa;
  void *temp1, *temp2;

  if (((size_t)bits1 + pos1 + size < (size_t)dest ||
        (size_t)dest + destpos + size < (size_t)bits1) &&
      ((size_t)bits2 + pos2 + size < (size_t)dest ||
        (size_t)dest + destpos + size < (size_t)bits2)) {
    switch (op) {
    case ANDOP:
      for (i = 0; i < size; i++) {
        SET(dest, destpos + i, GET(bits1, pos1 + i) & GET(bits2, pos2 + i));
      }
      break;
    case OROP:
      for (i = 0; i < size; i++) {
        SET(dest, destpos + i, GET(bits1, pos1 + i) | GET(bits2, pos2 + i));
      }
      break;
    case XOROP:
      for (i = 0; i < size; i++) {
        SET(dest, destpos + i, GET(bits1, pos1 + i) ^ GET(bits2, pos2 + i));
      }
      break;
    }
  } else {
    capa = (size / 8 + 1) * 2;
    temp1 = malloc(capa);
    temp2 = temp1 + capa / 2;
    bitcpy(temp1, 0, bits1, pos1, size);
    bitcpy(temp2, 0, bits2, pos2, size);

    switch (op) {
    case ANDOP:
      for (i = 0; i < size; i++) {
        SET(dest, destpos + i, GET(temp1, i) & GET(temp2, i));
      }
      break;
    case OROP:
      for (i = 0; i < size; i++) {
        SET(dest, destpos + i, GET(temp1, i) | GET(temp2, i));
      }
      break;
    case XOROP:
      for (i = 0; i < size; i++) {
        SET(dest, destpos + i, GET(temp1, i) ^ GET(temp2, i));
      }
      break;
    }
    free(temp1);
  }
}

void
bitand(void *dest, size_t destpos, const void *bits1, size_t pos1,
   const void *bits2, size_t pos2, size_t size)
{
  bitop(ANDOP, dest, destpos, bits1, pos1, bits2, pos2, size);
}

void
bitor(void *dest, size_t destpos, const void *bits1, size_t pos1,
   const void *bits2, size_t pos2, size_t size)
{
  bitop(OROP, dest, destpos, bits1, pos1, bits2, pos2, size);
}

void
bitxor(void *dest, size_t destpos, const void *bits1, size_t pos1,
   const void *bits2, size_t pos2, size_t size)
{
  bitop(XOROP, dest, destpos, bits1, pos1, bits2, pos2, size);
}

void
bitnot(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size)
{
  size_t i;
  void *temp;

  if ((size_t)src + srcpos + size < (size_t)dest ||
      (size_t)dest + destpos + size < (size_t)src) {
    for (i = 0; i < size; i++) {
      SET(dest, destpos + i, !GET(src, srcpos + i));
    }
  } else {
    temp = malloc(size / 8 + 1);
    bitcpy(temp, 0, src, srcpos, size);
    for (i = 0; i < size; i++) {
      SET(dest, destpos + i, !GET(temp, i));
    }
    free(temp);
  }
}

void
bitreverse(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size)
{
  size_t i;
  void *temp;

  if ((size_t)src + srcpos + size < (size_t)dest ||
      (size_t)dest + destpos + size < (size_t)src) {
    for (i = 0; i < size; i++) {
      SET(dest, destpos + i, GET(src, srcpos + (size - i - 1)));
    }
  } else {
    temp = malloc(size / 8 + 1);
    bitcpy(temp, 0, src, srcpos, size);
    for (i = 0; i < size; i++) {
      SET(dest, destpos + i, GET(temp, (size - i - 1)));
    }
    free(temp);
  }
}

