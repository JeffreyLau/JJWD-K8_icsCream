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
#include "usb/otgcore.h"
#include <platform/iomap.h>
#include <platform/reg_physical.h>
#include "usb/otgregs.h"

/* For Signature */
#define OTGCORE_SIGNATURE			'O','T','G','C','O','R','E','_'
#define OTGCORE_VERSION				'V','2','.','0','0','0'
static const unsigned char OTGCORE_C_Version[] = {SIGBYAHONG, OTGCORE_SIGNATURE, SIGN_OS ,SIGN_CHIPSET, OTGCORE_VERSION, NULL};


unsigned char* OTGCORE_TellLibraryVersion(void)
{
	return (unsigned char*)OTGCORE_C_Version;
}

void OTGCORE_Reset(void)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	volatile unsigned int i,cnt=0;

	// Core Soft Reset
	USB20OTG->GRSTCTL |= GRSTCTL_CSftRst;
	// wait self clear
	for(i=0 ; i<0x100000 && (USB20OTG->GRSTCTL & GRSTCTL_CSftRst) != 0 ; i++)
	{
		cnt++;
	}

	// wait 3 PHY clocks (synchronization delay)
	for(i=0;i<1000000;i++)
	{
		cnt++;
	}

	// wait AHB idle
	for(i=0 ; i<0x100000 && !(USB20OTG->GRSTCTL & GRSTCTL_AHBIdle) ; i++)
	{
		cnt++;
	}
}

void OTGCORE_Init(void)
{
	PUSB20OTG USB20OTG = (PUSB20OTG) TCC_USB20OTG_BASE;
	volatile unsigned int i,cnt=0;

#if defined(TCC892X)
	OTGCORE_Reset();
#else
	// wait AHB idle
	for(i=0 ; i<0x100000 && !(USB20OTG->GRSTCTL & GRSTCTL_AHBIdle) ; i++)
	{
		cnt++;
	}
#endif

#if defined(SLAVE_MODE_INCLUDE)
#ifdef PLATFORM_TCC88XX
	USB20OTG->GUSBCFG |= GUSBCFG_ForceDevMode;	// XXX: force device mode
#endif
	USB20OTG->GUSBCFG |= GUSBCFG_PHYIf_16BITS|GUSBCFG_TOutCal(7);	// set interface, timeout
	USB20OTG->GOTGCTL |= GOTGCTL_SesReq;	// session request
	USB20OTG->GINTSTS = 0xFFFFFFFF;		// clear interrupts
	USB20OTG->GAHBCFG = GAHBCFG_NPTXFEmpLvl_COMPLETELY|GAHBCFG_GlblIntrMsk_UNMASK;	// enable global interrupt
#elif defined(DMA_MODE_INCLUDE)
	// Core Initialization
	USB20OTG->GINTSTS = 0xFFFFFFFF;		// clear interrupts
	USB20OTG->GAHBCFG = GAHBCFG_DMAEn|GAHBCFG_HBstLen_SINGLE|GAHBCFG_GlblIntrMsk_UNMASK;
	USB20OTG->GUSBCFG |= GUSBCFG_PHYIf_16BITS|GUSBCFG_TOutCal(7);	// set interface, timeout
	//USB20OTG->GUSBCFG = GUSBCFG_USBTrdTim_16BIT_UTMIP|GUSBCFG_PHYIf_16BITS|GUSBCFG_TOutCal(7);
	USB20OTG->GOTGCTL |= GOTGCTL_SesReq;	// session request
	//USB20OTG->GINTMSK = GINTMSK_OTGIntMsk_UNMASK|GINTMSK_ModeMisMsk_UNMASK;
#endif
}
