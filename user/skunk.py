#!/usr/bin/env python3
import fcntl
import struct

import skunk_pb2

def binary_length_and_value(buffer, buffer_length):
    return struct.pack('I', buffer_length) + buffer

def call_function_one_arg(device, fname, fargs, fret, arg1, type1):
    func_type = skunk_pb2.function_type()
    func_type.args = fargs
    func_type.ret = fret
    func_type_binary = binary_length_and_value(func_type.SerializeToString(), func_type.ByteSize())

    func_with_one_arg = skunk_pb2.func_with_1_arg()
    func_with_one_arg.name = fname
    func_with_one_arg.arg1.type = type1

    if type1 == skunk_pb2.Argument.stringArg:
        func_with_one_arg.arg1.argString = arg1
    elif type1 == skunk_pb2.Argument.eightByteArg:
        func_with_one_arg.arg1.argInt8b = arg1
    else:
        raise ValueError("Unsupported argument type")

    func_with_one_arg_binary = binary_length_and_value(func_with_one_arg.SerializeToString(), func_with_one_arg.ByteSize())

    iotcl_args = binary_length_and_value(func_type_binary + func_with_one_arg_binary, len(func_type_binary + func_with_one_arg_binary))

    call_result = fcntl.ioctl(device, 0xc008ee00, iotcl_args)

    length_of_size_field = struct.calcsize('I')
    return_length, = struct.unpack('I', call_result[:length_of_size_field])

    return_value = skunk_pb2.ReturnValue()
    return_value.ParseFromString(call_result[length_of_size_field:length_of_size_field + return_length])

    return return_value


def run_skunk():
    with open("/dev/skunk", 'r') as skunk_device:
        ret = call_function_one_arg(skunk_device, "kallsyms_lookup_name",
        skunk_pb2.function_type.oneArg, skunk_pb2.function_type.eightByte,
        "kallsyms_lookup_name", skunk_pb2.Argument.stringArg)

        print("Got adress of {}".format(hex(2**64 + ret.ret64))) # Printing 2's complement of the address

        ret = call_function_one_arg(skunk_device, "round_jiffies",
        skunk_pb2.function_type.oneArg, skunk_pb2.function_type.eightByte,
        133713371337, skunk_pb2.Argument.eightByteArg)

        print("Got result of {}".format(ret.ret64))

if __name__ == "__main__":
    run_skunk()