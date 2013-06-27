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

#ifndef	__PLATFORM_PHYSICAL_H
#define	__PLATFORM_PHYSICAL_H


/************************************************************************
*	Bit Field Definition
************************************************************************/
#define	Hw37									(1LL << 37)
#define	Hw36									(1LL << 36)
#define	Hw35									(1LL << 35)
#define	Hw34									(1LL << 34)
#define	Hw33									(1LL << 33)
#define	Hw32									(1LL << 32)
#define	Hw31									0x80000000
#define	Hw30									0x40000000
#define	Hw29									0x20000000
#define	Hw28									0x10000000
#define	Hw27									0x08000000
#define	Hw26									0x04000000
#define	Hw25									0x02000000
#define	Hw24									0x01000000
#define	Hw23									0x00800000
#define	Hw22									0x00400000
#define	Hw21									0x00200000
#define	Hw20									0x00100000
#define	Hw19									0x00080000
#define	Hw18									0x00040000
#define	Hw17									0x00020000
#define	Hw16									0x00010000
#define	Hw15									0x00008000
#define	Hw14									0x00004000
#define	Hw13									0x00002000
#define	Hw12									0x00001000
#define	Hw11									0x00000800
#define	Hw10									0x00000400
#define	Hw9										0x00000200
#define	Hw8										0x00000100
#define	Hw7										0x00000080
#define	Hw6										0x00000040
#define	Hw5										0x00000020
#define	Hw4										0x00000010
#define	Hw3										0x00000008
#define	Hw2										0x00000004
#define	Hw1										0x00000002
#define	Hw0										0x00000001
#define	HwZERO									0x00000000

/*******************************************************************************
*
*    TCC88x DataSheet PART 2 SMU & PMU
*
********************************************************************************/
/************************************************************************
*   1. CKC (Base Addr = 0xF0400000)
************************************************************************/
#define HwCKC_BASE									(0xF0400000)

/************************************************************************
*   2. VPIC(Vectored Priority Interrupt Controller) (Base Addr = 0xF0401000)
************************************************************************/
#define HwVPIC_BASE									(0xF0401000)
#define HwPIC_BASE									(0xF0401000)
#define HwVIC_BASE									(0xF0401200)


// Interrupt Enable 0
#define HwINT0_EHI0 				Hw31										// R/W, External Host Interface0 Interrupt Enable
#define HwINT0_I2C1					Hw30										// R/W, I2C1 Interrupt Enable 
#define HwINT0_GDMA					Hw29										// R/W, General DMA Controller Interrupt Enable
#define HwINT0_TSADC				Hw28										// R/W, TSADC Interrupt Enable
#define HwINT0_CAM					Hw27										// R/W, Graphic Engine 2D Hardware Interrupt Enable
#define HwINT0_3DMMU				Hw26										// R/W, 3D MMU Interrupt Enable
#define HwINT0_3DGP 				Hw25										// R/W, 3D Geometary Interrupt Enable
#define HwINT0_3DPP 				Hw24										// R/W, 3D Pixel Processor Interrupt Enable
#define HwINT0_VCDC 				Hw23										// R/W, Video CODEC Interrupt Enable
#define HwINT0_JPGE 				Hw22										// R/W, JPEG Decoder Interrupt Enable
#define HwINT0_MIPID 				Hw21										// R/W, MIPI DSI Interrupt Enable
#define HwINT0_VIPET				Hw20										// R/W, VIPET Controller Interrupt Enable
#define HwINT0_LCD1 				Hw19										// R/W, LCD Controller1 Interrupt Enable
#define HwINT0_LCD0 				Hw18										// R/W, LCD Controller0 Interrupt Enable
#define HwINT0_HDMI					Hw17										// R/W, HDMI interrupt Enable
#define HwINT0_SC1					Hw16										// R/W, Mem-to-Mem Scaler1 Interrupt Enable
#define HwINT0_SC0					Hw15										// R/W, Mem-to-Mem Scaler0 Interrupt Enable
#define HwINT0_GMAC 				Hw14										// R/W, GMAC interrupt Enable
#define HwINT0_UB20H 				Hw13										// R/W, USB2.0 Host interrupt Enable
#define HwINT0_SATA1				Hw12										// R/W, SATA Host/PCI-express interrupt Enable
#define HwINT0_SATA0				Hw11										// R/W, SATA Host/Device interrupt Enable
#define HwINT0_ISP3					Hw10										// R/W, ISP3 interrupt Enable
#define HwINT0_ISP2					Hw9 										// R/W, ISP2 interrupt Enable
#define HwINT0_ISP1					Hw8 										// R/W, ISP1 interrupt Enable
#define HwINT0_ISP0					Hw7 										// R/W, ISP0 interrupt Enable
#define HwINT0_EI3					Hw6 										// R/W, External Interrupt3 Enable
#define HwINT0_TSD					Hw6 										// R/W, TS Demux interrupt Enable
#define HwINT0_EI2					Hw5 										// R/W, External Interrupt2 Enable
#define HwINT0_EI1					Hw4 										// R/W, External Interrupt1 Enable
#define HwINT0_EI0					Hw3 										// R/W, External Interrupt0 Enable
#define HwINT0_SMUI2C				Hw2 										// R/W, SMU_I2C Interrupt Enable
#define HwINT0_TC32					Hw1 										// R/W, Timer32 Interrupt Enable
#define HwINT0_TC0					Hw0 										// R/W, Timer0 Interrupt Enable

// Interrupt Enable 1
#define HwINT1_CEER					Hw31										// R/W, Not maskable error ARM DMA interrupt enable
#define HwINT1_CDMA					Hw30										// R/W, Secure ARM DMA select interrupt enable
#define HwINT1_CDMAS 				Hw29										// R/W, Non secure ARM DMA interrupt enable
#define HwINT1_CPMU 				Hw28										// R/W, ARM System Metrics interrupt enable
#define HwINT1_AUDIO				Hw27										// R/W, AUDIO interrupt enable
#define HwINT1_ADMA 				Hw26										// R/W, AUDIO DMA interrupt enable
#define HwINT1_DAI1TX				Hw25										// R/W, DAI transmit interrupt enable
#define HwINT1_DAI1RX				Hw24										// R/W, DAI receive interrupt enable
#define HwINT1_EI7					Hw25										// R/W, External Interrupt 7 enable
#define HwINT1_EI6					Hw24										// R/W, External Interrupt 6 enable
#define HwINT1_CDRX 				Hw23										// R/W, CDIF receive interrupt enable
#define HwINT1_TSIF0				Hw22										// R/W, TS interface 1 interrupt enable
#define HwINT1_TSIF1				Hw21										// R/W, TS interface 0 interrupt enable
#define HwINT1_DAI2TX 				Hw20										// R/W, GPS AGPS interrupt enable
#define HwINT1_DAI2RX 				Hw19										// R/W, GPS TCXO expired interrupt enable
#define HwINT1_EI5					Hw20										// R/W, External Interrupt 5 enable
#define HwINT1_EI4					Hw19										// R/W, External Interrupt 4 enable
#define HwINT1_CIPHER 				Hw18										// R/W, Cipher interrupt enable
#define HwINT1_UH11					Hw17										// R/W, USB 1.1 Host interrupt Enable
#define HwINT1_UOTG 				Hw16										// R/W, USB 2.0 OTG interrupt enable
#define HwINT1_UART 				Hw15										// R/W, UART interrupt enable
#define HwINT1_SPDTX				Hw14										// R/W, SPDIF transmitter interrupt enable
#define HwINT1_SD1					Hw13										// R/W, SD/MMC 1 interrupt enable
#define HwINT1_SD0					Hw12										// R/W, SD/MMC 0 interrupt enable
#define HwINT1_RTC					Hw11										// R/W, RTC interrupt enable
#define HwINT1_RMT					Hw10										// R/W, Remote Control interrupt enable
#define HwINT1_NFC					Hw9 										// R/W, Nand flash controller interrupt enable
#define HwINT1_MS					Hw8 										// R/W, Memory Stick interrupt enable
#define HwINT1_MPEFEC				Hw7 										// R/W, MPEFEC interrupt enable
#define HwINT1_I2C0					Hw6 										// R/W, I2C0 interrupt enable
#define HwINT1_HDD					Hw5 										// R/W, HDD controller interrupt enable
#define HwINT1_GPSB 				Hw4 										// R/W, GPSB Interrupt Enable
#define HwINT1_G2D		 			Hw3 										// R/W, Reserved
#define HwINT1_SD2	 				Hw2 										// R/W, SD/MMC 2 Interrupt enable
#define HwINT1_SD3		 			Hw1 										// R/W, SD/MMC 3 Interrupt enable
#define HwINT1_EHI1 				Hw0 	

/************************************************************************
*   3. Timer/Counter (Base Addr = 0xF0403000)
************************************************************************/
#define HwTMR_BASE									(0xF0403000)

/************************************************************************
*   4. PMU(Power Management Unit) (Base Addr = 0xF0404000)
************************************************************************/
#define HwPMU_BASE									(0xF0404000)

/************************************************************************
*   5. SMU_I2C (Base Addr = 0xF0405000)
************************************************************************/
#define HwSMUI2C_BASE								(0xF0405000)
#define HwSMUI2C_MASTER0_BASE						(0xF0405000)
#define HwSMUI2C_MASTER1_BASE						(0xF0405040)
#define HwSMUI2C_COMMON_BASE						(0xF0405080)


/************************************************************************
*   6. GPIO & Port Multiplexing (Base Addr = 0xF0308000)
************************************************************************/
#define HwGPIO_BASE									(0xF0308000)
#define HwGPIOA_BASE								(0xF0308000)
#define HwGPIOB_BASE								(0xF0308040)
#define HwGPIOC_BASE								(0xF0308080)
#define HwGPIOD_BASE								(0xF03080C0)
#define HwGPIOE_BASE								(0xF0308100)
#define HwGPIOF_BASE								(0xF0308140)
#define HwGPIOG_BASE								(0xF03081C0)

