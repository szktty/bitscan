#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "test.h"

extern void inittestrawbitcmp();
extern void inittestrawbitget();
extern void inittestrawbitset();
extern void inittestrawbitrand();
extern void inittestrawbitclear();
extern void inittestrawbitcpy();
extern void inittestrawbitop();
extern void inittestrawbitshift();
extern void inittestmake();

int
main(int argc, char **argv)
{
  srand((unsigned int)time(NULL));
  inittestrawbitclear();
  inittestrawbitcmp();
  inittestrawbitcpy();
  inittestrawbitget();
  inittestrawbitop();
  inittestrawbitrand();
  inittestrawbitset();
  inittestrawbitshift();
  inittestmake();
  testrun();
  return 0;
}

