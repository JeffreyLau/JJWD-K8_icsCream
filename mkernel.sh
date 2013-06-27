#! /bin/bash

. ./build/envsetup.sh
lunch 14
cd ./kernel/
make car_m805_892x_defconfig
#make config
make -j4
cd -
