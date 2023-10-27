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
//#include "manager.hpp"
#include "filer.hpp"


#define	PORT	5555

#define ALLOW_DAEMON     // Potentially become a daemon and run in the background

Filer       filer;
Threader	threader;

Listener	listener;

bool		stayOnline;


int main(int argc, const char * argv[]) {

    bool becomeDaemon = false;
    bool ready = false;

//    TIMER_Init();

#ifdef ALLOW_DAEMON
	if ( argc == 3 ) {
		becomeDaemon = true;
	}
#endif	// ALLOW_DAEMON
#ifdef ON_PI
#else	// else if not ON_PI
	becomeDaemon = false;
#endif	// ON_PI
	if ( becomeDaemon ) {
		
		pid_t pid;
		
		pid = fork();       // Fork off the parent process
		
		if (pid < 0)        // An error occurred
			exit(EXIT_FAILURE);
		
		if (pid > 0)        // We are the parent so we terminate while the child continues
			exit(EXIT_SUCCESS);
		
		if (setsid() < 0)   // On success: The child process becomes session leader
			exit(EXIT_FAILURE);
		
		signal(SIGCHLD, SIG_IGN);   // Catch, ignore and/or handle signals
		signal(SIGHUP, SIG_IGN);
		signals_setup();    // Setup and manage signal handling
		
		pid = fork();       // Fork off for the second time: process magic
		
		if (pid < 0)        // An error occurred
			exit(EXIT_FAILURE);

		if (pid > 0)        // Success: Let the parent terminate
			exit(EXIT_SUCCESS);
		
        // Now we are Daemon
		umask(0);           // Set new file permissions
		
		chdir("/");         // Change the working directory to the root directory
		
		long x;             // Close all open file descriptors
		for (x = sysconf(_SC_OPEN_MAX); x>=0; x--) {
			close ( int(x) );
		}

		openlog("mtrctllog", LOG_PID, LOG_DAEMON);        // Open the log file
		syslog(LOG_NOTICE, "Started mtrctl as daemon");
		
	} else {    // Not daemon, regular program
		
		signals_setup();

		openlog("mtrctllog", LOG_PID | LOG_PERROR, LOG_USER);	// Also log to stderr
		syslog(LOG_NOTICE, "Started mtrctl as user");
	}

    // Done figuring out whether we are a daemon which is running in the background, or not.
    stayOnline = true;
    ready = true;

    filer.getHostName();
    filer.setupFiles();

//	threader = Threader();
	threader.setupThreader();

//    listener = Listener();
    uint16_t portNo = PORT;
    threader.queueThread( listenThread, portNo, 0 );
	syslog(LOG_NOTICE, "Threader and Listener setup, ready to service queue and accept commands on port %u, v4.0.0", portNo );

	while ( stayOnline ) {
		threader.lock();
		ready = threader.areThreadsOnQueue();
		threader.unlock();
		if ( ready ) {
			threader.createThread();
            usleep( 1000 );
        } else {
            usleep( 200000 );
		}
	}
	
	threader.shutdownThreads();

    return 0;
}
