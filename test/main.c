#include <stdio.h>
#include <time.h>
#include "test.h"

extern void inittestrawbitcmp();
extern void inittestrawbitget();
extern void inittestrawbitset();
extern void inittestrawbitrand();
extern void inittestrawbitclear();
extern void inittestrawbitcpy();
extern void inittestmake();

int
main(int argc, char **argv)
{
  srand((unsigned int)time(NULL));
  /*
  inittestrawbitcmp();
  inittestrawbitget();
  inittestrawbitset();
  inittestrawbitrand();
  inittestrawbitclear();
  */
  inittestrawbitcpy();
  inittestmake();
  testrun();
  return 0;
}

