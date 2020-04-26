//
//  hardware.hpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef hardware_hpp
#define hardware_hpp

#include "speed.hpp"


#include <stdlib.h>			// malloc
#include <stdio.h>			// sprintf
#include <syslog.h>			// close read write
#include <string.h>			// strcat
#include <unistd.h>

#define ArdI2CAddr          0x08    // Needs to match value set in Arduino controller code
#define MOTOR_I2C_ADDRESS   0x6F    // Hard coded (literally) on motor controller board


class I2C {
	
public:
	explicit I2C( int addr );
	
	bool    debug;
	int     address;            // I2C address
	int     file_i2c;           // File handle to I2C device
	
    int     i2cRead(int reg);
    void    i2cWrite(int reg, int data);
    void    i2cWriteX(int reg, int data);

};


class PWM {
	
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


enum {		// Scan type
	NoScan,
	NarrowScan,
	MediumScan,
	WideScan
};

enum {		// Distance class
	NoDistance,
	ShortDistance,
	MediumDistance,
	LongDistance
};


class Hardware {
	
public:
	explicit Hardware();
	
	I2C     	*i2c;
	PWM     	*pwm;
	
	bool    	motor0Setup;
	bool   		motor1Setup;
	bool		sweepOneWay;
	bool		upsideDownScanner;
	
	int     	i2cAddress;
	int     	i2cFrequency;

	Speed		speed;
	
	
//	void initSpeedArrays();
	bool setupHardware();
	bool shutdownHardware();
	
	void setPin( int pin, int value );
	void setPWM( int pin, int value );
	
	void setMtrDirSpd(int motor, int direction, int speed);
	void setMtrSpd(int motor, int speed);
	void setMotors(int direction0, int speed0, int direction1, int speed1);

	void cmdSpeed( int speedIndex );	// Both motors

	int angleToPWM( int angle );
	void cmdAngle( int angle );
	
	void centerServo();
	void scanStop();
	void scanTest();
	void prepPing( int start, int end, int inc );
	void scanPing( int socket );
	void pingLoop();

	unsigned int ping( unsigned int angle );
	void allStop();
	void scanUntilFound( int scanType );
	void turnAndFollow( int followDistance );

};

#endif /* hardware_hpp */
