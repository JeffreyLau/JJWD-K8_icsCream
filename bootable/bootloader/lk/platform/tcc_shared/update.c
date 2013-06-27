/***************************************************************************************
*	FileName    : update.c
*	Description : firmware update 
****************************************************************************************
*
*	TCC Board Support Package
*	Copyright (c) Telechips, Inc.
*	ALL RIGHTS RESERVED
*
****************************************************************************************/

//#include <common.h>
#include <debug.h>
#include <err.h>
#include <reg.h>
#include <string.h>
#include <platform.h>
//#include <def_tcc.h>
#include <i2c.h>
#include <platform/globals.h>
#include <platform/reg_physical.h>

#ifndef _TODO_
//#include <i2c.h>
//#include <clock.h>
#include <fwdn/Disk.h>
#include <fwdn/file.h>
#include <fwdn/FSAPP.h>
#include <tnftl/nand_drv.h>
#include <sdmmc/sd_memory.h>

int gSDUpgrading = 0;

extern unsigned char pca9539u3_data[2];
extern int FWUG_MainFunc(int hFile, int iFileSize);
extern int VerifyROMFile(unsigned int *pBuffer,unsigned int size );

#if 0
int sd_check_files(int *fd_boot, int *fd_kernel, int *fd_ramdisk)
{
	*fd_boot = FILE_OpenName("tcboot.rom", "r", 1);
	*fd_kernel = FILE_OpenName("linux.rom", "r", 1);
	*fd_ramdisk = FILE_OpenName("ramdisk.rom", "r", 1);

	if (*fd_boot > 0 || *fd_kernel > 0 || *fd_ramdisk > 0) {
		return 0;
	} else {
		return -1;
	}
}

int sd_update_boot(int fd_boot)
{
	int iRetVal;
	int iLength;

	printf("Updating tcboot.rom -> ");

	iLength = FILE_Length(fd_boot);
	iRetVal = FWUG_MainFunc(fd_boot, iLength);

	if(iRetVal < 0 ) {
		printf("fail\n");
		return -1;
	}
	printf("success\n");
	return 0;
}

int sd_update_kernel(int fd_kernel)
{
	unsigned char *ptr;
	unsigned long pageCount;
	unsigned long i;
	unsigned int uiReadSize = 512, uiWritePage = 128;// 128 page = 64Kbyte
	unsigned int uiTotalSize;
	unsigned int uiHiddenAddr;
	unsigned int ProgramLength;
	unsigned int CRC;
	unsigned char TempBuf[64*1024];

	printf("Updating linux.rom -> ");

	/* get linux.rom length & CRC value */
	ptr = (unsigned char *)KERNEL_BASEADDRESS;
	FILE_Read(fd_kernel, ptr, uiReadSize);
	memcpy(&ProgramLength, ptr + 0x1C, 4);
	memcpy(&CRC, ptr + 0x18, 4);
	if (ProgramLength >= 0xffffffff) {
		goto err;
	}
	ptr += uiReadSize;

	/* get linux.rom page count */
	pageCount = ProgramLength / NAND_PAGE_SIZE;
	if (ProgramLength % NAND_PAGE_SIZE) {
		pageCount++;
	}

	/* check CRC */
	for (i = 0; i < pageCount; i++) {
		FILE_Read(fd_kernel, ptr, uiReadSize);
		ptr += uiReadSize;
	}
	if (VerifyROMFile((unsigned int *)KERNEL_BASEADDRESS, ProgramLength) != 1) {
		goto err;
	}

	/* update linux.rom */
	ptr = (unsigned char *)KERNEL_BASEADDRESS;
	uiTotalSize = pageCount;
	uiHiddenAddr = HIDDEN_OFFSET_OF_KERNEL;

	for (i = 0; i < pageCount; i++) {
		if ((i % 256) == 0) {
			printf(".");
		}
		if ((i % uiWritePage) == 0) {
#ifdef NAND_INCLUDE
			NAND_HDWritePage(uiHiddenAddr, uiWritePage, ptr);
			NAND_HDReadPage(uiHiddenAddr, uiWritePage, TempBuf);
#elif BOOTSD_INCLUDE
			if(DISK_HDWriteSector(DISK_DEVICE_TRIFLASH, uiHiddenAddr, uiWritePage, ptr) != 0) // != SUCCESS
				return -1;
			else if(DISK_HDReadSector(DISK_DEVICE_TRIFLASH, uiHiddenAddr, uiWritePage, TempBuf) != 0) // != SUCCESS
				return -1;
#endif
			if (memcmp(ptr, TempBuf, 64*1024) != 0) {
				goto err;
			}

			uiTotalSize -= uiWritePage;
			ptr += (uiWritePage * NAND_PAGE_SIZE);
			uiHiddenAddr += uiWritePage;

			if (uiTotalSize < uiWritePage) {
				uiWritePage = uiTotalSize;
			}
			if (uiWritePage == 0) {
				break;
			}
		}
	}

	printf("success\n");
	return 0;

err:
	printf("fail\n");
	return -1;
}

