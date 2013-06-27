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
#include <dev/keys.h>
#include <dev/gpio_keypad.h>
#include <lib/ptable.h>
#include <dev/flash.h>
#include <tnftl/tnftl_v7.h>
#include <platform/reg_physical.h>

#ifdef BOOTSD_INCLUDE
#include <fwdn/Disk.h>
#include <sdmmc/sd_memory.h>
#include <sdmmc/emmc.h>
#include <partition.h>
#endif

#define LINUX_MACHTYPE  4102

#define VARIABLE_LENGTH        0x10101010
#define DIFF_START_ADDR        0xF0F0F0F0

static struct ptable flash_ptable;

static struct ptentry board_part_list[] = {
	{
		.start = 0,
		.length = 0,
		.name = "bootloader",
		.flags = PART_HIDDEN,
	},
	{
		.start = 0,
		.length = 10, /* 10MB */
		.name = "boot",
	},
	{
		.start = 10,
		.length = 5, /* 5MB */
		.name = "kpanic",
	},
	{
		.start = 15,
		.length = 300, /* 200MB */
		.name = "system",
	},
	{
		.start = 315,
		.length = 4, /* 4MB */
		.name = "splash",
	},
	{
		.start = 319,
		.length = 150, /* 68MB, minimum free size of /cache is 55MB from Android 2.3 Compatibility Definition */
		.name = "cache",
	},
	{
		.start = 469,
		.length = VARIABLE_LENGTH,
		.name = "userdata",
	},
	{
		.start = DIFF_START_ADDR,
		.length = 10, /* 10MB */
		.name = "recovery",
	},
	{
		.start = DIFF_START_ADDR,
		.length = 1, /* 1MB */
		.name = "misc",
	},
        {
                .start = DIFF_START_ADDR,
                .length = 1, /* 1MB */
                .name = "tcc",
	}
};

	
static int num_parts = sizeof(board_part_list)/sizeof(struct ptentry);

void edi_init(void)
{

#ifdef TNFTL_V8_INCLUDE
	PEDI pEDI = (PEDI)HwEDI_BASE;

	BITCSET(pEDI->EDI_RDYCFG,       0x000FFFFF, 0x00011112 );
	BITCSET(pEDI->EDI_CSNCFG0,      0x00F00FFF, 0x00500876 );
#endif

}

