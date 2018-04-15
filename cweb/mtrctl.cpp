//
//  mtrctl.cpp
//  mtrctl
//
//  Created by William Snook on 4/8/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include <iostream>

#include "mtrctl.hpp"
#include "signals.hpp"
#include "listen.hpp"

int main(int argc, const char * argv[]) {	// We do not expect any args at this time

	std::cout << "Hello, World!\n";		// Test
	
	signals_setup();
//	sleep(10); // This is your chance to press CTRL-C
	
	listener = new Listener();
	listener->setupListener( 5555 );
	
	runLoop = true;
	while ( runLoop ) {
		
		
	}
	
	
    return 0;
}
