[![Build Status](https://travis-ci.com/carmeli-tamir/skunk.svg?token=fDEfs6sB6P1mJ2TXGVqR&branch=master)](https://travis-ci.com/carmeli-tamir/skunk)

## Overview

`Skunk` is a framework that enables to test linux kernel functions in user space, using python.
Current kernel testing solutions either require to write kernel code (for Unit Test solutions such as Kunit and KTF) 
or perform end to end tests (such as Kselftest, Linux Test Project, Kernel CI, Syzbot).

Since writing kernel code takes time and experience to do well, `Skunk` facilitates
writing unit test (i.e. testing kernel functions) or integration tests (e.g. testing a kernel subsystem or driver)
by writing python code in user mode.

A simple example of `Skunk`'s usage is testing the kernel implementation of strstr.
Here's a python code for a simple test:

```
sk = Skunk()
ret = sk.call_function_two_arg(
                                "strstr",                                               # Function name
                                2,                                                      # Number of arguments
                                skunk_pb2.FunctionCall.string,                          # Type of return value
                                "whatisthemeaningoflife", skunk_pb2.Argument.string,    # First argument value and type
                                "ning", skunk_pb2.Argument.string)                      # Second argument value and type

assert ret.string=="ningoflife", "error"
```

Currently, you can test only functions with limited dependencies. In the next milestone of `Skunk`, we intend to enable to mock kernel functions
so that you can extend your test coverage and flexibility.

## Architecture
![Skunk Architecture](https://github.com/carmeli-tamir/skunk/blob/readme/doc/skunk_arch.png)

Basically, the `skunk.py` library packs the function call in a protobuf message and sends it to `skunk.ko` via an ioctl.

`skunk.ko` then unpacks the message and performs the call within the kernel. 

The return value is returned to user space via another protobuf message.

## Setup

## Contributing