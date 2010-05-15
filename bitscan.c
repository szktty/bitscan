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

#include <ctype.h>
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

/*
 * compiled format code
 *
 * struct format {
 *   uint8_t magic_number = 0xff;
 *   size_t nparams;
 *   struct param params[nparams]; 
 * };
 *
 * struct param {
 *   uint8_t value_type;
 *   union {
 *     long intval;
 *     unsigned long uintval;
 *     double floatval;
 *     char cval;
 *     struct {
 *       size_t size;
 *       char s[size];
 *     } strval;
 *   } value;
 *   uint8_t type_specifier;
 *   uint8_t byte_order;
 *   struct {
 *     uint8_t null_type = POS_NULL;
 *     struct {
 *       uint8_t type = POS_VALUE;
 *       size_t pos;
 *     } value;
 *     uint8_t var_type = POS_VAR;
 *     uint8_t ptr_type = POS_PTR;
 *     uint8_t cur_type = POS_CUR;
 *   } pos_type;
 *   struct {
 *     uint8_t null_type = NBITS_NULL;
 *     struct {
 *       uint8_t type = NBITS_VALUE;
 *       size_t pos;
 *     } value;
 *     uint8_t var_type = NBITS_VAR;
 *     uint8_t ptr_type = NBITS_PTR;
 *   } bits;
 * };
 *
 */

typedef enum VALUE_TYPE {
  VALUE_NULL,
  VALUE_NBASE,
  VALUE_INT,
  VALUE_UINT,
  VALUE_FLOAT,
  VALUE_STR
} VALUE_TYPE;

typedef enum TYPE_SPCR {
  TYPE_CHAR,            /* c */
  TYPE_UCHAR,           /* C */
  TYPE_SHORT,           /* s */
  TYPE_USHORT,          /* S */
  TYPE_INT,             /* i */
  TYPE_UINT,            /* I */
  TYPE_LONG,            /* l */
  TYPE_ULONG,           /* L */
  TYPE_LLONG,           /* q */
  TYPE_ULLONG,          /* Q */
  TYPE_FLOAT,           /* f */
  TYPE_DOUBLE,          /* d */
  TYPE_PTR,             /* ^ */
  TYPE_STR_NULL,        /* a */
  TYPE_STR_NONNULL,     /* A */
  TYPE_IGNORE,          /* # */
} TYPE_SPCR;

typedef enum ENDIAN {
  ENDIAN_NATIVE,
  ENDIAN_LITTLE,
  ENDIAN_BIG,
  ENDIAN_NETWORK,
} ENDIAN;

typedef enum POS_TYPE {
  POS_NULL,
  POS_VALUE,
  POS_VAR,              /* ? */
  POS_PTR,              /* ^ */
  POS_CUR,              /* + */
} POS_TYPE;

typedef enum NBITS_TYPE {
  NBITS_NULL,
  NBITS_VALUE,
  NBITS_VAR,            /* ? */
  NBITS_PTR,            /* ^ */
} NBITS_TYPE;

#define MAGIC 0xff

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
bitrotate(void *dest, size_t destpos, const void *src, size_t srcpos,
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
      for (i = 0; i < size - shift; i++) {
        SET(dest, destpos + i, GET(src, srcpos + i + shift));
      }
      for (; i < size; i++) {
        SET(dest, destpos + i, GET(src, srcpos + i - (size - shift)));
      }
    } else {
      for (i = 0; i < shift; i++) {
        SET(dest, destpos + i, GET(src, srcpos + size - shift + i));
      }
      for (i = shift; i < size; i++) {
        SET(dest, destpos + i, GET(src, srcpos + i - shift));
      }
    }
  } else {
    temp = malloc(shift / 8 + 1);
    bitcpy(temp, 0, src, srcpos, size);

    if (left) {
      for (i = 0; i < size - shift; i++) {
        SET(dest, destpos + i, GET(temp, i + shift));
      }
      for (; i < size; i++) {
        SET(dest, destpos + i, GET(temp, i - (size - shift)));
      }
    } else {
      for (i = 0; i < shift; i++) {
        SET(dest, destpos + i, GET(temp, size - shift + i));
      }
      for (; i < size; i++) {
        SET(dest, destpos + i, GET(temp, i - shift));
      }
    }
    free(temp);
  }
}

void
bitlrotate(void *dest, size_t destpos, const void *src, size_t srcpos,
    size_t size, size_t shift)
{
  bitrotate(dest, destpos, src, srcpos, size, shift, true);
}

