#!/usr/bin/env python3
import fcntl
import struct

import skunk_pb2

def binary_length_and_value(buffer, buffer_length):
    return struct.pack('I', buffer_length) + buffer

def call_function_one_arg(device, fname, num_args, fret, arg1, type1):
    func_with_one_arg = skunk_pb2.FunctionCall()
    func_with_one_arg.returnType = fret
    func_with_one_arg.numberOfArguments = num_args
    func_with_one_arg.name = fname
    func_with_one_arg.arg1.type = type1

    if type1 == skunk_pb2.Argument.stringArg:
        func_with_one_arg.arg1.argString = arg1
    elif type1 == skunk_pb2.Argument.eightByteArg:
        func_with_one_arg.arg1.argInt8b = arg1
    else:
        raise ValueError("Unsupported argument type")

    func_with_one_arg_binary = binary_length_and_value(func_with_one_arg.SerializeToString(), func_with_one_arg.ByteSize())

    call_result = fcntl.ioctl(device, 0xc008ee00, func_with_one_arg_binary)

    length_of_size_field = struct.calcsize('I')
    return_length, = struct.unpack('I', call_result[:length_of_size_field])

    return_value = skunk_pb2.ReturnValue()
    return_value.ParseFromString(call_result[length_of_size_field:length_of_size_field + return_length])

    return return_value


def run_skunk():
    with open("/dev/skunk", 'r') as skunk_device:
        ret = call_function_one_arg(skunk_device, "kallsyms_lookup_name",
        1, skunk_pb2.FunctionCall.eightByte,
        "kallsyms_lookup_name", skunk_pb2.Argument.stringArg)

        print("Got adress of {}".format(hex(2**64 + ret.ret64))) # Printing 2's complement of the address

        ret = call_function_one_arg(skunk_device, "round_jiffies",
        1, skunk_pb2.FunctionCall.eightByte,
        133713371337, skunk_pb2.Argument.eightByteArg)

        print("Got result of {}".format(ret.ret64))

if __name__ == "__main__":
    run_skunk()