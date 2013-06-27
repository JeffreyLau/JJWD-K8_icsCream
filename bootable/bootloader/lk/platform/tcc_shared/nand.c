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

#if defined(NAND_BOOT_INCLUDE)
#if defined(TNFTL_V8_INCLUDE)
#include <debug.h>
#include <partition.h>

/* Test Code */
#define NAND_BOOT_MAGIC "ANDROID!"
#define NAND_BOOT_MAGIC_SIZE 8
#define NAND_BOOT_NAME_SIZE 16
#define NAND_BOOT_ARGS_SIZE 512
#define NAND_BOOT_IMG_HEADER_ADDR 0xFF000

typedef struct nand_boot_img_hdr nand_boot_img_hdr;

struct nand_boot_img_hdr
{
    unsigned char magic[NAND_BOOT_MAGIC_SIZE];

    unsigned kernel_size;  /* size in bytes */
    unsigned kernel_addr;  /* physical load addr */

    unsigned ramdisk_size; /* size in bytes */
    unsigned ramdisk_addr; /* physical load addr */

    unsigned second_size;  /* size in bytes */
    unsigned second_addr;  /* physical load addr */

    unsigned tags_addr;    /* physical addr for kernel tags */
    unsigned page_size;    /* flash page size we assume */
    unsigned unused[2];    /* future expansion: should be 0 */

    unsigned char name[NAND_BOOT_NAME_SIZE]; /* asciiz product name */
    
    unsigned char cmdline[NAND_BOOT_ARGS_SIZE];

    unsigned id[8]; /* timestamp / checksum / sha1 / etc */
};
/* Test Code */

#define BLOCK_SIZE                0x200
#define TABLE_ENTRY_0             0x1BE
#define TABLE_ENTRY_1             0x1CE
#define TABLE_ENTRY_2             0x1DE
#define TABLE_ENTRY_3             0x1EE
#define TABLE_SIGNATURE           0x1FE
#define TABLE_ENTRY_SIZE          0x010

#define OFFSET_STATUS             0x00
#define OFFSET_TYPE               0x04
#define OFFSET_FIRST_SEC 	  0x08
#define OFFSET_SIZE 		  0x0c

#define MAX_PARTITIONS 64

#define NATIVE_LINUX_PARTITION 0x83
#define VFAT_PARTITION 0xb
#define WIN_EXTENDED_PARTITION 0xf
#define LINUX_EXTENDED_PARTITION 0x5
#define RAW_EMPTY_PARTITION 0x0

#define GET_LWORD_FROM_BYTE(x) ((unsigned)*(x) | ((unsigned)*(x+1) << 8) | ((unsigned)*(x+2) <<16) | ((unsigned)*(x+3) << 24))

struct mbr_entry
{
	unsigned dstatus;
	unsigned dtype;
	unsigned dfirstsec;
	unsigned dsize;
	unsigned char name[64];
};
struct mbr_entry mbr[MAX_PARTITIONS];
static char *ext4_partitions[] = {"system" , "userdata", "cache"};
static char *raw_partitions[] = {"boot","recovery", "kpanic" , "splash" , "misc", "tcc" , "swap"};

unsigned nand_partition_count = 0;
unsigned int ext4_count = 0;
unsigned int raw_count = 0;

static void mbr_set_partition_name(struct mbr_entry* mbr_ent , unsigned int type)
{
	switch(type) {
		case VFAT_PARTITION:
			memcpy(mbr_ent->name , "data" , 4);
			break;
		case NATIVE_LINUX_PARTITION:
			if(ext4_count == sizeof(ext4_partitions) / sizeof(char*))
				return;
			strcpy((char*)mbr_ent->name , (const char*) ext4_partitions[ext4_count]);
			ext4_count++;
			break;
		case RAW_EMPTY_PARTITION:
			if(raw_count == sizeof(raw_partitions)/sizeof(char*))
				return;
			strcpy((char*)mbr_ent->name, (const char*)raw_partitions[raw_count]);
			raw_count++;
			break;
			
		default : 
			break;
	}
}

static unsigned int flash_boot_read_MBR()
{
	unsigned char buffer[512];
	unsigned int dtype;
	unsigned int dfirstsec;
	unsigned int EBR_first_sec;
	unsigned int EBR_current_sec;
	int ret = 0;
	int idx, i;

	/* Read Master Boot Record from eMMC */
	ret = NAND_ReadSector(0 , 0 , 1, buffer);

	if(ret)
	{
		dprintf(INFO , "Master Boot Record Read Fail on NAND\n");
		return ret;
	}

	/* check to see if signature exists */
	if((buffer[TABLE_SIGNATURE] != 0x55) || (buffer[TABLE_SIGNATURE+1] !=0xAA))
	{
		dprintf(CRITICAL , "MBR Signature does not match \n");
		return ret;
	}

	/* Search 4 Primary Partitions */
	idx = TABLE_ENTRY_0;
	for(i=0; i<4; i++)
	{
		mbr[nand_partition_count].dstatus = buffer[idx+i*TABLE_ENTRY_SIZE + OFFSET_STATUS];
		mbr[nand_partition_count].dtype = buffer[idx+i*TABLE_ENTRY_SIZE + OFFSET_TYPE];
		mbr[nand_partition_count].dfirstsec = GET_LWORD_FROM_BYTE(&buffer[idx+i*TABLE_ENTRY_SIZE + OFFSET_FIRST_SEC]);
		mbr[nand_partition_count].dsize = GET_LWORD_FROM_BYTE(&buffer[idx+i*TABLE_ENTRY_SIZE + OFFSET_SIZE]);
		dtype = mbr[nand_partition_count].dtype;
		dfirstsec = mbr[nand_partition_count].dfirstsec;
		mbr_set_partition_name(&mbr[nand_partition_count], mbr[nand_partition_count].dtype); 
		nand_partition_count++;
		if (nand_partition_count == MAX_PARTITIONS)
			return ret;
	}

	EBR_first_sec = dfirstsec;
	EBR_current_sec = dfirstsec;

	ret = NAND_ReadSector(0, EBR_first_sec , 1, buffer); 

	if(ret) return ret;

	/* Search for Extended partion Extended Boot Record*/
	for (i = 0;; i++)
	{

		/* check to see if signature exists */
		if((buffer[TABLE_SIGNATURE] != 0x55) || (buffer[TABLE_SIGNATURE+1] !=0xAA))
		{
			dprintf(CRITICAL , "Extended Partition MBR Signature does not match \n");
			break;
		}

		mbr[nand_partition_count].dstatus =  buffer[TABLE_ENTRY_0 + OFFSET_STATUS];
		mbr[nand_partition_count].dtype   = buffer[TABLE_ENTRY_0 + OFFSET_TYPE];
		mbr[nand_partition_count].dfirstsec = GET_LWORD_FROM_BYTE(&buffer[TABLE_ENTRY_0 + OFFSET_FIRST_SEC])+EBR_current_sec;
		mbr[nand_partition_count].dsize = GET_LWORD_FROM_BYTE(&buffer[TABLE_ENTRY_0 + OFFSET_SIZE]);
		mbr_set_partition_name(&mbr[nand_partition_count], mbr[nand_partition_count].dtype); 
		nand_partition_count++;
		if (nand_partition_count == MAX_PARTITIONS)
			return ret;

		dfirstsec = GET_LWORD_FROM_BYTE(&buffer[TABLE_ENTRY_1 + OFFSET_FIRST_SEC]);
		if(dfirstsec == 0)
		{
			/* Getting to the end of the EBR tables */
			break;
		}
		/* More EBR to follow - read in the next EBR sector */
		ret = NAND_ReadSector(0, EBR_first_sec+dfirstsec , 1 , buffer); 

		if (ret)
		{
			return ret;
		}
		EBR_current_sec = EBR_first_sec + dfirstsec;
	}

	return ret;
}

