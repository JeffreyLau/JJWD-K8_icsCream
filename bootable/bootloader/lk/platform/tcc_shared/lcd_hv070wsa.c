/*
 *	HV070WSA.c -- support for HV070WSA LVDS Panel
 *
 *  Copyright (C) 2009, 2010 Telechips, Inc.
 *  
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */



#include <debug.h>
#include <stdlib.h>
#include <string.h>
#include <tcc_lcd.h>


#include <debug.h>
#include <stdlib.h>
#include <string.h>
#include <reg.h>
#include <platform/iomap.h>
#include <dev/fbcon.h>

#ifdef HV070WSA
#include <dev/gpio.h>
#include <platform/gpio.h>
#include <platform/globals.h>
#include <tnftl/IO_TCCXXX.h>
#include <platform/gpio.h>
#include <platform/reg_physical.h>

extern void tca_ckc_setpmupwroff( unsigned int periname , unsigned int isenable);

static int hv070wsa_panel_init(struct lcd_panel *panel)
{
	struct lcd_platform_data *pdata = &(panel->dev);

#if defined(TCC892X)
	if(pdata->lcdc_num ==1)
		pdata->reset = (GPIO_PORTE |27);
	else
		pdata->reset = (GPIO_PORTB |19);
#endif//

	tcclcd_gpio_config(pdata->display_on, GPIO_OUTPUT);
	tcclcd_gpio_config(pdata->bl_on, GPIO_OUTPUT);
	tcclcd_gpio_config(pdata->reset, GPIO_OUTPUT);
	tcclcd_gpio_config(pdata->power_on, GPIO_OUTPUT);

	tcclcd_gpio_set_value(pdata->display_on, 0);
	tcclcd_gpio_set_value(pdata->bl_on, 0);
	tcclcd_gpio_set_value(pdata->reset, 0);
	tcclcd_gpio_set_value(pdata->power_on, 0);
	printf("%s : %d\n", __func__, 0);

	return 0;
}
static int hv070wsa_set_power(struct lcd_panel *panel, int on)
{
	unsigned int P, M, S, VSEL;
	#if defined(TCC88XX) || defined(TCC892X)
	PDDICONFIG		pDDICfg 	= (DDICONFIG *)HwDDI_CONFIG_BASE;
	#else
	PDDICONFIG		pDDICfg 	= (DDICONFIG *)&HwDDI_CONFIG_BASE;
	#endif//
	
	struct lcd_platform_data *pdata = &(panel->dev);
	printf("%s : onoff:%d lcdc_num\n", __func__, on, pdata->lcdc_num);

	if (on) {
		tcclcd_gpio_set_value(pdata->power_on, 1);
		lcd_delay_us(20);

		tcclcd_gpio_set_value(pdata->reset, 1);
		tcclcd_gpio_set_value(pdata->display_on, 1);
	
		lcdc_initialize(pdata->lcdc_num, panel);

		
	#if defined(TCC892X)

		// LVDS reset	
		pDDICfg->LVDS_CTRL.bREG.RST =1;
		pDDICfg->LVDS_CTRL.bREG.RST =0;		
		#if 0
		BITCSET(pDDICfg->LVDS_TXO_SEL0.nREG, 0xFFFFFFFF, 0x15141312);
		BITCSET(pDDICfg->LVDS_TXO_SEL1.nREG, 0xFFFFFFFF, 0x0B0A1716);
		BITCSET(pDDICfg->LVDS_TXO_SEL2.nREG, 0xFFFFFFFF, 0x0F0E0D0C);
		BITCSET(pDDICfg->LVDS_TXO_SEL3.nREG, 0xFFFFFFFF, 0x05040302);
		BITCSET(pDDICfg->LVDS_TXO_SEL4.nREG, 0xFFFFFFFF, 0x1A190706);
		BITCSET(pDDICfg->LVDS_TXO_SEL5.nREG, 0xFFFFFFFF, 0x1F1E1F18);
		BITCSET(pDDICfg->LVDS_TXO_SEL6.nREG, 0xFFFFFFFF, 0x1F1E1F1E);
		BITCSET(pDDICfg->LVDS_TXO_SEL7.nREG, 0xFFFFFFFF, 0x1F1E1F1E);
		BITCSET(pDDICfg->LVDS_TXO_SEL8.nREG, 0xFFFFFFFF, 0x001E1F1E);
		#else
		BITCSET(pDDICfg->LVDS_TXO_SEL0.nREG, 0xFFFFFFFF, 0x13121110);
		BITCSET(pDDICfg->LVDS_TXO_SEL1.nREG, 0xFFFFFFFF, 0x09081514);
		BITCSET(pDDICfg->LVDS_TXO_SEL2.nREG, 0xFFFFFFFF, 0x0D0C0B0A);
		BITCSET(pDDICfg->LVDS_TXO_SEL3.nREG, 0xFFFFFFFF, 0x03020100);
		
		BITCSET(pDDICfg->LVDS_TXO_SEL4.nREG, 0xFFFFFFFF, 0x1A190504);
		BITCSET(pDDICfg->LVDS_TXO_SEL5.nREG, 0xFFFFFFFF, 0x0E171618);
		BITCSET(pDDICfg->LVDS_TXO_SEL6.nREG, 0xFFFFFFFF, 0x1F07060F);
		BITCSET(pDDICfg->LVDS_TXO_SEL7.nREG, 0xFFFFFFFF, 0x1F1E1F1E);
		BITCSET(pDDICfg->LVDS_TXO_SEL8.nREG, 0xFFFFFFFF, 0x001E1F1E);
		#endif//
		M = 10; P = 10; S = 0; VSEL = 0;		
		BITCSET(pDDICfg->LVDS_CTRL.nREG, 0x00FFFFF0, (VSEL<<4)|(S<<5)|(M<<8)|(P<<15)); //LCDC1

		// LVDS Select LCDC 1
		if(pdata->lcdc_num ==1)
		BITCSET(pDDICfg->LVDS_CTRL.nREG, 0x3 << 30 , 0x1 << 30);
		else
			BITCSET(pDDICfg->LVDS_CTRL.nREG, 0x3 << 30 , 0x0 << 30);

	    	pDDICfg->LVDS_CTRL.bREG.RST = 1;	//  reset
	    		
	    	// LVDS enable
	  	pDDICfg->LVDS_CTRL.bREG.EN = 1;   // enable
	  	
	#else	
		// LVDS power on
		tca_ckc_setpmupwroff(PMU_LVDSPHY, ENABLE);

	    // LVDS reset
	    BITSET(pDDICfg->LVDS_CTRL, Hw1);    // reset
	    BITCLR(pDDICfg->LVDS_CTRL, Hw1);    // normal

		pDDICfg->LVDS_TXO_SEL0 = 0x15141312; // SEL_03, SEL_02, SEL_01, SEL_00,
		pDDICfg->LVDS_TXO_SEL1 = 0x0B0A1716; // SEL_07, SEL_06, SEL_05, SEL_04,
		pDDICfg->LVDS_TXO_SEL2 = 0x0F0E0D0C; // SEL_11, SEL_10, SEL_09, SEL_08,
		pDDICfg->LVDS_TXO_SEL3 = 0x05040302; // SEL_15, SEL_14, SEL_13, SEL_12,
		pDDICfg->LVDS_TXO_SEL4 = 0x1A190706; // SEL_19, SEL_18, SEL_17, SEL_16,
		pDDICfg->LVDS_TXO_SEL5 = 0x1F1E1F18; //                         SEL_20,
		pDDICfg->LVDS_TXO_SEL6 = 0x1F1E1F1E;
		pDDICfg->LVDS_TXO_SEL7 = 0x1F1E1F1E;
		pDDICfg->LVDS_TXO_SEL8 = 0x1F1E1F1E;
		
		// LVDS Select
	//	BITCLR(pDDICfg->LVDS_CTRL, Hw0); //LCDC0
		BITSET(pDDICfg->LVDS_CTRL, Hw0); //LCDC1

		#ifdef TCC88XX
		M = 10; P = 10; S = 0; VSEL = 0;
		
		BITCSET(pDDICfg->LVDS_CTRL, Hw21- Hw4, (VSEL<<4)|(S<<5)|(M<<8)|(P<<15)); //LCDC1

		BITSET(pDDICfg->LVDS_CTRL, Hw1);    // reset
		#endif//
		
	    	// LVDS enable
	  	 BITSET(pDDICfg->LVDS_CTRL, Hw2);    // enable
	#endif
	
	}
	else 	{
		tcclcd_gpio_set_value(pdata->power_on, 0);
	}
	return 0;
}

