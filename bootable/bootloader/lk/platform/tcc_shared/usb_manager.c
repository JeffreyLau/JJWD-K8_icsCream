/**************************************************************************************
 *
 * FILE NAME   : usb_manager.c
 * DESCRIPTION : This is common usb device source code
 *
 * ====================================================================================
 *
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 * ====================================================================================
 *
 * LAST UPDATED: $Date$ by $Author$
 *
 * FILE HISTORY:
 * 	Date: 2009.02.12	Start source coding
 *
 **************************************************************************************/
#include "usb/usb_defs.h"
#include "usb/usb_manager.h"
#include "usb/usb_device.h"
#include <platform/reg_physical.h>
#include <platform/globals.h>

/* For Signature */
#define USB_MANAGER_SIGNATURE	'U','S','B','_','M','A','N','A','G','E','R','_'
#define USB_MANAGER_VERSION		'V','2','.','0','0','1'
static const unsigned char USB_MANAGER_C_Version[] = {SIGBYAHONG, USB_MANAGER_SIGNATURE, SIGN_OS ,SIGN_CHIPSET, USB_MANAGER_VERSION, '\0'};


#define USBPHY_INCLUDE
#define OTGCORE_INCLUDE
#define OTGDEV_IO_INCLUDE

#if defined(USBPHY_INCLUDE)
#include "usb/usbphy.h"
#endif

#if defined(OTGCORE_INCLUDE)
#include "usb/otgcore.h"
#endif

#if defined(OTGDEV_IO_INCLUDE)
#include "usb/otgdev_io.h"
#endif

#include <platform/irqs.h>

//static USB_MODE_T g_USB_Mode;
//static char g_USB_DEVICE_fPrevOn;
//static char g_USB_DEVICE_fEnable;
static USB_DEVICE_MODE_T g_USB_DEVICE_mode;

void USB_DEVICE_SetDefaultSerialNumber(void);

unsigned char* USB_TellLibraryVersion(void)
{
	return (unsigned char*)USB_MANAGER_C_Version;
}

void USB_LINK_Reset(void)
{
#if defined(PLATFORM_TCC92XX)
	BITCLR(HwIOBUSCFG->HRSTEN0, HwIOBUSCFG_USB);
	{
		volatile unsigned int t=10000;
		while(t-->0);
	}
	BITSET(HwIOBUSCFG->HRSTEN0, HwIOBUSCFG_USB);
#elif defined(PLATFORM_TCC88XX) || defined(PLATFORM_TCC892X)
	tca_ckc_setioswreset(RB_USB20OTG, 1);
	{
		volatile unsigned int t=10000;
		while(t-->0);
	}
	tca_ckc_setioswreset(RB_USB20OTG, 0);
#else
	tca_ckc_set_iobus_swreset(RB_USB20OTG,OFF);
	{
		volatile unsigned int t=10000;
		while(t-->0);
	}
	tca_ckc_set_iobus_swreset(RB_USB20OTG,ON);
#endif
}

#if defined(FEATURE_USB_ISR)
enum handler_return {
	INT_NO_RESCHEDULE = 0,
	INT_RESCHEDULE,
};
static enum handler_return USB_Isr(void *arg)
#else
void USB_Isr(void)
#endif
{
	USBDEV_Isr();
}

