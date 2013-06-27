<!--
   Copyright 2011 The Android Open Source Project

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

# Notes on the implementation of encryption in Android 3.0
<br />

## Quick summary for 3rd parties.

If you want to enable encryption on your device based on Android 3.0
aka Honeycomb, there are only a few requirements:

1.  The /data filesystem must be on a device that presents a block device
    interface.  eMMC is used in the first devices.  This is because the
    encryption is done by the dm-crypt layer in the kernel, which works
    at the block device layer.

2.  The function get_fs_size() in system/vold/cryptfs.c assumes the filesystem
    used for /data is ext4.  It's just error checking code to make sure the
    filesystem doesn't extend into the last 16 Kbytes of the partition where
    the crypto footer is kept.  It was useful for development when sizes were
    changing, but should not be required for release.  If you are not using
    ext4, you can either delete it and the call to it, or fix it to understand
    the filesystem you are using.

3.  Most of the code to handle the setup and teardown of the temporary framework
    is in files that are not usually required to be changed on a per device
    basis.  However, the init.<device>.rc file will require some changes.  All
    services must be put in one of three classes: core, main or late_state.
    Services in the core class are not shutdown and restarted when the
    temporary framework gets the disk password.  Services in the main class
    are restarted when the framework is restarted.  Services in late_start are
    not started until after the temporary framework is restarted.  Put services
    here that are not required to be running while the temporary framework
    gets the disk password.

    Also any directories that need to be created on /data that are device
    specific need to be in the Action for post-fs-data, and that Action must end
    with the command "setprop vold.post_fs_data_done 1".  If your
    init.<device>.rc file does not have a post-fs-data Action, then the
    post-fs-data Action in the main init.rc file must end with the command
    "setprop vold.post_fs_data_done 1".


## How Android encryption works

Disk encryption on Android is based on dm-crypt, which is a kernel feature that
works at the block device layer.  Therefore, it is not usable with YAFFS, which
talks directly to a raw nand flash chip, but does work with emmc and similar
flash devices which present themselves to the kernel as a block device.  The
current preferred filesystem to use on these devices is ext4, though that is
independent of whether encryption is used or not.

While the actual encryption work is a standard linux kernel feature, enabling it
on an Android device proved somewhat tricky.  The Android system tries to avoid
incorporating GPL components, so using the cryptsetup command or libdevmapper
were not available options.  So making the appropriate ioctl(2) calls into the
kernel was the best choice.  The Android volume daemon (vold) already did this
to support moving apps to the SD card, so I chose to leverage that work
for whole disk encryption.  The actual encryption used for the filesystem for
first release is 128 AES with CBC and ESSIV:SHA256.  The master key is
encrypted with 128 bit AES via calls to the openssl library.

Once it was decided to put the smarts in vold, it became obvious that invoking
the encryption features would be done like invoking other vold commands, by
adding a new module to vold (called cryptfs) and teaching it various commands.
The commands are checkpw, restart, enablecrypto, changepw and cryptocomplete.
They will be described in more detail below.

The other big issue was how to get the password from the user on boot.  The
initial plan was to implement a minimal UI that could be invoked from init
in the initial ramdisk, and then init would decrypt and mount /data.  However,
the UI engineer said that was a lot of work, and suggested instead that init
communicate upon startup to tell the framework to pop up the password entry
screen, get the password, and then shutdown and have the real framework started.
It was decided to go this route, and this then led to a host of other decisions
described below.  In particular, init set a property to tell the framework to go
into the special password entry mode, and that set the stage for much
communication between vold, init and the framework using properties.  The
details are described below.

