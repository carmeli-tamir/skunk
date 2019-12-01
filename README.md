[![Build Status](https://travis-ci.com/carmeli-tamir/skunk.svg?token=fDEfs6sB6P1mJ2TXGVqR&branch=master)](https://travis-ci.com/carmeli-tamir/skunk)

## Overview

`Skunk` is a framework that enables to test linux kernel functions in user space, using python.
Current kernel testing solutions either require to write kernel code (for Unit Test solutions such as Kunit and KTF) 
or perform end to end tests (such as Kselftest, Linux Test Project, Kernel CI, Syzbot).

Since writing kernel code takes time and experience, `Skunk` facilitates
writing unit test (i.e. testing kernel functions) or integration tests (e.g. testing a kernel subsystem or driver)
by writing python code in user mode.

A simple example of `Skunk`'s usage is testing the kernel implementation of strstr.
Here's a simple test in python:

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
### Prerequisites
Compiled Linux Kernel source code of your target machine.

See the `before_install` section in `.travis.yml` for an example of a specific kernel version.

Or, See a [tutorial](https://www.freecodecamp.org/news/building-and-installing-the-latest-linux-kernel-from-source-6d8df5345980/ "Compile kernel tutorial")  explaining how to compile your kernel version sources.

### Install
Simply run `repo.sh -i`. This might take some time.

### Run
Simply run `repo.sh -r`, and then run your python code. For example you can run `examples/examples1.py`

## Contributing
There is a list of issues that we need help with. Waiting for your PRs!
If you want to make a big change, it might be best to consult first on the issue conversation.