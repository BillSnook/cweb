//
//  listen.cpp
//  cweb
//
//  Created by William Snook on 4/15/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <arpa/inet.h>

#include "listen.hpp"
#include "threader.hpp"
#include "commands.hpp"


#define bufferSize	            256

#define useDatagramProtocol     true


extern Threader		threader;
extern Commander    commander;


void Listener::setupListener() {
    
    syslog(LOG_NOTICE, "In setupListener" );
    keepAliveOn = false;
    timeLoop = false;
}

void Listener::shutdownListener() {
    
    syslog(LOG_NOTICE, "In shutdownListener" );
    timeLoop = false;
//    usleep( 100000 );
}

void Listener::acceptConnections( int rcvPortNo) {	// Create and bind socket for listening
	
    if ( useDatagramProtocol ) {
        socketfd = socket( AF_INET, SOCK_DGRAM, 0 );   // SOCK_DGRAM for UDP
    } else {
        socketfd = socket( AF_INET, SOCK_STREAM, 0 );   // SOCK_DGRAM for UDP
    }
	if ( socketfd < 0 ) {
		syslog(LOG_ERR, "ERROR opening socket" );
		return;
	}
    
    struct sockaddr_in    serv_addr;
	bzero( (char *)&serv_addr, sizeof( serv_addr ) );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons( rcvPortNo );
	if ( bind( socketfd, (struct sockaddr *)&serv_addr, sizeof( serv_addr) ) < 0) {
		syslog(LOG_ERR, "ERROR on binding"  );
		return;
	}

    threader.queueThread( keepAliveThread, (int)0, 0 );    // Start keep-alive monitor

    if ( useDatagramProtocol ) {        // Basically do once after binding to start server thread to handle incoming data
        syslog(LOG_NOTICE, "Success binding to UDP socket %d, port %d, on %s", socketfd, rcvPortNo, inet_ntoa(serv_addr.sin_addr));
        threader.queueThread( serverThread, inet_ntoa(serv_addr.sin_addr), socketfd );
    } else {                            // Basically listen forever for a new connection then create a server thread
        bool doListenerLoop = true;
        syslog(LOG_NOTICE, "Success binding to TCP socket port %d on %s", rcvPortNo, inet_ntoa(serv_addr.sin_addr) );
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof( cli_addr );
        while ( doListenerLoop ) {
            syslog(LOG_NOTICE, "In acceptConnections, listening on socket %d", socketfd);
            listen( socketfd, 5 );
            int connectionSockfd = accept( socketfd, (struct sockaddr *)&cli_addr, &clilen);
            syslog(LOG_NOTICE, "Listen socket %d accepted a connection on socket %d", socketfd, connectionSockfd);
            if ( connectionSockfd < 0 ) {
                syslog(LOG_ERR, "ERROR on accept" );
                break;
            }
            syslog(LOG_NOTICE, "Accepted connection, clientAddr: %s", inet_ntoa( cli_addr.sin_addr ) );
            
            threader.queueThread( serverThread, inet_ntoa( cli_addr.sin_addr ), connectionSockfd );
            
//          doListenerLoop = false; // Do once for testing
        }
        close( socketfd );
        syslog(LOG_NOTICE, "In acceptConnections at exit" );
    }
}

void Listener::serviceConnection( int connectionSockfd, char *inet_address ) {
	
    bool    localLoop = true;
    long    n;
	while ( localLoop ) {
        int sockOrAddr = connectionSockfd;
		char	*buffer = (char *)valloc( bufferSize ); // 256 bytes
		bzero( buffer, bufferSize );
//		syslog(LOG_NOTICE, "In serviceConnection waiting for data...");
        if ( useDatagramProtocol ) {
            // recv
            struct sockaddr_in serverStorage;
            socklen_t addr_size = sizeof( serverStorage );
            n = recvfrom(connectionSockfd, buffer, bufferSize, 0, (struct sockaddr *)&serverStorage, &addr_size);
//            syslog(LOG_NOTICE, "In datagram serviceConnection received %ld bytes of data from clientAddr: %s, port %d", n, inet_ntoa( serverStorage.sin_addr ), ntohs(serverStorage.sin_port));
            // WFS Need an addr/port reference vs socketfd here
            int addrno = ntohl(serverStorage.sin_addr.s_addr);
            int portno = ntohs(serverStorage.sin_port);
            sockOrAddr = findMatchOrNewIndex( addrno, portno );
//            syslog(LOG_NOTICE, "serviceConnection sockOrAddr %d", sockOrAddr );
        } else {
            n = read( connectionSockfd, buffer, bufferSize );    // Blocks waiting for incoming data from WiFi
        }
        if ( n <= 0 ) {
            syslog(LOG_NOTICE, "Connection closed by %s", inet_address );
//          syslog(LOG_ERR, "ERROR reading command from socket" );
            free( buffer );
            break;
        }
        
        if ( !keepAliveOn ) {
            syslog(LOG_NOTICE, "Keep-alive enabled" );  // Wake up
            keepAliveOn = true;
            timeLoop = true;
        }
        gettimeofday(&tvLatest, NULL);
        
        char cmd = buffer[0];
//        syslog(LOG_NOTICE, "Received command: %s", buffer );

        if ( cmd < '@' ) {              // Control characters, numbers, and punctuation
            if ( cmd == '?' ) {         // Special keep-alive - do nothing
                // Was sent if no other commmand in 1/2 second which indicates the communication channel is still open
            } else if ( cmd == '#' ) {  // Goodbye command - no further keep alive are to be expected
                keepAliveOn = false;
                timeLoop = false;
                syslog(LOG_NOTICE, "Received goodbye command, #, keep-alive disabled" );
            } else {
                // Real high priority or otherwise needs to have as much thread time as possible
                threader.queueThread( taskThread, buffer, sockOrAddr );    // addr/port reference or socketfd
            }
        } else if ( cmd < 'a' ) {       // Capitalized characters
            // Run real quick command such as setting a pin or pwm value
            commander.serviceCommand( buffer, sockOrAddr );
        } else {                        // Lower case characters
            // Command that may take a while to complete and needs it's own thread
            threader.queueThread( commandThread, buffer, sockOrAddr );    // addr/port reference or socketfd
        }

		free( buffer );
	}
	close( connectionSockfd );
//	syslog(LOG_NOTICE, "In serviceConnection at end" );
}

