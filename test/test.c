#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "test.h"

typedef struct testcase testcase;
typedef struct faillog  faillog;

struct testcase {
  char *name;
  void **(*provider)();
  void (*tester)(void *data);
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
  size_t i;
  void **data;
  faillog *fail;

  curtest = testcases;
  while (curtest != NULL) {
    printf("%s:", curtest->name);
    for (i = 0; i < 23 - strlen(curtest->name); i++)
      printf(" ");

    curtests = 0;
    data = curtest->provider();
    if (*data != NULL) {
      while (*data != NULL) {
        curtest->tester(*data);
        data++;
      }
    } else
      curtest->tester(NULL);
    printf("\n");
    curtest = curtest->next;
  }

  printf("\n");
  fail = faillogs;
  while (fail != NULL) {
    printf("%s (%u):\n    %s\n\n", fail->test->name, fail->n,
        fail->msg != NULL ? fail->msg : "failed");
    fail = fail->next;
  }

  printf("\ntests: %u, successes: %u, fails: %u\n", tests, successes, fails);
}

void
testadd(const char *name, void **(*provider)(), void (*tester)(void *data))
{
  testcase *new;

  tests++;
  new = (testcase *)malloc(sizeof(testcase));
  new->name = strdup(name);
  new->provider = provider;
  new->tester = tester;
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
    printf(".");
  } else {
    fails++;
    printf("F");

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

