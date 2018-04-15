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

void Listener::setupListener( int rcvPortNo) {
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		fprintf(stderr, "ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = rcvPortNo;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "ERROR on binding");
	}
	fprintf(stderr, "\nSuccess binding to socket %d, got fd: %d\n\n", portno, sockfd);
}

void Listener::doListen() {
	
	while ( doLoop ) {
		listen(sockfd,5);
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			fprintf(stderr, "ERROR on accept");
			return;
		}
		bzero(buffer,256);
		n = read(newsockfd,buffer,255);
		if (n < 0) {
			fprintf(stderr, "ERROR reading from socket");
			return;
		}
		printf("Here is the message: %s\n",buffer);
		n = write(newsockfd,"I got your message",18);
		if (n < 0) {
			fprintf(stderr, "ERROR writing to socket");
			return;
		}
	}

}
