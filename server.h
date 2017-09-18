//
//  server.h
//  cweb
//
//  Created by William Snook on 9/17/17.
//  Copyright © 2017 billsnook. All rights reserved.
//

#ifndef server_h
#define server_h


#define CONNECTION_PORT         5555            // Mostly arbitrary
#define CONNECTION_LISTENER     "zerowpi2.local"     // Default partner


void error(const char *msg);

int servermain( void );


#endif /* server_h */
