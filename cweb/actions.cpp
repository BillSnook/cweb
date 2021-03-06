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
//#include "vl53l0x/Api/core/inc/vl53l0x_api.h"
//#include "vl53l0x/VL53L0X_rasp/platform/inc/vl53l0x_platform.h"

//#include "vl53l0x.hpp"


#define VERSION_REQUIRED_MAJOR 1
#define VERSION_REQUIRED_MINOR 0
#define VERSION_REQUIRED_BUILD 1



extern Hardware		hardware;

Actor				actor;

/* */
void Actor::print_pal_error(VL53L0X_Error Status){
    if ( Status != 0 ) {
        char buf[VL53L0X_MAX_STRING_LENGTH];
        VL53L0X_GetPalErrorString(Status, buf);
        printf("API Status: %i : %s\n", Status, buf);
    }
}

//void print_range_status(VL53L0X_RangingMeasurementData_t* pRangingMeasurementData){
//    char buf[VL53L0X_MAX_STRING_LENGTH];
//    uint8_t RangeStatus;
//
//    // New Range Status: data is valid when pRangingMeasurementData->RangeStatus = 0
//    RangeStatus = pRangingMeasurementData->RangeStatus;
//
//    VL53L0X_GetRangeStatusString(RangeStatus, buf);
//    printf("Range Status: %i : %s\n", RangeStatus, buf);
//
//}

void Actor::setupActor() {
    
    syslog(LOG_NOTICE, "In setupActor" );

    setupTest();
}

void Actor::shutdownActor() {
    
    syslog(LOG_NOTICE, "In shutdownActor" );
    
    shutdownTest();
}

VL53L0X_Error Actor::setupTest() {
    VL53L0X_DeviceInfo_t                DeviceInfo;

    int32_t status_int;
    char ifc[] = "/dev/i2c-1";
    char *interface = (char *)&ifc;

    // Initialize Comms
    pMyDevice->I2cDevAddr      = 0x29;

    pMyDevice->fd = VL53L0X_i2c_init(interface, pMyDevice->I2cDevAddr); //choose between i2c-0 and i2c-1; On the raspberry pi zero, i2c-1 are pins 2 and 3
    if (MyDevice.fd<0) {
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        printf ("Failed to init\n");
    }

    //  Get the version of the VL53L0X API running in the firmware
    if (Status == VL53L0X_ERROR_NONE) {
        status_int = VL53L0X_GetVersion(pVersion);
        if (status_int != 0)
            Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }

    //  Verify the version of the VL53L0X API running in the firmware
    if (Status == VL53L0X_ERROR_NONE) {
        if ( pVersion->major != VERSION_REQUIRED_MAJOR ||
            pVersion->minor != VERSION_REQUIRED_MINOR
//           ||
//            pVersion->build != VERSION_REQUIRED_BUILD
           ) {
            printf("VL53L0X API Version Error: Your firmware has %d.%d.%d (revision %d). This example requires %d.%d.%d.\n",
                pVersion->major, pVersion->minor, pVersion->build, pVersion->revision,
                VERSION_REQUIRED_MAJOR, VERSION_REQUIRED_MINOR, VERSION_REQUIRED_BUILD);
        }
    }

    // End of implementation specific
    if (Status == VL53L0X_ERROR_NONE) {
        printf ("Call of VL53L0X_DataInit\n");
        Status = VL53L0X_DataInit(&MyDevice); // Data initialization
        print_pal_error(Status);
    }
    
    if (Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_GetDeviceInfo(&MyDevice, &DeviceInfo);
    }
    
    if (Status == VL53L0X_ERROR_NONE) {
        printf("VL53L0X_GetDeviceInfo:\n");
        printf("Device Name : %s\n", DeviceInfo.Name);
        printf("Device Type : %s\n", DeviceInfo.Type);
        printf("Device ID : %s\n", DeviceInfo.ProductId);
        printf("ProductRevisionMajor : %d\n", DeviceInfo.ProductRevisionMajor);
        printf("ProductRevisionMinor : %d\n", DeviceInfo.ProductRevisionMinor);

        if ((DeviceInfo.ProductRevisionMajor != 1) && (DeviceInfo.ProductRevisionMinor != 0)) {
            printf("Error expected cut 1.0.x but found %d.%d\n",
                    DeviceInfo.ProductRevisionMajor, DeviceInfo.ProductRevisionMinor);
            Status = VL53L0X_ERROR_NOT_SUPPORTED;
        }
    }
    return Status;
}

void Actor::shutdownTest() {
    //  Disconnect comms - part of VL53L0X_platform.c
//    printf ("Close Comms\n");
    VL53L0X_i2c_close();

//    print_pal_error(Status);
}

void Actor::stop() {
    
    syslog(LOG_NOTICE, "In stop" );
    
//    hardware.allStop();
}


