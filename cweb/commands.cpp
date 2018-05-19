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
#include <syslog.h>
#include <string.h>


#define bufferSize	256

#define aWaitOn		500000
#define xWaitOn		100000
#define xWaitOff	50000
#define tokenMax	4

Commander	commander;
Hardware	hardware;

extern Filer	filer;
extern Listener	listener;

void Commander::setupCommander() {	// ?
	
	syslog(LOG_NOTICE, "In setupCommander" );
	hardware = Hardware();
	hardware.setupForDCMotors();
}

void Commander::serviceCommand( char *command, int socket ) {	// Main command determination routine

	syslog(LOG_NOTICE, "In commandLoop with: %s", command );
	int len = int( strlen( command ) );
	char *nextToken[tokenMax];
	int i = 0;
	int t = 0;
	do {
		nextToken[t] = &command[i];
		t++;
		do {
			i++;
		} while ( ( command[i] != ' ' ) && ( command[i] != '\n' ) );
		command[i] = 0;
		i++;
	} while ( ( i < len ) && ( t < tokenMax ) );
	int tokenCount = t;
	int token1 = 0;
	int token2 = 0;
	if ( tokenCount > 1 ) {
		token1 = std::atoi( nextToken[1] );
	}
	if ( tokenCount > 2 ) {
		token2 = std::atoi( nextToken[2] );
	}
	char first = nextToken[0];	// Get command

	for ( int y = 0; y < tokenCount; y++ ) {
		syslog(LOG_NOTICE, "Token %d: %s", y, nextToken[y] );
	}

	char *msg = (char *)malloc( 1024 );
	memset( msg, 0, 1024 )
	memcpy( msg, "\nAck\n", 5 );
	switch ( first ) {
//		case 'A':
//		case 'a':
//			hardware.setMtrDirSpd( 0, 1, token1 );
//			usleep( aWaitOn );
//			hardware.setMtrSpd( 0, 0 );
//			break;
//
//		case 'B':
//		case 'b':
//			hardware.setMtrDirSpd( 0, 0, token1 );
//			usleep( aWaitOn );
//			hardware.setMtrSpd( 0, 0 );
//			break;
//
//		case 'C':
//		case 'c':
//			hardware.setMtrDirSpd( 1, 1, token1 );
//			usleep( aWaitOn );
//			hardware.setMtrSpd( 1, 0 );
//			break;
//
//		case 'D':
//		case 'd':
//			hardware.setMtrDirSpd( 1, 0, token1 );
//			usleep( aWaitOn );
//			hardware.setMtrSpd( 1, 0 );
//			break;
//
//		case 'E':
//		case 'e':
//			hardware.setMtrDirSpd( 0, 1, token1 );
//			hardware.setMtrDirSpd( 1, 1, token2 );
//			usleep( aWaitOn );
//			hardware.setMtrSpd( 0, 0 );
//			hardware.setMtrSpd( 1, 0 );
//			break;
//
//		case 'F':
//		case 'f':
//			hardware.setMtrDirSpd( 1, 0, token1 );
//			hardware.setMtrDirSpd( 1, 0, token2 );
//			usleep( aWaitOn );
//			hardware.setMtrSpd( 0, 0 );
//			hardware.setMtrSpd( 1, 0 );
//			break;
//
		case 'G':
		case 'g':
			syslog(LOG_NOTICE, "Command g calls: hardware.cmdSpd( %d )", token1 );
			hardware.cmdSpd( token1 );
			break;
			
		case 'H':
		case 'h':
		{
			syslog(LOG_NOTICE, "Command h calls: hardware.speed.displaySpeedArray()" );
			char *display = hardware.speed.displaySpeedArray();
			memcpy( msg, display, strlen( display ) );
			free( display );
		}
			break;
			
		case 'I':
		case 'i':
			filer.readData( hardware.speed.forward, hardware.speed.reverse );
//			hardware.speed.resetSpeedArray();
			memcpy( msg, "\nData read\n", 11 );
			break;
			
		case 'J':
		case 'j':
			hardware.speed.setSpeedTestIndex( token1 );
			break;
			
		case 'K':
		case 'k':
			hardware.speed.setSpeedRight( token1 );
			break;
			
		case 'L':
		case 'l':
			hardware.speed.setSpeedRight( token1 );
			break;
			
		case 'R':
		case 'r':
			filer.readData( hardware.speed.forward, hardware.speed.reverse );
			break;
			
		case 'S':
		case 's':
			hardware.cmdSpd( 0 );
			break;
			
		case 'W':
		case 'w':
			filer.saveData( hardware.speed.forward, hardware.speed.reverse );
			break;
			
		case 'X':
		case 'x':
			for ( int i = 0; i < 3; i++ ) {
				hardware.setMtrDirSpd( 1, 1, token1 );
				usleep( xWaitOn );
				hardware.setMtrDirSpd( 1, 1, 0 );
				usleep( xWaitOff );
				hardware.setMtrDirSpd( 1, 0, token1 );
				usleep( xWaitOn );
				hardware.setMtrDirSpd( 1, 0, 0 );
				usleep( xWaitOff );
				hardware.setMtrDirSpd( 0, 1, token1 );
				usleep( xWaitOn );
				hardware.setMtrDirSpd( 0, 1, 0 );
				usleep( xWaitOff );
				hardware.setMtrDirSpd( 0, 0, token1 );
				usleep( xWaitOn );
				hardware.setMtrDirSpd( 0, 0, 0 );
				usleep( xWaitOff );
			}
			break;

		case 'Z':
		case 'z':
			memcpy( msg, "\nStatus pending...\n", 19 );
			break;
			
		default:
//			usleep( 10000000 ); // 10 second delay
			break;
	}
	listener.writeBack( msg, socket );
	free( msg );
}
