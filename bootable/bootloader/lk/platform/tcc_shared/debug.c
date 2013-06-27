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

#include <debug.h>
#include <printf.h>
#include <arch/arm/dcc.h>
#include <dev/fbcon.h>
#include <dev/uart.h>
#include <target/debugconfig.h>

void _dputc(char c)
{
#if WITH_DEBUG_DCC
	if (c == '\n') {
		while (dcc_putc('\r') < 0);
	}
	while (dcc_putc(c) < 0);
#endif
#if WITH_DEBUG_UART
	if (c == '\n')
		uart_putc(DEBUG_UART, '\r');
	uart_putc(DEBUG_UART, c);
#endif
#if WITH_DEBUG_FBCON && WITH_DEV_FBCON
	fbcon_putc(c);
#endif
#if WITH_DEBUG_JTAG
	jtag_dputc(c);
#endif
}

int dgetc(char *c)
{
	int n;
#if WITH_DEBUG_DCC
	n = dcc_getc();
#elif WITH_DEBUG_UART
	n = uart_getc(DEBUG_UART, 0);
#else
	n = -1;
#endif
	if (n < 0) {
		return -1;
	} else {
		*c = n;
		return 0;
	}
}

void platform_halt(void)
{
	dprintf(INFO, "HALT: spinning forever...\n");
	for(;;);
}
