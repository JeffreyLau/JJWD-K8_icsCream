#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>

#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <cutils/log.h>


extern int swapon (__const char *__path, int __flags);

int swapon_main(int argc, char *argv[])
{
   
	if( argc != 2 ) {
		printf("swapon dev_path\n");
		return -1;
	}

	fprintf(stderr, "swapon %s\n ", argv[1]);
	return swapon(argv[1], 0);
}

