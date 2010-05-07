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

extern int rawbitcmp(const void *bits1, size_t pos1,
    const void *bits2, size_t pos2, size_t size);
extern bool rawbiteq(const void *bits1, size_t pos1,
    const void *bits2, size_t pos2, size_t size);

extern bool rawbitget(const void *bits, size_t pos);
extern void rawbitset(void *bits, size_t pos, bool value);
extern void rawbitsets(void *bits, size_t pos, uint8_t byte, size_t size);
extern void rawbitsetf(void *bits, size_t pos, size_t size,
    const char *format, ...);
extern void rawbitvsetf(void *bits, size_t pos, size_t size,
    const char *format, va_list ap);
extern void rawbitclear(void *bits, size_t pos, size_t size);
extern void rawbitrand(void *bits, size_t pos, size_t size,
    size_t randsize, void (*rand)(void *buf));
extern void rawbitstdrand(void *bits, size_t pos, size_t size);

extern void rawbitlshift(void *bits, size_t pos, size_t size, size_t shift);
extern void rawbitrshift(void *bits, size_t pos, size_t size, size_t shift);
extern void rawbitlrotate(void *bits, size_t pos, size_t size, size_t rotate);
extern void rawbitrrotate(void *bits, size_t pos, size_t size, size_t rotate);
extern void rawbitand(void *dest, size_t destpos,
   const void *bits1, size_t pos1,
   const void *bits2, size_t pos2, size_t size);
extern void rawbitor(void *dest, size_t destpos,
   const void *bits1, size_t pos1,
   const void *bits2, size_t pos2, size_t size);
extern void rawbitxor(void *dest, size_t destpos,
   const void *bits1, size_t pos1,
   const void *bits2, size_t pos2, size_t size);
extern void rawbitnot(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size);
extern void rawbitreverse(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size);

extern void rawbitcpy(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size);

extern size_t rawbitprintf(void *bits, size_t pos, const char *format, ...);
extern size_t rawbitvprintf(void *bits, size_t pos,
    const char *format, va_list ap);
extern size_t rawbitprintfsize(void *bits, size_t pos,
    const char *format, ...);
extern size_t rawbitprintfsizev(void *bits, size_t pos,
    const char *format, va_list ap);
extern size_t rawbitfprintf(FILE *fp, const char *format, ...);
extern size_t rawbitvfprintf(FILE *fp, const char *format, va_list ap);

extern size_t rawbitscanf(const void *bits, size_t pos,
    const char *format, ...);
extern size_t rawbitvscanf(const void *bits, size_t pos,
    const char *format, va_list ap); 

extern bool rawbitmatch(const void *bits, size_t pos,
    const void *pat, size_t patpos, size_t patsize);
extern bool rawbitmatchf(const void *bits, size_t pos,
    const char *format, ...);
extern bool rawbitvmatchf(const void *bits, size_t pos,
    const char *format, va_list ap); 
extern size_t rawbitfind(const void *bits, size_t pos,
    const void *pat, size_t patpos, size_t patsize);
extern size_t rawbitfindf(const void *bits, size_t pos,
    const char *format, ...);
extern size_t rawbitvfindf(const void *bits, size_t pos,
    const char *format, va_list ap);

extern bitarray *bitmake(void *buf, size_t pos, size_t size, bool copy,
    const bitalloc *alloc);
extern void bitinit(bitarray *bits, void *buf, size_t pos, size_t size);
extern void bitfree(bitarray *bits);

extern size_t bitsize(const bitarray *bits);
extern bool bitgrow(bitarray *bits, size_t growbytes);

extern int bitcmp(const bitarray *bits1, size_t pos1,
    const bitarray *bits2, size_t pos2, size_t size);
extern bool biteq(const bitarray *bits1, size_t pos1, 
    const bitarray *bits2, size_t pos2, size_t size);

extern bool bitget(const bitarray *bits, size_t pos);
extern void bitset(bitarray *bits, size_t pos, bool value);
extern void bitsets(bitarray *bits, size_t pos, uint8_t byte, size_t size);
extern void bitsetf(bitarray *bits, size_t pos, size_t size,
    const char *format, ...);
extern void bitvsetf(bitarray *bits, size_t pos, size_t size,
    const char *format, va_list ap);
extern void bitclear(bitarray *bits, size_t pos, size_t size);
extern void bitrand(bitarray *bits, size_t pos, size_t size);

extern void bitlshift(bitarray *bits, size_t shift);
extern void bitrshift(bitarray *bits, size_t shift);
extern void bitlrotate(bitarray *bits, size_t rotate);
extern void bitrrotate(bitarray *bits, size_t rotate);
extern void bitand(bitarray *dest, size_t destpos,
   const bitarray *src, size_t srcpos,  size_t size);
extern void bisizer(bitarray *dest, size_t destpos,
   const bitarray *src, size_t srcpos,  size_t size);
extern void bitxor(bitarray *dest, size_t destpos,
   const bitarray *src, size_t srcpos,  size_t size);
extern void bitnot(bitarray *bits, size_t pos, size_t size);
extern void bitreverse(bitarray *bits, size_t pos, size_t size);

extern void bitappend(bitarray *dest, const bitarray *src,
    size_t pos, size_t size);
extern void bitappendf(bitarray *dest, const char *format, ...);
extern void bitvappendf(bitarray *dest, const char *format, va_list ap);
extern void bitlpad(bitarray *bits, uint8_t pad, size_t padsize);
extern void bitrpad(bitarray *bits, uint8_t pad, size_t padsize);

extern void bitinsert(bitarray *dest, size_t destpos,
    const bitarray *src, size_t srcpos, size_t size);
extern void bitinsertf(bitarray *dest, size_t pos,
    const char *format, ...);
extern void bitvinsertf(bitarray *dest, size_t pos,
    const char *format, va_list ap);

extern void bitcpy(bitarray *dest, size_t destpos,
    const bitarray *src, size_t srcpos, size_t size);
extern bitarray *bitdup(bitarray *bits);
extern bitarray *bitsub(const bitarray *bits, size_t pos, size_t size);

extern size_t bitprintf(bitarray *bits, const char *format, ...);
extern size_t bitvprintf(bitarray *bits, const char *format, va_list ap);
extern size_t bitprintfsize(bitarray *bits, const char *format, ...);
extern size_t bitprintfsizev(bitarray *bits, const char *format, va_list ap);

extern size_t bitscanf(const bitarray *bits, size_t pos,
    const char *format, ...);
extern size_t bitvscanf(const bitarray *bits, size_t pos,
    const char *format, va_list ap); 

extern bool bitmatch(const bitarray *bits, size_t pos,
    const bitarray *pat, size_t patpos, size_t patsize);
extern bool bitmatchf(const bitarray *bits, size_t pos,
    const char *format, ...);
extern bool bitvmatchf(const bitarray *bits, size_t pos,
    const char *format, va_list ap); 
extern size_t bitfind(const bitarray *bits, size_t pos,
    const bitarray *pat, size_t patpos, size_t patsize);
extern size_t bitfindf(const bitarray *bits, size_t pos,
    const char *format, ...);
extern size_t bitvfindf(const bitarray *bits, size_t pos,
    const char *format, va_list ap);

#ifdef __cplusplus
}
#endif

#endif /* __BITSCAN_H__ */

