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
//#include "actions.hpp"


// The purpose of this class is to allow a task to run independently
// in a separate thread and allow the rest of the program to run normally.

enum TaskType {
	stopTask = 0,
	testTask1,		// 1
	testTask2,		// 2
	scanTask,		// 3	// Just scan
	pingTask,		// 4	// Just ping
	scanpingTask,	// 5	// Ping and scan
	huntTask,		// 6
	testTaskCount	// 7, size of TaskType enum
};


//extern Commander	commander;
extern Hardware		hardware;
extern Threader		threader;
//extern Actor		actor;

void TaskMaster::setupTaskMaster() {
	
	syslog(LOG_NOTICE, "In setupTaskMaster" );
//	actor.setupActor();
	stopLoop = false;
}

void TaskMaster::shutdownTaskMaster() {
	
	syslog(LOG_NOTICE, "In shutdownTaskMaster" );
//	actor.shutdownActor();
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
		case scanpingTask:
			taskScanPing();
			break;
		case huntTask:
			taskHunt();
			break;

		default:
			killTasks();
			break;
	}
}

void TaskMaster::killTasks() {
	
//	if ( !stopLoop ) {
//		hardware.scanStop();
////		actor.stop();	// Duplicates scanStop
//	}
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
	hardware.ping( 0 );
}

void TaskMaster::taskScanPing() {
	
	syslog(LOG_NOTICE, "In taskScanPing" );
	stopLoop = false;
	hardware.scanPing( 0 );
}

void TaskMaster::taskHunt() {
	
	syslog(LOG_NOTICE, "In taskHunt" );
	
//	actor.runHunt();
}