#define HwGPIO						((PGPIO)HwGPIO_BASE)
#define HwGPIOA						((PGPION)HwGPIOA_BASE)
#define HwGPIOB						((PGPION)HwGPIOB_BASE)
#define HwGPIOC						((PGPION)HwGPIOC_BASE)
#define HwGPIOD						((PGPION)HwGPIOD_BASE)
#define HwGPIOE						((PGPION)HwGPIOE_BASE)
#define HwGPIOF						((PGPION)HwGPIOF_BASE)
#define HwEINTSEL					((PGPIOINT)HwEINTSEL_BASE)

#define	HwPORTCFG_GPFN0(X)			((X)<<0)	// 0~3
#define HwPORTCFG_GPFN0_MASK		(0xF)		// HwPORTCFG_GPFN0(15)
#define	HwPORTCFG_GPFN1(X)			((X)<<4)	// 4~7
#define HwPORTCFG_GPFN1_MASK		(0xF<<4)	// HwPORTCFG_GPFN1(15)
#define	HwPORTCFG_GPFN2(X)			((X)<<8)	// 8~11
#define HwPORTCFG_GPFN2_MASK		(0xF<<8)	// HwPORTCFG_GPFN2(15)
#define	HwPORTCFG_GPFN3(X)			((X)<<12)	// 12~15
#define HwPORTCFG_GPFN3_MASK		(0xF<<12)	// HwPORTCFG_GPFN3(15)
#define	HwPORTCFG_GPFN4(X)			((X)<<16)	// 16~19
#define HwPORTCFG_GPFN4_MASK		(0xF<<16)	// HwPORTCFG_GPFN4(15)
#define	HwPORTCFG_GPFN5(X)			((X)<<20)	// 20~23
#define HwPORTCFG_GPFN5_MASK		(0xF<<20)	// HwPORTCFG_GPFN5(15)
#define	HwPORTCFG_GPFN6(X)			((X)<<24)	// 24~27
#define HwPORTCFG_GPFN6_MASK		(0xF<<24)	// HwPORTCFG_GPFN6(15)
#define	HwPORTCFG_GPFN7(X)			((X)<<28)	// 28~31
#define HwPORTCFG_GPFN7_MASK		(0xF<<28)	// HwPORTCFG_GPFN7(15)



#define HwEINTSEL_BASE								(0xF0308184)
#define HwSYSMONEN_BASE								(0xF030818C)
#define HwECID_BASE									(0xF0308190)

/************************************************************************
*   8. SMU Config (Base Addr = 0xF05F4000)
************************************************************************/
#define HwSMUCONFIG_BASE							(0xF05F4000)




/*******************************************************************************
*
*    TCC88x DataSheet PART 3 GRAPHIC BUS
*
********************************************************************************/

/************************************************************************
*   4. 2D/3D GPU Register Define				(Base Addr = 0xF0000000) // R/W
************************************************************************/
#define HwGPU_BASE								(0xF0000000)

/************************************************************************
*   4-3. Pixel Processor Register Define		(Base Addr = 0xF0000000) // R/W
************************************************************************/
#define HwPIXELPROCESSOR_BASE					(0xF0000000)
#define HwGPU_PXL_FRAMEREGISTERS_BASE			(0xF0000000)
#define HwGPU_PXL_WRITEBACKREGISTER_BASE		(0xF0000100)
#define HWGPU_PXL_MANAGEMENTREGISTER_BASE		(0xF0001000)

/************************************************************************
*   4-4. Geometry Processor Register Define         (Base Addr = 0xF0002000) // R/W
************************************************************************/
#define HwGEOMETRYPROCESSOR_BASE				(0xF0002000)
#define HwGPU_GEO_CONTROLREGISTER_BASE			(0xF0002000)
//#define HwGPU_GEO_PLBCONFIGREGISTER_BASE		0x0100	
//#define HwGPU_GEO_VERTEXSHADER_BASE			0x0000

/*************************************************************************
*   4-5. MMU Register Define                        (Base Addr = 0xF0003000) // R/W
************************************************************************/
#define HwGPUMMUCONFIG_BASE						(0xF0003000)

/************************************************************************
*   5. Graphic Bus Configuration Register Define  (Base Addr = 0xF0004000) // R/W
************************************************************************/
#define HwGRPBUSCONFIG_BASE						(0xF0004000)


/*******************************************************************************
*
*    TCC88x DataSheet PART 4 MEMORY BUS
*
********************************************************************************/
/*
* 0xF0300000    Memory Bus Matrix Control 
* 0xF0301000    PL340 Controller (LPDDR) 
* 0xF0302000    PL341 Controller (DDR2) 
* 0xF0303000    Memory Controller Configuration 
* 0xF0304000    DDR PHY Configuration 
* 0xF0305000    Memory Bus Configuration 1 
* 0xF0306000    LPCON Controller (LPDDR, LPDDR2) 
* 0xF0307000    Memory Bus Configuration 2 
* 0xF0308000    GPIO 
* 0xF0309000    DDR2/3 Controller (DDR2, DDR3) 
* 0xF0600000    Hardware MMU Table
*/

/************************************************************************
*   3. Multi-layer Bus Matrix Register (Base Addr = 0xF0300000) // R/W
************************************************************************/
#define HwMULTILAYERBUSMATRIX_BASE				(0xF0300000)

/************************************************************************
*   4. DDR SDRAM Memory Controller (Base Addr = 0xF0301000) // R/W
************************************************************************/
#define HwLPDDR_BASE							(0xF0301000)
#define HwDDR2_BASE								(0xF0302000)
#define HwLPCON_BASE							(0xF0306000)
#define HwDDR23_BASE							(0xF0309000)

#define HwDDRIFCFG_BASE							(0xF0303000)
#define HwDDRPHY_BASE							(0xF0304400)

/************************************************************************
*   6. Memory Bus Configuration (Base Addr = ) // R/W
************************************************************************/
#define HwMEMBUSCFG1_BASE						(0xF0305000)
#define HwMEMBUSCFG2_BASE						(0xF0307000)

/************************************************************************
*   7. Internal Memory BUS Configuration (Base Addr = ) // R/W
************************************************************************/
#define HwVMT_BASE								(0x20000000)
#define HwINTNLMEMBUSCFG_BASE					(0xF0600000)


/*******************************************************************************
*
*    TCC88x DataSheet PART 5 IO BUS
*
********************************************************************************/

/*******************************************************************************
*    4. Memory Stick Host Controller Register Define (Base Addr = 0xF0590000)
********************************************************************************/
#define HwSMSHC_BASE							(0xF0590000)
#define HwSMSHC_PORTCFG_BASE					(0xF05F1000)

/*******************************************************************************
*    5. SD/SDIO/MMC Host Controller Register Define (Base Addr = 0xF05A0000)
********************************************************************************/
#define HwSDMMC_BASE							(0xF05A0000)
#define HwSDMMC0_BASE							(0xF05A0000)
#define HwSDMMC1_BASE							(0xF05A0200)
#define HwSDMMC2_BASE							(0xF05A0400)
#define HwSDMMC3_BASE							(0xF05A0600)
#define HwSDMMC_CHCTRL_BASE						(0xF05A0800)

/*******************************************************************************
*    6. EHI Register Define (Base Addr = 0xF0570000)
********************************************************************************/
#define HwEHI_BASE								(0xF0570000)
#define HwEHI0_BASE								(0xF0570000)
#define HwEHI1_BASE								(0xF0580000)

/*******************************************************************************
*    7. DMA Controller Register Define (Base Addr = 0xF0540000)
********************************************************************************/
#define HwGDMA_BASE								(0xF0540000)
#define HwGDMA0_BASE							(0xF0540000)
#define HwGDMA1_BASE							(0xF0540100)
#define HwGDMA2_BASE							(0xF0540200)
#define HwGDMA3_BASE							(0xF0540300)

/*******************************************************************************
*    8. GPSB Controller Register Define (Base Addr = 0xF0535000)
********************************************************************************/
#define HwGPSB_BASE								(0xF0535000)
#define HwGPSBCH0_BASE							(0xF0535000)
#define HwGPSBCH1_BASE							(0xF0535100)
#define HwGPSBCH2_BASE							(0xF0535200)
#define HwGPSBCH3_BASE							(0xF0535300)
#define HwGPSBCH4_BASE							(0xF0535400)
#define HwGPSBCH5_BASE							(0xF0535500)
#define HwGPSB_PORTCFG_BASE						(0xF0535800)
#define HwGPSB_PIDTABLE_BASE					(0xF0535F00)

#define	HwGPSB_BASE_ADDR(X)						*(volatile unsigned long *)(0xF0535000+(X)*0x100)
#define HwGPSBPORTCFG_BASE						0xF0535800
#define HwGPSBPIDTABLE_BASE						*(volatile unsigned long*)0xF0535F00

#define	HwGPSB_MAX_CH				4

// Status
#define	HwGPSB_STAT_WOR				Hw8											// Write FIFO over-run error flag
#define	HwGPSB_STAT_RUR				Hw7											// Read FIFO under-run error flag
#define	HwGPSB_STAT_WUR				Hw6											// Write FIFO under-run error flag
#define	HwGPSB_STAT_ROR				Hw5											// Read FIFO over-run error flag
#define	HwGPSB_STAT_RF				Hw4											// Read FIFO full flag
#define	HwGPSB_STAT_WE				Hw3											// Write FIFO empty flag
#define	HwGPSB_STAT_RNE				Hw2											// Read FIFO not empty flag
#define	HwGPSB_STAT_WTH				Hw1											// Wrtie FIFO valid entry count is under threshold
#define	HwGPSB_STAT_RTH				Hw0											// Read FIFO valid entry increased over threshold

