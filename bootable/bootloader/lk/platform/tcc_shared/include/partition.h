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

	// 섹터내 파티션 테이블 번지																	   
#define PARTITION_TBL_POS	0x1BE																	 
	// 확장 파티션 타입 																			   
#define PARTITION_TYPE_EXT	0x0F																   
																									   
typedef struct _PARTITION																		   
{																								   
	unsigned char active;		// 부팅 가능 플래그 (0x80 : 부팅 가능, 0x00 : 부팅 불가)						   
	unsigned char begin[3]; // CHS 모드의 파티션 첫번째 섹터												   
	unsigned char type; 	// 파티션 타입																		 
	unsigned char end[3];		// CHS 모드의 파티션 마지막 섹터												   
	unsigned int start; 	// LBA 모드의 파티션 위치														   
	unsigned int length;	// 파티션에서 사용되는 섹터의 개수												   
} PARTITION, *PPARTITION;			


// 디스크 내의 모든 파티션 검색 																   
unsigned int GetLocalPartition(unsigned char Drv, PPARTITION pPartition);											 
// 확장 파티션 검색 (재귀 호출) 																   
void SearchExtPartition(unsigned char drv, PPARTITION pPartition, unsigned int *pnPartitionCnt, int BaseAddr);		 
// 디스크 내의 부트레코드에서 파티션 테이블 저장												   
void GetPartitionTbl(unsigned char Drv, unsigned int nSecPos, PPARTITION pPartition, int nReadCnt); 

unsigned int ChangePartitonType(unsigned int index);

#endif	/*__PARTITON_H__*/
