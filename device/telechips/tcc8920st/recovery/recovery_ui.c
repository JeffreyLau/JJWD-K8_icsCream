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

#include <linux/input.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "recovery_ui.h"
#include "common.h"

#define REM_DPAD_UP             19
#define REM_DPAD_DOWN           20
#define REM_DPAD_LEFT           21
#define REM_DPAD_RIGHT          22
#define REM_ENTER               66
#define REM_MENU                82

char* MENU_HEADERS[] = { "Volume up/down to move highlight;",
                         "power button to select.",
                         "",
                         NULL };

char* MENU_ITEMS[] = { "reboot system now",
                       "wipe data/factory reset",
                       "wipe cache partition",
                       "apply update from /usb",
//                       "apply update from /sata",
//                       "apply update from /sdcard",
//                       "apply update from /cache",
                       NULL };

void device_ui_init(UIParameters* ui_parameters) {
}

int device_recovery_start() {
    return 0;
}

int device_toggle_display(volatile char* key_pressed, int key_code) {
    return key_code == KEY_HOMEPAGE;
}

int device_reboot_now(volatile char* key_pressed, int key_code) {
    return 0;
}

int device_handle_key(int key_code, int visible) {
    if (visible) {
        switch (key_code) {
            case REM_DPAD_DOWN:
            case KEY_DOWN:
            case KEY_VOLUMEDOWN:
                return HIGHLIGHT_DOWN;

            case REM_DPAD_UP:
            case KEY_UP:
            case KEY_VOLUMEUP:
                return HIGHLIGHT_UP;

            case REM_ENTER:
            case KEY_ENTER:
                return SELECT_ITEM;
        }
    }

    return NO_ACTION;
}

int device_perform_action(int which) {
    switch (which) {
        case 0:
            return ITEM_REBOOT;
        case 1:
            return ITEM_WIPE_DATA;
        case 2:
            return ITEM_WIPE_CACHE;
        case 3:
            return ITEM_APPLY_USB;
/*
        case 4:
            return ITEM_APPLY_SATA;
        case 5:
            return ITEM_APPLY_SDCARD;
        case 6:
            return ITEM_APPLY_CACHE;
*/
        default:
            return which;
    }
}

int device_wipe_data() {
    return 0;
}
