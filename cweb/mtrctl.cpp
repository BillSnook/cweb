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
#include "listen.hpp"

int main(int argc, const char * argv[]) {	// We do not expect any args at this time

	std::cout << "Hello, World!\n";		// Test
	
	signals_setup();
//	sleep(10); // This is your chance to press CTRL-C
	
	fprintf(stderr, "\nargc = %d\n", argc);
	if ( argc > 1 ) {	// Should be sender as we pass in host name
		sender = new Sender();
		char buff[32], *buffer = (char *)&buff;
		bcopy( "Develop31.local", buffer, 16);
		sender->setupSender( buff, 0x2222 );
	} else {
		listener = new Listener();
		listener->setupListener( 0x2222 );
	}
	
	runLoop = true;
	while ( runLoop ) {
		
		
	}
	
	
    return 0;
}
