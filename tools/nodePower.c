#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define FILE_NAME "/tmp/NodePower"

void terminate( int x ) {
	if ( unlink( FILE_NAME ) ) {
		error( -1, errno, "unlink( \"%s\" ) ", FILE_NAME ); 
	}	
	exit(0);
}

int main()
{
	char* cmd = "ipmitool sensor reading NodeDCpower";
	char buf[512];

	signal( SIGHUP , terminate );
	signal( SIGINT , terminate );
	signal( SIGQUIT , terminate );
	
	FILE* x = fopen( FILE_NAME, "w+" );
	if ( x == NULL ) {
		error( -1, errno, "fopen( \"%s\" ) ", FILE_NAME ); 
	}	
	if ( chmod( FILE_NAME, S_IWUSR| S_IRGRP | S_IROTH  ) ) {
		error( -1, errno, "chmod( \"%s\" ) ", FILE_NAME ); 
	}

	while( 1 ) {
		FILE* fp = popen( cmd, "r" ); 
		if ( x == NULL ) {
			error( -1, errno, "popen( \"%s\" ) ", cmd ); 
		}

		if ( NULL == fgets( buf, 512, fp ) ) {
			error( -1, errno, "error reading command result" ); 
		}
		pclose(fp);
		
		int value;
		char tmp[100];
		if ( 3 != sscanf(buf,"%s %s %d\n",tmp, tmp, &value) ) {
			error( -1, errno, "error parsing command result" ); 
		}

		rewind( x );
		if ( 0 > fprintf( x, "%d\n", value ) ) {
			error( -1, errno, "error updating %s", FILE_NAME ); 
		}

	}
}
