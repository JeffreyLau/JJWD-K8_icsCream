/******************************************************************************
*
*  (C)Copyright All Rights Reserved by Telechips Inc.
*                                   
*  This material is confidential and shall remain as such. 
*  Any unauthorized use, distribution, reproduction is strictly prohibited. 
*
*   FileName    : TCCFile.h
*   Description : 
*   TCC Version 1.0
*   Copyright (c) Telechips, Inc.
*   ALL RIGHTS RESERVED
*******************************************************************************/
#ifndef	_TCC_UTIL_H__
#define	_TCC_UTIL_H__

/******************************************************************************
* include 
******************************************************************************/
//#include <stdio.h>
//#include <stdlib.h>
//#include <libgen.h>
//#include <sys/vfs.h>
//#include <sys/stat.h>
//#include <sys/types.h>

//#include <unistd.h>

/******************************************************************************
* typedefs & structure
******************************************************************************/
struct f_size{
	long blocks;
	long avail;
};

typedef struct _mountinfo {
	FILE *fp;			// ���� ��Ʈ�� ������
	char devname[80];	// ��ġ �̸�
	char mountdir[80];	// ����Ʈ ���丮 �̸�
	char fstype[12];	// ���� �ý��� Ÿ��
	struct f_size size;	// ���� �ý����� ��ũ��/�����
} MOUNTP;

/******************************************************************************
* defines 
******************************************************************************/

/******************************************************************************
* declarations
******************************************************************************/
MOUNTP *dfopen();
MOUNTP *dfget(MOUNTP *MP, int deviceType);
int dfclose(MOUNTP *MP);

#endif //_TCC_UTIL_H__
