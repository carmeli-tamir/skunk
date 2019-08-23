#!/bin/bash

generate_proto()
{
    echo "Generating proto files"
    protoc skunk.proto --c_out=kernel
    protoc skunk.proto --python_out=user
}

install_protoc_kernel_module()
{
    cd protobuf-c
    make
    cd ..
}

install_protoc()
{
    echo "Installing protobuf-c"
    cd third_party/protobuf-c
    ./autogen.sh && ./configure && make && sudo make install

    install_protoc_kernel_module

    cd ../..
}

make_skunk()
{
    echo "Compiling skunk"
    cd kernel
    make
    cd ..
}

load_skunk_kernel_module()
{
    echo "Loading skunk.ko"
    cd kernel
    sudo insmod skunk.ko
    cd ..
}

load_protoc_kernel_module()
{
    echo "Loading protobuf-c.ko"
    cd third_party/protobuf-c/protobuf-c/
    sudo insmod protobuf-c.ko
    cd ../../../
}

usage()
{
    echo "usage: $0 [[[-i|--install] [-r|--run]] | [-h|--help]]"
}

install()
{
    install_protoc
    generate_proto
    make_skunk
}

run()
{
    load_protoc_kernel_module
    load_skunk_kernel_module
}

##### Main

install=
run=

if [ $# == 0 ]; then
    usage
fi

while [ "$1" != "" ]; do
    case $1 in
        -i | --install )           shift
                                install=1
                                ;;
        -r | --run )            run=1
                                ;;
        -h | --help )           usage
                                exit
                                ;;
        * )                     usage
                                exit 1
    esac
    shift
done

if [ "$install" = "1" ]; then
    install
fi

if [ "$run" = "1" ]; then
    run
fi