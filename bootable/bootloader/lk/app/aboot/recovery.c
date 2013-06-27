/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <debug.h>
#include <arch/arm.h>
#include <dev/udc.h>
#include <string.h>
#include <kernel/thread.h>
#include <arch/ops.h>

#include <dev/flash.h>
#include <lib/ptable.h>
#include <dev/keys.h>

#include "recovery.h"
#include "bootimg.h"


#define ROUND_TO_PAGE(x,y) (((x) +(y)) & (~(y)))
#define BYTES_TO_SECTOR(x)	((x + 511) >> 9)

static const int MISC_PAGES = 3;			// number of pages to save
static const int MISC_COMMAND_PAGE = 1;		// bootloader command is this page
static char buf[16384];
unsigned boot_into_recovery = 0;

void reboot_device(unsigned);

#if !defined(TNFTL_V8_INCLUDE)
int get_recovery_message(struct recovery_message *out)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned offset = 0;
	unsigned pagesize = flash_page_size();

	ptable = flash_get_ptable();

	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}
	ptn = ptable_find(ptable, "misc");

	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: No misc partition found\n");
		return -1;
	}

	offset += (pagesize * MISC_COMMAND_PAGE);
	if (flash_read(ptn, offset, buf, pagesize)) {
		dprintf(CRITICAL, "ERROR: Cannot read recovery_header\n");
		return -1;
	}
	memcpy(out, buf, sizeof(*out));
	return 0;
}

int set_recovery_message(const struct recovery_message *in)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned offset = 0;
	unsigned pagesize = flash_page_size();
	unsigned n = 0;

	ptable = flash_get_ptable();

	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}
	ptn = ptable_find(ptable, "misc");

	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: No misc partition found\n");
		return -1;
	}

	n = pagesize * (MISC_COMMAND_PAGE + 1);

	if (flash_read(ptn, offset, SCRATCH_ADDR, n)) {
		dprintf(CRITICAL, "ERROR: Cannot read recovery_header\n");
		return -1;
	}

	offset += (pagesize * MISC_COMMAND_PAGE);
	offset += SCRATCH_ADDR;
	memcpy(offset, in, sizeof(*in));
	if (flash_write(ptn, 0, (void *)SCRATCH_ADDR, n)) {
		dprintf(CRITICAL, "ERROR: flash write fail!\n");
		return -1;
	}
}

int read_update_header_for_bootloader(struct update_header *header)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned offset = 0;
	unsigned pagesize = flash_page_size();

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}
	ptn = ptable_find(ptable, "cache");

	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: No cache partition found\n");
		return -1;
	}
	if (flash_read(ptn, offset, buf, pagesize)) {
		dprintf(CRITICAL, "ERROR: Cannot read recovery_header\n");
		return -1;
	}
	memcpy(header, buf, sizeof(*header));

	if(strncmp(header->MAGIC, UPDATE_MAGIC, UPDATE_MAGIC_SIZE))
	{
		return -1;
	}
	return 0;
}

int update_firmware_image (struct update_header *header, char *name)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned offset = 0;
	unsigned pagesize = flash_page_size();
	unsigned pagemask = pagesize -1;
	unsigned n = 0;

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}

	ptn = ptable_find(ptable, "cache");
	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: No cache partition found\n");
		return -1;
	}

	offset += header->image_offset;
	n = (header->image_length + pagemask) & (~pagemask);

	if (flash_read(ptn, offset, SCRATCH_ADDR, n)) {
		dprintf(CRITICAL, "ERROR: Cannot read radio image\n");
		return -1;
	}

	if (!strcmp(name, "radio")) {
		ptn = ptable_find(ptable, name);
		if (ptn == NULL) {
			dprintf(CRITICAL, "ERROR: No %s partition found\n", name);
			return -1;
		}

		if (flash_write(ptn, 0, SCRATCH_ADDR, n)) {
			dprintf(CRITICAL, "ERROR: flash write fail!\n");
			return -1;
		}
	} else if (!strcmp(name, "bootloader")) {
#ifdef PLATFORM_TCC
		struct ptentry ptn = {
			.name = "bootloader",
		};

		flash_write(&ptn, 0, SCRATCH_ADDR, n);
#endif
	}


	dprintf(INFO, "Partition writen successfully!");
	return 0;
}

