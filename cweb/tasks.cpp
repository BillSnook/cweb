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

#include <pigpio.h>

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
    cameraRunning = false;

    // So we can use our own signal handler:
    int cfg = gpioCfgGetInternals();
    cfg |= PI_CFG_NOSIGHANDLER;  // (1<<10)
    gpioCfgSetInternals(cfg);
    int status = gpioInitialise();

    tof = createArducamDepthCamera();

    if ( startCamera() != 0 ) {
        syslog(LOG_NOTICE, "In setupTaskMaster, failed to start camera, continuing" );
    }
    taskCount = 0;
}

void TaskMaster::shutdownTaskMaster() {
	

	syslog(LOG_NOTICE, "In shutdownTaskMaster" );
	killTasks();
    usleep( 100000 );   // 1/10 second
    stopCamera();     // Causing seg fault!
    gpioTerminate();
//	usleep( 100000 );
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
	
    if ( !cameraRunning ) {
        syslog(LOG_NOTICE, "In cameraStreamTest, camera has not been started" );
        return;
    }
    char msg[64];
    float x = getCameraData(socketOrAddr);
    snprintf(msg, 64, "T In cameraStreamTest, data = %.2f", x);
    listener.writeBack(msg, socketOrAddr);
    syslog(LOG_NOTICE, "In cameraStreamTest, data = %.2f", x );
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

// MARK: Camera stuff

int TaskMaster::startCamera() {

    syslog(LOG_NOTICE, "In tasks, in startCamera" );
    if ( arducamCameraOpen( tof, CSI, 0 ) ) {
        syslog(LOG_NOTICE, "arducamCameraOpen failed");
        return -2;
    }
    if ( arducamCameraStart( tof, DEPTH_FRAME ) ) {
        syslog(LOG_NOTICE, "arducamCameraStart failed");
        return -3;
    }

    cameraRunning = true;
    return 0;
}

float TaskMaster::getCameraData(int socketOrAddr) {
    struct timeval tvNow;

    if (!cameraRunning) {
        syslog(LOG_NOTICE, "In tasks, in getCameraData with camera not running" );
        return 0;
    }
    gettimeofday( &tvNow, NULL );
    syslog(LOG_NOTICE, "In tasks, in getCameraData started, time: %i", tvNow.tv_usec );

    ArducamFrameBuffer frame;

    float *depth_ptr = 0;
//    float *amplitude_ptr = 0;
//    uint8_t *preview_ptr = (uint8_t *)malloc( 180 * 240 * sizeof(uint8_t) ) ;

    // Is this needed - apparently yes, preps format
    ArducamFrameFormat format;
    if ( ( frame = arducamCameraRequestFrame( tof, 200 ) ) != 0x00 ) {
        format = arducamCameraGetFormat( frame, DEPTH_FRAME );
        arducamCameraReleaseFrame( tof, frame );
    }

    float savedDepth = 0.0;
    for ( int i = 0; i < 10; i++ ) {
        if ( ( frame = arducamCameraRequestFrame( tof, 200 ) ) != 0x00 ) {
            depth_ptr = (float*)arducamCameraGetDepthData( frame );
//            amplitude_ptr = (float*)arducamCameraGetAmplitudeData( frame );
//            getPreview( preview_ptr, depth_ptr, amplitude_ptr );
            gettimeofday( &tvNow, NULL );
            savedDepth = depth_ptr[21720];
            syslog(LOG_NOTICE, "Center distance: %.2f, time: %i\n", depth_ptr[21720], tvNow.tv_usec);
//            listener.writeBack((char *)preview_ptr, socketOrAddr);
            arducamCameraReleaseFrame( tof, frame );
            usleep(1000000);
        }
    }
//    free(preview_ptr);

    gettimeofday( &tvNow, NULL );
    syslog(LOG_NOTICE, "Clean exit from getCameraData routine, time: %i", tvNow.tv_usec );
    return savedDepth;
}

int TaskMaster::stopCamera() {

    if (!cameraRunning) {
        syslog(LOG_NOTICE, "In tasks, in stopCamera but cameraRunning is already false" );
        return -2;
    }
    cameraRunning = false;
    if ( arducamCameraStop( tof ) ) {
        syslog(LOG_NOTICE, "arducamCameraStop failed");
        return -1;
    }
//    if ( arducamCameraClose( &tof ) ) {
//        syslog(LOG_NOTICE, "arducamCameraClose failed");
//        return -1;
//    }
    return 0;
}

int TaskMaster::cameraDataSend(int socketOrAddr) {
    struct timeval tvNow;

    if (!cameraRunning) {
        syslog(LOG_NOTICE, "In tasks, in cameraDataSend with camera not started" );
        return 0;
    }
    gettimeofday( &tvNow, NULL );
    syslog(LOG_NOTICE, "In tasks, in cameraDataSend started, time: %i", tvNow.tv_usec );

    // 240 x 180 = 43200, half is 21600
    ArducamFrameBuffer frame;
    float *depth_ptr = 0;
    float *depth_line = 0;
    uint8_t preview_data[242];
    uint8_t *preview_ptr;   //  = &preview_data[2];
    preview_data[0] = 0x43; // "C"
    preview_data[1] = 0x30; // "0"

    ArducamFrameFormat format;
    if ( ( frame = arducamCameraRequestFrame( tof, 200 ) ) != 0x00 ) {
        format = arducamCameraGetFormat( frame, DEPTH_FRAME );
        arducamCameraReleaseFrame( tof, frame );
    }

    if ( ( frame = arducamCameraRequestFrame( tof, 200 ) ) != 0x00 ) {
        depth_ptr = (float*)arducamCameraGetDepthData( frame );
        depth_line = &depth_ptr[21600];
        for (unsigned long int i = 0; i < 240; i++) {
            float phase = (*(depth_line + i) / 2) * 255;
            uint8_t depth = phase > 255 ? 255 : phase;
            *(preview_ptr + 2 + i) = depth;
        }
        listener.writeBackCount((char *)preview_ptr, 242, socketOrAddr);
        arducamCameraReleaseFrame( tof, frame );
    }

    gettimeofday( &tvNow, NULL );
    syslog(LOG_NOTICE, "Clean exit from cameraDataSend routine, time: %i", tvNow.tv_usec );

    return 0;
}


//#ifdef ON_PI
//
//void getPreview(uint8_t *preview_ptr, float *phase_image_ptr, float *amplitude_image_ptr) {
//    unsigned long int len = 240 * 180;
//    for (unsigned long int i = 0; i < len; i++) {
//        uint8_t amplitude = *(amplitude_image_ptr + i) > 30 ? 254 : 0;
//        float phase = ((1 - (*(phase_image_ptr + i) / 2)) * 255);
//        uint8_t depth = phase > 255 ? 255 : phase;
//        *(preview_ptr + i) = depth & amplitude;
//    }
//}
//
//#endif  // ON_PI
