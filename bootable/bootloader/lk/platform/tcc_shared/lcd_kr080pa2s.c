/*
 * ek08009_lcd.c -- support for LG PHILIPS LB080WV6 LCD
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

#ifdef KR080PA2S
#include <dev/gpio.h>

static int kr080pa2s_panel_init(struct lcd_panel *panel)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d  \n", __func__, 0);

	tcclcd_gpio_config(pdata->display_on, GPIO_OUTPUT);
	tcclcd_gpio_config(pdata->bl_on, GPIO_OUTPUT);
	tcclcd_gpio_config(pdata->reset, GPIO_OUTPUT);
	tcclcd_gpio_config(pdata->power_on, GPIO_OUTPUT);

	tcclcd_gpio_set_value(pdata->display_on, 0);
	tcclcd_gpio_set_value(pdata->bl_on, 0);
	tcclcd_gpio_set_value(pdata->reset, 0);
	tcclcd_gpio_set_value(pdata->power_on, 0);

	return 0;
}


static int kr080pa2s_set_power(struct lcd_panel *panel, int on)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d ~ \n", __func__, on);

	if (on) {
		tcclcd_gpio_set_value(pdata->reset, 1);
#if defined(_M801_88_)
		lcd_delay_us(1000*10);
#else
		lcd_delay_us(1000);
#endif
		tcclcd_gpio_set_value(pdata->power_on, 1);
		lcd_delay_us(1000);
		tcclcd_gpio_set_value(pdata->reset, 0);
		lcd_delay_us(1000);
		tcclcd_gpio_set_value(pdata->reset, 1);
		
		tcclcd_gpio_set_value(pdata->display_on, 1);
		mdelay(10);

		lcdc_initialize(pdata->lcdc_num, panel);

		LCDC_IO_Set(pdata->lcdc_num, panel->bus_width);

		LCDC_IO_SetCurrent(pdata->lcdc_num, panel->bus_width);

		mdelay(16);
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


static int kr080pa2s_set_backlight_level(struct lcd_panel *panel, int level)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d\n", __func__, level);

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



static struct lcd_panel kr080pa2s_panel = {
	.name		= "kr080pa2s",
	.manufacturer	= "xingyuan",
	.id			= PANEL_ID_KR080PA2S,
	.xres		= 1024,
	.yres		= 768,
	.width		= 103,
	.height		= 62,
	.bpp		= 24,
	.clk_freq	= 650000,
	.clk_div	= 1,
	.bus_width	= 24,
	.lpw		= 5,
	.lpc		= 1024,
	.lswc		= 160,
	.lewc		= 160,
	.vdb		= 0,
	.vdf		= 0,
	.fpw1		= 2,
	.flc1		= 768,
	.fswc1		= 20,
	.fewc1		= 15,
	.fpw2		= 2,
	.flc2		= 768,
	.fswc2		= 20,
	.fewc2		= 15,
	.sync_invert	= IV_INVERT| IH_INVERT,
	.init		= kr080pa2s_panel_init,
	.set_power	= kr080pa2s_set_power,
	.set_backlight_level = kr080pa2s_set_backlight_level,
};

struct lcd_panel *tccfb_get_panel(void)
{
	return &kr080pa2s_panel;
}

#endif