int Listener::findMatchOrNewIndex( int addr, int port ) {
    
//    syslog(LOG_NOTICE, "input, addr %08X, port %d", addr, port);
    for ( int i = 1; i < AP_SIZE; i++ ) {
        addrPort ap = apArray[i];
        if ( ap.port == 0 ) {   // Blank entry, we have no matches, create new entry
            apArray[i].addr = addr;
            apArray[i].port = port;
            return( i );
        }
        if ( ( ap.addr == addr ) && ( ap.port == port ) ) {
            return( i );
        }
    }
    return 0;   // List is full!
}

void Listener::writeBack( char *msg, int sockOrAddr ) {  // We use an addr/port reference vs socketfd here
    long n;
    if ( useDatagramProtocol ) {
        // Get addr and port from sockOrAddr as addr/port array index
        if ( sockOrAddr == 0 ) {
//            syslog(LOG_ERR, "In writeBack, invalid response entry index");
            return;     // Invalid addr/port index
        }
        struct sockaddr_in serv_addr;
        socklen_t addr_size = sizeof( serv_addr );
        addrPort ap = apArray[sockOrAddr];
//        syslog(LOG_NOTICE, "writeBack index %d, addr %08X, port %d", sockOrAddr, ap.addr, ap.port);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(ap.addr);
        serv_addr.sin_port = htons( ap.port );
        n = sendto(socketfd, msg, strlen( msg ), 0, (struct sockaddr *)&serv_addr, addr_size);
//        syslog(LOG_ERR, "Sending back to addr %s, port %d, response length %ld", inet_ntoa(serv_addr.sin_addr), ap.port, n);
        if ( n < 0 ) {
            syslog(LOG_ERR, "ERROR writing to address %s, port %d", inet_ntoa(serv_addr.sin_addr), ap.port );
            return;
        }
//        syslog(LOG_NOTICE, "In UDP writeBack sent %ld bytes successfully to address %s, port %d", n, inet_ntoa(serv_addr.sin_addr), ap.port);
    } else {
        n = write( sockOrAddr, msg, strlen( msg ) );
        if ( n < 0 ) {
            syslog(LOG_ERR, "ERROR writing back to socket %d", sockOrAddr );
            return;
        }
        syslog(LOG_NOTICE, "In TCP writeBack sent successfully on socket %d", sockOrAddr);
    }
}

bool Listener::testTimedOut() {      // Keep-alive support - true iff too long
    
    struct timeval diffTime, tvNow;
    gettimeofday(&tvNow, NULL);
    diffTime.tv_sec = tvNow.tv_sec - tvLatest.tv_sec;;
    diffTime.tv_usec = tvNow.tv_usec - tvLatest.tv_usec;
    if ( diffTime.tv_usec < 0 ) {
        diffTime.tv_sec -= 1;
        diffTime.tv_usec += 1000000;
    }
    long microSecondTime = ( diffTime.tv_sec * 1000000 ) + diffTime.tv_usec;
    return microSecondTime > 1100000;
}

void Listener::monitor() {      // Intended to run in a thread to monitor keep alive timer

    syslog(LOG_NOTICE, "In Listener monitor, entering loop testing for loss of comm to controller" );
    while ( true ) {
        if ( timeLoop && testTimedOut() ) {
            // WFS test - may want to end this if we go to autonomous mode
            syslog(LOG_NOTICE, "In Listener monitor, lost communication with controller !!!!!!! " );
//            char killAction[] = "?";
//            commander.serviceCommand( (char *)&killAction, 0 ); // Send emergency stop command
        }
        usleep( 100000 );       // 1/10 seconds
    }
//    syslog(LOG_NOTICE, "In Listener monitor, exiting loop testing for loss of comm to controller" );
}

