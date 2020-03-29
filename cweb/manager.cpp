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

// MARK: Manager
void Manager::setupManager() {
	stopLoop = false;
	busy = false;
	lastAnythingTime = 0;
	lastStatusTime = 0;
	expectedControllerMode = initialMode;
	syslog(LOG_NOTICE, "In setupManager" );

//	vl53l0x = VL53L0X();				// VL53L0xes talk to the array of light-rangers
//	vl53l0x.setupVL53L0X( 0x29 );
	
	minion = Minion();					// Minions talk to the arduino to relay commands
	minion.setupMinion( ArdI2CAddr );
	
	pattern = SearchPattern( 45, 135, 5 );
	sitMap = SitMap( pattern );
	sitMap.setupSitMap();
}

void Manager::resetPattern( int start, int end, int inc ) {

	sitMap.shutdownSitMap();
	pattern = SearchPattern( start, end, inc );
	sitMap = SitMap( pattern );
	sitMap.setupSitMap();
}

void Manager::shutdownManager() {

	if ( vl53l0x.isSetup ) {
		vl53l0x.shutdownVL53L0X();
	}
	minion.shutdownMinion();
	sitMap.shutdownSitMap();
	syslog(LOG_NOTICE, "In shutdownManager" );
}

void Manager::monitor() {
	
	syslog(LOG_NOTICE, "In Manager::monitor, should only start once" );
	setStatus();				// Set mode so reads get status
	long now = getNowMs();
	lastAnythingTime = now;
	while ( !stopLoop ) {
		
		now = getNowMs();
		// Monitor microcontroller
		if ( now > lastStatusTime + statusCheckInterval ) {
			long tempStatus = 0; // getStatus();	// return 0 if busy, else ask minion to read status
			if ( tempStatus ) {
				lastStatusTime = now;
				lastAnythingTime = now;
				status = tempStatus;
			}
		}
		sleep( 1 );

//		lastAnythingTime = getNowMs();
	}
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
	minion.setStatus();
}

long Manager::getStatus() {
	
	syslog(LOG_NOTICE, "In Manager::getStatus()" );
	if ( busy ) {
        syslog(LOG_NOTICE, "Busy" );
        busy = false;
		return 0;
	}
	expectedControllerMode = statusMode;
	return minion.getStatus();
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
	minion.setRange( angle );
}

long Manager::getRangeResult() {
	
	busy = true;
	long result = minion.getRange();	// This will wait for a response to an I2C read
	expectedControllerMode = statusMode;
	busy = false;
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
