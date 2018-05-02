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
	void setupListener( int rcvPortNo );
	void acceptConnections();
	void serviceConnection( int connectionSockfd );

};

extern Listener	listener;

#endif /* listen_hpp */