void target_init(void)
{
#if defined(NAND_BOOT_INCLUDE)
	ASSERT(NAND_MTD_PARTITION_NUM == num_parts);
		
	unsigned	offset;	
	unsigned	total_num_of_blocks;
	unsigned	blocks_per_megabytes;
	unsigned	next_ptr_start_adr = 0;
	int		 	ret, i;

	struct 		flash_info *flash_info;
	bool  		start_addr_changed = false;
	
	unsigned long long			nMTDReserved_Num=0;			// total number of MTD Reserved Area
	TNFTL_MTDBadBlkInfo			MTDBadBlkInfo[num_parts];

	/////////////////////////////////////////////////////////////////////////////////////////////	
	unsigned long long	nROAreaSize, nPartitionSize = 0;
	unsigned long int	nBlockSize, nBlockSize_MB;
	unsigned int		nDevPBpV, nDevBBpZ, nDevBBpV, nRervRate;
	unsigned int		j, nUserDataArea = 0;
	struct ptable 		sPartition_List;
			

	memset( MTDBadBlkInfo, 0, sizeof(TNFTL_MTDBadBlkInfo) * num_parts );

	edi_init();

#endif
	dprintf(ALWAYS, "target_init()\n");

#if (!ENABLE_NANDWRITE)
#ifdef BOARD_TCC930X_STB_DEMO

#else
	keys_init();
	keypad_init();
#endif
#endif

#if defined(BOOTSD_INCLUDE)

	if (target_is_emmc_boot())
	{
		emmc_boot_main();
		return ;

	}
#endif

#if defined(NAND_BOOT_INCLUDE)
#if defined(TNFTL_V8_INCLUDE)
	if (check_fwdn_mode()) {
		fwdn_start();
		return;
	}

	flash_boot_main();

#else
if ( ( flash_get_ptable() == NULL )) {

		ptable_init(&flash_ptable);
		
		flash_set_partnum( num_parts );
		flash_init();
		flash_info = flash_get_info();
		ASSERT(flash_info);

		if ( (flash_info->num_blocks) && (!flash_check_table()) ) 
		{

			memcpy( sPartition_List.parts, board_part_list, sizeof( struct ptentry ) * num_parts );

			flash_get_DevPBpV( &nDevPBpV, &nDevBBpZ, &nMTDReserved_Num );

			nROAreaSize		= (unsigned long long)( nMTDReserved_Num << 20 );		
			nBlockSize		= flash_info->page_size << flash_info->ShiftPpB;			// Set Block Size ( Byte Size ) 
			//nROAreaSize		=	flash_info->num_blocks * nBlockSize;					// Set Total ROArea Size ( Byte Size )
			nBlockSize_MB	=	nBlockSize / ( 1 << 20 );								
			if( nBlockSize_MB > 1 )					// If Block is over the 1MB. Block must aligned. 
			{										// ex) Block Size 2MB, If Partition Size is 3MB. Partition Block Number must be 2. not 1.
			
				if( nBlockSize_MB == 2 )			// If Block Size 2MB.
				{
					for( i = 0; i < num_parts; i++ )
					{
						if( sPartition_List.parts[i].length & 0x01 )
							sPartition_List.parts[i].length++;
					}
				}
				else if ( nBlockSize_MB == 4 )		// If Block Size 4MB 
				{
					unsigned int nDiff_Val;
						
					for( i = 0; i < num_parts; i++ )
					{
						nDiff_Val = sPartition_List.parts[i].length & 0x03;
						if( nDiff_Val )
							sPartition_List.parts[i].length += ( 4 - nDiff_Val );
					}
				}
				
			}			

			nMTDReserved_Num 	= 	( nMTDReserved_Num << 20 ) / nBlockSize;

			nDevBBpV			=	( nDevPBpV / 1024 ) * nDevBBpZ;
			nRervRate			=	( nMTDReserved_Num * 100 ) / nDevPBpV;
			nDevBBpV			=	( nDevBBpV * nRervRate ) / 100;

			if( nRervRate != 0 )
				nRervRate			= 	( 100 / nRervRate );

			nMTDReserved_Num	=	nRervRate + nDevBBpV;								// Setup ROArea Reserved Block 

			if(nMTDReserved_Num == 0) 
				nMTDReserved_Num = 2;
			else if ( nMTDReserved_Num & 0x01 )
				nMTDReserved_Num++;

			if( flash_info->ExtInterrupt == TRUE )
				nMTDReserved_Num = nMTDReserved_Num << 1;

			for( i = 0; i < num_parts; i++ )
			{
				if( sPartition_List.parts[i].length != VARIABLE_LENGTH )
				{
					sPartition_List.parts[i].length  = (unsigned long long)(sPartition_List.parts[i].length << 20); 	// Convert Length Unit. MByte -> Byte	
					nPartitionSize					+= sPartition_List.parts[i].length;

					//ND_TRACE("sPartition_List.parts[%d].length:%lld [nPartitionSize:%d]\n", i, sPartition_List.parts[i].length, nPartitionSize);
				}
				else
				{
					nUserDataArea	=	i;
				}
			}

			if( nUserDataArea != 0 )
			{
				sPartition_List.parts[nUserDataArea].length	= nROAreaSize - nPartitionSize;			// Calculate UserDataArea Size ( include Rerv Block )
				//ND_TRACE("sPartition_List.parts[5].length:%lld [nPartitionSize:%d]\n", i, sPartition_List.parts[5].length, nPartitionSize);
				//sPartition_List.parts[nUserDataArea].length	-= (nMTDReserved_Num * nBlockSize ); 	// UserDataArea Size. Reverved Block Removed
			}	

			i = 1;
			sPartition_List.parts[0].length		/= nBlockSize;									// Partition 0 Length ( Block Unit )
			MTDBadBlkInfo[0].PartBlkNum			 =	sPartition_List.parts[0].length;			// Set Block Number Each Partition
			
			do
			{
				sPartition_List.parts[i].length /= nBlockSize;									// Partition i Length ( Block Unit )
				sPartition_List.parts[i].start	 = sPartition_List.parts[i-1].start + sPartition_List.parts[i-1].length;

				MTDBadBlkInfo[i].PartBlkNum		 = sPartition_List.parts[i].length;				// Set Block Number Each Partition

				++i;
				
			} while( i < num_parts );

			flash_set_rervnum( nMTDReserved_Num );				// Set Reserved Block Number
			flash_set_badblkinfo( MTDBadBlkInfo );				// Set Bad Block Table Info. About Block Number Each Partition
			

			for( i = 0; i < num_parts; i++ )
			{
				ptable_add(&flash_ptable, sPartition_List.parts[i].name, flash_info->offset + sPartition_List.parts[i].start,
						   				  sPartition_List.parts[i].length, sPartition_List.parts[i].flags);
			}

			ND_TRACE("\n-------------- [ Partition Table ] --------------\n");		          
			for( i = 0; i < num_parts; i++ )
			{
			ND_TRACE(" [Part %2d.%9s] [Start:%4d] [Length:%4d]\n", i, sPartition_List.parts[i].name ,sPartition_List.parts[i].start + flash_info->offset, sPartition_List.parts[i].length );	
			}
			ND_TRACE("-------------------------------------------------\n");
			
			dprintf(INFO, "[NAND        ] [Maker:0x%02x ][Device:0x%02x][Page_size:%d]\n",
				flash_info->vendor, flash_info->device, flash_info->page_size);
			dprintf(INFO, "               [Spare_Size:%d][Block_Size:%d][MTD_TotalBlock:%d]\n",
				flash_info->spare_size, flash_info->block_size, flash_info->num_blocks);
			//dprintf(INFO, "               [Spare_Size:%d][Block_Size:%d][MTD_Block:%d][Rerv_Block:%d]\n",
			//	flash_info->spare_size, flash_info->block_size, flash_info->num_blocks - (U32)nMTDReserved_Num, (U32)nMTDReserved_Num);
		
			//ptable_dump(&flash_ptable);
			flash_set_ptable(&flash_ptable);

			ret = flash_set_badblktable();

			if( ret != SUCCESS )
			{
				dprintf(INFO, " !!! Fail Create Bad Block Table. [func:%s] [line:%d] !!! \n", __func__, __LINE__ );
				ASSERT(-1);
			}
			
			flash_set_tablestatus(TRUE);
			
		}
	}
#endif
#endif
}

