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

enum ThreadType {
	listenThread = 0,
	serverThread = 1,
	commandThread = 2,
//	inputThread = 3,
//	blinkThread = 4,
	testThread = 5
};


class ThreadControl {
public:
	ThreadType	nextThreadType;
	int			nextSocket;
	uint		newAddress;
	char		nextCommand[ COMMAND_SIZE ];
	
public:
	static ThreadControl initThread( ThreadType threadType, int socket, uint address );
	static ThreadControl initThread( ThreadType threadType, char *command, int socket );
	const char *description();
};

void *runThreads(void *arguments);

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
	void runThread(void *arguments);
};

#endif /* threader_hpp */
