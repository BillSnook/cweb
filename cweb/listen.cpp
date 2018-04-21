//
//  listen.cpp
//  cweb
//
//  Created by William Snook on 4/15/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "listen.hpp"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void Listener::setupListener( int rcvPortNo) {
	
	listenSockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( listenSockfd < 0 )
		fprintf( stderr, "\nERROR opening socket" );
	bzero( (char *)&serv_addr, sizeof( serv_addr ) );
	portno = rcvPortNo;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( portno );
	if ( bind( listenSockfd, (struct sockaddr *)&serv_addr, sizeof( serv_addr)  ) < 0) {
		fprintf( stderr, "\nERROR on binding"  );
	}
	fprintf( stderr, "\nSuccess binding to socket port %d (0x%02X) on %s, got fd: %d\n\n", portno, portno, inet_ntoa(serv_addr.sin_addr), listenSockfd);
	doLoop = true;
}

void Listener::doListen() {
	
	listen(  listenSockfd, 5 );
	clilen = sizeof( cli_addr );
	while ( doLoop ) {
		connectionSockfd = accept(  listenSockfd, (struct sockaddr *)&cli_addr, &clilen);
		fprintf( stderr, "\nAccepted connection, clientAddr: %s", inet_ntoa( cli_addr.sin_addr ) );
		if ( connectionSockfd < 0 ) {
			fprintf( stderr, "\nERROR on accept" );
			return;
		}
		bzero( buffer, 256 );
		long n = read( connectionSockfd, buffer, 255 );
		if (n < 0) {
			fprintf(  stderr, "\nERROR reading command from socket");
			return;
		}
		
		printf( "\nHere is the message: %s\n", buffer );
		// start thread to service command
		

		
		n = write( connectionSockfd, "\nAck\n", 5 );
		if (n < 0) {
			fprintf( stderr, "\nERROR writing ack to socket" );
			return;
		}
		close( connectionSockfd );
	}
	close( listenSockfd );
}
