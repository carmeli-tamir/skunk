import pytest
import unittest
from unittest import mock
from unittest.mock import patch

from skunk import Skunk
import skunk_pb2


class TestCalls(unittest.TestCase):
    
    @classmethod
    def setup_class(cls):
        cls.skunk = Skunk("/dev/null")

    @staticmethod
    def build_return_value(status, eight_byte=None, string=None):
        ret = skunk_pb2.ReturnValue()
        ret.status = status
        if eight_byte:
            ret.eight_byte = eight_byte
        if string:
            ret.has_string = True
            ret.string = string
        else:
            ret.has_string = False

        return Skunk._binary_length_and_value(ret.SerializeToString(), ret.ByteSize())

    @staticmethod
    def assign_arg(protobuf_arg, arg_type, arg_value):
        protobuf_arg.type = arg_type
        if arg_type == skunk_pb2.Argument.string:
            protobuf_arg.arg_string = arg_value
        elif arg_type == skunk_pb2.Argument.eight_byte:
            protobuf_arg.arg_eight_byte = arg_value
        else:
            raise ValueError("Unsupported argument type")

    @staticmethod
    def build_function_call(return_type, function_name, *args):
        if len(args) % 2 != 0:
            raise ValueError("Bad argument list. Accepting a list of the form [<argu type>, <arg value>, <argu type>, <arg value>, ...]")
        func_with_one_arg = skunk_pb2.FunctionCall()
        func_with_one_arg.returnType = return_type
        func_with_one_arg.numberOfArguments = int(len(args) / 2)
        func_with_one_arg.name = function_name

        TestCalls.assign_arg(func_with_one_arg.arg1, args[0], args[1])
        if len(args) > 2:
            TestCalls.assign_arg(func_with_one_arg.arg2, args[2], args[3])

        return Skunk._binary_length_and_value(func_with_one_arg.SerializeToString(), func_with_one_arg.ByteSize())

    @patch('skunk.fcntl')
    def test_call_kallsyms(self, mock_requests):
        ioctl_ret = self.build_return_value(skunk_pb2.ReturnValue.Success, 0x1ee7, None)
        mock_requests.ioctl.return_value = ioctl_ret

        ret = TestCalls.skunk.call_function(
            "kallsyms_lookup_name",
            1,
            skunk_pb2.FunctionCall.eight_byte,
            "kallsyms_lookup_name",
            skunk_pb2.Argument.string
        )

        mock_requests.ioctl.assert_called_once_with(
                mock.ANY,
                mock.ANY, 
                self.build_function_call(skunk_pb2.FunctionCall.eight_byte, "kallsyms_lookup_name", skunk_pb2.Argument.string,  "kallsyms_lookup_name")
            )
        assert ret.eight_byte == 0x1ee7

    @patch('skunk.fcntl')
    def test_call_roundjiffies(self, mock_requests):
        ioctl_ret = self.build_return_value(skunk_pb2.ReturnValue.Success, 133713371500, None)
        mock_requests.ioctl.return_value = ioctl_ret

        ret = TestCalls.skunk.call_function(
            "round_jiffies",
            1,
            skunk_pb2.FunctionCall.eight_byte,
            133713371337,
            skunk_pb2.Argument.eight_byte
        )

        mock_requests.ioctl.assert_called_once_with(
                mock.ANY,
                mock.ANY, 
                self.build_function_call(skunk_pb2.FunctionCall.eight_byte, "round_jiffies", skunk_pb2.Argument.eight_byte,  133713371337)
            )
        assert ret.eight_byte == 133713371500

    @patch('skunk.fcntl')
    def test_call_strstr(self, mock_requests):
        ioctl_ret = self.build_return_value(skunk_pb2.ReturnValue.Success, None, "ningoflife")
        mock_requests.ioctl.return_value = ioctl_ret

        ret = TestCalls.skunk.call_function(
            "strstr",
            2,
            skunk_pb2.FunctionCall.string,
            "whatisthemeaningoflife",
            skunk_pb2.Argument.string,
            "ning",
            skunk_pb2.Argument.string
        )

        mock_requests.ioctl.assert_called_once_with(
                mock.ANY,
                mock.ANY, 
                self.build_function_call(skunk_pb2.FunctionCall.string, "strstr", skunk_pb2.Argument.string,  "whatisthemeaningoflife", skunk_pb2.Argument.string,  "ning")
            )
        assert ret.string == "ningoflife"

    @patch('skunk.fcntl')
    def test_raise_exception(self, mock_requests):
        ioctl_ret = self.build_return_value(skunk_pb2.ReturnValue.MockingError, None, "ningoflife")
        mock_requests.ioctl.return_value = ioctl_ret

        with pytest.raises(RuntimeError) as error:
            ret = TestCalls.skunk.call_function(
                    "some_func",
                    2,
                    skunk_pb2.FunctionCall.string,
                    "stringy1",
                    skunk_pb2.Argument.string,
                    "stringy2",
                    skunk_pb2.Argument.string
                )
            assert "MockingError" in str(error.value)


        mock_requests.ioctl.assert_called_once_with(
                mock.ANY,
                mock.ANY, 
                self.build_function_call(skunk_pb2.FunctionCall.string, "some_func", skunk_pb2.Argument.string,  "stringy1", skunk_pb2.Argument.string,  "stringy2")
            )