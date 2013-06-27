/*
 * linux/kernel/power/swap.c
 *
 * This file provides functions for reading the suspend image from
 * and writing it to a swap partition.
 *
 * Copyright (C) 1998,2001-2005 Pavel Machek <pavel@ucw.cz>
 * Copyright (C) 2006 Rafael J. Wysocki <rjw@sisk.pl>
 * Copyright (C) 2010 Bojan Smojver <bojan@rexursive.com>
 *
 * This file is released under the GPLv2.
 *
 */

#include <linux/module.h>
#include <linux/file.h>
#include <linux/delay.h>
#include <linux/bitops.h>
#include <linux/genhd.h>
#include <linux/device.h>
#include <linux/buffer_head.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/lzo.h>
#include <linux/csnappy.h>
#include <linux/lz4.h>
#include <linux/lz4hc.h>
#include <linux/vmalloc.h>

#include "power.h"

#define HIBERNATE_SIG	"S1SUSPEND"

/*
 *	The swap map is a data structure used for keeping track of each page
 *	written to a swap partition.  It consists of many swap_map_page
 *	structures that contain each an array of MAP_PAGE_ENTRIES swap entries.
 *	These structures are stored on the swap and linked together with the
 *	help of the .next_swap member.
 *
 *	The swap map is created during suspend.  The swap map pages are
 *	allocated and populated one at a time, so we only need one memory
 *	page to set up the entire structure.
 *
 *	During resume we also only need to use one swap_map_page structure
 *	at a time.
 */

#define MAP_PAGE_ENTRIES	(PAGE_SIZE / sizeof(sector_t) - 1)

struct swap_map_page {
	sector_t entries[MAP_PAGE_ENTRIES];
	sector_t next_swap;
};

/**
 *	The swap_map_handle structure is used for handling swap in
 *	a file-alike way
 */

struct swap_map_handle {
	struct swap_map_page *cur;
	sector_t cur_swap;
	sector_t first_sector;
	unsigned int k;
};
#ifdef CONFIG_SNAPSHOT_BOOT
#define BSIZE (sizeof(unsigned int) * 100)
#endif
struct swsusp_header {
#ifdef CONFIG_SNAPSHOT_BOOT
	char reserved[PAGE_SIZE - 20 - BSIZE - sizeof(sector_t) - sizeof(int)];
#else
	char reserved[PAGE_SIZE - 20 - sizeof(sector_t) - sizeof(int)];
#endif
	sector_t image;
	unsigned int flags;	/* Flags to pass to the "boot" kernel */
#ifdef CONFIG_SNAPSHOT_BOOT
	unsigned int reg[100]; 	
#endif
	//unsigned int ptable[7];	
	char	orig_sig[10];
	char	sig[10];
} __attribute__((packed));

static struct swsusp_header *swsusp_header;

/**
 *	The following functions are used for tracing the allocated
 *	swap pages, so that they can be freed in case of an error.
 */

struct swsusp_extent {
	struct rb_node node;
	unsigned long start;
	unsigned long end;
};

static struct rb_root swsusp_extents = RB_ROOT;

static int swsusp_extents_insert(unsigned long swap_offset)
{
	struct rb_node **new = &(swsusp_extents.rb_node);
	struct rb_node *parent = NULL;
	struct swsusp_extent *ext;

	/* Figure out where to put the new node */
	while (*new) {
		ext = container_of(*new, struct swsusp_extent, node);
		parent = *new;
		if (swap_offset < ext->start) {
			/* Try to merge */
			if (swap_offset == ext->start - 1) {
				ext->start--;
				return 0;
			}
			new = &((*new)->rb_left);
		} else if (swap_offset > ext->end) {
			/* Try to merge */
			if (swap_offset == ext->end + 1) {
				ext->end++;
				return 0;
			}
			new = &((*new)->rb_right);
		} else {
			/* It already is in the tree */
			return -EINVAL;
		}
	}
	/* Add the new node and rebalance the tree. */
	ext = kzalloc(sizeof(struct swsusp_extent), GFP_KERNEL);
	if (!ext)
		return -ENOMEM;

	ext->start = swap_offset;
	ext->end = swap_offset;
	rb_link_node(&ext->node, parent, new);
	rb_insert_color(&ext->node, &swsusp_extents);
	return 0;
}

/**
 *	alloc_swapdev_block - allocate a swap page and register that it has
 *	been allocated, so that it can be freed in case of an error.
 */

sector_t alloc_swapdev_block(int swap)
{
	unsigned long offset;

	offset = swp_offset(get_swap_page_of_type(swap));
	if (offset) {
		if (swsusp_extents_insert(offset))
			swap_free(swp_entry(swap, offset));
		else
			return swapdev_block(swap, offset);
	}
	return 0;
}

/**
 *	free_all_swap_pages - free swap pages allocated for saving image data.
 *	It also frees the extents used to register which swap entries had been
 *	allocated.
 */

void free_all_swap_pages(int swap)
{
	struct rb_node *node;

	while ((node = swsusp_extents.rb_node)) {
		struct swsusp_extent *ext;
		unsigned long offset;

		ext = container_of(node, struct swsusp_extent, node);
		rb_erase(node, &swsusp_extents);
		for (offset = ext->start; offset <= ext->end; offset++)
			swap_free(swp_entry(swap, offset));

		kfree(ext);
	}
}

int swsusp_swap_in_use(void)
{
	return (swsusp_extents.rb_node != NULL);
}

/*
 * General things
 */

static unsigned short root_swap = 0xffff;
struct block_device *hib_resume_bdev;

/*
 * Saving part
 */
#ifdef CONFIG_SNAPSHOT_BOOT
static unsigned int *pSave;
extern int save_cpu_reg_for_snapshot(unsigned int ptable, unsigned int pReg, void *);
extern void restore_snapshot_cpu_reg(void);
extern unsigned int reg[100]; 
#endif
static int mark_swapfiles(struct swap_map_handle *handle, unsigned int flags)
{
	int error;

	//printk("\n\n%s swsusp_resume_block[%ld]\n", __FUNCTION__,swsusp_resume_block);
#ifdef CONFIG_SNAPSHOT_BOOT
	unsigned long flag;	
	//hib_bio_read_page(swsusp_resume_block, swsusp_header, NULL);
#endif
	hib_bio_read_page(swsusp_resume_block, swsusp_header, 1);
	if (!memcmp("SWAP-SPACE",swsusp_header->sig, 10) ||
	    !memcmp("SWAPSPACE2",swsusp_header->sig, 10)) {
		memcpy(swsusp_header->orig_sig,swsusp_header->sig, 10);
		memcpy(swsusp_header->sig, HIBERNATE_SIG, 10);
#ifdef CONFIG_SNAPSHOT_BOOT
			local_irq_save(flag);
			local_irq_disable();		
			memcpy(swsusp_header->reg, reg, sizeof(reg));
			local_irq_enable();	
			local_irq_restore(flag);
#endif
		swsusp_header->image = handle->first_sector;
		swsusp_header->flags = flags;
		error = hib_bio_write_page(swsusp_resume_block,
					//swsusp_header, NULL);//B090162
					swsusp_header, 1);
		printk("\n%s swsusp_header[%p] %s\n", __FUNCTION__, (unsigned long)&swsusp_header, swsusp_header->sig);
	} else {
		printk(KERN_ERR "PM: Swap header not found!\n");
		error = -ENODEV;
	}
	return error;
}

