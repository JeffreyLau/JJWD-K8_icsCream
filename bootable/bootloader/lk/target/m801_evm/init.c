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

#ifdef TRIFLASH_INCLUDE
#include <fwdn/Disk.h>
#include <fwdn/fwdn_drv_v3.h>
#include <sdmmc/mmc_ext.h>
#include <partition.h>
#endif

#define LINUX_MACHTYPE  4102

#define VARIABLE_LENGTH        0x10101010
#define DIFF_START_ADDR        0xF0F0F0F0

static struct ptable flash_ptable;

static struct ptentry board_part_list[] = {
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
		.length = 150, /* 150MB */
		.name = "system",
	},
	{
		.start = 165,
		.length = 4, /* 4MB */
		.name = "splash",
	},
	{
		.start = 169,
		.length = 40, /* 40MB */
		.name = "cache",
	},
	{
		.start = 209,
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

void target_init(void)
{

	ASSERT(NAND_MTD_PARTITION_NUM == num_parts);

		
	unsigned	offset;	
	unsigned	total_num_of_blocks;
	unsigned	blocks_per_megabytes;
	unsigned	next_ptr_start_adr = 0;
	int		 	ret, i;

	struct 		flash_info *flash_info;
	bool  		start_addr_changed = false;
	
	unsigned int				nMTDReserved_Num=0;			// total number of MTD Reserved Area
	TNFTL_MTDBadBlkInfo			MTDBadBlkInfo[num_parts];

	/////////////////////////////////////////////////////////////////////////////////////////////	
	unsigned int  		nROAreaSize, nPartitionSize = 0;
	unsigned int		nBlockSize, nBlockSize_MB;
	unsigned int		nDevPBpV, nDevBBpZ, nDevBBpV, nRervRate;
	unsigned int		j, nUserDataArea = 0;
	struct ptable 		sPartition_List;
			

	memset( MTDBadBlkInfo, 0, sizeof(TNFTL_MTDBadBlkInfo) * num_parts );
	dprintf(ALWAYS, "target_init()\n");
	
#if _EMMC_BOOT_TCC	
	PARTITION PartitionArr[50]; 	 
	unsigned int nPartitionCnt = 0; 
#endif		

#ifdef TRIFLASH_INCLUDE
		ioctl_diskinfo_t	disk_info;
#endif	


#if (!ENABLE_NANDWRITE)
#ifdef BOARD_TCC930X_STB_DEMO

#else
	keys_init();
	keypad_init();
#endif
#endif

	if (target_is_emmc_boot())
	{
#if _EMMC_BOOT_TCC	
		dprintf(INFO, "target_init() emmc_boot\n");
 
		ptable_init(&flash_ptable);

		//SDMMC init  //MCC
		DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_INITIALIZE, NULL );

		//get flash info?   //MCC
		DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_GET_DISKINFO, (void *)&disk_info);

		dprintf(INFO, "disk info: head: %d cylinder: %d sector : %d  sector size: %d Total_sectors: %d \n",disk_info.head,disk_info.cylinder,disk_info.sector,disk_info.sector_size,disk_info.Total_sectors);

		//ptabel init			//MCC
//		offset = flash_info->offset;	
		offset = 0;     //fixme

//		total_num_of_blocks = flash_info->num_blocks;
		total_num_of_blocks=10000000;    // fixme
																					 
		memset(&PartitionArr, 0, sizeof(PARTITION) * 50);												 
		nPartitionCnt = GetLocalPartition(0, PartitionArr); 											 
																								   
		for(i=0; i<nPartitionCnt; i++)																	 
			PrintPartitionInfo(&PartitionArr[i], i);		


		/* convert partition size to block unit */  //512byte ?
		blocks_per_megabytes = 1024*1024 / (disk_info.sector_size);		
		ASSERT(blocks_per_megabytes);
		
		for (i = 0; i < num_parts; i++) {
			struct ptentry *ptn = &board_part_list[i];
			if (ptn->length != VARIABLE_LENGTH)
				ptn->length *= blocks_per_megabytes;
		}
		
		for (i = 0; i < num_parts; i++) {
			struct ptentry *ptn = &board_part_list[i];
			unsigned len = ptn->length;
	
			if (ptn->start != DIFF_START_ADDR)
			{			
				if(i==2)
				{
					ptn->start = PartitionArr[1].start;
				}
				else
				{
					ptn->start *= blocks_per_megabytes;
				}
			}	
			
			if (len == VARIABLE_LENGTH) {
				start_addr_changed = true;
				unsigned length_for_prt = ptn->start;
				unsigned j;
				for (j = i+1; j < num_parts; j++)
				{
						struct ptentry *temp_ptn = &board_part_list[j];
						ASSERT(temp_ptn->length != VARIABLE_LENGTH);
						length_for_prt += temp_ptn->length;
				}
				len = total_num_of_blocks - length_for_prt;
				ASSERT(len >= 0);
				next_ptr_start_adr = ptn->start + len;
			}
			if((ptn->start == DIFF_START_ADDR) && (start_addr_changed)) {
				ASSERT(next_ptr_start_adr);
				ptn->start = next_ptr_start_adr;
				next_ptr_start_adr = ptn->start + ptn->length;
			}
			ptable_add(&flash_ptable, ptn->name, offset + ptn->start,
				   len, ptn->flags);
		}
		ptable_dump(&flash_ptable);
		flash_set_ptable(&flash_ptable);		
#endif
		return;
	}

	if (flash_get_ptable() == NULL) {

		ptable_init(&flash_ptable);
		
		flash_set_partnum( num_parts );
		flash_init();
		flash_info = flash_get_info();
		ASSERT(flash_info);

		if ( (flash_info->num_blocks) && (!flash_check_table()) ) 
		{
			

			memcpy( sPartition_List.parts, board_part_list, sizeof( struct ptentry ) * num_parts );

			nBlockSize		=	flash_info->page_size << flash_info->ShiftPpB;			// Set Block Size ( Byte Size ) 
			nROAreaSize		=	flash_info->num_blocks * nBlockSize;					// Set Total ROArea Size ( Byte Size )
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
				
			flash_get_DevPBpV( &nDevPBpV, &nDevBBpZ, &nMTDReserved_Num );

			nMTDReserved_Num 	= 	( nMTDReserved_Num << 20 ) / nBlockSize;

			nDevBBpV			=	( nDevPBpV / 1024 ) * nDevBBpZ;
			nRervRate			=	( nMTDReserved_Num * 100 ) / nDevPBpV;
			nDevBBpV			=	( nDevBBpV * nRervRate ) / 100;
			nRervRate			= 	( 100 / nRervRate );

			nMTDReserved_Num	=	nRervRate + nDevBBpV;								// Setup ROArea Reserved Block 

			if( nMTDReserved_Num & 0x01 )
				nMTDReserved_Num++;

			if( flash_info->ExtInterrupt == TRUE )
				nMTDReserved_Num = nMTDReserved_Num << 1;

			for( i = 0; i < num_parts; i++ )
			{
				if( sPartition_List.parts[i].length != VARIABLE_LENGTH )
				{
					sPartition_List.parts[i].length  = sPartition_List.parts[i].length << 20; 	// Convert Length Unit. MByte -> Byte	
					nPartitionSize					+= sPartition_List.parts[i].length;
				}
				else
				{
					nUserDataArea	=	i;
				}
			}
		
			sPartition_List.parts[nUserDataArea].length	= nROAreaSize - nPartitionSize;			// Calculate UserDataArea Size ( include Rerv Block )
			sPartition_List.parts[nUserDataArea].length	-= (nMTDReserved_Num * nBlockSize ); 	// UserDataArea Size. Reverved Block Removed

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
			dprintf(INFO, "               [Spare_Size:%d][Block_Size:%d][MTD_Block:%d][Rerv_Block:%d]\n",
				flash_info->spare_size, flash_info->block_size, flash_info->num_blocks - (U32)nMTDReserved_Num, (U32)nMTDReserved_Num);
		
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

}

void target_reinit(int option)
{
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
	n = NAND_GetSerialNumber(temp, 32);
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
        n = NAND_GetSerialNumber(temp, 32);
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
	n = NAND_GetSerialNumber(temp, 32);
	for (i=4; i<16; i++)	// 12 = bluetooth bd address field(12)
		*btaddr++ = temp[i];
	*btaddr = '\0';
	return strlen(btaddr);
}
