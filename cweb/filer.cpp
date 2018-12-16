//
//  filer.cpp
//  cweb
//
//  Created by William Snook on 5/1/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "filer.hpp"
#include "speed.hpp"

//#include <stdlib.h>			// malloc
#include <string.h>			// strcat
#include <syslog.h>			// close read write
#include <stdio.h>

#ifdef ON_PI

#define SPEED_FILE_PATH         "/home/pi/code/c/cweb/cweb/speed.bin"

#else   // ON_PI

#define SPEED_FILE_PATH         "/Users/bill/Code/iOS/rTest/cweb/cweb/speed.bin"

#endif  // ON_PI

#define SPEED_NAME				"speed.bin"
#define SPEED1_NAME				"rechargeable.bin"
#define SPEED2_NAME				"copperTop.bin"


enum FileType {
	defaultFileName = 0,
	rechargeableFileName = 1,
	coppertopFileName = 2
};


Filer filer;

Filer::Filer() {
//	char *speedFileName;
}

void Filer::setFile( int whichFile ) {
	
	int sizeOfPath = sizeof( SPEED_FILE_PATH );
	memcpy( fileName, SPEED_FILE_PATH, sizeOfPath );

	speedFileName = fileName;
	int nameSize = 0;
//	switch ( whichFile ) {
//		case defaultFileName:
//			nameSize = sizeof( SPEED_NAME );
//			memcpy( &fileName[sizeOfPath], SPEED_NAME, nameSize );
//			printf( "0" );
//			break;
//		case rechargeableFileName:
//			nameSize = sizeof( SPEED1_NAME );
//			memcpy( &fileName[sizeOfPath], SPEED1_NAME, nameSize );
//			printf( "1" );
//			break;
//		case coppertopFileName:
//			nameSize = sizeof( SPEED2_NAME );
//			memcpy( &fileName[sizeOfPath], SPEED2_NAME, nameSize );
//			printf( "2" );
//			break;
//		default:
//			nameSize = sizeof( SPEED_NAME );
//			memcpy( &fileName[sizeOfPath], SPEED_NAME, nameSize );
//			printf( "?" );
//			break;
//	}
	memcpy( &fileName[sizeOfPath + nameSize], "\0", 1 );
}

void Filer::saveData( speed_array *forward, speed_array *reverse ) {
	
	FILE *fp;
	
	fp = fopen( speedFileName, "wb" );
	if ( NULL != fp ) {
		fwrite( forward, sizeof( speed_array ), SPEED_ARRAY, fp );
		fwrite( reverse, sizeof( speed_array ), SPEED_ARRAY, fp );
		fclose(fp);
	} else {
//		fprintf(stderr,"saveData failed opening file\n");
		syslog(LOG_ERR, "saveData failed opening file\n" );
	}
}

bool Filer::readData( speed_array *forward, speed_array *reverse ) {
	
	FILE *fp;
	
	fp = fopen( speedFileName, "rb" );
	if ( NULL != fp ) {
		fread( forward, sizeof( speed_array ), SPEED_ARRAY, fp );
		fread( reverse, sizeof( speed_array ), SPEED_ARRAY, fp );
		fclose(fp);
		return true;
	}
//	fprintf(stderr,"readData failed opening file\n");
	syslog(LOG_ERR, "readData failed opening file\n" );
	return false;
}