VL53L0X_Error Actor::WaitMeasurementDataReady(VL53L0X_DEV Dev) {
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

VL53L0X_Error Actor::WaitStopCompleted(VL53L0X_DEV Dev) {
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


VL53L0X_Error Actor::rangeSetup(VL53L0X_Dev_t *pMyDevice) {
    
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint32_t refSpadCount;
    uint8_t isApertureSpads;
    uint8_t VhvSettings;
    uint8_t PhaseCal;

    if (Status == VL53L0X_ERROR_NONE) {
//        printf ("Call of VL53L0X_StaticInit\n");
        Status = VL53L0X_StaticInit(pMyDevice); // Device Initialization
        // StaticInit will set interrupt by default
        print_pal_error(Status);
    }
    if (Status == VL53L0X_ERROR_NONE) {
//        printf ("Call of VL53L0X_PerformRefCalibration\n");
        Status = VL53L0X_PerformRefCalibration(pMyDevice,
                &VhvSettings, &PhaseCal); // Device Initialization
        print_pal_error(Status);
    }

    if (Status == VL53L0X_ERROR_NONE) {
//        printf ("Call of VL53L0X_PerformRefSpadManagement\n");
        Status = VL53L0X_PerformRefSpadManagement(pMyDevice,
                &refSpadCount, &isApertureSpads); // Device Initialization
        print_pal_error(Status);
    }

    if (Status == VL53L0X_ERROR_NONE) {

//        printf ("Call of VL53L0X_SetDeviceMode\n");
        Status = VL53L0X_SetDeviceMode(pMyDevice, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING); // Setup in single ranging mode
        print_pal_error(Status);
    }
    
    return Status;
}

VL53L0X_Error Actor::rangeRun(VL53L0X_Dev_t *pMyDevice, uint32_t no_of_measurements, uint32_t  delay_ms) {
    
    VL53L0X_RangingMeasurementData_t    RangingMeasurementData;
    VL53L0X_RangingMeasurementData_t   *pRangingMeasurementData    = &RangingMeasurementData;

    if (Status == VL53L0X_ERROR_NONE) {
        printf ("StartMeasurement\n");
        Status = VL53L0X_StartMeasurement(pMyDevice);
        print_pal_error(Status);
    }

    if (Status == VL53L0X_ERROR_NONE) {
        uint32_t measurement;

        uint16_t* pResults = (uint16_t*)malloc(sizeof(uint16_t) * no_of_measurements);

        for(measurement=0; measurement<no_of_measurements; measurement++) {

            Status = WaitMeasurementDataReady(pMyDevice);

//            printf( "In loop measurement" );
            if (Status == VL53L0X_ERROR_NONE) {
                Status = VL53L0X_GetRangingMeasurementData(pMyDevice, pRangingMeasurementData);

                *(pResults + measurement) = pRangingMeasurementData->RangeMilliMeter;
                printf("%d: %d cm\n", measurement, pRangingMeasurementData->RangeMilliMeter / 10);

                // Clear the interrupt
                VL53L0X_ClearInterruptMask(pMyDevice, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
                // VL53L0X_PollingDelay(pMyDevice);
            } else {
                break;
            }
            usleep( delay_ms * 1000 );
        }

//        if (Status == VL53L0X_ERROR_NONE) {
//            for(measurement=0; measurement<no_of_measurements; measurement++) {
//                printf("measurement %d: %d\n", measurement, *(pResults + measurement));
//            }
//        }

        free(pResults);
    }

    
    if (Status == VL53L0X_ERROR_NONE) {
//        printf ("Call of VL53L0X_StopMeasurement\n");
        Status = VL53L0X_StopMeasurement(pMyDevice);
    }

    return Status;
}

VL53L0X_Error Actor::rangeClose(VL53L0X_Dev_t *pMyDevice) {
    
    if (Status == VL53L0X_ERROR_NONE) {
//        printf ("Wait Stop to be competed\n");
        Status = WaitStopCompleted(pMyDevice);
    }

    if (Status == VL53L0X_ERROR_NONE)
        Status = VL53L0X_ClearInterruptMask(pMyDevice,
                                            VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);

    return Status;
}

/* */

void Actor::mainTest(uint32_t no_of_measurements, uint32_t delay_ms) {

    if (Status == VL53L0X_ERROR_NONE) {
        Status = rangingTest(pMyDevice, no_of_measurements, delay_ms);
    }

    print_pal_error(Status);
    
    // Implementation specific

}

VL53L0X_Error Actor::rangingTest(VL53L0X_Dev_t *pMyDevice, uint32_t no_of_measurements, uint32_t delay_ms ) {

    rangeSetup( pMyDevice );
    rangeRun( pMyDevice, no_of_measurements, delay_ms );
    rangeClose( pMyDevice );

    return Status;
}

void Actor::runHunt() {
    
    syslog(LOG_NOTICE, "In runHunt" );
    
    //    hardware.scanPing();
    
    hardware.scanUntilFound( WideScan );
    hardware.turnAndFollow( MediumDistance );
}
