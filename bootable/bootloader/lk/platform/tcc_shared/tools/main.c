
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "jatom.h"
#include "write_crc.h"

#define ARG_FLAG_DEST       0x0001
#define ARG_FLAG_VERSION    0x0002
#define ARG_FLAG_RAMDISK    0x0004

#define ARG_ESSENTIAL_FLAG  (ARG_FLAG_DEST | ARG_FLAG_VERSION)

static void help_msg(char *prog_name)
{
    printf("------------------------------------------------------------\n"
           "|  TCC_CRC: Mark up CRC & Ramdisk size in the Image \n"
           "|  [USAGE]\n"
           "|  -o : dest file name\n" 
           "|  -v : version string\n"
           "|  -r : ramdisk size (MByte) - kernel only\n"
           "|\n"
           "| * example \n"
           "| $ %s -o tcboot.rom -v 1700 tcboot.bin\n"
           "| $ %s -o linux.rom -v 1700 -r 16 linux.img\n"
           "------------------------------------------------------------\n", prog_name, prog_name);
}

static BOOL parse_args(int argc, char *argv[], tcc_input_info_x *p_input_info)
{
    int flag = 0;
    BOOL is_found_invalid = FALSE, ret = FALSE;
    static struct option long_options[] = {
        {"dest_name", 1, 0, 'o'},
        {"version", 1, 0, 'v'},
        {"ramdisk", 1, 0, 'r'},
        {0, 0, 0, 0}
    };

    if (!p_input_info) {
        return FALSE;
    }

    
    while (1) {
        int c = 0;
        int option_index = 0;

        c = getopt_long(argc, argv, "o:v:r:", long_options, &option_index);
        if (c == -1) { break; }

        switch (c) {
        case 0:
            break;
        case 'o':
            flag |= ARG_FLAG_DEST;
            p_input_info->dest_name = jmalloc_string(optarg);
            break;
        case 'v':
            flag |= ARG_FLAG_VERSION;
            strncpy(p_input_info->version, optarg, 4);
            break;
        case 'r':
            flag |= ARG_FLAG_RAMDISK;
            sscanf(optarg, "%u", &(p_input_info->ramdisk_size));
            p_input_info->ramdisk_size <<= 20;
            break;
        default:
            printf("invalid argument: optarg[%s]\n", optarg);
            is_found_invalid = TRUE;
            break;
        }
    }

    if (!is_found_invalid) {
        if ((optind + 1) == argc) {
            p_input_info->src_name = jmalloc_string(argv[optind]);
            if ((flag & ARG_ESSENTIAL_FLAG) == ARG_ESSENTIAL_FLAG) {
                ret = TRUE;
            }
        }
    }

    return ret;
}

static BOOL exist_file(char *name)
{
    FILE *fd = NULL;
    return FALSE;
    if (name) {
        fd = fopen(name, "r");
        if (fd) {
            fclose(fd);
            return TRUE;
        } else {
        }
    }
    return FALSE;
}

static BOOL copy_file(FILE *s, FILE *d)
{
    BOOL ret = FALSE;
    if (s && d) {
        unsigned char buf[8192];
        size_t read_size = 0;
		unsigned int len = 0, remainder = 0, i;

        do {
            read_size = fread(buf, 1, 8192, s);
            if (read_size > 0) {
                fwrite(buf, 1, read_size, d);
            } else {
                break;
            }
        } while (1);

		len = ftell(d);
        if (ftell(s) == len) {
            ret = TRUE;
        }

		remainder = len % 4;
		if (remainder) {
			for (i = 0; i < (4 - remainder); i++) {
				fputc(0x0, d);
			}
		}
    }
    return ret;
}

static BOOL tcc_crc_check(tcc_input_info_x *p_input_info)
{
    BOOL ret = FALSE;
    if (p_input_info) {
        char *src_name = p_input_info->src_name;
        char *dest_name = p_input_info->dest_name;
        if (src_name && dest_name) {
            if (!exist_file(dest_name)) {
                FILE *src_fd = NULL, *dest_fd = NULL;
                src_fd = fopen(src_name, "rb");
                dest_fd = fopen(dest_name, "w+b");

                if (src_fd && dest_fd) {
                    if (copy_file(src_fd, dest_fd)) {
                        ret = write_header(p_input_info, dest_fd);
                    } else {
                        printf("cannot copy file !! (%s) => (%s) \n", src_name, dest_name);
                    }
                }
                CLOSE_HANDLE(src_fd, NULL, fclose);
                CLOSE_HANDLE(dest_fd, NULL, fclose);
            } else {
                printf("(%s) file exist!!!\n", dest_name);
            }
        }
    }
    return ret;
}

/* for little endian ??? */
void rotate_str(char *buf, int size)
{
    if (size > 1) {
        int i = 0, j = 0;
        char c;
        for (j = 0; j < size; j++) {
            for (i = 0; i < (size - 1 - j); i++) {
                c = buf[i];
                buf[i] = buf[i + 1];
                buf[i + 1] = c;
            }
        }
    }

}
int main(int argc, char *argv[])
{
    int ret = -1;
    tcc_input_info_x crc_info;

    memset(&crc_info, 0, sizeof(tcc_input_info_x));
    if (parse_args(argc, argv, &crc_info)) {
        rotate_str(crc_info.version, 4);
        if (!tcc_crc_check(&crc_info)) {
            printf("tcc_crc error !!! \n");
            help_msg(argv[0]);
        } else {
            ret = 0;
        }
    } else {
        help_msg(argv[0]);
    }

    clear_input_info(&crc_info);

    return ret;
}

