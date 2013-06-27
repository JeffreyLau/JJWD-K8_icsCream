/************************************************************************
*	TELECHIPS Digital Audio Player
*	------------------------------------------------
*
*	FUNCTION	:
*	MODEL		: TCCXXX
*	CPU NAME	: TCCXXX
*	SOURCE		: IO_TCCXXX.h
*
*	START DATE	: FEB. 16. 2009
*	MODIFY DATE	: FEB. 16. 2009
*	DEVISION	: DEPT. SYSTEM 3 GROUP
*				: TELECHIPS, INC.
************************************************************************/


/****************************************************************************
  Revision History
 ****************************************************************************

 2009/02/16		: .

 ****************************************************************************/


#ifndef	__IO_TCCXXX_H
#define	__IO_TCCXXX_H

#if defined(_LINUX_) 
#include <common.h>      
#endif  

//#define	USE_IO_DEBUG

#if (defined(SDRAM_SIZE) && (SDRAM_SIZE > 0x200000))
#define	DBG_MAXSTR_TX	0x100000
#else
#define	DBG_MAXSTR_TX	0x10
#endif
#define	DBG_BUFMASK_TX	(DBG_MAXSTR_TX - 1)
#define	DBG_MAXSTR_RX	0x10

//#define	USE_DOMEASURE
//#define	CHECK_SPEED
#define	LOW_FREQ_PLL_INCLUDE

// Select EHI Slave System among the followings.
	//#define	EHIS_TCC77X
	#define	EHIS_TCC75X

//#include "main.h"

#if defined(USE_V_ADDRESS) && defined(ANDROID_KERNEL)
#include <mach/reg_physical.h>
#elif defined(ANDROID_BOOT)
#include <platform/reg_physical.h>
#else
#if defined(TCC92XX)
#include "TCC92xx_Physical.h"
#include "TCC92xx_Structures.h"
#elif defined(TCC89XX)
	#if defined(_LINUX_)
	#include <TCC89x_Physical.h>
	#include <TCC89x_Structures.h>
	#elif defined(_WINCE_)
	#include "TCC89x_Physical.h"
	#include "TCC89x_Structures.h"
	#endif
#elif defined(TCC93XX)
	#if defined(_LINUX_)
	#include <TCC93x_Physical.h>
	#include <TCC93x_Structures.h>
	#elif defined(_WINCE_)
	#include "TCC93x_Physical.h"
	#include "TCC93x_Structures.h"
	#endif
#elif defined(TCC88XX)
	#if defined(_LINUX_)
	#include <TCC88x_Physical.h>
	#include <TCC88x_Structures.h>
	#elif defined(_WINCE_)
	#include "TCC88x_Physical.h"
	#include "TCC88x_Structures.h"
	#endif
#elif defined(TCC79XX)
#include "TCC79xx.h"
#elif defined(TCC801X)
#include "Config\MCU\TCC801x.h"
#else
#error "--  Not defined chip models --"
#endif
#endif

#ifdef TCC79XX
	#define	HwSDR_FIX		0x07282000
	#define	HwSDR_RFR		0
	#define	HwSDCFG_P0		HwSDCFG
	#define	HwSDCFG_P1		HwSDCFG
#elif defined(TCC92XX) || defined(TCC89XX)
	#define	HwSDR_FIX		0x07282000
	#define	HwSDR_RFR		0
	#define	HwSDCFG_P0		HwSDCFG
	#define	HwSDCFG_P1		HwSDCFG
#endif

#define	HwSDR_RBC		HwSDCFG_AM_RBC

#define	HwSDR_CL3		HwSDCFG_CL

// SDRAM BusWidth
#define	HwSDR_X16		Hw30
#define	HwSDR_X32		HwZERO

// SDRAM Size (Col, Row Address bus width)
#define	HwSDR_2MB		(HwSDCFG_CW8 + HwSDCFG_RW11)
#define	HwSDR_4MB		(HwSDCFG_CW8 + HwSDCFG_RW11)
#define	HwSDR_8MB		(HwSDCFG_CW8 + HwSDCFG_RW12)
#define	HwSDR_16MB		(HwSDCFG_CW9 + HwSDCFG_RW12)
#define	HwSDR_32MB		(HwSDCFG_CW9 + HwSDCFG_RW13)
#define	HwSDR_64MB		(HwSDCFG_CW10 + HwSDCFG_RW13)


#if (defined(TELECHIPS_SV) && !defined(USE_IO_DEBUG))
	//#define	USE_IO_DEBUG
#endif

#ifndef SET
	#define	SET			1
#endif
#ifndef CLR
	#define	CLR			0
#endif

#if defined(_LINUX_)
#ifndef _U32_
#define _U32_
	typedef unsigned int U32;
#endif
#ifndef _U16_
#define _U16_
	typedef unsigned short U16;
#endif
#ifndef _U8_
#define _U8_
	typedef unsigned char U8;
#endif
#ifndef WINVER
	#ifndef _BOOL_
    #define _BOOL_
		typedef unsigned int BOOL;
	#endif
#endif
#else 
#ifndef U32
	typedef unsigned int U32;
#endif
#ifndef U16
	typedef unsigned short U16;
#endif
#ifndef U8
	typedef unsigned char U8;
#endif

#ifndef WINVER
	#ifndef BOOL
		typedef unsigned int BOOL;
	#endif
#endif
#endif

#ifndef S32
	typedef signed int S32;
#endif
#ifndef S16
	typedef signed short S16;
#endif
#ifndef S8
	typedef signed char S8;
#endif

typedef	int (*ICallBack)(int num);

/* =============================
   General Bit Operator
   ============================= */
// Bit manipulation macro that is modifying its argument. (task type)
#ifndef BITSET
#define	BITSET(X, MASK)				( (X) |= (U32)(MASK) )
#endif
#ifndef BITSCLR
#define	BITSCLR(X, SMASK, CMASK)	( (X) = ((((U32)(X)) | ((U32)(SMASK))) & ~((U32)(CMASK))) )
#endif
#ifndef BITCSET
#define	BITCSET(X, CMASK, SMASK)	( (X) = ((((U32)(X)) & ~((U32)(CMASK))) | ((U32)(SMASK))) )
#endif
#ifndef BITCLR
#define	BITCLR(X, MASK)				( (X) &= ~((U32)(MASK)) )
#endif
#ifndef BITXOR
#define	BITXOR(X, MASK)				( (X) ^= (U32)(MASK) )
#endif

// Bit manipulation macro that is not modifying its argument. (function type)
#ifndef fBITSET
#define	fBITSET(X, MASK)			( (X) | (U32)(MASK) )
#endif
#ifndef fBITSCLR
#define	fBITSCLR(X, SMASK, CMASK)	( ((((U32)(X)) | ((U32)(SMASK))) & ~((U32)(CMASK))) )
#endif
#ifndef fBITCSET
#define	fBITCSET(X, CMASK, SMASK)	( ((((U32)(X)) & ~((U32)(CMASK))) | ((U32)(SMASK))) )
#endif
#ifndef fBITCLR
#define	fBITCLR(X, MASK)			( (X) & ~((U32)(MASK)) )
#endif
#ifndef fBITXOR
#define	fBITXOR(X, MASK)			( (X) ^ (U32)(MASK) )
#endif

#ifndef ISSET
#define	ISSET(X, MASK)				( (U32)(X) & ((U32)(MASK)) )
#endif
#ifndef IS
#define	IS(X, MASK)					( (U32)(X) & ((U32)(MASK)) )
#endif
#ifndef ISONE
#define	ISONE(X, MASK)				( (U32)(X) & ((U32)(MASK)) )
#endif

#ifndef ISALLONE
#define	ISALLONE(X, MASK)			( ((U32)(X) & ((U32)(MASK))) == ((U32)(MASK)) )
#endif

#ifndef ISCLR
#define	ISCLR(X, MASK)				(  !(((U32)(X)) & ((U32)(MASK))) )
#endif
#ifndef ISZERO
#define	ISZERO(X, MASK)				(  !(((U32)(X)) & ((U32)(MASK))) )
#endif
#ifndef ISNOT
#define	ISNOT(X, MASK)				(  !(((U32)(X)) & ((U32)(MASK))) )
#endif

#ifndef BYTE_OF
#define	BYTE_OF(X)					( *(volatile unsigned char *)(&(X)) )
#endif
#ifndef SHORT_OF
#define	SHORT_OF(X)					( *(volatile short *)(&(X)) )
#endif
#ifndef HWORD_OF
#define	HWORD_OF(X)					( *(volatile unsigned short *)(&(X)) )
#endif
#ifndef WORD_OF
#define	WORD_OF(X)					( *(volatile unsigned int *)(&(X)) )
#endif

#ifndef byte_of
#define	byte_of(X)					( *(volatile unsigned char *)((X)) )
#endif
#ifndef short_of
#define	short_of(X)					( *(volatile short *)((X)) )
#endif
#ifndef hword_of
#define	hword_of(X)					( *(volatile unsigned short *)((X)) )
#endif
#ifndef word_of
#define	word_of(X)					( *(volatile unsigned int *)((X)) )
#endif

#define	CkWaitPLLLOCK()				{ while (ISCLR(HwPLLMODE, Hw20)); }

#define	SBase(X)					Load$$ ## X ## $$Base
#define	DBase(X)					Image$$ ## X ## $$Base
#define	SSize(X)					Image$$ ## X ## $$Length
#define	ZIBase(X)					Image$$ ## X ## $$ZI$$Base
#define	ZISize(X)					Image$$ ## X ## $$ZI$$Length
#define	Region(X)					&SBase(X), &DBase(X), &SSize(X), &ZIBase(X), &ZISize(X)
#define	ExternRegion(X)				SBase(X), DBase(X), SSize(X), ZIBase(X), ZISize(X)


/************************************************************************
*	Clock Controller
************************************************************************/
typedef struct {
	unsigned			uFpll;
	unsigned char		P, M, S, dummy;
} sPLL;


enum {
	XIN_FREQ_120000,
	XIN_FREQ_480000
};

enum {
	IO_CKC_Ftimerx	= 0,
	IO_CKC_Ftimert,
	IO_CKC_Ftimerz,
	IO_CKC_Flcd0,
	IO_CKC_Flcd1,
	IO_CKC_Flcdsi,		// 5
	IO_CKC_Fcifmc,
	IO_CKC_Fcifsc,
	IO_CKC_Fout0,
	IO_CKC_Fout1,
	IO_CKC_Fhdmi,		// 10
	IO_CKC_Fdummy,
	IO_CKC_Fsdmmc0,
	IO_CKC_Fmstick,
	IO_CKC_Fi2c,
	IO_CKC_Fuart0,		// 15
	IO_CKC_Fuart1,
	IO_CKC_Fuart2,
	IO_CKC_Fuart3,
	IO_CKC_Fuart4,
	IO_CKC_Fuart5,		// 20
	IO_CKC_Fgpsb0,
	IO_CKC_Fgpsb1,
	IO_CKC_Fgpsb2,
	IO_CKC_Fgpsb3,
	IO_CKC_Fgpsb4,		// 25
	IO_CKC_Fgpsb5,
	IO_CKC_Fadc,
	IO_CKC_Fspdif,
	IO_CKC_Fehi0,
	IO_CKC_Fehi1,		// 30
	IO_CKC_Faud,
	IO_CKC_Fdummy1,
	IO_CKC_Fdummy2,
	IO_CKC_Fsdmmc1,
	IO_CKC_Fdummy3,		// 35
	IO_CKC_Fdai,

	IO_CKC_Flast
};


/**********************************************************
*	void IO_CKC_WaitPLL(void);
*	Input		:
*	Return		:
*	Description	: Loop waiting PLL locked.
**********************************************************/
#define	IO_CKC_WaitPLL()			{ int i; for (i=0; i<0x1000; i++); }


/**********************************************************
*	void IO_CKC_EnableBUS(unsigned X);
*	Input		: X = Bitmap dedicated to each peripheral
*	Return		:
*	Description	: Enable(1) the bus clock fed to each peripheral.
*				  The other bus clocks are not influenced.
**********************************************************/
#define	IO_CKC_EnableBUS(X)			(BITSET(HwIOBUSCFG->HCLKEN0, (X)))
#define	IO_CKC_EnableBUS1(X)		(BITSET(HwIOBUSCFG->HCLKEN1, (X)))

/**********************************************************
*	Derivatives of IO_CKC_EnableBUS() function.
*	Input		:
*	Return		:
*	Description	: Enable the bus clock fed to each peripheral.
**********************************************************/
#define IO_CKC_EnableBUS_USB()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_USB))
#define IO_CKC_EnableBUS_IDE()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_IDE))
#define IO_CKC_EnableBUS_DMA()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_DMA))
#define IO_CKC_EnableBUS_SDC()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_SD))
#define IO_CKC_EnableBUS_MS()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_MS))
#define IO_CKC_EnableBUS_I2C()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_I2C))
#define IO_CKC_EnableBUS_NFC()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_NFC))
#define IO_CKC_EnableBUS_EHI0()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_EHI0))
#define IO_CKC_EnableBUS_EHI1()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_EHI1))
#define IO_CKC_EnableBUS_UART0()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_UART0))
#define IO_CKC_EnableBUS_UART1()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_UART1))
#define IO_CKC_EnableBUS_UART2()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_UART2))
#define IO_CKC_EnableBUS_UART3()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_UART3))
#define IO_CKC_EnableBUS_UART4()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_UART4))
#define IO_CKC_EnableBUS_UART5()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_UART5))
#define IO_CKC_EnableBUS_GPSB0()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPSB0))
#define IO_CKC_EnableBUS_GPSB1()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPSB1))
#define IO_CKC_EnableBUS_GPSB2()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPSB2))
#define IO_CKC_EnableBUS_GPSB3()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPSB3))
#define IO_CKC_EnableBUS_GPSB4()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPSB4))
#define IO_CKC_EnableBUS_GPSB5()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPSB5))
#define IO_CKC_EnableBUS_DAI()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_DAI))
#define IO_CKC_EnableBUS_ECC()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_ECC))
#define IO_CKC_EnableBUS_SPDIF()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_SPDIF))
#define IO_CKC_EnableBUS_RTC()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_RTC))
#define IO_CKC_EnableBUS_TSADC()	(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_TSADC))
#define IO_CKC_EnableBUS_GPS()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPS))
#define IO_CKC_EnableBUS_ADMA()		(BITSET(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_ADMA))
#define IO_CKC_EnableBUS_MPE()		(BITSET(HwIOBUSCFG->HCLKEN1, HwIOBUSCFG_MPE))
#define IO_CKC_EnableBUS_TSIF()		(BITSET(HwIOBUSCFG->HCLKEN1, HwIOBUSCFG_TSIF))
#define IO_CKC_EnableBUS_SRAM()		(BITSET(HwIOBUSCFG->HCLKEN1, HwIOBUSCFG_SRAM))


/**********************************************************
*	void IO_CKC_DisableBUS(unsigned X);
*	Input		: X = Bitmap dedicated to each peripheral
*	Return		:
*	Description	: Disable(1) the bus clock fed to each peripheral.
*				  The other bus clocks are not influenced.
**********************************************************/
#define	IO_CKC_DisableBUS(X)		(BITCLR(HwIOBUSCFG->HCLKEN0, X))
#define	IO_CKC_DisableBUS1(X)		(BITCLR(HwIOBUSCFG->HCLKEN1, (X)))

/**********************************************************
*	Derivatives of IO_CKC_DisableBUS() function.
*	Input		:
*	Return		:
*	Description	: Disable the bus clock fed to each peripheral.
**********************************************************/
#define IO_CKC_DisableBUS_USB()		(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_USB))
#define IO_CKC_DisableBUS_IDE()		(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_IDE))
#define IO_CKC_DisableBUS_DMA()		(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_DMA))
#define IO_CKC_DisableBUS_SDC()		(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_SD))
#define IO_CKC_DisableBUS_MS()		(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_MS))
#define IO_CKC_DisableBUS_I2C()		(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_I2C))
#define IO_CKC_DisableBUS_NFC()		(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_NFC))
#define IO_CKC_DisableBUS_EHI0()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_EHI0))
#define IO_CKC_DisableBUS_EHI1()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_EHI1))
#define IO_CKC_DisableBUS_UART0()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_UART0))
#define IO_CKC_DisableBUS_UART1()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_UART1))
#define IO_CKC_DisableBUS_UART2()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_UART2))
#define IO_CKC_DisableBUS_UART3()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_UART3))
#define IO_CKC_DisableBUS_UART4()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_UART4))
#define IO_CKC_DisableBUS_UART5()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_UART5))
#define IO_CKC_DisableBUS_GPSB0()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPSB0))
#define IO_CKC_DisableBUS_GPSB1()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPSB1))
#define IO_CKC_DisableBUS_GPSB2()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPSB2))
#define IO_CKC_DisableBUS_GPSB3()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPSB3))
#define IO_CKC_DisableBUS_GPSB4()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPSB4))
#define IO_CKC_DisableBUS_GPSB5()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPSB5))
#define IO_CKC_DisableBUS_DAI()		(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_DAI))
#define IO_CKC_DisableBUS_ECC()		(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_ECC))
#define IO_CKC_DisableBUS_SPDIF()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_SPDIF))
#define IO_CKC_DisableBUS_RTC()		(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_RTC))
#define IO_CKC_DisableBUS_TSADC()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_TSADC))
#define IO_CKC_DisableBUS_GPS()		(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_GPS))
#define IO_CKC_DisableBUS_ADMA()	(BITCLR(HwIOBUSCFG->HCLKEN0, HwIOBUSCFG_ADMA))
#define IO_CKC_DisableBUS_MPE()		(BITCLR(HwIOBUSCFG->HCLKEN1, HwIOBUSCFG_MPE))
#define IO_CKC_DisableBUS_TSIF()	(BITCLR(HwIOBUSCFG->HCLKEN1, HwIOBUSCFG_TSIF))
#define IO_CKC_DisableBUS_SRAM()	(BITCLR(HwIOBUSCFG->HCLKEN1, HwIOBUSCFG_SRAM))

// IOBUS AHB 0
#define IO_CKC_BUS_USB				HwIOBUSCFG_USB
#define IO_CKC_BUS_IDE				HwIOBUSCFG_IDE
#define IO_CKC_BUS_DMA				HwIOBUSCFG_DMA
#define IO_CKC_BUS_SD				HwIOBUSCFG_SD
#define IO_CKC_BUS_MS				HwIOBUSCFG_MS
#define IO_CKC_BUS_I2C				HwIOBUSCFG_I2C
#define IO_CKC_BUS_NFC				HwIOBUSCFG_NFC
#define IO_CKC_BUS_EHI0				HwIOBUSCFG_EHI0
#define IO_CKC_BUS_EHI1				HwIOBUSCFG_EHI1
#define IO_CKC_BUS_UART0			HwIOBUSCFG_UART0
#define IO_CKC_BUS_UART1			HwIOBUSCFG_UART1
#define IO_CKC_BUS_UART2			HwIOBUSCFG_UART2
#define IO_CKC_BUS_UART3			HwIOBUSCFG_UART3
#define IO_CKC_BUS_UART4			HwIOBUSCFG_UART4
#define IO_CKC_BUS_UART5			HwIOBUSCFG_UART5
#define IO_CKC_BUS_GPSB0			HwIOBUSCFG_GPSB0
#define IO_CKC_BUS_GPSB1			HwIOBUSCFG_GPSB1
#define IO_CKC_BUS_GPSB2			HwIOBUSCFG_GPSB2
#define IO_CKC_BUS_GPSB3			HwIOBUSCFG_GPSB3
#define IO_CKC_BUS_GPSB4			HwIOBUSCFG_GPSB4
#define IO_CKC_BUS_GPSB5			HwIOBUSCFG_GPSB5
#define IO_CKC_BUS_DAI				HwIOBUSCFG_DAI
#define IO_CKC_BUS_ECC				HwIOBUSCFG_ECC
#define IO_CKC_BUS_SPDIF			HwIOBUSCFG_SPDIF
#define IO_CKC_BUS_RTC				HwIOBUSCFG_RTC
#define IO_CKC_BUS_TSADC			HwIOBUSCFG_TSADC
#define IO_CKC_BUS_GPS				HwIOBUSCFG_GPS
#define IO_CKC_BUS_ADMA				HwIOBUSCFG_ADMA

// IOBUS AHB 1
#define IO_CKC_BUS_MPE				HwIOBUSCFG_MPE
#define IO_CKC_BUS_TSIF				HwIOBUSCFG_TSIF
#define IO_CKC_BUS_SRAM				HwIOBUSCFG_SRAM


/**********************************************************
*	void IO_CKC_SWRST(unsigned X);
*	Input		: X = Bitmap dedicated to each peripheral
*	Return		:
*	Description	: Make a reset signal for each peripheral.
*				  Set a corresponding bit field to make a reset signal to each peripheral.
**********************************************************/
#if defined(TCC92XX) || defined(TCC89XX)
#else  /* TCC92XX */
#define	IO_CKC_SWRST(X)				(HwSWRESET	= (X))
#endif /* TCC92XX */

