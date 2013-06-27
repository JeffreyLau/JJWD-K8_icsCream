#! /bin/bash

#make kernel
source build/envsetup.sh
lunch 14
#cd ./kernel/
#make m805_892x_defconfig
#cp ./config .config
#make -j4
#cd -

#make lk
#cd ./bootable/bootloader/lk/
#make m805_892x_evm EMMC_BOOT=0
#cd -

#make android
make -j8
