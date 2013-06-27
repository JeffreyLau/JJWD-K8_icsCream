/*
 * Copyright (c) 2008 Travis Geiselbrecht
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
#ifndef __DEV_I2C_H
#define __DEV_I2C_H

/* I2C Controller */
#define I2C_CORE0_BASE		HwI2C_CORE0_BASE	/* I2C master ch0 base address */
#define I2C_CORE1_BASE		HwI2C_CORE1_BASE	/* I2C master ch0 base address */
#define I2C_IRQSTR_OFFSET	0xC0				/* IRQ status register (only I2C) */

 /* SMU_I2C */
#define SMU_I2C_BASE		HwSMUI2C_BASE		/* SMU_I2C master ch0 base address */
#define I2C_ICLK_OFFSET		0x80				/* I2C_SCL divider register (only SMU_I2C) */

#define CH1_BASE_OFFSET		0x40		/* I2C & SMU_I2C ch1 base offset */

enum {
	I2C_CORE0_CH0 = 0,
	I2C_CORE0_CH1,
	I2C_CORE1_CH0,
	I2C_CORE1_CH1,
	SMU_I2C_CH0,
	SMU_I2C_CH1,
	NUM_I2C_CH,
};

void i2c_init(void);
void i2c_init_early(void);

extern int i2c_xfer(unsigned char device_addr,
					unsigned char out_len, unsigned char* out_buf,
					unsigned char in_len, unsigned char* in_buf,
					int ch);

#endif

