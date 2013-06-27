/***************************************************************************************
*	FileName    : lcd.h
*	Description : lcd Functions
****************************************************************************************
*
*	TCC Board Support Package
*	Copyright (c) Telechips, Inc.
*	ALL RIGHTS RESERVED
*
****************************************************************************************/
#ifndef __PARTITON_H__
#define __PARTITON_H__

	// ���ͳ� ��Ƽ�� ���̺� ����																	   
#define PARTITION_TBL_POS	0x1BE																	 
	// Ȯ�� ��Ƽ�� Ÿ�� 																			   
#define PARTITION_TYPE_EXT	0x0F																   
																									   
typedef struct _PARTITION																		   
{																								   
	unsigned char active;		// ���� ���� �÷��� (0x80 : ���� ����, 0x00 : ���� �Ұ�)						   
	unsigned char begin[3]; // CHS ����� ��Ƽ�� ù��° ����												   
	unsigned char type; 	// ��Ƽ�� Ÿ��																		 
	unsigned char end[3];		// CHS ����� ��Ƽ�� ������ ����												   
	unsigned int start; 	// LBA ����� ��Ƽ�� ��ġ														   
	unsigned int length;	// ��Ƽ�ǿ��� ���Ǵ� ������ ����												   
} PARTITION, *PPARTITION;			


// ��ũ ���� ��� ��Ƽ�� �˻� 																   
unsigned int GetLocalPartition(unsigned char Drv, PPARTITION pPartition);											 
// Ȯ�� ��Ƽ�� �˻� (��� ȣ��) 																   
void SearchExtPartition(unsigned char drv, PPARTITION pPartition, unsigned int *pnPartitionCnt, int BaseAddr);		 
// ��ũ ���� ��Ʈ���ڵ忡�� ��Ƽ�� ���̺� ����												   
void GetPartitionTbl(unsigned char Drv, unsigned int nSecPos, PPARTITION pPartition, int nReadCnt); 

unsigned int ChangePartitonType(unsigned int index);

#endif	/*__PARTITON_H__*/