// Interrupt Enable
#define	HwGPSB_INTEN_DW				Hw31										// DMA request enable for transmit FIFO
#define	HwGPSB_INTEN_DR				Hw30										// DMA request enable for receive FIFO
#define	HwGPSB_INTEN_SWD_BHW		Hw27										// Swap byte in half-word
#define	HwGPSB_INTEN_SWD_HWW		Hw26										// Swap half-word in word
#define	HwGPSB_INTEN_SRD_BHW		Hw25										// Swap byte in half-word
#define	HwGPSB_INTEN_SRD_HWW		Hw24										// Swap half-word in word
#define	HwGPSB_INTEN_CFGWTH(X)		((X)*Hw20)									// Write FIFO threshold for Interrupt or DMA Request
#define	HwGPSB_INTEN_CFGWTH_MASK	HwGPSB_INTEN_CFGWTH(7)
#define	HwGPSB_INTEN_CFGRTH(X)		((X)*Hw16)									// Read FIFO threshold for Interrupt or DMA Request
#define	HwGPSB_INTEN_CFGRTH_MASK	HwGPSB_INTEN_CFGRTH(7)
#define	HwGPSB_INTEN_RC				Hw15										// Clear status[8:0] at the end of read cycle
#define	HwGPSB_INTEN_WOR			Hw8											// Write FIFO over-run error interrupt enable
#define	HwGPSB_INTEN_RUR			Hw7											//Read FIFO under-run error flag interrupt enable
#define	HwGPSB_INTEN_WUR			Hw6											// Write FIFO under-run error flag interrupt enable
#define	HwGPSB_INTEN_ROR			Hw5											// Read FIFO over-run error flag interrupt enable
#define	HwGPSB_INTEN_RF				Hw4											// Read FIFO full flag interrupt enable
#define	HwGPSB_INTEN_WE				Hw3											// Write FIFO empty flag interrupt enable
#define	HwGPSB_INTEN_RNE			Hw2											// Read FIFO not empty flag interrupt enable
#define	HwGPSB_INTEN_WTH			Hw1											// Wrtie FIFO valid entry count is under threshold interrupt enable
#define	HwGPSB_INTEN_RTH			Hw0											// Read FIFO valid entry increased over threshold interrupt enable

// Mode
#define	HwGPSB_MODE_DIVLDV(X)		((X)*Hw24)
#define	HwGPSB_MODE_DIVLDV_MASK		HwGPSB_MODE_DIVLDV(255)
#define	HwGPSB_MODE_TRE				Hw23										// Master recovery time (TRE+1)*SCKO
#define	HwGPSB_MODE_THL				Hw22										// Master hold time (THL+1)*SCKO
#define	HwGPSB_MODE_TSU				Hw21										// Master setup time (TSU+1)*SCKO
#define	HwGPSB_MODE_PCS				Hw20										// Polarity control for CS(FRM) - Master Only
#define	HwGPSB_MODE_PCS_HIGH		Hw20
#define	HwGPSB_MODE_PCD				Hw19										// Polarity control for CMD(FRM)- Master Only
#define	HwGPSB_MODE_PCD_HIGH		Hw19
#define	HwGPSB_MODE_PWD				Hw18										// Polarity control for transmit data - Master Only
#define	HwGPSB_MODE_PWD_RISE		Hw18
#define	HwGPSB_MODE_PRD				Hw17										// Polarity control for receive data - Master Only
#define	HwGPSB_MODE_PRD_FALL		Hw17
#define	HwGPSB_MODE_PCK				Hw16										// Polarity control for serial clock
#define	HwGPSB_MODE_CRF				Hw15										// Clear receive FIFO counter
#define	HwGPSB_MODE_CWF				Hw14										// Clear transmit FIFO counter
#define	HwGPSB_MODE_BWS(X)			((X)*Hw8)									// Bit width Selection.(BWS+1. Valid = 7~31)
#define	HwGPSB_MODE_BWS_MASK		HwGPSB_MODE_BWS(31)
#define	HwGPSB_MODE_SD				Hw7											// Data shift direction control
#define	HwGPSB_MODE_LB				Hw6											// Data looop-back enable
#define	HwGPSB_MODE_CTF				Hw4											// Continuous transfer mode enable
#define	HwGPSB_MODE_EN				Hw3											// Operation enable
#define	HwGPSB_MODE_SLV				Hw2											// Slave mode configuration
#define	HwGPSB_MODE_MD_SPI			HwZERO										// SPI compatible
#define	HwGPSB_MODE_MD_SSP			Hw0											// SSP Compatible

// Control
#define	HwGPSB_CTRL_LCW				Hw31										// Last clock disable for write cycle
#define	HwGPSB_CTRL_LCR				Hw30										// Last clock disable for read cycle
#define	HwGPSB_CTRL_CMDEND(X)		((X)*Hw24)									// Command End Position
#define	HwGPSB_CTRL_CMDEND_MASK		HwGPSB_CTRL_CMDEND(31)
#define	HwGPSB_CTRL_CMDST(X)		((X)*Hw16)									// Command Start Position
#define	HwGPSB_CTRL_CMDST_MASK		HwGPSB_CTRL_CMDST(31)
#define	HwGPSB_CTRL_RDST(X)			((X)*Hw8)									// Read Start Position
#define	HwGPSB_CTRL_RDST_MASK		HwGPSB_CTRL_RDST(31)
#define	HwGPSB_CTRL_PLW				Hw7											// Polarity control for write command
#define	HwGPSB_CTRL_PSW(X)			((X)*Hw0)									// Write Command Position
#define	HwGPSB_CTRL_PSW_MASK		HwGPSB_CTRL_PSW(31)

// DMA Control
#define	HwGPSB_DMACTRL_DTE			Hw31										// Transmit DMA request enable
#define	HwGPSB_DMACTRL_DRE			Hw30										// Receive DMA request enable
#define	HwGPSB_DMACTRL_CT			Hw29										// Continuous mode enable
#define	HwGPSB_DMACTRL_END			Hw28										// Byte endian mode register
#define	HwGPSB_DMACTRL_MP			Hw19										// PID match mode register
#define	HwGPSB_DMACTRL_MS			Hw18										// Sync byte match control register
#define	HwGPSB_DMACTRL_TXAM(X)		((X)*Hw16)
#define	HwGPSB_DMACTRL_TXAM_MULTI	HwGPSB_DMACTRL_TXAM(0)						// TX address is rolling within full packet range
#define	HwGPSB_DMACTRL_TXAM_FIXED	HwGPSB_DMACTRL_TXAM(1)						// TX address is fixed to TXBASE
#define	HwGPSB_DMACTRL_TXAM_SINGLE	HwGPSB_DMACTRL_TXAM(2)						// TX address is rolling within 1 packet range
#define	HwGPSB_DMACTRL_TXAM_MASK	HwGPSB_DMACTRL_TXAM(3)
#define	HwGPSB_DMACTRL_RXAM(X)		((X)*Hw14)
#define	HwGPSB_DMACTRL_RXAM_MULTI	HwGPSB_DMACTRL_RXAM(0)						// RX address is rolling within full packet range
#define	HwGPSB_DMACTRL_RXAM_FIXED	HwGPSB_DMACTRL_RXAM(1)						// RX address is fixed to RXBASE
#define	HwGPSB_DMACTRL_RXAM_SINGLE	HwGPSB_DMACTRL_RXAM(2)						// RX address is rolling within 1 packet range
#define	HwGPSB_DMACTRL_RXAM_MASK	HwGPSB_DMACTRL_RXAM(3)
#define	HwGPSB_DMACTRL_MD_NOR		HwZERO										// normal mode
#define	HwGPSB_DMACTRL_MD_MP2TS		Hw4											// MPEG2-TS mode
#define	HwGPSB_DMACTRL_PCLR			Hw2											// Clear TX/RX Packet Counter
#define	HwGPSB_DMACTRL_EN			Hw0											// DMA enable

// DMA Interrupt Control
#define	HwGPSB_DMAICR_ISD			Hw29										// IRQ Status for "Done Interrupt"
#define	HwGPSB_DMAICR_ISP			Hw28										// IRQ Status for "Packet Interrupt"
#define	HwGPSB_DMAICR_IRQS_TRANS	Hw20										// IRQ is generated when transmit is done.
#define	HwGPSB_DMAICR_IRQS_RCV		HwZERO										// IRQ is generated when receiving is done.
#define	HwGPSB_DMAICR_IED_EN		Hw17										// Enable "Done Interrupt"
#define	HwGPSB_DMAICR_IEP_EN		Hw16										// Enable "Packet Interrupt"
#define	HwGPSB_DMAICR_IRQPCNT_MASK	(Hw13-1)

// Channel IRQ Status 
#define HwGPSB_CIRQST_ISTC3			Hw6											// GPSB core IRQ status for channel 3
#define HwGPSB_CIRQST_ISTC2			Hw4											// GPSB core IRQ status for channel 2
#define HwGPSB_CIRQST_ISTD1			Hw3											// GPSB DMA IRQ status for channel 1
#define HwGPSB_CIRQST_ISTC1			Hw2											// GPSB core IRQ status for channel 1
#define HwGPSB_CIRQST_ISTD0			Hw1											// GPSB DMA IRQ status for channel 0
#define HwGPSB_CIRQST_ISTC0			Hw0											// GPSB core IRQ status for channel 0
#define	HwGPSB_PIDT(X)				*(volatile unsigned long *)(0xF0535F00+(X)*4)	// R/W, PID Table Register
#define	HwGPSB_PIDT_CH2				Hw31											// Channel 2 enable
#define	HwGPSB_PIDT_CH1				Hw30											// Channel 1 enable
#define	HwGPSB_PIDT_CH0				Hw29											// Channel 0 enable

/*******************************************************************************
*    9. Overlay Mixer Register Define (Base Addr = 0xF0560000)
********************************************************************************/
#define HwOVERLAYMIXER_BASE						(0xF0560000)

/*******************************************************************************
*    10. Audio0(7.1ch) Register Define (Base Addr = 0xF0532000)
********************************************************************************/
#define HwAUDIO0_DMA_BASE						(0xF0532000)
#define HwAUDIO0_DAI_BASE						(0xF0533000)
#define HwAUDIO0_CDIF_BASE						(0xF0533080)
#define HwAUDIO0_SPDIFTX_BASE					(0xF0534000)
#define HwAUDIO0_SPDIFRX_BASE					(0xF0534800)

/*******************************************************************************
*    11. Audio1(Stereo) Register Define (Base Addr = 0xF0536000)
********************************************************************************/
#define HwAUDIO1_DAI0_BASE						(0xF0536000)
#define HwAUDIO1_CDIF_BASE						(0xF0536080)
#define HwAUDIO1_DAI1_BASE						(0xF0536100)
#define HwAUDIO1_SPDIFTX_BASE					(0xF0537000)

