#include <stdio.h>
#include "test.h"

extern void inittestrawbitcmp();
extern void inittestmake();

int
main(int argc, char **argv)
{
  inittestrawbitcmp();
  inittestmake();
  testrun();
  return 0;
}

