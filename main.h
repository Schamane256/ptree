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

//#ifndef PAGE_SIZE
//#define PAGE_SIZE 4096
//#endif

typedef struct PascalString_tiny {
	unsigned char length;
	char string[];
} PascalString_tiny;

typedef struct PascalString_short {
	unsigned short length;
	char string[];
} PascalString_short;

typedef struct PascalString {
	unsigned int length;
	char string[];
} PascalString;

typedef struct PascalString_long {
	unsigned long long length;
	char string[];
} PascalString_long;

struct Proc_Struct {
	struct Proc_Struct *parent;
	int pid;
	int ppid;
	char state;
	char level;
	PascalString cmdline;
};

struct Mem_Page {
	struct Mem_Page *Next_Page;
	struct Proc_Struct *links[ /* PAGE_SIZE / sizeof(void*) */ ];
};
