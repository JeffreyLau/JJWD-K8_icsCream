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

#include "usb/usb_defs.h"
#include "usb/usb_device.h"
#include "usb/usbdev_class.h"

/* For Signature */
#define USB_DEVICE_SIGNATURE	'U','S','B','_','D','E','V','I','C','E','_'
#define USB_DEVICE_VERSION		'V','2','.','0','0','1'
static const unsigned char USB_DEVICE_C_Version[] = {SIGBYAHONG, USB_DEVICE_SIGNATURE, SIGN_OS ,SIGN_CHIPSET, USB_DEVICE_VERSION, NULL};


static USBDEV_STATE_T g_USBDEV_State;
static USBDEV_CLASS_T g_USBDEV_CurrentClass = USBDEV_CLASS_UNKNOWN;
static unsigned char g_USBDEV_ConfigurationValue;

static USBDEV_CLASS_DRIVER_T *g_pUSBDEV_ClassDriver = NULL;
static USBDEVICE_IO_DRIVER_T *g_pUSBDEV_IO_Driver = NULL;

#if defined(_LINUX_)
extern void	memcpy(void *pvDest, const void *pvSrc, unsigned long iCount);
#endif

unsigned char* USBDEV_TellLibraryVersion(void)
{
	return (unsigned char*)USB_DEVICE_C_Version;
}

//void USBDEV_UpdateAsciiSerialNumber(unsigned char *pAsciiSerialNumber, unsigned int length)
//{
//	USBDEV_CLASS_UpdateAsciiSerialNumber(pAsciiSerialNumber, length);
//}

unsigned int USBDEV_IsConnected(void)
{
	return g_pUSBDEV_IO_Driver->IsConnected();
}

void USBDEV_SetState(USBDEV_STATE_T state)
{
	g_USBDEV_State = state;
}

USBDEV_STATE_T USBDEV_GetState(void)
{
	return g_USBDEV_State;
}

void USBDEV_SetClass(USBDEV_CLASS_T classType)
{
#if !defined(USBDEV_VTC_INCLUDE) && !defined(USBDEV_FASTBOOT_INCLUDE)
#error "At least one of these should be defined for proper working."
#endif

	if(g_USBDEV_CurrentClass == classType)
	{
		// nothing to do.
		return;
	}

	switch(classType)
	{
		default:
		#ifdef USBDEV_VTC_INCLUDE
		case USBDEV_CLASS_VTC:
		{
			extern USBDEV_CLASS_DRIVER_T *VTC_GetDriver(void);
			g_pUSBDEV_ClassDriver = VTC_GetDriver();
			g_USBDEV_CurrentClass = USBDEV_CLASS_VTC;
			break;
		}
		#endif
		#ifdef USBDEV_MSC_INCLUDE
		case USBDEV_CLASS_MSC:
		{
			extern USBDEV_CLASS_DRIVER_T *MSC_GetDriver(void);
			g_pUSBDEV_ClassDriver = MSC_GetDriver();
			g_USBDEV_CurrentClass = USBDEV_CLASS_MSC;
			break;
		}
		#endif
		#ifdef USBDEV_FASTBOOT_INCLUDE
		case USBDEV_CLASS_FASTBOOT:
		{
			extern USBDEV_CLASS_DRIVER_T *FASTBOOT_GetDriver(void);
			g_pUSBDEV_ClassDriver = FASTBOOT_GetDriver();
			g_USBDEV_CurrentClass = USBDEV_CLASS_FASTBOOT;
			break;
		}
		#endif
	}

	g_pUSBDEV_ClassDriver->Open();
}

USBDEV_CLASS_T USBDEV_GetCurrentClass(void)
{
	return g_USBDEV_CurrentClass;
}

void USBDEV_Open(USBDEVICE_IO_DRIVER_T *pIoDriver)
{
	g_pUSBDEV_IO_Driver = pIoDriver;

#if defined(USBDEV_VTC_INCLUDE)
	USBDEV_SetClass(USBDEV_CLASS_VTC);
#endif
}

void USBDEV_Init(void)
{
	g_pUSBDEV_IO_Driver->Init();
}

void USBDEV_Reset(void)
{
	g_USBDEV_ConfigurationValue = 0;
	USBDEV_SetState(USBDEV_STATE_DEFAULT);
	g_pUSBDEV_IO_Driver->Reset();
	g_pUSBDEV_ClassDriver->Event(USBDEV_EVENT_RESET);
}

