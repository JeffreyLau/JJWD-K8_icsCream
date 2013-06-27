# top level project rules for the m805_892x_evm project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := m805_892x_evm

MODULES += app/aboot

#DEFINES += WITH_DEBUG_JTAG=1
DEFINES += WITH_DEBUG_UART=1
#DEFINES += WITH_DEBUG_FBCON=1
#DEFINDES += _M805_8923_=1

TCC892X := 1

################################################
##EMMC DEFINES
################################################
EMMC_BOOT := 1
DEFINES += TCC892X_EMMC_BOOT


DEFINES += _EMMC_BOOT=1
DEFINES += _EMMC_BOOT_TCC=1

