#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>

#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <cutils/log.h>


extern int swapoff (__const char *__path);

int swapoff_main(int argc, char *argv[])
{
   
	if( argc != 2 ) {
		printf("swapoff dev_path\n");
		return -1;
	}

	fprintf(stderr, "swapoff %s\n ", argv[1]);
	SLOGI("swapoff %s ", argv[1]);
	return swapoff(argv[1]);
}

