//
//  manager.cpp
//  cweb
//
//  Created by William Snook on 1/12/19.
//  Copyright © 2019 billsnook. All rights reserved.
//

#include <time.h>
#include <syslog.h>

#include "manager.hpp"
#include "minion.hpp"
#include "hardware.hpp"

Minion	minion;

enum CheckTimes {	// milliSecond interval for various checks
	statusCheckInterval = 5000L,
	heartBeatInterval = 1000L
};


void Manager::setupManager() {
	stopLoop = false;
	lastAnythingTime = 0;
	lastStatusTime = 0;
	lastHeartbeatTime = 0;
	syslog(LOG_NOTICE, "In setupManager" );

	minion = Minion();
	minion.setupMinion( ArdI2CAddr );
}

void Manager::shutdownManager() {
	
	minion.resetMinion();
	syslog(LOG_NOTICE, "In shutdownManager" );
}

void Manager::monitor( int mode ) {
	
	syslog(LOG_NOTICE, "In Manager::monitor, should only run once" );
	setStatus();				// Set mode so reads get status
	long now = getNowMs();
	lastStatusTime = now;
	while ( !stopLoop ) {
		
		now = getNowMs();
		// Monitor microcontroller
		if ( now > lastStatusTime + statusCheckInterval ) {
			lastStatusTime = now;
			lastAnythingTime = now;
//			getStatus();
		}
		

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
	
	minion.setStatus();
}

long Manager::getStatus() {
	
	return minion.getStatus();
}


// Test
int Manager::testRead() {
	
	int got = 0;
#ifdef ON_PI
	unsigned char buffSpace[20] = {0};
	unsigned char *buffer = buffSpace;
	minion.getI2CData( buffer );
	got = strlen( (const char *)buffer );
	syslog(LOG_NOTICE, "Read 0x%X from I2C device", got);
#endif // ON_PI
	
	return got;
}

void Manager::testWrite(unsigned char *data) {
	
	minion.putI2CData(data);
	
}

