/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <dirent.h>

#define LOG_TAG "Netd"

#include "cutils/log.h"

#include "CommandListener.h"
#include "NetlinkManager.h"
#include "DnsProxyListener.h"

static void coldboot(const char *path);
static void sigchld_handler(int sig);

int main() {

    CommandListener *cl;
    NetlinkManager *nm;
    DnsProxyListener *dpl;

    LOGI("Netd 1.0 starting");

#ifdef FIRMWARE_PATH
    int fd;
    LOGE("WIFI_DRIVER_FW_PATH_PARAM = %s", (char*)WIFI_DRIVER_FW_PATH_PARAM);
    fd = open(WIFI_DRIVER_FW_PATH_PARAM, O_CREAT|O_RDWR|O_TRUNC, 0666);
    if (write(fd, WIFI_DRIVER_FW_PATH_STA, sizeof(WIFI_DRIVER_FW_PATH_STA)) != sizeof(WIFI_DRIVER_FW_PATH_STA)) {
       LOGE("Failed to write wlan fw path param (%s)", strerror(errno));
    }
#endif
//    signal(SIGCHLD, sigchld_handler);

    if (!(nm = NetlinkManager::Instance())) {
        LOGE("Unable to create NetlinkManager");
        exit(1);
    };


    cl = new CommandListener();
    nm->setBroadcaster((SocketListener *) cl);

    if (nm->start()) {
        LOGE("Unable to start NetlinkManager (%s)", strerror(errno));
        exit(1);
    }

    // Set local DNS mode, to prevent bionic from proxying
    // back to this service, recursively.
    setenv("ANDROID_DNS_MODE", "local", 1);
    dpl = new DnsProxyListener();
    if (dpl->startListener()) {
        LOGE("Unable to start DnsProxyListener (%s)", strerror(errno));
        exit(1);
    }

    /*
     * Now that we're up, we can respond to commands
     */
    if (cl->startListener()) {
        LOGE("Unable to start CommandListener (%s)", strerror(errno));
        exit(1);
    }

    // Eventually we'll become the monitoring thread
    while(1) {
        sleep(1000);
    }

    LOGI("Netd exiting");
    exit(0);
}

static void do_coldboot(DIR *d, int lvl)
{
    struct dirent *de;
    int dfd, fd;

    dfd = dirfd(d);

    fd = openat(dfd, "uevent", O_WRONLY);
    if(fd >= 0) {
        write(fd, "add\n", 4);
        close(fd);
    }

    while((de = readdir(d))) {
        DIR *d2;

        if (de->d_name[0] == '.')
            continue;

        if (de->d_type != DT_DIR && lvl > 0)
            continue;

        fd = openat(dfd, de->d_name, O_RDONLY | O_DIRECTORY);
        if(fd < 0)
            continue;

        d2 = fdopendir(fd);
        if(d2 == 0)
            close(fd);
        else {
            do_coldboot(d2, lvl + 1);
            closedir(d2);
        }
    }
}

static void coldboot(const char *path)
{
    DIR *d = opendir(path);
    if(d) {
        do_coldboot(d, 0);
        closedir(d);
    }
}

static void sigchld_handler(int sig) {
    pid_t pid = wait(NULL);
    LOGD("Child process %d exited", pid);
}
