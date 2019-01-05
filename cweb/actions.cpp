//
//  actions.cpp
//  cweb
//
//  Created by William Snook on 12/8/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "actions.hpp"
#include "hardware.hpp"

extern Hardware		hardware;

Actor				actor;

void Actor::setupActor() {
	
	syslog(LOG_NOTICE, "In setupActor" );
}

void Actor::shutdownActor() {
	
	syslog(LOG_NOTICE, "In shutdownActor" );
}

void Actor::stop() {
	
	syslog(LOG_NOTICE, "In stop" );
	
	hardware.allStop();
}

void Actor::runHunt() {
	
	syslog(LOG_NOTICE, "In runHunt" );
	
	//	hardware.scanPing();
	
	hardware.scanUntilFound( WideScan );
	hardware.turnAndFollow( MediumDistance );
}