/*******************************************************************************
*    12. NFC(NAND Flash Controller) Register Define (Base Addr = 0xF05B0000)
********************************************************************************/
#define HwNFC_BASE								(0xF05B0000)	

#define	HwNFC_CTRL_CS3SEL_1			Hw31							// Nand Flash nCS3 is High (Disabled)
#define	HwNFC_CTRL_CS3SEL_0			HwZERO							// Nand Flash nCS3 is Low (Enabled)
#define	HwNFC_CTRL_CS2SEL_1			Hw30							// Nand Flash nCS2 is High (Disabled)
#define	HwNFC_CTRL_CS2SEL_0			HwZERO							// Nand Flash nCS2 is Low (Enabled)
#define	HwNFC_CTRL_CS1SEL_1			Hw29							// Nand Flash nCS1 is High (Disabled)
#define	HwNFC_CTRL_CS1SEL_0			HwZERO							// Nand Flash nCS1 is Low (Enabled)
#define	HwNFC_CTRL_CS0SEL_1			Hw28							// Nand Flash nCS0 is High (Disabled)
#define	HwNFC_CTRL_CS0SEL_0			HwZERO							// Nand Flash nCS0 is Low (Enabled)
#define	HwNFC_CTRL_CFG_nCS3			HwNFC_CTRL_CS3SEL_1
#define	HwNFC_CTRL_CFG_nCS2			HwNFC_CTRL_CS2SEL_1
#define	HwNFC_CTRL_CFG_nCS1			HwNFC_CTRL_CS1SEL_1
#define	HwNFC_CTRL_CFG_nCS0			HwNFC_CTRL_CS0SEL_1
#define	HwNFC_CTRL_CFG_NOACT		(Hw31|Hw30|Hw29|Hw28)

#define	HwNFC_CTRL_RDNDLY(X)		((X)*Hw25)						// Nand Flash Read Data Delay 
#define	HwNFC_CTRL_RDNDLY_0			HwNFC_CTRL_RDNDLY(0)			// Nand Flash Read Data Delay (NONE)
#define	HwNFC_CTRL_RDNDLY_1			HwNFC_CTRL_RDNDLY(1)			// Nand Flash Read Data Delay (1 Clock)
#define	HwNFC_CTRL_RDNDLY_2			HwNFC_CTRL_RDNDLY(2)			// Nand Flash Read Data Delay (2 Clock)
#define	HwNFC_CTRL_RDNDLY_3			HwNFC_CTRL_RDNDLY(3)			// Nand Flash Read Data Delay (3 Clock)
#define	HwNFC_CTRL_RDNDLY_4			HwNFC_CTRL_RDNDLY(4)			// Nand Flash Read Data Delay (4 Clock)
#define	HwNFC_CTRL_RDNDLY_5			HwNFC_CTRL_RDNDLY(5)			// Nand Flash Read Data Delay (5 Clock)
#define	HwNFC_CTRL_RDNDLY_6			HwNFC_CTRL_RDNDLY(6)			// Nand Flash Read Data Delay (6 Clock)
#define	HwNFC_CTRL_RDNDLY_7			HwNFC_CTRL_RDNDLY(7)			// Nand Flash Read Data Delay (7 Clock)

#define	HwNFC_CTRL_ARFW_FLAG_EN		Hw24							// Nand Flash Automatically Ready Flag Wait Enanble 
#define	HwNFC_CTRL_ARFW_FLAG_DIS	~Hw24							// Nand Flash Automatically Ready Flag Wait Disable

#define	HwNFC_CTRL_DACK_EN			Hw20							// Nand Flash DMA Acknowledge Enable
#define	HwNFC_CTRL_DACK_DIS			~Hw20							// Nand Flash DMA Acknowledge Disable
#define	HwNFC_CTRL_DMODE_NDMA		Hw19							// Nand Flash DMA Mode Selection ( NDMA )
#define	HwNFC_CTRL_DMODE_GDMA		~Hw19							// Nand Flash DMA Mode Selection ( GDMA )
#define	HwNFC_CTRL_BMODE_BURST		Hw18							// Nand Flash Burst Operation Mode Selection For NDMA( Burst Mode[User Data + ECC] )
#define	HwNFC_CTRL_BMODE_NOR		~Hw18							// Nand Flash Burst Operation Mode Selection For NDMA( Normal Mode[User Data])
#define	HwNFC_CTRL_ECCON_EN			Hw17							// Nand Flash ECC Encoding/Decoding Enable
#define	HwNFC_CTRL_ECCON_DIS		~Hw17							// Nand Flash ECC Encoding/Decoding Disable
#define	HwNFC_CTRL_PRSEL_P			Hw16							// Nand Flash Program Mode Selection for Burst Transfer
#define	HwNFC_CTRL_PRSEL_R			~Hw16							// Nand Flash Read Mode Selection for Burst Transfer
#define	HwNFC_CTRL_DNUM(X)			((X)*Hw14)
#define	HwNFC_CTRL_DNUM_0			HwNFC_CTRL_DNUM(0)				// Nand Flash Multiplee Data Configuration Number 
#define	HwNFC_CTRL_DNUM_1			HwNFC_CTRL_DNUM(1)				// Nand Flash Multiplee Data Configuration Number 
#define	HwNFC_CTRL_DNUM_2			HwNFC_CTRL_DNUM(2)				// Nand Flash Multiplee Data Configuration Number 

#define	HwNFC_CTRL_RDY0SEL			Hw12							// NAND Flash Extend Ready Input Selection 
#define	HwNFC_CTRL_RDY3TO0SEL		~Hw12							// Address/Command Mask Enable
#define	HwNFC_CTRL_RDY_MSK_EN		Hw11							// NAND Flahs Ready Mask Enable(Mask) 
#define	HwNFC_CTRL_RDY_MSK_DIS		~Hw11							// NAND Flahs Ready Mask DIsable(UnMask)
#define	HwNFC_CTRL_MASK_EN			Hw10							// Address/Command Mask Enable
#define	HwNFC_CTRL_MASK_DIS			~Hw10							// Address/Command Mask Enable

#define	HwNFC_CTRL_BW(X)			((X)*Hw8)
#define	HwNFC_CTRL_BW_8				HwNFC_CTRL_BW(0)				// 1Read/Write
#define	HwNFC_CTRL_BW_16			HwNFC_CTRL_BW(1)				// 2Read/Write
#define	HwNFC_CTRL_BW_32			HwNFC_CTRL_BW(2)				// 4Read/Write

#define	HwNFC_CTRL_BSIZE(X)			((X)*Hw6)
#define	HwNFC_CTRL_BSIZE_1			HwNFC_CTRL_BSIZE(0)				// 1Read/Write
#define	HwNFC_CTRL_BSIZE_2			HwNFC_CTRL_BSIZE(1)				// 2Read/Write
#define	HwNFC_CTRL_BSIZE_4			HwNFC_CTRL_BSIZE(2)				// 4Read/Write
#define	HwNFC_CTRL_BSIZE_8			HwNFC_CTRL_BSIZE(3)				// 8Read/Write

#define	HwNFC_CTRL_PSIZE(X)			((X)*Hw3)
#define	HwNFC_CTRL_PSIZE_256		HwNFC_CTRL_PSIZE(0)				// 1 Page = 256 Half-Word
#define	HwNFC_CTRL_PSIZE_512		HwNFC_CTRL_PSIZE(1)				// 1 Page = 512 Byte
#define	HwNFC_CTRL_PSIZE_1024		HwNFC_CTRL_PSIZE(2)				// 1 Page = 1024 Half-Word
#define	HwNFC_CTRL_PSIZE_2048		HwNFC_CTRL_PSIZE(3)				// 1 Page = 2048 Byte
#define	HwNFC_CTRL_PSIZE_4096		HwNFC_CTRL_PSIZE(4)				// 1 Page = 4096 Byte
#define	HwNFC_CTRL_PSIZE_MASK		HwNFC_CTRL_PSIZE(7)
#define	HwNFC_CTRL_CADDR			Hw0								// Number of Address Cycle

/*******************************************************************************
* NFC IRQCFC														(0xF05B0064)
*******************************************************************************/
#define HwNFC_IRQCFG_RDYIEN         Hw31							//NAND Flash Ready Interrupt Enable
#define HwNFC_IRQCFG_PIEN			Hw30							//NAND Flash Program Interrupt Enable
#define HwNFC_IRQCFG_RDIEN			Hw29							//NAND Flash Read Interrupt Enable

#define HwNFC_IRQCFG_SPWIEN			Hw27							//NAND Flash Spare Area Burst Write Interrupt
#define HwNFC_IRQCFG_SPRIEN			Hw26							//NAND Flash Spare Area Burst Read Interrupt

#define HwNFC_IRQCFG_MEIEN			Hw26							//MLC ECC Decoding Interrupt Enable 

/*******************************************************************************
* NFC IRQ															(0xF05B0068)
*******************************************************************************/
#define HwNFC_IRQ_MEDIRQ			Hw23
#define HwNFC_IRQ_PFG				Hw14
#define HwNFC_IRQ_RDFG				Hw13

#define	HwECC_IREQ_MEDFG			Hw7								// MLC ECC Decoding Flag Register
#define	HwECC_IREQ_MEEFG			Hw6								// MLC ECC Encoding Flag Register
#define	HwECC_IREQ_SEDFG			Hw5								// SLC ECC Decoding Flag Register
#define	HwECC_IREQ_SEEFG			Hw4								// SLC ECC Encoding Flag Register
#define	HwECC_IREQ_ERRFG			Hw3								// MLC/SLC Error Flag

/*******************************************************************************
* NFC STATUS														(0xF05B006C)
*******************************************************************************/
#define HwNFC_STATUS_RDY_RDY		Hw31							// NAND Flash Ready Flag Status 
#define	HwNFC_CTRL_RDY_BUSY			~Hw31							// External Nand Flash Controller is Busy
#define	HwNFC_CTRL_FS_RDY			Hw30							// FIFO status is Ready to write and read in FIFO
#define	HwNFC_CTRL_FS_BUSY			~Hw30							// FIFO status is Busy to write and read in FIFO

