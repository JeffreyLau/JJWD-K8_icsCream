
/****************************************************************************
 *   FileName    : TCC_LCD_interfface.c
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/
/*****************************************************************************
*
* Header Files Include
*
******************************************************************************/
#include <debug.h>
#include <stdlib.h>
#include <string.h>
#include <reg.h>
#include <platform/iomap.h>
#include <dev/fbcon.h>

#include <platform/reg_physical.h>
#include <lcd.h>
#include <tnftl/TC_DRV.h>
#include <platform/irqs.h>
#include <platform/globals.h>
#include <i2c.h>
#include <tnftl/IO_TCCXXX.h>
#include <dev/gpio.h>
#include <platform/gpio.h>
#include <tcc_lcd.h>
#include "tca_ddic_interface.h"

volatile int lcd_nop_count = 0;
#define _ASM_NOP_ { lcd_nop_count++; }
void lcd_delay_us(unsigned int us)
{
	int i;
	while(us--)
	{
		for(i=0 ; i<20 ; i++)
			_ASM_NOP_ 
	}
}

void mdelay(unsigned int ms)
{
	lcd_delay_us(ms * 1000);
}


void GPSB_SETConfig(sDRV_GPSB * pDRV_GPSB, int iCH, int iPort)
{

	sHwGPSB* pHwGPSB;
	U32	uMode, uSCKFreq, uSwCtrl, uSDOSDI;
			// Get Channel

		pDRV_GPSB->CHSTS		= DRV_GPSB_STATUS_IDLE;
		pDRV_GPSB->CHANNEL		= iCH;
		pDRV_GPSB->PORT			= iPort;
		pDRV_GPSB->pHwGPSB		= IO_GPSB_GetBaseAddr(iCH);

		pHwGPSB		= pDRV_GPSB->pHwGPSB;

		uMode		= pDRV_GPSB->MODE;
		uSCKFreq	= pDRV_GPSB->SCKFREQ;
		uSwCtrl		= pDRV_GPSB->SWCTRL;
		uSDOSDI		= pDRV_GPSB->SDOSDI;

		//Initialize GPSB Channel
		IO_GPSB_InitChannel(pDRV_GPSB->CHANNEL, pDRV_GPSB->PORT, uMode, uSwCtrl, uSDOSDI);

		//Set the MODE Register
		pHwGPSB->MODE = uMode;

		//Set Handle Status
		pDRV_GPSB->CHSTS = DRV_GPSB_STATUS_ACTIVE;

}

void GPSB_Port_disable(void)
{

	/* gpio D12 : SDO,  D11: SDI,  D10 : SCLK   D9 : SFRM */
	BITCSET(HwGPIOD->GPFN1, HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | HwPORTCFG_GPFN4_MASK,
			HwPORTCFG_GPFN1(0) | HwPORTCFG_GPFN2(0) | HwPORTCFG_GPFN3(0) | HwPORTCFG_GPFN4(0) );

	BITCSET(HwGPIOD->GPEN, Hw9|Hw10|Hw11|Hw12, Hw9|Hw10|Hw11|Hw12);
	BITCLR(HwGPIOD->GPDAT, Hw9|Hw10|Hw11|Hw12);

}

void DEV_Lcd_PortSet(unsigned int bit_per_pixel)
{	
	BITCSET(HwGPIOC->GPEN, Hw24|Hw25|Hw26|Hw27|Hw28|Hw29|Hw30|Hw31, Hw24|Hw25|Hw26|Hw27|Hw28|Hw29|Hw30|Hw31);
	BITCSET(HwGPIOC->GPDAT, Hw24|Hw25|Hw26|Hw27|Hw28|Hw29|Hw30|Hw31, Hw24|Hw25|Hw26|Hw27|Hw28|Hw29|Hw30|Hw31);


	switch (bit_per_pixel) 
	{
		case 24:
			BITCSET(HwGPIOC->GPEN, Hw23|Hw22|Hw21|Hw20|Hw19|Hw18, Hw23|Hw22|Hw21|Hw20|Hw19|Hw18);
		case 18:
			BITCSET(HwGPIOC->GPEN, Hw17|Hw16, Hw17|Hw16);


		case 16:
			BITCSET(HwGPIOC->GPEN, 0x0000FFFF, 0x0000FFFF);
				break;
			
		default:
			// do nothing
			break;
	}
}



