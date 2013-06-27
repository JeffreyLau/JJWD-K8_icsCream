/****************************************************************************
	Decription :
		File System refer for K-FileSystem

		2008. 08. 20. Created			Telechips, Inc.
****************************************************************************/

#include <tnftl/IO_TCCXXX.h>
#include <fwdn/Disk.h>
#include <fwdn/FSAPP.h>
#include <fwdn/KFSutils.h>
#include <fwdn/file.h>
#include <fwdn/fat.h>


#ifndef NULL
#define NULL	0
#endif


HANDLERstruc		fhandler[MAX_HANDLE];
unsigned char		entry_bufferPRE[ENTRY_BUFFER_SIZE * MAX_HANDLE];

FDstruc				fd[MAX_FD];
FDIRENTstruc		fdir[MAX_DIR];
int					totalHdlr;
int					gDiskIdleTime = 0;
unsigned char		physicalDrvType[FSAPP_MAX_PART_NUMBER];

int					gFileBufferIndex;
int					gFileBufferSize = 65536;

// Volume Label Buffer
unsigned char BS_VolumeLabel_PR1[12] = {'T','E','L','E','C','H','I','P','S', '0', '1', 0x00};
unsigned char BS_VolumeLabel_PR2[12] = {'T','E','L','E','C','H','I','P','S', '0', '2', 0x00};
unsigned char BS_VolumeLabel_PR3[12] = {'T','E','L','E','C','H','I','P','S', '0', '3', 0x00};
unsigned char BS_VolumeLabel_PR4[12] = {'T','E','L','E','C','H','I','P','S', '0', '4', 0x00};

/************************************************************************
* FUNCTION
*			FSAPP_SetVolumeLabel
*
* DESCRIPTION
*			Volumn Label Setting.
*			NOTE
*				This Function is used in file system library.
*				Do not modify this function's name.
*
* INPUTS
*
* OUTPUTS
*			string pointer of the volume label
*************************************************************************/
unsigned char *FSAPP_SetVolumeLabel(unsigned int partID, void* VolLab)
{
	int				length = 0;
	unsigned char	*pVolLab = (unsigned char *)VolLab;

	length = str_len(VolLab);
	if(length > 11)		length = 11;

	switch(partID)
	{
		case 0:
			if(*pVolLab != 0x00)
			{
				mem_set((void *)BS_VolumeLabel_PR1, 0x20, 11);
				BS_VolumeLabel_PR1[11] = 0;	// end of string
				mem_cpy((unsigned char *)BS_VolumeLabel_PR1, (unsigned char *)VolLab, length);
			}
			return ((unsigned char *)BS_VolumeLabel_PR1);

		case 1:
			if(*pVolLab != 0x00)
			{
				mem_set((void *)BS_VolumeLabel_PR2, 0x20, 11);
				BS_VolumeLabel_PR2[11] = 0;	// end of string
				mem_cpy((unsigned char *)BS_VolumeLabel_PR2, (unsigned char *)VolLab, length);
			}
			return ((unsigned char *)BS_VolumeLabel_PR2);

		case 2:
			if(*pVolLab != 0x00)
			{
				mem_set((void *)BS_VolumeLabel_PR3, 0x20, 11);
				BS_VolumeLabel_PR3[11] = 0;	// end of string
				mem_cpy((unsigned char *)BS_VolumeLabel_PR3, (unsigned char *)VolLab, length);
			}
			return ((unsigned char *)BS_VolumeLabel_PR3);

		case 3:
			if(*pVolLab != 0x00)
			{
				mem_set((void *)BS_VolumeLabel_PR4, 0x20, 11);
				BS_VolumeLabel_PR4[11] = 0;	// end of string
				mem_cpy((unsigned char *)BS_VolumeLabel_PR4, (unsigned char *)VolLab, length);
			}
			return ((unsigned char *)BS_VolumeLabel_PR4);
	}
}

