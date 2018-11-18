//
//  commands.hpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef tasks_hpp
#define tasks_hpp

enum TaskType {
	stopTask = 0,
	testTask1,		// 1
	testTask2,		// 2
	scanTask,		// 3
	pingTask,		// 4
	scanpingTask,	// 5
	testTaskCount	// 6, size of TaskType enum
};


class TaskMaster {
	
	bool stopLoop;
public:
	void setupTaskMaster();
	void shutdownTaskMaster();
	void mobileTask( int taskNumber, int param );
	
	void serviceTaskMaster( int command, int socket ) ;

	void killTasks();
	void taskTest1();
	void taskTest2();
	void taskScan();
	void taskPing();
	void taskScanPing();
};

// extern TaskMaster	task;

#endif /* tasks_hpp */
