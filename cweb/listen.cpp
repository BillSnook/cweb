//
//  listen.cpp
//  cweb
//
//  Created by William Snook on 4/15/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "listen.hpp"
#include "threader.hpp"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


#define bufferSize	256

extern Threader	threader;


void Listener::setupListener( int rcvPortNo) {	// Create and bind socket for listening
	
	listenSockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( listenSockfd < 0 )
		fprintf( stderr, "\nERROR opening socket" );
	bzero( (char *)&serv_addr, sizeof( serv_addr ) );
	portno = rcvPortNo;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( portno );
	if ( bind( listenSockfd, (struct sockaddr *)&serv_addr, sizeof( serv_addr) ) < 0) {
		fprintf( stderr, "\nERROR on binding"  );
		return;
	}
	fprintf( stderr, "\nSuccess binding to socket port %d on %s\n", portno, inet_ntoa(serv_addr.sin_addr) );
	doListenerLoop = true;
}

void Listener::acceptConnections() {	// Main listening routine
	
	socklen_t clilen = sizeof( cli_addr );
	while ( doListenerLoop ) {
		fprintf( stderr, "\nIn acceptConnections, listening\n" );
		listen(  listenSockfd, 5 );
		int connectionSockfd = accept(  listenSockfd, (struct sockaddr *)&cli_addr, &clilen);
		if ( connectionSockfd < 0 ) {
			fprintf( stderr, "\nERROR on accept" );
			break;
		}
		fprintf( stderr, "\nAccepted connection, clientAddr: %s\n", inet_ntoa( cli_addr.sin_addr ) );
		
		threader.queueThread( serverThread, connectionSockfd, 0 );
		
//		doListenerLoop = false; // Do once for testing
	}
	close( listenSockfd );
	fprintf( stderr, "\nIn acceptConnections at exit\n" );
}

void Listener::serviceConnection( int connectionSockfd ) {
	
//	char	localBuffer[256];
	char	*buffer = (char *)valloc( bufferSize );
	long	n;
	bool	localLoop = true;
	while ( localLoop ) {
		bzero( buffer, bufferSize );
//		fprintf(  stderr, "\nIn serviceConnection waiting for data...\n");
		n = read( connectionSockfd, buffer, bufferSize );
		if ( n <= 0 ) {
			fprintf( stderr, "\nERROR reading command from socket\n" );
			break;
		}
		
		fprintf( stderr, "\nHere is a received message: %s", buffer );
		// start thread to service command
		
		threader.queueThread( commandThread, buffer );

		n = write( connectionSockfd, "\nAck\n", 5 );
		if ( n < 0 ) {
			fprintf( stderr, "\nERROR writing ack to socket\n" );
			break;
		}
	}
	close( connectionSockfd );
	fprintf( stderr, "\nIn serviceConnection at end\n\n" );
}
