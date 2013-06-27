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
#include <reg.h>
#include <dev/uart.h>
#include <platform/iomap.h>
#include <target/debugconfig.h>
#include <dev/gpio.h>
#include <platform/gpio.h>
#include <platform/reg_physical.h>

#ifndef BITCSET
#define BITCSET(X, CMASK, SMASK)	( (X) = ((((unsigned int)(X)) & ~((unsigned int)(CMASK))) | ((unsigned int)(SMASK))) )
#endif

struct uart_stat {
	addr_t base;
	int ch;
};

static struct uart_stat uart[] = {
	{ TCC_UART0_BASE, 0 },
	{ TCC_UART1_BASE, 1 },
	{ TCC_UART2_BASE, 2 },
	{ TCC_UART3_BASE, 3 },
	{ TCC_UART4_BASE, 4 },
	{ TCC_UART5_BASE, 5 },
#if defined (PLATFORM_TCC892X)
	{ TCC_UART6_BASE, 6 },
	{ TCC_UART7_BASE, 7 },
#endif
};

#define uart_reg_write(p, a, v)	writel(v, uart[p].base + (a))
#define uart_reg_read(p, a)		readl(uart[p].base + (a))

#define UART_RBR	0x00		/* receiver buffer register */
#define UART_THR	0x00		/* transmitter holding register */
#define UART_DLL	0x00		/* divisor latch (LSB) */
#define UART_IER	0x04		/* interrupt enable register */
#define UART_DLM	0x04		/* divisor latch (MSB) */
#define UART_IIR	0x08		/* interrupt ident. register */
#define UART_FCR	0x08		/* FIFO control register */
#define UART_LCR	0x0C		/* line control register */
#define UART_MCR	0x10		/* MODEM control register */
#define UART_LSR	0x14		/* line status register */
#define UART_MSR	0x18		/* MODEM status register */
#define UART_SCR	0x1C		/* scratch register */
#define UART_AFT	0x20		/* AFC trigger level register */
#define UART_SIER	0x50		/* interrupt enable register */

#define LCR_WLS_MSK	0x03		/* character length select mask */
#define LCR_WLS_5	0x00		/* 5 bit character length */
#define LCR_WLS_6	0x01		/* 6 bit character length */
#define LCR_WLS_7	0x02		/* 7 bit character length */
#define LCR_WLS_8	0x03		/* 8 bit character length */
#define LCR_STB		0x04		/* Number of stop Bits, off = 1, on = 1.5 or 2) */
#define LCR_PEN		0x08		/* Parity eneble */
#define LCR_EPS		0x10		/* Even Parity Select */
#define LCR_STKP	0x20		/* Stick Parity */
#define LCR_SBRK	0x40		/* Set Break */
#define LCR_BKSE	0x80		/* Bank select enable */

#define FCR_FIFO_EN	0x01		/* FIFO enable */
#define FCR_RXFR	0x02		/* receiver FIFO reset */
#define FCR_TXFR	0x04		/* transmitter FIFO reset */

#define MCR_RTS		0x02

#define LSR_DR		0x01
#define LSR_THRE	0x20		/* transmitter holding register empty */
#define LSR_TEMT	0x40		/* transmitter empty */

