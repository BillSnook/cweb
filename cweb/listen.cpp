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


#define bufferSize	            256

#define useDatagramProtocol     true

extern Threader		threader;


void Listener::acceptConnections( int rcvPortNo) {	// Create and bind socket for listening
	
	doListenerLoop = false;
    if ( useDatagramProtocol ) {
        listenSockfd = socket( AF_INET, SOCK_DGRAM, 0 );   // SOCK_DGRAM for UDP
    } else {
        listenSockfd = socket( AF_INET, SOCK_STREAM, 0 );   // SOCK_DGRAM for UDP
    }
	if ( listenSockfd < 0 ) {
		syslog(LOG_ERR, "ERROR opening socket" );
		return;
	}
    struct sockaddr_in    serv_addr;
	bzero( (char *)&serv_addr, sizeof( serv_addr ) );
	portno = rcvPortNo;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( portno );
	if ( bind( listenSockfd, (struct sockaddr *)&serv_addr, sizeof( serv_addr) ) < 0) {
		syslog(LOG_ERR, "ERROR on binding"  );
		return;
	}

    if ( useDatagramProtocol ) {
        syslog(LOG_NOTICE, "Success binding to UDP socket %d, port %d, on %s", listenSockfd, portno, inet_ntoa(serv_addr.sin_addr));
        threader.queueThread( serverThread, inet_ntoa(serv_addr.sin_addr), listenSockfd );
    } else {
        syslog(LOG_NOTICE, "Success binding to TCP socket port %d on %s", portno, inet_ntoa(serv_addr.sin_addr) );
        doListenerLoop = true;
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof( cli_addr );
        while ( doListenerLoop ) {
            syslog(LOG_NOTICE, "In acceptConnections, listening on socket %d", listenSockfd);
            listen(  listenSockfd, 5 );
            int connectionSockfd = accept( listenSockfd, (struct sockaddr *)&cli_addr, &clilen);
            syslog(LOG_NOTICE, "Listen socket %d accepted a connection on socket %d", listenSockfd, connectionSockfd);
            if ( connectionSockfd < 0 ) {
                syslog(LOG_ERR, "ERROR on accept" );
                break;
            }
            syslog(LOG_NOTICE, "Accepted connection, clientAddr: %s", inet_ntoa( cli_addr.sin_addr ) );
            
            threader.queueThread( serverThread, inet_ntoa( cli_addr.sin_addr ), connectionSockfd );
            
//          doListenerLoop = false; // Do once for testing
        }
        close( listenSockfd );
        syslog(LOG_NOTICE, "In acceptConnections at exit" );
    }
}

void Listener::serviceConnection( int connectionSockfd, char *inet_address ) {
	
	bool	localLoop = true;
	while ( localLoop ) {
        long    n;
		char	*buffer = (char *)valloc( bufferSize );
		bzero( buffer, bufferSize );
//		syslog(LOG_NOTICE, "In serviceConnection waiting for data...");
        if ( useDatagramProtocol ) {
            // recv
//            struct sockaddr_in serverStorage;
//            socklen_t addr_size = sizeof( serverStorage );
            n = recvfrom(connectionSockfd, buffer, bufferSize, 0, (struct sockaddr *)&serverStorage, &addr_size);
            syslog(LOG_NOTICE, "In datagram serviceConnection received data from clientAddr: %s", inet_ntoa( serverStorage.sin_addr ) );
            addrno = ntohl(serverStorage.sin_addr.s_addr);
        } else {
            n = read( connectionSockfd, buffer, bufferSize );    // Blocks waiting for incoming data from WiFi
        }
        if ( n <= 0 ) {
            syslog(LOG_NOTICE, "Connection closed by %s", inet_address );
//          syslog(LOG_ERR, "ERROR reading command from socket" );
            free( buffer );
            break;
        }
        
        syslog(LOG_NOTICE, "Received command: %s", buffer );
        // Now start thread to service command

		threader.queueThread( commandThread, buffer, connectionSockfd );	// Parse and execute command in its own thread with socket in case it needs to respond
		free( buffer );

//		n = write( connectionSockfd, "\nAck\n", 5 );
//		if ( n < 0 ) {
//			syslog(LOG_ERR, "ERROR writing ack to socket" );
//			break;
//		}
	}
	close( connectionSockfd );
//	syslog(LOG_NOTICE, "In serviceConnection at end" );
}

void Listener::writeBack( char *msg, int socket ) {
    long n;
    if ( useDatagramProtocol ) {
//        struct sockaddr_in serv_addr;
//        socklen_t addr_size = sizeof( serv_addr );
//        serv_addr.sin_family = AF_INET;
//        serv_addr.sin_addr.s_addr = htonl(addrno);
//        serv_addr.sin_port = htons( portno );
        n = sendto(socket, msg, strlen( msg ), 0, (struct sockaddr *)&serverStorage, addr_size);
        syslog(LOG_ERR, "Sending back to socket %d, addr %s, response length %ld", socket, inet_ntoa(serverStorage.sin_addr), n);
    } else {
        n = write( socket, msg, strlen( msg ) );
    }
    if ( n < 0 ) {
        syslog(LOG_ERR, "ERROR writing back to socket %d", socket );
        return;
    }
    syslog(LOG_NOTICE, "In writeBack sent successfully on socket %d", socket);
}

//void Listener::writeBlock( char *msg, int length, int socket ) {
//	long n = write( socket, msg, length );
//	if ( n < 0 ) {
//		syslog(LOG_ERR, "ERROR writing block to socket" );
//	}
//}
