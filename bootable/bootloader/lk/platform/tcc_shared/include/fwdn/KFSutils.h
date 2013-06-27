
/****************************************************************************
 *   FileName    : KFSutils.h
 *   Description : 
 ****************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips, Inc.
 *   ALL RIGHTS RESERVED
 *
 ****************************************************************************/   


extern char upperChar(char ch);
extern char lowerChar(char ch);
extern int printNum(char *sNum, int value);
extern int str_cmp(void *sA, void *sB);
extern int str_cmp16(void *sA, void *sB);
extern int str_len(void *pStr);
extern int mem_cmp(void *sA, void *sB, int len);
extern void* mem_cpy(void *pDes, void *pSrc, long size);
extern void* mem_cpyw(void *pDes, void *pSrc, long size);
extern void* mem_set(void *pDes, unsigned char value, long size);