static void printpartition()
{
	unsigned idx;

	for(idx = 0; idx < nand_partition_count; idx++)
	{
		if(mbr[idx].dtype == LINUX_EXTENDED_PARTITION)
			continue;

		dprintf(INFO, "[PARTITION : %10s] [START : %10d] [SIZE : %10d] [TYPE : %2x]\n" , mbr[idx].name, mbr[idx].dfirstsec, mbr[idx].dsize ,mbr[idx].dtype);
	}
}

void flash_boot_main()
{
	unsigned int nPartitionCnt =0;
	PARTITION PartitionArr[50];

	dprintf(INFO , "%s:init start from NAND\n" , __func__ );
	if(NAND_Ioctl(0, 0) != 0)
		dprintf(INFO , "%s:init failure!!!\n" , __func__ );

	memset(PartitionArr , 0 , sizeof(PartitionArr));
	nPartitionCnt = GetLocalPartition(0 , PartitionArr);

	flash_boot_read_MBR();
	printpartition();
}

unsigned long long flash_ptn_offset(unsigned char* name)
{
	unsigned n;

	for(n=0; n<nand_partition_count; n++){
		if(!strcmp((const char*)mbr[n].name, (const char*)name)){
			return (mbr[n].dfirstsec);
		}
	}

	return 0;
}

unsigned int flash_read_tnftl_v8(unsigned long long data_addr , unsigned data_len , void* in)
{
	int val = 0;
	unsigned int pageCount = 0;

	pageCount = (data_len + 511) / 512;

	if(pageCount)
		val = NAND_ReadSector(0, (unsigned long)data_addr, pageCount, in);

	return val;
}

unsigned int flash_write_tnftl_v8(char *part_name, unsigned long long data_addr , unsigned data_len , void* in)
{
	int val = 0;
	if( !strcmp( part_name, "bootloader"))
	{
		unsigned int romsize;
		unsigned int *data;

		data = in;				
		romsize = data[7];
		val = NAND_WriteFirmware(0, (unsigned char*)in, romsize);
	} 
	else
	{	
		unsigned int pageCount = 0;

		pageCount = (data_len + 511) / 512;

		if(pageCount)
			val = NAND_WriteSector(0, (unsigned long)data_addr, pageCount, in);	
	}
	
	return val;	
}

#define BYTE_TO_SECTOR(X)			((X + 511) >> 9)
#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))

unsigned int flash_nand_dump( unsigned char *buf)
{
	struct nand_boot_img_hdr *hdr = (void*) buf;
	struct nand_boot_img_hdr *uhdr;
	unsigned char *databuf = buf;
	unsigned n;
	unsigned long long ptn;
	unsigned offset = 0;
	unsigned int page_mask = 0;
	unsigned int flash_pagesize = 8192;

	flash_boot_read_MBR();
	printpartition();
		
	uhdr = (struct nand_boot_img_hdr *)NAND_BOOT_IMG_HEADER_ADDR;
	if(!memcmp(uhdr->magic , NAND_BOOT_MAGIC, NAND_BOOT_MAGIC_SIZE)){
		dprintf(INFO , "unified boot method!!\n");
		hdr = uhdr;
		return 0;
	}

	ptn = flash_ptn_offset("boot");

	if(ptn == 0){
		dprintf(CRITICAL , "ERROR : No BOOT Partition Found!!\n");
		return -1;
	}
	
	dprintf(CRITICAL , "[ Start Read Boot Image Read]\n");
	if(flash_read_tnftl_v8((ptn + BYTE_TO_SECTOR(offset)), flash_pagesize, (unsigned int*)buf))
	{
		dprintf(CRITICAL , "ERROR : Cannot read boto imgage header\n");
		return -1;
	}

	if(memcmp(hdr->magic, NAND_BOOT_MAGIC, NAND_BOOT_MAGIC_SIZE)){
		dprintf(CRITICAL ,"ERROR: Invalid boot image header\n");
		return -1;
	}

	if (hdr->page_size && (hdr->page_size != flash_pagesize)) {
		flash_pagesize = hdr->page_size;
		page_mask = flash_pagesize - 1;
	}

	offset += flash_pagesize;
	n = ROUND_TO_PAGE(hdr->kernel_size, page_mask);
	databuf += offset;

	dprintf(CRITICAL , "[ Start Read Kernel Image Read ]\n");
	
	if (flash_read_tnftl_v8(ptn + BYTE_TO_SECTOR(offset),n, (void *)databuf))
	{
		dprintf(CRITICAL, "ERROR: Cannot read kernel image\n");
                return -1;
	}
	databuf += n;
	n = ROUND_TO_PAGE(hdr->kernel_size, hdr->page_size-1); //base size is different from nand.		
	offset += n;
	n = ROUND_TO_PAGE(hdr->ramdisk_size, page_mask);

	dprintf(CRITICAL , "[ Start Read Ramdisk Image Read ]\n");
	if (flash_read_tnftl_v8(ptn + BYTE_TO_SECTOR(offset), n, (void *)databuf))
	{
		dprintf(CRITICAL, "ERROR: Cannot read ramdisk image\n");
                return -1;
	}
	offset += n;
	dprintf(CRITICAL, "[ End Read Boot Image ] [size:%d]\n", offset );
	return offset;
}

#else

#include <debug.h>
#include <reg.h>
#include <stdlib.h>
#include <string.h>
#include <dev/flash.h>
#include <lib/ptable.h>
#include <platform/nand.h>

#include <tnftl/nand_drv.h>
#include <tnftl/tnftl_v7.h>
#include <fwdn/fwupgrade.h>

#include <fwdn/Disk.h>
#include <sdmmc/sd_memory.h>

#ifdef BOOTSD_INCLUDE
#include <partition.h>
#endif

#define VERBOSE 0
#define VERIFY_WRITE 0
#define MAX_SUPPORT_MULTI_NANDFLASH 2

static unsigned char *flash_spare;
static unsigned char *flash_data;

static unsigned char *flash_data_read;
static unsigned char *flash_spare_read;

static struct flash_info flash_info;
static unsigned flash_pagesize = 0;

static struct ptable *flash_ptable = NULL;

static NAND_IO_DEVINFO sDevInfo[MAX_SUPPORT_MULTI_NANDFLASH];
static unsigned rMTDStBlk, rMTDEdBlk;

BOOL					gbTableInit = FALSE;
unsigned char			gPartNum;
unsigned int			gDevice_PBpV;
unsigned int			gDevice_BBpZ;