int sd_update_ramdisk(int fd_ramdisk, int fd_kernel)
{
	unsigned char *ptr;
	unsigned long pageCount, kernel_page_count;
	unsigned long i;
	unsigned int uiReadSize = 512, uiWritePage = 128;// 128 page = 64Kbyte
	unsigned int uiTotalSize;
	unsigned int uiHiddenAddr;
	unsigned int ProgramLength, RamdiskLength;
	unsigned char TempBuf[64*1024];

	printf("Updating ramdisk.rom -> ");

	/* get kernel & ramdisk length */
	ptr = (unsigned char *)KERNEL_BASEADDRESS;
	FILE_Read(fd_kernel, ptr, uiReadSize);
	memcpy(&ProgramLength, ptr + 0x1C, 4);
	memcpy(&RamdiskLength, ptr + 0x14, 4);
	if (ProgramLength >= 0xffffffff || RamdiskLength == 0x0) {
		goto err;
	}

	/* get kernel page count */
	kernel_page_count = ProgramLength / NAND_PAGE_SIZE;
	if (ProgramLength % NAND_PAGE_SIZE) {
		kernel_page_count++;
	}

	/* get ramdisk page count */
	pageCount = RamdiskLength / NAND_PAGE_SIZE;
	if (RamdiskLength % NAND_PAGE_SIZE) {
		pageCount++;
	}

	/* load ramdisk.rom */
	ptr = (unsigned char *)(KERNEL_BASEADDRESS + RamdiskLength);
	for (i = 0; i < pageCount; i++) {
		FILE_Read(fd_ramdisk, ptr, uiReadSize);
		ptr += uiReadSize;
	}

	/* update ramdisk */
	ptr = (unsigned char *)(KERNEL_BASEADDRESS + RamdiskLength);
	uiTotalSize = pageCount;
	uiHiddenAddr = HIDDEN_OFFSET_OF_KERNEL + kernel_page_count;

	for (i = 0; i < pageCount; i++) {
		if ((i % 256) == 0) {
			printf(".");
		}
		if ((i % uiWritePage) == 0) {
#ifdef NAND_INCLUDE
			NAND_HDWritePage(uiHiddenAddr, uiWritePage, ptr);
			NAND_HDReadPage(uiHiddenAddr, uiWritePage, TempBuf);
#elif BOOTSD_INCLUDE
			if(DISK_HDWriteSector(DISK_DEVICE_TRIFLASH, uiHiddenAddr, uiWritePage, ptr) != 0) // != SUCCESS
				return -1;
			else if(DISK_HDReadSector(DISK_DEVICE_TRIFLASH, uiHiddenAddr, uiWritePage, TempBuf) != 0) // != SUCCESS
				return -1;
#endif
			if (memcmp(ptr, TempBuf, 64*1024) != 0) {
				goto err;
			}

			uiTotalSize -= uiWritePage;
			ptr += (uiWritePage * NAND_PAGE_SIZE);
			uiHiddenAddr += uiWritePage;

			if (uiTotalSize < uiWritePage) {
				uiWritePage = uiTotalSize;
			}
			if (uiWritePage == 0) {
				break;
			}
		}
	}

	printf("success\n");
	return 0;

err:
	printf("fail\n");
	return -1;
}

