#!/usr/bin/env python3

import argparse
import itertools

import skunk_pb2

OUTPUT_FILE = "function_call/function_calls.c"
MAX_ARGUMENTS = 7

FUNCTION_CALL_TEMPLATE = """
    case {id}:
        ret->{ret_type} = (typeof(ret->{ret_type}))((ptrRet{ret_type}_{arg_types})func_addr)({arg_calls});
        ret->has_{ret_type} = 1;
        break;
"""

RETURN_TYPES_DICT = dict(skunk_pb2.FunctionCall.ReturnType.items())
ARG_TYPES_DICT = dict(skunk_pb2.Argument.ArgumentType.items())

def function_call_id(return_type, number_of_arguments, 
                    arg1_type=None, arg2_type=None, arg3_type=None, arg4_type=None,
                    arg5_type=None, arg6_type=None, arg7_type=None):
    return_code = RETURN_TYPES_DICT[return_type]
    arg1_code = ARG_TYPES_DICT[arg1_type] if arg1_type is not None else 0
    arg2_code = ARG_TYPES_DICT[arg2_type] if arg2_type is not None else 0
    arg3_code = ARG_TYPES_DICT[arg3_type] if arg3_type is not None else 0
    arg4_code = ARG_TYPES_DICT[arg4_type] if arg4_type is not None else 0
    arg5_code = ARG_TYPES_DICT[arg5_type] if arg5_type is not None else 0
    arg6_code = ARG_TYPES_DICT[arg6_type] if arg6_type is not None else 0
    arg7_code = ARG_TYPES_DICT[arg7_type] if arg7_type is not None else 0
    
    call_id = (return_code << 28) + (arg1_code << 24) + (arg2_code << 20) + (arg3_code << 16) \
            + (arg4_code << 12) + (arg5_code << 8) + (arg6_code << 4) + arg7_code 
    return hex(call_id)

def generate_cases(number_of_arguments):
    arg_calls_template = ', '.join(["arg{i}->arg_{type}".format(i=i,type='{{{}}}'.format(i)) for i in range(number_of_arguments)])
    single_argument_types = skunk_pb2.Argument.ArgumentType.keys()
    all_argument_types = [single_argument_types for i in range(number_of_arguments)]

    all_return_types = skunk_pb2.FunctionCall.ReturnType.keys()
    
    all_function_call_cases = itertools.product(all_return_types, *all_argument_types)
    
    s = ''
    for function_call_cases in all_function_call_cases:
        arg_calls = arg_calls_template.format(*function_call_cases[1:])
        arg_types = ''.join([x + 'Arg' for x in function_call_cases[1:]])
        call_id = str(function_call_id(function_call_cases[0], number_of_arguments, *function_call_cases[1:]))

        s+=FUNCTION_CALL_TEMPLATE.format(ret_type=function_call_cases[0], arg_calls=arg_calls, id=call_id, arg_types=arg_types)
    return s

def generate_function_call(number_of_arguments):
    argument_list = ', '.join([f'Skunk__Argument *arg{n}' for n in range(number_of_arguments)])
    argument_types = ', '.join([f'arg{n}->type' for n in range(number_of_arguments)]) + \
                     (', ' if number_of_arguments < MAX_ARGUMENTS else '') + \
                     ', '.join(['0' for n in range(MAX_ARGUMENTS - number_of_arguments)])
    cases = generate_cases(number_of_arguments)
    return f"""
void call_function_{number_of_arguments}Arg(char *name, Skunk__FunctionCall__ReturnType ret_type,
                                {argument_list}, Skunk__ReturnValue *ret)
{{
    unsigned long func_addr;

    func_addr = kallsyms_lookup_name(name);
    if (0 == func_addr) {{
        ret->status = SKUNK__RETURN_VALUE__CALL_STATUS__FunctionDoesntExist;
    }}

    switch (function_call_id(ret_type, {number_of_arguments}, {argument_types}))
    {{
    {cases}

    default:
        break;
    }}
}}
"""

HEAD = """#include <linux/kallsyms.h>

#include "skunk.h"

#include "skunk.pb-c.h"

"""
def generate_function_calls(output_file, max_arg):
    res = HEAD
    for i in range(1, max_arg + 1):
        res += generate_function_call(i)
    with open(output_file, 'w') as output:
        output.write(res)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Creates a c code that calls functions in linux kernel")
    parser.add_argument("-o", "--output", type=str, help="Output file path", default=OUTPUT_FILE)
    parser.add_argument("-n", "--maxarg",type=int, help="Max number of arguments", default=2)
    
    args = parser.parse_args()

    generate_function_calls(args.output, args.maxarg)