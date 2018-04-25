//
//  threader.cpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "mtrctl.hpp"

#include <pthread.h>


Threader	threader;

void ThreadControl::initThread( ThreadType threadType, int socket, uint address ) {
	nextThreadType = threadType;
	nextSocket = socket;
	newAddress = address;
	threader.createThread();
}


void *runThreads(void *arguments) {

	threader.runThread( arguments );
	return nullptr;
}


void Threader::setupThreader() {
	
	pthread_mutex_init( &threadControlMutex, nullptr );

}

void Threader::shutdownThreads() {
	
	pthread_mutex_destroy( &threadControlMutex );
}

void Threader::createThread() {
	
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

void *Threader::runThread(void *arguments) {

	ThreadControl tc;
	pthread_mutex_lock( &threadControlMutex );
//	if threadArray.count > 0 {
//		tc = threadArray.remove(at: 0)
//	}
	pthread_mutex_unlock( &threadControlMutex );
//	guard let nextThreadControl = tc else { return }
//
	threadCount += 1;
//	//	printx( "Thread count: \(threadCount) for \(nextThreadControl.nextThreadType.rawValue)" )
//
//	switch nextThreadControl.nextThreadType {
//	case .senderThread:
//		sender?.doLoop()
//	case .listenThread:
//		listener?.doListen()
//	case .serverThread:
//		serverThread( sockfd: nextThreadControl.nextSocket, address: nextThreadControl.newAddress )
//	case .inputThread:
//		consumer = Consumer()
//		consumer?.consume()
//	case .blinkThread:
//#if	os(Linux)
//		hardware.blink()
//#endif
//	case .testThread:
//		let testerThread = ThreadTester()
//		testerThread.testThread()
//	}
	threadCount -= 1;
//	//	printx( "Threads remaining: \(threadCount) after exit for \(nextThreadControl.nextThreadType.rawValue)" )

	return nullptr;
}

void Threader::startThread() {
	
	pthread_mutex_lock( &threadControlMutex );
//	threadArray.append( ThreadControl( threadType: threadType, socket: socket, address: address ) );
	pthread_mutex_unlock( &threadControlMutex );
}
