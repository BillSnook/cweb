//
//  actions.hpp
//  cweb
//
//  Created by William Snook on 12/8/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef actions_hpp
#define actions_hpp

//#include "vl53l0x.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"

class Actor {
	
public:
	VL53L0X_Error       Status = VL53L0X_ERROR_NONE;
	VL53L0X_Dev_t       MyDevice;
	VL53L0X_Dev_t       *pMyDevice = &MyDevice;
    VL53L0X_Version_t   Version = {0};
	VL53L0X_Version_t   *pVersion   = &Version;

	void print_pal_error(VL53L0X_Error Status);
    
    void setupActor();
    void shutdownActor();

    VL53L0X_Error setupTest();
    void shutdownTest();

    void stop();

    VL53L0X_Error rangeSetup(VL53L0X_Dev_t *pMyDevice);
    VL53L0X_Error rangeRun(VL53L0X_Dev_t *pMyDevice, uint32_t no_of_measurements, uint32_t delay_ms);
    VL53L0X_Error rangeClose(VL53L0X_Dev_t *pMyDevice);

	VL53L0X_Error WaitMeasurementDataReady(VL53L0X_DEV Dev);
	VL53L0X_Error WaitStopCompleted(VL53L0X_DEV Dev);
    
	VL53L0X_Error rangingTest(VL53L0X_Dev_t *pMyDevice, uint32_t no_of_measurements, uint32_t delay_ms);
    void mainTest(uint32_t no_of_measurements, uint32_t delay_ms);

    void runHunt();
};
#endif /* actions_hpp */