/* Bootloader / Recovery Flow
 *
 * On every boot, the bootloader will read the recovery_message
 * from flash and check the command field.  The bootloader should
 * deal with the command field not having a 0 terminator correctly
 * (so as to not crash if the block is invalid or corrupt).
 *
 * The bootloader will have to publish the partition that contains
 * the recovery_message to the linux kernel so it can update it.
 *
 * if command == "boot-recovery" -> boot recovery.img
 * else if command == "update-radio" -> update radio image (below)
 * else -> boot boot.img (normal boot)
 *
 * Radio Update Flow
 * 1. the bootloader will attempt to load and validate the header
 * 2. if the header is invalid, status="invalid-update", goto #8
 * 3. display the busy image on-screen
 * 4. if the update image is invalid, status="invalid-radio-image", goto #8
 * 5. attempt to update the firmware (depending on the command)
 * 6. if successful, status="okay", goto #8
 * 7. if failed, and the old image can still boot, status="failed-update"
 * 8. write the recovery_message, leaving the recovery field
 *    unchanged, updating status, and setting command to
 *    "boot-recovery"
 * 9. reboot
 *
 * The bootloader will not modify or erase the cache partition.
 * It is recovery's responsibility to clean up the mess afterwards.
 */

int recovery_init (void)
{
	struct recovery_message msg;
	struct update_header header;
	char partition_name[32];
	unsigned valid_command = 0;
	
	// get recovery message
	if(get_recovery_message(&msg))
		return -1;
	if (msg.command[0] != 0 && msg.command[0] != 255) {
		dprintf(INFO, "Recovery command: %s\n", msg.command);
	}
	msg.command[sizeof(msg.command)-1] = '\0'; //Ensure termination

	if (!strcmp("boot-recovery",msg.command)) {
		valid_command = 1;
		strcpy(msg.command, "");	// to safe against multiple reboot into recovery
		strcpy(msg.status, "OKAY");
		set_recovery_message(&msg);	// send recovery message
		boot_into_recovery = 1;		// Boot in recovery mode
		return 0;
	}

	if (!strcmp("update-radio",msg.command)) {
		valid_command = 1;
		strcpy(partition_name, "AMSS");
	} else if (!strcmp("update-bootloader", msg.command)) {
		valid_command = 1;
		strcpy(partition_name, "bootloader");
	}

	if(!valid_command) {
		//We need not to do anything
		return 0; // Boot in normal mode
	}

	if (read_update_header_for_bootloader(&header)) {
		strcpy(msg.status, "invalid-update");
		goto SEND_RECOVERY_MSG;
	}

	if (update_firmware_image (&header, partition_name)) {
		strcpy(msg.status, "failed-update");
		goto SEND_RECOVERY_MSG;
	}
	strcpy(msg.status, "OKAY");

SEND_RECOVERY_MSG:
	strcpy(msg.command, "boot-recovery");
	set_recovery_message(&msg);	// send recovery message
	boot_into_recovery = 1;		// Boot in recovery mode
	reboot_device(0);
	return 0;
}

#elif defined(TNFTL_V8_INCLUDE)
static int set_recovery_msg_v8(struct recovery_message *out)
{
	char *ptn_name = "misc";
	unsigned long long ptn = 0;
	unsigned int size = ROUND_TO_PAGE(sizeof(*out),511);
	unsigned char data[size];
	
	ptn = flash_ptn_offset(ptn_name);

	if(ptn == 0) {
		dprintf(CRITICAL,"partition %s doesn't exist\n",ptn_name);
		return -1;
	}
	
	memcpy(data, out, sizeof(*out));

	if (flash_write_tnftl_v8(ptn_name, ptn , size, (unsigned int*)data))
	{
		dprintf(CRITICAL,"write failure %s %d\n",ptn_name, sizeof(*out));
		return -1;
	}
	return 0;
}

