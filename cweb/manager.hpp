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
	void getStatus();

public:
	void setupManager();
	void shutdownManager();
	
	void monitor( int mode );
};

extern Manager	manager;


#endif /* manager_hpp */