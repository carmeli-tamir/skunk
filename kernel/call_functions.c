#include "skunk.h"

#include <linux/errno.h>
#include <linux/kallsyms.h>

#include "skunk.pb-c.h"

u32 function_call_id(Skunk__FunctionCall__ReturnType ret, u8 numOfArguments,
                     Skunk__Argument__ArgumentType arg1, Skunk__Argument__ArgumentType arg2,
                     Skunk__Argument__ArgumentType arg3, Skunk__Argument__ArgumentType arg4,
                     Skunk__Argument__ArgumentType arg5, Skunk__Argument__ArgumentType arg6,
                     Skunk__Argument__ArgumentType arg7)
{
    u32 id = 0;
    if (ret >= 0x10 || numOfArguments >= 8 ||
        arg1 >= 0x10 || arg2 >= 0x10 || arg3 >= 0x10 || arg4 >= 0x10 || arg5 >= 0x10 ||
        arg6 >= 0x10 || arg7 >= 0x10 ) {
            pr_info("Skunk got illegal function call id");
            return INT_MAX;
        }

    id = ret << 28;
    id |= numOfArguments >= 1 ? arg1 << 24 : 0;
    id |= numOfArguments >= 2 ? arg2 << 20 : 0;
    id |= numOfArguments >= 3 ? arg3 << 16 : 0;
    id |= numOfArguments >= 4 ? arg4 << 12 : 0;
    id |= numOfArguments >= 5 ? arg5 << 8 : 0;
    id |= numOfArguments >= 6 ? arg6 << 4 : 0;
    id |= numOfArguments >= 7 ? arg7 << 0 : 0;

    return id;
}

static void call_function_oneArg(char *name, Skunk__FunctionCall__ReturnType ret_type,
                                Skunk__Argument * arg1, Skunk__ReturnValue *ret)
{
    unsigned long func_addr;

    func_addr = kallsyms_lookup_name(name);
    if (0 == func_addr) {
        ret->status = SKUNK__RETURN_VALUE__CALL_STATUS__FunctionDoesntExist;
    }

    switch (function_call_id(ret_type, 1, arg1->type, 0, 0, 0, 0, 0, 0))
    {
    case 0x01000000:
        ret->ret64 = (int64_t)((ptrRet64StrArg)func_addr)(arg1->argstring);
        break;

    case 0x00000000:
        ret->ret64 = (int64_t)((ptrRet64Int8bArg)func_addr)(arg1->argint8b);
        break;

    default:
        break;
    }

    ret->has_ret64 = 1;
}

static void call_function_twoArg(char *name, Skunk__FunctionCall__ReturnType ret_type,
                                Skunk__Argument * arg1, Skunk__Argument * arg2, Skunk__ReturnValue *ret)
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

    switch (func_call->numberofarguments)
    {
        case 1:
            call_function_oneArg(func_call->name, func_call->returntype, func_call->arg1, ret);
        break;
        case 2:
            call_function_twoArg(func_call->name, func_call->returntype, func_call->arg1, func_call->arg2, ret);
        break;
    default:
        break;
    }
    
    skunk__function_call__free_unpacked(func_call, NULL);
    return 0;
}
