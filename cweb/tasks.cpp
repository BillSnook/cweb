//
//  tasks.cpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "tasks.hpp"

//#include "listen.hpp"
//#include "threader.hpp"
#include "hardware.hpp"
//
//#include <stdlib.h>
//#include <stdio.h>
//#include <syslog.h>
//#include <string.h>

extern Hardware	hardware;

void TaskMaster::setupTaskMaster() {
	
	syslog(LOG_NOTICE, "In setupTaskMaster" );
}

void TaskMaster::serviceTaskMaster( int command, int socket ) {	// Main command determination routine

	syslog(LOG_NOTICE, "In serviceTaskMaster with: %d, param: %d", command, socket );
	
	switch ( command ) {
		case 0:
			killTasks();
			break;
			
		case 1:
			taskTest1();
			break;
		case 2:
			taskTest2();
			break;
			
		default:
			killTasks();
			break;
	}
}

void TaskMaster::killTasks() {
	
	stopLoop = true;
}

void TaskMaster::taskTest1() {
	
	stopLoop = false;
	for ( int i = 0; i < 10; i++ ) {
		syslog(LOG_NOTICE, "In taskTest1, i = %d", i );
		sleep( 1 );
		if ( stopLoop ) {
			return;
		}
	}
	
}

void TaskMaster::taskTest2() {
	
	stopLoop = false;
	for ( int i = 0; i < 10; i++ ) {
		syslog(LOG_NOTICE, "In taskTest2, i = %d", i );
		sleep( 1 );
		if ( stopLoop ) {
			return;
		}
	}
	
}
