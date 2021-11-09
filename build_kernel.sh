#!/bin/bash

mkdir out

DTB_DIR=$(pwd)/out/arch/arm64/boot/dts
mkdir ${DTB_DIR}/exynos

export PLATFORM_VERSION=11
export ANDROID_MAJOR_VERSION=r

BUILD_CROSS_COMPILE=/home/pascua14/gcc-7.4.1/bin/aarch64-linux-gnu-
KERNEL_LLVM_BIN=/home/pascua14/clang-8/bin/clang
CLANG_TRIPLE=aarch64-linux-gnu-

make O=out ARCH=arm64 CC=$KERNEL_LLVM_BIN exynos9830-r8slte_defconfig
make O=out ARCH=arm64 \
	CROSS_COMPILE=$BUILD_CROSS_COMPILE CC=$KERNEL_LLVM_BIN \
	CLANG_TRIPLE=$CLANG_TRIPLE -j8

$(pwd)/tools/mkdtimg cfg_create $(pwd)/out/dtb.img dt.configs/exynos9830.cfg -d ${DTB_DIR}/exynos

IMAGE="out/arch/arm64/boot/Image"
if [[ -f "$IMAGE" ]]; then
	rm AnyKernel3/zImage > /dev/null 2>&1
	rm AnyKernel3/dtb > /dev/null 2>&1
	rm AnyKernel3/*.zip > /dev/null 2>&1
	mv out/dtb.img AnyKernel3/dtb
	mv $IMAGE AnyKernel3/zImage
	cd AnyKernel3
	zip -r9 Kernel-G780F.zip .
fi
