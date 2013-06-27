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

#include "usb/usbdev_class.h"
#include "usb/usb_defs.h"

/* For Signature */
#define USBDEV_CLASS_SIGNATURE		'U','S','B','D','E','V','_','C','L','A','S','S','_'
#define USBDEV_CLASS_VERSION		'V','2','.','0','0','0'
const unsigned char USBDEV_CLASS_C_Version[] = {SIGBYAHONG, USBDEV_CLASS_SIGNATURE, SIGN_OS ,SIGN_CHIPSET, USBDEV_CLASS_VERSION, NULL};

unsigned char* USBDEV_CLASS_TellLibraryVersion(void)
{
	return (unsigned char*)USBDEV_CLASS_C_Version;
}


//*************************************************************************************************************************
//
//		USB Common Class String Descriptors
//
//
//*************************************************************************************************************************
static struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bString[18];
} gUSBDEV_Class_StrManufacturer =
{
	sizeof(gUSBDEV_Class_StrManufacturer),
	USB_DESCRIPTOR_TYPE_STRING,
	{
		'T', 0,
		'E', 0,
		'L', 0,
		'E', 0,
		'C', 0,
		'H', 0,
		'I', 0,
		'P', 0,
		'S', 0,
	}
};

static struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bString[32];
} gUSBDEV_Class_StrProduct =
{
	sizeof(gUSBDEV_Class_StrProduct),
	USB_DESCRIPTOR_TYPE_STRING,
	{
		'T', 0,
		'e', 0,
		'l', 0,
		'e', 0,
		'c', 0,
		'h', 0,
		'i', 0,
		'p', 0,
		's', 0,
		' ', 0,
		'P', 0,
		'l', 0,
		'a', 0,
		'y', 0,
		'e', 0,
		'r', 0,
	}
};

static struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bString[STRING_ASCII_SERIAL_NUMBER_MAX*2];
} gUSBDEV_Class_StrSerialNumber =
{
	sizeof(gUSBDEV_Class_StrSerialNumber),
	USB_DESCRIPTOR_TYPE_STRING,
	{
		'0', 0,
		'0', 0,
		'0', 0,
		'0', 0,

		'0', 0,
		'0', 0,
		'0', 0,
		'0', 0,

		'0', 0,
		'0', 0,
		'0', 0,
		'0', 0,

		'0', 0,
		'0', 0,
		'0', 0,
		'0', 0,

		'0', 0,
		'0', 0,
		'0', 0,
		'0', 0,

		'0', 0,
		'0', 0,
		'0', 0,
		'0', 0,

		'0', 0,
		'0', 0,
		'0', 0,
		'0', 0,

		'0', 0,
		'0', 0,
		'0', 0,
		'0', 0,
	}
};

static const USBDEV_DESCRIPTOR_FOR_STRING_T gUSBDEV_Class_DscrForString_0x0409[] =
{
	{
		USBDEV_CLASS_STRING_INDEX_MANUFACTURER,
		(void*)&gUSBDEV_Class_StrManufacturer,
		sizeof(gUSBDEV_Class_StrManufacturer)
	},
	{
		USBDEV_CLASS_STRING_INDEX_PRODUCT,
		(void*)&gUSBDEV_Class_StrProduct,
		sizeof(gUSBDEV_Class_StrProduct)
	},
	{
		USBDEV_CLASS_STRING_INDEX_SERIALNUMBER,
		(void*)&gUSBDEV_Class_StrSerialNumber,
		sizeof(gUSBDEV_Class_StrSerialNumber)
	},
};

const USBDEV_STRING_DESCRIPTORS_FOR_LANGUAGE_T gUSBDEV_Class_StringDesciptorsSets[USBDEV_CLASS_STRING_DESCRIPTORS_SETS_NUM] =
{
	{ 0x0409, gUSBDEV_Class_DscrForString_0x0409, dim(gUSBDEV_Class_DscrForString_0x0409) }
};

static struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bSignature[7*2];
	unsigned char bVendorCode;
	unsigned char bPad;
} gUSBDEV_MTP_StrMSOS =
{
	sizeof(gUSBDEV_MTP_StrMSOS),
	USB_DESCRIPTOR_TYPE_STRING,
	{
		'M', 0,
		'S', 0,
		'F', 0,
		'T', 0,
		'1', 0,
		'0', 0,
		'0', 0
	},
	0x01,
	0
};


//*************************************************************************************************************************
//
//		MTP Class Specific String Descriptors
//
//
//*************************************************************************************************************************
static const USBDEV_DESCRIPTOR_FOR_STRING_T gUSBDEV_MTP_DscrForString_0x0000[] =
{
	{
		0xEE/*MSOS String Index*/,
		(void*)&gUSBDEV_MTP_StrMSOS,
		sizeof(gUSBDEV_MTP_StrMSOS)
	}
};

const USBDEV_STRING_DESCRIPTORS_FOR_LANGUAGE_T gUSBDEV_MTP_StringDesciptorsSets[USBDEV_MTP_STRING_DESCRIPTORS_SETS_NUM] =
{
	{ 0x0000, gUSBDEV_MTP_DscrForString_0x0000, dim(gUSBDEV_MTP_DscrForString_0x0000) },
	{ 0x0409, gUSBDEV_Class_DscrForString_0x0409, dim(gUSBDEV_Class_DscrForString_0x0409) }
};


//*************************************************************************************************************************
//
//		Serial Number Manager
//
//
//*************************************************************************************************************************
void USBDEV_CLASS_UpdateAsciiSerialNumber(unsigned char *pAsciiSerialNumber, unsigned int length)
{
	if( length <= STRING_ASCII_SERIAL_NUMBER_MAX )
	{
		unsigned int i=0;
		gUSBDEV_Class_StrSerialNumber.bLength = (unsigned char)( 2 + (length<<1) );
		while( length-- > 0 )
		{
			gUSBDEV_Class_StrSerialNumber.bString[i++] = *pAsciiSerialNumber++;
			gUSBDEV_Class_StrSerialNumber.bString[i++] = 0;
		}
	}
}

unsigned int USBDEV_CLASS_GetASCIISerialNumber(unsigned char *pSN, unsigned int size)
{
	unsigned int writtenSize = 0;
	while( size-- )
	{
		if( STRING_ASCII_SERIAL_NUMBER_MAX > writtenSize )
		{
			*pSN++ = gUSBDEV_Class_StrSerialNumber.bString[writtenSize<<1];
			writtenSize++;
		}
		else
			*pSN++ = ' ';
	}
	return writtenSize;
}
