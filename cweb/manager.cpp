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
#include "hardware.hpp"


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
	while ( !stopLoop ) {
		
		long now = getNowMs();
//		// Monitor microcontroller
//		if ( now > lastAnythingTime + heartBeatInterval ) {
//			lastAnythingTime = now;
//
//		}
//		
//		now = getNowMs();
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
	
	return ts.tv_nsec / 1000;
}

void Manager::getStatus() {
	syslog(LOG_NOTICE, "In Manager::getStatus" );

}
