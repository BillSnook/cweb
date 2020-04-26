//
//  manager.cpp
//  cweb
//
//  Created by William Snook on 1/12/19.
//  Copyright © 2019 billsnook. All rights reserved.
//

#include <time.h>
#include <syslog.h>
#include <stdio.h>			// sprintf

#include "vl53l0x.hpp"
#include "hardware.hpp"
#include "manager.hpp"


#ifdef ON_PI

#include <wiringPi.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <wiringPiI2C.h>

#endif  // ON_PI

#include "map.hpp"


enum CheckTimes {	// milliSecond interval for various checks
	statusCheckInterval = 5000L,
	heartBeatInterval = 1000L
};

// MARK: - I2C Queue

I2CControl I2CControl::initControl( I2CType type, int file, int command, int param ) {
    I2CControl newI2CControl = I2CControl();
    newI2CControl.i2cType = type;
    newI2CControl.i2cFile = file;
    newI2CControl.i2cCommand = command;
    newI2CControl.i2cParam = param;
//    newI2CControl.i2cData = nullptr;
    return newI2CControl;
}

I2CControl I2CControl::initControl( I2CType type, int file, int command, char *buffer ) {
    I2CControl newI2CControl = I2CControl();
    newI2CControl.i2cType = type;
    newI2CControl.i2cFile = file;
    newI2CControl.i2cCommand = command;
    newI2CControl.i2cParam = 0;
    newI2CControl.i2cData = buffer;
    return newI2CControl;
}

const char *I2CControl::description() {
    const char *name;
    switch ( i2cType ) {
        case writeReg8I2C:
            name = "writeReg8I2C";
            break;
        case readReg8I2C:
            name = "readReg8I2C";
            break;
        case writeI2C:
            name = "writeI2C";
            break;
        case readI2C:
            name = "readI2C";
            break;
        case otherI2C:
            name = "otherI2C";
            break;
        default:
            name = "noI2C";
            break;
    }
    return name;
}

// MARK: - Manager
void Manager::setupManager() {
	stopLoop = false;
	lastAnythingTime = 0;
	lastStatusTime = 0;
	expectedControllerMode = initialMode;
	syslog(LOG_NOTICE, "In setupManager" );

//	vl53l0x = VL53L0X();				// VL53L0xes talk to the array of light-rangers
//	vl53l0x.setupVL53L0X( 0x29 );
	
    file_i2c = wiringPiI2CSetup( ArdI2CAddr );

    pthread_mutex_init( &i2cQueueMutex, NULL );
    pthread_cond_init( &i2cQueueCond, NULL );

    pthread_mutex_init( &readWaitMutex, NULL );
    pthread_cond_init( &readWaitCond, NULL );

}

void Manager::shutdownManager() {

    stopLoop = true;
    if ( vl53l0x.isSetup ) {
		vl53l0x.shutdownVL53L0X();
	}

    pthread_mutex_lock( &readWaitMutex );
    pthread_cond_signal( &readWaitCond );   // Unblock thread so it can exit
    pthread_mutex_unlock( &readWaitMutex );
    
    pthread_mutex_lock( &i2cQueueMutex );
    pthread_cond_signal( &i2cQueueCond );
    pthread_mutex_unlock( &i2cQueueMutex );
    
    usleep( 1000 );
    
    pthread_mutex_destroy( &readWaitMutex );
    pthread_cond_destroy( &readWaitCond );
    
    pthread_mutex_destroy( &i2cQueueMutex );
    pthread_cond_destroy( &i2cQueueCond );

	syslog(LOG_NOTICE, "In shutdownManager" );
}

void Manager::monitor() {       // Wait for an i2c bus request, then execute it
	
	syslog(LOG_NOTICE, "In Manager::monitor, should only start once" );


    while ( !stopLoop ) {
        I2CControl i2cControl;

        pthread_mutex_lock( &i2cQueueMutex );
        
        while ( i2cQueue.empty() && !stopLoop ) {    // Until there is a queue entry
            pthread_cond_wait( &i2cQueueCond, &i2cQueueMutex ); // Free mutex and wait
        }
        if ( !stopLoop ) {
            try {
                i2cControl = i2cQueue.front();
                i2cQueue.pop();
                syslog(LOG_NOTICE, "In Manager::monitor, i2c command from queue" );
            } catch(...) {
                stopLoop = true;    // Error, we're done
                syslog(LOG_NOTICE, "In Manager::monitor, i2c queue pop failure occured" );
            }
        }
        
        pthread_mutex_unlock( &i2cQueueMutex );
        
        if ( !stopLoop ) {
            execute( i2cControl );
        }
	}
}

