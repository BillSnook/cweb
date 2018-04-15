//
//  sender.hpp
//  cweb
//
//  Created by William Snook on 4/15/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef sender_hpp
#define sender_hpp

#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


class Sender {
	
	int sockfd, portno, n;
	
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	char buffer[256];
	bool doLoop;
	
public:
	void setupSender( char *hostName, int rcvPortNo );
	void doSend();
	
};

#endif /* sender_hpp */
