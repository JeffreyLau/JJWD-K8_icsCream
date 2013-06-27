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

#include <dev/udc.h>

#include "usb/usb_defs.h"
#include "usb/fastbootusb.h"
#include "usb/usbdev_class.h"

/* For Signature */
#define FASTBOOT_SIGNATURE			'F','A','S','T','B','O','O','T','_'
#define FASTBOOT_VERSION			'V','2','.','0','0','0'
static const unsigned char FASTBOOT_C_Version[] = {SIGBYAHONG, FASTBOOT_SIGNATURE, SIGN_OS ,SIGN_CHIPSET, FASTBOOT_VERSION, NULL};

extern void fastboot_event_configured(void);

//*****************************************************************************
//*
//*
//*                          [ INTERNAL DEFINITION ]
//*
//*
//*****************************************************************************
#define FASTBOOT_BULK_OUT_EP			0x01
#define FASTBOOT_BULK_IN_EP				0x82

#define FASTBOOT_STRING_INDEX_MANUFACTURER		1
#define FASTBOOT_STRING_INDEX_PRODUCT			2
#define FASTBOOT_STRING_INDEX_INTERFACE			3

#define FASTBOOT_INTERFACE_CLASS_CODE			0xFF
#define FASTBOOT_INTERFACE_SUBCLASS_CODE		0x42
#define FASTBOOT_INTERFACE_PROTOCOL_CODE		0x03

typedef __packed struct _tag_FASTBOOT_CONFIG_DESCRIPTOR_T
{
	USB_CONFIGURATION_DESCRIPTOR_T	Configuration;
	USB_INTERFACE_DESCRIPTOR_T		Interface;
	USB_ENDPOINT_DESCRIPTOR_T		BulkInDscr;
	USB_ENDPOINT_DESCRIPTOR_T		BulkOutDscr;
} FASTBOOT_CONFIG_DESCRIPTOR_T;


const USB_DEVICE_DESCRIPTOR_T gFASTBOOT_DeviceDescriptor =
{
	sizeof(USB_DEVICE_DESCRIPTOR_T),				// bLength
	USB_DESCRIPTOR_TYPE_DEVICE,						// bDescriptorType
	0x0200,											// bcdUSB
	0,												// bDeviceClass
	0,												// bDeviceSubClass
	0,												// bDeviceProtocol
	64,												// bMaxPacketSize0
	0x18d1,											// idVendor
	0xD00D,											// idProduct
	0x0100,											// bcdDevice
	FASTBOOT_STRING_INDEX_MANUFACTURER,				// iManufacturer
	USBDEV_CLASS_STRING_INDEX_PRODUCT,				// iProduct
	0,												// iSerialNumber
	1												// bNumConfigurations
};

const USB_DEVICE_QUALIFIER_DESCRIPTOR_T gFASTBOOT_DeviceQualifierDescriptor =
{
	sizeof(USB_DEVICE_QUALIFIER_DESCRIPTOR_T),		// bLength
	USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,			// bDescriptorType
	0x0200,											// bcdUSB
	0,												// bDeviceClass
	0,												// bDeviceSubClass
	0,												// bDeviceProtocol
	64,												// bMaxPacketSize0
	1,												// bNumConfigurations
	0,												// bReserved
};

