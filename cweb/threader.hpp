//
//  threader.hpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef threader_hpp
#define threader_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


enum ThreadType {
	senderThread = 0,
	listenThread = 1,
	serverThread = 2,
	inputThread = 3,
	blinkThread = 4,
	testThread = 5
};


class ThreadControl {
	ThreadType nextThreadType;
	int nextSocket;
	uint newAddress;
	
public:
	void initThread( ThreadType threadType, int socket, uint address );
};

void *runThreads(void *arguments);

class Threader {
	
	pthread_mutex_t 	threadControlMutex;			// Protect the list
	ThreadControl		threadArray[16], *threads = threadArray;
	int					threadCount = 0;			// Debug counter
	
public:
	void setupThreader();
	void shutdownThreads();

	void createThread();
	void *runThread(void *arguments);
	void startThread();

};


#endif /* threader_hpp */
