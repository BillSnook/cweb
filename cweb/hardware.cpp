//
//  hardware.cpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "hardware.hpp"

#include <syslog.h>			// close read write
//#include <stdio.h>			// printf
//#include <fcntl.h>			// open
//#include <sys/ioctl.h>
//#include <getopt.h>
#include <math.h>


//  MARK: i2c interface read and write support
I2C::I2C(int addr) {
	
	debug = false;
	address = addr;
	
#ifdef ON_PI
	device = wiringPiI2CSetup( addr );
#endif  // ON_PI
}

int I2C::i2cRead(int reg) {
	
#ifdef ON_PI
	return wiringPiI2CReadReg8 ( device, reg ) ;	// Read 8 bits from register reg on device
#else
	return reg;
#endif  // ON_PI
}

int I2C::i2cReadReg8(int reg) {
	
#ifdef ON_PI
	return wiringPiI2CReadReg8 ( device, reg ) ;	// Read 8 bits from register reg on device
#else
	return reg;
#endif  // ON_PI
}

int I2C::i2cReadReg16(int reg) {
	
#ifdef ON_PI
	return wiringPiI2CReadReg16 ( device, reg ) ;	// Read 16 bits from register reg on device
#else
	return reg;
#endif  // ON_PI
}


int I2C::i2cWrite(int reg, int data) {
	
#ifdef ON_PI
	return wiringPiI2CWriteReg8(device, reg, data);
#else
	return reg + data;
#endif  // ON_PI
}

int I2C::i2cWriteReg8(int reg, int data) {
	
#ifdef ON_PI
	return wiringPiI2CWriteReg8(device, reg, data);
#else
	return reg + data;
#endif  // ON_PI
}

int I2C::i2cWriteReg16(int reg, int data) {
	
#ifdef ON_PI
	return wiringPiI2CWriteReg16(device, reg, data);
#else
	return reg + data;
#endif  // ON_PI
}


PWM::PWM( int addr ) {
	
	debug = false;
	address = addr;
	i2c = new I2C( addr );
	setPWMAll( 0, 0 );                  // Clear all to 0
	i2c->i2cWrite( MODE2, OUTDRV );
	i2c->i2cWrite( MODE1, ALLCALL );
	i2c->i2cWrite( CHANNEL0_OFF_L, 0 );
	i2c->i2cWrite( CHANNEL0_OFF_H, 0 );
	
#ifdef ON_PI
	delay( 1 );                         // Millisecond to let oscillator setup
#endif  // ON_PI
	
}

void PWM::setPWMFrequency( int freq ) {
	
	int prescaleval = 25000000.0;           // Nominal clock freq 25MHz
	prescaleval /= PWM_RESOLUTION;          // 12-bit
	prescaleval /= float( freq );
	prescaleval -= 1.0;                     // For averaging
	float prescale = floor(prescaleval + 0.5);
//	syslog(LOG_NOTICE, "prescaleval: %d, prescale: %f", prescaleval, prescale );
	int prescaleSetting = int(floor(prescale));
	if ( prescaleSetting < 3 ) {
		prescaleSetting = 3;
	}
	
	int oldmode = i2c->i2cRead( MODE1 );
	int newmode = ( oldmode & 0x7F ) | SLEEP;  // sleep
	i2c->i2cWrite( MODE1, newmode );             // go to sleep
	i2c->i2cWrite( PRESCALE, prescaleSetting );
	i2c->i2cWrite( MODE1, oldmode );
	
#ifdef ON_PI
	delay( 1 );                         // Millisecond to let oscillator stabileize
#endif  // ON_PI
}

void PWM::setPWM( int channel, int on, int off ) {
	
	if ( ( channel < 0 ) || ( channel > 15 ) ) {
		syslog(LOG_NOTICE, "ERROR: PWM:setPWM channel: %d; should be 0 <= channel <= 15", channel);
		return;
	}
	if ( ( on < 0 ) || ( on > PWM_COUNT ) || ( off < 0 ) || ( off > PWM_COUNT ) ) {
		syslog(LOG_NOTICE, "ERROR: PWM:setPWM%d on: %d, off: %d; should be 0 <= on or off <= %d (PWM_COUNT)", channel, on, off, PWM_COUNT);
		return;
	}
	
	//    syslog(LOG_NOTICE, "PWM:setPWM%d on: %04X, off: %04X", channel, on, off);
	i2c->i2cWrite( CHANNEL0_ON_L + (4 * channel), on & 0xFF );
	i2c->i2cWrite( CHANNEL0_ON_H + (4 * channel), on >> 8 );
	i2c->i2cWrite( CHANNEL0_OFF_L + (4 * channel), off & 0xFF );
	i2c->i2cWrite( CHANNEL0_OFF_H + (4 * channel), off >> 8 );
}

