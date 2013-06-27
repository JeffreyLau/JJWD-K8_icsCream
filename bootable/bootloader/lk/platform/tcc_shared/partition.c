
#include <debug.h>
#include <dev/keys.h>
#include <dev/gpio_keypad.h>
#include <lib/ptable.h>
#include <dev/flash.h>

#if defined(BOOTSD_INCLUDE)
#include <fwdn/Disk.h>
#include <sdmmc/sd_memory.h>

#elif defined(TCC_NAND_USE)
#if defined(TNFTL_V8_INCLUDE)
#include "nand_drv_v8.h"
#else
#include <tnftl/tnftl_v7.h>
#endif

#endif

#include <partition.h>

unsigned int ExtPartionBase;

unsigned int GetLocalPartition(unsigned char Drv, PPARTITION pPartition)
{
	unsigned int i;
	unsigned int nPartitionCnt = 0;
	PPARTITION pPriExtPartition = NULL;

	// �� ��Ƽ���̹Ƿ� 4���� ��Ƽ�� �о��
	GetPartitionTbl(Drv, 0, pPartition, 4);
	for(i=0; i<8 && pPartition->length != 0; i++) {
		if(pPartition->type == PARTITION_TYPE_EXT) {
			pPriExtPartition = pPartition;
		}
		pPartition++;		// ���� ��Ƽ������ �̵�
		nPartitionCnt++;	// ��Ƽ�� ī��Ʈ UP
	}

	if(!pPriExtPartition)
		return nPartitionCnt;

	//Ȯ�� ��Ƽ���� �˻� �Ҷ� ����Ѵ�
	ExtPartionBase = pPriExtPartition->start;

	SearchExtPartition(Drv, pPriExtPartition, &nPartitionCnt, 0);
	return nPartitionCnt;
}

void GetPartitionTbl(unsigned char Drv, unsigned int nSecPos, PPARTITION pPartition, int nReadCnt)
{
	char pSecBuf[512];

	// ������ ��ũ Drv�� nSecPos�� ���Ϳ��� 1���� ���� �о�´�.
#if defined(BOOTSD_INCLUDE)
	if(BOOTSD_Read(SD_BUS_GetBootSlotIndex(), nSecPos, 1, pSecBuf) != 0 )
#elif defined(TCC_NAND_USE) && defined(TNFTL_V8_INCLUDE)
	if(NAND_ReadSector(0, nSecPos, 1, pSecBuf) != 0 )
#endif
	{
		printf("Boot Sector Read Failed \n");
		return;
	}

	memcpy(pPartition, (pSecBuf + PARTITION_TBL_POS), sizeof(PARTITION)*nReadCnt);
}

//change to linux partition
unsigned int ChangePartitonType(unsigned int index)
{
	int i;
	int position=0;
	PPARTITION pPartition = 0;

	char pSecBuf[512];

#if defined(BOOTSD_INCLUDE)
	if( BOOTSD_Read(SD_BUS_GetBootSlotIndex(), 0, 1, pSecBuf) != 0 )
#elif defined(TCC_NAND_USE) && defined(TNFTL_V8_INCLUDE)
	if( NAND_ReadSector(0, 0, 1, pSecBuf) != 0 )
#endif
	{
		printf("Boot Sector Read Failed \n");
		return -1;
	}

	//change partiton type
	pPartition=pSecBuf+ (PARTITION_TBL_POS+ sizeof(PARTITION)*index);

	if (pPartition->type != 0x83) {
		pPartition->type = 0x83;
	}

#if defined(BOOTSD_INCLUDE)
	if(	BOOTSD_Write(SD_BUS_GetBootSlotIndex(), 0, 1, pSecBuf) != 0)
#elif defined(TCC_NAND_USE) && defined(TNFTL_V8_INCLUDE)
	if(	NAND_WriteSector(0, 0, 1, pSecBuf) != 0)
#endif
	{
		printf("Boot Sector Write Failed \n");
		return -1;
	}

	return 0;
}
void SearchExtPartition(unsigned char drv, PPARTITION pPartition, unsigned int *pnPartitionCnt, int BaseAddr)
{
	int nExtStart	= pPartition->start + BaseAddr;
	static int nCnt = 0;

	//�����͸� �о���� ���� �����͸� ���� ��Ƽ�� ������ �̵�
	pPartition++;
	//�� ��Ƽ�ǰ� Ȯ�� ��Ƽ���� ���� �� �����Ƿ� 2���� ��Ƽ���� �о��
	GetPartitionTbl(drv, nExtStart, pPartition, 2);
	while(pPartition->length != 0 && nCnt == 0)
	{
		(*pnPartitionCnt)++;
		if(pPartition->type == PARTITION_TYPE_EXT)
		{
			SearchExtPartition(drv, pPartition, pnPartitionCnt, ExtPartionBase);
		} else {
			pPartition++;
		}

		if( pPartition->length == 0 )
			nCnt = 1;
	}
}

void PrintPartitionInfo(PPARTITION pPartition, unsigned int nIdx)
{
	dprintf(INFO, "[PARTITION #%d]\n", nIdx);
	dprintf(INFO, "Bootable : 0x%X\n", pPartition->active);
	dprintf(INFO, "  Type : 0x%X\n", pPartition->type);
	dprintf(INFO, " Start : %d\n", pPartition->start);
	dprintf(INFO, "  Length : %d\n", pPartition->length);
	dprintf(INFO, "Partition Size : %d MB\n", pPartition->length / 1024 * 512 / 1024);
	dprintf(INFO, "------------------------\n\n");
}
