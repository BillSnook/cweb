//
//  vl53l0x.cpp
//  cweb
//
//  Created by William Snook on 6/5/19.
//  Copyright © 2019 billsnook. All rights reserved.
//

#include "vl53l0x.hpp"

#include <stdlib.h>			// malloc
#include <stdio.h>			// sprintf
#include <syslog.h>			// close read write
#include <string.h>			// strcat
#include <unistd.h>


#define VERSION_REQUIRED_MAJOR 1
#define VERSION_REQUIRED_MINOR 0
#define VERSION_REQUIRED_BUILD 2


VL53L0X		vl53l0x;


void VL53L0X::setupVL53L0X( int i2cAddr ) {
	
	syslog(LOG_NOTICE, "In setupVL53L0X" );

	VL53L0X_DeviceInfo_t	deviceInfo;

	char ifc[] = "/dev/i2c-1";
	char *interface = (char *)&ifc;

	// Initialize Comms
	pMyDevice->I2cDevAddr      = i2cAddr;
	
	pMyDevice->fd = VL53L0X_i2c_init(interface, pMyDevice->I2cDevAddr);
	if (pMyDevice->fd < 0) {
		status = VL53L0X_ERROR_CONTROL_INTERFACE;
		printf ("Failed to init\n");
	}
	
	/*
	 *  Get the version of the VL53L0X API running in the firmware
	 */
	
	if(status == VL53L0X_ERROR_NONE)
	{
		int32_t status_int = VL53L0X_GetVersion(pVersion);
		if (status_int != 0)
			status = VL53L0X_ERROR_CONTROL_INTERFACE;
	}
	
	/*
	 *  Verify the version of the VL53L0X API running in the firmrware
	 */
	
	if(status == VL53L0X_ERROR_NONE)
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
	if(status == VL53L0X_ERROR_NONE)
	{
//		printf ("Call of VL53L0X_DataInit\n");
		status = VL53L0X_DataInit(&myDevice); // Data initialization
//		printf ("Called VL53L0X_DataInit, got %d\n", status);
		print_pal_error(status);
	}
	
	if(status == VL53L0X_ERROR_NONE)
	{
		status = VL53L0X_GetDeviceInfo(&myDevice, &deviceInfo);
	}
	if(status == VL53L0X_ERROR_NONE)
	{
		printf("VL53L0X_GetDeviceInfo:\n");
		printf("Device Name : %s\n", deviceInfo.Name);
		printf("Device Type : %s\n", deviceInfo.Type);
		printf("Device ID : %s\n", deviceInfo.ProductId);
		printf("ProductRevisionMajor : %d\n", deviceInfo.ProductRevisionMajor);
		printf("ProductRevisionMinor : %d\n", deviceInfo.ProductRevisionMinor);
		
		if ((deviceInfo.ProductRevisionMinor != 1) && (deviceInfo.ProductRevisionMinor != 1)) {
			printf("Error expected cut 1.1 but found cut %d.%d\n",
				   deviceInfo.ProductRevisionMajor, deviceInfo.ProductRevisionMinor);
			status = VL53L0X_ERROR_NOT_SUPPORTED;
		}
	}
	if(status == VL53L0X_ERROR_NONE) {
		isSetup = true;
	}
}

void VL53L0X::shutdownVL53L0X() {
	
	syslog(LOG_NOTICE, "In shutdownVL53L0X" );

	VL53L0X_i2c_close();
}

void VL53L0X::measure() {
	
	syslog(LOG_NOTICE, "In VL53L0X::measure" );
	
	if(status == VL53L0X_ERROR_NONE) {
		status = rangingTest( pMyDevice );
	}
	
	print_pal_error(status);
	
}

void VL53L0X::measureRun() {
	
	syslog(LOG_NOTICE, "In VL53L0X::measureRun" );
	
	stopLoop = false;
//	if(status == VL53L0X_ERROR_NONE) {
		status = rangingTest( pMyDevice );
//	}
	
	print_pal_error(status);
	
}

void VL53L0X::measureStop() {
	
	syslog(LOG_NOTICE, "In VL53L0X::measureStop" );
	
	stopLoop = true;
	print_pal_error(status);
	
}


void VL53L0X::print_pal_error(VL53L0X_Error Status){
	char buf[VL53L0X_MAX_STRING_LENGTH];
	VL53L0X_GetPalErrorString(Status, buf);
	printf("API Status: %i : %s\n", Status, buf);
}

//void print_range_status(VL53L0X_RangingMeasurementData_t* pRangingMeasurementData){
//	char buf[VL53L0X_MAX_STRING_LENGTH];
//	uint8_t RangeStatus;
//
//	/*
//	 * New Range Status: data is valid when pRangingMeasurementData->RangeStatus = 0
//	 */
//
//	RangeStatus = pRangingMeasurementData->RangeStatus;
//
//	VL53L0X_GetRangeStatusString(RangeStatus, buf);
//	printf("Range Status: %i : %s\n", RangeStatus, buf);
//
//}


