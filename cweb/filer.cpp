//
//  filer.cpp
//  cweb
//
//  Created by William Snook on 5/1/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "filer.hpp"
#include "speed.hpp"

#include <syslog.h>			// close read write
#include <stdio.h>

Filer filer;

Filer::Filer() {
	
}

void Filer::saveData( speed_array *forward, speed_array *reverse ) {
	
	FILE *fp;
	
	fp = fopen( SPEED_FILE_NAME, "wb" );
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
	
	fp = fopen( SPEED_FILE_NAME, "rb" );
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
