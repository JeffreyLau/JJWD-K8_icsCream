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

#define IO_OFFSET	0x00000000
#define io_p2v(pa)	((pa) - IO_OFFSET)
#define tcc_p2v(pa)         io_p2v(pa)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)   (sizeof(x)/sizeof((x)[0]))
#endif

#ifndef BITCSET
#define BITCSET(X, CMASK, SMASK)	( (X) = ((((unsigned int)(X)) & ~((unsigned int)(CMASK))) | ((unsigned int)(SMASK))) )
#endif

struct gpio_cfg {
	unsigned port;
	unsigned config;
};

static struct gpio_cfg gpio_init_cfg[] = {
	{ TCC_GPC(31), GPIO_OUTPUT | GPIO_HIGH},
	{ TCC_GPA(0) , GPIO_FN1 }, /* SCL0 */
	{ TCC_GPA(1) , GPIO_FN1 }, /* SDA0 */

#if defined(AXP192_PMIC)
	{ TCC_GPF(24), GPIO_FN0 }, /* PMU_IRQ */
#if (HW_REV != 0x0005 && HW_REV != 0x0006)
	{ TCC_GPA(3) , GPIO_FN0 }, /* PMU_LVEN */
	{ TCC_GPF(14), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* PMU_GP5 */
	{ TCC_GPG(12), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* PMU_VBUSSEL */
	{ TCC_GPG(13), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* PMU_GP1 */
#endif

#else
	{ TCC_GPA(3) , GPIO_FN0 }, /* PWR_KEY */
	{ TCC_GPG(12), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* CHRG_EN */
#endif

	{ TCC_GPA(4) , GPIO_FN0 | GPIO_OUTPUT }, /* LCD BL */

#if (HW_REV != 0x0005 && HW_REV != 0x0006)
	{ TCC_GPA(5) , GPIO_FN0 | GPIO_PULLUP }, /* IR-SENSOR */

	{ TCC_GPA(7) , GPIO_FN1 }, /* SCL1 */
	{ TCC_GPA(8) , GPIO_FN1 }, /* SDA1 */

	{ TCC_GPA(14), GPIO_FN0 | GPIO_PULLDOWN }, /* BT_HWAKE */
#endif

#if (HW_REV == 0x0003 || HW_REV == 0x0004 || HW_REV == 0x0005 || HW_REV == 0x0006)
	{ TCC_GPA(15), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* MEM ZQ */
#endif

#if (HW_REV == 0x0003 || HW_REV == 0x0004 || HW_REV == 0x0005 || HW_REV == 0x0006)
	{ TCC_GPC(28), GPIO_FN0 },	/* RTC_INT */
#endif

#if (HW_REV == 0x0003 || HW_REV == 0x0004 || HW_REV == 0x0005 || HW_REV == 0x0006)
	{ TCC_GPE(0) , GPIO_FN1 },	/* UART_TXD0 */
	{ TCC_GPE(1) , GPIO_FN1 },	/* UART_RXD0 */
#else
	{ TCC_GPE(10), GPIO_FN1 },	/* UART_TXD0 */
	{ TCC_GPE(11), GPIO_FN1 },	/* UART_RXD0 */
#endif

	{ TCC_GPF(2), GPIO_FN0 },	/* KEY_CTL0 */
	{ TCC_GPF(3), GPIO_FN0 },	/* KEY_CTL1 */
	{ TCC_GPF(4), GPIO_FN0 },	/* KEY_CTL2 */
	{ TCC_GPF(5), GPIO_FN0 },	/* KEY_CTL3 */
	{ TCC_GPF(6), GPIO_FN0 },	/* KEY_CTL4 */

#if (HW_REV != 0x0005 && HW_REV != 0x0006)
	{ TCC_GPF(26), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH },	/* SHDN */
#endif 

	{ TCC_GPD(11), GPIO_FN0 | GPIO_OUTPUT },	/* MUTE_CTL */
	{ TCC_GPG(6) , GPIO_FN0 | GPIO_OUTPUT },	/* CODEC_SPCTL */

};

static void port_early_set_var(GPION *gpio, unsigned int bit, unsigned int config)
{
	unsigned int en=0, dat=0, pd=0, fn, cd;

	// function selection
	fn = ((config&GPIO_FN_BITMASK) >> GPIO_FN_SHIFT) - 1;
	if (bit >= 24)
		BITCSET(gpio->GPFN3, 0xF << ((bit-24)*4), fn << ((bit-24)*4));
	if (bit >= 16)
		BITCSET(gpio->GPFN2, 0xF << ((bit-16)*4), fn << ((bit-16)*4));
	if (bit >= 8)
		BITCSET(gpio->GPFN1, 0xF << ((bit-8)*4), fn << ((bit-8)*4));
	else
		BITCSET(gpio->GPFN0, 0xF << (bit*4), fn << (bit*4));

	// driver strenth
	cd = ((config&GPIO_CD_BITMASK) >> GPIO_CD_SHIFT) - 1;
	if (bit >= 16)
		BITCSET(gpio->GPCD1, 0x3 << ((bit-16)*2), cd << ((bit-16)*2));
	else
		BITCSET(gpio->GPCD0, 0x3 << (bit*2), cd << (bit*2));

	// pull-up/pull-down
	if (config & GPIO_PULLUP)
		pd |= 1;
	if (config & GPIO_PULLDOWN)
		pd |= 2;
	if (bit >= 16)
		BITCSET(gpio->GPPD1, 0x3 << ((bit-16)*2), pd << ((bit-16)*2));
	else
		BITCSET(gpio->GPPD0, 0x3 << (bit*2), pd << (bit*2));

	// direction
	if (config & GPIO_OUTPUT)
		en = 1;
	BITCSET(gpio->GPEN, 0x1 << bit, en << bit);

	// data
	if (config & GPIO_HIGH)
		dat = 1;
	BITCSET(gpio->GPDAT, 0x1 << bit, dat << bit);
}

static void port_early_set_reg(PGPION reg, PGPION gpio)
{
	reg->GPFN0 = gpio->GPFN0;
	reg->GPFN1 = gpio->GPFN1;
	reg->GPFN2 = gpio->GPFN2;
	reg->GPFN3 = gpio->GPFN3;
//	reg->GPCD0 = gpio->GPCD0;
//	reg->GPCD1 = gpio->GPCD1;
	reg->GPPD0 = gpio->GPPD0;
	reg->GPPD1 = gpio->GPPD1;
	reg->GPDAT = gpio->GPDAT;
	reg->GPEN  = gpio->GPEN;
	reg->GPDAT = gpio->GPDAT;
}

#define MAX_GPIO_REG_NUM	7
void gpio_init_early(void)
{
	unsigned int i;
	PGPIO	pGPIO = (PGPIO)tcc_p2v(HwGPIO_BASE);
	GPION	GPIO[MAX_GPIO_REG_NUM];

	for (i=0; i<MAX_GPIO_REG_NUM; i++) {
		GPIO[i].GPFN0 = 0x00000000;
		GPIO[i].GPFN1 = 0x00000000;
		GPIO[i].GPFN2 = 0x00000000;
		GPIO[i].GPFN3 = 0x00000000;
		GPIO[i].GPPD0 = 0x00000000;
		GPIO[i].GPPD1 = 0x00000000;
		GPIO[i].GPEN  = 0xFFFFFFFF;
		GPIO[i].GPDAT = 0x00000000;
	}

	for (i = 0; i < ARRAY_SIZE(gpio_init_cfg); i++) {
		switch (gpio_init_cfg[i].port & GPIO_REGMASK) {
			case GPIO_PORTA:
				port_early_set_var(&GPIO[0], gpio_init_cfg[i].port & GPIO_BITMASK, gpio_init_cfg[i].config);
				break;
			case GPIO_PORTB:
				port_early_set_var(&GPIO[1], gpio_init_cfg[i].port & GPIO_BITMASK, gpio_init_cfg[i].config);
				break;
			case GPIO_PORTC:
				port_early_set_var(&GPIO[2], gpio_init_cfg[i].port & GPIO_BITMASK, gpio_init_cfg[i].config);
				break;
			case GPIO_PORTD:
				port_early_set_var(&GPIO[3], gpio_init_cfg[i].port & GPIO_BITMASK, gpio_init_cfg[i].config);
				break;
			case GPIO_PORTE:
				port_early_set_var(&GPIO[4], gpio_init_cfg[i].port & GPIO_BITMASK, gpio_init_cfg[i].config);
				break;
			case GPIO_PORTF:
				port_early_set_var(&GPIO[5], gpio_init_cfg[i].port & GPIO_BITMASK, gpio_init_cfg[i].config);
				break;
			case GPIO_PORTG:
				port_early_set_var(&GPIO[6], gpio_init_cfg[i].port & GPIO_BITMASK, gpio_init_cfg[i].config);
				break;
		}
	}

	port_early_set_reg((PGPION)tcc_p2v(HwGPIOA_BASE), &GPIO[0]);
	port_early_set_reg((PGPION)tcc_p2v(HwGPIOB_BASE), &GPIO[1]);
	port_early_set_reg((PGPION)tcc_p2v(HwGPIOC_BASE), &GPIO[2]);
	port_early_set_reg((PGPION)tcc_p2v(HwGPIOD_BASE), &GPIO[3]);
	port_early_set_reg((PGPION)tcc_p2v(HwGPIOE_BASE), &GPIO[4]);
	port_early_set_reg((PGPION)tcc_p2v(HwGPIOF_BASE), &GPIO[5]);
	port_early_set_reg((PGPION)tcc_p2v(HwGPIOG_BASE), &GPIO[6]);
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