Finally, there were problems around killing and restarting various services
so that /data could be unmounted and remounted.  Bringing up the temporary
framework to get the user password requires that a tmpfs /data filesystem be
mounted, otherwise the framework will not run.  But to unmount the tmpfs /data
filesystem so the real decrypted /data filesystem could be mounted meant that
every process that had open files on the tmpfs /data filesystem had to be killed
and restarted on the real /data filesystem.  This magic was accomplished by
requiring all services to be in 1 of 3 groups: core, main and late_start.
Core services are never shut down after starting.  main services are shutdown
and then restarted after the disk password is entered.  late_start services
are not started until after /data has been decrypted and mounted.  The magic
to trigger these actions is by setting the property vold.decrypt to various
magic strings, which is described below.  Also, a new init command "class_reset"
was invented to stop a service, but allow it to be restarted with a
"class_start" command.  If the command "class_stop" was used instead of the
new command "class_reset" the flag SVC_DISABLED was added to the state of
any service stopped, which means it would not be started when the command
class_start was used on its class.

## Booting an encrypted system.

1.  When init fails to mount /data, it assumes the filesystem  is encrypted,
    and sets several properties:
      ro.crypto.state = "encrypted"
      vold.decrypt = 1
    It then mounts a /data on a tmpfs ramdisk, using parameters it picks
    up from ro.crypto.tmpfs_options, which is set in init.rc.

    If init was able to mount /data, it sets ro.crypto.state to "unencrypted".

    In either case, init then sets 5 properties to save the initial mount
    options given for /data in these properties:
        ro.crypto.fs_type
        ro.crypto.fs_real_blkdev
        ro.crypto.fs_mnt_point
        ro.crypto.fs_options
        ro.crypto.fs_flags (saved as an ascii 8 digit hex number preceded by 0x)

2.  The framework starts up, and sees that vold.decrypt is set to "1".  This
    tells the framework that it is booting on a tmpfs /data disk, and it needs
    to get the user password.  First, however, it needs to make sure that the
    disk was properly encrypted.  It sends the command "cryptfs cryptocomplete"
    to vold, and vold returns 0 if encryption was completed successfully, or -1
    on internal error, or -2 if encryption was not completed successfully. 
    Vold determines this by looking in the crypto footer for the
    CRYPTO_ENCRYPTION_IN_PROGRESS flag.  If it's set, the encryption process
    was interrupted, and there is no usable data on the device.  If vold returns
    an error, the UI should pop up a message saying the user needs to reboot and
    factory reset the device, and give the user a button to press to do so.
    
3.  Assuming the "cryptfs cryptocomplete" command returned success, the
    framework should pop up a UI asking for the disk password.  The UI then
    sends the command "cryptfs checkpw <passwd>" to vold.  If the password
    is correct (which is determined by successfully mounting the decrypted
    at a temporary location, then unmounting it), vold saves the name of the
    decrypted block device in the property ro.crypto.fs_crypto_blkdev, and
    returns status 0 to the UI.  If the password is incorrect, it returns -1
    to the UI.

4.  The UI puts up a crypto boot graphic, and then calls vold with the command
    "cryptfs restart".  vold sets the property vold.decrypt to
    "trigger_reset_main", which causes init.rc to do "class_reset main".  This
    stops all services in the main class, which allows the tmpfs /data to be
    unmounted.  vold then mounts the decrypted real /data partition, and then
    preps the new partition (which may never have been prepped if it was
    encrypted with the wipe option, which is not supported on first release).
    It sets the property vold.post_fs_data_done to "0", and then sets
    vold.decrypt to "trigger_post_fs_dat".  This causes init.rc to run the
    post-fs-data commands in init.rc and init.<device>.rc.  They will create
    any necessary directories, links, et al, and then set vold.post_fs_data_done
    to "1".  Vold waits until it sees the "1" in that property.  Finally, vold
    sets the property vold.decrypt to "trigger_restart_framework" which causes
    init.rc to start services in class main again, and also start services
    in class late_start for the first time since boot.

    Now the framework boots all its services using the decrypted /data
    filesystem, and the system is ready for use.

## Enabling encryption on the device.