#if !defined(_LINUX_) && !defined(TCC801X)
#pragma warning( push )
#pragma warning( disable: 4341 )		// enum --> unsigned longlong...X
#pragma warning( disable: 4309 )
#endif
enum {
	#if !defined(TCC801X)
	IO_CKC_TMRX		= Hw0,
	IO_CKC_TMRT		= Hw1,
	IO_CKC_TMRZ		= Hw2,
	IO_CKC_LCD0		= Hw3,
	IO_CKC_LCD1		= Hw4,
	IO_CKC_LCDSI	= Hw5,
	IO_CKC_CIFMC	= Hw6,
	IO_CKC_CIFSC	= Hw7,
	IO_CKC_OUT0		= Hw8,
	IO_CKC_OUT1		= Hw9,
	IO_CKC_HDMI		= Hw10,
	IO_CKC_SDMMC0	= Hw12,
	IO_CKC_MSTICK	= Hw13,
	IO_CKC_I2C		= Hw14,
	IO_CKC_UART0	= Hw15,
	IO_CKC_UART1	= Hw16,
	IO_CKC_UART2	= Hw17,
	IO_CKC_UART3	= Hw18,
	IO_CKC_UART4	= Hw19,
	IO_CKC_UART5	= Hw20,
	IO_CKC_GPSB0	= Hw21,
	IO_CKC_GPSB1	= Hw22,
	IO_CKC_GPSB2	= Hw23,
	IO_CKC_GPSB3	= Hw24,
	IO_CKC_GPSB4	= Hw25,
	IO_CKC_GPSB5	= Hw26,
	IO_CKC_ADC		= Hw27,		// PCK_YYY
	IO_CKC_SPDIF	= Hw28,		// PCK_YYY
	IO_CKC_EHI0		= Hw29,
	IO_CKC_EHI1		= Hw30,
	IO_CKC_AUD		= Hw31,		// PCK_YYY
	IO_CKC_SDMMC1	= Hw34,
	IO_CKC_DAI		= Hw36		// PCK_YYY
	#else
	IO_CKC_TMRX		= Hw0,
	IO_CKC_TMRT		= Hw1,
	IO_CKC_TMRZ		= Hw2,
	IO_CKC_LCD0		= Hw3,
	IO_CKC_LCD1		= Hw4,
	IO_CKC_LCDSI	= Hw5,
	IO_CKC_CIFMC	= Hw6,
	IO_CKC_CIFSC	= Hw7,
	IO_CKC_OUT0		= Hw8,
	IO_CKC_OUT1		= Hw9,
	IO_CKC_HDMI		= Hw10,
	IO_CKC_SDMMC0	= Hw12,
	IO_CKC_MSTICK	= Hw13,
	IO_CKC_I2C		= Hw14,
	IO_CKC_UART0	= Hw15,
	IO_CKC_UART1	= Hw16,
	IO_CKC_UART2	= Hw17,
	IO_CKC_UART3	= Hw18,
	IO_CKC_UART4	= Hw19,
	IO_CKC_UART5	= Hw20,
	IO_CKC_GPSB0	= Hw21,
	IO_CKC_GPSB1	= Hw22,
	IO_CKC_GPSB2	= Hw23,
	IO_CKC_GPSB3	= Hw24,
	IO_CKC_GPSB4	= Hw25,
	IO_CKC_GPSB5	= Hw26,
	IO_CKC_ADC		= Hw27,		// PCK_YYY
	IO_CKC_SPDIF	= Hw28,		// PCK_YYY
	IO_CKC_EHI0		= Hw29,
	IO_CKC_EHI1		= Hw30,
	IO_CKC_AUD		= Hw31		// PCK_YYY
	#endif
};
#if !defined(_LINUX_) && !defined(TCC801X)
#pragma warning( pop )
#endif

#define	IO_CKC_EnableClock(x)									\
{																\
	if ((x) & IO_CKC_TMRX)										\
		BITSET(HwCKC->PCLK_TCX, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_TMRT)										\
		BITSET(HwCKC->PCLK_TCT, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_TMRZ)					\
		BITSET(HwCKC->PCLK_TCZ, HwPCK_EN_EN);		\
	if ((x) & IO_CKC_LCD0)					\
		BITSET(HwCKC->PCLK_LCD0, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_LCD1)					\
		BITSET(HwCKC->PCLK_LCD1, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_LCDSI)					\
		BITSET(HwCKC->PCLK_LCDSI, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_CIFMC)					\
		BITSET(HwCKC->PCLK_CIFMC, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_CIFSC)					\
		BITSET(HwCKC->PCLK_CIFSC, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_OUT0)					\
		BITSET(HwCKC->PCLK_OUT0, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_OUT1)					\
		BITSET(HwCKC->PCLK_OUT1, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_HDMI)					\
		BITSET(HwCKC->PCLK_HDMI, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_SDMMC0)				\
		BITSET(HwCKC->PCLK_SDMMC0, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_MSTICK)				\
		BITSET(HwCKC->PCLK_MSTICK, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_I2C)					\
		BITSET(HwCKC->PCLK_I2C, HwPCK_EN_EN);		\
	if ((x) & IO_CKC_UART0)					\
		BITSET(HwCKC->PCLK_UART0, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_UART1)					\
		BITSET(HwCKC->PCLK_UART1, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_UART2)					\
		BITSET(HwCKC->PCLK_UART2, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_UART3)					\
		BITSET(HwCKC->PCLK_UART3, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_UART4)					\
		BITSET(HwCKC->PCLK_UART4, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_UART5)					\
		BITSET(HwCKC->PCLK_UART5, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_GPSB0)					\
		BITSET(HwCKC->PCLK_GPSB0, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_GPSB1)					\
		BITSET(HwCKC->PCLK_GPSB1, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_GPSB2)					\
		BITSET(HwCKC->PCLK_GPSB2, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_GPSB3)					\
		BITSET(HwCKC->PCLK_GPSB3, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_GPSB4)					\
		BITSET(HwCKC->PCLK_GPSB4, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_GPSB5)					\
		BITSET(HwCKC->PCLK_GPSB5, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_ADC)					\
		BITSET(HwCKC->PCLK_ADC, HwPCK_EN_EN);		\
	if ((x) & IO_CKC_SPDIF)					\
		BITSET(HwCKC->PCLK_SPDIF, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_EHI0)					\
		BITSET(HwCKC->PCLK_EHI0, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_EHI1)					\
		BITSET(HwCKC->PCLK_EHI1, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_AUD)					\
		BITSET(HwCKC->PCLK_AUD, HwPCK_EN_EN);		\
	if ((x) & IO_CKC_SDMMC1)				\
		BITSET(HwCKC->PCLK_SDMMC1, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_DAI)					\
		BITSET(HwCKC->PCLK_DAI, HwPCK_EN_EN);		\
}

#define	IO_CKC_DisableClock(x)				\
{											\
	if ((x) & IO_CKC_TMRX)					\
		BITCLR(HwCKC->PCLK_TCX, HwPCK_EN_EN);		\
	if ((x) & IO_CKC_TMRT)					\
		BITCLR(HwCKC->PCLK_TCT, HwPCK_EN_EN);		\
	if ((x) & IO_CKC_TMRZ)					\
		BITCLR(HwCKC->PCLK_TCZ, HwPCK_EN_EN);		\
	if ((x) & IO_CKC_LCD0)					\
		BITCLR(HwCKC->PCLK_LCD0, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_LCD1)					\
		BITCLR(HwCKC->PCLK_LCD1, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_LCDSI)					\
		BITCLR(HwCKC->PCLK_LCDSI, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_CIFMC)					\
		BITCLR(HwCKC->PCLK_CIFMC, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_CIFSC)					\
		BITCLR(HwCKC->PCLK_CIFSC, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_OUT0)					\
		BITCLR(HwCKC->PCLK_OUT0, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_OUT1)					\
		BITCLR(HwCKC->PCLK_OUT1, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_HDMI)					\
		BITCLR(HwCKC->PCLK_HDMI, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_SDMMC0)				\
		BITCLR(HwCKC->PCLK_SDMMC0, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_MSTICK)				\
		BITCLR(HwCKC->PCLK_MSTICK, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_I2C)					\
		BITCLR(HwCKC->PCLK_I2C, HwPCK_EN_EN);		\
	if ((x) & IO_CKC_UART0)					\
		BITCLR(HwCKC->PCLK_UART0, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_UART1)					\
		BITCLR(HwCKC->PCLK_UART1, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_UART2)					\
		BITCLR(HwCKC->PCLK_UART2, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_UART3)					\
		BITCLR(HwCKC->PCLK_UART3, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_UART4)					\
		BITCLR(HwCKC->PCLK_UART4, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_UART5)					\
		BITCLR(HwCKC->PCLK_UART5, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_GPSB0)					\
		BITCLR(HwCKC->PCLK_GPSB0, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_GPSB1)					\
		BITCLR(HwCKC->PCLK_GPSB1, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_GPSB2)					\
		BITCLR(HwCKC->PCLK_GPSB2, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_GPSB3)					\
		BITCLR(HwCKC->PCLK_GPSB3, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_GPSB4)					\
		BITCLR(HwCKC->PCLK_GPSB4, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_GPSB5)					\
		BITCLR(HwCKC->PCLK_GPSB5, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_ADC)					\
		BITCLR(HwCKC->PCLK_ADC, HwPCK_EN_EN);		\
	if ((x) & IO_CKC_SPDIF)					\
		BITCLR(HwCKC->PCLK_SPDIF, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_EHI0)					\
		BITCLR(HwCKC->PCLK_EHI0, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_EHI1)					\
		BITCLR(HwCKC->PCLK_EHI1, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_AUD)					\
		BITCLR(HwCKC->PCLK_AUD, HwPCK_EN_EN);		\
	if ((x) & IO_CKC_SDMMC1)				\
		BITCLR(HwCKC->PCLK_SDMMC1, HwPCK_EN_EN);	\
	if ((x) & IO_CKC_DAI)					\
		BITCLR(HwCKC->PCLK_DAI, HwPCK_EN_EN);		\
}


/************************************************************************
*	Function Declaration (Clock Controller)
************************************************************************/
/**********************************************************
*	void IO_CKC_EnterStandbyInt(unsigned uMode)
*
*	Input		: uMode = wake-up event selection
*				     0 : using XIN clock (that means program stops here until wake-up event is occurred.)
*					In this mode, only external interrupt can wake up system. but may be unstable while waking up.
*					NOT RECOMMENDED.
*				     non-zero : using XTIN clock.
*					In this mode, program continues but polling predefined wake-up interrupt.
*					the wake-up interrupt can be defined by this value of 'uMode'
*					this represents bitmap of wake-up interrupt sources.
*	Return		:
*	Description	: Enter Standby Mode (All clocks except XTIN are stopped.)
*				  This function is located in IO_LIB_INT area and
*				  it should be located in the internal SRAM.
*********************************************************/
void IO_CKC_EnterStandbyInt(unsigned uMode);

enum
{
	SPEED_MODE_0	= 0,
	#ifdef SPEED_MODE_INCLUDE
		SPEED_MODE_1,
		SPEED_MODE_2,
		SPEED_MODE_3,
	#endif
	MAX_SPEED_MODE
};

typedef struct {
	unsigned	uFmaxpll;
	unsigned	uFmaxcpu;
	unsigned	uFmaxbus;
	unsigned	uFpll[3];		// for 32KHz, 44.1KHz, 48KHz

	/*for chaning max clock by speed mode
	*/
	unsigned        uHighpll; //unit : 100hz
	unsigned short	uHighcpu; //unit : Mhz
	unsigned short	uHighbus; //unit : Mhz	
} sIO_CKC_SPD_MODE;

/**********************************************************
*	void IO_CKC_InitVariable(int iFmax, int iHmax);
*
*	Input		: iFmax = Maximum frequency of CPU clock (100Hz unit)
*				  iHmax = Maximum frequency of BUS clock (100Hz unit)
*	Return		:
*	Description	: Initialize Global Variables for Clock Driver
**********************************************************/
#if defined(TCC79XX) || defined(TCC92XX) || defined(TCC89XX)
	#define	IO_CKC_Fmaxcpu		PLL_FREQ
	#define	IO_CKC_Fmaxbus		(PLL_FREQ/2)

	#define	IO_CKC_Fxin			120000

	#define	IO_CKC_Fpll_32KHz	PLL_FREQ
	#define	IO_CKC_Fpll_44KHz	PLL_FREQ
	#define	IO_CKC_Fpll_48KHz	PLL_FREQ
#endif

/**********************************************************
*	void IO_CKC_InitDRV(void);
*
*	Input		: 
*	Output		:
*	Return		:
*
*	Description	: Initialize Global Variables for Clock Driver
**********************************************************/
void IO_CKC_InitDRV(void);

/**********************************************************
*	void IO_CKC_DisablePLL(unsigned uCH)
*
*	Input		: uCH = Channel of PLL (0 or 1)
*	Output		:
*	Return		:
*
*	Description	: 
*
**********************************************************/
void IO_CKC_DisablePLL(unsigned uCH);


/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_CKC_SelectSpeedMode(unsigned uSpeedMode);
*  
*  DESCRIPTION : Select System Speed Mode (Supported only by pull-up the supply voltage for CORE)
*  
*  INPUT:
*			uSpeedMode = Index of Speed Mode. (Refer sIO_CKC_SpeedMode[] Table)
*  
*  OUTPUT:	int - Return Type
*  			= -1 : Requested Speed Mode is not supported.
*			= 0 : Mode change successful.
*  
*  REMARK:	created on 2007/6/28    22:31:46
**************************************************************************/
int IO_CKC_SelectSpeedMode(unsigned uSpeedMode);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned IO_CKC_CalcBUS4CYCLE(unsigned uNewBusFreq);
*  
*  DESCRIPTION : Map real bus clock frequency to reference bus clock frequency
*			Reference bus clock frequency should be always higher or equal than real bus clock
*			to insert marginal cycles for stability.
*  
*  INPUT:
*			uNewBusFreq	= Real bus frequency
*  
*  OUTPUT:	unsigned - Return Type
*  			= Reference bus frequency
*  
**************************************************************************/
unsigned IO_CKC_CalcBUS4CYCLE(unsigned uNewBusFreq);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned IO_CKC_SetCYCLE(unsigned uNewBusFreq, unsigned uPosition);
*  
*  DESCRIPTION : Set Cycle Parameter for Peripherals which use Bus Clock.
*  
*  INPUT:
*			uNewBusFreq	= New Bus Clock Frequency (100 Hz unit)
*			uPosition	= Indicate position of calling this function
*					  1 : Before changing Bus Clock, 2 : After changing Bus Clock
*  
*  OUTPUT:	unsigned - Return Type
*  			= Old Bus Clock Frequency 
*			  (not real bus clock but reference bus clock for calculating cycle parameters)
*  
**************************************************************************/
unsigned IO_CKC_SetCYCLE(unsigned uNewBusFreq, unsigned uPosition);

/**********************************************************
*      void IO_CKC_AdjustClock(int Fmax, int Vmax, int Hmax, int Fmin, int Vmin, int Hmin, int iFclk, int iVclk, int iHclk);
*
*	Description	: Increasing/Decreasing System Clock (CPU, VCORE, BUS)
*				  (iFclk, iVclk, iHclk) means division/multiplication factor (not frequency)
*
*  INPUT:
*			Fmax	= Maximum Frequency for CPU clock
*			Vmax	= Maximum Frequency for VCORE clock
*			Hmax	= Maximum Frequency for BUS clock
*			Fmin	= Minimum Frequency for CPU clock
*			Vmin	= Minimum Frequency for VCORE clock
*			Hmin	= Minimum Frequency for BUS clock
*			iFclk	= CPU clock delta index (1~15 for increasing clock, -1~-15 for decreasing clock, 0 for holding)
*			iVclk	= VCore clock delta index (1~15 for increasing clock, -1~-15 for decreasing clock, 0 for holding)
*			iHclk	= BUS clock delta index (1~31 for increasing clock, -1~-31 for decreasing clock, 0 for holding)
*
*	Assumption	: Source clock (XIN or PLL) must be alive.
*	Output		:
*	Return		:
*
**********************************************************/
void IO_CKC_AdjustClock(int Fmax, int Vmax, int Hmax, int Fmin, int Vmin, int Hmin, int iFclk, int iVclk, int iHclk);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned IO_CKC_GetCurrentBUSClock4Cycle(void);
*  
*  DESCRIPTION : Returns current reference bus clock frequency
*				All the peripherals (like NAND, NOR, HDD, etc.) use bus clock 
*				should refer this value to calculate cycle parameters.
*  
*  INPUT:
*			None
*  
*  OUTPUT:	unsigned - Return Type
*  			= current reference bus clock frequency
*  
**************************************************************************/
unsigned IO_CKC_GetCurrentBUSClock4Cycle(void);

/**********************************************************
*	void IO_CKC_InitPLL(unsigned uCH, unsigned uPLL);
*
*	Input		: uPLL = target frequency of PLL (100Hz unit)
*	Return		:
*	Description	: call IO_CKC_SetPLL() according to frequency value
*				  currently supported frequency. (default = 240MHz)
*					240MHz, 120MHz, 248.5714MHz, 203.1428MHz
*					221.1428MHz, 196.5MHz
*
*				  After PLL set, it is tryed to maintain the frequencies of all clocks.
*				  If certain clock is not possible to be maintained,
*					the clock frequency is set as close as previous one.
**********************************************************/
void IO_CKC_InitPLL(unsigned uCH, unsigned uPLL);

/**********************************************************
*	void IO_CKC_SetPLL(unsigned uCH, int iP, int iM, int iS);
*
*	Input		: iP, iM, iS
*	Return		:
*	Description	: Set PLL according to PMS value, and wait until PLL is stable.
*					Fpll = Fxin * (iM + 8) / ((iP + 2) * 2^iS)
*				  (The combination of (P, M, S) should be confirmed before used)
*
*				  After PLL set, It is tryed to maintain the frequencies of all clocks.
*				  If certain clock is not possible to be maintained,
*					the clock frequency is set as close as previous one.
**********************************************************/
void IO_CKC_SetPLL(unsigned uCH, int iP, int iM, int iS);

/**********************************************************
*	void IO_CKC_SetClock(unsigned uPLLCH, unsigned uCKCCH, int iFclk)
*
*	Input		: uPLLCH = PLL channel index
*				  uCKCCH = Clock channel index
*				  iFclk = Freq. of uCKCCH Unit., 100Hz unit
*	Assumption	: Source clock (XIN or PLL) must be alive.
*				  (If XTIN is currently used, make sure XIN or PLL alive before calling this function)
*	Return		:
*	Description	: Set System Clock
*				  If target frequency exceeds its own max frequency (gCKC_Fmax]),
*				  the uIO_CKC_error flag is incremented, and system clock is not changed.
*				  If the frequency requested can not be set exactly,
*					this function set the frequency as close as possible.
*					(refer to the datasheet for possible system clock relationship.)
*				  The system clock source is selected according to the following rule.
*					1) if (Fclk >= Fpll) PLL is selected.
*					2) if (Fclk > Fxin) PLL divider is selected.
*					3) if (Fclk == Fxin) XIN is selected.
*					4) if (Fclk > Fxtin) XIN divider is selected.
*					5) if (Fclk == Fxtin) XTIN is selected.
*					6) other case, XTIN divider is selected.
**********************************************************/
void IO_CKC_SetClock(unsigned uPLLCH, unsigned uCKCCH, int iFclk);

/**********************************************************
*	void void IO_CKC_SetClockMul(unsigned uPLLCH, unsigned uCKCCH, int iSdiv, int iFdiv, int iSrc)
*
*	Input		: uPLLCH = PLL channel index
*				  uCKCCH = Clock channel index
*				  iSdiv = System clock division (= Fpll / Fsys)
*				  iFdiv = Fsys / Fclk (can be one of 2~32)
*				  iSrc = Fsys clock source selection
*	Assumption	: Source clock (XIN/XTIN/PLL) must be alive.
*	Output		:
*	Return		:
*
*	Description	: Set Clock.
**********************************************************/
void IO_CKC_SetClockMul(unsigned uPLLCH, unsigned uCKCCH, int iSdiv, int iFdiv, int iSrc);

/**********************************************************
*	unsigned IO_CKC_GetMinimumBusClock(unsigned uLCTRL);
*
*	Input		: LCDC LCTRL value ( register or variable )
*	Assumption	: 
*	Output		:
*	Return		: minimum bus clock frequency (100 Hz unit)
*
*	Description	: Get minimum bus clock frequency.
**********************************************************/
unsigned IO_CKC_GetMinimumBusClock(unsigned uLCTRL);

