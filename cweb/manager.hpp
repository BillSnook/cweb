//
//  manager.hpp
//  cweb
//
//  Created by William Snook on 1/12/19.
//  Copyright © 2019 billsnook. All rights reserved.
//

#ifndef manager_hpp
#define manager_hpp

// This class manages interations with the
// Arduino microcontroller over I2C

class Manager {
	
	bool stopLoop;
	long lastAnythingTime;
	long lastStatusTime;
	long lastHeartbeatTime;
	long status;

	long getNowMs();

public:
	void getStatus();
	void setupManager();
	void shutdownManager();
	
	void monitor( int mode );
	
	// test
	int	testRead();
	void testWrite( unsigned char *data );

};

extern Manager	manager;


#endif /* manager_hpp */
