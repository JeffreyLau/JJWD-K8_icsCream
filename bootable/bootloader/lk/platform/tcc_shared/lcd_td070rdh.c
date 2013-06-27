/*
 * tm070rdh11_lcd.c -- support for TPO TD070RDH11 LCD
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

#ifdef TD070RDH11
#include <dev/gpio.h>


static int tm070rdh11_panel_init(struct lcd_panel *panel)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d  \n", __func__, 0);

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

static int tm070rdh11_set_power(struct lcd_panel *panel, int on)
{
	struct lcd_platform_data *pdata = &(panel->dev);

	printf("%s : %d ~ \n", __func__, on);

	if (on) {
		tcclcd_gpio_set_value(pdata->reset, 1);
		lcd_delay_us(1000);

		tcclcd_gpio_set_value(pdata->power_on, 1);
		lcd_delay_us(1000);
		tcclcd_gpio_set_value(pdata->reset, 0);
		lcd_delay_us(1000);
		tcclcd_gpio_set_value(pdata->reset, 1);

		mdelay(10);

		lcdc_initialize(pdata->lcdc_num, panel);

		LCDC_IO_Set(pdata->lcdc_num, panel->bus_width);

		// lcd port current 
		BITCSET(HwGPIOC->GPCD0,0xFFFFFFFF, 0xAAAAAAAA);		
		BITCSET(HwGPIOC->GPCD1,0x00FFFFFF, 0x00FFAAAA);

		BITCSET(HwGPIOC->GPFN3, HwPORTCFG_GPFN0_MASK , HwPORTCFG_GPFN0(0));
		BITCSET(HwGPIOC->GPEN, Hw24 ,Hw24);
		BITCSET(HwGPIOC->GPDAT, Hw24 ,0);

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


static int tm070rdh11_set_backlight_level(struct lcd_panel *panel, int level)
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


static struct lcd_panel tm070rdh11_panel = {
	.name		= "TD070RDH11",
	.manufacturer	= "OPTOELECTRONICS",
	.id		= PANEL_ID_TD070RDH11,
	.xres		= 800,
	.yres		= 480,
	.width		= 154,
	.height		= 85,
	.bpp		= 24,
	.clk_freq	= 300000,
	.clk_div	= 2,
	.bus_width	= 24,

	.lpw		= 47,
	.lpc		= 800,
	.lswc		= 39,
	.lewc		= 39,

	.vdb		= 0,
	.vdf		= 0,
	
	.fpw1		= 2,
	.flc1		= 480,
	.fswc1		= 28,
	.fewc1		= 12,
	
	.fpw2		= 2,
	.flc2		= 480,
	.fswc2		= 28,
	.fewc2		= 12,
//юс╫ц	.sync_invert	= IV_INVERT | IH_INVERT | IP_INVERT,
	.sync_invert	= IV_INVERT | IH_INVERT,

	.init		= tm070rdh11_panel_init,
	.set_power	= tm070rdh11_set_power,
	.set_backlight_level = tm070rdh11_set_backlight_level,
};

struct lcd_panel *tccfb_get_panel(void)
{
	return &tm070rdh11_panel;
}
#endif//TD070RDH11
