/*
 * Copyright (c) 2008 Travis Geiselbrecht
 * Copyright (c) 2010 Telechips, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <debug.h>
#include <err.h>
#include <reg.h>
#include <string.h>
#include <platform.h>
#include <i2c.h>
#include <platform/globals.h>
#include <platform/iomap.h>
#include <platform/reg_physical.h>
#include <tnftl/IO_TCCXXX.h>

/* 
 * Define
 */

#define read_reg(a)			(*(volatile unsigned long *)a)
#define write_reg(v, a)		(*(volatile unsigned long *)a = v)

/* I2C Controller */
#define I2C_IRQSTR		(TCC_I2C_CH0_BASE + 0xC0)
#define CH1_BASE_OFFSET		0x40		/* I2C & SMU_I2C ch1 base offset */

/* SMU_I2C */
#define I2C_ICLK_ADDRESS	(TCC_SMU_I2C_CH0_BASE + 0x80)	/* only SMU_I2C */

/* read/write bit */
#define I2C_WR	0
#define I2C_RD	1

/* 
 * I2C register structure
 */
struct tcc_i2c_regs {
	volatile unsigned long PRES, CTRL, TXR, CMD, RXR, SR, TR;
};

/* 
 * register structure
 */
struct tcc_i2c {
	int core;
	int ch;
	unsigned int clk;
	unsigned long IRQSTR;
	volatile struct tcc_i2c_regs *regs;
	unsigned int peri_name;
	unsigned int iobus_name;
	int is_reset;
};

static struct tcc_i2c i2c[NUM_I2C_CH] = {
	/* I2C Core0 Master Controller */
	[I2C_CORE0_CH0] = {
		.core = 0,
		.ch = 0,
		.clk = 400,
		.IRQSTR = I2C_CORE0_BASE + I2C_IRQSTR_OFFSET,
		.regs = (volatile struct tcc_i2c_regs *)I2C_CORE0_BASE,
		.peri_name = PERI_I2C0,
		.iobus_name = RB_I2CCONTROLLER0,
		.is_reset = 0,
	},
	[I2C_CORE0_CH1] = {
		.core = 0,
		.ch = 1,
		.clk = 400,
		.IRQSTR = I2C_CORE0_BASE + I2C_IRQSTR_OFFSET,
		.regs = (volatile struct tcc_i2c_regs *)(I2C_CORE0_BASE + CH1_BASE_OFFSET),
		.peri_name = PERI_I2C0,
		.iobus_name = RB_I2CCONTROLLER0,
		.is_reset = 0,
	},
	/* I2C Core1 Master Controller */
	[I2C_CORE1_CH0] = {
		.core = 1,
		.ch = 0,
		.clk = 100,
		.IRQSTR = I2C_CORE1_BASE + I2C_IRQSTR_OFFSET,
		.regs = (volatile struct tcc_i2c_regs *)I2C_CORE1_BASE,
		.peri_name = PERI_I2C1,
		.iobus_name = RB_I2CCONTROLLER1,
		.is_reset = 0,
	},
	[I2C_CORE1_CH1] = {
		.core = 1,
		.ch = 1,
		.clk = 100,
		.IRQSTR = I2C_CORE1_BASE + I2C_IRQSTR_OFFSET,
		.regs = (volatile struct tcc_i2c_regs *)(I2C_CORE1_BASE + CH1_BASE_OFFSET),
		.peri_name = PERI_I2C1,
		.iobus_name = RB_I2CCONTROLLER1,
		.is_reset = 0,
	},
	/* SMU_I2C Controller */
	[SMU_I2C_CH0] = {
		.core = 2,
		.ch = 0,
		.clk = 100,									/* SMU_I2C prescale value */
		.IRQSTR = SMU_I2C_BASE + I2C_ICLK_OFFSET,	/* SMU_I2C ICLK register address */
		.regs = (volatile struct tcc_i2c_regs *)SMU_I2C_BASE,
	},
	[SMU_I2C_CH1] = {
		.core = 2,
		.ch = 1,
		.clk = 100,
		.IRQSTR = SMU_I2C_BASE + I2C_ICLK_OFFSET,
		.regs = (volatile struct tcc_i2c_regs *)(SMU_I2C_BASE + CH1_BASE_OFFSET),
	},
};