/**
 *	swsusp_swap_check - check if the resume device is a swap device
 *	and get its index (if so)
 *
 *	This is called before saving image
 */
static int swsusp_swap_check(void)
{
	int res;

	res = swap_type_of(swsusp_resume_device, swsusp_resume_block,
			&hib_resume_bdev);
	if (res < 0)
		return res;

	root_swap = res;
	res = blkdev_get(hib_resume_bdev, FMODE_WRITE, NULL);
	if (res)
		return res;

	res = set_blocksize(hib_resume_bdev, PAGE_SIZE);
	if (res < 0)
		blkdev_put(hib_resume_bdev, FMODE_WRITE);

	return res;
}

/**
 *	write_page - Write one page to given swap location.
 *	@buf:		Address we're writing.
 *	@offset:	Offset of the swap page we're writing to.
 *	@bio_chain:	Link the next write BIO here
 */

//static int write_page(void *buf, sector_t offset, struct bio **bio_chain)
static int write_page(void *buf, sector_t offset, int sync)
{
	void *src;

	if (!offset)
		return -ENOSPC;

	//if (bio_chain) {
	if(!sync) {
		src = (void *)__get_free_page(__GFP_WAIT | __GFP_HIGH);
		if (src) {
			copy_page(src, buf);
		} else {
			WARN_ON_ONCE(1);
			//bio_chain = NULL;	/* Go synchronous */
			sync = 1;	/* Go synchronous */
			src = buf;
		}
	} else {
		src = buf;
	}
	//return hib_bio_write_page(offset, src, bio_chain);
	return hib_bio_write_page(offset, src, sync);
}

static void release_swap_writer(struct swap_map_handle *handle)
{
	if (handle->cur)
		free_page((unsigned long)handle->cur);
	handle->cur = NULL;
}

static int get_swap_writer(struct swap_map_handle *handle)
{
	int ret;

	ret = swsusp_swap_check();
	if (ret) {
		if (ret != -ENOSPC)
			printk(KERN_ERR "PM: Cannot find swap device, try "
					"swapon -a.\n");
		return ret;
	}
	handle->cur = (struct swap_map_page *)get_zeroed_page(GFP_KERNEL);
	if (!handle->cur) {
		ret = -ENOMEM;
		goto err_close;
	}
	handle->cur_swap = alloc_swapdev_block(root_swap);
	if (!handle->cur_swap) {
		ret = -ENOSPC;
		goto err_rel;
	}
	handle->k = 0;
	handle->first_sector = handle->cur_swap;
	return 0;
err_rel:
	release_swap_writer(handle);
err_close:
	swsusp_close(FMODE_WRITE);
	return ret;
}

static int swap_write_page(struct swap_map_handle *handle, void *buf,
				//struct bio **bio_chain)
				int sync)
{
	int error = 0;
	sector_t offset;

	if (!handle->cur)
		return -EINVAL;
	offset = alloc_swapdev_block(root_swap);
	//error = write_page(buf, offset, bio_chain);
	error = write_page(buf, offset, sync);
	if (error)
		return error;
	handle->cur->entries[handle->k++] = offset;
	if (handle->k >= MAP_PAGE_ENTRIES) {
		//error = hib_wait_on_bio_chain(bio_chain);
		error = hib_wait_on_bio_chain();
		if (error)
			goto out;
		offset = alloc_swapdev_block(root_swap);
		if (!offset)
			return -ENOSPC;
		handle->cur->next_swap = offset;
		//error = write_page(handle->cur, handle->cur_swap, NULL);
		error = write_page(handle->cur, handle->cur_swap, 1);
		if (error)
			goto out;
		clear_page(handle->cur);
		handle->cur_swap = offset;
		handle->k = 0;
	}
 out:
	return error;
}

static int flush_swap_writer(struct swap_map_handle *handle)
{
	if (handle->cur && handle->cur_swap)
		return write_page(handle->cur, handle->cur_swap, 1);
	else
		return -EINVAL;
}

static int swap_writer_finish(struct swap_map_handle *handle,
		unsigned int flags, int error)
{
	if (!error) {
		flush_swap_writer(handle);
		printk(KERN_INFO "PM: S");
		error = mark_swapfiles(handle, flags);
		printk("|\n");
		flush_swap_writer(handle);
	}

	if (error)
		free_all_swap_pages(root_swap);
	release_swap_writer(handle);
	swsusp_close(FMODE_WRITE);

	return error;
}

/* We need to remember how much compressed data we need to read. */
#define LZO_HEADER	sizeof(size_t)

/* Number of pages/bytes we'll compress at one time. */
#define LZO_UNC_PAGES	32
#define LZO_UNC_SIZE	(LZO_UNC_PAGES * PAGE_SIZE)

/* Number of pages/bytes we need for compressed data (worst case). */
#define LZO_CMP_PAGES	DIV_ROUND_UP(lzo1x_worst_compress(LZO_UNC_SIZE) + \
			             LZO_HEADER, PAGE_SIZE)
#define LZO_CMP_SIZE	(LZO_CMP_PAGES * PAGE_SIZE)

/**
 *	save_image - save the suspend image data
 */

static int save_image(struct swap_map_handle *handle,
                      struct snapshot_handle *snapshot,
                      unsigned int nr_to_write)
{
	unsigned int m;
	int ret;
	int nr_pages;
	int err2;
	//struct bio *bio;
	struct timeval start;
	struct timeval stop;

	printk(KERN_INFO "PM: Saving image data pages (%u pages) ...     ",
		nr_to_write);
	m = nr_to_write / 100;
	if (!m)
		m = 1;
	nr_pages = 0;
	//bio = NULL;
	do_gettimeofday(&start);
	printk("\nstart first_sector[%lld] cur_swap[%lld]\n", handle->first_sector, handle->cur_swap);
	while (1) {
		ret = snapshot_read_next(snapshot);
		if (ret <= 0)
			break;
		//ret = swap_write_page(handle, data_of(*snapshot), &bio);
		ret = swap_write_page(handle, data_of(*snapshot), 1);
		if (ret)
			break;
		if (!(nr_pages % m))
			printk(KERN_CONT "\b\b\b\b%3d%%", nr_pages / m);
		nr_pages++;
	}
	printk("end first_sector[%lld] cur_swap[%lld]\n", handle->first_sector, handle->cur_swap);
	//err2 = hib_wait_on_bio_chain(&bio);
	err2 = hib_wait_on_bio_chain();
	do_gettimeofday(&stop);
	if (!ret)
		ret = err2;
	if (!ret)
		printk(KERN_CONT "\b\b\b\bdone\n");
	else
		printk(KERN_CONT "\n");
	swsusp_show_speed(&start, &stop, nr_to_write, "Wrote");
	return ret;
}


/**
 * save_image_lzo - Save the suspend image data compressed with LZO.
 * @handle: Swap mam handle to use for saving the image.
 * @snapshot: Image to read data from.
 * @nr_to_write: Number of pages to save.
 */
