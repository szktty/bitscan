#include <stdio.h>
#include "test.h"

extern void inittestrawbitcmp();
extern void inittestrawbitget();
extern void inittestmake();

int
main(int argc, char **argv)
{
  inittestrawbitcmp();
  inittestrawbitget();
  inittestmake();
  testrun();
  return 0;
}

