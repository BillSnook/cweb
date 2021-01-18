//
//  filer.cpp
//  cweb
//
//  Created by Bill Snook on 1/18/21.
//  Copyright © 2021 billsnook. All rights reserved.
//

#include <syslog.h>             // close read write
#include <stdlib.h>             // malloc
#include <string.h>             // strcat
#include <stdio.h>              // sprintf
#include <unistd.h>             // gethostname

#include "filer.hpp"

#ifdef ON_PI

#define SPEED_FILE_PATH         "/home/pi/code/c/cweb/cweb"

#else   // ON_PI

#define SPEED_FILE_PATH         "/Users/bill/Code/iOS/Tank/cweb/cweb"

#endif  // ON_PI

#define PINS_NAME               "pins.bin"

#define SPEED_NAME              "speed.bin"
#define SPEED1_NAME             "rechargeable.bin"
#define SPEED2_NAME             "copperTop.bin"


enum FileType {
    defaultFileName = 0,
    rechargeableFileName = 1,
    coppertopFileName = 2
};


extern  Filer filer;


// MARK: - Filer
char *Filer::getHostName() {
    
    int result = gethostname( hostName, 32 );
    if ( result != 0 ) {        // Error
        syslog(LOG_NOTICE, "Failed getting hostname; using default one" );
    } else {
        syslog(LOG_NOTICE, "Found hostname: %s", hostName );
    }
    return hostName;
}


void Filer::setFile( int whichFile ) {
    
//    int sizeOfPath = sizeof( SPEED_FILE_PATH );
    hostDirectoryName = hostName;
    syslog(LOG_NOTICE, "In setFile: %s    %s    %s", SPEED_FILE_PATH, hostDirectoryName, SPEED_NAME );
    sprintf( fileName, "%s/%s/%s", SPEED_FILE_PATH, hostDirectoryName, SPEED_NAME );
    syslog(LOG_NOTICE, "Found speed file path: %s", fileName );

//    speedFileName = fileName;
//    memcpy( &fileName[sizeOfPath], "\0", 1 );
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