static int wait_intr(int i2c_name)
{
	volatile unsigned long cnt = 0;

	if (i2c_name < SMU_I2C_CH0) {
//#if defined(BOARD_TCC880X_STB_DEMO)
#if 1	
		while (!(read_reg(i2c[i2c_name].IRQSTR) & (i2c[i2c_name].ch?Hw1:Hw0))) {
			cnt++;
			if (cnt > 100000) {
				printf("i2c-tcc: time out!\n");
				return -1;
			}
		}
#else
	/* check RxACK */
	while (1) {
		cnt++;
		if ((i2c[i2c_name].regs->SR & Hw0)) {
			if (!(i2c[i2c_name].regs->SR & Hw7)) {
				break;
			}
		}
		if (cnt > 100000) {
			printf("i2c-tcc: time out!\n");
			return -1;
		}
	}
#endif
	} else {
		/* SMU_I2C wait */
		while (1) {
			cnt++;
			if (!(i2c[i2c_name].regs->SR & Hw1)) break;
			if (cnt > 100000) {
				printf("smu-i2c-tcc: time out!\n");
				return -1;
			}
		}
		for (cnt = 0; cnt <15; cnt++);
	}

	return 0;
}

int i2c_xfer(unsigned char slave_addr, 
		unsigned char out_len, unsigned char* out_buf, 
		unsigned char in_len, unsigned char* in_buf, 
		int i2c_name)
{
	int ret;
	unsigned int i = 0;

	/* 
	 * WRITE 
	 */
	if (out_len > 0) {
		i2c[i2c_name].regs->TXR = slave_addr | I2C_WR;
		i2c[i2c_name].regs->CMD = Hw7 | Hw4;

		ret = wait_intr(i2c_name);
		if (ret != 0) return ret;
		BITSET(i2c[i2c_name].regs->CMD, Hw0); //Clear a pending interrupt

		for (i = 0; i < out_len; i++) {
			i2c[i2c_name].regs->TXR = out_buf[i];
			i2c[i2c_name].regs->CMD = Hw4;

			ret = wait_intr(i2c_name);
			if (ret != 0) return ret;
			BITSET(i2c[i2c_name].regs->CMD, Hw0); //Clear a pending interrupt
		}

		if(in_len <= 0)
		{
			i2c[i2c_name].regs->CMD = Hw6;

			ret = wait_intr(i2c_name);
			if (ret != 0) return ret;
		}

		BITSET(i2c[i2c_name].regs->CMD, Hw0); //Clear a pending interrupt
	}

	/* 
	 * READ
	 */
	if (in_len > 0) {
		i2c[i2c_name].regs->TXR = slave_addr | I2C_RD;
		i2c[i2c_name].regs->CMD = Hw7 | Hw4;

		ret = wait_intr(i2c_name);
		if (ret != 0) return ret;
		BITSET(i2c[i2c_name].regs->CMD, Hw0); //Clear a pending interrupt

		for (i = 0; i < in_len; i++) {
			//i2c[i2c_name].regs->CMD = Hw5 | Hw3;
			if (i == (in_len - 1)) 
    			i2c[i2c_name].regs->CMD = Hw5 | Hw3;
    		else
    			i2c[i2c_name].regs->CMD = Hw5;

			ret = wait_intr(i2c_name);
			if (ret != 0) return ret;

			BITSET( i2c[i2c_name].regs->CMD, Hw0); //Clear a pending interrupt

			in_buf[i] =(unsigned char)i2c[i2c_name].regs->RXR;
		}

		i2c[i2c_name].regs->CMD = Hw6;

		ret = wait_intr(i2c_name);
		if (ret != 0) return ret;
		BITSET( i2c[i2c_name].regs->CMD, Hw0); //Clear a pending interrupt
	}

	return 0;
}

