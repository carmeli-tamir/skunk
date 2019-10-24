#ifndef SKUNK_H
#define SKUNK_H

#include "skunk.pb-c.h"

#include <linux/types.h>

#define SKUNK_DEVICE "skunk"

typedef u64 (*ptrRet64StrArg)(char *arg1);
typedef u64 (*ptrRet64Int8bArg)(u64 arg1);

long parse_user_buffer_and_call_function(char *buffer, u32 length, Skunk__ReturnValue *ret);

#endif /* SKUNK_H */