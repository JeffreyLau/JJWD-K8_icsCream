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

#if defined(PLATFORM_TCC92XX) || defined(PLATFORM_TCC93XX) || defined(PLATFORM_TCC88XX) || defined(PLATFORM_TCC892X)
#include "usb/usb_defs.h"
#include "usb/vtc.h"
#include "usb/usbdev_class.h"

/* For Signature */
#define VTC_SIGNATURE			'V','T','C','_'
#define VTC_VERSION				'V','2','.','0','0','1'
static const unsigned char VTC_C_Version[] = {SIGBYAHONG, VTC_SIGNATURE, SIGN_OS ,SIGN_CHIPSET, VTC_VERSION, NULL};

//*****************************************************************************
//*
//*
//*                          [ INTERNAL DEFINITION ]
//*
//*
//*****************************************************************************
#define VTC_BULK_OUT_EP			0x01
#define VTC_BULK_IN_EP			0x82
#define VTC_INT_IN_EP			0x83

typedef __packed struct _tag_VTC_CONFIG_DESCRIPTOR_T
{
	USB_CONFIGURATION_DESCRIPTOR_T	Configuration;
	USB_INTERFACE_DESCRIPTOR_T		Interface;
	USB_ENDPOINT_DESCRIPTOR_T		BulkInDscr;
	USB_ENDPOINT_DESCRIPTOR_T		BulkOutDscr;
	USB_ENDPOINT_DESCRIPTOR_T		IntInDscr;
#if defined(_LINUX_)
} __attribute__((packed)) VTC_CONFIG_DESCRIPTOR_T;
#else
} VTC_CONFIG_DESCRIPTOR_T;
#endif


const USB_DEVICE_DESCRIPTOR_T gVTC_DeviceDescriptor =
{
	sizeof(USB_DEVICE_DESCRIPTOR_T),				// bLength
	USB_DESCRIPTOR_TYPE_DEVICE,						// bDescriptorType
	0x0200,											// bcdUSB
	0,												// bDeviceClass
	0,												// bDeviceSubClass
	0,												// bDeviceProtocol
	64,												// bMaxPacketSize0
	USBDEV_CLASS_VTC_VID,							// idVendor
	USBDEV_CLASS_VTC_PID,							// idProduct
	0x0100,											// bcdDevice
	0,												// iManufacturer
	0,												// iProduct
	0,												// iSerialNumber
	1												// bNumConfigurations
};

const USB_DEVICE_QUALIFIER_DESCRIPTOR_T gVTC_DeviceQualifierDescriptor =
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

const VTC_CONFIG_DESCRIPTOR_T gVTC_ConfigDescriptor_USB11 =
{
	// configuration descriptor
	{	sizeof(USB_CONFIGURATION_DESCRIPTOR_T),				// bLength
		USB_DESCRIPTOR_TYPE_CONFIGURATION,					// bDescriptorType
		sizeof(VTC_CONFIG_DESCRIPTOR_T),					// wTotalLength
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
		3,													// bNumEndpoints
		0xFF,												// bInterfaceClass
		0xFF,												// bInterfaceSubClass
		0xFF,												// bInterfaceProtocol
		0													// iInterface
	},

	// Bulk In endpoint descriptor
	{	sizeof(USB_ENDPOINT_DESCRIPTOR_T),					// bLength
		USB_DESCRIPTOR_TYPE_ENDPOINT,						// bDescriptorType
		VTC_BULK_IN_EP,										// bEndpointAddress, dir | ep_index
		USB_ENDPOINT_TRANSFER_TYPE_BULK,					// bmAttributes
		64,													// wMaxPacketSize
		0													// bInterval
	},

	// Bulk Out endpoint descriptor
	{	sizeof(USB_ENDPOINT_DESCRIPTOR_T),					// bLength
		USB_DESCRIPTOR_TYPE_ENDPOINT,						// bDescriptorType
		VTC_BULK_OUT_EP,									// bEndpointAddress, dir | ep_index
		USB_ENDPOINT_TRANSFER_TYPE_BULK,					// bmAttributes
		64,													// wMaxPacketSize
		0													// bInterval
	},

	// Interrupt In endpoint descriptor
	{	sizeof(USB_ENDPOINT_DESCRIPTOR_T),					// bLength
		USB_DESCRIPTOR_TYPE_ENDPOINT,						// bDescriptorType
		VTC_INT_IN_EP,										// bEndpointAddress, dir | ep_index
		USB_ENDPOINT_TRANSFER_TYPE_INTERRUPT,				// bmAttributes
		64,													// wMaxPacketSize
		16/*16ms*/											// bInterval, x*1ms for int(1~255), 2^(x-1)*1ms for iso(1~16)
	}
};

