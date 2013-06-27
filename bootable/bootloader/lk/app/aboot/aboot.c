/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <app.h>
#include <debug.h>
#include <arch/arm.h>
#include <dev/udc.h>
#include <string.h>
#include <kernel/thread.h>
#include <arch/ops.h>

#include <dev/flash.h>
#include <lib/ptable.h>
#include <dev/keys.h>
#include <dev/fbcon.h>
#include <dev/gpio.h>

#include "recovery.h"
#include "bootimg.h"
#include "fastboot.h"

#ifdef PLATFORM_TCC
#include <platform/reg_physical.h>
#include <platform/globals.h>
#include <platform/gpio.h>
#include "sparse_format.h"
#endif

#ifdef BOOTSD_INCLUDE
#include <fwdn/Disk.h>
#include <sdmmc/sd_memory.h>
#include <sdmmc/emmc.h>
#if defined(_TCC9200_) || defined(_TCC9201_)
extern void sd_deinit(void);
#endif
#endif

#define DEFAULT_CMDLINE "mem=100M console=null"

#define EMMC_BOOT_IMG_HEADER_ADDR 0xFF000
#if defined(TCC93XX)
#define RECOVERY_MODE   0x02
#define FASTBOOT_MODE   0x01
#else
#define RECOVERY_MODE   0x77665502
#define FASTBOOT_MODE   0x77665500
#endif

#define ATAG_NONE       0x00000000
#define ATAG_CORE       0x54410001
#define ATAG_MEM        0x54410002
#define ATAG_VIDEOTEXT  0x54410003
#define ATAG_RAMDISK    0x54410004
#define ATAG_INITRD2    0x54420005
#define ATAG_SERIAL     0x54410006
#define ATAG_REVISION   0x54410007
#define ATAG_VIDEOLFB   0x54410008
#define ATAG_CMDLINE    0x54410009

#if defined(PLATFORM_TCC)
#define ATAG_PARTITION	0x54434370 /* TCCp*/
#else
#define ATAG_PARTITION	0x4d534d70 /* MSMp */
#endif

#define ATAG_DISPLAY	0x54434380

#if !defined(TCC_NAND_USE)
unsigned boot_into_recovery = 0;
#endif

unsigned boot_into_factory = 0;

#define BYTE_TO_SECTOR(X)			((X + 511) >> 9)

//static const char *emmc_cmdline = " androidboot.emmc=true";

static struct udc_device fastboot_udc_device = {
	.vendor_id	= 0x18d1,
	.product_id	= 0xD00D,
	.version_id	= 0x0100,
	.manufacturer	= "Google",
	.product	= "Android",
};

struct atag_ptbl_entry
{
	char name[16];
	unsigned long long offset;
	unsigned long long size;
	unsigned flags;
};

struct atag_tcc_entry
{
	char output;
	char resolution;
	char hdmi_resolution;
	char composite_resolution;
	char component_resolution;
	char hdmi_mode;
};

struct atag_tcc_entry tcc_param = {
	.output	= 0,
	.resolution	= 0,
	.hdmi_resolution = 0,
	.composite_resolution = 0,
	.component_resolution = 0,
};

void platform_uninit_timer(void);
unsigned* target_atag_mem(unsigned* ptr);
unsigned* target_atag_panel(unsigned* ptr);
unsigned* target_atag_revision(unsigned* ptr);
unsigned board_machtype(void);
unsigned check_reboot_mode(void);
void *target_get_scratch_address(void);
int target_is_emmc_boot(void);
void reboot_device(unsigned);
void target_battery_charging_enable(unsigned enable, unsigned disconnect);
int board_get_serialno(char *serialno);
int board_get_wifimac(char *wifimac);
int board_get_btaddr(char *btaddr);
#if defined(BOOTSD_INCLUDE)
void cmd_flash_emmc_sparse_img(const char* arg , void* data , unsigned sz);
#endif

#if defined(TNFTL_V8_INCLUDE)
void cmd_flash_sparse_img(const char* arg , void* data , unsigned sz);
#endif

#if !defined(TNFTL_V8_INCLUDE)
static void ptentry_to_tag(unsigned **ptr, struct ptentry *ptn)
{
	struct atag_ptbl_entry atag_ptn;
#if defined(PLATFORM_TCC)
	struct flash_info *flash_info = flash_get_info();
	unsigned block_size = flash_info->page_size * flash_info->block_size;
#endif

	memcpy(atag_ptn.name, ptn->name, 16);
	atag_ptn.name[15] = '\0';
#if defined(PLATFORM_TCC)
	atag_ptn.offset = (unsigned long long)( ( ptn->start ) - flash_info->offset) * block_size;
	atag_ptn.size = (unsigned long long) (ptn->length * block_size);
#else
	atag_ptn.offset =(unsigned long long) ptn->start;
	atag_ptn.size = (unsigned long long) ptn->length;
#endif
	atag_ptn.flags = ptn->flags;
	memcpy(*ptr, &atag_ptn, sizeof(struct atag_ptbl_entry));
	*ptr += sizeof(struct atag_ptbl_entry) / sizeof(unsigned);
}
#endif