static int save_image_lzo(struct swap_map_handle *handle,
                          struct snapshot_handle *snapshot,
                          unsigned int nr_to_write)
{
	unsigned int m;
	int ret = 0;
	int nr_pages;
	int err2;
	//struct bio *bio;
	struct timeval start;
	struct timeval stop;
	size_t off, unc_len, cmp_len;
	unsigned char *unc, *cmp, *wrk, *page;

	page = (void *)__get_free_page(__GFP_WAIT | __GFP_HIGH);
	if (!page) {
		printk(KERN_ERR "PM: Failed to allocate LZO page\n");
		return -ENOMEM;
	}

	wrk = vmalloc(LZO1X_1_MEM_COMPRESS);
	if (!wrk) {
		printk(KERN_ERR "PM: Failed to allocate LZO workspace\n");
		free_page((unsigned long)page);
		return -ENOMEM;
	}

	unc = vmalloc(LZO_UNC_SIZE);
	if (!unc) {
		printk(KERN_ERR "PM: Failed to allocate LZO uncompressed\n");
		vfree(wrk);
		free_page((unsigned long)page);
		return -ENOMEM;
	}

	cmp = vmalloc(LZO_CMP_SIZE);
	if (!cmp) {
		printk(KERN_ERR "PM: Failed to allocate LZO compressed\n");
		vfree(unc);
		vfree(wrk);
		free_page((unsigned long)page);
		return -ENOMEM;
	}

	printk(KERN_INFO
		"PM: Compressing and saving image data (%u pages) ...     ",
		nr_to_write);
	m = nr_to_write / 100;
	if (!m)
		m = 1;
	nr_pages = 0;
	//bio = NULL;
	do_gettimeofday(&start);
	for (;;) {
		for (off = 0; off < LZO_UNC_SIZE; off += PAGE_SIZE) {
			ret = snapshot_read_next(snapshot);
			if (ret < 0)
				goto out_finish;

			if (!ret)
				break;

			memcpy(unc + off, data_of(*snapshot), PAGE_SIZE);

			if (!(nr_pages % m))
				printk(KERN_CONT "\b\b\b\b%3d%%", nr_pages / m);
			nr_pages++;
		}

		if (!off)
			break;

		unc_len = off;
		ret = lzo1x_1_compress(unc, unc_len,
		                       cmp + LZO_HEADER, &cmp_len, wrk);
		if (ret < 0) {
			printk(KERN_ERR "PM: LZO compression failed\n");
			break;
		}

		if (unlikely(!cmp_len ||
		             cmp_len > lzo1x_worst_compress(unc_len))) {
			printk(KERN_ERR "PM: Invalid LZO compressed length\n");
			ret = -1;
			break;
		}

		*(size_t *)cmp = cmp_len;

		/*
		 * Given we are writing one page at a time to disk, we copy
		 * that much from the buffer, although the last bit will likely
		 * be smaller than full page. This is OK - we saved the length
		 * of the compressed data, so any garbage at the end will be
		 * discarded when we read it.
		 */
		for (off = 0; off < LZO_HEADER + cmp_len; off += PAGE_SIZE) {
			memcpy(page, cmp + off, PAGE_SIZE);

			//ret = swap_write_page(handle, page, &bio);
			ret = swap_write_page(handle, page, 1);
			if (ret)
				goto out_finish;
		}
	}

out_finish:
	err2 = hib_wait_on_bio_chain();
//	err2 = hib_wait_on_bio_chain(&bio);//B090162
	do_gettimeofday(&stop);
	if (!ret)
		ret = err2;
	if (!ret)
		printk(KERN_CONT "\b\b\b\bdone\n");
	else
		printk(KERN_CONT "\n");
	swsusp_show_speed(&start, &stop, nr_to_write, "Wrote");

	vfree(cmp);
	vfree(unc);
	vfree(wrk);
	free_page((unsigned long)page);

	return ret;
}


#if defined(CONFIG_SNAPPY_COMPRESS) && defined (CONFIG_SNAPPY_DECOMPRESS)
/* Number of pages/bytes we'll compress at one time. */
#define SNAPPY_UNC_PAGES	32
#define SNAPPY_UNC_SIZE	(SNAPPY_UNC_PAGES * PAGE_SIZE)
#define SNAPPY_HEADER	sizeof(size_t)

/**
 * save_image_snappy - Save the suspend image data compressed with SNAPPY.
 * @handle: Swap mam handle to use for saving the image.
 * @snapshot: Image to read data from.
 * @nr_to_write: Number of pages to save.
 */
static int save_image_snappy(struct swap_map_handle *handle,
                          struct snapshot_handle *snapshot,
                          unsigned int nr_to_write)
{
	unsigned int m;
	int ret = 0;
	int nr_pages;
	int snappy_nr_to_write = 0;
	int err2;
	//struct bio *bio;
	struct timeval start;
	struct timeval stop;
	size_t off, unc_len, cmp_len;
	unsigned char *unc, *cmp, *wrk, *page;
	uint32_t max_compressed_len;

    printk("%s\n",__FUNCTION__);

	page = (void *)__get_free_page(__GFP_WAIT | __GFP_HIGH);
	if (!page) {
		printk(KERN_ERR "PM: Failed to allocate SNAPPY page\n");
		return -ENOMEM;
	}

	wrk = vmalloc(CSNAPPY_WORKMEM_BYTES);
	if (!wrk) {
		printk(KERN_ERR "PM: Failed to allocate SNAPPY workspace\n");
		free_page((unsigned long)page);
		return -ENOMEM;
	}

	unc = vmalloc(SNAPPY_UNC_SIZE);
	if (!unc) {
		printk(KERN_ERR "PM: Failed to allocate SNAPPY uncompressed\n");
		vfree(wrk);
		free_page((unsigned long)page);
		return -ENOMEM;
	}

	max_compressed_len = csnappy_max_compressed_length(SNAPPY_UNC_SIZE);

	cmp = vmalloc(max_compressed_len + SNAPPY_HEADER);
	if (!cmp) {
		printk(KERN_ERR "PM: Failed to allocate SNAPPY compressed\n");
		vfree(unc);
		vfree(wrk);
		free_page((unsigned long)page);
		return -ENOMEM;
	}

	printk(KERN_INFO
		"PM: Compressing and saving image data (%u pages) ...     ",
		nr_to_write);
	m = nr_to_write / 100;
	if (!m)
		m = 1;
	nr_pages = 0;
	//bio = NULL;
	do_gettimeofday(&start);
	for (;;) {
		for (off = 0; off < SNAPPY_UNC_SIZE; off += PAGE_SIZE) {
			ret = snapshot_read_next(snapshot);
			if (ret < 0)
				goto out_finish;

			if (!ret)
				break;

			memcpy(unc + off, data_of(*snapshot), PAGE_SIZE);

			if (!(nr_pages % m))
				printk(KERN_CONT "\b\b\b\b%3d%%", nr_pages / m);
			nr_pages++;
		}

		if (!off)
			break;

		unc_len = off;

		
		csnappy_compress(unc, unc_len,
		                       cmp + SNAPPY_HEADER, &cmp_len, wrk, CSNAPPY_WORKMEM_BYTES_POWER_OF_TWO);

		if (unlikely(!cmp_len ||
		             cmp_len > csnappy_max_compressed_length(unc_len))) {
			printk(KERN_ERR "PM: Invalid snappy compressed length\n");
			ret = -1;
			break;
		}

		*(size_t *)cmp = cmp_len;

		/*
		 * Given we are writing one page at a time to disk, we copy
		 * that much from the buffer, although the last bit will likely
		 * be smaller than full page. This is OK - we saved the length
		 * of the compressed data, so any garbage at the end will be
		 * discarded when we read it.
		 */
		for (off = 0; off < SNAPPY_HEADER + cmp_len; off += PAGE_SIZE) {
			memcpy(page, cmp + off, PAGE_SIZE);

			//ret = swap_write_page(handle, page, &bio);
			ret = swap_write_page(handle, page, 1);

			snappy_nr_to_write ++;
			if (ret)
				goto out_finish;
		}
	}

out_finish:
	err2 = hib_wait_on_bio_chain();
	do_gettimeofday(&stop);
	if (!ret)
		ret = err2;
	if (!ret)
		printk(KERN_CONT "\b\b\b\bdone\n");
	else
		printk(KERN_CONT "\n");
	swsusp_show_speed(&start, &stop, snappy_nr_to_write, "compress snappy & Wrote");


	vfree(cmp);
	vfree(unc);
	vfree(wrk);
	free_page((unsigned long)page);

	return ret;
}
#endif