const VTC_CONFIG_DESCRIPTOR_T gVTC_ConfigDescriptor_USB20 =
{
	// configuration descriptor
	{	sizeof(USB_CONFIGURATION_DESCRIPTOR_T),				// bLength
		USB_DESCRIPTOR_TYPE_CONFIGURATION,					// bDescriptorType
		sizeof(VTC_CONFIG_DESCRIPTOR_T),					// wTotalLength
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
		3,													// bNumEndpoints
		0xFF,												// bInterfaceClass
		0xFF,												// bInterfaceSubClass
		0xFF,												// bInterfaceProtocol
		0													// iInterface
	},

	// Bulk In endpoint descriptor
	{	sizeof(USB_ENDPOINT_DESCRIPTOR_T),					// bLength
		USB_DESCRIPTOR_TYPE_ENDPOINT,						// bDescriptorType
		VTC_BULK_IN_EP,										// bEndpointAddress, dir | ep_index
		USB_ENDPOINT_TRANSFER_TYPE_BULK,					// bmAttributes
		512,												// wMaxPacketSize
		0													// bInterval
	},

	// Bulk Out endpoint descriptor
	{	sizeof(USB_ENDPOINT_DESCRIPTOR_T),					// bLength
		USB_DESCRIPTOR_TYPE_ENDPOINT,						// bDescriptorType
		VTC_BULK_OUT_EP,									// bEndpointAddress, dir | ep_index
		USB_ENDPOINT_TRANSFER_TYPE_BULK,					// bmAttributes
		512,												// wMaxPacketSize
		0													// bInterval
	},

	// Interrupt In endpoint descriptor
	{	sizeof(USB_ENDPOINT_DESCRIPTOR_T),					// bLength
		USB_DESCRIPTOR_TYPE_ENDPOINT,						// bDescriptorType
		VTC_INT_IN_EP,										// bEndpointAddress, dir | ep_index
		USB_ENDPOINT_TRANSFER_TYPE_INTERRUPT,				// bmAttributes
		512,												// wMaxPacketSize
		8/*16ms*/											// bInterval, 2^(x-1)*125us for int/iso(1~16)
	}
};

const USBDEV_STRING_DESCRIPTORS_FOR_LANGUAGE_T gVTC_StringDesciptorsSets[] =
{
	{ 0x0409, NULL, 0 }
};

static struct {
	USBDEV_ENDPOINT_T epBulkOut;
	USBDEV_ENDPOINT_T epBulkIn;
	USBDEV_ENDPOINT_T epIntIn;
} gVTC;

void VTC_Open(void)
{
	int res = FALSE;
	{
		USBDEV_QUEUE_ALLOC_T stBulkOutQueueForDma = {
			VTC_BULK_OUT_EP,		// EP Number
			2,						// Queue Number
			64*1024,				// Buffer Size/Queue
			NULL					// Next Queue Allocation Information
		};
		USBDEV_QUEUE_ALLOC_T stBulkInQueueForDma = {
			VTC_BULK_IN_EP,			// EP Number
			2,						// Queue Number
			64*1024,				// Buffer Size/Queue
			NULL					// Next Queue Allocation Information
		};
		USBDEV_QUEUE_ALLOC_T stIntInQueueForDma = {
			VTC_INT_IN_EP,			// EP Number
			16,						// Queue Number
			1024,					// Buffer Size/Queue
			NULL					// Next Queue Allocation Information
		};
		stBulkOutQueueForDma.pNext = &stBulkInQueueForDma;
		stBulkInQueueForDma.pNext = &stIntInQueueForDma;
		res = USBDEV_AllocQueue(&stBulkOutQueueForDma);
	}

	if(res!=TRUE && USBDEV_GetLastError()==USBDEV_ERROR_QUEUE_ALLOC_BUFFER_SIZE)
	{
		// reduce queue buffer size

		USBDEV_QUEUE_ALLOC_T stBulkOutQueue = {
			VTC_BULK_OUT_EP,		// EP Number
			2,						// Queue Number
			17*1024,				// Buffer Size/Queue
			NULL					// Next Queue Allocation Information
		};
		USBDEV_QUEUE_ALLOC_T stBulkInQueue = {
			VTC_BULK_IN_EP,			// EP Number
			2,						// Queue Number
			17*1024,				// Buffer Size/Queue
			NULL					// Next Queue Allocation Information
		};
		USBDEV_QUEUE_ALLOC_T stIntInQueue = {
			VTC_INT_IN_EP,			// EP Number
			16,						// Queue Number
			1024,					// Buffer Size/Queue
			NULL					// Next Queue Allocation Information
		};
		stBulkOutQueue.pNext = &stBulkInQueue;
		stBulkInQueue.pNext = &stIntInQueue;
		res = USBDEV_AllocQueue(&stBulkOutQueue);
	}

	while(res!=TRUE);
}