/* Here SubPage!!!! */

/*******************************************************************************
* ECC CONTROL REGISTER 
*******************************************************************************/
#define HwECC_BASE								(0xF05B0200)	

#define HwECC_CTRL_DSIZE_CLEAR		(Hw26|Hw25|Hw24|Hw23|Hw22|Hw21|Hw20|Hw19|Hw18|Hw17|Hw16)

#define HwECC_CTRL_CORPASS_EN		Hw5								// MLC Correction Pass Enable
#define HwECC_CTRL_CORPASS_DIS		~Hw5							// MLC Correction Pass Disable
#define HwECC_CTRL_ECCMODE_NDM_EN	Hw4								// ECC Mode Selection( 0: Nor 1: NFC Dedicated Memory)
#define HwECC_CTRL_ECCMODE_NDM_DIS	~Hw4							// ECC Mode Selection( 0: Nor 1: NFC Dedicated Memory)

#define	HwECC_CTRL_EN_SLCEN			Hw2								// SLC ECC Encoding Enable
#define	HwECC_CTRL_EN_SLCDE			(Hw2|Hw0)						// SLC ECC Decoding Enable
#define	HwECC_CTRL_EN_MCL4EN		(Hw2|Hw1)						// MLC ECC4 Encoding Enable
#define	HwECC_CTRL_EN_MCL4DE		(Hw2|Hw1|Hw0)					// MLC ECC4 Decoding Enable
#define	HwECC_CTRL_EN_MCL6EN		(Hw3)							// MLC ECC6 Encoding Enable
#define	HwECC_CTRL_EN_MCL6DE		(Hw3|Hw0)						// MLC ECC6 Decoding Enable
#define	HwECC_CTRL_EN_MCL12EN		(Hw3|Hw1)						// MLC ECC12 Encoding Enable
#define	HwECC_CTRL_EN_MCL12DE		(Hw3|Hw1|Hw0)					// MLC ECC12 Decoding Enable
#define	HwECC_CTRL_EN_MCL16EN		(Hw3|Hw2)						// MLC ECC16 Encoding Enable
#define	HwECC_CTRL_EN_MCL16DE		(Hw3|Hw2|Hw0)					// MLC ECC16 Decoding Enable
#define	HwECC_CTRL_EN_MCL24EN		(Hw3|Hw2|Hw1)					// MLC ECC24 Encoding Enable
#define	HwECC_CTRL_EN_MCL24DE		(Hw3|Hw2|Hw1|Hw0)				// MLC ECC24 Decoding Enable
#define	HwECC_CTRL_EN_DIS			~(Hw3|Hw2|Hw1|Hw0)				// ECC Disable

/*******************************************************************************
* ECC Error Status Register 
*******************************************************************************/
#define HwNFCECCERR_BASE						 (0xF05B0260)

#define	HwERR_NUM_ERR1				Hw0								// 1 Error Occurred(MLC4)
#define	HwERR_NUM_ERR2				Hw1								// 2 Error Occurred(MLC4)
#define	HwERR_NUM_ERR3				(Hw1|Hw0)						// 3 Error Occurred(MLC4)
#define	HwERR_NUM_ERR4				Hw2								// 4 Error Occurred(MLC4)
#define	HwERR_NUM_ERR5				(Hw2|Hw0)						// 5 Error Occurred(MLC4)
#define	HwERR_NUM_ERR6				(Hw2|Hw1)						// 6 Error Occurred(MLC4)
#define	HwERR_NUM_ERR7				(Hw2|Hw1|Hw0)					// 7 Error Occurred(MLC4)
#define	HwERR_NUM_ERR8				Hw3								// 8 Error Occurred(MLC4)
#define	HwERR_NUM_ERR9				(Hw3|Hw0)						// 9 Error Occurred(MLC4)
#define	HwERR_NUM_ERR10				(Hw3|Hw1)						// 10 Error Occurred(MLC4)
#define	HwERR_NUM_ERR11				(Hw3|Hw1|Hw0)					// 11 Error Occurred(MLC4)
#define	HwERR_NUM_ERR12				(Hw3|Hw2)						// 12 Error Occurred(MLC4)
#define	HwERR_NUM_ERR13				(Hw3|Hw2|Hw0)					// 13 Error Occurred(MLC4)
#define	HwERR_NUM_ERR14				(Hw3|Hw2|Hw1)					// 14 Error Occurred(MLC4)
#define	HwERR_NUM_ERR15				(Hw3|Hw2|Hw1|Hw0)				// 15 Error Occurred(MLC4)
#define	HwERR_NUM_ERR16				Hw4								// 16 Error Occurred(MLC4)
#define	HwERR_NUM_ERR17				(Hw4|Hw0)						// 17 Error Occurred(MLC4)
#define	HwERR_NUM_ERR18				(Hw4|Hw1)						// 18 Error Occurred(MLC4)
#define	HwERR_NUM_ERR19				(Hw4|Hw1|Hw0)					// 19 Error Occurred(MLC4)
#define	HwERR_NUM_ERR20				(Hw4|Hw2)						// 20 Error Occurred(MLC4)
#define	HwERR_NUM_ERR21				(Hw4|Hw2|Hw0)					// 21 Error Occurred(MLC4)
#define	HwERR_NUM_ERR22				(Hw4|Hw2|Hw1)					// 22 Error Occurred(MLC4)
#define	HwERR_NUM_ERR23				(Hw4|Hw2|Hw1|Hw0)				// 23 Error Occurred(MLC4)
#define	HwERR_NUM_ERR24				(Hw4|Hw3)						// 24 Error Occurred(MLC4)
#define	HwERR_NUM_CORIMP			(Hw4|Hw3|Hw0)					// Correction Impossible Error

#define	HwERR_NUM_NOERR				HwZERO							// No Error

/*******************************************************************************
*    13. SMC(Static Memory Controller) Register Define (Base Addr = 0xF05F0000)
********************************************************************************/
#define HwSMC_BASE								(0xF05F0000)
#define HwSMC_CSNCFG0							(0xF05F0020)

/*******************************************************************************
*    14. EDI(External Device Interface) Register Define (Base Addr = 0xF05F6000)
********************************************************************************/
#define HwEDI_BASE								(0xF05F6000)

/*******************************************************************************
*    15. IDE Register Define (Base Addr = 0xF0520000)
********************************************************************************/
#define HwIDE_BASE								(0xF0520000)

/*******************************************************************************
*    16. USB 1.1 Host Controller & Transceiver Register Define (Base Addr = 0xF0500000)
********************************************************************************/
#define HwUSBHOST_BASE							(0xF0500000)

/*******************************************************************************
*    17. USB 2.0 OTG Controller Register Define (Base Addr = 0xF0550000)
********************************************************************************/
#define HwUSB20OTG_BASE							(0xF0550000)
#define HwUSBPHYCFG_BASE						(0xF05F5028)

/*******************************************************************************
*    18. I2C Controller Register Define (Base Addr = 0xF0550000)
********************************************************************************/
#define HwI2C_CORE0_BASE						(0xF0530000)
#define HwI2C_CORE0_MASTER0_BASE				(0xF0530000)
#define HwI2C_CORE0_MASTER1_BASE				(0xF0530040)
#define HwI2C_CORE0_SLAVE_BASE					(0xF0530080)
#define HwI2C_CORE0_INT_BASE					(0xF05300C0)

#define HwI2C_CORE1_BASE						(0xF0530100)
#define HwI2C_CORE1_MASTER0_BASE				(0xF0530100)
#define HwI2C_CORE1_MASTER1_BASE				(0xF0530140)
#define HwI2C_CORE1_SLAVE_BASE					(0xF0530180)
#define HwI2C_CORE1_INT_BASE					(0xF05301C0)

#define HwI2CIRQ_BASE							(0xF0530300)
#define HwI2CDREQSEL_BASE						(0xF0530400)

/*******************************************************************************
*    19. PWM(Pulse Width Modulation) Register Define (Base Addr = 0xF05D0000)
********************************************************************************/
#define HwPWM_BASE								(0xF05D0000)
#define HwPDM_BASE								(0xF05D0000)

/*******************************************************************************
*    20. Remote Control Interface Register Define (Base Addr = 0xF05F3000)
********************************************************************************/
#define HwREMOTE_BASE							(0xF05F3000)

/*******************************************************************************
*    21. TSADC Interface Register Define (Base Addr = 0xF05F4000)
********************************************************************************/
#define HwTSADC_BASE							(0xF05F4000)

/*******************************************************************************
*    22. TSIF(Transport Stream Interface) Register Define (Base Addr = 0x)
********************************************************************************/
#define HwTSIF_BASE								(0xF0539000)
#define HwTSIF0_BASE							(0xF0539000)
#define HwTSIF1_BASE							(0xF0539100)
#define HwTSIF_PORTSEL_BASE						(0xF0539800)

/*******************************************************************************
*    23. UART Register Define (Base Addr = 0xF0531000)
********************************************************************************/
#define HwUART_BASE								(0xF0531000)
#define HwUART0_BASE							(0xF0531000)
#define HwUART1_BASE							(0xF0531100)
#define HwUART2_BASE							(0xF0531200)
#define HwUART3_BASE							(0xF0531300)
#define HwUART4_BASE							(0xF0531400)
#define HwUART5_BASE							(0xF0531500)
#define HwUART_PORTMUX_BASE						(0xF0531600)

/*******************************************************************************
*    24. MPEFEC Register Define (Base Addr = 0xF0510000)
********************************************************************************/
#define HwMPEFEC_BASE							(0xF0510000)

/*******************************************************************************
*    25. RTC(Real-Time Clock) Register Define (Base Addr = 0xF05F2000)
********************************************************************************/
#define HwRTC_BASE								(0xF05F2000)

/*******************************************************************************
*    26. IOBUS Configuration Register Define (Base Addr = 0xF05F5000)
********************************************************************************/
#define HwIOBUSCFG_BASE							(0xF05F5000)
#define HwIOBUSCFG								((PIOBUSCFG)HwIOBUSCFG_BASE)

