//
//  actions.cpp
//  cweb
//
//  Created by William Snook on 12/8/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "actions.hpp"
#include "hardware.hpp"

#include <stdlib.h>
#include <stdio.h>
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"

#include "vl53l0x.hpp"


#define VERSION_REQUIRED_MAJOR 1
#define VERSION_REQUIRED_MINOR 0
#define VERSION_REQUIRED_BUILD 1



extern Hardware		hardware;

Actor				actor;

/* */
void print_pal_error(VL53L0X_Error Status){
    char buf[VL53L0X_MAX_STRING_LENGTH];
    VL53L0X_GetPalErrorString(Status, buf);
    printf("API Status: %i : %s\n", Status, buf);
}

void print_range_status(VL53L0X_RangingMeasurementData_t* pRangingMeasurementData){
    char buf[VL53L0X_MAX_STRING_LENGTH];
    uint8_t RangeStatus;

    // New Range Status: data is valid when pRangingMeasurementData->RangeStatus = 0
    RangeStatus = pRangingMeasurementData->RangeStatus;

    VL53L0X_GetRangeStatusString(RangeStatus, buf);
    printf("Range Status: %i : %s\n", RangeStatus, buf);

}


VL53L0X_Error WaitMeasurementDataReady(VL53L0X_DEV Dev) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint8_t NewDatReady=0;
    uint32_t LoopNb;

    // Wait until it finished
    // use timeout to avoid deadlock
    if (Status == VL53L0X_ERROR_NONE) {
        LoopNb = 0;
        do {
            Status = VL53L0X_GetMeasurementDataReady(Dev, &NewDatReady);
            if ((NewDatReady == 0x01) || Status != VL53L0X_ERROR_NONE) {
                break;
            }
            LoopNb = LoopNb + 1;
            VL53L0X_PollingDelay(Dev);
        } while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);

        if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP) {
            Status = VL53L0X_ERROR_TIME_OUT;
        }
    }

    return Status;
}

VL53L0X_Error WaitStopCompleted(VL53L0X_DEV Dev) {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint32_t StopCompleted=0;
    uint32_t LoopNb;

    // Wait until it finished
    // use timeout to avoid deadlock
    if (Status == VL53L0X_ERROR_NONE) {
        LoopNb = 0;
        do {
            Status = VL53L0X_GetStopCompletedStatus(Dev, &StopCompleted);
            if ((StopCompleted == 0x00) || Status != VL53L0X_ERROR_NONE) {
                break;
            }
            LoopNb = LoopNb + 1;
            VL53L0X_PollingDelay(Dev);
        } while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);

        if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP) {
            Status = VL53L0X_ERROR_TIME_OUT;
        }
	
    }

    return Status;
}


