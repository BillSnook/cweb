//
//  tasks.hpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef tasks_hpp
#define tasks_hpp


class TaskMaster {
	
	bool    stopLoop;
    int     taskCount;
    
public:
    
	void setupTaskMaster();
	void shutdownTaskMaster();
	
	void serviceTask( char *commandString, int socket );

	void killTasks();
	void taskTest1();
	void taskTest2();
	void taskScan();
	void taskPing();
	void taskScanPing();
	void taskHunt();
};

#endif /* tasks_hpp */
