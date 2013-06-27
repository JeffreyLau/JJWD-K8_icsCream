#Makefile to generate tcc_crc

SRC_DIR  := platform/tcc_shared/tools
COMPILER := gcc

SRCS     := $(SRC_DIR)/main.c $(SRC_DIR)/write_crc.c

$(BUILDDIR)/tcc_crc: $(SRCS) $(SRC_DIR)/write_crc.h $(SRC_DIR)/jatom.h
	$(NOECHO)$(COMPILER) -o $@ $(SRCS)

