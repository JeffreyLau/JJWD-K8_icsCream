#! /bin/bash

#. ./build/envsetup.sh
#lunch 14
cd ./bootable/bootloader/lk/
make m805_892x_evm clean
make m805_892x_evm 
#rm ../../../out/target/product/m805_892x/lk.rom
cp ./build-m805_892x_evm/lk.rom ../../../out/target/product/m805_892x/
cd -
