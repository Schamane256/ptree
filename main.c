#include "main.h"

	//	Read Time Stamp Counter
static inline unsigned long long __rdtsc( void ) {
	unsigned int tickl , tickh;
	__asm__ __volatile__( "rdtsc" : "=a"( tickl ) , "=d"( tickh ) );
	return (unsigned long long)tickl | ( (unsigned long long)tickh << 32 );
}
#define DEBUG
#ifdef DEBUG
#	define ERROR_OPEN() error( 0 , errno , "Error : open %s\n" , Path );
#	define ERROR_READ() error( 0 , errno , "Errer : read %s\n" , Path );
#else
#	define ERROR_OPEN()	//	an empty line
#	define ERROR_READ()	//	an empty line
#endif

int main( void ) {
	int __page_size = getpagesize();
	char Path[PATH_MAX] = "/proc/";
	char *const Path_Proc = Path + sizeof( "/proc/" ) - 1;
	char *Path_Proc_FileName , *ProcDirName_Buf;
	DIR *Directory;
	unsigned int ProcDirName_Len;
	int fd , Bytes_Read;
	struct dirent *NextDirectoryEntry;
	char *Buffer = malloc( __page_size );

	int MaxLinks = ( __page_size - sizeof( void* ) ) / sizeof( struct Proc_Struct );
	int PageSize = MaxLinks * sizeof( struct Proc_Struct ) + sizeof( void* );
	struct Mem_Page *First_Page = malloc( PageSize );
	First_Page->Next_Page = NULL;
	struct Mem_Page *Page = First_Page;
	struct Proc_Struct **Link = ( struct Proc_Struct ** )&First_Page->links[0];
	struct Proc_Struct *Proc;
	int NumberOfProcesses = 0;

	char *p , *eos , *a , *s;

/*	int i;
	long long time;
for ( i = 0 ; i < 10 ; i++ ) {
	time = 0;*/

	if ( ( Directory = opendir( "/proc" ) ) == NULL ) {
		error( -23 , errno , "\n\tUnable to open /proc for read\n\n" );
	}

ContinueWhile_NextDir:
	while ( ( NextDirectoryEntry = readdir( Directory ) ) != NULL ) {

		ProcDirName_Buf = NextDirectoryEntry->d_name;

		ProcDirName_Len = strlen( ProcDirName_Buf );

		for ( unsigned int i = 0 ; i < ProcDirName_Len ; i++ ) {
			if ( !isdigit( ProcDirName_Buf[i] ) ) {
				goto ContinueWhile_NextDir;	//	continue the "while" loop
			}
		}

		memcpy( Path_Proc , ProcDirName_Buf , ProcDirName_Len );
		Path_Proc_FileName = Path_Proc + ProcDirName_Len;
		memcpy( Path_Proc_FileName++ , "/cmdline" , sizeof( "/cmdline" ) );

		if ( ( fd = open( Path , O_RDONLY ) ) == -1 ) {
			ERROR_OPEN()
			continue;
		}
		lseek( fd , 0 , SEEK_SET );
		Bytes_Read = read( fd , Buffer , __page_size );
		close( fd );
		if ( Bytes_Read == -1 ) {
			ERROR_READ()
			continue;
		}

		if ( Bytes_Read == 0 ) {

			Proc = malloc( sizeof( struct Proc_Struct ) );
			Proc->cmdline.length = 0;

		} else {

			for ( p = Buffer , eos = Buffer + Bytes_Read ; ( p < eos ) && ( p != 0 ) ; ) {
				if ( ( p = memchr( p , ' ' , (int)( eos - p ) ) ) != 0 ) {
					Bytes_Read++;
					p++;
				}
			}
			Proc = malloc( sizeof( struct Proc_Struct ) + Bytes_Read );
			Proc->cmdline.length = Bytes_Read;

			for ( s = Buffer , p = Proc->cmdline.string , eos = Proc->cmdline.string + Bytes_Read ; ( p < eos - 1 ) && ( p != 0 ) ; ) {
				a = p;
				if ( ( p = memccpy( p , s , ' ' , (int)( eos - p - 1 ) ) ) != 0 ) {
					s = s - a + p;
					p[-1] = '\\';
					p++[0] = ' ';
				}
			}
			for ( p = Proc->cmdline.string , eos = Proc->cmdline.string + Bytes_Read ; ( p < eos - 1 ) && ( p != 0 ) ; ) {
				if ( ( p = memchr( p , '\0' , (int)( eos - p - 1 ) ) ) != 0 ) {
					*(p++) = ' ';
				}
			}
		}	//	end :	if ( Bytes_Read == 0 )

		memcpy( Path_Proc_FileName , "status" , sizeof( "status" ) );
		if ( ( fd = open( Path , O_RDONLY ) ) == -1 ) {
			ERROR_OPEN()
			continue;
		}
		lseek( fd , 0 , SEEK_SET );
		Bytes_Read = read( fd , Buffer , __page_size );
		close( fd );
		if ( Bytes_Read == -1 ) {
			ERROR_READ()
			free( Proc );
			continue;
		}

		a = Buffer;

#define PARSE_STATUS( first_char , the_string , propertion , operation ) \
\
		while ( a < Buffer + Bytes_Read ) { \
			a = memchr( a , first_char , Bytes_Read ); \
			if ( memcmp( a++ - 1 , the_string , sizeof( the_string ) - 1 ) == 0 ) { \
				propertion = operation( a = a - 2 + sizeof( the_string ) - 1 ); \
				break; \
			} \
		}
//end#define PARSE_STATUS()
		PARSE_STATUS( 'S' , "\nState:\t" , Proc->state , * );
		PARSE_STATUS( 'P' , "\nPid:\t" , Proc->pid , atoi );
		PARSE_STATUS( 'P' , "\nPPid:\t" , Proc->ppid , atoi );
#undef PARSE_STATUS

		*Link = Proc;
		if ( ++NumberOfProcesses % MaxLinks == 0 ) {
			//	Allocate and separate for pages of links to each proc and its cmdline , and make a linked list of links to these procs.
			Page->Next_Page = malloc( PageSize );
			Page = Page->Next_Page;
			Page->Next_Page = NULL;
			Link = ( struct Proc_Struct ** )&Page->links[0];
		} else {
			Link++;
		}	//	end :	if ( ++NumberOfProcesses % MaxLinks == 0 )

	}	//	end :	while ( ( NextDirectoryEntry = readdir( Directory ) ) != NULL )
//	todo :
//	sort the procs !
//	then print them !

//	clean :
	closedir( Directory );

	while ( NumberOfProcesses % MaxLinks != 0 ) {
		free( *(--Link) );
		NumberOfProcesses--;
	};

	Page = First_Page;
	do {
		if ( NumberOfProcesses ) {
			Link = ( struct Proc_Struct ** )&Page->links[ MaxLinks ];
			do {
				free( *(--Link) );
			} while ( --NumberOfProcesses % MaxLinks != 0 );
			First_Page = Page;
			Page = Page->Next_Page;
			free( First_Page );
		} else {
			free( Page );
			break;
		}
	} while ( Page );

	free( Buffer );
	return 0;
}