void uart_set_gpio(void)
{
#if defined(_TCC9200S_)
	writel(0x43215, 0xf0532600);
#elif defined(_TCC8800_) && ((HW_REV == 0x0602) || (HW_REV == 0x0612))
	writel(0x045213, 0xf0531600); //uart 3
#elif defined(_M801_88_)
	if(HW_REV != 0x0003 && HW_REV != 0x0004 && HW_REV != 0x0005 && HW_REV != 0x0006)
	    writel(0x045213, 0xf0531600); //uart 3
#elif defined(_TCC8800_) || defined(_TCC8900_) || defined(_M801_) || defined(_M57TE_) || defined(_M803_)
	writel(0x345210, 0xf0531600); // uart3 for gps
#elif defined (TARGET_TCC8920_EVM)
	PUARTPORTCFG pUARTPORTCFG = (PUARTPORTCFG)HwUART_PORTCFG_BASE;

	//Bruce, should be initialized to not used port.
	pUARTPORTCFG->PCFG0.nREG = 0x00000000;
	pUARTPORTCFG->PCFG1.nREG = 0x00000000;

	#if (HW_REV == 0x1005 || HW_REV == 0x1007)
		// UART0 : Debug UART : GPIO_F[25,26] - UT[16]
		BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF, 16);
		gpio_config(TCC_GPF(25), GPIO_FN9);
		gpio_config(TCC_GPF(26), GPIO_FN9);

		// UART1 : BT UART : GPIO_G[12-15] - UT[20]
		BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<8, 20<<8);

		// UART3 : GPS UART :GPIO_G[4,5] - UT[18]
		BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<24, 18<<24);
	#elif (HW_REV == 0x1006)
		// UART0 : Debug UART : GPIO_D[4,5] - UT[2]
		BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF, 2);
		gpio_config(TCC_GPD(4), GPIO_FN7);
		gpio_config(TCC_GPD(5), GPIO_FN7);

		// UART1 : BT UART : GPIO_G[12-15] - UT[20]
		BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<8, 20<<8);
		//Bruce, GPS is changed to option from defalut..
		// UART3 : GPS UART :GPIO_G[4,5] - UT[18]
		//BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<24, 18<<24);
		//gpio_config(TCC_GPG(4), GPIO_FN3);
		//gpio_config(TCC_GPG(5), GPIO_FN3);
	#elif (HW_REV == 0x1008)
		// UART0 : Debug UART : GPIO_E[28,29] - UT[13] Func5
		BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF, 13);
		gpio_config(TCC_GPE(28), GPIO_FN5);
		gpio_config(TCC_GPE(29), GPIO_FN5);

		// UART1 : BT UART : GPIO_G[12-15] - UT[20] Func3
		BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<8, 20<<8);

		// UART3 : GPS UART :GPIO_G[0,1] - UT[17] Func3
		BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<24, 17<<24);
	#else
		// UART0 : Debug UART : GPIO_G[0,1] - UT[17]
		BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF, 17);
		gpio_config(TCC_GPG(0), GPIO_FN3);
		gpio_config(TCC_GPG(1), GPIO_FN3);

		// UART1 : BT UART : GPIO_G[12-15] - UT[20]
		BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<8, 20<<8);

		// UART3 : GPS
		BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<24, 16<<24);
	#endif

#elif defined(TARGET_TCC8920ST_EVM)
	PUARTPORTCFG pUARTPORTCFG = (PUARTPORTCFG)HwUART_PORTCFG_BASE;

	//Bruce, should be initialized to not used port.
	pUARTPORTCFG->PCFG0.nREG = 0x00000000;
	pUARTPORTCFG->PCFG1.nREG = 0x00000000;

#if defined(TARGET_TCC8925_STB_DONGLE) || defined(TARGET_TCC8925_HDB892F)
	// UART0 : Debug UART : GPIO_F[25,26] - UT[16]
	BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF, 16);
	gpio_config(TCC_GPF(25), GPIO_FN9);
	gpio_config(TCC_GPF(26), GPIO_FN9);
#else
	// UART0 : Debug UART : GPIO_G[0,1] - UT[17]
	BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF, 17);
	gpio_config(TCC_GPG(0), GPIO_FN3);
	gpio_config(TCC_GPG(1), GPIO_FN3);
#endif

#if !defined(TARGET_TCC8925_STB_DONGLE)
	// UART1 : BT UART : GPIO_G[12-15] - UT[20]
	BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<8, 20<<8);
	gpio_config(TCC_GPG(12), GPIO_FN3);
	gpio_config(TCC_GPG(13), GPIO_FN3);
	gpio_config(TCC_GPG(14), GPIO_FN3);
	gpio_config(TCC_GPG(15), GPIO_FN3);
#endif

#if !(defined(TARGET_TCC8925_STB_DONGLE) || defined(TARGET_TCC8925_HDB892F))
    // UART3 : GPS
    BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<24, 16<<24);
    gpio_config(TCC_GPF(25), GPIO_FN9);
    gpio_config(TCC_GPF(26), GPIO_FN9);
#endif
        
