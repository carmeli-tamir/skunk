#include "skunk.h"

#include <linux/errno.h>
#include <linux/kallsyms.h>

#include "skunk.pb-c.h"

static void parse_proto_and_call_function_ret_64_oneArg(char *name, Skunk__Argument * arg1, Skunk__ReturnValue *ret)
{
    unsigned long func_addr;

    func_addr = kallsyms_lookup_name(name);
    if (0 == func_addr) {
        ret->status = SKUNK__RETURN_VALUE__CALL_STATUS__FunctionDoesntExist;
    }

    switch (arg1->type)
    {
    case SKUNK__ARGUMENT__ARGUMENT_TYPE__stringArg:
        ret->ret64 = (int64_t)((ptrRet64StrArg)func_addr)(arg1->argstring);
        break;

    case SKUNK__ARGUMENT__ARGUMENT_TYPE__eightByteArg:
        ret->ret64 = (int64_t)((ptrRet64Int8bArg)func_addr)(arg1->argint8b);
        break;

    default:
        break;
    }

    ret->has_ret64 = 1;
}

static void parse_proto_and_call_function_ret_64_twoArg(char *name, Skunk__Argument * arg1, Skunk__Argument * arg2, Skunk__ReturnValue *ret)
{
    pr_info("Hello Function two args");
}

long parse_user_buffer_and_call_function(char *buffer, u32 length, Skunk__ReturnValue *ret)
{
    Skunk__FunctionCall *func_call;

    func_call = skunk__function_call__unpack(NULL, length, buffer);
    if (NULL == func_call) {
        return -EINVAL;
    }
    
    if (func_call->returntype != SKUNK__FUNCTION_CALL__RETURN_TYPE__eightByte) {
        pr_info("Currently supporting only 8 bytes return value");
        skunk__function_call__free_unpacked(func_call, NULL);
        return -EINVAL;
    }

    switch (func_call->numberofarguments)
    {
        case 1:
            parse_proto_and_call_function_ret_64_oneArg(func_call->name, func_call->arg1, ret);
        break;
        case 2:
            parse_proto_and_call_function_ret_64_twoArg(func_call->name, func_call->arg1, func_call->arg2, ret);
        break;
    default:
        break;
    }
    
    skunk__function_call__free_unpacked(func_call, NULL);
    return 0;
}