VL53L0X_Error rangingTest(VL53L0X_Dev_t *pMyDevice)
{
    VL53L0X_RangingMeasurementData_t    RangingMeasurementData;
    VL53L0X_RangingMeasurementData_t   *pRangingMeasurementData    = &RangingMeasurementData;
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint32_t refSpadCount;
    uint8_t isApertureSpads;
    uint8_t VhvSettings;
    uint8_t PhaseCal;

    if(Status == VL53L0X_ERROR_NONE)
    {
        printf ("Call of VL53L0X_StaticInit\n");
        Status = VL53L0X_StaticInit(pMyDevice); // Device Initialization
        // StaticInit will set interrupt by default
        print_pal_error(Status);
    }
    if(Status == VL53L0X_ERROR_NONE)
    {
        printf ("Call of VL53L0X_PerformRefCalibration\n");
        Status = VL53L0X_PerformRefCalibration(pMyDevice,
        		&VhvSettings, &PhaseCal); // Device Initialization
        print_pal_error(Status);
    }

    if(Status == VL53L0X_ERROR_NONE)
    {
        printf ("Call of VL53L0X_PerformRefSpadManagement\n");
        Status = VL53L0X_PerformRefSpadManagement(pMyDevice,
        		&refSpadCount, &isApertureSpads); // Device Initialization
        print_pal_error(Status);
    }

    if(Status == VL53L0X_ERROR_NONE)
    {

        printf ("Call of VL53L0X_SetDeviceMode\n");
        Status = VL53L0X_SetDeviceMode(pMyDevice, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING); // Setup in single ranging mode
        print_pal_error(Status);
    }
	
    if(Status == VL53L0X_ERROR_NONE)
    {
		printf ("Call of VL53L0X_StartMeasurement\n");
		Status = VL53L0X_StartMeasurement(pMyDevice);
		print_pal_error(Status);
    }

    if(Status == VL53L0X_ERROR_NONE)
    {
        uint32_t measurement;
        uint32_t no_of_measurements = 50;

        uint16_t* pResults = (uint16_t*)malloc(sizeof(uint16_t) * no_of_measurements);

        for(measurement=0; measurement<no_of_measurements; measurement++)
        {

            Status = WaitMeasurementDataReady(pMyDevice);

            if(Status == VL53L0X_ERROR_NONE)
            {
                Status = VL53L0X_GetRangingMeasurementData(pMyDevice, pRangingMeasurementData);

                *(pResults + measurement) = pRangingMeasurementData->RangeMilliMeter;
                printf("In loop measurement %d: %d\n", measurement, pRangingMeasurementData->RangeMilliMeter);

                // Clear the interrupt
                VL53L0X_ClearInterruptMask(pMyDevice, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
                // VL53L0X_PollingDelay(pMyDevice);
            } else {
                break;
            }
        }

        if(Status == VL53L0X_ERROR_NONE)
        {
            for(measurement=0; measurement<no_of_measurements; measurement++)
            {
                printf("measurement %d: %d\n", measurement, *(pResults + measurement));
            }
        }

        free(pResults);
    }

	
    if(Status == VL53L0X_ERROR_NONE)
    {
        printf ("Call of VL53L0X_StopMeasurement\n");
        Status = VL53L0X_StopMeasurement(pMyDevice);
    }

    if(Status == VL53L0X_ERROR_NONE)
    {
        printf ("Wait Stop to be competed\n");
        Status = WaitStopCompleted(pMyDevice);
    }

    if(Status == VL53L0X_ERROR_NONE)
	Status = VL53L0X_ClearInterruptMask(pMyDevice,
		VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);

    return Status;
}

int mainTest() {
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    VL53L0X_Dev_t MyDevice;
    VL53L0X_Dev_t *pMyDevice = &MyDevice;
    VL53L0X_Version_t                   Version;
    VL53L0X_Version_t                  *pVersion   = &Version;
    VL53L0X_DeviceInfo_t                DeviceInfo;

    int32_t status_int;
    char ifc[] = "/dev/i2c-1";
    char *interface = (char *)&ifc;

    printf ("VL53L0X PAL Continuous Ranging example\n\n");

    // Initialize Comms
    pMyDevice->I2cDevAddr      = 0x29;

    pMyDevice->fd = VL53L0X_i2c_init(interface, pMyDevice->I2cDevAddr); //choose between i2c-0 and i2c-1; On the raspberry pi zero, i2c-1 are pins 2 and 3
    if (MyDevice.fd<0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        printf ("Failed to init\n");
    }

    //  Get the version of the VL53L0X API running in the firmware
    if(Status == VL53L0X_ERROR_NONE)
    {
        status_int = VL53L0X_GetVersion(pVersion);
        if (status_int != 0)
            Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }

    //  Verify the version of the VL53L0X API running in the firmrware
    if(Status == VL53L0X_ERROR_NONE)
    {
        if( pVersion->major != VERSION_REQUIRED_MAJOR ||
            pVersion->minor != VERSION_REQUIRED_MINOR ||
            pVersion->build != VERSION_REQUIRED_BUILD )
        {
            printf("VL53L0X API Version Error: Your firmware has %d.%d.%d (revision %d). This example requires %d.%d.%d.\n",
                pVersion->major, pVersion->minor, pVersion->build, pVersion->revision,
                VERSION_REQUIRED_MAJOR, VERSION_REQUIRED_MINOR, VERSION_REQUIRED_BUILD);
        }
    }

    // End of implementation specific
    if(Status == VL53L0X_ERROR_NONE)
    {
        printf ("Call of VL53L0X_DataInit\n");
        Status = VL53L0X_DataInit(&MyDevice); // Data initialization
        print_pal_error(Status);
    }
	
    if(Status == VL53L0X_ERROR_NONE)
    {
        Status = VL53L0X_GetDeviceInfo(&MyDevice, &DeviceInfo);
    }
    if(Status == VL53L0X_ERROR_NONE)
    {
        printf("VL53L0X_GetDeviceInfo:\n");
        printf("Device Name : %s\n", DeviceInfo.Name);
        printf("Device Type : %s\n", DeviceInfo.Type);
        printf("Device ID : %s\n", DeviceInfo.ProductId);
        printf("ProductRevisionMajor : %d\n", DeviceInfo.ProductRevisionMajor);
        printf("ProductRevisionMinor : %d\n", DeviceInfo.ProductRevisionMinor);

        if ((DeviceInfo.ProductRevisionMinor != 1) && (DeviceInfo.ProductRevisionMinor != 1)) {
        	printf("Error expected cut 1.1 but found cut %d.%d\n",
        			DeviceInfo.ProductRevisionMajor, DeviceInfo.ProductRevisionMinor);
        	Status = VL53L0X_ERROR_NOT_SUPPORTED;
        }
    }

    if(Status == VL53L0X_ERROR_NONE)
    {
        Status = rangingTest(pMyDevice);
    }

    print_pal_error(Status);
	
    // Implementation specific

    //  Disconnect comms - part of VL53L0X_platform.c
    printf ("Close Comms\n");
    VL53L0X_i2c_close();


    print_pal_error(Status);
	
    return (0);
}
/* */

void Actor::setupActor() {
	
	syslog(LOG_NOTICE, "In setupActor" );

	mainTest();
}

void Actor::shutdownActor() {
	
	syslog(LOG_NOTICE, "In shutdownActor" );
}

void Actor::stop() {
	
	syslog(LOG_NOTICE, "In stop" );
	
	hardware.allStop();
}

void Actor::runHunt() {
	
	syslog(LOG_NOTICE, "In runHunt" );
	
	//	hardware.scanPing();
	
	hardware.scanUntilFound( WideScan );
	hardware.turnAndFollow( MediumDistance );
}

void Actor::doTest() {
	
	syslog(LOG_NOTICE, "In mainTest" );
	
	mainTest();
}



/*
 
 mtrctllog[1267]: Started mtrctl as user - syslog + LOG_PERROR
 mtrctllog[1267]: In setupCommander
 mtrctllog[1267]: I2C address: 0x6F, PWM freq: 50
 mtrctllog[1267]: In setupHardware
 mtrctllog[1267]: readData failed opening file
 mtrctllog[1267]: Failed reading speed array from file; making default one
 mtrctllog[1267]: In setupTaskMaster
 mtrctllog[1267]: In setupActor
 VL53L0X PAL Continuous Ranging example
 
 VL53L0X API Version Error: Your firmware has 1.0.2 (revision 4823). This example requires 1.0.1.
 Call of VL53L0X_DataInit
 mtrctllog[1267]: Setting I2C standard mode
 mtrctllog[1267]: Setting up calibration
 mtrctllog[1267]: Getting device params
 mtrctllog[1267]: Sigma estimating variable
 mtrctllog[1267]: Use internal defualt settings
 mtrctllog[1267]: Setting stop variable
 mtrctllog[1267]: Checks
 API Status: 0 : No Error
 VL53L0X_GetDeviceInfo:
 Device Name : VL53L0X ES1 or later
 Device Type : VL53L0X
 Device ID : VL53L0CBV0DH/1$1
 ProductRevisionMajor : 1
 ProductRevisionMinor : 1
 Call of VL53L0X_StaticInit
 API Status: 0 : No Error
 Call of VL53L0X_PerformRefCalibration
 API Status: 0 : No Error
 Call of VL53L0X_PerformRefSpadManagement
 API Status: 0 : No Error
 Call of VL53L0X_SetDeviceMode
 API Status: 0 : No Error
 Call of VL53L0X_StartMeasurement
 API Status: 0 : No Error
 In loop measurement 0: 8190

 ...
 
 measurement 4999: 8190
 Call of VL53L0X_StopMeasurement
 Wait Stop to be competed
 API Status: 0 : No Error
 Close Comms
 VL53L0X_i2c_close
 API Status: 0 : No Error
 mtrctllog[1267]: In setupManager
 mtrctllog[1267]: In setupMinion, opened I2C bus on port 8
 mtrctllog[1267]: In setupVL53L0X
 mtrctllog[1267]: Setting I2C standard mode
 ?mtrctllog[1267]: Setting up calibration
 mtrctllog[1267]: Getting device params
 ?
 Failed to read from the i2c bus.
 mtrctllog[1267]: Sigma estimating variable
 mtrctllog[1267]: Use internal defualt settings
 ???
 mtrctllog[1267]: Setting stop variable
 ??mtrctllog[1267]: Checks
 API Status: -20 : Control Interface Error
 mtrctllog[1267]: In SitMap::setupSitMap(), before distanceMap, size: 19
 mtrctllog[1267]: In SitMap::setupSitMap(), after distanceMap
 mtrctllog[1267]: mtrctl argc = 1
 mtrctllog[1267]: Ready to service queue
 mtrctllog[1267]: In shutdownThreads
 mtrctllog[1267]: In shutdownVL53L0X
 VL53L0X_i2c_close
 mtrctllog[1267]: In resetMinion
 mtrctllog[1267]: In shutdownManager
 mtrctllog[1267]: In shutdownTaskMaster
 mtrctllog[1267]: In shutdownActor
 mtrctllog[1267]: In shutdownCommander
 mtrctllog[1267]: In resetHardware
 pi@Develop00:~/code/c/cweb/cweb $

 
 Versus
 
 
 mtrctllog[1210]: Started mtrctl as user - syslog + LOG_PERROR
 mtrctllog[1210]: In setupCommander
 mtrctllog[1210]: In setupTaskMaster
 mtrctllog[1210]: In setupActor
 mtrctllog[1210]: In setupManager
 mtrctllog[1210]: In setupVL53L0X
 mtrctllog[1210]: Setting I2C standard mode
 ?mtrctllog[1210]: Setting up calibration
 mtrctllog[1210]: Getting device params
 ?
 Failed to read from the i2c bus.
 mtrctllog[1210]: Sigma estimating variable
 mtrctllog[1210]: Use internal defualt settings
 ???
 mtrctllog[1210]: Setting stop variable
 ??mtrctllog[1210]: Checks
 API Status: -20 : Control Interface Error
 mtrctllog[1210]: In SitMap::setupSitMap(), before distanceMap, size: 19
 mtrctllog[1210]: In SitMap::setupSitMap(), after distanceMap
 mtrctllog[1210]: mtrctl argc = 1
 mtrctllog[1210]: Ready to service queue
 mtrctllog[1210]: In runThread with managerThread, thread count 1
 mtrctllog[1210]: In Manager::monitor, should only start once
 mtrctllog[1210]: In Manager::setStatus()
 mtrctllog[1210]: In runThread with listenThread, thread count 2
 mtrctllog[1210]: Success binding to socket port 5555 on 0.0.0.0
 mtrctllog[1210]: In acceptConnections, listening
 
 ^Cmtrctllog[1210]: Received INT signal (ctrl-C), exiting now.
 mtrctllog[1210]: In shutdownThreads
 mtrctllog[1210]: In shutdownVL53L0X
 VL53L0X_i2c_close
 mtrctllog[1210]: In shutdownManager
 mtrctllog[1210]: In shutdownTaskMaster
 mtrctllog[1210]: In shutdownActor
 mtrctllog[1210]: In shutdownCommander

 
 
*/
