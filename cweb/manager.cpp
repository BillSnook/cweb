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

extern Minion	minion;

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
}

void Manager::shutdownManager() {
	
	syslog(LOG_NOTICE, "In shutdownManager" );
}

void Manager::monitor( int mode ) {
	
	syslog(LOG_NOTICE, "In Manager::monitor, should only run once" );
	long now = 0;
	while ( !stopLoop ) {
		
//		now = getNowMs();
//		// Monitor microcontroller
//		if ( now > lastAnythingTime + heartBeatInterval ) {
//			lastAnythingTime = now;
//
//		}

		now = getNowMs();
//		syslog(LOG_NOTICE, "In Manager::monitor, now is: %ld", now );
		// Monitor microcontroller
		if ( now > lastStatusTime + statusCheckInterval ) {
			lastStatusTime = now;
			lastAnythingTime = now;
			getStatus();
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

void Manager::getStatus() {

	minion.putI2CCmd( 'c' );
	usleep( 1000 );
	long result = minion.getI2CCmd();
	syslog(LOG_NOTICE, "In Manager::getStatus, got: %ld - 0x%08lX", result, result );
}