void USBDEV_Isr(void)
{
	if( g_pUSBDEV_IO_Driver != NULL )
		g_pUSBDEV_IO_Driver->Isr();
}

USBDEV_SPEED_T USBDEV_GetSpeedLimit(void)
{
	return g_pUSBDEV_ClassDriver->speedLimit;
}

void USBDEV_Request_GetStatus(USB_DEVICE_REQUEST_T *pDeviceRequest)
{
	unsigned int stall = FALSE;
	unsigned short status;
	switch( pDeviceRequest->bmRequestType & USB_DEVREQ_RECIPIENT_MASK )
	{
		case USB_DEVREQ_RECIPIENT_DEVICE:
			status = 0x00;						// bit0 [Bus-powered]0:[Self-powered]1 , bit1=RemoteWakeup
			break;
		case USB_DEVREQ_RECIPIENT_INTERFACE:
			status = 0x00;
			break;
		case USB_DEVREQ_RECIPIENT_ENDPOINT:
			if( g_pUSBDEV_IO_Driver->EP_IsStalled((unsigned char)(pDeviceRequest->wIndex&0xFF)) == FALSE )
				status = 0x00;
			else
				status = 0x01;
			break;
		default:
			stall = TRUE;
			break;
	}
	if( stall == FALSE )
	{
		g_pUSBDEV_IO_Driver->EP0_Write(&status, (unsigned int)min(pDeviceRequest->wLength,2));
	}
	else
	{
		g_pUSBDEV_IO_Driver->EP_Stall(0x80);
	}
}

void USBDEV_Request_ClearFeature(USB_DEVICE_REQUEST_T *pDeviceRequest)
{
	unsigned char recipient = pDeviceRequest->bmRequestType&USB_DEVREQ_RECIPIENT_MASK;
	unsigned short featureSelector = pDeviceRequest->wValue;
	switch( recipient )
	{
		case USB_DEVREQ_RECIPIENT_DEVICE:
			if( featureSelector == USB_FEATURE_SELECTOR_DEVICE_REMOTE_WAKEUP )
			{
			}
			else if( featureSelector == USB_FEATURE_SELECTOR_TEST_MODE )
			{
				// Note: The Test_Mode feature cannot be cleared by the ClearFeature() request
			}
			break;
		case USB_DEVREQ_RECIPIENT_INTERFACE:
			break;
		case USB_DEVREQ_RECIPIENT_ENDPOINT:
			if( featureSelector == USB_FEATURE_SELECTOR_ENDPOINT_HALT )
			{
				unsigned char epAddress = (unsigned char)(pDeviceRequest->wIndex&0xFF);
				g_pUSBDEV_IO_Driver->EP_Unstall(epAddress);
			}
			break;
	}

	g_pUSBDEV_IO_Driver->EP0_Write(NULL, 0);
}

void USBDEV_Request_SetFeature(USB_DEVICE_REQUEST_T *pDeviceRequest)
{
	unsigned char recipient = pDeviceRequest->bmRequestType&USB_DEVREQ_RECIPIENT_MASK;
	unsigned short featureSelector = pDeviceRequest->wValue;
	unsigned char result = FALSE;
	switch( recipient )
	{
		case USB_DEVREQ_RECIPIENT_DEVICE:
			if( featureSelector == USB_FEATURE_SELECTOR_DEVICE_REMOTE_WAKEUP )
			{
				result=TRUE;
			}
			else if( featureSelector == USB_FEATURE_SELECTOR_TEST_MODE )
			{
				g_pUSBDEV_IO_Driver->TestMode((unsigned char)((pDeviceRequest->wIndex>>8)&0xFF));
				return;		// special case. Status transaction is already sended at TestMode().
			}
			break;
		case USB_DEVREQ_RECIPIENT_INTERFACE:
			break;
		case USB_DEVREQ_RECIPIENT_ENDPOINT:
			if( featureSelector == USB_FEATURE_SELECTOR_ENDPOINT_HALT )
			{
				unsigned char epAddress = (unsigned char)(pDeviceRequest->wIndex&0xFF);
				result=TRUE;
				g_pUSBDEV_IO_Driver->EP_Stall(epAddress);
			}
			break;
		default:
			break;
	}
	if(result==TRUE)
		g_pUSBDEV_IO_Driver->EP0_Write(NULL, 0);
	else
		g_pUSBDEV_IO_Driver->EP_Stall(0x80);
}

