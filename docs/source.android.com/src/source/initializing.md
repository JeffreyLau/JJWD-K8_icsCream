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

# Initializing a Build Environment #

The "Getting Started" section describes how to set up your local work environment, how to use Repo to get the Android files, and how to build the files on your machine.  To build the Android source files, you will need to use Linux or Mac OS. Building under Windows is not currently supported.

*Note: The source is approximately 2.6GB in size. You will need 10GB free to complete the build.*

For an overview of the entire code-review and code-update process, see [Life of a Patch](life-of-a-patch.html).

To see snapshots and histories of the files available in the public Android repositories, visit the [GitWeb](http://android.git.kernel.org) web interface.



# Setting up a Linux build environment #

The Android build is routinely tested in house on recent versions of Ubuntu (10.04 and later), but most distributions should have the required build tools available.  Reports of successes or failures on other distributions are welcome.  

*Note: It is also possible to build Android in a virtual machine.  If you are running Linux in a virtual machine, you will need at least 8GB of RAM/swap and 12GB or more of disk space in order to build the Android tree.*

In general you will need:

 - Python 2.4 -- 2.7, which you can download from [python.org](http://www.python.org/download/).

 - JDK 6 if you wish to build Gingerbread or newer; JDK 5 for Froyo or older.  You can download both from [java.sun.com](http://java.sun.com/javase/downloads/).

 - Git 1.5.4 or newer. You can find it at [git-scm.com](http://git-scm.com/download).

 - (optional) Valgrind, a tool that will help you find memory leaks, stack corruption, array bounds overflows, etc. Download from [valgrind.org](http://valgrind.org/downloads/current.html).

Detailed instructions for Ubuntu 10.04+ follow.

## Installing the JDK ##

The Sun JDK is no longer in Ubuntu's main package repository.  In order to download it, you need to add the appropriate repository and indicate to the system which JDK should be used.

Java 6: for Gingerbread and newer

    $ sudo add-apt-repository "deb http://archive.canonical.com/ lucid partner"
    $ sudo add-apt-repository "deb-src http://archive.canonical.com/ubuntu lucid partner"
    $ sudo apt-get update
    $ sudo apt-get install sun-java6-jdk

Java 5: for Froyo and older

    $ sudo add-apt-repository "deb http://archive.ubuntu.com/ubuntu hardy main multiverse"
    $ sudo add-apt-repository "deb http://archive.ubuntu.com/ubuntu hardy-updates main multiverse"
    $ sudo apt-get update
    $ sudo apt-get install sun-java5-jdk

## Installing required packages ##

To set up your development environment, install the following required packages:

    $ sudo apt-get install git-core gnupg flex bison gperf build-essential \
      zip curl zlib1g-dev libc6-dev lib32ncurses5-dev ia32-libs \
      x11proto-core-dev libx11-dev lib32readline5-dev lib32z-dev \
      libgl1-mesa-dev g++-multilib mingw32 tofrodos python-markdown \
      libxml2-utils

## Configuring USB Access ##

Under GNU/linux systems (and specifically under Ubuntu systems),
regular users can't directly access USB devices by default. The
system needs to be configured to allow such access.

The recommended approach is to create a file
`/etc/udev/rules.d/51-android.rules` (as the root user) and to copy
the following lines in it. <username> must be replaced by the
actual username of the user who is authorized to access the phones
over USB.

    # adb protocol on passion (Nexus One)
    SUBSYSTEM=="usb", ATTR{idVendor}=="18d1", ATTR{idProduct}=="4e12", MODE="0600", OWNER="<username>"
    # fastboot protocol on passion (Nexus One)
    SUBSYSTEM=="usb", ATTR{idVendor}=="0bb4", ATTR{idProduct}=="0fff", MODE="0600", OWNER="<username>"
    # adb protocol on crespo/crespo4g (Nexus S)
    SUBSYSTEM=="usb", ATTR{idVendor}=="18d1", ATTR{idProduct}=="4e22", MODE="0600", OWNER="<username>"
    # fastboot protocol on crespo/crespo4g (Nexus S)
    SUBSYSTEM=="usb", ATTR{idVendor}=="18d1", ATTR{idProduct}=="4e20", MODE="0600", OWNER="<username>"

Those new rules take effect the next time a device is plugged in.
It might therefore be necessary to unplug the device and plug it
back into the computer.

This is known to work on both Ubuntu Hardy Heron (8.04.x LTS) and
Lucid Lynx (10.04.x LTS). Other versions of Ubuntu or other
variants of GNU/linux might require different configurations.


# Setting up a Mac OS X build environment #

To build the Android files in a Mac OS environment, you need an Intel/x86 machine running MacOS 10.4 (Tiger), 10.5 (Leopard), or 10.6 (Snow Leopard). The Android build system and tools do not support the obsolete PowerPC architecture.

Android must be built on a case-sensitive file system because the sources contain files that differ only in case. We recommend that you build Android on a partition that has been formatted with the journaled file system HFS+.  HFS+ is required to successfully build Mac OS applications such as the Android Emulator for OS X.

## Creating a case sensitive disk image ##

If you want to avoid partitioning/formatting your hard drive, you can use
a case-sensitive disk image instead. To create the image, launch Disk
Utility and select "New Image".  A size of 12GB is the minimum to
complete the build, larger numbers are more future-proof. Using sparse images
saves space while allowing to grow later as the need arises. Be sure to select
"case sensitive, journaled" as the volume format.

You can also create it from a shell with the following command:

    # hdiutil create -type SPARSE -fs 'Case-sensitive Journaled HFS+' -size 40g ~/android.dmg

This will create a .dmg file which, once mounted, acts as a drive with the required formatting for Android development. For a disk image named "android.dmg" stored in your home directory, you can add the following to your `~/.bash_profile` to mount the image when you execute "mountAndroid": 

    # mount the android file image
    function mountAndroid { hdiutil attach ~/android.dmg -mountpoint /Volumes/android; }

Once mounted, you'll do all your work in the "android" volume. You can eject it (unmount it) just like you would with an external drive.

## Installing required packages ##

- Install XCode from [the Apple developer site](http://developer.apple.com/). We recommend version 3.0 or newer.  If you are not already registered as an Apple developer, you will have to create an Apple ID in order to download.

- Install MacPorts from [macports.org](http://www.macports.org/install.php).

    *Note: Make sure that `/opt/local/bin` appears in your path BEFORE `/usr/bin`.  If not, add* 

        export PATH=/opt/local/bin:$PATH

    *to your `~/.bash_profile`.*

- Get make, git, and GPG packages from MacPorts: 

        $ POSIXLY_CORRECT=1 sudo port install gmake libsdl git-core gnupg

    If using Mac OS 10.4, also install bison:

        $ POSIXLY_CORRECT=1 sudo port install bison

## Reverting from make 3.82 ##

There is a bug in gmake 3.82 that prevents android from building.  You can install version 3.81 using MacPorts by taking the following steps:

- Edit `/opt/local/etc/macports/sources.conf` and add a line that says
    
        file:///Users/Shared/dports

    above the rsync line.  Then create this directory: 

        $ mkdir /Users/Shared/dports

- In the new `dports` directory, run 

        $ svn co --revision 50980 http://svn.macports.org/repository/macports/trunk/dports/devel/gmake/ devel/gmake/

- Create a port index for your new local repository: 

        $ portindex /Users/Shared/dports

- Finally, install the old version of gmake with 

        $ sudo port install gmake @3.81

## Setting a file descriptor limit ##

On MacOS the default limit on the number of simultaneous file descriptors open is too low and a highly parallel build process may exceed this limit.  

To increase the cap, add the following lines to your `~/.bash_profile`: 

    # set the number of open files to be 1024
    ulimit -S -n 1024

# Next: Download the source #

Your build environment is good to go!  Proceed to [downloading the source](downloading.html)....
