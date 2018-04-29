//
//  sender.hpp
//  cweb
//
//  Created by William Snook on 4/15/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef sender_hpp
#define sender_hpp

#include <stdio.h>

#include <netdb.h>


class Sender {
	
public:
	void setupSender( char *hostName, int portNo );
};

#endif /* sender_hpp */
