//
//  mtrctl.cpp
//  mtrctl
//
//  Created by William Snook on 4/8/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include <iostream>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mtrctl.hpp"
#include "common.hpp"
#include "signals.hpp"

#include "sender.hpp"
#include "listen.hpp"


Listener	*listener;
Sender		*sender;

int main(int argc, const char * argv[]) {

//	std::cout << "Hello, World!\n";			// Test
	signals_setup();
	
	threader = Threader();
	threader.setupThreader();
	
//	fprintf(stderr, "\nargc = %d\n", argc);
	if ( argc > 1 ) {	// Should be sender as we pass in host name
		sender = new Sender();
		char buff[32], *buffer = (char *)&buff;
		bcopy( argv[1], buffer, 31);
		sender->setupSender( buff, PORT );
	} else {
		listener = new Listener();
		listener->setupListener( PORT );
		listener->doListen();
	}	
	threader.shutdownThreads();

    return 0;
}