void USB_Init(USBDEV_T mode)
{
	extern volatile void InitRoutine_Start(void);
	volatile void *tempPtr = InitRoutine_Start;
	// prevent from removing by optimized compilation
	if(tempPtr==NULL)
		InitRoutine_Start();
	//g_USB_Mode = USB_MODE_NONE;

	///////////////////////////////////////////
	// USB Driver Pre-Process
	///////////////////////////////////////////
	USBPHY_SetMode(USBPHY_MODE_RESET);
	USB_LINK_Reset();

	///////////////////////////////////////////
	// OTG Core Initializing Sequence
	///////////////////////////////////////////
	#if defined(OTGCORE_INCLUDE)
	OTGCORE_Init();
	#endif

	///////////////////////////////////////////
	// USB Host Driver Initializing Sequence
	///////////////////////////////////////////

	///////////////////////////////////////////
	// USB Device Driver Initializing Sequence
	///////////////////////////////////////////
	{
		USBDEVICE_IO_DRIVER_T *pIoDriver;

		//USB_DEVICE_SetDefaultSerialNumber();

		#if defined(OTGDEV_IO_INCLUDE)
		pIoDriver = OTGDEV_IO_GetDriver();
		#else
		#error
		#endif
		USBDEV_Open(pIoDriver);
		USBDEV_SetClass(mode);

		// Setting Port for checking of connection USB cable [ Port A.0 ]
		//#if defined(TCC79X)
		//BITCSET(HwEINTSEL1, HwEINTSEL1_EINT7SEL_MASK, HwEINTSEL1_EINT7(31));
		//BITCSET(HwIRQSEL, HwIRQSEL_SEL4_MASK, HwIRQSEL_SEL4_EINT7);
		//IO_INT_HwTMODE	= BITSET(uTMODE, IO_INT_HwEXT7);				// level trigger type for VBUS ON
		//#endif

		//g_USB_DEVICE_fPrevOn = FALSE;
		//g_USB_DEVICE_fEnable = TRUE;
	}

	///////////////////////////////////////////
	// USB Driver Post-Process
	///////////////////////////////////////////
	{
		#if defined(FEATURE_USB_ISR)
#if 1
		register_int_handler(INT_USB_OTG, &USB_Isr, NULL);
//		BITSET(HwPIC->SEL1, HwINT1_UOTG);							// 0:FIQ / 1:IRQ
//		BITCLR(HwPIC->POL1, HwINT1_UOTG);							// 0:active-high / 1:active-low
//		BITSET(HwPIC->MODE1, HwINT1_UOTG);							// 1:Level / 0:edge trigger type for USB interrupt
		//BITCLR(HwPIC->MODEA1, HwINT1_UOTG);						// 0:single edge / 1:both edge
//		BITSET(HwPIC->IEN1, HwINT1_UOTG);
		unmask_interrupt(INT_USB_OTG);
#else
		//BITSET(HwPIC->CLR1, /*HwINT1_UOTG*/1<<(48-32));
		BITSET(HwPIC->SEL1, HwINT1_UOTG);							// 0:FIQ / 1:IRQ
		BITCLR(HwPIC->POL1, HwINT1_UOTG);							// 0:active-high / 1:active-low
		BITSET(HwPIC->MODE1, HwINT1_UOTG);							// 1:Level / 0:edge trigger type for USB interrupt
		//BITCLR(HwPIC->MODEA1, HwINT1_UOTG);						// 0:single edge / 1:both edge
		BITSET(HwPIC->IEN1, HwINT1_UOTG);
#endif
		#endif
	}
}

unsigned int USB_DEVICE_IsConnected(void)
{
	return (USBDEV_IsConnected()==TRUE) ? TRUE : FALSE;
}

//void USB_DEVICE_Enable(void)
//{
//	g_USB_DEVICE_fEnable = TRUE;
//}

//void USB_DEVICE_Disable(void)
//{
//	g_USB_DEVICE_fEnable = FALSE;
//}

void USB_DEVICE_On(void)
{
	//if( g_USB_DEVICE_fPrevOn == FALSE )
	{
		//g_USB_DEVICE_fPrevOn = TRUE;
		#if defined(PLATFORM_TCC92XX)
		USBPHY_SetID(1);	// device mode
		#endif

		USBDEV_Init();
		#if defined(USBPHY_INCLUDE)
		USBPHY_SetMode(USBPHY_MODE_DEVICE);
		#endif
	}
}

//void USB_DEVICE_Off(void)
//{
//	if( g_USB_DEVICE_fPrevOn == TRUE )
//	{
//		g_USB_DEVICE_fPrevOn = FALSE;
//		#if defined(USBPHY_INCLUDE)
//		USBPHY_SetMode(USBPHY_MODE_RESET);
//		#endif
//		//BITSET(HwIOBUSCFG->HRSTEN0, HwIOBUSCFG_USB);
//		//IO_CKC_DisableBUS_USB();
//	}
//	USBDEV_DEBUG("USB_Device_Off\n");
//}

void USB_DEVICE_Run(void)
{
	USBDEV_Run();
}

//unsigned int USB_DEVICE_IsPreOccupied(void)
//{
//	if( g_USB_DEVICE_mode == MODE_MTP_PLAYBACK )
//		return FALSE;
//	else
//		return TRUE;
//}

//void USB_DEVICE_SetMode(USB_DEVICE_MODE_T mode)
//{
//	switch(mode)
//	{
//		case MODE_VTC:
//			USBDEV_SetClass(USBDEV_CLASS_VTC);
//			break;
//		case MODE_MSC:
//			USBDEV_SetClass(USBDEV_CLASS_MSC);
//			break;
//		case MODE_MTP:
//		case MODE_MTP_PLAYBACK:
//			USBDEV_SetClass(USBDEV_CLASS_MTP);
//			break;
//		//case MODE_PCDMB:
//		//	USBDEV_SetClass(USBDEV_CLASS_PCDMB);
//		//	break;
//	}
//	g_USB_DEVICE_mode = mode;
//}

//USB_DEVICE_MODE_T USB_DEVICE_GetMode(void)
//{
//	return g_USB_DEVICE_mode;
//}
