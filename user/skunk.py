#!/usr/bin/env python3
import fcntl
import struct

import skunk_pb2

def binary_length_and_value(buffer, buffer_length):
    return struct.pack('N', buffer_length) + buffer

def call_function_demo(device, fname, fargs, fret, arg1):
    func_type = skunk_pb2.function_type()
    func_type.args = fargs
    func_type.ret = fret

    func_type_binary = binary_length_and_value(func_type.SerializeToString(), func_type.ByteSize())

    func_with_one_arg = skunk_pb2.func_with_1_arg()
    func_with_one_arg.name = fname
    func_with_one_arg.arg1 = arg1

    func_with_one_arg_binary = binary_length_and_value(func_with_one_arg.SerializeToString(), func_with_one_arg.ByteSize())

    iotcl_args = binary_length_and_value(func_type_binary + func_with_one_arg_binary, len(func_type_binary + func_with_one_arg_binary))
    print(fcntl.ioctl(device, 0xc008ee00, iotcl_args))


def run_skunk():
    with open("/dev/skunk", 'r') as skunk_device:
        call_function_demo(skunk_device, "kallsyms_lookup_name", skunk_pb2.function_type.stringArg1, skunk_pb2.function_type.fourByte, "kallsyms_lookup_name")
        

if __name__ == "__main__":
    run_skunk()