//
//  listen.hpp
//  cweb
//
//  Created by William Snook on 4/15/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef listen_hpp
#define listen_hpp

#include <netinet/in.h>
#include <sys/time.h>

#define AP_SIZE     32

struct addrPort {
    int addr;
    int port;
};


class Listener {
	
	int					socketfd;
    bool                keepAliveOn;
    addrPort            apArray[AP_SIZE];
    
    struct timeval      tvLatest = {0};

    int findMatchOrNewIndex( int addr, int portno );
    long testTimedOut();                // True if timed out, called to monitor fotoo long between comms

public:
    
    void setupListener();
    void shutdownListener();
	void acceptConnections( int rcvPortNo );
	void serviceConnection( int connectionSockfd, char *inet_address );
	void writeBack( char *msg, int sockOrAddr );

    void monitor();
};

extern Listener	listener;

#endif /* listen_hpp */