void Manager::execute( I2CControl i2cControl ) {
    
    syslog(LOG_NOTICE, "In Manager::execute, command type: %d, %d started", i2cControl.i2cType, i2cControl.i2cCommand );
    
    switch ( i2cControl.i2cType ) {
        case writeI2C:
        case writeReg8I2C:
            {
                unsigned char buffer[4] = {0};
                buffer[0] = i2cControl.i2cCommand;  // Send this command
                buffer[1] = i2cControl.i2cParam;    // With this optional parameter
                write( i2cControl.i2cFile, buffer, 2 );
            }
            break;

        case readI2C:
            {
                pthread_mutex_lock( &readWaitMutex );
                read( i2cControl.i2cFile, &i2cControl.i2cData[2], i2cControl.i2cCommand );
                syslog(LOG_NOTICE, "In Manager::execute, data read: %02X %02X %02X %02X, command: 0x%04X\n", i2cControl.i2cData[2], i2cControl.i2cData[3], i2cControl.i2cData[4], i2cControl.i2cData[5], i2cControl.i2cCommand);
//                i2cControl.i2cParam = (i2cControl.i2cData[2] << 24) | (i2cControl.i2cData[3] << 16) | (i2cControl.i2cData[4] << 8) | i2cControl.i2cData[5];
                i2cControl.i2cData[0] = 1;  // Signal completion
                i2cControl.i2cCommand = 0;  // Signal completion - deprecated
                pthread_cond_broadcast( &readWaitCond );    // Tell them all, they can check for done
                pthread_mutex_unlock( &readWaitMutex );
            }
            break;

//        case readReg8I2C:
//            {
//                pthread_mutex_lock( &readWaitMutex );
//                int rdByte = wiringPiI2CReadReg8( i2cControl.i2cFile, i2cControl.i2cCommand );    // Read 8 bits from register reg on device
//                i2cControl.i2cData[2] = rdByte;  // Return result
//                i2cControl.i2cData[0] = 1;  // Signal completion
//                i2cControl.i2cCommand = 0;  // Signal completion - deprecated
//                pthread_cond_broadcast( &readWaitCond );    // Tell them all, they can check for done
//                pthread_mutex_unlock( &readWaitMutex );
//            }
//            break;
            
        case readReg8I2C:
            {
                pthread_mutex_lock( &readWaitMutex );
                write( i2cControl.i2cFile, &i2cControl.i2cCommand, 1 );
                read( i2cControl.i2cFile, &i2cControl.i2cData[2], 1 );
                i2cControl.i2cData[0] = 1;  // Signal completion
                i2cControl.i2cCommand = 0;  // Signal completion - deprecated
                pthread_cond_broadcast( &readWaitCond );    // Tell them all, they can check for done
                pthread_mutex_unlock( &readWaitMutex );
            }
            break;
            
        default:
            break;
    }
    
    syslog(LOG_NOTICE, "In Manager::execute, command type: %d, %d completed, 0x%08X returned", i2cControl.i2cType, i2cControl.i2cCommand, i2cControl.i2cParam );
}

// Typically a single write operation with command being a register and param being the value to write
void Manager::request( I2CType type, int file, int command, int param ) {
    
    I2CControl i2cControl = I2CControl::initControl( type, file, command, param );
    pthread_mutex_lock( &i2cQueueMutex );
    try {
        i2cQueue.push( i2cControl );
        syslog(LOG_NOTICE, "In Manager::request, i2c command %s put on queue", i2cControl.description() );
        pthread_cond_signal( &i2cQueueCond );
    } catch(...) {
        syslog(LOG_NOTICE, "In Manager::request, i2c queue push failure occured" );
    }
    pthread_mutex_unlock( &i2cQueueMutex );
}

