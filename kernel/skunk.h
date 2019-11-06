#ifndef SKUNK_H
#define SKUNK_H

#include "skunk.pb-c.h"

#include <linux/types.h>

#define SKUNK_DEVICE "skunk"

// One arg
typedef u64 (*ptrReteight_byte_stringArg)(char *arg1);
typedef u64 (*ptrReteight_byte_eight_byteArg)(u64 arg1);

typedef char* (*ptrRetstring_stringArg)(char *arg1);
typedef char* (*ptrRetstring_eight_byteArg)(u64 arg1);

// Two args
typedef u64 (*ptrReteight_byte_eight_byteArgeight_byteArg)(u64 arg1, u64 arg2);
typedef u64 (*ptrReteight_byte_eight_byteArgstringArg)(u64 arg1, char *arg2);
typedef u64 (*ptrReteight_byte_stringArgeight_byteArg)(char *arg1, u64 arg2);
typedef u64 (*ptrReteight_byte_stringArgstringArg)(char *arg1, char *arg2);

typedef char* (*ptrRetstring_eight_byteArgeight_byteArg)(u64 arg1, u64 arg2);
typedef char* (*ptrRetstring_eight_byteArgstringArg)(u64 arg1, char *arg2);
typedef char* (*ptrRetstring_stringArgeight_byteArg)(char *arg1, u64 arg2);
typedef char* (*ptrRetstring_stringArgstringArg)(char *arg1, char *arg2);


u32 function_call_id(Skunk__FunctionCall__ReturnType ret, u8 numOfArguments,
                     Skunk__Argument__ArgumentType arg1, Skunk__Argument__ArgumentType arg2,
                     Skunk__Argument__ArgumentType arg3, Skunk__Argument__ArgumentType arg4,
                     Skunk__Argument__ArgumentType arg5, Skunk__Argument__ArgumentType arg6,
                     Skunk__Argument__ArgumentType arg7);

long parse_user_buffer_and_call_function(char *buffer, u32 *length);

void call_function_1Arg(char *name, Skunk__FunctionCall__ReturnType ret_type,
                                                Skunk__Argument * arg1, Skunk__ReturnValue *ret);

void call_function_2Arg(char *name, Skunk__FunctionCall__ReturnType ret_type,
                                Skunk__Argument *arg0, Skunk__Argument *arg1, Skunk__ReturnValue *ret);

#endif /* SKUNK_H */