void PWM::setPWMAll( int on, int off ) {
	
	if ( ( on < 0 ) || ( on > PWM_COUNT ) || ( off < 0 ) || ( off > PWM_COUNT ) ) {
		syslog(LOG_NOTICE, "ERROR: PWM:setPWMAll on: %d, off: %d; should be 0 <= on or off <= %d (PWM_COUNT)", on, off, PWM_COUNT);
		return;
	}
	i2c->i2cWrite( ALLCHANNEL_ON_L, on & 0xFF );
	i2c->i2cWrite( AllCHANNEL_ON_H, on >> 8 );
	i2c->i2cWrite( ALLCHANNEL_OFF_L, off & 0xFF );
	i2c->i2cWrite( ALLCHANNEL_OFF_H, off >> 8 );
}

int PWM::getPWMResolution() {
	
	return PWM_RESOLUTION;
}

////DCM::DCM( hardware board, int motor ) {

////    controller = board;
////    motorNumber = motor;
////}}




Hardware::Hardware() {
	
	motor0Setup = false;
	motor1Setup = false;
	
#ifdef ON_PI
	int setupResult = wiringPiSetup();
	if ( setupResult == -1 ) {
		syslog(LOG_NOTICE, "Error setting up wiringPi." );
		return;
	}
//	syslog(LOG_NOTICE, "Pi version: %d", setupResult );
#endif  // ON_PI
	
	pwm = new PWM( 0x6F );          // Default for Motor Hat PWM chip
	pwm->setPWMFrequency( 1600 );
	
//	syslog(LOG_NOTICE, "Speed adjustment: %d", SPEED_ADJUSTMENT );
}

bool Hardware::setupForDCMotors() {
	
	return true;
}

bool Hardware::resetForDCMotors() {
	
	setPWM( M0En, 0 );
	setPin( M0Fw, 0 );
	setPin( M0Rv, 0 );
	setPWM( M1En, 0 );
	setPin( M1Fw, 0 );
	setPin( M1Rv, 0 );
	
	motor0Setup = false;
	motor1Setup = false;
	return true;
}

void Hardware::setPin( int pin, int value ) {
	
	if ( ( pin < 0 ) || ( pin > 15 ) ) {
		return;
	}
	if ( value == 0 ) {
		pwm->setPWM( pin, 0, PWM_COUNT );
	} else {
		pwm->setPWM( pin, PWM_COUNT, 0 );
	}
	return;
}

void Hardware::setPWM( int pin, int value ) {
	
	if ( ( pin < 0 ) || ( pin > 15 ) ) {
		syslog(LOG_NOTICE, "ERROR: Hardware:setPWM pin: %d; should be 0 <= pin <= 15", pin);
		return;
	}
	if ( ( value < 0 ) || ( value > PWM_COUNT ) ) {
		syslog(LOG_NOTICE, "ERROR: Hardware::setPWM value: %d", value);
		return;
	}
	pwm->setPWM( pin, 0, value );
}

//DCM Hardware::getMotor( int motor ) {
//    if ( ( motor < 1 ) || ( motor > 4 ) ) {
//        return nullptr;
//    }
//    return motors[motor]
//}

void Hardware::setMtrDirSpd(int motor, int direction , int speed) {
	
	if ( ( speed < 0 ) || ( speed > (PWM_COUNT / SPEED_ADJUSTMENT) ) ) {
		syslog(LOG_NOTICE, "ERROR: Hardware::setMtrDirSpd speed: %d", speed);
		return;
	}
	syslog(LOG_NOTICE, "setMtrDirSpd m%d, d: %s, speed: %d", motor, direction ? "f" : "r", speed);
	if ( motor == 0 ) {
		if ( direction == 1 ) {
			setPin( M0Fw, 1 );
			setPin( M0Rv, 0 );
		} else {
			setPin( M0Fw, 0 );
			setPin( M0Rv, 1 );
		}
		motor0Setup = true;
		setPWM( M0En, speed * SPEED_ADJUSTMENT );
	}
	if ( motor == 1 ) {
		if ( direction == 1 ) {
			setPin( M1Fw, 1 );
			setPin( M1Rv, 0 );
		} else {
			setPin( M1Fw, 0 );
			setPin( M1Rv, 1 );
		}
		motor1Setup = true;
		setPWM( M1En, speed * SPEED_ADJUSTMENT );
	}
}

void Hardware::setMtrSpd(int motor, int speed) {
	
	syslog(LOG_NOTICE, "setMtrSpd m%d, speed: %d", motor, speed);
	if ( ( motor == 0 ) && motor0Setup ) {
		setPWM( M0En, speed * SPEED_ADJUSTMENT );
	}
	if ( ( motor == 1 ) && motor1Setup ) {
		setPWM( M1En, speed * SPEED_ADJUSTMENT );
	}
}

