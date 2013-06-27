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
#include <debug.h>
#include <sys/types.h>
#include <compiler.h>
#include <arch.h>
#include <arch/arm.h>
#include <arch/arm/mmu.h>
#include <lib/heap.h>

#if ARM_WITH_MMU

#define MB (1024*1024)

/* the location of the table may be brought in from outside */
#if WITH_EXTERNAL_TRANSLATION_TABLE
#if !defined(MMU_TRANSLATION_TABLE_ADDR)
#error must set MMU_TRANSLATION_TABLE_ADDR in the make configuration
#endif
static uint32_t *tt = (void *)MMU_TRANSLATION_TABLE_ADDR;
#else
/* the main translation table */
static uint32_t tt[4096] __ALIGNED(16384);
#endif

void arm_mmu_map_section(addr_t paddr, addr_t vaddr, uint flags)
{
	int index;
	uint AP;
	uint CB;
	uint TEX = 0;

	// XXX: move platform dependent codes
#if defined(PLATFORM_MSM7K)
	if ((paddr >= 0x88000000) && (paddr < 0xD0000000)) {
            /* peripherals in the 0x88000000 - 0xD0000000 range must
             * be mapped as DEVICE NON-SHARED: TEX=2, C=0, B=0
             */
		TEX = 2;
		flags &= (~(MMU_FLAG_CACHED | MMU_FLAG_BUFFERED));
	}
#endif
#if defined(PLATFORM_TCC88XX) || defined(PLATFORM_TCC92XX)
	if ((paddr >= 0xF0000000) && (paddr <= 0xFFFFFFFF)) {
		TEX = 2;
		flags &= (~(MMU_FLAG_CACHED | MMU_FLAG_BUFFERED));
	}
#endif
#if defined(PLATFORM_TCC892X)
	if ((paddr >= 0x00000000) && (paddr < 0x20000000)) {
		flags &= (~(MMU_FLAG_CACHED | MMU_FLAG_BUFFERED));
	}
	if ((paddr >= 0x60000000) && (paddr < 0x80000000)) {
		TEX = 2;
		flags &= (~(MMU_FLAG_CACHED | MMU_FLAG_BUFFERED));
	}
#endif
#if defined(PLATFORM_TCC93XX)
	if ((paddr >= 0xB0000000) && (paddr <= 0xBFFFFFFF)) {
		TEX = 2;
		flags &= (~(MMU_FLAG_CACHED | MMU_FLAG_BUFFERED));
	}
#endif

#ifdef WITH_DMA_ZONE
	if ((paddr >= dma_base()) && (paddr < (dma_base() + dma_size()))) {
		flags &= (~(MMU_FLAG_CACHED | MMU_FLAG_BUFFERED));
	}
#endif

	AP = (flags & MMU_FLAG_READWRITE) ? 0x3 : 0x2;
	CB = ((flags & MMU_FLAG_CACHED) ? 0x2 : 0) | ((flags & MMU_FLAG_BUFFERED) ? 0x1 : 0);

	index = vaddr / MB;
	// section mapping
	tt[index] = (paddr & ~(MB-1)) | (TEX << 12) | (AP << 10) | (0<<5) | (CB << 2) | (2<<0);

	arm_invalidate_tlb();
}

void arm_mmu_init(void)
{
	int i;

	/* set some mmu specific control bits */
	arm_write_cr1(arm_read_cr1() & ~((1<<29)|(1<<28)|(1<<0))); // access flag disabled, TEX remap disabled, mmu disabled

	/* set up an identity-mapped translation table with cache disabled */
	for (i=0; i < 4096; i++) {
#ifdef WITH_DMA_ZONE
		#if defined(PLATFORM_TCC892X)
		arm_mmu_map_section(i * MB, i * MB,  MMU_FLAG_READWRITE | MMU_FLAG_CACHED | MMU_FLAG_BUFFERED);
		#else
		arm_mmu_map_section(i * MB, i * MB,  MMU_FLAG_READWRITE | MMU_FLAG_CACHED);
		#endif
#else
		arm_mmu_map_section(i * MB, i * MB,  MMU_FLAG_READWRITE); // map everything uncached
#endif
	}

	/* set up the translation table base */
	arm_write_ttbr((uint32_t)tt);

	/* set up the domain access register */
	arm_write_dacr(0x00000001);

	/* turn on the mmu */
	arm_write_cr1(arm_read_cr1() | 0x1);
}

void arch_disable_mmu(void)
{
	arm_write_cr1(arm_read_cr1() & ~(1<<0)); // access flag disabled, TEX remap disabled, mmu disabled
}

#endif // ARM_WITH_MMU

