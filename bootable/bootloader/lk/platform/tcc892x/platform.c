/*
 * Copyright (C) 2010 Telechips, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <debug.h>
#include <kernel/thread.h>
#include <platform/debug.h>
#include <dev/fbcon.h>
#include <dev/uart.h>
#include <dev/adc.h>
#include <platform/gpio.h>
#include <reg.h>
#include <platform/iomap.h>
#include <i2c.h>

#define IOBUSCFG_HCLKEN0	(TCC_IOBUSCFG_BASE + 0x10)
#define IOBUSCFG_HCLKEN1	(TCC_IOBUSCFG_BASE + 0x14)
#define PMU_WATCHDOG		(TCC_PMU_BASE + 0x04)
#define PMU_CONFIG1			(TCC_PMU_BASE + 0x10)
#define PMU_USSTATUS		(TCC_PMU_BASE + 0x18)

static struct fbcon_config *fb_config;

extern void clock_init_early(void);
extern void clock_init(void);

void platform_init_interrupts(void);
void platform_init_timer();
void pca953x_init(void);
void uart_init(void);

unsigned int system_rev;

void platform_early_init(void)
{
	system_rev = HW_REV;

	/* initialize clocks */
	clock_init_early();

	/* initialize GPIOs */
	gpio_init_early();

	/* initialize the interrupt controller */
	platform_init_interrupts();

	/* initialize the timer block */
	platform_init_timer();

	/* initialize the uart */
	uart_init_early();

#if defined(TCC_I2C_USE)
	i2c_init_early();
#endif

	adc_init_early();
}

void platform_init(void)
{
#if defined(TARGET_M805_892X_EVM)
	int ac_charger_detect_flag, usb_detect_flag;
	volatile int i;
	i = 0;
#endif

#if defined(TCC_I2C_USE)
	i2c_init();
#endif

	clock_init();

#if defined(TCC_PCA953X_USE)
	pca953x_init();
#endif

	uart_init();

	adc_init();

#if defined(DISPLAY_SPLASH_SCREEN) || defined(DISPLAY_SPLASH_SCREEN_DIRECT)
	#ifndef DEFAULT_DISPLAY_OUTPUT_DUAL
		display_init();
		dprintf(INFO, "Display initialized\n");
		//disp_init = 1;
	#endif

#endif

#if defined(TARGET_M805_892X_EVM)
	ac_charger_detect_flag = ac_charger_detect();
	usb_detect_flag = usb_detect();
	
	dprintf(INFO, "USB DETECT(%d) AC CHARGER DETECT(%d)\n", usb_detect_flag, ac_charger_detect_flag);

    if( check_fwdn_mode() )
        return;
    
	if(! (ac_charger_detect_flag || usb_detect_flag) ) {
		if( check_low_battery()) {
			while(1) {
				dprintf(CRITICAL, "Low Battery!!!! Power Off\n");
				display_lowbattery_logo();				
				for ( i = 0 ; i < 1000000 ; i ++ );
				for ( i = 0 ; i < 1000000 ; i ++ );
				while(1) powercon_disable();
			}
		}
	} else {
        if( check_low_battery()) {
            display_lowbattery_logo();
        
        
		while( check_low_battery()) {
        	ac_charger_detect_flag = ac_charger_detect();
        	usb_detect_flag = usb_detect();

            for ( i = 0 ; i < 1000000 ; i ++ );
            
            if(! (ac_charger_detect_flag || usb_detect_flag) ) {
				dprintf(CRITICAL, "Low Battery!!!! Power Off\n");
				while(1) powercon_disable();
            }
	    }
        }
		
    }
#endif
    display_booting_logo();

	dprintf(INFO, "platform_init()\n");
}

void display_init(void)
{
#if defined(TCC_LCD_USE)
	fb_config = lcdc_init();
	ASSERT(fb_config);
	fbcon_setup(fb_config);	
#endif
}

void reboot(unsigned reboot_reason)
{
	unsigned int usts;

	if (reboot_reason == 0x77665500) {
		usts = 1;
	} else {
		usts = 0;
	}

	writel(usts, PMU_USSTATUS);

	writel((readl(PMU_CONFIG1) & 0xCFFFFFFF), PMU_CONFIG1);

	writel(0xFFFFFFFF, IOBUSCFG_HCLKEN0);
	writel(0xFFFFFFFF, IOBUSCFG_HCLKEN1);

	while (1) {
		writel((1 << 31) | 1, PMU_WATCHDOG);
	}
}

unsigned check_reboot_mode(void)
{
	unsigned int usts;
	unsigned int mode;

	/* XXX: convert reboot mode value because USTS register
	 * hold only 8-bit value
	 */
	usts = readl(PMU_USSTATUS);
	switch (usts) {
	case 1:
		mode = 0x77665500;	/* fastboot mode */
		break;

	case 2:
		mode = 0x77665502;	/* recovery mode */
		break;

	default:
		mode = 0x77665501;
		break;
	}

	dprintf(SPEW, "reboot mode = 0x%x\n", mode);
	return mode;
}

unsigned is_reboot_mode(void)
{
	return check_reboot_mode();
}
