//
//  sender.cpp
//  cweb
//
//  Created by William Snook on 4/15/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "sender.hpp"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void Sender::setupSender( char *hostName, int rcvPortNo) {
	
	portno = rcvPortNo;
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( sockfd < 0 )
		fprintf( stderr, "\nERROR opening socket\n" );
	server = gethostbyname( hostName );
	if ( server == NULL ) {
		fprintf( stderr,"\nERROR, no such host: %s\n", hostName );
		return;
	}
	bzero( (char *)&serv_addr, sizeof( serv_addr ) );
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		  (char *)&serv_addr.sin_addr.s_addr,
		  server->h_length);
	serv_addr.sin_port = htons( portno );
	fprintf( stderr, "\nFound host %s, ready to connect on socket port %d (0x%02X)\n\n", inet_ntoa(serv_addr.sin_addr), portno, portno );
	fflush( stderr );
	int connectResult = connect( sockfd, (struct sockaddr *)&serv_addr,sizeof( serv_addr ) );
	if ( connectResult < 0 ) {
		fprintf( stderr, "\nERROR connecting: %d\n", connectResult );
		return;
	}
	fprintf( stderr, "Please enter the message: " );
	bzero( buffer, 256 );
	fgets( buffer, 255, stdin );
	long n = write( sockfd, buffer, strlen( buffer ) );
	if (n < 0) {
		fprintf( stderr, "\nERROR writing to socket\n" );
		return;
	}
	bzero( buffer, 256 );
	n = read( sockfd, buffer, 255 );
	if (n < 0)
		fprintf( stderr, "\nERROR reading from socket\n" );
	printf( "%s\n", buffer );
}

void Sender::doSend() {
	
//	while ( doLoop ) {
//		listen(sockfd,5);
//		clilen = sizeof(cli_addr);
//		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
//		if (newsockfd < 0) {
//			fprintf(stderr, "\nERROR on accept");
//			return;
//		}
//		bzero(buffer,256);
//		n = read(newsockfd,buffer,255);
//		if (n < 0) {
//			fprintf(stderr, "\nERROR reading from socket");
//			return;
//		}
//		printf("Here is the message: %s\n",buffer);
//		n = write(newsockfd,"I got your message",18);
//		if (n < 0) {
//			fprintf(stderr, "\nERROR writing to socket");
//			return;
//		}
//	}
	
}
