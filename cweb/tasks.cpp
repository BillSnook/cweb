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
#include "listen.hpp"


// The purpose of this class is to allow a task to run independently
// in a separate thread and allow the rest of the program to run normally.


//extern Commander	commander;
extern Hardware		hardware;
extern Threader     threader;
extern Listener     listener;

void TaskMaster::setupTaskMaster() {
	
	syslog(LOG_NOTICE, "In setupTaskMaster" );
	stopLoop = false;
    taskCount = 0;
}

void TaskMaster::shutdownTaskMaster() {
	
	syslog(LOG_NOTICE, "In shutdownTaskMaster" );
	killTasks();
	usleep( 200000 );
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
			cameraStreamTest(socketOrAddr);
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

void TaskMaster::cameraStreamTest(int socketOrAddr) {	// Print out messages so we know this task is running
	
    char msg[64];
	stopLoop = false;
	for ( int i = 0; i < 10; i++ ) {
        int x = getCameraData();
        snprintf(msg, 64, "T In cameraStreamTest, i = %d, data = %i", i, x);
        listener.writeBack(msg, socketOrAddr);
		syslog(LOG_NOTICE, "In cameraStreamTest, i = %d", i );
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
}

int TaskMaster::getCameraData() {

    syslog(LOG_NOTICE, "In tasks, in getCameraData" );
    runCamera();
    return 3;
}

#ifdef ON_PI

void getPreview(uint8_t *preview_ptr, float *phase_image_ptr, float *amplitude_image_ptr) {
    unsigned long int len = 240 * 180;
    for (unsigned long int i = 0; i < len; i++)
    {
        uint8_t amplitude = *(amplitude_image_ptr + i) > 30 ? 254 : 0;
        float phase = ((1 - (*(phase_image_ptr + i) / 2)) * 255);
        uint8_t depth = phase > 255 ? 255 : phase;
        *(preview_ptr + i) = depth & amplitude;
    }
}

void runCamera() {
    struct timeval tvNow;

    syslog(LOG_NOTICE, "At start for runCamera test routine");
    ArducamDepthCamera tof = createArducamDepthCamera();
    ArducamFrameBuffer frame;
    if (arducamCameraOpen(tof,CSI,0)) {
        syslog(LOG_NOTICE, "arducamCameraOpen failed");
        return;
    }
    if (arducamCameraStart(tof,DEPTH_FRAME)) {
        syslog(LOG_NOTICE, "arducamCameraStart failed");
        return;
    }
    uint8_t *preview_ptr = malloc(180*240*sizeof(uint8_t)) ;
    float* depth_ptr = 0;
    int16_t *raw_ptr = 0;
    float *amplitude_ptr = 0;
    ArducamFrameFormat format;
    if ((frame = arducamCameraRequestFrame(tof,200)) != 0x00){
        format = arducamCameraGetFormat(frame,DEPTH_FRAME);
        arducamCameraReleaseFrame(tof,frame);
    }
    for (i = 0; i < 200; i++)
    {
        if ((frame = arducamCameraRequestFrame(tof,200)) != 0x00)
        {
            depth_ptr = (float*)arducamCameraGetDepthData(frame);
            gettimeofday(&tvNow, NULL);
            syslog(LOG_NOTICE, "Center distance:%.2f    time: %i\n",depth_ptr[21600], tvNow.tv_usec);
            amplitude_ptr = (float*)arducamCameraGetAmplitudeData(frame);
            getPreview(preview_ptr,depth_ptr,amplitude_ptr);
            arducamCameraReleaseFrame(tof,frame);
        }
    }

    free(preview_ptr);
    if (arducamCameraStop(tof)) {
        syslog(LOG_NOTICE, "arducamCameraStop failed");
        return;
    }
    if (arducamCameraClose(tof)) {
        syslog(LOG_NOTICE, "arducamCameraClose failed");
        return;
    }
    psyslog(LOG_NOTICE, "Clean exit from runCamera test routine");
    return;

}
#endif  // ON_PI
