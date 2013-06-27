# top level project rules for the tcc8920_evm project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := tcc8920_evm

MODULES += app/aboot

#DEFINES += WITH_DEBUG_JTAG=1
DEFINES += WITH_DEBUG_UART=1
#DEFINES += WITH_DEBUG_FBCON=1

TCC892X := 1
