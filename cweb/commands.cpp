//
//  commands.cpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "commands.hpp"

#include "listen.hpp"
#include "tasks.hpp"
#include "hardware.hpp"
#include "manager.hpp"

#include <stdlib.h>			// malloc
#include <stdio.h>			// sprintf
#include <syslog.h>			// close read write
#include <string.h>			// strcat


#define bufferSize	256

//#define aWaitOn		500000
#define xWaitOn		100000
#define xWaitOff	50000
#define tokenMax	5

Commander	commander;
TaskMaster	taskMaster;
Hardware	hardware;


extern Filer	filer;
extern Listener	listener;
extern Manager 	manager;

void Commander::setupCommander() {
	
	syslog(LOG_NOTICE, "In setupCommander" );
	hardware = Hardware();
	hardware.setupHardware();
}

void Commander::shutdownCommander() {
	
	syslog(LOG_NOTICE, "In shutdownCommander" );
	hardware.resetHardware();
}

void Commander::serviceCommand( char *command, int socket ) {	// Main command determination routine

	syslog(LOG_NOTICE, "In serviceCommand with: %s", command );
	int len = int( strlen( command ) );
	char *nextToken[tokenMax+1];
	int i = 0;
	int t = 0;
	do {
		nextToken[t] = &command[i];
		t++;
		do {
			i++;
		} while ( ( i < len ) && ( command[i] != ' ' ) && ( command[i] != '\n' ) && ( command[i] != 0 ) );
		command[i] = 0;
		i++;
	} while ( ( i < len ) && ( t < tokenMax ) );
	int tokenCount = t;
	int token1 = 0;
	int token2 = 0;
	int token3 = 0;
	int token4 = 0;
	if ( tokenCount > 1 ) {
		token1 = atoi( nextToken[1] );
	}
	if ( tokenCount > 2 ) {
		token2 = atoi( nextToken[2] );
	}
	if ( tokenCount > 3 ) {
		token3 = atoi( nextToken[3] );
	}
	if ( tokenCount > 4 ) {
		token4 = atoi( nextToken[4] );
	}
	char commandType = command[0];	// Get command

//	for ( int y = 0; y < tokenCount; y++ ) {
//		syslog(LOG_NOTICE, "Token %d: %s", y, nextToken[y] );
//	}

//	unsigned char buffer[20] = {0};
	char *msg = (char *)malloc( 1024 );
	memset( msg, 0, 1024 );
//	memcpy( msg, "\nAck\n", 5 );
	switch ( commandType ) {
		case '0':
			hardware.setMtrDirSpd( 0, token1, token2 );
			break;
			
		case '1':
			hardware.setMtrDirSpd( 1, token1, token2 );
			break;
			
		case 'A':
			hardware.setMotors( token1, token2, token3, token4 );
			break;
			
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
		case 'B':
		case 'b':
			syslog(LOG_NOTICE, "Command b calls: manager.testRead()" );
			manager.testRead();
			break;
			
		case 'C':
		case 'c':
			syslog(LOG_NOTICE, "Command c calls: manager.testWrite('Test' )" );
//			memcpy( buffer, "Test", 5 );
			manager.testWrite( (unsigned char *)"Test" );
			break;
			
		case 'D':
		case 'd':
			syslog(LOG_NOTICE, "Command d calls: manager.testWrite('Test1' )" );
			manager.testWrite( (unsigned char *)"Test1" );
			break;
			
		case 'E':
		case 'e':
			syslog(LOG_NOTICE, "Command e calls: manager.testWrite('Test12' )" );
			manager.testWrite( (unsigned char *)"Test12" );
			break;
			
		case 'G':
		case 'g':
			syslog(LOG_NOTICE, "Command g calls: hardware.cmdSpeed( %d )", token1 );
			hardware.cmdSpeed( token1 );
			break;
			
		case 'H':
		case 'h':
		{
			char *display = hardware.speed.displaySpeedArray();
			syslog(LOG_NOTICE, "displaySpeedArray():\n%s", display );
			memcpy( msg, display, strlen( display ) );
			free( display );
		}
			break;
			
		case 'I':
		case 'i':
			filer.readData( hardware.speed.forward, hardware.speed.reverse );
			memcpy( msg, "\nData read\n", 11 );
			break;
			
		case 'J':
		case 'j':
		{
			char *display = hardware.speed.setSpeedTestIndex( token1 );
			memcpy( msg, display, strlen( display ) );
			free( display );
		}
			break;
			
		case 'K':
		case 'k':
			hardware.speed.setSpeedRight( token1 );
			break;
			
		case 'L':
		case 'l':
			hardware.speed.setSpeedLeft( token1 );
			break;
			
		case 'M':
		case 'm':
			taskMaster.mobileTask( token1, token2 );
			break;

//		case 'R':
//		case 'r':
//			filer.readData( hardware.speed.forward, hardware.speed.reverse );
//			break;
			
		case 'S':
		case 's':
			hardware.cmdSpeed( 0 );
			taskMaster.mobileTask( 0, 0 );
			hardware.centerServo();
			break;
		// Test case for app feature - send response, wait 5 seconds, send another
		case 'T':
		case 't':
			memcpy( msg, "\nMessage 1...\n", 15 );
			listener.writeBack( msg, socket );
			usleep( 5000000 ); // 5 second delay
			memcpy( msg, "\nMessage 2   \n", 15 );
			break;
			
		case 'U':
		case 'u':	// Set forward array based on index 1 and index 8
			hardware.speed.setSpeedForward();
			break;
			
		case 'V':
		case 'v':	// Set reverse array based on index -1 and index -8
			hardware.speed.setSpeedReverse();
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

		case 'Y':
		case 'y':
			hardware.scanTest();
			break;
			
		case 'Z':
		case 'z':
			sprintf(msg, "@ %d %d %d \n", SPEED_ARRAY, SPEED_ADJUSTMENT, SPEED_ADJUSTMENT * 2 );
			break;
			
		default:
//			usleep( 10000000 ); // 10 second delay
			break;
	}
	if ( strlen(msg) > 0 ) {
		listener.writeBack( msg, socket );
	}
	free( msg );
}
