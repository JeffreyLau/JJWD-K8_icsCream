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
##TNFTL V8 DEFINES
################################################
TNFTL_V8_USE := 1

DEFINES += TNFTL_V8_INCLUDE
