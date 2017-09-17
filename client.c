#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "client.h"
#include "server.h"


int clientmain( void ) {
    
    printf( "\nIn clientmain\n\n" );
//    return(0);
    
    
    int sockfd, portno;
    long n = 0;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    portno = CONNECTION_PORT;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname( CONNECTION_LISTENER ); // "localhost" ); // "workpi.local" );
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    while ( n < 255 ) {
        // TODO: check inputs here to see if message is to be set else prompt
        printf("\nPlease enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);    // Waits for input
        
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
             error("ERROR writing to socket");
        bzero(buffer,256);
        n = read(sockfd,buffer,255);
        if (n < 0) 
             error("ERROR reading from socket");
        
        printf("\n%s\n",buffer);
    }
    
    close(sockfd);
    return 0;
}

