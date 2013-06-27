/*
 * dx08d11vm0aaa_lcd.c -- support for Hitachi DX08D11VM0AAA LCD
 *
 * Copyright (C) 2009, 2010 Telechips, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <debug.h>
#include <stdlib.h>
#include <string.h>
#include <tcc_lcd.h>

#ifdef TD043MGEB1
#include <dev/gpio.h>

#define WAIT_FLAG	0xFF
#define END_FLAG	0xFE

unsigned char gLCDInit_Table[][2]=
{
	{0x02, 0x07}, 
	{0x03, 0x5F},
	{0x04, 0x17},
	{0x05, 0x20}, 
	{0x06, 0x08}, 
	{0x07, 0x20}, 
	{0x08, 0x20}, 
	{0x09, 0x20}, 
	{0x0A, 0x20}, 
	
	{0x0B, 0x20}, 
	{0x0C, 0x20}, 
	{0x0D, 0x20}, 

	{0x0E, 0x10}, 
	{0x0F, 0x10}, 
	{0x10, 0x10}, 
	{0x11, 0x15}, 
	{0x12, 0xAA}, 
	{0x13, 0xFF}, 
	{0x14, 0x86}, 
	{0x15, 0x8d}, 
	{0x16, 0xd4}, 
	{0x17, 0xfb}, 
	{0x18, 0x2e}, 
	{0x19, 0x5a}, 
	{0x1A, 0x89}, 
	{0x1B, 0xfe}, 
	{0x1C, 0x5a}, 
	{0x1D, 0x9b}, 
	{0x1E, 0xc5}, 
	{0x1F, 0xFF}, 
	{0x20, 0xF0}, 
	{0x21, 0xF0}, 
	{0x22, 0x08}, 
	{WAIT_FLAG, 4},//wait 20ms
	{END_FLAG, 0x0}
};

void GPSB_Set_Data(unsigned char Reg, unsigned char Data)
{
	unsigned short value;

	value = (Reg << 10) | Data;
	IO_GPSB_TxRxData(DRV_GPSB_CH1, &value, 0, 1);
	lcd_delay_us(10);
}
static sDRV_GPSB	gDRV_SPI_LCD =
{
	0 
	| DRV_GPSB_MODE_OpMod_SPI 			// 0 : spi
	| DRV_GPSB_MODE_MasterMode			// 0 : Master mode enable	
	| DRV_GPSB_MODE_BitWidth(15)		// 31 : Bit width selection, 32bits = 31+1
//	| DRV_GPSB_MODE_SCKPolCtrl_High		// 16
	| DRV_GPSB_MODE_EnableOperation		// 3
//	| DRV_GPSB_MODE_RcvRisingEdge		// 17
//	| DRV_GPSB_MODE_TransRisingEdge		// 18
	| DRV_GPSB_MODE_ClearRcvFifoCount	// 1 : Assert Rx FIFO Counter Clear
//	| DRV_GPSB_MODE_SCKPolCtrl_High		// 1 : SCKI inverted. for SPI timing 1, 2	
//	, 90000								// Clock Divider
	, 60000								// Clock Divider
	, DRV_GPSB_CTRL_HW					// Hardware Control
	, DRV_GPSB_SDOSDI_SDO				// SPI Output Only
	, 0
};

static void TD043MGEB1_on(void)
{
	unsigned int i = 0;

	GPSB_SETConfig(&gDRV_SPI_LCD, DRV_GPSB_CH1, DRV_GPSB_PORT10);

	while(gLCDInit_Table[i][0] != END_FLAG)
	{
		if(gLCDInit_Table[i][0] == WAIT_FLAG)
			lcd_delay_us(gLCDInit_Table[i][1]);
		else
			GPSB_Set_Data(gLCDInit_Table[i][0], gLCDInit_Table[i][1]);
		i++;			
	}

	GPSB_Port_disable();

}

static int td043mgeb1_panel_init(struct lcd_panel *panel)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d  \n", __func__, 0);

	tcclcd_gpio_config(pdata->display_on, 1);
	tcclcd_gpio_config(pdata->bl_on, 1);
	tcclcd_gpio_config(pdata->reset, 1);
	tcclcd_gpio_config(pdata->power_on, 1);

	return 0;
}

static int td043mgeb1_set_power(struct lcd_panel *panel, int on)
{
	struct lcd_platform_data *pdata = &(panel->dev);


	printf("%s : %d \n", __func__, on);
	if (on) {
		tcclcd_gpio_set_value(pdata->power_on, 1);
		lcd_delay_us(1000);

		tcclcd_gpio_set_value(pdata->display_on, 1);
		lcd_delay_us(1000);

		tcclcd_gpio_set_value(pdata->reset, 1);
		lcd_delay_us(100);

		mdelay(10);
		TD043MGEB1_on();
		lcdc_initialize(pdata->lcdc_num, panel);
		
		LCDC_IO_Set(pdata->lcdc_num, panel->bus_width);

		mdelay(16);
		
		tcclcd_gpio_set_value(pdata->display_on, 1);

	}
	else 
	{
		tcclcd_gpio_set_value(pdata->display_on, 0);
		mdelay(10);
		tcclcd_gpio_set_value(pdata->reset, 0);
		tcclcd_gpio_set_value(pdata->power_on, 0);
		LCDC_IO_Disable(pdata->lcdc_num, panel->bus_width);
	}

	return 0;
}


static int td043mgeb1_set_backlight_level(struct lcd_panel *panel, int level)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d\n", __func__, level);

	//LCD_BL_ON ¸¦ LOW·Î ÇØ¾ß lcd backlight control µÊ
	BITCSET(HwGPIOC->GPEN, Hw29, Hw29);
	BITCLR(HwGPIOC->GPDAT, Hw29);

	if (level == 0) {
		tcclcd_gpio_set_value(pdata->bl_on, 0);
	} else {
//		tcclcd_gpio_set_value(pdata->bl_on, 1);
		#ifdef TCC892X
		if(HW_REV == 0x1005 || HW_REV == 0x1006 || HW_REV == 0x1007|| HW_REV == 0x1008 || HW_REV == 0x2002 || HW_REV == 0x2003 || (HW_REV == 0x2004) || (HW_REV == 0x2005))
			tca_tco_pwm_ctrl(0, pdata->bl_on, MAX_BACKLIGTH, level);
		else
			tca_tco_pwm_ctrl(1, pdata->bl_on, MAX_BACKLIGTH, level);

		#else
			tca_tco_pwm_ctrl(0, pdata->bl_on, MAX_BACKLIGTH, level);
		#endif//
	}

	return 0;
}


static struct lcd_panel td043mgeb1_panel = {
	.name		= "TD043MGEB1",
	.manufacturer	= "TPO",
	.id		= PANEL_ID_TD043MGEB1,
	.xres		= 800,
	.yres		= 480,
	.width		= 92,
	.height		= 56,
	.bpp		= 32,
	.clk_freq	= 340000,
	.clk_div	= 2,
	.bus_width	= 24,
	.lpw		= 0,
	.lpc		= 800,
	.lswc		= 215,
	.lewc		= 37,
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 0,
	.flc1		= 480,
	.fswc1		= 34,
	.fewc1		= 9,
	.fpw2		= 0,
	.flc2		= 480,
	.fswc2		= 34,
	.fewc2		= 9,
	.sync_invert	= IV_INVERT | IH_INVERT,
	.init		= td043mgeb1_panel_init,
	.set_power	= td043mgeb1_set_power,
	.set_backlight_level = td043mgeb1_set_backlight_level,
};
struct lcd_panel *tccfb_get_panel(void)
{
	return &td043mgeb1_panel;
}
#endif//TD043MGEB1
