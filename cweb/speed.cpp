//
//  speed.cpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "speed.hpp"
//#include "filer.hpp"


#include <syslog.h>			// close read write
//#include <stdio.h>			// printf
//#include <fcntl.h>			// open
//#include <sys/ioctl.h>
//#include <getopt.h>

extern Filer	filer;

// Converts speed index into speeds

Speed::Speed() {
	
}

bool Speed::setupForSpeed() {
	
#ifdef ON_PI
#endif  // ON_PI
	
	return true;
}

bool Speed::resetForSpeed() {
	
#ifdef ON_PI
#endif  // ON_PI
	
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

void Speed::resetSpeedArray() {
	
	for ( int i = 0; i < SPEED_ARRAY; i++ ) {
		forward[i].left = i * SPEED_ADJUSTMENT;
		forward[i].right = i * SPEED_ADJUSTMENT;
		reverse[i].left = i * SPEED_ADJUSTMENT;
		reverse[i].right = i * SPEED_ADJUSTMENT;
	}
}

void Speed::displaySpeedArray() {
	
	syslog(LOG_NOTICE, "Speed array, forward:" );
	for ( int i = 0; i < SPEED_ARRAY; i++ ) {
		syslog(LOG_NOTICE, "i: %d - l: %d, r: %d", i, forward[i].left, forward[i].right );
	}
	syslog(LOG_NOTICE, "Speed array, reverse:" );
	for ( int i = 0; i < SPEED_ARRAY; i++ ) {
		syslog(LOG_NOTICE, "i: %d - l: %d, r: %d", i, reverse[i].left, reverse[i].right );
	}
}

int Speed::speedLeft( int speedIndex ) {
	if ( ( speedIndex > -SPEED_ARRAY ) && ( speedIndex < SPEED_ARRAY ) ) {
		if ( speedIndex > 0 ) {
			return forward[speedIndex].left;
		} else {
			return reverse[speedIndex].left;
		}
	}
	return 0;
}

int Speed::speedRight( int speedIndex ) {
	if ( ( speedIndex > -SPEED_ARRAY ) && ( speedIndex < SPEED_ARRAY ) ) {
		if ( speedIndex > 0 ) {
			return forward[speedIndex].right;
		} else {
			return reverse[speedIndex].right;
		}
	}
	return 0;
}
