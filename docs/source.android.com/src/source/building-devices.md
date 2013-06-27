<!--
   Copyright 2010 The Android Open Source Project

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
-->

# Building for devices #

This page complements the main page about [Building](building.html) with information
that is specific to individual devices.

The only supported devices are Nexus S, a.k.a. "crespo", and Nexus S 4G, a.k.a.
"crespo4g".
Nexus S is the recommended device to use with the Android Open-Source Project.
Nexus One a.k.a. "passion" is experimental and unsupported. Android Developer
Phones (ADP1 and ADP2, a.k.a. "dream" and "sapphire") are obsolete, were
experimental and unsupported in froyo, and can't be used with gingerbread.

## Building fastboot and adb ##

If you don't already have those tools, fastboot and adb can be built with
the regular build system. Follow the instructions on the page about
[building](building.html), and replace the main `make` command with

    $ make fastboot adb

## Booting into fastboot mode ##

During a cold boot, the following key combinations can be used to boot into fastboot mode,
which is a mode in the bootloader that can be used to flash the devices:

Device   | Keys
---------|------
crespo   | Press and hold *Volume Up*, then press and hold *Power*
crespo4g | Press and hold *Volume Up*, then press and hold *Power*
passion  | Press and hold the trackball, then press *Power*
sapphire | Press and hold *Back*, then press *Power*
dream    | Press and hold *Back*, then press *Power*

Also, on devices running froyo or later where adb is enabled,
the command `adb reboot bootloader` can be used to reboot from
Android directly into the bootloader with no key combinations.

## Unlocking the bootloader ##

It's only possible to flash a custom system if the bootloader allows it.

This is the default setup on ADP1 and ADP2.

On Nexus One, Nexus S, and Nexus S 4G, the bootloader is locked by default.
With the device in fastboot mode, the bootloader is unlocked with

    $ fastboot oem unlock

The procedure must be confirmed on-screen, and deletes the user data for
privacy reasons. It only needs to be run once.

On Nexus One, the operation voids the warranty and is irreversible.

On Nexus S and Nexus S 4G, the bootloader can be locked back with

    $ fastboot oem lock

## Obtaining proprietary binaries ##

While it's possible to build and run a system using exclusively source code
from Android Open-Source Project, such a system can only use the devices'
hardware capabilities for which Open-Source support exists.

