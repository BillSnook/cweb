//
//  speed.cpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "speed.hpp"
//#include "listen.hpp"


#include <syslog.h>			// close read write
#include <stdlib.h>			// malloc
#include <string.h>			// strcat
#include <stdio.h>			// sprintf
//#include <getopt.h>

extern Filer	filer;

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

char * Speed::displaySpeedArray() {
	
	char *displayString = (char *)malloc( 1024 );
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
