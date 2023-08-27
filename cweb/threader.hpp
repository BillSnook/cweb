//
//  threader.hpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef threader_hpp
#define threader_hpp

#include <unistd.h>
#include <netinet/in.h>

#include <queue>


#define		COMMAND_SIZE		32

enum ThreadType: uint16_t {
	managerThread = 0,
	listenThread = 1,
	serverThread = 2,
	commandThread = 3,
    taskThread = 4,
    keepAliveThread = 5,
	testThread = 6
};


class ThreadControl {
public:
	ThreadType	nextThreadType;
	int			nextSocket;
	uint		nextAddress;
	char		nextCommand[ COMMAND_SIZE ];
	
public:
	static ThreadControl initThread( ThreadType threadType, int socket, uint address );
	static ThreadControl initThread( ThreadType threadType, int socket, char *command );
	const char *description();
};

void *startThread(void *arguments); // Works best with pthread create as wrapper for c++ method

class Threader {
	
	pthread_mutex_t 			threadArrayMutex;		// Protect the queue
	std::queue<ThreadControl>	threadQueue;
	int							threadCount = 0;		// Debug counter
	
public:
	void setupThreader();
	void shutdownThreads();
	
	void lock();
	void unlock();
	
	bool areThreadsOnQueue();
	
	void queueThread( ThreadType threadType, int socket, uint address );
	void queueThread( ThreadType threadType, char *command, int socket );
	void createThread();
	void runNextThread(void *tcPointer);
};

#endif /* threader_hpp */
*