void LCDC_IO_Set (char lcdctrl_num, unsigned bit_per_pixel)
{
	char LCD_PORT_ENABLE;


	if(lcdctrl_num)
		LCD_PORT_ENABLE = 5;
	else
		LCD_PORT_ENABLE = 2;

#ifdef CONFIG_LCD_CPU_INTERFACE
	DEV_Lcd_PortSet(bit_per_pixel);
	LCD_PORT_ENABLE = 1;
#endif//_CONFIG_LCD_CPU_INTERFACE_	

	BITCSET(HwGPIOC->GPFN3, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK,
							HwPORTCFG_GPFN0(LCD_PORT_ENABLE) | HwPORTCFG_GPFN1(LCD_PORT_ENABLE) | HwPORTCFG_GPFN2(LCD_PORT_ENABLE) | 
							HwPORTCFG_GPFN3(LCD_PORT_ENABLE));

	switch (bit_per_pixel) {
		case 24:
			BITCSET(HwGPIOC->GPFN2, HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK |
									HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK,
									HwPORTCFG_GPFN2(LCD_PORT_ENABLE) | HwPORTCFG_GPFN3(LCD_PORT_ENABLE) | HwPORTCFG_GPFN4(LCD_PORT_ENABLE) | 
									HwPORTCFG_GPFN5(LCD_PORT_ENABLE) | HwPORTCFG_GPFN6(LCD_PORT_ENABLE) | HwPORTCFG_GPFN7(LCD_PORT_ENABLE));
		case 18:
			BITCSET(HwGPIOC->GPFN2, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK,
									HwPORTCFG_GPFN0(LCD_PORT_ENABLE) | HwPORTCFG_GPFN1(LCD_PORT_ENABLE));

		case 16:
			BITCSET(HwGPIOC->GPFN1, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | 
									HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK | HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK,
									HwPORTCFG_GPFN0(LCD_PORT_ENABLE) | HwPORTCFG_GPFN1(LCD_PORT_ENABLE) | HwPORTCFG_GPFN2(LCD_PORT_ENABLE) | 
									HwPORTCFG_GPFN3(LCD_PORT_ENABLE) | HwPORTCFG_GPFN4(LCD_PORT_ENABLE) | HwPORTCFG_GPFN5(LCD_PORT_ENABLE) | 
									HwPORTCFG_GPFN6(LCD_PORT_ENABLE) | HwPORTCFG_GPFN7(LCD_PORT_ENABLE));
		case 8:
			BITCSET(HwGPIOC->GPFN0, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | 
									HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK | HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK,
									HwPORTCFG_GPFN0(LCD_PORT_ENABLE) | HwPORTCFG_GPFN1(LCD_PORT_ENABLE) | HwPORTCFG_GPFN2(LCD_PORT_ENABLE) | 
									HwPORTCFG_GPFN3(LCD_PORT_ENABLE) | HwPORTCFG_GPFN4(LCD_PORT_ENABLE) | HwPORTCFG_GPFN5(LCD_PORT_ENABLE) | 
									HwPORTCFG_GPFN6(LCD_PORT_ENABLE) | HwPORTCFG_GPFN7(LCD_PORT_ENABLE));
			break;
			
		default:
			// do nothing
			break;
	}

}

