/*
 * USB PHY (UTMI+) driver source code
 *
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
#include "usb/usbphy.h"
#include <platform/iomap.h>
#include <platform/reg_physical.h>
#include "usb/otgregs.h"

/* For Signature */
#define USBPHY_SIGNATURE			'U','S','B','P','H','Y','_'
#define USBPHY_VERSION				'V','2','.','0','0','0'
static const unsigned char USBPHY_C_Version[] = {SIGBYAHONG, USBPHY_SIGNATURE, SIGN_OS ,SIGN_CHIPSET, USBPHY_VERSION, NULL};

#define UPCR0_PR						Hw14					// (1:enable)/(0:disable) Port Reset
#define UPCR0_CM_EN						Hw13					// Common Block Power Down Enable
#define UPCR0_CM_DIS					~Hw13					// Common Block Power Down Disable
#define UPCR0_RCS_11					(Hw12+Hw11)				// Reference Clock Select for PLL Block ; The PLL uses CLKCORE as reference
#define UPCR0_RCS_10					Hw12					// Reference Clock Select for PLL Block ; The PLL uses CLKCORE as reference
#define UPCR0_RCS_01					Hw11					// Reference Clock Select for PLL Block ; The XO block uses an external clock supplied on the XO pin
#define UPCR0_RCS_00					((~Hw12)&(~Hw11))		// Reference Clock Select for PLL Block ; The XO block uses the clock from a crystal
#define UPCR0_RCD_48					Hw10					// Reference Clock Frequency Select ; 48MHz
#define UPCR0_RCD_24					Hw9						// Reference Clock Frequency Select ; 24MHz
#define UPCR0_RCD_12					((~Hw10)&(~Hw9))		// Reference Clock Frequency Select ; 12MHz
#define UPCR0_SDI_EN					Hw8						// IDDQ Test Enable ; The analog blocks are powered down
#define UPCR0_SDI_DIS					~Hw8					// IDDQ Test Disable ; The analog blocks are not powered down
#define UPCR0_FO_SI						Hw7						// UTMI/Serial Interface Select ; Serial Interface
#define UPCR0_FO_UTMI					~Hw7					// UTMI/Serial Interface Select ; UTMI
#define UPCR0_VBDS						Hw6
#define UPCR0_DMPD						Hw5						// 1:enable / 0:disable the pull-down resistance on D-
#define UPCR0_DPPD						Hw4						// 1: enable, 0:disable the pull-down resistance on D+
#define UPCR0_VBD						Hw1						// The VBUS signal is (1:valid)/(0:invalid), and the pull-up resistor on D+ is (1:enabled)/(0:disabled)

#define UPCR1_TXFSLST(x)				((x&0xF)<<12)
#define UPCR1_SQRXT(x)					((x&0x7)<<8)
#define UPCR1_OTGT(x)					((x&0x7)<<4)
#define UPCR1_CDT(x)					(x&0x7)

#define UPCR2_TM_FS						Hw14
#define UPCR2_TM_HS						0
#define UPCR2_XCVRSEL_LS_ON_FS			(Hw13|Hw12)
#define UPCR2_XCVRSEL_LS				Hw13
#define UPCR2_XCVRSEL_FS				Hw12
#define UPCR2_XCVRSEL_HS				0
#define UPCR2_OPMODE_MASK				(Hw10|Hw9)
#define UPCR2_OPMODE_SYNC_EOP			(Hw10|Hw9)
#define UPCR2_OPMODE_DIS_BITS_NRZI		Hw10
#define UPCR2_OPMODE_NON_DRVING			Hw9
#define UPCR2_OPMODE_NORMAL				0
#define UPCR2_TXVRT(x)					((x&0xF)<<5)
#define UPCR2_TXRT(x)					((x&0x3)<<2)
#define UPCR2_TP_EN						Hw0
#define UPCR2_TP_DIS					0

unsigned char* USBPHY_TellLibraryVersion(void)
{
	return (unsigned char*)USBPHY_C_Version;
}

static void USBPHY_Delay_100ms(void)
{
#define TCFG_TCKSEL(x)			(x<<4)
#define TCFG_CON				Hw1
#define TCFG_EN					Hw0

	PTIMER					pTIMER		= (TIMER *) TCC_TIMER_BASE;
	volatile unsigned int	baseCounter;
	volatile unsigned int	timed;
#if defined(TCC892X)
	volatile unsigned long *pulTCFG0 = &(pTIMER->TCFG0.nREG);
	volatile unsigned long *pTCNT0 = &(pTIMER->TCNT0.nREG);
#else
	volatile unsigned long *pulTCFG0 = &(pTIMER->TCFG0);
	volatile unsigned long *pTCNT0 = &(pTIMER->TCNT0);
#endif
	*pulTCFG0 = TCFG_TCKSEL(5)|TCFG_CON|TCFG_EN;	// 5: 11718.75 clk/s

	baseCounter = *pTCNT0;

	do {
		timed = ( *pTCNT0 - baseCounter ) & 0xFFFF;
	} while( timed < 1172 );	// wait 1172clk=100ms
	BITCLR(*pulTCFG0,TCFG_EN);
}