For first release, we only support encrypt in place, which requires the
framework to be shutdown, /data unmounted, and then every sector of the
device encrypted, after which the device reboots to go through the process
described above.  Here are the details:

1.  From the UI, the user selects to encrypt the device.  The UI ensures that
    there is a full charge on the battery, and the AC adapter is plugged in.
    It does this to make sure there is enough power to finish the encryption
    process, because if the device runs out of power and shuts down before it
    has finished encrypting, file data is left in a partially encrypted state,
    and the device must be factory reset (and all data lost).

    Once the user presses the final button to encrypt the device, the UI calls
    vold with the command "cryptfs enablecrypto inplace <passwd>" where passwd
    is the user's lock screen password.

2.  vold does some error checking, and returns -1 if it can't encrypt, and
    prints a reason in the log.  If it thinks it can, it sets the property
    vold.decrypt to "trigger_shutdown_framework".  This causes init.rc to
    stop services in the classes late_start and main.  vold then unmounts
    /mnt/sdcard and then /data.
    
3.  If doing an inplace encryption, vold then mounts a tmpfs /data (using the
    tmpfs options from ro.crypto.tmpfs_options) and sets the property
    vold.encrypt_progress to "0".  It then preps the tmpfs /data filesystem as
    mentioned in step 3 for booting an encrypted system, and then sets the
    property vold.decrypt to "trigger_restart_min_framework".  This causes
    init.rc to start the main class of services.  When the framework sees that
    vold.encrypt_progress is set to "0", it will bring up the progress bar UI,
    which queries that property every 5 seconds and updates a progress bar.

4.  vold then sets up the crypto mapping, which creates a virtual crypto block
    device that maps onto the real block device, but encrypts each sector as it
    is written, and decrypts each sector as it is read.  vold then creates and
    writes out the crypto footer.
    
    The crypto footer contains details on the type of encryption, and an
    encrypted copy of the master key to decrypt the filesystem.  The master key
    is a 128 bit number created by reading from /dev/urandom.  It is encrypted
    with a hash of the user password created with the PBKDF2 function from the
    SSL library.  The footer also contains a random salt (also read from
    /dev/urandom) used to add entropy to the hash from PBKDF2, and prevent
    rainbow table attacks on the password.  Also, the flag
    CRYPT_ENCRYPTION_IN_PROGRESS is set in the crypto footer to detect failure
    to complete the encryption process.  See the file cryptfs.h for details
    on the crypto footer layout.  The crypto footer is kept in the last 16
    Kbytes of the partition, and the /data filesystem cannot extend into that
    part of the partition.

5.  If told was to enable encryption with wipe, vold invokes the command
    "make_ext4fs" on the crypto block device, taking care to not include
    the last 16 Kbytes of the partition in the filesystem.

    If the command was to enable inplace, vold starts a loop to read each sector
    of the real block device, and then write it to the crypto block device.
    This takes about an hour on a 30 Gbyte partition on the Motorola Xoom.
    This will vary on other hardware.  The loop updates the property
    vold.encrypt_progress every time it encrypts another 1 percent of the
    partition.  The UI checks this property every 5 seconds and updates
    the progress bar when it changes.

6.  When either encryption method has finished successfully, vold clears the
    flag ENCRYPTION_IN_PROGRESS in the footer, and reboots the system.
    If the reboot fails for some reason, vold sets the property
    vold.encrypt_progress to "error_reboot_failed" and the UI should
    display a message asking the user to press a button to reboot.
    This is not expected to ever occur.

7.  If vold detects an error during the encryption process, and if no data has
    been destroyed yet and the framework is up, vold sets the property
    vold.encrypt_progress to "error_not_encrypted" and the UI should give the
    user the option to reboot, telling them that the encryption process
    never started.  If the error occurs after the framework has been torn
    down, but before the progress bar UI is up, vold will just reboot the
    system.  If the reboot fails, it sets vold.encrypt_progress to
    "error_shutting_down" and returns -1, but there will not be anyone
    to catch the error.  This is not expected to happen.
    
    If vold detects an error during the encryption process, it sets
    vold.encrypt_progress to "error_partially_encrypted" and returns -1.
    The UI should then display a message saying the encryption failed, and
    provide a button for the user to factory reset the device.