#elif defined(TARGET_M805_892X_EVM)
	PUARTPORTCFG pUARTPORTCFG = (PUARTPORTCFG)HwUART_PORTCFG_BASE;

	//Bruce, should be initialized to not used port.
	pUARTPORTCFG->PCFG0.nREG = 0x00000000;
	pUARTPORTCFG->PCFG1.nREG = 0x00000000;

	#if (HW_REV == 0x2002) || (HW_REV == 0x2003) || (HW_REV == 0x2004) || (HW_REV == 0x2005)
	// UART0 : Debug UART : GPIO_D[4,5] - UT[2]
	BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF, 2);
	gpio_config(TCC_GPD(4), GPIO_FN7);
	gpio_config(TCC_GPD(5), GPIO_FN7);

	// UART1 : BT UART : GPIO_G[16-19] - UT[21]
	BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<8, 21<<8);
	gpio_config(TCC_GPG(16), GPIO_FN3);
	gpio_config(TCC_GPG(17), GPIO_FN3);
	gpio_config(TCC_GPG(18), GPIO_FN3);
	gpio_config(TCC_GPG(19), GPIO_FN3);

	// UART3 : GPS
	BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<24, 12<<24);
	gpio_config(TCC_GPE(12), GPIO_FN5);
	gpio_config(TCC_GPE(13), GPIO_FN5);

	#else
	// UART0 : Debug UART : GPIO_F[25,26] - UT[16]
	BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF, 16);
	gpio_config(TCC_GPF(25), GPIO_FN9);
	gpio_config(TCC_GPF(26), GPIO_FN9);

	// UART3 : GPS
	BITCSET(pUARTPORTCFG->PCFG0.nREG, 0xFF<<24, 19<<24);
	gpio_config(TCC_GPG(8), GPIO_FN3);
	gpio_config(TCC_GPG(9), GPIO_FN3);
	#endif
#endif
}

void uart_init_port(int port, uint baud)
{
	uint16_t baud_divisor = (UART_CLK / 16 / baud);

	uart_reg_write(port, UART_LCR, LCR_EPS | LCR_STB | LCR_WLS_8);	/* 8 data, 1 stop, no parity */
	uart_reg_write(port, UART_IER, 0);
	uart_reg_write(port, UART_LCR, LCR_BKSE | LCR_EPS | LCR_STB | LCR_WLS_8);	/* 8 data, 1 stop, no parity */
	uart_reg_write(port, UART_DLL, baud_divisor & 0xff);
	uart_reg_write(port, UART_DLM, (baud_divisor >> 8) & 0xff);
	uart_reg_write(port, UART_FCR, FCR_FIFO_EN | FCR_RXFR | FCR_TXFR | Hw4 | Hw5);
	uart_reg_write(port, UART_LCR, LCR_EPS | LCR_STB | LCR_WLS_8);	/* 8 data, 1 stop, no parity */
}

int uart_putc(int port, char c )
{
	/* wait for the last char to get out */
	while (!(uart_reg_read(port, UART_LSR) & LSR_THRE))
		;
	uart_reg_write(port, UART_THR, c);
	return 0;
}

int uart_getc(int port, bool wait)  /* returns -1 if no data available */
{
	if (wait) {
		/* wait for data to show up in the rx fifo */
		while (!(uart_reg_read(port, UART_LSR) & LSR_DR))
			;
	} else {
		if (!(uart_reg_read(port, UART_LSR) & LSR_DR))
			return -1;
	}
	return uart_reg_read(port, UART_RBR);
}

void uart_flush_tx(int port)
{
	/* wait for the last char to get out */
	while (!(uart_reg_read(port, UART_LSR) & LSR_TEMT))
		;
}

void uart_flush_rx(int port)
{

	/* empty the rx fifo */
	while (uart_reg_read(port, UART_LSR) & LSR_DR) {
		volatile char c = uart_reg_read(port, UART_RBR);
		(void)c;
	}
}

void uart_init_early(void)
{
	uart_set_gpio();

	uart_init_port(DEBUG_UART, 115200);
}

void uart_init(void)
{
	uart_flush_rx(DEBUG_UART);
}

