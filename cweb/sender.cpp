//
//  sender.cpp
//  cweb
//
//  Created by William Snook on 4/15/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "sender.hpp"

#include <string.h>
#include <unistd.h>
#include <syslog.h>

#include <arpa/inet.h>

void Sender::setupSender( char *hostName, int portNo) {
	
	int sockfd;
	
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	char buffer[256];
	bool doSenderLoop;
	
    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );   // SOCK_DGRAM for UDP
//    sockfd = socket( AF_INET, SOCK_STREAM, 0 );   // SOCK_DGRAM for UDP
	if ( sockfd < 0 ) {
		syslog(LOG_ERR, "ERROR opening socket" );
		return;
	}
	server = gethostbyname( hostName );
	if ( server == NULL ) {
		syslog(LOG_ERR, "ERROR, no such host: %s", hostName );
		return;
	}
	bzero( (char *)&serv_addr, sizeof( serv_addr ) );
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		  (char *)&serv_addr.sin_addr.s_addr,
		  server->h_length);
	serv_addr.sin_port = htons( portNo );
	syslog(LOG_NOTICE, "Found host %s, ready to connect on socket port %d", inet_ntoa(serv_addr.sin_addr), portNo );
	int connectResult = connect( sockfd, (struct sockaddr *)&serv_addr,sizeof( serv_addr ) );
	if ( connectResult < 0 ) {
		syslog(LOG_ERR, "ERROR connecting: %d", connectResult );
		return;
	}
	
	doSenderLoop = true;
	while ( doSenderLoop ) {
		syslog(LOG_NOTICE, "Please enter a message: " );
		bzero( buffer, 256 );
		fgets( buffer, 255, stdin );
		long n = write( sockfd, buffer, strlen( buffer ) );
		if (n < 0) {
			syslog(LOG_ERR, "ERROR writing to socket" );
			return;
		}
		bzero( buffer, 256 );
		n = read( sockfd, buffer, 255 );
		if (n < 0) {
			syslog(LOG_ERR, "ERROR reading from socket" );
			return;
		}
		syslog(LOG_NOTICE, "%s", buffer );
	}

}
