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

#if defined(PLATFORM_TCC88XX) && defined(TARGET_BOARD_STB)
	#define CONFIG_MACH_TCC8800ST
	#define machine_is_tcc8800st()		1
#endif

//#define TCC_PM_BOTH_SUSPEND_USED
#define CONFIG_TCC88XX_RTC_INTERNAL_PATH

#if defined(CONFIG_TCC88XX_RTC_INTERNAL_PATH)
#define CONFIG_SLEEP_MODE
#else
#define CONFIG_SHUTDOWN_MODE
#endif

#ifdef TCC_PM_BOTH_SUSPEND_USED
 #define TCC_PM_SHUTDOWN_MODE
 #define TCC_PM_SLEEP_MODE
 #define TCC_PM_SLEEP_WFI_USED
#else
 #if defined(CONFIG_SHUTDOWN_MODE)
 #define TCC_PM_SHUTDOWN_MODE
 #elif defined(CONFIG_SLEEP_MODE)
 #define TCC_PM_SLEEP_MODE
 #define TCC_PM_SLEEP_WFI_USED
 #endif
#endif

#define TCC_MEMBUS_PWR_CTRL_USED

//#define TCC_PMIC_COREPWR_CTRL_USED

//#define PORT_ARRANGE_USED


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

#if defined(TCC_PM_SHUTDOWN_MODE)
/*===========================================================================

                              Shut-down MAP

===========================================================================*/

/*---------------------------------------------------------------------------
 1) Shut-down (shut down + sram boot + sdram self-refresh)

     << sram >>
     0x10000000(0xEFF00000) ------------------
                           |    Boot code     | 0x700
                     0x700  ------------------
                           |      Stack       | 0x100
                     0x800  ------------------
                           |   I/O restore    | 0x200
                     0xA00  ------------------
                           |   SDRAM Init     | 0x700
                    0x1100  ------------------
                           | GPIO Backup Data | 0x300
                    0x1400  ------------------
                           |  Enter Shutdown  | 0x400
                    0x1800  ------------------
                           |   mmu on code    | 0x50
                    0x1850  ------------------
                           | cpu_reg/mmu data | 0x100
                    0x1950  ------------------
#ifdef PORT_ARRANGE_USED
                           | GPIO Backup Data | 0x300
                    0x1C50  ------------------
#endif
                           |                  |
                             : : : : : : : : :
                           |                  |
                    0x2000  ------------------  
                           | core power ctrl. | 0x1000
                    0x3000  ------------------

---------------------------------------------------------------------------*/

#define SRAM_BOOT_ADDR           0xEFF00000
#define SRAM_BOOT_SIZE           0x00000700

#define SHUTDOWN_STACK_ADDR      0xEFF00700
#define SHUTDOWN_STACK_SIZE      0x00000100
#define SHUTDOWN_STACK_PHY       0x10000700

#define IO_RESTORE_FUNC_ADDR     0xEFF00800
#define IO_RESTORE_FUNC_SIZE     0x00000200

#define GPIO_BUF_ADDR            0xEFF01100
#define GPIO_BUF_PHY             0x10001100

#define SHUTDOWN_FUNC_ADDR       0xEFF01400
#define SHUTDOWN_FUNC_SIZE       0x00000400
#define SHUTDOWN_FUNC_PHY        0x10001400

#define MMU_SWITCH_FUNC_ADDR     0xEFF01800
#define MMU_SWITCH_FUNC_SIZE     0x00000020
#define MMU_SWITCH_FUNC_PHY      0x10001800

#define REG_MMU_DATA_ADDR        0xEFF01850
#define REG_MMU_DATA_PHY         0x10001850

#ifdef PORT_ARRANGE_USED
#define GPIO_BUF_ADDR_           0xEFF01950
#define GPIO_BUF_PHY_            0x10001950
#endif

#define MMU_SWITCH_EXEC_ADDR     0xF0700100

/*-------------------------------------------------------------------------*/


/*===========================================================================

                        Shut-down Backup Registers

===========================================================================*/

typedef struct _TCC_REG_{
	CKC ckc;
	PIC pic;
	VIC vic;
	TIMER timer;
	PMU pmu;
	SMUCONFIG smuconfig;
	//GPIO gpio;

	IOBUSCFG iobuscfg;

	NFC		nfc;
	UART	uart;
	UARTPORTMUX uartportmux;

	volatile unsigned int backup_peri_iobus0;
	volatile unsigned int backup_peri_iobus1;
} TCC_REG, *PTCC_REG;

/*-------------------------------------------------------------------------*/
#endif /* TCC_PM_SHUTDOWN_MODE */



#if defined(TCC_PM_SLEEP_MODE)
/*===========================================================================

                                Sleep MAP

===========================================================================*/

/*---------------------------------------------------------------------------
 2) sleep (sleep + sdram self-refresh)

     0x10000000(0xEFF00000) ------------------
                           |    Sleep code    | 0x700
                     0x700  ------------------
                           |      Stack       | 0x100
                     0x800  ------------------
                           |     not used     | 0x200
                     0xA00  ------------------
                           |   SDRAM Init     | 0x700
                    0x1100  ------------------
                           |                  |
                             : : : : : : : : :
                           |                  |
                    0x2000  ------------------  
                           | core power ctrl. | 0x1000
                    0x3000  ------------------

---------------------------------------------------------------------------*/

//#define SLEEP_FUNC_ADDR          0xEFF00000
#define SLEEP_FUNC_ADDR          0x10000100
#define SLEEP_FUNC_SIZE          0x00000600

//#define SLEEP_STACK_ADDR         0xEFF00700
#define SLEEP_STACK_ADDR         0x10000700
#define SLEEP_STACK_SIZE         0x00000100
#define SLEEP_STACK_PHY          0x10000700

/*-------------------------------------------------------------------------*/
#endif


/*===========================================================================

                         Shut-down/Sleep Common MAP

===========================================================================*/

//#define SDRAM_INIT_FUNC_ADDR     0xEFF00A00
#define SDRAM_INIT_FUNC_ADDR     0x10000A00
#define SDRAM_INIT_FUNC_SIZE     0x00000700
#define SDRAM_INIT_FUNC_PHY      0x10000A00

#define COREPWR_FUNC_ADDR        0xEFF02000
#define COREPWR_FUNC_SIZE        0x00000FE0
#define COREPWR_FUNC_PHY         0x10002000

#define COREPWR_PARAM_ADDR       0xEFF02FE0
#define COREPWR_PARAM_SIZE       0x00000020
#define COREPWR_PARAM_PHY        0x10002FE0

/*-------------------------------------------------------------------------*/

#endif  /*__TCC_PM_H__*/
