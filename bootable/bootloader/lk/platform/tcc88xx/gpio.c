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
#include <reg.h>
#include <dev/gpio.h>
#include <platform/iomap.h>
#include <platform/gpio.h>

typedef struct gpioregs gpioregs;

#define GPIO_REG(off)	(TCC_GPIO_BASE + (off))

struct gpioregs
{
	unsigned data;		/* data */
	unsigned out_en;	/* output enable */
	unsigned out_or;	/* OR fnction on output data */
	unsigned out_bic;	/* BIC function on output data */
	unsigned out_xor;	/* XOR function on output data */
	unsigned strength0;	/* driver strength control 0 */
	unsigned strength1;	/* driver strength control 1 */
	unsigned pe;		/* pull-up/down enable */
	unsigned ps;		/* pull-up/down function */
	unsigned fn0;		/* port configuration */
	unsigned fn1;		/* port configuration */
	unsigned fn2;		/* port configuration */
	unsigned fn3;		/* port configuration */
	unsigned in_en;		/* input enable */
	unsigned in_type;	/* input type (Shmitt / CMOS) */
	unsigned slew_rate;	/* slew rate */
};

static gpioregs GPIO_REGS[] = {
	/* GPIO A */
	{
		.data =		GPIO_REG(0x000),
		.out_en =	GPIO_REG(0x004),
		.out_or =	GPIO_REG(0x008),
		.out_bic =	GPIO_REG(0x00C),
		.out_xor =	GPIO_REG(0x010),
		.strength0 =	GPIO_REG(0x014),
		.strength1 =	GPIO_REG(0x018),
		.pe =		GPIO_REG(0x01C),
		.ps =		GPIO_REG(0x020),
		.fn0 =		GPIO_REG(0x024),
		.fn1 =		GPIO_REG(0x028),
		.fn2 =		GPIO_REG(0x02C),
		.fn3 =		GPIO_REG(0x030),
		.in_en =	GPIO_REG(0x034),
		.in_type =	GPIO_REG(0x038),
		.slew_rate =	GPIO_REG(0x03C),
	},
	/* GPIO B */
	{
		.data =		GPIO_REG(0x040),
		.out_en =	GPIO_REG(0x044),
		.out_or =	GPIO_REG(0x048),
		.out_bic =	GPIO_REG(0x04C),
		.out_xor =	GPIO_REG(0x050),
		.strength0 =	GPIO_REG(0x054),
		.strength1 =	GPIO_REG(0x058),
		.pe =		GPIO_REG(0x05C),
		.ps =		GPIO_REG(0x060),
		.fn0 =		GPIO_REG(0x064),
		.fn1 =		GPIO_REG(0x068),
		.fn2 =		GPIO_REG(0x06C),
		.fn3 =		GPIO_REG(0x070),
		.in_en =	GPIO_REG(0x074),
		.in_type =	GPIO_REG(0x078),
		.slew_rate =	GPIO_REG(0x07C),
	},
	/* GPIO C */
	{
		.data =		GPIO_REG(0x080),
		.out_en =	GPIO_REG(0x084),
		.out_or =	GPIO_REG(0x088),
		.out_bic =	GPIO_REG(0x08C),
		.out_xor =	GPIO_REG(0x090),
		.strength0 =	GPIO_REG(0x094),
		.strength1 =	GPIO_REG(0x098),
		.pe =		GPIO_REG(0x09C),
		.ps =		GPIO_REG(0x0A0),
		.fn0 =		GPIO_REG(0x0A4),
		.fn1 =		GPIO_REG(0x0A8),
		.fn2 =		GPIO_REG(0x0AC),
		.fn3 =		GPIO_REG(0x0B0),
		.in_en =	GPIO_REG(0x0B4),
		.in_type =	GPIO_REG(0x0B8),
		.slew_rate =	GPIO_REG(0x0BC),
	},
	/* GPIO D */
	{
		.data =		GPIO_REG(0x0C0),
		.out_en =	GPIO_REG(0x0C4),
		.out_or =	GPIO_REG(0x0C8),
		.out_bic =	GPIO_REG(0x0CC),
		.out_xor =	GPIO_REG(0x0D0),
		.strength0 =	GPIO_REG(0x0D4),
		.strength1 =	GPIO_REG(0x0D8),
		.pe =		GPIO_REG(0x0DC),
		.ps =		GPIO_REG(0x0E0),
		.fn0 =		GPIO_REG(0x0E4),
		.fn1 =		GPIO_REG(0x0E8),
		.fn2 =		GPIO_REG(0x0EC),
		.fn3 =		GPIO_REG(0x0F0),
		.in_en =	GPIO_REG(0x0F4),
		.in_type =	GPIO_REG(0x0F8),
		.slew_rate =	GPIO_REG(0x0FC),
	},
	/* GPIO E */
	{
		.data =		GPIO_REG(0x100),
		.out_en =	GPIO_REG(0x104),
		.out_or =	GPIO_REG(0x108),
		.out_bic =	GPIO_REG(0x10C),
		.out_xor =	GPIO_REG(0x110),
		.strength0 =	GPIO_REG(0x114),
		.strength1 =	GPIO_REG(0x118),
		.pe =		GPIO_REG(0x11C),
		.ps =		GPIO_REG(0x120),
		.fn0 =		GPIO_REG(0x124),
		.fn1 =		GPIO_REG(0x128),
		.fn2 =		GPIO_REG(0x12C),
		.fn3 =		GPIO_REG(0x130),
		.in_en =	GPIO_REG(0x134),
		.in_type =	GPIO_REG(0x138),
		.slew_rate =	GPIO_REG(0x13C),
	},
	/* GPIO F */
	{
		.data =		GPIO_REG(0x140),
		.out_en =	GPIO_REG(0x144),
		.out_or =	GPIO_REG(0x148),
		.out_bic =	GPIO_REG(0x14C),
		.out_xor =	GPIO_REG(0x150),
		.strength0 =	GPIO_REG(0x154),
		.strength1 =	GPIO_REG(0x158),
		.pe =		GPIO_REG(0x15C),
		.ps =		GPIO_REG(0x160),
		.fn0 =		GPIO_REG(0x164),
		.fn1 =		GPIO_REG(0x168),
		.fn2 =		GPIO_REG(0x16C),
		.fn3 =		GPIO_REG(0x170),
		.in_en =	GPIO_REG(0x174),
		.in_type =	GPIO_REG(0x178),
		.slew_rate =	GPIO_REG(0x17C),
	},
	/* GPIO G */
	{
		.data =		GPIO_REG(0x1C0),
		.out_en =	GPIO_REG(0x1C4),
		.out_or =	GPIO_REG(0x1C8),
		.out_bic =	GPIO_REG(0x1CC),
		.out_xor =	GPIO_REG(0x1D0),
		.strength0 =	GPIO_REG(0x1D4),
		.strength1 =	GPIO_REG(0x1D8),
		.pe =		GPIO_REG(0x1DC),
		.ps =		GPIO_REG(0x1E0),
		.fn0 =		GPIO_REG(0x1E4),
		.fn1 =		GPIO_REG(0x1E8),
		.fn2 =		GPIO_REG(0x1EC),
		.fn3 =		GPIO_REG(0x1F0),
		.in_en =	GPIO_REG(0x1F4),
		.in_type =	GPIO_REG(0x1F8),
		.slew_rate =	GPIO_REG(0x1FC),
	},
};

