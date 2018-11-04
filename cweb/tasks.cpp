//
//  tasks.cpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "tasks.hpp"
#include "hardware.hpp"


//extern Commander	commander;
extern Hardware		hardware;

void TaskMaster::setupTaskMaster() {
	
	syslog(LOG_NOTICE, "In setupTaskMaster" );
}

void TaskMaster::serviceTaskMaster( int task, int param ) {	// Main command determination routine

	syslog(LOG_NOTICE, "In serviceTaskMaster with: %d, param: %d", task, param );

	switch ( task ) {
		case stopTask:
			killTasks();
			break;
			
		case testTask1:
			taskTest1();
			break;
		case testTask2:
			taskTest2();
			break;
		case scanTask:
			taskScan( param );
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

void TaskMaster::taskScan() {
	
	stopLoop = false;
	hardware.
	if ( stopLoop ) {
		
		return;
	}
	
}