int sd_update(void)
{
	int ret = 0, status = 0;
	int fd_kernel, fd_boot, fd_ramdisk;

	unsigned int i2c_ch = SMU_I2C_CH0;
	unsigned char DestAddress;
	unsigned char i2cData_mode[3] = {0,0,0};
	unsigned char i2cData_data[3] = {0,0,0};
	PGPION pGPIOA = (GPION *)&HwGPIOA_BASE;
	PGPION pGPIOE = (GPION *)&HwGPIOE_BASE;
	PTIMER pTIMER = (TIMER *)&HwTMR_BASE;

	/* card detect */
	BITCLR(pGPIOA->GPFN0, Hw28-Hw24);	// GPIO_A[6] SD_CD
	BITCLR(pGPIOA->GPEN, Hw6);			// input mode
	for (ret = 0; ret < 1000; ret++);
	ret = (pGPIOA->GPDAT & Hw6) ? -1 : 0;
	if (ret) {
		printf("error: insert SD card\n");
		goto sd_cd;
	}

	/* SD init & Filesystem Initialize */
	if (!Initialize_FileSystem(DISK_DEVICE_MMC, 0)) {
		ret = -1;
		printf("error: init filesystem fail\n");
		goto hw_deinit;
	}

	/* check rom files */
	if (sd_check_files(&fd_boot, &fd_kernel, &fd_ramdisk) == 0) {
		if (fd_boot > 0) {
			status |= Hw0; 
		}
		if (fd_kernel > 0)	{
			status |= Hw1; 
		}
		if (fd_ramdisk > 0) {	
			status |= Hw2; 
		}
		gSDUpgrading = 1; 
	} else {
		gSDUpgrading = 0;	
		ret = -1;
		printf("error: can't find rom files\n");
		goto hw_deinit;
	}

	/* update firmware */
	status |= Hw6;
	if (fd_boot > 0) {
		if (sd_update_boot(fd_boot) == 0) {
			status |= Hw3;
		}
	}
	if (fd_kernel > 0) {
		if (sd_update_kernel(fd_kernel) == 0) {
			status |= Hw4;
		}
	}
	if (fd_ramdisk > 0) {
		if (fd_kernel <= 0) {
			ret = -1;
			printf("error: update of ramdisk.rom needs linux.rom\n");
		} else {
			if (sd_update_ramdisk(fd_ramdisk, fd_kernel) == 0) {
				status |= Hw5;
			}
		}
	}

	//printf("status 0x%x\n", status);

//exit:
	if (fd_boot > 0) {
		FILE_Close(fd_boot);
	}
	if (fd_kernel > 0) {
		FILE_Close(fd_kernel);
	}
	if (fd_ramdisk > 0) {
		FILE_Close(fd_ramdisk);
	}

hw_deinit:
	/* 32Bit Timer */
	BITCLR(pTIMER->TC32EN, Hw24);

	/* Hardware Initialize */
	BITCLR(pGPIOE->GPFN1, 0xFFFF0000);
	BITCLR(pGPIOE->GPFN2, 0x00FF0000);
	BITCLR(pGPIOA->GPFN0, 0x0F000000);

	/* clock */
	tca_ckc_setperi(PERI_SDMMC0, DISABLE, 120000);
	tca_ckc_setiopwdn(RB_SDMMC1CONTROLLER, 1);

	/* slot power */
	i2cData_data[1] = pca9539u3_data[0];
	i2cData_data[2] = pca9539u3_data[1];
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);

sd_cd:
	/* card detect */
	BITSET(pGPIOA->GPEN, Hw6);		// output
	BITCLR(pGPIOA->GPDAT, Hw6);		// low

	return ret;
}
#endif
void sd_deinit(void)
{
	#if defined (PLATFORM_TCC892X)
	unsigned int i2c_ch = I2C_CH_SMU;
	#else
	unsigned int i2c_ch = SMU_I2C_CH0;
	#endif
	unsigned char DestAddress;
	unsigned char i2cData_mode[3] = {0,0,0};
	unsigned char i2cData_data[3] = {0,0,0};
	//PPIC pPIC = (PIC *)&HwPIC_BASE;

	/* disable clock & io bus */
	//tca_ckc_setperi(PERI_SDMMC0, DISABLE, 120000);
	//tca_ckc_setiopwdn(RB_SDMMCCONTROLLER, 1);

	/* reset peri block */
	tca_ckc_setioswreset(RB_SDMMC0CONTROLLER, 1);
	tca_ckc_setioswreset(RB_SDMMC0CONTROLLER, 0);
	
	/* slot0,1 power down */
	DestAddress = 0xE8;
	i2cData_mode[0] = 6;
	i2cData_data[0] = 2;
	BITCLR(pca9539u3_data[0], Hw6|Hw7);
	i2cData_data[1] = pca9539u3_data[0];
	i2cData_data[2] = pca9539u3_data[1];
	i2c_xfer(DestAddress, 3, i2cData_mode, 0, 0, i2c_ch);
	i2c_xfer(DestAddress, 3, i2cData_data, 0, 0, i2c_ch);

	/* clear interrupt */
	//BITSET(pPIC->CLR1, Hw12|Hw13);
}
#endif

/************* end of file *************************************************************/
