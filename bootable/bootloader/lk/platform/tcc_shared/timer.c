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
#include <sys/types.h>
#include <err.h>
#include <reg.h>
#include <debug.h>
#include <kernel/thread.h>
#include <platform.h>
#include <platform/interrupts.h>
#include <platform/timer.h>
#include <platform/irqs.h>
#include <platform/iomap.h>

static time_t tick_interval;
static platform_timer_callback timer_callback;
static void *callback_arg;

#define TIMER_REG(off)	*REG32(TCC_TIMER_BASE + (off))

#define TIMER_TCFG	0x00
#define TIMER_TCNT	0x04
#define TIMER_TREF	0x08
#define TIMER_TMREF	0x0C

#define TIMER_TC32EN	0x80		/* 32-bit counter enable / pre-scale value */
#define TIMER_TC32LDV	0x84		/* 32-bit counter load value */
#define TIMER_TC32CMP0	0x88		/* 32-bit counter match value 0 */
#define TIMER_TC32CMP1	0x8C		/* 32-bit */
#define TIMER_TC32PCNT	0x90
#define TIMER_TC32MCNT	0x94
#define TIMER_TC32IRQ	0x98

#define TIMER_TICK_RATE	12000000

static volatile uint32_t ticks;

status_t platform_set_periodic_timer(platform_timer_callback callback, void *arg, time_t interval)
{
	uint32_t ticks_per_interval = (uint64_t)interval * TIMER_TICK_RATE / 1000; // interval is in ms

	enter_critical_section();

	timer_callback = callback;
	callback_arg = arg;
	tick_interval = interval;

	TIMER_REG(TIMER_TC32EN) = 0;	/* stop the timer */
	TIMER_REG(TIMER_TC32LDV) = 0;
	TIMER_REG(TIMER_TC32IRQ) = 1 << 19;
	TIMER_REG(TIMER_TC32EN) = ticks_per_interval | (1 << 24);

	unmask_interrupt(INT_TIMER1);
	exit_critical_section();
	return NO_ERROR;
}

time_t current_time(void)
{
	return ticks;
}

static enum handler_return os_timer_tick(void *arg)
{
	if (TIMER_REG(TIMER_TC32IRQ) & 0x80000000)
		TIMER_REG(TIMER_TC32IRQ) = 0x80000000;

	ticks += tick_interval;
	return timer_callback(callback_arg, current_time());
}

void platform_init_timer(void)
{
	register_int_handler(INT_TIMER1, &os_timer_tick, NULL);
}

void platform_uninit_timer(void)
{
	TIMER_REG(TIMER_TC32EN) = 0x7fff;	/* stop the timer */
	TIMER_REG(TIMER_TC32IRQ) = 0x00000000;
}

void platform_halt_timers(void)
{
}