/**********************************************************
*	void IO_CKC_SetClockDiv(int iFclk, int iHclk, int iSrc);
*
*	Input		: iFclk = Division Factor for CPU clock (acquired by Fsrc / Fcpu)
*				  iHclk = Division Factor for BUS clock (acquired by Fcpu / Fbus)
*				  iSrc = Clock source definition (same as CKSEL[2:0] of HwCLKCTRL register)
*	Assumption	: Source clock (XIN/PLL/XTIN) must be alive.
*				  (If XTIN is currently used, make sure XIN or PLL alive before calling this function)
*	Return		:
*	Description	: Set System Clock (CPU, BUS) as follows.
*				  Frequency of CPU clock (Fcpu) = Fsrc / iFclk. (Fsrc = Frequency of source clock selected by iSrc parameter)
*				  Frequency of BUS clock (Fbus) = Fcpu / iHclk.
**********************************************************/
#ifdef CPU_CLOCK_DIV_USED
void IO_CKC_SetClockDiv(int iFdiv, int iHdiv, int iCCKdiv, int iSrc);
#else
void IO_CKC_SetClockDiv(int iFdiv, int iHdiv, int iSrc);
#endif

/**********************************************************
*	unsigned IO_CKC_GetCurrentBUSClock(void);
*
*	Input		:
*	Return		: Frequency of Current BUS Clock (100Hz unit)
*	Description	: Return the current bus clock frequency
**********************************************************/
unsigned IO_CKC_GetCurrentBUSClock(void);

/**********************************************************
*	void IO_CKC_EnterHalt(unsigned uSDEN);
*
*	Input		: uSDEN = option for SDRAM control
*					0 = don't touch sdram
*					1 = control SDRAM, (enter self-refresh mode during halt)
*	Return		:
*	Description	: Enter Halt Mode
*				  In Halt mode, only CPU halts until interrupt request occurred.
************************************************************/
void IO_CKC_EnterHalt(unsigned uSDEN);
void IO_CKC_EnterHalt_Main(void);
void IO_CKC_EnterHalt_End(void);

/**********************************************************
*	void IO_CKC_SetRefreshClock(int iFreq);
*
*	Input		: iFreq = Frequency of Refresh Clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Return		:
*	Description	: Set Refresh clock frequency and Enable refresh clock
*				  It is assumed that the BUS clock freq is larger than half of XIN frequency.
*				  The refresh clock is implicitely driven by XIN clock.
***********************************************************/
void IO_CKC_SetRefreshClock(int iFreq);

/**********************************************************
*	void IO_CKC_SetUSBHostClock(void);
*
*	Input		:
*	Return		:
*	Description	: Enable USB Host Clock, it is fixed to use XIN as USB Host clock
***********************************************************/
void IO_CKC_SetUSBHostClock(void);

/**********************************************************
*	void IO_CKC_SetI2CClock(int iFreq);
*
*	Input		: iFreq = Frequency of I2C Main Clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Return		:
*	Description	: Set frequency and Enable I2C main Clock
*				  The real I2C clock (SCL) frequency is determined by prescale register of I2C block.
*				  The I2C main clock is implicitely driven by XIN clock.
***********************************************************/
void IO_CKC_SetI2CClock(int iFreq);

/**********************************************************
*	void IO_CKC_SetMSCClock(int iFreq);
*
*	Input		: iFreq = Frequency of MemoryStic Controller clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Return		:
*	Description	: Set frequency and Enable Memory Stick Controller Clock
*				  The memory stic controller clock is implicitely driven by PLL clock.
***********************************************************/
void IO_CKC_SetMSCClock(int iFreq);

/**********************************************************
*	void IO_CKC_SetUA0Clock(int iFreq);
*	void IO_CKC_SetUA1Clock(int iFreq);
*	void IO_CKC_SetUA2Clock(int iFreq);
*	void IO_CKC_SetUA3Clock(int iFreq);
*
*	Input		: iFreq = Frequency of UART clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Return		:
*	Description	: Set frequency and Enable UART Clock
*				  The baud rate of UART is determined by DL register of UART block
*				  The UART clock is implicitely driven by PLL clock.
***********************************************************/
void IO_CKC_SetUartClock(int iCH, int iFreq);
void IO_CKC_SetUA0Clock(int iFreq);
void IO_CKC_SetUA1Clock(int iFreq);
void IO_CKC_SetUA2Clock(int iFreq);
void IO_CKC_SetUA3Clock(int iFreq);

/**********************************************************
*	void IO_CKC_SetTimerTClock(int iFreq);
*
*	Input		: iFreq = Frequency of Timer-T clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Return		:
*	Description	: Set frequency and Enable Timer-T Clock
*				  The timer-T clock manages 6 timer/counters
*				  (refer to datasheet)
*				  The Timer-T clock is implicitely driven by XIN clock.
***********************************************************/
void IO_CKC_SetTimerTClock(int iFreq);

/**********************************************************
*	void IO_CKC_SetGSIOClock(int iFreq);
*
*	Input		: iFreq = Frequency of GSIO base clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Return		:
*	Description	: Set frequency and Enable GSIO base clock
*				  The real GSIO clock (SCK) is determined by speed control field of GSCRx register.
*				  (refer to datasheet)
***********************************************************/
void IO_CKC_SetGSIOClock(int iFreq);


/**********************************************************
*	void IO_TC32_SetTIMER(unsigned uCTRL, unsigned uFreq, unsigned uLOADVAL, unsigned uCMP0, unsigned uCMP1);
*
*	Input		: uCTRL = TC32 Control Register (HwTC32EN) value except Pre-scale value.
*				  uICTRL = TC32 Interrupt Control Register (HwTC32IRQ) value.
*				  uFreq = TC32 counting frequency to set up pre-scale value. (TC32 clock should be set ahead)
*				  uLOADVAL = load value (HwTC32LDV)
*				  uCMP0 = Match value 0 (HwTC32CMP0).
*				  uCMP1 = Match value 1 (HwTC32CMP1).
*	Return		:
*	Description	: Set TC32 timer (timer is enabled if uFreq > 0, or timer is disabled)
*				  If uFreq < Freq(TimerZ clock), then TC32 is counted by TimerZ clock directly.
**********************************************************/
void IO_TC32_SetTIMER(unsigned uCTRL, unsigned uICTRL, unsigned uFreq, unsigned uLOADVAL, unsigned uCMP0, unsigned uCMP1);

/**********************************************************
*	void IO_CKC_SetTimerZClock(int iFreq);
*
*	Input		: iFreq = Frequency of Timer-Z clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Return		:
*	Description	: Set frequency and Enable Timer-Z Clock
*				  The timer-Z clock manages TC32 counter block
*				  (refer to datasheet)
*				  The Timer-Z clock is implicitely driven by XTIN clock.
***********************************************************/
void IO_CKC_SetTimerZClock(int iFreq);

/**********************************************************
*	void IO_CKC_SetTimerXClock(int iFreq);
*
*	Input		: iFreq = Frequency of Timer-X clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Return		:
*	Description	: Set frequency and Enable Timer-X Clock
*				  The timer-X clock manages watchdog counter block
*				  (refer to datasheet)
*				  The Timer-X clock is implicitely driven by XIN clock.
***********************************************************/
void IO_CKC_SetTimerXClock(int iFreq);

/**********************************************************
*	void IO_CKC_SetDAIClock(int iFreq);
*
*	Input		: iFreq = Frequency of DAI Main Clock (100Hz unit)
*				  Clock disabled if iFreq == 0.
*	Return		:
*	Description	: Set frequency and Enable DAI Clock
*				  The DAI clock should be set appropriately according to audio sampling rate.
***********************************************************/
void IO_CKC_SetDAIClock(int iFreq);

/**********************************************************
*	unsigned IO_CKC_GetPLLClock(int iFreq);
*
*	Input		: iFreq = Frequency of Audio Sampling Clock (Hz unit)
*	Return		: Frequency of PLL (100Hz unit)
*	Description	: Get frequency of PLL that is capable to generate iFreq.
*				  Unsupported audio sampling rate is regarded as 44100 Hz.
*				Audio Sampling Rate	PLL frequency
*				=========================
*				44100 Hz	& related		203.1428 MHz
*				48000 Hz	& related		221.1428 MHz
*				32000 Hz	& related		196.5000 MHz
***********************************************************/
unsigned IO_CKC_GetPLLClock(int iFreq);

/**********************************************************
*	unsigned IO_CKC_GetDAIClock(int iFreq, int iOSR);
*
*	Input		: iFreq = Frequency of Audio Sampling Clock (Hz unit)
*				  iOSR = Over Sampling Rate. (ex. 256, 384)
*	Return		: Frequency of DAI Clock (100Hz unit)
*	Description	: Get frequency of DAI clock that is fit to iFreq audio frequency.
***********************************************************/
unsigned IO_CKC_GetDAIClock(int iFreq, int iOSR);

/**********************************************************
*	void IO_CKC_SetADCClock(int iFreq);
*
*	Input		: iFreq = Frequency of ADC Clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Return		:
*	Description	: Set frequency and Enable ADC Clock
*				  The ADC clock is implicitely driven by XIN clock.
***********************************************************/
void IO_CKC_SetADCClock(int iFreq);

/**********************************************************
*	void IO_CKC_SetSDMMCClock(int iCH, int iFreq);
*
*	Input		: iCH = SD/MMC channel index
*				  iFreq = Frequency of SDMMC Clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Output		:
*	Return		:
*
*	Description	: Set frequency and Enable Clock
***********************************************************/
//void IO_CKC_SetSDMMCClock(int iCH, int iFreq);

/**********************************************************
*	void IO_CKC_SetLCDClock(int iCH, int iFreq)
*
*	Input		: iCH = LCD channel index
*				  iFreq = Frequency of LCD Clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Output		:
*	Return		:
*
*	Description	: Set frequency and Enable Clock
***********************************************************/
void IO_CKC_SetLCDClock(int iCH, int iFreq);

/**********************************************************
*	void IO_CKC_SetEHIClock(int iFreq, int iCS);
*
*	Input		: iFreq = Frequency of EHI Clock, 100Hz unit
*				          Clock disabled if iFreq == 0.
*                 iCS = EHI Channel ( 0 or 1 )
*	Output		:
*	Return		:
*
*	Description	: Set frequency and Enable Clock
***********************************************************/
void IO_CKC_SetEHIClock(int iFreq, int iCS);


#ifdef CAMERA_INCLUDE
/**********************************************************
*	void IO_CKC_SetCAMClock(int iFreq);
*
*	Input		: iFreq = Frequency of CAM Clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Output		:
*	Return		:
*
*	Description	: Set frequency and Enable Clock
***********************************************************/
void IO_CKC_SetCAMClock(int iFreq);

/**********************************************************
*	void IO_CKC_SetCAMScalerClock(int iFreq);
*
*	Input		: iFreq = Frequency of CAM Scaler Clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Output		:
*	Return		:
*
*	Description	: Set frequency and Enable Clock
***********************************************************/
void IO_CKC_SetCAMScalerClock(int iFreq);
#endif

/**********************************************************
*	void IO_CKC_SetSPDIFClock(int iFreq);
*
*	Input		: iFreq = Frequency of SPDIF Clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Output		:
*	Return		:
*
*	Description	: Set frequency and Enable Clock
***********************************************************/
void IO_CKC_SetSPDIFClock(int iFreq);

/**********************************************************
*	void IO_CKC_SetSPI0Clock(int iFreq);
*	void IO_CKC_SetSPI1Clock(int iFreq);
*	void IO_CKC_SetSPI2Clock(int iFreq);
*	void IO_CKC_SetSPI3Clock(int iFreq);
*
*	Input		: iFreq = Frequency of SPIS Clock, 100Hz unit
*				  Clock disabled if iFreq == 0.
*	Output		:
*	Return		:
*
*	Description	: Set frequency and Enable Clock
***********************************************************/
void IO_CKC_SetSPI0Clock(int iFreq);
void IO_CKC_SetSPI1Clock(int iFreq);
void IO_CKC_SetSPI2Clock(int iFreq);
void IO_CKC_SetSPI3Clock(int iFreq);

/**********************************************************
*	void IO_CKC_UpdateFPERI(unsigned uPeri, unsigned uFperi);
*
*	Input		: uPeri = Index of peripheral clock frequency table.
*				  uFperi = New Frequency value.
*	Output		:
*	Return		:
*
*	Description	: updates pIO_CKC_Fpll table with new frequency.
***********************************************************/
void IO_CKC_UpdateFPERI(unsigned uPeri, unsigned uFperi);


extern unsigned		uIO_CKC_Fpll[];			// Current PLL Frequency, 100Hz unit
extern unsigned		uIO_CKC_Fsys[];
extern unsigned		uIO_CKC_Fclk[];			// Current Frequency, 100Hz unit
extern unsigned		uTMODE0;				// image of HwTMODE register.
extern unsigned		uTMODE1;				// image of HwTMODE register.
extern unsigned		pIO_CKC_Fperi[];		// Current Peripheral Frequency, 100Hz unit
extern const sPLL	pIO_CKC_PLL[];			// PLL Setting (PMS) Table
extern unsigned		uIO_CKC_FrzClk;			// Freeze CPU, BUS Clock
extern unsigned		uIO_CKC_Fmax[];			// Fclk Max Frequency, 100Hz unit
extern unsigned		uIO_CKC_CurSpeedSet;	// Current Speed Mode
extern unsigned		uIO_CKC_Fbus4cycle;		// Reference Bus Frequency & Period for calculating cycle parameter	

/************************************************************************
*	ARM
************************************************************************/
/**********************************************************
*	unsigned IO_ARM_DrainWBInt(void);
*
*	Input		: none
*	Return		: 0
*	Description	: Drain Write Buffer
*				  This function is located in IO_LIB_INT area and
*				  it should be located in the internal SRAM.
**********************************************************/
unsigned IO_ARM_DrainWBInt(void);

/**********************************************************
*	unsigned IO_ARM_CleanCACHEInt(unsigned uDrainWB);
*
*	Input		: uDrainWB = Write Buffer control
*					1 = Execute Drain Write Buffer
*	Return		: 0
*	Description	: Clean Data cache and/or Drain Write Buffer
*				  This function is located in IO_LIB_INT area and
*				  it should be located in the internal SRAM.
**********************************************************/
unsigned IO_ARM_CleanCACHEInt(unsigned uDrainWB);

/**********************************************************
*	int IO_ARM_LockICACHE(void *pFPTRstart, void *pFPTRend);
*
*	Input		: pFPTRstart = function start address for locking
*				  pFPTRend   = function end address for locking
*	Return		: -1 = if error
*				    n = lock index (n >= 0)
*	Description	: Lock the object function into I cache
*				  The locked region
*					starts from (pFPTRstart & ~0x7F) and and at (pFPTRend | 0x7F).
**********************************************************/
int IO_ARM_LockICACHE(void *pFPTRstart, void *pFPTRend);

/**********************************************************
*	unsigned IO_ARM_DrainWB(void);
*
*	Input		: none
*	Return		: 0
*	Description	: Drain Write Buffer
**********************************************************/
unsigned IO_ARM_DrainWB(void);

/**********************************************************
*	unsigned IO_ARM_CleanCACHE(unsigned uDrainWB);
*
*	Input		: uDrainWB = Write Buffer control
*					1 = Drain write buffer
*					0 = Don't drain write buffer
*	Return		: 0
*	Description	: Clean Data cache (and Drain Write Buffer)
**********************************************************/
unsigned IO_ARM_CleanCACHE(unsigned uDrainWB);

/**********************************************************
*	unsigned IO_ARM_SetMMU(unsigned uMMU);
*
*	Input		: uMMU = MMU control register value
*					Hw16 = DTCM control (1 = to enable)
*					Hw12 = ICache control (1 = to enable)
*					Hw2   = DCache control (1 = to enable)
*					Hw0   = Protection control (1 = to enable)
*	Return		: same as input
*	Description	: Set MMU control register
**********************************************************/
unsigned IO_ARM_SetMMU(unsigned);

/**********************************************************
*	unsigned IO_ARM_GetMMU(void);
*
*	Input		: None
*	Return		: current MMU control register (C1 register) value
*					Hw16 = DTCM control (1 = enabled)
*					Hw12 = ICache control (1 = enabled)
*					Hw2   = DCache control (1 = enabled)
*					Hw0   = Protection control (1 = enabled)
*	Description	: Get current MMU control register (C1 register)
**********************************************************/
unsigned IO_ARM_GetMMU(void);

/**********************************************************
*	unsigned IO_ARM_ClearMMU(unsigned);
*
*	Input		: Flag for MMU control
*					Hw16 = DTCM control (1 = to disable)
*					Hw12 = ICache control (1 = to disable)
*					Hw2   = DCache control (1 = to disable)
*					Hw0   = Protection control (1 = to disable)
*	Return		: MMU Settings before modifying
*	Description	: Clear MMU control flags according to argument
**********************************************************/
unsigned IO_ARM_ClearMMU(unsigned);

/**********************************************************
*	unsigned IO_ARM_FlushCACHE(void);
*
*	Input		: None
*	Return		: 0
*	Description	: Flush Data & Instruction Cache
**********************************************************/
unsigned IO_ARM_FlushCACHE(void);

/**********************************************************
*	unsigned IO_ARM_FlushICACHE(void);
*
*	Input		: None
*	Return		: 0
*	Description	: Flush Instruction Cache
**********************************************************/
unsigned IO_ARM_FlushICACHE(void);

/**********************************************************
*	unsigned IO_ARM_FlushDCACHE(void);
*
*	Input		: None
*	Return		: 0
*	Description	: Flush Data Cache
**********************************************************/
unsigned IO_ARM_FlushDCACHE(void);

/**********************************************************
*	unsigned IO_ARM_GetICACHE(void);
*
*	Input		: None
*	Return		: Current region flags for Inst Cache
*					HwX : Region X flag (1 = instruction cache enabled)
*	Description	: Set Region flags for Inst Cache
**********************************************************/
unsigned IO_ARM_GetICACHE(void);

/**********************************************************
*	unsigned IO_ARM_GetDCACHE(void);
*
*	Input		: None
*	Return		: Current region flags for Data Cache
*					HwX : Region X flag (1 = data cache enabled)
*	Description	: Set Region flags for Data Cache
**********************************************************/
unsigned IO_ARM_GetDCACHE(void);

/**********************************************************
*	unsigned IO_ARM_GetWB(void);
*
*	Input		: None
*	Return		: Current region flags for Write Buffering
*					HwX : Region X flag (1 = write buffer enabled)
*	Description	: Get Region flags for Write Buffering
**********************************************************/
unsigned IO_ARM_GetWB(void);

/**********************************************************
*	unsigned IO_ARM_SetCPSR(unsigned uCPSR);
*
*	Input		: uCPSR = value for new CPSR
*	Return		: same as uCPSR
*	Description	: Set the CPSR as uCPSR value
**********************************************************/
unsigned IO_ARM_SetCPSR(unsigned uCPSR);

/**********************************************************
*	unsigned IO_ARM_GetCPSR(void);
*
*	Input		: None
*	Return		: the CPSR register value
*	Description	: Get the CPSR register value
**********************************************************/
unsigned IO_ARM_GetCPSR(void);

/**********************************************************
*	unsigned IO_ARM_SetINT(unsigned uCPSR);
*
*	Description	: same as IO_ARM_SetCPSR()
**********************************************************/
unsigned IO_ARM_SetINT(unsigned uCPSR);

/**********************************************************
*	unsigned IO_ARM_GetINT(void);
*
*	Description	: same as IO_ARM_GetCPSR()
**********************************************************/
unsigned IO_ARM_GetINT(void);

/**********************************************************
*	unsigned IO_ARM_EnableFIQ(void);
*
*	Input		: None
*	Return		: old CPSR (before FIQ enabled)
*	Description	: Enable FIQ
**********************************************************/
unsigned IO_ARM_EnableFIQ(void);

/**********************************************************
*	unsigned IO_ARM_DisableFIQ(void);
*
*	Input		: None
*	Return		: old CPSR (before FIQ disabled)
*	Description	: Disable FIQ
**********************************************************/
unsigned IO_ARM_DisableFIQ(void);

/**********************************************************
*	unsigned IO_ARM_EnableIRQ(void);
*
*	Input		: None
*	Return		: old CPSR (before IRQ enabled)
*	Description	: Enable IRQ
**********************************************************/
unsigned IO_ARM_EnableIRQ(void);

/**********************************************************
*	unsigned IO_ARM_DisableIRQ(void);
*
*	Input		: None
*	Return		: old CPSR (before IRQ disabled)
*	Description	: Disable IRQ
**********************************************************/
unsigned IO_ARM_DisableIRQ(void);

/**********************************************************
*	unsigned IO_ARM_EnableINT(void);
*
*	Input		: None
*	Return		: old CPSR (before Interrupt enabled)
*	Description	: Enable IRQ/FIQ Interrupt
**********************************************************/
unsigned IO_ARM_EnableINT(void);

/**********************************************************
*	unsigned IO_ARM_DisableINT(void);
*
*	Input		: None
*	Return		: old CPSR (before Interrupt disabled)
*	Description	: Disable IRQ/FIQ Interrupt
**********************************************************/
unsigned IO_ARM_DisableINT(void);