void boot_linux(void *kernel, unsigned *tags,
		const char *cmdline, unsigned machtype,
		void *ramdisk, unsigned ramdisk_size)
{
	unsigned *ptr = tags;
	void (*entry)(unsigned,unsigned,unsigned*) = kernel;
	struct ptable *ptable;
	size_t cmdline_len;

	/* CORE */
	*ptr++ = 2;
	*ptr++ = ATAG_CORE;

	if (ramdisk_size) {
		*ptr++ = 4;
		*ptr++ = ATAG_INITRD2;
		*ptr++ = (unsigned)ramdisk;
		*ptr++ = ramdisk_size;
	}

	ptr = target_atag_mem(ptr);

	if (cmdline) {
		cmdline_len = strlen(cmdline);
		*ptr++ = 2 + ((cmdline_len + 3) / 4);
		*ptr++ = ATAG_CMDLINE;
		strcpy(ptr, cmdline);
		ptr += (cmdline_len + 3) / 4;
	}

	/* tag for the board revision */
	ptr = target_atag_revision(ptr);

	ptr = target_atag_is_camera_enable(ptr);

#if defined(NAND_BOOT_INCLUDE) && !defined(TNFTL_V8_INCLUDE)
	//if (!target_is_emmc_boot())
	{
		/* Skip NAND partition ATAGS for eMMC boot */
		if ((ptable = flash_get_ptable()) && (ptable->count != 0)) {
			int i, pt_count = 0;
			unsigned *p = ptr;

			*ptr++ = 2;
			*ptr++ = ATAG_PARTITION;
			for (i = 0; i < ptable->count; ++i) {
				struct ptentry *ptn = ptable_get(ptable, i);
				if (!(ptn->flags & PART_HIDDEN)) {
					ptentry_to_tag(&ptr, ptn);
					++pt_count;
				}
			}
			*p += pt_count * (sizeof(struct atag_ptbl_entry) /
					sizeof(unsigned));
		}
	}
#endif

#if defined(PLATFORM_TCC) && defined(TCC_LCD_USE)
	ptr = target_atag_panel(ptr);
#endif

	if(tcc_param.resolution == 0xFF)
	{
		tcc_param.output = 0x80;
		tcc_param.resolution = 0;
		//tcc_param.hdmi_resolution = 0;
		//tcc_param.composite_resolution = 0;
		//tcc_param.component_resolution = 0;
	}

	*ptr++ = 8;
	*ptr++ = ATAG_DISPLAY;
	*ptr++ = (unsigned)tcc_param.output;
	*ptr++ = (unsigned)tcc_param.resolution;
	*ptr++ = (unsigned)tcc_param.hdmi_resolution;
	*ptr++ = (unsigned)tcc_param.composite_resolution;
	*ptr++ = (unsigned)tcc_param.component_resolution;
	*ptr++ = (unsigned)tcc_param.hdmi_mode;

	/* END */
	*ptr++ = 0;
	*ptr++ = 0;

	dprintf(INFO, "booting linux @ %p, ramdisk @ %p (%d)\n",
			kernel, ramdisk, ramdisk_size);
	if (cmdline)
		dprintf(INFO, "cmdline: %s\n", cmdline);

	enter_critical_section();
	platform_uninit_timer();
	arch_disable_cache(UCACHE);
	arch_disable_mmu();

#ifdef PLATFORM_TCC92XX
	// XXX: should be removed
	// Interrupt Register Setting clear..
	*(unsigned int*)0xF0401000 = 0x00000000;
	*(unsigned int*)0xF0401008 = 0xFFFFFFFF;
	*(unsigned int*)0xF040100C = 0xFFFFFFFF;
	*(unsigned int*)0xF0401018 = 0x00000000;
	*(unsigned int*)0xF040101C = 0x00008200;
	*(unsigned int*)0xF0401030 = 0x00000000;
	*(unsigned int*)0xF0401034 = 0x00000000;
	*(unsigned int*)0xF0401038 = 0x00000000;
	*(unsigned int*)0xF040103C = 0x00000000;
	*(unsigned int*)0xF0401060 = 0x00000000;
	*(unsigned int*)0xF0401064 = 0x00008200;
	*(unsigned int*)0xF0401068 = 0xFFFFFFFF;
	*(unsigned int*)0xF040106C = 0xFFFFFFFF;
	*(unsigned int*)0xF0401100 = 0xFFFFFFFF;
	*(unsigned int*)0xF0401104 = 0xFFFFFFFF;
	*(unsigned int*)0xF0401108 = 0x00000001;

	// Timer Register Setting Clear...
	*(unsigned int*)0xF0403000 = 0x00000000;
	*(unsigned int*)0xF0403004 = 0x00000000;
	*(unsigned int*)0xF0403008 = 0x0000FFFF;
	*(unsigned int*)0xF040300C = 0x00000000;
	*(unsigned int*)0xF0403010 = 0x00000000;
	*(unsigned int*)0xF0403014 = 0x00000000;
	*(unsigned int*)0xF0403018 = 0x0000FFFF;
	*(unsigned int*)0xF040301C = 0x00000000;
	*(unsigned int*)0xF0403020 = 0x00000000;
	*(unsigned int*)0xF0403024 = 0x00000000;
	*(unsigned int*)0xF0403028 = 0x0000FFFF;
	*(unsigned int*)0xF040302C = 0x00000000;
	*(unsigned int*)0xF0403030 = 0x00000000;
	*(unsigned int*)0xF0403034 = 0x00000000;
	*(unsigned int*)0xF0403038 = 0x0000FFFF;
	*(unsigned int*)0xF040303C = 0x00000000;
	*(unsigned int*)0xF0403040 = 0x00000000;
	*(unsigned int*)0xF0403044 = 0x00000000;
	*(unsigned int*)0xF0403048 = 0x000FFFFF;
	*(unsigned int*)0xF040304C = 0x00000000;
	*(unsigned int*)0xF0403050 = 0x00000000;
	*(unsigned int*)0xF0403054 = 0x00000000;
	*(unsigned int*)0xF0403058 = 0x000FFFFF;
	*(unsigned int*)0xF0403060 = 0x00000000;
	*(unsigned int*)0xF0403070 = 0x00000000;
	*(unsigned int*)0xF0403078 = 0x00000000;
	*(unsigned int*)0xF0403080 = 0x00007FFF;
	*(unsigned int*)0xF0403098 = 0x00000000;

	// Interrupt Clear
	*(unsigned int*)0xF0401008 = 0xFFFFFFFF;
	*(unsigned int*)0xF040100C = 0xFFFFFFFF;
#endif

#if PLATFORM_TCC92XX // add for ITCM boot
	{
		extern void ITCM_Boot(void) __attribute__((weak));
		extern volatile void InitRoutine_Start(void) __attribute__((weak));

		unsigned int uCacheControl, uTLBBase;
		unsigned int uTemp;
		PVMTREGION pVMTREGION = (VMTREGION *)&HwREGION_BASE;
		// Region 2 for ITCM
		pVMTREGION->REGION2	= 0x00000000 + HwVMT_SZ(SIZE_1MB)+ HwVMT_DOMAIN(2) +
			HwVMT_REGION_AP_ALL + HwVMT_REGION_EN + HwVMT_CACHE_OFF + HwVMT_BUFF_ON;

		uTLBBase		= (unsigned int)&HwVMT_BASE;
		uCacheControl	= 0x00050078	// should be one
			+ Hw14			// round-robin replacement
			//	+ Hw13			// Vector address = 0xFFFF0000
			+ Hw12			// I-Cache Enable
			//	+ Hw9			// Rom Protection
			//	+ Hw8			// System Protection
			//	+ Hw7			// Big Endian
			+ Hw2			// D-Cache Enable
			//	+ Hw1			// Align fault enable
			+ Hw0			// MMU enable
			;
		uTemp = 0x55555555;

		__asm__ volatile (
				"mov	r0, #0x0 \n"
				"mcr	p15, 0, r0, c9, c2, 0 \n"

				// DTCM control
				"ldr r0, =0xA0000101 \n"   // DTCM : base = 0xA0000000, size = 16k
				"mcr p15, 0, r0, c9, c1, 0 \n"

				// ITCM control
				"ldr r0, =0x00000101 \n"   // ITCM : base = 0x00000000, size = 16k
				"mcr p15, 0, r0, c9, c1, 1 \n"

				"mov	r0, #0x1 \n"
				"mcr	p15, 0, r0, c9, c2, 0 \n"
				"ldr r0, =0xA0000101 \n"   // DTCM : base = 0xA0000000, size = 16k
				"mcr p15, 0, r0, c9, c1, 0 \n"
				"ldr r0, =0x00000101 \n"   // ITCM : base = 0x00000000, size = 16k
				"mcr p15, 0, r0, c9, c1, 1 \n"

				// MMU control
				"mov r0, %0 \n"                   // %0 = uTLBBase
				"mcr p15, 0, r0, c2, c0, 0 \n"	  // set TLB Base = HwVMT_BASE, r0 = uTLBBase
				"mcr p15, 0, r0, c8, c7, 0 \n"	  // Invalidate TLB
				//"ldr r0, =0x55555555 \n"
				"mov r0, %2 \n"
				"mcr p15, 0, r0, c3, c0, 0 \n"    // Domain Setting (permission check is determined by AP setting of each region)

				// Cache control
				"mov r2, %1 \n"                   // %1 = uCacheControl
				"mov r0, #0x0 \n"
				"mcr p15, 0, r0, c7, c5, 0 \n"    // Flush Icache

				"cpy r0, lr \n"
				"bl asm_dcache_flush \n"		  // Flush Dcache (MUST use it)
				"cpy lr, r0 \n"

				"mcr p15, 0, r2, c1, c0, 0 \n"    // Enable Cache Control

				: /* output nothing */
				: "r" (uTLBBase), "r" (uCacheControl), "r" (uTemp)
				: "r0", "r2", "memory" );

		__asm__ volatile (
				"mov	r0, #0x0 \n"
				"mcr	p15, 0, r0, c9, c2, 0 \n" //; TCM Select
				"mov	r0, #0x0 \n"
				"mcr	p15, 0, r0, c9, c1, 0 \n"
				"mcr	p15, 0, r0, c9, c1, 1 \n"
				"ldr	r0, =(0xa0000000 + (0x04 << 2) + 1) \n"	//; DTCM : base = 0xa0000000, size = 8k
				"mcr	p15, 0, r0, c9, c1, 0 \n"
				"ldr	r0, =(0x00000000 + (0x04 << 2) + 1) \n"	//; ITCM : base = 0x00000000, size = 8k
				"mcr	p15, 0, r0, c9, c1, 1 \n"
				"mov r0, #0x01 \n"
				"mcr	p15, 0, r0, c9, c1, 2 \n"
				"mcr	p15, 0, r0, c9, c1, 3 \n"

				"mov	r0, #0x1 \n"
				"mcr	p15, 0, r0, c9, c2, 0 \n"
				"mov	r0, #0x0 \n"
				"mcr	p15, 0, r0, c9, c1, 0 \n"
				"mcr	p15, 0, r0, c9, c1, 1 \n"
				"ldr	r0, =(0xa0002000 + (0x04 << 2) + 1) \n"
				"mcr	p15, 0, r0, c9, c1, 0 \n"
				"ldr	r0, =(0x00002000 + (0x04 << 2) + 1) \n"
				"mcr	p15, 0, r0, c9, c1, 1 \n"
				"mov r0, #0x01 \n"
				"mcr	p15, 0, r0, c9, c1, 2 \n"
				"mcr	p15, 0, r0, c9, c1, 3 \n"
				);

		memcpy(0x00000000, ITCM_Boot, 0x1000);
		memcpy(0x00001000, InitRoutine_Start, 0x1000);
	}
#endif
	entry(0, machtype, tags);

}

