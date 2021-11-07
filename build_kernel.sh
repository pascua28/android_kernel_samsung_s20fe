#!/bin/bash


export PLATFORM_VERSION=11
export ANDROID_MAJOR_VERSION=r
export ARCH=arm64

make ARCH=arm64 exynos9830-r8slte_defconfig
make ARCH=arm64 -j64