static int get_recovery_msg_v8(struct recovery_message *in)
{
	char *ptn_name = "misc";
	unsigned long long ptn = 0;
	unsigned int size = ROUND_TO_PAGE(sizeof(*in),511);
	unsigned char data[size];

	ptn = flash_ptn_offset(ptn_name);

	if(ptn == 0) {
		dprintf(CRITICAL,"partition %s doesn't exist\n",ptn_name);
		return -1;
	}

	if (flash_read_tnftl_v8(ptn , size ,(unsigned int*)data))
	{
		dprintf(CRITICAL,"read failure %s %d\n",ptn_name, size);
		return -1;
	}
	
	memcpy(in, data, sizeof(*in));
	return 0;
}

int update_firmware_image_v8(struct update_header *header, char *name)
{
	unsigned offset = 0;
	unsigned pagesize = flash_page_size();
	unsigned pagemask = pagesize -1;
	unsigned n = 0;
	unsigned long long ptn = 0;
	unsigned ret;

	ptn = flash_ptn_offset("cache");
	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: No cache partition found\n");
		return -1;
	}

	offset += header->image_offset;
	n = (header->image_length + pagemask) & (~pagemask);

	if (flash_read_tnftl_v8(ptn, n+60, SCRATCH_ADDR))
	{
		dprintf(CRITICAL, "ERROR: Cannot read bootloader image\n");
		return -1;
	}

	memcpy(SCRATCH_ADDR,SCRATCH_ADDR+0x03c, n); 

	if (!strcmp(name, "bootloader")) 
	{
	 	dprintf(ALWAYS, "\n[NAND] Bootloader Update Start !!! \n");
	 	ret = flash_write_tnftl_v8( name, 0, n , SCRATCH_ADDR);
		if( ret != 0 )	
		{
			dprintf(ALWAYS, "\n[NAND] Bootloader Update Fail [ret:0x%08X]!!! \n", ret);		
			return ret;
		}
		else	
			dprintf(ALWAYS, "\n[NAND] Bootloader Update Success !!! \n");

	}
	dprintf(INFO, "Partition writen successfully!");
	return 0;
}


int read_update_header_for_bootloader_v8(struct update_header *header)
{
	char *ptn_name = "cache";
	unsigned long long ptn = 0;
	unsigned pagesize = 512;

	ptn = flash_ptn_offset("cache");
	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: No cache partition found\n");
		return -1;
	}

	if (flash_read_tnftl_v8(ptn, pagesize , buf))
	{
		dprintf(CRITICAL, "ERROR: Cannot read recovery_header\n");
		return -1;
	}
	memcpy(header, buf, sizeof(*header));

	if(strncmp(header->MAGIC, UPDATE_MAGIC, UPDATE_MAGIC_SIZE))
		return -1;
	return 0;
}

int recovery_init_v8(void)
{
	int update_status = 0;
	struct recovery_message msg;
	struct update_header header;
	unsigned valid_command  = 0;
	

	// get recovery message
	if(get_recovery_msg_v8(&msg))
		return -1;
	if (msg.command[0] != 0 && msg.command[0] != 255) {
		dprintf(INFO,"Recovery command:%s\n",msg.command);
	}
	msg.command[sizeof(msg.command)-1] = '\0'; //Ensure termination

	if (!strcmp("boot-recovery",msg.command)) {
		valid_command = 1;
		strcpy(msg.command, "");	// to safe against multiple reboot into recovery
		strcpy(msg.status, "OKAY");
		set_recovery_msg_v8(&msg);	// send recovery message
		boot_into_recovery = 1;		// Boot in recovery mode
		return 0;
	}

	if (!strcmp("update-bootloader",msg.command))
	{
		read_update_header_for_bootloader_v8(&header);

		if(update_firmware_image_v8(&header , "bootloader") == -1)
			return -1;
		
	}
	else
		return 0;	// do nothing
	
	strcpy(msg.command, "");	// clearing recovery command
	set_recovery_msg_v8(&msg);	// send recovery message
	boot_into_recovery = 1;		// Boot in recovery mode
	reboot_device(0);
	return 0;
}

#endif

