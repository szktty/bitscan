#ifndef __TEST_H__
#define __TEST_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TESTADD(name)   testadd(#name, data##name, name, free##name)

extern void testrun();
extern void testadd(const char *name, void **(*provider)(),
    void (*tester)(void *data), void (*free)(void *data));
extern void testassert(bool cond, const char *msg);
extern void testfail(const char *msg);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_H__ */