#if (defined(CONFIG_LZ4_COMPRESS) || defined(CONFIG_LZ4_HC_COMPRESS)) && defined (CONFIG_LZ4_DECOMPRESS)
/* Number of pages/bytes we'll compress at one time. */
#define LZ4_UNC_PAGES  32
#define LZ4_UNC_SIZE   (LZ4_UNC_PAGES * PAGE_SIZE)
#define LZ4_HEADER     4

/**
 * save_image_lz4 - Save the suspend image data compressed with LZ4.
 * @handle: Swap mam handle to use for saving the image.
 * @snapshot: Image to read data from.
 * @nr_to_write: Number of pages to save.
 */
static int save_image_lz4(struct swap_map_handle *handle,
                          struct snapshot_handle *snapshot,
                          unsigned int nr_to_write)
{
	unsigned int m;
	int ret = 0;
	int nr_pages;
	int lz4_nr_to_write = 0;
	int err2;
	//struct bio *bio;
	struct timeval start;
	struct timeval stop;
	int off, unc_len, cmp_len;
	unsigned char *unc, *cmp, *page, *hashTable;
	uint32_t max_compressed_len;

	page = (void *)__get_free_page(__GFP_WAIT | __GFP_HIGH);
	if (!page) {
		printk(KERN_ERR "PM: Failed to allocate LZ4 page\n");
		return -ENOMEM;
	}

#if defined(CONFIG_LZ4_HC_COMPRESS)
	hashTable = vmalloc(sizeof(LZ4HC_Data_Structure));
#else
	hashTable = vmalloc(PAGE_SIZE*sizeof(uint32_t));
#endif
	if (!hashTable) {
		printk(KERN_ERR "PM: Failed to allocate LZ4 hashTable\n");
		free_page((unsigned long)page);
		return -ENOMEM;
	}

	unc = vmalloc(LZ4_UNC_SIZE);
	if (!unc) {
		printk(KERN_ERR "PM: Failed to allocate LZ4 uncompressed\n");
		free_page((unsigned long)page);
		free_page((unsigned long)hashTable);
		return -ENOMEM;
	}

	max_compressed_len = LZ4_compressBound(LZ4_UNC_SIZE);

	cmp = vmalloc(max_compressed_len);
	if (!cmp) {
		printk(KERN_ERR "PM: Failed to allocate LZ4 compressed\n");
		vfree(unc);
		free_page((unsigned long)page);
		free_page((unsigned long)hashTable);
		return -ENOMEM;
	}

	printk(KERN_INFO
		"PM: Compressing and saving image data (%u pages) ...     ",
		nr_to_write);
	m = nr_to_write / 100;
	if (!m)
		m = 1;
	nr_pages = 0;
	//bio = NULL;
	do_gettimeofday(&start);
	for (;;) {
		for (off = 0; off < LZ4_UNC_SIZE; off += PAGE_SIZE) {
			ret = snapshot_read_next(snapshot);
			if (ret < 0)
				goto out_finish;

			if (!ret)
				break;

			memcpy(unc + off, data_of(*snapshot), PAGE_SIZE);

			if (!(nr_pages % m))
				printk(KERN_CONT "\b\b\b\b%3d%%", nr_pages / m);
			nr_pages++;
		}

		if (!off)
			break;

		unc_len = off;

        memset(hashTable, 0, PAGE_SIZE*sizeof(uint32_t));
#if defined(CONFIG_LZ4_HC_COMPRESS)
		cmp_len = LZ4_compressHC(unc, cmp+LZ4_HEADER, unc_len, hashTable);
#else
		cmp_len = LZ4_compress(unc, cmp+LZ4_HEADER, unc_len, hashTable);
#endif

//		if (unlikely(!cmp_len ||
//		             cmp_len > LZ4_compressBound(unc_len))) {
        if (unlikely(cmp_len < 0)) {
			printk(KERN_ERR "PM: Invalid snappy compressed length\n");
			ret = -1;
			break;
		}

		*(unsigned int *)cmp = cmp_len;

		/*
		 * Given we are writing one page at a time to disk, we copy
		 * that much from the buffer, although the last bit will likely
		 * be smaller than full page. This is OK - we saved the length
		 * of the compressed data, so any garbage at the end will be
		 * discarded when we read it.
		 */
		for (off = 0; off < LZ4_HEADER + cmp_len; off += PAGE_SIZE) {
			memcpy(page, cmp + off, PAGE_SIZE);

			//ret = swap_write_page(handle, page, &bio);
			ret = swap_write_page(handle, page, 1);

			lz4_nr_to_write ++;
			if (ret)
				goto out_finish;
		}
	}

out_finish:
	err2 = hib_wait_on_bio_chain();
	do_gettimeofday(&stop);
	if (!ret)
		ret = err2;
	if (!ret)
		printk(KERN_CONT "\b\b\b\bdone\n");
	else
		printk(KERN_CONT "\n");
	swsusp_show_speed(&start, &stop, lz4_nr_to_write, "compress lz4 & Wrote");


	vfree(cmp);
	vfree(unc);
	free_page((unsigned long)page);
	free_page((unsigned long)hashTable);

	return ret;
}
#endif

/**
 *	enough_swap - Make sure we have enough swap to save the image.
 *
 *	Returns TRUE or FALSE after checking the total amount of swap
 *	space avaiable from the resume partition.
 */

static int enough_swap(unsigned int nr_pages, unsigned int flags)
{
	unsigned int free_swap = count_swap_pages(root_swap, 1);
	unsigned int required;

	pr_debug("PM: Free swap pages: %u\n", free_swap);

#if defined(CONFIG_SNAPPY_COMPRESS) && defined (CONFIG_SNAPPY_DECOMPRESS)
    if( flags & SF_NOCOMPRESS_MODE ) {
        required = PAGES_FOR_IO + nr_pages;
    } else {
        /*
         required = (nr_pages + SNAPPY_UNC_PAGES - 1) / SNAPPY_UNC_PAGES;
         required *= (csnappy_max_compressed_length(SNAPPY_UNC_SIZE) + PAGE_SIZE - 1) / PAGE_SIZE;
         required += PAGES_FOR_IO + 1;
         */

         required = PAGES_FOR_IO + nr_pages / 2;
    }
#else
	required = PAGES_FOR_IO + ((flags & SF_NOCOMPRESS_MODE) ?
		nr_pages : ((nr_pages * LZO_CMP_PAGES) / LZO_UNC_PAGES + 1))/2;
#endif

//	printk("PM: Free swap pages: %u required swap pages %u %u %u\n", free_swap, required, nr_pages, csnappy_max_compressed_length(SNAPPY_UNC_SIZE));
	return free_swap > required;
}