#define LCD_PORT_DISABLE	0	
void LCDC_IO_Disable (char lcdctrl_num, unsigned bit_per_pixel)
{

	BITCSET(HwGPIOC->GPFN3, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK,
							HwPORTCFG_GPFN0(LCD_PORT_DISABLE) | HwPORTCFG_GPFN1(LCD_PORT_DISABLE) | HwPORTCFG_GPFN2(LCD_PORT_DISABLE) | 
							HwPORTCFG_GPFN3(LCD_PORT_DISABLE));

	BITCSET(HwGPIOC->GPEN, Hw24|Hw25|Hw26|Hw27, Hw24|Hw25|Hw26|Hw27);
	BITCLR(HwGPIOC->GPDAT, Hw24|Hw25|Hw26|Hw27);


	switch (bit_per_pixel) {
		case 24:
			BITCSET(HwGPIOC->GPFN2, HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK |
									HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK,
									HwPORTCFG_GPFN2(LCD_PORT_DISABLE) | HwPORTCFG_GPFN3(LCD_PORT_DISABLE) | HwPORTCFG_GPFN4(LCD_PORT_DISABLE) | 
									HwPORTCFG_GPFN5(LCD_PORT_DISABLE) | HwPORTCFG_GPFN6(LCD_PORT_DISABLE) | HwPORTCFG_GPFN7(LCD_PORT_DISABLE));

			BITCSET(HwGPIOC->GPEN, Hw23|Hw22|Hw21|Hw20|Hw19|Hw18, Hw23|Hw22|Hw21|Hw20|Hw19|Hw18);
			BITCLR(HwGPIOC->GPDAT, Hw23|Hw22|Hw21|Hw20|Hw19|Hw18);

		case 18:
			BITCSET(HwGPIOC->GPFN2, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK,
									HwPORTCFG_GPFN0(LCD_PORT_DISABLE) | HwPORTCFG_GPFN1(LCD_PORT_DISABLE));

			BITCSET(HwGPIOC->GPEN, Hw17|Hw16, Hw17|Hw16);
			BITCLR(HwGPIOC->GPDAT, Hw17|Hw16);
	
		case 16:
			BITCSET(HwGPIOC->GPFN1, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | 
									HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK | HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK,
									HwPORTCFG_GPFN0(LCD_PORT_DISABLE) | HwPORTCFG_GPFN1(LCD_PORT_DISABLE) | HwPORTCFG_GPFN2(LCD_PORT_DISABLE) | 
									HwPORTCFG_GPFN3(LCD_PORT_DISABLE) | HwPORTCFG_GPFN4(LCD_PORT_DISABLE) | HwPORTCFG_GPFN5(LCD_PORT_DISABLE) | 
									HwPORTCFG_GPFN6(LCD_PORT_DISABLE) | HwPORTCFG_GPFN7(LCD_PORT_DISABLE));
			BITCSET(HwGPIOC->GPFN0, HwPORTCFG_GPFN0_MASK | HwPORTCFG_GPFN1_MASK | HwPORTCFG_GPFN2_MASK | HwPORTCFG_GPFN3_MASK | 
									HwPORTCFG_GPFN4_MASK | HwPORTCFG_GPFN5_MASK | HwPORTCFG_GPFN6_MASK | HwPORTCFG_GPFN7_MASK,
									HwPORTCFG_GPFN0(LCD_PORT_DISABLE) | HwPORTCFG_GPFN1(LCD_PORT_DISABLE) | HwPORTCFG_GPFN2(LCD_PORT_DISABLE) | 
									HwPORTCFG_GPFN3(LCD_PORT_DISABLE) | HwPORTCFG_GPFN4(LCD_PORT_DISABLE) | HwPORTCFG_GPFN5(LCD_PORT_DISABLE) | 
									HwPORTCFG_GPFN6(LCD_PORT_DISABLE) | HwPORTCFG_GPFN7(LCD_PORT_DISABLE));

			BITCSET(HwGPIOC->GPEN, 0x0000FFFF, 0x0000FFFF);
			BITCLR(HwGPIOC->GPDAT, 0x0000FFFF);
			break;

		default:
			// do nothing
			break;
	}
	
	
}