unsigned page_size = 0;
unsigned page_mask = 0;

#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))
#define BYTE_TO_SECOTR(x) (x)/512

static unsigned char buf[16384]; //Equal to max-supported pagesize

static char serialno[21];	// max length of serial number is 20(CTS sepc)
static char wifimac[18];	// xx:xx:xx:xx:xx:xx
static char btaddr[13];		// Bluetooth device address (12 chars)
static char bootmode[12];
//static char memtype[2];		// memory type
static char cmdline[2048];

#define BOOTMODE tcc_factory

static void cmdline_bootmode(char *cmdline)
{
	char s[64];
	sprintf(s, " androidboot.bootmode=%s", bootmode);
	//sprintf(s, " androidboot.bootmode=tcc_factory");
	strcat(cmdline, s);
}

static void set_bootmode(char *cmdline)
{
	char s[11] = "tcc_factory";
	sprintf(bootmode, s);
}

static void cmdline_serialno(char *cmdline)
{
	char s[64];

	sprintf(s, " androidboot.serialno=%s", serialno);
	strcat(cmdline, s);
}

static void cmdline_wifimac(char *cmdline)
{
	char s[64];

	sprintf(s, " androidboot.wifimac=%s", wifimac);
	strcat(cmdline, s);
}

static void cmdline_btaddr(char *cmdline)
{
	char s[64];

	sprintf(s, " androidboot.btaddr=%s", btaddr);
	strcat(cmdline, s);
}

static void cmdline_memtype(char *cmdline)
{
	char s[64];
	char memtype[2] = {""};
#if defined(DRAM_DDR2)
	char temp = '1';
#elif defined(DRAM_DDR3)
	char temp = '2';
#elif ! defined(TCC88XX)
	char temp = '1';
#else
	char temp = '0';
#endif
	memtype[0] = temp;
	memtype[1] = '\0';

	sprintf(s, " androidboot.memtype=%s", memtype);
	strcat(cmdline, s);
}

