/*
 * Copyright (C) 2010 Telechips, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <string.h>
#include <arch/tcc_used_mem.h>

#define ARRAY_SIZE(x)	(sizeof(x)/sizeof((x)[0]))

#define ATAG_MEM		0x54410002
#define ATAG_REVISION	0x54410007
#define ATAG_CAMERA		0x5441000c
#define ATAG_TCC_PMAP	0x5443436d	/* TCCm */

#define KERNEL_MEM_SIZE	TOTAL_SDRAM_SIZE

#define TCC_PMAP_NAME_LEN	16

typedef struct {
	char name[TCC_PMAP_NAME_LEN];
	unsigned base;
	unsigned size;
} pmap_t;

static pmap_t pmap_table[] = {
	{ "pmem", PMEM_SURF_BASE, PMEM_SURF_SIZE },
	{ "viqe", PMEM_VIQE_BASE, PMEM_VIQE_SIZE },
	{ "ump_reserved", UMP_RESERVED_BASE, UMP_RESERVED_SIZE },
	{ "overlay", OVERLAY_0_PHY_ADDR, OVERLAY_0_MEM_SIZE },
	{ "overlay1", OVERLAY_1_PHY_ADDR, OVERLAY_1_MEM_SIZE },
	{ "overlay_rot", OVERLAY_ROTATION_PHY_ADDR, OVERLAY_ROTATION_MEM_SIZE },
	{ "ext_camera", EXT_CAM_BASE, EXT_CAM_SIZE },
	{ "video", VIDEO_MEM_BASE, VIDEO_MEM_SIZE },
	{ "fb_video", FB_MEM_BASE, FB_MEM_SIZE },
	{ "fb_scale", FB_SCALE_MEM_BASE, FB_SCALE_MEM_TOTAL_SIZE },
	{ "fb_scale0", FB_SCALE_ADDR0, FB_SCALE_SIZE },
	{ "fb_scale1", FB_SCALE_ADDR1, FB_SCALE_SIZE },
	{ "fb_g2d0", FB_G2D_ADDR0, FB_G2D_SIZE },
	{ "fb_g2d1", FB_G2D_ADDR1, FB_G2D_SIZE },
	{ "video_dual", VIDEO_DUAL_DISPLAY_BASE, VIDEO_DUAL_DISPLAY_SIZE },
	{ "jpeg_header", PA_JPEG_HEADER_BASE_ADDR, TCC_JPEG_HEADER_SIZE },
	{ "jpeg_raw", PA_JPEG_RAW_BASE_ADDR, JPEG_RAW_MEM_SIZE },
	{ "jpeg_stream", PA_JPEG_STREAM_BASE_ADDR, JPEG_STREAM_MEM_SIZE },
#ifdef OUTPUT_ATTACH_ADDR
	{ "output_attach", OUTPUT_ATTACH_ADDR, OUTPUT_ATTACH_SIZE},
#endif		
	{ "total", PMEM_SURF_BASE, TOTAL_FIXED_MEM_SIZE },
};

unsigned* target_atag_mem(unsigned* ptr)
{
	unsigned i;

	/* ATAG_MEM */
	*ptr++ = 4;
	*ptr++ = ATAG_MEM;
	*ptr++ = KERNEL_MEM_SIZE;
	*ptr++ = BASE_ADDR;

	/* ATAG_TCC_PMAP */
	*ptr++ = 2 + (sizeof(pmap_table) / sizeof(unsigned));
	*ptr++ = ATAG_TCC_PMAP;
	for (i = 0; i < ARRAY_SIZE(pmap_table); i++) {
		memcpy(ptr, &pmap_table[i], sizeof(pmap_t));
		ptr += sizeof(pmap_t) / sizeof(unsigned);
	}

	return ptr;
}

unsigned* target_atag_revision(unsigned* ptr)
{
	*ptr++ = 3;
	*ptr++ = ATAG_REVISION;
	*ptr++ = HW_REV;
	return ptr;
}

unsigned* target_atag_is_camera_enable(unsigned* ptr)
{
	*ptr++ = 3;
	*ptr++ = ATAG_CAMERA;	
	*ptr++ = 1;

	return ptr;
}
