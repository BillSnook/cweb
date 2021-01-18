//
//  filer.cpp
//  cweb
//
//  Created by Bill Snook on 1/18/21.
//  Copyright © 2021 billsnook. All rights reserved.
//

#include <syslog.h>            // close read write
#include <stdlib.h>            // malloc
#include <string.h>            // strcat
#include <stdio.h>            // sprintf

#include "filer.hpp"

#ifdef ON_PI

#define SPEED_FILE_PATH         "/home/pi/code/c/cweb/cweb/speed.bin"

#else   // ON_PI

#define SPEED_FILE_PATH         "/Users/bill/Code/iOS/Tank/cweb/cweb/speed.bin"

#endif  // ON_PI

#define SPEED_NAME                "speed.bin"
#define SPEED1_NAME                "rechargeable.bin"
#define SPEED2_NAME                "copperTop.bin"


enum FileType {
    defaultFileName = 0,
    rechargeableFileName = 1,
    coppertopFileName = 2
};


Filer filer;

// MARK: - Filer
void Filer::setFile( int whichFile ) {
    
    int sizeOfPath = sizeof( SPEED_FILE_PATH );
    memcpy( fileName, SPEED_FILE_PATH, sizeOfPath );

//    speedFileName = fileName;
    memcpy( &fileName[sizeOfPath], "\0", 1 );
}

void Filer::saveData( speed_array *forward, speed_array *reverse ) {
    
    FILE *fp;
    
    fp = fopen( speedFileName, "wb" );
    if ( NULL != fp ) {
        fwrite( forward, sizeof( speed_array ), SPEED_ARRAY, fp );
        fwrite( reverse, sizeof( speed_array ), SPEED_ARRAY, fp );
        fclose(fp);
    } else {
//        fprintf(stderr,"saveData failed opening file\n");
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
//    fprintf(stderr,"readData failed opening file\n");
    syslog(LOG_ERR, "readData failed opening file\n" );
    return false;
}

