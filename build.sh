#!/bin/bash
protoc skunk.proto --c_out=kernel
protoc skunk.proto --python_out=user
