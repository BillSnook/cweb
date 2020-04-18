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

#include "minion.hpp"
#include "vl53l0x.hpp"
#include "manager.hpp"

#include <pthread.h>


Minion	minion;

enum CheckTimes {	// milliSecond interval for various checks
	statusCheckInterval = 5000L,
	heartBeatInterval = 1000L
};

// MARK: SearchPattern
SearchPattern::SearchPattern() {

//	startAngle = 0;
//	endAngle = 180;
//	incrementAngle = 20;
//	indexCount = ( ( endAngle - startAngle ) / incrementAngle ) + 1;
	
	SearchPattern( 45, 135, 5 );
}

SearchPattern::SearchPattern( int start, int end, int inc ) {

	startAngle = start;
	endAngle = end;
	incrementAngle = inc;
	indexCount = ( ( endAngle - startAngle ) / incrementAngle ) + 1;
}

// MARK: SitMap
SitMap::SitMap() {
	
	pattern = SearchPattern();
}

SitMap::SitMap( SearchPattern newPattern ) {
	
	pattern = newPattern;
}

void SitMap::setupSitMap() {
	
	syslog(LOG_NOTICE, "In SitMap::setupSitMap(), before distanceMap, size: %d", pattern.indexCount );
	distanceMap = new DistanceEntry[pattern.indexCount];
	syslog(LOG_NOTICE, "In SitMap::setupSitMap(), after distanceMap" );
	for ( int i = 0; i < pattern.indexCount; i++ ) {
		distanceMap[ i ].angle = 0;
		distanceMap[ i ].range = 0;
	}
}

void SitMap::resetSitMap() {
	
	for ( int i = 0; i < pattern.indexCount; i++ ) {
		distanceMap[ i ].angle = 0;
		distanceMap[ i ].range = 0;
	}
}

void SitMap::shutdownSitMap() {
	
	delete distanceMap;
}

void SitMap::updateEntry( long entry ) {
	
	unsigned int range = (entry >> 16) & 0x0FFFF;		// Actual range value
	unsigned int angle = entry & 0x0FFFF;				// Angle used to track value of range
//	syslog(LOG_NOTICE, "In SitMap::updateEntry(), angle: %u, range: %u", angle, range );

	if ( ( angle <= pattern.endAngle ) && ( angle >= pattern.startAngle ) ) {
		unsigned int index = ( angle - pattern.startAngle ) / pattern.incrementAngle;
//		syslog(LOG_NOTICE, "In SitMap::updateEntry(), index: %u", index );
		if ( ( index >= 0 ) && ( index < pattern.indexCount ) ) {
			distanceMap[ index ].range = range;
			distanceMap[ index ].angle = angle;
		}
	}
}

char *SitMap::returnMap( char *buffer ) {

	sprintf( buffer, "@Map\n" );
	for ( int i = 0; i < pattern.indexCount; i++ ) {
		sprintf( buffer, "%s %4d  %5d\n", buffer, distanceMap[ i ].angle, distanceMap[ i ].range );
	}
	sprintf( buffer, "%s\n", buffer );	// Terminate string
//	syslog(LOG_NOTICE, "In SitMap::returnMap()\n%s", buffer );
	return buffer;
}

unsigned char *SitMap::returnMapData( unsigned char *buffer ) {	// buffer is 1024 bytes

//	for ( int i = 0; i < pattern.indexCount; i++ ) {
//		sprintf( buffer, "%s %4d  %4d\n", buffer, distanceMap[ i ].angle, distanceMap[ i ].range );
//	}
//	sprintf( buffer, "%s\n", buffer );
	memcpy( buffer, &pattern, sizeof( pattern ) );
	memcpy( buffer + sizeof( pattern ), distanceMap, sizeof( distanceMap ) * pattern.indexCount );
	return buffer;
}

// MARK: - I2C Queue

I2CControl I2CControl::initControl( I2CType type, int file, int command, int param ) {
    I2CControl newI2CControl = I2CControl();
    newI2CControl.i2cType = type;
    newI2CControl.i2cFile = file;
    newI2CControl.i2cCommand = command;
    newI2CControl.i2cParam = param;
    return newI2CControl;
}

I2CControl I2CControl::initControl( I2CType type, int file, int command, char *buffer ) {
    I2CControl newI2CControl = I2CControl();
    newI2CControl.i2cType = type;
    newI2CControl.i2cFile = file;
    newI2CControl.i2cCommand = command;
    newI2CControl.i2cData = buffer;
    return newI2CControl;
}

bool I2CControl::isComplete() {
    return 0 == i2cCommand;
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
	
	minion = Minion();					// Minions talk to the arduino to relay commands
	minion.setupMinion( ArdI2CAddr );
    file_i2c = minion.file_i2c;
	
	pattern = SearchPattern( 45, 135, 5 );
	sitMap = SitMap( pattern );
	sitMap.setupSitMap();

    pthread_mutex_init( &i2cQueueMutex, NULL );
    pthread_cond_init( &i2cQueueCond, NULL );

    pthread_mutex_init( &readWaitMutex, NULL );
    pthread_cond_init( &readWaitCond, NULL );

}