//TNFTL_MTDBadBlkInfo		gMTDBadBlkInfo[NAND_MTD_PARTITION_NUM];
	
static int flash_erase_block(unsigned block)
{
	NAND_IO_ERROR res;	
	unsigned int nPageAddr, rPhyBlk, nChipNo;

	NAND_MTD_GetPhyBlkAddr( &sDevInfo, block, &rPhyBlk, &nChipNo );
	dprintf(INFO, "flash Erase Block: %d\n", rPhyBlk );

	nPageAddr = rPhyBlk << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;

	//res = NAND_IO_EraseBlock(&gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0], nPageAddr, INTER_LEAVE_OFF);
	res = NAND_IO_EraseBlock(&sDevInfo[nChipNo], nPageAddr, INTER_LEAVE_OFF);

	if (res != SUCCESS)
		dprintf(INFO, "cannot erase @ %d(phy:%d) (bad block?)\n", block, rPhyBlk);

	return (int)res;
}

static int flash_write_page(unsigned chipno, unsigned page, void *addr, void *spareaddr)
{
	unsigned char sparebuf[flash_info.spare_size];
	NAND_IO_ERROR res;

	sparebuf[0] = 0xFF;
	sparebuf[1] = 0xFF;
	memcpy(sparebuf + 2, spareaddr, flash_info.spare_size - 2);
	res = NAND_IO_WritePageMTD(&sDevInfo[chipno], page, 0, sDevInfo[chipno].PPages, addr, sparebuf, ECC_ON );

	memcpy( flash_spare, sparebuf, flash_info.spare_size );
	return res;
}

static int flash_write_pageErrorBit(unsigned page, void *addr, void *spareaddr)
{
	unsigned char sparebuf[flash_info.spare_size];
	NAND_IO_ERROR res;

	sparebuf[0] = 0xEE;
	sparebuf[1] = 0xEE;
	memcpy(sparebuf + 2, spareaddr, flash_info.spare_size - 2);
	res = NAND_IO_WritePageMTD(&sDevInfo, page, 0, sDevInfo[0].PPages, addr, sparebuf, ECC_ON );
//	res = NAND_IO_WritePageMTD(&sDevInfo, page, 0, sDevInfo.PPages, addr, apareaddr, ECC_ON );
	return res;
}

static int flash_read_page(unsigned chipno, unsigned page, void *addr, void *spareaddr)
{
	NAND_IO_ERROR res;

	res = NAND_IO_ReadPageMTD( &sDevInfo[chipno], page, 0, sDevInfo[chipno].PPages, addr, spareaddr, ECC_ON );		
	
	return res;
}

int flash_set_badblktable()
{
	return NAND_MTD_SetBadBlkTable( &sDevInfo );
}

void flash_set_badblkinfo( TNFTL_MTDBadBlkInfo *MTD_BadBlkInfo )
{
	NAND_MTD_SetBadblkInfo( MTD_BadBlkInfo );	
}

void flash_set_partnum( int nPartNum )
{
	gPartNum			=	nPartNum;
	NAND_MTD_SetPartNum( nPartNum );
}

void flash_set_rervnum( int nRervNum )
{
	NAND_MTD_SetRervNum( nRervNum );
}

int flash_get_partnum()
{
	return gPartNum;
}

void flash_set_tablestatus( BOOL status )
{
	gbTableInit = status;
}

BOOL flash_check_table()
{
	return gbTableInit;
}

void flash_get_DevPBpV( unsigned int *nDev_PBpV, unsigned int *nDev_BBpZ, unsigned long long *RoAreaSize )
{
	*nDev_PBpV	=	gDevice_PBpV;
	*nDev_BBpZ	=	gDevice_BBpZ;
	*RoAreaSize = 	NAND_GetROAreaSize();
}

void flash_init(void)
{	
	int res;

	ASSERT(flash_ptable == NULL);

	NAND_Ioctl(0, 0);
	res = NAND_MTD_Init(&sDevInfo, &rMTDStBlk, &rMTDEdBlk, 0);

	if( res != SUCCESS )
	{
		ND_TRACE(" !!! NAND MTD Init Fail !!!!\n");
		ASSERT( res == SUCCESS );
	}
	flash_info.offset	=	rMTDStBlk;
	flash_info.ShiftPpB	=	sDevInfo[0].ShiftPpB;

	flash_info.vendor 	=	sDevInfo[0].Feature.DeviceID.Code[0];
	flash_info.device 	=	sDevInfo[0].Feature.DeviceID.Code[1];

	flash_pagesize		=	sDevInfo[0].Feature.PageSize;
	gDevice_PBpV		=	sDevInfo[0].Feature.PBpV;
	gDevice_BBpZ		=	sDevInfo[0].Feature.BBpZ;
	

	flash_info.page_size	= flash_pagesize;
	flash_info.block_size	= sDevInfo[0].Feature.PpB;
	
	/* XXX: determine spare size using page size */
	if (flash_info.page_size == 4096)
		flash_info.spare_size = 128;
	else if (flash_info.page_size == 8192)
		flash_info.spare_size = 256;
	else if (flash_info.page_size == 16384)
		flash_info.spare_size = 512;
	else
		flash_info.spare_size = 64;
	
	//flash_info.spare_size = sDevInfo.Feature.SpareSize;
	if ( ( sDevInfo[0].Feature.MediaType & S_IL ) || ( sDevInfo[0].ExtInterleaveUsable == TRUE) )
		flash_info.num_blocks = ((rMTDEdBlk-rMTDStBlk) << 1);
	else
		flash_info.num_blocks = rMTDEdBlk-rMTDStBlk;

	if ( sDevInfo[0].ExtInterleaveUsable == TRUE )
		flash_info.ExtInterrupt	= TRUE;
	else
		flash_info.ExtInterrupt = FALSE;

	flash_data			= (unsigned char*)dma_alloc(32, sDevInfo[0].Feature.PageSize + flash_info.spare_size );
	flash_spare 		= (unsigned char*)dma_alloc(32, flash_info.spare_size);

	flash_data_read		= (unsigned char*)dma_alloc(32, sDevInfo[0].Feature.PageSize+ flash_info.spare_size);
	flash_spare_read	= (unsigned char*)dma_alloc(32, flash_info.spare_size);

	return ;
}

struct ptable *flash_get_ptable(void)
{
	return flash_ptable;
}

void flash_init_ptable(void)
{
	flash_ptable = NULL;
}

void flash_set_ptable(struct ptable *new_ptable)
{
	ASSERT(flash_ptable == NULL && new_ptable != NULL);
	flash_ptable = new_ptable;
}

struct flash_info *flash_get_info(void)
{
	return &flash_info;
}

int flash_erase(struct ptentry *ptn)
{
	unsigned block = ptn->start;
	unsigned count = ptn->length;

	while (count-- > 0) {
		flash_erase_block(block);
		block++;
	}
	return 0;
}

