#
## Makefile for MCTCP support code.
#
#

obj-$(CONFIG_MCPS) += mcps.o

mcps-y := mcps_main.o mcps_gro_main.o mcps_core.o mcps_params.o mcps_buffer.o
mcps-$(CONFIG_MCPS_DEBUG) += mcps_debug.o

obj-$(CONFIG_KUNIT) += kunit_test/
GCOV_PROFILE_mcps_params.o := y
GCOV_PROFILE_mcps_core.o := y