void LCDC_IO_SetCurrent(char lcdctrl_num, unsigned bit_per_pixel)
{
	// lcd port current 
	BITCSET(HwGPIOC->GPCD0,0xFFFFFFFF, 0xAAAAAAAA);		
	BITCSET(HwGPIOC->GPCD1,0x00FFFFFF, 0x00FFAAAA);

	BITCSET(HwGPIOC->GPFN3, HwPORTCFG_GPFN0_MASK , HwPORTCFG_GPFN0(0));
	BITCSET(HwGPIOC->GPEN, Hw24 ,Hw24);
	BITCSET(HwGPIOC->GPDAT, Hw24 ,0);
}


//#define LCDC_AXI_BUS

void lcdc_initialize(char lcdctrl_num, struct lcd_panel *lcd_spec)
{
	unsigned int lcd_reg = 0;
	PLCDC pLCDC;

	if(lcdctrl_num) // Set the address for LCDC0 or LCDC1
		pLCDC = (LCDC *)HwLCDC1_BASE;
	else
		pLCDC = (LCDC *)HwLCDC0_BASE;
	

	if(lcd_spec->bus_width == 24)
		lcd_reg |= (0xC<<16);
	else if(lcd_spec->bus_width == 18)
		lcd_reg |= (0x5<<16);
	else
		lcd_reg |= (0x3<<16);

	if(lcd_spec->sync_invert & ID_INVERT)
		lcd_reg |= Hw15;

	if(lcd_spec->sync_invert & IV_INVERT)
		lcd_reg |= Hw14;

	if(lcd_spec->sync_invert & IH_INVERT)
		lcd_reg |= Hw13;

	if(lcd_spec->sync_invert & IP_INVERT)
		lcd_reg |= Hw12;

#ifdef CONFIG_LCD_CPU_INTERFACE
	lcd_reg |= Hw23; // clock gating enable 
#endif//

#ifdef LCDC_AXI_BUS
	lcd_reg |= Hw27; // AXI bus
#endif//

	lcd_reg |=	Hw8 |			// Non-interlace
				(0x2<<5)|		// TFT-LCD mode: STN(Hw5), TFT(Hw6), TV(Hw7)
				//Hw4 | 		// Master Select for IMG0
				(0x5<<1)|		// OVP[3:1] - 5 : Image2 > Image1 > Image0
				//Hw0 | 		// LCD Controller Enable
				0;			// End Of Value

	pLCDC->LCTRL = lcd_reg;

// Set LCD clock
	pLCDC->LCLKDIV = ( (1 << 16) | lcd_spec->clk_div/2); //((PCK_LCD+1)/2)/((LCD_DEVIDE)*2)

// Background color
	pLCDC->LBC = 0x80000000;

//	Horizontal timing
	pLCDC->LHTIME1 = ((lcd_spec->lpw) << 16) | (lcd_spec->xres - 1);
	pLCDC->LHTIME2 = ((lcd_spec->lswc) << 16) | (lcd_spec->lewc);

//	Vertical timing
	pLCDC->LVTIME1 = ((lcd_spec->fpw1) << 16) | (lcd_spec->yres- 1);
	pLCDC->LVTIME2 = ((lcd_spec->fswc1) << 16) | (lcd_spec->fewc1);
	pLCDC->LVTIME3 = ((lcd_spec->fpw2) << 16) | (lcd_spec->yres - 1);
	pLCDC->LVTIME4 = ((lcd_spec->fswc2) << 16) | (lcd_spec->fewc2);

//	Display Size
	pLCDC->LDS = (lcd_spec->yres << 16) | lcd_spec->xres;

//	Interrupt mask
	pLCDC->LIM &= 0xFFFFFFE6;
	pLCDC->LIM |= ((1 << 4) | 1);
	
	pLCDC->LCTRL |= Hw0;
}


