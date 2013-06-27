/* drivers/video/msm/logo.c
 *
 * Show Logo in RLE 565 format
 *
 * Copyright (C) 2008 Google Incorporated
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fb.h>
#include <linux/vt_kern.h>
#include <linux/unistd.h>
#include <linux/syscalls.h>

#include <linux/irq.h>
#include <asm/system.h>

#define fb_width(fb)	((fb)->var.xres)
#define fb_height(fb)	((fb)->var.yres)
#define fb_size(fb)	((fb)->var.xres * (fb)->var.yres * 2)

static void memset16(void *_ptr, unsigned short val, unsigned count)
{
	unsigned short *ptr = _ptr;
	count >>= 1;
	while (count--)
		*ptr++ = val;
}
#if 0

unsigned short *data; // file read data memory pointer
unsigned count;

/* 565RLE image format: [count(2 bytes), rle(2 bytes)] */
int load_565rle_image(char *filename)
{
	struct fb_info *info;
	int fd, err = 0;
	unsigned count, max;
	unsigned short *data, *bits, *ptr;

	printk(KERN_WARNING "~~  %s:  %s\n",__func__, filename);
	info = registered_fb[0];
	if (!info) {
		printk(KERN_WARNING "%s: Can not access framebuffer\n",
			__func__);
		return -ENODEV;
	}

	fd = sys_open(filename, O_RDONLY, 0);
	if (fd < 0) {
		printk(KERN_WARNING "%s: Can not open %s\n",
			__func__, filename);
		return -ENOENT;
	}
	count = (unsigned)sys_lseek(fd, (off_t)0, 2);
	if (count == 0) {
		sys_close(fd);
		err = -EIO;
		goto err_logo_close_file;
	}
	sys_lseek(fd, (off_t)0, 0);
	data = kmalloc(count, GFP_KERNEL);
	if (!data) {
		printk(KERN_WARNING "%s: Can not alloc data\n", __func__);
		err = -ENOMEM;
		goto err_logo_close_file;
	}
	if ((unsigned)sys_read(fd, (char *)data, count) != count) {
		err = -EIO;
		goto err_logo_free_data;
	}

	max = fb_width(info) * fb_height(info);
	ptr = data;
	bits = (unsigned short *)(info->screen_base);
	while (count > 3) {
		unsigned n = ptr[0];
		if (n > max)
			break;
		memset16(bits, ptr[1], n << 1);
		bits += n;
		max -= n;
		ptr += 2;
		count -= 4;
	}

err_logo_free_data:
	kfree(data);

err_logo_close_file:
	sys_close(fd);

	printk(KERN_WARNING "~~  %s:  %s  err:%d end \n",__func__, filename, err);

	return err;
}
EXPORT_SYMBOL(load_565rle_image);
#endif//

#if 1
unsigned short *data; // file read data memory pointer
unsigned count;

/* 565RLE image format: [count(2 bytes), rle(2 bytes)] */
int load_565rle_image(char *filename)
{
	int fd, err = 0;
	printk(KERN_WARNING "~~  %s:  %s   \n",__func__, filename);

	data = NULL;
	if( (fd = sys_open(filename, O_RDONLY, 0)) < 0 ) {
		printk(KERN_WARNING "%s: Can not open %s\n", 
			__FUNCTION__, filename);
		return -ENOENT;
	}
	if( (count = (unsigned)sys_lseek(fd, (off_t)0, 2)) == 0 ) {
		sys_close(fd);
		err = -EIO;
		goto err_logo_close_file;
	}

	sys_lseek(fd, (off_t)0, 0);
	data = (unsigned short *)kmalloc(count, GFP_KERNEL);
	if( !data ) {
		printk(KERN_WARNING "%s: Can not alloc data\n", __FUNCTION__);
		err = -ENOMEM;
		goto err_logo_close_file;
	}

	if( (unsigned)sys_read(fd, (char *)data, count) != count ) {
		err = -EIO;
	}

err_logo_close_file:	
	sys_close(fd);
	return err;
}
EXPORT_SYMBOL(load_565rle_image);
#endif

int load_image_display(void)
{
	struct fb_info *info;
	unsigned max;
	unsigned short  *bits, *ptr;

	printk(KERN_WARNING "%s:  \n", __FUNCTION__);

	info = registered_fb[0];
	if( !info ) {
		printk(KERN_WARNING "%s: Can not access framebuffer\n", __FUNCTION__);
		return -ENODEV;
	}
	
	max = fb_width(info) * fb_height(info);
	ptr = data;
	//bits = (unsigned short *)(info->screen_base) + (info->var.xres *info->var.yoffset * (info->var.bits_per_pixel/8));
	bits = (unsigned short *)(info->screen_base) ;
	while (count > 3) {
		unsigned n = ptr[0];
		
		if (n > max)
			break;
		
		memset16(bits, ptr[1], n << 1);
		bits += n;
		max -= n;
		ptr += 2;
		count -= 4;
	}
	return 0;
}
EXPORT_SYMBOL(load_image_display);

int load_image_free(void)
{
	if(data != NULL)	{
		printk(KERN_WARNING "%s:  \n", __FUNCTION__);
		kfree(data);
		return 0;
	}
	printk(KERN_WARNING "%s : ERROR  \n", __FUNCTION__);
	return (-ENOMEM);
}
EXPORT_SYMBOL(load_image_free);
