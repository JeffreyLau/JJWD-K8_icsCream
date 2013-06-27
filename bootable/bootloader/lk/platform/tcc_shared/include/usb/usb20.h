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

#ifndef _USB20_H
#define _USB20_H

/* bmRequestType */

#define USB_DEVREQ_DIR_MASK						(1<<7)		//in bmRequestType
#define	USB_DEVREQ_DIR_HOST_TO_DEVICE			(0<<7)
#define USB_DEVREQ_DIR_DEVICE_TO_HOST			(1<<7)

#define USB_DEVREQ_TYPE_MASK					(3<<5)		//in bmRequestType
#define USB_DEVREQ_TYPE_STANDARD				(0<<5)
#define USB_DEVREQ_TYPE_CLASS					(1<<5)
#define USB_DEVREQ_TYPE_VENDOR					(2<<5)

#define USB_DEVREQ_RECIPIENT_MASK				0x1F		//in bmRequestType
#define USB_DEVREQ_RECIPIENT_DEVICE				0x00
#define USB_DEVREQ_RECIPIENT_INTERFACE			0x01
#define USB_DEVREQ_RECIPIENT_ENDPOINT			0x02

/* standard */

#define USB_REQUEST_GET_STATUS					0
#define USB_REQUEST_CLEAR_FEATURE				1
#define USB_REQUEST_SET_FEATURE					3
#define USB_REQUEST_SET_ADDRESS					5
#define USB_REQUEST_GET_DESCRIPTOR				6
#define USB_REQUEST_SET_DESCRIPTOR				7
#define USB_REQUEST_GET_CONFIGURATION			8
#define USB_REQUEST_SET_CONFIGURATION			9
#define USB_REQUEST_GET_INTERFACE				10
#define USB_REQUEST_SET_INTERFACE				11
#define USB_REQUEST_SYNC_FRAME					12


/****************/

#define USB_DESCRIPTOR_TYPE_DEVICE						1
#define USB_DESCRIPTOR_TYPE_CONFIGURATION				2
#define USB_DESCRIPTOR_TYPE_STRING						3
#define USB_DESCRIPTOR_TYPE_INTERFACE					4
#define USB_DESCRIPTOR_TYPE_ENDPOINT					5
#define USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER			6
#define USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION	7
#define USB_DESCRIPTOR_TYPE_INTERFACE_POWER				8

#define USB_FEATURE_SELECTOR_DEVICE_REMOTE_WAKEUP		1
#define USB_FEATURE_SELECTOR_ENDPOINT_HALT				0
#define USB_FEATURE_SELECTOR_TEST_MODE					2

#define USB_ENDPOINT_NUMBER_MASK						0x0F		//in bEndpointAddress

#define	USB_ENDPOINT_DIRECTION_MASK						0x80		//in bEndpointAddress
#define USB_ENDPOINT_DIRECTION_IN						0x80
#define	USB_ENDPOINT_DIRECTION_OUT						0x00

#define USB_ENDPOINT_TRANSFER_TYPE_MASK					0x03		//in bEndpointAddress
#define USB_ENDPOINT_TRANSFER_TYPE_CONTROL				0x00
#define USB_ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS			0x01
#define USB_ENDPOINT_TRANSFER_TYPE_BULK					0x02
#define USB_ENDPOINT_TRANSFER_TYPE_INTERRUPT			0x03

#define USB_ENDPOINT_SYNCHRONIZATION_TYPE_MASK			(0x3<<2)	//in bEndpointAddress

#define USB_ENDPOINT_USAGE_TYPE_MASK					(0x3<<4)	//in bEndpointAddress

#define USB_ENDPOINT_MAX_PACKET_SIZE_MASK				0x07FF		//in wMaxPacketSize

#define USB_ENDPOINT_NUM_ADDITIONAL_MICROFRAME_MASK		(0x3<<11)	//in wMaxPacketSize

////////////////////////////////////////////////////////
//
//    USB CLASS CODE
//
#define USB_CLASS_CODE_DEFINED_IN_CLASS				0x00
#define USB_CLASS_CODE_AUDIO						0x01
#define USB_CLASS_CODE_CDC							0x02
#define USB_CLASS_CODE_HID							0x03
#define USB_CLASS_CODE_PHYSICAL						0x04
#define USB_CLASS_CODE_IMAGE						0x06
#define USB_CLASS_CODE_PRINTER						0x07
#define USB_CLASS_CODE_MASSSTORAGE					0x08
#define USB_CLASS_CODE_HUB							0x09
#define USB_CLASS_CODE_CDC_DATA						0x0A
#define USB_CLASS_CODE_SMARTCARD					0x0B
#define USB_CLASS_CODE_CONTENTSECURITY				0x0D
#define USB_CLASS_CODE_VIDEO						0x0E
#define USB_CLASS_CODE_DIAGOSTIC					0xDC
#define USB_CLASS_CODE_WIRELESS_CONTROLLER			0xE0
#define USB_CLASS_CODE_MISCELLANEOUS				0xEF
#define USB_CLASS_CODE_APPLICATION_SPECIFIC			0xFE
#define USB_CLASS_CODE_VENDOR_SPECIFIC				0xFF