void target_reinit(int option)
{
#if defined(NAND_BOOT_INCLUDE)
#if defined(TNFTL_V8_INCLUDE)
	target_init();
#else
	unsigned	offset;
	struct		flash_info *flash_info;
	int 		ret, i;

	dprintf(INFO, "target_reinit()\n");

	if (flash_get_ptable() == NULL) {
		target_init();
		return;
	}
	
	flash_init_ptable();
	ptable_init(&flash_ptable);

	flash_init();
	flash_info = flash_get_info();
	ASSERT(flash_info);

	if (flash_info->num_blocks) 
	{
		offset = flash_info->offset;	// TODO
		//dprintf(INFO, "offset: %d\n", offset);
		
		for (i = 0; i < num_parts; i++) 
		{
			struct ptentry *ptn = &board_part_list[i];

			dprintf(INFO, "offset: %d ptn->start: %d\n", offset,  ptn->start );
			ptable_add(&flash_ptable, ptn->name, offset + ptn->start,
				   ptn->length, ptn->flags);
		}
		ptable_dump(&flash_ptable);
		flash_set_ptable(&flash_ptable);

		ret = flash_set_badblktable();
		ASSERT( ret == SUCCESS );
	}
#endif
#endif
}

unsigned board_machtype(void)
{
	return LINUX_MACHTYPE;
}

void reboot_device(unsigned reboot_reason)
{
	reboot(reboot_reason);
}

int board_get_serialno(char *serialno)
{
	int n,i;
	char temp[32];
#ifdef BOOTSD_INCLUDE
	if (target_is_emmc_boot())
		n = get_emmc_serial(temp);
	else
#endif
#if defined(NAND_BOOT_INCLUDE)
		n = NAND_GetSerialNumber(temp, 32);
#else
		return 0;
#endif
	for (i=0; i<4; i++)	// 4 = custon field(2) + product number(2)
		*serialno++ = temp[i];
	for (i=16; i<32; i++)	// 16 = time(12) + serial count(4)
		*serialno++ = temp[i];
	*serialno = '\0';
	return strlen(serialno);
}

int board_get_wifimac(char *wifimac)
{
	int n,i;
	char temp[32];
#ifdef BOOTSD_INCLUDE
	if (target_is_emmc_boot())
		n = get_emmc_serial(temp);
	else
#endif
#if defined(NAND_BOOT_INCLUDE)
		n = NAND_GetSerialNumber(temp, 32);
#else
		return 0;
#endif
	if (temp[1] == '1') {
		for (i=0; i<12; i++) {
			*wifimac++ = temp[4+i];
			if (i==11) break;
			if (!((i+1)%2)) *wifimac++ = ':';
		}
	} else if(temp[1] == '2') {
		for (i=0; i<12; i++) {
			*wifimac++ = temp[16+i];
			if (i==11) break;
			if (!((i+1)%2)) *wifimac++ = ':';
		}
	}
	*wifimac = '\0';
	return strlen(wifimac);
}

int board_get_btaddr(char *btaddr)
{
	int n,i;
	char temp[32];
#ifdef BOOTSD_INCLUDE
	if (target_is_emmc_boot())
		n = get_emmc_serial(temp);
	else
#endif
#if defined(NAND_BOOT_INCLUDE)
		n = NAND_GetSerialNumber(temp, 32);
#else
		return 0;
#endif
	for (i=4; i<16; i++)	// 12 = bluetooth bd address field(12)
		*btaddr++ = temp[i];
	*btaddr = '\0';
	return strlen(btaddr);
}