#if defined(BOOTSD_INCLUDE)
int boot_linux_from_emmc(void)
{
	struct boot_img_hdr *hdr = (void*) buf;
	struct boot_img_hdr *uhdr;
	unsigned n;
	unsigned long long ptn;
	unsigned offset = 0;

	uhdr = (struct boot_img_hdr *)EMMC_BOOT_IMG_HEADER_ADDR;
	if(!memcmp(uhdr->magic , BOOT_MAGIC, BOOT_MAGIC_SIZE)){
		dprintf(INFO , "unified boot method!!\n");
		hdr = uhdr;
		goto unified_boot;
	}

	if(!boot_into_recovery){
		ptn = emmc_ptn_offset("boot");

		if(ptn == 0){
			dprintf(CRITICAL , "ERROR : No BOOT Partition Found!!\n");
			return -1;
		}
	}
	else{
		ptn = emmc_ptn_offset("recovery");

		if(ptn == 0){
			dprintf(CRITICAL , "ERROR : No Recvoery Partition Found!!\n");
			return -1;
		}
	}

	if(emmc_read((ptn + BYTE_TO_SECTOR(offset)), page_size, (unsigned int*)buf))
	{
		dprintf(CRITICAL , "ERROR : Cannot read boto imgage header\n");
		return -1;
	}

	if(memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)){
		dprintf(CRITICAL ,"ERROR: Invalid boot image header\n");
		return -1;
	}

	if (hdr->page_size && (hdr->page_size != page_size)) {
		page_size = hdr->page_size;
		page_mask = page_size - 1;
	}

	offset += page_size;
	n = ROUND_TO_PAGE(hdr->kernel_size, page_mask);

	if (emmc_read(ptn + BYTE_TO_SECTOR(offset),n, (void *)hdr->kernel_addr))
	{
		dprintf(CRITICAL, "ERROR: Cannot read kernel image\n");
		return -1;
	}

	n = ROUND_TO_PAGE(hdr->kernel_size, hdr->page_size-1); //base size is different from nand.
	offset += n;
	n = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);

	if (emmc_read(ptn + BYTE_TO_SECTOR(offset), n, (void *)hdr->ramdisk_addr))
	{
		dprintf(CRITICAL, "ERROR: Cannot read ramdisk image\n");
		return -1;
	}
	offset += n;


#if defined(_TCC9200_) || defined(_TCC9201_)
	/* SD/MMC power off for kernel mmc driver */
	sd_deinit();
#endif

unified_boot:
	dprintf(INFO, "\nkernel  @ %x (%d bytes)\n", hdr->kernel_addr,
			hdr->kernel_size);
	dprintf(INFO, "ramdisk @ %x (%d bytes)\n", hdr->ramdisk_addr,
			hdr->ramdisk_size);

	if(hdr->cmdline[0]) {
		strcpy(cmdline, (char*) hdr->cmdline);
	} else {
		strcpy(cmdline, DEFAULT_CMDLINE);
	}
	cmdline_serialno(cmdline);
	cmdline_wifimac(cmdline);
	cmdline_btaddr(cmdline);
	cmdline_memtype(cmdline);
	dprintf(SPEW, "cmdline = '%s'\n", cmdline);

	/* TODO: create/pass atags to kernel */

	dprintf(INFO, "\nBooting Linux\n");
	boot_linux((void *)hdr->kernel_addr, (void *)TAGS_ADDR,
			(const char *)cmdline, board_machtype(),
			(void *)hdr->ramdisk_addr, hdr->ramdisk_size);


	return 0;
}
#endif

#if !defined(TNFTL_V8_INCLUDE)
int boot_linux_from_flash(void)
{
#if defined(NAND_BOOT_INCLUDE)
	struct boot_img_hdr *hdr = (void*) buf;
	unsigned n;
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned offset = 0;
	struct fbcon_config *fb_display = NULL;
	char* data;

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}

#if defined(DISPLAY_SPLASH_SCREEN) && !defined(DISPLAY_SPLASH_SCREEN_DIRECT)
	ptn = ptable_find(ptable, "splash");
	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: No splash partition found\n");
	} else {
		fb_display = fbcon_display();
		if (fb_display) {
			if (flash_read(ptn, 0, fb_display->base,
						(fb_display->width * fb_display->height * (fb_display->bpp/8)))) {
				fbcon_clear();
				dprintf(CRITICAL, "ERROR: Cannot read splash image\n");
			}
		}
	}
#endif

	ptn = ptable_find(ptable, "tcc");
	if (ptn == NULL)
	{
		dprintf(CRITICAL, "ERROR: No tcc partition found\n");
	}
	else
	{
#if !defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
		data = (char*)buf;

		if (flash_read(ptn, 0, &data[0], 1024))
		{
			dprintf(CRITICAL, "ERROR: Cannot read data from tcc partition\n");
		}
		else
		{
			tcc_param.output = data[0];
			tcc_param.resolution = data[1];
		}
#endif
	}

	if(!boot_into_recovery)
	{
		ptn = ptable_find(ptable, "boot");
		if (ptn == NULL) {
			dprintf(CRITICAL, "ERROR: No boot partition found\n");
			return -1;
		}
	}
	else
	{
		ptn = ptable_find(ptable, "recovery");
		if (ptn == NULL) {
			dprintf(CRITICAL, "ERROR: No recovery partition found\n");
			return -1;
		}
	}

	if (flash_read(ptn, offset, buf, page_size)) {
		dprintf(CRITICAL, "ERROR: Cannot read boot image header\n");
		return -1;
	}
	offset += page_size;

	if (memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
		dprintf(CRITICAL, "ERROR: Invaled boot image heador\n");
		return -1;
	}

	if (hdr->page_size != page_size) {
		dprintf(CRITICAL, "ERROR: Invaled boot image pagesize. Device pagesize: %d, Image pagesize: %d\n",page_size,hdr->page_size);
		return -1;
	}

	n = ROUND_TO_PAGE(hdr->kernel_size, page_mask);
	if (flash_read(ptn, offset, (void *)hdr->kernel_addr, n)) {
		dprintf(CRITICAL, "ERROR: Cannot read kernel image\n");
		return -1;
	}
	offset += n;

	n = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);
	if (flash_read(ptn, offset, (void *)hdr->ramdisk_addr, n)) {
		dprintf(CRITICAL, "ERROR: Cannot read ramdisk image\n");
		return -1;
	}
	offset += n;

continue_boot:
	dprintf(INFO, "\nkernel  @ %x (%d bytes)\n", hdr->kernel_addr,
			hdr->kernel_size);
	dprintf(INFO, "ramdisk @ %x (%d bytes)\n", hdr->ramdisk_addr,
			hdr->ramdisk_size);

	if(hdr->cmdline[0]) {
		strcpy(cmdline, (char*) hdr->cmdline);
	} else {
		strcpy(cmdline, DEFAULT_CMDLINE);
	}
	cmdline_serialno(cmdline);
	cmdline_wifimac(cmdline);
	cmdline_btaddr(cmdline);
	cmdline_bootmode(cmdline);
	cmdline_memtype(cmdline);
	dprintf(SPEW, "cmdline = '%s'\n", cmdline);

	/* TODO: create/pass atags to kernel */

	dprintf(INFO, "\nBooting Linux\n");
	boot_linux((void *)hdr->kernel_addr, (void *)TAGS_ADDR,
			(const char *)cmdline, board_machtype(),
			(void *)hdr->ramdisk_addr, hdr->ramdisk_size);

