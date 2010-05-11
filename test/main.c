#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "test.h"

extern void inittestbitcmp();
extern void inittestbitget();
extern void inittestbitset();
extern void inittestbitrand();
extern void inittestbitclear();
extern void inittestbitcpy();
extern void inittestbitop();
extern void inittestbitrotate();
extern void inittestbitshift();

int
main(int argc, char **argv)
{
  srand((unsigned int)time(NULL));
  inittestbitclear();
  inittestbitcmp();
  inittestbitcpy();
  inittestbitget();
  inittestbitop();
  inittestbitrand();
  inittestbitrotate();
  inittestbitset();
  inittestbitshift();
  testrun();
  return 0;
}