void Manager::resetPattern( int start, int end, int inc ) {

	sitMap.shutdownSitMap();
	pattern = SearchPattern( start, end, inc );
	sitMap = SitMap( pattern );
	sitMap.setupSitMap();
}

void Manager::shutdownManager() {

    stopLoop = true;
    if ( vl53l0x.isSetup ) {
		vl53l0x.shutdownVL53L0X();
	}
	minion.shutdownMinion();
	sitMap.shutdownSitMap();

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
                write( file_i2c, buffer, 2 );
            }
            break;

        case readI2C:
            {
                pthread_mutex_lock( &readWaitMutex );
                read( file_i2c, i2cControl.i2cData, i2cControl.i2cCommand );
                syslog(LOG_NOTICE, "In Manager::execute, data read: %02X %02X %02X %02X    0x%04X\n", i2cControl.i2cData[0], i2cControl.i2cData[1], i2cControl.i2cData[2], i2cControl.i2cData[3], i2cControl.i2cCommand);
                i2cControl.i2cCommand = 0;  // Signal completion
                pthread_cond_broadcast( &readWaitCond );    // Tell them all, they can check for done
                pthread_mutex_unlock( &readWaitMutex );
            }
            break;

        case readReg8I2C:
            {
                int rdByte = wiringPiI2CReadReg8( file_i2c, i2cControl.i2cCommand );    // Read 8 bits from register reg on device

                pthread_mutex_lock( &readWaitMutex );
                i2cControl.i2cCommand = 0;  // Signal completion
                i2cControl.i2cParam = rdByte;  // Return result
                pthread_cond_broadcast( &readWaitCond );    // Tell them all, they can check for done
                pthread_mutex_unlock( &readWaitMutex );
            }
            break;
            
        default:
            break;
    }
    
    syslog(LOG_NOTICE, "In Manager::execute, command type: %d, %d completed, %d returned", i2cControl.i2cType, i2cControl.i2cCommand, i2cControl.i2cParam );
}

void Manager::request( I2CControl i2cControl ) {
    
    pthread_mutex_lock( &i2cQueueMutex );
    try {
        i2cQueue.push( i2cControl );
        syslog(LOG_NOTICE, "In Manager::request, i2c command put on queue" );
        pthread_cond_signal( &i2cQueueCond );
    } catch(...) {
        syslog(LOG_NOTICE, "In Manager::request, i2c queue push failure occured" );
    }
    pthread_mutex_unlock( &i2cQueueMutex );


}

long Manager::getNowMs() {
	
	struct timespec ts;
	clock_gettime( CLOCK_MONOTONIC, &ts );
	
	long ms = ts.tv_nsec / 1000000;
	long sec = ts.tv_sec * 1000;
	return sec + ms;
}

void Manager::setStatus() {
	
	syslog(LOG_NOTICE, "In Manager::setStatus()" );
//    minion.setStatus();
    
    I2CControl i2cControl = I2CControl::initControl( writeI2C, file_i2c, 's', 0 );
    request( i2cControl );
}

long Manager::getStatus() {
	
	syslog(LOG_NOTICE, "In Manager::getStatus()" );
	expectedControllerMode = statusMode;
//	return minion.getStatus();
    
    char buffSpace[8] = {0};
    char *buffer = buffSpace;

    I2CControl i2cControl = I2CControl::initControl( readI2C, file_i2c, 4, buffer );
    pthread_mutex_lock( &readWaitMutex );
    request( i2cControl );
    int completed = i2cControl.isComplete();
    while ( !completed ) {    // Until there is a response
        syslog(LOG_NOTICE, "In Manager::getStatus(), wait for readWaitCond" );
        pthread_cond_wait( &readWaitCond, &readWaitMutex ); // Free mutex and wait
        completed = i2cControl.isComplete();
        syslog(LOG_NOTICE, "In Manager::getStatus(), got readWaitCond: %d - 0x%02X", completed, i2cControl.i2cData[0] );
    }
    pthread_mutex_unlock( &readWaitMutex );

    long status = (buffSpace[0] << 24) | (buffSpace[1] << 16) | (buffSpace[2] << 8) | buffSpace[3];
    syslog(LOG_NOTICE, "In Manager::getStatus data read: %02X %02X %02X %02X    0x%08lX\n", buffSpace[0], buffSpace[1], buffSpace[2], buffSpace[3], status);

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
//	minion.setRange( angle );
    
    I2CControl i2cControl = I2CControl::initControl( writeI2C, file_i2c, 'p', angle );
    request( i2cControl );
}

long Manager::getRangeResult() {
	
	long result = minion.getRange();	// This will wait for a response to an I2C read
	expectedControllerMode = statusMode;
	sitMap.updateEntry( result );
	syslog(LOG_NOTICE, "In Manager::getRangeResult(): 0x%08lX", result );
	return result;
}

unsigned int Manager::getRange() {
	
	long result = getRangeResult();
	unsigned int range = (result >> 16) & 0x0FFFF;		// Actual range value
	return range;
}

void Manager::setMotorPower( bool On ) {
//	minion.setRelay( 0, On );
}
