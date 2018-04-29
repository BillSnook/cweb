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
#include <syslog.h>
#include <arpa/inet.h>


#define bufferSize	256

extern Threader	threader;


void Listener::setupListener( int rcvPortNo) {	// Create and bind socket for listening
	
	listenSockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( listenSockfd < 0 )
		syslog(LOG_NOTICE, "ERROR opening socket" );
	bzero( (char *)&serv_addr, sizeof( serv_addr ) );
	portno = rcvPortNo;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( portno );
	if ( bind( listenSockfd, (struct sockaddr *)&serv_addr, sizeof( serv_addr) ) < 0) {
		syslog(LOG_NOTICE, "ERROR on binding"  );
		return;
	}
	syslog(LOG_NOTICE, "Success binding to socket port %d on %s", portno, inet_ntoa(serv_addr.sin_addr) );
	doListenerLoop = true;
}

void Listener::acceptConnections() {	// Main listening routine
	
	socklen_t clilen = sizeof( cli_addr );
	while ( doListenerLoop ) {
		syslog(LOG_NOTICE, "In acceptConnections, listening" );
		listen(  listenSockfd, 5 );
		int connectionSockfd = accept(  listenSockfd, (struct sockaddr *)&cli_addr, &clilen);
		if ( connectionSockfd < 0 ) {
			syslog(LOG_NOTICE, "ERROR on accept" );
			break;
		}
		syslog(LOG_NOTICE, "Accepted connection, clientAddr: %s", inet_ntoa( cli_addr.sin_addr ) );
		
		threader.queueThread( serverThread, connectionSockfd, 0 );
		
//		doListenerLoop = false; // Do once for testing
	}
	close( listenSockfd );
	syslog(LOG_NOTICE, "In acceptConnections at exit" );
}

void Listener::serviceConnection( int connectionSockfd ) {
	
	long	n;
	bool	localLoop = true;
	while ( localLoop ) {
		char	*buffer = (char *)valloc( bufferSize );
		bzero( buffer, bufferSize );
//		syslog(LOG_NOTICE, "In serviceConnection waiting for data...");
		n = read( connectionSockfd, buffer, bufferSize );
		if ( n <= 0 ) {
			syslog(LOG_NOTICE, "ERROR reading command from socket" );
			break;
		}
		
		syslog(LOG_NOTICE, "Here is a received message: %s", buffer );
		// start thread to service command
		
		threader.queueThread( commandThread, buffer );
		free( buffer );

		n = write( connectionSockfd, "\nAck\n", 5 );
		if ( n < 0 ) {
			syslog(LOG_NOTICE, "ERROR writing ack to socket" );
			break;
		}
	}
	close( connectionSockfd );
	syslog(LOG_NOTICE, "In serviceConnection at end" );
}
