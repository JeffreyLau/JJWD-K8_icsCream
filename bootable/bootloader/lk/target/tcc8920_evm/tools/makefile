#Makefile to generate lk.rom

TOOLS_DIR  := target/$(TARGET)/tools

BOOTLOADER_FILE := $(BUILDDIR)/lk.rom

APPSBOOTHEADER: $(BOOTLOADER_FILE)

$(BUILDDIR)/lk.rom: $(OUTBIN) $(BUILDDIR)/tcc_crc
	$(NOECHO)echo generating $@
	$(NOECHO)$(BUILDDIR)/tcc_crc -v __BL -o $(BOOTLOADER_FILE) $(OUTBIN)
