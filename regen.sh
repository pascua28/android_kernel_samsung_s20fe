#!/bin/bash

export ARCH=arm64
mkdir out

KERNEL_LLVM_BIN=/home/pascua14/clang-8/bin

make -j8 O=out ARCH=arm64 \
	CC=$KERNEL_LLVM_BIN/clang \
	oldconfig

cp out/.config arch/arm64/configs/exynos9830-r8slte_defconfig