int flash_read_ext(struct ptentry *ptn, unsigned extra_per_page,
			unsigned offset, void *data, unsigned bytes)
{
	unsigned block_size = flash_info.block_size;
	unsigned curr_lblock = ptn->start;
	unsigned curr_pblock;
	unsigned block_offset = offset/(block_size*flash_info.page_size);
	unsigned page_offset = (offset/flash_info.page_size)%block_size;
	unsigned page = 0;
//	unsigned lastpage = (ptn->start + ptn->length) * flash_info.block_size;
	unsigned count = (bytes + flash_pagesize - 1 + extra_per_page) / (flash_pagesize + extra_per_page);
	unsigned char *spare = (unsigned *) flash_spare;
	unsigned errors = 0;
	unsigned char *image = data;
	unsigned char cBadBlkNum = 0, cAccBadBlkNum, nPartNum;
	unsigned n, nChipNo;
	int result;

	nPartNum		= ptable_GetPartNum( flash_ptable, ptn );
	cAccBadBlkNum	= NAND_MTD_GetAccBadBlkNum( nPartNum );

	curr_lblock 	+= cAccBadBlkNum;

	NAND_IO_ERROR res;

	if (offset & (flash_pagesize - 1))
		return -1;

	/* Adjust page offset based on number of bad blocks from start to current page */
	/* TODO */
	//dprintf(INFO, "flash_read: name:%s offset:%d size:%d spare:%d \n", ptn->name, offset, bytes, extra_per_page);
	//dprintf(INFO, "            block_offset:%d page_offset:%d \n", block_offset, page_offset);
	//dprintf(INFO, "            offset:%d block_size:%d flash_info.page_size:%d \n", offset, block_size, flash_info.page_size);	

	while (count > 0)
	{

		for ( n = curr_lblock; n < (ptn->start + ptn->length + cBadBlkNum + cAccBadBlkNum) ; ++n ) 
		{
			NAND_MTD_GetPhyBlkAddr( &sDevInfo, n, &curr_pblock, &nChipNo );
			page = curr_pblock << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;

			//dprintf(INFO,"[PartName:%10s] [P-Block:%3d] [Loop:%d]\n", ptn->name, curr_pblock, l );		
			res = NAND_GetGoldenBadMarkofPBlock( nChipNo, curr_pblock );

			if( res != SUCCESS )			
			{
				++cBadBlkNum;
				dprintf(INFO, "##### %d(phy:%d) is bad block #####\n", n, curr_pblock);	
			}
			else
			{
				curr_lblock = n + 1;
				if( block_offset )
				{
					block_offset--;
					continue;
				}
				else
					break;
			}		
			
		}	

			if( res >= 0 )
			{
				for ( n = 0; n < block_size; ++n )
				{
					if (page_offset) {
						page_offset--;
						continue;
					}
				
					if (count == 0) {
						//dprintf(INFO, "flash_read_image: success (%d errors)\n", errors);
						return 0;
					}

					result = flash_read_page(nChipNo, page + n, image, spare);
					if (result < 0) {
						/* bad page, go to next page */
						//page++;
						errors++;
						continue;
					}
					image += flash_pagesize;
					image += extra_per_page;
					count -= 1;
				}
			}
		}
	
	if (count == 0) 
	{
		//dprintf(INFO, "flash_read_image: success (%d errors)\n", errors);
		return 0;
	}

	/* could not find enough valid pages before we hit the end */
	dprintf(INFO, "flash_read_image: failed (%d errors)\n", errors);
	return 0xffffffff;
}

int flash_write(struct ptentry *ptn, unsigned extra_per_page, const void *data,
		unsigned bytes)
{
	unsigned block_size = flash_info.block_size;
	unsigned curr_lblock = ptn->start;
	unsigned curr_pblock;
	unsigned page = 0;
//	unsigned lastpage = (ptn->start + ptn->length) * block_size;
	unsigned *spare = (unsigned *) flash_spare;
	const unsigned char *image = data;
	unsigned wsize = flash_info.page_size + extra_per_page;
	unsigned n, nChipNo;
	NAND_IO_ERROR res;

	for (n = 0; n <  flash_info.spare_size/sizeof(unsigned); n++)
		spare[n] = 0xffffffff;

//	dprintf(INFO, "flash_write: name:%s size:%d spare:%d \n", ptn->name, bytes, extra_per_page);

	/* bootloader does not exist in MTD partition */
	if (strcmp(ptn->name, "bootloader") == 0) {
		unsigned int nStBlockOffset, nStPageOffset;
		unsigned int dwBlockOffset, dwPageOffset;
		unsigned int nSecureMode;
		unsigned char GMC_Num;
		unsigned char bigger;

		#if 0
#ifdef TNFTL_V7_INCLUDE
		FWUG_NAND_SetFlagCopyExistingRom( DISABLE );
		FWUG_NAND_SetEnableNandBootOnlyMode( ENABLE );
		FWUG_NAND_SetNBAreaEndPBAdd(TNFTL_MAX_BLOCK_NUM_OF_NBAREA);		// Physical Block Num
#endif
		nSecureMode = FWUG_NAND_GetFlagOfUseSecureMode();

		if (FWUG_NAND_PreProcess(bytes, &bigger) != SUCCESS)
			return -1;

		for (n = 0; n < 2; n++) {
			if (bigger == TRUE)
				GMC_Num = (n == 0) ? FIRST : SECOND;
			else
				GMC_Num = (n == 0) ? SECOND : FIRST;

			res = FWUG_NAND_WriteCodePreProcess(GMC_Num, bytes,
							    &nStBlockOffset, &nStPageOffset, nSecureMode);
			if (res != SUCCESS)
				return -1;
#ifdef TNFTL_V7_INCLUDE
			res = FWUG_NAND_WriteCodeNAND(GMC_Num, nStBlockOffset, nStPageOffset, data, bytes,
						      &dwBlockOffset, &dwPageOffset, nSecureMode);
#else
			res = FWUG_NAND_WriteCodeNAND(nStBlockOffset, nStPageOffset, data, bytes,
						      &dwBlockOffset, &dwPageOffset, nSecureMode);
#endif
			if (res != SUCCESS)
				return -1;
			FWUG_NAND_WriteCodePostProcess(GMC_Num, nStBlockOffset, nStPageOffset);
			
		}
		#else
		FWUG_MainFunc( data, bytes );
		#endif
	} 
	else 
	{

		unsigned char	cPartNum, cAccBadBlkNum, cBadBlkNum = 0;

		cPartNum		=	ptable_GetPartNum( flash_get_ptable(), ptn );
		cAccBadBlkNum	=	NAND_MTD_GetAccBadBlkNum( cPartNum );

		curr_lblock 	+= cAccBadBlkNum;

		NAND_MTD_GetPhyBlkAddr( &sDevInfo, curr_lblock, &curr_pblock, &nChipNo );
		dprintf(INFO, "\n[FASTBOOT Write Func] [PartName:%8s] [StBlk:%3d] [ChipNo:%d]\n"
				 	,	ptn->name, curr_pblock + cAccBadBlkNum, nChipNo );

		while (bytes > 0)
		{
			for ( n = curr_lblock ; n < (ptn->start + ptn->length + cBadBlkNum + cAccBadBlkNum ) ; ++n ) 
			{
				
				NAND_MTD_GetPhyBlkAddr( &sDevInfo, n, &curr_pblock, &nChipNo );
				res = NAND_GetGoldenBadMarkofPBlock( nChipNo, curr_pblock );

				if( res == SUCCESS ) 
				{
					page = curr_pblock << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;
					//NAND_IO_EraseBlock( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0], page, INTER_LEAVE_OFF );
					NAND_IO_EraseBlock( &sDevInfo[nChipNo], page, INTER_LEAVE_OFF );
					curr_lblock = n + 1;
                    break;
				}
				else
				{
					dprintf(INFO, "[FASTBOOT Write Func] [PartName:%8s] [BadBlk:%3d] \n", ptn->name, curr_pblock );
					++cBadBlkNum;
					continue;
				}
			}

			if( res >= 0 )
			{				
				for ( n = 0; n < block_size; ++n )
				{
					if (bytes < wsize) 
					{
						if (bytes == 0)
							break;

						if (extra_per_page)
							dprintf(CRITICAL, "flash_write_image: image undersized (%d < %d)\n", bytes, wsize);
						else 
						{
							memset(flash_data, 0xFF, flash_pagesize + flash_info.spare_size);
							memcpy(flash_data, image, bytes);
							res = flash_write_page(nChipNo, page + n, flash_data, spare);
							if( res != SUCCESS )
							{
								dprintf("[MTD_Write_Fail] [BlkAddr:%4d]\n", curr_pblock );
								return res;
							}
						}
						bytes = 0;
						break;
					}
					if (extra_per_page) 
					{
						res = flash_write_page(nChipNo,page + n, image, image + flash_pagesize);
						if( res != SUCCESS )
						{
							dprintf("[MTD_Write_Fail] [BlkAddr:%4d]\n", curr_pblock );
							return res;
						}						
						
					} 
					else 
					{
						res = flash_write_page(nChipNo,page + n, image, spare);
						if( res != SUCCESS )
						{
							dprintf("[MTD_Write_Fail] [BlkAddr:%4d]\n", curr_pblock );
							return res;
						}
							
					}
					image += wsize;
					bytes -= wsize;
				}
			}
		}
		

		/* erase any remaining pages in the partition */
		for ( n = curr_lblock ; n < (ptn->start + ptn->length + cAccBadBlkNum + cBadBlkNum ) ; ++n ) 
		{
			
			NAND_MTD_GetPhyBlkAddr( &sDevInfo, n, &curr_pblock, &nChipNo );
			res = NAND_GetGoldenBadMarkofPBlock( nChipNo, curr_pblock );

			if( res == SUCCESS )
			{
				page = curr_pblock << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;
				//NAND_IO_EraseBlock( &gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0], page, INTER_LEAVE_OFF );
				NAND_IO_EraseBlock( &sDevInfo[nChipNo], page, INTER_LEAVE_OFF );
				//break;
			}
			else
			{
				dprintf(INFO, "[FASTBOOT Write Func] [PartName:%8s] [BadBlk:%3d] [ChipNo:%d]\n", ptn->name, curr_pblock, nChipNo );
				++cBadBlkNum;
				continue;
			}
			
		}
	}		
	
	dprintf(INFO, "[FASTBOOT Write Func] [PartName:%8s] [EndBlk:%3d] [TotalBlk:%3d]\n", ptn->name, curr_pblock, ptn->length );
	//dprintf(INFO, "[FASTBOOT Write Func] [flash_write_image:success]\n");
	return 0;
}

