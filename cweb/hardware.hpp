//
//  hardware.hpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef hardware_hpp
#define hardware_hpp

#include <stdlib.h>            // malloc
#include <stdio.h>             // sprintf
#include <syslog.h>            // close read write
#include <string.h>            // strcat
#include <unistd.h>

#include "speed.hpp"

#ifdef ON_PI

#include "ArducamDepthCamera.h"

#endif  // ON_PI



#define ARD_I2C_ADDR        0x08    // Needs to match value set in Arduino controller code
#define MOTOR_I2C_ADDRESS   0x6F    // Hard coded (literally) on motor controller board


class I2C {
	
public:
	explicit I2C( int addr );
	
	int     address;            // I2C address
	int     file_i2c;           // File handle to I2C device
    int     motor_i2c;          // pigpio handle for device at address on I2C bus

    int     i2cRead(int reg);
    void    i2cWrite(int reg, int data);
    void    i2cWriteX(int reg, int data);
};


class PWM {
	
public:
	explicit PWM( int addr );
	
	int     address;             // I2C address
	I2C     *i2c;

	void    setPWMFrequency( int freq );
	void    setPWM( int channel, int on, int off );
	void    setPWMAll( int on, int off );
	int     getPWMResolution();
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

enum getStatusBit {
    statusScannerOrientation = 0x01,       // Scanner may be inverted and controller may want to know
    getStatusEnd
};

enum setStatusBit {
    statusControlledMode = 0x01,           // Device expects to have a controller give it commands
    setStatusEnd
};


struct RangeData {
    int pwmCenter;
    int servoPort;
};


class Hardware {
	
public:
	
	PWM     	    *pwm;
	
    Speed           speed;
    
	bool    	    motor0Setup;
	bool   		    motor1Setup;
	bool		    sweepOneWay;
	bool		    upsideDownScanner;

    RangeData       rangeData;
    int             minimumPWM;


    bool            gpioInitialised;    // Flag for successful response from call to gpioInitialise()
    unsigned        i2cDevice;          // Handle to opened i2c channel

    bool            cameraInitialized;  // Flag for successful response from call to createArducamDepthCamera()
#ifdef ON_PI
    ArducamDepthCamera      tof;
#endif  // ON_PI

//	void initSpeedArrays();
	bool setupHardware();
	bool shutdownHardware();
    long getStatus();

	void setPin( int pin, int value );
	void setPWM( int pin, int value );
	
	void setMtrDirSpd(int motor, int direction, int speed);
	void setMtrSpd(int motor, int speed);
    void setMotorsPWM(int direction0, int pwm0, int direction1, int pwm1);
    void setMotors(int direction0, int speed0, int direction1, int speed1);

	void cmdSpeed( int speedIndex );	// Both motors

    long getDiffUSec( struct timeval startTime, struct timeval endTime );
	int angleToPWM( int angle );
    void cmdPWM( int pulseCount );
	void cmdAngle( int angle );
    long cmdPing();
    void testPing(int no_of_measurements, int delay_ms);
    void pinState( int pin, int state );
    
	void centerServo();
	void scanStop();
	void scanTest();
	void prepPing( int start, int end, int inc );
	void scanPing( int sockOrAddr );
	void pingLoop();
    long doPing();

    unsigned int ping( unsigned int angle );
    long pingTest( unsigned int angle );
    
	void allStop();

    void cameraInit();
    int startCamera();
    float getCameraData(int socketOrAddr);
    int stopCamera();
    int cameraDataSend(int socketOrAddr);
    void cameraStreamTest(int socketOrAddr);
};

#endif /* hardware_hpp */
