#include <sdmmc/emmc.h>
#include <sdmmc/sd_bus.h>
#include <debug.h>
#include <string.h>

struct mbr_entry mbr[MAX_PARTITIONS];

static char *ext4_partitions[] = {"system" , "userdata", "cache"};
static char *raw_partitions[] = {"boot","recovery", "kpanic" , "splash" , "misc", "tcc" , "swap"};

unsigned mmc_partition_count = 0;
unsigned int ext4_count = 0;
unsigned int raw_count = 0;

void emmc_boot_main()
{
	ioctl_diskinfo_t disk_info;

	dprintf(INFO , "%s:init start from eMMC\n" , __func__ );
	SD_BUS_Initialize();
	DISK_Ioctl(DISK_DEVICE_TRIFLASH , DEV_INITIALIZE , NULL);
	DISK_Ioctl(DISK_DEVICE_TRIFLASH , DEV_GET_DISKINFO , (void*)&disk_info);

	emmc_boot_read_MBR();
	printpartition();
}

static unsigned int emmc_boot_read_MBR()
{
	unsigned char buffer[512];
	unsigned int dtype;
	unsigned int dfirstsec;
	unsigned int EBR_first_sec;
	unsigned int EBR_current_sec;
	int ret = 0;
	int idx, i;

	/* Read Master Boot Record from eMMC */
	ret = BOOTSD_Read(SD_BUS_GetBootSlotIndex() , 0 , 1, buffer);

	if(ret)
	{
		dprintf(INFO , "Master Boot Record Read Fail on eMMC\n");
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
		mbr[mmc_partition_count].dstatus = buffer[idx+i*TABLE_ENTRY_SIZE + OFFSET_STATUS];
		mbr[mmc_partition_count].dtype = buffer[idx+i*TABLE_ENTRY_SIZE + OFFSET_TYPE];
		mbr[mmc_partition_count].dfirstsec = GET_LWORD_FROM_BYTE(&buffer[idx+i*TABLE_ENTRY_SIZE + OFFSET_FIRST_SEC]);
		mbr[mmc_partition_count].dsize = GET_LWORD_FROM_BYTE(&buffer[idx+i*TABLE_ENTRY_SIZE + OFFSET_SIZE]);
		dtype = mbr[mmc_partition_count].dtype;
		dfirstsec = mbr[mmc_partition_count].dfirstsec;
		mbr_set_partition_name(&mbr[mmc_partition_count], mbr[mmc_partition_count].dtype); 
		mmc_partition_count++;
		if (mmc_partition_count == MAX_PARTITIONS)
			return ret;
	}

	EBR_first_sec = dfirstsec;
	EBR_current_sec = dfirstsec;

	ret = BOOTSD_Read(SD_BUS_GetBootSlotIndex(), EBR_first_sec , 1, buffer); 

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

		mbr[mmc_partition_count].dstatus =  buffer[TABLE_ENTRY_0 + OFFSET_STATUS];
		mbr[mmc_partition_count].dtype   = buffer[TABLE_ENTRY_0 + OFFSET_TYPE];
		mbr[mmc_partition_count].dfirstsec = GET_LWORD_FROM_BYTE(&buffer[TABLE_ENTRY_0 + OFFSET_FIRST_SEC])+EBR_current_sec;
		mbr[mmc_partition_count].dsize = GET_LWORD_FROM_BYTE(&buffer[TABLE_ENTRY_0 + OFFSET_SIZE]);
		mbr_set_partition_name(&mbr[mmc_partition_count], mbr[mmc_partition_count].dtype); 
		mmc_partition_count++;
		if (mmc_partition_count == MAX_PARTITIONS)
			return ret;

		dfirstsec = GET_LWORD_FROM_BYTE(&buffer[TABLE_ENTRY_1 + OFFSET_FIRST_SEC]);
		if(dfirstsec == 0)
		{
			/* Getting to the end of the EBR tables */
			break;
		}
		/* More EBR to follow - read in the next EBR sector */
		ret = BOOTSD_Read(SD_BUS_GetBootSlotIndex(), EBR_first_sec+dfirstsec , 1 , buffer); 

		if (ret)
		{
			return ret;
		}
		EBR_current_sec = EBR_first_sec + dfirstsec;
	}

	return ret;
}

