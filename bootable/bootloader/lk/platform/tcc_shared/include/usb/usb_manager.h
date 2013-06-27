/**************************************************************************************
 *
 * FILE NAME   : usb_manager.h
 * DESCRIPTION : This is usb manager source code
 *
 * ====================================================================================
 *
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 * ====================================================================================
 *
 * FILE HISTORY:
 * 	Date: 2009.02.16	Start source coding
 *
 **************************************************************************************/
#ifndef _USB_MANAGER_H
#define _USB_MANAGER_H

typedef enum {
	USB_MODE_HOST,
	USB_MODE_DEVICE,
} USB_MODE_T;

/* It must matched with USBDEV_CLASS_T in usb_device.h */
typedef enum {
	USBDEV_DEFAULT,
	USBDEV_VTC,
	USBDEV_MSC,
	USBDEV_MTP,
	USBDEV_FASTBOOT,
} USBDEV_T;

#define FEATURE_USB_ISR

typedef enum
{
	MODE_VTC = 0,				// Vendor Telechips	Class
	MODE_MSC,					// Mass Storage	Class
	MODE_MTP,					// Media Transfer Protocol
	MODE_MTP_PLAYBACK,			// PlayBack of MTP
	MODE_PCDMB,
	MODE_FASTBOOT,
	MODE_MAX_NUM
} USB_DEVICE_MODE_T;

void USB_Init(USBDEV_T mode);
//void USB_Isr(void);
unsigned int USB_DEVICE_IsConnected(void);
void USB_DEVICE_Enable(void);
void USB_DEVICE_Disable(void);
void USB_DEVICE_On(void);
void USB_DEVICE_Off(void);
void USB_DEVICE_Run(void);
unsigned int USB_DEVICE_IsPreOccupied(void);
void USB_DEVICE_SetMode(USB_DEVICE_MODE_T mode);
USB_DEVICE_MODE_T USB_DEVICE_GetMode(void);

#endif //_USB_MANAGER_H
