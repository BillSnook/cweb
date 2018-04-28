//
//  threader.cpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "threader.hpp"
#include "listen.hpp"

#include <pthread.h>


extern Listener	listener;

Threader	threader;


ThreadControl ThreadControl::initThread( ThreadType threadType, int socket, uint address ) {
	ThreadControl newThreadControl = ThreadControl();
	newThreadControl.nextThreadType = threadType;
	newThreadControl.nextSocket = socket;
	newThreadControl.newAddress = address;
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
		default:
			name = "noThread";
			break;
	}
	return name;
}


void *runThreads(void *arguments) {

	threader.runThread( arguments );
	return nullptr;
}


void Threader::setupThreader() {
	
	pthread_mutex_init( &threadArrayMutex, nullptr );
	
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
	
	fprintf( stderr, "\nIn queueThread at start\n" );
	ThreadControl nextThreadControl = ThreadControl::initThread( threadType, socket, address );
	pthread_mutex_lock( &threadArrayMutex );
	try {
		threadQueue.push( nextThreadControl );
	} catch(...) {
		fprintf( stderr, "\nIn queueThread, thread queue push failure occured\n" );
	}
	pthread_mutex_unlock( &threadArrayMutex );
}

void Threader::createThread() {
	
	fprintf( stderr, "\nIn createThread at start\n" );
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
	pthread_mutex_lock( &threadArrayMutex );
	try {
		if ( ! threadQueue.empty() ) {
			nextThreadControl = threadQueue.front();
			threadQueue.pop();
			fprintf( stderr, "\nIn runThread with entry in threadQueue for thread type %s\n", nextThreadControl.description() );
		} else {
			fprintf( stderr, "\nIn runThread with no entries in threadQueue\n" );
		}
	} catch(...) {
		fprintf( stderr, "\nIn runThread and threadQueue pop failure occured\n" );
	}
	pthread_mutex_unlock( &threadArrayMutex );
	
	threadCount += 1;
	fprintf( stderr, "\nThread count: %d\n", threadCount );
	switch ( nextThreadControl.nextThreadType ) {
		case listenThread:
			listener.setupListener( nextThreadControl.nextSocket );
			listener.acceptConnections();
			break;
		case serverThread:
			listener.serviceConnection( nextThreadControl.nextSocket );
			break;
		case testThread:
			break;
		default:
			break;
	}
	threadCount -= 1;
	fprintf( stderr, "\nIn runThread with %d threads remaining after exit for thread type %s\n", threadCount, nextThreadControl.description() );
}
