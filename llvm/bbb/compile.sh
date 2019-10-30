#!/bin/bash
set -e
set -x

llc=../cmake-build-debug/bin/llc
lld=../cmake-build-debug/bin/lld
llvm_mc=../cmake-build-debug/bin/llvm-mc
llvm_objcopy=../cmake-build-debug/bin/llvm-objcopy
clang=../cmake-build-debug/bin/clang

echo 'Compiling:'

$llvm_mc --filetype obj -triple=tl45-unknown-unknown -debug crt0.s -o crt0.o

$clang --target=tl45-unknown-none -fintegrated-as -O3 -c -v -Iinclude "$@" -S -emit-llvm -o user_code.ll
$clang --target=tl45-unknown-none -fintegrated-as -O3 -c -v -Iinclude "$@" -S -o user_code.s
$clang --target=tl45-unknown-none -fintegrated-as -O3 -c -v -Iinclude "$@" -o user_code.o

echo 'Linking:'

$lld -flavor gnu --oformat binary -image-base 0 crt0.o user_code.o -o a.out

echo 'Dumping:'

# $llvm_objcopy -O binary --only-section=.text a.out text.out

./hexify.py a.out | tee text.out.hex

$llvm_mc -mdis -triple=tl45-unknown-unknown text.out.hex