#include <fwdn/fwdn_drv_v7.h>


#define NAND_CMP 0

int flash_write_by_vtc(struct ptentry *ptn, unsigned extra_per_page, unsigned long long bytes
					,FXN_FWDN_DRV_Response_RequestData fxnFwdnDrvResponseRequestData
					,FXN_FWDN_DRV_DirectReadFromHost fxnFwdnDrvDirectReadFromHost
					,unsigned nStartOffSet, unsigned nExpectBadNum)
{
	unsigned block_size = flash_info.block_size;
	unsigned curr_lblock = ptn->start + nStartOffSet;
	unsigned curr_pblock;
	unsigned page = 0;
	//unsigned repage = 0;
	unsigned *spare = (unsigned *) flash_spare;
	unsigned char *readBuf;
	unsigned wsize = flash_info.page_size + extra_per_page;
	unsigned backwardBlock = 0;
	unsigned n, nChipNo, retval;

	NAND_IO_ERROR res;

	for (n = 0; n <  flash_info.spare_size/sizeof(unsigned); n++)
		spare[n] = 0xffffffff;

	dprintf(INFO, "flash_write_by_vtc: name:%s size:%d spare:%d flashpage=%d\n", ptn->name, bytes, extra_per_page, flash_pagesize);
	dprintf(INFO, "flash_write_by_vtc: pagesize:%d, blocksize:%d\n", flash_info.page_size, flash_info.block_size );

	NAND_MTD_GetPhyBlkAddr( &sDevInfo, curr_lblock, &curr_pblock, &nChipNo );
	dprintf(ALWAYS, "flash_write_by_vtc: [PartName:%8s] [StBlk :%3d] \n", ptn->name, curr_pblock );

	while (bytes > 0)
	{
		res = ERR_NAND_IO_NOT_READY_DEVICE_IO;

		for ( n = curr_lblock ; n < (ptn->start + ptn->length + nStartOffSet + backwardBlock) ; ++n ) 
		{
			NAND_MTD_GetPhyBlkAddr( &sDevInfo, n, &curr_pblock, &nChipNo );
			//dprintf(INFO,"[PartName:%10s] [P-Block:%4d] [byte:%d]\n", ptn->name, curr_pblock, bytes );
			res = NAND_GetGoldenBadMarkofPBlock( nChipNo, curr_pblock );
			
			if ( res == SUCCESS ) 
			{	
				//dprintf(INFO, "[FWDN Write Func] [PartName:%8s] [WriteBlk:%4d] [ChipNo:%d]\n", ptn->name, curr_pblock, nChipNo );
				page = curr_pblock << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;
				curr_lblock = n + 1;
				break;					
			}
			else
			{
				dprintf(INFO, "flash_write_by_vtc: [PartName:%8s] [BadBlk:%4d] \n", ptn->name, curr_pblock );
				++backwardBlock;
			}
		}	
		
		if( res == SUCCESS )
		{
			for ( n = 0; n < block_size; ++n )
			{
				if (bytes < wsize) {
					if (bytes == 0)
						break;
					
					fxnFwdnDrvResponseRequestData(bytes);

					retval = fxnFwdnDrvDirectReadFromHost((void**)&readBuf);
				
					if( retval != bytes)
						return -1;
				
					if (extra_per_page)
						dprintf(CRITICAL, "flash_write_image: image undersized (%d < %d)\n", bytes, wsize);
					else 
					{
						dprintf(CRITICAL, "Request Data Size: %d \n", bytes);
						memset(flash_data, 0xFF, flash_pagesize + flash_info.spare_size);
						memcpy(flash_data, readBuf, bytes);
						res = flash_write_page(nChipNo, page + n, flash_data, spare);
						if( res != SUCCESS )
						{
							dprintf(INFO,"[MTD_Write_Fail_1] [BlkAddr:%4d] [res:0x%08X]\n", curr_pblock, res );
							return res;
						}

						#if NAND_CMP	
						res = flash_read_page(nChipNo, page + n, flash_data_read, flash_spare_read );
						if( res != SUCCESS )
						{
							dprintf(INFO,"[MTD_Read_Fail_1] [BlkAddr:%4d] [Line:%d]\n", curr_pblock, __LINE__ );
							return res;
						}					
						

						if( memcmp( readBuf, flash_data_read, flash_info.page_size ) )
						{
							dprintf(INFO,"[MTD_CMP_Data] [BlkAddr:%4d] [Line:%d]\n", curr_pblock, __LINE__ );
							dprintf(INFO,"[MTD_CMP_Data] Write_Data-Read_Data Not Matched!! \n" );

							unsigned int q;
								
							dprintf(INFO, " Write- Data : \n" );
							for( q = 0; q < flash_info.page_size; ++q )
							{
								dprintf(INFO, "%02X", readBuf[q]);
							}

							dprintf(INFO,"\n Read- Data : \n" );
							for( q = 0; q < flash_info.page_size; ++q )
							{
								dprintf(INFO, "%02X", flash_data_read[q] );
							}
							dprintf(INFO, "\n");

							dprintf(INFO, " Write-Spare Data : \n" );
							for( q = 0; q < 32; ++q )
							{
								dprintf(INFO, "%02X", flash_spare[q] );
							}

							dprintf(INFO,"\n Read-Spare Data : \n" );
							for( q = 0; q < 32; ++q )
							{
								dprintf(INFO, "%02X", flash_spare_read[q] );
							} 

							dprintf(INFO, "\n\n Write-Spare Data ECC: \n" );
							for( q = 32; q < flash_info.spare_size; ++q )
							{
								dprintf(INFO, "%02X", flash_spare[q] );
							}

							dprintf(INFO,"\n Read-Spare Data ECC: \n" );
							for( q = 32; q < flash_info.spare_size; ++q )
							{
								dprintf(INFO, "%02X", flash_spare_read[q] );
							}
						
							return -1;
						}	
						#endif
						
					}

					bytes = 0;
					break;
				}

				fxnFwdnDrvResponseRequestData(wsize);
				retval = fxnFwdnDrvDirectReadFromHost((void**)&readBuf);
				
				if( retval != wsize)
				{
					return -1;
				}
					

				if (extra_per_page) 
				{
					res = flash_write_page(nChipNo, page + n, readBuf, readBuf + flash_pagesize);							
					if( res != SUCCESS )
					{
						dprintf(INFO,"[MTD_Write_Fail_2] [BlkAddr:%4d] [res:0x%08X]\n", curr_pblock, res );
						return res;
					}

					#if NAND_CMP
					res = flash_read_page(nChipNo, page + n, flash_data_read, flash_spare_read );
					if( res != SUCCESS )
					{
						dprintf(INFO,"[MTD_Read_Fail_2] [Error:%X] [BlkAddr:%4d] [Line:%d]\n", res, curr_pblock, __LINE__ );
						//return res;
					}

					if( memcmp( readBuf, flash_data_read, flash_info.page_size ) )
					{
						dprintf(INFO,"[MTD_CMP_Data] [BlkAddr:%4d] [Line:%d]\n", curr_pblock, __LINE__ );
						dprintf(INFO,"[MTD_CMP_Data] Write_Data-Read_Data Not Matched!! \n" );

						unsigned int q;
							
						dprintf(INFO, " Write- Data : \n" );
						for( q = 0; q < flash_info.page_size; ++q )
						{
							dprintf(INFO, "%02X", readBuf[q]);
						}

						dprintf(INFO,"\n Read- Data : \n" );
						for( q = 0; q < flash_info.page_size; ++q )
						{
							dprintf(INFO, "%02X", flash_data_read[q] );
						}
						dprintf(INFO, "\n");

						dprintf(INFO, " Write-Spare Data : \n" );
						for( q = 0; q < 20; ++q )
						{
							dprintf(INFO, "%02X", flash_spare[q] );
						}

						dprintf(INFO,"\n Read-Spare Data : \n" );
						for( q = 0; q < 20; ++q )
						{
							dprintf(INFO, "%02X", flash_spare_read[q] );
						} 

						dprintf(INFO, "\n\n Write-Spare Data ECC: \n" );
						for( q = 32; q < flash_info.spare_size; ++q )
						{
							dprintf(INFO, "%02X", flash_spare[q] );
						}

						dprintf(INFO,"\n Read-Spare Data ECC: \n" );
						for( q = 32; q < flash_info.spare_size; ++q )
						{
							dprintf(INFO, "%02X", flash_spare_read[q] );
						}
						
						return -1;
					} 
					else 
					{
						if( memcmp( flash_spare, flash_spare_read, 20) )
						{
							dprintf(INFO,"[MTD_CMP_Spare] [BlkAddr:%4d] [Line:%d]\n", curr_pblock, __LINE__ );
							dprintf(INFO,"[MTD_CMP_Spare] Write_Spare-Read_Spare Not Matched!! \n" );

							unsigned int q;

							dprintf(INFO, " Write-Spare Data : \n" );
							for( q = 0; q < 32; ++q )
							{
								dprintf(INFO, "%02X", flash_spare[q] );
							}

							dprintf(INFO,"\n Read-Spare Data : \n" );
							for( q = 0; q < 32; ++q )
							{
								dprintf(INFO, "%02X", flash_spare_read[q] );
							} 

							dprintf(INFO, "\n\n Write-Spare Data ECC: \n" );
							for( q = 32; q < flash_info.spare_size; ++q )
							{
								dprintf(INFO, "%02X", flash_spare[q] );
							}

							dprintf(INFO,"\n Read-Spare Data ECC: \n" );
							for( q = 32; q < flash_info.spare_size; ++q )
							{
								dprintf(INFO, "%02X", flash_spare_read[q] );
							}
							
							
							return -1;	
						}
					}

					if( res != SUCCESS )
						return res;
					#endif
				} 
				else 
				{
					res = flash_write_page(nChipNo, page + n, readBuf, spare);
					if( res != SUCCESS )
					{
						dprintf(INFO,"[MTD_Write_Fail] [BlkAddr:%4d] [res:0x%08X]\n", curr_pblock, res );
						return res;
					}	
					
					#if NAND_CMP
					res = flash_read_page(nChipNo, page + n, flash_data_read, flash_spare_read );
					if( res != SUCCESS )
					{
						dprintf(INFO,"[MTD_Read_Fail] [Error:%X] [BlkAddr:%4d] [Line:%d]\n", res, curr_pblock, __LINE__ );							
					}

					if( memcmp( readBuf, flash_data_read, flash_info.page_size ) )
					{
						dprintf(INFO,"[MTD_CMP_Data] [BlkAddr:%4d] [Line:%d]\n", curr_pblock, __LINE__ );
						dprintf(INFO,"[MTD_CMP_Data] Write_Data-Read_Data Not Matched!! \n" );

						unsigned int q;
							
						dprintf(INFO, " Write- Data : \n" );
						for( q = 0; q < flash_info.page_size; ++q )
						{
							dprintf(INFO, "%02X", readBuf[q]);
						}

						dprintf(INFO,"\n Read- Data : \n" );
						for( q = 0; q < flash_info.page_size; ++q )
						{
							dprintf(INFO, "%02X", flash_data_read[q] );
						}
						dprintf(INFO, "\n");

						dprintf(INFO, " Write-Spare Data : \n" );
						for( q = 0; q < 32; ++q )
						{
							dprintf(INFO, "%02X", flash_spare[q] );
						}

						dprintf(INFO,"\n Read-Spare Data : \n" );
						for( q = 0; q < 32; ++q )
						{
							dprintf(INFO, "%02X", flash_spare_read[q] );
						} 

						dprintf(INFO, "\n\n Write-Spare Data ECC: \n" );
						for( q = 32; q < flash_info.spare_size; ++q )
						{
							dprintf(INFO, "%02X", flash_spare[q] );
						}

						dprintf(INFO,"\n Read-Spare Data ECC: \n" );
						for( q = 32; q < flash_info.spare_size; ++q )
						{
							dprintf(INFO, "%02X", flash_spare_read[q] );
						}
						
						return -1;
					}

					if( res!= SUCCESS )
						return res;
					
					#endif
						
				}

				bytes -= wsize;
			}
		}
	}

	
	for ( n = curr_lblock ; n < (ptn->start + ptn->length + nStartOffSet + backwardBlock); ++n ) 
	{
		NAND_MTD_GetPhyBlkAddr( &sDevInfo, n, &curr_pblock, &nChipNo );
		res = NAND_GetGoldenBadMarkofPBlock( nChipNo, curr_pblock );

		if( res != SUCCESS )
		{
			dprintf(INFO, "flash_write_by_vtc: [PartName:%8s] [BadBlk:%4d] \n", ptn->name, curr_pblock );
			++backwardBlock;
		}
		else
		{
			//dprintf(INFO, "flash_write_by_vtc: [PartName:%8s] [EraseBlk:%4d] \n", ptn->name, curr_pblock );
		}
	}

	dprintf(INFO, "flash_write_by_vtc: [PartName:%8s] [EndBlk:%4d] [TotalBlk:%4d] [Bad Block:%4d]\n", ptn->name, curr_pblock, (U32)ptn->length, backwardBlock);	
	if(nExpectBadNum!=backwardBlock)
	{
		dprintf(INFO, "flash_write_by_vtc: [Expect Bad Block Num:%2d] [Detect Bad Block Num:%2d]\n", nExpectBadNum, backwardBlock );
		return -1;
	}
	return 0;
}

