//
//  hardware.cpp
//  cweb
//
//  Created by William Snook on 4/28/18.
//  Copyright © 2018 billsnook. All rights reserved.
//

#include "hardware.hpp"

#include <syslog.h>			// close read write
#include <math.h>

#define I2C_MOTOR_ADDRESS		0x6F
#define CHANNEL_MAX				15

#define PWM_RESOLUTION          4096.0
#define PWM_COUNT               4096	// Also used as value for PWM pin to be all on
#define PWM_MAX                 4095	// Supplys full voltage to motor
#define PWM_FREQ                50		// For servos, motors seem to not care

#define SPEED_INDEX_MAX			PWM_COUNT / SPEED_ADJUSTMENT	// 8

#define MIN_PWM					150		// For servos, 1.0 ms
#define MAX_PWM					510		// to 2.0 ms
#define	DEGREE_PER_PWM			( MAX_PWM - MIN_PWM ) / 180	// == 2 per degree == 0.5 degree accuracy?

#define TRIG					8
#define ECHO					7

extern Filer	filer;

bool	scanLoop;

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
	return wiringPiI2CReadReg8 ( device, reg );	// Read 8 bits from register reg on device
#else
	return reg;
#endif  // ON_PI
}

int I2C::i2cReadReg8(int reg) {
	
#ifdef ON_PI
	return wiringPiI2CReadReg8 ( device, reg );	// Read 8 bits from register reg on device
#else
	return reg;
#endif  // ON_PI
}

int I2C::i2cReadReg16(int reg) {
	
#ifdef ON_PI
	return wiringPiI2CReadReg16 ( device, reg );	// Read 16 bits from register reg on device
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
	
	if ( ( channel < 0 ) || ( channel > CHANNEL_MAX ) ) {
		syslog(LOG_ERR, "ERROR: PWM:setPWM channel: %d; should be 0 <= channel <= CHANNEL_MAX", channel);
		return;
	}
	if ( ( on < 0 ) || ( on > PWM_COUNT ) || ( off < 0 ) || ( off > PWM_COUNT ) ) {
		syslog(LOG_ERR, "ERROR: PWM:setPWM%d on: %d, off: %d; should be 0 <= on or off <= %d (PWM_COUNT)", channel, on, off, PWM_COUNT);
		return;
	}
	
	if ( on + off > PWM_COUNT ) {
		syslog(LOG_ERR, "ERROR: PWM:setPWM%d on: %d, off: %d; should be on + off <= %d (PWM_COUNT)", channel, on, off, PWM_COUNT);
		return;
	}
	
//	syslog(LOG_NOTICE, "PWM:setPWM%d on: %04X, off: %04X", channel, on, off);
	i2c->i2cWrite( CHANNEL0_ON_L + (4 * channel), on & 0xFF );
	i2c->i2cWrite( CHANNEL0_ON_H + (4 * channel), on >> 8 );
	i2c->i2cWrite( CHANNEL0_OFF_L + (4 * channel), off & 0xFF );
	i2c->i2cWrite( CHANNEL0_OFF_H + (4 * channel), off >> 8 );
}

void PWM::setPWMAll( int on, int off ) {
	
	if ( ( on < 0 ) || ( on > PWM_MAX ) || ( off < 0 ) || ( off > PWM_MAX ) ) {
		syslog(LOG_ERR, "ERROR: PWM:setPWMAll on: %d, off: %d; should be 0 <= on or off <= %d (PWM_MAX)", on, off, PWM_MAX);
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
		syslog(LOG_ERR, "Error setting up wiringPi." );
		return;
	}
//	syslog(LOG_NOTICE, "wiringPi version: %d", setupResult );
#endif  // ON_PI
	
	pwm = new PWM( I2C_MOTOR_ADDRESS );		// Default for Motor Hat PWM chip
	pwm->setPWMFrequency( PWM_FREQ );
	
	syslog(LOG_NOTICE, "I2C address: 0x%02X, PWM freq: %d", I2C_MOTOR_ADDRESS, PWM_FREQ );
}

// #define TRIG					8		Brown	Out
// #define ECHO					7		White	In