VL53L0X_Error VL53L0X::WaitMeasurementDataReady(VL53L0X_DEV Dev) {
//	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	uint8_t NewDatReady=0;
	uint32_t LoopNb;
	
	// Wait until it finished
	// use timeout to avoid deadlock
	if (status == VL53L0X_ERROR_NONE) {
		LoopNb = 0;
		do {
			status = VL53L0X_GetMeasurementDataReady(Dev, &NewDatReady);
			if ((NewDatReady == 0x01) || status != VL53L0X_ERROR_NONE) {
				break;
			}
			LoopNb = LoopNb + 1;
			VL53L0X_PollingDelay(Dev);
		} while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);
		
		if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP) {
			status = VL53L0X_ERROR_TIME_OUT;
		}
	}
	
	return status;
}

VL53L0X_Error VL53L0X::WaitStopCompleted(VL53L0X_DEV Dev) {
//	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	uint32_t StopCompleted=0;
	uint32_t LoopNb;
	
	// Wait until it finished
	// use timeout to avoid deadlock
	if (status == VL53L0X_ERROR_NONE) {
		LoopNb = 0;
		do {
			status = VL53L0X_GetStopCompletedStatus(Dev, &StopCompleted);
			if ((StopCompleted == 0x00) || status != VL53L0X_ERROR_NONE) {
				break;
			}
			LoopNb = LoopNb + 1;
			VL53L0X_PollingDelay(Dev);
		} while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);	// Nominally 2000
		
		if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP) {
			status = VL53L0X_ERROR_TIME_OUT;
		}
		
	}
	
	return status;
}


VL53L0X_Error VL53L0X::rangingTest(VL53L0X_Dev_t *pMyDevice)
{
	VL53L0X_RangingMeasurementData_t    RangingMeasurementData;
	VL53L0X_RangingMeasurementData_t   *pRangingMeasurementData    = &RangingMeasurementData;
//	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	uint32_t refSpadCount;
	uint8_t isApertureSpads;
	uint8_t VhvSettings;
	uint8_t PhaseCal;
	
	if(status == VL53L0X_ERROR_NONE)
	{
		printf ("Call of VL53L0X_StaticInit\n");
		status = VL53L0X_StaticInit(pMyDevice); // Device Initialization
		// StaticInit will set interrupt by default
		print_pal_error(status);
	}
	if(status == VL53L0X_ERROR_NONE)
	{
		printf ("Call of VL53L0X_PerformRefCalibration\n");
		status = VL53L0X_PerformRefCalibration(pMyDevice,
											   &VhvSettings, &PhaseCal); // Device Initialization
		print_pal_error(status);
	}
	
	if(status == VL53L0X_ERROR_NONE)
	{
		printf ("Call of VL53L0X_PerformRefSpadManagement\n");
		status = VL53L0X_PerformRefSpadManagement(pMyDevice,
												  &refSpadCount, &isApertureSpads); // Device Initialization
		print_pal_error(status);
	}
	
	if(status == VL53L0X_ERROR_NONE)
	{
		
		printf ("Call of VL53L0X_SetDeviceMode\n");
		status = VL53L0X_SetDeviceMode(pMyDevice, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING); // Setup in single ranging mode
		print_pal_error(status);
	}
	
	if(status == VL53L0X_ERROR_NONE)
	{
		printf ("Call of VL53L0X_StartMeasurement\n");
		status = VL53L0X_StartMeasurement(pMyDevice);
		print_pal_error(status);
	}
	
	if(status == VL53L0X_ERROR_NONE)
	{
		uint32_t measurement;
		uint32_t no_of_measurements = 50;
		
		uint16_t* pResults = (uint16_t*)malloc(sizeof(uint16_t) * no_of_measurements);
		
		for(measurement=0; measurement<no_of_measurements; measurement++)
		{
			
			status = WaitMeasurementDataReady(pMyDevice);
			
			if(status == VL53L0X_ERROR_NONE)
			{
				status = VL53L0X_GetRangingMeasurementData(pMyDevice, pRangingMeasurementData);
				
				*(pResults + measurement) = pRangingMeasurementData->RangeMilliMeter;
				printf("In loop measurement %d: %d\n", measurement, pRangingMeasurementData->RangeMilliMeter);
				
				// Clear the interrupt
				VL53L0X_ClearInterruptMask(pMyDevice, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
				// VL53L0X_PollingDelay(pMyDevice);
			} else {
				break;
			}
			if (stopLoop) {
				break;
			}
		}
		
		if(status == VL53L0X_ERROR_NONE)
		{
			for(measurement=0; measurement<no_of_measurements; measurement++)
			{
				printf("measurement %d: %d\n", measurement, *(pResults + measurement));
			}
		}
		
		free(pResults);
	}
	
	
	if(status == VL53L0X_ERROR_NONE)
	{
		printf ("Call of VL53L0X_StopMeasurement\n");
		status = VL53L0X_StopMeasurement(pMyDevice);
	}
	
	if(status == VL53L0X_ERROR_NONE)
	{
		printf ("Wait Stop to be competed\n");
		status = WaitStopCompleted(pMyDevice);
	}
	
	if(status == VL53L0X_ERROR_NONE)
		status = VL53L0X_ClearInterruptMask(pMyDevice,
											VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
	
	return status;
}
