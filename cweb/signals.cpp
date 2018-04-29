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

//#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <signal.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <syslog.h>


void signals_setup() {

	signal(SIGINT, sig_handler);
	signal(SIGKILL, sig_handler);
}

void sig_handler(int signum) {

	switch ( signum ) {
		case 2:
			syslog(LOG_NOTICE, "Received INT signal (ctrl-C), exiting now." );
			exit( 0 );
			break;
		case 9:
			syslog(LOG_NOTICE, "Received Kill signal, exiting now." );
			leaveGracefully( signum );
			break;
		default:
			syslog(LOG_NOTICE, "Received unhandled signal %d", signum );
			break;
	}
}

void leaveGracefully( int signum ) {
	
	exit( 0 );
}
