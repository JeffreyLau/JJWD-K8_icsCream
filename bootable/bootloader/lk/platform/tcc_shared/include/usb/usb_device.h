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

#ifndef _USB_DEVICE_H
#define _USB_DEVICE_H

#include "usb20.h"

typedef enum {
	USBDEV_STATE_DEFAULT,
	USBDEV_STATE_ADDRESS,
	USBDEV_STATE_CONFIGURED
} USBDEV_STATE_T;

typedef enum {
	USBDEV_FULL_SPEED,
	USBDEV_HIGH_SPEED
} USBDEV_SPEED_T;

typedef enum {
	USBDEV_CLASS_UNKNOWN,
	USBDEV_CLASS_VTC,
	USBDEV_CLASS_MSC,
	USBDEV_CLASS_MTP,
	USBDEV_CLASS_FASTBOOT
} USBDEV_CLASS_T;

typedef enum {
	USBDEV_EVENT_RESET
} USBDEV_EVENT_T;

typedef enum {
	USBDEV_EP_TRANSFER_TYPE_CONTROL = USB_ENDPOINT_TRANSFER_TYPE_CONTROL,
	USBDEV_EP_TRANSFER_TYPE_ISOCHRONOUS = USB_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS,
	USBDEV_EP_TRANSFER_TYPE_BULK = USB_ENDPOINT_TRANSFER_TYPE_BULK,
	USBDEV_EP_TRANSFER_TYPE_INTERRUPT = USB_ENDPOINT_TRANSFER_TYPE_INTERRUPT
} USBDEV_EP_TRANSFER_TYPE_T;

typedef enum {
	USBDEV_EP_SYNC_TYPE_NO_SYNC = 0,
	USBDEV_EP_SYNC_TYPE_ASYNC = 1,
	USBDEV_EP_SYNC_TYPE_ADAPTIVE = 2,
	USBDEV_EP_SYNC_TYPE_SYNC = 3
} USBDEV_EP_SYNC_TYPE_T;

typedef enum {
	USBDEV_EP_USAGE_TYPE_DATA = 0,
	USBDEV_EP_USAGE_TYPE_FEEDBACK = 1,
	USBDEV_EP_USAGE_TYPE_IMPLICIT_FEEDBACK_DATA = 2
} USBDEV_EP_USAGE_TYPE_T;

typedef enum {
	USBDEV_ERROR_FAILURE = -1,
	USBDEV_ERROR_QUEUE_ALLOC_BUFFER_SIZE = -2,
	USBDEV_ERROR_QUEUE_ALLOC_QUEUE_NUM = -3,
} USBDEV_ERROR_T;

typedef struct _tag_USBDEV_ENDPOINT_T
{
	unsigned char index;
	unsigned char isIn;
	USBDEV_EP_TRANSFER_TYPE_T transferType;
	USBDEV_EP_SYNC_TYPE_T syncType;
	USBDEV_EP_USAGE_TYPE_T usageType;
	unsigned short maximumPacketSize;
	unsigned char nAdditionalTransaction;
	unsigned char bInterval;
	unsigned int uiPreOutSize;
} USBDEV_ENDPOINT_T;

typedef struct _tag_USBDEV_QUEUE_T
{
	unsigned int xferLen;
	unsigned int actualLen;
	unsigned int buffSize;
	unsigned char *buff;
	volatile unsigned char state;
} USBDEV_QUEUE_T, *PUSBDEV_QUEUE_T;

typedef struct _tag_USBDEV_QUEUE_ALLOC_T
{
	unsigned char bEpAddress;
	unsigned char bQueueNum;
	unsigned int uiQueueBuffSize;
	struct _tag_USBDEV_QUEUE_ALLOC_T *pNext;
} USBDEV_QUEUE_ALLOC_T, *PUSBDEV_QUEUE_ALLOC_T;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	USB CLASS DRIVER Definition
////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _tag_USBDEVICE_IO_DRIVER_T
{
	const unsigned char *sign;
	USBDEV_SPEED_T currentSpeed;
	int (*AllocQueue)(PUSBDEV_QUEUE_ALLOC_T);
	unsigned int (*IsConnected)(void);
	void (*Init)(void);
	void (*Reset)(void);
	void (*EP_Stall)(unsigned char);
	void (*EP_Unstall)(unsigned char);
	void (*EP_UnstallEnable)(unsigned char, unsigned int);
	unsigned int (*EP_IsStalled)(unsigned char);
	void (*EP_Flush)(USBDEV_ENDPOINT_T *);
	unsigned int (*EP0_Read)(void *, unsigned int);
	unsigned int (*EP0_Write)(void *, unsigned int);
	void (*SetAddress)(unsigned short);
	void (*TestMode)(unsigned char);
	void (*Isr)(void);
	unsigned int (*EP_Enable)(USBDEV_ENDPOINT_T *, unsigned int);
	unsigned int (*EP_IsReady)(USBDEV_ENDPOINT_T *);
	PUSBDEV_QUEUE_T (*EP_GetQueue)(USBDEV_ENDPOINT_T *);
	unsigned int (*EP_SetQueue)(USBDEV_ENDPOINT_T *, unsigned int);
	USBDEV_ERROR_T (*GetLastError)(void);
} USBDEVICE_IO_DRIVER_T;