/**
 *	swsusp_write - Write entire image and metadata.
 *	@flags: flags to pass to the "boot" kernel in the image header
 *
 *	It is important _NOT_ to umount filesystems at this point. We want
 *	them synced (in case something goes wrong) but we DO not want to mark
 *	filesystem clean: it is not. (And it does not matter, if we resume
 *	correctly, we'll mark system clean, anyway.)
 */

int swsusp_write(unsigned int flags)
{
	struct swap_map_handle handle;
	struct snapshot_handle snapshot;
	struct swsusp_info *header;
	unsigned long pages;
	int error;

	pages = snapshot_get_image_size();
	error = get_swap_writer(&handle);
	if (error) {
		printk(KERN_ERR "PM: Cannot get swap writer\n");
		return error;
	}
	if (!enough_swap(pages, flags)) {
		printk(KERN_ERR "PM: Not enough free swap\n");
		error = -ENOSPC;
		goto out_finish;
	}
	memset(&snapshot, 0, sizeof(struct snapshot_handle));
	error = snapshot_read_next(&snapshot);
	if (error < PAGE_SIZE) {
		if (error >= 0)
			error = -EFAULT;

		goto out_finish;
	}
	header = (struct swsusp_info *)data_of(snapshot);
	//error = swap_write_page(&handle, header, NULL);
	error = swap_write_page(&handle, header, 1);
	if (!error) {
		error = (flags & SF_NOCOMPRESS_MODE) ?
			save_image(&handle, &snapshot, pages - 1) :

        #if (defined(CONFIG_LZ4_COMPRESS) || defined(CONFIG_LZ4_HC_COMPRESS)) && defined (CONFIG_LZ4_DECOMPRESS)	
   			save_image_lz4(&handle, &snapshot, pages - 1);
        #elif defined(CONFIG_SNAPPY_COMPRESS) && defined (CONFIG_SNAPPY_DECOMPRESS)	
   			save_image_snappy(&handle, &snapshot, pages - 1);
        #else    			
			save_image_lzo(&handle, &snapshot, pages - 1);
        #endif
	}
out_finish:
	error = swap_writer_finish(&handle, flags, error);
	return error;
}

/**
 *	The following functions allow us to read data using a swap map
 *	in a file-alike way
 */

static void release_swap_reader(struct swap_map_handle *handle)
{
	if (handle->cur)
		free_page((unsigned long)handle->cur);
	handle->cur = NULL;
}

static int get_swap_reader(struct swap_map_handle *handle,
		unsigned int *flags_p)
{
	int error;

	*flags_p = swsusp_header->flags;

	if (!swsusp_header->image) /* how can this happen? */
		return -EINVAL;

	handle->cur = (struct swap_map_page *)get_zeroed_page(__GFP_WAIT | __GFP_HIGH);
	if (!handle->cur)
		return -ENOMEM;

	//error = hib_bio_read_page(swsusp_header->image, handle->cur, NULL);
	error = hib_bio_read_page(swsusp_header->image, handle->cur, 1);
	if (error) {
		release_swap_reader(handle);
		return error;
	}
	handle->k = 0;
	return 0;
}

static int swap_read_page(struct swap_map_handle *handle, void *buf,
				//struct bio **bio_chain)
				int sync)
{
	sector_t offset;
	int error;

	if (!handle->cur)
		return -EINVAL;
	offset = handle->cur->entries[handle->k];
	if (!offset)
		return -EFAULT;
	//error = hib_bio_read_page(offset, buf, bio_chain);
	error = hib_bio_read_page(offset, buf, sync);
	if (error)
		return error;
	if (++handle->k >= MAP_PAGE_ENTRIES) {
		//error = hib_wait_on_bio_chain(bio_chain);
		error = hib_wait_on_bio_chain();
		handle->k = 0;
		offset = handle->cur->next_swap;
		if (!offset)
			release_swap_reader(handle);
		else if (!error)
			//error = hib_bio_read_page(offset, handle->cur, NULL);
			error = hib_bio_read_page(offset, handle->cur, 1);
	}
	return error;
}

static int swap_read_multi_pages(struct swap_map_handle *handle, void *buf, int page_count)
{
	sector_t offset;
	int error;
	int bio_readable_count = 0;
	int bio_remaind_count = page_count;
	unsigned char *read_buffer = (unsigned char *)buf;

	start_swap_read_multi_pages:

	if (!handle->cur)
		return -EINVAL;
	offset = handle->cur->entries[handle->k];
	if (!offset)
		return -EFAULT;

	bio_readable_count = MAP_PAGE_ENTRIES - handle->k;

	if( bio_readable_count >  bio_remaind_count ) {
	    bio_readable_count = bio_remaind_count;
	}

	//printk("bio_readable_count[%d]", bio_readable_count);
	error = hib_bio_read_multi_pages(offset, (void*)read_buffer, bio_readable_count, 1);

	//error = hib_bio_read_page(offset, buf, bio_chain);
	//error = hib_bio_read_page(offset, buf, sync);

	if (error)
		return error;

#if 0
   {
	    int i;
	    for(i=0; i<bio_readable_count; i++) {
             int k;

             printk("\noffset[%d] ", (int)offset + i);
             for(k=0; k < 16; k++) {
                 printk("%x, ", ((unsigned char*)read_buffer)[PAGE_SIZE*i + k]);
             }
             printk("\n");
	    }
    }
#endif

	bio_remaind_count -= bio_readable_count;
	handle->k += bio_readable_count;
	read_buffer += bio_readable_count * PAGE_SIZE;

	if (handle->k >= MAP_PAGE_ENTRIES) {
		//error = hib_wait_on_bio_chain(bio_chain);
		error = hib_wait_on_bio_chain();
		handle->k = 0;
		offset = handle->cur->next_swap;
		if (!offset)
			release_swap_reader(handle);
		else if (!error) {
			//error = hib_bio_read_page(offset, handle->cur, NULL);
			error = hib_bio_read_page(offset, handle->cur, 1);

			 if (error)
			     return error;
		}
	}

	if( bio_remaind_count ) {
	    goto start_swap_read_multi_pages;
	}

	return error;
}

static int swap_reader_finish(struct swap_map_handle *handle)
{
	release_swap_reader(handle);

	return 0;
}

/**
 *	load_image - load the image using the swap map handle
 *	@handle and the snapshot handle @snapshot
 *	(assume there are @nr_pages pages to load)
 */

