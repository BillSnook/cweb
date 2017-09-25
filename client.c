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


//struct hostent {
//	char	*h_name;	/* official name of host */
//	char	**h_aliases;	/* alias list */
//	int	h_addrtype;	/* host address type */
//	int	h_length;	/* length of address */
//	char	**h_addr_list;	/* list of addresses from name server */
//#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
//#define	h_addr	h_addr_list[0]	/* address, for backward compatibility */
//#endif /* (!_POSIX_C_SOURCE || _DARWIN_C_SOURCE) */
//};



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
    server = gethostbyname( CONNECTION_LISTENER ); // "localhost" ); // "workpi.local" ); // usr/include/netdb.h - dns call??
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
        printf("> ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);    // Waits for input
        
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
             error("ERROR writing to socket");
        bzero(buffer,256);
        n = read(sockfd,buffer,255);
        if (n < 0) 
             error("ERROR reading from socket");
        
//        printf("%s\n",buffer);
    }
    
    close(sockfd);
    return 0;
}