void
bitrrotate(void *dest, size_t destpos, const void *src, size_t srcpos,
    size_t size, size_t shift)
{
  bitrotate(dest, destpos, src, srcpos, size, shift, false);
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

char *
bitcompilef(const char *format)
{
  const char *s;
  uint8_t *code = NULL, *wcode;
  bool build = false, neg;
  size_t codesize, nparams = 0, posval = 0, nbitsval = 0;
  uint8_t valtype, spcr = 0, ordertype = 0, postype = 0, nbitstype = 0, v;
  unsigned long base, intval;
  long uintval;

  /* magic + write_size + nparams */
  codesize = 1 + sizeof(size_t) * 2;

parse:
  s = format;
  while (*s != '\0') {
    switch (*s) {
    case ' ':
      s++;
      continue;

    case '%':
      break;

    case '\'':
      /* TODO: character */

    case '"': case '|':
      /* TODO: string */

    case '.': case '-':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      /* int, float literals */
      codesize += 1; /* value_type */
      valtype = VALUE_NULL;
      neg = false;

      if (*s == '-') {
        neg = true;
        s++;
      }

      /* for xx#xxxx */
      base = 0;
      while (isdigit(*s)) {
        base = (base * 10) + (*s - '0');
        s++;
      }

      if (*s == '#') {
        valtype = VALUE_NBASE;
        codesize += sizeof(size_t);
        s++; /* # */

        if (neg)
          intval = -1;
        else
          uintval = 0;
        while (isdigit(*s) || isalpha(*s)) {
          if (isdigit(*s))
            v = *s - '0';
          else if (isupper(*s))
            v = *s - 'A';
          else
            v = *s - 'a';

          if (neg)
            intval = (intval * base) + v;
          else
            uintval = (uintval * base) + v;
          s++;
        }
      } else if (*s == '.') {
        valtype = VALUE_FLOAT;
        codesize += sizeof(double);
        /* TODO */

      } else {
        codesize += sizeof(long);
        if (neg) {
          valtype = VALUE_INT;
          if (neg) {
            intval = -1;
            s++;
          } else
            intval = 0;

          while (isdigit(*s)) {
            intval = (intval * 10) + (*s - '0');
            s++;
          }
        } else {
          valtype = VALUE_UINT;
          uintval = 0;
          while (isdigit(*s)) {
            uintval = (uintval * 10) + (*s - '0');
            s++;
          }
        }
      }
      break;

    default:
      goto error;
    }

    /* parse type specifier */
    if (*s != '%') {
      goto error;
    } else {
      s++; /* '%' */
      nparams++;
      codesize += 4; /* type_specifier, byte_order, pos, nbits */

      switch (*s) {
      case 'c':
        spcr = TYPE_CHAR;
        break;
      case 'C':
        spcr = TYPE_UCHAR;
        break;
      case 's':
        spcr = TYPE_SHORT;
        break;
      case 'S':
        spcr = TYPE_USHORT;
        break;
      case 'i':
        spcr = TYPE_INT;
        break;
      case 'I':
        spcr = TYPE_UINT;
        break;
      case 'l':
        spcr = TYPE_LONG;
        break;
      case 'L':
        spcr = TYPE_ULONG;
        break;
      case 'q':
        spcr = TYPE_LLONG;
        break;
      case 'Q':
        spcr = TYPE_ULLONG;
        break;
      case 'f':
        spcr = TYPE_FLOAT;
        break;
      case 'd':
        spcr = TYPE_DOUBLE;
        break;
      case '^':
        spcr = TYPE_PTR;
        break;
      case 'a':
        spcr = TYPE_STR_NULL;
        break;
      case 'A':
        spcr = TYPE_STR_NONNULL;
        break;
      case '#':
        spcr = TYPE_IGNORE;
        break;
      default:
        goto error;
      }
      s++; /* type specifier */

      /* byte order */
      switch (*s) {
      case '=':
        ordertype = ENDIAN_NATIVE;
        s++;
        break;
      case '>': case '!':
        ordertype = ENDIAN_BIG;
        s++;
        break;
      case '<':
        ordertype = ENDIAN_LITTLE;
        s++;
        break;
      default:
        ordertype = ENDIAN_NATIVE;
        break;
      }

      /* pos */
      postype = POS_NULL;
      if (*s == '@') {
        s++;
        switch (*s) {
        case '?':
          postype = POS_VAR;
          s++;
          break;
        case '^':
          postype = POS_PTR;
          s++;
          break;
        case '+':
          postype = POS_CUR;
          s++;
          break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          postype = POS_VALUE;
          posval = *s - '0';
          s++;
          while (isdigit(*s)) {
            posval = (posval * 10) + (*s - '0');
            s++;
          }
          codesize += sizeof(size_t);
          break;
        default:
          /* error */
          return NULL;
        }
      }

      /* bits */
      nbitstype = NBITS_NULL;
      if (*s == ':') {
        s++;
        switch (*s) {
        case '?':
          nbitstype = NBITS_VAR;
          break;
        case '^':
          nbitstype = NBITS_PTR;
          break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          nbitstype = NBITS_VALUE;
          nbitsval = *s - '0';
          s++;
          while (isdigit(*s)) {
            nbitsval = (posval * 10) + (*s - '0');
            s++;
          }
          codesize += sizeof(size_t);
          break;
        default:
          /* error */
          return NULL;
        }
      }
    }

    if (build) {
      *wcode++ = valtype;
      switch (valtype) {
      case VALUE_NBASE:
        if (neg) {
          memcpy(wcode, &intval, sizeof(long));
          wcode += sizeof(long);
        } else {
          memcpy(wcode, &uintval, sizeof(unsigned long));
          wcode += sizeof(unsigned long);
        }
        break;

      case VALUE_FLOAT:
        /* TODO */
        break;

      case VALUE_INT:
        /* TODO */
        break;

      case VALUE_UINT:
        /* TODO */
        break;
      }

      /* type specifier */
      s += 2;
      *wcode++ = spcr;

      /* byte order */
      *wcode++ = ordertype;

      /* pos */
      *wcode++ = postype;
      if (postype == POS_VALUE) {
        *((size_t *)wcode) = posval;
        wcode += sizeof(size_t);
      }

      /* bits */
      *wcode++ = nbitstype;
      if (nbitstype == NBITS_VALUE) {
        *((size_t *)wcode) = nbitsval;
        wcode += sizeof(size_t);
      }
    }
  }

  if (!build) {
    build = true;
    code = wcode = (uint8_t *)malloc(codesize);
    *wcode++ = MAGIC;
    wcode += sizeof(size_t); /* nparams */
    goto parse;
  }
  *((size_t *)code+1) = nparams;
  return (char *)code;

error:
  if (build)
    free(code);
  return NULL;
}

