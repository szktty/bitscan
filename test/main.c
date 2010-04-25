#include <stdio.h>
#include "test.h"

extern void inittestmake();

int
main(int argc, char **argv)
{
  inittestmake();
  testrun();
  return 0;
}