void USBDEV_Request_SetAddress(USB_DEVICE_REQUEST_T *pDeviceRequest)
{
	if( 0 < pDeviceRequest->wValue && pDeviceRequest->wValue < 128 )
	{
		g_pUSBDEV_IO_Driver->SetAddress(pDeviceRequest->wValue);
		USBDEV_SetState(USBDEV_STATE_ADDRESS);
	}
	else
		USBDEV_SetState(USBDEV_STATE_DEFAULT);

	g_pUSBDEV_IO_Driver->EP0_Write(NULL, 0);
}

static void * USBDEV_GetStringDescriptor(unsigned char index, unsigned short languageID, unsigned int *pSize)
{
#define LANGUAGE_ID_NUMBER_MAX		10
	unsigned int nLanguageID = g_pUSBDEV_ClassDriver->stringDescriptorsSets.number;
	unsigned int i;

	if( nLanguageID > LANGUAGE_ID_NUMBER_MAX )
		return NULL;

	if( index == 0 )
	{
		static __packed struct {
			unsigned char bLength;
			unsigned char bDescriptorType;
			unsigned short wLANGID[LANGUAGE_ID_NUMBER_MAX];
		} languageDescriptor;
		languageDescriptor.bLength = (unsigned char)( 2 + (nLanguageID<<1) );
		languageDescriptor.bDescriptorType = USB_DESCRIPTOR_TYPE_STRING;
		for( i = 0 ; i < nLanguageID ; i++ )
			languageDescriptor.wLANGID[i] = g_pUSBDEV_ClassDriver->stringDescriptorsSets.pLanguageSets[i].languageID;
		*pSize = (unsigned int)languageDescriptor.bLength;
		return (void*)&languageDescriptor;
	}
	else
	{
		unsigned int iLanguage = nLanguageID;
		for( i = 0 ; i < nLanguageID ; i++ )
		{
			if( g_pUSBDEV_ClassDriver->stringDescriptorsSets.pLanguageSets[i].languageID == languageID )
			{
				iLanguage = i;
				break;
			}
		}
		if( iLanguage != nLanguageID )
		{
			for( i = 0 ; i < g_pUSBDEV_ClassDriver->stringDescriptorsSets.pLanguageSets[iLanguage].number ; i++ )
			{
				if( index == g_pUSBDEV_ClassDriver->stringDescriptorsSets.pLanguageSets[iLanguage].pDescriptors[i].index )
				{
					*pSize = g_pUSBDEV_ClassDriver->stringDescriptorsSets.pLanguageSets[iLanguage].pDescriptors[i].size;
					return g_pUSBDEV_ClassDriver->stringDescriptorsSets.pLanguageSets[iLanguage].pDescriptors[i].pDescriptor;
				}
			}
		}

		return NULL;
	}
}