## Changing the password

To change the password for the disk encryption, the UI sends the command
"cryptfs changepw <newpw>" to vold, and vold re-encrypts the disk master
key with the new password.

## Summary of related properties

Here is a table summarizing the various properties, their possible values,
and what they mean:


    vold.decrypt  1                               Set by init to tell the UI to ask
                                                  for the disk pw
    
    vold.decrypt  trigger_reset_main              Set by vold to shutdown the UI
                                                  asking for the disk password
    
    vold.decrypt  trigger_post_fs_data            Set by vold to prep /data with
                                                  necessary dirs, et al.
    
    vold.decrypt  trigger_restart_framework       Set by vold to start the real
                                                  framework and all services
    
    vold.decrypt  trigger_shutdown_framework      Set by vold to shutdown the full
                                                  framework to start encryption
    
    vold.decrypt  trigger_restart_min_framework   Set by vold to start the progress
                                                  bar UI for encryption.
    
    vold.enrypt_progress                          When the framework starts up, if
                                                  this property is set, enter the
                                                  progress bar UI mode.
    
    vold.encrypt_progress  0 to 100               The progress bar UI should display
                                                  the percentage value set.
    
    vold.encrypt_progress  error_partially_encrypted  The progress bar UI should
                                                      display a message that the
                                                      encryption failed, and give
                                                      the user an option to factory
                                                      reset the device.
    
    vold.encrypt_progress  error_reboot_failed    The progress bar UI should display
                                                  a message saying encryption
                                                  completed, and give the user a
                                                  button to reboot the device.
                                                  This error is not expected to
                                                  happen.
    
    vold.encrypt_progress  error_not_encrypted    The progress bar UI should display
                                                  a message saying an error occured,
                                                  and no data was encrypted or lost,
                                                  and give the user a button to
                                                  reboot the system.
    
    vold.encrypt_progress  error_shutting_down    The progress bar UI is not
                                                  running, so it's unclear who
                                                  will respond to this error,
                                                  and it should never happen
                                                  anyway.
    
    vold.post_fs_data_done  0                     Set by vold just before setting
                                                  vold.decrypt to
                                                  trigger_post_fs_data.
    
    vold.post_fs_data_done  1                     Set by init.rc or init.<device>.rc
                                                  just after finishing the task
                                                  post-fs-data.
     
    ro.crypto.fs_crypto_blkdev                    Set by the vold command checkpw
                                                  for later use by the vold command
                                                  restart.
    
    ro.crypto.state unencrypted                   Set by init to say this system is
                                                  running with an unencrypted /data
    
    ro.crypto.state encrypted                     Set by init to say this system is
                                                  running with an encrypted /data
    
    ro.crypto.fs_type                             These 5 properties are set by init
    ro.crypto.fs_real_blkdev                      when it tries to mount /data with
    ro.crypto.fs_mnt_point                        parameters passed in from init.rc.
    ro.crypto.fs_options                          vold uses these to setup the
    ro.crypto.fs_flags                            crypto mapping.
    
    ro.crypto.tmpfs_options                       Set by init.rc with the options
                                                  init should use when mounting
                                                  the tmpfs /data filesystem.

## Summary of new init actions

A list of the new Actions that are added to init.rc and/or init.<device>.rc:

    on post-fs-data
    on nonencrypted
    on property:vold.decrypt=trigger_reset_main
    on property:vold.decrypt=trigger_post_fs_data
    on property:vold.decrypt=trigger_restart_min_framework
    on property:vold.decrypt=trigger_restart_framework
    on property:vold.decrypt=trigger_shutdown_framework


