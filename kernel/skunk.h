#ifndef SKUNK_H
#define SKUNK_H

#include "skunk.pb-c.h"

#include <linux/types.h>

#define SKUNK_DEVICE "skunk"

typedef void * (*ptrRetOneArg)(void *arg1);
typedef void * (*ptrRetTwoArg)(void *arg1, void *arg2);
typedef void * (*ptrRetThreeArg)(void *arg1, void *arg2, void *arg3);

typedef u64 (*ptrRet64OneArg)(void *arg1);

long parse_user_buffer_and_call_function(char *buffer, u32 length, Skunk__ReturnValue *ret);

#endif /* SKUNK_H */