// IOBUS AHB 0
#define HwIOBUSCFG_USB				Hw1											// USB2.0 OTG
#define HwIOBUSCFG_I2C				Hw7											// I2C Controller
#define HwIOBUSCFG_NFC				Hw8											// NFC Controller
#define HwIOBUSCFG_EHI0				Hw9											// External Host Interface 0
#define HwIOBUSCFG_EHI1				Hw10										// External Host Interface 1
#define HwIOBUSCFG_UART0			Hw11										// UART Controller 0
#define HwIOBUSCFG_UART1			Hw12										// UART Controller 1
#define HwIOBUSCFG_UART2			Hw13										// UART Controller 2
#define HwIOBUSCFG_UART3			Hw14										// UART Controller 3
#define HwIOBUSCFG_UART4			Hw15										// UART Controller 4
#define HwIOBUSCFG_UART5			Hw16										// UART Controller 5
#define HwIOBUSCFG_GPSB0			Hw17										// GPSB Controller 0
#define HwIOBUSCFG_GPSB1			Hw18										// GPSB Controller 1
#define HwIOBUSCFG_GPSB2			Hw19										// GPSB Controller 2
#define HwIOBUSCFG_GPSB3			Hw20										// GPSB Controller 3
#define HwIOBUSCFG_GPSB4			Hw21										// GPSB Controller 4
#define HwIOBUSCFG_GPSB5			Hw22										// GPSB Controller 5
#define HwIOBUSCFG_DAI				Hw23										// DAI/CDIF Interface


/*******************************************************************************
*
*    TCC88x DataSheet PART 6 HSIO BUS
*
********************************************************************************/

/************************************************************************
*   4. PCIe(PCI Expresss)  (Base Addr = 0xF0800000) // R/W
*************************************************************************/
#define HwPCIE_BASE								(0xF0800000)
#define HwPCIE_DBI_BASE							(0xF0810000)

/************************************************************************
*   5. SATA Host 0/1 (Base Addr = 0xF0820000, 0xF0850000) // R/W
*************************************************************************/
#define HwSATAHOST0_BASE						(0xF0820000)
#define HwSATAHOST1_BASE						(0xF0850000)

/************************************************************************
*   6. SATA Device (Base Addr = 0xF0860000) // R/W
*************************************************************************/
#define HwSATADEVICE_BASE						(0xF0860000)

/************************************************************************
*   7. SATA(PCIe) PHY 0/1 (Base Addr = 0xF0830000, 0xF0870000) // R/W
*************************************************************************/
#define HwSATAPHY0_BASE							(0xF0830000)
#define HwSATAPHY1_BASE							(0xF0870000)

/************************************************************************
*   8. Ethernet(GMAC) (Base Addr = 0xF0840000) // R/W
*************************************************************************/
#define HwGMAC_BASE								(0xF0840000)

/************************************************************************
*   9. USB 2.0 Host (Base Addr = 0xF0880000, 0xF0890000) // R/W
*************************************************************************/
#define HwUSB20HOST_EHCI_BASE					(0xF0880000)
#define HwUSB20HOST_OHCI_BASE					(0xF0890000)

/************************************************************************
*   10. Cipher (Base Addr = 0xF08A2000) // R/W
*************************************************************************/
#define HwCIPHER_BASE							(0xF08A2000)

/************************************************************************
*   11. DMAX Controller (Base Addr = 0xF08A3000) // R/W
*************************************************************************/
#define HwDMAX_BASE								(0xF08A3000)

/************************************************************************
*   12. TS-Demux (Base Addr = 0xF08B0000) // R/W
*************************************************************************/
#define HwTSDEMUX_BASE							(0xF08B0000)

/************************************************************************
*   13. HSDMA Controller  (Base Addr = 0xF08C0000) // R/W
*************************************************************************/
#define HwHSDMA_BASE							(0xF08C0000)

/************************************************************************
*   14. HSIO BUS Configuration (Base Addr = 0xF08A0000) // R/W
*************************************************************************/
#define HwHSIOBUSCFG_BASE						(0xF08A0000)
#define HwHSIOBUSMATRIX_BASE					(0xF08A1000) 


/*******************************************************************************
*
*    TCC88x DataSheet PART 7 DISPLAY BUS
*
********************************************************************************/

/************************************************************************
*   4. LCD Controller (Base Addr = 0xF0200000) // R/W
*************************************************************************/
#define HwLCDC_BASE								(0xF0200000)
#define HwLCDC0_BASE							(0xF0200000)
#define HwLCDLUT0_BASE							(0xF0200400)
#define HwLCDC1_BASE							(0xF0204000)
#define HwLCDLUT1_BASE							(0xF0204400)

#define HwLCD						((PLCDC)HwLCDC0_BASE)
#define HwLCD1						((PLCDC)HwLCDC1_BASE)

// LCD Control
#define	HwLCTRL_EVP					Hw31										// External Vsync Polarity
#define	HwLCTRL_EVS					Hw30										// External Vsync Enable
#define	HwLCTRL_R2YMD				(Hw29+Hw28+Hw27+Hw26)						// RGB to YCbCr Conversion Option
#define	HwLCTRL_GEN					Hw25										// Gamma Correction Enable Bit
#define	HwLCTRL_656					Hw24										// CCIR 656 Mode
#define	HwLCTRL_CKG					Hw23										// Clock Gating Enable for Timing Generator
#define	HwLCTRL_BPP					(Hw22+Hw21+Hw20)							// Bit Per Pixel for STN_LCD
#define	HwLCTRL_PXDW				(Hw19+Hw18+Hw17+Hw16)						// PXDW
#define	HwLCTRL_ID					Hw15										// Inverted Data Enable
#define	HwLCTRL_IV					Hw14										// Inverted Vertical Sync
#define	HwLCTRL_IH					Hw13										// Inverted Horizontal Sync
#define	HwLCTRL_IP					Hw12										// Inverted Pixel Clock
#define	HwLCTRL_CLEN				Hw11										// Clipping Enable
#define	HwLCTRL_DP					Hw10										// Double Pixel Data
#define	HwLCTRL_R2Y					Hw9											// RGB to YCbCr Converter Enable for Output 
#define	HwLCTRL_NI					Hw8											// Non-Interlace
#define	HwLCTRL_TV					Hw7											// TV mode 
#define	HwLCTRL_TFT					Hw6											// TFT LCD mode
#define	HwLCTRL_STN					Hw5											// STN LCD mode
#define	HwLCTRL_MSEL				Hw4											// Master Select for Image 0
#define	HwLCTRL_OVP					(Hw3+Hw2+Hw1)								// Overlay priority
#define	HwLCTRL_LEN					Hw0											// LCD Controller Enable

// LCD Clock Divider
#define	HwLCLKDIV_CS				Hw31										// Clock Source		
#define	HwLCLKDIV_ACDIV				Hw16										// AC bias clock divisor (STN only)
#define	HwLCLKDIV_LCLKDIV			Hw8											// LCLK clocks divider 
#define	HwLCLKDIV_PXCLKDIV			(Hw8 - Hw0)											// Pixel Clock Divider

// LCD Vertical Timing 1
#define	HwLVTIME1_VDB				Hw27										// Back porchVSYNC delay	
#define	HwLVTIME1_VDF				Hw22										// Front porch of VSYNC delay	
#define	HwLVTIME1_FPW				Hw16										// Frame pulse width	 
#define	HwLVTIME1_FLC				Hw0											// Frame line count	 

// LCD Status
#define	HwLSTATUS_VS				Hw31										// Monitoring vertical sync
#define	HwLSTATUS_BUSY				Hw30										// Busy signal
#define	HwLSTATUS_EF				Hw29										// Even-Field(Read Only). 0:Odd field or frame, 1:Even field or frame
#define	HwLSTATUS_DEOF				Hw28										// DMA End of Frame flag
#define	HwLSTATUS_I0EOF				Hw27										// Image 0 End of Frame flag
#define	HwLSTATUS_I1EOF				Hw26										// Image 1 End of Frame flag
#define	HwLSTATUS_I2EOF				Hw25										// Image 2 End of Frame flag
#define	HwLSTATUS_IE2F				Hw12										// Image 2 end-of-frame falling edge flag
#define	HwLSTATUS_IE2R				Hw11										// Image 2 end-of-frame rising edge flag
#define	HwLSTATUS_IE1F				Hw10										// Image 1 end-of-frame falling edge flag
#define	HwLSTATUS_IE1R				Hw9											// Image 1 end-of-frame rising edge flag
#define	HwLSTATUS_IE0F				Hw8											// Image 0 end-of-frame falling edge flag
#define	HwLSTATUS_IE0R				Hw7											// Image 0 end-of-frame rising edge flag
#define	HwLSTATUS_DEF				Hw6											// DMA end-of-frame falling edge flag
#define	HwLSTATUS_DER				Hw5											// DMA end-of-frame rising edge flag
#define	HwLSTATUS_DD				Hw4											// Disable done
#define	HwLSTATUS_RU				Hw3											// Register update flag
#define	HwLSTATUS_VSF				Hw2											// VS falling flag
#define	HwLSTATUS_VSR				Hw1											// VS rising flag
#define	HwLSTATUS_FU				Hw0											// LCD output fifo under-run flag.

// LCD Interrupt
#define	HwLIM_FU					Hw0											//LCD output fifo under-run interrupt mask
#define	HwLIM_VSR					Hw1											// VS rising interrupt mask
#define	HwLIM_VSF					Hw2											// VS falling interrupt mask
#define	HwLIM_RU					Hw3											// Register update interrupt mask
#define	HwLIM_DD					Hw4											// Disable done interrupt mask
#define	HwLIM_DER					Hw5											// DMA end-of-frame rising edge interrupt mask
#define	HwLIM_DEF					Hw6											// DMA end-of-frame falling edge interrupt mask
#define	HwLIM_IE0R					Hw7											// Image 0 end-of-frame rising edge interrupt mask
#define	HwLIM_IE0F					Hw8											// Image 0 end-of-frame falling edge interrupt mask
#define	HwLIM_IE1R					Hw9											// Image 1 end-of-frame rising edge interrupt mask
#define	HwLIM_IE1F					Hw10										// Image 1 end-of-frame falling edge interrupt mask
#define	HwLIM_IE2R					Hw11										// Image 2 end-of-frame rising edge interrupt mask
#define	HwLIM_IE2F					Hw12										// Image 2 end-of-frame falling edge interrupt mask

