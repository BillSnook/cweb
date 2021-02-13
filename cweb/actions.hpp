//
//  actions.hpp
//  cweb
//
//  Created by William Snook on 12/8/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef actions_hpp
#define actions_hpp

#include "vl53l0x.hpp"

class Actor {
	
public:
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	VL53L0X_Dev_t MyDevice;
	VL53L0X_Dev_t *pMyDevice = &MyDevice;
	VL53L0X_Version_t                   Version;
	VL53L0X_Version_t                  *pVersion   = &Version;

	void print_pal_error(VL53L0X_Error Status);
	VL53L0X_Error WaitMeasurementDataReady(VL53L0X_DEV Dev);
	VL53L0X_Error WaitStopCompleted(VL53L0X_DEV Dev);
	VL53L0X_Error rangingTest(VL53L0X_Dev_t *pMyDevice, uint32_t no_of_measurements);
	void setupTest();
	void mainTest(uint32_t no_of_measurements);
	void shutdownTest();

	void setupActor();

	void shutdownActor();

	void stop();

	void runHunt();
	
	void doTest();

};
#endif /* actions_hpp */