static int load_image(struct swap_map_handle *handle,
                      struct snapshot_handle *snapshot,
                      unsigned int nr_to_read)
{
	unsigned int m;
	int error = 0;
	struct timeval start;
	struct timeval stop;
	//struct bio *bio;
	int err2;
	unsigned nr_pages;

	printk(KERN_INFO "PM: Loading image data pages (%u pages) ...     ",
		nr_to_read);
	m = nr_to_read / 100;
	if (!m)
		m = 1;
	nr_pages = 0;
	//bio = NULL;
	do_gettimeofday(&start);
	for ( ; ; ) {
		error = snapshot_write_next(snapshot);
		if (error <= 0)
			break;
		//error = swap_read_page(handle, data_of(*snapshot), &bio);//B090162
		error = swap_read_page(handle, data_of(*snapshot), 0);
		if (error)
			break;
		if (snapshot->sync_read)
			//error = hib_wait_on_bio_chain(&bio);
			error = hib_wait_on_bio_chain();//B090162
		if (error)
			break;
		if (!(nr_pages % m))
			printk("\b\b\b\b%3d%%", nr_pages / m);
		nr_pages++;
	}
	//err2 = hib_wait_on_bio_chain(&bio);
	err2 = hib_wait_on_bio_chain();
	do_gettimeofday(&stop);
	if (!error)
		error = err2;
	if (!error) {
		printk("\b\b\b\bdone\n");
		snapshot_write_finalize(snapshot);
		if (!snapshot_image_loaded(snapshot))
			error = -ENODATA;
	} else
		printk("\n");
	swsusp_show_speed(&start, &stop, nr_to_read, "Read");
	return error;
}

/**
 * load_image_lzo - Load compressed image data and decompress them with LZO.
 * @handle: Swap map handle to use for loading data.
 * @snapshot: Image to copy uncompressed data into.
 * @nr_to_read: Number of pages to load.
 */
static int load_image_lzo(struct swap_map_handle *handle,
                          struct snapshot_handle *snapshot,
                          unsigned int nr_to_read)
{
	unsigned int m;
	int error = 0;
	//struct bio *bio;
	struct timeval start;
	struct timeval stop;
	unsigned nr_pages;
	size_t i, off, unc_len, cmp_len;
	unsigned char *unc, *cmp, *page[LZO_CMP_PAGES];

	for (i = 0; i < LZO_CMP_PAGES; i++) {
		page[i] = (void *)__get_free_page(__GFP_WAIT | __GFP_HIGH);
		if (!page[i]) {
			printk(KERN_ERR "PM: Failed to allocate LZO page\n");

			while (i)
				free_page((unsigned long)page[--i]);

			return -ENOMEM;
		}
	}

	unc = vmalloc(LZO_UNC_SIZE);
	if (!unc) {
		printk(KERN_ERR "PM: Failed to allocate LZO uncompressed\n");

		for (i = 0; i < LZO_CMP_PAGES; i++)
			free_page((unsigned long)page[i]);

		return -ENOMEM;
	}

	cmp = vmalloc(LZO_CMP_SIZE);
	if (!cmp) {
		printk(KERN_ERR "PM: Failed to allocate LZO compressed\n");

		vfree(unc);
		for (i = 0; i < LZO_CMP_PAGES; i++)
			free_page((unsigned long)page[i]);

		return -ENOMEM;
	}

	printk(KERN_INFO
		"PM: Loading and decompressing image data (%u pages) ...     ",
		nr_to_read);
	m = nr_to_read / 100;
	if (!m)
		m = 1;
	nr_pages = 0;
	//bio = NULL;
	do_gettimeofday(&start);

	error = snapshot_write_next(snapshot);
	if (error <= 0)
		goto out_finish;

	for (;;) {
		error = swap_read_page(handle, page[0], 1); /* sync */
		if (error)
			break;

		cmp_len = *(size_t *)page[0];
		if (unlikely(!cmp_len ||
		             cmp_len > lzo1x_worst_compress(LZO_UNC_SIZE))) {
			printk(KERN_ERR "PM: Invalid LZO compressed length\n");
			error = -1;
			break;
		}

		for (off = PAGE_SIZE, i = 1;
		     off < LZO_HEADER + cmp_len; off += PAGE_SIZE, i++) {
			error = swap_read_page(handle, page[i], 1);
			if (error)
				goto out_finish;
		}

		error = hib_wait_on_bio_chain(); /* need all data now */

//		error = hib_wait_on_bio_chain(&bio); /* need all data now */
		if (error)
			goto out_finish;

		for (off = 0, i = 0;
		     off < LZO_HEADER + cmp_len; off += PAGE_SIZE, i++) {
			memcpy(cmp + off, page[i], PAGE_SIZE);
		}

		unc_len = LZO_UNC_SIZE;
		error = lzo1x_decompress_safe(cmp + LZO_HEADER, cmp_len,
		                              unc, &unc_len);
		if (error < 0) {
			printk(KERN_ERR "PM: LZO decompression failed\n");
			break;
		}

		if (unlikely(!unc_len ||
		             unc_len > LZO_UNC_SIZE ||
		             unc_len & (PAGE_SIZE - 1))) {
			printk(KERN_ERR "PM: Invalid LZO uncompressed length\n");
			error = -1;
			break;
		}

		for (off = 0; off < unc_len; off += PAGE_SIZE) {
			memcpy(data_of(*snapshot), unc + off, PAGE_SIZE);

			if (!(nr_pages % m))
				printk("\b\b\b\b%3d%%", nr_pages / m);
			nr_pages++;

			error = snapshot_write_next(snapshot);
			if (error <= 0)
				goto out_finish;
		}
	}

out_finish:
	do_gettimeofday(&stop);
	if (!error) {
		printk("\b\b\b\bdone\n");
		snapshot_write_finalize(snapshot);
		if (!snapshot_image_loaded(snapshot))
			error = -ENODATA;
	} else
		printk("\n");
	swsusp_show_speed(&start, &stop, nr_to_read, "Read");

	vfree(cmp);
	vfree(unc);
	for (i = 0; i < LZO_CMP_PAGES; i++)
		free_page((unsigned long)page[i]);

	return error;
}

#if defined(CONFIG_SNAPPY_COMPRESS) && defined (CONFIG_SNAPPY_DECOMPRESS)

#define snappy_max_compressed_length(x)     (x + x/6)

/* Number of pages/bytes we need for compressed data (worst case). */
#define SNAPPY_CMP_PAGES    DIV_ROUND_UP(lzo1x_worst_compress(SNAPPY_UNC_SIZE) + SNAPPY_HEADER, PAGE_SIZE)
#define SNAPPY_CMP_SIZE	    (SNAPPY_CMP_PAGES * PAGE_SIZE)


/**
 * load_image_snappy - Load compressed image data and decompress them with SNAPPY.
 * @handle: Swap map handle to use for loading data.
 * @snapshot: Image to copy uncompressed data into.
 * @nr_to_read: Number of pages to load.
 */
