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
//#include "actions.hpp"
#include "minion.hpp"       // For testing smb/i2c upgrades

#include <stdlib.h>			// malloc
#include <stdio.h>			// sprintf
#include <syslog.h>			// close read write
#include <string.h>			// strcat
#include <strings.h>		// strlen


#define bufferSize	256

//#define aWaitOn		500000
#define xWaitOn		100000
#define xWaitOff	50000
#define tokenMax	5

Commander	commander;
TaskMaster	taskMaster;
Hardware	hardware;

extern Minion   minion;


extern Filer	filer;
//extern Listener	listener;
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
	int tokenCount = 0;
	do {
		nextToken[tokenCount] = &command[i];
		tokenCount++;
		do {
			i++;
		} while ( ( i < len ) && ( command[i] != ' ' ) && ( command[i] != '\n' ) && ( command[i] != 0 ) );
		command[i] = 0;
		i++;
	} while ( ( i < len ) && ( tokenCount < tokenMax ) );
//	int tokenCount = t;
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

	char msg[ 1024 ];
//	char *msg = (char *)malloc( 1024 );
	memset( msg, 0, 1024 );
	switch ( commandType ) {
			// Motor control for direct screen
		case '0':
			hardware.setMtrDirSpd( 0, token1, token2 );
			break;
			
		case '1':
			hardware.setMtrDirSpd( 1, token1, token2 );
			break;
			
		case '2':
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
			// Controller commands
		case 'A':
		case 'a':
			syslog(LOG_NOTICE, "Command a calls: setStatus()" );
			manager.setStatus();
			break;
			
		case 'B':
		case 'b':
		{
			long response = manager.getStatus();
			syslog(LOG_NOTICE, "Command b calls: getStatus(): 0x%08lX", response );
			long vIn = ( response >> 16 ) & 0x0FFFF;
			double voltage = ( vIn * 15 ) / 1024.0;			// vIn is 1/3 of input voltage
			sprintf((char *)msg, "vIn: %0.2F", voltage );
		}
			break;

		case 'C':
		case 'c':
			manager.setRange( 90 );
			syslog(LOG_NOTICE, "Command c calls: setRange()" );
			break;
			
		case 'D':
		case 'd':
		{
			unsigned int range = manager.getRange();
			syslog(LOG_NOTICE, "Command c calls: getRange(), got: %d (0x%04X)", range, range );
		}
			break;
			
		case 'E':
		case 'e':
		{
			char *display = (char *)manager.sitMap.returnMap( msg );	// msg is 1024 bytes
			syslog(LOG_NOTICE, "Command e calls: returnMap(), got: %s", display );
//			listener.writeBlock( msg, int( strlen( (char *)msg ) ), socket );
		}
			break;
			
		case 'F':
		case 'f':
			syslog(LOG_NOTICE, "Command f calls: manager.setMotorPower( token1 )" );
			manager.setMotorPower( token1 );
			break;
			
//		case 'C':
//		case 'c':
//			syslog(LOG_NOTICE, "Command c calls: manager.testWrite( 'Test' )" );
////			memcpy( buffer, "Test", 5 );
//			manager.testWrite( (unsigned char *)"Test" );
//			break;
//
//		case 'D':
//		case 'd':
//			syslog(LOG_NOTICE, "Command d calls: manager.testWrite( 'Test1' )" );
//			manager.testWrite( (unsigned char *)"Test1" );
//			break;
//
//		case 'E':
//		case 'e':
//			syslog(LOG_NOTICE, "Command e calls: manager.testWrite( 'Test12' )" );
//			manager.testWrite( (unsigned char *)"Test12" );
//			break;
//
//		case 'F':
//		case 'f':
//			syslog(LOG_NOTICE, "Command f calls: manager.testRead()" );
//			manager.testRead();
//			break;
//
			// Motor/speed commands
		case 'G':
		case 'g':
			syslog(LOG_NOTICE, "Command g calls: hardware.cmdSpeed( %d )", token1 );
			hardware.cmdSpeed( token1 );
			break;
			
		case 'H':
		case 'h':
		{
			char *display = hardware.speed.displaySpeedArray( (char *)msg );
			syslog(LOG_NOTICE, "displaySpeedArray():\n%s", display );
//			memcpy( msg, display, strlen( display ) );
//			free( display );
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

// Testing for Map page
		case 'N':
		case 'n':
			hardware.prepPing( token1, token2, token3 );
			hardware.scanPing( socket );
			break;
			
        case 'O':
        case 'o':
// WFS           actor.doTest();            // Available
        {
            putI2CCmd( 's', 0x66 );
            unsigned char buffSpace[10] = {0};
            unsigned char *buffer = buffSpace;
            minion.getI2CData( buffer );
            syslog(LOG_NOTICE, "Test o, got response: %02X %02X %02X %02X\n", buffer[0], buffer[1], buffer[2], buffer[3] );
            break;
        }
        case 'P':
        case 'p':
//            actor.doTest();
        {
            putI2CCmd( 'p', 0x99 );
            unsigned char buffSpace[10] = {0};
            unsigned char *buffer = buffSpace;
            minion.getI2CData( buffer );
            syslog(LOG_NOTICE, "Test p, got response: %02X %02X %02X %02X\n", buffer[0], buffer[1], buffer[2], buffer[3] );
            break;
        }
//		case 'R':
//		case 'r':
//			filer.readData( hardware.speed.forward, hardware.speed.reverse );
//			break;
			
		case 'S':
		case 's':
			manager.stopVL();
			syslog(LOG_NOTICE, "Did stopVL" );
			hardware.scanStop();
			syslog(LOG_NOTICE, "Did scanStop" );
			hardware.cmdSpeed( 0 );
			syslog(LOG_NOTICE, "Did cmdSpeed" );
//			taskMaster.mobileTask( 0, 0 );
//			syslog(LOG_NOTICE, "Did mobiletask" );
			hardware.centerServo();
			syslog(LOG_NOTICE, "Did centerservo" );
			break;
		// Test case for app feature - send response, wait 5 seconds, send another
		case 'T':
		case 't':
			memcpy( msg, "\nMessage 1...\n", 15 );
			listener.writeBack( (char *)msg, socket );
			usleep( 5000000 ); // 5 second delay as test
			memcpy( msg, "\nMessage 2   \n", 15 );	// msg gets written at end of this routine
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
			sprintf((char *)msg, "@ %d %d %d \n", SPEED_ARRAY, SPEED_ADJUSTMENT, SPEED_ADJUSTMENT * 2 );
			break;
			
		default:
//			usleep( 10000000 ); // 10 second delay
			break;
	}
	if ( strlen((char *)msg) > 0 ) {
		listener.writeBack( (char *)msg, socket );
	}
//	free( msg );
}
