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

#include <dev/gpio.h>
#include <platform/gpio.h>
#include <platform/reg_physical.h>


//kch temp
#define IO_OFFSET	0x00000000
#define io_p2v(pa)	((pa) - IO_OFFSET)
#define tcc_p2v(pa)         io_p2v(pa)
//

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)   (sizeof(x)/sizeof((x)[0]))
#endif

#ifndef BITCLR
#define BITCLR(X, MASK) 			( (X) &= ~((unsigned int)(MASK)) )
#endif

#ifndef BITCSET
#define BITCSET(X, CMASK, SMASK)	( (X) = ((((unsigned int)(X)) & ~((unsigned int)(CMASK))) | ((unsigned int)(SMASK))) )
#endif


struct gpio_cfg {
	unsigned port;
	unsigned config;
};

struct gpio_cfg gpio_init_cfg[] = {
	{ TCC_GPC(31), GPIO_OUTPUT | GPIO_HIGH},
	{ TCC_GPA(0), GPIO_FN1 }, /* SCL0 */
	{ TCC_GPA(1), GPIO_FN1 }, /* SDA0 */
	{ TCC_GPA(3), GPIO_FN0 }, /* PWR_KEY */

	{ TCC_GPA(4), GPIO_FN0 | GPIO_OUTPUT }, /* LCD BL */

	{ TCC_GPA(7), GPIO_FN1 }, /* SCL1 */
	{ TCC_GPA(8), GPIO_FN1 }, /* SDA1 */

	//	{ TCC_GPE(1),  GPIO_FN0 | GPIO_OUTPUT }, /* LCD DISP */

	{ TCC_GPF(26), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH },	/* SHDN */
};

void port_HW_init(void)
{
#if defined(AXP192_PMIC)
		// GPIO_A
		*((volatile unsigned long *)0xF0308024) = 0x00000011;		//GPIOA Config - [0],[1] is i2c oherwise gpio
		*((volatile unsigned long *)0xF0308028) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF030802C) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF0308030) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF030801C) = 0x20000000;		//GPIOA [14] Pull-down, otherwise Pull-up/down disable
		*((volatile unsigned long *)0xF0308020) = 0x00000000;		//GPIOA Pull-up/down disable
		*((volatile unsigned long *)0xF0308004) = 0xFFFFBFFF;		//GPIOA [14] input, otherwise output
		if(HW_REV==0x0001 || HW_REV==0x0002 || HW_REV==0x0012 || HW_REV==0x0013) // for only m803 temp
		    *((volatile unsigned long *)0xF0308000) = Hw5|Hw7|Hw8|Hw15;	//GPIOA [5],[7],[8] High, otherwise  Low
		else
		    *((volatile unsigned long *)0xF0308000) = Hw5|Hw7|Hw8;	//GPIOA [5],[7],[8] High, otherwise  Low
#else
		*((volatile unsigned long *)0xF0308024) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF0308028) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF030802C) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF0308030) = 0x00000000;		//GPIOA Config - gpio
		*((volatile unsigned long *)0xF030801C) = 0x20000000;		//GPIOA [14] Pull-down, otherwise Pull-up/down disable
		*((volatile unsigned long *)0xF0308020) = 0x00000000;		//GPIOA Pull-up/down disable
		*((volatile unsigned long *)0xF0308004) = 0xFFFFBFF7;		//GPIOA [3],[14] input, otherwise output
		*((volatile unsigned long *)0xF0308000) = Hw0|Hw1|Hw5|Hw7|Hw8;		//GPIOA [0],[1],[5],[7],[8] High, otherwise  Low
