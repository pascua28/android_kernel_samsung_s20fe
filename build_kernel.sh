#!/bin/bash

mkdir out

DTB_DIR=$(pwd)/out/arch/arm64/boot/dts
mkdir ${DTB_DIR}/exynos

export PLATFORM_VERSION=11
export ANDROID_MAJOR_VERSION=r

echo "**********************************"
echo "(1) OneUI 3"
echo "(2) OneUI 4"
read -p "Selected OneUI version: " ui_ver

make O=out ARCH=arm64 exynos9830-r8slte_defconfig

if [ $ui_ver == "1" ]; then
	echo "
Compiling kernel for OneUI 3
"

elif [ $ui_ver == "2" ]; then
	echo "
Compiling kernel for OneUI 4
"

	scripts/configcleaner "
CONFIG_DEVFREQ_THERMAL
CONFIG_MALI_VALHALL_R25P1
CONFIG_MALI_DDK_VALHALL_R32P1
"

	echo "
CONFIG_DEVFREQ_THERMAL=y
# CONFIG_MALI_VALHALL_R25P1 is not set
CONFIG_MALI_DDK_VALHALL_R32P1=y
CONFIG_MALI_PLATFORM_NAME=\"exynos\"
CONFIG_MALI_EXYNOS_CLOCK=y
CONFIG_MALI_EXYNOS_DVFS=y
CONFIG_MALI_EXYNOS_PM=y
CONFIG_MALI_EXYNOS_RTPM=y
# CONFIG_MALI_EXYNOS_IFPO is not set
CONFIG_MALI_EXYNOS_QOS=y
CONFIG_MALI_EXYNOS_THERMAL=y
CONFIG_MALI_EXYNOS_BTS_MO=y
# CONFIG_MALI_EXYNOS_SECURE_RENDERING_ARM is not set
CONFIG_MALI_EXYNOS_SECURE_RENDERING_LEGACY=y
# CONFIG_MALI_EXYNOS_DEBUG is not set
CONFIG_MALI_EXYNOS_DEVICETREE=y
# CONFIG_MALI_EXYNOS_LLC is not set
CONFIG_MALI_EXYNOS_CL_BOOST=y
# CONFIG_MALI_NOTIFY_UTILISATION is not set
# CONFIG_MALI_EXYNOS_SECURE_SMC_NOTIFY_GPU is not set
CONFIG_MALI_EXYNOS_WAKEUP_CLOCK_WA=y
# CONFIG_MALI_EXYNOS_BLOCK_RPM_WHILE_SUSPEND_RESUME is not set
CONFIG_MALI_EXYNOS_UNIT_TESTS=y
CONFIG_MALI_PRFCNT_SET_PRIMARY=y
# CONFIG_MALI_PRFCNT_SET_TERTIARY is not set
# CONFIG_MALI_PRFCNT_SET_SELECT_VIA_DEBUG_FS is not set
# CONFIG_MALI_ARBITRATION is not set
" >> out/.config

make -j8 O=out ARCH=arm64 oldconfig

fi

make O=out ARCH=arm64 -j8

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