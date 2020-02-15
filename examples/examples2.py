from skunk import Skunk
import skunk_pb2

def run_skunk():
    sk = Skunk()
    
    ret = sk.call_function("call_usermodehelper",
        4, skunk_pb2.FunctionCall.four_byte,
        "command", skunk_pb2.Argument.string, 
        0, skunk_pb2.Argument.eight_byte, 0, skunk_pb2.Argument.eight_byte,
        1, skunk_pb2.Argument.four_byte)
    print("The return value of call_usermodehelper is {}".format(ret.four_byte))

if __name__ == "__main__":
    run_skunk()