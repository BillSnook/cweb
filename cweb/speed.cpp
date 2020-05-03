//
//  speed.cpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//



#include <syslog.h>			// close read write
#include <stdlib.h>			// malloc
#include <string.h>			// strcat
#include <stdio.h>			// sprintf
//#include <getopt.h>

#include "speed.hpp"


#ifdef ON_PI

#define SPEED_FILE_PATH         "/home/pi/code/c/cweb/cweb/speed.bin"

#else   // ON_PI

#define SPEED_FILE_PATH         "/Users/bill/Code/iOS/rTest/cweb/cweb/speed.bin"

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

    speedFileName = fileName;
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

// MARK: - Speed
// Converts speed index into speeds
Speed::Speed() {
    
}

bool Speed::setupForSpeed() {
	
	return true;
}

bool Speed::resetForSpeed() {
	
	return false;
}

void Speed::initializeSpeedArray() {
	
	filer = Filer();
	filer.setFile( 1 );
	bool success = filer.readData( forward, reverse );
	if ( ! success ) {
		syslog(LOG_NOTICE, "Failed reading speed array from file; making default one" );
		resetSpeedArray();
	} else {
		syslog(LOG_NOTICE, "Read speed array from file" );
	}
}

void Speed::resetSpeedArray() {		// Create simple default to assist calibration
	
	for ( int i = 0; i < SPEED_ARRAY; i++ ) {
		forward[i].left = i * SPEED_ADJUSTMENT;
		forward[i].right = i * SPEED_ADJUSTMENT;
		reverse[i].left = i * SPEED_ADJUSTMENT;
		reverse[i].right = i * SPEED_ADJUSTMENT;
	}
}

char * Speed::displaySpeedArray( char *displayString ) {
	
//	char *displayString = (char *)malloc( 1024 );
	strcat( displayString, " Speed array, forward:\n" );
	for ( int i = 0; i < SPEED_ARRAY; i++ ) {
		sprintf( displayString, "%s i: %d - l: %d, r: %d\n", displayString, i, forward[i].left, forward[i].right );
	}
	strcat( displayString, " Speed array, reverse:\n" );
	for ( int i = 0; i < SPEED_ARRAY; i++ ) {
		sprintf( displayString, "%s i: %d - l: %d, r: %d\n", displayString, i, reverse[i].left, reverse[i].right );
	}
	return displayString;
}

char *Speed::setSpeedTestIndex( int newSpeedIndex ) {
	calibrationTestIndex = newSpeedIndex;
	char *displayString = (char *)malloc( 32 );
	sprintf( displayString, "i %d %d %d", newSpeedIndex, speedLeft( newSpeedIndex ), speedRight( newSpeedIndex ) );
	return( displayString );

}

int Speed::speedLeft( int speedIndex ) {
	if ( ( speedIndex > -SPEED_ARRAY ) && ( speedIndex < SPEED_ARRAY ) ) {
		if ( speedIndex > 0 ) {
			return forward[speedIndex].left;
		} else {
			return reverse[-speedIndex].left;
		}
	}
	return 0;
}

int Speed::speedRight( int speedIndex ) {
	if ( ( speedIndex > -SPEED_ARRAY ) && ( speedIndex < SPEED_ARRAY ) ) {
		if ( speedIndex > 0 ) {
			return forward[speedIndex].right;
		} else {
			return reverse[-speedIndex].right;
		}
	}
	return 0;
}

void Speed::setSpeedLeft( int speedIndex, int newSpeed ) {
	if ( ( speedIndex > -SPEED_ARRAY ) && ( speedIndex < SPEED_ARRAY ) ) {
		if ( speedIndex > 0 ) {
			forward[speedIndex].left = newSpeed;
		} else {
			reverse[-speedIndex].left = newSpeed;
		}
	}
}

void Speed::setSpeedRight( int speedIndex, int newSpeed ) {
	if ( ( speedIndex > -SPEED_ARRAY ) && ( speedIndex < SPEED_ARRAY ) ) {
		if ( speedIndex > 0 ) {
			forward[speedIndex].right = newSpeed;
		} else {
			reverse[-speedIndex].right = newSpeed;
		}
	}
}

void Speed::setSpeedLeft( int newSpeed ) {
	if ( ( calibrationTestIndex > -SPEED_ARRAY ) && ( calibrationTestIndex < SPEED_ARRAY ) ) {
		if ( calibrationTestIndex > 0 ) {
			forward[calibrationTestIndex].left = newSpeed;
		} else {
			reverse[-calibrationTestIndex].left = newSpeed;
		}
	}
}

void Speed::setSpeedRight( int newSpeed ) {
	if ( ( calibrationTestIndex > -SPEED_ARRAY ) && ( calibrationTestIndex < SPEED_ARRAY ) ) {
		if ( calibrationTestIndex > 0 ) {
			forward[calibrationTestIndex].right = newSpeed;
		} else {
			reverse[-calibrationTestIndex].right = newSpeed;
		}
	}
}

void Speed::setSpeedForward() {
	// Assume index 1 os slowest speed, index 8 is the fastest
	// Find a pattern so each entry is linear
	int slowest = forward[1].left;
	int fastest = forward[SPEED_ARRAY-1].left;
	int interval = ( fastest - slowest ) / ( SPEED_ARRAY - 2 );
	syslog(LOG_NOTICE, "Slowest: %d, fastest: %d, interval: %d", slowest, fastest, interval );
	for ( int i = 2; i < ( SPEED_ARRAY - 1 ); i++ ) {
//		int next = slowest + ( ( i - 1 ) * interval );
//		syslog(LOG_NOTICE, "%d  %d", i, next );
		forward[i].left = slowest + ( ( i - 1 ) * interval );
	}
	slowest = forward[1].right;
	fastest = forward[SPEED_ARRAY-1].right;
	interval = ( fastest - slowest ) / ( SPEED_ARRAY - 2 );
	for ( int i = 2; i < ( SPEED_ARRAY - 1 ); i++ ) {
		forward[i].right = slowest + ( ( i - 1 ) * interval );
	}
}

void Speed::setSpeedReverse() {
	// Assume index 1 os slowest speed, index 8 is the fastest
	// Find a pattern so each entry is linear
	int slowest = reverse[1].left;
	int fastest = reverse[SPEED_ARRAY-1].left;
	int interval = ( fastest - slowest ) / ( SPEED_ARRAY - 2 );
	syslog(LOG_NOTICE, "Slowest: %d, fastest: %d, interval: %d", slowest, fastest, interval );
	for ( int i = 2; i < ( SPEED_ARRAY - 1 ); i++ ) {
		//		int next = slowest + ( ( i - 1 ) * interval );
		//		syslog(LOG_NOTICE, "%d  %d", i, next );
		reverse[i].left = slowest + ( ( i - 1 ) * interval );
	}
	slowest = reverse[1].right;
	fastest = reverse[SPEED_ARRAY-1].right;
	interval = ( fastest - slowest ) / ( SPEED_ARRAY - 2 );
	for ( int i = 2; i < ( SPEED_ARRAY - 1 ); i++ ) {
		reverse[i].right = slowest + ( ( i - 1 ) * interval );
	}
}
