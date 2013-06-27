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

struct gpio_cfg gpio_init_cfg[] = {
	{ TCC_GPA(0) , GPIO_FN1 }, /* I2C0 (SCL0) */
	{ TCC_GPA(1) , GPIO_FN1 }, /* I2C0 (SDA0) */

	{ TCC_GPA(3) , GPIO_FN0 | GPIO_INPUT }, /* KEY1 */
	{ TCC_GPA(4) , GPIO_FN0 | GPIO_INPUT }, /* KEY2 */

	{ TCC_GPA(7) , GPIO_FN1 }, /* I2C1 (SCL1) */
	{ TCC_GPA(8) , GPIO_FN1 }, /* I2C1 (SDA1) */

	{ TCC_GPB(28), GPIO_FN0 | GPIO_INPUT | GPIO_PULLUP }, /* TEST_PIN */

	{ TCC_GPD(0) , GPIO_CD0 | GPIO_FN0 }, /* I2S strength: zero */
	{ TCC_GPD(1) , GPIO_CD0 | GPIO_FN0 },
	{ TCC_GPD(2) , GPIO_CD0 | GPIO_FN0 },
	{ TCC_GPD(3) , GPIO_CD0 | GPIO_FN0 },
	{ TCC_GPD(4) , GPIO_CD0 | GPIO_FN0 },

	{ TCC_GPD(21), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* SLEEP_MODE_CTL */

	{ TCC_GPE(0) , GPIO_FN1 }, /* UART_TXD0 */
	{ TCC_GPE(1) , GPIO_FN1 }, /* UART_RXD0 */

	{ TCC_GPF(26), GPIO_FN0 | GPIO_INPUT }, /* VE_DET */
	{ TCC_GPF(27), GPIO_FN0 | GPIO_INPUT }, /* CVBS_DET */
#if defined(BOOTSD_INCLUDE)
	{ TCC_GPB(0), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH },	/* SDWF_RST# */
	{ TCC_GPB(1), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* SD_ON */
	{ TCC_GPB(2), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* SLEEP */
	{ TCC_GPB(3), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* LOCK */
	{ TCC_GPB(4), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* HDD_ON */
	{ TCC_GPB(5), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* SATA_ON(Main TP) */

	{ TCC_GPB(6), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH  }, /* CORE1_ON */
	{ TCC_GPB(7), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* CORE2_ON */
#else
	{ TCC_GPE(21), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH  }, /* CORE1_ON */
	{ TCC_GPE(22), GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* CORE2_ON */
#endif /* BOOTSD_INCLUDE */

	{ TCC_GPG(5) , GPIO_FN0 | GPIO_OUTPUT | GPIO_HIGH }, /* MEM ZQ */
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
