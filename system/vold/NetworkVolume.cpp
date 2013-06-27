/*
 * Copyright (C) 2011 Telechips Inc.
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
#include <string.h>
#include <errno.h>

#include <linux/kdev_t.h>

#define LOG_TAG "NetworkVolume"

#include <cutils/log.h>
#include <sysutils/NetlinkEvent.h>
#include <sys/mount.h>

#include <cutils/properties.h>
#include <sqlite3.h>

#include "NetworkVolume.h"
#include "VolumeManager.h"
#include "ResponseCode.h"
#include "Process.h"

sqlite3 *pDbHandle = NULL;

NetworkVolume::NetworkVolume(VolumeManager *vm, const char *label,
                           const char *mount_point) :
              Volume(vm, label, mount_point) {
    setState(Volume::State_NoMedia);
    property_set("tcc.networkfilesystem.support", "enable");
}

NetworkVolume::~NetworkVolume() {
    if (pDbHandle) sqlite3_close(pDbHandle);
}

int NetworkVolume::mountVol() {
    char mountCmd[256];
    char *mode, *remote_ip, *remote_path, *user_id, *user_pw;

    setState(Volume::State_Pending);

    int rc;
    char szSQL[1024];
    sqlite3_stmt *stmt;
    sprintf(szSQL, "SELECT connect_mode, remote_ip, remote_path, user_id, user_pw FROM nfs_info WHERE local_path='%s'", getMountpoint());

    if (pDbHandle == NULL) {
        sqlite3_open("/data/system/nfs_settings.db", &pDbHandle);
    }
    sqlite3_prepare(pDbHandle, szSQL, strlen(szSQL), &stmt, NULL);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        LOGE("Can not open Database for nfs!");
        setState(Volume::State_NoMedia);
        return -1;
    }

    mode = (char *)sqlite3_column_text(stmt, 0);
    remote_ip = (char *)sqlite3_column_text(stmt, 1);
    remote_path = (char *)sqlite3_column_text(stmt, 2);
    user_id = (char *)sqlite3_column_text(stmt, 3);
    user_pw = (char *)sqlite3_column_text(stmt, 4);

    if (!strcmp((char *)mode, "nfs")) {
        sprintf(mountCmd, "busybox mount -t nfs -o vers=3,nolock %s:/%s %s",
                remote_ip, remote_path, getMountpoint());
    } else if (!strcmp((char *)mode, "cifs")) {
        sprintf(mountCmd, "busybox mount -t cifs -o user=%s,password=%s,iocharset=utf8 //%s/%s %s",
                user_id, user_pw, remote_ip, remote_path, getMountpoint());
    }
    sqlite3_finalize(stmt);

    if (system(mountCmd)) {
        setState(Volume::State_NoMedia);
        LOGE("Failed to mount %s", getMountpoint());
        return -1;
    } else {
        setState(Volume::State_Mounted);
        return 0;
    }
}

int NetworkVolume::doUnmount(const char *path, bool force) {
    int retries = 10;

    if (mDebug) {
        SLOGD("Unmounting {%s}, force = %d", path, force);
    }

    while (retries--) {
        if (!umount(path) || errno == EINVAL || errno == ENOENT) {
            SLOGI("%s sucessfully unmounted", path);
            return 0;
        }

        int action = 0;

        if (force) {
            if (retries == 1) {
                action = 2; // SIGKILL
            } else if (retries == 2) {
                action = 1; // SIGHUP
            }
        }

        SLOGW("Failed to unmount %s (%s, retries %d, action %d)",
                path, strerror(errno), retries, action);

        Process::killProcessesWithOpenFiles(path, action);
        usleep(1000*1000);
    }
    errno = EBUSY;
    SLOGE("Giving up on unmount %s (%s)", path, strerror(errno));
    return -1;
}

int NetworkVolume::unmountVol(bool force, bool revert) {
    char msg[255];

    if (getState() != Volume::State_Mounted) {
        SLOGE("Volume %s unmount request when not mounted", getLabel());
        errno = EINVAL;
        return UNMOUNT_NOT_MOUNTED_ERR;
    }

    setState(Volume::State_Unmounting);
    usleep(1000 * 1000); // Give the framework some time to react

    if (doUnmount(getMountpoint(), force)) {
        SLOGE("Failed to unmount %s (%s)", SEC_STGDIR, strerror(errno));
        setState(Volume::State_Mounted);
        return -1;
    }

    mCurrentlyMountedKdev = -1;

    setState(Volume::State_Idle);

    snprintf(msg, sizeof(msg), "Volume %s %s nfs removed (0:0)",
             getLabel(), getMountpoint());
    mVm->getBroadcaster()->sendBroadcast(ResponseCode::VolumeDiskRemoved,
                                             msg, false);
    setState(Volume::State_NoMedia);
    return 0;
}