/************************************************************************
*	Interrupt Controller
************************************************************************/
extern void IOS_INT_HandleIRQ0(int inum);
extern void IOS_INT_HandleIRQ1(int inum);
extern void IOS_INT_HandleIRQ2(int inum);
extern void IOS_INT_HandleIRQ3(int inum);
extern void IOS_INT_HandleIRQ4(int inum);
extern void IOS_INT_HandleIRQ5(int inum);
extern void IOS_INT_HandleIRQ6(int inum);
extern void IOS_INT_HandleIRQ7(int inum);
extern void IOS_INT_HandleIRQ8(int inum);
extern void IOS_INT_HandleIRQ9(int inum);
extern void IOS_INT_HandleIRQ10(int inum);
extern void IOS_INT_HandleIRQ11(int inum);
extern void IOS_INT_HandleIRQ12(int inum);
extern void IOS_INT_HandleIRQ13(int inum);
extern void IOS_INT_HandleIRQ14(int inum);
extern void IOS_INT_HandleIRQ15(int inum);
extern void IOS_INT_HandleIRQ16(int inum);
extern void IOS_INT_HandleIRQ17(int inum);
extern void IOS_INT_HandleIRQ18(int inum);
extern void IOS_INT_HandleIRQ19(int inum);
extern void IOS_INT_HandleIRQ20(int inum);
extern void IOS_INT_HandleIRQ21(int inum);
extern void IOS_INT_HandleIRQ22(int inum);
extern void IOS_INT_HandleIRQ23(int inum);
extern void IOS_INT_HandleIRQ24(int inum);
extern void IOS_INT_HandleIRQ25(int inum);
extern void IOS_INT_HandleIRQ26(int inum);
extern void IOS_INT_HandleIRQ27(int inum);
extern void IOS_INT_HandleIRQ28(int inum);
extern void IOS_INT_HandleIRQ29(int inum);
extern void IOS_INT_HandleIRQ30(int inum);
extern void IOS_INT_HandleIRQ31(int inum);

/**********************************************************
;	unsigned IO_INT_EnableINT(void)
;
;	Input		: None
;	Return		: HwALLMSK before Interrupt Enabled
;
;	Description	: Enable IRQ/FIQ Interrupt
**********************************************************/
unsigned IO_INT_EnableINT(void);

/**********************************************************
;	unsigned IO_INT_EnableIRQ(void)
;
;	Input		: None
;	Return		: HwALLMSK before Interrupt Enabled
;
;	Description	: Enable IRQ Interrupt
**********************************************************/
unsigned IO_INT_EnableIRQ(void);

/**********************************************************
;	unsigned IO_INT_EnableFIQ(void)
;
;	Input		: None
;	Return		: HwALLMSK before Interrupt Enabled
;
;	Description	: Enable FIQ Interrupt
**********************************************************/
unsigned IO_INT_EnableFIQ(void);

/**********************************************************
;	unsigned IO_INT_DisableINT(void)
;
;	Input		: None
;	Return		: HwALLMSK before Interrupt Disabled
;
;	Description	: Disable IRQ/FIQ Interrupt
**********************************************************/
unsigned IO_INT_DisableINT(void);

/**********************************************************
;	unsigned IO_INT_DisableIRQ(void)
;
;	Input		: None
;	Return		: HwALLMSK before Interrupt Disabled
;
;	Description	: Disable IRQ Interrupt
**********************************************************/
unsigned IO_INT_DisableIRQ(void);

/**********************************************************
;	unsigned IO_INT_DisableFIQ(void)
;
;	Input		: None
;	Return		: HwALLMSK before Interrupt Disabled
;
;	Description	: Disable FIQ Interrupt
**********************************************************/
unsigned IO_INT_DisableFIQ(void);

/**********************************************************
;	unsigned IO_INT_RestoreINT(unsigned uMSK)
;
;	Input		: uMSK = Mask value for HwALLMSK
;	Return		: uMSK
;
;	Description	: Restore IRQ/FIQ Interrupt
**********************************************************/
unsigned IO_INT_RestoreINT(unsigned uMSK);

/************************************************************************
*	Memory Controller
************************************************************************/
/**********************************************************
*	void IO_MC_SetCFGInt(unsigned uCS, unsigned uValue);
*
*	Input		: uCS = 0~4
*				  uValue = register value for corresponding chip select
*	Return		:
*	Description	: Set the corresponding memory configuration register (HwCSCFG0~3, HwSDCFG)
*				  This function is located in IO_LIB_INT area and
*				  it should be located in the internal SRAM.
**********************************************************/
void IO_MC_SetCFGInt(unsigned uCS, unsigned uValue);

/************************************************************************
*	DMA Controller
************************************************************************/
enum { 
	IO_DMA_CH0, 
	IO_DMA_CH1, 
	IO_DMA_CH2,
	IO_DMA_CH3,
	IO_DMA_CH4,
	IO_DMA_CH5,
	IO_DMA_CH6,
	IO_DMA_CH7,
	IO_DMA_CH8,
	IO_DMA_CH9,
	IO_DMA_CH10,
	IO_DMA_CH11,
	IO_DMA_CH_MAX
};

#if defined(TCC79XX) || defined(TCC92XX) || defined(TCC89XX)
	#define	IO_DMA_PORTCFG_NFC_READ		0
	#define	IO_DMA_PORTCFG_NFC_WRITE	0
#endif

#define	IO_DMA_EnableDMA(X)		{ BITSET(((sHwDMA *)(X))->CHCTRL, HwCHCTRL_EN_EN); }
#define	IO_DMA_DisableDMA(X)	{ BITSET(((sHwDMA *)(X))->CHCTRL, HwCHCTRL_EN_EN); }

/**********************************************************
*	void IO_DMA_SetCTRL(unsigned uCH, unsigned	uCHCTRL);
*
*	Input		: uCH = Channel number (0~2)
*				  uValue = DMA control register value of corresponding channel
*	Return		:
*	Description	: Set the corresponding DMA control register (HwCHCTRL0~1 in General DMA, HwCHCTRL0 in Storage DMA)
**********************************************************/
PGDMACTRL IO_DMA_SetCTRL(unsigned uCH, unsigned	uCHCTRL);

/**********************************************************
*	void IO_DMA_SetDMA(unsigned uCH, void *pSRC, unsigned uSPARAM,
*						void *pDST, unsigned uDPARAM,
*						unsigned	uCHCTRL, unsigned uSize);
*	Input		: uCH = Channel number (0~2)
*				  pSRC = source address
*				  uSPARAM = source parameter
*				  pDST = destination address
*				  uDPARAM = destination parameter
*				  uCHCTRL = DMA control register
*				  uSize = Transfer size (in byte unit)
*	Return		:
*	Description	: Set the DMA (and start to transfer)
**********************************************************/
#ifdef TCC79XX
sHwDMA *IO_DMA_SetDMA(
	unsigned uCH,
	void *pSRC, unsigned uSPARAM,
	void *pDST, unsigned uDPARAM,
	unsigned	uCHCTRL,
	unsigned 	uREQSRC,  // added for TCC83XX 2007.9.13
	unsigned uSize
	);

sHwDMA *IO_DMA_SetDMA_NAND(
	unsigned uCH,
	void *pSRC, unsigned uSPARAM,
	void *pDST, unsigned uDPARAM,
	unsigned	uCHCTRL,
	unsigned uEXTREQ,
	unsigned uSize	
);
#endif


#define	HwCHCTRL_CONT_C				Hw15				// DMA transfer begins from C_SADR/C_DADR Address. It must be used after the former transfer has been executed, so that C_SADR and C_DADR contain a meaningful value.
#define	HwCHCTRL_CONT_ST				(0)					// DMA trnaster begins from ST_SADR/ST_DADR Address
#define	HwCHCTRL_DTM_EN				Hw14				// Differential Transfer Mode Enable
#define	HwCHCTRL_DTM_ON				Hw14				// Differential Transfer Mode Enable
#define	HwCHCTRL_DTM_OFF				(0)					// Differential Transfer Mode Disable
#define	HwCHCTRL_SYNC_ON				Hw13				// Synchronize HardWare Request
#define	HwCHCTRL_SYNC_OFF			(0)					// Do not Synchronize HardWare Request
#define 	HwCHCTRL_SYNC_EN				Hw13				// Synchronize Hardware Request
#define	HwCHCTRL_HRD_W				Hw12				// ACK/EOT signals are issued When DMA-Write Operation
#define	HwCHCTRL_LOCK_EN				Hw11				// DMA transfer executed with lock transfer
#define	HwCHCTRL_BST_NOARB			Hw10				// DMA transfer executed with no arbitration(burst operation)
#define	HwCHCTRL_HRD_WR				Hw12				// ACK/EOT signals are issued When DMA-Write Operation
#define	HwCHCTRL_HRD_RD				(0)					// ACK/EOT signals are issued When DMA-Read Operation
#define	HwCHCTRL_LOCK_ON				Hw11				// DMA transfer executed with lock transfer
#define	HwCHCTRL_LOCK_OFF			(0)					//
#define	HwCHCTRL_BST_BURST			Hw10				// DMA transfer executed with no arbitration(burst operation)
#define	HwCHCTRL_BST_ARB				(0)					// DMA transfer executed wth arbitration
#define	HwCHCTRL_TYPE_SINGE			(0)					// SINGLE transfer with edge-triggered detection
#define	HwCHCTRL_TYPE_HW				Hw8					// HW Transfer
#define	HwCHCTRL_TYPE_SW				Hw9					// SW transfer
#define	HwCHCTRL_TYPE_SINGL			(Hw9|Hw8)			// SINGLE transfer with level-triggered detection
#define	HwCHCTRL_TYPE_SL				(Hw9|Hw8)			// SINGLE transfer with level-triggered detection
#define HwCHCTRL_TYPE_SE				HwZERO				// SINGLE transfer with edge-triggered detection

#define	HwCHCTRL_BSIZE_1				(0)					// 1 Burst transfer consists of 1 read or write cycle
#define	HwCHCTRL_BSIZE_2				Hw6					// 1 Burst transfer consists of 2 read or write cycles
#define	HwCHCTRL_BSIZE_4				Hw7					// 1 Burst transfer consists of 4 read or write cycles
#define	HwCHCTRL_BSIZE_8				(Hw6|Hw7)			// 1 Burst transfer consists of 8 read or write cycles

#define	HwCHCTRL_WSIZE_8				(0)					// Each cycle read or write 8bit data
#define	HwCHCTRL_WSIZE_16			Hw4					// Each cycle read or write 16bit data
#define	HwCHCTRL_WSIZE_32			Hw5					// Each cycle read or write 32bit data

#define	HwCHCTRL_FLAG					Hw3					// Clears FLAG to 0
#define	HwCHCTRL_IEN_ON				Hw2					// At the same time the FLAG goes to 1, DMA interrupt request is generated
#define HwCHCTRL_IEN_EN				Hw2					// At the same time the FLAG goes to 1, DMA interrupt request is generated
#define	HwCHCTRL_IEN_OFF			~Hw2				//
#define	HwCHCTRL_REP_EN				Hw1					// The DMA channel remains enabled
#define	HwCHCTRL_REP_DIS				~Hw1				// After all of hop transfer has executed, the DMA channel is disabled
#define	HwCHCTRL_EN_ON				Hw0					// DMA channel is Enabled
#define	HwCHCTRL_EN_OFF				~Hw0				// DMA channel is terminated and disabled/*}}}*/
#define 	HwCHCTRL_EN_EN				Hw0					// DMA channel is enabled. If software type transfer is selected, this bit generates DMA request directly, or if hardware type transfer is used, the selected interrupt request flag generate DMA request


/************************************************************************
*	ECC Controller
************************************************************************/
#define	IO_ECC_DisableECC()	{ HwECC_CTRL &= HwECC_CTRL_SE_DIS; }
#define	IO_ECC_EnableECC()	{ HwECC_CTRL |= HwECC_CTRL_SE_EN; }

/**********************************************************
*	int IO_ECC_CheckSLC(unsigned char *pcDATA, unsigned char *pcSPARE);
*
*	Input		: pcDATA = start address of data block
*				  pcSPARE = start address of spare block
*	Return		: 0 = no error or correctable error
*				  -1 = uncorrectable error
*	Description	: Check ECC and Correct Data Error
************************************************************/
int IO_ECC_CheckSLC(unsigned char *pcDATA, unsigned char *pcSPARE);

/************************************************************************
*	NAND Flash Controller
************************************************************************/
#define	HwNAND_CMD		(pHwND->CMD)
#define	HwNAND_LADR		(pHwND->LADR)
#define	HwNAND_DATA		(pHwND->WDATA.D32)
#define	HwNAND_SDATA	((cIO_NFC_MEM) ? pHwND->WDATA.D8 : pHwND->SDATA.D32)
#define	HwNAND_SADR		(pHwND->SADR)

#define	ECC_BASEPAGE	((unsigned)&HwNAND_DATA)

typedef	struct {				// NAND Request Structure
	unsigned	CTRL;
	unsigned	PADDR;			// Physical Address
	unsigned	NFCCFG;			// image of HwNFC_CTRL register for corresponding request
	unsigned	*PBUF;			// Pointer for Page Data
	void		*SBUF;			// Pointer for Spare Data
} sNFC_REQ;

typedef	struct {				// NAND Request Master Structure
	unsigned	uSEMA;			// Semaphore
	sNFC_REQ	*pReqHead;		// request is poped from here.
	sNFC_REQ	*pReqTail;		// request is pushed to here.
} sNFC_REQMST;

#define	REQ_FAIL		((sNFC_REQ *)(-1))

// sNFC_REQ.CTRL bit-field definition
//-----------------------------------------------------------------------------------
#define	REQ_INACTIVE	(Hw31)			// Inactive, or End of Write Request (1)
#define	REQ_EOR			(Hw30)			// End of Read Request, but not copied yet. (1)
#define	REQ_ECCERR		(Hw29)			// Uncorrectable ECC error occurred. (1)
#define	REQ_SPARE		(Hw28)			// Start from the spare area. (1)
//-----------------------------------------------------------------------------------
#define	REQ_DISABLE_MK	(Hw27)			// Disable Writing ECC MARK in data transfer (1)
#define	REQ_ECCEN		(Hw26)			// Calc ECC in data transfer (1)
#define	REQ_EOW			(Hw25)			// End of Write Request, but not check OK bit yet. (1)
#define	REQ_WOK			(Hw24)			// Write OK Flag (1)
//-----------------------------------------------------------------------------------
#define	REQ_SKIPHCMD	(Hw23)			// Skip header command process (1)
#define	REQ_SKIPTCMD	(Hw22)			// Skip tail command process (1)
#define	REQ_SKIPCMD		(Hw23|Hw22)
#define	REQ_CPBACK		(Hw21)			// Copy-Back Enabled. (1)
#define	REQ_CPBACKW		(Hw20)			// Copy-Back with partial writing. (1)
//-----------------------------------------------------------------------------------
#define	REQ_SKIP10		(Hw19)			// Skip 0x10 command in writePAGE. (1)
#define	REQ_ERASEBLK	(Hw18)			// Erase Block command. (1)
#define	REQ_PRO_SPARE	(Hw17)			// Process spare area. (1)
//-----------------------------------------------------------------------------------
#define	REQ_READY		(Hw12)			// All settings for Request is ready (1)
#define	REQ_DSIZE_MASK	(REQ_READY - 1)	// [11:0] = Data Size
#define	REQ_DSIZE(X)	((X)&(REQ_DSIZE_MASK))

// uCS bit-field definition
#define	REQ_CSMASK		0x0000000F

enum { IO_NFC_READ = 0, IO_NFC_WRITE = Hw15};

void IO_NFC_InitDRV(void);
void IO_NFC_OpenREQ(sNFC_REQ *pReq, unsigned uMaxNFC);
void IO_NFC_CloseREQ(void);
void IO_NFC_PopREQ(sNFC_REQ *pReq);
void IO_NFC_EnableREQ(sNFC_REQ *pReq);
void IO_NFC_IRQHandler(void);
void IO_NFC_StartREAD(sNFC_REQ *pReq);
void IO_NFC_ReadPAGE(unsigned *pBuffer, unsigned uDSize);
void IO_NFC_ReadDATA(unsigned char *pBuffer, unsigned uDSize);
int IO_NFC_CopyPAGE(unsigned char *pDest, unsigned *pSrc, unsigned uSize);
sNFC_REQ *IO_NFC_PushRREQ(unsigned uCS, unsigned uPage, unsigned uColumn, unsigned *pSpare, unsigned uSize);
void IO_NFC_StartWRITE(sNFC_REQ *pReq);
void IO_NFC_WritePAGE(unsigned *pBuffer, unsigned uDSize);
void IO_NFC_WriteDATA(unsigned char *pBuffer, unsigned uDSize);
sNFC_REQ *IO_NFC_PushWREQ(	unsigned uCS, unsigned uPage, unsigned uColumn,
							unsigned *pData, unsigned char *pSpare, unsigned uSize);
int IO_NFC_CheckWOK(sNFC_REQ *pReq);
unsigned IO_NFC_LookupID(unsigned uDID);
unsigned IO_NFC_SetCONFIG(unsigned uType);
unsigned IO_NFC_MakeNFC(unsigned uCS);
unsigned IO_NFC_GetID(unsigned uCS);
unsigned IO_NFC_ResetNAND(unsigned uCS);
void IO_NFC_SetCYCLE(void);
void IO_NFC_WaitEOT(sNFC_REQ *pReq);

extern unsigned			uIO_NFC_CONFIG, uIO_NFC_MASK;
extern unsigned char	cIO_NFC_MEM;
extern PNFC				pIO_NFC_HwND;

#define	IO_NFC_MakeMASK(uCS)	((uIO_NFC_CONFIG & (Hw14 << (uCS*16))) ? 0xFFFF : 0xFF)

#define	IO_NFC_SetNFC(uCS)						\
{												\
	if (cIO_NFC_MEM)							\
		HwCSCFG2	= IO_NFC_MakeNFC(uCS);		\
	else										\
		HwNFC_CTRL	= IO_NFC_MakeNFC(uCS);		\
}

#ifdef TCC79XX
	#if defined(TCC7900_BOARD)
		#define	GPIO_ND_nCS		HwPORTCFG2
		#define	GPIO_ND_nCS1Bit	Hw5
		#define	GPIO_ND_nCS0Bit	Hw4
		#define	GPIO_ND_nWP		HwPORTCFG2
		#define	GPIO_ND_nWPBit	Hw2
		#define	GPIO_ND_RDY		HwPORTCFG2
		#define	GPIO_ND_RDYBit	Hw3
		#define	GPIO_NFC_nWP	HwPORTCFG2
		#define	GPIO_NFC_nWPBit	Hw17
	#elif defined(TCC792X_BOARD)
		#define	GPIO_ND_nCS		HwPORTCFG2
		#define	GPIO_ND_nCS1Bit	Hw9
		#define	GPIO_ND_nCS0Bit	Hw8
		#define	GPIO_ND_RDY		HwPORTCFG2
		#define	GPIO_ND_RDYBit	Hw14
	#endif
#elif defined(TCC92XX) || defined(TCC89XX)
	#if defined(TCC9200_BOARD)
		#define	GPIO_ND_nCS		HwPORTCFG2
		#define	GPIO_ND_nCS1Bit	Hw9
		#define	GPIO_ND_nCS0Bit	Hw8
		#define	GPIO_ND_RDY		HwPORTCFG2
		#define	GPIO_ND_RDYBit	Hw14
	#endif
#endif

#define	IO_NFC_DisableCS()										\
{																\
	if (cIO_NFC_MEM)											\
		BITSET(GPIO_ND_nCS, GPIO_ND_nCS1Bit|GPIO_ND_nCS0Bit);	\
	else														\
		BITSET(HwNFC_CTRL, HwNFC_CTRL_CFG_NOACT);				\
}

#define	IO_NFC_EnableCS(uCS)											\
{																		\
	if (cIO_NFC_MEM)													\
		BITSCLR(GPIO_ND_nCS, (uCS) ? GPIO_ND_nCS0Bit: GPIO_ND_nCS1Bit, (uCS) ? GPIO_ND_nCS1Bit: GPIO_ND_nCS0Bit);	\
	else																\
		BITSCLR(HwNFC_CTRL, (uCS) ? Hw22: Hw23, (uCS) ? Hw23 : Hw22);	\
}

#define	IO_NFC_DisableWP()						\
{												\
	if (cIO_NFC_MEM)							\
		BITSET(GPIO_ND_nWP, GPIO_ND_nWPBit);	\
	else										\
		BITSET(GPIO_NFC_nWP, GPIO_NFC_nWPBit);	\
}

#define	IO_NFC_EnableWP()						\
{												\
	if (cIO_NFC_MEM)							\
		BITCLR(GPIO_ND_nWP, GPIO_ND_nWPBit);	\
	else										\
		BITCLR(GPIO_NFC_nWP, GPIO_NFC_nWPBit);	\
}

#define	IO_NFC_NotREADY()						\
	( (cIO_NFC_MEM) ? ISZERO(GPIO_ND_RDY, GPIO_ND_RDYBit) : ISZERO(HwNFC_CTRL, HwNFC_CTRL_RDY_RDY))

#define	IO_NFC_WaitSTARDY()		{ ; }

