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

#include "filer.hpp"
//#include "speed.hpp"


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
	
	bool success = filer.readSpeedArrays( forward, reverse );
	if ( ! success ) {
		syslog(LOG_NOTICE, "Failed reading speed array from file; making and saving default one" );
		resetSpeedArray();
        filer.saveSpeedArrays( forward, reverse );
	} else {
		syslog(LOG_NOTICE, "Read speed array from file" );
	}
}

void Speed::resetSpeedArray() {		// Create simple default to assist calibration
	
	for ( int i = 0; i < SPEED_INDEX_MAX; i++ ) {
        int setSpeed = i * SPEED_ADJUSTMENT;
        if (setSpeed > SPEED_MAX_PWM) {
            setSpeed = SPEED_MAX_PWM;
        }
		forward[i].left = setSpeed;
		forward[i].right = setSpeed;
		reverse[i].left = setSpeed;
		reverse[i].right = setSpeed;
	}
}

void Speed::returnSpeedArray( char *displayString ) {
    
    sprintf( displayString, "S %d\n", SPEED_INDEX_MAX - 1 );
    for ( int i = 0; i < SPEED_INDEX_MAX; i++ ) {
        sprintf( displayString, "%s%d %d %d\n", displayString, i, forward[i].left, forward[i].right );
    }
    for ( int i = 0; i < SPEED_INDEX_MAX; i++ ) {
        sprintf( displayString, "%s%d %d %d\n", displayString, -i, reverse[i].left, reverse[i].right );
    }
}

char * Speed::displaySpeedArray( char *displayString ) {
	
	strcat( displayString, " Speed array, forward:\n" );
	for ( int i = 0; i < SPEED_INDEX_MAX; i++ ) {
		sprintf( displayString, "%s i: %d - l: %d, r: %d\n", displayString, i, forward[i].left, forward[i].right );
	}
	strcat( displayString, " Speed array, reverse:\n" );
	for ( int i = 0; i < SPEED_INDEX_MAX; i++ ) {
		sprintf( displayString, "%s i: %d - l: %d, r: %d\n", displayString, -i, reverse[i].left, reverse[i].right );
	}
	return displayString;
}

char *Speed::setSpeedTestIndex( int newSpeedIndex ) {
	calibrationTestIndex = newSpeedIndex;
	char *displayString = (char *)malloc( 32 );
	sprintf( displayString, "i %d %d %d", newSpeedIndex, speedLeft( newSpeedIndex ), speedRight( newSpeedIndex ) );
	return( displayString );

}

// Get a power setting from an index
int Speed::speedLeft( int speedIndex ) {
	if ( ( speedIndex > -SPEED_INDEX_MAX ) && ( speedIndex < SPEED_INDEX_MAX ) ) {
		if ( speedIndex > 0 ) {
			return forward[speedIndex].left;
		} else {
			return reverse[-speedIndex].left;
		}
	}
	return 0;
}

int Speed::speedRight( int speedIndex ) {
	if ( ( speedIndex > -SPEED_INDEX_MAX ) && ( speedIndex < SPEED_INDEX_MAX ) ) {
		if ( speedIndex > 0 ) {
			return forward[speedIndex].right;
		} else {
			return reverse[-speedIndex].right;
		}
	}
	return 0;
}

void Speed::setSpeedBoth( int speedIndex, int leftSpeed, int rightSpeed ) {
    if ( ( speedIndex > -SPEED_INDEX_MAX ) && ( speedIndex < SPEED_INDEX_MAX ) ) {
        if ( speedIndex > 0 ) {
            forward[speedIndex].left = leftSpeed;
            forward[speedIndex].right = rightSpeed;
        } else {
            reverse[-speedIndex].left = leftSpeed;
            reverse[-speedIndex].right = rightSpeed;
        }
    }
}