#endif

	return 0;
}
#elif defined(TNFTL_V8_INCLUDE)
int boot_linux_from_flash_v8(void)
{
	struct boot_img_hdr *hdr = (void*) buf;
	struct boot_img_hdr *uhdr;
	unsigned n;
	unsigned long long ptn;
	unsigned offset = 0;

	uhdr = (struct boot_img_hdr *)EMMC_BOOT_IMG_HEADER_ADDR;
	if(!memcmp(uhdr->magic , BOOT_MAGIC, BOOT_MAGIC_SIZE)){
		dprintf(INFO , "unified boot method!!\n");
		hdr = uhdr;
		goto unified_boot;
	}

	if(!boot_into_recovery){
		ptn = flash_ptn_offset("boot");

		if(ptn == 0){
			dprintf(CRITICAL , "ERROR : No BOOT Partition Found!!\n");
			return -1;
		}
	}
	else{
		ptn = flash_ptn_offset("recovery");

		if(ptn == 0){
			dprintf(CRITICAL , "ERROR : No Recvoery Partition Found!!\n");
			return -1;
		}
	}

	if(flash_read_tnftl_v8((ptn + BYTE_TO_SECTOR(offset)), page_size, (unsigned int*)buf))
	{
		dprintf(CRITICAL , "ERROR : Cannot read boto imgage header\n");
		return -1;
	}

	if(memcmp(hdr->magic, BOOT_MAGIC, BOOT_MAGIC_SIZE)){
		dprintf(CRITICAL ,"ERROR: Invalid boot image header\n");
		return -1;
	}

	if (hdr->page_size && (hdr->page_size != page_size)) {
		page_size = hdr->page_size;
		page_mask = page_size - 1;
	}

	offset += page_size;
	n = ROUND_TO_PAGE(hdr->kernel_size, page_mask);

	if (flash_read_tnftl_v8(ptn + BYTE_TO_SECTOR(offset),n, (void *)hdr->kernel_addr))
	{
		dprintf(CRITICAL, "ERROR: Cannot read kernel image\n");
		return -1;
	}

	n = ROUND_TO_PAGE(hdr->kernel_size, hdr->page_size-1); //base size is different from nand.
	offset += n;
	n = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);

	if (flash_read_tnftl_v8(ptn + BYTE_TO_SECTOR(offset), n, (void *)hdr->ramdisk_addr))
	{
		dprintf(CRITICAL, "ERROR: Cannot read ramdisk image\n");
		return -1;
	}
	offset += n;

unified_boot:
	dprintf(INFO, "\nkernel  @ %x (%d bytes)\n", hdr->kernel_addr,
			hdr->kernel_size);
	dprintf(INFO, "ramdisk @ %x (%d bytes)\n", hdr->ramdisk_addr,
			hdr->ramdisk_size);

	if(hdr->cmdline[0]) {
		strcpy(cmdline, (char*) hdr->cmdline);
	} else {
		strcpy(cmdline, DEFAULT_CMDLINE);
	}
	cmdline_serialno(cmdline);
	cmdline_wifimac(cmdline);
	cmdline_btaddr(cmdline);
	cmdline_memtype(cmdline);
	dprintf(SPEW, "cmdline = '%s'\n", cmdline);

	/* TODO: create/pass atags to kernel */

	dprintf(INFO, "\nBooting Linux\n");
	boot_linux((void *)hdr->kernel_addr, (void *)TAGS_ADDR,
			(const char *)cmdline, board_machtype(),
			(void *)hdr->ramdisk_addr, hdr->ramdisk_size);


	return 0;
}

#endif