void IO_NFC_WaitREADY(void);
void IO_NFC_WaitREADYForWriteCommand(void);

#define	IO_NFC_WaitDONE(X)		{ while (ISZERO(HwNFC_IREQ, (X))); }

#define	USE_NFC_DMA
#define	USE_NFC_FIFO		// only meaningful when "USE_NFC_DMA" is defined.

/************************************************************************
*	DTCM Allocation Manager
************************************************************************/
//#define	USE_DYNAMIC_DTCM

#if defined(TCC79XX)
	#define	DTCM_BASE				0xA0000000
	#define	DTCM_LIMIT				0xA0002000
	#define	DTCM_SIZE				(DTCM_LIMIT - DTCM_BASE)
	#define	ITCM_SIZE				0x00001000
	#define	IO_USB_BUFFER0_BASE		(DTCM_BASE + 0x1800)
	#define	IO_USB_BUFFER1_BASE		(DTCM_BASE + 0x1A00)
	#define	IO_NFC_BUFFER0_BASE		(DTCM_BASE + 0x1C00)
	#define	IO_NFC_BUFFER1_BASE		(DTCM_BASE + 0x1E00)
#elif defined(TCC92XX) || defined(TCC89XX)
	#define	DTCM_BASE				0xA0000000
	#define	DTCM_LIMIT				0xA0004000
	#define	DTCM_SIZE				(DTCM_LIMIT - DTCM_BASE)
	#define	ITCM_SIZE				0x00004000
	#define	IO_USB_BUFFER0_BASE		(DTCM_BASE + 0x1800)
	#define	IO_USB_BUFFER1_BASE		(DTCM_BASE + 0x1A00)
	#define	IO_NFC_BUFFER0_BASE		(DTCM_BASE + 0x1C00)
	#define	IO_NFC_BUFFER1_BASE		(DTCM_BASE + 0x1E00)
#endif


#define	IO_HDD_BUFFER_BASE			IO_NFC_BUFFER0_BASE

#ifdef	USE_DYNAMIC_DTCM
	#define	DTCM_MaskSize			(8 * sizeof(unsigned))							// bit width of uDTCM_MAT[] table
	#define	DTCM_Mask				((unsigned)((1 << DTCM_MaskSize) - 1))			// mask pattern of uDTCM_MAT[] table
	#define	DTCM_SIZE				4096
	#define	DTCM_AUNIT				64												// minimum allocation unit.
	#define	DTCM_MAXBULK			(DTCM_SIZE / DTCM_AUNIT)						// maximum concurrent allocation.
	#define	DTCM_MATSIZE			((DTCM_SIZE / DTCM_AUNIT) / DTCM_MaskSize)		// Bitmap table for each chunk (1 = used, 0 = not-used)
#endif

void IO_DTCM_InitDRV(void);
void *IO_DTCM_Malloc(unsigned uDSize);
void IO_DTCM_Free(void *pSrc, unsigned uDSize);

unsigned IO_DTCM_AllocMAT(unsigned uASize);
unsigned IO_DTCM_FindFMAX(void);


/************************************************************************
*	SRAM ADDR
************************************************************************/
#ifdef TCC79XX
	#define	SRAM_BASE					0x00000000
	#define	SRAM_LIMIT					0x00010000
#elif defined(TCC92XX) || defined(TCC89XX)
	#define	SRAM_BASE					0x10000000
	#define	SRAM_LIMIT					0x00004000
#endif

/************************************************************************
*	Interrupt Controller
************************************************************************/
#if defined(TCC92XX) || defined(TCC89XX)

	// Interrupt Enable 0
/*
	#define	IO_INT_EHI0		HwINT0_EHI0
	#define	IO_INT_ECC		HwINT0_ECC
	#define	IO_INT_DMA		HwINT0_DMA
	#define	IO_INT_TSADC	HwINT0_TSADC
	#define	IO_INT_G2D		HwINT0_G2D
	#define	IO_INT_3DMMU	HwINT0_3DMMU
	#define	IO_INT_3DGP		HwINT0_3DGP
	#define	IO_INT_3DPP		HwINT0_3DPP
	#define	IO_INT_VCDC		HwINT0_VCDC
	#define	IO_INT_JPGD		HwINT0_JPGD
	#define	IO_INT_JPGE		HwINT0_JPGE
	#define	IO_INT_VIPET	HwINT0_VIPET
	#define	IO_INT_LCD1		HwINT0_LCD1
	#define	IO_INT_LCD0		HwINT0_LCD0
	#define	IO_INT_CAM		HwINT0_CAM
	#define	IO_INT_SC1		HwINT0_SC1
	#define	IO_INT_SC0		HwINT0_SC0
	#define	IO_INT_EI11		HwINT0_EI11
	#define	IO_INT_EI10		HwINT0_EI10
	#define	IO_INT_EI9		HwINT0_EI9
	#define	IO_INT_EI8		HwINT0_EI8
	#define	IO_INT_EI7		HwINT0_EI7
	#define	IO_INT_EI6		HwINT0_EI6
	#define	IO_INT_EI5		HwINT0_EI5
	#define	IO_INT_EI4		HwINT0_EI4
	#define	IO_INT_EI3		HwINT0_EI3
	#define	IO_INT_EI2		HwINT0_EI2
	#define	IO_INT_EI1		HwINT0_EI1
	#define	IO_INT_EI0		HwINT0_EI0
	#define	IO_INT_SMUI2C	HwINT0_SMUI2C
	#define	IO_INT_TC1		HwINT0_TC1
	#define	IO_INT_TC0		HwINT0_TC0
*/
	enum {
		IO_INT_nTC0,		// 0
		IO_INT_nTC1,
		IO_INT_nSMUI2C,
		IO_INT_nEI0,
		IO_INT_nEI1,
		IO_INT_nEI2,		// 5
		IO_INT_nEI3,
		IO_INT_nEI4,
		IO_INT_nEI5,
		IO_INT_nEI6,
		IO_INT_nEI7,		// 10
		IO_INT_nEI8,
		IO_INT_nEI9,
		IO_INT_nEI10,
		IO_INT_nEI11,
		IO_INT_nSC0,		// 15
		IO_INT_nSC1,
		IO_INT_nCAM,
		IO_INT_nLCD0,
		IO_INT_nLCD1,
		IO_INT_nVIPET,		// 20
		IO_INT_nJPGE,
		IO_INT_nJPGD,
		IO_INT_nVCDC,
		IO_INT_n3DPP,
		IO_INT_n3DGP,		// 25
		IO_INT_n3DMMU,
		IO_INT_nG2D,
		IO_INT_nTSADC,
		IO_INT_nDMA,
		IO_INT_nECC,		// 30
		IO_INT_nEHI0
	};

	// Interrupt Enable 1
/*
	#define	IO_INT_AEIRQ	HwINT1_AEIRQ
	#define	IO_INT_ASIRQ	HwINT1_ASIRQ
	#define	IO_INT_AIRQ		HwINT1_AIRQ
	#define	IO_INT_APMU		HwINT1_APMU
	#define	IO_INT_AUDIO	HwINT1_AUDIO
	#define	IO_INT_ADMA		HwINT1_ADMA
	#define	IO_INT_DAITX	HwINT1_DAITX
	#define	IO_INT_DAIRX	HwINT1_DAIRX
	#define	IO_INT_CDRX		HwINT1_CDRX
	#define	IO_INT_TSIF1	HwINT1_TSIF1
	#define	IO_INT_TSIF0	HwINT1_TSIF0
	#define	IO_INT_GPS2		HwINT1_GPS2
	#define	IO_INT_GPS1		HwINT1_GPS1
	#define	IO_INT_GPS0		HwINT1_GPS0
	#define	IO_INT_UOTG		HwINT1_UOTG
	#define	IO_INT_UART		HwINT1_UART
	#define	IO_INT_SPDTX	HwINT1_SPDTX
	#define	IO_INT_SD1		HwINT1_SD1	
	#define	IO_INT_SD0		HwINT1_SD0
	#define	IO_INT_RTC		HwINT1_RTC
	#define	IO_INT_RMT		HwINT1_RMT
	#define	IO_INT_NFG		HwINT1_NFC
	#define	IO_INT_MS		HwINT1_MS
	#define	IO_INT_MPEFEC	HwINT1_MPEFEC
	#define	IO_INT_I2C		HwINT1_I2C
	#define	IO_INT_HDD		HwINT1_HDD
	#define	IO_INT_GPSB		HwINT1_GPSB
	#define	IO_INT_HDMI		HwINT1_HDMI
	#define	IO_INT_EHI1		HwINT1_EHI1
*/
	enum {
		IO_INT_nEHI1,		// 0
		IO_INT_nNotUsed0,
		IO_INT_nHDMI,
		IO_INT_nNotUsed1,
		IO_INT_nGPSB,
		IO_INT_nHDD,		// 5
		IO_INT_nI2C,
		IO_INT_nMPEFEC,
		IO_INT_nMS,
		IO_INT_nNFC,
		IO_INT_nRMT,		// 10
		IO_INT_nRTC,
		IO_INT_nSD0,
		IO_INT_nSD1,
		IO_INT_nSPDTX,
		IO_INT_nUART,		// 15
		IO_INT_nUOTG,
		IO_INT_nNotUsed2,
		IO_INT_nGPS0,
		IO_INT_nGPS1,
		IO_INT_nGPS2,		// 20
		IO_INT_nTSIF0,
		IO_INT_nTSIF1,
		IO_INT_nCDRX,
		IO_INT_nDAIRX,
		IO_INT_nDAITX,		// 25
		IO_INT_nADMA,
		IO_INT_nAUDIO,
		IO_INT_nAPMU,
		IO_INT_nAIRQ,
		IO_INT_nASIRQ,		// 30
		IO_INT_nAEIRQ,
	};

#elif defined(TCC79XX)
	#define	IO_INT_HwIEN		HwIEN
	#define	IO_INT_HwISTS		HwSTS
	#define	IO_INT_HwMSTS		HwMSTS
	#define	IO_INT_HwICLR		HwCLR
	#define	IO_INT_HwISEL		HwSEL
	#define	IO_INT_HwTMODE		HwMODE
	#define	IO_INT_HwPOL		HwPOL
	#define	IO_INT_HwEXT0		HwINT_EI0
	#define	IO_INT_HwEXT1		HwINT_EI1
	#define	IO_INT_HwEXT2		HwINT_EI2
	#define	IO_INT_HwEXT3		HwINT_EI3
	#define	IO_INT_HwRTC		HwINT_RTC
	#define IO_INT_HwGPSB       HwINT_GPSB
	#define	IO_INT_HwGPSB_CH0	HwINT_GPSB
	#define	IO_INT_HwGPSB_CH1	HwINT_GPSB
	#define	IO_INT_HwGPSB_CH2	HwINT_GPSB
	#define	IO_INT_HwGPSB_CH3	HwINT_GPSB
	#define	IO_INT_HwTIMER		(HwINT_TC1 | HwINT_TC0)
	#define	IO_INT_HwTC0		HwINT_TC0
	#define	IO_INT_HwTC1		HwINT_TC1
	#define	IO_INT_HwSCORE		HwINT_SCORE
	#define	IO_INT_HwSPDTX		HwINT_SPDTX
	#define	IO_INT_HwSEL0		HwINT_SEL0
	#define	IO_INT_HwSEL1		HwINT_SEL1
	#define	IO_INT_HwSC			HwINT_SC
	#define	IO_INT_HwI2C		HwINT_I2C
	#define	IO_INT_HwDAIRX		HwINT_DAIRX
	#define	IO_INT_HwDAITX		HwINT_DAITX
	#define	IO_INT_HwCDIF		HwINT_CDRX
	#define	IO_INT_HwEHI		HwINT_HPI
	#define	IO_INT_HwUART		HwINT_UT
	#define	IO_INT_HwUART0		HwINT_UT
	#define	IO_INT_HwUART1		HwINT_UT
	#define	IO_INT_HwSEL2		HwINT_SEL2
	#define	IO_INT_HwG2D		HwINT_G2D
	#define	IO_INT_HwUSBD		HwINT_UD
	#define	IO_INT_HwUSBH		HwINT_UH
	#define	IO_INT_HwDMA		HwINT_DMA
	#define	IO_INT_HwDMA_CH0	HwINT_DMA
	#define	IO_INT_HwDMA_CH1	HwINT_DMA
	#define	IO_INT_HwDMA_CH2	HwINT_DMA
	#define	IO_INT_HwHDD		HwINT_HDD
	#define	IO_INT_HwSEL3		HwINT_SEL3
	#define	IO_INT_HwNFC		HwINT_NFC
	#define	IO_INT_HwSDMMC		HwINT_SD
	#define	IO_INT_HwCAM		HwINT_CAM
	#define	IO_INT_HwLCD		HwINT_LCD
	#define	IO_INT_HwADC		HwINT_ADC
	#define	IO_INT_HwSEL4		HwINT_SEL4
	#define	IO_INT_HwGSIO		0

	#define	IO_INT_HwVIDEO		HwINT_SEL0
	#define	IO_INT_HwEXT5		HwINT_SEL1
	#define	IO_INT_HwEHI1		HwINT_SEL2
	#define	IO_INT_HwMSC		HwINT_SEL3
	#define	IO_INT_HwEXT7		HwINT_SEL4
	#define	IO_INT_HwUSBDMA		HwINT_SEL4

	enum {
		IO_INT_nEI0, 	// 0
		IO_INT_nEI1, 
		IO_INT_nEI2, 
		IO_INT_nEI3, 
		IO_INT_nRTC, 
		IO_INT_nGPSB,	// 5 
		IO_INT_nTC0, 
		IO_INT_nTC1,
		IO_INT_nVCORE, 
		IO_INT_nSPDTX, 
		IO_INT_nVIDEO, 	// 10
		IO_INT_nEI5, 
		IO_INT_nSC, 
		IO_INT_nI2C, 
		IO_INT_nDAIRX, 
		IO_INT_nDAITX,	// 15
		IO_INT_nCDRX, 
		IO_INT_nEHI, 
		IO_INT_nUT0, 
		IO_INT_nEHI1, 
		IO_INT_nUDMA, 	// 20
		IO_INT_nUD, 
		IO_INT_nUH, 
		IO_INT_nDMA,
		IO_INT_nHDD, 
		IO_INT_nMS, 	// 25
		IO_INT_nNFC, 
		IO_INT_nSD, 
		IO_INT_nCAM, 
		IO_INT_nLCD, 
		IO_INT_nADC, 	// 30
		IO_INT_nUSBDMA
	};
#endif

#if 0
#define	IO_INT_EnableIRQ(X)					\
		{									\
			BITSET(IO_INT_HwISEL, X);		\
			BITSET(IO_INT_HwIEN, X);		\
		}
#define	IO_INT_DisableIRQ(X)				\
		{									\
			BITCLR(HwIEN, X);				\
		}
#endif
/************************************************************************
*	EHI Controller
************************************************************************/
#define	EHI_MD_68		HwEHCFG_MD_68
#define	EHI_MD_80		HwEHCFG_MD_80
#define	EHI_USE_MASK	Hw1
#define	EHI_BW_8		HwEHCFG_BW_8
#define	EHI_BW_16		HwEHCFG_BW_16
#define	EHI_USE_RDY		HwEHCFG_RDYE_RDY
#define	EHI_USE_IRQ		HwEHCFG_RDYE_IRQ
#define	EHI_HIGH_RDY	HwZERO
#define	EHI_LOW_RDY		HwEHCFG_RDYP

extern	unsigned	uEHI_CSCFG, uEHI_TACC;
extern	volatile unsigned *pEHI_CSCFG;

#ifdef	EHIS_TCC77X
	#define	EHIS_HwEHST			0x90000800	// R/W, Status register
	#define	EHIS_HwEHIINT		0x90000804	// R/W,  Internal interrupt control register
	#define	EHIS_HwEHEINT		0x90000808	// R/W,  External interrupt control register
	#define	EHIS_HwEHA			0x9000080C	// R/W,  Address register
	#define	EHIS_HwEHAM			0x90000810	// R,  Address masking register
	#define	EHIS_HwEHD			0x90000814	// R/W,  Data register
	#define	EHIS_HwEHSEM		0x90000818	// R/W,  Semaphore register
	#define	EHIS_HwEHCFG		0x9000081C	// R/W,  Configuration register
	#define	EHIS_HwEHIND		0x90000820	// W,  Index register
	#define	EHIS_HwEHRWCS		0x90000824	// R/W,  Read/Write Control/Status register
#endif

#ifdef EHI_MASTER
	/**********************************************************
	*	void IO_EHI_InitDRV(unsigned uCONFIG, unsigned uCS, unsigned uMask);
	*
	*	Input		: uCONFIG = Configuration Parameter
	*					Hw0 = 68000 (1), x86 (0) interface
	*					Hw1 = Use Mask (1), Don't used Mask (0)
	*					Hw2 = 8bit (1), 16bit (0) interface
	*					Hw3 = used as Ready signal (1), used as Interrupt signal (0)
	*					Hw4 = Active Low Ready signal (1), Active High Ready signal (0)
	*				  uCS = Chip Select number for EHI slave (0~3)
	*				  uMask = Address Mask Pattern.
	*	Return		:
	*	Description	: Initialize EHI I/F module at the Master Site.
	************************************************************/
	void IO_EHI_InitDRV(unsigned uCONFIG, unsigned uCS, unsigned uMask);

	/**********************************************************
	*	unsigned IO_EHI_SetSPEED(unsigned uTAcc);
	*
	*	Input		: uTAcc = Access time in nano second
	*	Output		:
	*	Return		: previous CSCFG value
	*
	*	Description	: Set EHI Access parameter
	************************************************************/
	unsigned IO_EHI_SetSPEED(unsigned uTAcc);

	/**********************************************************
	*	unsigned IO_EHI_IncSPEED(int iTAccDelta);
	*
	*	Input		: iTAccDelta = Increment(+)/Decrement(-) of Access time in nano second
	*	Output		:
	*	Return		: Old TAcc
	*
	*	Description	: Increment EHI Access parameter
	************************************************************/
	unsigned IO_EHI_IncSPEED(int iTAccDelta);

	/**********************************************************
	*	void IO_EHI_WriteREG(unsigned uADDR, unsigned uDATA, unsigned uSize);
	*
	*	Input		: uADDR = Address of EHI Register
	*				  uDATA = Data for EHI Register
	*				  uSize = Register Size in byte.
	*	Return		:
	*	Description	: Write EHI Register
	************************************************************/
	void IO_EHI_WriteREG(unsigned uADDR, unsigned uDATA, unsigned uSize);

	/**********************************************************
	*	unsigned IO_EHI_ReadREG(unsigned uADDR, unsigned uSize);
	*
	*	Input		: uADDR = Address of EHI Register
	*				  uSize = Register Size in byte.
	*	Return		: Register value
	*	Description	: Read EHI Register
	************************************************************/
	unsigned IO_EHI_ReadREG(unsigned uADDR, unsigned uSize);

	/**********************************************************
	*	unsigned IO_EHI_ReadST(void);
	*
	*	Input		:
	*	Output		:
	*	Return		: EHST value
	*
	*	Description	: Read EHST Register
	************************************************************/
	unsigned IO_EHI_ReadST(void);

	/**********************************************************
	*	unsigned IO_EHI_WriteDATA(unsigned uADDR, unsigned uParam1, unsigned uParam2);
	*
	*	Input		: uADDR = Address of EHI Slave memory.
	*				  uParam1 = Pointer or Data value to write.
	*				  uParam2
	*					[31]   = Non-continuous(0), Continuous(1)
	*					[30:0] = Data amount to Transfer (word unit)
	*	Return		: 0
	*	Description	: Write data to Memory of Slave
	************************************************************/
	unsigned IO_EHI_WriteDATA(unsigned uADDR, unsigned uParam1, unsigned uParam2);

	/**********************************************************
	*	unsigned IO_EHI_ReadDATA(unsigned uADDR, unsigned uParam1, unsigned uParam2);
	*
	*	Input		: uADDR = Address of EHI Slave memory.
	*				  uParam1 = Pointer to store read data.
	*				  uParam2
	*					[31]   = Non-continuous (0), Continuous (1) transfer.
	*					[30:0] = Data amount to Transfer (word unit)
	*	Return		: Data value read or Pointer to bulk of data read.
	*	Description	: Read data from Memory of Slave
	************************************************************/
	unsigned IO_EHI_ReadDATA(unsigned uADDR, unsigned uParam1, unsigned uParam2);
#else
	/**********************************************************
	*	void IO_EHI_InitDRVS(unsigned uCONFIG);
	*
	*	Input		: uCONFIG = Configuration Parameter
	*					Hw0 = 68000 (1), x86 (0) interface
	*					Hw2 = 8bit (1), 16bit (0) interface
	*					Hw3 = used as Ready signal (1), used as Interrupt signal (0)
	*					Hw4 = Active Low Ready signal (1), Active High Ready signal (0)
	*	Return		:
	*	Description	: Initialize EHI I/F module at the Slave Site.
	************************************************************/
	void IO_EHI_InitDRVS(unsigned uCONFIG);
#endif


