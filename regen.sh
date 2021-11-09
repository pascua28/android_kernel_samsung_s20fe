#!/bin/bash

export ARCH=arm64
mkdir out

make -j8 O=out ARCH=arm64 oldconfig

cp out/.config arch/arm64/configs/exynos9830-r8slte_defconfig