void cmd_boot(const char *arg, void *data, unsigned sz)
{
	unsigned kernel_actual;
	unsigned ramdisk_actual;
	static struct boot_img_hdr hdr;
	char *ptr = ((char*) data);

	if (sz < sizeof(hdr)) {
		fastboot_fail("invalid bootimage header");
		return;
	}

	memcpy(&hdr, data, sizeof(hdr));

	/* ensure commandline is terminated */
	hdr.cmdline[BOOT_ARGS_SIZE-1] = 0;

	kernel_actual = ROUND_TO_PAGE(hdr.kernel_size, page_mask);
	ramdisk_actual = ROUND_TO_PAGE(hdr.ramdisk_size, page_mask);

	if (page_size + kernel_actual + ramdisk_actual < sz) {
		fastboot_fail("incomplete bootimage");
		return;
	}

	memmove((void*) KERNEL_ADDR, ptr + page_size, hdr.kernel_size);
	memmove((void*) RAMDISK_ADDR, ptr + page_size + kernel_actual,
			hdr.ramdisk_size);

	fastboot_okay("");
	target_battery_charging_enable(0, 1);

#if defined(TCC_USB_USE)
	udc_stop();
#endif

	boot_linux((void*) KERNEL_ADDR, (void*) TAGS_ADDR,
			(const char*) hdr.cmdline, board_machtype(),
			(void*) RAMDISK_ADDR, hdr.ramdisk_size);
}
void cmd_erase_emmc(const char* arg , void *data, unsigned sz)
{
#if 0
	printf("cmd_erase_emmc, arg : %s\n", arg);
	unsigned long long ptn;
	unsigned char *erase_buffer = malloc(sizeof(char)*4*1024);
	memset(erase_buffer, 0x0 , sizeof(char)*4*1024);

	ptn = emmc_ptn_offset(arg);

	printf("[ cmd_erase_emmc ] ptn : %llu\n", ptn);

	if(!emmc_write(ptn , sizeof(char)*4*1024, erase_buffer)){
		fastboot_okay("");
		printf("complete\n");
	}
	else 
		fastboot_fail("emmcpartition erase fail");

#else
	int res = 0;
	unsigned long start_addr = 0;
	unsigned long erase_size = 0;

	printf("cmd_erase_emmc, arg : %s\n", arg);
	res = get_partition_info(arg, &start_addr, &erase_size);
	if( res ) {
		printf("[cmd_erase_emmc] get_partition_info() error!\n");
		return -1;
	}

	if( !erase_emmc(start_addr, erase_size, 0) ) {
		printf("Finished!\n");
		fastboot_okay("");
	} else {
		printf("Failed...\n");
		fastboot_fail("emmcpartition erase fail");
	}
#endif
}
void cmd_erase(const char *arg, void *data, unsigned sz)
{
	#if defined(NAND_BOOT_INCLUDE)
	#if !defined(TNFTL_V8_INCLUDE)
	struct ptentry *ptn;
	struct ptable *ptable;

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		fastboot_fail("partition table doesn't exist");
		return;
	}

	ptn = ptable_find(ptable, arg);
	if (ptn == NULL) {
		fastboot_fail("unknown partition name");
		return;
	}

	if (flash_erase(ptn)) {
		fastboot_fail("failed to erase partition");
		return;
	}
	fastboot_okay("");
	#elif defined(TNFTL_V8_INCLUDE)
	// temporal partition erase code.
	// set 0xFF Data at first 16Kbyte of buf.
	unsigned long long ptn;
	unsigned char* erase_buffer;

	erase_buffer = malloc(sizeof(char)*16*1024);
	memset(erase_buffer , 0xFF , 16*1024);

	ptn = flash_ptn_offset(arg);

	if(!flash_write_tnftl_v8(arg, ptn , 16*1024, erase_buffer)){
		fastboot_okay("");
	}
	else
		fastboot_fail("nand partition erase fail");

	dprintf(CRITICAL, " Input 0xFF Data to NAND Partition \n");

	return ;
	#endif
	#endif
}
#if defined(BOOTSD_INCLUDE)
void cmd_flash_emmc(const char *arg, void *data , unsigned sz)
{
	unsigned long long ptn = 0;
	sparse_header_t *sparse_header;

	ptn = emmc_ptn_offset(arg);
	dprintf(INFO ,"ptn start addr : %llu \n" , ptn);

	sparse_header = (sparse_header_t*)data;
	if(sparse_header->magic != SPARSE_HEADER_MAGIC){

		if(emmc_write(ptn, sz , data)){
			fastboot_fail("falsh write failure");
			return;
		}
	}
	else{
		cmd_flash_emmc_sparse_img(arg, data, sz);
	}


	fastboot_okay("");
	return;

}
void cmd_flash_emmc_sparse_img(const char *arg, void *data , unsigned sz)
{

	unsigned int chunk;
	unsigned int chunk_data_sz;
	sparse_header_t *sparse_header;
	chunk_header_t *chunk_header;
	uint32_t total_blocks = 0;
	unsigned long long ptn = 0;

	ptn = emmc_ptn_offset(arg);
	dprintf(INFO ,"ptn start addr : %llu \n" , ptn);

	sparse_header = (sparse_header_t *)data;
	data+=sparse_header->file_hdr_sz;
	if(sparse_header->file_hdr_sz > sizeof(sparse_header_t))
	{
		/* Skip the remaining bytes in a header that is long than we expected.*/
		data += (sparse_header->file_hdr_sz - sizeof(sparse_header_t));
	}

	dprintf (INFO, "=== Sparse Image Header ===\n");
	dprintf (INFO, "magic: 0x%x\n", sparse_header->magic);
	dprintf (INFO, "major_version: 0x%x\n", sparse_header->major_version);
	dprintf (INFO, "minor_version: 0x%x\n", sparse_header->minor_version);
	dprintf (INFO, "file_hdr_sz: %d\n", sparse_header->file_hdr_sz);
	dprintf (INFO, "chunk_hdr_sz: %d\n", sparse_header->chunk_hdr_sz);
	dprintf (INFO, "blk_sz: %d\n", sparse_header->blk_sz);
	dprintf (INFO, "total_blks: %d\n", sparse_header->total_blks);
	dprintf (INFO, "total_chunks: %d\n", sparse_header->total_chunks);

	/* start processing chunks*/
	for(chunk =0; chunk<sparse_header->total_chunks; chunk++)
	{
		/* Read and skip over chunk header*/
		chunk_header = (chunk_header_t*)data;
		data +=sizeof(chunk_header_t);
#if 0
		dprintf (INFO, "=== Chunk Header ===\n");
		dprintf (INFO, "chunk_type: 0x%x\n", chunk_header->chunk_type);
		dprintf (INFO, "chunk_data_sz: 0x%x\n", chunk_header->chunk_sz);
		dprintf (INFO, "total_size: 0x%x\n", chunk_header->total_sz);
#endif

		if(sparse_header->chunk_hdr_sz > sizeof(chunk_header_t))
		{
			/* Skip the remaining bytes in a header that is long than we expected.*/
			data+= (sparse_header->chunk_hdr_sz - sizeof(chunk_header_t));
		}

		chunk_data_sz = sparse_header->blk_sz * chunk_header->chunk_sz;
		switch(chunk_header->chunk_type)
		{
			case CHUNK_TYPE_RAW:
				if(chunk_header->total_sz != (sparse_header->chunk_hdr_sz + chunk_data_sz))
				{
					fastboot_fail("bogus chunk size for chunk type raw");
					return;
				}
				if(emmc_write(ptn+(total_blocks*sparse_header->blk_sz)/512,chunk_data_sz,data))
				{
					fastboot_fail("flash write failure");
					return;
				}
				total_blocks += chunk_header-> chunk_sz;
				data+=chunk_data_sz;
				break;
			case CHUNK_TYPE_DONT_CARE:
				total_blocks += chunk_header->chunk_sz;
				break;
			case CHUNK_TYPE_CRC:
				if(chunk_header->total_sz != sparse_header->chunk_hdr_sz)
				{
					fastboot_fail("bogus chunk size ofr chunk type dont care");
					return;
				}
				total_blocks += chunk_header->chunk_sz;
				data += chunk_data_sz;
				break;
			default:
				fastboot_fail("unknown chunk type");
				return;

		}

	}

	dprintf(INFO, "Wrote %d blocks, expected to write %d blocks\n",
			total_blocks, sparse_header->total_blks);

	fastboot_okay("");
	return;
}
#endif


void cmd_flash(const char *arg, void *data, unsigned sz)
{
#if defined(TCC_NAND_USE) && !defined(TNFTL_V8_INCLUDE)
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned extra = 0;
	struct flash_info *flash_info = flash_get_info();

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		fastboot_fail("partition table doesn't exist");
		return;
	}

	ptn = ptable_find(ptable, arg);
	if (ptn == NULL) {
		fastboot_fail("unknown partition name");
		return;
	}

	if (!strcmp(ptn->name, "boot") || !strcmp(ptn->name, "recovery")) {
		if (memcmp((void *)data, BOOT_MAGIC, BOOT_MAGIC_SIZE)) {
			fastboot_fail("image is not a boot image");
			return;
		}
	}

	if (!strcmp(ptn->name, "system") || !strcmp(ptn->name, "userdata"))
		extra = flash_info->spare_size;// 64;
	else
		sz = ROUND_TO_PAGE(sz, page_mask);

	dprintf(INFO, "writing %d bytes to '%s'\n", sz, ptn->name);
	if (flash_write(ptn, extra, data, sz)) {
		fastboot_fail("flash write failure");
		return;
	}
	dprintf(INFO, "partition '%s' updated\n", ptn->name);
	fastboot_okay("");

