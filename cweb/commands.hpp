//
//  commands.hpp
//  cweb
//
//  Created by William Snook on 4/21/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef commands_hpp
#define commands_hpp


class Commander {
	
public:
	void setupCommander();
	void serviceCommand( char *command, int socket ) ;

};

extern Commander	commander;

#endif /* commands_hpp */