/************************************************************************
* FUNCTION
*			FSAPP_GetVolumeLabel
*
* DESCRIPTION
*			Get Volumn Label string pointer.
*
* INPUTS
*
* OUTPUTS
*			string pointer of the volume label
*************************************************************************/
unsigned char *FSAPP_GetVolumeLabel(unsigned int partID)
{

	switch(partID)
	{
		case 0:
			return ((unsigned char *)BS_VolumeLabel_PR1);

		case 1:
			return ((unsigned char *)BS_VolumeLabel_PR2);

		case 2:
			return ((unsigned char *)BS_VolumeLabel_PR3);

		case 3:
			return ((unsigned char *)BS_VolumeLabel_PR4);

		default:
			return 0;
	}
}

/************************************************************************
* FUNCTION
*				FSAPP_InitDiskDevice
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_InitDiskDevice(int DeviceID)
{
	int		res, drvTypeID, partIDnum;

	res = DISK_Ioctl( DeviceID, DEV_INITIALIZE, NULL);
	if(res < 0)		return (-1);

	drvTypeID = FSAPP_Get_part_id(DeviceID,1);

	partIDnum = FAT_MountDrive(drvTypeID, 0);
	if(partIDnum < 0)		return (-1);

	res = DISK_SetDiskPartitionIndex(DeviceID, res);
	if(res < 0)		return (-1);

	res = DISK_SetState(DeviceID, DISK_STATE_MOUNTSUCCEED);
	if(res < 0)		return (-1);

	return 	partIDnum;
}

/************************************************************************
* FUNCTION
*			FSAPP_GetMaxCopySize
*
* DESCRIPTION
*			return maximun copy size unit
*			Do not modify this function name & type. This function is used in F/S library.
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_GetMaxCopySize(void)
{
	return ((int)gFileBufferSize);
}

/************************************************************************
* FUNCTION
*			FSAPP_GetFileBuffer
*
* DESCRIPTION
*			return buffer pointer for copying.
*			Do not modify this function name & type. This function is used in F/S library.
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
unsigned char *FSAPP_GetFileBuffer(void)
{
//    return &gFileBuffer[gFileBufferIndex];
}

/************************************************************************
* FUNCTION
*			FSAPP_InitializeFS
*
* DESCRIPTION
*			Initialize File System global variables or structure.
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
void FSAPP_InitializeFS(void)
{
	int i;

	for(i=0;i<FSAPP_MAX_PART_NUMBER;i++)
	{
		mem_set((void *)&physicalDrvType[i],0xFF,sizeof(unsigned char));
	}

	FILE_InitSystem();
	FAT_InitDriveInfo();
	FAT_InitVariable();
	FAT_InitializeForFlexibility((unsigned char)MAX_HANDLE, entry_bufferPRE, fhandler,
								(unsigned char)MAX_FD, fd, (unsigned char)MAX_DIR, fdir);
}

/************************************************************************
* FUNCTION
*
*        FSAPP_GetDiskSector
*
* DESCRIPTION
*             Obtain total disk sector for data area.
* INPUTS
*
* OUTPUTS
*             # of disk sector
*************************************************************************/
unsigned long FSAPP_GetDiskSector(unsigned char drvType, DISKINFOstruc *disk)
{
	ioctl_diskinfo_t diskInfo;
	unsigned long nSector;

	switch(drvType)
	{
		case DISK_DEVICE_HDD:
			DISK_Ioctl(DISK_DEVICE_HDD, DEV_GET_DISKINFO, (void *)&diskInfo);
			disk->head=diskInfo.head; disk->cylinder=diskInfo.cylinder; disk->sector=diskInfo.sector; disk->sector_size=diskInfo.sector_size;
			nSector = (unsigned int)(disk->cylinder * disk->head * disk->sector) - ((unsigned int)HIDDEN_SIZE*2 + 1);
			break;

		case DISK_DEVICE_MMC:
			DISK_Ioctl(DISK_DEVICE_MMC, DEV_GET_DISKINFO, (void *)&diskInfo);
			disk->head=diskInfo.head; disk->cylinder=diskInfo.cylinder; disk->sector=diskInfo.sector; disk->sector_size=diskInfo.sector_size;
			nSector = (unsigned int)(disk->cylinder * disk->head * disk->sector) - ((unsigned int)HIDDEN_SIZE*2 + 1);
			break;

		case DISK_DEVICE_UHP:
			return (-1);

		case DISK_DEVICE_NAND:
			DISK_Ioctl(DISK_DEVICE_NAND, DEV_GET_DISKINFO, (void *)&diskInfo);
			disk->head=diskInfo.head; disk->cylinder=diskInfo.cylinder; disk->sector=diskInfo.sector; disk->sector_size=diskInfo.sector_size;
			if((disk->cylinder == 0) || (disk->head == 0) || (disk->sector == 0))
				return (-1);

			nSector = (unsigned int)(disk->cylinder * disk->head * disk->sector) - (unsigned int)HIDDEN_SIZE;
			break;
		case DISK_DEVICE_TRIFLASH:
			DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_GET_DISKINFO, (void *)&diskInfo);
			disk->head=diskInfo.head; disk->cylinder=diskInfo.cylinder; disk->sector=diskInfo.sector; disk->sector_size=diskInfo.sector_size;
			if((disk->cylinder == 0) || (disk->head == 0) || (disk->sector == 0))
				return (-1);

			nSector = (unsigned int)(disk->cylinder * disk->head * disk->sector) - (unsigned int)HIDDEN_SIZE;
			break;

		default:
		{
			int res;
			res = DISK_Ioctl(drvType, DEV_GET_DISKINFO, (void *)&diskInfo);
			if ( res < 0) return res;
			disk->head=diskInfo.head; disk->cylinder=diskInfo.cylinder; disk->sector=diskInfo.sector; disk->sector_size=diskInfo.sector_size;
			nSector = (unsigned int)(disk->cylinder * disk->head * disk->sector) - ((unsigned int)HIDDEN_SIZE*2 + 1);

			break;
		}
	}

	return nSector;
}


