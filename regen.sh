#!/bin/bash

export ARCH=arm64
mkdir out

KERNEL_LLVM_BIN=/home/pascua14/clang/bin
KERNEL_MAKE_ENV="DTC_EXT=$(pwd)/tools/dtc CONFIG_BUILD_ARM64_DT_OVERLAY=y"

scripts/configcleaner "
CONFIG_SAMSUNG_NFC
CONFIG_NFC_PN547
CONFIG_NFC_PN547_ESE_SUPPORT
CONFIG_NFC_FEATURE_SN100U
"

echo "
CONFIG_SAMSUNG_NFC=y
CONFIG_NFC_PN547=y
CONFIG_NFC_PN547_ESE_SUPPORT=y
CONFIG_NFC_FEATURE_SN100U=y
" >> out/.config

make -j8 -C $(pwd) O=$(pwd)/out $KERNEL_MAKE_ENV ARCH=arm64 \
	CC=$KERNEL_LLVM_BIN/clang \
	oldconfig

cp out/.config arch/arm64/configs/r8q_defconfig
