#include "test.h"

static void **
datatestmake()
{
}

static void
testmake(void *data)
{
  testassert(true, "failed");
  testassert(false, "failed ok");
}

void
inittestmake()
{
  TESTADD(testmake);
}

