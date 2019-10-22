#include "skunk.h"

#include <linux/errno.h>
#include <linux/kallsyms.h>

#include "skunk.pb-c.h"

static void parse_proto_and_call_function_ret_64_stringArg1(char *buffer, u32 length, Skunk__ReturnValue *ret)
{
    Skunk__FuncWith1Arg *func_1arg;
    u32 message_size;
    u32 offset = 0;
    unsigned long func_addr;

    message_size = *((u32*)buffer);
    offset = sizeof(message_size);
    if (message_size < 0 || message_size > length - offset) {
        ret->status = SKUNK__RETURN_VALUE__CALL_STATUS__BadProtobufMessage;
    }

    func_1arg = skunk__func_with_1_arg__unpack(NULL, message_size, buffer + sizeof(message_size));
    if (NULL == func_1arg) {
        ret->status = SKUNK__RETURN_VALUE__CALL_STATUS__BadProtobufMessage;
    }

    func_addr = kallsyms_lookup_name(func_1arg->name);
    if (0 == func_addr) {
        ret->status = SKUNK__RETURN_VALUE__CALL_STATUS__FunctionDoesntExist;
    }
    ret->ret64 = (int64_t)((ptrRet64OneArg)func_addr)(func_1arg->arg1);
    ret->has_ret64 = 1;
}

static void parse_proto_and_call_function_ret_64_fourByteArg1(char *buffer, u32 length)
{
    pr_info("Hello four byte arg1");
}

long parse_user_buffer_and_call_function(char *buffer, u32 length, Skunk__ReturnValue *ret)
{
    Skunk__FunctionType *func_type;
    u32 message_size;
    u32 offset = 0;

    message_size = *((u32*)buffer);
    offset = sizeof(message_size);
    if (message_size < 0 || message_size > length - offset) {
        return message_size;
    }

    func_type = skunk__function_type__unpack(NULL, message_size, buffer + sizeof(message_size));
    if (NULL == func_type) {
        return -EINVAL;
    }
    
    if (func_type->ret != SKUNK__FUNCTION_TYPE__RETURN_TYPE__eightByte) {
        pr_info("Currently supporting only 4 bytes return value");
        skunk__function_type__free_unpacked(func_type, NULL);
        return -EINVAL;
    }

    switch (func_type->args)
    {
        case SKUNK__FUNCTION_TYPE__ARGUMENTS__stringArg1:
            parse_proto_and_call_function_ret_64_stringArg1(buffer + offset + message_size, length - offset, ret);
        break;
        case SKUNK__FUNCTION_TYPE__ARGUMENTS__fourByteArg1:
            parse_proto_and_call_function_ret_64_fourByteArg1(buffer + offset + message_size, length - offset);
        break;
    default:
        break;
    }
    
    skunk__function_type__free_unpacked(func_type, NULL);
    return 0;
}
