/*
 * arch/arm/mach-tcc88xx/pm.h
 *
 * Author:  <linux@telechips.com>
 * Created: April 21, 2008
 * Description: LINUX POWER MANAGEMENT FUNCTIONS
 *
 * Copyright (C) 2008-2009 Telechips 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
#ifndef __TCC_PM_H__
#define __TCC_PM_H__

/*===========================================================================

                                  MODE

===========================================================================*/

//#define TCC_PM_PMU_CTRL
//#define TCC_PM_MEMQ_PWR_CTRL
#define TCC_PM_SSTLIO_CTRL

#define CONFIG_SLEEP_MODE
#if defined(CONFIG_DDR2_HY5PS1G831CFPS6) \
		|| defined(CONFIG_DDR2_HXB18T2G160AF) \
		|| defined(CONFIG_DDR2_HY5PS1G1631CFPS6) \
		|| defined(CONFIG_DDR2_K4T1G164QE_HCF7) \
		|| defined(CONFIG_DDR2_K4T1G084QF_BCE7) \
		|| defined(CONFIG_DDR2_CT83486C1) \
		|| defined(CONFIG_DDR2_MT47H256M8EB25E)
	#define CONFIG_DRAM_DDR2
#elif defined(CONFIG_DDR3_H5TQ2G83BFR_H9C) \
		|| defined(CONFIG_DDR3_K4B2G1646C_HCK0) \
		|| defined(CONFIG_DDR3_K4B1G1646E_HCH9) \
		|| defined(CONFIG_DDR3_H5TQ1G83BFR_H9C)
	#define CONFIG_DRAM_DDR3
#elif defined(CONFIG_LPDDR2_K4P4G324EB_AGC1)
    #define CONFIG_DRAM_LPDDR2
#endif
#if defined(PLATFORM_TCC892X) && defined(TARGET_BOARD_STB)
	#define CONFIG_MACH_TCC8920ST
	#define machine_is_tcc8920st()		1
	#define machine_is_m801_88()		0
	#define machine_is_m803()			0
	#define machine_is_m805_892x()		0
#endif

/*===========================================================================

                               BOOTLOADER

===========================================================================*/

#define asm		__asm__ volatile

#define IO_OFFSET		0x00000000	/* Virtual IO = 0xF0000000 */
#define tcc_p2v(pa)	((unsigned int)(pa) + IO_OFFSET)

/*
 * Save the current interrupt enable state.
 */
