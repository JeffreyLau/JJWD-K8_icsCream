
#ifndef __WRITE_CRC_H__
#define __WRITE_CRC_H__

#define ALIGN_SIZE 4
#define MIN_IMAGE_SIZE (128 * 1024)

#define HEADER_POS_MAJOR_VER    (2 * ALIGN_SIZE)
#define HEADER_POS_MINOR_VER    (3 * ALIGN_SIZE)
#define HEADER_POS_FIRST_CRC    (4 * ALIGN_SIZE)
#define HEADER_POS_RESERVED_CRC (5 * ALIGN_SIZE)
#define HEADER_POS_SECOND_CRC   (6 * ALIGN_SIZE)
#define HEADER_POS_IMAGE_SIZE   (7 * ALIGN_SIZE)

#define HEADER_LOGICAL_POS_MINOR_VER    (0)
#define HEADER_LOGICAL_POS_FIRST_CRC    (1 * ALIGN_SIZE)
#define HEADER_LOGICAL_POS_RESERVED_CRC (2 * ALIGN_SIZE)
#define HEADER_LOGICAL_POS_SECOND_CRC   (3 * ALIGN_SIZE)
#define HEADER_LOGICAL_POS_IMAGE_SIZE   (4 * ALIGN_SIZE)


#define GET_ALIGNED_SIZE(X) ((X) / ALIGN_SIZE)
#define GET_ALIGNED_SIZE(X) ((X) / ALIGN_SIZE)

#define HEADER_START_POS   HEADER_POS_MINOR_VER
#define HEADERS_SIZE (HEADER_POS_IMAGE_SIZE + ALIGN_SIZE - HEADER_START_POS)

#ifdef FWDN_CRC_CHECK
#   define INIT_CRC(C) ((C) = 0xFFFFFFFFL)
#   define MAKE_CRC(C) ((C) ^= 0xFFFFFFFFL)
#else
#   define INIT_CRC(C) ((C) = 0)
#   define MAKE_CRC(C) ((C) = (C))
#endif

#include "jatom.h"

typedef struct _tcc_input_info_x {
    char *src_name;
    char *dest_name;
    char version[5];
    unsigned int ramdisk_size;
} tcc_input_info_x;

extern char *jmalloc_string(char *sz);
extern BOOL write_header(tcc_input_info_x *p_input_info, FILE *fd);
extern void clear_input_info(tcc_input_info_x *p_input_info);


#endif /* __WRITE_CRC_H__ */

