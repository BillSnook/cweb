//
//  vl53l0x.hpp
//  cweb
//
//  Created by William Snook on 6/5/19.
//  Copyright © 2019 billsnook. All rights reserved.
//

#ifndef vl53l0x_hpp
#define vl53l0x_hpp


#include <stdlib.h>
#include <stdio.h>
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"


class VL53L0X {
	
	bool 					stopLoop = false;

	VL53L0X_Error			status = VL53L0X_ERROR_NONE;
	VL53L0X_Dev_t 			myDevice;
	VL53L0X_Dev_t 			*pMyDevice = &myDevice;
	VL53L0X_Version_t		version;
	VL53L0X_Version_t		*pVersion   = &version;

public:
	void setupVL53L0X( int i2cFile );
	void shutdownVL53L0X();
	
	void measure();
	void measureRun();
	void measureStop();

	void print_pal_error(VL53L0X_Error Status);
	VL53L0X_Error WaitMeasurementDataReady(VL53L0X_DEV Dev);
	VL53L0X_Error WaitStopCompleted(VL53L0X_DEV Dev);

	VL53L0X_Error rangingTest(VL53L0X_Dev_t *pMyDevice);
};

extern VL53L0X	vl53l0x;


#endif /* vl53l0x_hpp */
