//
//  threader.cpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "threader.hpp"
#include "listen.hpp"
#include "commands.hpp"
#include "tasks.hpp"
#include "manager.hpp"

#include <pthread.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>

extern Threader		threader;
extern TaskMaster	taskMaster;
extern Manager		manager;
extern Commander    commander;



ThreadControl ThreadControl::initThread( ThreadType threadType, int socket, uint address ) {
	ThreadControl newThreadControl = ThreadControl();
	newThreadControl.nextThreadType = threadType;
	newThreadControl.nextSocket = socket;
	newThreadControl.nextAddress = address;
	return newThreadControl;
}

ThreadControl ThreadControl::initThread( ThreadType threadType, char *command, int socket ) {
	ThreadControl newThreadControl = ThreadControl();
	newThreadControl.nextThreadType = threadType;
	newThreadControl.nextSocket = socket;
	memcpy( newThreadControl.nextCommand, command, COMMAND_SIZE );
	return newThreadControl;
}

const char *ThreadControl::description() {
	const char *name;
	switch (nextThreadType ) {
		case managerThread:
			name = "managerThread";
			break;
		case listenThread:
			name = "listenThread";
			break;
		case serverThread:
			name = "serverThread";
			break;
		case commandThread:
			name = "commandThread";
			break;
		case taskThread:
			name = "taskThread";
			break;
		default:
			name = "noThread";
			break;
	}
	return name;
}


void Threader::setupThreader() {
	
	pthread_mutex_init( &threadArrayMutex, nullptr );
    manager = Manager();
    manager.setupManager();         // Manages i2c queue and controller communication
    threader.queueThread( managerThread, 8, 0 );
    threader.createThread();
    sleep( 1 );

	commander = Commander();
	commander.setupCommander();		// Manages mostly external commands

    taskMaster = TaskMaster();
	taskMaster.setupTaskMaster(); 	// Manages task queue - to allow multiple tasks at once
}

void Threader::shutdownThreads() {
	
	syslog(LOG_NOTICE, "In shutdownThreads" );
	
	taskMaster.shutdownTaskMaster();
	commander.shutdownCommander();
    manager.shutdownManager();
	pthread_mutex_destroy( &threadArrayMutex );
}

void Threader::lock() {
	
	pthread_mutex_lock( &threadArrayMutex );
}

void Threader::unlock() {
	
	pthread_mutex_unlock( &threadArrayMutex );
}

bool Threader::areThreadsOnQueue() {
	
	return !threadQueue.empty();
}

void Threader::queueThread( ThreadType threadType, int socket, uint address ) {
	
//	syslog(LOG_NOTICE, "In queueThread1 at start" );
	ThreadControl nextThreadControl = ThreadControl::initThread( threadType, socket, address );
	pthread_mutex_lock( &threadArrayMutex );
	try {
		threadQueue.push( nextThreadControl );
	} catch(...) {
		syslog(LOG_NOTICE, "In queueThread, thread queue push failure occured" );
	}
	pthread_mutex_unlock( &threadArrayMutex );
}

void Threader::queueThread( ThreadType threadType, char *command, int socket ) {
	
//	syslog(LOG_NOTICE, "In queueThread2 for command at start" );
	ThreadControl nextThreadControl = ThreadControl::initThread( threadType, command, socket );
	pthread_mutex_lock( &threadArrayMutex );
	try {
		threadQueue.push( nextThreadControl );
	} catch(...) {
		syslog(LOG_NOTICE, "In queueThread, thread queue push failure occured" );
	}
	pthread_mutex_unlock( &threadArrayMutex );
}

void Threader::createThread() {
	
//	syslog(LOG_NOTICE, "In createThread at start" );
	pthread_t		*threadPtr = new pthread_t;
	pthread_attr_t	*attrPtr = new pthread_attr_t;

	pthread_attr_init( attrPtr );
	pthread_attr_setdetachstate( attrPtr, 0 );

	pthread_create(threadPtr,
				   attrPtr,
				   startThread,
				   nullptr);

	free( attrPtr );
	free( threadPtr );
}

void Threader::runNextThread( void *arguments ) {

	ThreadControl nextThreadControl;
	bool foundThread = false;
	pthread_mutex_lock( &threadArrayMutex );
	try {
		if ( ! threadQueue.empty() ) {
			nextThreadControl = threadQueue.front();
			threadQueue.pop();
			foundThread = true;
//		} else {
//			syslog(LOG_NOTICE, "In runNextThread with no entries in threadQueue" );
		}
	} catch(...) {
		syslog(LOG_NOTICE, "In runNextThread and threadQueue pop failure occured" );
	}
	pthread_mutex_unlock( &threadArrayMutex );
	if ( !foundThread ) {
		return;
	}
	threadCount += 1;
	syslog(LOG_NOTICE, "In runNextThread with %s, thread count %d", nextThreadControl.description(), threadCount );
	switch ( nextThreadControl.nextThreadType ) {
		case managerThread:
			manager.monitor();
			break;
		case listenThread:
			listener.acceptConnections( nextThreadControl.nextSocket );
			break;
		case serverThread:
			listener.serviceConnection( nextThreadControl.nextSocket );
			break;
		case commandThread:
			commander.serviceCommand( nextThreadControl.nextCommand, nextThreadControl.nextSocket );
			break;
		case taskThread:
			taskMaster.serviceTaskMaster( nextThreadControl.nextSocket, nextThreadControl.nextAddress );
			break;
		case testThread:
			syslog(LOG_NOTICE, "In runNextThread with testThreads" );
			break;
		default:
			break;
	}
	threadCount -= 1;
	syslog(LOG_NOTICE, "In runNextThread, exiting from %s, thread count %d", nextThreadControl.description(), threadCount );
}

void *startThread(void *arguments) {
	
	threader.runNextThread( arguments );
	return nullptr;
}
