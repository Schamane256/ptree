#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define Open_Read \
	if ( ( fd = open( Current_Path,O_RDONLY )) == -1 ) { \
		error( 0 , errno , "Cannot open %s for read\n" , Current_Path ); \
		continue; \
	} \
	lseek(fd,0,SEEK_SET); \
	if ( ( read( fd , Buffer , page_size )) == -1 ) { \
		error( 0 , errno , "Cannot read file: %s\n" , Current_Path ); \
		close( fd ); \
		continue; \
	} \
	close( fd );

struct Proc_struct {
	struct Proc_struct* next;
	char* name;
	unsigned int pid;
	unsigned int ppid;
	char status;
};
