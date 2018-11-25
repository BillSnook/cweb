//
//  mtrctl.cpp
//  mtrctl
//
//  Created by William Snook on 4/8/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include <syslog.h>

#include <strings.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "mtrctl.hpp"
#include "signals.hpp"
#include "sender.hpp"
#include "listen.hpp"
#include "threader.hpp"
//#include "power.hpp"


#define	PORT	5555

#define MAKE_DAEMON

Threader	threader;

Listener	listener;
Sender		sender;

bool		becomeDaemon;
bool		ready;

bool		doLoop;


int main(int argc, const char * argv[]) {

//	TIMER_Init();
	
#ifdef MAKE_DAEMON
	if ( argc == 3 ) {
		becomeDaemon = true;
	}
#else	// not MAKE_DAEMON
	becomeDaemon = false;
#endif	// MAKE_DAEMON
	if ( argc == 2 ) {
		becomeDaemon = false;		// Do not become daemon if sender or by intent
	}
#ifdef ON_PI
#else	// not ON_PI
	becomeDaemon = false;
#endif	// ON_PI
	if ( becomeDaemon ) {
		
		pid_t pid;
		
		/* Fork off the parent process */
		pid = fork();
		
		/* An error occurred */
		if (pid < 0)
			exit(EXIT_FAILURE);
		
		/* Success: Let the parent terminate */
		if (pid > 0)
			exit(EXIT_SUCCESS);
		
		/* On success: The child process becomes session leader */
		if (setsid() < 0)
			exit(EXIT_FAILURE);
		
		/* Catch, ignore and/or handle signals */
		signal(SIGCHLD, SIG_IGN);
		signal(SIGHUP, SIG_IGN);
		signals_setup();
		
		/* Fork off for the second time*/
		pid = fork();
		
		/* An error occurred */
		if (pid < 0)
			exit(EXIT_FAILURE);
		
		/* Success: Let the parent terminate */
		if (pid > 0)
			exit(EXIT_SUCCESS);
		
		/* Set new file permissions */
		umask(0);
		
		/* Change the working directory to the root directory */
		/* or another appropriated directory */
		chdir("/");
		
		/* Close all open file descriptors */
		long x;
		for (x = sysconf(_SC_OPEN_MAX); x>=0; x--) {
			close ( int(x) );
		}
		/* Open the log file */
		openlog("mtrctllog", LOG_PID, LOG_DAEMON);
		syslog(LOG_NOTICE, "Started mtrctl as daemon");
		
	} else {
		
		signals_setup();

		/* Open the log file */
		openlog("mtrctllog", LOG_PID | LOG_PERROR, LOG_USER);	// Also log to stderr
		syslog(LOG_NOTICE, "Started mtrctl as user - syslog + LOG_PERROR");

//		fprintf( stderr, "\nStarted mtrctl as user\n" );
	}
	
	// Done figuring out whether we are a daemon, running in the background, or not.
	doLoop = true;
	ready = true;
	
	threader = Threader();
	threader.setupThreader();
	
#ifdef ON_PI_X	// Power not attached to robot tank currently
	Power power = Power();
	char *pStatus = power.getUPS2();
	syslog(LOG_NOTICE, "Power status: %s", pStatus );
	free( pStatus );
#endif
	
	syslog(LOG_NOTICE, "mtrctl argc = %d", argc );
	if ( argc == 2 ) {	// Should be sender as we pass in host name
		char buff[32], *buffer = (char *)&buff;
		bcopy( argv[1], buffer, 31);
		sender = Sender();
		sender.setupSender( buff, PORT );
	} else {
		listener = Listener();
		threader.queueThread( listenThread, PORT, 0 );
	}

	syslog(LOG_NOTICE, "Ready to service queue" );

	while ( doLoop ) {
		usleep( 200000 );
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