/************************************************************************
*	Timer/Counter
************************************************************************/
#if defined(TCC92XX) || defined(TCC89XX)
	#define	IO_TMR_IREQT0	HwTIREQ_TI0
	#define	IO_TMR_IREQT1	HwTIREQ_TI1
	#define	IO_TMR_IREQT2	HwTIREQ_TI2
	#define	IO_TMR_IREQT3	HwTIREQ_TI3
	#define	IO_TMR_IREQT4	HwTIREQ_TI4
	#define	IO_TMR_IREQT5	HwTIREQ_TI5
	#define	IO_TMR_ClearTIREQ(X)	{ HwTMR->TIREQ = 1 << (X); }
#else
#ifdef TCC79XX
	#define	IO_TMR_IREQT0	HwTIREQ_TI0
	#define	IO_TMR_IREQT1	HwTIREQ_TI1
	#define	IO_TMR_IREQT2	HwTIREQ_TI2
	#define	IO_TMR_IREQT3	HwTIREQ_TI3
	#define	IO_TMR_IREQT4	HwTIREQ_TI4
	#define	IO_TMR_IREQT5	HwTIREQ_TI5
#endif
#define	IO_TMR_ClearTIREQ(X)	{ HwTIREQ = 1 << (X); }
#endif

/**********************************************************
*	void IO_TMR_SetTIMER(unsigned uCH, unsigned uCTRL, unsigned uTREF, unsigned uTMREF);
*
*	Input		: uCH = Select timer channel. 0~5 is available.
*				  uCTRL = Timer Control Register (HwTCFG) value.
*				  uTREF = Timer Reference Register (HwTREF) value.
*				  uTMREF = Timer Middle Reference Register (HwTMREF) value.
*	Return		:
*	Description	: Set and Enable a Timer/Counter (timer is automatically enabled regardless of uCTRL value)
**********************************************************/
void IO_TMR_SetTIMER(unsigned uCH, unsigned uCTRL, unsigned uTREF, unsigned uTMREF);

/**********************************************************
*	unsigned IO_TMR_GetTIMER(unsigned uCH);
*
*	Input		: uCH = Select timer channel. 0~5 is available.
*	Return		: Current TCNT value.
*	Description	: Get the current count value of channel.
**********************************************************/
unsigned IO_TMR_GetTIMER(unsigned uCH);

/**********************************************************
*	void IO_TMR_DisableTIMER(unsigned uCH);
*
*	Input		: uCH = Select timer channel. 0~5 is available.
*	Return		:
*	Description	: Disable a Timer
**********************************************************/
void IO_TMR_DisableTIMER(unsigned uCH);

/**********************************************************
*	void IO_TMR_EnableTIMER(unsigned uCH);
*
*	Input		: uCH = Select timer channel. 0~5 is available.
*	Return		:
*	Description	: Enable a Timer
**********************************************************/
void IO_TMR_EnableTIMER(unsigned uCH);

#define	DBG_MAX_MEASURE		32
#define	START_MEASURE		1
#define	STOP_MEASURE		0

typedef	struct
{
	unsigned	uStamp;				// Time Stamp value at the start time.
	char		*pDescription;		// Timer description string.
	unsigned	uPreCH;				// Bitmap of Channels that must to be enabled ahead to measure this channel.
	unsigned	uMin;				// 1tic ~= 2.67us
	unsigned	uMax;				// 1tic ~= 2.67us
	unsigned	long long	uSum;	// summation of duration
	unsigned	uNum;				// Number of durations measured. The average duration is acquired by (uSum / uNum)
} sDBG_Timer;

extern	sDBG_Timer	DBG_Timer[DBG_MAX_MEASURE];
extern	unsigned	uMEA_CTRL, uMEA_STATE;

/**********************************************************
*	void IO_TMR_InitMEASURE(unsigned uCH);
*
*	Input		: uCH = Select a channel for initialize. (0xFFFFFFFF, 0 ~ 31 are possible, 0xFFFFFFFF means all of channel)
*						(this is not same as physical timer channel number. this uses TIMER channel 4 only)
*	Return		:
*	Description	: Initialize MEASURE variables.
*				  If all of channel should be initialized, use -1 as channel number.
**********************************************************/
void IO_TMR_InitMEASURE(unsigned uCH);

/**********************************************************
*	void IO_TMR_StartMEASURE(unsigned uCH, char *pDescriptor);
*
*	Input		: uCH = Select a channel for measure. (0 ~ 31 are possible)
*						(this is not same as physical timer channel number. this uses TIMER channel 4 only)
*				  pDescriptor = string for describing this channel.
*	Return		:
*	Description	: Start the timer for measuring a duration.
*				  This uses only timer/counter 4.
*				  It can measure the min/max/avg time duration of certain operation with limited period.
**********************************************************/
void IO_TMR_StartMEASURE(unsigned uCH, unsigned uPreCH, char *pDescriptor);
void IO_TMR_StopMEASURE(unsigned uCH);
void IO_TMR_StartMEASURE_NoIRQCnt(unsigned uCH, unsigned uPreCH, char *pDescriptor);
unsigned IO_TMR_StopMEASURE_NoIRQCnt(unsigned uCH);

/**********************************************************
*	void IO_TMR_FinishMEASURE(unsigned uCH);
*
*	Input		: uCH = Select a channel for measure. (0 ~ 31 are possible)
*						(this is not same as physical timer channel number. this uses TIMER channel 4 only)
*	Return		:
*	Description	: Finish the timer for measuring a duration.
*				  This uses only timer/counter 4.
*				  It stops measuring, and update min/max/sum/num field.
**********************************************************/
void IO_TMR_FinishMEASURE(unsigned uCH);

/**********************************************************
*	void IO_TMR_GetMEASURE(unsigned uCH, unsigned *pAvg, unsigned *pNum, unsigned *pMin, unsigned *pMax);
*
*	Input		: uCH = Select a channel for measure. (0 ~ 31 are possible)
*						(this is not same as physical timer channel number. this uses TIMER channel 4 only)
*				  pAvg = pointer for containing average time
*				  pNum = pointer for containing number of times
*				  pMin = pointer for containing minimum time
*				  pMax = pointer for containing maximum time
*				  *) all pointers can be zero not to contain values.
*	Return		:
*	Description	:
**********************************************************/
void IO_TMR_GetMEASURE(unsigned uCH, unsigned *pAvg, unsigned *pNum, unsigned *pMin, unsigned *pMax);

// Time Duration Meter
#ifndef USE_DOMEASURE
	#define	DoMEASURE(CH, ONOFF, PreCH, Descriptor)			{;}
#else
	#define	DoMEASURE(CH, ONOFF, PreCH, Descriptor)			\
	{														\
		if (uMEA_CTRL & (1 << (CH)))						\
		{													\
			if ((ONOFF) != 0)								\
				IO_TMR_StartMEASURE(CH, PreCH, Descriptor);	\
			else											\
				IO_TMR_FinishMEASURE(CH);					\
		}													\
	}
#endif

/**********************************************************
*	unsigned int IO_TMR_Get32bitValue(void);
*
*	Input		:
*	Return		: 32-bit Counter Current Value (main counter) 
*	Description	:
**********************************************************/
unsigned int IO_TMR_Get32bitValue(void);


/************************************************************************
*	UART Controller
************************************************************************/

/**********************************************************
*
*	void IO_UART_Test(unsigned uCH);
*
*	Input		: uCH = channel number (0~2)
*	Output		:
*	Return		:
*
*	Description	: Test UART functions.
**********************************************************/
void IO_UART_Test(unsigned uCH);

/**********************************************************
*
*	void IO_UART_Init(unsigned uCH);
*
*	Input		: uCH = channel number (0~2)
*	Output		:
*	Return		:
*
*	Description	: Initialize UART registers and UART Clocks.
**********************************************************/
void IO_UART_Init(unsigned uCH);

/**********************************************************
*
*	void IO_UART_WriteString(unsigned uCH, const char *ccptrString)
*
*	Input		: uCH = channel number (0~2)
*				  ccptrString = string to print
*	Output		:
*	Return		:
*
*	Description	: print argument string considering '\n' as '\r'+'n'.
**********************************************************/
void IO_UART_WriteString(unsigned uCH, const char *ccptrString);

/**********************************************************
*
*	void IO_UART_WriteByte(unsigned uCH, char cChar)
*
*	Input		: uCH = channel number (0~2)
*				  cChar = character to print
*	Output		:
*	Return		:
*
*	Description	: Print one character. Consider '\n' as '\r' + '\n'.
**********************************************************/
void IO_UART_WriteByte(unsigned uCH, char cChar);

/**********************************************************
*
*	int IO_UART_InputByte(unsigned uCH, char *cptrChar)
*
*	Input		: uCH = channel number (0~2)
*				  cptrChar = pointer for receiving character
*	Output		:
*	Return		: 0 = there is no input.
*				  1 = there exist at least one byte and it is contained at the *cptrChar.
*	Description	: Check there is at least one character in buffer, and if exist, store the code to *cptrChar and return 1.
*				   Or return 0.
**********************************************************/
int IO_UART_InputByte(unsigned uCH, char *cptrChar);

/**********************************************************
*
*	void IO_UART0_PutExtChar(unsigned uCH, const unsigned char cucChar)
*
*	Input		: uCH = channel number (0~2)
*				  cucChar = character code
*	Output		: Send one character
*	Return		:
*
*	Description	: Print cucChar if cucChar is printable code or print '.' character.
**********************************************************/
void IO_UART_PutExtChar(unsigned uCH, const unsigned char cucChar);

/**********************************************************
*
*	char IO_UART_GetChar(unsigned uCH)
*
*	Input		: uCH = channel number (0~2)
*	Output		:
*	Return		: Received character
*
*	Description	: Wait until at least one character is received & return the code with echoing.
**********************************************************/
char IO_UART_GetChar(unsigned uCH);

/**********************************************************
*
*	char IO_UART_GetCh(unsigned uCH)
*
*	Input		: uCH = channel number (0~2)
*	Output		:
*	Return		: Received character
*
*	Description	: Wait until at least one character is received & return the code without echoing.
**********************************************************/
char IO_UART_GetCh(unsigned uCH);


/************************************************************************
*	RTC Controller
************************************************************************/
typedef struct {
	unsigned	char		second;		// (0 ~ 59)
	unsigned	char		minute;		// (0 ~ 59)
	unsigned	char		hour;		// (0 ~ 23)
	unsigned	char		day;		// Day of Week (SUN=0, MON, TUE, WED, THR, FRI, SAT=6)

	unsigned	char		date;		// (1 ~ 28,29,30,31)
	unsigned	char		month;		// (1 ~ 12)
	unsigned short	year;
} IO_RTC_DATETIME;

/**********************************************************************************
*	unsigned IO_RTC_Init(unsigned uWUPolarity);
*
*	Input		:
*	Return		: 0
*	Description	: Initialize RTC, RTC is disabled.
*				  This function should not be called in normal case.
**********************************************************************************/
unsigned IO_RTC_Init(unsigned uWUPolarity);

/**********************************************************************************
*	unsigned IO_RTC_Start(void);
*
*	Input		:
*	Return		: value of RTCCON register after enabled.
*	Description	: RTC starts to operate.
**********************************************************************************/
unsigned IO_RTC_Start(void);

/**********************************************************************************
*	unsigned IO_RTC_Stop(void);
*
*	Input		:
*	Return		: value of RTCCON register after disabled.
*	Description	: RTC stops to operate.
**********************************************************************************/
unsigned IO_RTC_Stop(void);

/**********************************************************************************
*	unsigned IO_RTC_SetCON(unsigned uRTCCON);
*
*	Input		: uRTCCON = value for RTCCON register.
*	Return		: value of RTCCON register after updated.
*	Description	: Set RTCCON register as wanted value.
**********************************************************************************/
unsigned IO_RTC_SetCON(unsigned uRTCCON);

/**********************************************************************************
*	unsigned IO_RTC_GetTIME(IO_RTC_DATETIME *pTime);
*	Input		: pTime = structure for getting RTC Time. (each element has decimal (non-BCD) value.)
*	Return		: 0 = OK
*				  1 = Read value has some error, and pTime contains predefined initial values
*					 for calling IO_RTC_SetTIME();
*	Description	: Get current time of RTC.
*				  RTC has no power-on reset feature, it has random value after power-on.
*				  It is reported by return value of 1 so user must re-set the current time.
**********************************************************************************/
unsigned IO_RTC_GetTIME(IO_RTC_DATETIME *pTime);

/**********************************************************************************
*	unsigned	IO_RTC_IsValidTime(void);
*
*	Input		: void
*	Return		: 0 = OK
*				  1 = Read value has some error
*	Description	:This function is made to check whether current time setteing is correct or not
*				 Maybe this function will be called by only the Janus core and will be used for setting
*				 current RTC state(SET or UNSET).
**********************************************************************************/
unsigned IO_RTC_IsValidTime(void);

/**********************************************************************************
*	unsigned IO_RTC_SetTIME(RTC_APP_DATETIME *pTime);
*
*	Input		: pTime = structure for setting RTC Time (refer to IO_RTC_GetTIME())
*	Return		: 0
*	Description	:
**********************************************************************************/
unsigned IO_RTC_SetTIME(IO_RTC_DATETIME *pTime);

/**********************************************************************************
*	unsigned IO_RTC_SetBCDALARM(IO_RTC_DATETIME *pTime, unsigned uCON);
*
*	Input		: pTime = structure for setting RTC Time. (each element has BCD format)
*				  uCON = same as HwRTCALM register map (refer to datasheet)
*	Return		: 0
*	Description	: Set ALARM time. It is not supported all combination of ALARM time.
**********************************************************************************/
unsigned IO_RTC_SetBCDALARM(IO_RTC_DATETIME *pTime, unsigned uCON);

/**********************************************************************************
*	unsigned IO_RTC_GetBCDTIME(RTC_APP_DATETIME *pTime);
*
*	Input		: pTime = structure for getting RTC Time. (BCD format)
*	Return		: 0
*	Description	: The current time is stored to structure pointed by pTime.
**********************************************************************************/
unsigned IO_RTC_GetBCDTIME(IO_RTC_DATETIME *pTime);

/**********************************************************************************
*	unsigned IO_RTC_SetBCDTIME(RTC_APP_DATETIME *pTime);
*
*	Input		: pTime = structure for setting RTC Time. (BCD format)
*	Return		: 0
*	Description	:
**********************************************************************************/
unsigned IO_RTC_SetBCDTIME(IO_RTC_DATETIME *pTime);

/**********************************************************************************
*	unsigned	IO_RTC_BCD2DEC( unsigned nBCD );
*
*	Input		: nBCD = BCD format value
*	Return		: Equivalent value of hexa-decimal format
*	Description	:
**********************************************************************************/
unsigned IO_RTC_BCD2DEC( unsigned nBCD );

/**********************************************************************************
*	unsigned	IO_RTC_DEC2BCD( unsigned uDEC );
*
*	Input		: nDEC = hexa-decimal format value
*	Return		: Equivalent value of BCD format
*	Description	:
**********************************************************************************/
unsigned IO_RTC_DEC2BCD( unsigned nDEC );

/**********************************************************************************
*	unsigned IO_RTC_SetALARM(IO_RTC_DATETIME *pTime);
*
*	Input		: pTime = structure for setting RTC Time. (Hexa-decimal format)
*	Output		:
*	Return		: 0 = OK
*	Description	:
**********************************************************************************/
unsigned IO_RTC_SetALARM(IO_RTC_DATETIME *pTime);

/**********************************************************************************
*	unsigned IO_RTC_GetALARM(IO_RTC_DATETIME *pTime);
*
*	Input		: pTime = structure for setting RTC Time. (Hexa-decimal format)
*	Output		:
*	Return		: 0 = OK
*	Description	:
**********************************************************************************/
unsigned IO_RTC_GetALARM(IO_RTC_DATETIME *pTime);

/**********************************************************************************
*	unsigned IO_RTC_WriteREG(volatile unsigned *pReg, unsigned uValue);
*
*	Input		: pReg = Register Address (BCD register address)
*				  uValue = Register Value
*	Return		: Register Value after writing.
*	Description	:
**********************************************************************************/
unsigned IO_RTC_WriteREG(volatile unsigned *pReg, unsigned uValue);

/**********************************************************************************
*	unsigned	IO_RTC_GetBCDALARM(RTC_APP_DATETIME *pTime);
*
*	Input		: pTime = structure for setting RTC Time. (BCD format)
*	Output		:
*	Return		: HwRTCALM value
*	Description	:
**********************************************************************************/
unsigned IO_RTC_GetBCDALARM(IO_RTC_DATETIME *pTime);

/**************************************************************************
 FUNCTION NAME : IO_RTC_DisableALMINT
 
 DESCRIPTION : Disable Alarm Interrupt (ALMINT)
 
 INPUT   : void - Parameter
 OUTPUT  : void - Return Type
 REMARK  : 
**************************************************************************/
void IO_RTC_DisableALMINT(void);

/**********************************************************************************
*	unsigned	IO_RTC_SetWKUP(IO_RTC_DATETIME *pTime, unsigned uActiveMode);
*
*	Input		: pTime = structure for setting RTC Time. (Hexa-decimal format)
*				  uActiveMode : 1 = PMWKUP is active HIGH, 0 = PMWKUP is active LOW
*	Output		:
*	Return		: 0 = OK
*	Description	: Set Wakeup Time. (System can be powered off & rebooted by wake-up signal)
**********************************************************************************/
unsigned IO_RTC_SetWKUP(IO_RTC_DATETIME *pTime, unsigned uActiveMode);



/************************************************************************
*	Debug Monitor
************************************************************************/
// Print Character for time stamp.
#define	ST_ON	'1'
#define	ST_OFF	'0'
#ifndef WINVER
#ifdef	USE_IO_DEBUG
	//----------definition for GLOBAL monitoring ([7:0] are allocated)
	#define	IO_DBG_Init			IO_DBG_Init_
	#define	IO_DBG_Printf		IO_DBG_Printf_
	#define	IO_DBG_SerialPrintf	IO_DBG_SerialPrintf_
	#define	IO_DBG_Putc			IO_DBG_Putc_
	#define	IO_DBG_TIME			HwTCNT4

	//----------definition for GLOBAL monitoring ([-:0] are allocated)
	#define	DBG_CTRL_USBD		Hw0
	#define	DBG_CTRL_NFC		Hw1
	#define	DBG_CTRL_DTCM		Hw2
	#define	DBG_CTRL_SSFDC		Hw3
	#define	DBG_CTRL_SSFDC_DRV	Hw4
	#define	DBG_CTRL_FILE		Hw5
	#define	DBG_CTRL_FAT		Hw6
	#define	DBG_CTRL_MP3DEC	Hw7
	//----------stamp definition ([31:-] are allocated)
	#define	DBG_CTRL_STAMP0	Hw31
	#define	DBG_CTRL_STAMP1	Hw30
	#define	DBG_CTRL_STAMP2	Hw29
	#define	DBG_CTRL_STAMP3	Hw28
	#define	DBG_CTRL_STAMP4	Hw27
	#define	DBG_CTRL_STAMP5	Hw26
	#define	DBG_CTRL_STAMP6	Hw25
	#define	DBG_CTRL_STAMP7	Hw24
#else
	// Disable All of monitoring functions
	#define	IO_DBG_Init()			{;}
	#define	IO_DBG_Printf(...)	
	#define	IO_DBG_SerialPrintf(...)
	#define	IO_DBG_Putc			
	#define	IO_DBG_TIME			0

	#define	DBG_CTRL_USBD		0
	#define	DBG_CTRL_NFC		0
	#define	DBG_CTRL_DTCM		0
	#define	DBG_CTRL_SSFDC		0
	#define	DBG_CTRL_SSFDC_DRV	0
	#define	DBG_CTRL_FILE		0
	#define	DBG_CTRL_FAT		0
	#define	DBG_CTRL_MP3DEC		0
	#define	DBG_CTRL_STAMP0		0
	#define	DBG_CTRL_STAMP1		0
	#define	DBG_CTRL_STAMP2		0
	#define	DBG_CTRL_STAMP3		0
	#define	DBG_CTRL_STAMP4		0
	#define	DBG_CTRL_STAMP5		0
	#define	DBG_CTRL_STAMP6		0
	#define	DBG_CTRL_STAMP7		0