#define raw_local_irq_save(x)					\
	({							\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ local_irq_save\n"	\
	"cpsid	i"						\
	: "=r" (x) : : "memory", "cc");				\
	})

/*
 * restore saved IRQ & FIQ state
 */
#define raw_local_irq_restore(x)				\
	__asm__ __volatile__(					\
	"msr	cpsr_c, %0		@ local_irq_restore\n"	\
	:							\
	: "r" (x)						\
	: "memory", "cc")

/*
 * Disable IRQs
 */
#define raw_local_irq_disable()					\
	({							\
		unsigned long temp;				\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ local_irq_disable\n"	\
"	orr	%0, %0, #128\n"					\
"	msr	cpsr_c, %0"					\
	: "=r" (temp)						\
	:							\
	: "memory", "cc");					\
	})

#define local_irq_save(flags)		raw_local_irq_save(flags)
#define local_irq_disable()			raw_local_irq_disable()
#define local_flush_tlb_all()		while(0)
#define local_irq_restore(i)		raw_local_irq_restore(i)
#define flush_cache_all()			while(0)

typedef long word;

#define lsize sizeof(word)
#define lmask (lsize - 1)

static void *memcpy(void *dest, const void *src, int count)
{
    char *d = (char *)dest;
    const char *s = (const char *)src;
    int len;

    if(count == 0 || dest == src)
        return dest;

    if(((long)d | (long)s) & lmask) {
        // src and/or dest do not align on word boundary
        if((((long)d ^ (long)s) & lmask) || (count < lsize))
            len = count; // copy the rest of the buffer with the byte mover
        else
            len = lsize - ((long)d & lmask); // move the ptrs up to a word boundary

        count -= len;
        for(; len > 0; len--)
            *d++ = *s++;
    }
    for(len = count / lsize; len > 0; len--) {
        *(word *)d = *(word *)s;
        d += lsize;
        s += lsize;
    }
    for(len = count & lmask; len > 0; len--)
        *d++ = *s++;

    return dest;
}

#define HwCLK_BASE		HwCKC_BASE

#if defined(HW_REV)
	int system_rev = HW_REV;
#endif

#if defined(CONFIG_SHUTDOWN_MODE)
/*===========================================================================

                              Shut-down MAP

===========================================================================*/

/*---------------------------------------------------------------------------
 1) Shut-down (shut down + sram boot + sdram self-refresh)

     << sram >>
     0xF0000000(0x10000000) ------------------
                           |    Boot code     | 0x700
                     0x700  ------------------
                           |      Stack       | 0x100
                     0x800  ------------------
                           |     Shutdown     | 0x600
                     0xE00  ------------------
                           |     Wake-up      | 0x400
                    0x1200  ------------------
                           |    SDRAM Init    | 0x700
                    0x1900  ------------------
                           |   GPIO Storage   | 0x300
                    0x1C00  ------------------
                           |   cpu_reg/mmu data | 0x100
                    0x1D00  ------------------

---------------------------------------------------------------------------*/

#define SRAM_BOOT_ADDR           0xF0000000
#define SRAM_BOOT_SIZE           0x00000700

#define SRAM_STACK_ADDR          0xF0000700
#define SRAM_STACK_SIZE          0x00000100

#define SHUTDOWN_FUNC_ADDR       0xF0000800
#define SHUTDOWN_FUNC_SIZE       0x00000600

#define WAKEUP_FUNC_ADDR         0xF0000E00
#define WAKEUP_FUNC_SIZE         0x00000400

#define SDRAM_INIT_FUNC_ADDR     0xF0001200
#define SDRAM_INIT_FUNC_SIZE     0x00000700

#define GPIO_REPOSITORY_ADDR     0xF0001900
#define GPIO_REPOSITORY_SIZE     0x00000300

#define CPU_DATA_REPOSITORY_ADDR 0xF0001C00
#define CPU_DATA_REPOSITORY_SIZE 0x00000100

/*-------------------------------------------------------------------------*/


/*===========================================================================

                        Shut-down Backup Registers

===========================================================================*/
#if defined(CONFIG_PM_CONSOLE_NOT_SUSPEND)
typedef struct _BACKUP_UART {
	volatile unsigned int	DLL;	// 0x000  R/W  0x00000000   Divisor Latch (LSB) (DLAB=1)
	volatile unsigned int	IER;		// 0x004  R/W  0x00000000   Interrupt Enable Register (DLAB=0)
	volatile unsigned int	DLM;	// 0x004  R/W  0x00000000   Divisor Latch (MSB) (DLAB=1)
	volatile unsigned int	LCR;	// 0x00C  R/W  0x00000003   Line Control Register
	volatile unsigned int	MCR;	// 0x010  R/W  0x00000040   MODEM Control Register
	volatile unsigned int	AFT;	// 0x020  R/W  0x00000000   AFC Trigger Level Register
	volatile unsigned int	UCR;	// 0x024  R/W  0x00000000   UART Control register
	volatile unsigned int	CFG0;	// R/W  0x00000000   Port Configuration Register 0(PCFG0)
	volatile unsigned int	CFG1;	// R/W  0x00000000   Port Configuration Register 1(PCFG1)
} bkUART;
#endif

typedef struct _TCC_REG_{
	CKC ckc;
	PIC pic;
	VIC vic;
	TIMER timer;
#if defined(TCC_PM_PMU_CTRL)
	PMU pmu;
#endif
	SMUCONFIG smuconfig;
	IOBUSCFG iobuscfg;
	MEMBUSCFG membuscfg;
	unsigned L2_aux;

#if defined(CONFIG_PM_CONSOLE_NOT_SUSPEND)
	bkUART	bkuart;
#endif
} TCC_REG, *PTCC_REG;

#elif defined(CONFIG_SLEEP_MODE)

/*===========================================================================

                                  MODE

===========================================================================*/

#define TCC_PM_SLEEP_WFI_USED

/*===========================================================================

                              Sleep MAP

===========================================================================*/

/*---------------------------------------------------------------------------
 1) Sleep (Sleep + sdram self-refresh)

     << sram >>
     0xF0000000(0x10000000) ------------------
                           |                  | 0x700
                     0x700  ------------------
                           |      Stack       | 0x100
                     0x800  ------------------
                           |      Sleep       | 0xA00
                    0x1200  ------------------
                           |    SDRAM Init    | 0x700
                    0x1900  ------------------

---------------------------------------------------------------------------*/

#define SRAM_STACK_ADDR          0xF0000700
#define SRAM_STACK_SIZE          0x00000100

#define SLEEP_FUNC_ADDR          0xF0000800
#define SLEEP_FUNC_SIZE          0x00000A00

#define SDRAM_INIT_FUNC_ADDR     0xF0001200
#define SDRAM_INIT_FUNC_SIZE     0x00000700

/*-------------------------------------------------------------------------*/


/*===========================================================================

                         Sleep Backup Registers

===========================================================================*/

typedef struct _TCC_REG_{
	CKC ckc;
	PMU pmu;
	unsigned L2_aux;
} TCC_REG, *PTCC_REG;
#endif

/*-------------------------------------------------------------------------*/

#endif  /*__TCC_PM_H__*/
