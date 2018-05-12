//
//  commands.cpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "commands.hpp"

#include "listen.hpp"
#include "threader.hpp"
#include "hardware.hpp"

#include <syslog.h>
#include <string.h>


#define bufferSize	256

#define aWaitOn		500000
#define xWaitOn		100000
#define xWaitOff	50000

Commander	commander;
Hardware	hardware;

void Commander::setupCommander() {	// ?
	
	hardware = Hardware();
}

void Commander::serviceCommand( char *command ) {	// Main listening routine

	bool commandLoop = true;
	while ( commandLoop ) {
		syslog(LOG_NOTICE, "In commandLoop with: %s", command );
		int len = int( strlen( command ) );
		char first = command[0];	// Get command
		unsigned char speed1 = 30;
		unsigned char speed2 = 30;
//		if ( len > 2 ) {
//			speed1 = command[1];
//		}
//		if ( len > 3 ) {
//			speed2 = command[2];
//		}
		hardware.setupForDCMotors();
		switch ( first ) {
			case 'A':
			case 'a':
				hardware.setMtrDirSpd( 0, 1, speed1 );
				usleep( aWaitOn );
				hardware.setMtrSpd( 0, 0 );
				break;
				
			case 'B':
			case 'b':
				hardware.setMtrDirSpd( 0, 0, speed1 );
				usleep( aWaitOn );
				hardware.setMtrSpd( 0, 0 );
				break;
				
			case 'C':
			case 'c':
				hardware.setMtrDirSpd( 1, 1, speed1 );
				usleep( aWaitOn );
				hardware.setMtrSpd( 1, 0 );
				break;
				
			case 'D':
			case 'd':
				hardware.setMtrDirSpd( 1, 0, speed1 );
				usleep( aWaitOn );
				hardware.setMtrSpd( 1, 0 );
				break;
				
			case 'E':
			case 'e':
				hardware.setMtrDirSpd( 0, 1, speed1 );
				hardware.setMtrDirSpd( 1, 1, speed2 );
				usleep( aWaitOn );
				hardware.setMtrSpd( 0, 0 );
				hardware.setMtrSpd( 1, 0 );
				break;
				
			case 'F':
			case 'f':
				hardware.setMtrDirSpd( 1, 0, speed1 );
				hardware.setMtrDirSpd( 1, 0, speed2 );
				usleep( aWaitOn );
				hardware.setMtrSpd( 0, 0 );
				hardware.setMtrSpd( 1, 0 );
				break;
				
			case 'S':
			case 's':
				hardware.setMtrSpd( 0, 0 );
				hardware.setMtrSpd( 1, 0 );
				break;
				
			case 'X':
			case 'x':
				for ( int i = 0; i < 3; i++ ) {
					hardware.setMtrDirSpd( 1, 1, speed1 );
					usleep( xWaitOn );
					hardware.setMtrDirSpd( 1, 1, 0 );
					usleep( xWaitOff );
					hardware.setMtrDirSpd( 1, 0, speed1 );
					usleep( xWaitOn );
					hardware.setMtrDirSpd( 1, 0, 0 );
					usleep( xWaitOff );
					hardware.setMtrDirSpd( 0, 1, speed1 );
					usleep( xWaitOn );
					hardware.setMtrDirSpd( 0, 1, 0 );
					usleep( xWaitOff );
					hardware.setMtrDirSpd( 0, 0, speed1 );
					usleep( xWaitOn );
					hardware.setMtrDirSpd( 0, 0, 0 );
					usleep( xWaitOff );
				}
				break;

			case 'Z':
			case 'z':
				
				break;
				
			default:
				usleep( 10000000 ); // 10 second delay
				break;
		}
		commandLoop = false;
	}
}
