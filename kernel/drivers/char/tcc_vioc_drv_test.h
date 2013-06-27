/****************************************************************************
 *	 TCC Version 0.6
 *	 Copyright (c) telechips, Inc.
 *	 ALL RIGHTS RESERVED
 *
****************************************************************************/

#define TRUE 		1
#define FALSE 		0

static int tccxxx_vioc_drv_mmap(struct file *file, struct vm_area_struct *vma);
long tccxxx_vioc_drv_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int tccxxx_vioc_drv_release(struct inode *inode, struct file *filp);
int tccxxx_vioc_drv_open(struct inode *inode, struct file *filp);


