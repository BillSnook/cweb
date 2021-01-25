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
#include "manager.hpp"
#include "filer.hpp"
#include "hardware.hpp"

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
Hardware	hardware;


extern TaskMaster   taskMaster;
extern Manager 	    manager;

void Commander::setupCommander() {
	
	syslog(LOG_NOTICE, "In setupCommander" );
	hardware.setupHardware();
}

void Commander::shutdownCommander() {
	
	syslog(LOG_NOTICE, "In shutdownCommander" );
	hardware.shutdownHardware();
}

// This is launched on it's own thread from the listeners serviceConnection when command data comes in over wifi
void Commander::serviceCommand( char *command, int sockOrAddr ) {	// Main command determination routine
    // WFS sockOrAddr is only used here for use as an indicator of where to respond,
    //  but this does not work with UDP datagrams that need an addr/port pair to target the response
    //  I propose to set up an array of response addr/port structs and pass the index here instead of the socket.
    //  That way a datagram mode can use it differently than a connection oriented service like TCP.
    //  And the value is only generated in listen.serviceConnection and used eventually only in writeBack.
//	syslog(LOG_NOTICE, "In serviceCommand with sockOrAddr %d, command %s", sockOrAddr, command );
    // First interpret tokens
    char *nextToken[tokenMax+1];
	int len = int( strlen( command ) );
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
	for ( int y = 0; y < tokenCount; y++ ) {        // Display all token values
		syslog(LOG_NOTICE, "Token %d: %s", y, nextToken[y] );
	}

    // Prepare for return msg
	char msg[ 1024 ]; // Reply back to sender, if non-empty at end of routine
//	char *msg = (char *)malloc( 1024 );
	memset( msg, 0, 1024 );
	switch ( commandType ) {
            // Commands '0' - '9' are used when we need the command to have higher thread scheduling priority
		case '0':
            hardware.prepPing( token1, token2, token3 );
            hardware.scanPing( sockOrAddr );
			break;
			
		case '1':
//			hardware.setMtrDirSpd( 1, token1, token2 );
			break;
			
		case '2':
			break;

            // Normal Controller commands, normal thread priority
		case 'A':
		case 'a':
            syslog(LOG_NOTICE, "Command a calls: setStatus( 0x%04X )", token1 );
            hardware.setStatus( token1 );
			break;
			
		case 'B':
		case 'b':
		{
			long response = hardware.getStatus();
			syslog(LOG_NOTICE, "Command b calls: getStatus(): 0x%08lX", response );
            if ( response & statusScannerOrientation ) {
                sprintf((char *)msg, "Status response: scanner inverted");
            } else {
                sprintf((char *)msg, "Status response: scanner upright" );
            }
		}
			break;

		case 'C':
		case 'c':
            hardware.cmdAngle( 0 );
//			manager.setRange( 90 );
//			syslog(LOG_NOTICE, "Command c calls: setRange()" );
			break;
			
		case 'D':
		case 'd':
            hardware.cmdAngle( 180 );
//		{
//			unsigned int range = manager.getRange();
//			syslog(LOG_NOTICE, "Command c calls: getRange(), got: %d (0x%04X)", range, range );
//		}
			break;
			
		case 'E':
		case 'e':
            hardware.cmdPWM( token1, token2 );
//		{
//            long range = hardware.pingTest( 90 );
//            double cm = range / 29.0 / 2.0;
//            double inches = range / 74.0 / 2.0;
////            long mm = (range*10)/29/2
//            sprintf( msg, "Distance is %.2f inches, %.2f cm", inches, cm );

//			char *display = (char *)manager.siteMap.returnMap( msg );	// msg is 1024 bytes
///			syslog(LOG_NOTICE, "Error - Sitmap moved" );
//			listener.writeBlock( msg, int( strlen( (char *)msg ) ), sockOrAddr );
//		}
			break;
			
		case 'F':
		case 'f':
            hardware.cmdAngle( token1 );
//			syslog(LOG_NOTICE, "Command f calls: manager.setMotorPower( token1 )" );
//			manager.setMotorPower( token1 );
			break;
			
			// Motor/speed commands
		case 'G':
		case 'g':
//			syslog(LOG_NOTICE, "Command g calls: hardware.cmdSpeed( %d )", token1 );
//			hardware.cmdSpeed( token1 );
        {
            long pingTimeuSec = hardware.doPing();
            long cm = pingTimeuSec/29/2;
            long inches = pingTimeuSec/74/2;
//            long mm = (pingTimeuSec*10)/29/2;
            sprintf( msg, "Ping distance %ld cm, %ld inches", cm, inches );
        }
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
			filer.readSpeedArrays( hardware.speed.forward, hardware.speed.reverse );
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
            switch ( token1 ) {
                case 3:
                    hardware.scanTest();
                    break;
                case 4:
                    hardware.scanPing( token2 );
                    break;
                case 5:
                    hardware.pingTest( 90 );
                    break;
                default:
                    break;
            }
//			taskMaster.mobileTask( token1, token2 );
			break;

// Testing for Map page
		case 'N':
		case 'n':
            hardware.pinState( token1, token2 );
//			hardware.prepPing( token1, token2, token3 );
//			hardware.scanPing( sockOrAddr );
			break;
			
        case 'O':
        case 'o':
//           actor.doTest();            // Available
        {
            manager.request( writeI2C, manager.file_i2c, 's', 0x66 );
            long status = manager.request( readI2C, manager.file_i2c, 4 );
            syslog(LOG_NOTICE, "Test getting status, got response: %08lX\n", status );
            break;
        }
        case 'P':
        case 'p':
//            actor.doTest();
        {
            manager.request( writeI2C, manager.file_i2c, 'p', token1 );
            long status = manager.request( readI2C, manager.file_i2c, 4 );
            syslog(LOG_NOTICE, "Test p, got response: %08lX\n", status );
            break;
        }
		case 'R':
		case 'r':
            // Motor control for direct screen
            hardware.setMotors( token1, token2, token3, token4 );
//			filer.readSpeedArrays( hardware.speed.forward, hardware.speed.reverse );
			break;
			
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
			listener.writeBack( (char *)msg, sockOrAddr );
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
			filer.saveSpeedArrays( hardware.speed.forward, hardware.speed.reverse );
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
		listener.writeBack( (char *)msg, sockOrAddr );
	}
//	free( msg );
}


/*
 
 Vehicle status
    Unknown
    Starting
    Ready
    Fault
 
 
 Vehicle modes
    Controlled
    Tasked - various: follow, flee, push. crash, etc.
    Autonomous
 
 
 */
