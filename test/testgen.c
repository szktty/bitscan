#include <stdio.h>
#include <stdlib.h>
#include "testgen.h"

size_t
gencapa(size_t max)
{
  size_t capa;

  capa = (size_t)(abs(rand()) % (max+1));
  return capa > 0 ? capa : 1;
}

size_t
gensize(size_t capa)
{
  size_t size;

  size = (size_t)(abs(rand()) % (capa * 8));
  if (size == 0)
    size = 1;
  return size;
}

size_t
genpos(size_t capa, size_t size)
{
  if (size == capa * 8)
    return 0;
  else
    return (size_t)(abs(rand()) % (capa * 8 - size));
}

bool
genbool()
{
  return (bool)(abs(random()));
}

