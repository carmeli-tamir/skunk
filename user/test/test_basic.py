import unittest
from unittest.mock import patch

import skunk
import skunk_pb2


class TestCalls(unittest.TestCase):
    
    @staticmethod
    def build_return_value(status, eight_byte, has_string):
        return_kallsym_adress = skunk_pb2.ReturnValue()
        return_kallsym_adress.status = status
        return_kallsym_adress.eight_byte = eight_byte
        return_kallsym_adress.has_string = has_string

        return skunk.binary_length_and_value(return_kallsym_adress.SerializeToString(), return_kallsym_adress.ByteSize())

    @staticmethod
    def build_function_call_1_arg(return_type, function_name, arg_type, arg_value):
        func_with_one_arg = skunk_pb2.FunctionCall()
        func_with_one_arg.returnType = return_type
        func_with_one_arg.numberOfArguments = 1
        func_with_one_arg.name = function_name
        func_with_one_arg.arg1.type = arg_type

        if arg_type == skunk_pb2.Argument.string:
            func_with_one_arg.arg1.arg_string = arg_value
        elif arg_type == skunk_pb2.Argument.eight_byte:
            func_with_one_arg.arg1.arg_eight_byte = arg_value
        else:
            raise ValueError("Unsupported argument type")
        
        return skunk.binary_length_and_value(func_with_one_arg.SerializeToString(), func_with_one_arg.ByteSize())

    @patch('skunk.fcntl')
    def test_call_kallsyms(self, mock_requests):
        ioctl_ret = self.build_return_value(skunk_pb2.ReturnValue.Success, 0x1ee7, False)
        mock_requests.ioctl.return_value = ioctl_ret

        ret = skunk.call_function_two_arg(
            None,
            "kallsyms_lookup_name",
            1,
            skunk_pb2.FunctionCall.eight_byte,
            "kallsyms_lookup_name",
            skunk_pb2.Argument.string
        )

        mock_requests.ioctl.assert_called_once_with(
                None,
                0xc008ee00, 
                self.build_function_call_1_arg(skunk_pb2.FunctionCall.eight_byte, "kallsyms_lookup_name", skunk_pb2.Argument.string,  "kallsyms_lookup_name")
            )
        assert ret.eight_byte == 0x1ee7