bool Hardware::setupHardware() {
	
#ifdef ON_PI
	pinMode( TRIG, OUTPUT );	// Brown
	pinMode( ECHO, INPUT );		// White
	
	digitalWrite(TRIG, LOW);	// Off

#endif  // ON_PI

	syslog(LOG_NOTICE, "In setupHardware" );
	speed = Speed();
	speed.initializeSpeedArray();
	
	scanLoop = false;

	return true;
}

bool Hardware::resetHardware() {
	
//	scanStop();
//	centerServo();
	
	syslog(LOG_NOTICE, "In resetHardware" );
	
	setPWM( M0En, 0 );		// Turn off motors
	setPin( M0Fw, 0 );
	setPin( M0Rv, 0 );
	setPWM( M1En, 0 );
	setPin( M1Fw, 0 );
	setPin( M1Rv, 0 );
	
	motor0Setup = false;
	motor1Setup = false;
	
	setPWM( Scanner, 0 );		// Turn off servos
	
	return true;
}

void Hardware::setPin( int pin, int value ) {
	
	if ( ( pin < 0 ) || ( pin > CHANNEL_MAX ) ) {
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
	
	if ( ( pin < 0 ) || ( pin > CHANNEL_MAX ) ) {
		syslog(LOG_ERR, "ERROR: Hardware:setPWM pin: %d; should be 0 <= pin <= CHANNEL_MAX", pin);
		return;
	}
	if ( ( value < 0 ) || ( value > PWM_MAX ) ) {
		syslog(LOG_ERR, "ERROR: Hardware::setPWM%d value: %d; should be 0 <= value <= %d", pin, value, PWM_MAX);
		return;
	}
//	syslog(LOG_INFO, "INFO: Hardware::setPWM%d value: %d", pin, value);
	pwm->setPWM( pin, 0, value );
}

//DCM Hardware::getMotor( int motor ) {
//    if ( ( motor < 1 ) || ( motor > 4 ) ) {
//        return nullptr;
//    }
//    return motors[motor]
//}

// MARK: Motor section
void Hardware::setMtrDirSpd(int motor, int direction , int speedIndex) {
	
	if ( ( speedIndex < 0 ) || ( speedIndex > SPEED_INDEX_MAX ) ) {
		syslog(LOG_ERR, "ERROR: Hardware::setMtrDirSpd speed: %d; should be 0 <= speed <= %d", speedIndex, SPEED_INDEX_MAX);
		return;
	}
	syslog(LOG_NOTICE, "setMtrDirSpd m%d, d: %s, speed: %d", motor, direction ? "f" : "r", speedIndex);
	if ( motor == 0 ) {
		if ( direction == 1 ) {
			setPin( M0Fw, 1 );
			setPin( M0Rv, 0 );
		} else {
			setPin( M0Fw, 0 );
			setPin( M0Rv, 1 );
		}
		motor0Setup = true;
//		setPWM( M0En, speedIndex * SPEED_ADJUSTMENT );
		setPWM( M0En, speed.speedLeft( speedIndex ) );
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
//		setPWM( M1En, speedIndex * SPEED_ADJUSTMENT );
		setPWM( M1En, speed.speedRight( speedIndex ) );
	}
}

void Hardware::setMtrSpd(int motor, int speedIndex) {
	
	syslog(LOG_NOTICE, "setMtrSpd m%d, speed: %d", motor, speedIndex);
	if ( ( motor == 0 ) && motor0Setup ) {
//		setPWM( M0En, speedIndex * SPEED_ADJUSTMENT );
		setPWM( M0En, speed.speedLeft( speedIndex ) );
	}
	if ( ( motor == 1 ) && motor1Setup ) {
//		setPWM( M1En, speedIndex * SPEED_ADJUSTMENT );
		setPWM( M1En, speed.speedRight( speedIndex ) );
	}
}

void Hardware::setMotors(int direction0, int speedIndex0, int direction1, int speedIndex1) {
	if ( ( speedIndex0 < 0 ) || ( speedIndex0 > SPEED_INDEX_MAX ) ||
		 ( speedIndex1 < 0 ) || ( speedIndex1 > SPEED_INDEX_MAX ) ) {
		syslog(LOG_ERR, "ERROR: Hardware::setMtrDirSpd speed: %d - %d; should be 0 <= speed <= %d", speedIndex0, speedIndex1, SPEED_INDEX_MAX);
		return;
	}
	if ( direction0 == 1 ) {
		setPin( M0Fw, 1 );
		setPin( M0Rv, 0 );
	} else {
		setPin( M0Fw, 0 );
		setPin( M0Rv, 1 );
	}
	if ( direction1 == 1 ) {
		setPin( M1Fw, 1 );
		setPin( M1Rv, 0 );
	} else {
		setPin( M1Fw, 0 );
		setPin( M1Rv, 1 );
	}

	setPWM( M0En, speed.speedLeft( speedIndex0 ) );
	setPWM( M1En, speed.speedRight( speedIndex1 ) );

}

void Hardware::cmdSpeed( int speedIndex ) {
	
	if ( speedIndex == 0 ) {
		setPin( M0Fw, 0 );
		setPin( M0Rv, 0 );
		setPWM( M0En, 0 );
		setPin( M1Fw, 0 );
		setPin( M1Rv, 0 );
		setPWM( M1En, 0 );
		return;
	}
	int speedLeft = speed.speedLeft( speedIndex );	// Index says f or r but speedL or R is absolute
	int speedRight = speed.speedRight( speedIndex );
	syslog( LOG_NOTICE, "cmdSpeed, sl: %d, sr: %d", speedLeft, speedRight );
	if ( speedIndex > 0 ) {
		setPin( M0Fw, 1 );
		setPin( M0Rv, 0 );
	} else {
		setPin( M0Fw, 0 );
		setPin( M0Rv, 1 );
	}
	setPWM( M0En, speedLeft );
	if ( speedIndex > 0 ) {
		setPin( M1Fw, 1 );
		setPin( M1Rv, 0 );
	} else {
		setPin( M1Fw, 0 );
		setPin( M1Rv, 1 );
	}
	setPWM( M1En, speedRight );
}

// MARK: Servo section
int Hardware::angleToPWM( int angle ) {
	
	return MIN_PWM + ( angle * DEGREE_PER_PWM );
}

void Hardware::cmdAngle( int angle ) {
	
	setPWM( Scanner, angleToPWM( angle - 2 ) );	// Calibrated
}

void Hardware::centerServo() {
	
	cmdAngle( 90 );
}

void Hardware::scanStop() {
	
	syslog(LOG_NOTICE, "In scanStop" );
	scanLoop = false;
}

void Hardware::scanTest() {
	if ( scanLoop ) {
		syslog(LOG_NOTICE, "Attempting to run scanTest multiple times" );
		return;				// If this is run multiple times, mayhem!
	}
	scanLoop = true;

	syslog(LOG_NOTICE, "In scanTest" );
	
	do {
		for( int angle = 45; angle < 135; angle += 5 ) {
			if ( !scanLoop ) {
				break;
			}
			syslog(LOG_NOTICE, "scanTest pin %d to %d", Scanner, angle );
			cmdAngle( angle );
			usleep( 100000 );	// .1 second
		}
		for( int angle = 135; angle > 45; angle -= 5 ) {
			if ( !scanLoop ) {
				break;
			}
			syslog(LOG_NOTICE, "scanTest pin %d to %d", Scanner, angle );
			cmdAngle( angle );
			usleep( 100000 );	// .1 second
		}
	} while ( scanLoop );
	centerServo();
}

void Hardware::scanPing() {
	
	
}


// MARK: Range Finder section
void Hardware::ping() {
	
	syslog(LOG_NOTICE, "In ping" );
	
#ifdef ON_PI
//	pinMode( TRIG, OUTPUT );	// Brown	8
//	pinMode( ECHO, INPUT );		// White	7
	
	digitalWrite( TRIG, HIGH );	// On
	usleep( 1000000 );
	digitalWrite( TRIG, LOW );	// Off
	
	bool echo = digitalRead( ECHO );		//
	syslog(LOG_NOTICE, "In ping, read %d", echo );

#endif  // ON_PI

}
