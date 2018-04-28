//
//  mtrctl.cpp
//  mtrctl
//
//  Created by William Snook on 4/8/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include <iostream>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mtrctl.hpp"
#include "common.hpp"
#include "signals.hpp"

#include "sender.hpp"
#include "listen.hpp"
#include "threader.hpp"

extern Threader	threader;

Listener	listener;
Sender		sender;

bool		doLoop;
bool		ready;

int main(int argc, const char * argv[]) {

	doLoop = true;
	ready = true;

	signals_setup();
	
	threader = Threader();
	threader.setupThreader();
	
//	fprintf( stderr, "\nargc = %d\n", argc );
	if ( argc > 1 ) {	// Should be sender as we pass in host name
		char buff[32], *buffer = (char *)&buff;
		bcopy( argv[1], buffer, 31);
		sender = Sender();
		sender.setupSender( buff, PORT );
	} else {
		listener = Listener();
		threader.queueThread( listenThread, PORT, 0 );
	}
	
	while ( doLoop ) {
		usleep( 10000 );
		threader.lock();
		ready = threader.areThreadsOnQueue();
		threader.unlock();
		if ( ready ) {
			threader.createThread();
		}
	}
	
	threader.shutdownThreads();

    return 0;
}
