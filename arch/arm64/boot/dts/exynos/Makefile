# SPDX-License-Identifier: GPL-2.0
dtb-$(CONFIG_ARCH_EXYNOS) += \
	exynos5433-tm2.dtb	\
	exynos5433-tm2e.dtb	\
	exynos7-espresso.dtb

dtb-$(CONFIG_SOC_EMULATOR9820) += exynos9820-emulator9820.dtb

dtb-$(CONFIG_SOC_EXYNOS9820) += \
	exynos9820.dtb
dtb-$(CONFIG_SOC_EXYNOS9820) += \
	exynos9820-smdk9820.dtbo \
	exynos9820-maestro9820_5g.dtbo

ifeq ($(CONFIG_SOC_EXYNOS9830_EVT0), y)
dtb-$(CONFIG_SOC_EXYNOS9830) += \
	exynos9830_evt0.dtb
dtb-$(CONFIG_SOC_EXYNOS9830) += \
	exynos9830-smdk9830.dtbo \
	exynos9830-smdk9830_r01.dtbo \
	exynos9830-universal9830.dtbo \
	exynos9830-universal9830_r01.dtbo \
	exynos9830-universal9830_system_as_root.dtbo
else
dtb-$(CONFIG_SOC_EXYNOS9830) += \
	exynos9830.dtb
dtb-$(CONFIG_SOC_EXYNOS9830) += \
	exynos9830-smdk9830_r02.dtbo \
	exynos9830-universal9830_r04.dtbo \
	exynos9830-universal9830_r05.dtbo \
	exynos9830-universal9830_r06.dtbo \
	exynos9830-universal9830_r10.dtbo
endif

# always	:= $(dtb-y) $(dtbo-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb *.dtbo
