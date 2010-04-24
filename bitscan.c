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

bitarray *
bitmake(void *buf, size_t pos, size_t size, const bitalloc *alloc)
{
  bitarray *bits;
  size_t bpos = 0;

  if (alloc == NULL)
    alloc = &default_alloc;

  bits = (bitarray *)alloc->alloc(sizeof(bitarray));
  bits->_alloc = alloc;
  bits->_capa = GROWSIZE((size / 8) + 1);
  bits->_bytes = (uint8_t *)alloc->alloc(bits->_capa);
  bits->_pos = 0;
  bits->_bitsize = size;

  if (pos == 0) {
    memmove(bits->_bytes, buf, pos / 8);
    bpos = pos / 8 * 8;
  }

  for (; bpos < size; pos++, bpos++) {
    SET(bits->_bytes, bpos, GET(buf, pos));
  }

  return bits;
}

void
bitfree(bitarray *bits)
{
  bits->_alloc->free(bits->_bytes);
  bits->_alloc->free(bits);
}

size_t
bitsize(const bitarray *bits)
{
  return bits->_bitsize;
}

uint8_t
bitget(const bitarray *bits, size_t index)
{
  return GET(bits->_bytes, index);
}

void
bitset(bitarray *bits, size_t index, uint8_t value)
{
  SET(bits->_bytes, index, value);
}

