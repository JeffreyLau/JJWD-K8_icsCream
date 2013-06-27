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

#include <platform/reg_physical.h>

#define TCC_GPIO_BASE        HwGPIO_BASE
#define TCC_UART0_BASE       HwUART0_BASE
#define TCC_UART1_BASE       HwUART1_BASE
#define TCC_UART2_BASE       HwUART2_BASE
#define TCC_UART3_BASE       HwUART3_BASE
#define TCC_UART4_BASE       HwUART4_BASE
#define TCC_UART5_BASE       HwUART5_BASE
#define TCC_UART6_BASE       HwUART6_BASE
#define TCC_UART7_BASE       HwUART7_BASE
#define TCC_IOBUSCFG_BASE    HwIOBUSCFG_BASE
#define TCC_VIC_BASE         HwPIC_BASE
#define TCC_TIMER_BASE       HwTMR_BASE
#define TCC_PMU_BASE         HwPMU_BASE
#define TCC_I2C_CH0_BASE     HwI2C_MASTER0_BASE
#define TCC_SMU_I2C_CH0_BASE HwSMUI2C_BASE
#define TCC_USB20OTG_BASE	 HwUSB20OTG_BASE
#define TCC_USBPHYCFG_BASE	 HwUSBPHYCFG_BASE
#define TCC_TSADC_BASE		 HwTSADC_BASE

#endif
