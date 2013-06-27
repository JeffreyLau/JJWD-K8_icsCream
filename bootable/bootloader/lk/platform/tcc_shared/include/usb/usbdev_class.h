/*
 * Copyright (c) 2010 Telechips, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _USBDEV_CLASS_H
#define _USBDEV_CLASS_H
#include "usb_device.h"

#define USBDEV_CLASS_VID							0x140E

#define USBDEV_CLASS_VTC_VID						0x140E

#if defined(TCC88XX)
	#define USBDEV_CLASS_VTC_PID					0xB068
	#define USBDEV_CLASS_MSC_PID					0xB069
	#define USBDEV_CLASS_MTP_PID					0xB070
#elif defined(TCC89XX)
	#define USBDEV_CLASS_VTC_PID					0xB071
	#define USBDEV_CLASS_MSC_PID					0xB072
	#define USBDEV_CLASS_MTP_PID					0xB073
#elif defined(TCC91XX)
	#define USBDEV_CLASS_VTC_PID					0xB077
	#define USBDEV_CLASS_MSC_PID					0xB078
	#define USBDEV_CLASS_MTP_PID					0xB079
#elif defined(TCC92XX)
	#define USBDEV_CLASS_VTC_PID					0xB080
	#define USBDEV_CLASS_MSC_PID					0xB081
	#define USBDEV_CLASS_MTP_PID					0xB082
#elif defined(TCC93XX)
	#define USBDEV_CLASS_VTC_PID					0xB083
	#define USBDEV_CLASS_MSC_PID					0xB084
	#define USBDEV_CLASS_MTP_PID					0xB085
#elif defined(TCC892X)
	#define USBDEV_CLASS_VTC_PID					0xB086
	#define USBDEV_CLASS_MSC_PID					0xB087
#endif

#define USBDEV_CLASS_MAX_POWER						45					// MaxPower, 2mA unit, 45 = 90mA

#define STRING_ASCII_SERIAL_NUMBER_MAX				32

#define MAX_SIZE_SERIAL_NUMBER						32		/* 32Bytes */
#define USB_NON_UNIQUE_SERIAL_NUMBER				0
#define USB_UNIQUE_SERIAL_NUMBER					1
#define IMEI_LEN									8

#define USBDEV_CLASS_STRING_INDEX_MANUFACTURER		1
#define USBDEV_CLASS_STRING_INDEX_PRODUCT			2
#define USBDEV_CLASS_STRING_INDEX_SERIALNUMBER		3

#define USBDEV_CLASS_STRING_DESCRIPTORS_SETS_NUM	1
#define USBDEV_MTP_STRING_DESCRIPTORS_SETS_NUM		2
extern const USBDEV_STRING_DESCRIPTORS_FOR_LANGUAGE_T gUSBDEV_Class_StringDesciptorsSets[];
extern const USBDEV_STRING_DESCRIPTORS_FOR_LANGUAGE_T gUSBDEV_MTP_StringDesciptorsSets[];

extern void USBDEV_CLASS_UpdateAsciiSerialNumber(unsigned char *pAsciiSerialNumber, unsigned int length);
extern unsigned int USBDEV_CLASS_GetASCIISerialNumber(unsigned char *pSN, unsigned int size);

#endif //_USBDEV_CLASS_H