#endif

		// GPIO_B
		*((volatile unsigned long *)0xF0308064) = 0x00000000;	//GPIOB config - gpio
		*((volatile unsigned long *)0xF0308068) = 0x00000000;	//GPIOB config - gpio
		*((volatile unsigned long *)0xF030806C) = 0x10111010;	//GPIOB [16],[18],[22] Config - gpio, otherwise nand
		*((volatile unsigned long *)0xF0308070) = 0x01100000;	//GPIOB [24],[25],[26],[27],[28],[31] Config - gpio, otherwise nand	
		*((volatile unsigned long *)0xF030805C) = 0x00000000;	//GPIOB Pull-up/down disable
		*((volatile unsigned long *)0xF0308060) = 0x80000000;	//GPIOB [31] Pull-down, otherwise Pull-up/down disable
		//*((volatile unsigned long *)0xF030805C) = 0x55555555;	//GPIOB[0]~[15] Pull-up
		//*((volatile unsigned long *)0xF0308060) = 0x80050000;	//GPIOB [31] Pull-down, [24~25] Pull-up, otherwise Pull-up/down disable
		*((volatile unsigned long *)0xF0308044) &= ~(Hw31|Hw28);			//GPIOB Input
		//*((volatile unsigned long *)0xF0308044) |= (0x0000ffff|Hw27|Hw26|Hw25|Hw24|Hw22|Hw18|Hw16);	//GPIOB Output
		*((volatile unsigned long *)0xF0308044) |= (Hw27|Hw26|Hw22|Hw18|Hw16);	//GPIOB Output
		*((volatile unsigned long *)0xF0308040) &= ~(Hw27|Hw26|Hw22|Hw18|Hw16);	//GPIOB Low
		//*((volatile unsigned long *)0xF0308040) |= (0x0000ffff|Hw25|Hw24);	//GPIOB High

		// GPIO_C
		*((volatile unsigned long *)0xF03080A4) = 0x00000000;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF03080A8) = 0x00000000;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF03080AC) = 0x00000000;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF03080B0) = 0x00000000;		//GPIOC Config - gpio
		*((volatile unsigned long *)0xF030809C) = 0x00000000;		//GPIOC Pull-up/down disable
		*((volatile unsigned long *)0xF03080A0) = 0x00000000;		//GPIOC Pull-up/down disable
		*((volatile unsigned long *)0xF0308084) = 0xFFFFFFFF;		//GPIOC output
		*((volatile unsigned long *)0xF0308080) = 0x00000000;		//GPIOC Low	

		// GPIO_D
		*((volatile unsigned long *)0xF03080E4) = 0x00000000;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080E8) = 0x00000000;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080EC) = 0x00000000;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080F0) = 0x00000000;		//GPIOD Config - gpio
		*((volatile unsigned long *)0xF03080DC) = 0x00000000;		//GPIOD Pull-up/down disable
		*((volatile unsigned long *)0xF03080E0) = 0x00000000;		//GPIOD Pull-up/down disable
		*((volatile unsigned long *)0xF03080C4) = 0xFFFFFFFF;		//GPIOD output
		*((volatile unsigned long *)0xF03080C0) = Hw22|Hw23|Hw24;		//GPIOD [22],[23],[24] High, otherwise  Low

		// GPIO_E
		*((volatile unsigned long *)0xF0308124) = 0x00000000;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF0308128) = 0x00000000;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF030812C) = 0x00000000;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF0308130) = 0x00000000;		//GPIOE Config - gpio
		*((volatile unsigned long *)0xF030811C) = 0x00000000;		//GPIOE Pull-up/down disable
		*((volatile unsigned long *)0xF0308120) = 0x00000000;		//GPIOE Pull-up/down disable
		*((volatile unsigned long *)0xF0308104) = 0xFFFFFFFF;		//GPIOE output
		*((volatile unsigned long *)0xF0308100) = Hw10|Hw11;		//GPIOE [10],[11]High, otherwise  Low

		// GPIO_F
		*((volatile unsigned long *)0xF0308164) = 0x00000000;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF0308168) = 0x00000000;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF030816C) = 0x00000000;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF0308170) = 0x00000000;		//GPIOF Config - gpio
		*((volatile unsigned long *)0xF030815C) = 0x00000000;		//GPIOF Pull-up/down disable
		*((volatile unsigned long *)0xF0308160) = 0x00000000;		//GPIOF Pull-up/down disable	
#if defined(AXP192_PMIC)
		*((volatile unsigned long *)0xF0308144) = 0xFEFDFF83;		//GPIOF [2~6],[17],[24] input, otherwise output		
		*((volatile unsigned long *)0xF0308140) = 0x047E4000;		//GPIOF [14],[18],[19],[20],[21],[22],[26] High, otherwise  Low
#else
		*((volatile unsigned long *)0xF0308144) = 0xFCFDFF83;		//GPIOF [2~6],[17],[24],[25] input, otherwise output		
		*((volatile unsigned long *)0xF0308140) = 0x047E0000;		//GPIOF [18],[19],[20],[21],[22],[26] High, otherwise  Low
#endif

		// GPIO_G
		*((volatile unsigned long *)0xF03081E4) = 0x00000000;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081E8) = 0x00000000;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081EC) = 0x00000000;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081F0) = 0x00000000;		//GPIOG Config - gpio
		*((volatile unsigned long *)0xF03081DC) = 0x00000000;		//GPIOG Pull-up/down disable
		*((volatile unsigned long *)0xF03081E0) = 0x00000000;		//GPIOG Pull-up/down disable
		*((volatile unsigned long *)0xF03081C4) = 0xFFFFFFFF;		//GPIOG output
#if defined(AXP192_PMIC)
		*((volatile unsigned long *)0xF03081C0) = 0x00003000;		//GPIOG [12],[13] HIGH, otherwise Low
#else
		*((volatile unsigned long *)0xF03081C0) = 0x00001000;		//GPIOG [12] HIGH, otherwise Low