// Typically a read where we are expected to wait for a long result
long Manager::request( I2CType type, int file, int command ) {
        
    char buffSpace[8] = {0};
    char *buffer = buffSpace;
    
    I2CControl i2cControl = I2CControl::initControl( type, file, command, buffer );
    pthread_mutex_lock( &i2cQueueMutex );
    try {
        i2cQueue.push( i2cControl );
        syslog(LOG_NOTICE, "In Manager::request, i2c command %s put on queue", i2cControl.description() );
        pthread_cond_signal( &i2cQueueCond );
    } catch(...) {
        syslog(LOG_NOTICE, "In Manager::request, i2c queue push failure occured" );
    }
    pthread_mutex_unlock( &i2cQueueMutex );

    pthread_mutex_lock( &readWaitMutex );
    while ( 0 == i2cControl.i2cData[0] ) {    // Until there is a response ready
        syslog(LOG_NOTICE, "In Manager::request, wait for readWaitCond", command );
        pthread_cond_wait( &readWaitCond, &readWaitMutex ); // Free mutex and wait
        syslog(LOG_NOTICE, "In Manager::request, got readWaitCond: %d - 0x%02X", i2cControl.i2cCommand, i2cControl.i2cData[0] );
    }
    pthread_mutex_unlock( &readWaitMutex );

    long result = (i2cControl.i2cData[2] << 24) | (i2cControl.i2cData[3] << 16) | (i2cControl.i2cData[4] << 8) | i2cControl.i2cData[5];
    syslog(LOG_NOTICE, "In Manager::request data read: %04X\n", result );

    return result;
}

// Typically a multiple write sequence
//long Manager::request( I2CControl writeList[] ) {
//        
//    pthread_mutex_lock( &i2cQueueMutex );
//    try {
//        for ( int i = 0; i < writeList->size(); i++ ) {
//            i2cQueue.push( writeList[i] );
//        }
//        syslog(LOG_NOTICE, "In Manager::request, i2c command put on queue" );
//        pthread_cond_signal( &i2cQueueCond );
//    } catch(...) {
//        syslog(LOG_NOTICE, "In Manager::request, i2c queue push failure occured" );
//    }
//    pthread_mutex_unlock( &i2cQueueMutex );
//}

long Manager::getNowMs() {
	
	struct timespec ts;
	clock_gettime( CLOCK_MONOTONIC, &ts );
	
	long ms = ts.tv_nsec / 1000000;
	long sec = ts.tv_sec * 1000;
	return sec + ms;
}

int Manager::readReg8( int file, int reg ) {
    
    long result = request( readReg8I2C, file, reg );

    syslog(LOG_NOTICE, "In Manager::readReg8 data read: %04X\n", result );

    return (int)result;
}

void Manager::setStatus() {
	
	syslog(LOG_NOTICE, "In Manager::setStatus()" );
    request( writeI2C, file_i2c, 's', 0 );
}

long Manager::getStatus() {
	
	syslog(LOG_NOTICE, "In Manager::getStatus()" );
	expectedControllerMode = statusMode;
    long status = request( readI2C, file_i2c, 4 );
    syslog(LOG_NOTICE, "In Manager::getStatus data read: 0x%08lX\n", status);

    return status;
}

void Manager::startVL() {
	
	syslog(LOG_NOTICE, "In Manager::startVL()" );
	if ( vl53l0x.isSetup ) {
		vl53l0x.measureRun();
	}
}

void Manager::stopVL() {
	
	syslog(LOG_NOTICE, "In Manager::stopVL()" );
	if ( vl53l0x.isSetup ) {
		vl53l0x.measureStop();
	}
}


// These routines need to manage the freshness of the range data
// They could compare the index to a copy of the timestamp when it was sent
void Manager::setRange( unsigned int angle) {

//	syslog(LOG_NOTICE, "In Manager::setRange( %u )", index );
	expectedControllerMode = rangeMode;
    request( writeI2C, file_i2c, 'p', angle );
}

long Manager::getRangeResult() {
	
   long status = request( readI2C, file_i2c, 4 );
    syslog(LOG_NOTICE, "In Manager::getStatus data read: 0x%08lX\n", status);

    expectedControllerMode = statusMode;    // Controller should drop back to this too
	sitMap.updateEntry( status );
	syslog(LOG_NOTICE, "In Manager::getRangeResult(): 0x%08lX", status );
	return status;
}

unsigned int Manager::getRange() {
	
	long result = getRangeResult();
	unsigned int range = (result >> 16) & 0x0FFFF;		// Actual range value
	return range;
}

void Manager::setMotorPower( bool On ) {

    syslog( LOG_NOTICE, "In Manager::setMotorPower(), test for access to manager from vl code" );
}
