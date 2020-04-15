#include "skunk.h"
#include "skunk.pb-c.h"
#include "mock_dispatcher.h"

#include <linux/errno.h>
#include <linux/kallsyms.h>

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

static void call_poc_function_4Arg(char *name, Skunk__FunctionCall__ReturnType ret_type,
                                Skunk__Argument *arg0, Skunk__Argument *arg1, Skunk__Argument *arg2, Skunk__Argument *arg3
                                , Skunk__ReturnValue *ret){
    unsigned long func_addr;

    func_addr = kallsyms_lookup_name(name);
    if (0 == func_addr) {
        ret->status = SKUNK__RETURN_VALUE__CALL_STATUS__FunctionDoesntExist;
        return;
    }

    switch (function_call_id(ret_type, 4, arg0->type, arg1->type, arg2->type, arg3->type, 0, 0, 0))
    {
    case 0x2110000:
        ret->four_byte = (typeof(ret->four_byte))((ptrRetfour_byte_stringArgeight_byteArgeight_byteArgfour_byteArg)func_addr)(arg0->arg_string, arg1->arg_eight_byte, arg2->arg_eight_byte, arg3->arg_four_byte);
        ret->has_four_byte = 1;
        pr_info("Ret value is %d", ret->four_byte);
        break;
    default:
        break;
    }
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

    if (start_mocking() < 0) {
        skunk_ret.status = SKUNK__RETURN_VALUE__CALL_STATUS__MockingError;
        goto cleanup;
    }

    switch (func_call->numberofarguments)
    {
        case 1:
            call_function_1Arg(func_call->name, func_call->returntype, func_call->arg1, &skunk_ret);
            break;
        case 2:
            call_function_2Arg(func_call->name, func_call->returntype, func_call->arg1, func_call->arg2, &skunk_ret);
            break;
        case 4:
            call_poc_function_4Arg(func_call->name, func_call->returntype,
             func_call->arg1, func_call->arg2, func_call->arg3, func_call->arg4, &skunk_ret);
        break;
    default:
        break;
    }

 cleanup:
    stop_mocking();

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