static gpioregs *find_gpio(unsigned n, unsigned *bit)
{
	*bit = 1 << (n&GPIO_BITMASK);
	return GPIO_REGS + ((n&GPIO_REGMASK) >> 5);
}

int gpio_config(unsigned n, unsigned flags)
{
	gpioregs *r;
	unsigned b;

	if ((r = find_gpio(n, &b)) == 0)
		return -1;

	if (flags & GPIO_FN_BITMASK) {
		unsigned bit = n & GPIO_BITMASK;;
		unsigned fn = ((flags & GPIO_FN_BITMASK) >> GPIO_FN_SHIFT) - 1;

		if (bit < 8)
			RMWREG32(r->fn0, bit*4, 4, fn);
		else if (bit < 16)
			RMWREG32(r->fn1, (bit-8)*4, 4, fn);
		else if (bit < 24)
			RMWREG32(r->fn2, (bit-16)*4, 4, fn);
		else
			RMWREG32(r->fn3, (bit-24)*4, 4, fn);
	}

	if (flags & GPIO_CD_BITMASK) {
		unsigned bit = n & GPIO_BITMASK;;
		unsigned cd = ((flags & GPIO_CD_BITMASK) >> GPIO_CD_SHIFT) - 1;

		if (bit < 16)
			RMWREG32(r->strength0, bit*2, 2, cd);
		else
			RMWREG32(r->strength1, (bit-16)*2, 2, cd);
	}

	if (flags & GPIO_OUTPUT) {
		if (flags & GPIO_HIGH)
			writel(readl(r->data) | b, r->data);
		writel(readl(r->out_en) | b, r->out_en);
		if (flags & GPIO_HIGH)
			writel(readl(r->data) | b, r->data);

	} else if (flags & GPIO_INPUT) {
		writel(readl(r->out_en) & (~b), r->out_en);
	}
	return 0;
}

void gpio_set(unsigned n, unsigned on)
{
	gpioregs *r;
	unsigned b;
	unsigned v;

	if((r = find_gpio(n, &b)) == 0)
		return;

	v = readl(r->data);
	if (on) {
		writel(v | b, r->data);
	} else {
		writel(v & (~b), r->data);
	}
}

int gpio_get(unsigned n)
{
	gpioregs *r;
	unsigned b;

	if((r = find_gpio(n, &b)) == 0) return 0;

	return (readl(r->data) & b) ? 1 : 0;
}
