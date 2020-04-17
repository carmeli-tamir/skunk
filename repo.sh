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
    make clean
    make
    cd ..
}

install_protobuf()
{
    echo "Installing Protobuf"
    cd third_party/protobuf
    ./autogen.sh && ./configure --prefix=/usr/local && make && sudo make prefix=/usr/local install
    sudo ldconfig
    cd ../..

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

is_module_loaded()
{
    if lsmod | grep "$1" >> /dev/null ; then
        echo 1
    else
        echo 0
    fi
}

load_skunk_kernel_module()
{
    res=$(is_module_loaded "skunk")
    if [[ $res -eq 1 ]]; then
        echo "skunk.ko is already loaded"
        return;
    fi

    echo "Loading skunk.ko"
    cd kernel
    sudo insmod skunk.ko
    cd ..
}

load_protoc_kernel_module()
{
    res=$(is_module_loaded "protobuf_c")
    if [[ $res -eq 1 ]]; then
        echo "protobuf-c.ko is already loaded"
        return;
    fi

    echo "Loading protobuf-c.ko"
    cd third_party/protobuf-c/protobuf-c/
    sudo insmod protobuf-c.ko
    cd ../../../
}

pip_install()
{
    res=$(which pip)
    sudo $res install -e user
}

init_submodules()
{
    git submodule update --init
}

usage()
{
    echo "usage: $0 [[[-i|--install] [-r|--run]] | [-h|--help]]"
}

install()
{
    init_submodules
    pip_install
    install_protobuf
    install_protoc
    generate_proto
    make_skunk
    pip_install
}

run()
{
    load_protoc_kernel_module
    load_skunk_kernel_module
}

compile_kernel()
{
    sudo apt install -y bison flex gcc-arm-linux-gnueabihf qemu-system-arm
    export LINUX_VERSION="linux-4.19.84"
    if [ ! -d "$LINUX_VERSION" ]; then
        wget "https://cdn.kernel.org/pub/linux/kernel/v4.x/$LINUX_VERSION.tar.xz"
        tar -xJf $LINUX_VERSION.tar.xz
    fi

    cp `dirname $0`/tests/setup/skunk_defconfig  $LINUX_VERSION/arch/arm/configs
    cd $LINUX_VERSION

    make ARCH=arm skunk_defconfig
    make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)
    cd ..

    qemu-system-arm -machine virt -cpu cortex-a15 -m 512 -kernel $LINUX_VERSION/arch/arm/boot/zImage -append 'console=ttyAMA0 earlyprintk=ttyAMA0' --nographic -no-reboot -serial mon:stdio
}

run_tests()
{
    compile_kernel
    echo "OMG test"
}

##### Main

install=
run=
test=

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
        -p | --proto )          proto=1
                                ;;
        -t | --test )           test=1
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

if [ "$proto" = "1" ]; then
    generate_proto
fi

if [ "$test" = "1" ]; then
    run_tests
fi