void USBDEV_Request_GetDescriptor(USB_DEVICE_REQUEST_T *pDeviceRequest)
{
	unsigned char descriptorType = (unsigned char)( (pDeviceRequest->wValue >> 8) & 0xFF );
	const void *pBuffer = NULL;
	unsigned int size=0;

	switch( descriptorType )
	{
		case USB_DESCRIPTOR_TYPE_DEVICE:
			pBuffer = g_pUSBDEV_ClassDriver->deviceDescriptor.pDescriptor;
			size = (unsigned int)min(pDeviceRequest->wLength, g_pUSBDEV_ClassDriver->deviceDescriptor.size);
			break;
		case USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER:
			pBuffer = g_pUSBDEV_ClassDriver->deviceQualifierDescriptor.pDescriptor;
			size = (unsigned int)min(pDeviceRequest->wLength, g_pUSBDEV_ClassDriver->deviceQualifierDescriptor.size);
			break;
		case USB_DESCRIPTOR_TYPE_CONFIGURATION:
			if( g_pUSBDEV_IO_Driver->currentSpeed == USBDEV_HIGH_SPEED )
			{
				pBuffer = g_pUSBDEV_ClassDriver->configDescriptor_USB20.pDescriptor;
				size = (unsigned int)min(pDeviceRequest->wLength, g_pUSBDEV_ClassDriver->configDescriptor_USB20.size);
			}
			else
			{
				pBuffer = g_pUSBDEV_ClassDriver->configDescriptor_USB11.pDescriptor;
				size = (unsigned int)min(pDeviceRequest->wLength, g_pUSBDEV_ClassDriver->configDescriptor_USB11.size);
			}
			break;
		case USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION:
			if( g_pUSBDEV_ClassDriver->speedLimit == USBDEV_HIGH_SPEED )
			{
				USB_CONFIGURATION_DESCRIPTOR_T *pConfigDescriptor;

				if( g_pUSBDEV_IO_Driver->currentSpeed == USBDEV_HIGH_SPEED )	// It wants other speed configuration
				{
					pBuffer = g_pUSBDEV_ClassDriver->configDescriptor_USB11.pDescriptor;
					size = (unsigned int)min(pDeviceRequest->wLength, g_pUSBDEV_ClassDriver->configDescriptor_USB11.size);
				}
				else
				{
					pBuffer = g_pUSBDEV_ClassDriver->configDescriptor_USB20.pDescriptor;
					size = (unsigned int)min(pDeviceRequest->wLength, g_pUSBDEV_ClassDriver->configDescriptor_USB20.size);
				}

				pConfigDescriptor = (USB_CONFIGURATION_DESCRIPTOR_T *)pBuffer;
				pConfigDescriptor->bDescriptorType = USB_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION;
				g_pUSBDEV_IO_Driver->EP0_Write((void*)pBuffer,size);
				pConfigDescriptor->bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION;
				return;
			}
			break;
		case USB_DESCRIPTOR_TYPE_STRING:
			pBuffer = USBDEV_GetStringDescriptor((unsigned char)( pDeviceRequest->wValue & 0xFF ), pDeviceRequest->wIndex, &size);
			size = (unsigned int)min(pDeviceRequest->wLength, size);
			break;
		default:
			break;
	}

	if( pBuffer != NULL )
		g_pUSBDEV_IO_Driver->EP0_Write((void*)pBuffer,size);
	else
		g_pUSBDEV_IO_Driver->EP_Stall(0x80);
}

void USBDEV_Request_GetConfiguration(USB_DEVICE_REQUEST_T *pDeviceRequest)
{
	if( pDeviceRequest->wValue == 0 && pDeviceRequest->wIndex == 0 && pDeviceRequest->wLength == 1 )
		g_pUSBDEV_IO_Driver->EP0_Write(&g_USBDEV_ConfigurationValue,1);
	else
		g_pUSBDEV_IO_Driver->EP_Stall(0x80);
}

void USBDEV_EP0_Setup(USB_DEVICE_REQUEST_T *pDeviceRequest)
{
	if( (pDeviceRequest->bmRequestType&USB_DEVREQ_TYPE_MASK) != USB_DEVREQ_TYPE_STANDARD )
	{
		if( g_pUSBDEV_ClassDriver->EP0_Setup(pDeviceRequest) == FALSE )
			g_pUSBDEV_IO_Driver->EP_Stall(0x80);		// TODO:
		return;
	}

	switch( pDeviceRequest->bRequest )
	{
		case USB_REQUEST_GET_STATUS:
			USBDEV_Request_GetStatus(pDeviceRequest);
			break;

		case USB_REQUEST_CLEAR_FEATURE:
			USBDEV_Request_ClearFeature(pDeviceRequest);
			break;

		case USB_REQUEST_SET_FEATURE:
			USBDEV_Request_SetFeature(pDeviceRequest);
			break;

		case USB_REQUEST_SET_ADDRESS:
			USBDEV_Request_SetAddress(pDeviceRequest);
			break;

		case USB_REQUEST_GET_DESCRIPTOR:
			USBDEV_Request_GetDescriptor(pDeviceRequest);
			break;

		case USB_REQUEST_GET_CONFIGURATION:
			USBDEV_Request_GetConfiguration(pDeviceRequest);
			break;

		case USB_REQUEST_SET_CONFIGURATION:
			g_USBDEV_ConfigurationValue = (unsigned char)pDeviceRequest->wValue;
		case USB_REQUEST_GET_INTERFACE:
		case USB_REQUEST_SET_INTERFACE:
			if( g_pUSBDEV_ClassDriver->EP0_Setup(pDeviceRequest) == FALSE )
				g_pUSBDEV_IO_Driver->EP_Stall(0x80);		// TODO:
			break;

		case USB_REQUEST_SYNC_FRAME:
			// TODO for isochronous transfer
			break;
		default:
			g_pUSBDEV_IO_Driver->EP_Stall(0x80);		// TODO:
			break;
	}
}

