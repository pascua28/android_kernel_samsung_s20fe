#!/bin/bash

sudo apt-get install git ccache automake lzop bison gperf build-essential zip curl zlib1g-dev zlib1g-dev:i386 g++-multilib libxml2-utils bzip2 libbz2-dev libbz2-1.0 libghc-bzlib-dev squashfs-tools pngcrush schedtool dpkg-dev liblz4-tool make optipng

sudo apt-get install gcc-aarch64-linux-gnu

sudo apt-get install clang

sudo apt-get install clang-12


export ARCH=arm64
mkdir out

BUILD_CROSS_COMPILE=aarch64-linux-gnu-
KERNEL_LLVM_BIN=clang-12
CLANG_TRIPLE=aarch64-linux-gnu-
KERNEL_MAKE_ENV="DTC_EXT=$(pwd)/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y"

echo "**********************************"
echo "Select variant (Snapdragon only)"
echo "(1) 4G Variant"
echo "(2) 5G Variant"
read -p "Selected variant: " variant

make -j8 -C $(pwd) O=$(pwd)/out $KERNEL_MAKE_ENV ARCH=arm64 CROSS_COMPILE=$BUILD_CROSS_COMPILE CC=$KERNEL_LLVM_BIN CLANG_TRIPLE=$CLANG_TRIPLE r8q_defconfig

if [ $variant == "1" ]; then
	echo "
Compiling for 4G variant
"
	MODEL="G780G"

elif [ $variant == "2" ]; then
	echo "
Compiling for 5G variant
"
	MODEL="G781B"

	scripts/configcleaner "
CONFIG_SAMSUNG_NFC
CONFIG_NFC_PN547
CONFIG_NFC_PN547_ESE_SUPPORT
CONFIG_NFC_FEATURE_SN100U
CONFIG_FIVE
"

	echo "
# CONFIG_SAMSUNG_NFC is not set
# CONFIG_NFC_PN547 is not set
# CONFIG_NFC_PN547_ESE_SUPPORT is not set
# CONFIG_NFC_FEATURE_SN100U is not set
# CONFIG_FIVE is not set
" >> out/.config

make -j8 -C $(pwd) O=$(pwd)/out $KERNEL_MAKE_ENV ARCH=arm64 CROSS_COMPILE=$BUILD_CROSS_COMPILE CC=$KERNEL_LLVM_BIN CLANG_TRIPLE=$CLANG_TRIPLE oldconfig

fi

make -j8 -C $(pwd) O=$(pwd)/out $KERNEL_MAKE_ENV ARCH=arm64 CROSS_COMPILE=$BUILD_CROSS_COMPILE CC=$KERNEL_LLVM_BIN CLANG_TRIPLE=$CLANG_TRIPLE

cat out/arch/arm64/boot/dts/vendor/qcom/*.dtb > out/dtb.img

DTB="out/dtb.img"

IMAGE="out/arch/arm64/boot/Image.gz"

if [[ -f "$IMAGE" ]]; then
	rm AnyKernel3/zImage > /dev/null 2>&1
	rm AnyKernel3/*.zip > /dev/null 2>&1
	cp $IMAGE AnyKernel3/Image.gz
	cp $DTB AnyKernel3/dtb
	cd AnyKernel3
	zip -r9 Kernel-$MODEL.zip .
fi