#endif
#endif
#ifdef	CHECK_SPEED
	// Stamp for
	#define	MakeSTAMP0(X)	{if (uDBG_CTRL & DBG_CTRL_STAMP0) IO_DBG_Printf("[%05x]T0(%c)\n", IO_DBG_TIME, X);}

	// Stamp for SSFDC_FS_WriteSector() execution time.
	#define	MakeSTAMP1(X)	{if (uDBG_CTRL & DBG_CTRL_STAMP1) IO_DBG_Printf("[%05x]T1(%c)\n", IO_DBG_TIME, X);}

	// Stamp for SSFDC_WriteSector() execution time.
	#define	MakeSTAMP2(X)	{if (uDBG_CTRL & DBG_CTRL_STAMP2) IO_DBG_Printf("[%05x]T2(%c)\n", IO_DBG_TIME, X);}

	// Stamp for read_file() or write_file() execution time.
	#define	MakeSTAMP3(X)	{if (uDBG_CTRL & DBG_CTRL_STAMP3) IO_DBG_Printf("[%05x]T3(%c)\n", IO_DBG_TIME, X);}

	// Stamp for
	#define	MakeSTAMP4(X)	{if (uDBG_CTRL & DBG_CTRL_STAMP4) IO_DBG_Printf("[%05x]T4(%c)\n", IO_DBG_TIME, X);}

	// Stamp for DISK_WriteSector() execution time.
	#define	MakeSTAMP5(X)	{if (uDBG_CTRL & DBG_CTRL_STAMP5) IO_DBG_Printf("[%05x]T5(%c)\n", IO_DBG_TIME, X);}

	// Stamp for pure NAND Data transfer execution time.
	#define	MakeSTAMP6(X)	{if (uDBG_CTRL & DBG_CTRL_STAMP6) IO_DBG_Printf("[%05x]T6(%c)\n", IO_DBG_TIME, X);}

	// Stamp for SSFDC_IO_R/W() execution time.
	#define	MakeSTAMP7(X)	{if (uDBG_CTRL & DBG_CTRL_STAMP7) IO_DBG_Printf("[%05x]T7(%c)\n", IO_DBG_TIME, X);}
#else
	#define	MakeSTAMP0(X)	{;}
	#define	MakeSTAMP1(X)	{;}
	#define	MakeSTAMP2(X)	{;}
	#define	MakeSTAMP3(X)	{;}
	#define	MakeSTAMP4(X)	{;}
	#define	MakeSTAMP5(X)	{;}
	#define	MakeSTAMP6(X)	{;}
	#define	MakeSTAMP7(X)	{;}
#endif

// definition for USB device monitoring
#define	IO_USBD_Printf			if (uDBG_CTRL & DBG_CTRL_USBD) IO_DBG_Printf
#define	IO_USBD_Putc(x)			if (uDBG_CTRL & DBG_CTRL_USBD) IO_DBG_Putc(x)
// definition for NFC monitoring
#define	IO_NFC_Printf			if (uDBG_CTRL & DBG_CTRL_NFC) IO_DBG_Printf
#define	IO_NFC_Putc(x)			if (uDBG_CTRL & DBG_CTRL_NFC) IO_DBG_Putc(x)
// definition for DTCM monitoring
#define	IO_DTCM_Printf			if (uDBG_CTRL & DBG_CTRL_DTCM) IO_DBG_Printf
#define	IO_DTCM_Putc(x)			if (uDBG_CTRL & DBG_CTRL_DTCM) IO_DBG_Putc(x)
// definition for SSFDC monitoring
#define	IO_SSFDC_Printf			if (uDBG_CTRL & DBG_CTRL_SSFDC) IO_DBG_Printf
#define	IO_SSFDC_Putc(x)		if (uDBG_CTRL & DBG_CTRL_SSFDC) IO_DBG_Putc(x)
// definition for SSFDC_DRV monitoring
#define	IO_SSFDC_DRV_Printf		if (uDBG_CTRL & DBG_CTRL_SSFDC_DRV) IO_DBG_Printf
#define	IO_SSFDC_DRV_Putc(x)	if (uDBG_CTRL & DBG_CTRL_SSFDC_DRV) IO_DBG_Putc(x)
// definition for FILE monitoring
#define	IO_FILE_Printf			if (uDBG_CTRL & DBG_CTRL_FILE) IO_DBG_Printf
#define	IO_FILE_Putc(x)			if (uDBG_CTRL & DBG_CTRL_FILE) IO_DBG_Putc(x)
// definition for FAT monitoring
#define	IO_FAT_Printf			if (uDBG_CTRL & DBG_CTRL_FAT) IO_DBG_Printf
#define	IO_FAT_Putc(x)			if (uDBG_CTRL & DBG_CTRL_FAT) IO_DBG_Putc(x)
// definition for MP3DEC monitoring
#define	IO_MP3DEC_Printf		if (uDBG_CTRL & DBG_CTRL_MP3DEC) IO_DBG_Printf
#define	IO_MP3DEC_Putc(x)		if (uDBG_CTRL & DBG_CTRL_MP3DEC) IO_DBG_Putc(x)

void IO_DBG_Printf_(char *format, ...);
void IO_DBG_SerialPrintf_(char *format, ...);
void IO_DBG_Sprintf(char *dst, char *format, ...);
void IO_DBG_Putc_(char c);
void IO_DBG_Init_(void);

extern	const unsigned	IO_DBG_CRC_TABLE[256];
unsigned IO_DBG_CalcCRC32_s(unsigned crc_in, unsigned data, unsigned size);

/**********************************************************************************
*	int stod(char *s);
*
*	Input		: s = string of decimal or hexa-decimal format
*	Return		: converted value equivalent with s.
*	Description	: if input string starts from "0x", it is regarded as hexa-decimal format,
*				  or it is regarded as decimal format.
**********************************************************************************/
int stod(char *s);

extern char		cDBG_txbuf[];		// all of string that is printed by IO_DBG_Printf() is stored at this buffer.
									// this is ring type buffer.
extern char		cDBG_rxbuf[];
extern char		*p_prtbuf, cDirectPrint;
extern unsigned	uDBG_CTRL, uDBG_txbuf_length;


/************************************************************************
*	UART
************************************************************************/

#if defined(TCC92XX) || defined(TCC89XX)
// Should be fixed.
/*
	#define	IO_UART_CH		0

	#define	IO_UART_LSR(X)	(((X) == 0) ? HwUART0_UTLSR : ((X) == 1) ? HwUART1_LSR : HwUART2_UTLSR)
	#define	IO_UART_RCVD	Hw0
	#define	IO_UART_TF(X)	(((X) == 0) ? HwUART0_UTLSR_TF_NOSTR : ((X) == 1) ? HwUART1_LSR_THRE_ON : HwUART2_UTLSR_TF_NOSTR)
	#define	IO_UART_RXD(X)	(((X) == 0) ? HwUART0_UTRXD : ((X) == 1) ? HwUART1_RBR : HwUART2_UTRXD)
	#define	IO_UART_TXD(X)	(((X) == 0) ? HwUART0_UTTXD : ((X) == 1) ? HwUART1_THR : HwUART2_UTTXD)
	#define	IO_UART_WaitTXRDY(X)										\
	{																	\
		if ((X) == 0)														\
			while (ISONE(HwUART0_UTLSR, HwUART0_UTLSR_TF_NOSTR));		\
		else if ((X) == 1)													\
			while (ISZERO(HwUART1_LSR, HwUART1_LSR_THRE_ON));			\
		else if ((X) == 2)													\
			while (ISONE(HwUART2_UTLSR, HwUART2_UTLSR_TF_NOSTR));		\
	}
	#define	IO_UART_WaitRXRDY(X)										\
	{																	\
		if ((X) == 0)														\
			while (ISZERO(HwUART0_UTLSR, HwUART0_UTLSR_RA_RECV));		\
		else if ((X) == 1)													\
			while (ISZERO(HwUART1_LSR, HwUART1_LSR_DR_ON));			\
		else if ((X) == 2)													\
			while (ISZERO(HwUART2_UTLSR, HwUART2_UTLSR_RA_RECV));		\
	}
	#define	IO_UART_RXRDY(X)	( ((X) == 0) ? HwUART0_UTLSR & IO_UART_RCVD : \
								  ((X) == 1) ? HwUART1_LSR & IO_UART_RCVD : HwUART2_UTLSR & IO_UART_RCVD )
	#define	IO_UART_LSR_ERR(X)	( ((X) == 0) ? (HwUART0_UTLSR_FE_ERR | HwUART0_UTLSR_PE_ERR) :		\
								  ((X) == 1) ? (HwUART1_LSR_FE_ON | HwUART1_LSR_PE_ON) :			\
								  			(HwUART2_UTLSR_FE_ERR | HwUART2_UTLSR_PE_ERR) )
	#define	IO_UART_ERR_Frame(X)	( ((X) == 0) ? HwUART0_UTLSR_FE_ERR : \
									  ((X) == 1) ? HwUART1_LSR_FE_ON : HwUART2_UTLSR_FE_ERR )
	#define	IO_UART_ERR_Parity(X)	( ((X) == 0) ? HwUART0_UTLSR_PE_ERR : \
									  ((X) == 1) ? HwUART1_LSR_PE_ON : HwUART2_UTLSR_PE_ERR )
*/									  
#endif

void IO_UART_Test(unsigned uCH);
void IO_UART_Init(unsigned uCH);
void IO_UART_WriteString(unsigned uCH, const char *ccptrString);
void IO_UART_WriteByte(unsigned uCH, char cChar);
int IO_UART_InputByte(unsigned uCH, char *cptrChar);
void IO_UART_PutExtChar(unsigned uCH, const unsigned char cucChar);
char IO_UART_GetChar(unsigned uCH);
char IO_UART_GetCh(unsigned uCH);

/************************************************************************
*	GSIO & SPIS
************************************************************************/

#define	IO_GSIO_HwEN				Hw31
#define	IO_GSIO_HwMSB1ST			Hw30
#define	IO_GSIO_HwLSB1ST			HwZERO
#define	IO_GSIO_HwWSIZE(X)			(((X)-1)*Hw26)
#define	IO_GSIO_HwWSDYNAMIC			Hw25
#define	IO_GSIO_HwDIV(X)			((X) * Hw18)
#define	IO_GSIO_HwWSFIX				HwZERO
#define	IO_GSIO_HwPOSSYNC			Hw17
#define	IO_GSIO_HwNEGSYNC			HwZERO
#define	IO_GSIO_HwMASKLSCK			Hw16
#define	IO_GSIO_HwIEN				Hw15
#define	IO_GSIO_HwTXDLY(X)			((X)*Hw13)
#define	IO_GSIO_HwFRMACTHIGH		Hw12
#define	IO_GSIO_HwFRMACTLOW			HwZERO
#define	IO_GSIO_HwFRMST(X)			((X)*Hw6)
#define	IO_GSIO_HwFRMEND(X)			((X)*Hw0)

typedef	volatile struct
{
	unsigned	DO;
	unsigned	DI;
	unsigned	CTRL;
	unsigned	dummy;
} sHwGSIO;

#define	IO_SPIS_HwTXFIFOCNT(X)		(((X)-1)*Hw29)
#define	IO_SPIS_HwRXFIFOCNT(X)		(((X)-1)*Hw26)
#define	IO_SPIS_HwISRC_RXCFULL		(0*Hw8)
#define	IO_SPIS_HwISRC_RXFEMPTY		(1*Hw8)
#define	IO_SPIS_HwISRC_RXFFULL		(2*Hw8)
#define	IO_SPIS_HwISRC_TXCFULL		(4*Hw8)
#define	IO_SPIS_HwISRC_TXFEMPTY		(5*Hw8)
#define	IO_SPIS_HwISRC_TXFFULL		(6*Hw8)
#define	IO_SPIS_HwMSB1ST			Hw5
#define	IO_SPIS_HwLSB1ST			HwZERO
#define	IO_SPIS_HwWSIZE(X)			(((X)/8-1)*Hw3)
#define	IO_SPIS_HwPOSSYNC			Hw2
#define	IO_SPIS_HwNEGSYNC			HwZERO
#define	IO_SPIS_HwIEN				Hw1
#define	IO_SPIS_HwEN				Hw0
typedef struct
{
	unsigned	CTRL;
	unsigned	DO;
	unsigned	DI;
	unsigned	dummy;
} sHwSPIS;

#define	IO_GSIO_WaitBUSY(CH, Tout)		{ Tout = 300;	 while (ISONE(HwGSGCR, Hw0 << (CH)) && (Tout --));	}
#define	IO_SPIS_IsRXEMPTY(pSPIS)		ISONE((pSPIS)->CTRL, HwSPCTRL_EMP_RX)
#define	IO_SPIS_IsRXFULL(pSPIS)			ISONE((pSPIS)->CTRL, HwSPCTRL_FUL_RX)
#define	IO_SPIS_WaitRX(pSPIS, Tout)		{ Tout = 300; while (IO_SPIS_IsRXEMPTY(pSPIS) && (Tout --));	}
#define	IO_SPIS_IsTXEMPTY(pSPIS)		ISONE((pSPIS)->CTRL, HwSPCTRL_EMP_TX)
#define	IO_SPIS_IsTXFULL(pSPIS)			ISONE((pSPIS)->CTRL, HwSPCTRL_FUL_TX)
#define	IO_SPIS_WaitTX(pSPIS, Tout)		{ Tout = 300; while (IO_SPIS_IsTXFULL(pSPIS) && (Tout --));	}

/**********************************************************
*	void IO_GSIO_InitCH(unsigned uCH, unsigned uCONTROL, unsigned uSCKfreq);
*
*	Input		: uCH = Select GSIO Master channel. 0~1 is available.
*				  uCONTROL = GSIO control flags
*					[31] = Enable(1)
*					[30] = MSB First (1)
*					[29:26] = Word Size (bit unit)
*					[25] = Word size is dynamically controlled by GSDO register (1)
*					[17] = Data transition occurs at the SCK rising (1)
*					[16] = Mask out the last SCK (1)
*					[15]	= Enable Interrupt (1)
*					[14:13] = Transmission starting delay (1~3 is available)
*					[12] = FRM is high active pulse
*					[11:6] = FRM pulse start position
*					[5:0] = FRM pulse end position
*				  uSCKfreq = GSIO SCK clock frequency
*	Return		:
*	Description	: Set GSIO host channel
**********************************************************/
void IO_GSIO_InitCH(unsigned uCH, unsigned uCONTROL, unsigned uSCKfreq);

/**********************************************************
*	void IO_SPIS_InitCH(unsigned uCH, unsigned uCONTROL);
*
*	Input		: uCH = Select SPI slave channel. 0 is available.
*				  uCONTROL = GSIO control flags
*					[31:29] = TX FIFO count
*					[28:26] = RX FIFO count
*					[10:8] = Interrupt Source Selection
*						0 : RX FIFO Counter Full
*						1 : RX FIFO Empty
*						2 : RX FIFO Full
*						4 : TX FIFO Counter Full
*						5 : TX FIFO Empty
*						6 : TX FIFO Full
*					[5] = MSB First (1)
*					[4:3] = Word Size
*						0 : 8bit, 1 : 16bit, 2 : 24bit, 3 : 32bit
*					[2] = Data transition occurs at the SCK rising (1)
*					[1] = Enable Interrupt (1)
*					[0] = Enable (1)
*	Return		:
*	Description	: Set SPI slave channel
**********************************************************/
void IO_SPIS_InitCH(unsigned uCH, unsigned uCONTROL);

enum
{
	LCD_18BIT_SET,
	LCD_16BIT_SET,	
	LCD_8BIT_SET
};


/************************************************************************
*	Mail Box
************************************************************************/
typedef	volatile struct {
	unsigned	TXD;
	unsigned	uRsv1[7];
	unsigned	RXD;
	unsigned	uRsv2[7];
	unsigned	CTR;
	unsigned	STR;
} sMBOX;

enum {
	MBOX_MAIN,
	MBOX_SUB
};


/////////////////////////////////////////////

#if !defined(_LINUX_)
typedef	int (*ICallBack)(int num);
#endif

#define	ALIGN_UP(X, Y)				(((unsigned int)(X)+(unsigned int)(Y)-1) & ~((unsigned int)(Y)-1))


/************************************************************************
*	GPSB
************************************************************************/
typedef struct
{
	volatile unsigned long *BASE;
	unsigned	uFRM, uSCK, uSDI, uSDO;
	unsigned	uMODE;
} sGPSBPORT;

typedef struct	tag_sGPSBPacket
{
	char		TxStatus;		// 0=Empty, 1=Valid, 2=Sent
	char		RxStatus;	// 0=Empty, 1=Wait, 2=Received
	char		dummy[2];
	unsigned	CurSize;
	unsigned	*TxBufBASE;
	unsigned	*RxBufBASE;
} sGPSBPacket;

typedef struct
{
	unsigned short	Num;		// total number of packet buffer.
	unsigned short	CurNum;		// current number of packet buffer.
	unsigned short	Head;		// Head
	unsigned short	Tail;			// Tail
	unsigned			MaxSize;		// Buffer Max size
	sGPSBPacket		*Pkt;		// Packet Array
} sGPSBPKTManager;

typedef struct
{
	unsigned	char *pBuffer;
	unsigned	short	uPktSize;
	unsigned	short	uIncSize;
	int		iRemainLength;
} sGPSBTXManager;

extern sGPSBPKTManager	gGPSBPKTManager[2];
extern sGPSBTXManager	gGPSBTXManager[2];

