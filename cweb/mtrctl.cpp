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
#include "signals.hpp"

int main(int argc, const char * argv[]) {	// We do not expect any args at this time

//	std::cout << "Hello, World!\n";		// Test
	
	signals_setup();
	
	threader = new Threader();
	threader->setupThreader();
	
//	fprintf(stderr, "\nargc = %d\n", argc);
	if ( argc > 1 ) {	// Should be sender as we pass in host name
		sender = new Sender();
		char buff[32], *buffer = (char *)&buff;
		bcopy( argv[1], buffer, 31);
		sender->setupSender( buff, 5555 );
	} else {
		listener = new Listener();
		listener->setupListener( 5555 );
		listener->doListen();
	}	
	threader->shutdownThreads();

    return 0;
}
