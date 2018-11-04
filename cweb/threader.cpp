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

#include <pthread.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>

extern Threader		threader;
extern TaskMaster	taskMaster;


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
	commander = Commander();
	commander.setupCommander();		// Manages mostly external commands
	taskMaster = TaskMaster();
	taskMaster.setupTaskMaster(); 		// Manages task queue - to allow multiple tasks at once
}

void Threader::shutdownThreads() {
	
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
	
	syslog(LOG_NOTICE, "In queueThread at start" );
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
	
	syslog(LOG_NOTICE, "In queueThread for command at start" );
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
	
	syslog(LOG_NOTICE, "In createThread at start" );
	pthread_t		*threadPtr = new pthread_t;
	pthread_attr_t	*attrPtr = new pthread_attr_t;

	pthread_attr_init( attrPtr );
	pthread_attr_setdetachstate( attrPtr, 0 );

	pthread_create(threadPtr,
				   attrPtr,
				   runThreads,
				   nullptr);

	free( attrPtr );
	free( threadPtr );
}

void Threader::runThread( void *arguments ) {

	ThreadControl nextThreadControl;
	bool foundThread = false;
	pthread_mutex_lock( &threadArrayMutex );
	try {
		if ( ! threadQueue.empty() ) {
			nextThreadControl = threadQueue.front();
			threadQueue.pop();
			syslog(LOG_NOTICE, "In runThread with entry in threadQueue for thread type %s", nextThreadControl.description() );
			foundThread = true;
		} else {
			syslog(LOG_NOTICE, "In runThread with no entries in threadQueue" );
		}
	} catch(...) {
		syslog(LOG_NOTICE, "In runThread and threadQueue pop failure occured" );
	}
	pthread_mutex_unlock( &threadArrayMutex );
	if ( !foundThread ) {
		return;
	}
	threadCount += 1;
	syslog(LOG_NOTICE, "Thread count: %d", threadCount );
	switch ( nextThreadControl.nextThreadType ) {
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
			syslog(LOG_NOTICE, "In runThread with testThreads" );
			break;
		default:
			break;
	}
	threadCount -= 1;
	syslog(LOG_NOTICE, "In runThread at exit from thread type %s with %d threads left", nextThreadControl.description(), threadCount );
}

void *runThreads(void *arguments) {
	
	threader.runThread( arguments );
	return nullptr;
}
