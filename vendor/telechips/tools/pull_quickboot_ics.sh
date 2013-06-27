# !/bin/sh

# for using quick boot branch merging

#change current dir to root dir
cd ../../../

# setup build environment 
. build/envsetup.sh
lunch

repo forall -c git reset --hard v12.05_r1-tcc-android-4.0.4

# branch merging 
echo "Start Mergingg Gingerbread and Quic Boot Branch"
cd $ANDROID_BUILD_TOP

cd vendor/telechips/tools
git pull telechips ics
cd $ANDROID_BUILD_TOP

cd packages/providers/MediaProvider
git pull telechips ics
cd $ANDROID_BUILD_TOP

cd packages/apps/Settings
echo "$PWD ------------------------------------------------------------------------------------------------------"
git branch -D quickboot_ics
git branch quickboot_ics
git checkout quickboot_ics
git pull telechips quickboot_ics
cd $ANDROID_BUILD_TOP

cd hardware/telechips/nand
echo "$PWD ------------------------------------------------------------------------------------------------------"
git branch -D quickboot_ics
git branch quickboot_ics
git checkout quickboot_ics
git pull telechips quickboot_ics
cd $ANDROID_BUILD_TOP

cd bionic
echo "$PWD ------------------------------------------------------------------------------------------------------" 
git branch -D quickboot_ics
git branch quickboot_ics
git checkout quickboot_ics
git pull telechips quickboot_ics
cd $ANDROID_BUILD_TOP

cd system/core
echo "$PWD ------------------------------------------------------------------------------------------------------" 
git branch -D quickboot_ics
git branch quickboot_ics
git checkout quickboot_ics
git pull telechips quickboot_ics
cd $ANDROID_BUILD_TOP

cd device/telechips/common
echo "$PWD ------------------------------------------------------------------------------------------------------" 
git branch -D quickboot_ics_lib
git branch quickboot_ics_lib
git checkout quickboot_ics_lib
git pull telechips quickboot_ics_lib
cd $ANDROID_BUILD_TOP

cd device/telechips/m805_892x
echo "$PWD ------------------------------------------------------------------------------------------------------" 
git branch -D quickboot_ics
git branch quickboot_ics
git checkout quickboot_ics
git pull telechips quickboot_ics
cd $ANDROID_BUILD_TOP

cd device/telechips/tcc8800
echo "$PWD ------------------------------------------------------------------------------------------------------" 
git branch -D quickboot_ics
git branch quickboot_ics
git checkout quickboot_ics
git pull telechips quickboot_ics
cd $ANDROID_BUILD_TOP

cd device/telechips/tcc8920
echo "$PWD ------------------------------------------------------------------------------------------------------" 
git branch -D quickboot_ics
git branch quickboot_ics
git checkout quickboot_ics
git pull telechips quickboot_ics
cd $ANDROID_BUILD_TOP

cd device/telechips/tcc8920st
echo "$PWD ------------------------------------------------------------------------------------------------------" 
git branch -D quickboot_ics
git branch quickboot_ics
git checkout quickboot_ics
git pull telechips quickboot_ics
cd $ANDROID_BUILD_TOP

cd device/telechips/tcc892x-common
echo "$PWD ------------------------------------------------------------------------------------------------------" 
git branch -D quickboot_ics
git branch quickboot_ics
git checkout quickboot_ics
git pull telechips quickboot_ics
cd $ANDROID_BUILD_TOP

cd frameworks/base
echo "$PWD ------------------------------------------------------------------------------------------------------" 
git branch -D quickboot_ics_lib
git branch quickboot_ics_lib
git checkout quickboot_ics_lib
git pull telechips quickboot_ics_lib
cd $ANDROID_BUILD_TOP

cd bootable/bootloader/lk
echo "$PWD ------------------------------------------------------------------------------------------------------" 
git branch -D quickboot_ics
git branch quickboot_ics
git checkout quickboot_ics
git pull telechips ics
git pull telechips quickboot_ics
cd $ANDROID_BUILD_TOP

cd kernel
echo "$PWD ------------------------------------------------------------------------------------------------------" 
git branch -D quickboot_ics
git branch quickboot_ics
git checkout quickboot_ics
git pull telechips quickboot_ics
cd $ANDROID_BUILD_TOP

cd bootable/recovery
echo "$PWD ------------------------------------------------------------------------------------------------------" 
git branch -D quickboot_ics
git branch quickboot_ics
git checkout quickboot_ics
git pull telechips quickboot_ics
cd $ANDROID_BUILD_TOP

