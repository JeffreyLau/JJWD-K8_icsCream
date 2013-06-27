#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>

#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <cutils/log.h>
#include <pwd.h>

#define PAGE_SIZE 4096

extern int swapon (__const char *__path, int __flags);

/*
 * Magic header for a swap area. The first part of the union is
 * what the swap magic looks like for the old (limited to 128MB)
 * swap area format, the second part of the union adds - in the
 * old reserved area - some extra information. Note that the first
 * kilobyte is reserved for boot loader or disk label stuff...
 *
 * Having the magic at the end of the PAGE_SIZE makes detecting swap
 * areas somewhat tricky on machines that support multiple page sizes.
 * For 2.5 we'll probably want to move the magic to just beyond the
 * bootbits...
 */
union swap_header {
	struct {
		char reserved[PAGE_SIZE - 10];
		char magic[10];			/* SWAP-SPACE or SWAPSPACE2 */
	} magic;
	struct {
		char		bootbits[1024];	/* Space for disklabel etc. */
		__u32		version;
		__u32		last_page;
		__u32		nr_badpages;
		unsigned char	sws_uuid[16];
		unsigned char	sws_volume[16];
		__u32		padding[117];
		__u32		badpages[1];
	} info;
};

int mkswap_main(int argc, char *argv[])
{
    int fd;
    off_t len;
    union swap_header *swap_header;
    
	if( argc != 2 ) {
		printf("mkdir dev_path\n");
		return -1;
	}

	swap_header = malloc(PAGE_SIZE);
    memset(swap_header, 0, PAGE_SIZE);

	fprintf(stderr, "mkswap %s\n ", argv[1]);
    fd = open(argv[1], O_RDWR);

	if( fd < 0 ) {
		SLOGE("mkswap file open failed! path[%s]", argv[1]);
		return -1;
	}

	len = lseek(fd, 0, SEEK_END);

	SLOGE("swap len %d", len);

    swap_header->info.version = 1;
    swap_header->info.last_page = (len/PAGE_SIZE) - 1;

    
    memcpy(swap_header->magic.magic, "SWAPSPACE2", 10);

    lseek(fd, 0, SEEK_SET);
    write(fd, swap_header, PAGE_SIZE);
	fsync(fd);
	close(fd);

	free(swap_header);

	return swapon(argv[1], 0);
}