static int load_image_snappy(struct swap_map_handle *handle,
                          struct snapshot_handle *snapshot,
                          unsigned int nr_to_read)
{
	unsigned int m;
	int error = 0;
	//struct bio *bio;
	struct timeval start;
	struct timeval stop;


	struct timeval start1;
	struct timeval stop1;

	
	unsigned nr_pages;
	size_t i, off, unc_len, cmp_len;
	int percent = -1;
	int snappy_nr_to_read = 0;
	s64 snappy_elapsed_centisecs64 = 0;
	s64 snappy_decmpress_elapsed_centisecs64 = 0;
	unsigned char *unc, *cmp, *page[SNAPPY_CMP_PAGES];

    printk("%s\n", __FUNCTION__);

	for (i = 0; i < SNAPPY_CMP_PAGES; i++) {
		page[i] = (void *)__get_free_page(__GFP_WAIT | __GFP_HIGH);
		if (!page[i]) {
			printk(KERN_ERR "PM: Failed to allocate LZO page\n");

			while (i)
				free_page((unsigned long)page[--i]);

			return -ENOMEM;
		}
	}

	unc = vmalloc(SNAPPY_UNC_SIZE);
	if (!unc) {
		printk(KERN_ERR "PM: Failed to allocate SNAPPY uncompressed\n");
		free_pages((unsigned long)page, SNAPPY_UNC_SIZE );
		return -ENOMEM;
	}

	cmp = vmalloc(SNAPPY_CMP_SIZE);
	if (!cmp) {
		printk(KERN_ERR "PM: Failed to allocate SNAPPY compressed\n");

		vfree(unc);
		free_pages((unsigned long)page, SNAPPY_CMP_SIZE);
		return -ENOMEM;
	}

	printk(KERN_INFO
		"PM: Loading and decompressing image data (%u pages) ...     ",
		nr_to_read);
	m = nr_to_read / 100;
	if (!m)
		m = 1;
	nr_pages = 0;
	//bio = NULL;
	do_gettimeofday(&start);

	error = snapshot_write_next(snapshot);
	if (error <= 0)
		goto out_finish;

	for (;;) {
		error = swap_read_page(handle, page[0], 1); /* sync */
		if (error)
			break;

 		cmp_len = *(size_t *)page[0];
        if (unlikely(!cmp_len || cmp_len > SNAPPY_CMP_SIZE)) {
            printk(KERN_ERR "PM: Invalid SNAPPY compressed length cmp_len[%d] [%d]\n",cmp_len,  SNAPPY_CMP_SIZE);
            error = -1;
            break;
        }

        do_gettimeofday(&start1);
        for (off = PAGE_SIZE, i = 1;
             off < SNAPPY_HEADER + cmp_len; off += PAGE_SIZE, i++) {

            snappy_nr_to_read++;
            error = swap_read_page(handle, page[i], 1);
            if (error)
                goto out_finish;
        }
        do_gettimeofday(&stop1);
        snappy_elapsed_centisecs64 += (timeval_to_ns(&stop1) - timeval_to_ns(&start1));

        error = hib_wait_on_bio_chain(); /* need all data now */
        if (error)
            goto out_finish;

		for (off = 0, i = 0;
		     off < SNAPPY_HEADER + cmp_len; off += PAGE_SIZE, i++) {
			memcpy(cmp + off, page[i], PAGE_SIZE);
		}

        do_gettimeofday(&start1);
		unc_len = SNAPPY_UNC_SIZE;
		error = csnappy_decompress(cmp + SNAPPY_HEADER, cmp_len,
		                              unc, &unc_len);
		if (error < 0) {
			printk(KERN_ERR "PM: SNAPPY decompression failed\n");
			break;
		}
        do_gettimeofday(&stop1);
        snappy_decmpress_elapsed_centisecs64 += (timeval_to_ns(&stop1) - timeval_to_ns(&start1));

		if (unlikely(!unc_len ||
		             unc_len > SNAPPY_UNC_SIZE ||
		             unc_len & (PAGE_SIZE - 1))) {
			printk(KERN_ERR "PM: Invalid SNAPPY uncompressed length unc_len[%d] %d %d\n", unc_len, SNAPPY_UNC_SIZE, PAGE_SIZE-1);
			error = -1;
			break;
		}

		for (off = 0; off < unc_len; off += PAGE_SIZE) {
			memcpy(data_of(*snapshot), unc + off, PAGE_SIZE);

			if (!(nr_pages % m)) {
			    int temp = nr_pages / m;

			    if(temp != percent ) {
                    percent = temp;
                    printk("\b\b\b\b%3d%%", percent);
			    }
			}

			nr_pages++;

			error = snapshot_write_next(snapshot);
			if (error <= 0) {
				goto out_finish;
			}
		}
	}

out_finish:
	do_gettimeofday(&stop);
	if (!error) {
		printk("\b\b\b\bdone\n");
		snapshot_write_finalize(snapshot);
		if (!snapshot_image_loaded(snapshot))
			error = -ENODATA;
	} else
		printk("\n");

	swsusp_show_speed(&start, &stop, nr_to_read, "Read");
	swsusp_show_speed1(snappy_elapsed_centisecs64, snappy_nr_to_read, "SNAPPY Read");
	swsusp_show_speed1(snappy_decmpress_elapsed_centisecs64, snappy_nr_to_read, "SNAPPY decompress");

	vfree(cmp);
	vfree(unc);

    for (i = 0;i < SNAPPY_CMP_PAGES;i++)
        free_page((unsigned long)page[i]);

	return error;
}
#endif

#if (defined(CONFIG_LZ4_COMPRESS) || defined(CONFIG_LZ4_HC_COMPRESS)) && defined (CONFIG_LZ4_DECOMPRESS)

/* Number of pages/bytes we need for compressed data (worst case). */
#define LZ4_CMP_PAGES       DIV_ROUND_UP(LZ4_compressBound(LZ4_UNC_SIZE) + LZ4_HEADER, PAGE_SIZE)
#define LZ4_CMP_SIZE        (LZ4_CMP_PAGES * PAGE_SIZE)


/**
 * load_image_lz4 - Load compressed image data and decompress them with LZ4.
 * @handle: Swap map handle to use for loading data.
 * @snapshot: Image to copy uncompressed data into.
 * @nr_to_read: Number of pages to load.
 */
