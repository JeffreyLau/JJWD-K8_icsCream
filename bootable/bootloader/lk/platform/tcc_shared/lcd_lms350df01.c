/*
 * lms350df01_lcd.c -- support for Samsung LMS350DF01 LCD
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

#ifdef LMS350DF01

static int gDRV_SPI_Handle_Test = -1;

static sDRV_GPSB	gDRV_SPI_Test =
{
	0 
	| DRV_GPSB_MODE_OpMod_SPI 			// 0 : spi
	| DRV_GPSB_MODE_MasterMode			// 0 : Master mode enable	
	| DRV_GPSB_MODE_BitWidth(23)		// 31 : Bit width selection, 32bits = 31+1
	| DRV_GPSB_MODE_SCKPolCtrl_High		// 16
	| DRV_GPSB_MODE_EnableOperation		// 3
	| DRV_GPSB_MODE_RcvRisingEdge		// 17
	| DRV_GPSB_MODE_TransRisingEdge		// 18
	| DRV_GPSB_MODE_ClearRcvFifoCount	// 1 : Assert Rx FIFO Counter Clear
//	| DRV_GPSB_MODE_SCKPolCtrl_High		// 1 : SCKI inverted. for SPI timing 1, 2	

//	, 90000								// Clock Divider
	, 60000								// Clock Divider
	, DRV_GPSB_CTRL_HW					// Hardware Control
	, DRV_GPSB_SDOSDI_SDO				// SPI Input Only
	, 0
};



static void LCD_SPI_WRITE_CMDDATA(unsigned short cmd, unsigned short data)
{
	unsigned int value;
	volatile unsigned int cmd_reg = 0x740000;
	volatile unsigned int data_reg = 0x760000;

	value = (cmd_reg | (cmd & 0xFFFF));
	IO_GPSB_TxRxData(DRV_GPSB_CH1, &value, 0, 1);
	value = (data_reg | (data & 0xFFFF));
	IO_GPSB_TxRxData(DRV_GPSB_CH1, &value, 0, 1);
}

static void LMS350DF01_PL_initialize(void)
{
	LCD_SPI_WRITE_CMDDATA(0x07, 0x0000);
		//wait moer than 6frames
	lcd_delay_us(1000 * 16 * 10);
	
	LCD_SPI_WRITE_CMDDATA(0x11, 0x222F);
	LCD_SPI_WRITE_CMDDATA(0x12, 0x0F00);
	LCD_SPI_WRITE_CMDDATA(0x13, 0x7FE3);
	LCD_SPI_WRITE_CMDDATA(0x76, 0x2213);
	LCD_SPI_WRITE_CMDDATA(0x74, 0x0001);
	LCD_SPI_WRITE_CMDDATA(0x76, 0x0000);	
	LCD_SPI_WRITE_CMDDATA(0x10, 0x560C);
	//wait moer than 6frames
	lcd_delay_us(1000 * 16);
	
	LCD_SPI_WRITE_CMDDATA(0x12, 0x0C63);
	//wait moer than 5frames	
	lcd_delay_us(1000 * 16);
	
	LCD_SPI_WRITE_CMDDATA(0x01, 0x0B3B);
	LCD_SPI_WRITE_CMDDATA(0x02, 0x0300);
	LCD_SPI_WRITE_CMDDATA(0x03, 0xC040);
	LCD_SPI_WRITE_CMDDATA(0x08, 0x0004);
	LCD_SPI_WRITE_CMDDATA(0x09, 0x000B);
	
	LCD_SPI_WRITE_CMDDATA(0x76, 0x2213);
	LCD_SPI_WRITE_CMDDATA(0x0B, 0x3340);
	LCD_SPI_WRITE_CMDDATA(0x0C, 0x0020);
	LCD_SPI_WRITE_CMDDATA(0x1C, 0x7770);
	LCD_SPI_WRITE_CMDDATA(0x76, 0x0000);
	LCD_SPI_WRITE_CMDDATA(0x0D, 0x0000);
	LCD_SPI_WRITE_CMDDATA(0x0E, 0x0500);
	LCD_SPI_WRITE_CMDDATA(0x14, 0x0000);
	LCD_SPI_WRITE_CMDDATA(0x15, 0x0803);
	LCD_SPI_WRITE_CMDDATA(0x16, 0x0000);
	LCD_SPI_WRITE_CMDDATA(0x30, 0x0005);
	LCD_SPI_WRITE_CMDDATA(0x31, 0x070F);
	LCD_SPI_WRITE_CMDDATA(0x32, 0x0300);
	LCD_SPI_WRITE_CMDDATA(0x33, 0x0003);
	LCD_SPI_WRITE_CMDDATA(0x34, 0x090C);
	LCD_SPI_WRITE_CMDDATA(0x35, 0x0001);
	LCD_SPI_WRITE_CMDDATA(0x36, 0x0001);
	LCD_SPI_WRITE_CMDDATA(0x37, 0x0303);
	LCD_SPI_WRITE_CMDDATA(0x38, 0x0F09);
	LCD_SPI_WRITE_CMDDATA(0x39, 0x0105);

	//Display ON Sequence
	LCD_SPI_WRITE_CMDDATA(0x07, 0x0001);
	//wait moer than 5frames		
	lcd_delay_us(1000 * 16);

	LCD_SPI_WRITE_CMDDATA(0x07, 0x0101);
	//wait moer than 5frames		
	lcd_delay_us(1000 * 16);

	LCD_SPI_WRITE_CMDDATA(0x07, 0x0103);

}

static void lcd_on(void)
{
	// Install Interrupt Handler
	#if defined(TCC88XX)
	GPSB_SETConfig(&gDRV_SPI_Test, DRV_GPSB_CH1, DRV_GPSB_PORT10);
	#else
	GPSB_SETConfig(&gDRV_SPI_Test, DRV_GPSB_CH1, DRV_GPSB_PORT9);
	#endif

	LMS350DF01_PL_initialize();
	GPSB_Port_disable();
	BITCLR(HwGPIOF->GPEN, Hw25);
	
}

static int lms350df01_panel_init(struct lcd_panel *panel)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d\n", __func__, 0);
	tcclcd_gpio_config(pdata->display_on, 1);
	tcclcd_gpio_config(pdata->bl_on, 1);
	tcclcd_gpio_config(pdata->reset, 1);
	tcclcd_gpio_config(pdata->power_on, 1);

	tcclcd_gpio_set_value(pdata->display_on, 0);
	tcclcd_gpio_set_value(pdata->bl_on, 0);
	tcclcd_gpio_set_value(pdata->reset, 0);
	tcclcd_gpio_set_value(pdata->power_on, 0);
	return 0;
}

static int lms350df01_set_power(struct lcd_panel *panel, int on)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d \n", __func__, on);
	if (on) {
		tcclcd_gpio_set_value(pdata->power_on, 1);
		tcclcd_gpio_set_value(pdata->reset, 1);
		lcd_on();
		lcdc_initialize(pdata->lcdc_num, panel);
		
		LCDC_IO_Set(pdata->lcdc_num, panel->bus_width);

	
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

static int lms350df01_set_backlight_level(struct lcd_panel *panel, int level)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d\n", __func__, level);
	
	if (level == 0) {
		tcclcd_gpio_set_value(pdata->bl_on, 0);
	} else {
		tcclcd_gpio_set_value(pdata->bl_on, 1);
	}

	return 0;
}

static struct lcd_panel lms350df01_panel = {
	.name		= "LMS350DF01",
	.manufacturer	= "Samsung",
	.id		= PANEL_ID_LMS350DF01,
	.xres		= 320,
	.yres		= 480,
	.width		= 103,
	.height		= 62,
	.bpp		= 32,
	.clk_freq	= 100000,
	.clk_div	= 2,
	.bus_width	= 24,
	.lpw		= 0,
	.lpc		= 320,
	.lswc		= 9,
	.lewc		= 6,
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 0,
	.flc1		= 480,
	.fswc1		= 4,
	.fewc1		= 102,
	.fpw2		= 0,
	.flc2		= 480,
	.fswc2		= 4,
	.fewc2		= 102,
	.sync_invert	= IV_INVERT | IH_INVERT,
	.init		= lms350df01_panel_init,
	.set_power	= lms350df01_set_power,
	.set_backlight_level = lms350df01_set_backlight_level,
};

struct lcd_panel *tccfb_get_panel(void)
{
	return &lms350df01_panel;
}
#endif//LMS350DF01