unsigned int tcc_LCDC_set_gamma(lcdc_gamma_params *gamma)
{
	volatile PLCDC pstLCDC;
	unsigned uRegValue[2];

	if(gamma->lcdc_num == 0)
		pstLCDC =(LCDC *)HwLCDC0_BASE;
	else
		pstLCDC =(LCDC *)HwLCDC1_BASE;
	
	if(gamma->onoff)
		BITSET(pstLCDC->LCTRL, HwLCTRL_GEN);
	else
		BITCLR(pstLCDC->LCTRL, HwLCTRL_GEN);

	uRegValue[0] = gamma->GammaRed[0] | (gamma->GammaRed[1]<<4) | (gamma->GammaRed[2]<<8) | (gamma->GammaRed[3]<<12)
				  | (gamma->GammaRed[4]<<16) |(gamma->GammaRed[5]<<20) |( gamma->GammaRed[6]<<24) | (gamma->GammaRed[7]<<28) ;
	uRegValue[1] = gamma->GammaRed[8] | (gamma->GammaRed[9]<<4) | (gamma->GammaRed[10]<<8) | (gamma->GammaRed[11]<<12)
				  | (gamma->GammaRed[12]<<16) |(gamma->GammaRed[13]<<20) |( gamma->GammaRed[14]<<24) | (gamma->GammaRed[15]<<28) ;
	pstLCDC->LGR0 = uRegValue[0];
	pstLCDC->LGR1 = uRegValue[1];
	

	uRegValue[0] = gamma->GammaGreen[0] | (gamma->GammaGreen[1]<<4) | (gamma->GammaGreen[2]<<8) | (gamma->GammaGreen[3]<<12)
				  | (gamma->GammaGreen[4]<<16) |(gamma->GammaGreen[5]<<20) |( gamma->GammaGreen[6]<<24) | (gamma->GammaGreen[7]<<28) ;
	uRegValue[1] = gamma->GammaGreen[8] | (gamma->GammaGreen[9]<<4) | (gamma->GammaGreen[10]<<8) | (gamma->GammaGreen[11]<<12)
				  | (gamma->GammaGreen[12]<<16) |(gamma->GammaGreen[13]<<20) |( gamma->GammaGreen[14]<<24) | (gamma->GammaGreen[15]<<28) ;
	pstLCDC->LGG0 = uRegValue[0];
	pstLCDC->LGG1 = uRegValue[1];


	uRegValue[0] = gamma->GammaBlue[0] | (gamma->GammaBlue[1]<<4) | (gamma->GammaBlue[2]<<8) | (gamma->GammaBlue[3]<<12)
				  | (gamma->GammaBlue[4]<<16) |(gamma->GammaBlue[5]<<20) |( gamma->GammaBlue[6]<<24) | (gamma->GammaBlue[7]<<28) ;
	uRegValue[1] = gamma->GammaBlue[8] | (gamma->GammaBlue[9]<<4) | (gamma->GammaBlue[10]<<8) | (gamma->GammaBlue[11]<<12)
				  | (gamma->GammaBlue[12]<<16) |(gamma->GammaBlue[13]<<20) |( gamma->GammaBlue[14]<<24) | (gamma->GammaBlue[15]<<28) ;
	pstLCDC->LGB0 = uRegValue[0];
	pstLCDC->LGB1 = uRegValue[1];

	return 0;	
}