Official binaries for Nexus S and Nexus S 4G can be downloaded from
[Google's Nexus driver page](http://code.google.com/android/nexus/drivers.html),
which add access to additional hardware capabilities with non-Open-Source code.

There are no official binaries for Nexus One, ADP2 or ADP1.

### Extracting the proprietary binaries ###

Each driver comes as a self-extracting script in a compressed archive.
After uncompressing each archive, run the included self-extracting script
from the root of the source tree, confirm that you agree to the terms of the
enclosed license agreement, and the binaries and their matching makefiles
will get installed in the `vendor/` hierarchy of the source tree.

There's an additional step on Nexus S 4G. Build the signapk tool with

    $ make signapk

Then reassemble the proprietary applicatons with

    $ vendor/samsung/crespo4g/reassemble-apks.sh

### Cleaning up when adding proprietary binaries ###

In order to make sure that the newly installed binaries are properly
taken into account after being extracted, the existing output of any previous
build needs to be deleted with

    $ make clobber

## Picking and building the configuration that matches a device ##

The steps to configure and build the Android Open-Source Project
are described in the page about [Building](building.html).

The recommended builds for the various devices are available through
the lunch menu, accesed when running the `lunch` command with no arguments:

Device   | Branch           | Build configuration
---------|------------------|------------------------
crespo   | android-2.3.5_r1 | full_crespo-userdebug
crespo4g | android-2.3.5_r1 | full_crespo4g-userdebug
passion  | android-2.3.5_r1 | full_passion-userdebug
sapphire | android-2.2.2_r1 | full_sapphire-userdebug
dream    | android-2.2.2_r1 | full_dream-userdebug

## Flashing a device ##

Set the device in fastboot mode if necessary (see above).

Because user data is typically incompatible between builds of Android,
it's typically better to delete it when flashing a new system.

    $ fastboot erase cache
    $ fastboot erase userdata

An entire Android system can be flashed in a single command: this writes
the boot, recovery and system partitions together after verifying that the
system being flashed is compatible with the installed bootloader and radio,
and reboots the system.

    $ fastboot flashall

On crespo, crespo4g, sapphire and dream (but not on passion), the commands above can
be replaced with a single command

    $ fastboot -w flashall

### Nexus S and Nexus S 4G Bootloader and Cell Radio compatibility ###

On Nexus S and Nexus S 4G, each version of Android has only been thoroughly
tested with on specific version of the underlying bootloader and cell radio
software.
However, no compatibility issues are expected when running newer systems
with older bootloaders and radio images according to the following tables.

Nexus S (worldwide version "XX"):

Android Version | Preferred Bootloader | Preferred Radio | Also possible
----------------|----------------------|-----------------|--------------
2.3 (GRH55)     | I9020XXJK1           | I9020XXJK8
2.3.1 (GRH78)   | I9020XXJK1           | I9020XXJK8
2.3.2 (GRH78C)  | I9020XXJK1           | I9020XXJK8
2.3.3 (GRI40)   | I9020XXKA3           | I9020XXKB1      | All previous versions
2.3.4 (GRJ22)   | I9020XXKA3           | I9020XXKD1      | All previous versions
2.3.5 (GRJ90)   | I9020XXKA3           | I9020XXKF1      | All previous versions
2.3.6 (GRK39F)  | I9020XXKA3           | I9020XXKF1      | All previous versions

Nexus S (850MHz version "UC"):

Android Version | Preferred Bootloader | Preferred Radio | Also possible
----------------|----------------------|-----------------|--------------
2.3.3 (GRI54)   | I9020XXKA3           | I9020UCKB2
2.3.4 (GRJ22)   | I9020XXKA3           | I9020UCKD1      | All previous versions
2.3.5 (GRJ90)   | I9020XXKA3           | I9020UCKF1      | All previous versions
2.3.6 (GRK39C)  | I9020XXKA3           | I9020UCKF1      | All previous versions
2.3.6 (GRK39F)  | I9020XXKA3           | I9020UCKF1      | All previous versions

Nexus S (Korea version "KR"):

Android Version | Preferred Bootloader | Preferred Radio | Also possible
----------------|----------------------|-----------------|--------------
2.3.3 (GRI54)   | I9020XXKA3           | I9020KRKB3
2.3.4 (GRJ22)   | I9020XXKA3           | M200KRKC1       | All previous versions
2.3.5 (GRJ90)   | I9020XXKA3           | M200KRKC1       | All previous versions
2.3.6 (GRK39F)  | I9020XXKA3           | M200KRKC1       | All previous versions

Nexus S 4G:

Android Version | Preferred Bootloader | Preferred Radio | Also possible
----------------|----------------------|-----------------|--------------
2.3.4 (GRJ06D)  | D720SPRKC5           | D720SPRKC9
2.3.4 (GRJ22)   | D720SPRKC5           | D720SPRKD8      | All previous versions
2.3.5 (GRJ90)   | D720SPRKC5           | D720SPRKE5      | All previous versions
2.3.7 (GWK74)   | D720SPRKE1           | D720SPRKH1 (*)  | All previous versions

If you're building a new version of Android, if your Nexus S or Nexus S 4G has
an older bootloader and radio image that is marked as being also possible in
the table above but is not recognized by fastboot, you can locally
delete the `version-bootloader` and `version-baseband` lines in
`device/samsung/crespo/board-info.txt` or `device/samsung/crespo4g/board-info.txt`.

(*) As a note, radio version D720SPRKH1 for Nexus S 4G sometimes erroneously
reports version D720SPRKE1. If this is the case for your Nexus S 4G, you can
locally modify the version-baseband line in
`device/samsung/crespo4g/board-info.txt` accordingly.