/************************************************************************
* FUNCTION
*
*        FSAPP_Get_part_id
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_Get_part_id(unsigned char drvType, unsigned char mount)
{
	int i, nPart = 0;

	for(i=0; i<FSAPP_MAX_PART_NUMBER; i++)
	{
		if((physicalDrvType[i]==0xFF)&&(mount==1))
		{
			physicalDrvType[i] = drvType;
			nPart = i + 1;
			break;
		}
		else if(physicalDrvType[i]==drvType)
		{
			nPart = i + 1;
			break;
		}
		else	;
	}

	return nPart;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_disk_RWsector
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_disk_RWsector(int drvTypeID, unsigned char drv_num,unsigned long LBA_addr,
						unsigned short nSector, void *buff, unsigned char RWflag)
{
	int res;
	unsigned char drv_type;
	drv_type = physicalDrvType[drvTypeID - 1];

	if(RWflag==1)
	{
		res = DISK_ReadSector(drv_type, drv_num, LBA_addr, nSector, buff);
	}
	else
	{
		res	= DISK_WriteSector(drv_type, drv_num, LBA_addr, nSector, buff);
	}
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_diskIoctl_DEV_GET_MAXMULTISECTOR
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_diskIoctl_DEV_GET_MAXMULTISECTOR(int drvTypeID, unsigned short *nSector)
{
	int res;
	unsigned char drv_type;
	unsigned short temp;
	drv_type = physicalDrvType[drvTypeID - 1];
	if(drv_type==DISK_DEVICE_HDD)
	{
		res = DISK_Ioctl(DISK_DEVICE_HDD, DEV_GET_MAXMULTISECTOR, &temp);
		*nSector = temp;
	}
	else
		res = (-1);
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_diskIoctl_DEV_ERASE_INIT
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_diskIoctl_DEV_ERASE_INIT(int drvTypeID, unsigned char secPerClus, unsigned long dataStartSec)
{
	int res;
	unsigned char drv_type;
	ioctl_diskeraseinit_t	erase;

	drv_type = physicalDrvType[drvTypeID - 1];
	if(drv_type==DISK_DEVICE_NAND)
	{
		erase.sector_per_cluster 	=	secPerClus;
		erase.data_start_sector		=	dataStartSec;
		res = DISK_Ioctl(DISK_DEVICE_NAND, DEV_ERASE_INIT, (void *) &erase);
	}
	else
		res = 1;
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_diskIoctl_DEV_ERASE_BLOCK
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_diskIoctl_DEV_ERASE_BLOCK(int drvTypeID, unsigned long currCluster, unsigned long contentFAT)
{
	int res;
	unsigned char drv_type;
	ioctl_diskerase_t 	erase;

	drv_type = physicalDrvType[drvTypeID - 1];
	if(drv_type==DISK_DEVICE_NAND)
	{
		erase.current_cluster	= 	currCluster;
		erase.content_fat		=	contentFAT;
		res = DISK_Ioctl(DISK_DEVICE_NAND, DEV_ERASE_BLOCK, (void *) &erase);
	}
	else
		res = 1;
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_diskIoctl_DEV_ERASE_CLOSE
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_diskIoctl_DEV_ERASE_CLOSE(int drvTypeID)
{
	int res;
	unsigned char drv_type;
	drv_type = physicalDrvType[drvTypeID - 1];
	if(drv_type==DISK_DEVICE_NAND)
		res = DISK_Ioctl(DISK_DEVICE_NAND, DEV_ERASE_CLOSE, 0);
	else
		res = 1;
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_diskIoctl_DEV_SET_MULTISECTOR
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_diskIoctl_DEV_SET_MULTISECTOR(int drvTypeID, unsigned short *max_multi_sector)
{
	int res;
	unsigned char drv_type;
	unsigned short temp;
	drv_type = physicalDrvType[drvTypeID - 1];
	if(drv_type==DISK_DEVICE_HDD)
	{
		res = DISK_Ioctl(DISK_DEVICE_HDD, DEV_SET_MULTISECTOR, &temp);
		*max_multi_sector = temp;
	}
	else
		res = (-1);
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_diskIoctl_DEV_TELL_DATASTARTSECTOR
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_diskIoctl_DEV_TELL_DATASTARTSECTOR(int drvTypeID, unsigned long int data_start_sector)
{
	int res;
	unsigned char drv_type;
	drv_type = physicalDrvType[drvTypeID - 1];
	if(drv_type==DISK_DEVICE_NAND)
		res = DISK_Ioctl(DISK_DEVICE_NAND, DEV_TELL_DATASTARTSECTOR, &data_start_sector);
	else
		res = 1;
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_diskIoctl_DEV_GET_MAX_SECTOR_PER_BLOCK
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_diskIoctl_DEV_GET_MAX_SECTOR_PER_BLOCK(int drvTypeID, unsigned short int *SpB)
{
	int res;
	unsigned char drv_type;
	unsigned short int temp;
	drv_type = physicalDrvType[drvTypeID - 1];
	if(drv_type==DISK_DEVICE_NAND)
	{
		res = DISK_Ioctl(DISK_DEVICE_NAND, DEV_GET_MAX_SECTOR_PER_BLOCK, (void *)&temp);
		*SpB = temp;
	}
	else
		res = (-1);
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_diskIoctl_DEV_STOP_TRANSFER
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_diskIoctl_DEV_STOP_TRANSFER(int drvTypeID)
{
	int res;
	res = DISK_Ioctl(physicalDrvType[drvTypeID - 1], DEV_STOP_TRANSFER, 0);
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_DISK_WriteMultiStart
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_DISK_WriteMultiStart(int drvTypeID, int lba_addr, int size)
{
	int res;
	res = DISK_WriteMultiStart(physicalDrvType[drvTypeID - 1], lba_addr, size);
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_DISK_WriteMultiSector
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_DISK_WriteMultiSector(int drvTypeID, int lun, unsigned long lba_addr, unsigned short nSector, void *buff)
{
	int res;
	res = DISK_WriteMultiSector(physicalDrvType[drvTypeID - 1], lun, lba_addr, nSector, buff);
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_DISK_ReadMultiStart
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int	FSAPP_DISK_ReadMultiStart(int drvTypeID, int lba_addr, int size)
{
	int res;
	res = DISK_ReadMultiStart(physicalDrvType[drvTypeID - 1], lba_addr, size);
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_DISK_ReadMultiSector
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_DISK_ReadMultiSector(int drvTypeID, int lun, unsigned long lba_addr, unsigned short nSector, void *buff)
{
	int res;
	res = DISK_ReadMultiSector(physicalDrvType[drvTypeID - 1], lun, lba_addr, nSector, buff);
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_DISK_ReadMultiStop
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_DISK_ReadMultiStop(int drvTypeID)
{
	int res;
	res = DISK_ReadMultiStop(physicalDrvType[drvTypeID - 1]);
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_DISK_WriteMultiStop
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_DISK_WriteMultiStop(int drvTypeID)
{
	int res;
	res = DISK_WriteMultiStop(physicalDrvType[drvTypeID - 1]);
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_physicalStorage_HDD
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_physicalStorage_HDD(int drvTypeID)
{
	physicalDrvType[drvTypeID - 1] = DISK_DEVICE_HDD;
	return 1;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_ReadSector_Common
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_ReadSector_Common(int drvTypeID, int lun, unsigned long lba_addr, unsigned short nSector, void *buff)
{
	int res;
	if(physicalDrvType[drvTypeID - 1] == DISK_DEVICE_TRIFLASH)
	{
		res = FSAPP_DISK_ReadMultiSector(drvTypeID, lun, lba_addr, nSector, buff);
	}
	else
	{
		res = drv_ReadSector(drvTypeID, lba_addr, nSector, buff);
	}
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_WriteSector_Common
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_WriteSector_Common(int drvTypeID, int lun, unsigned long lba_addr, unsigned short nSector, void *buff, unsigned char FormatFlag)
{
	int res;
	if(physicalDrvType[drvTypeID - 1] == DISK_DEVICE_TRIFLASH)
	{
		if(FormatFlag==1)	FSAPP_DISK_WriteMultiStart(drvTypeID, lba_addr, nSector);
		res = FSAPP_DISK_WriteMultiSector(drvTypeID, lun, lba_addr, nSector, buff);
	}
	else
	{
		res = drv_WriteSector(drvTypeID, lba_addr, nSector, buff);
	}
	return res;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_getUsedClus_PS
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_getUsedClus_PS(int drvTypeID, int partID, unsigned FAT1_PhySector, unsigned short nSector)
{
	int	dirty_count = 0;
	if(physicalDrvType[drvTypeID - 1] == DISK_DEVICE_TRIFLASH)
	{
		FSAPP_DISK_ReadMultiStart(drvTypeID, FAT1_PhySector, 1);
		dirty_count = FAT_getUsedClus_Common(drvTypeID, partID, nSector, FAT1_PhySector);
		if(dirty_count<0)	return (-1);
		FSAPP_DISK_ReadMultiStop(drvTypeID);
	}
	else
	{
		dirty_count = FAT_getUsedClus_Common(drvTypeID, partID, nSector, FAT1_PhySector);
		if(dirty_count<0)	return (-1);
	}
	return dirty_count;
}

/************************************************************************
* FUNCTION
*			FSAPP_cleanFATcache_PS
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_cleanFATcache_PS(int drvTypeID, int partID, unsigned long FAT2_PhySector, unsigned long FAT_Sector, unsigned char *sbuffer)
{
	if(physicalDrvType[drvTypeID - 1] == DISK_DEVICE_TRIFLASH)
	{
		FSAPP_DISK_WriteMultiStart(drvTypeID, FAT_Sector, 1);

		if(FAT_fatCache_Common(drvTypeID, partID, FAT2_PhySector, FAT_Sector, 1, sbuffer, 0)<0)
			return (-1);

		FSAPP_DISK_WriteMultiStop(drvTypeID);
	}
	else if((physicalDrvType[drvTypeID - 1]==DISK_DEVICE_NAND)||(physicalDrvType[drvTypeID - 1]==DISK_DEVICE_NAND_HD))
	{
		if(FAT_fatCache_Common(drvTypeID, partID, FAT2_PhySector, FAT_Sector, 0, sbuffer, 0)<0)
			return (-1);
	}
	else
	{
		if(FAT_fatCache_Common(drvTypeID, partID, FAT2_PhySector, FAT_Sector, 1, sbuffer, 0)<0)
			return (-1);
	}
	return 0;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_changeFATcache_PS
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_changeFATcache_PS(int drvTypeID, int partID, unsigned long FAT1_PhySector, unsigned int Offset_Sector, unsigned char *fatBuff)
{
	if(physicalDrvType[drvTypeID - 1] == DISK_DEVICE_TRIFLASH)
	{
		FSAPP_DISK_ReadMultiStart(drvTypeID, FAT1_PhySector+Offset_Sector, 1);

		if(FAT_fatCache_Common(drvTypeID, partID, 0, (unsigned long)(FAT1_PhySector+Offset_Sector), 1, fatBuff, 1) < 0)
		{
			return (-1);
		}

		FSAPP_DISK_ReadMultiStop(drvTypeID);
	}
	else if((physicalDrvType[drvTypeID - 1]==DISK_DEVICE_NAND)||(physicalDrvType[drvTypeID - 1]==DISK_DEVICE_NAND_HD))
	{
		if(FAT_fatCache_Common(drvTypeID, partID, 0, (unsigned long)(FAT1_PhySector+Offset_Sector), 0, fatBuff, 1) < 0)
		{
			return (-1);
		}
	}
	else
	{
		if(FAT_fatCache_Common(drvTypeID, partID, 0, (unsigned long)(FAT1_PhySector+Offset_Sector), 1, fatBuff, 1) < 0)
		{
			return (-1);
		}
	}
	return 0;
}

/************************************************************************
* FUNCTION
*			FSAPP_fatWriteClus_PS
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_fatWriteClus_PS(int drvTypeID, unsigned short nCount, unsigned long sector_addr, unsigned short nSector,
							unsigned short multi_sector, int multi_byte, unsigned char *temp_buff)
{
	int i;
	if(nCount)
	{
		if(physicalDrvType[drvTypeID - 1] == DISK_DEVICE_TRIFLASH)
		{
			FSAPP_DISK_WriteMultiStart(drvTypeID, (sector_addr), 1);
			for(i=0; i < nCount; i++)
			{
				if(FSAPP_WriteSector_Common(drvTypeID, 0, (sector_addr + i*multi_sector), multi_sector, (temp_buff + i*multi_byte), 0) < 0)
					return (-1);
			}
			FSAPP_DISK_WriteMultiStop(drvTypeID);
		}
		else
		{
			for(i=0; i < nCount; i++)
			{
				if(FSAPP_WriteSector_Common(drvTypeID, 0, (sector_addr + i*multi_sector), multi_sector, (temp_buff + i*multi_byte), 0) < 0)
					return (-1);			}
		}
	}
	else // cluster 크기가 16 Kbyte 미만이면...
	{
		if(physicalDrvType[drvTypeID - 1] == DISK_DEVICE_TRIFLASH)
		{
			FSAPP_DISK_WriteMultiStart(drvTypeID, (sector_addr), 1);
			if(FSAPP_WriteSector_Common(drvTypeID, 0, sector_addr, nSector, temp_buff, 0) < 0)
				return (-1);
			FSAPP_DISK_WriteMultiStop(drvTypeID);
		}
		else
		{
			if(FSAPP_WriteSector_Common(drvTypeID, 0, sector_addr, nSector, temp_buff, 0) < 0)
				return (-1);
		}
	}
	return 0;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_FormatClear_PS
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
unsigned int FSAPP_FormatClear_PS(int drvTypeID, unsigned long sectorPerFAT, unsigned int nSector, unsigned char *pBuff, void *SecBuff)
{
	if(physicalDrvType[drvTypeID - 1] == DISK_DEVICE_TRIFLASH)
	{
		nSector = FAT_Format_ClearCommon(drvTypeID, (int)sectorPerFAT, (unsigned long)nSector, pBuff, SecBuff, 1);
	}
	else if(physicalDrvType[drvTypeID - 1]==DISK_DEVICE_NAND)
	{
		nSector = FAT_Format_ClearCommon(drvTypeID, (int)sectorPerFAT, (unsigned long)nSector, pBuff, SecBuff, 1);
	}
	else
	{
		nSector = FAT_Format_ClearCommon(drvTypeID, (int)sectorPerFAT, (unsigned long)nSector, pBuff, SecBuff, 0);
	}
	return nSector;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_FormatRootEntry_PS
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FSAPP_FormatRootEntry_PS(int drvTypeID, int halfEntryNum, unsigned nSector,
								unsigned char writeVolume, void *pBuff, unsigned char *BS_VolLab)
{
	if(physicalDrvType[drvTypeID - 1] == DISK_DEVICE_TRIFLASH)
	{
		if(FAT_FormatRE_Common(drvTypeID, halfEntryNum, nSector, writeVolume,	pBuff, BS_VolLab) < 0)
			return (-1);

		FSAPP_DISK_WriteMultiStop(drvTypeID);
	}
	else
	{
		if(FAT_FormatRE_Common(drvTypeID, halfEntryNum, nSector, writeVolume, pBuff, BS_VolLab) < 0)
			return (-1);
	}
	return 0;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_decide_MakeMBR
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
unsigned int FSAPP_decide_MakeMBR(int drvTypeID, unsigned char SecPerClus, unsigned int *numOfSecPR, unsigned int *validFAT, void *pBuff, struct _DISK_INFO *disk)
{
	unsigned int nSector;

	nSector = FAT_makeMBR(drvTypeID, numOfSecPR, validFAT, SecPerClus, pBuff, disk);
	if(nSector == (unsigned int)(-1))		return (-1);
	else									return nSector;
}

/************************************************************************
* FUNCTION
*			FSAPP_FormatDrive
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
unsigned int FSAPP_FormatDrive( DISK_DEVICE diskDevice, unsigned int *multiPartition_SectorSize, unsigned int *validFAT)
{
	int				iRetVal, drvTypeID;
	unsigned int	nSector, mode = 0; // Format Disk For NAND(Forcely set mode to quick level mode)
	DISKINFOstruc disk;

   	FSAPP_InitializeFS();

	if(diskDevice == DISK_DEVICE_NAND)
	{
		iRetVal = DISK_Ioctl(DISK_DEVICE_NAND, DEV_FORMAT_DISK, (void*)&mode );
		if(iRetVal == 0)
		{
			nSector = FSAPP_GetDiskSector(DISK_DEVICE_NAND, &disk);
			if(nSector == -1)	return 0;

			drvTypeID = FSAPP_Get_part_id(DISK_DEVICE_NAND, 1);

			iRetVal = FILE_FormatMBR(drvTypeID, 1, 32, multiPartition_SectorSize, validFAT, &(disk));
			if(iRetVal != 1)	return 0;
		}
		else					return 0;
	}
	else if(diskDevice == DISK_DEVICE_TRIFLASH)
	{
		iRetVal = DISK_Ioctl(DISK_DEVICE_TRIFLASH, DEV_FORMAT_DISK, (void*)&mode );
		if(iRetVal == 0)
		{
			nSector = FSAPP_GetDiskSector(DISK_DEVICE_TRIFLASH, &disk);
			if(nSector == -1)	return 0;

			drvTypeID = FSAPP_Get_part_id(DISK_DEVICE_TRIFLASH, 1);

			iRetVal = FILE_FormatMBR(drvTypeID, 1, 32, multiPartition_SectorSize, validFAT, &(disk));
			if(iRetVal != 1)	return 0;
		}
		else					return 0;
	}

	return 1;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_decide_DrvNum
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
unsigned char FSAPP_decide_DrvNum(int drvTypeID)
{
	unsigned char DrvNum;

	if(physicalDrvType[drvTypeID - 1] == DISK_DEVICE_HDD)
		DrvNum = 0x80;
	else
		DrvNum = 0x00;

	return DrvNum;
}

/************************************************************************
* FUNCTION
*			FileSystemMount
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int FileSystemMount(DISK_DEVICE device, int partID, int dir_num)
{
	int drvTypeID;

	drvTypeID = FSAPP_Get_part_id(device,0);

	if(FILE_OpenDirByEntryId(drvTypeID, partID, ROOT_DIR_ID, dir_num) != dir_num)	// 디렉토리를 연다.
		return (-1);

	return 1;
}

/************************************************************************
* FUNCTION
*			Initialize_FileSystem
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
int Initialize_FileSystem(DISK_DEVICE mDeviceNum, int partID)
{
	int iDeviceNum, partIDnum;
	int iMountCondition = 1;

   	FSAPP_InitializeFS();		// initialize all drive information in FS

	for(iDeviceNum = 0 ; iDeviceNum < MAX_DEVICE_NUM ; iDeviceNum++)
	{
		if(DISK_GetDeviceStatus(iDeviceNum,0,0))
		{
			partIDnum = FSAPP_InitDiskDevice(iDeviceNum);

			if(DISK_GetState(iDeviceNum) == DISK_STATE_MOUNTERR)
			{

//				UI_API_DrawPopUp2Line("FAT Error!", "Plz Connet UMS" );
//				TotalMusicFileNum 	= 0;
//				CurrentFileNum 		= 0;
			}
			else if(mDeviceNum == iDeviceNum)
			{
				if(partIDnum!=0)
					partID = partIDnum + partID;
				if(FileSystemMount(iDeviceNum, partID, 1) > 0)
					iMountCondition = 1;
				else
					iMountCondition = 0;
			}
			else	;
		}
		else	;
	}
	return iMountCondition;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_PartitionWrite
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
unsigned int FSAPP_PartitionWrite(DISK_DEVICE diskDevice, unsigned int partitionID, unsigned int lba, void *pBuff, unsigned short nSector)
{
	int drvTypeID = FSAPP_Get_part_id(diskDevice, 0);
	if(FDISK_PartitionWrite(drvTypeID, partitionID, lba, pBuff, nSector)<0)
		return 0;

	return 1;
}

/************************************************************************
* FUNCTION
*
*        FSAPP_PartitionRead
*
* DESCRIPTION
*
* INPUTS
*
* OUTPUTS
*
*************************************************************************/
unsigned int FSAPP_PartitionRead(DISK_DEVICE diskDevice, unsigned int partitionID, unsigned int lba, void *pBuff, unsigned short nSector)
{
	int drvTypeID = FSAPP_Get_part_id(diskDevice, 0);
	if(FDISK_PartitionRead(drvTypeID, partitionID, lba, pBuff, nSector)<0)
		return 0;

	return 1;
}

