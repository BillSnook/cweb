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
#include "signals.hpp"

#include "sender.hpp"
#include "listen.hpp"
#include "threader.hpp"

#include "power.hpp"

#define	PORT	5555

extern Threader	threader;

Listener	listener;
Sender		sender;

bool		ready;

int main(int argc, const char * argv[]) {

	doLoop = true;
	ready = true;

	signals_setup();
	
	threader = Threader();
	threader.setupThreader();
	
#ifdef ON_PI
	Power power = Power();
	char *pStatus = power.getUPS2();
	fprintf( stderr, "\nPower status: %s\n\n", pStatus );
	free( pStatus );
#endif
	
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
		usleep( 1000000 );
		threader.lock();
		ready = threader.areThreadsOnQueue();
		threader.unlock();
		if ( ready ) {
			threader.createThread();
		}
#ifdef ON_PI
		pStatus = power.getUPS2();
		fprintf( stderr, "Power status: %s\n", pStatus );
		free( pStatus );
#endif
	}
	
	threader.shutdownThreads();

    return 0;
}
