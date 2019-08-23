#ifndef SKUNK_H
#define SKUNK_H

#include <linux/types.h>

#define SKUNK_DEVICE "skunk"

long parse_user_buffer_and_call_function(char *buffer, u32 length);

u32 parse_proto_and_call_function_stringArg1(char *buffer, u32 length);

u32 parse_proto_and_call_function_fourByteArg1(char *buffer, u32 length);

#endif /* SKUNK_H */