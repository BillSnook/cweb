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
	testTask1
	testTask2,
	scanTask,
	testTask4,
	testTaskCount
};


class TaskMaster {
	
	bool stopLoop;
public:
	void setupTaskMaster();
	void serviceTaskMaster( int command, int socket ) ;

	void killTasks();
	void taskTest1();
	void taskTest2();
};

extern TaskMaster	task;

#endif /* tasks_hpp */