int flash_read_by_vtc(struct ptentry *ptn, unsigned extra_per_page, unsigned long long bytes
								,FXN_FWDN_DRV_Response_NotifyData fxnFwdnDrvResponseNotifyData
								,FXN_FWDN_DRV_SendToHost fxnFwdnDrvSendToHost )
{
#define SEND_STATUS_EVERY_N_BYTES		(1024*1024)

	unsigned block_size = flash_info.block_size;
	unsigned curr_lblock = ptn->start;
	unsigned curr_pblock;
	unsigned page = 0;
//	unsigned lastpage = (ptn->start + ptn->length) * flash_info.block_size;
//	unsigned count = (bytes + flash_pagesize - 1 + extra_per_page) / (flash_pagesize + extra_per_page);
	unsigned char *spare = (unsigned char*)((unsigned int)flash_data + flash_info.page_size);
	unsigned errors = 0;
	unsigned char cBadBlkNum = 0, cAccBadBlkNum, nPartNum;
	unsigned n, nChipNo;
	int result;
	//signed nCalcedBytes=0;
	unsigned nNextSendStatusBytes = SEND_STATUS_EVERY_N_BYTES;

	NAND_IO_ERROR res;

	dprintf(INFO, "flash_read_by_vtc: name:%s size:0x%X spare:%d\n", ptn->name, bytes, extra_per_page);
	dprintf(INFO, "flash_read_by_vtc: pagesize:%3d, blocksize:%3d]\n", flash_info.page_size, flash_info.block_size );

	nPartNum		= ptable_GetPartNum( flash_ptable, ptn );
	cAccBadBlkNum	= NAND_MTD_GetAccBadBlkNum( nPartNum );

	curr_lblock 	+= cAccBadBlkNum;

	while (bytes > 0)
	{
		res = ERR_NAND_IO_NOT_READY_DEVICE_IO;

		for ( n = curr_lblock; n < (ptn->start + ptn->length + cBadBlkNum + cAccBadBlkNum) ; ++n ) 
		{
			NAND_MTD_GetPhyBlkAddr( &sDevInfo, n, &curr_pblock, nChipNo );
			page = curr_pblock << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;

			//dprintf(INFO,"[PartName:%10s] [P-Block:%3d] [Loop:%d]\n", ptn->name, curr_pblock, l );
			res = NAND_GetGoldenBadMarkofPBlock( nChipNo, curr_pblock );

			if( res != SUCCESS )
			{
				++cBadBlkNum;
				dprintf(INFO, "[FWDN Read Func] [PartName:%8s] [Read-Fail-Blk:%4d]\n", ptn->name, curr_pblock );
				//dprintf(INFO, "##### %d(phy:%d) is bad block #####\n", n, curr_pblock);
			}
			else
			{
				//dprintf(INFO, "[FWDN_CRC Read Func] [PartName:%8s] [Read-Blk:%4d] \n", ptn->name, curr_pblock );
				curr_lblock = n + 1;
				break;
			}
		}
			
		if( res == SUCCESS )
		{
			for ( n = 0; n < block_size; ++n )
			{
				if (bytes == 0) {
					dprintf(INFO, "flash_read_by_vtc: success (%d errors)\n", errors);
					return 0;
				}
				result = flash_read_page( nChipNo, page + n, flash_data, spare);
				if (result < 0) {
					/* bad page, go to next page */
					//page++;
					dprintf(ALWAYS,"flash_read_by_vtc: read_page error!\n");
					errors++;
					continue;
				}
				// send to vtc
				{
					unsigned int calc_size;
					calc_size = (bytes>flash_info.page_size)? flash_info.page_size : bytes;
					bytes -= calc_size;
					//alcedBytes += calc_size;
 					//rc = FWUG_CalcCrc8I(*crc,(unsigned char*)flash_data,calc_size,CRC32_TABLE);
					if(extra_per_page)
					{
						if(bytes<extra_per_page)
							dprintf(ALWAYS,"flash_read_by_vtc: remain=%d extra_per_page=%d\n",bytes,extra_per_page);
						bytes -= extra_per_page;
						memmove(spare,&spare[2],30);
						memset(&spare[30],0xFF,extra_per_page-30);
						calc_size += extra_per_page;
						//alcedBytes += extra_per_page;
						{
							//unsigned int i;
							//unsigned char *pData = &spare[2];
							//for(i=2;i<extra_per_page;i++)
							//{
							//	dprintf(INFO,"%X ",spare[i]);
							//	if((i&0xF)==1)
							//		dprintf(INFO,"\n");
							//}
							//dprintf(INFO,"\n\n\n");
						}
						//*crc = FWUG_CalcCrc8I(*crc,&spare[2],28/*yaffs2 tag*/,CRC32_TABLE);
						//memset(spare,0xFF,flash_info.spare_size);
						//*crc = FWUG_CalcCrc8I(*crc,spare,flash_info.spare_size-28,CRC32_TABLE);
					}
					fxnFwdnDrvResponseNotifyData(calc_size,0);
					fxnFwdnDrvSendToHost(flash_data,calc_size);
				}
			}

			//if(nCalcedBytes>nNextSendStatusBytes)
			//{
			//	nNextSendStatusBytes += SEND_STATUS_EVERY_N_BYTES;
			//	fxnFwdnDrvSendStatus((unsigned int)nCalcedBytes>>9,0,0);
			//}
		}

	}

	if (bytes == 0) 
	{
		dprintf(INFO, "flash_read_by_vtc: success (%d errors)X\n", errors );
		return 0;
	}

	/* could not find enough valid pages before we hit the end */
	dprintf(INFO, "flash_read_by_vtc: failed (%d errors)\n", errors);

	return 0xffffffff;
}

