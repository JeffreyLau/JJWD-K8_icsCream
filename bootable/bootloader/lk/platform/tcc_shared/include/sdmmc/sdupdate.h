#ifndef _SDUPDATE_H
#define _SDUPDATE_H

typedef struct{
	unsigned char	tagHeader[8];
	unsigned long	ulHeaderSize;
	unsigned long	ulCRC32;
	char			tagImageType[16];
	char			areaName[16];
} FAI_RAW_IMAGE_HEADER_T;

typedef struct{
	unsigned long long		ullTargetAddress;
	unsigned long long		ullLength;
} FAI_IMAGE_BUNCH_HEADER_T;

extern int SDUpdate(void);
//extern int SDRecovery(unsigned int *sizePartition, unsigned int *validFAT, unsigned int reFormat);

#endif //_SDUPDATE_H