void tcclcd_image_ch_set(char lcdctrl_num, struct tcc_lcdc_image_update *ImageInfo)
{
	unsigned int  y2r_option;
	PLCDC_IMG pLCDC_channel;
	PLCDC pLCDC;

	if(lcdctrl_num) // Set the address for LCDC0 or LCDC1
		pLCDC = (LCDC *)HwLCDC1_BASE;
	else
		pLCDC = (LCDC *)HwLCDC0_BASE;
	
	pLCDC_channel = (PLCDC_IMG)&(pLCDC->LI2C);

	printf("%s lcdc:%d pLCD_ch :0x%x  \n", __func__, lcdctrl_num, *pLCDC_channel);
	switch(ImageInfo->fmt)
	{
		case TCC_LCDC_IMG_FMT_RGB444:
		case TCC_LCDC_IMG_FMT_RGB565:
		case TCC_LCDC_IMG_FMT_RGB555:
			pLCDC_channel->LIxO = ImageInfo->Frame_width *2;
			y2r_option = 0;
			break;

		case TCC_LCDC_IMG_FMT_RGB888:
		case TCC_LCDC_IMG_FMT_RGB666:
			pLCDC_channel->LIxO = ImageInfo->Frame_width * 4;
			y2r_option = 0;
			break;
		
		case TCC_LCDC_IMG_FMT_YUV420SP:
		case TCC_LCDC_IMG_FMT_YUV422SP:
			pLCDC_channel->LIxO = ((ImageInfo->Frame_width / 2)<<16)|(ImageInfo->Frame_width);
			y2r_option = HwLIC_Y2R;
			break;

		case TCC_LCDC_IMG_FMT_YUV422SQ:
			pLCDC_channel->LIxO = ImageInfo->Frame_width * 2;
			y2r_option = HwLIC_Y2R;
			break;

		case TCC_LCDC_IMG_FMT_YUV420ITL0:
		case TCC_LCDC_IMG_FMT_YUV420ITL1:
		case TCC_LCDC_IMG_FMT_YUV422ITL0:
		case TCC_LCDC_IMG_FMT_YUV422ITL1:
			pLCDC_channel->LIxO = (ImageInfo->Frame_width << 16)| (ImageInfo->Frame_width);
			y2r_option = HwLIC_Y2R;
			break;
			
		default:
			pLCDC_channel->LIxO = ImageInfo->Frame_width *2;
			y2r_option = HwLIC_Y2R;
			break;
	}

	pLCDC_channel->LIxS =((ImageInfo->Image_height << 16) | (ImageInfo->Image_width));	// size

	// image address
	pLCDC_channel->LIxBA0 = ImageInfo->addr0;
	pLCDC_channel->LIxBA1 = ImageInfo->addr1;
	pLCDC_channel->LIxBA2 = ImageInfo->addr2;

	if(pLCDC->LCTRL & HwLCTRL_NI)
	{
		// position
		pLCDC_channel->LIxP =((ImageInfo->offset_y << 16) | (ImageInfo->offset_x));
		//channel control
		pLCDC_channel->LIxC = HwLIC_IEN | HwLIC_UPD | y2r_option | ImageInfo->fmt;
	}
	else
	{
		// position
		pLCDC_channel->LIxP =(((ImageInfo->offset_y/2) << 16) | (ImageInfo->offset_x));
		//channel control
		pLCDC_channel->LIxC = HwLIC_INTL | HwLIC_IEN | HwLIC_UPD | y2r_option | ImageInfo->fmt;
	}
}


//DDIC0_CHn_SEL CACHE Selection
#define DDIC_LCD0_DMA_0_0		0
#define DDIC_LCD0_DMA_0_1		1
#define DDIC_LCD0_DMA_0_2		2
#define DDIC_LCD0_DMA_1			3	
#define DDIC_LCD0_DMA_2			4
#define DDIC_MSCL0_DMA_0		5
#define DDIC_MSCL0_DMA_1		6
#define DDIC_MSCL0_DMA_2		7
#define DDIC_VIQE_DMA_0_0		8
#define DDIC_VIQE_DMA_0_1		9
#define DDIC_VIQE_DMA_0_2		10
#define DDIC_LCD0_DMA_3			11
#define DDIC_LCD1_DMA_3			12
#define DDIC_Reserved				13


// DDIC1_CHn_SEL CACHE Selection
#define DDIC_LCD1_DMA_0_0		0
#define DDIC_LCD1_DMA_0_1		1
#define DDIC_LCD1_DMA_0_2		2
#define DDIC_LCD1_DMA_1			3	
#define DDIC_LCD1_DMA_2			4
#define DDIC_MSCL1_DMA_0		5
#define DDIC_MSCL1_DMA_1		6
#define DDIC_MSCL1_DMA_2		7
#define DDIC_VIQE_DMA_1_0		8
#define DDIC_VIQE_DMA_1_1		9
#define DDIC_VIQE_DMA_1_2		10
#define DDIC_VIQE_DMA_2_0		11
#define DDIC_VIQE_DMA_2_1		12
#define DDIC_VIQE_DMA_2_2		13