static int emmc_set_recovery_msg(struct recovery_message *out)
{
	char *ptn_name = "misc";
	unsigned long long ptn = 0;
	unsigned int size = ROUND_TO_PAGE(sizeof(*out),511);
	unsigned char data[size];
	
	ptn = emmc_ptn_offset(ptn_name);

	if(ptn == 0) {
		dprintf(CRITICAL,"partition %s doesn't exist\n",ptn_name);
		return -1;
	}
	memcpy(data, out, sizeof(*out));
	if (emmc_write(ptn , size, (unsigned int*)data))
	{
		dprintf(CRITICAL,"write failure %s %d\n",ptn_name, sizeof(*out));
		return -1;
	}
	return 0;
}

static int emmc_get_recovery_msg(struct recovery_message *in)
{
	char *ptn_name = "misc";
	unsigned long long ptn = 0;
	unsigned int size = ROUND_TO_PAGE(sizeof(*in),511);
	unsigned char data[size];

	ptn = emmc_ptn_offset(ptn_name);
	if(ptn == 0) {
		dprintf(CRITICAL,"partition %s doesn't exist\n",ptn_name);
		return -1;
	}
	
	if (emmc_read(ptn , size ,(unsigned int*)data))
	{
		dprintf(CRITICAL,"read failure %s %d\n",ptn_name, size);
		return -1;
	}
	memcpy(in, data, sizeof(*in));
	return 0;
}

int update_firmware_emmc_image (struct update_header *header, char *name)
{
	unsigned offset = 0;
	unsigned pagesize = flash_page_size();
	unsigned pagemask = pagesize -1;
	unsigned n = 0;
	unsigned long long ptn = 0;
	unsigned ret;

	ptn = emmc_ptn_offset("cache");

	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: No cache partition found\n");
		return -1;
	}

	offset += header->image_offset;
	n = (header->image_length + pagemask) & (~pagemask);


	if (emmc_read(ptn, n+60, SCRATCH_ADDR))
	{
		dprintf(CRITICAL, "ERROR: Cannot read bootloader image\n");
		return -1;
	}

	memcpy(SCRATCH_ADDR,SCRATCH_ADDR+0x03c, n); 

	if (!strcmp(name, "bootloader")) 
		FwdnUpdateBootSDFirmware(n);

	dprintf(INFO, "Partition writen successfully!");
	return 0;
}


int read_update_header_for_emmc_bootloader(struct update_header *header)
{
	char *ptn_name = "cache";
	unsigned long long ptn = 0;
	unsigned pagesize = 512;

	ptn = emmc_ptn_offset("cache");

	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: No cache partition found\n");
		return -1;
	}

	if (emmc_read(ptn, pagesize , buf))
	{
		dprintf(CRITICAL, "ERROR: Cannot read recovery_header\n");
		return -1;
	}
	memcpy(header, buf, sizeof(*header));

	if(strncmp(header->MAGIC, UPDATE_MAGIC, UPDATE_MAGIC_SIZE))
		return -1;
	return 0;
}

int emmc_recovery_init(void)
{
	int update_status = 0;
	struct recovery_message msg;
	struct update_header header;
	unsigned valid_command  = 0;
	

	// get recovery message
	if(emmc_get_recovery_msg(&msg))
		return -1;
	if (msg.command[0] != 0 && msg.command[0] != 255) {
		dprintf(INFO,"Recovery command:%s\n",msg.command);
	}
	msg.command[sizeof(msg.command)-1] = '\0'; //Ensure termination

	if (!strcmp("boot-recovery",msg.command)) {
		valid_command = 1;
		strcpy(msg.command, "");	// to safe against multiple reboot into recovery
		strcpy(msg.status, "OKAY");
		emmc_set_recovery_msg(&msg);	// send recovery message
		boot_into_recovery = 1;		// Boot in recovery mode
		return 0;
	}

	if (!strcmp("update-bootloader",msg.command))
	{
		read_update_header_for_emmc_bootloader(&header);

		if(update_firmware_emmc_image(&header , "bootloader") == -1)
			return -1;
		
	}
	else
		return 0;	// do nothing
	
	strcpy(msg.command, "boot-recovery");	// clearing recovery command
	emmc_set_recovery_msg(&msg);	// send recovery message
	boot_into_recovery = 1;		// Boot in recovery mode
	reboot_device(0);
	return 0;
}
