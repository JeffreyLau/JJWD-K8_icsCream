# top level project rules for the tcc8800_evm project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := tcc8800_evm

MODULES += app/aboot

#DEFINES += WITH_DEBUG_JTAG=1
DEFINES += WITH_DEBUG_UART=1
#DEFINES += WITH_DEBUG_FBCON=1

TCC88XX := 1

################################################
##EMMC DEFINES
################################################
EMMC_BOOT := 1
DEFINES += TCC88XX_EMMC_BOOT=1


DEFINES += _EMMC_BOOT=1
DEFINES += _EMMC_BOOT_TCC=1

