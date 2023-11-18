//
//  tasks.cpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "tasks.hpp"
//#include "hardware.hpp"
#include "threader.hpp"
#include "listen.hpp"

#ifdef ON_PI
#include <pigpio.h>
#endif  // ON_PI

// The purpose of this class is to allow a task to run independently
// in a separate thread and allow the rest of the program to run normally.
// The tof camera in particular is here now. Soon it should get it's own class.


//extern Commander	commander;
//extern Hardware		hardware;   // Needed here?
extern Threader     threader;
extern Listener     listener;

void TaskMaster::setupTaskMaster() {
	
	syslog(LOG_NOTICE, "In setupTaskMaster" );
	stopLoop = false;

    // So we can use our own signal handler:
    taskCount = 0;
}

void TaskMaster::shutdownTaskMaster() {
	

	syslog(LOG_NOTICE, "In shutdownTaskMaster" );
	killTasks();
    usleep( 100000 );   // 1/10 second
}

// MARK: Tasks section

// This runs this task in a separate thread - it should be a high priority one
void TaskMaster::serviceTask( int task, int socketOrAddr ) {	// Main command determination routine

    taskCount += 1;
	syslog(LOG_NOTICE, "In serviceTask with task %i, task count: %d", task, taskCount );
	switch ( task ) {
		case stopTask:
			killTasks();
			break;
		case cameraTest:
//			cameraStreamTest(socketOrAddr);
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
    taskCount -= 1;
}

void TaskMaster::killTasks() {
	
//	if ( !stopLoop ) {
//		hardware.scanStop();
//	}
	stopLoop = true;
}

/*
~ 0.8-0.9 meter
 mtrctllog[1076]: Center distance: 0.82, amplitude: 58.24, preview_ptr: 96
 mtrctllog[1076]: Center distance: 0.70, amplitude: 53.67, preview_ptr: A4
 mtrctllog[1076]: Center distance: 0.74, amplitude: 60.93, preview_ptr: A0
 mtrctllog[1076]: Center distance: 0.74, amplitude: 60.93, preview_ptr: A0
 mtrctllog[1076]: Center distance: 0.84, amplitude: 65.97, preview_ptr: 92


 ~10-11 cm
 mtrctllog[1088]: Center distance: 0.12, amplitude: 9037.04, preview_ptr: EE
 mtrctllog[1088]: Center distance: 0.13, amplitude: 9031.54, preview_ptr: EC
 mtrctllog[1088]: Center distance: 0.13, amplitude: 9008.93, preview_ptr: EE
 mtrctllog[1088]: Center distance: 0.13, amplitude: 8992.43, preview_ptr: EC
 mtrctllog[1088]: Center distance: 0.13, amplitude: 9008.80, preview_ptr: EE


 mtrctllog[1088]: In datagram serviceConnection received 1 bytes of data from clientAddr: 192.168.1.2, port 51576
 mtrctllog[1088]: Keep-alive enabled
 mtrctllog[1088]:   Run next thread type commandThread, count: 3
 mtrctllog[1088]:     socket: 1, addr: 0, command: o
 mtrctllog[1088]: Test camera streaming
 mtrctllog[1088]: In tasks, in startCamera
 mtrctllog[1088]: In tasks, in getCameraData started
 mtrctllog[1088]: Center distance: 0.12, amplitude: 9037.04, preview_ptr: EE
 mtrctllog[1088]: Center distance: 0.13, amplitude: 9031.54, preview_ptr: EC
 mtrctllog[1088]: Center distance: 0.13, amplitude: 9008.93, preview_ptr: EE
 mtrctllog[1088]: Center distance: 0.13, amplitude: 8992.43, preview_ptr: EC
 mtrctllog[1088]: Center distance: 0.13, amplitude: 9008.80, preview_ptr: EE
 mtrctllog[1088]: Clean exit from getCameraData routine
 mtrctllog[1088]: In cameraStreamTest, data = 0.13
 mtrctllog[1088]: arducamCameraStop failed
 mtrctllog[1088]: Run next thread exit commandThread, thread count: 2
 mtrctllog[1088]: In datagram serviceConnection received 1 bytes of data from clientAddr: 192.168.1.2, port 51576
 mtrctllog[1088]:   Run next thread type commandThread, count: 3
 mtrctllog[1088]:     socket: 1, addr: 0, command: o
 mtrctllog[1088]: Test camera streaming
 mtrctllog[1088]: In tasks, in startCamera
 setVideoMode ***WARNING*** TOFCamera: Could not set current frame format
 mtrctllog[1088]: arducamCameraStart failed
 getFrame ***WARNING*** Failed to dequeue buffer
 mtrctllog[1088]: In cameraStreamTest, failed to start camera
 mtrctllog[1088]: Run next thread exit commandThread, thread count: 2
 getFrame ***WARNING*** Failed to dequeue buffer
 getFrame ***WARNING*** Failed to dequeue buffer
 getFrame ***WARNING*** Failed to dequeue buffer

 */

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
//	hardware.scanTest();
}

void TaskMaster::taskPing() {
	
	syslog(LOG_NOTICE, "In taskPing" );
	stopLoop = false;
//	hardware.ping( 0 );
}

void TaskMaster::taskScanPing() {
	
	syslog(LOG_NOTICE, "In taskScanPing" );
	stopLoop = false;
//	hardware.scanPing( 0 );
}

void TaskMaster::taskHunt() {

    syslog(LOG_NOTICE, "In taskHunt" );
}