void Speed::setSpeedLeft( int speedIndex, int newSpeed ) {
	if ( ( speedIndex > -SPEED_INDEX_MAX ) && ( speedIndex < SPEED_INDEX_MAX ) ) {
		if ( speedIndex > 0 ) {
			forward[speedIndex].left = newSpeed;
		} else {
			reverse[-speedIndex].left = newSpeed;
		}
	}
}

void Speed::setSpeedRight( int speedIndex, int newSpeed ) {
	if ( ( speedIndex > -SPEED_INDEX_MAX ) && ( speedIndex < SPEED_INDEX_MAX ) ) {
		if ( speedIndex > 0 ) {
			forward[speedIndex].right = newSpeed;
		} else {
			reverse[-speedIndex].right = newSpeed;
		}
	}
}

// For current test index
void Speed::setSpeedLeft( int newSpeed ) {
	if ( ( calibrationTestIndex > -SPEED_INDEX_MAX ) && ( calibrationTestIndex < SPEED_INDEX_MAX ) ) {
		if ( calibrationTestIndex > 0 ) {
			forward[calibrationTestIndex].left = newSpeed;
		} else {
			reverse[-calibrationTestIndex].left = newSpeed;
		}
	}
}

void Speed::setSpeedRight( int newSpeed ) {
	if ( ( calibrationTestIndex > -SPEED_INDEX_MAX ) && ( calibrationTestIndex < SPEED_INDEX_MAX ) ) {
		if ( calibrationTestIndex > 0 ) {
			forward[calibrationTestIndex].right = newSpeed;
		} else {
			reverse[-calibrationTestIndex].right = newSpeed;
		}
	}
}

// Assuming index 1 has been set to the slowest practicable speed, and index 8 to the highest,
// calculate a linear series of power settings so each index speed changes smoothly.
void Speed::setSpeedForward() {
	// Assume index 1 is slowest speed, index 8 is the fastest
	// Find a pattern so each entry is linear
	int slowest = forward[1].left;
	int fastest = forward[SPEED_INDEX_MAX-1].left;
	int interval = ( fastest - slowest ) / ( SPEED_INDEX_MAX - 2 );
	syslog(LOG_NOTICE, "Slowest: %d, fastest: %d, interval: %d", slowest, fastest, interval );
	for ( int i = 2; i < ( SPEED_INDEX_MAX - 1 ); i++ ) {
		forward[i].left = slowest + ( ( i - 1 ) * interval );
	}
	slowest = forward[1].right;
	fastest = forward[SPEED_INDEX_MAX-1].right;
	interval = ( fastest - slowest ) / ( SPEED_INDEX_MAX - 2 );
	for ( int i = 2; i < ( SPEED_INDEX_MAX - 1 ); i++ ) {
		forward[i].right = slowest + ( ( i - 1 ) * interval );
	}
}

void Speed::setSpeedReverse() {
	// Assume index 1 is slowest speed, index 8 is the fastest
	// Find a pattern so each entry is linear
	int slowest = reverse[1].left;
	int fastest = reverse[SPEED_INDEX_MAX-1].left;
	int interval = ( fastest - slowest ) / ( SPEED_INDEX_MAX - 2 );
	syslog(LOG_NOTICE, "Slowest: %d, fastest: %d, interval: %d", slowest, fastest, interval );
	for ( int i = 2; i < ( SPEED_INDEX_MAX - 1 ); i++ ) {
		reverse[i].left = slowest + ( ( i - 1 ) * interval );
	}
	slowest = reverse[1].right;
	fastest = reverse[SPEED_INDEX_MAX-1].right;
	interval = ( fastest - slowest ) / ( SPEED_INDEX_MAX - 2 );
	for ( int i = 2; i < ( SPEED_INDEX_MAX - 1 ); i++ ) {
		reverse[i].right = slowest + ( ( i - 1 ) * interval );
	}
}

void Speed::saveSpeedArray() {

    filer.saveSpeedArrays( forward, reverse );
}

void Speed::makeSpeedArray() {

    setSpeedForward();
    setSpeedReverse();
    filer.saveSpeedArrays( forward, reverse );
}
