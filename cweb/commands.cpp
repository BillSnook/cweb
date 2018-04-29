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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <arpa/inet.h>


#define bufferSize	256

Commander	commander;
Hardware	hardware;

void Commander::setupCommander() {	// ?
	
	hardware = Hardware();
}

void Commander::serviceCommand( char *command ) {	// Main listening routine

	bool commandLoop = true;
	while ( commandLoop ) {
		fprintf( stderr, "\nIn commandLoop with: %s\n", command );
		char first = command[0];	// Get command
		switch ( first ) {
			case 'A':
			case 'a':
				hardware.setupForDCMotors();
				hardware.setMtrDirSpd( 1, 1, 40 );
				usleep( 500000 );
				hardware.setMtrSpd( 1, 0 );
				break;
				
			default:
				usleep( 10000000 ); // 10 second delay
				break;
		}
		commandLoop = false;
	}
}
