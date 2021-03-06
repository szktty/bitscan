#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "test.h"

typedef struct testcase testcase;
typedef struct faillog  faillog;

struct testcase {
  char *name;
  void **(*provider)();
  void (*tester)(void *data);
  void (*free)(void *data);
  testcase *next;
};

struct faillog {
  testcase *test;
  unsigned int n;
  char *msg;
  faillog *next;
};

static unsigned int tests;
static unsigned int successes;
static unsigned int fails;
static testcase *testcases;
static testcase *lastcase;
static faillog *faillogs;
static faillog *lastfail;
static testcase *curtest;
static unsigned int curtests;

void
testrun()
{
  void **data, **orig;
  faillog *fail;

  curtest = testcases;
  while (curtest != NULL) {
    printf("testing %s...\n", curtest->name);

    curtests = 0;
    orig = data = curtest->provider();
    if (*data != NULL) {
      while (*data != NULL) {
        curtest->tester(*data);
        if (curtest->free != NULL)
          curtest->free(*data);
        data++;
      }
    } else
      curtest->tester(NULL);
    free(orig);
    curtest = curtest->next;
  }

  fail = faillogs;
  while (fail != NULL) {
    printf("%s (%u):\n    %s\n\n", fail->test->name, fail->n,
        fail->msg != NULL ? fail->msg : "failed");
    fail = fail->next;
  }

  printf("tests: %u, successes: %u, fails: %u\n", tests, successes, fails);
}

void
testadd(const char *name, void **(*provider)(),
    void (*tester)(void *data), void (*free)(void *data))
{
  testcase *new;

  tests++;
  new = (testcase *)malloc(sizeof(testcase));
  new->name = strdup(name);
  new->provider = provider;
  new->tester = tester;
  new->free = free;
  new->next = NULL;
  if (lastcase != NULL)
    lastcase->next = new;
  else
    testcases = new;
  lastcase = new;
}

void
testassert(bool cond, const char *msg)
{
  faillog *fail;

  curtests++;
  if (cond) {
    successes++;
  } else {
    fails++;

    fail = (faillog *)malloc(sizeof(faillog));
    fail->test = curtest;
    fail->n = curtests;
    if (msg != NULL)
      fail->msg = strdup(msg);
    fail->next = NULL;
    if (lastfail != NULL)
      lastfail->next = fail;
    else
      faillogs = fail;
    lastfail = fail;
  }
}

void
testfail(const char *msg)
{
  testassert(false, msg);
}