typedef struct _tag_USBDEV_DESCRIPTOR_T
{
	const void *pDescriptor;
	unsigned int size;
} USBDEV_DESCRIPTOR_T;

typedef struct _tag_USBDEV_DESCRIPTOR_FOR_STRING_T
{
	unsigned int index;
	void *pDescriptor;
	unsigned int size;
} USBDEV_DESCRIPTOR_FOR_STRING_T;

typedef struct _tag_USBDEV_STRING_DESCRIPTORS_FOR_LANGUAGE_T
{
	unsigned short languageID;
	const USBDEV_DESCRIPTOR_FOR_STRING_T *pDescriptors;
	unsigned int number;
} USBDEV_STRING_DESCRIPTORS_FOR_LANGUAGE_T;

typedef struct _tag_USBDEV_STRING_DESCRIPTORS_SETS_T
{
	const USBDEV_STRING_DESCRIPTORS_FOR_LANGUAGE_T *pLanguageSets;
	unsigned int number;
} USBDEV_STRING_DESCRIPTORS_SETS_T;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//	USB CLASS DRIVER Definition
////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _tag_USBDEV_CLASS_DRIVER_T
{
	const unsigned char *sign;
	USBDEV_SPEED_T speedLimit;
	USBDEV_DESCRIPTOR_T deviceDescriptor;
	USBDEV_DESCRIPTOR_T deviceQualifierDescriptor;
	USBDEV_DESCRIPTOR_T configDescriptor_USB11;
	USBDEV_DESCRIPTOR_T configDescriptor_USB20;
	USBDEV_STRING_DESCRIPTORS_SETS_T stringDescriptorsSets;
	void (*Open)(void);
	void (*Event)(USBDEV_EVENT_T);
	unsigned int (*EP0_Setup)(USB_DEVICE_REQUEST_T *);
	void (*Run)(void);
} USBDEV_CLASS_DRIVER_T;

void				USBDEV_UpdateAsciiSerialNumber(unsigned char *pAsciiSerialNumber, unsigned int length);
unsigned int		USBDEV_IsConnected(void);
void				USBDEV_SetState(USBDEV_STATE_T state);
USBDEV_STATE_T		USBDEV_GetState(void);
void				USBDEV_SetClass(USBDEV_CLASS_T classType);
USBDEV_CLASS_T		USBDEV_GetCurrentClass(void);
void				USBDEV_Open(USBDEVICE_IO_DRIVER_T *pIoDriver);
void				USBDEV_Init(void);
void				USBDEV_Reset(void);
void				USBDEV_Isr(void);
USBDEV_SPEED_T		USBDEV_GetSpeedLimit(void);
void				USBDEV_EP0_Setup(USB_DEVICE_REQUEST_T *pDeviceRequest);
void				USBDEV_Run(void);



/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	Functions for Class Driver
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
int USBDEV_AllocQueue(PUSBDEV_QUEUE_ALLOC_T pQueueAlloc);
USBDEV_SPEED_T USBDEV_GetCurrentSpeed(void);
void USBDEV_ExtractEP(USBDEV_ENDPOINT_T *pEP, const USB_ENDPOINT_DESCRIPTOR_T *pDescriptor);
unsigned int USBDEV_EP_Enable(USBDEV_ENDPOINT_T *pEP, unsigned int enale);
unsigned int USBDEV_EP_IsReady(USBDEV_ENDPOINT_T *pEP);
unsigned int USBDEV_EP0_Read(void *pRxBuffer, unsigned int nBufferSize);
void USBDEV_EP_Read_SetSize(USBDEV_ENDPOINT_T *pEP, unsigned int nBufferSize);
unsigned int USBDEV_EP_Read_GetPointer(USBDEV_ENDPOINT_T *pEP, void **pRxBuffer);
unsigned int USBDEV_EP_Read(USBDEV_ENDPOINT_T *pEP, void *pRxBuffer, unsigned int nBufferSize);
unsigned int USBDEV_EP0_Write(void *pTxBuffer, unsigned int nBufferSize);
unsigned int USBDEV_EP_Write(USBDEV_ENDPOINT_T *pEP, const void *pTxBuffer, unsigned int nBufferSize);
void USBDEV_EP_Stall(USBDEV_ENDPOINT_T *pEP);
void USBDEV_EP_Unstall(USBDEV_ENDPOINT_T *pEP);
void USBDEV_EP_UnstallEnable(USBDEV_ENDPOINT_T *pEP, unsigned int enable);
unsigned int USBDEV_EP_IsStalled(USBDEV_ENDPOINT_T *pEP);
void USBDEV_EP_Flush(USBDEV_ENDPOINT_T *pEP);
USBDEV_ERROR_T USBDEV_GetLastError(void);

#endif //_OTG_DEVICE_H
