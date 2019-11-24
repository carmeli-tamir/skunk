from skunk import Skunk
import skunk_pb2

def run_skunk():
    sk = Skunk()
    
    ret = sk.call_function_two_arg("kallsyms_lookup_name",
        1, skunk_pb2.FunctionCall.eight_byte,
        "kallsyms_lookup_name", skunk_pb2.Argument.string)
    print("The address of kallsyms_lookup_name is {}".format(hex(2**64 + ret.eight_byte))) # Printing 2's complement of the address

    ret = sk.call_function_two_arg("round_jiffies",
        1, skunk_pb2.FunctionCall.eight_byte,
        133713371337, skunk_pb2.Argument.eight_byte)

    print("The result of round_jiffies(133713371337) is {}".format(ret.eight_byte))

    ret = sk.call_function_two_arg("strstr",
        2, skunk_pb2.FunctionCall.string,
        "whatisthemeaningoflife", skunk_pb2.Argument.string, "ning", skunk_pb2.Argument.string)

    print("strstr(\"whatisthemeaningoflife\", \"ning\") == \"{}\"".format(ret.string))

if __name__ == "__main__":
    run_skunk()