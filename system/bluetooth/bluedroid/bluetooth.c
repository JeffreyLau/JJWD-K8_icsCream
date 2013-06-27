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

#define LOG_TAG "bluedroid"

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cutils/log.h>
#include <cutils/properties.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <bluedroid/bluetooth.h>

#ifndef HCI_DEV_ID
#define HCI_DEV_ID 0
#endif

#define HCID_STOP_DELAY_USEC 500000

#define MIN(x,y) (((x)<(y))?(x):(y))
#define TCC_BT_DEVICE_PATH			"/dev/tcc_bt_dev"
#define BT_DEV_MAJOR_NUM 			234
#define IOCTL_BT_DEV_POWER   		_IO(BT_DEV_MAJOR_NUM, 100)
#define IOCTL_BT_DEV_SPECIFIC		_IO(BT_DEV_MAJOR_NUM, 101)

#define BT_DEV_ON					12
#define BT_DEV_OFF					0

#define CSR_MODULE                  0x12
#define BRCM_MODULE                 0x34
#define RDA_MODULE                  0x56
#define ATHEROS_MODULE				0x78

static int rfkill_id = -1;
static char *rfkill_state_path = NULL;

int bt_on_off_flag = 0;

typedef struct 
{
	int module;  // 0x12:CSR, 0x34:Broadcom
	int TMP1;
	int TMP2;
} tcc_bt_info_t;

static int tcc_bluetooth_vedor_tool(int on)
{
#if 1
	int tcc_bt_fd = -1;
	 int ret = -1;
	char supp_status[PROPERTY_VALUE_MAX] = {'\0'};
	int running_time_count = 0;
	int bt_on_off;
	tcc_bt_info_t bt_info;

	tcc_bt_fd = open(TCC_BT_DEVICE_PATH, O_RDWR);
	if( tcc_bt_fd < 0 ) 
	{
		LOGI("[##### TCC BT #####] [%s] open error[%d]", TCC_BT_DEVICE_PATH, tcc_bt_fd);
		return -1;
	}

	if(on)
	{
        	bt_info.module = 0;
	    	ioctl(tcc_bt_fd, IOCTL_BT_DEV_SPECIFIC, &bt_info);

        	LOGI("[##### TCC BT #####] BT_INFO module[%x]", bt_info.module);
	    
		if(bt_info.module == CSR_MODULE)
		{
	    		LOGI("[##### TCC BT #####] Starting bccmd app");
	    		if (property_set("ctl.start", "bccmd") < 0) {
	    			LOGE("Failed to start bccmd");
	    			goto out;
	    		}
	    		
	    		do
	    		{
	    			usleep(200000);
	    			running_time_count++;
	    			property_get("init.svc.bccmd", supp_status, NULL);
	    		}while(strcmp(supp_status, "stopped") != 0);
	    		
	    		LOGI("[##### TCC BT #####] Stopped bccmd app count[%d]", running_time_count);
	    		usleep(500000);

	        }
	}
	if (tcc_bt_fd >= 0)	close( tcc_bt_fd );
	return 0;
out:	
	if (tcc_bt_fd >= 0)	close( tcc_bt_fd );
	return -1;
#endif
}



static int init_rfkill() {
    char path[64];
    char buf[16];
    int fd;
    int sz;
    int id;

	LOGI("[BT] init_rfkill..\n");
    for (id = 0; ; id++) {
        snprintf(path, sizeof(path), "/sys/class/rfkill/rfkill%d/type", id);
        fd = open(path, O_RDONLY);
        if (fd < 0) {
            LOGW("open(%s) failed: %s (%d)\n", path, strerror(errno), errno);
            return -1;
        }
        sz = read(fd, &buf, sizeof(buf));
        close(fd);
        if (sz >= 9 && memcmp(buf, "bluetooth", 9) == 0) {
            rfkill_id = id;
            break;
        }
    }
	LOGI("[BT] init_rfkill rfkill_id[%d]\n", rfkill_id);

    asprintf(&rfkill_state_path, "/sys/class/rfkill/rfkill%d/state", rfkill_id);
    return 0;
}

static int check_bluetooth_power() {
    int sz;
    int fd = -1;
    int ret = -1;
    char buffer;

//	return bt_on_off_flag;

    if (rfkill_id == -1) {
        if (init_rfkill()) goto out;
    }

    fd = open(rfkill_state_path, O_RDONLY);
    if (fd < 0) {
        LOGE("open(%s) failed: %s (%d)", rfkill_state_path, strerror(errno),
             errno);
        goto out;
    }
    sz = read(fd, &buffer, 1);
    if (sz != 1) {
        LOGE("read(%s) failed: %s (%d)", rfkill_state_path, strerror(errno),
             errno);
        goto out;
    }

    switch (buffer) {
    case '1':
        ret = 1;
        break;
    case '0':
        ret = 0;
        break;
    }

out:
    if (fd >= 0) close(fd);
    return ret;
}