void ddic_setdefault(void)
{
    int ret = 0;
    PSDDICACHE pCache =  (PSDDICACHE)HwDDICACHE_BASE;
    PSDDICONFIG pDDICFG = (PSDDICONFIG)HwDDI_CONFIG_BASE;
    PDDICACHE pCsh = (PDDICACHE)pCache;

    SDDICACHE Cache = {0,};    
    
    UPWDN      upwdn    = *(&pDDICFG->uPWDN);
    upwdn.B.ddic = 0;
    *((unsigned int*)&pDDICFG->uPWDN) = upwdn.pwdn;
    
    //Cache.uDDICCTRL.B.ddic0_en = ();
    //Cache.uDDICCTRL.B.ddic1_en = ();

#ifdef LCDC_AXI_BUS
    Cache.uDDICCFG00.B.sel00 = DDIC_MSCL0_DMA_0;
    Cache.uDDICCFG00.B.sel01 = DDIC_MSCL0_DMA_1;
    Cache.uDDICCFG00.B.sel02 = DDIC_MSCL0_DMA_2;
    Cache.uDDICCFG00.B.sel03 = DDIC_LCD0_DMA_0_0;
    Cache.uDDICCFG00.B.sel04 = DDIC_LCD0_DMA_0_1;
    Cache.uDDICCFG00.B.sel05 = DDIC_LCD0_DMA_2;
    
    Cache.uDDICCFG01.B.sel06 = DDIC_VIQE_DMA_0_0;
    Cache.uDDICCFG01.B.sel07 = DDIC_VIQE_DMA_0_1;
    Cache.uDDICCFG01.B.sel08 = DDIC_LCD0_DMA_0_2;
    Cache.uDDICCFG01.B.sel09 = DDIC_LCD0_DMA_1;
    Cache.uDDICCFG01.B.sel10 = DDIC_VIQE_DMA_0_2;
    Cache.uDDICCFG01.B.sel11 = DDIC_LCD0_DMA_3;
    Cache.uDDICCFG01.B.sel12 = DDIC_LCD1_DMA_3;
    Cache.uDDICCFG01.B.sel13 = DDIC_Reserved;

    Cache.uDDICCFG10.B.sel00 = DDIC_MSCL1_DMA_0;
    Cache.uDDICCFG10.B.sel01 = DDIC_MSCL1_DMA_1;
    Cache.uDDICCFG10.B.sel02 = DDIC_MSCL1_DMA_2;
    Cache.uDDICCFG10.B.sel03 = DDIC_VIQE_DMA_1_0;
    Cache.uDDICCFG10.B.sel04 = DDIC_VIQE_DMA_1_1;
    Cache.uDDICCFG10.B.sel05 = DDIC_VIQE_DMA_1_2;
    
    Cache.uDDICCFG11.B.sel06 = DDIC_LCD1_DMA_0_0;
    Cache.uDDICCFG11.B.sel07 = DDIC_LCD1_DMA_0_1;
    Cache.uDDICCFG11.B.sel08 = DDIC_LCD1_DMA_0_2;
    Cache.uDDICCFG11.B.sel09 = DDIC_LCD1_DMA_1;
    Cache.uDDICCFG11.B.sel10 = DDIC_LCD1_DMA_2;
    Cache.uDDICCFG11.B.sel11 = DDIC_VIQE_DMA_2_0;
    Cache.uDDICCFG11.B.sel12 = DDIC_VIQE_DMA_2_1;
    Cache.uDDICCFG11.B.sel13 = DDIC_VIQE_DMA_2_2;
#else
	Cache.uDDICCFG00.B.sel00 = DDIC_MSCL0_DMA_0;
	Cache.uDDICCFG00.B.sel01 = DDIC_MSCL0_DMA_1;
	Cache.uDDICCFG00.B.sel02 = DDIC_MSCL0_DMA_2;
	Cache.uDDICCFG00.B.sel03 = DDIC_LCD0_DMA_0_0;
	Cache.uDDICCFG00.B.sel04 = DDIC_LCD0_DMA_0_1;
	Cache.uDDICCFG00.B.sel05 = DDIC_LCD0_DMA_2;

	Cache.uDDICCFG01.B.sel06 = DDIC_VIQE_DMA_0_0;
	Cache.uDDICCFG01.B.sel07 = DDIC_VIQE_DMA_0_1;
	Cache.uDDICCFG01.B.sel08 = DDIC_LCD0_DMA_0_2;
	Cache.uDDICCFG01.B.sel09 = DDIC_LCD0_DMA_1;
	Cache.uDDICCFG01.B.sel10 = DDIC_VIQE_DMA_0_2;
	Cache.uDDICCFG01.B.sel11 = DDIC_LCD0_DMA_3;
	Cache.uDDICCFG01.B.sel12 = DDIC_LCD1_DMA_3;
	Cache.uDDICCFG01.B.sel13 = DDIC_Reserved;

	Cache.uDDICCFG10.B.sel00 = DDIC_MSCL1_DMA_0;
	Cache.uDDICCFG10.B.sel01 = DDIC_MSCL1_DMA_1;
	Cache.uDDICCFG10.B.sel02 = DDIC_MSCL1_DMA_2;
	Cache.uDDICCFG10.B.sel03 = DDIC_LCD1_DMA_0_0;
	Cache.uDDICCFG10.B.sel04 = DDIC_LCD1_DMA_0_1;
	Cache.uDDICCFG10.B.sel05 = DDIC_LCD1_DMA_2;

	Cache.uDDICCFG11.B.sel06 = DDIC_VIQE_DMA_1_0;
	Cache.uDDICCFG11.B.sel07 = DDIC_VIQE_DMA_1_1;
	Cache.uDDICCFG11.B.sel08 = DDIC_LCD1_DMA_0_2;
	Cache.uDDICCFG11.B.sel09 = DDIC_LCD1_DMA_1;
	Cache.uDDICCFG11.B.sel10 = DDIC_VIQE_DMA_1_2;
	Cache.uDDICCFG11.B.sel11 = DDIC_VIQE_DMA_2_0;
	Cache.uDDICCFG11.B.sel12 = DDIC_VIQE_DMA_2_1;
	Cache.uDDICCFG11.B.sel13 = DDIC_VIQE_DMA_2_2;
#endif//

    pCsh->DDIC0_CH0_AMAX = 0xFFFFFFFF;
    pCsh->DDIC0_CH1_AMAX = 0xFFFFFFFF;
    pCsh->DDIC0_CH2_AMAX = 0xFFFFFFFF;
    pCsh->DDIC0_CH3_AMAX = 0xFFFFFFFF;
    pCsh->DDIC0_CH4_AMAX = 0xFFFFFFFF;
    pCsh->DDIC0_CH5_AMAX = 0xFFFFFFFF;
    pCsh->DDIC1_CH0_AMAX = 0xFFFFFFFF;
    pCsh->DDIC1_CH1_AMAX = 0xFFFFFFFF;
    pCsh->DDIC1_CH2_AMAX = 0xFFFFFFFF;
    pCsh->DDIC1_CH3_AMAX = 0xFFFFFFFF;
    pCsh->DDIC1_CH4_AMAX = 0xFFFFFFFF;
    pCsh->DDIC1_CH5_AMAX = 0xFFFFFFFF;
    
    Cache.uDDICCTRL.ddicctrl = 0x003F003F; // All Cache Enable
    *pCache = Cache;

    return ret;
}



void tcclcd_gpio_config(unsigned n, unsigned flags)
{
	if(n == GPIO_NC)
	{
		// no connect
	}
	else if(n >= GPIO_EXT1)
	{
		// extend gpio 
	}
	else
	{
		gpio_config(n, flags);
	}
}
void tcclcd_gpio_set_value(unsigned n, unsigned on)
{
	if(n == GPIO_NC)
	{
		// no connect
	}
	else if(n >= GPIO_EXT1)
	{
		// extend gpio 
	}
	else
	{
		gpio_set(n, on);
	}

}