static int load_image_lz4(struct swap_map_handle *handle,
                          struct snapshot_handle *snapshot,
                          unsigned int nr_to_read)
{
	unsigned int m;
	int error = 0;
	//struct bio *bio;
	struct timeval start;
	struct timeval stop;


	struct timeval start1;
	struct timeval stop1;

	
	unsigned nr_pages;
	size_t i;
	int off , unc_len;
	uint32_t cmp_len;
	int percent = -1;
	int lz4_nr_to_read = 0;
	s64 lz4_elapsed_centisecs64 = 0;
	s64 lz4_decmpress_elapsed_centisecs64 = 0;
	unsigned char *unc, *cmp, *page[LZ4_CMP_PAGES];

    printk("%s\n", __FUNCTION__);

	for (i = 0; i < LZ4_CMP_PAGES; i++) {
		page[i] = (void *)__get_free_page(__GFP_WAIT | __GFP_HIGH);
		if (!page[i]) {
			printk(KERN_ERR "PM: Failed to allocate LZ4 page\n");

			while (i)
				free_page((unsigned long)page[--i]);

			return -ENOMEM;
		}
	}

	unc = vmalloc(LZ4_UNC_SIZE);
	if (!unc) {
		printk(KERN_ERR "PM: Failed to allocate LZ4 uncompressed\n");
		free_pages((unsigned long)page, LZ4_CMP_SIZE );
		return -ENOMEM;
	}

	cmp = vmalloc(LZ4_CMP_SIZE);
	if (!cmp) {
		printk(KERN_ERR "PM: Failed to allocate LZ4 compressed\n");

		vfree(unc);
		free_pages((unsigned long)page, LZ4_CMP_SIZE);
		return -ENOMEM;
	}

	printk(KERN_INFO
		"PM: Loading and decompressing image data (%u pages) ...     ",
		nr_to_read);
	m = nr_to_read / 100;
	if (!m)
		m = 1;
	nr_pages = 0;
	//bio = NULL;
	do_gettimeofday(&start);

	error = snapshot_write_next(snapshot);
	if (error <= 0)
		goto out_finish;

	for (;;) {
		error = swap_read_page(handle, page[0], 1); /* sync */
		if (error)
			break;

 		cmp_len = *(uint32_t *)page[0];
        if (unlikely(!cmp_len || cmp_len > LZ4_CMP_SIZE)) {
            printk(KERN_ERR "PM: Invalid LZ4 compressed length cmp_len[%d] [%d]\n",cmp_len,  LZ4_CMP_SIZE);
            error = -1;
            break;
        }

        do_gettimeofday(&start1);
        for (off = PAGE_SIZE, i = 1;
             off < LZ4_HEADER + cmp_len; off += PAGE_SIZE, i++) {

            lz4_nr_to_read++;
            error = swap_read_page(handle, page[i], 1);
            if (error)
                goto out_finish;
        }
        do_gettimeofday(&stop1);
        lz4_elapsed_centisecs64 += (timeval_to_ns(&stop1) - timeval_to_ns(&start1));

        error = hib_wait_on_bio_chain(); /* need all data now */
        if (error)
            goto out_finish;

		for (off = 0, i = 0;
		     off < LZ4_HEADER + cmp_len; off += PAGE_SIZE, i++) {
			memcpy(cmp + off, page[i], PAGE_SIZE);
		}

        do_gettimeofday(&start1);
		unc_len = LZ4_uncompress_unknownOutputSize(cmp + LZ4_HEADER, unc, cmp_len, LZ4_UNC_SIZE);
		if (unc_len < 0) {
			printk(KERN_ERR "PM: LZ4 decompression failed\n");
			break;
		}
        do_gettimeofday(&stop1);
        lz4_decmpress_elapsed_centisecs64 += (timeval_to_ns(&stop1) - timeval_to_ns(&start1));

		if (unlikely(!unc_len ||
		             unc_len > LZ4_UNC_SIZE ||
		             unc_len & (PAGE_SIZE - 1))) {
			printk(KERN_ERR "PM: Invalid LZ4 uncompressed length unc_len[%d] %d %d\n", unc_len, LZ4_UNC_SIZE, PAGE_SIZE-1);
			error = -1;
			break;
		}

		for (off = 0; off < unc_len; off += PAGE_SIZE) {
			memcpy(data_of(*snapshot), unc + off, PAGE_SIZE);

			if (!(nr_pages % m)) {
			    int temp = nr_pages / m;

			    if(temp != percent ) {
                    percent = temp;
                    printk("\b\b\b\b%3d%%", percent);
			    }
			}

			nr_pages++;

			error = snapshot_write_next(snapshot);
			if (error <= 0) {
				goto out_finish;
			}
		}
	}

out_finish:
	do_gettimeofday(&stop);
	if (!error) {
		printk("\b\b\b\bdone\n");
		snapshot_write_finalize(snapshot);
		if (!snapshot_image_loaded(snapshot))
			error = -ENODATA;
	} else
		printk("\n");

	swsusp_show_speed(&start, &stop, nr_to_read, "Read");
	swsusp_show_speed1(lz4_elapsed_centisecs64, lz4_nr_to_read, "LZ4 Read");
	swsusp_show_speed1(lz4_decmpress_elapsed_centisecs64, lz4_nr_to_read, "LZ4 decompress");

	vfree(cmp);
	vfree(unc);

    for (i = 0;i < LZ4_CMP_PAGES;i++)
        free_page((unsigned long)page[i]);

	return error;
}
#endif
/**
 *	swsusp_read - read the hibernation image.
 *	@flags_p: flags passed by the "frozen" kernel in the image header should
 *		  be written into this memory location
 */

int swsusp_read(unsigned int *flags_p)
{
	int error;
	struct swap_map_handle handle;
	struct snapshot_handle snapshot;
	struct swsusp_info *header;

	memset(&snapshot, 0, sizeof(struct snapshot_handle));
	error = snapshot_write_next(&snapshot);
	if (error < PAGE_SIZE)
		return error < 0 ? error : -EFAULT;
	header = (struct swsusp_info *)data_of(snapshot);
	error = get_swap_reader(&handle, flags_p);
	if (error)
		goto end;
	if (!error)
		//error = swap_read_page(&handle, header, NULL);//B090162
		error = swap_read_page(&handle, header, 1);
	if (!error) {
		error = (*flags_p & SF_NOCOMPRESS_MODE) ?
			load_image(&handle, &snapshot, header->pages - 1) :
			#if (defined(CONFIG_LZ4_COMPRESS) || defined(CONFIG_LZ4_HC_COMPRESS)) && defined (CONFIG_LZ4_DECOMPRESS)	
	    		load_image_lz4(&handle, &snapshot, header->pages - 1);
			#elif defined(CONFIG_SNAPPY_COMPRESS) && defined (CONFIG_SNAPPY_DECOMPRESS)	
	    		load_image_snappy(&handle, &snapshot, header->pages - 1);
			#else
			load_image_lzo(&handle, &snapshot, header->pages - 1);
			#endif
	}
	swap_reader_finish(&handle);
end:
	if (!error)
		pr_debug("PM: Image successfully loaded\n");
	else
		pr_debug("PM: Error %d resuming\n", error);
	return error;
}

/**
 *      swsusp_check - Check for swsusp signature in the resume device
 */

int swsusp_check(void)
{
	int error;

	hib_resume_bdev = blkdev_get_by_dev(swsusp_resume_device,
					    FMODE_READ, NULL);
	if (!IS_ERR(hib_resume_bdev)) {
		set_blocksize(hib_resume_bdev, PAGE_SIZE);
		printk("swsusp_heder sig[%s] orig_sig[%s] sector[%lld]\n", swsusp_header->sig, swsusp_header->orig_sig, swsusp_header->image);
		clear_page(swsusp_header);
		//B090162
		error = hib_bio_read_page(swsusp_resume_block,
					//swsusp_header, NULL);
					swsusp_header, 1);
		if (error)
			goto put;
		printk("swsusp_heder sig[%s] orig_sig[%s] sector[%lld]\n", swsusp_header->sig, swsusp_header->orig_sig, swsusp_header->image);

		if (!memcmp(HIBERNATE_SIG, swsusp_header->sig, 10)) {
			memcpy(swsusp_header->sig, swsusp_header->orig_sig, 10);
			/* Reset swap signature now */
			//error = hib_bio_write_page(swsusp_resume_block,//B090162
						//swsusp_header, NULL);
			//			swsusp_header, 1);
		} else {
			printk("%s SWSUSP_SIG cmp error\n", __FUNCTION__);
			error = -EINVAL;
		}

put:
		if (error)
			blkdev_put(hib_resume_bdev, FMODE_READ);
		else
			pr_debug("PM: Image signature found, resuming\n");
	} else {
		error = PTR_ERR(hib_resume_bdev);
	}

	if (error)
		pr_debug("PM: Image not found (code %d)\n", error);

	return error;
}

/**
 *	swsusp_close - close swap device.
 */

void swsusp_close(fmode_t mode)
{
	if (IS_ERR(hib_resume_bdev)) {
		pr_debug("PM: Image device not initialised\n");
		return;
	}

	blkdev_put(hib_resume_bdev, mode);
}

static int swsusp_header_init(void)
{
	swsusp_header = (struct swsusp_header*) __get_free_page(GFP_KERNEL);
	if (!swsusp_header)
		panic("Could not allocate memory for swsusp_header\n");
	return 0;
}

core_initcall(swsusp_header_init);
