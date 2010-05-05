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

static bitalloc default_alloc = {
  malloc,
  realloc,
  free
};

#define GROWRATE            1.6
#define GROWSIZE(n)         (size_t)((n)*GROWRATE)

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

int
rawbitcmp(const void *bits1, size_t pos1,
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
rawbiteq(const void *bits1, size_t pos1,
    const void *bits2, size_t pos2, size_t size)
{
  return (bool)(rawbitcmp(bits1, pos1, bits2, pos2, size) == 0);
}

bool
rawbitget(const void *bits, size_t pos)
{
  return (bool)GET(bits, pos);
}

void
rawbitset(void *bits, size_t pos, bool value)
{
  SET(bits, pos, value);
}

void
rawbitsets(void *bits, size_t pos, uint8_t byte, size_t size)
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
rawbitcpy(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size)
{
  size_t i = 0;

  for (; i < size; i++) {
    SET(dest, destpos + i, GET(src, srcpos + i));
  }
}

void
rawbitclear(void *bits, size_t pos, size_t size)
{
  size_t i = 0;

  for (; i < size; i++) {
    SET(bits, pos + i, false);
  }
}

void
rawbitrand(void *bits, size_t pos, size_t size,
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
rawbitstdrand(void *bits, size_t pos, size_t size)
{
  rawbitrand(bits, pos, size, sizeof(int) * 8, stdrand);
}

void
rawbitand(void *dest, size_t destpos, const void *bits1, size_t pos1,
   const void *bits2, size_t pos2, size_t size)
{
  size_t i;

  for (i = 0; i < size; i++) {
    SET(dest, destpos + i,
        GET(bits1, pos1 + i) & GET(bits2, pos2 + i));
  }
}

void
rawbitor(void *dest, size_t destpos, const void *bits1, size_t pos1,
   const void *bits2, size_t pos2, size_t size)
{
  size_t i;

  for (i = 0; i < size; i++) {
    SET(dest, destpos + i,
        GET(bits1, pos1 + i) | GET(bits2, pos2 + i));
  }
}

void
rawbitxor(void *dest, size_t destpos, const void *bits1, size_t pos1,
   const void *bits2, size_t pos2, size_t size)
{
  size_t i, capa;
  void *temp1, *temp2;

  if (((size_t)bits1 + pos1 >= (size_t)dest + destpos) &&
      ((size_t)bits2 + pos2 >= (size_t)dest + destpos)) {
    for (i = 0; i < size; i++) {
      SET(dest, destpos + i, GET(bits1, pos1 + i) ^ GET(bits2, pos2 + i));
    }
  } else {
    capa = (size / 8 + 1) * 2;
    temp1 = malloc(capa);
    temp2 = temp1 + capa / 2;
    rawbitcpy(temp1, 0, bits1, pos1, size);
    rawbitcpy(temp2, 0, bits2, pos2, size);
    for (i = 0; i < size; i++) {
      SET(dest, destpos + i, GET(temp1, i) ^ GET(temp2, i));
    }
    free(temp1);
  }
}

void
rawbitreverse(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size)
{
  size_t i;
  void *temp;

  if ((size_t)src + srcpos + size < dest ||
      (size_t)dest + destpos + size < src) {
    for (i = 0; i < size; i++) {
      SET(dest, destpos + i, !GET(src, srcpos + i));
    }
  } else {
    temp = malloc(size / 8 + 1);
    rawbitcpy(temp, 0, src, srcpos, size);
    for (i = 0; i < size; i++) {
      SET(dest, destpos + i, !GET(temp, i));
    }
    free(temp);
  }
}

bitarray *
bitmake(void *buf, size_t pos, size_t size, bool copy, const bitalloc *alloc)
{
  bitarray *bits;

  if (alloc == NULL)
    alloc = &default_alloc;

  bits = (bitarray *)alloc->alloc(sizeof(bitarray));
  bits->_alloc = alloc;
  bits->_size = size;
  bits->_copy = copy;
  if (copy) {
    bits->_capa = GROWSIZE((size / 8) + 2);
    bits->_bytes = (uint8_t *)alloc->alloc(bits->_capa);
    memset(bits->_bytes, 0, bits->_capa);
    bits->_pos = 0;
    rawbitcpy(bits->_bytes, 0, buf, pos, size);
  } else {
    bits->_capa = 0;
    bits->_bytes = buf;
    bits->_pos = pos;
  }

  return bits;
}

void
bitinit(bitarray *bits, void *buf, size_t pos, size_t size)
{
  bits->_alloc = NULL;
  bits->_capa = 0;
  bits->_bytes = buf;
  bits->_pos = pos;
  bits->_size = size;
  bits->_copy = true;
}

void
bitfree(bitarray *bits)
{
  if (bits->_copy)
    bits->_alloc->free(bits->_bytes);
  bits->_alloc->free(bits);
}

size_t
bitsize(const bitarray *bits)
{
  return bits->_size;
}

bool
bitgrow(bitarray *bits, size_t growbytes)
{
  void *new;

  if (bits->_copy) {
    new = realloc(bits->_bytes, growbytes);
    if (new != NULL) {
      bits->_bytes = new;
      return true;
    } else
      return false;
  } else
    return false;
}

bool
bitget(const bitarray *bits, size_t pos)
{
  return (bool)GET(bits->_bytes, pos);
}

void
bitset(bitarray *bits, size_t pos, bool value)
{
  SET(bits->_bytes, pos, value);
}

