//
//  filer.cpp
//  cweb
//
//  Created by William Snook on 5/1/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "filer.hpp"

#include <stdio.h>


Filer::Filer() {
	
}

void Filer::saveData( speed_array *spd ) {
	
	FILE *fp;
	
	fp = fopen( SPEED_FILE_NAME, "wb" );
	if ( NULL != fp ) {
		fwrite( spd, sizeof( speed_array), SPEED_ARRAY, fp );
		fclose(fp);
	} else {
		fprintf(stderr,"saveData failed opening file\n");
	}
}

bool Filer::readData( speed_array *spd ) {
	
	FILE *fp;
	
	fp = fopen( SPEED_FILE_NAME, "rb" );
	if ( NULL != fp ) {
		fread( spd, sizeof( speed_array), SPEED_ARRAY, fp );
		fclose(fp);
		return true;
	}
	fprintf(stderr,"readData failed opening file\n");
	return false;
}
