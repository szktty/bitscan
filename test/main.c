#include <stdio.h>
#include "test.h"

extern void inittestrawbitcmp();
extern void inittestrawbitget();
extern void inittestrawbitrand();
extern void inittestmake();

int
main(int argc, char **argv)
{
  inittestrawbitcmp();
  inittestrawbitget();
  inittestrawbitrand();
  inittestmake();
  testrun();
  return 0;
}

