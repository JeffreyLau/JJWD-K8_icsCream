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
#include <arch/arm.h>
#include <reg.h>
#include <kernel/thread.h>
#include <platform/interrupts.h>
#include <platform/irqs.h>
#include <platform/iomap.h>

#define VIC_REG(off)	(TCC_VIC_BASE + (off))

#define VIC_INT_EN0         VIC_REG(0x0000)
#define VIC_INT_EN1         VIC_REG(0x0004)
#define VIC_INT_CLEAR0      VIC_REG(0x0008)
#define VIC_INT_CLEAR1      VIC_REG(0x000C)
#define VIC_IRQ_STATUS0     VIC_REG(0x0010)
#define VIC_IRQ_STATUS1     VIC_REG(0x0014)
#define VIC_INT_SELECT0     VIC_REG(0x0018)  /* 1: FIQ, 0: IRQ */
#define VIC_INT_SELECT1     VIC_REG(0x001C)  /* 1: FIQ, 0: IRQ */
#define VIC_INT_POLARITY0   VIC_REG(0x0038)  /* 1: NEG, 0: POS */
#define VIC_INT_POLARITY1   VIC_REG(0x003C)  /* 1: NEG, 0: POS */
#define VIC_RAW_STATUS0     VIC_REG(0x0040)
#define VIC_RAW_STATUS1     VIC_REG(0x0044)
#define VIC_INT_MODE0       VIC_REG(0x0060)
#define VIC_INT_MODE1       VIC_REG(0x0064)
#define VIC_INT_MODEA0      VIC_REG(0x0078)
#define VIC_INT_MODEA1      VIC_REG(0x007C)
#define VIC_INT_MASK0       VIC_REG(0x0100)
#define VIC_INT_MASK1       VIC_REG(0x0104)
#define VIC_INT_ALLMASK     VIC_REG(0x0108)
#define VIC_VNIRQ           VIC_REG(0x0208)
#define VIC_VNFIQ           VIC_REG(0x020C)
#define VIC_VCTRL           VIC_REG(0x0210)

struct ihandler {
	int_handler func;
	void *arg;
};

static struct ihandler handler[NR_IRQS];

void platform_init_interrupts(void)
{
	/* XXX: relocate vector table */
	__asm__ ("ldr	r1, =_vectors\n" \
		 "mov	r0, #0x00000000\n" \
		 "add	r2, r0, #(15*4)\n" \
		 "1:\n" \
		 "ldr	r3, [r1], #4\n" \
		 "str	r3, [r0], #4\n" \
		 "cmp	r0, r2\n" \
		 "bne	1b\n");

	writel(0, VIC_INT_MASK0);
	writel(0, VIC_INT_MASK1);

	/* disable all interrupts */
	writel(0x00000000, VIC_INT_EN0);
	writel(0x00000000, VIC_INT_EN1);

	writel(0xffffffff, VIC_INT_SELECT0);
	writel(0xffffffff, VIC_INT_SELECT1);

	writel(0x00000000, VIC_INT_POLARITY0);
	writel(0x10000000, VIC_INT_POLARITY1);

	writel(0xffffffff, VIC_INT_MODE0);
	writel(0xffffffff, VIC_INT_MODE1);

	writel(0x00000000, VIC_INT_MASK0);
	writel(0x00000000, VIC_INT_MASK1);

	writel(0x00000000, VIC_INT_MODEA0);
	writel(0x00000000, VIC_INT_MODEA1);

	writel(0x00000001, VIC_INT_ALLMASK);

	writel(0x00000000/*0x80000000*/, VIC_VCTRL);
}

enum handler_return platform_irq(struct arm_iframe *frame)
{
	unsigned num;
	enum handler_return ret;
	num = readl(VIC_VNIRQ);

	if (num > NR_IRQS)
		return 0;
	ret = handler[num].func(handler[num].arg);
	writel(readl(VIC_INT_CLEAR0), VIC_INT_CLEAR0);
	return ret;
}

void platform_fiq(struct arm_iframe *frame)
{
	PANIC_UNIMPLEMENTED;
}

status_t mask_interrupt(unsigned int vector)
{
	unsigned reg0 = (vector > 31) ? VIC_INT_MASK1 : VIC_INT_MASK0;
	unsigned reg1 = (vector > 31) ? VIC_INT_EN1 : VIC_INT_EN0;
	unsigned bit = 1 << (vector & 31);
	writel(readl(reg0) & ~bit, reg0);
	writel(readl(reg1) & ~bit, reg1);
	return 0;
}

status_t unmask_interrupt(unsigned int vector)
{
	unsigned reg0 = (vector > 31) ? VIC_INT_MASK1 : VIC_INT_MASK0;
	unsigned reg1 = (vector > 31) ? VIC_INT_EN1 : VIC_INT_EN0;
	unsigned bit = 1 << (vector & 31);
	writel(readl(reg0) | bit, reg0);
	writel(readl(reg1) | bit, reg1);
	return 0;
}

void register_int_handler(unsigned int vector, int_handler func, void *arg)
{
	if (vector >= NR_IRQS)
		return;

	enter_critical_section();
	handler[vector].func = func;
	handler[vector].arg = arg;
	exit_critical_section();
}

