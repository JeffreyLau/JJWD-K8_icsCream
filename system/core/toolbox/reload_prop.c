/*
 * Copyright (C) 2007 The Android Open Source Project
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
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#include <cutils/misc.h>
#include <cutils/sockets.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/atomics.h>
#include <private/android_filesystem_config.h>
#include <cutils/log.h>

//#include "property_service.h"
//#include "init.h"
//#include "util.h"
//#include "log.h"

#define PERSISTENT_PROPERTY_DIR  "/data/property"


int reload_prop_main(int argc, char **argv)
{
    DIR* dir = opendir(PERSISTENT_PROPERTY_DIR);
    struct dirent*  entry;
    char path[PATH_MAX];
    char value[PROP_VALUE_MAX];
    int fd, length;
    int retval = 0;

    SLOGE("quickboot profile start loading properties\n");

    if (dir) {
        while ((entry = readdir(dir)) != NULL) {
            if (strncmp("persist.", entry->d_name, strlen("persist.")))
                continue;

            if (!strncmp("persist.sys.spdif_setting", entry->d_name, strlen("persist.sys.spdif_setting")))
                continue;

            if (!strncmp("persist.sys.output_mode", entry->d_name, strlen("persist.sys.output_mode")))
                continue;

            if (!strncmp("persist.sys.hdmi_resolution", entry->d_name, strlen("persist.sys.hdmi_resolution")))
                continue;

            if (!strncmp("persist.sys.composite_mode", entry->d_name, strlen("persist.sys.composite_mode")))
                continue;

            if (!strncmp("persist.sys.component_mode", entry->d_name, strlen("persist.sys.component_mode")))
                continue;

                
#if HAVE_DIRENT_D_TYPE
            if (entry->d_type != DT_REG)
                continue;
#endif
             
            /* open the file and read the property value */
            snprintf(path, sizeof(path), "%s/%s", PERSISTENT_PROPERTY_DIR, entry->d_name);

            fd = open(path, O_RDONLY);
            if (fd >= 0) {
                length = read(fd, value, sizeof(value) - 1);
                value[sizeof(value)-1] = 0;
                if (length > 0) {
                    value[length] = 0;

                    //SLOGE("quickboot profile start property_set(%s %s)\n", entry->d_name, value);
                    property_set(entry->d_name, value);
                    //SLOGE("quickboot profile end property_set(%s %s)\n", entry->d_name, value);
                    //SLOGI("property_set(%s %s)\n", entry->d_name, value);
                } else {
                    SLOGE("Unable to read persistent property file %s errno: %d\n", path, errno);
                    retval = -1;
                }
                close(fd);
            } else {
                SLOGE("Unable to open persistent property file %s errno: %d\n", path, errno);
                retval = -1;
            }
        }
        closedir(dir);

    } else {
        SLOGE("Unable to open persistent property directory %s errno: %d\n", PERSISTENT_PROPERTY_DIR, errno);
        retval = -1;
    }    

    SLOGE("quickboot profile end loading properties\n");
    property_set("tcc.hibernate.property", "reloaded");

    return retval;
}
