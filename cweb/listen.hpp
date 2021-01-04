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

#define AP_SIZE     32

struct addrPort {
    int addr;
    int port;
};


class Listener {
	
	int					listenSockfd, portno;
    unsigned int        addrno;
//	char				buffer[256];
	bool				doListenerLoop;
    addrPort            apArray[AP_SIZE];

public:
	void acceptConnections( int rcvPortNo );
	void serviceConnection( int connectionSockfd, char *inet_address );
	void writeBack( char *msg, int socket );
	void writeBlock( char *msg, int length, int socket );
};

extern Listener	listener;

#endif /* listen_hpp */