void VTC_Reset(void)
{
}

void VTC_USB_Event(USBDEV_EVENT_T event)
{
	if( event == USBDEV_EVENT_RESET )
	{
		VTC_Reset();
	}
}

const VTC_CONFIG_DESCRIPTOR_T * VTC_USB_GetCurrentConfigDscr(void)
{
	return ( USBDEV_GetCurrentSpeed() == USBDEV_HIGH_SPEED ) ? &gVTC_ConfigDescriptor_USB20 : &gVTC_ConfigDescriptor_USB11;
}

void VTC_USB_Enable(void)
{
	const VTC_CONFIG_DESCRIPTOR_T *pConfigDescriptor = VTC_USB_GetCurrentConfigDscr();

	USBDEV_ExtractEP(&gVTC.epBulkOut, &pConfigDescriptor->BulkOutDscr);
	USBDEV_ExtractEP(&gVTC.epBulkIn, &pConfigDescriptor->BulkInDscr);
	USBDEV_ExtractEP(&gVTC.epIntIn, &pConfigDescriptor->IntInDscr);

	USBDEV_EP_Enable(&gVTC.epBulkOut, TRUE);
	USBDEV_EP_Enable(&gVTC.epBulkIn, TRUE);
	USBDEV_EP_Enable(&gVTC.epIntIn, TRUE);
}

unsigned int VTC_USB_Setup(USB_DEVICE_REQUEST_T *pDeviceRequest)
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
					const VTC_CONFIG_DESCRIPTOR_T *pConfigDescriptor = VTC_USB_GetCurrentConfigDscr();

					if( configurationValue == pConfigDescriptor->Configuration.bConfigurationValue )
					{
						VTC_USB_Enable();
						USBDEV_SetState(USBDEV_STATE_CONFIGURED);
					}
				}
				USBDEV_EP0_Write(NULL,0);
				retVal = TRUE;
				break;
			}
			case USB_REQUEST_SET_INTERFACE:
				VTC_USB_Enable();
				USBDEV_SetState(USBDEV_STATE_CONFIGURED);
				USBDEV_EP0_Write(NULL,0);
				retVal = TRUE;
				break;
		}
	}

	return retVal;
}

extern void FWDN_PROT_CheckCommand(void);
void VTC_Run(void)
{
	FWDN_PROT_CheckCommand();
}

USBDEV_CLASS_DRIVER_T gVTC_USBDEV_ClassDriver = {
	VTC_C_Version,
	USBDEV_HIGH_SPEED,
	{ (const void *)&gVTC_DeviceDescriptor, sizeof(gVTC_DeviceDescriptor) },
	{ (const void *)&gVTC_DeviceQualifierDescriptor, sizeof(gVTC_DeviceQualifierDescriptor) },
	{ (const void *)&gVTC_ConfigDescriptor_USB11, sizeof(gVTC_ConfigDescriptor_USB11) },
	{ (const void *)&gVTC_ConfigDescriptor_USB20, sizeof(gVTC_ConfigDescriptor_USB20) },
	{ gVTC_StringDesciptorsSets, dim(gVTC_StringDesciptorsSets) },
	VTC_Open,
	VTC_USB_Event,
	VTC_USB_Setup,
	VTC_Run
};

USBDEV_CLASS_DRIVER_T *VTC_GetDriver(void)
{
	return &gVTC_USBDEV_ClassDriver;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	Functions for Client Driver
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int VTC_CheckIsReceivedData(void)
{
	return USBDEV_EP_IsReady(&gVTC.epBulkOut);
}

unsigned int VTC_CheckIsTransmitReady(void)
{
	return USBDEV_EP_IsReady(&gVTC.epBulkIn);
}

void VTC_ReceiveData_SetSize(unsigned int nBufferSize)
{
	USBDEV_EP_Read_SetSize(&gVTC.epBulkOut, nBufferSize);
}

unsigned int VTC_ReceiveData_GetPointer(void **ppBuffer)
{
	return USBDEV_EP_Read_GetPointer(&gVTC.epBulkOut, ppBuffer);
}

unsigned int VTC_ReceiveData(void* lpBuffer, unsigned int nBufferSize)
{
	return USBDEV_EP_Read(&gVTC.epBulkOut, lpBuffer, nBufferSize);
}

unsigned int VTC_SendData(const void* lpBuffer, unsigned int nBufferSize)
{
	return USBDEV_EP_Write(&gVTC.epBulkIn, lpBuffer, nBufferSize);
}

unsigned int VTC_SendMessage(const void* lpBuffer, unsigned int nBufferSize)
{
	if(USBDEV_GetState() == USBDEV_STATE_CONFIGURED)
		return USBDEV_EP_Write(&gVTC.epIntIn, lpBuffer, nBufferSize);
	else
		return 0;
}
#endif
