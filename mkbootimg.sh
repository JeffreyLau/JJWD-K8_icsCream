#! /bin/bash
out/host/linux-x86/bin/mkbootimg  --kernel out/target/product/m805_892x/kernel  --ramdisk out/target/product/m805_892x/ramdisk-recovery.img --cmdline "console=ttyTCC0,115200n8 androidboot.console=ttyTCC0" --base 0x80000000 --pagesize 8192 --output out/target/product/m805_892x/recovery.img

