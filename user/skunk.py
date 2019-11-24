#!/usr/bin/env python3
import fcntl
import struct

import skunk_pb2

class Skunk(object):
    def __init__(self, device_name="/dev/skunk", ioctl_num=0xc008ee00):
        self.device_name = device_name
        self.ioctl_num = ioctl_num

    def call_function_two_arg(self, fname, num_args, fret, arg1, type1, arg2=None, type2=None):
        func_with_one_arg = skunk_pb2.FunctionCall()
        func_with_one_arg.returnType = fret
        func_with_one_arg.numberOfArguments = num_args
        func_with_one_arg.name = fname
        self._assign_arg(func_with_one_arg.arg1, type1, arg1)
        
        if arg2 is not None and type2 is not None:
            self._assign_arg(func_with_one_arg.arg2, type2, arg2)

        func_with_one_arg_binary = self._binary_length_and_value(func_with_one_arg.SerializeToString(), func_with_one_arg.ByteSize())
        
        with open(self.device_name, 'r') as skunk_device:
            call_result = fcntl.ioctl(skunk_device, self.ioctl_num, func_with_one_arg_binary)

        length_of_size_field = struct.calcsize('I')
        return_length, = struct.unpack('I', call_result[:length_of_size_field])

        return_value = skunk_pb2.ReturnValue()
        return_value.ParseFromString(call_result[length_of_size_field:length_of_size_field + return_length])

        return return_value

    @staticmethod
    def _assign_arg(protobuf_arg, arg_type, arg_value):
        protobuf_arg.type = arg_type
        if arg_type == skunk_pb2.Argument.string:
            protobuf_arg.arg_string = arg_value
        elif arg_type == skunk_pb2.Argument.eight_byte:
            protobuf_arg.arg_eight_byte = arg_value
        else:
            raise ValueError("Unsupported argument type")
    
    @staticmethod
    def _binary_length_and_value(buffer, buffer_length):
        return struct.pack('I', buffer_length) + buffer