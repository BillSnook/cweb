//
//  signals.cpp
//  cweb
//
//  Created by William Snook on 4/15/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "signals.hpp"

/*
case HUP    = 1
case INT    = 2		// ^C
case QUIT   = 3
case ABRT   = 6
case KILL   = 9
case ALRM   = 14
case TERM   = 15
*/

#include <stdlib.h>
#include <signal.h>


void signals_setup() {
	
	signal(SIGINT, sig_handler);
	signal(SIGKILL, sig_handler);
}

void sig_handler(int signum) {
	
	switch ( signum ) {
		case 2:
			fprintf( stderr, "\n\nReceived INT signal (ctrl-C), exiting now.\n\n" );
			exit( 0 );
			break;
		case 9:
			fprintf( stderr, "\n\nReceived Kill signal, exiting now.\n\n" );
			leaveGracefully( signum );
			break;
		default:
			fprintf( stderr, "\n\nReceived unhandled signal %d\n", signum );
			break;
	}
}

void leaveGracefully( int signum ) {
	
	exit( 0 );
}
