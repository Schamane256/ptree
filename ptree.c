#include "ptree.h"

int main( void ) {
	char Current_Path[256] = "/proc/";
	char *const Current_Path_Proc = Current_Path + sizeof( "/proc/" ) - 1;
	char *Current_File , *ProcDirName_Buf;
	unsigned int i , ProcDirName_Len;
	int status , fd;
	int NumberOfProcesses = 0;
	int page_size = getpagesize();
	char *Buffer = malloc( page_size );
	DIR *Directory;
	struct dirent *NextDirectoryEntry;

	if ( ( Directory = opendir( "/proc" ) ) == NULL ) {
		error( -23 , errno , "\n\tERROR: unable to open /proc for read\n\n" );
	}

	while ( ( NextDirectoryEntry = readdir( Directory ) ) != NULL ) {
		ProcDirName_Buf = NextDirectoryEntry->d_name;
		ProcDirName_Len = strlen( ProcDirName_Buf );
		for ( i=0 , status=0 ; i < ProcDirName_Len ; i++ ) {
			if( !isdigit( ProcDirName_Buf[i] ) ) {
				status = 1;
				break;
			}
		}
		if ( status ) {
			continue;
		}

		memcpy( Current_Path_Proc , ProcDirName_Buf , ProcDirName_Len );
		Current_File = Current_Path_Proc + ProcDirName_Len;
		memcpy( Current_File++ , "/stat" , sizeof( "/stat" ) );

		Open_Read;

		printf(Buffer);
		printf("\n\n");

		memcpy( Current_File , "cmdline" , sizeof( "cmdline" ) );

		Open_Read;
		printf(Buffer);
		printf("\n\n");
	}
	free( Buffer );
	return 0;
}
