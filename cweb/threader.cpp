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

Manager      manager;
TaskMaster   taskMaster;

extern Threader		threader;
extern Commander    commander;


// Class or factory methods to create and initialize an instance of ThreadControl
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
    int cmdSize = (int)strlen( command );
    syslog(LOG_NOTICE, "In initThread with cmdSize: %d, command: %s.", cmdSize, command );
	memcpy( newThreadControl.nextCommand, command, cmdSize );
    cmdSize = (int)strlen( newThreadControl.nextCommand );
    syslog(LOG_NOTICE, "In initThread with cmdSize: %d, command: %s.", cmdSize, newThreadControl.nextCommand );
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
    queueThread( managerThread, 8, 0 );
    usleep( 100000 );   // 1/10 second but will it change threads?
    createThread();
    usleep( 100000 );   // 1/10 second but will it change threads?

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

//    syslog(LOG_NOTICE, "In createThread at start" );
//    usleep(1000000);
    ThreadControl nextThreadControl;
    bool foundThread = false;
    pthread_mutex_lock( &threadArrayMutex );
    try {
        if ( ! threadQueue.empty() ) {
            nextThreadControl = threadQueue.front();
            threadQueue.pop();
            foundThread = true;
//        } else {
//            syslog(LOG_NOTICE, "In runNextThread with no entries in threadQueue" );
        }
    } catch(...) {
        syslog(LOG_NOTICE, "In runNextThread and threadQueue pop failure occured" );
    }
    pthread_mutex_unlock( &threadArrayMutex );
    if ( !foundThread ) {
        return;
    }

//    syslog(LOG_NOTICE, "In createThread after threadControl accessed" );
	pthread_t		*threadPtr = new pthread_t;
	pthread_attr_t	*attrPtr = new pthread_attr_t;

	pthread_attr_init( attrPtr );
	pthread_attr_setdetachstate( attrPtr, 0 );
    
    // WFS We need a better way to discriminate how we want threads to be high priority
    if ( ( nextThreadControl.nextThreadType == commandThread ) && ( nextThreadControl.nextCommand[0] <= '9' ) ) {    // 0 - 9
        int result = pthread_attr_setschedpolicy( attrPtr, SCHED_FIFO );
        if (result != 0) {
            syslog(LOG_ERR, "In createThread, failed setting thread FIFO policy to SCHED_FIFO" );
        }

//        int min = sched_get_priority_min( SCHED_FIFO );
//        syslog(LOG_NOTICE, "In createThread, sched priority min: %d", min );

        struct sched_param priority = {0};
        priority.sched_priority = 10;       // Values can be from 1 to 99
        result = pthread_attr_setschedparam( attrPtr, &priority );
        if (result != 0) {
            syslog(LOG_ERR, "In createThread, failed setting initial thread FIFO parameter to %d", priority.sched_priority );
        }
        syslog(LOG_NOTICE, "In createThread with SCHED_FIFO policy set with priority of %d", priority.sched_priority);
    }

	pthread_create(threadPtr,
				   attrPtr,
				   startThread,
                   &nextThreadControl);

	free( attrPtr );
	free( threadPtr );
}

void Threader::runNextThread( void *tcPointer ) {
    
    ThreadControl nextThreadControl = *((ThreadControl *)tcPointer);
	threadCount += 1;
	syslog(LOG_NOTICE, "In runNextThread with %s, thread count %d", nextThreadControl.description(), threadCount );
	switch ( nextThreadControl.nextThreadType ) {
		case managerThread:         // Singleton, started first, manages I2C communication
			manager.monitor();
			break;
		case listenThread:          // Singleton, started second, accepts WiFi connections from controllers
                                    // For datagram, binds socket to port and returns
			listener.acceptConnections( nextThreadControl.nextSocket );
			break;
		case serverThread:          // One started for each connection accepted, queues commands received
			listener.serviceConnection( nextThreadControl.nextSocket, nextThreadControl.nextCommand );
			break;
		case commandThread:         // One for each command queued, executes method for command with params
        {    int cmdSize = (int)strlen(nextThreadControl.nextCommand);
            syslog(LOG_NOTICE, "In runNextThread with %d bytes in command %s", cmdSize, nextThreadControl.nextCommand );
			commander.serviceCommand( nextThreadControl.nextCommand, nextThreadControl.nextSocket );
            break; }
		case taskThread:            // Thread intended for longer running discrete tasks - some commands initiate tasks
			taskMaster.serviceTaskMaster( nextThreadControl.nextSocket, nextThreadControl.nextAddress );
			break;
		case testThread:
			syslog(LOG_NOTICE, "In runNextThread with testThread" );
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