#endif

}

void gpio_init_early(void)
{
	unsigned int i;
	PGPIO	pGPIO = (PGPIO)tcc_p2v(HwGPIO_BASE);

	port_HW_init();

#if 0	
	// GPIOA Setting
	pGPIO->GPAEN = 0xFFAF; // All Output
	pGPIO->GPADAT = Hw2|Hw3; // Low
	//pGPIO->GPACD0
	//pGPIO->GPACD1
	//pGPIO->GPAPD0
	//pGPIO->GPAPD1
	pGPIO->GPAFN0 = HwZERO; // GPIO
	pGPIO->GPAFN1 = HwZERO; // GPIO
	pGPIO->GPAFN2 = HwZERO; // GPIO
	pGPIO->GPAFN3 = HwZERO; // GPIO

	// GPIOB Setting
	//pGPIO->GPBEN = 0xFFFFFFFF; // All Output
	pGPIO->GPBEN = 0xFFFF8FF0; // All Output
	pGPIO->GPBDAT = HwZERO; // Low
	pGPIO->GPBCD0 = 0x0;
	pGPIO->GPBCD1 = 0x0;
	pGPIO->GPBPD0 = 0x0;
	pGPIO->GPBPD1 = 0x0;
	pGPIO->GPBFN0 = HwZERO; // GPIO
	pGPIO->GPBFN1 = HwZERO; // GPIO
	pGPIO->GPBFN2 = HwZERO; // GPIO
	pGPIO->GPBFN3 = HwZERO; // GPIO


	// GPIOC Setting
	pGPIO->GPCEN = 0xFFFFFFFF; // All Output
	pGPIO->GPCDAT = HwZERO;// Low
	//pGPIO->GPCCD0
	//pGPIO->GPCCD1
	//pGPIO->GPCPD0
	//pGPIO->GPCPD1
	pGPIO->GPCFN0 = HwZERO; // GPIO
	pGPIO->GPCFN1 = HwZERO; // GPIO
	pGPIO->GPCFN2 = HwZERO; // GPIO
	pGPIO->GPCFN3 = HwZERO; // GPIO


	// GPIOD Setting
	pGPIO->GPDEN = 0xFFFFFFFF; // All Output
	//pGPIO->GPDDAT = HwZERO; // Low
	pGPIO->GPDDAT = Hw14; // 
	//pGPIO->GPDCD0
	//pGPIO->GPDCD1
	//pGPIO->GPDPD0 = 0xAAAAAAAA;
	//pGPIO->GPDPD1 = 0x000AAAAA;
	pGPIO->GPDFN0 = HwZERO; // GPIO
	pGPIO->GPDFN1 = HwZERO; // GPIO
	pGPIO->GPDFN2 = HwZERO; // GPIO
	pGPIO->GPDFN3 = HwZERO; // GPIO


	// GPIOE Setting
	//pGPIO->GPEEN = 0xFFFFF00F; // All Output - BT,DXB Input 
	pGPIO->GPEEN = 0xFFC0000F; // All Output - BT,DXB Input 
	pGPIO->GPEDAT = HwZERO; // Low
	//pGPIO->GPECD0
	//pGPIO->GPECD1
	//pGPIO->GPEPD0 = 0xAAAAAAAA;
	//pGPIO->GPEPD1 = 0xAAAAAAAA;
	pGPIO->GPEFN0 = HwZERO; // GPIO
	pGPIO->GPEFN1 = HwZERO; // GPIO
	pGPIO->GPEFN2 = HwZERO; // GPIO
	pGPIO->GPEFN3 = HwZERO; // GPIO

	// GPIOF Setting
	pGPIO->GPFEN = 0xFFFFFFFF; // All Output
	pGPIO->GPFDAT = HwZERO; // Low
	//pGPIO->GPFCD0
	//pGPIO->GPFCD1
	//pGPIO->GPFPD0 // No Pullup/down register
	//pGPIO->GPFPD1 // No Pullup/down register
	pGPIO->GPFFN0 = HwZERO; // GPIO
	pGPIO->GPFFN1 = HwZERO; // GPIO
	pGPIO->GPFFN2 = HwZERO; // GPIO
	pGPIO->GPFFN3 = HwZERO; // GPIO


	// GPIOG Setting
	pGPIO->GPGEN = 0xFFFFFFFF; // All Output
	pGPIO->GPGDAT = HwZERO; // Low
	//pGPIO->GPFCD0
	//pGPIO->GPFCD1
	//pGPIO->GPFPD0 // No Pullup/down register
	//pGPIO->GPFPD1 // No Pullup/down register
	pGPIO->GPGFN0 = HwZERO; // GPIO
	pGPIO->GPGFN1 = HwZERO; // GPIO
	pGPIO->GPGFN2 = HwZERO; // GPIO
	pGPIO->GPGFN3 = HwZERO; // GPIO

	pGPIO->GPAPD0 = 0;
	pGPIO->GPAPD1 = 0;
	//pGPIO->GPBPD0 = 0;
	//pGPIO->GPBPD1 = 0;
	pGPIO->GPCPD0 = 0;
	pGPIO->GPCPD1 = 0;
	pGPIO->GPDPD0 = 0;
	pGPIO->GPDPD1 = 0;
	pGPIO->GPEPD0 = 0;
	pGPIO->GPEPD1 = 0;
	pGPIO->GPFPD0 = 0;
	pGPIO->GPFPD1 = 0;
	pGPIO->GPGPD0 = 0;
	pGPIO->GPGPD1 = 0;

	// EINTSEL
	pGPIO->EINTSEL0 =HwZERO;
	pGPIO->EINTSEL1 =HwZERO;
	pGPIO->EINTSEL2 =HwZERO;
#endif

#if 1 // For M801_88
	/* PWR_SHDN */
	BITCSET(pGPIO->GPFEN,  Hw26, Hw26);
	BITCSET(pGPIO->GPFDAT, Hw26, Hw26);

	/* MUTE_CTL*/
	BITCSET(pGPIO->GPDEN,  Hw11, Hw11);
	BITCSET(pGPIO->GPDDAT, Hw11,    0);

	/* CODEC_SPCTL */
	BITCSET(pGPIO->GPGEN,   Hw6,  Hw6);
	BITCSET(pGPIO->GPGDAT,  Hw6,    0);

	/* WIFI_RST# */
	BITCSET(pGPIO->GPGEN,  Hw11, Hw11);
	BITCSET(pGPIO->GPGDAT, Hw11, Hw11);

	/* WIFI_PWREN */
	BITCSET(pGPIO->GPGEN,  Hw10, Hw10);
	BITCSET(pGPIO->GPGDAT, Hw10, Hw10);

	/* WIFI_RST# */
	BITCLR(pGPIO->GPGDAT,  Hw11);

	/* VCORE_CTL0 */
	BITCSET(pGPIO->GPFEN,   Hw0,  Hw0);
	BITCSET(pGPIO->GPFDAT,  Hw0,  Hw0);

	/* VCORE_CTL1 */
	BITCSET(pGPIO->GPFEN,   Hw1,  Hw1);
	BITCSET(pGPIO->GPFDAT,  Hw1,    0);

	/* HDMI_PWREN */
	BITCSET(pGPIO->GPGEN,   Hw3,  Hw3);		// move hdmi.c file on kernel source
	BITCSET(pGPIO->GPGDAT,  Hw3,    0);

#if 0
	// CHARGER
	BITCSET(pGPIO->GPGEN, Hw12|Hw13, Hw12|Hw13);	// CHRG_CTL, CHRG_EN
	BITCSET(pGPIO->GPFEN, Hw25, 0);					// CHRG#
	BITCSET(pGPIO->GPFEN, Hw24, 0);					// CHRG_STBY#
	BITCSET(pGPIO->GPGDAT, Hw12|Hw13, 0);			// CHRG_CTL, CHRG_EN
#endif

	// WIFI_RST# - HIGH
	BITCSET(pGPIO->GPGDAT, Hw11, Hw11);

	//BITCLR(HwGPIOF->GPEN, Hw14|Hw15|Hw16|Hw17);
#endif

	for (i = 0; i < ARRAY_SIZE(gpio_init_cfg); i++) {
		gpio_config(gpio_init_cfg[i].port, gpio_init_cfg[i].config);

	    if(HW_REV==0x0001 || HW_REV==0x0002 || HW_REV==0x0012 || HW_REV==0x0013){ // for only m803 uart 0
                gpio_config( TCC_GPE(0), GPIO_FN1 ); /* UART_TXD0 */
                gpio_config( TCC_GPE(1), GPIO_FN1 ); /* UART_RXD0 */
	    }else{
                gpio_config( TCC_GPE(10), GPIO_FN1 ); /* UART_TXD0 */
                gpio_config( TCC_GPE(11), GPIO_FN1 ); /* UART_RXD0 */
	    }
	}
}
void gpio_powercon_disable(void)
{
	//PGPIO	pGPIO = (GPIO *)&HwGPIO_BASE;

	//BITCLR(pGPIO->GPFDAT, Hw26);
	//TODO:
}

int gpio_is_ac_charger_detect(void) 
{
	//TODO:
	return 0;
}

#define INT_EI0				3

int gpio_is_usb_detect(void)
{
	//BITCSET(HwGPIO->EINTSEL0, (Hw6-Hw0), 62);

	//if( HwPIC->STS0 & (1<<INT_EI0) )
	//	return 1;
	//TODO:
	return 0;
}