void IO_GPSB_WaitDONE(sGPSBPORT *pPORT);
void IO_GPSBSW_DelayLOOP(unsigned uDelay);
void IO_GPSBSW_SetFRM(sGPSBPORT *pPORT, unsigned uValue);
void IO_GPSBSW_InvFRM(sGPSBPORT *pPORT);
void IO_GPSBSW_SetSCK(sGPSBPORT *pPORT, unsigned uValue);
void IO_GPSBSW_InvSCK(sGPSBPORT *pPORT);
void IO_GPSBSW_SetSDO(sGPSBPORT *pPORT, unsigned uValue);
unsigned IO_GPSBSW_GetSDI(sGPSBPORT *pPORT);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      IO_GPSB_ConfigPORT(unsigned uCHPort, unsigned uSDO, unsigned uMode);
*  
*  DESCRIPTION : Configure GPSB Channel and its PORT.
*  
*  INPUT:
*			uCHPort	= GPSB Channel & I/O Port number
*			[7:0]	= channel number (0 or 1)
*			[8]		= I/O PORT number for GPSB Channel
*					GPSB channel 0 can use 3 ports. (port number 0 ~ 2 are used)
*						Port 0 : GPIO_E[19:16]
*						Port 1 : GPIO_A[27:24]
*						Port 2 : GPIO_C[15:12]
*					GPSB channel 1 can only use 1 port. (port number 0 is used)
*						Port 0 : GPIO_E[23:20]
*
*			uSDOSDI	= flag for using SDO or SDI pin
*			[1]		= 0 : don't use SDO, 1 : use SDO
*			[0]		= 0 : don't use SDI, 1 : use SDI
*
*			uMode	= 0 : for setting PORT as H/W, 1 : for setting PORT as GPIO
*  
*  OUTPUT:	
*  			0 = Successful
*			-1 = Illegal configuration.
*  
*  REMARK:	created on 2006년 11월 15일 수요일 오후 9:02:58    by vizirdo
**************************************************************************/
int IO_GPSB_ConfigPORT(unsigned uCHPort, unsigned uSDO, unsigned uMode);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      void IO_GPSB_InitCH(int iCH, unsigned uMode, unsigned uSCKfreq);
*  
*  DESCRIPTION : Initialize GPSB channel for Master mode only.
*  
*  INPUT:
*			iCH		= GPSB Channel & I/O Port number
*			[7:0]	= channel number (0 or 1)
*			[8]		= I/O PORT number for GPSB Channel
*					GPSB channel 0 can use 3 ports. (port number 0 ~ 2 are used)
*						Port 0 : GPIO_E[19:16]
*						Port 1 : GPIO_A[27:24]
*						Port 2 : GPIO_C[15:12]
*					GPSB channel 1 can only use 1 port. (port number 0 is used)
*						Port 0 : GPIO_E[23:20]
*
*			uMode = Mode control
*			[20] : PCS (0 = CS active low, 1 = CS active high)
*			[17] : PD (0 = RX data on rising edge SCK & TX data on falling edge SCK)
*					(1 = RX data on falling edge SCK & TX data on rising edge SCK)
*			[16] : PCK (0 = SCK starts from 0, 1 = SCK starts from 1)
*			[7] : Data shifting direction control. (0 = MSB first, 1 = LSB first)
*
*			uSCKfreq	= SCK frequency (with 100 Hz unit)
*			uSW		= 0 for H/W mode, 1 for S/W mode.
*  
*  OUTPUT:	void - Return Type
*  
*  REMARK:	created on 2006년 8월 7일 월요일 오전 11:35:56    by vizirdo
**************************************************************************/
void IO_GPSB_InitCH(int iCH, unsigned uMode, unsigned uSCKfreq, unsigned uSW);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned	IO_GPSB_SendCMD(int iCH, unsigned uCmd, unsigned uCmdBitSize, unsigned uContinuous);
*  
*  DESCRIPTION : Send Command through GPSB port.
*  
*  INPUT:
*			iCH	= channel number. (negative for using S/W SPI routine)
*			uCmd	= command value
*			uCmdBitSize	= bit size of command value.
*			uContinuous	= flag for controlling nCS line. 
*							if zero, nCS line goes to inactive state after the command value is sent.
*							if non-zero, nCS line remains in active state so additional process for the command can follow.
*  
*  OUTPUT:	unsigned - Return Type
*  			= response of command
*  
*  REMARK:	created on 2006년 8월 7일 월요일 오전 11:36:57    by vizirdo
**************************************************************************/
unsigned	IO_GPSB_SendCMD(int iCH, unsigned uCmd, unsigned uCmdBitSize, unsigned uContinuous);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned	IO_GPSB_TRxPKT(int iCH, void *pTxBuf, void *pRxBuf, unsigned uLength);
*  
*  DESCRIPTION : Transmit & Receive a packet data.
*  
*  INPUT:
*			iCH		= GPSB Channel & I/O Port number (same as IO_GPSB_InitCH)
*			pRxBuf	= base address of received data.
*			pTxBuf	= base address of transmitting data.
*			uLength	= data size (byte unit)
*  
*  OUTPUT:	unsigned - Return Type
*  			= 0
*  
*  REMARK:	created on 2006년 12월 21일 목요일 오후 11:04:33    by vizirdo
**************************************************************************/
unsigned	IO_GPSB_TRxPKT(int iCH, void *pTxBuf, void *pRxBuf, unsigned uLength);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned	IO_GPSB_SendDATA(int iCH, unsigned char *pData, unsigned uLength, unsigned uBitConfig);
*  
*  DESCRIPTION : Send data through GPSB port
*  
*  INPUT:
*			iCH	= channel number. (negative for using S/W SPI routine)
*			pData		= pointer of data to send.
*			uLength		= length of data to send.
*			uBitConfig	= bit size of GPSB port.
*  
*  OUTPUT:	unsigned - Return Type
*  			= 
*  
*  REMARK:	created on 2006년 8월 7일 월요일 오전 11:42:47    by vizirdo
**************************************************************************/
unsigned	IO_GPSB_SendDATA(int iCH, unsigned char *pData, unsigned uLength, unsigned uBitConfig);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned	IO_GPSB_RecvDATA(int iCH, unsigned char *pData, unsigned uLength, unsigned uBitConfig);
*  
*  DESCRIPTION : Receive data through GPSB port
*  
*  INPUT:
*			iCH	= channel number. (negative for using S/W SPI routine)
*			pData		= pointer of data to receive.
*			uLength		= length of data to receive.
*			uBitConfig	= 
*  
*  OUTPUT:	unsigned - Return Type
*  			= 
*  
*  REMARK:	created on 2006년 8월 7일 월요일 오전 11:44:32    by vizirdo
**************************************************************************/
unsigned	IO_GPSB_RecvDATA(int iCH, unsigned char *pData, unsigned uLength, unsigned uBitConfig);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      void IO_GPSBS_InitCH(int iCH, unsigned uSDO, unsigned uMode);
*  
*  DESCRIPTION : Initialize GPSB channel for slave mode.
*  
*  INPUT:
*			iCH		= GPSB Channel & I/O Port number
*			[7:0]	= channel number (0 or 1)
*			[8]		= I/O PORT number for GPSB Channel
*					GPSB channel 0 can use 3 ports. (port number 0 ~ 2 are used)
*						Port 0 : GPIO_E[19:16]
*						Port 1 : GPIO_A[27:24]
*						Port 2 : GPIO_C[15:12]
*					GPSB channel 1 can only use 1 port. (port number 0 is used)
*						Port 0 : GPIO_E[23:20]
*
*			uSDOSDI	= flag for using SDO or SDI pin
*			[1]		= 0 : don't use SDO, 1 : use SDO
*			[0]		= 0 : don't use SDI, 1 : use SDI
*
*			uMode	= GPSB Mode Selection
*			[17]		= SDI capture control
*				0 : SDI captured at rising edge SCK
*				1 : SDI captured at falling edge SCK
*			[16]		= SCK polarity control
*				0 : SCKI is not inverted
*				1 : SCKI is inverted
*			[15:14]	= should be 3
*			[12:8]	= Bit width - 1
*			[7]		= 0 : MSB first, 1 : LSB first
*			[4]		= 1 : continuous transfer mode
*			[2]		= should be 1
*			[0]		= 0 : SPI, 1 : SSP
*  
*  OUTPUT:	void - Return Type
*  
*  REMARK:	created on 2006년 11월 16일 목요일 오후 6:27:55    by vizirdo
**************************************************************************/
void IO_GPSBS_InitCH(int iCH, unsigned uSDO, unsigned uMode);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_ConfigDMA(unsigned uCH, unsigned uINTEN, unsigned uDMACTR, 
*      	unsigned uPacketSize, unsigned uPacketNum);
*  
*  DESCRIPTION : Configurate a GPSB DMA setting for Transmit/Receive data.
*                          GPSB & DMA operation is not yet enabled.
*  
*  INPUT:
*			uCH			= channel number (0 or 1)
*			uDMACTR	= DMACTR register value
*			uINTEN		= INTEN register value
*			uPacketNum	= number of packets
*			uPacketSize	= packet size (byte unit)
*  
*  OUTPUT:	int - Return Type
*  			= 0 : successful
*			= negative : failure
*  
*  REMARK:	created on 2006년 12월 21일 목요일 오후 11:08:46    by vizirdo
**************************************************************************/
int IO_GPSBS_ConfigDMA(unsigned uCH, unsigned uINTEN, unsigned uDMACTR, 
	unsigned uPacketSize, unsigned uPacketNum);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_StartRXDMA(unsigned uCH, unsigned uINTMode, unsigned uINTEN, unsigned uDMACTR, 
*      	unsigned uPacketSize, unsigned uPacketNum, unsigned *pBuffer);
*  
*  DESCRIPTION : Start GPSB RX DMA at slave mode.
*  
*  INPUT:
*			uCH			= GPSB channel number (0 or 1)
*			uINTMode	= 0 : use edge triggered interrupt, 1 : use level triggered interrupt
*			uINTEN		= GPSB.INTEN register value
*			uDMACTR	= GPSB.DMACTR register value
*			uPacketSize	= 1 packet size in bytes
*			uPacketNum	= number of packets which RX DMA manipulate.
*			pBuffer		= base of buffer to store RX data
*  
*  OUTPUT:	int - Return Type
*  			= 0 : setup is successful.
*			= -1 : illegal parameter
*  
*  REMARK:	created on 2006년 11월 16일 목요일 오후 9:29:16    by vizirdo
**************************************************************************/
int IO_GPSBS_StartRXDMA(unsigned uCH, unsigned uINTMode, unsigned uINTEN, unsigned uDMACTR, 
	unsigned uPacketSize, unsigned uPacketNum, unsigned *pBuffer);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      void IO_GPSBS_EnablePKTDMA(unsigned uCH, unsigned uPktSize, unsigned *pTxBase, unsigned *pRxBase);
*  
*  DESCRIPTION : Enable GPSB & DMA Operation for packet data transfer.
*  
*  INPUT:
*			uCH		= GPSB channel number (0 or 1)
*			uPktSize	= packet size (in bytes)
*			pTxBase	= base address of transmitting data buffer
*			pRxBase	= base address of receiving data buffer
*  
*  OUTPUT:	void - Return Type
*  
*  REMARK:	created on 2006년 12월 21일 목요일 오후 11:11:50    by vizirdo
**************************************************************************/
void IO_GPSBS_EnablePKTDMA(unsigned uCH, unsigned uPktSize, unsigned *pTxBase, unsigned *pRxBase);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      void IO_GPSBS_EnableDMA(unsigned uCH, unsigned *pTxBase, unsigned *pRxBase);
*  
*  DESCRIPTION : Enable GPSB & DMA Operation for data transfer.
*  
*  INPUT:
*			uCH		= GPSB channel number (0 or 1)
*			pTxBase	= base address of transmitting data buffer
*			pRxBase	= base address of receiving data buffer
*  
*  OUTPUT:	void - Return Type
*  
*  REMARK:	created on 2006년 12월 21일 목요일 오후 11:11:50    by vizirdo
**************************************************************************/
void IO_GPSBS_EnableDMA(unsigned uCH, unsigned *pTxBase, unsigned *pRxBase);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      void IO_GPSBS_DisableDMA(unsigned uCH);
*  
*  DESCRIPTION : Disable DMA Operation
*  
*  INPUT:
*			uCH		= GPSB channel number (0 or 1)
*  
*  OUTPUT:	void - Return Type
*  
*  REMARK:	created on 2007/1/25    11:24:46    by vizirdo
**************************************************************************/
void IO_GPSBS_DisableDMA(unsigned uCH);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      void IO_GPSBS_PauseCH(int iCH);
*  
*  DESCRIPTION : Disable H/W Channel.
*  
*  INPUT:
*			iCH	= channel number (0 or 1)
*  
*  OUTPUT:	void - Return Type
*  
*  REMARK:	created on 2006년 11월 17일 금요일 오전 12:25:30    by vizirdo
**************************************************************************/
void IO_GPSBS_PauseCH(int iCH);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned IO_GPSBS_CheckDATA(int iCH);
*  
*  DESCRIPTION : Check whether RX FIFO is empty or not.
*  
*  INPUT:
*			iCH	= channel number (0 or 1)
*  
*  OUTPUT:	unsigned - Return Type
*  			= 0 : RX FIFO is empty
*			= 1 : RX FIFO is not empty
*  
*  REMARK:	created on 2006년 11월 18일 토요일 오후 6:40:50    by vizirdo
**************************************************************************/
unsigned IO_GPSBS_CheckDATA(int iCH);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned IO_GPSBS_GetDATA(int iCH);
*  
*  DESCRIPTION : Read RX FIFO
*  
*  INPUT:
*			iCH	= channel number (0 or 1)
*  
*  OUTPUT:	unsigned - Return Type
*  			= RX FIFO data.
*  
*  REMARK:	created on 2006년 11월 18일 토요일 오후 6:41:38    by vizirdo
**************************************************************************/
unsigned IO_GPSBS_GetDATA(int iCH);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned IO_GPSBS_SetDATA(int iCH, unsigned uData);
*  
*  DESCRIPTION : Send 1 word to master
*  
*  INPUT:
*			iCH		= GPSB Channel number (0 or 1)
*			uData	= Word value
*  
*  OUTPUT:	unsigned - Return Type
*  			= FIFO valid count
*  
*  REMARK:	created on 2007/1/25    11:25:53    by vizirdo
**************************************************************************/
unsigned IO_GPSBS_SetDATA(int iCH, unsigned uData);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned IO_GPSBS_IsValidPKT(unsigned uCH);
*  
*  DESCRIPTION : Check Packet Buffer is empty or not.
*  
*  INPUT:
*			uCH	= channel number (0 or 1)
*  
*  OUTPUT:	unsigned - Return Type
*  			= 0 : Empty, 1 : Not empty
*  
*  REMARK:	created on 2006년 12월 14일 목요일 오후 9:37:16    by vizirdo
**************************************************************************/
unsigned IO_GPSBS_IsValidPKT(unsigned uCH);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      unsigned IO_GPSBS_IsFullPKT(unsigned uCH);
*  
*  DESCRIPTION : Check Packet Buffer is full or not.
*  
*  INPUT:
*			uCH	= channel number (0 or 1)
*  
*  OUTPUT:	unsigned - Return Type
*  			= 0 : Not full, 1 : Full
*  
*  REMARK:	created on 2006년 12월 14일 목요일 오후 9:45:53    by vizirdo
**************************************************************************/
unsigned IO_GPSBS_IsFullPKT(unsigned uCH);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_CheckEmptyPKT(unsigned uCH, char **ppRxBuf, char **ppTxBuf);
*  
*  DESCRIPTION : Check if there exist an empty packet buffer.
*  
*  INPUT:
*			uCH		= channel number (0 or 1)
*			ppRxBuf	= pointer to store base address of receiving data (NULL if not needed)
*			ppTxBuf	= pointer to store base address of transmitting data (NULL if not needed)
*  
*  OUTPUT:	int - Return Type
*  			= 1 : there exist an empty packet buffer, and its address is stored to ppRxBuf, ppTxBuf
*  
*  REMARK:	created on 2006년 12월 21일 목요일 오후 11:17:36    by vizirdo
**************************************************************************/
int IO_GPSBS_CheckEmptyPKT(unsigned uCH, char **ppRxBuf, char **ppTxBuf);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_PushPKT(unsigned uCH, unsigned char *pTxPKT, unsigned uSize);
*  
*  DESCRIPTION : Push a packet into packet buffer.
*                          If pTxPkt == NULL, this packet only receives RX data.
*  
*  INPUT:
*			uCH		= channel number (0 or 1)
*			pTxPKT	= base address of transmitting data (NULL if not needed
*			uSize	= size of data (in bytes)
*  
*  OUTPUT:	int - Return Type
*  			= 0 : successful
*			= negative : push failed
*  
*  REMARK:	created on 2006년 12월 21일 목요일 오후 11:20:32    by vizirdo
**************************************************************************/
int IO_GPSBS_PushPKT(unsigned uCH, unsigned char *pTxPKT, unsigned uSize);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_CheckValidPKT(unsigned uCH, char **ppRxBuf, char **ppTxBuf);
*  
*  DESCRIPTION : Check if there exist a valid packet buffer
*  
*  INPUT:
*			uCH		= channel number (0 or 1)
*			ppRxBuf	= pointer to store base address of receiving data (NULL if not needed)
*			ppTxBuf	= pointer to store base address of transmitting data (NULL if not needed)
*  
*  OUTPUT:	int - Return Type
*  			= 1 : there exist an valid packet buffer, and its address is stored to ppRxBuf, ppTxBuf
*			= 0 : there is no valid packet. (packet is empty)
*  
*  REMARK:	created on 2006년 12월 21일 목요일 오후 11:24:29    by vizirdo
**************************************************************************/
int IO_GPSBS_CheckValidPKT(unsigned uCH, char **ppRxBuf, char **ppTxBuf);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_PopPKT(unsigned uCH, sHwGPSB *pHwGPSB, char *pRxBuf);
*  
*  DESCRIPTION : Pop a packet data and process next packet buffer. if packet buffer is empty, 
*                          GPSB & DMA operation is disabled.
*  
*  INPUT:
*			uCH			= channel number (0 or 1)
*			pHwGPSB	= register base address for channel 0 or 1
*			pRxBuf		= base address to store just received packet data.
*  
*  OUTPUT:	unsigned - Return Type
*  			= 0 : there is no data.
*			= 1 : last packet is just processed. GPSB & DMA operation is disabled
*			= 2 : next packet operation starts.
*  
*  REMARK:	created on 2006년 12월 21일 목요일 오후 11:42:25    by vizirdo
**************************************************************************/
#if !defined(TCC801X)
int IO_GPSBS_PopPKT(unsigned uCH, PGPSB pHwGPSB, char *pRxBuf);
#endif

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_FlushPKT(unsigned uCH);
*  
*  DESCRIPTION : Remove existing all valid packet data
*  
*  INPUT:
*			uCH	= channel number (0 or 1)
*  
*  OUTPUT:	int - Return Type
*  			= 0 : successful
*  
*  REMARK:	created on 2006년 12월 22일 금요일 오후 2:24:50    by vizirdo
**************************************************************************/
int IO_GPSBS_FlushPKT(unsigned uCH);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_InsertPIDEntry(unsigned *pPIDTable, unsigned uPIDLength, int iIndex, unsigned uPIDValue, unsigned uPIDMask);
*  
*  DESCRIPTION : Insert PID entry at PID table
*  
*  INPUT:
*			pPIDTable	= base address of PID table
*			uPIDLength	= PID table length (0 ~ 63 is valid)
*			iIndex		= index of PID table to insert. (if invalid index is used, search empty entry and insert at that index)
*			uPIDValue	= PID bit field value
*			uPIDMask	= PID Mask bit field value (1 = Masking Enabled, 0 = Masking Disabled)
*
*			uPIDMask & uPIDValue Format
*				[14]		= PayLoad Start bit
*				[13]		= Error Flag bit
*				[12:0]	= PID value
*  
*  OUTPUT:	int - Return Type
*  			= 0 ~ (uPIDLength-1) : index value which is selected to insert.
*			= -1 : there is no empty entry
**************************************************************************/
int IO_GPSBS_InsertPIDEntry(unsigned *pPIDTable, unsigned uPIDLength, int iIndex, unsigned uPIDValue, unsigned uPIDMask);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_RemovePIDEntry(unsigned *pPIDTable, unsigned uPIDLength, int iIndex, unsigned uPIDValue, unsigned uPIDMask);
*  
*  DESCRIPTION : Remove PID entry at PID Table
*  
*  INPUT:
*			pPIDTable	= base address of PID table
*			uPIDLength	= PID table length (0 ~ 63 is valid)
*			iIndex		= index of PID table to remove. 
*						   (if invalid index is used, search matched entry with uPIDValue, uPIDMask and remove that entry)
*			uPIDValue	= PID bit field value
*			uPIDMask	= PID Mask bit field value (1 = Masking Enabled, 0 = Masking Disabled)
*
*			uPIDMask & uPIDValue Format
*				[14]		= PayLoad Start bit
*				[13]		= Error Flag bit
*				[12:0]	= PID value
*  
*  OUTPUT:	int - Return Type
*  			= 0 ~ (uPIDLength-1) : index value which is removed.
*			= -1 : there is no matched entry
**************************************************************************/
int IO_GPSBS_RemovePIDEntry(unsigned *pPIDTable, unsigned uPIDLength, int iIndex, unsigned uPIDValue, unsigned uPIDMask);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      void	IO_GPSBS_StartTSIF(unsigned uCH, unsigned *pPIDTable, unsigned uPIDLength, unsigned uMatchPID, unsigned uMatchSync, 
*      	unsigned *pBuffer, unsigned uPacketSize, unsigned uPacketNum);
*  
*  DESCRIPTION : Start TS I/F DMA mode
*  
*  INPUT:
*			uCH			= GPSB Channel Number (0 or 1)
*			pPIDTable	= base address of PID table
*			uPIDLength	= entry size of PID table (1 ~ 64)
*			uMatchPID	= 1 : PID filtering is enabled
*			uMatchSync	= 1 : Sync byte matching is enabled
*			pBuffer		= base address of RX DMA buffer (= uPacketSize * uPacketNum)
*			uPacketSize	= packet size of RX DMA buffer
*			uPacketNum	= packet number of RX DMA buffer
*  
*  OUTPUT:	void - Return Type
**************************************************************************/
void	IO_GPSBS_StartTSIF(unsigned uCH, unsigned *pPIDTable, unsigned uPIDLength, unsigned uMatchPID, unsigned uMatchSync, 
	unsigned *pBuffer, unsigned uPacketSize, unsigned uPacketNum);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_CalcPKTConfig(unsigned uSize, unsigned uPktSize, unsigned *pPktNum);
*  
*  DESCRIPTION : Calculate packet number for uSize & uPktSize.
*  
*  INPUT:
*			uSize	= Entire data size
*			uPktSize	= 1 packet size
*			pPktNum	= pointer to store calculated packet number.
*  
*  OUTPUT:	int - Return Type
*  			= 1 : uSize is larger than maximum transfer size.
*			= 0 : uSize is less or equal than maximum transfer size.
*  
*  REMARK:	created on 2007/1/25    12:7:0    by vizirdo
**************************************************************************/
int IO_GPSBS_CalcPKTConfig(unsigned uSize, unsigned uPktSize, unsigned *pPktNum);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_SendDATA(unsigned uCH, unsigned char *pBuffer, unsigned uSize, unsigned uPktSize);
*  
*  DESCRIPTION : Transmit bulk of data in slave mode.
*  
*  INPUT:
*			uCH		= GPSB channel (0 or 1)
*			pBuffer	= base address of bulk data.
*			uSize	= size of bulk data.
*			uPktSize	= 1 packet size for transmitting bulk of data. 
*					   The Host should have idle time (~= 5ms) at every maximum transfer unit (= uPktSize * 32)
*					   and must read uPktSize unit.
*  
*  OUTPUT:	int - Return Type
*  			= 0 : Transfer begins
*			= negative : invalid parameter
*  
*  REMARK:	created on 2007/1/25    12:51:55    by vizirdo
**************************************************************************/
int IO_GPSBS_SendDATA(unsigned uCH, unsigned char *pBuffer, unsigned uSize, unsigned uPktSize);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_GetRemainLength(unsigned uCH);
*  
*  DESCRIPTION : Get the size of data remains.
*  
*  INPUT:
*			uCH	= GPSB Channel (0 or 1)
*  
*  OUTPUT:	int - Return Type
*  			= Size of Data remains.
*  
*  REMARK:	created on 2007/1/25    12:56:38    by vizirdo
**************************************************************************/
int IO_GPSBS_GetRemainLength(unsigned uCH);

/**************************************************************************
*  FUNCTION NAME : 
*  
*      int IO_GPSBS_SendNextDATA(unsigned uCH);
*  
*  DESCRIPTION : Process next transfer for sending bulk of data.
*  
*  INPUT:
*			uCH	= GPSB Channel (0 or 1)
*  
*  OUTPUT:	int - Return Type
*  			= 0 : Transfer finished
*			= 1 : Next packet is ready to be transferred
*  
*  REMARK:	created on 2007/1/25    12:58:4    by vizirdo
**************************************************************************/
int IO_GPSBS_SendNextDATA(unsigned uCH);


#endif /* __IO_TCCXXX_H */
