#include "skunk.h"

#include <linux/errno.h>
#include <linux/kallsyms.h>

#include "skunk.pb-c.h"

long parse_user_buffer_and_call_function(char *buffer, u32 length)
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
    
    if (func_type->ret != SKUNK__FUNCTION_TYPE__RETURN_TYPE__fourByte) {
        //TODO: Handle non 4 bytes return values
        pr_info("Currently supporting only 4 bytes return value");
        return -EINVAL;
    }

    switch (func_type->args)
    {
        case SKUNK__FUNCTION_TYPE__ARGUMENTS__stringArg1:
            //TODO: Pack return value
            parse_proto_and_call_function_stringArg1(buffer + offset + message_size, length - offset);
        break;
        case SKUNK__FUNCTION_TYPE__ARGUMENTS__fourByteArg1:
            parse_proto_and_call_function_fourByteArg1(buffer + offset + message_size, length - offset);
        break;
    default:
        break;
    }
    
    return 0;
}


u32 parse_proto_and_call_function_stringArg1(char *buffer, u32 length)
{
    Skunk__FuncWith1Arg *func_1arg;
    u32 message_size;
    u32 offset = 0;
    u64 ret = 0;
    unsigned long func_addr;

    message_size = *((u32*)buffer);
    offset = sizeof(message_size);
    if (message_size < 0 || message_size > length - offset) {
        return message_size;
    }

    func_1arg = skunk__func_with_1_arg__unpack(NULL, message_size, buffer + sizeof(message_size));
    if (NULL == func_1arg) {
        return -EINVAL;
    }

    func_addr = kallsyms_lookup_name(func_1arg->name);
    if (0 == func_addr) {
        return -EINVAL;
    }

    ret =(u64) ((ptrRet64OneArg)func_addr)(func_1arg->arg1);

    pr_info("Got ret value of %p", (void*)ret);

    return 0;
}

u32 parse_proto_and_call_function_fourByteArg1(char *buffer, u32 length)
{
    pr_info("Hello four byte arg1");
    return 0;
}