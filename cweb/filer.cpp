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

#define CONFIG_FILE_PATH        "/home/pi/code/c/cweb/cweb"

#else   // ON_PI

#define CONFIG_FILE_PATH         "/Users/bill/Code/iOS/Tank/cweb/cweb"

#endif  // ON_PI

#define PINS_NAME               "pins.bin"

#define SPEED_FILE_NAME         "speed.bin"
#define RANGE_FILE_NAME         "range.bin"


// MARK: - Filer
void Filer::getHostName() {
    
    int result = gethostname( hostName, 32 );
    if ( result != 0 ) {        // Error
        syslog(LOG_NOTICE, "Failed getting hostname" );
    } else {
        syslog(LOG_NOTICE, "Found hostname: %s", hostName );
    }
}


void Filer::setupFiles() {
    
    sprintf( speedFileName, "%s/%s-%s", CONFIG_FILE_PATH, hostName, SPEED_FILE_NAME );
    sprintf( rangeFileName, "%s/%s-%s", CONFIG_FILE_PATH, hostName, RANGE_FILE_NAME );
    syslog(LOG_NOTICE, "Set speed file path: %s", speedFileName );
    syslog(LOG_NOTICE, "Set range file path: %s", rangeFileName );
}

void Filer::saveSpeedArrays( speed_array *forward, speed_array *reverse ) {
    
    FILE *fp;
    
    fp = fopen( speedFileName, "wb" );
    if ( NULL != fp ) {
        fwrite( forward, sizeof( speed_array ), SPEED_ARRAY, fp );
        fwrite( reverse, sizeof( speed_array ), SPEED_ARRAY, fp );
        fclose(fp);
    } else {
        syslog(LOG_ERR, "saveSpeedArrays failed opening file\n" );
    }
}

bool Filer::readSpeedArrays( speed_array *forward, speed_array *reverse ) {
    
    FILE *fp;
    
    fp = fopen( speedFileName, "rb" );
    if ( NULL != fp ) {
        fread( forward, sizeof( speed_array ), SPEED_ARRAY, fp );
        fread( reverse, sizeof( speed_array ), SPEED_ARRAY, fp );
        fclose(fp);
        return true;
    }
    syslog(LOG_ERR, "readSpeedArrays failed opening file\n" );
    return false;
}

bool Filer::saveRange( RangeData *rangeDataPtr ) {
    
    FILE *fp;
    
    fp = fopen( rangeFileName, "wb" );
    if ( NULL != fp ) {
        fwrite( rangeDataPtr, sizeof( RangeData ), 1, fp );
        fclose(fp);
        return true;
    }
    syslog(LOG_ERR, "saveRange failed opening file\n" );
    return false;
}

bool Filer::readRange( RangeData *rangeDataPtr ) {
    
    FILE *fp;
    
    fp = fopen( rangeFileName, "rb" );
    if ( NULL != fp ) {
        fread( rangeDataPtr, sizeof( RangeData ), 1, fp );
        fclose(fp);
        syslog(LOG_WARNING, "readRange got scanner center: %d", rangeDataPtr->scannerPort );
        return true;
    }
    syslog(LOG_ERR, "readRange failed opening file\n" );
    return false;
}

