/*
 * linux/drivers/video/tcc_composite_internal.c
 *
 * Author:  <linux@telechips.com>
 * Created: March 18, 2010
 * Description: TCC Component TV-Out Driver
 *
 * Copyright (C) 20010-2011 Telechips 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/system.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <mach/bsp.h>
#include "tcc_composite.h"
#include "tcc_composite_internal.h"

/* Debugging stuff */
static int debug = 0;
#define dprintk(msg...)	if (debug) { printk( "tcc_composite_internal: " msg); }

void internal_tve_set_mode(unsigned int type)
{
	volatile PPMU pHwPMU = (volatile PPMU)tcc_p2v(HwPMU_BASE);
	volatile PNTSCPAL pHwTVE = (volatile PNTSCPAL)tcc_p2v(HwTVE_BASE);
	
	//pHwPMU->PMU_ISOL.bREG.VDAC = 1;
	tca_ckc_setippwdn(PMU_ISOL_VDAC, 0);

	switch(type)
	{
		case NTSC_M:
			pHwTVE->ECMDA.nREG  = 
				HwTVECMDA_PWDENC_PD 			|	// [7]	 Power down mode for entire digital logic of TV encoder
				HwTVECMDA_FDRST_1				|	// [6]	 Chroma is free running as compared to H-sync
				HwTVECMDA_FSCSEL_NTSC			|	// [5:4] Color subcarrier frequency is 3.57954545 MHz for NTSC
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_525				|	// [1]	 Output data has 525 lines
				HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
				0;
			break;

		case NTSC_M_J:
			pHwTVE->ECMDA.nREG  = 
				HwTVECMDA_PWDENC_PD 			|	// [7]	 Power down mode for entire digital logic of TV encoder
				HwTVECMDA_FDRST_1				|	// [6]	 Chroma is free running as compared to H-sync
				HwTVECMDA_FSCSEL_NTSC			|	// [5:4] Color subcarrier frequency is 3.57954545 MHz for NTSC
				HwTVECMDA_NO_PEDESTAL			|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_525				|	// [1]	 Output data has 525 lines
				HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
				0;
			break;

		case NTSC_N:
			pHwTVE->ECMDA.nREG  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_NTSC			|	// [5:4] Color subcarrier frequency is 3.57954545 MHz for NTSC
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_625				|	// [1]	 Output data has 625 lines
				HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
				0;
			break;

		case NTSC_N_J:
			pHwTVE->ECMDA.nREG  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_NTSC			|	// [5:4] Color subcarrier frequency is 3.57954545 MHz for NTSC
				HwTVECMDA_NO_PEDESTAL			|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_625				|	// [1]	 Output data has 625 lines
				HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
				0;
			break;

		case NTSC_443:
			pHwTVE->ECMDA.nREG  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_PALX			|	// [5:4] Color subcarrier frequency is 4.43361875 MHz for PAL-B,D,G,H,I,N
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_525				|	// [1]	 Output data has 525 lines
				HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
				0;
			break;

		case PAL_M:
			pHwTVE->ECMDA.nREG  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_PALM			|	// [5:4] Color subcarrier frequency is 3.57561149 MHz for PAL-M
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_525				|	// [1]	 Output data has 525 lines
				HwTVECMDA_PHALT_PAL 			|	// [0]	 PAL encoded chroma signal output
				0;
			break;

		case PAL_N:
			pHwTVE->ECMDA.nREG  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_PALCN			|	// [5:4] Color subcarrier frequency is 3.58205625 MHz for PAL-combination N
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_625				|	// [1]	 Output data has 625 lines
				HwTVECMDA_PHALT_PAL 			|	// [0]	 PAL encoded chroma signal output
				0;
			break;

		case PAL_B:
		case PAL_G:
		case PAL_H:
		case PAL_I:
			pHwTVE->ECMDA.nREG  = 
				HwTVECMDA_FDRST_1				|	// [6]	 Chroma is free running as compared to H-sync
				HwTVECMDA_FSCSEL_PALX			|	// [5:4] Color subcarrier frequency is 4.43361875 MHz for PAL-B,D,G,H,I,N
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_625				|	// [1]	 Output data has 625 lines
				HwTVECMDA_PHALT_PAL 			|	// [0]	 PAL encoded chroma signal output
				0;
			break;

		case PSEUDO_PAL:
			pHwTVE->ECMDA.nREG  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_PALX			|	// [5:4] Color subcarrier frequency is 4.43361875 MHz for PAL-B,D,G,H,I,N
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_525				|	// [1]	 Output data has 525 lines
				HwTVECMDA_PHALT_PAL 			|	// [0]	 PAL encoded chroma signal output
				0;
			break;

		case PSEUDO_NTSC:
			pHwTVE->ECMDA.nREG  = 
				HwTVECMDA_FDRST_0				|	// [6]	 Relationship between color burst & H-sync is maintained for video standards
				HwTVECMDA_FSCSEL_NTSC			|	// [5:4] Color subcarrier frequency is 3.57954545 MHz for NTSC
				HwTVECMDA_PEDESTAL				|	// [3]	 Video Output has a pedestal (0 is NTSC-J)
				HwTVECMDA_PIXEL_601 			|	// [2]	 Input data is at 601 rates.
				HwTVECMDA_IFMT_625				|	// [1]	 Output data has 625 lines
				HwTVECMDA_PHALT_NTSC			|	// [0]	 NTSC encoded chroma signal output
				0;
			break;
	}
}

