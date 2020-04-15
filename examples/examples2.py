from skunk import Skunk
import skunk_pb2

def run_skunk():
    sk = Skunk()

    # Fail the memory allocation in call_usermodehelper_setup
    sk.add_mock("kmem_cache_alloc_trace", 0)

    with sk.apply_mock():
        ret = sk.call_function("call_usermodehelper",
            4, skunk_pb2.FunctionCall.four_byte,
            "command", skunk_pb2.Argument.string,
            0, skunk_pb2.Argument.eight_byte, 0, skunk_pb2.Argument.eight_byte,
            1, skunk_pb2.Argument.four_byte)
        print("The return value of call_usermodehelper is {}".format(ret.four_byte))

    # Return arbitrary value from call_usermodehelper_exec
    sk.add_mock("call_usermodehelper_exec", -42)

    with sk.apply_mock():
        ret = sk.call_function("call_usermodehelper",
            4, skunk_pb2.FunctionCall.four_byte,
            "command", skunk_pb2.Argument.string,
            0, skunk_pb2.Argument.eight_byte, 0, skunk_pb2.Argument.eight_byte,
            1, skunk_pb2.Argument.four_byte)
        print("The return value of call_usermodehelper is {}".format(ret.four_byte))

if __name__ == "__main__":
    run_skunk()