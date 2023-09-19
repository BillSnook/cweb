//
//  tasks.hpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef tasks_hpp
#define tasks_hpp

#ifdef ON_PI

#include "ArducamDepthCamera.h"

#endif  // ON_PI

#include <stdlib.h>
#include <stdio.h>

enum TaskType {
    stopTask = 0,
    cameraTest,      // 1    // Simple quick camera validation
    testTask2,       // 2
    scanTask,        // 3    // Just scan
    pingTask,        // 4    // Just ping
    scanpingTask,    // 5    // Ping and scan
    huntTask,        // 6
    testTaskCount    // 7, size of TaskType enum
};

class TaskMaster {
	
	bool    stopLoop;
    bool    cameraRunning;
    int     taskCount;

    ArducamDepthCamera tof;
    ArducamFrameBuffer frame;

public:
    
	void setupTaskMaster();
	void shutdownTaskMaster();
	
	void serviceTask( int task, int socket );

	void killTasks();
	void cameraStreamTest(int socketOrAddr);
	void taskTest2();
	void taskScan();
	void taskPing();
	void taskScanPing();
	void taskHunt();

    int  startCamera();
    int  getCameraData(int socketOrAddr);
    int  stopCamera();
};

void getPreview(uint8_t *, float *, float *);
void runCamera();

#endif /* tasks_hpp */
