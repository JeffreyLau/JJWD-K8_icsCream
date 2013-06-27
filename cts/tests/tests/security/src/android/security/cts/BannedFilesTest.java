/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.security.cts;

import android.os.cts.FileUtils;

import junit.framework.TestCase;

public class BannedFilesTest extends TestCase {

    /**
     * setuid or setgid "ip" command can be used to modify the
     * routing tables of a device, potentially allowing a malicious
     * program to intercept all network traffic to and from
     * the device.
     */
    public void testNoSetuidIp() {
        assertNotSetugid("/system/bin/ip");
        assertNotSetugid("/system/xbin/ip");
        assertNotSetugid("/vendor/bin/ip");
    }

    /**
     * setuid or setgid tcpdump can be used maliciously to monitor
     * all traffic in and out of the device.
     */
    public void testNoSetuidTcpdump() {
        assertNotSetugid("/system/bin/tcpdump");
        assertNotSetugid("/system/bin/tcpdump-arm");
        assertNotSetugid("/system/xbin/tcpdump");
        assertNotSetugid("/system/xbin/tcpdump-arm");
        assertNotSetugid("/vendor/bin/tcpdump");
        assertNotSetugid("/vendor/bin/tcpdump-arm");
    }

    private static void assertNotSetugid(String file) {
        FileUtils.FileStatus fs = new FileUtils.FileStatus();
        if (!FileUtils.getFileStatus(file, fs, false)) {
            return;
        }
        assertTrue((fs.mode & FileUtils.S_ISUID) == 0);
        assertTrue((fs.mode & FileUtils.S_ISGID) == 0);
    }
}
