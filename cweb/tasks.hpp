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
#include <stdlib.h>
#include <stdio.h>

#endif  // ON_PI


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
    int     taskCount;
    
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

    int  getCameraData();
};

void getPreview(uint8_t *, float *, float *);
void runCamera();

#endif /* tasks_hpp */
