/*
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

#ifndef _PLATFORM_TCC88XX_IOMAP_H_
#define _PLATFORM_TCC88XX_IOMAP_H_

#define TCC_GPIO_BASE		0xF0308000
#define TCC_UART0_BASE		0xF0531000
#define TCC_UART1_BASE		0xF0531100
#define TCC_UART2_BASE		0xF0531200
#define TCC_UART3_BASE		0xF0531300
#define TCC_UART4_BASE		0xF0531400
#define TCC_UART5_BASE		0xF0531500
#define TCC_IOBUSCFG_BASE	0xF05F5000
#define TCC_VIC_BASE		0xF0401000
#define TCC_TIMER_BASE		0xF0403000
#define TCC_PMU_BASE		0xF0404000
#define TCC_I2C_CH0_BASE	0xF0530000	/* I2C master ch0 base address */
#define TCC_SMU_I2C_CH0_BASE	0xF0405000	/* SMU_I2C master ch0 base address */
#define TCC_USB20OTG_BASE	0xF0550000
#define TCC_USBPHYCFG_BASE	0xF05F5028
#define TCC_TSADC_BASE		0xF05F4000

#endif