void internal_tve_set_config(unsigned int type)
{
	volatile PNTSCPAL pHwTVE = (volatile PNTSCPAL)tcc_p2v(HwTVE_BASE);
	volatile PNTSCPAL_ENCODER_CTRL 	pHwTVE_VEN = (volatile PNTSCPAL_ENCODER_CTRL)tcc_p2v(HwNTSCPAL_ENC_CTRL_BASE);
	
	//Disconnect LCDC with NTSC/PAL encoder
	BITCLR(pHwTVE_VEN->VENCON.nREG, HwTVEVENCON_EN_EN);
		
	//Set ECMDA Register
	internal_tve_set_mode(type);

	//Set ECMDB Register
	BITSET(pHwTVE->ECMDB.nREG, HwTVECMDB_CBW(2)|HwTVECMDB_YBW(2));
	
	//Set SAT Register
#if defined(CONFIG_TCC_COMPOSITE_SIGNAL_QUALITY)
	BITSET(pHwTVE->SAT.nREG, 0x1A); //0x12
#else
	BITSET(pHwTVE->SAT.nREG, 0x4C); //0x44
#endif
	
	//Set DACSEL Register
	BITSET(pHwTVE->DACSEL.nREG, HwTVEDACSEL_DACSEL_CVBS);
	//Set DACPD Register

	BITCLR(pHwTVE->DACPD.nREG, HwTVEDACPD_PD_EN);

	BITSET(pHwTVE->ICNTL.nREG, HwTVEICNTL_VSIP_HIGH);
	BITSET(pHwTVE->ICNTL.nREG, HwTVEICNTL_HSVSP_RISING);
#ifdef TCC_COMPOSITE_CCIR656
	BITCSET(pHwTVE->ICNTL.nREG, HwTVEICNTL_ISYNC_MASK, HwTVEICNTL_ISYNC_ESAV_F);
#else
	BITCSET(pHwTVE->ICNTL.nREG, HwTVEICNTL_ISYNC_MASK, HwTVEICNTL_ISYNC_HVSI);
#endif
		
	//Set the Vertical Offset
	BITCSET(pHwTVE->HVOFFST.nREG, 0x07, ((0 & 0x700)>>8));
	pHwTVE->HOFFST.nREG = (0 & 0xFF);
			
	//Set the Horizontal Offset
	BITCSET(pHwTVE->HVOFFST.nREG, 0x08, ((1 & 0x100)>>5));
	pHwTVE->VOFFST.nREG = (1 & 0xFF);
			
	//Set the Digital Output Format
	BITCSET(pHwTVE->HVOFFST.nREG, HwTVEHVOFFST_INSEL_MASK, HwTVEHVOFFST_INSEL(2));
			
	//Set HSVSO Register
	BITCSET(pHwTVE->HSVSO.nREG, 0x07, ((0 & 0x700)>>8));
	pHwTVE->HSOE.nREG = (0 & 0xFF);
	BITCSET(pHwTVE->HSVSO.nREG, 0x38, ((0 & 0x700)>>5));
	pHwTVE->HSOB.nREG = (0 & 0xFF);
	BITCSET(pHwTVE->HSVSO.nREG, 0x40, ((0 & 0x100)>>2));
	pHwTVE->VSOB.nREG = (0 & 0xFF);

	//Set VSOE Register
	BITCSET(pHwTVE->VSOE.nREG, 0x1F, (0 & 0x1F));
	BITCSET(pHwTVE->VSOE.nREG, 0xC0, (0 & 0x03)<<6);
	BITCSET(pHwTVE->VSOE.nREG, 0x20, (0 & 0x01)<<5);
			
	//Set the Connection Type
	BITSET(pHwTVE_VEN->VENCIF.nREG, HwTVEVENCIF_FMT_1);

	BITSET(pHwTVE_VEN->VENCON.nREG, HwTVEVENCON_EN_EN);
	BITSET(pHwTVE->DACPD.nREG, HwTVEDACPD_PD_EN);
	BITCLR(pHwTVE->ECMDA.nREG, HwTVECMDA_PWDENC_PD);

	
}

void internal_tve_enable(unsigned int type, unsigned int onoff)
{
	volatile PNTSCPAL pHwTVE = (volatile PNTSCPAL)tcc_p2v(HwTVE_BASE);
	volatile PNTSCPAL_ENCODER_CTRL 	pHwTVE_VEN = (volatile PNTSCPAL_ENCODER_CTRL)tcc_p2v(HwNTSCPAL_ENC_CTRL_BASE);
	
	if(onoff)
	{
		internal_tve_set_config(type);
		BITSET(pHwTVE_VEN->VENCON.nREG, HwTVEVENCON_EN_EN);
		BITSET(pHwTVE->DACPD.nREG, HwTVEDACPD_PD_EN);
		BITCLR(pHwTVE->ECMDA.nREG, HwTVECMDA_PWDENC_PD);
	}
	else
	{
		BITCLR(pHwTVE_VEN->VENCON.nREG, HwTVEVENCON_EN_EN);
		BITCLR(pHwTVE->DACPD.nREG, HwTVEDACPD_PD_EN);
		BITSET(pHwTVE->ECMDA.nREG, HwTVECMDA_PWDENC_PD);
	}
}