#elif defined(TNFTL_V8_INCLUDE)

	unsigned long long ptn = 0;
	sparse_header_t *sparse_header;

	ptn = flash_ptn_offset(arg);
	dprintf(INFO ,"ptn start addr : %llu \n" , ptn);

	sparse_header = (sparse_header_t*)data;
	if(sparse_header->magic != SPARSE_HEADER_MAGIC)
	{
		if(flash_write_tnftl_v8(arg, ptn, sz , data))
		{
			fastboot_fail("falsh write failure");
			return;
		}
	}
	else
	{
		cmd_flash_sparse_img(arg, data, sz);
	}

	fastboot_okay("");
	return;

#endif
}

void cmd_flash_sparse_img(const char *arg, void *data , unsigned sz)
{

	unsigned int chunk;
	unsigned int chunk_data_sz;
	sparse_header_t *sparse_header;
	chunk_header_t *chunk_header;
	uint32_t total_blocks = 0;
	unsigned long long ptn = 0;

	ptn = flash_ptn_offset(arg);
	dprintf(INFO ,"ptn start addr : %llu \n" , ptn);

	sparse_header = (sparse_header_t *)data;
	data+=sparse_header->file_hdr_sz;
	if(sparse_header->file_hdr_sz > sizeof(sparse_header_t))
	{
		/* Skip the remaining bytes in a header that is long than we expected.*/
		data += (sparse_header->file_hdr_sz - sizeof(sparse_header_t));
	}

	dprintf (INFO, "=== [TNFTL V8]Sparse Image Header ===\n");
	dprintf (INFO, "magic: 0x%x\n", sparse_header->magic);
	dprintf (INFO, "major_version: 0x%x\n", sparse_header->major_version);
	dprintf (INFO, "minor_version: 0x%x\n", sparse_header->minor_version);
	dprintf (INFO, "file_hdr_sz: %d\n", sparse_header->file_hdr_sz);
	dprintf (INFO, "chunk_hdr_sz: %d\n", sparse_header->chunk_hdr_sz);
	dprintf (INFO, "blk_sz: %d\n", sparse_header->blk_sz);
	dprintf (INFO, "total_blks: %d\n", sparse_header->total_blks);
	dprintf (INFO, "total_chunks: %d\n", sparse_header->total_chunks);

	/* start processing chunks*/
	for(chunk =0; chunk<sparse_header->total_chunks; chunk++)
	{
		/* Read and skip over chunk header*/
		chunk_header = (chunk_header_t*)data;
		data +=sizeof(chunk_header_t);
#if 0
		dprintf (INFO, "=== Chunk Header ===\n");
		dprintf (INFO, "chunk_type: 0x%x\n", chunk_header->chunk_type);
		dprintf (INFO, "chunk_data_sz: 0x%x\n", chunk_header->chunk_sz);
		dprintf (INFO, "total_size: 0x%x\n", chunk_header->total_sz);
#endif

		if(sparse_header->chunk_hdr_sz > sizeof(chunk_header_t))
		{
			/* Skip the remaining bytes in a header that is long than we expected.*/
			data+= (sparse_header->chunk_hdr_sz - sizeof(chunk_header_t));
		}

		chunk_data_sz = sparse_header->blk_sz * chunk_header->chunk_sz;
		switch(chunk_header->chunk_type)
		{
			case CHUNK_TYPE_RAW:
				if(chunk_header->total_sz != (sparse_header->chunk_hdr_sz + chunk_data_sz))
				{
					fastboot_fail("bogus chunk size for chunk type raw");
					return;
				}
				//if(emmc_write(ptn+(total_blocks*sparse_header->blk_sz)/512,chunk_data_sz,data))
				if( flash_write_tnftl_v8(arg, ptn+(total_blocks*sparse_header->blk_sz)/512, chunk_data_sz, data ))
				{
					fastboot_fail("flash write failure");
					return;
				}
				total_blocks += chunk_header-> chunk_sz;
				data+=chunk_data_sz;
				break;
			case CHUNK_TYPE_DONT_CARE:
				total_blocks += chunk_header->chunk_sz;
				break;
			case CHUNK_TYPE_CRC:
				if(chunk_header->total_sz != sparse_header->chunk_hdr_sz)
				{
					fastboot_fail("bogus chunk size ofr chunk type dont care");
					return;
				}
				total_blocks += chunk_header->chunk_sz;
				data += chunk_data_sz;
				break;
			default:
				fastboot_fail("unknown chunk type");
				return;

		}

	}

	dprintf(INFO, "Wrote %d blocks, expected to write %d blocks\n",
			total_blocks, sparse_header->total_blks);

	fastboot_okay("");
	return;
}



void cmd_continue(const char *arg, void *data, unsigned sz)
{
#if defined(NAND_BOOT_INCLUDE)
	fastboot_okay("");
	target_battery_charging_enable(0, 1);

#if defined(TCC_USB_USE)
	udc_stop();
#endif

#if defined(TNFTL_V8_INCLUDE)
	boot_linux_from_flash_v8();
#else
	boot_linux_from_flash();
#endif

#endif
}

void cmd_reboot(const char *arg, void *data, unsigned sz)
{
	dprintf(INFO, "rebooting the device\n");
	fastboot_okay("");
	reboot_device(0);
}

void cmd_reboot_bootloader(const char *arg, void *data, unsigned sz)
{
	dprintf(INFO, "rebooting the device\n");
	fastboot_okay("");
	reboot_device(FASTBOOT_MODE);
}