const FASTBOOT_CONFIG_DESCRIPTOR_T gFASTBOOT_ConfigDescriptor_USB11 =
{
	// configuration descriptor
	{	sizeof(USB_CONFIGURATION_DESCRIPTOR_T),				// bLength
		USB_DESCRIPTOR_TYPE_CONFIGURATION,					// bDescriptorType
		sizeof(FASTBOOT_CONFIG_DESCRIPTOR_T),				// wTotalLength
		1,													// bNumInterfaces
		1,													// bConfigurationValue
		0,													// iConfiguration, index of config. string
		0x80,												// bmAttributes, Bus-powered and No Remote-Wakeup
		USBDEV_CLASS_MAX_POWER								// MaxPower
	},

	// interface descriptor
	{	sizeof(USB_INTERFACE_DESCRIPTOR_T),					// bLength
		USB_DESCRIPTOR_TYPE_INTERFACE,						// bDescriptorType
		0,													// bInterfaceNumber
		0,													// bAlternateSetting
		2,													// bNumEndpoints
		FASTBOOT_INTERFACE_CLASS_CODE,						// bInterfaceClass
		FASTBOOT_INTERFACE_SUBCLASS_CODE,					// bInterfaceSubClass
		FASTBOOT_INTERFACE_PROTOCOL_CODE,					// bInterfaceProtocol
		FASTBOOT_STRING_INDEX_INTERFACE						// iInterface
	},

	// Bulk In endpoint descriptor
	{	sizeof(USB_ENDPOINT_DESCRIPTOR_T),					// bLength
		USB_DESCRIPTOR_TYPE_ENDPOINT,						// bDescriptorType
		FASTBOOT_BULK_IN_EP,								// bEndpointAddress, dir | ep_index
		USB_ENDPOINT_TRANSFER_TYPE_BULK,					// bmAttributes
		64,													// wMaxPacketSize
		0													// bInterval
	},

	// Bulk Out endpoint descriptor
	{	sizeof(USB_ENDPOINT_DESCRIPTOR_T),					// bLength
		USB_DESCRIPTOR_TYPE_ENDPOINT,						// bDescriptorType
		FASTBOOT_BULK_OUT_EP,								// bEndpointAddress, dir | ep_index
		USB_ENDPOINT_TRANSFER_TYPE_BULK,					// bmAttributes
		64,													// wMaxPacketSize
		0													// bInterval
	}
};

const FASTBOOT_CONFIG_DESCRIPTOR_T gFASTBOOT_ConfigDescriptor_USB20 =
{
	// configuration descriptor
	{	sizeof(USB_CONFIGURATION_DESCRIPTOR_T),				// bLength
		USB_DESCRIPTOR_TYPE_CONFIGURATION,					// bDescriptorType
		sizeof(FASTBOOT_CONFIG_DESCRIPTOR_T),				// wTotalLength
		1,													// bNumInterfaces
		1,													// bConfigurationValue
		0,													// iConfiguration, index of config. string
		0x80,												// bmAttributes, Bus-powered and No Remote-Wakeup
		USBDEV_CLASS_MAX_POWER								// MaxPower, 2mA unit
	},

	// interface descriptor
	{	sizeof(USB_INTERFACE_DESCRIPTOR_T),					// bLength
		USB_DESCRIPTOR_TYPE_INTERFACE,						// bDescriptorType
		0,													// bInterfaceNumber
		0,													// bAlternateSetting
		2,													// bNumEndpoints
		FASTBOOT_INTERFACE_CLASS_CODE,						// bInterfaceClass
		FASTBOOT_INTERFACE_SUBCLASS_CODE,					// bInterfaceSubClass
		FASTBOOT_INTERFACE_PROTOCOL_CODE,					// bInterfaceProtocol
		FASTBOOT_STRING_INDEX_INTERFACE						// iInterface
	},

	// Bulk In endpoint descriptor
	{	sizeof(USB_ENDPOINT_DESCRIPTOR_T),					// bLength
		USB_DESCRIPTOR_TYPE_ENDPOINT,						// bDescriptorType
		FASTBOOT_BULK_IN_EP,								// bEndpointAddress, dir | ep_index
		USB_ENDPOINT_TRANSFER_TYPE_BULK,					// bmAttributes
		512,												// wMaxPacketSize
		0													// bInterval
	},

	// Bulk Out endpoint descriptor
	{	sizeof(USB_ENDPOINT_DESCRIPTOR_T),					// bLength
		USB_DESCRIPTOR_TYPE_ENDPOINT,						// bDescriptorType
		FASTBOOT_BULK_OUT_EP,								// bEndpointAddress, dir | ep_index
		USB_ENDPOINT_TRANSFER_TYPE_BULK,					// bmAttributes
		512,												// wMaxPacketSize
		0													// bInterval
	}
};

static struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bString[12];
} gFASTBOOT_StrManufacturer =
{
	sizeof(gFASTBOOT_StrManufacturer),
	USB_DESCRIPTOR_TYPE_STRING,
	{
		'G', 0,
		'o', 0,
		'o', 0,
		'g', 0,
		'l', 0,
		'e', 0,
	}
};

static struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bString[14];
} gFASTBOOT_StrProduct =
{
	sizeof(gFASTBOOT_StrProduct),
	USB_DESCRIPTOR_TYPE_STRING,
	{
		'A', 0,
		'n', 0,
		'd', 0,
		'r', 0,
		'o', 0,
		'i', 0,
		'd', 0,
	}
};

static struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bString[16];
} gFASTBOOT_StrInterface =
{
	sizeof(gFASTBOOT_StrInterface),
	USB_DESCRIPTOR_TYPE_STRING,
	{
		'f', 0,
		'a', 0,
		's', 0,
		't', 0,
		'b', 0,
		'o', 0,
		'o', 0,
		't', 0,
	}
};

static const USBDEV_DESCRIPTOR_FOR_STRING_T gFASTBOOT_DscrForString_0x0409[] =
{
	{
		FASTBOOT_STRING_INDEX_MANUFACTURER,
		(void*)&gFASTBOOT_StrManufacturer,
		sizeof(gFASTBOOT_StrManufacturer)
	},
	{
		FASTBOOT_STRING_INDEX_PRODUCT,
		(void*)&gFASTBOOT_StrProduct,
		sizeof(gFASTBOOT_StrProduct)
	},
	{
		FASTBOOT_STRING_INDEX_INTERFACE,
		(void*)&gFASTBOOT_StrInterface,
		sizeof(gFASTBOOT_StrInterface)
	},
};

const USBDEV_STRING_DESCRIPTORS_FOR_LANGUAGE_T gFASTBOOT_StringDesciptorsSets[] =
{
	{ 0x0409, gFASTBOOT_DscrForString_0x0409, dim(gFASTBOOT_DscrForString_0x0409) },
};

static struct {
	USBDEV_ENDPOINT_T epBulkIn;
	USBDEV_ENDPOINT_T epBulkOut;
} gFASTBOOT;

void FASTBOOT_Open(void)
{
	int res = FALSE;

	{
		// reduce queue buffer size

		USBDEV_QUEUE_ALLOC_T stBulkOutQueue = {
			FASTBOOT_BULK_OUT_EP,		// EP Number
			2,						// Queue Number
			16*1024,				// Buffer Size/Queue
			NULL					// Next Queue Allocation Information
		};
		USBDEV_QUEUE_ALLOC_T stBulkInQueue = {
			FASTBOOT_BULK_IN_EP,			// EP Number
			2,						// Queue Number
			16*1024,				// Buffer Size/Queue
			NULL					// Next Queue Allocation Information
		};
		stBulkOutQueue.pNext = &stBulkInQueue;
		res = USBDEV_AllocQueue(&stBulkOutQueue);
	}

	while(res!=TRUE);
}

void FASTBOOT_Reset(void)
{
}

void FASTBOOT_USB_Event(USBDEV_EVENT_T event)
{
	if( event == USBDEV_EVENT_RESET )
	{
		FASTBOOT_Reset();
	}
}

const FASTBOOT_CONFIG_DESCRIPTOR_T * FASTBOOT_USB_GetCurrentConfigDscr(void)
{
	return ( USBDEV_GetCurrentSpeed() == USBDEV_HIGH_SPEED ) ? &gFASTBOOT_ConfigDescriptor_USB20 : &gFASTBOOT_ConfigDescriptor_USB11;
}

void FASTBOOT_USB_Enable(void)
{
	const FASTBOOT_CONFIG_DESCRIPTOR_T *pConfigDescriptor = FASTBOOT_USB_GetCurrentConfigDscr();

	USBDEV_ExtractEP(&gFASTBOOT.epBulkIn, &pConfigDescriptor->BulkInDscr);
	USBDEV_ExtractEP(&gFASTBOOT.epBulkOut, &pConfigDescriptor->BulkOutDscr);

	USBDEV_EP_Enable(&gFASTBOOT.epBulkIn, TRUE);
	USBDEV_EP_Enable(&gFASTBOOT.epBulkOut, TRUE);
}