static int set_bluetooth_power(int on) {
    int sz;
    int fd = -1;
    int ret = -1;
    const char buffer = (on ? '1' : '0');
    char buf;

    LOGI( "[## BT ##] rfkill_id [%d] buffer[%d]", rfkill_id,buffer);
    if (rfkill_id == -1) {
        if (init_rfkill()) goto out;
    }

    LOGI( "[## BT ##] rfkill_state_path[%s]", rfkill_state_path);

    fd = open(rfkill_state_path, O_WRONLY);
    if (fd < 0) {
        LOGE("open(%s) for write failed: %s (%d)", rfkill_state_path,
             strerror(errno), errno);
        goto out;
    }
    sz = write(fd, &buffer, 1);
    if (sz < 0) {
        LOGE("write(%s) failed: %s (%d)", rfkill_state_path, strerror(errno),
             errno);
        goto out;
    }
	
#if 1  //for csr	
    if(on) {
	if(tcc_bluetooth_vedor_tool(on) < 0) {
              LOGE("tcc_bluetooth_vedor_tool failed !" ),
    		buf = 0;
    		write(fd, &buf, 1);
		goto out;
    	}
    }
#endif

    ret = 0;

out:
    if (fd >= 0) close(fd);
    return ret;
}

static inline int create_hci_sock() {
    int sk = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (sk < 0) {
        LOGE("Failed to create bluetooth hci socket: %s (%d)",
             strerror(errno), errno);
    }
    return sk;
}

int bt_enable() {
    LOGV(__FUNCTION__);

    int ret = -1;
    int hci_sock = -1;
    int attempt = 1;	

    if (set_bluetooth_power(1) < 0) goto out;

    LOGI("Starting hciattach daemon");
    if (property_set("ctl.start", "hciattach") < 0) {
        LOGE("Failed to start hciattach");
        set_bluetooth_power(0);
        goto out;
    }

    // Try for 10 seconds, this can only succeed once hciattach has sent the
    // firmware and then turned on hci device via HCIUARTSETPROTO ioctl
    for (attempt = 1000; attempt > 0;  attempt--) {
        hci_sock = create_hci_sock();
        if (hci_sock < 0) goto out;

        ret = ioctl(hci_sock, HCIDEVUP, HCI_DEV_ID);

        LOGV("bt_enable: ret: %d, errno: %d", ret, errno);
        if (!ret) {
            break;
        } else if (errno == EALREADY) {
            LOGW("Bluetoothd already started, unexpectedly!");
            break;
        }

        close(hci_sock);
        usleep(100000);  // 100 ms retry delay
    }
    if (attempt == 0) {
        LOGE("%s: Timeout waiting for HCI device to come up, error- %d, ",
            __FUNCTION__, ret);
        if (property_set("ctl.stop", "hciattach") < 0) {
            LOGE("Error stopping hciattach");
        }
        set_bluetooth_power(0);
        goto out;
    }

    LOGI("Starting bluetoothd deamon");
    if (property_set("ctl.start", "bluetoothd") < 0) {
        LOGE("Failed to start bluetoothd");
        set_bluetooth_power(0);
        goto out;
    }

    ret = 0;

out:
    if (hci_sock >= 0) close(hci_sock);
    return ret;
}

int bt_disable() {
    LOGV(__FUNCTION__);

    int ret = -1;
    int hci_sock = -1;

    LOGI("Stopping bluetoothd deamon");
    if (property_set("ctl.stop", "bluetoothd") < 0) {
        LOGE("Error stopping bluetoothd");
        goto out;
    }
    usleep(HCID_STOP_DELAY_USEC);

    hci_sock = create_hci_sock();
    if (hci_sock < 0) goto out;
    ioctl(hci_sock, HCIDEVDOWN, HCI_DEV_ID);

    LOGI("Stopping hciattach deamon");
    if (property_set("ctl.stop", "hciattach") < 0) {
        LOGE("Error stopping hciattach");
        goto out;
    }

    if (set_bluetooth_power(0) < 0) {
        goto out;
    }
    ret = 0;

out:
    if (hci_sock >= 0) close(hci_sock);
    return ret;
}

int bt_is_enabled() {
    LOGV(__FUNCTION__);

    int hci_sock = -1;
    int ret = -1;
    struct hci_dev_info dev_info;


    // Check power first
    ret = check_bluetooth_power();
    if (ret == -1 || ret == 0) goto out;

    ret = -1;

    // Power is on, now check if the HCI interface is up
    hci_sock = create_hci_sock();
    if (hci_sock < 0) goto out;

    dev_info.dev_id = HCI_DEV_ID;
    if (ioctl(hci_sock, HCIGETDEVINFO, (void *)&dev_info) < 0) {
        ret = 0;
        goto out;
    }

    if (dev_info.flags & (1 << (HCI_UP & 31))) {
        ret = 1;
    } else {
        ret = 0;
    }

out:
    if (hci_sock >= 0) close(hci_sock);
    return ret;
}

int ba2str(const bdaddr_t *ba, char *str) {
    return sprintf(str, "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
                ba->b[5], ba->b[4], ba->b[3], ba->b[2], ba->b[1], ba->b[0]);
}

int str2ba(const char *str, bdaddr_t *ba) {
    int i;
    for (i = 5; i >= 0; i--) {
        ba->b[i] = (uint8_t) strtoul(str, &str, 16);
        str++;
    }
    return 0;
}