#if defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
static int get_tcc_param(void)
{
#if !defined(TNFTL_V8_INCLUDE)
	struct ptentry *ptn;
	struct ptable *ptable;
	#if !defined(BOOTSD_INCLUDE)
	struct flash_info *flash_info = flash_get_info();
	#endif
	char* data;
	int output_mode = 0xff, output_resolution = 0;
	int hdmi_resolution = 0;
	int hdmi_mode = 0;
	int composite_resolution = 0;
	int component_resolution = 0;
	int page = 0, page_offset = 0;

#if defined(BOOTSD_INCLUDE)
	tcc_param.output = 0;
	tcc_param.resolution = 0;
	tcc_param.hdmi_resolution = 0;
	tcc_param.composite_resolution = 0;
	tcc_param.component_resolution = 0;
#else
	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}

	ptn = ptable_find(ptable, "tcc");
	if (ptn == NULL)
	{
		dprintf(CRITICAL, "ERROR: No tcc partition found\n");
	}
	else
	{
		data = (char*)buf;

		for(page=0; page<flash_info->block_size; page++)
		{
			if (flash_read(ptn, page_offset, &data[0], 128))
			{
				dprintf(CRITICAL, "ERROR: Cannot read data from tcc partition\n");
				break;
			}
			else
			{
				//printf("page:%d, data[0]=0x%02x, data[1]=0x%02x, data[2]=0x%02x, data[3]=0x%02x, data[4]=0x%02x, data[5]=0x%02x, data[6]=0x%02x, data[7]=0x%02x, data[8]=0x%02x, data[9]=0x%02x, data[10]=0x%02x\n",
				//		page, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10]);

				if(data[0] == 0xff && data[1] == 0xff && data[2] == 0xff)
				{
					printf("[tcc_param] page=%d, output=%d, resolution=%d, hdmi_resolution=%d\n", page, output_mode, output_resolution, hdmi_resolution);
					printf("[tcc_param] composite_resolution=%d, component_resolution=%d, hdmi_mode=%d\n", composite_resolution, component_resolution, hdmi_mode);
					tcc_param.output = output_mode;
					tcc_param.resolution = output_resolution;
					tcc_param.hdmi_resolution = hdmi_resolution;
					tcc_param.composite_resolution = composite_resolution;
					tcc_param.component_resolution = component_resolution;
					tcc_param.hdmi_mode = hdmi_mode;
					break;
				}
				else
				{
					output_mode = data[3];
					output_resolution = data[4];
					hdmi_resolution	= data[5];
					composite_resolution = data[6];
					component_resolution = data[7];
					hdmi_mode = data[10];
				}
			}

			page_offset += page_size;
		}
	}
#endif

#endif
	return 0;
}
#endif

void aboot_init(const struct app_descriptor *app)
{
	unsigned reboot_mode = 0;
	unsigned disp_init = 0;
	unsigned char c = 0;

	board_get_serialno(serialno);
	board_get_wifimac(wifimac);
	board_get_btaddr(btaddr);

#if defined(TCC_NAND_USE)
	page_size = flash_page_size();
	page_mask = page_size - 1;
#endif

#if (defined(DISPLAY_SPLASH_SCREEN) || defined(DISPLAY_SPLASH_SCREEN_DIRECT)) && defined(TCC_LCD_USE)
#if defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
	get_tcc_param();
	display_init();
	dprintf(INFO, "Diplay initialized\n");
#endif

	disp_init = 1;
#endif

#if defined(PLATFORM_TCC) && defined(FWDN_V7) && defined(TCC_FWDN_USE)
	if (check_fwdn_mode() || keys_get_state(KEY_MENU) != 0) {
		fwdn_start();
		return;
	}
#endif

#ifdef BOOT_REMOCON
	if(getRemoconState() == 2)
	{
		if(isMagicKey())
			boot_into_recovery = 1;
	}
	else
#endif

		if(getc(&c) >= 0)
		{
			printf("==========================================\n");
			printf("getc: %c\n", c);

			if (c == 'r')
				boot_into_recovery = 1;
			if (c == 'f')
				goto fastboot;
		}
		else
		{
			boot_into_recovery = 0;
		}

	if(boot_into_recovery == 0)
	{
		if (keys_get_state(KEY_HOME) != 0)
			boot_into_recovery = 1;
		if( keys_get_state(KEY_F1) != 0 )
			boot_into_factory = 1;
		if (keys_get_state(KEY_BACK) != 0 || keys_get_state(KEY_VOLUMEUP) != 0 || keys_get_state(KEY_CLEAR) != 0)
			goto fastboot;
	}
#ifdef TCC_FACTORY_RESET_SUPPORT
	gpio_config(GPIO_PORTE|20, GPIO_FN0 | GPIO_INPUT); /* CVBS_DET */

	if(boot_into_recovery == 0)
	{
		if(gpio_get(GPIO_PORTE|20) == false)
		{
			boot_into_recovery = 1;
		}
	}
#endif

	if( boot_into_factory == 1 )
	{
		set_bootmode(bootmode);
	}

	reboot_mode = check_reboot_mode();
	if (reboot_mode == RECOVERY_MODE){
		boot_into_recovery = 1;
	}else if(reboot_mode == FASTBOOT_MODE){
		goto fastboot;
	}
#if defined(BOOTSD_INCLUDE)
	emmc_recovery_init();
	boot_linux_from_emmc();

#elif defined(TCC_NAND_USE)
#if defined(TNFTL_V8_INCLUDE)
	recovery_init_v8();
	boot_linux_from_flash_v8();
#else
	recovery_init();

	boot_linux_from_flash();
#endif
#endif
	dprintf(CRITICAL, "ERROR: Could not do normal boot. Reverting "
			"to fastboot mode.\n");

fastboot:
#if !defined(DEFAULT_DISPLAY_OUTPUT_DUAL)
	if(!disp_init) {
		display_init();
	} else {
		fbcon_clear();
	}
#endif

	dprintf(INFO, "Diplay initialized\n");

#if defined(TCC_USB_USE)
	udc_init(&fastboot_udc_device);

	fastboot_register("boot", cmd_boot);
	fastboot_register("erase:", cmd_erase);

#if defined(BOOTSD_INCLUDE)
	fastboot_register("flash:", cmd_flash_emmc);
	fastboot_register("erase:", cmd_erase_emmc);
#elif defined(TNFTL_V8_INCLUDE)
	fastboot_register("flash:", cmd_flash);
#else
	fastboot_register("flash:", cmd_flash);
#endif

	fastboot_register("continue", cmd_continue);
	fastboot_register("reboot", cmd_reboot);
	fastboot_register("reboot-bootloader", cmd_reboot_bootloader);

#ifdef _M801_
	fastboot_publish("product", "m57te_evm");
#else
	fastboot_publish("product", BOARD);
#endif

	fastboot_publish("kernel", "lk");
	fastboot_publish("serialno", serialno);

	fastboot_init(target_get_scratch_address(), 300 * 1024 * 1024);
	udc_start();
#endif
	target_battery_charging_enable(1, 0);
}

APP_START(aboot)
	.init = aboot_init,
	APP_END

