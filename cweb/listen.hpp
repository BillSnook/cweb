//
//  listen.hpp
//  cweb
//
//  Created by William Snook on 4/15/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef listen_hpp
#define listen_hpp

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Listener {
	
	int					listenSockfd, portno;
	socklen_t			clilen;
	char				buffer[256];
	struct sockaddr_in	serv_addr, cli_addr;
	bool				doLoop;

public:
	int	connectionSockfd;
	
	void setupListener( int rcvPortNo );
	int  doListen();
	void serviceConnection();

};

extern Listener	listener;

#endif /* listen_hpp */
