/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"
#include "message.h"


void error(const char *msg) {
    perror(msg);
    exit(1);
}

int servermain( void ) {

    printf( "\nIn servermain\n\n" );
//    return(0);

    long n = 0;
    int sockfd, newsockfd, portno;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = CONNECTION_PORT; // atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
          sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd,5);

    newsockfd = accept(sockfd,
             (struct sockaddr *) &cli_addr, 
             &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");

    messagesInit();

    while ( n < 255 ) {
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0)
            error("ERROR reading from socket");
        // TODO: handle message here
        if ( n > 0 ) {
            messageHandler( buffer );
            n = write(newsockfd,"OK",2);
            if (n < 0)
                error("ERROR writing to socket");
        }
    }

    close(newsockfd);
    close(sockfd);
    return 0; 
}