static int hv070wsa_set_backlight_level(struct lcd_panel *panel, int level)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d\n", __func__, level);
	
	if (level == 0) {
		tcclcd_gpio_set_value( pdata->bl_on, 0);	// LVDS_BL_off
	} else {

		tcclcd_gpio_set_value( pdata->bl_on ,1);	// LVDS_BL_EN
	}
		

	return 0;
}

static struct lcd_panel hv070wsa_panel = {
	.name		= "HV070WSA",
	.manufacturer	= "BOE",
	.id			= PANEL_ID_HV070WSA,			// CLAA070NP01 is announced in the tcclcd.h (in /tcc892x)	
	.xres		= 1024,
	.yres		= 600,	
	.width		= 153,
	.height		= 90,
	.bpp			= 24,
	.clk_freq		= 512000, 
	.clk_div		= 2,	
	.bus_width	= 24,		
	.lpw			= 0,
	.lpc			= 1024,			
	.lswc		= 0,		
	.lewc		= 323,	
	.vdb			= 0,
	.vdf			= 0,	
	.fpw1		= 0,
	.flc1			= 600,	
	.fswc1		= 0,
	.fewc1		= 34,	
	.fpw2		= 0,
	.flc2			= 600,
	.fswc2		= 0,
	.fewc2		= 34,
	.sync_invert	= IV_INVERT | IH_INVERT,
	.init		= hv070wsa_panel_init,
	.set_power	= hv070wsa_set_power,
	.set_backlight_level = hv070wsa_set_backlight_level,
};

struct lcd_panel *tccfb_get_panel(void)
{
	return &hv070wsa_panel;

}
#endif//HV070WSA


