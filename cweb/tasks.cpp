//
//  tasks.cpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "tasks.hpp"
#include "hardware.hpp"
#include "threader.hpp"


//extern Commander	commander;
extern Hardware		hardware;
extern Threader		threader;

void TaskMaster::setupTaskMaster() {
	
	syslog(LOG_NOTICE, "In setupTaskMaster" );
	stopLoop = false;
}

void TaskMaster::shutdownTaskMaster() {
	
	syslog(LOG_NOTICE, "In shutdownTaskMaster" );
	killTasks();
	usleep( 200000 );
}

// MARK: Tasks section
// TODO: move to tasks class
void TaskMaster::mobileTask( int taskNumber, int param ) {
	
	threader.queueThread( taskThread, taskNumber, (uint)param );
}

// This runs in a seperate thread
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
			taskScan();
			break;
		case pingTask:
			taskPing();
			break;

		default:
			killTasks();
			break;
	}
}

void TaskMaster::killTasks() {
	
	if ( !stopLoop ) {
		hardware.scanStop();
	}
	stopLoop = true;
}

void TaskMaster::taskTest1() {	// Print out messages so we know this task is running
	
	stopLoop = false;
	for ( int i = 0; i < 10; i++ ) {
		syslog(LOG_NOTICE, "In taskTest1, i = %d", i );
		sleep( 1 );
		if ( stopLoop ) {
			return;
		}
	}
	
}

void TaskMaster::taskTest2() {	// Print out messages so we know this task is running
	
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
	hardware.scanTest();
}

void TaskMaster::taskPing() {
	
	syslog(LOG_NOTICE, "In taskPing" );
	stopLoop = false;
	hardware.ping();
}

void TaskMaster::taskScanPing() {
	
	syslog(LOG_NOTICE, "In taskScanPing" );
	stopLoop = false;
	hardware.scanPing();
}
