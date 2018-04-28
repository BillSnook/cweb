//
//  motor.hpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#ifndef motor_hpp
#define motor_hpp

#ifdef ON_PI

#include <wiringPi.h>
#include <linux/i2c-dev.h>
#include <wiringPiI2C.h>

#endif  // ON_PI


#define L1  23
#define L2  24
#define L3  27
#define L4  25

#define M1En    0
#define M1Fw    2
#define M1Rv    3
#define M2En    6
#define M2Fw    5
#define M2Rv    4


class Motor {
	
public:
	explicit Motor();
	
	bool    debug;
	int     i2cAddress;             // I2C address
	int     i2cFrequency;
	int     motors;
	//   PWM     pwm;
	
	bool setupForMotor();
	bool resetForMotor();
	
	void blinkLED();
	
	void onPin( int led );
	void offPin( int led );
	void setPWMPin( int pin, int value );
	
	void checkMotor( int motor, int direction, int speed );
	void setMtrDirSpd(int motor, int direction , int speed);
	void setMtrSpd(int motor, int speed);
	
	int getI2CReg( int reg );
	void putI2CReg( int reg, int newValue );
	
};

#endif /* motor_hpp */
