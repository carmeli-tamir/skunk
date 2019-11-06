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

long parse_user_buffer_and_call_function(char *buffer, u32 *length)
{
    Skunk__FunctionCall *func_call;
    Skunk__ReturnValue skunk_ret;
    u32 ret_message_size;
    long ret = 0;

    func_call = skunk__function_call__unpack(NULL, *length, buffer);
    if (NULL == func_call) {
        return -EINVAL;
    }

    skunk__return_value__init(&skunk_ret);

    switch (func_call->numberofarguments)
    {
        case 1:
            call_function_1Arg(func_call->name, func_call->returntype, func_call->arg1, &skunk_ret);
        break;
        case 2:
            call_function_2Arg(func_call->name, func_call->returntype, func_call->arg1, func_call->arg2, &skunk_ret);
        break;
    default:
        break;
    }
    // Return must be packed before freeing func_call, since a memory in ret can point to arguments in func_call.
    ret_message_size = skunk__return_value__get_packed_size(&skunk_ret);
    if (ret_message_size > *length) {
        pr_info("Buffer is too small");
        ret = -ENOMEM;
        goto out;
    }
    *length = ret_message_size;
    skunk__return_value__pack(&skunk_ret, buffer);

 out:   
    skunk__function_call__free_unpacked(func_call, NULL);
    return ret;
}
