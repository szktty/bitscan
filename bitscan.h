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

extern int bitcmp(const void *bits1, size_t pos1,
    const void *bits2, size_t pos2, size_t size);
extern bool biteq(const void *bits1, size_t pos1,
    const void *bits2, size_t pos2, size_t size);

extern bool bitget(const void *bits, size_t pos);
extern void bitset(void *bits, size_t pos, bool value);
extern void bitsets(void *bits, size_t pos, uint8_t byte, size_t size);
extern void bitsetf(void *bits, size_t pos, size_t size,
    const char *format, ...);
extern void bitvsetf(void *bits, size_t pos, size_t size,
    const char *format, va_list ap);
extern void bitclear(void *bits, size_t pos, size_t size);
extern void bitrand(void *bits, size_t pos, size_t size,
    size_t randsize, void (*rand)(void *buf));
extern void bitstdrand(void *bits, size_t pos, size_t size);

extern void bitlshift(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size, size_t shift);
extern void bitrshift(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size, size_t shift);
extern void bitlrotate(void *bits, size_t pos, size_t size, size_t rotate);
extern void bitrrotate(void *bits, size_t pos, size_t size, size_t rotate);
extern void bitand(void *dest, size_t destpos,
   const void *bits1, size_t pos1,
   const void *bits2, size_t pos2, size_t size);
extern void bitor(void *dest, size_t destpos,
   const void *bits1, size_t pos1,
   const void *bits2, size_t pos2, size_t size);
extern void bitxor(void *dest, size_t destpos,
   const void *bits1, size_t pos1,
   const void *bits2, size_t pos2, size_t size);
extern void bitnot(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size);
extern void bitreverse(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size);

extern void bitcpy(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size);

extern void bitinsert(void *dest, size_t destpos,
    const void *src, size_t srcpos, size_t size);
extern void bitinsertf(void *dest, size_t pos,
    const char *format, ...);
extern void bitvinsertf(void *dest, size_t pos,
    const char *format, va_list ap);

extern char *bitcompilef(const char *format, size_t *size);

extern size_t bitprintf(const char *format, ...);
extern size_t bitvprintf(const char *format, va_list ap);
extern size_t bitsprintf(void *bits, size_t pos, const char *format, ...);
extern size_t bitvsprintf(void *bits, size_t pos,
    const char *format, va_list ap);
extern size_t bitfprintf(FILE *fp, const char *format, ...);
extern size_t bitvfprintf(FILE *fp, const char *format, va_list ap);

extern size_t bitscanf(const char *format, ...);
extern size_t bitvscanf(const char *format, va_list ap); 
extern size_t bitsscanf(const void *bits, size_t pos,
    const char *format, ...);
extern size_t bitvsscanf(const void *bits, size_t pos,
    const char *format, va_list ap); 
extern size_t bitfscanf(FILE *fp, const char *format, ...);
extern size_t bitvfscanf(FILE *fp, const char *format, va_list ap);

#ifdef __cplusplus
}
#endif

#endif /* __BITSCAN_H__ */