void USBPHY_DEVICE_Detach(void)
{
	PUSBPHYCFG USBPHYCFG = (PUSBPHYCFG) TCC_USBPHYCFG_BASE;

	BITCSET(USBPHYCFG->UPCR2,UPCR2_OPMODE_MASK,UPCR2_OPMODE_NON_DRVING);
	USBPHYCFG->UPCR0 = 0x4840;

	USBPHY_Delay_100ms();
}

void USBPHY_DEVICE_Attach(void)
{
	PUSBPHYCFG USBPHYCFG = (PUSBPHYCFG) TCC_USBPHYCFG_BASE;

	BITCSET(USBPHYCFG->UPCR2,UPCR2_OPMODE_MASK,UPCR2_OPMODE_NORMAL);
	USBPHYCFG->UPCR0 = 0x2842;
}

void USBPHY_EnableClock(void)
{
	PUSBPHYCFG USBPHYCFG = (PUSBPHYCFG) TCC_USBPHYCFG_BASE;

	// VBDS=1 for the VBUSVLDEXT input is used. (not internal session valid comparator)
	// If not, D+ will be pulled up even at host mode.
	BITSET(USBPHYCFG->UPCR0,Hw6);
	// MODE=0 for USBOTG, SDI=0 for turn on all analog blocks, VBD=0 for disable the pull-up resister on D+
	BITCLR(USBPHYCFG->UPCR0,Hw15|Hw8|Hw1);
}

void USBPHY_Disable(void)
{
	PUSBPHYCFG USBPHYCFG = (PUSBPHYCFG) TCC_USBPHYCFG_BASE;

	// MODE=0 for USBOTG, SDI=0 for turn on all analog blocks, VBD=0 for disable the pull-up resister on D+
	BITSET(USBPHYCFG->UPCR0,Hw8);
}

void USBPHY_SetMode(USBPHY_MODE_T mode)
{
	PUSBPHYCFG USBPHYCFG = (PUSBPHYCFG) TCC_USBPHYCFG_BASE;

	USBPHYCFG->UPCR1	=
					UPCR1_TXFSLST(7)	// FS/LS Pull-Up Resistance Adjustment = Design default
					| UPCR1_SQRXT(3)	// Squelch Threshold Tune = -5%
					| UPCR1_OTGT(4)		// VBUS Valid Threshold Adjustment = Design default
					| UPCR1_CDT(3);		// Disconnect Threshold Adjustment = Design default
	USBPHYCFG->UPCR2	=
					UPCR2_TM_HS
					| UPCR2_XCVRSEL_HS
					| UPCR2_OPMODE_NORMAL
					| UPCR2_TXVRT(8)	// HS DC voltage level adjustment = Design default
					| UPCR2_TXRT(0)		// HS Transmitter Rise/Fall Time Adjustment = Design default
					| UPCR2_TP_DIS;
	//USBPHYCFG->UPCR3	= 0x9000;	// OTG disable

	/*if ( mode == USB_MODE_NONE )
	{
		USBPHYCFG->UPCR0	= 0x2940;	// float DP,PM pins
	}
	else*/ if ( mode == USBPHY_MODE_RESET )
	{
		USBPHYCFG->UPCR0	= 0x4870;
	}
	else if ( mode == USBPHY_MODE_OTG )
	{
		USBPHYCFG->UPCR0	= 0x80002800;
	}
	else if ( mode == USBPHY_MODE_HOST )
	{
		//HwUSBPHYCFG->UPCR0	= 0x2872;
		USBPHYCFG->UPCR0	= 0x800028b2;
	}
	else if ( mode == USBPHY_MODE_DEVICE )
	{
		USBPHYCFG->UPCR0	= 0x2842;
	}
}

#ifndef OTGID_ID_HOST
#define OTGID_ID_HOST						(~Hw9)
#endif
#ifndef OTGID_ID_DEVICE
#define OTGID_ID_DEVICE						Hw9
#endif

#if defined(PLATFORM_TCC92XX)
void USBPHY_SetID(unsigned int ID)
{
	if ( ID )
		HwUSBOTGCFG->OTGID |= OTGID_ID_DEVICE;
	else
		HwUSBOTGCFG->OTGID &= OTGID_ID_HOST;
}
#endif