static void mbr_set_partition_name(struct mbr_entry* mbr_ent , unsigned int type)
{
	switch(type) {
		case VFAT_PARTITION:
		case VFAT16_PARTITION:
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

#if defined(FWDN_HIDDEN_KERNEL_MODE)
unsigned int emmc_boot_write(unsigned int data_len, unsigned int* data)
{
	ioctl_diskrwpage_t rwHiddenPage;

	dprintf(INFO, "boot write\n");

	rwHiddenPage.start_page =0;
	rwHiddenPage.rw_size = data_len / 512;
	rwHiddenPage.hidden_index = 0;
	rwHiddenPage.buff = (unsigned char*)data;

	return DISK_Ioctl(DISK_DEVICE_TRIFLASH , DEV_HIDDEN_WRITE_PAGE, (void*)&rwHiddenPage); 
}
#endif

unsigned int emmc_write(unsigned long long data_addr, unsigned int data_len, void* in)
{
	int val = 0;
	unsigned int pageCount = 0;

	pageCount = (data_len +511) / 512;
	if(pageCount)
		val = BOOTSD_Write(SD_BUS_GetBootSlotIndex(), data_addr, pageCount, in);
	return val;
}

unsigned int emmc_read(unsigned long long data_addr , unsigned data_len , void* in)
{
	int val = 0;
	unsigned int pageCount = 0;

	pageCount = (data_len + 511) / 512;

	if(pageCount)
		val = BOOTSD_Read(SD_BUS_GetBootSlotIndex(), data_addr, pageCount, in);

	return val;
}

unsigned long long emmc_ptn_offset(unsigned char* name)
{
	unsigned n;

	for(n=0; n<mmc_partition_count; n++){
		if(!strcmp((const char*)mbr[n].name, (const char*)name)){
			return (mbr[n].dfirstsec);
		}
	}

	return 0;
}

int get_emmc_serial(char* Serial)
{
	ioctl_diskinfo_t disk_info;
	ioctl_diskrwpage_t bootCodeRead;
	unsigned char readData[512];
	int res;
	int i;

	if(DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_GET_DISKINFO , (void*)&disk_info) <0)
		return -1;

	if(disk_info.sector_size != 512)
		return -1;

	bootCodeRead.start_page = BOOTSD_GetHeaderAddr();
	bootCodeRead.rw_size = 1;
	bootCodeRead.buff  = readData;
	res = DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_BOOTCODE_READ_PAGE, (void*)&bootCodeRead);
	if(res !=0) return -1;

	for(i=0; i<16; i++)
		Serial[i] = ((BOOTSD_Header_Info_T*)readData)->ucSerialNum[i];
	for(i=16 ; i<32; i++)
		Serial[i]= ((BOOTSD_Header_Info_T*)readData)->ucSerialNum[i+16];

	return 0;
}

static void printpartition()
{
	unsigned idx;

	for(idx = 0; idx < mmc_partition_count; idx++)
	{
		if(mbr[idx].dtype == LINUX_EXTENDED_PARTITION)
			continue;

		dprintf(INFO, "[PARTITION : %10s] [START : %10d] [SIZE : %10d] [TYPE : %2x]\n" , mbr[idx].name, mbr[idx].dfirstsec, mbr[idx].dsize ,mbr[idx].dtype);
	}
}

int get_partition_info(char *partition_name, unsigned long *addr, unsigned long *size)
{
	unsigned int idx;

	for( idx=0; idx<mmc_partition_count; idx++ )
	{
		if( !strcmp( mbr[idx].name, partition_name ) )
		{
			*addr = mbr[idx].dfirstsec;
			*size = mbr[idx].dsize;
			dprintf(INFO, "[ERASED PARTITION : %10s] [START : %10d] [SIZE : %10d] [TYPE : %2x]\n" , 
					mbr[idx].name, mbr[idx].dfirstsec, mbr[idx].dsize ,mbr[idx].dtype);
			
			return 0;
		}
	}
	return -1;
}