unsigned int FASTBOOT_USB_Setup(USB_DEVICE_REQUEST_T *pDeviceRequest)
{
	unsigned int retVal = FALSE;

	if( (pDeviceRequest->bmRequestType&USB_DEVREQ_TYPE_MASK) == USB_DEVREQ_TYPE_STANDARD )
	{
		switch(pDeviceRequest->bRequest)
		{
			case USB_REQUEST_SET_CONFIGURATION:
			{
				unsigned char configurationValue = (unsigned char)pDeviceRequest->wValue;
				if( configurationValue == 0 )
				{
					USBDEV_SetState(USBDEV_STATE_ADDRESS);
				}
				else
				{
					const FASTBOOT_CONFIG_DESCRIPTOR_T *pConfigDescriptor = FASTBOOT_USB_GetCurrentConfigDscr();

					if( configurationValue == pConfigDescriptor->Configuration.bConfigurationValue )
					{
						FASTBOOT_USB_Enable();
						USBDEV_SetState(USBDEV_STATE_CONFIGURED);
					}
				}
				USBDEV_EP0_Write(NULL,0);
				udc_notify(UDC_EVENT_ONLINE);
				//fastboot_event_configured();
				retVal = TRUE;
				break;
			}
			case USB_REQUEST_SET_INTERFACE:
				FASTBOOT_USB_Enable();
				USBDEV_SetState(USBDEV_STATE_CONFIGURED);
				USBDEV_EP0_Write(NULL,0);
				retVal = TRUE;
				break;
		}
	}

	return retVal;
}

void FASTBOOT_Run(void)
{
}

USBDEV_CLASS_DRIVER_T gFASTBOOT_USBDEV_ClassDriver = {
	FASTBOOT_C_Version,
	USBDEV_HIGH_SPEED,
	{ (const void *)&gFASTBOOT_DeviceDescriptor, sizeof(gFASTBOOT_DeviceDescriptor) },
	{ (const void *)&gFASTBOOT_DeviceQualifierDescriptor, sizeof(gFASTBOOT_DeviceQualifierDescriptor) },
	{ (const void *)&gFASTBOOT_ConfigDescriptor_USB11, sizeof(gFASTBOOT_ConfigDescriptor_USB11) },
	{ (const void *)&gFASTBOOT_ConfigDescriptor_USB20, sizeof(gFASTBOOT_ConfigDescriptor_USB20) },
	{ gFASTBOOT_StringDesciptorsSets, dim(gFASTBOOT_StringDesciptorsSets) },
	FASTBOOT_Open,
	FASTBOOT_USB_Event,
	FASTBOOT_USB_Setup,
	FASTBOOT_Run
};

USBDEV_CLASS_DRIVER_T *FASTBOOT_GetDriver(void)
{
	return &gFASTBOOT_USBDEV_ClassDriver;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	Functions for Client Driver
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int FASTBOOT_CheckIsReceivedData(void)
{
	return USBDEV_EP_IsReady(&gFASTBOOT.epBulkOut);
}

unsigned int FASTBOOT_CheckIsTransmitReady(void)
{
	return USBDEV_EP_IsReady(&gFASTBOOT.epBulkIn);
}

void FASTBOOT_ReceiveData_SetSize(unsigned int nBufferSize)
{
	USBDEV_EP_Read_SetSize(&gFASTBOOT.epBulkOut, nBufferSize);
}

unsigned int FASTBOOT_ReceiveData_GetPointer(void **ppBuffer)
{
	return USBDEV_EP_Read_GetPointer(&gFASTBOOT.epBulkOut, ppBuffer);
}

unsigned int FASTBOOT_ReceiveData(void* lpBuffer, unsigned int nBufferSize)
{
	return USBDEV_EP_Read(&gFASTBOOT.epBulkOut, lpBuffer, nBufferSize);
}

unsigned int FASTBOOT_SendData(const void* lpBuffer, unsigned int nBufferSize)
{
	return USBDEV_EP_Write(&gFASTBOOT.epBulkIn, lpBuffer, nBufferSize);
}
