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

#define GROWRATE        1.6
#define GROWSIZE(n)     (size_t)((n)*GROWRATE)

#define AT(bytes,idx)     ((uint8_t *)(bytes))[(idx)/8]
#define GET(bytes,idx)    (AT(bytes,idx) & (1<<(8-(idx)%8)))
#define SET(bytes,idx,v)  \
  AT(bytes,idx) = (AT(bytes,idx) & ~(1<<(8-(idx)%8))) | (v)

int
rawbitcmp(const void *bits1, size_t from1,
    const void *bits2, size_t from2, size_t size)
{
  size_t i;

  for (i = 0; i < size; i++) {
    if (GET(bits1, from1 + i) == GET(bits2, from2 + i))
      continue;
    else if (GET(bits1, from1 + i) > GET(bits2, from2 + i))
      return 1;
    else
      return -1;
  }
  return 0;
}

bool
rawbiteq(const void *bits1, size_t from1,
    const void *bits2, size_t from2, size_t size)
{
  return rawbitcmp(bits1, 0, bits2, 0, size) == 0;
}

void
rawbitsets(void *dest, size_t destfrom,
    const void *src, size_t srcfrom, size_t size)
{
  size_t i = 0;

  for (; i < size; i++) {
    SET(dest, destfrom + i, GET(src, srcfrom + i));
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
    bits->_capa = GROWSIZE((size / 8) + 1);
    bits->_bytes = (uint8_t *)alloc->alloc(bits->_capa);
    bits->_pos = 0;
    rawbitsets(bits->_bytes, 0, buf, pos, size);
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
bitget(const bitarray *bits, size_t index)
{
  return (bool)GET(bits->_bytes, index);
}

void
bitset(bitarray *bits, size_t index, bool value)
{
  SET(bits->_bytes, index, value);
}

