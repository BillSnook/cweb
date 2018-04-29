//
//  signals.hpp
//  cweb
//
//  Created by William Snook on 4/15/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef signals_hpp
#define signals_hpp

//#include <stdio.h>
//#include <unistd.h>


void signals_setup();

void sig_handler(int signum);

void leaveGracefully( int signum );

#endif /* signals_hpp */
