//
//  hardware.hpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef hardware_hpp
#define hardware_hpp

#include "filer.hpp"
#include "speed.hpp"

#ifdef ON_PI

#include <wiringPi.h>
#include <linux/i2c-dev.h>
#include <wiringPiI2C.h>

#endif  // ON_PI

#include <stdlib.h>			// malloc
#include <stdio.h>			// sprintf
#include <syslog.h>			// close read write
#include <string.h>			// strcat
#include <unistd.h>


class I2C {
	
public:
	explicit I2C( int addr );
	
	bool    debug;
	int     address;            // I2C address
	int     device;             // File handle to I2C device
	
	int i2cRead           (int reg) ;
	int i2cReadReg8       (int reg) ;
	int i2cReadReg16      (int reg) ;
	
	int i2cWrite          (int reg, int data) ;
	int i2cWriteReg8      (int reg, int data) ;
	int i2cWriteReg16     (int reg, int data) ;
	
};


class PWM {
	
	// Addresses
#define MODE1                   0x00
#define MODE2                   0x01
#define SUBADR1                 0x02
#define SUBADR2                 0x03
#define SUBADR3                 0x04
#define PRESCALE                0xFE
	
#define CHANNEL0_ON_L           0x06
#define CHANNEL0_ON_H           0x07
#define CHANNEL0_OFF_L          0x08
#define CHANNEL0_OFF_H          0x09
	
#define ALLCHANNEL_ON_L         0xFA
#define AllCHANNEL_ON_H         0xFB
#define ALLCHANNEL_OFF_L        0xFC
#define ALLCHANNEL_OFF_H        0xFD
	
	// Bits
#define RESTART                 0x80
#define SLEEP                   0x10
#define ALLCALL                 0x01
#define INVRT                   0x10
#define OUTDRV                  0x04
	
	
	// Address of PWM channels - Fw and Rv expect their on and off values to be 0 to PWM_MAX
#define M0Fw                    9       // Motor 1 Forward enable PWM channel
#define M0Rv                    10      // Motor 1 Reverse enable - both 0 is safe off
#define M0En                    8       // Motor 1 enable, values from 0 to PWM_MAX
	
#define M1Fw                    11
#define M1Rv                    12
#define M1En                    13
	
#define M2Fw                    4
#define M2Rv                    3
#define M2En                    2
	
#define M3Fw                    5
#define M3Rv                    6
#define M3En                    7
	
	
public:
	explicit PWM( int addr );
	
	bool    debug;
	int     address;             // I2C address
	I2C     *i2c;
	
	void setPWMFrequency( int freq );
	void setPWM( int channel, int on, int off );
	void setPWMAll( int on, int off );
	int getPWMResolution();
};

////class DCM {

////public:
////    explicit DCM( hardware board, int motor );

////    bool        debug;
////    hardware    controller;
////    int         motorNumber;
////};


class Hardware {
	
public:
	explicit Hardware();
	
	I2C     	*i2c;
	PWM     	*pwm;
	
	bool    	motor0Setup;
	bool   		motor1Setup;
	
	int     	i2cAddress;
	int     	i2cFrequency;

	Speed		speed;
	
	
//	void initSpeedArrays();
	bool setupForDCMotors();
	bool resetForDCMotors();
	
	void setPin( int pin, int value );
	void setPWM( int pin, int value );
	
	void setMtrDirSpd(int motor, int direction , int speed);
	void setMtrSpd(int motor, int speed);

	void cmdSpeed( int speedIndex );	// Both motors

	int angleToPWM( int angle );
	void cmdAngle( int pin, int angle );
	
	void centerServo( int pin );
	void scanStop( int pin );
	void scanTest( int pin );

	void mobileTask( int taskNumber, int param );
	void mobileAction( int actionNumber, int param );
};

#endif /* hardware_hpp */