void USBDEV_Run(void)
{
	if( USBDEV_IsConnected() == FALSE )
		USBDEV_SetState(USBDEV_STATE_DEFAULT);
	else if( USBDEV_GetState() == USBDEV_STATE_CONFIGURED )
		g_pUSBDEV_ClassDriver->Run();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	Functions for Class Driver
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////


int USBDEV_AllocQueue(PUSBDEV_QUEUE_ALLOC_T pQueueAlloc)
{
	return g_pUSBDEV_IO_Driver->AllocQueue(pQueueAlloc);
}

USBDEV_SPEED_T USBDEV_GetCurrentSpeed(void)
{
	return g_pUSBDEV_IO_Driver->currentSpeed;
}

void USBDEV_ExtractEP(USBDEV_ENDPOINT_T *pEP, const USB_ENDPOINT_DESCRIPTOR_T *pDescriptor)
{
	pEP->index = pDescriptor->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
	pEP->isIn = ( (pDescriptor->bEndpointAddress&USB_ENDPOINT_DIRECTION_MASK) == USB_ENDPOINT_DIRECTION_IN ) ? TRUE : FALSE;
	pEP->transferType = (USBDEV_EP_TRANSFER_TYPE_T)( pDescriptor->bmAttributes & USB_ENDPOINT_TRANSFER_TYPE_MASK );
	pEP->syncType = (USBDEV_EP_SYNC_TYPE_T)( (pDescriptor->bmAttributes&USB_ENDPOINT_SYNCHRONIZATION_TYPE_MASK)>>2 );
	pEP->usageType = (USBDEV_EP_USAGE_TYPE_T)( (pDescriptor->bmAttributes&USB_ENDPOINT_USAGE_TYPE_MASK)>>4 );
	pEP->maximumPacketSize = (unsigned short)(pDescriptor->wMaxPacketSize&USB_ENDPOINT_MAX_PACKET_SIZE_MASK);
	pEP->nAdditionalTransaction = (unsigned char)((pDescriptor->wMaxPacketSize&USB_ENDPOINT_NUM_ADDITIONAL_MICROFRAME_MASK)>>11);
	pEP->bInterval = pDescriptor->bInterval;
}

unsigned int USBDEV_EP_Enable(USBDEV_ENDPOINT_T *pEP, unsigned int enable)
{
	pEP->uiPreOutSize = 0;
	return g_pUSBDEV_IO_Driver->EP_Enable(pEP, enable);
}

unsigned int USBDEV_EP_IsReady(USBDEV_ENDPOINT_T *pEP)
{
	return g_pUSBDEV_IO_Driver->EP_IsReady(pEP);
}

unsigned int USBDEV_EP0_Read(void *pRxBuffer, unsigned int nBufferSize)
{
	return g_pUSBDEV_IO_Driver->EP0_Read(pRxBuffer, nBufferSize);
}

void USBDEV_EP_Read_SetSize(USBDEV_ENDPOINT_T *pEP, unsigned int nBufferSize)
{
	if(pEP->isIn == FALSE)
	{
		g_pUSBDEV_IO_Driver->EP_SetQueue(pEP,nBufferSize);
		pEP->uiPreOutSize = nBufferSize;
	}
}

unsigned int USBDEV_EP_Read_GetPointer(USBDEV_ENDPOINT_T *pEP, void **pRxBuffer)
{
	PUSBDEV_QUEUE_T pQueue;
	unsigned int _actualLen = 0;
	pQueue = g_pUSBDEV_IO_Driver->EP_GetQueue(pEP);
	if( pQueue != NULL )
	{
		*pRxBuffer = (void*)pQueue->buff;
		_actualLen = pQueue->actualLen;

		if(pEP->uiPreOutSize < _actualLen)
		{
			pEP->uiPreOutSize = 0;
		}
		else
		{
			pEP->uiPreOutSize -= _actualLen;
		}
	}
	else
	{
		*pRxBuffer = NULL;
		pEP->uiPreOutSize = 0;
	}

	return _actualLen;
}

unsigned int USBDEV_EP_Read(USBDEV_ENDPOINT_T *pEP, void *pRxBuffer, unsigned int nBufferSize)
{
	unsigned int nTotalReadSize = 0;

	if(pEP->isIn == FALSE)
	{
		if(pEP->uiPreOutSize == 0)
		{
			g_pUSBDEV_IO_Driver->EP_SetQueue(pEP,nBufferSize);
		}
		else if(pEP->uiPreOutSize < nBufferSize)
		{
			pEP->uiPreOutSize = 0;

			return nTotalReadSize;
		}
		else
		{
			pEP->uiPreOutSize -= nBufferSize;
		}

		do
		{
			PUSBDEV_QUEUE_T pQueue;
			pQueue = g_pUSBDEV_IO_Driver->EP_GetQueue(pEP);
			if( pQueue != NULL )
			{
				if(pRxBuffer)
				{
					memcpy(pRxBuffer,pQueue->buff,pQueue->actualLen);
					pRxBuffer = (void*)((unsigned int)pRxBuffer + pQueue->actualLen);
				}
				nBufferSize -= pQueue->actualLen;
				nTotalReadSize += pQueue->actualLen;
			}
			else // failure!
			{
				pEP->uiPreOutSize = 0;
				nBufferSize = 0;
			}
		} while(nBufferSize > 0);
	}

	return nTotalReadSize;
}

unsigned int USBDEV_EP0_Write(void *pTxBuffer, unsigned int nBufferSize)
{
	return g_pUSBDEV_IO_Driver->EP0_Write(pTxBuffer, nBufferSize);
}

unsigned int USBDEV_EP_Write(USBDEV_ENDPOINT_T *pEP, const void *pTxBuffer, unsigned int nBufferSize)
{
	unsigned int nTotalWriteSize=0;
	do
	{
		PUSBDEV_QUEUE_T pQueue = g_pUSBDEV_IO_Driver->EP_GetQueue(pEP);
		if( pQueue )
		{
			unsigned int writeSize = min(pQueue->buffSize,nBufferSize);
			memcpy(pQueue->buff,pTxBuffer,writeSize);
			pTxBuffer = (void*)((unsigned int)pTxBuffer + writeSize );
			writeSize = g_pUSBDEV_IO_Driver->EP_SetQueue(pEP,writeSize);
			nBufferSize -= writeSize;
			nTotalWriteSize += writeSize;
		}
		else
			return nTotalWriteSize;
	} while( nBufferSize > 0 );

	return nTotalWriteSize;
}

void USBDEV_EP_Stall(USBDEV_ENDPOINT_T *pEP)
{
	unsigned char epAddress = pEP->index;
	if( pEP->isIn == TRUE )
	{
		epAddress |= 0x80;
		while( USBDEV_IsConnected() == TRUE )
		{
			if( g_pUSBDEV_IO_Driver->EP_IsReady(pEP) == TRUE )
				break;
		}
	}

	g_pUSBDEV_IO_Driver->EP_Stall(epAddress);
}

void USBDEV_EP_Unstall(USBDEV_ENDPOINT_T *pEP)
{
	unsigned char epAddress = pEP->index;
	g_pUSBDEV_IO_Driver->EP_Unstall(epAddress);
}

void USBDEV_EP_UnstallEnable(USBDEV_ENDPOINT_T *pEP, unsigned int enable)
{
	unsigned char epAddress = pEP->index;
	if( pEP->isIn == TRUE )
		epAddress |= 0x80;
	g_pUSBDEV_IO_Driver->EP_UnstallEnable(epAddress, enable);
}

unsigned int USBDEV_EP_IsStalled(USBDEV_ENDPOINT_T *pEP)
{
	unsigned char epAddress = pEP->index;
	if( pEP->isIn == TRUE )
		epAddress |= 0x80;
	return g_pUSBDEV_IO_Driver->EP_IsStalled(epAddress);
}

void USBDEV_EP_Flush(USBDEV_ENDPOINT_T *pEP)
{
	g_pUSBDEV_IO_Driver->EP_Flush(pEP);
}

USBDEV_ERROR_T USBDEV_GetLastError(void)
{
	return g_pUSBDEV_IO_Driver->GetLastError();
}
