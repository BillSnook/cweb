//
//  commands.hpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef commands_hpp
#define commands_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Commander {
	
public:
	void setupCommander();
	void serviceCommand( char *command ) ;

};

extern Commander	commander;

#endif /* commands_hpp */
