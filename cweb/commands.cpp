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
#include "actions.hpp"

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
//extern Actor        actor;


void Commander::setupCommander() {
	
	syslog(LOG_NOTICE, "In setupCommander" );
	hardware.setupHardware();
//    actor.setupActor();
}

void Commander::shutdownCommander() {
	
	syslog(LOG_NOTICE, "In shutdownCommander" );
//    actor.shutdownActor();
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
    char commandType = command[0];  // Get command
    if ( commandType == '?' ) {     // Keep-alive timed out, all stop
        hardware.cmdSpeed( 0 );
        hardware.scanStop();
        syslog(LOG_NOTICE, "scanStop in Commander::serviceCommand for command '?'" );
        hardware.centerServo();
//        actor.stop();
        return;
    }
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
//	for ( int y = 0; y < tokenCount; y++ ) {        // Display all token values
//		syslog(LOG_NOTICE, "Token %d: %s", y, nextToken[y] );
//	}

    // Prepare for return msg
	char msg[ 1024 ]; // Reply back to sender, if non-empty at end of routine
//	char *msg = (char *)malloc( 1024 );
	memset( msg, 0, 1024 );
	switch ( commandType ) {        // 0 - 9 very high priority thresd, capital letters run quickly, lower case run on a thread
            // Commands '0' - '9' are used when we need the command to have higher thread scheduling priority
		case '0':
            hardware.prepPing( token1, token2, token3 );
            hardware.scanPing( sockOrAddr );
			break;
			
        // Test and calibration routines
        case '8':   // Test ultrasonic ranger
            hardware.testPing( token1, token2 );
            break;

		case '9':   // Test lidar
//            actor.mainTest( token1, token2 );
			break;
			
            // MARK: Lower case are on a thread, upper case are being called from the listen response and should be quick - no sync calls or usleeps
        case '@':       // Doesn't need thread
        {
            long response = hardware.getStatus();
            syslog(LOG_NOTICE, "Command b calls: getStatus(): 0x%08lX", response );
            if ( response & statusScannerOrientation ) {
                sprintf((char *)msg, "Status response: scanner inverted");
            } else {
                sprintf((char *)msg, "Status response: scanner upright" );
            }
//            if ( actor.Version.major != 0 ) {
//                sprintf((char *)msg, "%s, lidar found", msg );
//            } else {
//                sprintf((char *)msg, "%s, no lidar found", msg );
//            }
            if ( manager.arduino_i2c > 0 ) {
                sprintf((char *)msg, "%s, arduino found", msg );
            } else {
                sprintf((char *)msg, "%s, no arduino found", msg );
            }
        }
            break;

            // MARK: - Calibration -- A through F reserved for scanner zeroing and speed syncronization
        case 'A':
//            syslog(LOG_NOTICE, "Command A, return rangeData" );
            sprintf((char *)msg, "R %d %d", hardware.rangeData.pwmCenter, hardware.rangeData.servoPort );
            break;
            
        case 'a':
//            syslog(LOG_NOTICE, "Command a, save rangeData, pwm: %d, servo pin: %d", token1, token2 );
            hardware.rangeData.pwmCenter = token1;
            hardware.rangeData.servoPort = token2;
            hardware.minimumPWM = token1 - 180;
            filer.saveRange( &(hardware.rangeData) );
            break;

        case 'B':
//            syslog(LOG_NOTICE, "Command B, cmdPWM to set servo to %d pwm value - ~150 to 550, 330 is nominal center", token1 );
            hardware.cmdPWM( token1 );
            break;
            
		case 'b':       // WFS Available for thread
			break;

		case 'C':
            syslog(LOG_NOTICE, "Command C, cmdAngle to set servo to %d degree value - 0 to 180, 90 is center", token1 );
            hardware.cmdAngle( token1 );
			break;
			
        case 'c':       // WFS Available for thread
            break;
            
		case 'D':
        {
            syslog(LOG_NOTICE, "Command D, return speed array data" );
            char *display = (char *)malloc( 1024 );
            hardware.speed.returnSpeedArray( display );
//            syslog(LOG_NOTICE, "returnSpeedArray():\n%s", display );
            memcpy( msg, display, strlen( display ) );
            free( display );
            break;
        }

        case 'd':       // WFS Available for thread
			break;
            
		case 'E':
            syslog(LOG_NOTICE, "Command E, set speed array entry" );
            hardware.speed.setSpeedBoth( token1, token2, token3 );
            break;
            
		case 'e':
            syslog(LOG_NOTICE, "Command e, setup speed array from endpoints and save it" );
            hardware.speed.saveSpeedArray();
			break;
			
		case 'F':
            syslog(LOG_NOTICE, "Command F, set speed directly to test entry" );
            hardware.setMotorsPWM( token1, token2, token3, token4 );
            break;
            
		case 'f':       // WFS Available for thread
			break;
			
            // MARK: - Calibration -- Motor control/speed commands
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
			syslog(LOG_NOTICE, "displaySpeedArray():\n%s", msg );
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
        case 'o':            // Available
        {
            manager.request( writeI2C, manager.arduino_i2c, 's', 0x66 );
            long status = manager.request( readI2C, manager.arduino_i2c, 4 );
            syslog(LOG_NOTICE, "Test getting status, got response: %08lX\n", status );
            break;
        }
        case 'P':
        case 'p':
        {
            manager.request( writeI2C, manager.arduino_i2c, 'p', token1 );
            long status = manager.request( readI2C, manager.arduino_i2c, 4 );
            syslog(LOG_NOTICE, "Test p %d, got response: %08lX\n", token1, status );
        }
            break;
        case 'Q':
//        case 'q':
            system( "sudo shutdown now" );
            break;
            
		case 'R':
		case 'r':
            // Motor control for direct screen
            hardware.setMotors( token1, token2, token3, token4 );
//			filer.readSpeedArrays( hardware.speed.forward, hardware.speed.reverse );
			break;
			
		case 'S':
            hardware.cmdSpeed( 0 );
            hardware.scanStop();
            syslog(LOG_NOTICE, "scanStop in Commander::serviceCommand for command 'S'" );
            hardware.centerServo();
//            actor.stop();
			break;

        case 's':
//            manager.stopVL();
            syslog(LOG_NOTICE, "Did stopVL" );
            hardware.scanStop();
            syslog(LOG_NOTICE, "Did scanStop for command 's'" );
            hardware.cmdSpeed( 0 );
            syslog(LOG_NOTICE, "Did cmdSpeed" );
//            taskMaster.killTasks();
//            syslog(LOG_NOTICE, "Did killTasks" );
            hardware.centerServo();
            syslog(LOG_NOTICE, "Did centerservo" );
//            actor.stop();
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
			sprintf((char *)msg, "@ %d %d %d \n", SPEED_INDEX_MAX, SPEED_ADJUSTMENT, SPEED_ADJUSTMENT * 2 );
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