#if defined(_WINCE_)
#pragma pack(push, 1)
#endif
#if defined(_WINCE_) || defined(_LINUX_)
#define __packed
#endif

typedef __packed struct _tag_USB_DEVICE_DESCRIPTOR_T
{
  	unsigned char		bLength;				// Descriptor size
    unsigned char		bDescriptorType;		// Descriptor Type
    unsigned short		bcdUSB;
  	unsigned char		bDeviceClass;
    unsigned char 		bDeviceSubClass;
    unsigned char 		bDeviceProtocol;
    unsigned char 		bMaxPacketSize0;
    unsigned short 		idVendor;
    unsigned short 		idProduct;
    unsigned short 		bcdDevice;
    unsigned char 		iManufacturer;
    unsigned char 		iProduct;
    unsigned char 		iSerialNumber;
    unsigned char 		bNumConfigurations;
#if defined(_LINUX_)
} __attribute__ ((packed)) USB_DEVICE_DESCRIPTOR_T;
#else
} USB_DEVICE_DESCRIPTOR_T;
#endif

typedef __packed struct _tag_USB_DEVICE_QUALIFIER_DESCRIPTOR_T
{
	unsigned char		bLength;
	unsigned char		bDescriptorType;
	unsigned short		bcdUSB;
	unsigned char		bDeviceClass;
	unsigned char		bDeviceSubClass;
	unsigned char		bDeviceProtocol;
	unsigned char		bMaxPacketSize0;
	unsigned char		bNumConfigurations;
	unsigned char		bReserved;
#if defined(_LINUX_)
} __attribute__ ((packed)) USB_DEVICE_QUALIFIER_DESCRIPTOR_T;
#else
} USB_DEVICE_QUALIFIER_DESCRIPTOR_T;
#endif

typedef __packed struct _tag_USB_CONFIGURATION_DESCRIPTOR_T
{
    unsigned char 		bLength;
    unsigned char 		bDescriptorType;
    unsigned short 		wTotalLength;
    unsigned char 		bNumInterfaces;
    unsigned char 		bConfigurationValue;
    unsigned char 		iConfiguration;
    unsigned char 		bmAttributes;
    unsigned char 		MaxPower;
#if defined(_LINUX_)
} __attribute__ ((packed)) USB_CONFIGURATION_DESCRIPTOR_T;
#else
} USB_CONFIGURATION_DESCRIPTOR_T;
#endif

typedef __packed struct _tag_USB_INTERFACE_DESCRIPTOR_T
{
	unsigned char		bLength;
	unsigned char		bDescriptorType;
	unsigned char		bInterfaceNumber;
	unsigned char		bAlternateSetting;
	unsigned char		bNumEndpoints;
    unsigned char		bInterfaceClass;
    unsigned char		bInterfaceSubClass;
    unsigned char		bInterfaceProtocol;
    unsigned char		iInterface;
#if defined(_LINUX_)
} __attribute__ ((packed)) USB_INTERFACE_DESCRIPTOR_T;
#else
} USB_INTERFACE_DESCRIPTOR_T;
#endif

typedef __packed struct _tag_USB_ENDPOINT_DESCRIPTOR_T
{
	unsigned char		bLength;
	unsigned char		bDescriptorType;
	unsigned char		bEndpointAddress;
	unsigned char		bmAttributes;
	unsigned short		wMaxPacketSize;
	unsigned char		bInterval;
#if defined(_LINUX_)
} __attribute__ ((packed)) USB_ENDPOINT_DESCRIPTOR_T;
#else
} USB_ENDPOINT_DESCRIPTOR_T;
#endif

typedef __packed struct _tag_USB_DEVICE_REQUEST_T
{
	unsigned char		bmRequestType;
	unsigned char		bRequest;
	unsigned short		wValue;
	unsigned short		wIndex;
	unsigned short		wLength;
#if defined(_LINUX_)
} __attribute__ ((packed)) USB_DEVICE_REQUEST_T;
#else
} USB_DEVICE_REQUEST_T;
#endif

#if defined(_WINCE_)
#pragma pack(pop)
#endif

#endif //_USB20_H