// LCD Image Control
#define	HwLIC_INTL					Hw31										// DMA interlace type
#define	HwLIC_AEN					Hw30										// AlphaBlending Function for Each Image
#define	HwLIC_CEN					Hw29										// Chroma-Key Function for Each Image
#define	HwLIC_IEN					Hw28										// Image Display Function for Each Image
#define	HwLIC_SRC					Hw27										// Image Source Select
#define	HwLIC_AOPT					(Hw26+Hw25)									// Alpha -blending Option Selection Bits
#define	HwLIC_ASEL					Hw24										// Image Displaying Function for Each Image
#define	HwLIC_UPD					Hw16										// data update enable
#define	HwLIC_PD					Hw15										// Bit padding
#define	HwLIC_Y2RMD					(Hw10+Hw9)									// YCbCr to RGB Conversion Option
#define	HwLIC_Y2R					Hw8											// YCbCr to RGB Conversion Enable Bit
#define	HwLIC_BR					Hw7											// Bit Reverse
#define	HwLIC_FMT					(Hw4+Hw3+Hw2+Hw1+Hw0)						// Image Format

/************************************************************************
*   5. LCD System Interface (Base Addr = ) // R/W
*************************************************************************/
#define HwLCDSI_BASE							(0xF020C000)


/************************************************************************
*   6. Memory To Memory Scaler (Base Addr = 0xF0210000) // R/W
*************************************************************************/
#define HwM2MSCALER_BASE						(0xF0210000)
#define HwM2MSCALER0_BASE						(0xF0210000)
#define HwM2MSCALER1_BASE						(0xF0220000)

#define HwSCALERPOSTFILTER_BASE					(0xF025B000)

/************************************************************************
*   7. NTSC/PAL Encoder Composite Output (Base Addr = 0xF0240000) // R/W
*************************************************************************/
#define HwNTSCPAL_BASE							(0xF0240000)
#define HwTVENCODER_BASE						(0xF0240000)

#define HwTVE_BASE								(0xF0240000)
#define HwTVE_VEN_BASE							(0xF0240800)

// Encoder Mode Control A
#define	HwTVECMDA_PWDENC_PD			Hw7											// Power down mode for entire digital logic of TV encoder
#define	HwTVECMDA_FDRST_1			Hw6											// Chroma is free running as compared to H-sync
#define	HwTVECMDA_FDRST_0			HwZERO										// Relationship between color burst & H-sync is maintained for video standards
#define	HwTVECMDA_FSCSEL(X)			((X)*Hw4)
#define	HwTVECMDA_FSCSEL_NTSC		HwTVECMDA_FSCSEL(0)							// Color subcarrier frequency is 3.57954545 MHz for NTSC
#define	HwTVECMDA_FSCSEL_PALX		HwTVECMDA_FSCSEL(1)							// Color subcarrier frequency is 4.43361875 MHz for PAL-B,D,G,H,I,N
#define	HwTVECMDA_FSCSEL_PALM		HwTVECMDA_FSCSEL(2)							// Color subcarrier frequency is 3.57561149 MHz for PAL-M
#define	HwTVECMDA_FSCSEL_PALCN		HwTVECMDA_FSCSEL(3)							// Color subcarrier frequency is 3.58205625 MHz for PAL-combination N
#define	HwTVECMDA_FSCSEL_MASK		HwTVECMDA_FSCSEL(3)
#define	HwTVECMDA_PEDESTAL			Hw3											// Video Output has a pedestal
#define	HwTVECMDA_NO_PEDESTAL		HwZERO										// Video Output has no pedestal
#define	HwTVECMDA_PIXEL_SQUARE		Hw2											// Input data is at square pixel rates.
#define	HwTVECMDA_PIXEL_601			HwZERO										// Input data is at 601 rates.
#define	HwTVECMDA_IFMT_625			Hw1											// Output data has 625 lines
#define	HwTVECMDA_IFMT_525			HwZERO										// Output data has 525 lines
#define	HwTVECMDA_PHALT_PAL			Hw0											// PAL encoded chroma signal output
#define	HwTVECMDA_PHALT_NTSC		HwZERO										// NTSC encoded chroma signal output

// Encoder Mode Control B
#define	HwTVECMDB_YBIBLK_BLACK		Hw4											// Video data is forced to Black level for Vertical non VBI processed lines.
#define	HwTVECMDB_YBIBLK_BYPASS		HwZERO										// Input data is passed through forn non VBI processed lines.
#define	HwTVECMDB_CBW(X)			((X)*Hw2)
#define	HwTVECMDB_CBW_LOW			HwTVECMDB_CBW(0)							// Low Chroma band-width
#define	HwTVECMDB_CBW_MEDIUM		HwTVECMDB_CBW(1)							// Medium Chroma band-width
#define	HwTVECMDB_CBW_HIGH			HwTVECMDB_CBW(2)							// High Chroma band-width
#define	HwTVECMDB_CBW_MASK			HwTVECMDB_CBW(3)							// 
#define	HwTVECMDB_YBW(X)			((X)*Hw0)
#define	HwTVECMDB_YBW_LOW			HwTVECMDB_YBW(0)							// Low Luma band-width
#define	HwTVECMDB_YBW_MEDIUM		HwTVECMDB_YBW(1)							// Medium Luma band-width
#define	HwTVECMDB_YBW_HIGH			HwTVECMDB_YBW(2)							// High Luma band-width
#define	HwTVECMDB_YBW_MASK			HwTVECMDB_YBW(3)							// 

// Encoder Clock Generator
#define	HwTVEGLK_XT24_24MHZ			Hw4											// 24MHz Clock input
#define	HwTVEGLK_XT24_27MHZ			HwZERO										// 27MHz Clock input
#define	HwTVEGLK_GLKEN_RST_EN		Hw3											// Reset Genlock
#define	HwTVEGLK_GLKEN_RST_DIS		~Hw3										// Release Genlock
#define	HwTVEGLK_GLKE(X)			((X)*Hw1)
#define	HwTVEGLK_GLKE_INT			HwTVEGLK_GLKE(0)							// Chroma Fsc is generated from internal constants based on current user setting
#define	HwTVEGLK_GLKE_RTCO			HwTVEGLK_GLKE(2)							// Chroma Fsc is adjusted based on external RTCO input
#define	HwTVEGLK_GLKE_CLKI			HwTVEGLK_GLKE(3)							// Chroma Fsc tracks non standard encoder clock (CLKI) frequency
#define	HwTVEGLK_GLKE_MASK			HwTVEGLK_GLKE(3)							//
#define	HwTVEGLK_GLKEN_GLKPL_HIGH	Hw0											// PAL ID polarity is active high
#define	HwTVEGLK_GLKEN_GLKPL_LOW	HwZERO										// PAL ID polarity is active low

// Encoder Mode Control C
#define	HwTVECMDC_CSMDE_EN			Hw7											// Composite Sync mode enabled
#define	HwTVECMDC_CSMDE_DIS			~Hw7										// Composite Sync mode disabled (pin is tri-stated)
#define	HwTVECMDC_CSMD(X)			((X)*Hw5)
#define	HwTVECMDC_CSMD_CSYNC		HwTVECMDC_CSMD(0)							// CSYN pin is Composite sync signal
#define	HwTVECMDC_CSMD_KEYCLAMP		HwTVECMDC_CSMD(1)							// CSYN pin is Keyed clamp signal
#define	HwTVECMDC_CSMD_KEYPULSE		HwTVECMDC_CSMD(2)							// CSYN pin is Keyed pulse signal
#define	HwTVECMDC_CSMD_MASK			HwTVECMDC_CSMD(3)
#define	HwTVECMDC_RGBSYNC(X)		((X)*Hw3)
#define	HwTVECMDC_RGBSYNC_NOSYNC	HwTVECMDC_RGBSYNC(0)						// Disable RGBSYNC (when output is configured for analog EGB mode)
#define	HwTVECMDC_RGBSYNC_RGB		HwTVECMDC_RGBSYNC(1)						// Sync on RGB output signal (when output is configured for analog EGB mode)
#define	HwTVECMDC_RGBSYNC_G			HwTVECMDC_RGBSYNC(2)						// Sync on G output signal (when output is configured for analog EGB mode)
#define	HwTVECMDC_RGBSYNC_MASK		HwTVECMDC_RGBSYNC(3)

// DAC Output Selection
#define	HwTVEDACSEL_DACSEL_CODE0	HwZERO										// Data output is diabled (output is code '0')
#define	HwTVEDACSEL_DACSEL_CVBS		Hw0											// Data output in CVBS format

// DAC Power Down
#define	HwTVEDACPD_PD_EN			Hw0											// DAC Power Down Enabled
#define	HwTVEDACPD_PD_DIS			~Hw0										// DAC Power Down Disabled

