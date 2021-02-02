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
    bool                firstAccess;
    addrPort            apArray[AP_SIZE];
    
    int findMatchOrNewIndex( int addr, int portno );

public:
    
    struct timeval tvLatest = {0};

	void acceptConnections( int rcvPortNo );
	void serviceConnection( int connectionSockfd, char *inet_address );
	void writeBack( char *msg, int sockOrAddr );
//	void writeBlock( char *msg, int length, int socket );
    
    long getDiffMicroSec( struct timeval startTime, struct timeval endTime );

};

extern Listener	listener;

#endif /* listen_hpp */
