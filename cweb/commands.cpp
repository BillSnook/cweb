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

//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
#include <syslog.h>


#define bufferSize	256

Commander	commander;
Hardware	hardware;

void Commander::setupCommander() {	// ?
	
	hardware = Hardware();
}

void Commander::serviceCommand( char *command ) {	// Main listening routine

	bool commandLoop = true;
	while ( commandLoop ) {
		syslog(LOG_NOTICE, "In commandLoop with: %s", command );
		char first = command[0];	// Get command
		switch ( first ) {
			case 'A':
			case 'a':
				hardware.setupForDCMotors();
				hardware.setMtrDirSpd( 0, 1, 40 );
				usleep( 500000 );
				hardware.setMtrSpd( 0, 0 );
				break;
				
			case 'B':
			case 'b':
				hardware.setupForDCMotors();
				hardware.setMtrDirSpd( 0, 0, 40 );
				usleep( 500000 );
				hardware.setMtrSpd( 0, 0 );
				break;
				
			case 'C':
			case 'c':
				hardware.setupForDCMotors();
				hardware.setMtrDirSpd( 1, 1, 40 );
				usleep( 500000 );
				hardware.setMtrSpd( 1, 0 );
				break;
				
			case 'D':
			case 'd':
				hardware.setupForDCMotors();
				hardware.setMtrDirSpd( 1, 0, 40 );
				usleep( 500000 );
				hardware.setMtrSpd( 1, 0 );
				break;
				
			case 'X':
			case 'x':
				hardware.setupForDCMotors();
				for ( int i = 0; i < 2; i++ ) {
				hardware.setMtrDirSpd( 1, 1, 30 );
				usleep( 100000 );
				hardware.setMtrDirSpd( 1, 1, 0 );
				usleep( 50000 );
				hardware.setMtrDirSpd( 1, 0, 30 );
				usleep( 100000 );
				hardware.setMtrDirSpd( 1, 0, 0 );
				usleep( 50000 );
				hardware.setMtrDirSpd( 0, 1, 30 );
				usleep( 100000 );
				hardware.setMtrDirSpd( 0, 1, 0 );
				usleep( 50000 );
				hardware.setMtrDirSpd( 0, 0, 30 );
				usleep( 100000 );
				hardware.setMtrDirSpd( 0, 0, 0 );
				usleep( 50000 );
				}
				break;
				
			default:
				usleep( 10000000 ); // 10 second delay
				break;
		}
		commandLoop = false;
	}
}
