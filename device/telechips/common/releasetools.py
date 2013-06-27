# Copyright (C) 2010 Telechips, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import common
import re


def InstallBootloader(bootloader_img, api_version, input_zip, info):
    common.ZipWriteStr(info.output_zip, "bootloader.img", bootloader_img)

    if api_version >= 3:
        bitmap_txt = input_zip.read("RADIO/bitmap_size.txt")
        install_img = input_zip.read("RADIO/firmware_install.565")
        error_img = input_zip.read("RADIO/firmware_error.565")
        width, height, bpp = bitmap_txt.split()

        info.script.UnmountAll()
        info.script.AppendExtra(('''
assert(telechips.install_bootloader(package_extract_file("bootloader.img"),
                         %(width)s, %(height)s, %(bpp)s,
                         package_extract_file("install.565"),
                         package_extract_file("error.565")));
''' % locals()).lstrip())

        common.ZipWriteStr(info.output_zip, "install.565", install_img)
        common.ZipWriteStr(info.output_zip, "error.565", error_img)
    elif info.input_version >= 2:
         info.script.AppendExtra(
            'write_firmware_image("PACKAGE:bootloader.img", "bootloader");')


def FullOTA_InstallEnd(info):
    try:
        bootloader_img = info.input_zip.read("RADIO/bootloader.img")
    except KeyError:
        print "warning: no bootloader image in input target_files; not flashing bootloader"
        return

    info.script.Print("Writing bootloader image...")

    InstallBootloader(bootloader_img, info.input_version, info.input_zip, info)


def IncrementalOTA_InstallEnd(info):
    try:
        target_bootloader = info.input_zip.read("RADIO/bootloader.img")
    except KeyError:
        print "warning: bootloader image missing from target: not flashing bootloader"
        return

    try:
        source_bootloader = info.source_zip.read("RADIO/bootloader.img")
    except KeyError:
        source_bootloader = None

    if source_bootloader == target_bootloader:
        print "bootloader image unchanged; skipping"
    return

    InstallBootloader(bootloader_bootloader, info.target_version, info.target_zip, info)
