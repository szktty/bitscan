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

#ifndef __BITSCAN_H__
#define __BITSCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct bitarray     bitarray;
typedef struct bitalloc     bitalloc;

struct bitarray {
  uint8_t *_bytes;
  bool _copy;
  size_t _capa;
  size_t _pos;
  size_t _size;
  const bitalloc *_alloc;
};

struct bitalloc {
  void *(*alloc)(size_t size);
  void *(*realloc)(void *p, size_t size);
  void (*free)(void *p);
};

extern bitarray *bitmake(void *buf, size_t pos, size_t size, bool copy,
    const bitalloc *alloc);
extern void bitinit(bitarray *bits, void *buf, size_t pos, size_t size);
extern void bitfree(bitarray *bits);

extern size_t bitsize(const bitarray *bits);
extern void bitgrow(bitarray *bits, size_t growbytes);

extern int bitcmp(const bitarray *bits1, size_t from1, size_t to1,
    const bitarray *bits2, size_t from2, size_t to2);
extern bool biteq(const bitarray *bits1, const bitarray *bits2);

extern uint8_t bitget(const bitarray *bits, size_t index);
extern void bitset(bitarray *bits, size_t index, uint8_t value);
extern void bitsets(bitarray *dest, size_t destfrom,
    const bitarray *src, size_t srcfrom, size_t size);
extern void bitfsets(bitarray *bits, size_t from, size_t to,
    const char *format, ...);
extern void bitvfsets(bitarray *bits, size_t from, size_t to,
    const char *format, va_list ap);
extern void bitclear(bitarray *bits, size_t from, size_t to);

extern void bitappend(bitarray *dest, const bitarray *src,
    size_t from, size_t to);
extern void bitappendf(bitarray *dest, const char *format, ...);
extern void bitvappendf(bitarray *dest, const char *format, va_list ap);

extern void bitinsert(bitarray *dest, size_t index,
    const bitarray *src, size_t from, size_t to);
extern void bitinsertf(bitarray *dest, size_t index,
    const char *format, ...);
extern void bitvinsertf(bitarray *dest, size_t index,
    const char *format, va_list ap);

extern void bitcpy(void *dest, size_t destfrom,
    const bitarray *src, size_t srcfrom, size_t size);
extern bitarray *bitdup(bitarray *bits);
extern bitarray *bitsub(const bitarray *bits, size_t from, size_t to);

extern size_t bitbprintf(bitarray *bits, const char *format, ...);
extern size_t bitvbprintf(bitarray *bits, const char *format, va_list ap);
extern size_t bitbprintfsize(bitarray *bits, const char *format, ...);
extern size_t bitbprintfsizev(bitarray *bits, const char *format, va_list ap);
extern size_t bitfprintf(FILE *fp, const char *format, ...);
extern size_t bitvfprintf(FILE *fp, const char *format, va_list ap);

extern size_t bitscanf(const bitarray *bits, size_t from,
    const char *format, ...);
extern size_t bitvscanf(const bitarray *bits, size_t from,
    const char *format, va_list ap); 

extern bool bitmatch(const bitarray *bits, size_t from,
    const bitarray *pat, size_t patfrom, size_t patto);
extern bool bitmatchf(const bitarray *bits, size_t from,
    const char *format, ...);
extern bool bitvmatchf(const bitarray *bits, size_t from,
    const char *format, va_list ap); 
extern size_t bitfind(const bitarray *bits, size_t from,
    const bitarray *pat, size_t patfrom, size_t patto);
extern size_t bitfindf(const bitarray *bits, size_t from,
    const char *format, ...);
extern size_t bitvfindf(const bitarray *bits, size_t from,
    const char *format, va_list ap);

#ifdef __cplusplus
}
#endif

#endif /* __BITSCAN_H__ */