static void i2c_reset(int i2c_name)
{
	if (!i2c[i2c_name].is_reset) {
		tca_ckc_setioswreset(i2c[i2c_name].iobus_name, 1);
		tca_ckc_setioswreset(i2c[i2c_name].iobus_name, 0);

		if (i2c[i2c_name].core == 0) {
			i2c[0].is_reset = 1;
			i2c[1].is_reset = 1;
		} else if (i2c[i2c_name].core == 1) {
			i2c[2].is_reset = 1;
			i2c[3].is_reset = 1;
		}
	}
}

static void i2c_set_gpio(int i2c_name)
{
	PGPIO gpio = (PGPIO)HwGPIO_BASE;

	switch (i2c_name) {
		case I2C_CORE0_CH0:
			BITCSET(gpio->GPAFN0, (Hw8-Hw0), Hw4|Hw0);	/* GPIO_A[1:0] */
			BITSET(gpio->GPAEN, Hw1|Hw0);
			BITCLR(gpio->GPADAT, Hw1|Hw0);
			break;
		case I2C_CORE0_CH1:
			BITCSET(gpio->GPAFN0, (Hw32-Hw28), Hw28);	/* GPIO_A[7] */
			BITCSET(gpio->GPAFN1, (Hw4-Hw0), Hw0);		/* GPIO_A[8] */
			BITSET(gpio->GPAEN, Hw8|Hw7);
			BITCLR(gpio->GPADAT, Hw8|Hw7);
			break;
		case I2C_CORE1_CH0:
			/* Not Used */
			break;
		case I2C_CORE1_CH1:
			BITCSET(gpio->GPDFN2, (Hw32-Hw24), Hw30|Hw28 | Hw26|Hw24);	/* GPIO_D[23:22] */
			BITSET(gpio->GPDEN, Hw23|Hw22);
			BITCLR(gpio->GPDDAT, Hw23|Hw22);
			break;
		default:
			break;
		}
}

static void i2c_set_clock(int i2c_name, unsigned int i2c_clk_input_freq_khz)
{
	unsigned int prescale;

	prescale = i2c_clk_input_freq_khz / (i2c[i2c_name].clk * 5) - 1;
	i2c[i2c_name].regs->PRES = prescale;
	i2c[i2c_name].regs->CTRL = Hw7 | Hw6 | HwZERO;	/* start enable, stop enable, 8bit mode */
	BITSET(i2c[i2c_name].regs->CMD, Hw0);			/* clear pending interrupt */
#if 0
	printf("i2c%d SCK(%d) <-- input clk(%dKhz) prescale(%d)\n", 
			i2c_name, i2c[i2c_name].clk, i2c_clk_input_freq_khz, prescale)
#endif
}

static void i2c_enable(int i2c_name)
{
	int input_freq;

	tca_ckc_setperi(i2c[i2c_name].peri_name, ENABLE, 60000);	/* I2C peri bus enable */
	tca_ckc_setiopwdn(i2c[i2c_name].iobus_name, 0);						/* I2C io bus enable */
	input_freq = tca_ckc_getperi(i2c[i2c_name].peri_name);					/* get I2C bus clock */
	i2c_set_clock(i2c_name, (input_freq / 10));
}

int i2c_transmit(int bus, uint8_t address, const void *buf, size_t count)
{
}

int i2c_receive(int bus, uint8_t address, void *buf, size_t count)
{
}

int i2c_write_reg(int bus, uint8_t address, uint8_t reg, uint8_t val)
{
}

int i2c_read_reg(int bus, uint8_t address, uint8_t reg, uint8_t *val)
{
}

void i2c_init_early(void)
{
}

void i2c_init(void)
{
	int ch, input_freq;

	/* I2C Controller */
	for (ch = I2C_CORE0_CH0; ch <= I2C_CORE1_CH1; ch++) {
		i2c_reset(ch);
		i2c_set_gpio(ch);
		i2c_enable(ch);
	}

	/* SMU_I2C */
	for (ch = SMU_I2C_CH0; ch <= SMU_I2C_CH1; ch++) {
		write_reg(0x80000000, i2c[ch].IRQSTR);
		i2c[ch].regs->CTRL = 0;
		i2c[ch].regs->PRES = i2c[ch].clk;
		BITSET(i2c[ch].regs->CTRL, Hw7|Hw6);
	}
}