int flash_calc_crc_by_vtc(struct ptentry *ptn, unsigned extra_per_page, unsigned long long bytes
									, unsigned int *crc, FXN_FWDN_DRV_SendStatus fxnFwdnDrvSendStatus)
{
#define SEND_STATUS_EVERY_N_BYTES		(1024*1024)

	unsigned block_size = flash_info.block_size;
	unsigned curr_lblock = ptn->start;
	unsigned curr_pblock;
	unsigned page = 0;
//	unsigned lastpage = (ptn->start + ptn->length) * flash_info.block_size;
//	unsigned count = (bytes + flash_pagesize - 1 + extra_per_page) / (flash_pagesize + extra_per_page);
	unsigned char *spare = (unsigned char*) flash_spare;
	unsigned errors = 0;
	unsigned char cBadBlkNum = 0, cAccBadBlkNum, nPartNum;
	unsigned n, nChipNo;
	int result;
	unsigned nCalcedBytes=0;
	unsigned nNextSendStatusBytes = SEND_STATUS_EVERY_N_BYTES;

	NAND_IO_ERROR res;

	dprintf(INFO, "flash_calc_crc_by_vtc: name:%s size:0x%X spare:%d\n", ptn->name, bytes, extra_per_page);
	dprintf(INFO, "flash_calc_crc_by_vtc: pagesize:%3d, blocksize:%3d]\n", flash_info.page_size, flash_info.block_size );

	nPartNum		= ptable_GetPartNum( flash_ptable, ptn );
	cAccBadBlkNum	= NAND_MTD_GetAccBadBlkNum( nPartNum );

	curr_lblock 	+= cAccBadBlkNum;

	while (bytes > 0)
	{
		res = ERR_NAND_IO_NOT_READY_DEVICE_IO;

		for ( n = curr_lblock; n < (ptn->start + ptn->length + cBadBlkNum + cAccBadBlkNum) ; ++n ) 
		{
			NAND_MTD_GetPhyBlkAddr( &sDevInfo, n, &curr_pblock, &nChipNo );
			page = curr_pblock << gNAND_DrvInfo[0].NFTLDrvInfo->MediaDevInfo[0].ShiftPpB;

			//dprintf(INFO,"[PartName:%10s] [P-Block:%3d] [Loop:%d]\n", ptn->name, curr_pblock, l );
			res = NAND_GetGoldenBadMarkofPBlock( nChipNo, curr_pblock );

			if( res != SUCCESS )
			{
				++cBadBlkNum;
				dprintf(INFO, "[FWDN_CRC Read Func] [PartName:%8s] [Read-Fail-Blk:%4d]\n", ptn->name, curr_pblock );
				//dprintf(INFO, "##### %d(phy:%d) is bad block #####\n", n, curr_pblock);
			}
			else
			{
				//dprintf(INFO, "[FWDN_CRC Read Func] [PartName:%8s] [Read-Blk:%4d] \n", ptn->name, curr_pblock );
				curr_lblock = n + 1;
				break;
			}
		}
			
		if( res == SUCCESS )
		{
			for ( n = 0; n < block_size; ++n )
			{
				if (bytes == 0) {
					dprintf(INFO, "flash_calc_crc_by_vtc: success (%d errors), crc=0x%X\n", errors, *crc);
					return 0;
				}
				result = flash_read_page( nChipNo, page + n, flash_data, spare);
				if (result < 0) {
					/* bad page, go to next page */
					//page++;
					dprintf(ALWAYS,"flash_calc_crc_by_vtc: read_page error!\n");
					errors++;
					continue;
				}
				// calc crc
				{
					unsigned int calc_size;
					calc_size = (bytes>flash_info.page_size)? flash_info.page_size : bytes;
					bytes -= calc_size;
					nCalcedBytes += calc_size;
					//{
					//	unsigned char *pData = (unsigned char*)flash_data;
					//	dprintf(INFO,"\n%X %X %X %X %X %X %X %X\n\n",pData[0],pData[1],pData[2],pData[3],pData[4],pData[5],pData[6],pData[7]);
					//}
					*crc = FWUG_CalcCrc8I(*crc,(unsigned char*)flash_data,calc_size);
					if(extra_per_page)
					{
						if(bytes<extra_per_page)
							dprintf(ALWAYS,"flash_calc_crc_by_vtc: remain=%d extra_per_page=%d\n",bytes,extra_per_page);
						bytes -= extra_per_page;
						nCalcedBytes += extra_per_page;
						{
							//unsigned int i;
							//unsigned char *pData = &spare[2];
							//for(i=2;i<extra_per_page;i++)
							//{
							//	dprintf(INFO,"%X ",spare[i]);
							//	if((i&0xF)==1)
							//		dprintf(INFO,"\n");
							//}
							//dprintf(INFO,"\n\n\n");
						}
						*crc = FWUG_CalcCrc8I(*crc,&spare[2],18/*yaffs2 tag*/);
						memset(spare,0xFF,flash_info.spare_size);
						*crc = FWUG_CalcCrc8I(*crc,spare,flash_info.spare_size-18);
					}
				}
			}

			if(nCalcedBytes>nNextSendStatusBytes)
			{
				nNextSendStatusBytes += SEND_STATUS_EVERY_N_BYTES;
				fxnFwdnDrvSendStatus((unsigned int)nCalcedBytes>>9,0,0);
			}
		}

	}

	if (bytes == 0) 
	{
		dprintf(INFO, "flash_calc_crc_by_vtc: success (%d errors), crc=0x%X\n", errors, *crc);
		return 0;
	}

	/* could not find enough valid pages before we hit the end */
	dprintf(INFO, "flash_calc_crc_by_vtc: failed (%d errors)\n", errors);

	return 0xffffffff;
}

#endif
#endif

unsigned flash_page_size(void)
{
#if defined(_EMMC_BOOT_TCC) || defined(TNFTL_V8_INCLUDE)
	return 512;
#else
	return flash_pagesize;
#endif
}

