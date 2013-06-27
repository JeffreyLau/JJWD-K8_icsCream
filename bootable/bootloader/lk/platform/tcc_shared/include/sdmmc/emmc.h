#include <sdmmc/sd_memory.h>
#include <partition.h>
#include <fwdn/Disk.h>

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
#define VFAT16_PARTITION 0x6
#define WIN_EXTENDED_PARTITION 0xf
#define LINUX_EXTENDED_PARTITION 0x5
#define RAW_EMPTY_PARTITION 0x0

struct mbr_entry
{
	unsigned dstatus;
	unsigned dtype;
	unsigned dfirstsec;
	unsigned dsize;
	unsigned char name[64];
};


#define GET_LWORD_FROM_BYTE(x) ((unsigned)*(x) | ((unsigned)*(x+1) << 8) | ((unsigned)*(x+2) <<16) | ((unsigned)*(x+3) << 24))
//#define ROUND_TO_PAGE(x,y) (((x)+(y)) & (~(y)))

void emmc_boot_main();
static unsigned int emmc_boot_read_MBR();
static void mbr_set_partition_name(struct mbr_entry* mbr_ent , unsigned int type);
unsigned int emmc_write(unsigned long long data_addr , unsigned int data_len , void* in);
unsigned int emmc_read(unsigned long long adta_addr , unsigned data_len, void* in);
#if defined(FWDN_HIDDEN_KERNEL_MODE)
unsigned int emmc_boot_write(unsigned int data_len , unsigned int *data);
#endif
unsigned long long emmc_ptn_offset(unsigned char* name);
int get_emmc_serial(char* Serial);
static void printpartition(void);
