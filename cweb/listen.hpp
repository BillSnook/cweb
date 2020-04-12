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


class Listener {
	
	int					listenSockfd, portno;
	char				buffer[256];
	struct sockaddr_in	serv_addr, cli_addr;
	bool				doListenerLoop;

public:
	void acceptConnections( int rcvPortNo );
	void serviceConnection( int connectionSockfd );
	void writeBack( char *msg, int socket );
	void writeBlock( char *msg, int length, int socket );
};

extern Listener	listener;

#endif /* listen_hpp */
