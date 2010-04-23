/*
 * Copyright (c) 2010 SUZUKI Tetsuya
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

typedef struct bitarray bitarray;

struct bitarray {
  void *bits;
  size_t pos;
  size_t size;
};

extern bitarray bitmake(void *bits, size_t pos, size_t size);

extern bitarray bitcat(bitarray dest, const bitarray src);
extern void bitcpy(bitarray dest, ssize_t from, ssize_t to,
    const bitarray src);
extern bitarray bitsub(const bitarray bits, ssize_t from, ssize_t to);

extern size_t bitformat(bitarray bits, const char *format, ...);
extern size_t bitformatv(bitarray bits, const char *format, va_list ap);
extern size_t bitformatf(FILE *fp, const char *format, ...);
extern size_t bitformatfv(FILE *fp, const char *format, va_list ap);

extern size_t bitscan(const bitarray bits, const char *format, ...);
extern size_t bitscanv(const bitarray bits, const char *format, va_list ap); 

extern bool bitmatch(const bitarray bits, const char *format, ...);
extern bool bitmatchv(const bitarray bits, const char *format, va_list ap); 

extern int bitcmp(const bitarray bits1, const bitarray bits2);
extern bool biteq(const bitarray bits1, const bitarray bits2);

extern size_t bitfind(const bitarray bits, const char *format, ...);
extern size_t bitfindv(const bitarray bits, const char *format, va_list ap);

extern void bitset(bitarray bits, ssize_t from, ssize_t to,
    const char *format, ...);
extern void bitsetv(bitarray bits, ssize_t from, ssize_t to,
    const char *format, va_list ap);
extern void bitclear(bitarray bits, ssize_t from, ssize_t to);

extern bitarray bitinsert(bitarray bits, ssize_t from,
    const char *format, ...);
extern bitarray bitinsertv(bitarray bits, ssize_t from,
    const char *format, va_list ap);

#ifdef __cplusplus
}
#endif

#endif /* __BITSCAN_H__ */

