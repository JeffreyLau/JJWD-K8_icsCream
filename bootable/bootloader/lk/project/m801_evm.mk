# top level project rules for the m801_evm project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := m801_evm

MODULES += app/aboot

#DEFINES += WITH_DEBUG_JTAG=1
DEFINES += WITH_DEBUG_UART=1
#DEFINES += WITH_DEBUG_FBCON=1
DEFINE += _M801_88_=1

TCC88XX := 1
