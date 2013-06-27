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

#ifndef _USB_DEFS_H
#define _USB_DEFS_H

#if defined(TCC89XX) || defined(TCC91XX) || defined(TCC92XX)
#define OTG_INCLUDE
//#define HwPIC						((PPIC)&(HwPIC_BASE))
#define HwUSBPHYCFG					((PUSBPHYCFG)&HwUSBPHYCFG_BASE)
#define HwUSB20OTG					((PUSB20OTG)&HwUSB20OTG_BASE)
#define HwUSBOTGCFG					((PUSBOTGCFG)&(HwUSBOTGCFG_BASE))
#elif defined(TCC93XX) || defined(TCC88XX) || defined(TCC892X)
#define HwPIC						((PPIC)&(HwPIC_BASE))
#define HwUSBPHYCFG					((PUSBPHYCFG)&(HwUSBPHYCFG_BASE))
#define HwUSB20OTG					((PUSB20OTG)&(HwUSB20OTG_BASE))
#else
PPIC HwPIC;
PUSBPHYCFG HwUSBPHYCFG;
PUSB20OTG HwUSB20OTG;
PUSBOTGCFG HwUSBOTGCFG;
PGPIO HwGPIO;
#error
#endif


#define USBDEV_VTC_INCLUDE
//#define USBDEV_MSC_INCLUDE
#define USBDEV_FASTBOOT_INCLUDE

//===================================================================
//
//		DRIVER SIGNATURE
//
//===================================================================
#define SIGBYAHONG				'S','I','G','B','Y','A','H','O','N','G','_'

#if defined(_WINCE_)
#define SIGN_OS					'W','I','N','C','E','_'
#elif defined(_LINUX_)
#define SIGN_OS					'L','I','N','U','X','_'
#else
#define SIGN_OS					'N','U','_'
#endif

#if defined(TCC77X)
#define SIGN_CHIPSET			'T','C','C','7','7','X','X','_'
#elif defined(TCC78X)
#define SIGN_CHIPSET			'T','C','C','7','8','X','X','_'
#elif defined(TCC79X)
#define SIGN_CHIPSET			'T','C','C','7','9','X','X','_'
#elif defined(TCC81XX)
#define SIGN_CHIPSET			'T','C','C','8','1','X','X','_'
#elif defined(TCC82XX)
#define SIGN_CHIPSET			'T','C','C','8','2','X','X','_'
#elif defined(TCC83XX)
#define SIGN_CHIPSET			'T','C','C','8','3','X','X','_'
#elif defined(TCC87XX)
#define SIGN_CHIPSET			'T','C','C','8','7','X','X','_'
#elif defined(TCC89XX)
#define SIGN_CHIPSET			'T','C','C','8','9','X','X','_'
#elif defined(TCC91XX)
#define SIGN_CHIPSET			'T','C','C','9','1','X','X','_'
#elif defined(TCC92XX)
#define SIGN_CHIPSET			'T','C','C','9','2','X','X','_'
#elif defined(TCC93XX)
#define SIGN_CHIPSET			'T','C','C','9','3','X','X','_'
#elif defined(TCC88XX)
#define SIGN_CHIPSET			'T','C','C','8','8','X','X','_'
#elif defined(TCC892X)
#define SIGN_CHIPSET			'T','C','C','8','9','2','X','_'
#else
#error
#endif

#ifndef NULL
#define NULL				0
#endif

#ifndef FALSE
#define FALSE				0
#endif

#ifndef TRUE
#define TRUE				1
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef dim
#define dim(x) (sizeof(x) / sizeof(x[0]))
#endif

#ifndef BITSET
#define BITSET(X, MASK) 			( (X) |= (unsigned int)(MASK) )
#endif
#ifndef BITSCLR
#define BITSCLR(X, SMASK, CMASK)	( (X) = ((((unsigned int)(X)) | ((unsigned int)(SMASK))) & ~((unsigned int)(CMASK))) )
#endif
#ifndef BITCSET
#define BITCSET(X, CMASK, SMASK)	( (X) = ((((unsigned int)(X)) & ~((unsigned int)(CMASK))) | ((unsigned int)(SMASK))) )
#endif
#ifndef BITCLR
#define BITCLR(X, MASK) 			( (X) &= ~((unsigned int)(MASK)) )
#endif
#ifndef BITXOR
#define BITXOR(X, MASK) 			( (X) ^= (unsigned int)(MASK) )
#endif
#ifndef ISZERO
#define ISZERO(X, MASK) 			(  ! (((unsigned int)(X)) & ((unsigned int)(MASK))) )
#endif

extern int uart_send_printf(const char *str , ... );
//#define USBDEV_DEBUG(fmt,args...)		uart_send_printf(fmt,##args)
#define USBDEV_DEBUG(fmt,...)
//#define USBDEV_ERROR(fmt,args...)		uart_send_printf(fmt,##args)
#define USBDEV_ERROR(fmt,...)

#endif //_USB_DEFS_H