// Sync Control
#define	HwTVEICNTL_FSIP_ODDHIGH		Hw7											// Odd field active high
#define	HwTVEICNTL_FSIP_ODDLOW		HwZERO										// Odd field active low
#define	HwTVEICNTL_VSIP_HIGH		Hw6											// V-sync active high
#define	HwTVEICNTL_VSIP_LOW			HwZERO										// V-sync active low
#define	HwTVEICNTL_HSIP_HIGH		Hw5											// H-sync active high
#define	HwTVEICNTL_HSIP_LOW			HwZERO										// H-sync active low
#define	HwTVEICNTL_HSVSP_RISING		Hw4											// H/V-sync latch enabled at rising edge
#define	HwTVEICNTL_HVVSP_FALLING	HwZERO										// H/V-sync latch enabled at falling edge
#define	HwTVEICNTL_VSMD_START		Hw3											// Even/Odd field H/V sync output are aligned to video line start
#define	HwTVEICNTL_VSMD_MID			HwZERO										// Even field H/V sync output are aligned to video line midpoint
#define	HwTVEICNTL_ISYNC(X)			((X)*Hw0)
#define	HwTVEICNTL_ISYNC_FSI		HwTVEICNTL_ISYNC(0)							// Alignment input format from FSI pin
#define	HwTVEICNTL_ISYNC_HVFSI		HwTVEICNTL_ISYNC(1)							// Alignment input format from HSI,VSI,FSI pin
#define	HwTVEICNTL_ISYNC_HVSI		HwTVEICNTL_ISYNC(2)							// Alignment input format from HSI,VSI pin
#define	HwTVEICNTL_ISYNC_VFSI		HwTVEICNTL_ISYNC(3)							// Alignment input format from VSI,FSI pin
#define	HwTVEICNTL_ISYNC_VSI		HwTVEICNTL_ISYNC(4)							// Alignment input format from VSI pin
#define	HwTVEICNTL_ISYNC_ESAV_L		HwTVEICNTL_ISYNC(5)							// Alignment input format from EAV,SAV codes (line by line)
#define	HwTVEICNTL_ISYNC_ESAV_F		HwTVEICNTL_ISYNC(6)							// Alignment input format from EAV,SAV codes (frame by frame)
#define	HwTVEICNTL_ISYNC_FREE		HwTVEICNTL_ISYNC(7)							// Alignment is free running (Master mode)
#define	HwTVEICNTL_ISYNC_MASK		HwTVEICNTL_ISYNC(7)

// Offset Control
#define	HwTVEHVOFFST_INSEL(X)		((X)*Hw6)
#define	HwTVEHVOFFST_INSEL_BW16_27MHZ		HwTVEHVOFFST_INSEL(0)				// 16bit YUV 4:2:2 sampled at 27MHz
#define	HwTVEHVOFFST_INSEL_BW16_13P5MH		HwTVEHVOFFST_INSEL(1)				// 16bit YUV 4:2:2 sampled at 13.5MHz
#define	HwTVEHVOFFST_INSEL_BW8_13P5MHZ		HwTVEHVOFFST_INSEL(2)				// 8bit YUV 4:2:2 sampled at 13.5MHz
#define	HwTVEHVOFFST_INSEL_MASK		HwTVEHVOFFST_INSEL(3)
#define	HwTVEHVOFFST_VOFFST_256		Hw3											// Vertical offset bit 8 (Refer to HwTVEVOFFST)
#define	HwTVEHVOFFST_HOFFST_1024	Hw2											// Horizontal offset bit 10 (Refer to HwTVEHOFFST)
#define	HwTVEHVOFFST_HOFFST_512		Hw1											// Horizontal offset bit 9 (Refer to HwTVEHOFFST)
#define	HwTVEHVOFFST_HOFFST_256		Hw0											// Horizontal offset bit 8 (Refer to HwTVEHOFFST)

// Sync Output Control
#define	HwTVEHSVSO_VSOB_256			Hw6											// VSOB bit 8 (Refer to HwVSOB)
#define	HwTVEHSVSO_HSOB_1024		Hw5											// HSOB bit 10 (Refer to HwHSOB)
#define	HwTVEHSVSO_HSOB_512			Hw4											// HSOB bit 9 (Refer to HwHSOB)
#define	HwTVEHSVSO_HSOB_256			Hw3											// HSOB bit 8 (Refer to HwHSOB)
#define	HwTVEHSVSO_HSOE_1024		Hw2											// HSOE bit 10 (Refer to HwHSOE)
#define	HwTVEHSVSO_HSOE_512			Hw1											// HSOE bit 9 (Refer to HwHSOE)
#define	HwTVEHSVSO_HSOE_256			Hw0											// HSOE bit 8 (Refer to HwHSOE)

// Trailing Edge of Vertical Sync Control
#define	HwTVEVSOE_VSOST(X)			((X)*Hw6)									// Programs V-sync relative location for Odd/Even Fields.
#define	HwTVEVSOE_NOVRST_EN			Hw5											// No vertical reset on every field
#define	HwTVEVSOE_NOVRST_NORMAL		HwZERO										// Normal vertical reset operation (interlaced output timing)
#define	HwTVEVSOE_VSOE(X)			((X)*Hw0)									// Trailing Edge of Vertical Sync Control

// VBI Control Register
#define	HwTVEVCTRL_VBICTL(X)		((X)*Hw5)									// VBI Control indicating the current line is VBI.
#define	HwTVEVCTRL_VBICTL_NONE		HwTVEVCTRL_VBICTL(0)						// Do nothing, pass as active video.
#define	HwTVEVCTRL_VBICTL_10LINE	HwTVEVCTRL_VBICTL(1)						// Insert blank(Y:16, Cb,Cr: 128), for example, 10 through 21st line.
#define	HwTVEVCTRL_VBICTL_1LINE		HwTVEVCTRL_VBICTL(2)						// Insert blank data 1 line less for CC processing.
#define	HwTVEVCTRL_VBICTL_2LINE		HwTVEVCTRL_VBICTL(3)						// Insert blank data 2 line less for CC and CGMS processing.
#define	HwTVEVCTRL_MASK				HwTVEVCTRL_VBICTL(3)
#define	HwTVEVCTRL_CCOE_EN			Hw4											// Closed caption odd field enable.
#define	HwTVEVCTRL_CCEE_EN			Hw3											// Closed caption even field enable.
#define	HwTVEVCTRL_CGOE_EN			Hw2											// Copy generation management system enable odd field.
#define	HwTVEVCTRL_CGEE_EN			Hw1											// Copy generation management system enable even field.
#define	HwTVEVCTRL_WSSE_EN			Hw0											// Wide screen enable.

// Connection between LCDC & TVEncoder Control
#define	HwTVEVENCON_EN_EN			Hw0											// Connection between LCDC & TVEncoder Enabled
#define	HwTVEVENCON_EN_DIS			~Hw0										// Connection between LCDC & TVEncoder Disabled

// I/F between LCDC & TVEncoder Selection
#define	HwTVEVENCIF_MV_1			Hw1											// reserved
#define	HwTVEVENCIF_FMT_1			Hw0											// PXDATA[7:0] => CIN[7:0], PXDATA[15:8] => YIN[7:0]
#define	HwTVEVENCIF_FMT_0			HwZERO										// PXDATA[7:0] => YIN[7:0], PXDATA[15:8] => CIN[7:0]

/************************************************************************
*   8. HDMI (Base Addr = 0xF0254000) // R/W
*************************************************************************/
#define HwHDMI_BASE								(0xF0254000)
#define HwHDMICTRL_BASE							(0xF0254000)
#define HwHDMICORE_BASE							(0xF0255000)
#define HwHDMIAES_BASE							(0xF0256000)
#define HwHDMISPDIF_BASE						(0xF0257000)
#define HwHDMII2S_BASE							(0xF0258000)
#define HwHDMICEC_BASE							(0xF0259000)

/************************************************************************
*   9. Video and Image Quality Enhancer(VIQE) (Base Addr = ) // R/W
*************************************************************************/
#define HwVIQE_BASE								(0xF0252000)

/************************************************************************
*   10. MIPI(Mobile Industry Processor Interface) (Base Addr = ) // R/W
*************************************************************************/
#define HwMIPI_BASE								(0xF025B000)

/************************************************************************
*   11. DDI_CONFIG (Base Addr = 0xF0250000) // R/W
*************************************************************************/
#define HwDDI_CONFIG_BASE						(0xF0251000)

/************************************************************************
*   12. DDI_CACHE (Base Addr = ) // R/W
*************************************************************************/
#define HwDDICTRL_BASE							(0xF0250000)
#define HwDDICACHE_BASE							(0xF0250000)

/*******************************************************************************
*
*    TCC88x DataSheet PART 8 VIDEO BUS
*
********************************************************************************/
/************************************************************************
*   4. JPEG Encoder (Base Addr = 0xF0780000) // R/W
*************************************************************************/
#define HwJPEGENC_BASE 							(0xF0780000)

/************************************************************************
*   5. Video Codec (Base Addr = 0xF0700000) // R/W
*************************************************************************/
#define HwVIDEOCODEC_BASE 						(0xF0700000) 

/************************************************************************
*   6. Video Cache (Base Addr = 0xF0710000) // R/W
*************************************************************************/
#define HwVIDEOCACHE_BASE 						(0xF0710000) 

/************************************************************************
*   7. Video Bus Configuration (Base Addr = 0xF0720000) // R/W
*************************************************************************/
#define HwVIDEOBUSCFG_BASE 						(0xF0720000) 

/*******************************************************************************
*
*    TCC88x DataSheet PART 9 CAMERA BUS
*
********************************************************************************/

/************************************************************************
*   4. Camera Interface (Base Addr = 0xF0900000) // R/W
*************************************************************************/
#define HwCIF_BASE								(0xF0900000)
#define HwCIFEFFECTOR_BASE						(0xF0900100)
#define HwCIFSCALER_BASE						(0xF0900200)


/************************************************************************
*   5. ISP(Image Signal Processor) (Base Addr = 0xF0910000) // R/W
*************************************************************************/
#define HwISPBASE								(0xF0910000)
#define HwISPIMGEFF_BASE						(0xF0910200)
#define HwISPSI_BASE							(0xF0910300)
#define HwISPISP_BASE							(0xF0910400)
#define HwISPFLASH_BASE							(0xF0910660)
#define HwISPSHUT_BASE							(0xF0910680)
#define HwISPCPROC_BASE							(0xF0910800)
#define HwISPMRSZ_BASE							(0xF0910C00)
#define HwISPSRSZ_BASE							(0xF0911000)
#define HwISPMI_BASE							(0xF0911400)
#define HwISPJPE_BASE							(0xF0911800)
#define HwISPMIPI_BASE							(0xF0911C00)

/************************************************************************
*   6. CAMBUS Configuration (Base Addr = 0xF0920000) // R/W
*************************************************************************/
#define HwCAMBUSMATRIX_BASE						(0xF0920000)
#define HwCAMBUSCFG_BASE						(0xF0930000)

#endif						
